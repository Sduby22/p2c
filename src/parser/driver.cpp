#include "driver.h"
#include <iostream>

using namespace p2c;
using namespace std;

// 语法树根节点（全局变量）
unique_ptr<p2c::ASTNode> rootNode;

Driver::Driver() : _scanner(), _parser(_scanner) {}

Driver::~Driver() {}

int Driver::parse(std::istream &is, std::ostream &os) {
  _scanner.switch_streams(is, os);
  return _parser.parse();
}

std::string Driver::getCCodeStr() {
  return rootNode->genCCode();
}

std::string Driver::getSyntaxTreeStr() {
  return rootNode->printNode();
}