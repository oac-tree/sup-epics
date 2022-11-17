# -----------------------------------------------------------------------------
# Modules
# -----------------------------------------------------------------------------

include(CTest)
include(GNUInstallDirs)

# -----------------------------------------------------------------------------
# Find if we are on CODAC infrastructure
# -----------------------------------------------------------------------------

get_filename_component(SUP_EPICS_PROJECT_DIR "${CMAKE_CURRENT_LIST_DIR}/../.." ABSOLUTE)

if (SUP_EPICS_CODAC)
  if (DEFINED ENV{CODAC_ROOT})
    message(STATUS "CODAC environment detected at $ENV{CODAC_ROOT}")
  else()
    message(FATAL "No CODAC environment detected")
  endif()
else()
  message(STATUS "Compiling without CODAC")
endif()

# -----------------------------------------------------------------------------
# Variables
# -----------------------------------------------------------------------------

set(SUP_EPICS_SOVERSION ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR})
set(SUP_EPICS_BUILDVERSION ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH})

if (NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "RelWithDebInfo")
endif()

# -----------------------------------------------------------------------------
# Directories
# -----------------------------------------------------------------------------

if (DEFINED ENV{CODAC_ROOT})
  set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${SUP_EPICS_PROJECT_DIR}/target/bin)
else()
  set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/bin)
endif()

# -----------------------------------------------------------------------------
# Dependencies
# -----------------------------------------------------------------------------

find_package(Threads)

if (NOT SUP_EPICS_CODAC)
  find_package(sup-dto REQUIRED)
endif()

# -----------------------------------------------------------------------------
# Flags
# -----------------------------------------------------------------------------

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic")
if (COVERAGE)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0 -g -fprofile-arcs -ftest-coverage --coverage")
  message(INFO " Coverage enabled ${CMAKE_CXX_FLAGS}")
endif()
