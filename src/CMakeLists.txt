cmake_minimum_required(VERSION 3.14)
project(sup-epics-project VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(SUPEPICS_CODAC "Is in CODAC environment (option is set to ON automaticaly if CODAC_ROOT is defined)" OFF)
option(COVERAGE "Generate unit test coverage information" OFF)

set(TARGET_DIRECTORY ${CMAKE_CURRENT_LIST_DIR})

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/../src/cmake/modules)
include(configuration)

add_subdirectory(main/c++)
add_subdirectory(test/c++)
