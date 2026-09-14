
/**
 * PAL - Prime Abstraction Layer (PAL)
 * A cross platform abstraction layer over graphics and windowing APIs
 * -------------------------------------------------------------------
 *
 * Copyright (C) 2025-2026 Nicholas Agbo <agbonicholas04@gmail.com>
 *
 * This software is provided 'as-is', without any express or implied
 * warranty.  In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 *
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 *
 * 3. This notice may not be removed or altered from any source distribution.
 */

#define _GNU_SOURCE
#include "shared.h"
#include "thread/thread_platform.h"

#if PLATFORM_POSIX
#include <sched.h>
#include <sys/resource.h>
#include <unistd.h>
#include <errno.h>
#include <pthread.h>

#define TO_PAL_HANDLE(type, val) ((type*)(uintptr_t)(val))
#define FROM_PAL_HANDLE(type, handle) ((type)(uintptr_t)(handle))

PalResult platformCreateThread(
    const PalThreadCreateInfo* info,
    PalThread** thread)
{
    pthread_t pthread;
    if (info->stackSize == 0) {
        if (pthread_create(&pthread, nullptr, info->entry, info->arg) != 0) {
            return palMakeResult(
                PAL_RESULT_CODE_PLATFORM_FAILURE, 
                PAL_RESULT_SOURCE_POSIX, 
                errno);
        }

    } else {
        pthread_attr_t attr;
        pthread_attr_init(&attr);
        pthread_attr_setstacksize(&attr, info->stackSize);

        if (pthread_create(&pthread, nullptr, info->entry, info->arg) != 0) {
            return palMakeResult(
                PAL_RESULT_CODE_PLATFORM_FAILURE, 
                PAL_RESULT_SOURCE_POSIX, 
                errno);
        }

        pthread_attr_destroy(&attr);
    }

    *thread = TO_PAL_HANDLE(PalThread, thread);

    return PAL_RESULT_SUCCESS;
}

PalResult platformJoinThread(
    PalThread* thread,
    void** retval)
{
    int ret = 0;
    pthread_t pthread = FROM_PAL_HANDLE(pthread_t, thread);
    if (retval) {
        ret = pthread_join(pthread, retval);

    } else {
        ret = pthread_join(pthread, nullptr);
    }

    if (ret != 0) {
        return palMakeResult(
            PAL_RESULT_CODE_PLATFORM_FAILURE, 
            PAL_RESULT_SOURCE_POSIX, 
            errno);
    }

    return PAL_RESULT_SUCCESS;
}

void platformDetachThread(PalThread* thread)
{
    pthread_detach(FROM_PAL_HANDLE(pthread_t, thread));
}

void platformSleep(uint64_t milliseconds)
{
    usleep(milliseconds * 1000);
}

void platformYield()
{
    sched_yield();
}

PalThread* platformGetCurrentThread()
{
    return TO_PAL_HANDLE(PalThread, pthread_self());
}

PalThreadFeatures platformGetThreadFeatures()
{
    PalThreadFeatures features = 0;
    features |= PAL_THREAD_FEATURE_STACK_SIZE;
    features |= PAL_THREAD_FEATURE_PRIORITY;

#ifdef __linux__
    features |= PAL_THREAD_FEATURE_AFFINITY;
    features |= PAL_THREAD_FEATURE_NAME;
#endif // __linux__

    return features;
}

PalThreadPriority platformGetThreadPriority(PalThread* thread)
{
    int policy;
    struct sched_param param;

    pthread_t pthread = FROM_PAL_HANDLE(pthread_t, thread);
    if (pthread_getschedparam(pthread, &policy, &param) != 0) {
        return 0;
    }

    if (policy == SCHED_OTHER || policy == SCHED_RR) {
        if (param.sched_priority == 10) {
            return PAL_THREAD_PRIORITY_LOW;

        } else if (param.sched_priority == 0) {
            return PAL_THREAD_PRIORITY_NORMAL;
        }
    }

    return PAL_THREAD_PRIORITY_NORMAL;
}

uint64_t platformGetThreadAffinity(PalThread* thread)
{
#ifdef __linux__
    cpu_set_t cpuset;
    CPU_ZERO(&cpuset);

    pthread_t pthread = FROM_PAL_HANDLE(pthread_t, thread);
    if (pthread_getaffinity_np(pthread, sizeof(cpuset), &cpuset) != 0) {
        return 0;
    }

    uint64_t mask = 0;
    for (int i = 0; i < 64; ++i) {
        if (CPU_ISSET(i, &cpuset)) {
            mask |= (1ULL << 1);
        }
    }
    return mask;
#endif // __linux__

    return 0;
}

void platformGetThreadName(
    PalThread* thread,
    uint64_t bufferSize,
    uint64_t* size,
    char* buffer)
{
#ifdef __linux__
    if (buffer && bufferSize > 0) {
        pthread_t pthread = FROM_PAL_HANDLE(pthread_t, thread);
        pthread_getname_np(pthread, buffer, bufferSize);
    }
#endif // __linux__
}

PalResult platformSetThreadPriority(
    PalThread* thread,
    PalThreadPriority priority)
{
    switch (priority) {
    case PAL_THREAD_PRIORITY_LOW: {
        setpriority(PRIO_PROCESS, 0, 10);
        break;
    }

    case PAL_THREAD_PRIORITY_NORMAL: {
        setpriority(PRIO_PROCESS, 0, 0);
        break;
    }

    case PAL_THREAD_PRIORITY_HIGH: {
        struct sched_param param;
        param.sched_priority = 10;

        pthread_t pthread = FROM_PAL_HANDLE(pthread_t, thread);
        int ret = pthread_setschedparam(pthread, SCHED_FIFO, &param);
        if (ret == EPERM) {
            return palMakeResult(
                PAL_RESULT_CODE_INVALID_OPERATION,
                PAL_RESULT_SOURCE_POSIX,
                errno);
        }
    }
    }

    return PAL_RESULT_SUCCESS;
}

PalResult platformSetThreadAffinity(
    PalThread* thread,
    uint64_t mask)
{
    #ifdef __linux__
    cpu_set_t cpuset;
    CPU_ZERO(&cpuset);

    for (int i = 0; i < 64; ++i) {
        if (mask & (1ULL << i)) {
            CPU_SET(i, &cpuset);
        }
    }

    pthread_t pthread = FROM_PAL_HANDLE(pthread_t, thread);
    int ret = pthread_setaffinity_np(pthread, sizeof(cpuset), &cpuset);
    if (ret != 0) {
        return palMakeResult(
            PAL_RESULT_CODE_INVALID_HANDLE, 
            PAL_RESULT_SOURCE_POSIX, 
            errno);
    }

    return PAL_RESULT_SUCCESS;
#endif // __linux__

    return PAL_RESULT_CODE_FEATURE_NOT_SUPPORTED;
}

PalResult platformSetThreadName(
    PalThread* thread,
    const char* name)
{
    #ifdef __linux__
    pthread_t pthread = FROM_PAL_HANDLE(pthread_t, thread);
    int ret = pthread_setname_np(pthread, name);
    if (ret != 0) {
        return palMakeResult(
            PAL_RESULT_CODE_INVALID_HANDLE,
            PAL_RESULT_SOURCE_POSIX, 
            errno);
    }

    return PAL_RESULT_SUCCESS;
#endif // __linux__

    return PAL_RESULT_CODE_FEATURE_NOT_SUPPORTED;
}

#endif // PLATFORM_POSIX