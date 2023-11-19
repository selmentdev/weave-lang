add_library(thirdparty_catch2
    "lib/catch_amalgamated.cpp"
    "include/catch_amalgamated.hpp"
)

target_include_directories(thirdparty_catch2 PUBLIC "include")
