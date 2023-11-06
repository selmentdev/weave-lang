add_library(Weave.Builtin STATIC
 "cxx/numerics.uint128.cxx")

target_include_directories(Weave.Builtin PUBLIC include)
target_link_libraries(Weave.Builtin PUBLIC Fmt)
WEAVE_CXX_FORTIFY_CODE(Weave.Builtin)
