add_library(sup-epics-app-utils SHARED)
add_library(sup-epics::sup-epics-app-utils ALIAS sup-epics-app-utils)

set_target_properties(sup-epics-app-utils PROPERTIES
    VERSION ${LIBVERSION}
    SOVERSION ${LIBSOVERSION}
    OUTPUT_NAME sup-epics-app-utils
)

target_include_directories(sup-epics-app-utils PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/../lib>
)

target_link_libraries(sup-epics-app-utils PUBLIC
  sup-epics::sup-epics
  sup-protocol::sup-protocol
  sup-utils::sup-cli
)

target_sources(sup-epics-app-utils
  PRIVATE
  app_utils.cpp
)

add_subdirectory(epics-rpc-logging-client)
add_subdirectory(epics-rpc-logging-server)
add_subdirectory(epics-protocol-logging-client)
add_subdirectory(epics-protocol-logging-server)

install(TARGETS sup-epics-app-utils EXPORT sup-epics-targets LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
