add_executable(epics-rpc-logging-server)

target_include_directories(epics-rpc-logging-server PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/..>
)

target_link_libraries(epics-rpc-logging-server
  PRIVATE
  sup-epics::sup-epics-app-utils
)

target_sources(epics-rpc-logging-server
  PRIVATE
  main.cpp
)

# -- Installation --
install(TARGETS epics-rpc-logging-server RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
