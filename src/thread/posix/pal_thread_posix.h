
/**
    PAL - Prime Abstraction Layer
    Copyright (C) 2025
    Licensed under the Zlib license. See LICENSE file in root.
 */

#ifndef _PAL_THREAD_LINUX_H
#define _PAL_THREAD_LINUX_H

#include "pal_platform.h"
#if _PAL_HAS_POSIX

#define _POSIX_C_SOURCE 200112L
#include "pal2/pal_thread.h"
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

#endif // _PAL_HAS_POSIX
#endif // _PAL_THREAD_LINUX_H