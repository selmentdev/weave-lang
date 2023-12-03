if (LINUX)
add_subdirectory(posix)
endif()

if (WIN32)
add_subdirectory(windows)
endif()
