#include <string>
#include <sstream>
#include <emscripten/bind.h>
#include <logging.h>

#include "driver.h"

namespace es = emscripten;

int main() {}

struct ParseResult {
  bool success = false;
  std::string c_code;
  std::string syntax_tree;
  std::string message;
};

ParseResult parse(std::string pascalCode) {
  std::stringstream is(pascalCode);
  std::stringstream os;
  p2c::Driver driver;
  ParseResult res{};
  logging::clearLog();
  if (driver.parse(is, os) == 0) {
    try {
      res.c_code = driver.getCCodeStr();
      res.syntax_tree = driver.getSyntaxTreeStr();
      res.success = true;
    } catch(...) {
      res.c_code.clear();
      res.syntax_tree.clear();
      res.success = false;
    }
  } 
  res.message = logging::getLog();
  return res;
}

EMSCRIPTEN_BINDINGS(libparser) {
  es::value_object<ParseResult>("ParseResult")
    .field("success", &ParseResult::success)
    .field("c_code", &ParseResult::c_code)
    .field("syntax_tree", &ParseResult::syntax_tree)
    .field("message", &ParseResult::message);

  es::function("parse", &parse);
}