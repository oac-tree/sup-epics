cmake_minimum_required(VERSION 3.14)
project(sup-common-epics-project VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

option(SUPCOMMONEPICS_CODAC "Is in CODAC environment (option is set to ON automaticaly if CODAC_ROOT is defined)" OFF)

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/cmake/modules)
include(configuration)

add_subdirectory(source)
add_subdirectory(tests)
