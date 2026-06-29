
/**
    PAL - Prime Abstraction Layer
    Copyright (C) 2025
    Licensed under the Zlib license. See LICENSE file in root.
 */

#ifndef _PAL_PLATFORM_H
#define _PAL_PLATFORM_H

// clang-format off

#if defined(__linux__) || defined(__APPLE__) || defined(__FreeBSD__) \
                       || defined(__NetBSD) || defined(__OpenBSD)
#define _PAL_HAS_POSIX 1
#else 
#define _PAL_HAS_POSIX 0
#endif // _PAL_HAS_POSIX

#if defined(__linux__) || defined(__ANDROID__)
#define _PAL_HAS_EGL 1
#else 
#define _PAL_HAS_EGL 0
#endif // _PAL_HAS_EGL

// clang-format on

#endif // _PAL_PLATFORM_H