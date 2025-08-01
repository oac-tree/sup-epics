set(test sup-epics-unit-tests)

file(GLOB include_files "*.h")

add_executable(${test} ${include_files})

set_target_properties(${test} PROPERTIES OUTPUT_NAME "unit-tests")
set_target_properties(${test} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${TEST_OUTPUT_DIRECTORY})

target_sources(${test}
  PRIVATE
  anyvalue_from_pvxs_builder_tests.cpp
  channel_access_base_tests.cpp
  channel_access_client_tests.cpp
  channel_access_pv_tests.cpp
  dto_conversion_utils_tests.cpp
  dto_scalar_conversion_utils_tests.cpp
  dto_typecode_conversion_utils_tests.cpp
  epics_protocol_factory_tests.cpp
  global_ioc_environment.cpp
  main.cpp
  pv_access_client_pv_tests.cpp
  pv_access_client_server_integration_tests.cpp
  pv_access_client_tests.cpp
  pv_access_logging_client_server_tests.cpp
  pv_access_rpc_tests.cpp
  pv_access_server_pv_tests.cpp
  pv_access_server_tests.cpp
  pvxs_builder_nodes_tests.cpp
  pvxs_type_builder_tests.cpp
  pvxs_utils_tests.cpp
  pvxs_value_basics_tests.cpp
  pvxs_builder_nodes_tests.cpp
  pvxs_value_builder_extended_tests.cpp
  pvxs_value_builder_tests.cpp
  sup_epics_di_tests.cpp
)

if(GTest_VERSION VERSION_LESS_EQUAL "1.8")
  target_link_libraries(${test} PRIVATE pthread gtest gmock)
else()
  target_link_libraries(${test} PRIVATE pthread GTest::gtest GTest::gmock)
endif()

target_link_libraries(${test} PRIVATE sup-epics sup-epics-di sup-epics-test)

set_target_properties(${test} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${TEST_OUTPUT_DIRECTORY})

gtest_discover_tests(${test} DISCOVERY_MODE PRE_TEST)
