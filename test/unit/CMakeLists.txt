set(unit-tests sup-epics-unit-tests)

add_executable(${unit-tests})

set_target_properties(${unit-tests} PROPERTIES OUTPUT_NAME "unit-tests")
set_target_properties(${unit-tests} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${TEST_OUTPUT_DIRECTORY})

target_sources(${unit-tests}
  PRIVATE
  anyvalue_from_pvxs_builder_tests.cpp
  channel_access_base_tests.cpp
  channel_access_client_tests.cpp
  channel_access_pv_tests.cpp
  dto_conversion_utils_tests.cpp
  dto_scalar_conversion_utils_tests.cpp
  dto_typecode_conversion_utils_tests.cpp
  epics_protocol_factory_utils_tests.cpp
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

target_link_libraries(${unit-tests}
  PRIVATE
    sup-epics
    sup-epics-di
    sup-epics-test
    pthread
)

if(GTest_VERSION VERSION_LESS_EQUAL "1.8")
  target_link_libraries(${unit-tests} PRIVATE gtest gtest_main gmock)
else()
  target_link_libraries(${unit-tests} PRIVATE GTest::gtest GTest::gtest_main GTest::gmock)
endif()

gtest_discover_tests(${unit-tests} DISCOVERY_MODE PRE_TEST)
