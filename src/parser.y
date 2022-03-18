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
%token EQUAL ASSIGN GREATER_THAN LESS_THAN GREATER_EQUAL LESS_EQUAL NOT_EQUAL

//+ - * / div mod
%token ADD MINUS STAR SLASH DIV MOD

//逻辑运算符 and or not
%token AND OR NOT

//..
%token ARRAY_RANGE_SEPARATOR

//保留字 program const var procedure function begin end of
%token PROGRAM CONST VAR PROCEDURE FUNC BEGIN END OF

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
programstruct:
  program_head SEMICOLON program_body DOT;

program_head:
  PROGRAM IDENTIFIER LBRACKET idlist RBRACKET |
  PROGRAM IDENTIFIER;

program_body:
  const_declarations var_declarations subprogram_declarations compound_statement;

/* this is now only used for parameters */
idlist:
  idlist COMMA IDENTIFIER |
  IDENTIFIER;

const_declarations:
  CONST const_declaration SEMICOLON |
  ;

const_declaration:
  const_declaration SEMICOLON IDENTIFIER EQUAL const_value |
  IDENTIFIER EQUAL const_value;

const_value:
  ADD num |
  MINUS num |
  num |
  SINGLE_QUOTES CONST_CHAR SINGLE_QUOTES;

var_declarations:
  VAR var_declaration SEMICOLON |
  ;

var_declaration:
  var_declaration SEMICOLON idlist COLON type |
  idlist COLON type;

type:
  basic_type |
  ARRAY LSQUARE_BRACKET period RSQUARE_BRACKET OF basic_type;

basic_type:
  INTEGER |
  REAL |
  BOOLEAN |
  CHAR;

period:
  period COMMA CONST_INT ARRAY_RANGE_SEPARATOR CONST_INT |
  CONST_INT ARRAY_RANGE_SEPARATOR CONST_INT;

subprogram_declarations: 
  subprogram_declarations subprogram SEMICOLON |
  ;

subprogram:
  subprogram_head SEMICOLON subprogram_body;

subprogram_head:
  PROCEDURE IDENTIFIER formal_parameter |
  FUNC IDENTIFIER formal_parameter COLON basic_type;

formal_parameter:
  LBRACKET parameter_list RBRACKET |
  ;

parameter_list:
  parameter_list SEMICOLON parameter |
  parameter;

parameter:
  var_parameter |
  value_parameter;

var_parameter:
  VAR value_parameter;

value_parameter:
  idlist COLON basic_type;

subprogram_body:
  const_declarations var_declarations compound_statement;

compound_statement:
  BEGIN statement_list END;

statement_list:
  statement_list SEMICOLON statement |
  statement;

statement:
  variable ASSIGN expression |
  procedure_call |
  compound_statement |
  IF expression THEN statement else_part |
  FOR IDENTIFIER ASSIGN expression TO expression DO statement |
  READ LBRACKET variable_list RBRACKET |
  WRITE LBRACKET expression_list RBRACKET |
  ;

variable_list:
  variable_list COMMA variable |
  variable;

variable:
  IDENTIFIER id_varpart;

id_varpart:
  LSQUARE_BRACKET expression_list RSQUARE_BRACKET |
  ;

procedure_call:
  IDENTIFIER |
  IDENTIFIER LBRACKET expression_list RBRACKET;

else_part:
  ELSE statement |
  ;

expression_list:
  expression_list COMMA expression |
  expression;

expression:
  simple_expression relop simple_expression |
  simple_expression;

relop:
  GREATER_THAN |
  LESS_THAN |
  GREATER_EQUAL |
  LESS_EQUAL |
  EQUAL |
  NOT_EQUAL;

simple_expression:
  simple_expression addop term |
  term;

addop:
  ADD |
  MINUS |
  OR

term:
  term mulop factor |
  factor;

mulop:
  STAR |
  SLASH |
  DIV |
  MOD |
  AND

factor:
  num |
  variable |
  IDENTIFIER LBRACKET expression_list RBRACKET |
  LBRACKET expression RBRACKET |
  NOT factor |
  MINUS factor;

num:
  CONST_INT |
  CONST_REAL

%%
/*Parser实现错误处理接口*/
void p2c::Parser::error(const p2c::location& location,const std::string& message){
  logger.error("at line {} column {}: {}", location.begin.line, location.begin.column, message);
}
