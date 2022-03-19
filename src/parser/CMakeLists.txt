# ======== Bison =======
find_package(FLEX REQUIRED)
find_package(BISON REQUIRED)
BISON_TARGET(BisonParser parser.y ${CMAKE_CURRENT_BINARY_DIR}/parser.cpp
             COMPILE_FLAGS "-Wcounterexamples -Wconflicts-sr")
FLEX_TARGET(FlexScanner lexer.l ${CMAKE_CURRENT_BINARY_DIR}/scanner.cpp)
ADD_FLEX_BISON_DEPENDENCY(FlexScanner BisonParser)

add_library(parser ${BISON_BisonParser_OUTPUTS} ${FLEX_FlexScanner_OUTPUTS} ASTNode.cpp driver.cpp)
target_include_directories(parser PUBLIC ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_SOURCE_DIR}/include .)
target_link_libraries(parser PUBLIC spdlog::spdlog)
