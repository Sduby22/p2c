cmake_minimum_required(VERSION 3.10)

project(p2c)
set(CMAKE_CXX_STANDARD 17)

# aux_source_directory(src SRC)
add_subdirectory(lib/spdlog-1.9.2)
add_subdirectory(lib/Catch2-2.13.8)
add_subdirectory(lib/cxxopts-3.0.0)

add_subdirectory(src/parser)

# ======== Main =========
add_executable(main src/main.cpp)
target_include_directories(main PUBLIC include)
target_link_libraries(main PUBLIC spdlog::spdlog cxxopts::cxxopts parser)

# ======== CTest =========
option(BUILD_TESTING OFF)
if (BUILD_TESTING)
  list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/lib/Catch2-2.13.8/contrib)
  include(CTest)
  include(Catch)

  add_executable(test_main tests/test_main.cpp tests/test_example.cpp tests/test_lexer.cpp)
  target_link_libraries(test_main Catch2::Catch2 parser)

  catch_discover_tests(test_main)
endif()
# add_test(NAME Catch2.Test
#   COMMAND test_main)
