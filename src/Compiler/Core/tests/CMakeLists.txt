add_executable(Weave.Core.Tests
    "Main.cxx"
    "SourceText.cxx"
    "SourceCursor.cxx"
    "Sha256.cxx")

target_link_libraries(Weave.Core.Tests PUBLIC Weave.Core)
target_link_libraries(Weave.Core.Tests PUBLIC Catch2)
WEAVY_CXX_FORTIFY_CODE(Weave.Core.Tests)

add_test(
    NAME        Weave.Core.Tests
    COMMAND     Weave.Core.Tests
)
