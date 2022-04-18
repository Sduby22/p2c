#include "catch2/catch.hpp"
#include <string>
#include <sstream>
#include <fstream>
#include "../src/parser/driver.h"
#include <math.h>
using namespace std;
static std::string path = "../tests/parser_cases/";
int test_parse(std::string filename)
{
  p2c::Driver driver;
  std::ifstream input(path + filename + ".p");
  if(!input)
  {
    input.close();
    return 1;
  }
  int ret =  driver.parse(input, cout);
  input.close();
  return ret;
}
//注释
TEST_CASE("[parser] comment")
{
  int result1 = !test_parse("comment_success1");
  int result2 = !test_parse("comment_success2");
  int result3 = !test_parse("comment_failed1");
  int result4 = !test_parse("comment_failed2");
  REQUIRE(result1);
  REQUIRE(result2);
  REQUIRE(!result3);
  REQUIRE(!result4);
}
// 变量定义,常量定义需要在变量定义之前，变量定义需要在begin之前
TEST_CASE("[parser] variable")
{
  int result1 = !test_parse("variable_success1");
  int result2 = !test_parse("variable_failed1");
  int result3 = !test_parse("variable_failed2");
  int result4 = !test_parse("variable_failed3");
  int result5 = !test_parse("variable_failed4");
  REQUIRE(result1);
  REQUIRE(!result2);
  REQUIRE(!result3);
  REQUIRE(!result4);
  REQUIRE(!result5);
}

// programma
TEST_CASE("[parser] programma")
{
  int result1 = !test_parse("programma_success1");
  int result2 = !test_parse("programma_success2");
  int result3 = !test_parse("programma_failed1");
  int result4 = !test_parse("programma_failed2");
  int result5 = !test_parse("programma_failed3");
  int result6 = !test_parse("programma_failed4");
  int result7 = !test_parse("programma_failed5");
  int result8 = !test_parse("programma_failed6");
  REQUIRE(result1);
  // 无参数
  // REQUIRE(result2);
  REQUIRE(!result3);
  REQUIRE(!result4);
  REQUIRE(!result5);
  REQUIRE(!result6);
  REQUIRE(!result7);
  REQUIRE(!result8);
}

//procedure
TEST_CASE("[parser] procedure")
{
  int result1 = !test_parse("procedure_success1");
  int result2 = !test_parse("procedure_success2");
  int result3 = !test_parse("procedure_failed1");
  int result4 = !test_parse("procedure_failed2");
  int result5 = !test_parse("procedure_failed3");
  int result6 = !test_parse("procedure_failed4");
  REQUIRE(result1);
  // REQUIRE(result2);
  REQUIRE(!result3);
  REQUIRE(!result4);
  REQUIRE(!result5);
  REQUIRE(!result6);
}

//function
TEST_CASE("[parser] function")
{
  int result1 = !test_parse("function_success1");
  int result2 = !test_parse("function_success2");
  int result3 = !test_parse("function_failed1");
  int result4 = !test_parse("function_failed2");
  int result5 = !test_parse("function_failed3");
  int result6 = !test_parse("function_failed4");
  REQUIRE(result1);
  // 无参数
  // REQUIRE(result2);
  REQUIRE(!result3);
  REQUIRE(!result4);
  REQUIRE(!result5);
  REQUIRE(!result6);
}
//参数表,以procedure为例
TEST_CASE("[parser] parameter list ")
{
  int result1 = !test_parse("parameter_success1");
  int result2 = !test_parse("parameter_success2");
  int result3 = !test_parse("parameter_failed1");
  int result4 = !test_parse("parameter_failed2");
  int result5 = !test_parse("parameter_failed3");
  int result6 = !test_parse("parameter_failed4");
  REQUIRE(result1);
  // REQUIRE(result2);
  REQUIRE(!result3);
  REQUIRE(!result4);
  REQUIRE(!result5);
  REQUIRE(!result6);
}
//if...else语句
TEST_CASE("[parser] if")
{
  int result1 = !test_parse("if_success1");
  int result2 = !test_parse("if_success2");
  int result3 = !test_parse("if_failed1");
  int result4 = !test_parse("if_failed2");
  int result5 = !test_parse("if_failed3");
  int result6 = !test_parse("if_failed4");
  int result7 = !test_parse("if_failed5");
  int result8 = !test_parse("if_failed6");

  REQUIRE(result1);
  REQUIRE(result2);
  REQUIRE(!result3);
  REQUIRE(!result4);
  REQUIRE(!result5);
  REQUIRE(!result6);
  REQUIRE(!result7);
  REQUIRE(!result8);
}

//for...to...do语句
TEST_CASE("[parser] for")
{
  int result1 = !test_parse("for_success1");
  int result2 = !test_parse("for_success2");
  int result3 = !test_parse("for_success3");
  int result4 = !test_parse("for_failed1");
  int result5 = !test_parse("for_failed2");
  int result6 = !test_parse("for_failed3");
  REQUIRE(result1);
  REQUIRE(result2);
  REQUIRE(result3);
  REQUIRE(!result4);
  REQUIRE(!result5);
  REQUIRE(!result6); 
}
//read&write语句
TEST_CASE("[parser] io")
{
  int result1 = !test_parse("io_success1");
  int result2 = !test_parse("io_success2");
  int result3 = !test_parse("io_failed1");
  int result4 = !test_parse("io_failed2");
  int result5 = !test_parse("io_failed3");
  int result6 = !test_parse("io_failed4");
  int result7 = !test_parse("io_failed5");
  //写常量
  // REQUIRE(result1);
  // REQUIRE(result2);
  REQUIRE(!result3);
  REQUIRE(!result4);
  REQUIRE(!result5);
  REQUIRE(!result6); 
  REQUIRE(!result7); 
}
//array语句
TEST_CASE("[parser] array")
{
  int result1 = !test_parse("array_success1");
  int result2 = !test_parse("array_failed1");
  int result3 = !test_parse("array_failed2");
  int result4 = !test_parse("array_failed3");
  int result5 = !test_parse("array_failed4");
  // array未完成
  // REQUIRE(result1);
  // REQUIRE(!result2);
  // REQUIRE(!result3);
  // REQUIRE(!result4);
  // REQUIRE(!result5); 
}