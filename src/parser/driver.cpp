#include "driver.h"
#include "symtable.h"
#include <iostream>

using namespace p2c;
using namespace std;

Driver::Driver() : _scanner(), _parser(_scanner, rootNode) {}

Driver::~Driver() {}

int Driver::parse(std::istream &is, std::ostream &os) {
  _scanner.switch_streams(is, os);
  int res = _parser.parse();
  return res;
}

std::string Driver::getCCodeStr() {
  init_tables();
  return rootNode->genCCode();
}

std::string Driver::getSyntaxTreeStr() {
  return rootNode->printNode();
}