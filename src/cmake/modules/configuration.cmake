# -----------------------------------------------------------------------------
# Modules
# -----------------------------------------------------------------------------

include(CTest)
include(GNUInstallDirs)

# -----------------------------------------------------------------------------
# Find if we are on CODAC infrastructure
# -----------------------------------------------------------------------------

if (DEFINED ENV{CODAC_ROOT})
  message(STATUS "CODAC environment detected at $ENV{CODAC_ROOT}")
  set(SUPEPICS_CODAC ON)
else()
  message(STATUS "No CODAC environment detected - expecting COACompact installed")
  set(SUPEPICS_CODAC OFF)
endif()

# -----------------------------------------------------------------------------
# Variables
# -----------------------------------------------------------------------------

get_filename_component(SUPEPICS_PROJECT_DIR "${CMAKE_CURRENT_LIST_DIR}/../.." ABSOLUTE)

set(SUPEPICS_SOVERSION ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR})
set(SUPEPICS_BUILDVERSION ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH})

# -----------------------------------------------------------------------------
# Directories
# -----------------------------------------------------------------------------

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})

file(MAKE_DIRECTORY ${CMAKE_LIBRARY_OUTPUT_DIRECTORY})
file(MAKE_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})

# -----------------------------------------------------------------------------
# Dependencies
# -----------------------------------------------------------------------------

find_package(Threads)

if (NOT SUPEPICS_CODAC)
  find_package(COACompact REQUIRED sup-dto)
endif()

# -----------------------------------------------------------------------------
# Flags
# -----------------------------------------------------------------------------

if (COVERAGE)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0 -g -fprofile-arcs -ftest-coverage --coverage")
    message(INFO " Coverage enabled ${CMAKE_CXX_FLAGS}")
endif()
