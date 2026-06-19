cmake_minimum_required(VERSION 3.23)

project(net LANGUAGES CXX VERSION 0.1.0)

option(enable_net_debug_mode "enable debug mode" OFF)
option(enable_net_unit_tests "enable unit tests" OFF)

add_library(${PROJECT_NAME} SHARED)

set_target_properties(${PROJECT_NAME}
    PROPERTIES
        CXX_STANDARD_REQUIRED    ON
        LIBRARY_OUTPUT_DIRECTORY lib
        VERSION                  ${PROJECT_VERSION}
        SOVERSION                ${PROJECT_VERSION_MAJOR}
)

if (enable_net_debug_mode)

    target_compile_definitions(${PROJECT_NAME} PUBLIC NET_DEBUG_MODE__)

endif()

target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_23)

target_compile_options(${PROJECT_NAME}
    PRIVATE "$<$<CXX_COMPILER_ID:GNU>:--debug;-Wall;-Werror;-Wextra>")

target_sources(${PROJECT_NAME}
    PUBLIC ${PROJECT_SOURCE_DIR}/src/net/ipv4/calculate_checksum.cpp
    PUBLIC ${PROJECT_SOURCE_DIR}/src/net/ipv4/icmp/header.cpp
    PUBLIC ${PROJECT_SOURCE_DIR}/src/net/ipv4/header.cpp

    PUBLIC
        FILE_SET  HEADERS
        BASE_DIRS ${PROJECT_SOURCE_DIR}/include
)

if (enable_net_unit_tests)

    enable_testing()

    add_subdirectory(${PROJECT_SOURCE_DIR}/test)

endif()
