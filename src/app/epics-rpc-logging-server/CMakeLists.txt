add_executable(epics-rpc-logging-server)

target_link_libraries(epics-rpc-logging-server
  PRIVATE
  sup-epics::sup-epics
  sup-protocol::sup-protocol
  sup-utils::sup-cli
)

target_sources(epics-rpc-logging-server
  PRIVATE
  main.cpp
  utils.cpp
)

# -- Installation --
install(TARGETS epics-rpc-logging-server RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
