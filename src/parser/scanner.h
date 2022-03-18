#ifndef SCANNER_H
#define SCANNER_H

/***********************************/
/*对yyFlexLexer进行继承，获得更多的控制
 *
 ***********************************/

/* 重要 */
#if !defined(yyFlexLexerOnce)
#undef yyFlexLexer
#define yyFlexLexer p2c_FlexLexer // 根据prefix修改

#include <FlexLexer.h>

#endif
/* 替换默认的get_next_token定义 */
#undef YY_DECL
#define YY_DECL p2c::Parser::symbol_type p2c::Scanner::nextToken()

#include "parser.hpp"

namespace p2c {
class Driver;

class Scanner : public yyFlexLexer {
private:
  /* data */
  Driver &_driver;

public:
  Scanner(p2c::Driver &driver) : _driver(driver) {}

  virtual p2c::Parser::symbol_type
  nextToken(); // 不需要手动实现这个函数，Flex会生成YY_DECL宏定义的代码来实现这个函数

  virtual ~Scanner() {}
};
} // namespace p2c

#endif
