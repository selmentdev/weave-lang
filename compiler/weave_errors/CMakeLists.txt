add_library(weave_errors STATIC
    "cxx/Handler.cxx"
 )

target_include_directories(weave_errors PUBLIC include)
WEAVE_CXX_FORTIFY_CODE(weave_errors)

#add_subdirectory(tests)
