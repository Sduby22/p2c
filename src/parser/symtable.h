#pragma once

#include "types.h"
#include <stack>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>

namespace p2c {

struct Symbol {
  std::string name;
  std::variant<BasicType, ArrayType> type;
  bool is_ref = false;
};

class SymbolTable {
public:
  SymbolTable(const std::string &name);
  void add(std::string name, p2c::BasicType type, bool is_ref = false);
  void add(std::string name, p2c::ArrayType type, bool is_ref = false);
  bool contains(std::string name);
  Symbol& get(std::string name);
  void print();
  std::string name;

private:
  std::unordered_map<std::string, Symbol> _symbols;
};

struct Function {
  std::string name;
  p2c::BasicType return_type;
  std::vector<Symbol> params;
};

class FunctionTable {
public:
  FunctionTable() = default;
  void add(std::string name, p2c::BasicType return_type, std::vector<Symbol> params);
  bool contains(std::string name);
  Function& get(std::string name);
  void print();
  void clear();

private:
  std::unordered_map<std::string, Function> _functions;
};

FunctionTable& function_table();
Function& find_function(std::string name);

SymbolTable& current_table();
Symbol& find_symbol(std::string name);
void push_table(const std::string &name);
void pop_table();
void initTables();

}