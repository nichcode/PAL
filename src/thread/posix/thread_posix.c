/**
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
#include "thread/platform.h"
#include "pal2/thread/thread.h"

#if PLATFORM_POSIX
#include <errno.h>
#include <pthread.h>
#include <sched.h>
#include <string.h>
#include <sys/resource.h>
#include <time.h>
#include <unistd.h>

#define TO_PAL_HANDLE(type, val) ((type*)(uintptr_t)(val))
#define FROM_PAL_HANDLE(type, handle) ((type)(uintptr_t)(handle))

PalResult platformCreateThread(
    const PalThreadCreateInfo* info,
    PalThread** thread)
{
    pthread_t pthread;
    int ret = 0;

    if (info->stackSize == 0) {
        ret = pthread_create(&pthread, nullptr, info->entry, info->arg);
    } else {
        pthread_attr_t attr;
        ret = pthread_attr_init(&attr);
        if (ret != 0) {
            return palMakeResult(
                PAL_RESULT_CODE_PLATFORM_FAILURE,
                PAL_RESULT_SOURCE_POSIX,
                (uint32_t)ret);
        }

        ret = pthread_attr_setstacksize(&attr, info->stackSize);
        if (ret != 0) {
            pthread_attr_destroy(&attr);
            return palMakeResult(
                PAL_RESULT_CODE_INVALID_ARGUMENT,
                PAL_RESULT_SOURCE_POSIX,
                (uint32_t)ret);
        }

        ret = pthread_create(&pthread, &attr, info->entry, info->arg);
        pthread_attr_destroy(&attr);
    }

    if (ret != 0) {
        return palMakeResult(
            PAL_RESULT_CODE_PLATFORM_FAILURE,
            PAL_RESULT_SOURCE_POSIX,
            (uint32_t)ret);
    }

    *thread = TO_PAL_HANDLE(PalThread, pthread);
    return PAL_RESULT_SUCCESS;
}

PalResult platformJoinThread(
    PalThread* thread,
    void** retval)
{
    pthread_t pthread = FROM_PAL_HANDLE(pthread_t, thread);
    int ret = pthread_join(pthread, retval);
    if (ret != 0) {
        return palMakeResult(
            PAL_RESULT_CODE_PLATFORM_FAILURE,
            PAL_RESULT_SOURCE_POSIX,
            (uint32_t)ret);
    }

    return PAL_RESULT_SUCCESS;
}

void platformDetachThread(PalThread* thread)
{
    pthread_detach(FROM_PAL_HANDLE(pthread_t, thread));
}

void platformSleep(uint64_t milliseconds)
{
    struct timespec req;
    req.tv_sec = (time_t)(milliseconds / 1000ULL);
    req.tv_nsec = (long)(milliseconds % 1000ULL) * 1000000L;

    struct timespec rem;
    while (nanosleep(&req, &rem) == -1 && errno == EINTR) {
        req = rem;
    }
}

void platformYield(void)
{
    sched_yield();
}

PalThread* platformGetCurrentThread(void)
{
    return TO_PAL_HANDLE(PalThread, pthread_self());
}

PalThreadFeatures platformGetThreadFeatures(void)
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
        return PAL_THREAD_PRIORITY_NORMAL;
    }

    if (policy == SCHED_FIFO || policy == SCHED_RR) {
        if (param.sched_priority > 0) {
            return PAL_THREAD_PRIORITY_HIGH;
        }
    }

    if (pthread_equal(pthread, pthread_self())) {
        errno = 0;
        int niceVal = getpriority(PRIO_PROCESS, 0);
        if (errno == 0 && niceVal > 0) {
            return PAL_THREAD_PRIORITY_LOW;
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
        if (CPU_ISSET((size_t)i, &cpuset)) {
            mask |= (1ULL << i);
        }
    }
    return mask;
#else
    (void)thread;
    return 0;
#endif // __linux__
}

void platformGetThreadName(
    PalThread* thread,
    size_t bufferSize,
    size_t* size,
    char* buffer)
{
#ifdef __linux__
    pthread_t pthread = FROM_PAL_HANDLE(pthread_t, thread);
    char tmp[16] = {0};

    if (pthread_getname_np(pthread, tmp, sizeof(tmp)) != 0) {
        if (size) {
            *size = 0;
        }
        if (buffer && bufferSize > 0) {
            buffer[0] = '\0';
        }
        return;
    }

    size_t len = strlen(tmp) + 1;
    if (size) {
        *size = len;
    }

    if (buffer && bufferSize > 0) {
        size_t copyLen = (len < bufferSize) ? (len - 1) : (bufferSize - 1);
        memcpy(buffer, tmp, copyLen);
        buffer[copyLen] = '\0';
    }
#else
    (void)thread;
    if (size) {
        *size = 0;
    }
    if (buffer && bufferSize > 0) {
        buffer[0] = '\0';
    }
#endif // __linux__
}

PalResult platformSetThreadPriority(
    PalThread* thread,
    PalThreadPriority priority)
{
    pthread_t pthread = FROM_PAL_HANDLE(pthread_t, thread);

    switch (priority) {
    case PAL_THREAD_PRIORITY_LOW: {
        struct sched_param param = {0};
        (void)pthread_setschedparam(pthread, SCHED_OTHER, &param);
        if (pthread_equal(pthread, pthread_self())) {
            if (setpriority(PRIO_PROCESS, 0, 10) != 0) {
                return palMakeResult(
                    PAL_RESULT_CODE_INVALID_OPERATION,
                    PAL_RESULT_SOURCE_POSIX,
                    (uint32_t)errno);
            }
        }
        break;
    }

    case PAL_THREAD_PRIORITY_NORMAL: {
        struct sched_param param = {0};
        int ret = pthread_setschedparam(pthread, SCHED_OTHER, &param);
        if (ret != 0) {
            return palMakeResult(
                PAL_RESULT_CODE_INVALID_OPERATION,
                PAL_RESULT_SOURCE_POSIX,
                (uint32_t)ret);
        }
        if (pthread_equal(pthread, pthread_self())) {
            (void)setpriority(PRIO_PROCESS, 0, 0);
        }
        break;
    }

    case PAL_THREAD_PRIORITY_HIGH: {
        struct sched_param param = {0};
        param.sched_priority = 10;

        int ret = pthread_setschedparam(pthread, SCHED_FIFO, &param);
        if (ret != 0) {
            return palMakeResult(
                PAL_RESULT_CODE_INVALID_OPERATION,
                PAL_RESULT_SOURCE_POSIX,
                (uint32_t)ret);
        }
        break;
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
            CPU_SET((size_t)i, &cpuset);
        }
    }

    pthread_t pthread = FROM_PAL_HANDLE(pthread_t, thread);
    int ret = pthread_setaffinity_np(pthread, sizeof(cpuset), &cpuset);
    if (ret != 0) {
        return palMakeResult(
            PAL_RESULT_CODE_INVALID_HANDLE,
            PAL_RESULT_SOURCE_POSIX,
            (uint32_t)ret);
    }

    return PAL_RESULT_SUCCESS;
#else
    (void)thread;
    (void)mask;
    return PAL_RESULT_CODE_FEATURE_NOT_SUPPORTED;
#endif // __linux__
}

PalResult platformSetThreadName(
    PalThread* thread,
    const char* name)
{
#ifdef __linux__
    char truncated[16];
    size_t i = 0;
    while (name[i] != '\0' && i < sizeof(truncated) - 1) {
        truncated[i] = name[i];
        i++;
    }
    truncated[i] = '\0';

    pthread_t pthread = FROM_PAL_HANDLE(pthread_t, thread);
    int ret = pthread_setname_np(pthread, truncated);
    if (ret != 0) {
        return palMakeResult(
            PAL_RESULT_CODE_INVALID_HANDLE,
            PAL_RESULT_SOURCE_POSIX,
            (uint32_t)ret);
    }

    return PAL_RESULT_SUCCESS;
#else
    (void)thread;
    (void)name;
    return PAL_RESULT_CODE_FEATURE_NOT_SUPPORTED;
#endif // __linux__
}

#endif // PLATFORM_POSIX
