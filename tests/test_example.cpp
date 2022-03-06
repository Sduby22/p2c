#include "catch2/catch.hpp"
#include <cmath>
#include <string>

TEST_CASE("Negative can't be sqrted.") {
  auto fail = std::sqrt(-1);
  REQUIRE(std::isnan(fail));
}

TEST_CASE("Should return helloworld.") {
  REQUIRE([]() -> std::string { return "helloworld"; }() == "helloworld");
}
