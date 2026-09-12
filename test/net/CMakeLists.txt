add_net_unit_test(icmp.cpp)

target_sources(icmp
    PRIVATE ${PROJECT_SOURCE_DIR}/src/net/detail/calculate_checksum.cpp

    PRIVATE ${PROJECT_SOURCE_DIR}/src/net/icmp.cpp
    PRIVATE ${PROJECT_SOURCE_DIR}/src/net/ipv4.cpp
)

add_net_unit_test(ipv4.cpp)

target_sources(ipv4 PRIVATE ${PROJECT_SOURCE_DIR}/src/net/ipv4.cpp)

add_net_unit_test(tcp.cpp)

add_net_unit_test(udp.cpp)

add_subdirectory(name_requirement)
