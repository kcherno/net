find_package(Boost REQUIRED COMPONENTS unit_test_framework)

cmake_language(EVAL CODE "
    set(boost_test_\${PROJECT_NAME}_options --log_level=all)

    function(make_\${PROJECT_NAME}_unit_test source_file)

        get_filename_component(test_name \${source_file} NAME_WLE)

        add_executable(\${test_name})

        add_test(
            NAME    \${test_name}
            COMMAND \${test_name} \${boost_test_\${PROJECT_NAME}_options}
        )

        if (enable_\${PROJECT_NAME}_debug_mode)

            target_compile_definitions(\${test_name} PRIVATE NET_DEBUG_MODE__)

        endif()

        target_compile_features(\${test_name} PRIVATE cxx_std_23)

        target_link_libraries(\${test_name} PRIVATE Boost::unit_test_framework)

        target_sources(\${test_name}
            PRIVATE \${source_file}

            PRIVATE
                FILE_SET  HEADERS
                BASE_DIRS \${PROJECT_SOURCE_DIR}/include
        )

    endfunction()"
)

add_subdirectory(net)
