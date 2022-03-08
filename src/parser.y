/* 使用指令％skeleton "lalr1.cc"选择C++解析器的骨架 */
%skeleton "lalr1.cc"

/* 指定bison的版本 */
%require "3.0.4"

%define api.namespace { p2c } //声明命名空间与下面声明的类名结合使用 p2c::Parser::  在scanner.l中有体现
%define api.parser.class { Parser }
%define api.token.constructor
%define api.value.type variant //使得类型与token定义可以使用各种复杂的结构与类型
%define parse.assert  //开启断言功能
%code requires
{
  /*requires中的内容会放在YYLTYPE与YYSTPYPE定义前*/
  #include <iostream>
  #include <string>
  #include <cstdint>

  namespace p2c { /*避免包含头文件时冲突*/
    class Scanner;
    class Driver;
  }
}

%code top
{
  /*尽可能放在parser.cpp靠近头部的地方，与requires相似*/
  #include <iostream>
  #include <sstream>
  #include "scanner.h"
  #include "parser.hpp"
  #include "driver.h"
  #include "location.hh"
  #include "logging.h"

  auto logger = logging::getLogger("Parser");

  /*注意：这里的参数由%parse-param决定*/
  static p2c::Parser::symbol_type yylex(p2c::Scanner& scanner,p2c::Driver &driver){
    return scanner.nextToken();
  }
  using namespace p2c;
}

/*定义parser传给scanner的参数*/
%lex-param { p2c::Scanner& scanner }
%lex-param { p2c::Driver& driver }

/*定义driver传给parser的参数*/
%parse-param { p2c::Scanner& scanner }
%parse-param { p2c::Driver& driver }

%locations
//%define parse-trace

/*详细显示错误信息*/
%define parse.error verbose

/*通过p2c::Parser::make_XXX(loc)给token添加前缀*/
%define api.token.prefix {TOKEN_}

%token <std::string>IDENTIFIER
%token <uint64_t>INT
%token <float>REAL

%token EOF ADD SUB DIV MUL 

%%
program: 
%%
/*Parser实现错误处理接口*/
void p2c::Parser::error(const p2c::location& location,const std::string& message){
  std::stringstream ss;
  ss << location;
  logger.error("at {}: {}", ss.str(), message);
}
