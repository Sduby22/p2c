#include "symtable.h"
#include "logging.h"
#include "magic_enum.hpp"
#include <stack>
#include <stdexcept>

auto logger = logging::getLogger("Symbols");

namespace p2c {

std::vector<SymbolTable> symbol_tables;

struct InitSymbolTable {
  InitSymbolTable() {
    symbol_tables.push_back(SymbolTable("global"));
  }
};

SymbolTable& current_table() {
  return symbol_tables.back();
}

void push_table(const std::string &name) {
  symbol_tables.push_back(SymbolTable(name));
}

void pop_table() {
  symbol_tables.pop_back();
}

SymbolTable::SymbolTable(const std::string &name): name(name) {
  // static InitSymbolTable init_symbol_table;
}

void SymbolTable::add(std::string name, p2c::BasicType type, bool is_ref) {
  if (contains(name)) 
    logger.warn("Symbol {} already exists in table {}, possible symbol redefinition.", name, this->name);
  _symbols[name] = Symbol{name, type, is_ref};
}

bool SymbolTable::contains(std::string name) {
  return _symbols.find(name) != _symbols.end();
}

Symbol& SymbolTable::get(std::string name) {
  return _symbols.at(name);
}

void SymbolTable::print() {
  std::string ss;
  ss += fmt::format("Symbol table {}:\n", this->name);
  for (auto &symbol : _symbols) {
    ss += fmt::format("  {}: {}, isref: {}\n", symbol.first, magic_enum::enum_name(symbol.second.type), symbol.second.is_ref);
  }
  logger.info(ss);
}

Symbol& find_symbol(std::string name) {
  for (auto it = symbol_tables.rbegin(); it != symbol_tables.rend(); ++it) {
    if (it->contains(name)) {
      return it->get(name);
    }
  }
  throw std::runtime_error("Symbol " + name + " not found.");
}

FunctionTable& function_table() {
  static FunctionTable ft;
  return ft;
}

Function& find_function(std::string name) {
  return function_table().get(name);
}


void FunctionTable::add(std::string name, p2c::BasicType return_type, std::vector<Symbol> params) {
  if (contains(name)) 
    logger.warn("Function {} already exists in table, possible function redefinition.", name);
  _functions[name] = Function{name, return_type, params};
}

bool FunctionTable::contains(std::string name) {
  return _functions.find(name) != _functions.end();
}

Function& FunctionTable::get(std::string name) {
  return _functions.at(name);
}

void FunctionTable::print() {
  static auto paramString = [](const Function &func) {
    std::string ss;
    for (auto &param : func.params) {
      ss += fmt::format(param.is_ref ? "{} &{}" : "{} {}" , magic_enum::enum_name(param.type), param.name);
      ss.push_back(',');
    }
    if (!ss.empty())
      ss.pop_back();
    return ss;
  };

  std::string ss;
  ss += fmt::format("Function table:\n");
  for (auto &function : _functions) {
    ss += fmt::format("  {}({}) -> {}\n", function.first, paramString(function.second), 
                                                 magic_enum::enum_name(function.second.return_type));
  }
  logger.info(ss);
}

}