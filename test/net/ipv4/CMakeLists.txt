set(source_files calculate_checksum.cpp endpoint.cpp ip_header.cpp)

foreach(source_file ${source_files})

    add_boost_unit_test(${source_file})

endforeach()

target_sources(calculate_checksum
    PRIVATE ${PROJECT_SOURCE_DIR}/src/net/ipv4/calculate_checksum.cpp)

target_sources(ip_header
    PRIVATE ${PROJECT_SOURCE_DIR}/src/net/ipv4/header.cpp

    PRIVATE
        FILE_SET  HEADERS
        BASE_DIRS ${PROJECT_SOURCE_DIR}/include
)

add_subdirectory(icmp)
