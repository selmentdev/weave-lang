add_executable(Weave.Frontend
    cxx/Main.cxx)

target_link_libraries(Weave.Frontend PUBLIC
    Weave.Core)


set_target_properties(Weave.Frontend PROPERTIES
    OUTPUT_NAME "weave-frontend"
)
