set(library_name sup-epics-di)
set(export_name sup-epics-di)

add_library(${library_name} SHARED "")
add_library(sup-epics::${export_name} ALIAS ${library_name}) # alias for build-tree usage

set_target_properties(${library_name} PROPERTIES
    EXPORT_NAME ${export_name}
    SOVERSION ${LIBSOVERSION}
    VERSION ${LIBVERSION}
  )

target_link_libraries(${library_name}
  PRIVATE
    sup-epics
  PUBLIC
    sup-di::sup-di
)

target_include_directories(${library_name} PRIVATE
    $<INSTALL_INTERFACE:include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/../..>)


target_sources(${library_name} PRIVATE
  pv_access_rpc.cpp
)

# -- Installation --

install(TARGETS ${library_name} EXPORT sup-epics-targets LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})

install(FILES
  registered_names.h
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/sup/epics-di
)
