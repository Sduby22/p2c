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
  using namespace std;
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
%type <std::unique_ptr<ASTNode>> programstruct
%type <std::unique_ptr<ASTNode>> program_head
%type <std::unique_ptr<ASTNode>> program_body
%type <std::unique_ptr<ASTNode>> idlist
%type <std::unique_ptr<ASTNode>> const_declarations
%type <std::unique_ptr<ASTNode>> const_declaration
%type <std::unique_ptr<ASTNode>> const_value
%type <std::unique_ptr<ASTNode>> var_declarations
%type <std::unique_ptr<ASTNode>> var_declaration
%type <std::unique_ptr<ASTNode>> type
%type <std::unique_ptr<ASTNode>> basic_type
%type <std::unique_ptr<ASTNode>> period
%type <std::unique_ptr<ASTNode>> subprogram_declarations
%type <std::unique_ptr<ASTNode>> subprogram
%type <std::unique_ptr<ASTNode>> subprogram_head
%type <std::unique_ptr<ASTNode>> formal_parameter
%type <std::unique_ptr<ASTNode>> parameter_list
%type <std::unique_ptr<ASTNode>> parameter
%type <std::unique_ptr<ASTNode>> var_parameter
%type <std::unique_ptr<ASTNode>> value_parameter
%type <std::unique_ptr<ASTNode>> subprogram_body
%type <std::unique_ptr<ASTNode>> compound_statement
%type <std::unique_ptr<ASTNode>> statement
%type <std::unique_ptr<ASTNode>> statement_list
%type <std::unique_ptr<ASTNode>> variable_list
%type <std::unique_ptr<ASTNode>> variable
%type <std::unique_ptr<ASTNode>> id_varpart
%type <std::unique_ptr<ASTNode>> procedure_call
%type <std::unique_ptr<ASTNode>> else_part
%type <std::unique_ptr<ASTNode>> expression_list
%type <std::unique_ptr<ASTNode>> expression
%type <std::unique_ptr<ASTNode>> relop
%type <std::unique_ptr<ASTNode>> simple_expression
%type <std::unique_ptr<ASTNode>> addop
%type <std::unique_ptr<ASTNode>> term
%type <std::unique_ptr<ASTNode>> mulop
%type <std::unique_ptr<ASTNode>> factor
%type <std::unique_ptr<ASTNode>> num

%%

// ========== 语法规则 & 语义动作定义区 ===========

/*
  格式 aa : bb cc
        
                {
                  $$ = nullptr;
                } dd ee
  https://github.com/UO-cis561/reflex-bison-ast/blob/master/src/calc.yxx
*/

// 注：先把ppt上的生成式抄下来，暂时不管二义性的问题。
// 有些二义性可以通过bison的运算符优先级来解决。
programstruct:
  program_head SEMICOLON program_body DOT
                {
                  $$ = nullptr;
                };

program_head:
  PROGRAM IDENTIFIER LBRACKET idlist RBRACKET
                {
                  $$ = nullptr;
                }
  | PROGRAM IDENTIFIER
                {
                  $$ = nullptr;
                };

program_body:
  const_declarations var_declarations subprogram_declarations compound_statement
                {
                  $$ = nullptr;
                };

idlist:
  idlist COMMA IDENTIFIER
                {
                  $$ = nullptr;
                }
  | IDENTIFIER
                {
                  $$ = nullptr;
                };

const_declarations:
  CONST const_declaration SEMICOLON
                {
                  $$ = nullptr;
                }
  | /* %empty */
                {
                  $$ = nullptr;
                };

const_declaration:
  const_declaration SEMICOLON IDENTIFIER EQUAL const_value
                {
                  $$ = nullptr;
                }
  | IDENTIFIER EQUAL const_value
                {
                  $$ = nullptr;
                };

const_value:
  ADD num
                {
                  $$ = nullptr;
                }
  | MINUS num
                {
                  $$ = nullptr;
                }
  | num
                {
                  $$ = nullptr;
                }
  | CONST_CHAR
                {
                  $$ = nullptr;
                };

var_declarations:
  VAR var_declaration SEMICOLON
                {
                  $$ = nullptr;
                }
  | /* %empty */
                {
                  $$ = nullptr;
                };

var_declaration:
  var_declaration SEMICOLON idlist COLON type
                {
                  $$ = nullptr;
                }
  | idlist COLON type
                {
                  $$ = nullptr;
                };

type:
  basic_type
                {
                  $$ = nullptr;
                }
  | ARRAY LSQUARE_BRACKET period RSQUARE_BRACKET OF basic_type
                {
                  $$ = nullptr;
                };

basic_type:
  INTEGER
                {
                  $$ = nullptr;
                }
  | REAL
                {
                  $$ = nullptr;
                }
  | BOOLEAN
                {
                  $$ = nullptr;
                }
  | CHAR
                {
                  $$ = nullptr;
                };

period:
  period COMMA CONST_INT ARRAY_RANGE_SEPARATOR CONST_INT
                {
                  $$ = nullptr;
                }
  | CONST_INT ARRAY_RANGE_SEPARATOR CONST_INT
                {
                  $$ = nullptr;
                };

subprogram_declarations:
  subprogram_declarations subprogram SEMICOLON
                {
                  $$ = nullptr;
                }
  |  /* %empty */
                {
                  $$ = nullptr;
                };

