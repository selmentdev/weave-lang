add_library(Catch2
    "lib/catch_amalgamated.cpp"
    "include/catch_amalgamated.hpp"
)

target_include_directories(Catch2 PUBLIC "include")
