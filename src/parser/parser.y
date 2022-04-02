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
  #include <variant>
  #include <cstdint>
  #include "ASTNode.h"

  namespace p2c { /*避免包含头文件时冲突*/
    class Scanner;
    class Driver;
  }

  using namespace std;
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
  static p2c::Parser::symbol_type yylex(p2c::Scanner& scanner){
    return scanner.nextToken();
  }
  using namespace p2c;
  using namespace std;
}

/*定义parser传给scanner的参数*/
%lex-param { p2c::Scanner& scanner }

/*定义driver传给parser的参数*/
%parse-param { p2c::Scanner& scanner }
/* %parse-param { p2c::Driver& driver } */

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
%token <string> IDENTIFIER

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
%token EOF 0 "end of file"

%precedence THEN
%precedence ELSE

// 下面是非终结符列表
%type <unique_ptr<ASTNode>> programstruct
%type <unique_ptr<ASTNode>> program_head
%type <unique_ptr<ASTNode>> program_body
%type <unique_ptr<ConstDeclarations>> const_declarations
%type <unique_ptr<ConstDeclarations>> const_declaration
%type <vector<unique_ptr<VarDeclaration>>> var_declarations
%type <vector<unique_ptr<VarDeclaration>>> var_declaration
%type <unique_ptr<ASTNode>> subprogram_declarations
%type <unique_ptr<ASTNode>> subprogram
%type <unique_ptr<ASTNode>> subprogram_head
%type <unique_ptr<ASTNode>> formal_parameter
%type <unique_ptr<ASTNode>> parameter_list
%type <unique_ptr<ASTNode>> parameter
%type <unique_ptr<ASTNode>> var_parameter
%type <unique_ptr<ASTNode>> value_parameter
%type <unique_ptr<ASTNode>> subprogram_body
%type <unique_ptr<CompoundStatement>> compound_statement
%type <unique_ptr<Statement>> statement
%type <unique_ptr<StatementList>> statement_list
%type <unique_ptr<VariableList>> variable_list
%type <unique_ptr<Variable>> variable
%type <unique_ptr<IdVarpart>> id_varpart
%type <unique_ptr<ProcedureCall>> procedure_call
%type <vector<unique_ptr<Expression>>> expression_list
%type <unique_ptr<Expression>> expression
%type <unique_ptr<SimpleExpression>> simple_expression
%type <unique_ptr<Term>> term
%type <unique_ptr<Factor>> factor

