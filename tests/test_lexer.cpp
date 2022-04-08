#include "catch2/catch.hpp"
#include <string>
#include <sstream>
#include <fstream>
#include "parser.hpp"
#include "../src/parser/scanner.h"
#include <math.h>
using namespace std;

p2c::Parser::symbol_type get_token(std::string a)
{
  p2c::Scanner scanner;
  std::stringstream ss;
  ss << a;
  scanner.switch_streams(ss, cout);
  p2c::Parser::symbol_type token = scanner.nextToken();
  return token;
}
//字符型
TEST_CASE("[Lexer] char")
{
  p2c::Parser::symbol_type token = get_token("'c'");
  REQUIRE(token.kind() == p2c::Parser::symbol_kind_type::S_CONST_CHAR);
  REQUIRE(token.value.as<char>() == 'c');
}
//浮点数和整数
TEST_CASE("[Lexer] real&int")
{
  p2c::Parser::symbol_type token1 = get_token("1.");
  p2c::Parser::symbol_type token2 = get_token(".1");
  p2c::Parser::symbol_type token3 = get_token("0.1");
  p2c::Parser::symbol_type token4 = get_token("2");
  REQUIRE(token1.kind() == p2c::Parser::symbol_kind_type::S_CONST_INT);
  REQUIRE([&token2]() -> bool { return token2.kind() != p2c::Parser::symbol_kind_type::S_CONST_REAL && token2.kind() != p2c::Parser::symbol_kind_type::S_CONST_INT;}() == 1);
  REQUIRE(token3.kind() == p2c::Parser::symbol_kind_type::S_CONST_REAL);
  REQUIRE(token4.kind() == p2c::Parser::symbol_kind_type::S_CONST_INT);
  REQUIRE([&token3]() -> bool { return fabs(token3.value.as<float>() - 0.1) < 1e-4; }() == 1);
  REQUIRE([&token4]() -> bool { return token4.value.as<uint64_t>() == 2; }() == 1);
}
//BOOL型
TEST_CASE("[Lexer] bool")
{
  p2c::Parser::symbol_type token1 = get_token("1");
  p2c::Parser::symbol_type token2 = get_token("0");
  p2c::Parser::symbol_type token3 = get_token("true");
  p2c::Parser::symbol_type token4 = get_token("False");
  p2c::Parser::symbol_type token5 = get_token("FALSE");
  REQUIRE(token1.kind() != p2c::Parser::symbol_kind_type::S_CONST_BOOL);
  REQUIRE(token2.kind() != p2c::Parser::symbol_kind_type::S_CONST_BOOL);
  REQUIRE(token3.kind() == p2c::Parser::symbol_kind_type::S_CONST_BOOL);
  REQUIRE(token4.kind() == p2c::Parser::symbol_kind_type::S_CONST_BOOL);
  REQUIRE(token5.kind() == p2c::Parser::symbol_kind_type::S_CONST_BOOL);
  REQUIRE([&token3]() -> bool { return token3.value.as<bool>() == true; }() == 1);
  REQUIRE([&token4]() -> bool { return token4.value.as<bool>() == false; }() == 1);
  REQUIRE([&token5]() -> bool { return token5.value.as<bool>() == false; }() == 1);
}
//变量名判断
TEST_CASE("[Lexer] invalid_identifier")
{
  p2c::Parser::symbol_type token1 = get_token("1abc");
  p2c::Parser::symbol_type token2 = get_token("a1bc");
  REQUIRE(token1.kind() != p2c::Parser::symbol_kind_type::S_IDENTIFIER);
  REQUIRE(token2.kind() == p2c::Parser::symbol_kind_type::S_IDENTIFIER);
  REQUIRE(token2.value.as<std::string>() == "a1bc");
}
//关键字(以else为例)
TEST_CASE("[Lexer] keyword")
{
  p2c::Parser::symbol_type token1 = get_token("ELSE");
  p2c::Parser::symbol_type token2 = get_token("eLse");
  REQUIRE(token1.kind() == p2c::Parser::symbol_kind_type::S_ELSE);
  REQUIRE(token2.kind() == p2c::Parser::symbol_kind_type::S_ELSE);
}
//半角全角标点符号
TEST_CASE("[Lexer] punctuation")
{
  p2c::Parser::symbol_type token1 = get_token("；");
  p2c::Parser::symbol_type token2 = get_token(";");
  REQUIRE(token1.kind() != p2c::Parser::symbol_kind_type::S_YYUNDEF);
  REQUIRE(token2.kind() == p2c::Parser::symbol_kind_type::S_SEMICOLON);
}
//==
TEST_CASE("[Lexer] comparison operator")
{
  p2c::Parser::symbol_type token1 = get_token(">=");
  p2c::Parser::symbol_type token2 = get_token(">");
  p2c::Parser::symbol_type token3 = get_token("<=");
  p2c::Parser::symbol_type token4 = get_token("<");
  p2c::Parser::symbol_type token5 = get_token("=");
  p2c::Parser::symbol_type token6 = get_token("<>");
  REQUIRE(token1.kind() == p2c::Parser::symbol_kind_type::S_GREATER_EQUAL);
  REQUIRE(token2.kind() == p2c::Parser::symbol_kind_type::S_GREATER_THAN);
  REQUIRE(token3.kind() == p2c::Parser::symbol_kind_type::S_LESS_EQUAL);
  REQUIRE(token4.kind() == p2c::Parser::symbol_kind_type::S_LESS_THAN);
  REQUIRE(token5.kind() == p2c::Parser::symbol_kind_type::S_EQUAL);
  REQUIRE(token6.kind() == p2c::Parser::symbol_kind_type::S_NOT_EQUAL);
}
TEST_CASE("[Lexer] basic operator")
{
  p2c::Parser::symbol_type token1 = get_token("+");
  p2c::Parser::symbol_type token2 = get_token("-");
  p2c::Parser::symbol_type token3 = get_token("*");
  p2c::Parser::symbol_type token4 = get_token("/");
  p2c::Parser::symbol_type token5 = get_token("div");
  p2c::Parser::symbol_type token6 = get_token("mod");
  REQUIRE(token1.kind() == p2c::Parser::symbol_kind_type::S_ADD);
  REQUIRE(token2.kind() == p2c::Parser::symbol_kind_type::S_MINUS);
  REQUIRE(token3.kind() == p2c::Parser::symbol_kind_type::S_STAR);
  REQUIRE(token4.kind() == p2c::Parser::symbol_kind_type::S_SLASH);
  REQUIRE(token5.kind() == p2c::Parser::symbol_kind_type::S_DIV);
  REQUIRE(token6.kind() == p2c::Parser::symbol_kind_type::S_MOD);
}
TEST_CASE("[Lexer] logic operator")
{
  p2c::Parser::symbol_type token1 = get_token("and");
  p2c::Parser::symbol_type token2 = get_token("or");
  p2c::Parser::symbol_type token3 = get_token("not");
  REQUIRE(token1.kind() == p2c::Parser::symbol_kind_type::S_AND);
  REQUIRE(token2.kind() == p2c::Parser::symbol_kind_type::S_OR);
  REQUIRE(token3.kind() == p2c::Parser::symbol_kind_type::S_NOT);
}
TEST_CASE("[Lexer] comment")
{
  p2c::Parser::symbol_type token1 = get_token("//123\ntrue");
  p2c::Parser::symbol_type token2 = get_token("{123}true");
  REQUIRE(token1.kind() == p2c::Parser::symbol_kind_type::S_CONST_BOOL);
  REQUIRE(token2.kind() == p2c::Parser::symbol_kind_type::S_CONST_BOOL);
}