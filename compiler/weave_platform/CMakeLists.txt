add_library(weave_platform STATIC
  cxx/StringBuffer.cxx
)

if (WIN32)

target_sources(weave_platform PRIVATE
 cxx/windows/String.cxx
)

endif()

target_include_directories(weave_platform PUBLIC include)

WEAVE_CXX_FORTIFY_CODE(weave_platform)
