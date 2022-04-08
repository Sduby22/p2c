#pragma once

#include "types.h"
#include <stack>
#include <string>
#include <unordered_map>

namespace p2c {

struct Symbol {
  std::string name;
  p2c::BasicType type;
  bool is_ref = false;
};

class SymbolTable {
public:
  SymbolTable(const std::string &name);
  void add(std::string name, p2c::BasicType type, bool is_ref = false);
  bool contains(std::string name);
  Symbol get(std::string name);
  void print();
  std::string name;

private:
  std::unordered_map<std::string, Symbol> _symbols;
};

SymbolTable& current_table();
void push_table(const std::string &name);
void pop_table();

}