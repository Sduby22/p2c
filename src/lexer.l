%{
  #include <iostream>
  #include <cerrno>
  #include <climits>
  #include <cstdlib>
  #include <string>

  #include "parser.hpp"  //包含由parser.y生成的头文件
  #include "scanner.h"   //包含yyFlexLexer子类的头文件
  #include "location.hh" //包含位置调试信息头文件
  #include "logging.h"

  static auto logger = logging::getLogger("Lexer");
  static p2c::location loc;//声明位置实例
  /* 定义了YY_USER_ACTION，该宏在每个记号的语义动作之前被调用，来根据记号的长度设置位置的信息 */
  #define YY_USER_ACTION  loc.columns (yyleng); 

  #undef yywrap
  #define yywrap() 1

  using namespace p2c;
  using namespace std;
  #define yyterminate() Parser::make_EOF(loc);
%}
/* 声明使用C++版本FLEXER，c++是大小写不敏感的 */
%option c++

/* 支持调试 */
%option noyywrap debug

/* 使用Scanner::yylex() */
%option yyclass="Scanner"

/* 一些与编译常量使用该前缀否则为yy */
%option prefix="p2c_"

/*正则式的各种简写，使程序有层次感而显得明晰 */

/* 基本符号，包含正确的高优先级字符和低优先级字符的错误形式 */
/* 按优先级排序 修改时确保 callback 顺序符合符号定义顺序 */
letter                  [a-zA-Z]
digits                  [0-9]
comment                 (\/\/[^\n]*)|(\{[^}\n]*\})
noquote                 [^'^\n]
charac                  ['].[']
wrong_identifier        {digits}+({letter})({digits}|{letter})*
quotation_not_match     (\'|\")
whitespace              [\s\t]
newline                 \r\n|\n\r|\n|\r

/* 关键字 */
key_program             [Pp][Rr][Oo][Gg][Rr][Aa][Mm]
key_const               [Cc][Oo][Nn][Ss][Tt]
key_var                 [Vv][Aa][Rr]
key_int                 [Ii][Nn][Tt][Ee][Gg][Ee][Rr]
key_bool                [Bb][Oo][Oo][Ll][Ee][Aa][Nn]
key_real                [Rr][Ee][Aa][Ll]
key_char                [Cc][Hh][Aa][Rr]
key_array               [Aa][Rr][Rr][Aa][Yy]
key_proc                [Pp][Rr][Oo][Cc][Ee][Dd][Uu][Rr][Ee]
key_func                [Ff][Uu][Nn][Cc][Tt][Ii][Oo][Nn]
key_begin               [Bb][Ee][Gg][Ii][Nn]
key_end                 [Ee][Nn][Dd]
key_of                  [Oo][Ff]
key_if                  [Ii][Ff]
key_then                [Tt][Hh][Ee][Nn]
key_else                [Ee][Ll][Ss][Ee]
key_for                 [Ff][Oo][Rr]
key_to                  [Tt][Oo]
key_do                  [Dd][Oo]
key_read                [Rr][Ee][Aa][Dd]
key_write               [Ww][Rr][Ii][Tt][Ee]

/* 运算符 */
op_assign               :=
op_l_parentheses        \(
op_r_parentheses        \)
op_l_square             \[
op_r_square             \]
op_add                  \+
op_sub                  \-
op_mul                  \*
op_div                  \/
op_mod                  [Mm][Oo][Dd]
op_div_no_rem           [Dd][Ii][Vv]
op_and                  [Aa][Nn][Dd]
op_or                   [Oo][Rr]
op_not                  [Nn][Oo][Tt]
op_period               \.\.

/* 运算符 relop */
op_gt                   >
op_lt                   <
op_eq                   =
op_neq                  <>
op_gte                  >=
op_lte                  <=

/* 其他字符 */
semicolon               ;
colon                   :
dot                     \.
comma                   ,

/* 低优先级符号 */
bool_true               [Tt][Rr][Uu][Ee]
bool_false              [Ff][Aa][Ll][Ss][Ee]
identifier              [A-Za-z][A-Za-z0-9]*
integer                 [0-9]+
real                    [0-9]+.[0-9]+
illegal_letter          .

%%

%{
  // C++ 兼容的词法分析器的规则，step函数把位置的起始值设置为与结束值相等，这样位置就指向了上一个极少的结束位置。
  loc.step();
%}

{wrong_identifier} {
  loc.step(); 
}

{quotation_not_match} {
  loc.step();
}

{whitespace}|{comment} {
  // 跳过注释和空白符号
  // step函数把位置的起始值设置为与结束值相等，这样位置就指向了上一个极少的结束位置。
  // 由于注释和空白符号识别后并不会返回，而前一个step的调用是在上一次yylex返回时，所以此处需要手动更新记号的起始位置
  // TODO 添加调试标记，如 file_name:l12:c5
  loc.step();
}

{newline} {
  loc.lines(yyleng); // 使用lines函数来更新位置信息中的行号
  loc.step();
}

{key_program} {
  logger.debug("key_program");
  return Parser::make_PROGRAM(loc);
}

{key_const} {
  logger.debug("key_const");
  return Parser::make_CONST(loc);
}

{key_var} {
  logger.debug("key_var");
  return Parser::make_VAR(loc);   
}

{key_int} {
  logger.debug("key_integer");
  return Parser::make_INTEGER(loc);       
}

{key_bool} {
  logger.debug("key_bool");
  return Parser::make_BOOLEAN(loc);     
}

{key_real} {
  logger.debug("key_real");
  return Parser::make_REAL(loc);        
}

{key_char} {
  logger.debug("key_char");
  return Parser::make_CHAR(loc);        
}

{key_array} {
  logger.debug("key_array");
  return Parser::make_ARRAY(loc);       
}

{key_proc} {
  logger.debug("key_procedure");
  return Parser::make_PROCEDURE(loc);       
}

{key_func} {
  logger.debug("key_function");
  return Parser::make_FUNC(loc);       
}

{key_begin} {
  logger.debug("key_begin");
  return Parser::make_BEGIN(loc);       
}

{key_end} {
  logger.debug("key_end");
  return Parser::make_END(loc);       
}

{key_of} {
  logger.debug("key_of");
  return Parser::make_OF(loc);    
}

{key_if} {
  logger.debug("key_if");
  return Parser::make_IF(loc);    
}

{key_then} {
  logger.debug("key_then");
  return Parser::make_THEN(loc);    
}

{key_else} {
  logger.debug("key_else");
  return Parser::make_ELSE(loc);    
}

{key_for} {
  logger.debug("key_for");
  return Parser::make_FOR(loc);    
}

{key_to} {
  logger.debug("key_to");
  return Parser::make_TO(loc);    
}

{key_do} {
  logger.debug("key_do");
  return Parser::make_DO(loc);    
}

{key_read} {
  logger.debug("key_read");
  return Parser::make_READ(loc);    
}

{key_write} {
  logger.debug("key_write");
  return Parser::make_WRITE(loc);        
}

{op_assign} {
  logger.debug("op_assign");
  return Parser::make_ASSIGN(loc);        
}

{op_l_parentheses} {
  logger.debug("op_l_parentheses");
  return Parser::make_LBRACKET(loc);        
}

{op_r_parentheses} {
  logger.debug("op_r_parentheses");
  return Parser::make_RBRACKET(loc);       
}

{op_l_square} {
  logger.debug("op_l_square");
  return Parser::make_LSQUARE_BRACKET(loc); 
}

{op_r_square} {
  logger.debug("op_r_square");
  return Parser::make_RSQUARE_BRACKET(loc);
}

{op_add} {
  logger.debug("op_add");
  return Parser::make_ADD(loc);
}

{op_sub} {
  logger.debug("op_sub");
  return Parser::make_MINUS(loc);
}

{op_mul} {
  logger.debug("op_mul");
  return Parser::make_STAR(loc);
}

{op_div} {
  logger.debug("op_div");
  return Parser::make_SLASH(loc);
}

{op_mod} {
  logger.debug("op_mod");
  return Parser::make_MOD(loc);
}

{op_div_no_rem} {
  logger.debug("op_div_no_rem");
  return Parser::make_DIV(loc);
}

{op_and} {
  logger.debug("op_and");
  return Parser::make_AND(loc);
}

{op_or} {
  logger.debug("op_or");
  return Parser::make_OR(loc);
}

{op_not} {
  logger.debug("op_not");
  return Parser::make_NOT(loc);
}

{op_period} {
  logger.debug("op_period");
  return Parser::make_ARRAY_RANGE_SEPARATOR(loc);
}

{op_gte} {
  logger.debug("op_gte");
  return Parser::make_GREATER_EQUEL(loc);
}

{op_lte} {
  logger.debug("op_lte");
  return Parser::make_LESS_EQUEL(loc);
}

{op_neq} {
  logger.debug("op_neq");
  return Parser::make_NOT_EQUEL(loc);
}

{op_gt} {
  logger.debug("op_gt");
  return Parser::make_GREATER_THAN(loc);
}

{op_lt} {
  logger.debug("op_lt");
  return Parser::make_LESS_THAN(loc);
}

{op_eq} {
  logger.debug("op_eq");
  return Parser::make_EQUEL(loc);
}

{semicolon} {
  logger.debug("semicolon");
  return Parser::make_SEMICOLON(loc);
}

{colon} {
  logger.debug("colon");
  return Parser::make_COLON(loc);
}

{dot} {
  logger.debug("dot");
  return Parser::make_DOT(loc);
}

{comma} {
  logger.debug("comma");
  return Parser::make_COMMA(loc);
}

{bool_true} {
  bool value = true;
  logger.debug("const_bool:{}",value);
  return Parser::make_CONST_BOOL(value, loc);
}

{bool_false} {
  bool value = false;
  logger.debug("const_bool:{}",value);
  return Parser::make_CONST_BOOL(value, loc);
}

{identifier} {
  logger.debug("identifier: {}", yytext);
  return Parser::make_IDENTIFIER(string(yytext), loc);
}

{integer} {
  uint64_t value = atoll(yytext);
  logger.debug("const_int: {}", value);
  return Parser::make_CONST_INT(value, loc);
}

{real} {
  float value = atof(yytext);
  logger.debug("const_float:{}",value);
  return Parser::make_CONST_REAL(value, loc);
}

{charac} {
  char value = yytext[1];
  logger.debug("const_char:{}",value);
  return Parser::make_CONST_CHAR(value, loc);
}

<<EOF>>   { return yyterminate(); }

{illegal_letter} {
  logger.error("Illegal character {}", *yytext);
  loc.step();
}

%%
//这里可以放一些C或者C++代码
//