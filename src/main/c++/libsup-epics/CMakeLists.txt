set(library_name sup-epics)

add_library(${library_name} SHARED "")
add_library(sup-epics::sup-epics ALIAS ${library_name}) # alias for build-tree usage

if (SUP_EPICS_CODAC)
  target_link_libraries(${library_name} PUBLIC pthread sup-dto pvxs ca)
  target_include_directories(${library_name} PUBLIC $ENV{CODAC_ROOT}/include $ENV{CODAC_ROOT}/include/sup $ENV{CODAC_ROOT}/epics/include $ENV{CODAC_ROOT}/epics/include/os/Linux $ENV{CODAC_ROOT}/epics/include/compiler/gcc)
  target_link_directories(${library_name} PUBLIC $ENV{CODAC_ROOT}/lib $ENV{CODAC_ROOT}/epics/lib/linux-x86_64)
  target_include_directories(${library_name} PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}>)

else()
  find_package(EPICS REQUIRED)
  target_link_directories(${library_name} PUBLIC ${EPICS_LINK_DIRECTORIES})
  target_link_libraries(${library_name} PUBLIC pthread sup-dto::sup-dto ${EPICS_LIBRARIES})
  target_include_directories(${library_name} PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}$<SEMICOLON>${EPICS_INCLUDE_DIRECTORIES}>)
endif()

add_subdirectory(sup)

# -- Installation --

set_target_properties(${library_name} PROPERTIES
    EXPORT_NAME sup-epics
    SOVERSION ${SUP_EPICS_SOVERSION}
    VERSION ${SUP_EPICS_BUILDVERSION}
)

install(TARGETS ${library_name} EXPORT sup-epics-targets LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
set_target_properties(${library_name} PROPERTIES EXPORT_NAME sup-epics)

install(FILES
        sup/epics/ca_types.h
        sup/epics/channel_access_client.h
        sup/epics/channel_access_pv.h
        sup/epics/dto_conversion_utils.h
        sup/epics/dto_types_fwd.h
        sup/epics/pv_access_client.h
        sup/epics/pv_access_server.h
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/sup/epics
)
