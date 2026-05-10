cmake_minimum_required(VERSION 3.11)

project(net
    VERSION   0.1.0
    LANGUAGES CXX)

set(CMAKE_CXX_STANDARD          23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_compile_options("$<$<CXX_COMPILER_ID:GNU>:--debug;-Wall;-Werror;-Wextra>")

option(enable_unit_tests "enable unit tests" ON)

if (${enable_unit_tests})

    enable_testing()

    add_subdirectory(test)

endif()

add_library(${CMAKE_PROJECT_NAME} SHARED)

target_include_directories(${CMAKE_PROJECT_NAME}
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)

set_target_properties(${CMAKE_PROJECT_NAME} PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY lib
    VERSION                  ${CMAKE_PROJECT_VERSION}
    SOVERSION                ${CMAKE_PROJECT_VERSION_MAJOR})

target_sources(${CMAKE_PROJECT_NAME}
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src/net/ipv4/icmp/header.cpp
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src/net/ipv4/calculate_checksum.cpp)
