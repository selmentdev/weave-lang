#pragma once
#include "weave/platform/Compiler.hxx"

#if !defined(WIN32)
#error "This header is only for Windows builds"
#endif

WEAVE_EXTERNAL_HEADERS_BEGIN

#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

WEAVE_EXTERNAL_HEADERS_END
