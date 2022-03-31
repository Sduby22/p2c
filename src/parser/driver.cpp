#include "driver.h"
#include <iostream>

using namespace p2c;
using namespace std;

Driver::Driver() : _scanner(), _parser(_scanner) {}

Driver::~Driver() {}

int Driver::parse(std::istream &is, std::ostream &os) {
  _scanner.switch_streams(is, os);
  return _parser.parse();
}
