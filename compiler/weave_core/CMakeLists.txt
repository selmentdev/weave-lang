add_library(weave_core STATIC
 cxx/String.cxx
)

target_include_directories(weave_core PUBLIC include)

target_link_libraries(weave_core PUBLIC weave_bugcheck)

WEAVE_CXX_FORTIFY_CODE(weave_core)
