add_library(sup-epics SHARED "")
add_library(sup-epics::sup-epics ALIAS sup-epics) # alias for build-tree usage

set_target_properties(sup-epics PROPERTIES
    EXPORT_NAME sup-epics
    SOVERSION ${SUP_EPICS_SOVERSION}
    VERSION ${SUP_EPICS_BUILDVERSION}
)

target_link_directories(sup-epics PUBLIC ${EPICS_LINK_DIRECTORIES})

target_link_libraries(sup-epics
  PUBLIC
    sup-dto::sup-dto
    sup-protocol::sup-protocol
    ${EPICS_LIBRARIES}
  PRIVATE
    pthread
)

target_include_directories(sup-epics PUBLIC
  $<INSTALL_INTERFACE:include>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/../..$<SEMICOLON>${EPICS_INCLUDE_DIRECTORIES}>)

add_subdirectory(utils)
add_subdirectory(ca)
add_subdirectory(pvxs)
add_subdirectory(rpc)
# -- Installation --

install(TARGETS sup-epics EXPORT sup-epics-targets LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})

install(FILES
  ca_types.h
  channel_access_client.h
  channel_access_pv.h
  pv_access_client_pv.h
  pv_access_client.h
  pv_access_rpc_client_config.h
  pv_access_rpc_client.h
  pv_access_rpc_server_config.h
  pv_access_rpc_server.h
  pv_access_server.h
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/sup/epics
)
