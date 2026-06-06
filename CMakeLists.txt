cmake_minimum_required(VERSION 3.23)

project(net
    VERSION   0.1.0
    LANGUAGES CXX)

option(enable_unit_tests "enable unit tests" OFF)

if (${enable_unit_tests})

    enable_testing()

    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/test)

endif()

add_library(${PROJECT_NAME} SHARED)

set_target_properties(${PROJECT_NAME} PROPERTIES
    CXX_STANDARD_REQUIRED    ON
    LIBRARY_OUTPUT_DIRECTORY lib
    VERSION                  ${PROJECT_VERSION}
    SOVERSION                ${PROJECT_VERSION_MAJOR})

target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_23)

target_compile_options(${PROJECT_NAME}
    PRIVATE "$<$<CXX_COMPILER_ID:GNU>:--debug;-Wall;-Werror;-Wextra>")

target_sources(${PROJECT_NAME}
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src/net/ipv4/icmp/header.cpp
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src/net/ipv4/calculate_checksum.cpp

    PUBLIC
        FILE_SET  HEADERS
        BASE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/include)
