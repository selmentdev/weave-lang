add_library(thirdparty_catch2 STATIC)

target_include_directories(thirdparty_catch2 PUBLIC include)

add_subdirectory(lib)
