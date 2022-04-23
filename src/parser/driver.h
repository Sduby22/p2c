#ifndef DRIVER_H
#define DRIVER_H

#include "parser.hpp"
#include "scanner.h"
#include <iostream>

namespace p2c {
class Driver {
private:
  Parser _parser;
  Scanner _scanner;
  unique_ptr<p2c::ASTNode> rootNode;

public:
  Driver();

  int parse(std::istream &is = std::cin, std::ostream &os = std::cout);
  std::string getCCodeStr();
  std::string getSyntaxTreeStr();
  virtual ~Driver();
};
} // namespace p2c

#endif