%type <variant<uint64_t, float>> num
%type <Operator> relop
%type <Operator> addop
%type <Operator> mulop
%type <variant<BasicType, ArrayType>> type
%type <BasicType> basic_type
%type <vector<tuple<int, int>>> period
%type <vector<string>> idlist
%type <variant<int64_t, float, char>> const_value
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
  /* CONST_INT CONST_REAL CONST_BOOL CONST_CHAR IDENTIFIER */
                {
                  logger.debug("wow");
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

const_declarations:
  CONST const_declaration SEMICOLON
                {
                  $$ = move($2);
                  logger.debug($$->printNode());
                }
  | /* %empty */
                {
                  $$ = make_unique<ConstDeclarations>();
                };

const_declaration:
  const_declaration SEMICOLON IDENTIFIER EQUAL const_value
                {
                  $$ = move($1);
                  auto node = make_unique<ConstDeclaration>();
                  node->identifier = $3;
                  node->const_value = $5;
                  $$->appendChild(move(node));
                }
  | IDENTIFIER EQUAL const_value
                {
                  $$ = make_unique<ConstDeclarations>();
                  auto node = make_unique<ConstDeclaration>();
                  node->identifier = $1;
                  node->const_value = $3;
                  $$->appendChild(move(node));
                };

var_declarations:
  VAR var_declaration SEMICOLON
                {
                  $$ = move($2);
                  for (auto& declaration: $$) {
                    logger.debug(declaration->printNode());
                  }
                }
  | /* %empty */
                {
                  $$ = vector<unique_ptr<VarDeclaration>>{};
                };

var_declaration:
  var_declaration SEMICOLON idlist COLON type
                {
                  $$ = move($1);
                  auto node = make_unique<VarDeclaration>();
                  node->idlist = move($3);
                  node->type = move($5);
                  $$.push_back(move(node));
                }
  | idlist COLON type
                {
                  $$ = vector<unique_ptr<VarDeclaration>>{};
                  auto node = make_unique<VarDeclaration>();
                  node->idlist = move($1);
                  node->type = move($3);
                  $$.push_back(move(node));
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
                  $$ = make_unique<CompoundStatement>();
                  $$->appendChild(move($2));
                  logger.debug($$->printNode());
                };

statement_list:
  statement_list SEMICOLON statement
                {
                  $$ = move($1);
                  $$->appendChild(move($3));
                  logger.debug($$->printNode());
                }
  | statement
                {
                  $$ = make_unique<StatementList>();
                  $$->appendChild(move($1));
                  logger.debug($$->printNode());
                };

statement:
  variable ASSIGN expression
                {
                  $$ = make_unique<Statement>();
                  $$->appendChild(move($1));
                  $$->appendChild(move($3));
                  $$->type = 1;
                  $$->type_name = "var_assign";
                  logger.debug($$->printNode());
                }
  | procedure_call
                {
                  $$ = make_unique<Statement>();
                  $$->appendChild(move($1));
                  $$->type = 2;
                  $$->type_name = "procedure_call";
                  logger.debug($$->printNode());
                }
  | compound_statement
                {
                  $$ = make_unique<Statement>();
                  $$->appendChild(move($1));
                  $$->type = 3;
                  $$->type_name = "compound_statement";
                  logger.debug($$->printNode());
                }
  | IF expression THEN statement ELSE statement
                {
                  $$ = make_unique<Statement>();
                  $$->appendChild(move($2));
                  $$->appendChild(move($4));
                  $$->appendChild(move($6));
                  $$->type = 4;
                  $$->type_name = "if_else_else";
                  logger.debug($$->printNode());
                }
  | IF expression THEN statement 
                {
                  $$ = make_unique<Statement>();
                  $$->appendChild(move($2));
                  $$->appendChild(move($4));
                  $$->type = 5;
                  $$->type_name = "if_else";
                  logger.debug($$->printNode());
                }
  | FOR IDENTIFIER ASSIGN expression TO expression DO statement
                {
                  $$ = make_unique<Statement>();
                  $$->appendChild(move($4));
                  $$->appendChild(move($6));
                  $$->appendChild(move($8));
                  $$->type = 6;
                  $$->type_name = "for_loop";
                  $$->type_info = move($2);
                  logger.debug($$->printNode());
                }
  | READ LBRACKET variable_list RBRACKET
                {
                  $$ = make_unique<Statement>();
                  $$->appendChild(move($3));
                  $$->type = 7;
                  $$->type_name = "read";
                  logger.debug($$->printNode());
                }
  | WRITE LBRACKET expression_list RBRACKET
                {
                  $$ = make_unique<Statement>();
                  for (auto& expression: $3) {
                    $$->appendChild(move(expression));
                  }
                  $$->type = 8;
                  $$->type_name = "write";
                  logger.debug($$->printNode());
                }
  |   /*empty*/
                {
                  $$ = make_unique<Statement>();
                  $$->type = 9;
                  $$->type_name = "empty";
                  logger.debug($$->printNode());
                };

variable_list:
  variable_list COMMA variable
                {
                  $$ = move($1);
                  $$->appendChild(move($3));
                }
  | variable
                {
                  $$ = make_unique<VariableList>();
                  $$->appendChild(move($1));
                };

variable:
  IDENTIFIER id_varpart
                {
                  $$ = make_unique<Variable>();
                  $$->identifier = move($1);
                  $$->appendChild(move($2));
                };

id_varpart:
  LSQUARE_BRACKET expression_list RSQUARE_BRACKET //access expression-node directly
                {
                  $$ = make_unique<IdVarpart>();
                  for (auto& expression: $2) {
                    $$->appendChild(move(expression));
                  }
                  $$->isEmpty = false;
                }
  |  /* %empty */
                {
                  $$ = make_unique<IdVarpart>();
                  $$->isEmpty = true;
                };

procedure_call:
  IDENTIFIER
                {
                  $$ = make_unique<ProcedureCall>();
                  $$->identifier = move($1);
                  logger.debug($$->printNode());
                }
  | IDENTIFIER LBRACKET expression_list RBRACKET
                {
                  $$ = make_unique<ProcedureCall>();
                  $$->identifier = move($1);
                  for (auto& expression: $3) {
                    $$->appendChild(move(expression));
                  }
                  logger.debug($$->printNode());
                };

expression_list:
  expression_list COMMA expression
                {
                  $$ = move($1);
                  $$.push_back(move($3));
                  logger.debug("<expression-list>");
                }
  | expression
                {
                  $$ = vector<unique_ptr<Expression>>{};
                  $$.push_back(move($1));
                  logger.debug("<expression-list>");
                };

expression:
  simple_expression relop simple_expression
                {
                  $$ = make_unique<Expression>();
                  $$->relop = move($2);
                  $$->appendChild(move($1));
                  $$->appendChild(move($3));
                }
  | simple_expression
                {
                  $$ = make_unique<Expression>();
                  $$->appendChild(move($1));
                };

simple_expression:
  simple_expression addop term
                {
                  $$ = move($1);
                  $$->addops.push(move($2));
                  $$->appendChild(move($3));
                }
  | term
                {
                  $$ = make_unique<SimpleExpression>();
                  $$->appendChild(move($1));
                };

term:
  term mulop factor
                {
                  $$ = move($1);
                  $$->mulops.push(move($2));
                  $$->appendChild(move($3));
                }
  | factor
                {
                  $$ = make_unique<Term>();
                  $$->appendChild(move($1));
                };

factor:
  num
                {
                  $$ = make_unique<Factor>();
                  $$->type = 1;
                  if (holds_alternative<uint64_t>($1)){
                    $$->value = to_string(get<0>($1));
                  }
                  else{
                    $$->value = to_string(get<1>($1));
                  }
                }
  | variable
                {
                  $$ = make_unique<Factor>();
                  $$->type = 2;
                  $$->appendChild(move($1));
                }
  | IDENTIFIER LBRACKET expression_list RBRACKET
                {
                  $$ = make_unique<Factor>();
                  $$->type = 3;
                  $$->value = $1;
                  for (auto& expression: $3) {
                    $$->appendChild(move(expression));
                  }
                  logger.debug($$->printNode());
                }
  | LBRACKET expression RBRACKET
                {
                  $$ = make_unique<Factor>();
                  $$->type = 4;
                  $$->appendChild(move($2));
                }
  | NOT factor
                {
                  $$ = make_unique<Factor>();
                  $$->type = 5;
                  $$->appendChild(move($2));
                }
  | MINUS factor
                {
                  $$ = make_unique<Factor>();
                  $$->type = 6;
                  $$->appendChild(move($2));
                };

idlist:
  idlist COMMA IDENTIFIER
                {
                  $$ = move($1);
                  $$.push_back($3);
                }
  | IDENTIFIER
                {
                  $$ = vector<string>{};
                  $$.push_back($1);
                };

num:
  CONST_INT
                {
                  $$ = $1;
                }
  | CONST_REAL
                {
                  $$ = $1;
                };

addop:
  ADD                
                {
                  $$ = Operator::ADD;
                }
  | MINUS
                {
                  $$ = Operator::MINUS;
                }
  | OR
                {
                  $$ = Operator::OR;
                };

mulop:
  STAR
                {
                  $$ = Operator::STAR;
                }
  | SLASH
                {
                  $$ = Operator::SLASH;
                }
  | DIV
                {
                  $$ = Operator::DIV;
                }
  | MOD
                {
                  $$ = Operator::MOD;
                }
  | AND
                {
                  $$ = Operator::AND;
                };

relop:
  GREATER_THAN
                {
                  $$ = Operator::GREATER_THAN;
                }
  | LESS_THAN
                {
                  $$ = Operator::LESS_THAN;
                }
  | GREATER_EQUAL
                {
                  $$ = Operator::GREATER_EQUAL;
                }
  | LESS_EQUAL
                {
                  $$ = Operator::LESS_EQUAL;
                }
  | EQUAL
                {
                  $$ = Operator::EQUAL;
                }
  | NOT_EQUAL
                {
                  $$ = Operator::NOT_EQUAL;
                };

type:
  basic_type
                {
                  $$ = $1;
                }
  | ARRAY LSQUARE_BRACKET period RSQUARE_BRACKET OF basic_type
                {
                  ArrayType arrayType = {$6, $3};
                  $$ = arrayType;
                };

basic_type:
  INTEGER
                {
                  $$ = BasicType::INTEGER;
                }
  | REAL
                {
                  $$ = BasicType::REAL;
                }
  | BOOLEAN
                {
                  $$ = BasicType::BOOLEAN;
                }
  | CHAR
                {
                  $$ = BasicType::CHAR;
                };

period:
  period COMMA CONST_INT ARRAY_RANGE_SEPARATOR CONST_INT
                {
                  $$ = move($1);
                  $$.push_back({$3, $5});
                }
  | CONST_INT ARRAY_RANGE_SEPARATOR CONST_INT
                {
                  $$ = vector<tuple<int, int>>{};
                  $$.push_back({$1, $3});
                };

const_value:
  ADD num
                {
                  visit([&](auto &val) { 
                    using T = decay_t<decltype(val)>;
                    if constexpr (std::is_same_v<T, uint64_t>) {
                      $$ = static_cast<int64_t>(val);
                    } else {
                      $$ = val;
                    }
                  }, $2);
                }
  | MINUS num
                {
                  visit([&](auto &val) { 
                    using T = decay_t<decltype(val)>;
                    if constexpr (std::is_same_v<T, uint64_t>) {
                      $$ = -static_cast<int64_t>(val);
                    } else {
                      $$ = -val;
                    }
                  }, $2);
                }
  | num
                {
                  visit([&](auto &val) { 
                    using T = decay_t<decltype(val)>;
                    if constexpr (std::is_same_v<T, uint64_t>) {
                      $$ = static_cast<int64_t>(val);
                    } else {
                      $$ = val;
                    }
                  }, $1);
                }
  | CONST_CHAR
                {
                  $$ = $1;
                };

%%
/*Parser实现错误处理接口*/
void p2c::Parser::error(const p2c::location& location,const string& message){
  logger.error("at line {} column {}: {}", location.begin.line, location.begin.column, message);
}
