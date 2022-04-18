#pragma once

#include <vector>
#include <tuple>

namespace p2c {
enum class Operator {
  ADD,
  MINUS,
  OR,
  STAR,
  SLASH,
  DIV,
  MOD,
  AND,
  GREATER_THAN,
  LESS_THAN,
  GREATER_EQUAL,
  LESS_EQUAL,
  EQUAL,
  NOT_EQUAL
  // ...
};

enum class BasicType {
  INTEGER,
  REAL,
  BOOLEAN,
  CHAR
};

struct ArrayType {
  BasicType basictype;
  std::vector<std::tuple<int, int>> dimensions;
};

}