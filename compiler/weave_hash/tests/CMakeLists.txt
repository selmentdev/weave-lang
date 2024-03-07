add_executable(weave_hash_tests
    "Aes.cxx"
    "Sha256.cxx"
)

target_link_libraries(weave_hash_tests PUBLIC weave_hash)
target_link_libraries(weave_hash_tests PUBLIC thirdparty_catch2)

WEAVE_CXX_FORTIFY_CODE(weave_hash_tests)

add_test(
    NAME        weave_hash_tests
    COMMAND     weave_hash_tests
)
