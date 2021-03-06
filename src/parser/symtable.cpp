#include "symtable.h"
#include "logging.h"
#include "magic_enum.hpp"
#include "types.h"
#include <stdexcept>
#include <variant>

auto logger = logging::getLogger("Symbols");

namespace p2c {

static std::vector<SymbolTable> symbol_tables;
static FunctionTable ft;

void init_tables() {
  symbol_tables.clear();
  symbol_tables.push_back(SymbolTable("global"));
  ft.clear();
}

SymbolTable& current_symbol_table() {
  return symbol_tables.back();
}

void push_symbol_table(const std::string &name) {
  symbol_tables.push_back(SymbolTable(name));
}

void pop_symbol_table() {
  symbol_tables.pop_back();
}

SymbolTable::SymbolTable(const std::string &name): name(name) {}

void SymbolTable::add(std::string name, BasicType type, bool is_ref) {
  if (contains(name)) 
    logger.warn("Symbol {} already exists in table {}, possible symbol redefinition.", name, this->name);
  _symbols[name] = Symbol{name, type, is_ref};
}

void SymbolTable::add(std::string name, ArrayType type, bool is_ref) {
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

std::string _type_string(const BasicType& type) {
  return std::string(magic_enum::enum_name(type));
}

std::string _type_string(const ArrayType& type) {
  std::string ss;
  ss += _type_string(type.basictype);
  for (auto [start, end] : type.dimensions) {
    ss += fmt::format("[{}, {}]", start, end);
  }
  return ss;
}

std::string type_string(const std::variant<BasicType, ArrayType>& type) {
  return std::visit([](auto&& arg) { return _type_string(arg); }, type);
}

void SymbolTable::print() {
  std::string ss;
  ss += fmt::format("Symbol table {}:\n", this->name);
  for (auto &symbol : _symbols) {
    ss += fmt::format("  {}: {}, isref: {}", symbol.first, type_string(symbol.second.type), symbol.second.is_ref);
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
  auto str = fmt::format("Symbol {} not found in any symbol table.", name);
  throw std::runtime_error(str);
}

FunctionTable& function_table() {
  return ft;
}

Function& find_function(std::string name) {
  return function_table().get(name);
}


void FunctionTable::add(std::string name, BasicType return_type, std::vector<Symbol> params) {
  if (contains(name)) 
    logger.warn("Function {} already exists in table, possible function redefinition.", name);
  _functions[name] = Function{name, return_type, params};
}

bool FunctionTable::contains(std::string name) {
  return _functions.find(name) != _functions.end();
}

Function& FunctionTable::get(std::string name) {
  auto it = _functions.find(name);
  if (it == _functions.end()) {
    auto str = fmt::format("Function {} not found in function table.", name);
    logger.critical(str);
    throw std::runtime_error(str);
  }
  return it->second;
}

void FunctionTable::print() {
  static auto paramString = [](const Function &func) {
    std::string ss;
    for (auto &param : func.params) {
      ss += fmt::format(param.is_ref ? "{} &{}" : "{} {}" , type_string(param.type), param.name);
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

void FunctionTable::clear() {
  this->_functions.clear();
}
}