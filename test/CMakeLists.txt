find_package(Boost REQUIRED COMPONENTS unit_test_framework)

set(boost_unit_test_options --log_level=all)

function(add_boost_unit_test source_file)

    get_filename_component(filename ${source_file} NAME_WLE)

    add_executable(${filename} ${source_file})

    target_include_directories(${filename} PRIVATE ${CMAKE_SOURCE_DIR}/include)

    target_link_libraries(${filename} PRIVATE Boost::unit_test_framework)

    add_test(NAME ${filename} COMMAND ${filename} ${boost_unit_test_options})

endfunction()

add_subdirectory(net)
