#include "symtable.h"
#include "logging.h"
#include "magic_enum.hpp"
#include <stack>
#include <stdexcept>

auto logger = logging::getLogger("Symbols");

namespace p2c {

std::vector<SymbolTable> symbol_tables;

SymbolTable& current_table() {
  if (symbol_tables.empty()) {
    symbol_tables.push_back(SymbolTable("global"));
  }
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

}