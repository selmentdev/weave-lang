add_library(weave_unicode STATIC
 "cxx/Unicode.cxx"
)

target_include_directories(weave_unicode PUBLIC include)

target_link_libraries(weave_unicode PUBLIC weave_bugcheck)

WEAVE_CXX_FORTIFY_CODE(weave_unicode)
