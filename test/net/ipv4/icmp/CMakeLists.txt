add_boost_unit_test(icmp_header.cpp)
add_boost_unit_test(icmp_socket.cpp)

target_sources(icmp_header
    PRIVATE ${PROJECT_SOURCE_DIR}/src/net/ipv4/icmp/header.cpp

    PRIVATE ${PROJECT_SOURCE_DIR}/src/net/ipv4/calculate_checksum.cpp
    PRIVATE ${PROJECT_SOURCE_DIR}/src/net/ipv4/header.cpp

    PRIVATE
        FILE_SET  HEADERS
        BASE_DIRS ${PROJECT_SOURCE_DIR}/include
)
