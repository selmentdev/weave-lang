add_library(weave_stringpool STATIC
 "cxx/StringPool.cxx"
)

target_include_directories(weave_stringpool PUBLIC include)

target_link_libraries(weave_stringpool PUBLIC weave_bugcheck)
target_link_libraries(weave_stringpool PUBLIC weave_memory)
target_link_libraries(weave_stringpool PUBLIC weave_hash)

WEAVE_CXX_FORTIFY_CODE(weave_stringpool)
