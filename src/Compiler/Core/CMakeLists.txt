add_library(Weave.Core STATIC
 "cxx/Assert.cxx"
 "cxx/Charset.cxx"
 "cxx/Cryptography/Sha256.cxx"
 "cxx/Diagnostic.cxx"
 "cxx/HandleTable.cxx"
 "cxx/IO/FileHandle.Posix.cxx"
 "cxx/IO/FileHandle.Windows.cxx"
 "cxx/IO/FileReader.cxx"
 "cxx/IO/FileSystem.cxx"
 "cxx/IO/FileSystem.Windows.cxx"
 "cxx/IO/FileWriter.cxx"
 "cxx/IO/Utils.Windows.cxx"
 "cxx/IO/Utils.Posix.cxx"
 "cxx/Platform/PageAllocator.cxx"
 "cxx/Platform/Windows.cxx"
 "cxx/SourceCursor.cxx"
 "cxx/SourceText.cxx"
 "cxx/Unicode.cxx"
 "cxx/Uri.cxx"
 "cxx/Memory/Allocator.cxx"
 "cxx/StringPool.cxx"
 "cxx/UInt128.cxx"
 "cxx/Int128.cxx"
 "cxx/String.cxx"
 "cxx/IO/Pipe.cxx"
 "cxx/IO/Pipe.Posix.cxx"
 "cxx/IO/Pipe.Windows.cxx"
 "cxx/Process.Posix.cxx"
 "cxx/Process.Windows.cxx"
)

target_include_directories(Weave.Core PUBLIC include)
target_link_libraries(Weave.Core PUBLIC Fmt)
target_link_libraries(Weave.Core PUBLIC Weave.Builtin)
WEAVE_CXX_FORTIFY_CODE(Weave.Core)

add_subdirectory(tests)
