add_library(Weave.Core STATIC
 "cxx/Charset.cxx"
 "cxx/Diagnostic.cxx"
 "cxx/HandleTable.cxx"
 "cxx/Platform/PageAllocator.cxx"
 "cxx/Platform/Windows.cxx"
 "cxx/SourceCursor.cxx"
 "cxx/SourceText.cxx"
 "cxx/Unicode.cxx"
 "cxx/Uri.cxx"
 "cxx/IO/FileHandle.Windows.cxx"
 "cxx/IO/FileHandle.Posix.cxx"
 "cxx/IO/FileSystem.cxx"
 "cxx/IO/FileReader.cxx"
 "cxx/IO/FileWriter.cxx"
 "cxx/Cryptography/Sha256.cxx"
 "cxx/IO/FileSystem.Windows.cxx"
 "cxx/IO/Utils.Windows.cxx")

target_include_directories(Weave.Core PUBLIC include)

target_link_libraries(Weave.Core PUBLIC Fmt)

add_subdirectory(tests)
