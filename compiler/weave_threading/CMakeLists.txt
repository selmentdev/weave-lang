add_library(weave_threading STATIC)

target_include_directories(weave_threading PUBLIC include)

target_link_libraries(weave_threading PUBLIC weave_time)
target_link_libraries(weave_threading PUBLIC weave_platform)
target_link_libraries(weave_threading PUBLIC weave_bugcheck)

if (WIN32)
target_link_libraries(weave_threading PRIVATE "Winmm")
endif()

WEAVE_CXX_FORTIFY_CODE(weave_threading)

add_subdirectory(cxx)
