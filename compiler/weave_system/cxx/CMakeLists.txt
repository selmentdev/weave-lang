target_sources(weave_system
    PRIVATE
)

if (WIN32)
add_subdirectory("windows")
endif()

if (LINUX)
add_subdirectory("posix")
endif()
