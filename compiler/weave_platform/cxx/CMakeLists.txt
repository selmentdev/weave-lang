target_sources(weave_platform
    PRIVATE
        "SystemErrorFromErrno.cxx"
)

if (WIN32)
add_subdirectory(windows)
endif()
