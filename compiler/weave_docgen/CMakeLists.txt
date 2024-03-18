add_executable(weave_docgen)

target_link_libraries(weave_docgen PUBLIC weave_syntax)
target_link_libraries(weave_docgen PUBLIC weave_filesystem)
target_link_libraries(weave_docgen PUBLIC weave_time)
target_link_libraries(weave_docgen PUBLIC weave_threading)
target_link_libraries(weave_docgen PUBLIC weave_source)
target_link_libraries(weave_docgen PUBLIC weave_system)
target_link_libraries(weave_docgen PUBLIC weave_commandline)
target_link_libraries(weave_docgen PUBLIC weave_bugcheck)
target_link_libraries(weave_docgen PUBLIC weave_profiler)

target_include_directories(weave_docgen PUBLIC include)

set_target_properties(weave_docgen PROPERTIES OUTPUT_NAME "weave-docgen")

WEAVE_CXX_FORTIFY_CODE(weave_docgen)

add_subdirectory(cxx)

#add_custom_target(weave_docgen_copyfiles ALL
#    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/res ${CMAKE_CURRENT_BINARY_DIR}/res
#    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/res
#    COMMENT "Copying weave_docgen resources"
#)
#add_dependencies(weave_docgen weave_docgen_copyfiles)

install(TARGETS weave_docgen)
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/res/ DESTINATION "share/weave-docgen")
