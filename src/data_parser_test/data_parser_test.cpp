#include <iostream>
#include <fstream>
#include <regex>
#include <iomanip>
#include <sstream>
#include <cassert>
#include <filesystem>
#include <chrono>

// Some ANSI escape codes for colors
//
#define RESET       "\033[0m"         /* reset        */
#define BLACK       "\033[30m"        /* Black        */
#define RED         "\033[31m"        /* Red          */
#define GREEN       "\033[32m"        /* Green        */
#define YELLOW      "\033[33m"        /* Yellow       */
#define BLUE        "\033[34m"        /* Blue         */
#define MAGENTA     "\033[35m"        /* Magenta      */
#define CYAN        "\033[36m"        /* Cyan         */
#define WHITE       "\033[37m"        /* White        */
#define BOLDBLACK   "\033[1m\033[30m" /* Bold Black   */
#define BOLDRED     "\033[1m\033[31m" /* Bold Red     */
#define BOLDGREEN   "\033[1m\033[32m" /* Bold Green   */
#define BOLDYELLOW  "\033[1m\033[33m" /* Bold Yellow  */
#define BOLDBLUE    "\033[1m\033[34m" /* Bold Blue    */
#define BOLDMAGENTA "\033[1m\033[35m" /* Bold Magenta */
#define BOLDCYAN    "\033[1m\033[36m" /* Bold Cyan    */
#define BOLDWHITE   "\033[1m\033[37m" /* Bold White   */

#include <data_parser_lib.hpp>

using namespace std::string_literals;

struct DataItem
{
  double freq;
  double s11;
};

struct DataStruct
{
  std::pair<double, double> freqRange;
  std::pair<double, double> s11Range;

  std::vector<DataItem> data;
};

static const std::string fileNamePattern
  { R"(^(([A-Za-z]:)?[\\\/])?([a-zA-Z_\-0-9.]*[\\\/])*[a-zA-Z_\-0-9.]*.([sS]1[pP])$)" };

std::tuple<size_t, size_t> transformData(const std::vector<std::string>& data, DataStruct& dataStruct)
{
  dataStruct.data.clear();

  const std::string doublePattern{ R"(\-?[1-9]?[0-9]+\.[0-9]+[eE][+\-]?[0-9]{2,})" };
  const std::string spacePattern{ R"([ ]{1,2})" };
  std::stringstream ssDataLinePattern;
  ssDataLinePattern << "^"s << "(" << doublePattern << ")" << spacePattern << "(" << doublePattern << ")" << spacePattern << "(" << doublePattern << ")";
  const std::regex dataLineRegex{ ssDataLinePattern.str() };

  size_t linesSkipped = 0;
  size_t linesFailed = 0;

  std::string line;
  size_t linesTotal = 0;
  for (const std::string& line : data)
  {
    ++linesTotal;
    if (line.front() == '#' || line.front() == '!')
    {
      ++linesSkipped;
      continue;
    }

    if (std::smatch matchResult; std::regex_match(line, matchResult, dataLineRegex))
    {
      ++linesFailed;

      const double freq = std::stof(matchResult[1]);
      const double re = std::stof(matchResult[2]);
      const double im = std::stof(matchResult[3]);
      const double s11 = 20. * std::log10(std::sqrt(std::pow(re, 2) + std::pow(im, 2)));

      if (freq > dataStruct.freqRange.second)
        dataStruct.freqRange.second = freq;
      if (freq < dataStruct.freqRange.first)
        dataStruct.freqRange.first = freq;

      if (s11 > dataStruct.s11Range.second)
        dataStruct.s11Range.second = s11;
      if (s11 < dataStruct.s11Range.first)
        dataStruct.s11Range.first = s11;

      dataStruct.data.push_back({ freq, s11 });
    }
  }

  assert(dataStruct.data.size() == (linesTotal - linesSkipped));

  return {linesSkipped, linesFailed};
}

std::vector<std::string> readFileBuffered(std::string fileName)
{
  std::vector<std::string> lines;

  std::ifstream data_file(fileName, std::ios_base::binary);
  if (!data_file.is_open())
    return {};

  const size_t data_size = std::filesystem::file_size(fileName);
  std::vector<char> buffer(data_size);
  if (data_file.read(buffer.data(), data_size))
  {
    auto _1 = buffer.begin();
    auto _2 = buffer.begin();

    while (_2 != buffer.end())
    {
      while (*_2 != '\r' && *_2 != '\n') ++_2;

      lines.emplace_back(_1, _2);
      if (*_2 == '\r')
        ++_2;
      if (*_2 == '\n')
        ++_2;
      _1 = _2;
    }
  }

  return lines;
}

int printLines(const std::vector<std::string>& lines)
{
  if (lines.empty())
    return 1;

  for (const auto& line : lines)
    std::cout << line << "\n";

  return 0;
}

void normalize(DataStruct& dataStruct, std::pair<double, double> freqRange, std::pair<double, double> s11Range)
{
  const double freqFactor = (freqRange.second - freqRange.first) / (dataStruct.freqRange.second - dataStruct.freqRange.first);
  const double s11Factor = (s11Range.second - s11Range.first) / (dataStruct.s11Range.second - dataStruct.s11Range.first);
  for(auto& item : dataStruct.data)
  {
    item.freq = freqRange.first + freqFactor * (item.freq - dataStruct.freqRange.first);
    item.s11  = s11Range.first + s11Factor * (item.s11 - dataStruct.s11Range.first);

    assert( freqRange.first <= item.freq && item.freq <= freqRange.second);
    assert(   s11Range.first <= item.s11 && item.s11 <= s11Range.second);
  }
}

int main(int argc, char** argv)
{
  // Path with .s1p file regexp pattern
  const std::regex reFilename{ fileNamePattern };
  if (argc < 2 || false == std::regex_match(argv[1], reFilename))
  {
    std::cerr << RED << "[ ERROR ] The only argument must be data file name with \"s1p\" extension." << RESET << "\n";
    return 1;
  }

  std::ifstream data_file(argv[1], std::ios_base::in);
  if (false == data_file.is_open())
  {
    std::cerr << RED << "[ ERROR ] Failed to open file" << RESET << "\n";
    return 1;
  }

  {
    std::cout << GREEN << "Buffered read:\n" << RESET;

    DataStruct processed;
    const auto t0 = std::chrono::high_resolution_clock::now();

    const auto result = readFileBuffered(argv[1]);

    const auto t1 = std::chrono::high_resolution_clock::now();

    const auto [skipped, failed] = transformData(result, processed);

    const auto t2 = std::chrono::high_resolution_clock::now();

    const auto d1 = std::chrono::duration_cast<std::chrono::microseconds>(t1 - t0);
    const auto d2 = std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1);

    normalize(processed, {-10., 10}, {-10, 10});

    for(const auto item : processed.data)
    {
      std::cout << item.freq << " " << item.s11 << "\n";
    }

    std::cout << YELLOW;
    std::cout << "  File read time    : " << d1.count() << "us\n";
    std::cout << "  Data process time : " << d2.count() << "us\n";
    std::cout << "  Data lines read   : " << result.size() << "\n";
    std::cout << "      Lines skipped : " << skipped << "\n";
    std::cout << "      Lines failed  : " << failed << "\n";
    std::cout << RESET;
  }

  return 0;// printLines(result);
}