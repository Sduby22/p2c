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
  #include "ASTNode.h"

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

  static auto logger = logging::getLogger("Parser");

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

/*终结符列表*/

//基本数据类型
%token <uint64_t> CONST_INT
%token <float> CONST_REAL
%token <char> CONST_CHAR
%token <bool> CONST_BOOL
%token <std::string> IDENTIFIER

//标点符号 ( ) , : ; . [ ] '
%token LBRACKET RBRACKET COMMA COLON SEMICOLON DOT LSQUARE_BRACKET RSQUARE_BRACKET SINGLE_QUOTES

//= := > < >= <= <>
%token EQUEL ASSIGN GREATER_THAN LESS_THAN GREATER_EQUEL LESS_EQUEL NOT_EQUEL

//+ - * / div mod
%token ADD MINUS STAR SLASH DIV MOD

//逻辑运算符 and or not
%token AND OR NOT

//..
%token ARRAY_RANGE_SEPARATOR

//保留字 program const var procedure function begin end of
%token PROGRAM CONST VAR PROCEDURE FUNCBEGIN END OF

//逻辑保留字 if then else for to do read write
%token IF THEN ELSE FOR TO DO READ WRITE

//基本类型保留字  integer real boolean char array
%token INTEGER REAL BOOLEAN CHAR ARRAY
%token EOF

// 下面是非终结符列表
%type <ASTNode> program 

%%

// ========== 语法规则 & 语义动作定义区 ===========

/*
  格式 aa : bb cc
         | dd ee
  https://github.com/UO-cis561/reflex-bison-ast/blob/master/src/calc.yxx
*/

// 注：先把ppt上的生成式抄下来，暂时不管二义性的问题。
// 有些二义性可以通过bison的运算符优先级来解决。
program:        IDENTIFIER { logger.info("wow"); }
        ;

%%
/*Parser实现错误处理接口*/
void p2c::Parser::error(const p2c::location& location,const std::string& message){
  logger.error("at line {} column {}: {}", location.begin.line, location.begin.column, message);
}
