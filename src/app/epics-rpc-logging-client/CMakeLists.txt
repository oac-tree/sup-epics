add_executable(epics-rpc-logging-client)

target_link_libraries(epics-rpc-logging-client
  PRIVATE
  sup-epics::sup-epics
  sup-protocol::sup-protocol
  sup-utils::sup-cli
)

target_sources(epics-rpc-logging-client
  PRIVATE
  main.cpp
  utils.cpp
)

# -- Installation --
install(TARGETS epics-rpc-logging-client RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
