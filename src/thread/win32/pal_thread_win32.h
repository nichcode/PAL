
/**
    PAL - Prime Abstraction Layer
    Copyright (C) 2025
    Licensed under the Zlib license. See LICENSE file in root.
 */

#ifndef _PAL_THREAD_WIN32_H
#define _PAL_THREAD_WIN32_H
#ifdef _WIN32

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif // WIN32_LEAN_AND_MEAN

#ifndef NOMINMAX
#define NOMINMAX
#endif // NOMINMAX

// set unicode
#ifndef UNICODE
#define UNICODE
#endif // UNICODE

#include "pal/pal_thread.h"
#include <windows.h>

struct PalMutex {
    const PalAllocator* allocator;
    CRITICAL_SECTION sc;
};

struct PalCondVar {
    const PalAllocator* allocator;
    CONDITION_VARIABLE cv;
};

#endif // _WIN32
#endif // _PAL_THREAD_WIN32_H