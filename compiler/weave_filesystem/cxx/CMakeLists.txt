target_sources(weave_filesystem
    PRIVATE
        "FileReader.cxx"
        "FileSystem.cxx"
        "FileWriter.cxx"
        "Pipe.cxx"
        "Path.cxx"
)

if (WIN32)
add_subdirectory(windows)
endif()

if (LINUX)
add_subdirectory(posix)
endif()
