CMAKE_MINIMUM_REQUIRED_VERSION(3.0)

project(p2c)

set(CMAKE_CXX_STANDARD 17)

aux_source_directory(src SRC)

add_executable(main ${SRC})

target_include_directories(main PUBLIC include)
