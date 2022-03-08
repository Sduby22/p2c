#include "driver.h"
#include <iostream>

using namespace p2c;
using namespace std;

Driver::Driver() : _scanner(*this), _parser(_scanner, *this) {}

Driver::~Driver() {}

int Driver::parse() {
  _scanner.switch_streams(cin, cout);
  return _parser.parse();
}
