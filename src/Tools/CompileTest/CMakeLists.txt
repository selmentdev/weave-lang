add_executable(Weave.CompileTest
    cxx/Main.cxx
)

target_link_libraries(Weave.CompileTest PUBLIC Weave.Core)
WEAVE_CXX_FORTIFY_CODE(Weave.CompileTest)

set_target_properties(Weave.CompileTest PROPERTIES
    OUTPUT_NAME "weave-compiletest"
)
