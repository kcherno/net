set(source_files calculate_checksum.cpp endpoint.cpp)

foreach(source_file ${source_files})

    add_boost_unit_test(${source_file})

endforeach()

target_sources(calculate_checksum
    PRIVATE ${PROJECT_SOURCE_DIR}/src/net/ipv4/calculate_checksum.cpp)

add_subdirectory(icmp)
