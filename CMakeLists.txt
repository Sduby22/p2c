cmake_minimum_required(VERSION 3.10)

project(p2c)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# aux_source_directory(src SRC)
add_subdirectory(lib/spdlog-1.9.2)
add_subdirectory(lib/Catch2-2.13.8)
add_subdirectory(lib/cxxopts-3.0.0)

if (${EMSCRIPTEN}) 
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fexceptions")
endif()

add_subdirectory(src/parser)

# ======== Main =========
if (${EMSCRIPTEN}) 
  add_executable(libparser src/libparser.cpp)
  target_include_directories(libparser PUBLIC include)
  target_link_libraries(libparser PUBLIC spdlog::spdlog cxxopts::cxxopts parser)
  set_target_properties(libparser PROPERTIES LINK_FLAGS "-lembind -s EXPORT_ES6=1 -s MODULARIZE=1 -s EXPORT_NAME=CreateParserModule -fexceptions")
else ()
  add_executable(main src/main.cpp)
  target_include_directories(main PUBLIC include)
  target_link_libraries(main PUBLIC spdlog::spdlog cxxopts::cxxopts parser)
endif ()

# ======== CTest =========
option(BUILD_TESTING OFF)
if (BUILD_TESTING)
  list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/lib/Catch2-2.13.8/contrib)
  include(CTest)
  include(Catch)

  add_executable(test_main tests/test_main.cpp tests/test_example.cpp tests/test_lexer.cpp tests/test_yacc.cpp)
  target_link_libraries(test_main Catch2::Catch2 parser)

  catch_discover_tests(test_main)
endif()
# add_test(NAME Catch2.Test
#   COMMAND test_main)

# ========= Doxygen ========
option(BUILD_DOC "Build Doxygen" OFF)
find_package(
  Doxygen
  OPTIONAL_COMPONENTS mscgen dia dot
)

if(BUILD_DOC AND DOXYGEN_FOUND)
  add_subdirectory(doc)
else()
  message("Needs doxygen installed to generate documentation")
endif()
