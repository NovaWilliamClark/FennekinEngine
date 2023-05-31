#pragma once
// Detect the operating system
#if defined(_WIN32) || defined(_WIN64)
#define PLATFORM_WINDOWS
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#undef ERROR
#undef CRITICAL
#elif defined(__unix__) || defined(__APPLE__)
#define PLATFORM_UNIX
#include <unistd.h>
#else
#error "Unknown platform"
#endif

// Further detection for Unix variants if needed
#ifdef PLATFORM_UNIX
#if defined(__APPLE__)
#define PLATFORM_MAC
#elif defined(__linux__)
#define PLATFORM_LINUX
#else
#warning "Unknown Unix platform"
#endif
#endif

// TODO: Instruction Set Detection to allow for SIMD optimizations