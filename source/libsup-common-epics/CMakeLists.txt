set(library_name sup-common-epics)

add_library(${library_name} SHARED "")

if (SUPCOMMONEPICS_CODAC)
  target_link_libraries(${library_name} PUBLIC pthread sup-dto)
  target_include_directories(${library_name} PUBLIC $ENV{CODAC_ROOT}/include $ENV{CODAC_ROOT}/include/sup)
  target_link_directories(${library_name} PUBLIC $ENV{CODAC_ROOT}/lib)
else()
  target_link_libraries(${library_name} PUBLIC pthread COACompact::sup-dto)
endif()

add_subdirectory(sup)

target_include_directories(${library_name} PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}>)

# -- Installation --

set_target_properties(${library_name} PROPERTIES EXPORT_NAME sup-common-epics SOVERSION ${SUPCOMMONEPICS_SOVERSION} VERSION ${SUPCOMMONEPICS_BUILDVERSION})
