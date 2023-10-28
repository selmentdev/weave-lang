add_executable(Weave.Frontend
    cxx/Main.cxx)

target_link_libraries(Weave.Frontend PUBLIC Weave.Core)
target_link_libraries(Weave.Frontend PUBLIC Weave.Syntax)
WEAVE_CXX_FORTIFY_CODE(Weave.Frontend)


set_target_properties(Weave.Frontend PROPERTIES
    OUTPUT_NAME "weave-frontend"
)
