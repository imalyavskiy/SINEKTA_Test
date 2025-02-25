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

int TouchstoneData::rowCount() const
{
    return m_data.count();
}

QVariantMap TouchstoneData::get(int row) const
{
    const TouchstoneDataItem item = m_data.value(row);
    return { {"Frequency", item.freq}, {"S11_logmag", item.s11_logmag}};
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

std::optional<TouchstoneData::FileReadResult> TouchstoneData::readTouchstoneData(std::ifstream& data_file, QList<TouchstoneDataItem>& m_data)
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
            const double fr = std::stod(smatch[1]);
            const double re = std::stod(smatch[2]);
            const double im = std::stod(smatch[3]);
            m_data.push_back({ fr, {re, im}, 20 * std::log10(std::sqrt(std::pow(re, 2) + std::pow(im, 2))) });
        }
        else
            state.lines_failed++;
    }

    return {state};
}

void TouchstoneData::loadData(QString filePath, int viewportWidth, int viewportHeight)
{
    Q_UNUSED(viewportWidth);
    Q_UNUSED(viewportHeight);

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
    emit fileLoadSuccess(filePath);
}
