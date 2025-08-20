add_executable(epics-protocol-logging-server)

target_include_directories(epics-protocol-logging-server PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/..>
)

target_link_libraries(epics-protocol-logging-server
  PRIVATE
  sup-epics::sup-epics-app-utils
)

target_sources(epics-protocol-logging-server
  PRIVATE
  main.cpp
)

# -- Installation --
install(TARGETS epics-protocol-logging-server RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
