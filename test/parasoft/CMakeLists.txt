if(NOT COA_PARASOFT_INTEGRATION)
  return()
endif()

include(COASetupParasoft)

# Add parasoft project for sup-epics-target
# This generates the build time information needed by Parasoft for analysis
cpptest_add_executable(sup-epics-cpptest
  CPPTEST_PROJECT_LOC ${CMAKE_CURRENT_BINARY_DIR}
  CPPTEST_PROJECT_FOLDERS sup-epics=${CMAKE_SOURCE_DIR}/src/lib/sup/epics
  TARGETS sup-epics
)
get_target_property(SUP_EPICS_INCLUDE_DIRECTORIES sup-epics INCLUDE_DIRECTORIES)
target_include_directories(sup-epics-cpptest PUBLIC ${SUP_EPICS_INCLUDE_DIRECTORIES})

target_link_libraries(sup-epics-cpptest
  PUBLIC
  sup-di::sup-di
  sup-dto::sup-dto
  sup-epics::sup-epics
)
