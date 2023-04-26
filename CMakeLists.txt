cmake_minimum_required(VERSION 3.14)
project(sup-epics-project VERSION 1.0.0 LANGUAGES CXX)

option(COVERAGE "Generate unit test coverage information" OFF)
option(COA_EXPORT_BUILD_TREE "Export build tree in /home/user/.cmake registry" OFF)
option(COA_BUILD_TESTS "Build unit tests" ON)
option(COA_BUILD_DOCUMENTATION "Build documentation" OFF)
option(COA_NO_CODAC "Don't look for the presence of CODAC environment" OFF)

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/cmake/module)
set(CMAKE_CONFIG_PATH ${CMAKE_CURRENT_LIST_DIR}/cmake/config)
include(configuration)

add_subdirectory(src)
if (COA_BUILD_TESTS)
  add_subdirectory(test)
endif()
add_subdirectory(doc)

include(installation)
