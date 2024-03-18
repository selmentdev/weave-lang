cmake_minimum_required(VERSION 3.25)

set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT ProgramDatabase)

if(MSVC)
list(APPEND CMAKE_EXE_LINKER_FLAGS_RELEASE "-DEBUG")
list(APPEND CMAKE_SHARED_LINKER_FLAGS_RELEASE "-DEBUG")
endif()

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_INTERPROCEDURAL_OPTIMIZATION_RELEASE TRUE)
set(CMAKE_INTERPROCEDURAL_OPTIMIZATION_RELWITHDEBINFO TRUE)
set(CMAKE_OPTIMIZE_DEPENDENCIES_RELEASE TRUE)
set(CMAKE_OPTIMIZE_DEPENDENCIES_RELWITHDEBINFO TRUE)

# Get the project hash and branch from git
execute_process(
    COMMAND git rev-parse HEAD
    WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
    OUTPUT_VARIABLE WEAVE_PROJECT_HASH
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

execute_process(
    COMMAND git rev-parse --abbrev-ref HEAD
    WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
    OUTPUT_VARIABLE WEAVE_PROJECT_BRANCH
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

message(STATUS "Project hash: ${WEAVE_PROJECT_HASH}")
message(STATUS "Project branch: ${WEAVE_PROJECT_BRANCH}")

#if (MSVC)
#    add_compile_options("$<$<CONFIG:RELWITHDEBINFO>:/GA /GT /Ob3 ?Oi /Gw /Gy /GL /O2 /GS- /sdl- /guart:cf>")
#endif()

enable_testing()
enable_language(CXX)

project(weave-lang VERSION 1.0.0.0)

include_directories("${CMAKE_CURRENT_BINARY_DIR}/include")

configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/Version.hxx.in"
    "${CMAKE_CURRENT_BINARY_DIR}/include/weave/Version.hxx" @ONLY
)

include("cmake/Warnings.cmake")

add_subdirectory(compiler)
add_subdirectory(libraries)
add_subdirectory(tests)
add_subdirectory(thirdparty)
add_subdirectory(tools)
