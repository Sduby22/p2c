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
  #define YY_USER_ACTION  loc.columns (yyleng); /* 定义了YY_USER_ACTION，该宏在每个记号的语义动作之前被调用，来根据记号的长度设置位置的信息 */

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
string  \"[^\n"]+\"
/**/
ws      [ \t]+
id   [A-Za-z]
real \d+\.\d+
int  \d+
%%
%{
  // C++ 兼容的词法分析器的规则，step函数把位置的起始值设置为与结束值相等，这样位置就指向了上一个极少的结束位置。
  loc.step();
%}
"//".* |
[ \t]     {
            /* 跳过注释和空白符号 */
            // step函数把位置的起始值设置为与结束值相等，这样位置就指向了上一个极少的结束位置。
            // 由于注释和空白符号识别后并不会返回，而前一个step的调用是在上一次yylex返回时，所以此处需要手动更新记号的起始位置
            loc.step();
          }

\n        {
            loc.lines(yyleng); // 使用lines函数来更新位置信息中的行号
            loc.step();
          }
<<EOF>>   { return yyterminate(); }
.         {
             printf("Mystery character %c\n", *yytext);
             loc.step();
          }
%%
//这里可以放一些C或者C++代码
