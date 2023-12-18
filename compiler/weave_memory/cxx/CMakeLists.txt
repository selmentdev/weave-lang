target_sources(weave_memory
    PRIVATE
        "LinearAllocator.cxx"
)

if (WIN32)
add_subdirectory(windows)
endif()

if (LINUX)
add_subdirectory(posix)
endif()
