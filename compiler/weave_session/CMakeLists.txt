add_library(weave_session STATIC
    "cxx/Session.cxx"
 )

target_include_directories(weave_session PUBLIC include)
target_link_libraries(weave_session PUBLIC weave_core)
target_link_libraries(weave_session PUBLIC weave_commandline)
target_link_libraries(weave_session PUBLIC weave_errors)
WEAVE_CXX_FORTIFY_CODE(weave_session)

#add_subdirectory(tests)
