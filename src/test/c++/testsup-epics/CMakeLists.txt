set(test testsup-epics)

file(GLOB source_files "*.cpp")
file(GLOB include_files "*.h")

add_executable(${test} ${source_files} ${include_files})

target_link_libraries(${test} PRIVATE PUBLIC pthread gtest gmock sup-epics)

gtest_discover_tests(${test})
