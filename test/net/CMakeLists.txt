add_net_unit_test(ipv4.cpp)

target_sources(ipv4 PRIVATE ${PROJECT_SOURCE_DIR}/src/net/ipv4.cpp)

add_subdirectory(name_requirement)
