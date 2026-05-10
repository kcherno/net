cmake_minimum_required(VERSION 3.10)

project(net LANGUAGES CXX)

set(CMAKE_CXX_STANDARD          23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_compile_options("$<$<CXX_COMPILER_ID:GNU>:--debug;-Wall;-Werror;-Wextra>")

option(enable_unit_tests "enable unit tests" ON)

if (${enable_unit_tests})

    enable_testing()

    add_subdirectory(test)

endif()
