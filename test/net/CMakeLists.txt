set(source_files
    # to_host_byte_order.cpp
    # to_network_byte_order.cpp
)

foreach(source_file ${source_files})

    add_boost_unit_test(${source_file})

endforeach()

add_subdirectory(ipv4)
