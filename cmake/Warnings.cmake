
function(weave_cxx_fortify_code target)
    if (MSVC)
        target_compile_options(${target} PRIVATE
            -Wall       # enable all warnings
            -WX         # warnings as errors
            -wd4710
            -wd4711
            ################
            -wd4514
            -wd4582
            -wd4820
            -wd4365
            -wd5264
            -wd5039
            -wd5026
            -wd4625
            -wd4626
            -wd5027
            -wd4623
            -wd5045
            -wd4583
            -wd4587
            -wd4588
            -wd4574
            -wd4686
            -wd4868
            -wd4061 # Not all labels are EXPLICITLY handled in switch; w4062 (not all labels are handled and default is missing) is enabled
            -Zc:preprocessor
            -Zc:__cplusplus
            -Zc:lambda
            -Zc:referenceBinding
            -Zc:rvalueCast
            -Zc:inline
            -Zc:strictStrings
            -Zc:ternary
        )
    else()
        target_compile_options(${target} PRIVATE
            -Wall
        )
    endif()
endfunction()
