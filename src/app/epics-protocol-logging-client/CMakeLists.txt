add_executable(epics-protocol-logging-client)

target_include_directories(epics-protocol-logging-client PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/..>
)

target_link_libraries(epics-protocol-logging-client
  PRIVATE
  sup-epics::sup-epics-app-utils
)

target_sources(epics-protocol-logging-client
  PRIVATE
  main.cpp
)

# -- Installation --
install(TARGETS epics-protocol-logging-client RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
