cmake_minimum_required(VERSION 3.13)

project(rpc-sniff-server
        VERSION 1.0
        LANGUAGES CXX)

add_library(rpc-sniff-server SHARED)

set_target_properties(rpc-sniff-server PROPERTIES
    OUTPUT_NAME rpc-sniff-server
)

# --- Find CODAC Enviorenment ---
if(DEFINED ENV{CODAC_ROOT})
  list(APPEND CMAKE_PREFIX_PATH $ENV{CODAC_ROOT})
endif()

find_package(sup-dto REQUIRED)
find_package(sup-di REQUIRED)

target_link_libraries(rpc-sniff-server
    PRIVATE
    sup-di::sup-di
    sup-dto::sup-dto
)

add_subdirectory(src)
