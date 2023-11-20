target_sources(weave_platform PRIVATE
    StringBuffer.cxx
)

if (WIN32)
add_subdirectory(windows)
endif()
