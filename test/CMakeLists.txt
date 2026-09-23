find_package(Boost REQUIRED COMPONENTS unit_test_framework)

function(add_net_unit_test test_file)

    set(boost_test_options --log_level=all)

    get_filename_component(test_name ${test_file} NAME_WLE)

    add_executable(${test_name})

    if (enable_net_debug_mode)

        target_compile_definitions(${test_name} PRIVATE NET_DEBUG_MODE__)

    endif()

    target_compile_features(${test_name} PRIVATE cxx_std_23)

    target_compile_options(${test_name}
        PRIVATE "$<$<CXX_COMPILER_ID:GNU>:-g;-Wall;-Werror;-Wextra>"
    )

    target_link_libraries(${test_name} PRIVATE Boost::unit_test_framework)

    target_link_libraries(${test_name} PRIVATE net)

    target_sources(${test_name}
        PRIVATE ${test_file}

        PRIVATE
            FILE_SET  HEADERS
            BASE_DIRS ${PROJECT_SOURCE_DIR}/include
    )

    add_test(
        NAME    ${CMAKE_CURRENT_SOURCE_DIR}/${test_name}
        COMMAND ${test_name} ${boost_test_options}
    )

endfunction()

add_subdirectory(net)
