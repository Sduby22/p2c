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

public:
  Driver();

  int parse();

  virtual ~Driver();
};
} // namespace p2c

#endif
