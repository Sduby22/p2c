#include "cxxopts.hpp"
#include "logging.h"
#include <cstdio>
#include <string>

using namespace std;
auto logger = logging::getLogger("Main");

void parse(int argc, char *argv[]) {
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
      logging::setGlobalLevel(logging::LEVEL::DEBUG);
    else if (result.count("verbose"))
      logging::setGlobalLevel(logging::LEVEL::INFO);
    else
      logging::setGlobalLevel(logging::LEVEL::WARN);

    if (result.count("input") == 0) {
      logger->critical("no input file");
    }
  } catch (const cxxopts::OptionException &e) {
    cout << opts.help();
  }
}

int main(int argc, char *argv[]) {
  parse(argc, argv);

  for (int i = 0; i != 3; i++) {
    logger->debug("test debug log {}", i);
    logger->info("test info log {}", i);
    logger->warn("test warn log {}", i);
    logger->error("test error log {}", i);
  }

  return 0;
}
