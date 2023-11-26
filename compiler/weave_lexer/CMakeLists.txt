add_library(weave_lexer STATIC)

target_include_directories(weave_lexer PUBLIC "include")

target_link_libraries(weave_lexer PUBLIC weave_bugcheck)
target_link_libraries(weave_lexer PUBLIC weave_unicode)
target_link_libraries(weave_lexer PUBLIC weave_source)
target_link_libraries(weave_lexer PUBLIC weave_hash)
target_link_libraries(weave_lexer PUBLIC weave_stringpool)
target_link_libraries(weave_lexer PUBLIC weave_memory)

WEAVE_CXX_FORTIFY_CODE(weave_lexer)

add_subdirectory(cxx)
