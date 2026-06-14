
/**
 PAL - Prime Abstraction Layer
 Copyright (C) 2025
 Licensed under the Zlib license. See LICENSE file in root.
 */

#ifdef __linux__
#define _POSIX_C_SOURCE 200112L
#include "pal/pal_core.h"
#include <time.h>

uint64_t PAL_CALL palGetPerformanceCounter()
{
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (uint64_t)ts.tv_sec * 1000000000LL + (uint64_t)ts.tv_nsec;
}

uint64_t PAL_CALL palGetPerformanceFrequency()
{
    return 1000000000LL;
}

#endif // __linux__