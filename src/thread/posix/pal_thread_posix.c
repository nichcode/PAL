
/**
 PAL - Prime Abstraction Layer
 Copyright (C) 2025
 Licensed under the Zlib license. See LICENSE file in root.
 */

#include "pal_posix.h"
#ifdef __linux__
#define _GNU_SOURCE
#endif // __linux__

#if _PAL_ON_POSIX
#include "pal_thread_posix.h"
#include <sys/resource.h>
#include <unistd.h>
#include <sched.h>

#define TO_PAL_HANDLE(type, val) ((type*)(uintptr_t)(val))
#define FROM_PAL_HANDLE(type, handle) ((type)(uintptr_t)(handle))

PalResult PAL_CALL palCreateThread(
    const PalThreadCreateInfo* info,
    PalThread** outThread)
{
    if (!info || !outThread) {
        return PAL_RESULT_CODE_INVALID_ARGUMENT;
    }

    if (info->allocator) {
        if (!info->allocator->allocate && !info->allocator->free) {
            return PAL_RESULT_CODE_INVALID_ARGUMENT;
        }
    }

    pthread_t thread;
    if (info->stackSize == 0) {
        if (pthread_create(&thread, nullptr, info->entry, info->arg) != 0) {
            return palMakeResult(PAL_RESULT_CODE_PLATFORM_FAILURE, PAL_RESULT_SOURCE_POSIX, errno);
        }

    } else {
        pthread_attr_t attr;
        pthread_attr_init(&attr);
        pthread_attr_setstacksize(&attr, info->stackSize);

        if (pthread_create(&thread, nullptr, info->entry, info->arg) != 0) {
            return palMakeResult(PAL_RESULT_CODE_PLATFORM_FAILURE, PAL_RESULT_SOURCE_POSIX, errno);
        }
        pthread_attr_destroy(&attr);
    }

    *outThread = TO_PAL_HANDLE(PalThread, thread);
    return PAL_RESULT_SUCCESS;
}

PalResult PAL_CALL palJoinThread(
    PalThread* thread,
    void** retval)
{
    if (!thread) {
        return PAL_RESULT_CODE_INVALID_ARGUMENT;
    }

    int ret = 0;
    pthread_t _thread = FROM_PAL_HANDLE(pthread_t, thread);
    if (retval) {
        ret = pthread_join(_thread, retval);
    } else {
        ret = pthread_join(_thread, nullptr);
    }

    if (ret == 0) {
        return PAL_RESULT_SUCCESS;

    } else {
        return palMakeResult(PAL_RESULT_CODE_PLATFORM_FAILURE, PAL_RESULT_SOURCE_POSIX, errno);
    }
}

void PAL_CALL palDetachThread(PalThread* thread)
{
    if (thread) {
        pthread_detach(FROM_PAL_HANDLE(pthread_t, thread));
    }
}

void PAL_CALL palSleep(uint64_t milliseconds)
{
    usleep(milliseconds * 1000);
}

void PAL_CALL palYield()
{
    sched_yield();
}

PalThread* PAL_CALL palGetCurrentThread()
{
    return TO_PAL_HANDLE(PalThread, pthread_self());
}

PalThreadFeatures PAL_CALL palGetThreadFeatures()
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

PalThreadPriority PAL_CALL palGetThreadPriority(PalThread* thread)
{
    if (!thread) {
        return 0;
    }

    int policy;
    struct sched_param param;
    pthread_t _thread = FROM_PAL_HANDLE(pthread_t, thread);
    if (pthread_getschedparam(_thread, &policy, &param) != 0) {
        return 0;
    }

    // set priority
    if (policy == SCHED_OTHER || policy == SCHED_RR) {
        if (param.sched_priority == 10) {
            return PAL_THREAD_PRIORITY_LOW;

        } else if (param.sched_priority == 0) {
            return PAL_THREAD_PRIORITY_NORMAL;
        }

    } else {
        return PAL_THREAD_PRIORITY_HIGH;
    }
}

uint64_t PAL_CALL palGetThreadAffinity(PalThread* thread)
{
    if (!thread) {
        return 0;
    }

#ifdef __linux__
    cpu_set_t cpuset;
    CPU_ZERO(&cpuset);
    pthread_t _thread = FROM_PAL_HANDLE(pthread_t, thread);
    if (pthread_getaffinity_np(_thread, sizeof(cpuset), &cpuset) != 0) {
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

PalResult PAL_CALL palGetThreadName(
    PalThread* thread,
    uint64_t bufferSize,
    uint64_t* outSize,
    char* outBuffer)
{
    if (!thread) {
        return PAL_RESULT_CODE_INVALID_ARGUMENT;
    }

#ifdef __linux__
    // see if user provided a buffer and write to it
    if (outBuffer && bufferSize > 0) {
        pthread_t _thread = FROM_PAL_HANDLE(pthread_t, thread);
        if (pthread_getname_np(_thread, outBuffer, bufferSize) != 0) {
            return palMakeResult(PAL_RESULT_CODE_INVALID_HANDLE, PAL_RESULT_SOURCE_POSIX, errno);
        }
    }

    return PAL_RESULT_SUCCESS;
#endif // __linux__

    return PAL_RESULT_CODE_FEATURE_NOT_SUPPORTED;
}

PalResult PAL_CALL palSetThreadPriority(
    PalThread* thread,
    PalThreadPriority priority)
{
    if (!thread) {
        return PAL_RESULT_CODE_INVALID_ARGUMENT;
    }

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
            pthread_t _thread = FROM_PAL_HANDLE(pthread_t, thread);
            int ret = pthread_setschedparam(_thread, SCHED_FIFO, &param);
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

PalResult PAL_CALL palSetThreadAffinity(
    PalThread* thread,
    uint64_t mask)
{
    if (!thread) {
        return PAL_RESULT_CODE_INVALID_ARGUMENT;
    }

#ifdef __linux__
    cpu_set_t cpuset;
    CPU_ZERO(&cpuset);
    for (int i = 0; i < 64; ++i) {
        // check for all bits to see which CPUs will be used
        if (mask & (1ULL << i)) {
            CPU_SET(i, &cpuset);
        }
    }

    pthread_t _thread = FROM_PAL_HANDLE(pthread_t, thread);
    int ret = pthread_setaffinity_np(_thread, sizeof(cpuset), &cpuset);
    if (ret == 0) {
        return PAL_RESULT_SUCCESS;

    } else {
        return palMakeResult(PAL_RESULT_CODE_INVALID_HANDLE, PAL_RESULT_SOURCE_POSIX, errno);
    }
#endif // __linux__

    return PAL_RESULT_CODE_FEATURE_NOT_SUPPORTED;
}

PalResult PAL_CALL palSetThreadName(
    PalThread* thread,
    const char* name)
{
    if (!thread || !name) {
        return PAL_RESULT_CODE_INVALID_ARGUMENT;
    }

#ifdef __linux__
    pthread_t _thread = FROM_PAL_HANDLE(pthread_t, thread);
    int ret = pthread_setname_np(_thread, name);
    if (ret == 0) {
        return PAL_RESULT_SUCCESS;

    } else {
        return palMakeResult(PAL_RESULT_CODE_INVALID_HANDLE, PAL_RESULT_SOURCE_POSIX, errno);
    }
#endif // __linux__
    
    return PAL_RESULT_CODE_FEATURE_NOT_SUPPORTED;
}

#endif // _PAL_ON_POSIX