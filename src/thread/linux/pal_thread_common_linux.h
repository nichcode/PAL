
/**
    PAL - Prime Abstraction Layer
    Copyright (C) 2025
    Licensed under the Zlib license. See LICENSE file in root.
 */

#ifdef __linux__
#ifndef _PAL_THREAD_COMMON_LINUX_H
#define _PAL_THREAD_COMMON_LINUX_H

#define _POSIX_C_SOURCE 200112L
#include "pal/pal_thread.h"
#include <errno.h>
#include <pthread.h>
#include <time.h>

struct PalCondVar {
    const PalAllocator* allocator;
    pthread_cond_t handle;
};

struct PalMutex {
    const PalAllocator* allocator;
    pthread_mutex_t handle;
};

#endif // _PAL_THREAD_COMMON_LINUX_H
#endif // __linux__