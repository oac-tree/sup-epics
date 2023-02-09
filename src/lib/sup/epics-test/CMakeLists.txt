set(library_name sup-epics-test)
set(export_name sup-epics-test)

add_library(${library_name} SHARED "")
add_library(sup-epics::${export_name} ALIAS ${library_name}) # alias for build-tree usage

set_target_properties(${library_name} PROPERTIES
    EXPORT_NAME ${export_name}
    SOVERSION ${SUP_EPICS_SOVERSION}
    VERSION ${SUP_EPICS_BUILDVERSION}
)

target_include_directories(${library_name} PUBLIC
    $<INSTALL_INTERFACE:include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/../..>)


target_sources(${library_name} PRIVATE
  softioc_runner.cpp
  softioc_utils.cpp
  unit_test_helper.cpp
)

# -- Installation --

install(TARGETS ${library_name} EXPORT sup-epics-targets LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
install(FILES
  softioc_runner.h
  softioc_utils.h
  unit_test_helper.h
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/sup/epics-test
)
