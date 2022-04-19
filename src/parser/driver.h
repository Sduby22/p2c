#ifndef DRIVER_H
#define DRIVER_H

#include "parser.hpp"
#include "scanner.h"
#include <iostream>

extern unique_ptr<p2c::ASTNode> rootNode;

namespace p2c {
class Driver {
private:
  Parser _parser;
  Scanner _scanner;

public:
  Driver();

  int parse(std::istream &is = std::cin, std::ostream &os = std::cout);
  std::string getCCodeStr();
  std::string getSyntaxTreeStr();
  virtual ~Driver();
};
} // namespace p2c

#endif
