#include "cxxopts.hpp"
#include "driver.h"
#include "logging.h"
#include <cstdio>
#include <fstream>
#include <stdexcept>
#include <string>

using namespace std;
static auto logger = logging::getLogger("Main");

int parse_file(const string &ifile, const string &ofile) {
  std::ifstream is;
  std::ofstream os;
  if (ifile != "-") {
    is.open(ifile);
    if (!is.good()) {
      logger.critical("Cannot open file: {}", ifile);
      return -1;
    }
  }
  if (ofile != "-") {
    os.open(ofile);
    if (!os.good()) {
      logger.critical("Cannot open file: {}", ofile);
      return -1;
    }
  }
  p2c::Driver driver;
  int res = driver.parse(is.is_open() ? is : cin, os.is_open() ? os : cout);
  if(os.is_open()) {
    os << driver.getCCodeStr();
  } else {
    cout << driver.getCCodeStr();
  }
  return res;
}

void parse_cmd(int argc, char *argv[]) {
  cxxopts::Options opts("p2c", "A Pascal-S to C translator");
  // clang-format off
  opts.add_options()
    ("d,debug", "Enable debugging")
    ("i,input", "Input File", cxxopts::value<string>())
    ("o,output", "Output File", cxxopts::value<string>()->default_value("a.out.c"))
    ("v,verbose", "Verbose output", cxxopts::value<bool>())
    ("h,help", "Print this help message");
  // clang-format on
  opts.parse_positional({"input"});

  try {
    auto result = opts.parse(argc, argv);
    // print help
    if (result.count("help")) {
      cout << opts.help();
      return;
    }

    // set log level
    if (result.count("debug"))
      logging::setGlobalLevel(logging::LEVEL::debug);
    else if (result.count("verbose"))
      logging::setGlobalLevel(logging::LEVEL::info);
    else
      logging::setGlobalLevel(logging::LEVEL::warn);

    if (result.count("input") == 0) {
      logger.critical("no input file");
    }

    parse_file(result["input"].as<string>(), result["output"].as<string>());
  } catch (const cxxopts::OptionException &e) {
    cout << opts.help();
  }
}

int main(int argc, char *argv[]) {
  parse_cmd(argc, argv);

  return 0;
}
