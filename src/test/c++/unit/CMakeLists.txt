set(test unit-tests)

file(GLOB source_files "*.cpp")
file(GLOB include_files "*.h")

add_executable(${test} ${source_files} ${include_files})

if (GTest_VERSION VERSION_LESS_EQUAL "1.8")
  target_link_libraries(${test} PRIVATE PUBLIC pthread gtest gmock sup-epics)
else()
  target_link_libraries(${test} PRIVATE PUBLIC pthread GTest::gtest GTest::gmock sup-epics)
endif()

gtest_discover_tests(${test})
