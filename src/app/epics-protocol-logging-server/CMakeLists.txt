add_executable(epics-protocol-logging-server)

target_link_libraries(epics-protocol-logging-server
  PRIVATE
  sup-epics::sup-epics
  sup-protocol::sup-protocol
  sup-utils::sup-cli
)

target_sources(epics-protocol-logging-server
  PRIVATE
  main.cpp
  utils.cpp
)

# -- Installation --
install(TARGETS epics-protocol-logging-server RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
