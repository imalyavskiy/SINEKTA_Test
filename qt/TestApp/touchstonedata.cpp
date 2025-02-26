#include <QDebug>
#include <QFileInfo>
#include "touchstonedata.h"

#include <cmath>
#include <iostream>
#include <fstream>
#include <regex>
#include <sstream>
#include <optional>
#include <cassert>

using namespace std::string_literals;

TouchstoneData::TouchstoneData(QObject *parent ) : QObject(parent)
{
    m_data.clear();
}

int TouchstoneData::size() const
{
    return m_data.size();
}

int TouchstoneData::getY(int row) const
{
    if(row >= m_data.size())
        return 0;

    return m_data[row].s11_logmag;
}

int TouchstoneData::getX(int row) const
{
    if(row >= m_data.size())
        return 0;
    return m_data[row].freq;
}

std::optional<std::ifstream> TouchstoneData::openFile(QString filePath)
{
    static const std::string fileNamePattern
        { R"(^((([A-Za-z]:)|(((file)|(qrc)):\/\/))?[\\\/])?([a-zA-Z_\-0-9.]*[\\\/])*[a-zA-Z_\-0-9.]*.([sS]1[pP])$)"};

    // Path with .s1p file regexp patter
    const std::regex reFilename{ fileNamePattern };
    if (false == std::regex_match(filePath.toStdString(), reFilename))
        return {};

    if(filePath.startsWith("file://"))
        filePath.remove("file://");
    if(filePath.startsWith("qrc://"))
        filePath.remove("qrc://");

    std::ifstream data_file(filePath.toStdString(), std::ios_base::in);
    if (false == data_file.is_open())
        return{};

    return { std::move(data_file) };
}

std::optional<TouchstoneData::FileReadResult> TouchstoneData::readTouchstoneData(std::ifstream& data_file, std::vector<TouchstoneDataItem>& m_data)
{
    FileReadResult state;

    std::string doublePattern{ R"(\-?[1-9]?[0-9]+\.[0-9]+[eE][+\-]?[0-9]{2,})" };
    std::string spacePattern{ R"([ ]{1,2})" };
    std::stringstream ssDataLinePattern;
    ssDataLinePattern << "^"s << "(" << doublePattern << ")" << spacePattern << "(" << doublePattern << ")" << spacePattern << "(" << doublePattern << ")";
    std::regex dataLineRegex{ ssDataLinePattern.str()};

    std::string line;
    for (; std::getline(data_file, line); ++state.lines_read)
    {
        if(line[0] == '#' || line[0] == '!')
        {
            state.lines_skipped++;
            continue;
        }

        std::smatch smatch;
        if (std::regex_match(line, smatch, dataLineRegex))
        {
            const double freq  = std::stod(smatch[1]);
            const double real  = std::stod(smatch[2]);
            const double imag  = std::stod(smatch[3]);
            const double s11_logmag = 20 * std::log10(std::sqrt(std::pow(real, 2) + std::pow(imag, 2)));

            m_data.push_back({ freq, s11_logmag });

            if(freq > dataRect.freqRange.max)
                dataRect.freqRange.max = freq;
            else if(freq < dataRect.freqRange.min)
                dataRect.freqRange.min = freq;

            if(s11_logmag > dataRect.s11Range.max)
                dataRect.s11Range.max = s11_logmag;
            else if(s11_logmag < dataRect.s11Range.min)
                dataRect.s11Range.min = s11_logmag;
        }
        else
            state.lines_failed++;
    }

    return {state};
}

void TouchstoneData::normalizeData(int left, int right, int bottom, int top)
{
    if(right <= left)
        throw std::logic_error("invalid width args");
    if(top <= bottom)
        throw std::logic_error("invalid height args");

    const double freqFactor = (right - left)/(dataRect.freqRange.max - dataRect.freqRange.min);
    const double s11Factor = (top - bottom)/(dataRect.s11Range.max - dataRect.s11Range.min);

    for(auto& dataItem : m_data)
    {
        dataItem.freq        = left + freqFactor * (dataItem.freq - dataRect.freqRange.min);
        dataItem.s11_logmag  = bottom   + s11Factor  * (dataItem.s11_logmag  - dataRect.s11Range.min);
    }
}

double TouchstoneData::Dist(const TouchstoneDataItem& left, const TouchstoneDataItem& right)
{
    double distance = sqrt(std::pow(left.freq - right.freq, 2) + std::pow(left.s11_logmag - right.s11_logmag, 2));
    return distance;
}

void TouchstoneData::filterData(int epsilon)
{
    size_t size = m_data.size();

    for(auto it = m_data.begin(); it != m_data.end(); ++it)
    {
        auto it_next = it + 1;
        if(it_next == m_data.end())
            continue;

        while(epsilon > Dist(*it, *it_next))
        {
            m_data.erase(it_next);
            it_next = it + 1;
            if(it_next == m_data.end())
                break;
        }
    }

    return;
}

void TouchstoneData::loadData(QString filePath, int viewportWidth, int viewportHeight, int epsilon)
{
    m_data.clear();

    auto open_file_result = openFile(filePath);
    if(false == open_file_result.has_value())
    {
        emit fileLoadFailure(filePath);
        return;
    }

    std::ifstream data_file =
        std::move(open_file_result.value());

    auto read_file_result =
        readTouchstoneData(data_file, m_data);
    if(false == read_file_result.has_value())
    {
        emit fileLoadFailure(filePath);
        return;
    }

    if(m_data.size() != (read_file_result->lines_read - read_file_result->lines_skipped))
        emit fileLoadFailure(filePath);

    normalizeData(0, viewportWidth, 0, viewportHeight);

    filterData(epsilon);

    emit fileLoadSuccess(filePath);
}
