#include "symtable.h"
#include "logging.h"
#include "magic_enum.hpp"
#include <stack>

auto logger = logging::getLogger("Symbols");

namespace p2c {

std::stack<SymbolTable> symbol_tables;

SymbolTable& current_table() {
  if (symbol_tables.empty()) {
    symbol_tables.push(SymbolTable("global"));
  }
  return symbol_tables.top();
}

void push_table(const std::string &name) {
  symbol_tables.push(SymbolTable(name));
}

void pop_table() {
  symbol_tables.pop();
}

SymbolTable::SymbolTable(const std::string &name): name(name) {}

void SymbolTable::add(std::string name, p2c::BasicType type, bool is_ref) {
  if (contains(name)) 
    logger.warn("Symbol {} already exists in table {}, possible symbol redefinition.", name, this->name);
  _symbols[name] = Symbol{name, type, is_ref};
}

bool SymbolTable::contains(std::string name) {
  return _symbols.find(name) != _symbols.end();
}

Symbol SymbolTable::get(std::string name) {
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

}