cmake_minimum_required(VERSION 3.22)

project(p2c)
set(CMAKE_CXX_STANDARD 17)

aux_source_directory(src SRC)
add_subdirectory(lib/spdlog-1.9.2)

add_executable(main ${SRC})
target_include_directories(main PUBLIC include)
target_link_libraries(main PUBLIC spdlog::spdlog)
