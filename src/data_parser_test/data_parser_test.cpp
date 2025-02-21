#include <iostream>
#include <fstream>
#include <regex>
#include <iomanip>
#include <sstream>
#include <cassert>

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

struct Item
{
  double freq;
  struct
  {
    double re;
    double im;
  } s11;
};

static const std::string fileNamePattern
  { R"(^(([A-Za-z]:)?[\\\/])?([a-zA-Z_\-0-9.]*[\\\/])*[a-zA-Z_\-0-9.]*.([sS]1[pP])$)" };

int fileParse(int argc, char** argv)
{
  // Path with .s1p file regexp patter
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
  
  std::string doublePattern{ R"(\-?[1-9]?[0-9]+\.[0-9]+[eE][+\-]?[0-9]{2,})" };
  std::string spacePattern{ R"([ ]{1,2})" };
  std::stringstream ssDataLinePattern;
  ssDataLinePattern << "^"s << "(" << doublePattern << ")" << spacePattern << "(" << doublePattern << ")" << spacePattern << "(" << doublePattern << ")";
  std::regex dataLineRegex{ ssDataLinePattern.str()};

  std::vector<Item> data;

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
      data.push_back({ std::stod(smatch[1]), {std::stod(smatch[2]), std::stod(smatch[3])} });
    else
      ++lines_failed, std::cout << RED << std::setw(4) << std::setfill(' ') << ln + 1 << "|" << line << "\n" << RESET << std::resetiosflags({});
  }

  assert(data.size() == (ln - lines_skiped));

  std::cout << YELLOW;
  std::cout << "Data lines read   : " << data.size() << "\n";
  std::cout << "    Lines total   : " << ln << "\n";
  std::cout << "    Lines skipped : " << lines_skiped << "\n";
  std::cout << "    Lines failed  : " << lines_failed << "\n";
  std::cout << RESET;

  std::cout << GREEN << "Done!" << RESET << "\n";

  return 0;
}

int main(int argc, char** argv)
{
  return fileParse(argc, argv);
}