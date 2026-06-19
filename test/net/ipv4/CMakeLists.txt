make_net_unit_test(calculate_checksum.cpp)
make_net_unit_test(ip_header.cpp)
make_net_unit_test(endpoint.cpp)

target_sources(calculate_checksum
    PRIVATE ${PROJECT_SOURCE_DIR}/src/net/ipv4/calculate_checksum.cpp)

target_sources(ip_header
    PRIVATE ${PROJECT_SOURCE_DIR}/src/net/ipv4/header.cpp

    PRIVATE
        FILE_SET  HEADERS
        BASE_DIRS ${PROJECT_SOURCE_DIR}/include
)

add_subdirectory(icmp)
