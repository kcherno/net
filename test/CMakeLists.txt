find_package(Boost REQUIRED COMPONENTS unit_test_framework)

set(boost_unit_test_options --log_level=all)

function(add_boost_unit_test source_file)

    get_filename_component(filename ${source_file} NAME_WLE)

    add_executable(${filename})

    add_test(NAME ${filename} COMMAND ${filename} ${boost_unit_test_options})

    if (enable_net_debug_mode)

        target_compile_definitions(${filename} PRIVATE NET_DEBUG_MODE__)

    endif()

    target_compile_features(${filename} PRIVATE cxx_std_23)

    target_link_libraries(${filename} PRIVATE Boost::unit_test_framework)

    target_sources(${filename}
        PRIVATE ${source_file}
        
        PRIVATE
            FILE_SET  HEADERS
            BASE_DIRS ${PROJECT_SOURCE_DIR}/include)

endfunction()

add_subdirectory(net)
