%{
  #include <iostream>
  #include <cerrno>
  #include <climits>
  #include <cstdlib>
  #include <string>

  #include "parser.hpp"  //包含由parser.y生成的头文件
  #include "scanner.h"   //包含yyFlexLexer子类的头文件
  #include "location.hh" //包含位置调试信息头文件

  static p2c::location loc;//声明位置实例
  /* 定义了YY_USER_ACTION，该宏在每个记号的语义动作之前被调用，来根据记号的长度设置位置的信息 */
  #define YY_USER_ACTION  loc.columns (yyleng); 

  #undef yywrap
  #define yywrap() 1

  using namespace p2c;
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
comment                 \/\/.*?
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
l_braces                \{
r_braces                \}

/* 低优先级符号 */
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
    
}

{quotation_not_match} {
    
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
    
}

{key_const} {
    
}

{key_var} {
    
}

{key_int} {
    
}

{key_bool} {
    
}

{key_real} {
    
}

{key_char} {
    
}

{key_array} {
    
}

{key_proc} {
    
}

{key_func} {
    
}

{key_begin} {
    
}

{key_end} {
    
}

{key_of} {
    
}

{key_if} {
    
}

{key_then} {
    
}

{key_else} {
    
}

{key_for} {
    
}

{key_to} {
    
}

{key_do} {
    
}

{key_read} {
    
}

{key_write} {
    
}

{op_assign} {
    
}

{op_l_parentheses} {
    
}

{op_r_parentheses} {
    
}

{op_l_square} {
    
}

{op_r_square} {
    
}

{op_add} {
    
}

{op_sub} {
    
}

{op_mul} {
    
}

{op_div} {
    
}

{op_mod} {
    
}

{op_div_no_rem} {
    
}

{op_and} {
    
}

{op_or} {
    
}

{op_not} {
    
}

{op_period} {
    
}

{op_gt} {
    
}

{op_lt} {
    
}

{op_eq} {
    
}

{op_neq} {
    
}

{op_gte} {
    
}

{op_lte} {
    
}

{semicolon} {
    
}

{colon} {
    
}

{dot} {
    
}

{comma} {
    
}

{l_braces} {
    
}

{r_braces} {
    
}

{identifier} {
    
}

{integer} {
    
}

{real} {
    
}

<<EOF>>   { return yyterminate(); }

{illegal_letter} {
  printf("Mystery character %c\n", *yytext);
  loc.step();  
}

%%
//这里可以放一些C或者C++代码
