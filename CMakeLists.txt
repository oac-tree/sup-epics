cmake_minimum_required(VERSION 3.14)
project(sup-epics-project VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(COVERAGE "Generate unit test coverage information" OFF)
option(SUP_EPICS_EXPORT_BUILDTREE "Export build tree in /home/user/.cmake registry" OFF)
option(NO_CODAC "Don't look for the presence of CODAC environment" OFF)

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/cmake/module)
include(configuration)

add_subdirectory(src)
add_subdirectory(test)

include(installation)
