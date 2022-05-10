set(library_name sup-common-epics)

add_library(${library_name} SHARED "")

if (SUPCOMMONEPICS_CODAC)
  target_link_libraries(${library_name} PUBLIC pthread sup-dto pvxs ca)
  target_include_directories(${library_name} PUBLIC $ENV{CODAC_ROOT}/include $ENV{CODAC_ROOT}/include/sup $ENV{CODAC_ROOT}/epics/include)
  target_link_directories(${library_name} PUBLIC $ENV{CODAC_ROOT}/lib $ENV{CODAC_ROOT}/epics/lib/linux-x86_64)
  target_include_directories(${library_name} PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}>)

else()
  find_package(EPICS REQUIRED)
  target_link_directories(${library_name} PUBLIC ${EPICS_LINK_DIRECTORIES})
  target_link_libraries(${library_name} PUBLIC pthread COACompact::sup-dto ${EPICS_LIBRARIES})
  target_include_directories(${library_name} PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}$<SEMICOLON>${EPICS_INCLUDE_DIRECTORIES}>)
endif()

add_subdirectory(sup)

# -- Installation --

set_target_properties(${library_name} PROPERTIES EXPORT_NAME sup-common-epics SOVERSION ${SUPCOMMONEPICS_SOVERSION} VERSION ${SUPCOMMONEPICS_BUILDVERSION})
install(TARGETS ${library_name} LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
install(FILES ${CMAKE_CURRENT_LIST_DIR}/sup/epics/dtotypes_fwd.h DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/sup/epics)
install(FILES ${CMAKE_CURRENT_LIST_DIR}/sup/epics/dtoconversionutils.h DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/sup/epics)
