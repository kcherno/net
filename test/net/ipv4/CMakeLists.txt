set(source_files endpoint.cpp)

foreach(source_file ${source_files})

    add_boost_unit_test(${source_file})

endforeach()
