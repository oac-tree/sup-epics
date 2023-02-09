set(test unit-tests)

file(GLOB include_files "*.h")

add_executable(${test} ${include_files})

target_sources(${test}
  PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/anyvalue_from_pvxs_builder_tests.cpp
    ${CMAKE_CURRENT_LIST_DIR}/channel_access_base_tests.cpp
    ${CMAKE_CURRENT_LIST_DIR}/channel_access_client_tests.cpp
    ${CMAKE_CURRENT_LIST_DIR}/channel_access_pv_tests.cpp
    ${CMAKE_CURRENT_LIST_DIR}/dto_conversion_utils_tests.cpp
    ${CMAKE_CURRENT_LIST_DIR}/dto_scalar_conversion_utils_tests.cpp
    ${CMAKE_CURRENT_LIST_DIR}/dto_typecode_conversion_utils_tests.cpp
    ${CMAKE_CURRENT_LIST_DIR}/global_ioc_environment.cpp
    ${CMAKE_CURRENT_LIST_DIR}/main.cpp
    ${CMAKE_CURRENT_LIST_DIR}/pv_access_client_pv_tests.cpp
    ${CMAKE_CURRENT_LIST_DIR}/pv_access_client_server_integration_tests.cpp
    ${CMAKE_CURRENT_LIST_DIR}/pv_access_client_tests.cpp
    ${CMAKE_CURRENT_LIST_DIR}/pv_access_rpc_tests.cpp
    ${CMAKE_CURRENT_LIST_DIR}/pv_access_server_pv_tests.cpp
    ${CMAKE_CURRENT_LIST_DIR}/pv_access_server_tests.cpp
    ${CMAKE_CURRENT_LIST_DIR}/pvxs_builder_nodes_tests.cpp
    ${CMAKE_CURRENT_LIST_DIR}/pvxs_type_builder_tests.cpp
    ${CMAKE_CURRENT_LIST_DIR}/pvxs_utils_tests.cpp
    ${CMAKE_CURRENT_LIST_DIR}/pvxs_value_basics_tests.cpp
    ${CMAKE_CURRENT_LIST_DIR}/pvxs_builder_nodes_tests.cpp
    ${CMAKE_CURRENT_LIST_DIR}/pvxs_value_builder_extended_tests.cpp
    ${CMAKE_CURRENT_LIST_DIR}/pvxs_value_builder_tests.cpp
)

if (GTest_VERSION VERSION_LESS_EQUAL "1.8")
  target_link_libraries(${test} PRIVATE PUBLIC pthread gtest gmock sup-epics sup-epics-test)
else()
  target_link_libraries(${test} PRIVATE PUBLIC pthread GTest::gtest GTest::gmock sup-epics sup-epics-test)
endif()

set_target_properties(${test} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${TEST_OUTPUT_DIRECTORY})

gtest_discover_tests(${test})
