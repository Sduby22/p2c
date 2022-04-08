#include "symtable.h"
#include "logging.h"
#include "magic_enum.hpp"
#include <stack>
#include <stdexcept>

auto logger = logging::getLogger("Symbols");

namespace p2c {

std::vector<SymbolTable> symbol_tables = {SymbolTable("global")};

SymbolTable& current_table() {
  return symbol_tables.back();
}

void push_table(const std::string &name) {
  symbol_tables.push_back(SymbolTable(name));
}

void pop_table() {
  symbol_tables.pop_back();
}

SymbolTable::SymbolTable(const std::string &name): name(name) {}

void SymbolTable::add(std::string name, p2c::BasicType type, bool is_ref) {
  if (contains(name)) 
    logger.warn("Symbol {} already exists in table {}, possible symbol redefinition.", name, this->name);
  _symbols[name] = Symbol{name, type, is_ref};
}

void SymbolTable::add(std::string name, p2c::BasicType type, std::vector<std::tuple<int, int>> array_dimensions) {
  if (contains(name)) 
    logger.warn("Symbol {} already exists in table {}, possible symbol redefinition.", name, this->name);
  _symbols[name] = Symbol{name, type, false, array_dimensions};
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
    ss += fmt::format("  {}: {}, isref: {}", symbol.first, magic_enum::enum_name(symbol.second.type), symbol.second.is_ref);
    if (symbol.second.array_dimensions) {
      ss += fmt::format(", array_dimensions: ");
      for (auto &dim : *symbol.second.array_dimensions) {
        ss += fmt::format("[{}, {}]", std::get<0>(dim), std::get<1>(dim));
      }
    }
    ss += "\n";
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