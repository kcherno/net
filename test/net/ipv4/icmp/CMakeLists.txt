set(source_files header.cpp socket.cpp)

foreach(source_file ${source_files})

    add_boost_unit_test(${source_file})

endforeach()

target_sources(header
    PRIVATE ${CMAKE_SOURCE_DIR}/src/net/ipv4/icmp/header.cpp
    PRIVATE ${CMAKE_SOURCE_DIR}/src/net/ipv4/calculate_checksum.cpp)
