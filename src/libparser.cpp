#include <string>
#include <sstream>
#include <emscripten/bind.h>

#include "driver.h"

namespace es = emscripten;

int main() {}

struct ParseResult {
  bool success;
  std::string c_code;
  std::string syntax_tree;
};

ParseResult parse(std::string pascalCode) {
  std::stringstream is(pascalCode);
  std::stringstream os;
  p2c::Driver driver;
  ParseResult res;
  if (driver.parse(is, os) == 0) {
    res.success = true;
    res.c_code = driver.getCCodeStr();
    res.syntax_tree = driver.getSyntaxTreeStr();
  } else {
    res.success = false;
  }
  return res;
}

EMSCRIPTEN_BINDINGS(libparser) {
  es::value_object<ParseResult>("ParseResult")
    .field("success", &ParseResult::success)
    .field("c_code", &ParseResult::c_code)
    .field("syntax_tree", &ParseResult::syntax_tree);

  es::function("parse", &parse);
}