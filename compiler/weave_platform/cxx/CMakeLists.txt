target_sources(weave_platform
    PRIVATE
        "StringBuffer.cxx"
        "SystemErrorFromErrno.cxx"
)

if (WIN32)
add_subdirectory(windows)
endif()
