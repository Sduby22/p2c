#ifndef SCANNER_H
#define SCANNER_H

/***********************************/
/*对yyFlexLexer进行继承，获得更多的控制
 *
 ***********************************/

/* 重要 */
#include "location.hh"
#if !defined(yyFlexLexerOnce)
#undef yyFlexLexer
#define yyFlexLexer p2c_FlexLexer // 根据prefix修改

#include <FlexLexer.h>

#endif
/* 替换默认的get_next_token定义 */
#undef YY_DECL
#define YY_DECL p2c::Parser::symbol_type p2c::Scanner::nextToken()

#include "parser.hpp"

extern p2c::location loc;//声明位置实例

namespace p2c {


class Scanner : public yyFlexLexer {
private:

public:
  Scanner() {
    loc = p2c::location();
  }

  virtual p2c::Parser::symbol_type
  nextToken(); // 不需要手动实现这个函数，Flex会生成YY_DECL宏定义的代码来实现这个函数

  virtual ~Scanner() {}
};
} // namespace p2c

#endif