subprogram:
  subprogram_head SEMICOLON subprogram_body
                {
                  $$ = nullptr;
                };

subprogram_head:
  PROCEDURE IDENTIFIER formal_parameter
                {
                  $$ = nullptr;
                }
  | FUNC IDENTIFIER formal_parameter COLON basic_type
                {
                  $$ = nullptr;
                };

formal_parameter:
  LBRACKET parameter_list RBRACKET
                {
                  $$ = nullptr;
                }
  |  /* %empty */
                {
                  $$ = nullptr;
                };

parameter_list:
  parameter_list SEMICOLON parameter
                {
                  $$ = nullptr;
                }
  | parameter
                {
                  $$ = nullptr;
                };

parameter:
  var_parameter
                {
                  $$ = nullptr;
                }
  | value_parameter
                {
                  $$ = nullptr;
                };

var_parameter:
  VAR value_parameter
                {
                  $$ = nullptr;
                };

value_parameter:
  idlist COLON basic_type
                {
                  $$ = nullptr;
                };

subprogram_body:
  const_declarations var_declarations compound_statement
                {
                  $$ = nullptr;
                };

compound_statement:
  BEGIN statement_list END
                {
                  $$ = nullptr;
                };

statement_list:
  statement_list SEMICOLON statement
                {
                  $$ = nullptr;
                }
  | statement
                {
                  $$ = nullptr;
                };

statement:
  variable ASSIGN expression
                {
                  $$ = nullptr;
                }
  | procedure_call
                {
                  $$ = nullptr;
                }
  | compound_statement
                {
                  $$ = nullptr;
                }
  | IF expression THEN statement else_part
                {
                  $$ = nullptr;
                }
  | FOR IDENTIFIER ASSIGN expression TO expression DO statement
                {
                  $$ = nullptr;
                }
  | READ LBRACKET variable_list RBRACKET
                {
                  $$ = nullptr;
                }
  | WRITE LBRACKET expression_list RBRACKET
                {
                  $$ = nullptr;
                }
  | /* %empty */
                {
                  $$ = nullptr;
                };

variable_list:
  variable_list COMMA variable
                {
                  $$ = nullptr;
                }
  | variable
                {
                  $$ = nullptr;
                };

variable:
  IDENTIFIER id_varpart
                {
                  $$ = nullptr;
                };

id_varpart:
  LSQUARE_BRACKET expression_list RSQUARE_BRACKET
                {
                  $$ = nullptr;
                }
  |  /* %empty */
                {
                  $$ = nullptr;
                };

procedure_call:
  IDENTIFIER
                {
                  $$ = nullptr;
                }
  | IDENTIFIER LBRACKET expression_list RBRACKET
                {
                  $$ = nullptr;
                };

else_part:
  ELSE statement
                {
                  $$ = nullptr;
                }
  |  /* %empty */
                {
                  $$ = nullptr;
                };

expression_list:
  expression_list COMMA expression
                {
                  $$ = nullptr;
                }
  | expression
                {
                  $$ = nullptr;
                };

expression:
  simple_expression relop simple_expression
                {
                  $$ = nullptr;
                }
  | simple_expression
                {
                  $$ = nullptr;
                };

relop:
  GREATER_THAN
                {
                  $$ = nullptr;
                }
  | LESS_THAN
                {
                  $$ = nullptr;
                }
  | GREATER_EQUAL
                {
                  $$ = nullptr;
                }
  | LESS_EQUAL
                {
                  $$ = nullptr;
                }
  | EQUAL
                {
                  $$ = nullptr;
                }
  | NOT_EQUAL
                {
                  $$ = nullptr;
                };

simple_expression:
  simple_expression addop term
                {
                  $$ = nullptr;
                }
  | term
                {
                  $$ = nullptr;
                };

addop:
  ADD
                {
                  $$ = nullptr;
                }
  | MINUS
                {
                  $$ = nullptr;
                }
  | OR
                {
                  $$ = nullptr;
                };

term:
  term mulop factor
                {
                  $$ = nullptr;
                }
  | factor
                {
                  $$ = nullptr;
                };

mulop:
  STAR
                {
                  $$ = nullptr;
                }
  | SLASH
                {
                  $$ = nullptr;
                }
  | DIV
                {
                  $$ = nullptr;
                }
  | MOD
                {
                  $$ = nullptr;
                }
  | AND
                {
                  $$ = nullptr;
                };

factor:
  num
                {
                  $$ = nullptr;
                }
  | variable
                {
                  $$ = nullptr;
                }
  | IDENTIFIER LBRACKET expression_list RBRACKET
                {
                  $$ = nullptr;
                }
  | LBRACKET expression RBRACKET
                {
                  $$ = nullptr;
                }
  | NOT factor
                {
                  $$ = nullptr;
                }
  | MINUS factor
                {
                  $$ = nullptr;
                };

num:
  CONST_INT
                {
                  $$ = nullptr;
                }
  | CONST_REAL
                {
                  $$ = nullptr;
                };

%%
/*Parser实现错误处理接口*/
void p2c::Parser::error(const p2c::location& location,const std::string& message){
  logger.error("at line {} column {}: {}", location.begin.line, location.begin.column, message);
}
