# Simplified search for EPICS

if(DEFINED ENV{EPICS_BASE} AND NOT EPICS_BASE)
  set(EPICS_BASE $ENV{EPICS_BASE})
endif()

if(EPICS_BASE)
  message(VERBOSE "\tEPICS_BASE: ${EPICS_BASE}")
else()
  message(FATAL_ERROR "Environment variable EPICS_BASE doesn't exist")
endif()

# Finding PVXS

if(DEFINED ENV{PVXS_DIR} AND NOT PVXS_DIR)
  set(PVXS_DIR $ENV{PVXS_DIR})
endif()

if(PVXS_DIR)
  message(VERBOSE "\tPVXS_DIR: ${PVXS_DIR}")
else()
  message(VERBOSE "\tPVXS_DIR: no dedicated PVXS installation, expecting to find it in ${EPICS_BASE}")
  set(PVXS_DIR ${EPICS_BASE})
endif()

# finding architecture

if(DEFINED ENV{EPICS_HOST_ARCH} AND NOT EPICS_HOST_ARCH)
  set(EPICS_HOST_ARCH $ENV{EPICS_HOST_ARCH})
endif()

if(EPICS_HOST_ARCH)
  message(VERBOSE "\tEPICS_HOST_ARCH: ${EPICS_HOST_ARCH}")
else()
  message(FATAL_ERROR "Environment variable EPICS_HOST_ARCH doesn't exist")
endif()

# finding compiler

set(compiler_subdir compiler/gcc)
if(CMAKE_CXX_COMPILER_ID MATCHES ".*Clang.*")
  set(compiler_subdir compiler/clang)
endif()

set(EPICS_LINK_DIRECTORIES ${EPICS_BASE}/lib/${EPICS_HOST_ARCH} ${PVXS_DIR}/lib/${EPICS_HOST_ARCH})
set(EPICS_LIBRARIES pvxs ca)
set(EPICS_INCLUDE_DIRECTORIES ${EPICS_BASE}/include ${EPICS_BASE}/include/${compiler_subdir} ${EPICS_BASE}/include/os/${CMAKE_HOST_SYSTEM_NAME} ${PVXS_DIR}/include)
set(EPICS_FOUND 1)

mark_as_advanced(EPICS_FOUND EPICS_LINK_DIRECTORIES EPICS_LIBRARIES EPICS_INCLUDE_DIRECTORIES)
