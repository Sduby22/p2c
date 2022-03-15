#include "cxxopts.hpp"
#include "driver.h"
#include "logging.h"
#include "spdlog/common.h"
#include "spdlog/spdlog.h"
#include <cstdio>
#include <string>

using namespace std;
static auto logger = logging::getLogger("Main");

int parse_file() {
  p2c::Driver driver;
  return driver.parse();
}

void parse_cmd(int argc, char *argv[]) {
  cxxopts::Options opts("p2c", "A Pascal-S to C translator");
  // clang-format off
  opts.add_options()
    ("d,debug", "Enable debugging")
    ("i,input", "Input File", cxxopts::value<string>())
    ("o,output", "Output File", cxxopts::value<string>())
    ("v,verbose", "Verbose output", cxxopts::value<bool>()->default_value("false"))
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

    parse_file();
  } catch (const cxxopts::OptionException &e) {
    cout << opts.help();
  }
}

int main(int argc, char *argv[]) {
  parse_cmd(argc, argv);

  return 0;
}
