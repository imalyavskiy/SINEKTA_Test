#include <QDebug>
#include <QFileInfo>
#include "touchstonedata.h"

#include <cmath>
#include <iostream>
#include <fstream>
#include <regex>
#include <sstream>
#include <cassert>

using namespace std::string_literals;

#define S11_VALUE(REAL, IMAGINABLE) 20 * std::log10(std::sqrt(std::pow(REAL, 2) + std::pow(IMAGINABLE, 2)))

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
    return { {"Frequency", item.freq}, {"S11", S11_VALUE(item.s11.re, item.s11.im)}};
}

void TouchstoneData::loadData(QString filePath, int viewportWidth, int viewportHeight)
{
    Q_UNUSED(viewportWidth);
    Q_UNUSED(viewportHeight);

    m_data.clear();

    static const std::string fileNamePattern
        { R"(^((([A-Za-z]:)|(((file)|(qrc)):\/\/))?[\\\/])?([a-zA-Z_\-0-9.]*[\\\/])*[a-zA-Z_\-0-9.]*.([sS]1[pP])$)"};

    // Path with .s1p file regexp patter
    const std::regex reFilename{ fileNamePattern };
    if (false == std::regex_match(filePath.toStdString(), reFilename))
    {
        qDebug() << "[ ERROR ] Failed to read file path.";
        emit fileLoadFailure(filePath);
        return;
    }

    if(filePath.startsWith("file://"))
        filePath.remove("file://");
    if(filePath.startsWith("qrc://"))
        filePath.remove("qrc://");

    std::ifstream data_file(filePath.toStdString(), std::ios_base::in);
    if (false == data_file.is_open())
    {
        qDebug() << "[ ERROR ] Failed to open file";
        emit fileLoadFailure(filePath);
        return;
    }

    std::string doublePattern{ R"(\-?[1-9]?[0-9]+\.[0-9]+[eE][+\-]?[0-9]{2,})" };
    std::string spacePattern{ R"([ ]{1,2})" };
    std::stringstream ssDataLinePattern;
    ssDataLinePattern << "^"s << "(" << doublePattern << ")" << spacePattern << "(" << doublePattern << ")" << spacePattern << "(" << doublePattern << ")";
    std::regex dataLineRegex{ ssDataLinePattern.str()};

    size_t lines_skiped = 0;
    size_t lines_failed = 0;

    std::string line;
    size_t ln = 0;
    for (; std::getline(data_file, line); ++ln)
    {
        if(line[0] == '#' || line[0] == '!')
        {
            ++lines_skiped;
            continue;
        }

        std::smatch smatch;
        if (std::regex_match(line, smatch, dataLineRegex))
            m_data.push_back({ std::stod(smatch[1]), {std::stod(smatch[2]), std::stod(smatch[3])} });
        else
            ++lines_failed;
    }

    if(m_data.size() != (ln - lines_skiped))
        emit fileLoadFailure(filePath);

    emit fileLoadSuccess(filePath);
}
