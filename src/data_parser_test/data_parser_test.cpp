#include <iostream>
#include <fstream>
#include <regex>
#include <iomanip>
#include <sstream>

#include <data_parser_lib.hpp>

int main(int argc, char** argv)
{
  // Windows or Linux path
  const std::regex reFilename("^(([A-Za-z]:)?[\\\\\\/])?([a-zA-Z_\\-0-9\.]*[\\\\\\/])*[a-zA-Z_\\-0-9\.]*.([sS]1[pP])$");
  if (argc < 2 || false == std::regex_match(argv[1], reFilename))
  {
    std::cerr << "The only argument must be data file name with \"s1p\" extension." << "\n";
    return 1;
  }

  std::ifstream data_file(argv[1], std::ios_base::in);
  if (false == data_file.is_open())
  {
    std::cerr << "Failed to open file" << "\n";
    return 1;
  }

  std::string fpnpt{ "\\-?([0-9]+\\.[0-9]+)([eE][+\\-][0-9]+)?"};
  std::string fpnln{ fpnpt + " " + fpnpt + "  " + fpnpt};

  std::regex fpnre(fpnln);
  std::string numbers[]
  {
    "3.00000000000E+05 -4.87399039514E-01  7.84540075739E-01",
    "4.52985000000E+07 -4.14090484138E-01  8.24439150397E-01",
    "9.02970000000E+07 -3.39253327384E-01  8.58125275881E-01",
    "1.35295500000E+08 -2.60341943202E-01  8.85740900599E-01",
    "1.80294000000E+08 -1.80839904414E-01  9.05237745511E-01",
  };

  for(const auto& num: numbers)
  {
    std::smatch match;
    if (false == std::regex_match(num, match, fpnre))
      std::cerr << "invalid: " << num << "\n";
    else
      std::cerr << "valid: " << num << "\n";
  }

  std::string line;
  for(size_t ln = 1; std::getline(data_file, line); ++ln)
  {
    std::cout << std::setw(4) << std::setfill(' ') << ln << "|" << line << "\n" << std::resetiosflags({});
  }

  std::cout << "Done!" << "\n";
  
  return 0;
}