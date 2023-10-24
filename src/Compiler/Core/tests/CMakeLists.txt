add_executable(Weave.Core.Tests
    "Main.cxx"
    "SourceText.cxx"
    "SourceCursor.cxx"
    "Sha256.cxx")

target_link_libraries(Weave.Core.Tests PUBLIC Weave.Core)
target_link_libraries(Weave.Core.Tests PUBLIC Catch2)

add_test(
    NAME        Weave.Core.Tests
    COMMAND     Weave.Core.Tests
)
