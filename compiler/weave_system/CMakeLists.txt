add_library(weave_system STATIC)


if (WIN32)

target_sources(weave_system PRIVATE
 cxx/windows/process.cxx
)

endif()

if(LINUX)

target_sources(weave_system PRIVATE
 cxx/posix/process.cxx
)

endif()

target_include_directories(weave_system PUBLIC include)

target_link_libraries(weave_system PUBLIC weave_platform)
target_link_libraries(weave_system PUBLIC thirdparty_libfmt)

WEAVE_CXX_FORTIFY_CODE(weave_system)
