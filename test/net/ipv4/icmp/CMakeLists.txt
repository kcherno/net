set(source_files icmp_header.cpp socket.cpp)

foreach(source_file ${source_files})

    add_boost_unit_test(${source_file})

endforeach()

target_sources(header
    PRIVATE ${PROJECT_SOURCE_DIR}/src/net/ipv4/calculate_checksum.cpp
    PRIVATE ${PROJECT_SOURCE_DIR}/src/net/ipv4/icmp/header.cpp

    PRIVATE
        FILE_SET  HEADERS
        BASE_DIRS ${PROJECT_SOURCE_DIR}/include
)
