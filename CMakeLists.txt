cmake_minimum_required(VERSION 3.25)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)
set(CMAKE_OPTIMIZE_DEPENDENCIES TRUE)

enable_testing()

project(weave-lang VERSION 1.0.0.0)

include_directories("${CMAKE_CURRENT_BINARY_DIR}/include")

configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/Version.hxx.in"
    "${CMAKE_CURRENT_BINARY_DIR}/include/Weave/Version.hxx" @ONLY
)

include("cmake/Warnings.cmake")

add_subdirectory("src")
