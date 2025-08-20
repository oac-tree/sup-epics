add_executable(epics-rpc-logging-client)

target_include_directories(epics-rpc-logging-client PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/..>
)

target_link_libraries(epics-rpc-logging-client
  PRIVATE
  sup-epics::sup-epics-app-utils
)

target_sources(epics-rpc-logging-client
  PRIVATE
  main.cpp
)

# -- Installation --
install(TARGETS epics-rpc-logging-client RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
