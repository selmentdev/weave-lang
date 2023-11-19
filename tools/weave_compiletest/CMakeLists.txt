add_executable(weave_compiletest
    cxx/Main.cxx
)

target_link_libraries(weave_compiletest PUBLIC weave_system)
target_link_libraries(weave_compiletest PUBLIC weave_commandline)
target_link_libraries(weave_compiletest PUBLIC weave_filesystem)

WEAVE_CXX_FORTIFY_CODE(weave_compiletest)

set_target_properties(weave_compiletest PROPERTIES OUTPUT_NAME "weave-compiletest")
