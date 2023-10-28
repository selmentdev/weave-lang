#pragma once

// Detect ASAN support

#if defined(__has_feature)
#if __has_feature(address_sanitizer)
#define WEAVE_FEATURE_ADDRESS_SANITIZER 1
#endif
#elif defined(__SANITIZE_ADDRESS__)
#define WEAVE_FEATURE_ADDRESS_SANITIZER 1
#endif

#ifndef WEAVE_FEATURE_ADDRESS_SANITIZER
#define WEAVE_FEATURE_ADDRESS_SANITIZER 0
#endif

// Attribute to disable ASAN instrumentation
#if WEAVE_FEATURE_ADDRESS_SANITIZER

#if defined(_MSC_VER)
#define WEAVE_ASAN_DISABLE __declspec(no_sanitize_address, noinline)
#else
#define WEAVE_ASAN_DISABLE __attribute__((__no_sanitize_address__))
#endif
#endif

#ifndef WEAVE_ASAN_DISABLE
#define WEAVE_ASAN_DISABLE
#endif


#include <sanitizer/asan_interface.h>

#if defined(_MSC_VER)

#define WEAVE_EXTERNAL_HEADERS_BEGIN \
    __pragma(warning(push, 0));

#define WEAVE_EXTERNAL_HEADERS_END \
    __pragma(warning(pop));

#else

#define WEAVE_EXTERNAL_HEADERS_BEGIN \
    _Pragma("clang diagnostic push"); \
    _Pragma("clang diagnostic ignored \"-Weverything\"")

#define WEAVE_EXTERNAL_HEADERS_END \
    _Pragma("clang diagnostic pop")

#endif


#if defined(_MSC_VER)
#define WEAVE_DEBUGBREAK __debugbreak
#elif defined(__GNUC__)
#define WEAVE_DEBUGBREAK __builtin_trap
#elif defined(__clang__)
#define WEAVE_DEBUGBREAK __builtin_debugtrap
#else
#error Not implemented
#endif
