#include "catch2/catch.hpp"
#include <string>
#include <sstream>
#include "parser.hpp"
#include "scanner.h"

using namespace std;

TEST_CASE("[Lexer] char") {
  p2c::Scanner scanner;
  std::stringstream ss;
  ss << "'c'";

  scanner.switch_streams(ss, cout);
  p2c::Parser::symbol_type token = scanner.nextToken();

  REQUIRE(token.kind() == p2c::Parser::symbol_kind_type::S_CONST_CHAR);
  REQUIRE(token.value.as<char>() == 'c');
}