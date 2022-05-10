set(test testsup-common-epics)

file(GLOB source_files "*.cpp")
file(GLOB include_files "*.h")

if (COVERAGE)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0 -g -fprofile-arcs -ftest-coverage --coverage")
endif()

add_executable(${test} ${source_files} ${include_files})

target_link_libraries(${test} PRIVATE PUBLIC pthread gtest gmock sup-common-epics)

gtest_discover_tests(${test})
