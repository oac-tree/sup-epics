target_sources(rpc-sniff-server
    PRIVATE
    registration.cpp
    rpc_sniff_server.cpp
)

target_include_directories(rpc-sniff-server
    PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}
)
