
/**

Copyright (C) 2025 Nicholas Agbo

This software is provided 'as-is', without any express or implied
warranty.  In no event will the authors be held liable for any damages
arising from the use of this software.

Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it
freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not
   claim that you wrote the original software. If you use this software
   in a product, an acknowledgment in the product documentation would be
   appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be
   misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.

 */

// ==================================================
// Includes
// ==================================================

#define _GNU_SOURCE
#define _POSIX_C_SOURCE 200112L
#include <pthread.h>
#include <errno.h>
#include <sched.h>
#include <unistd.h>
#include <sys/resource.h>
#include <time.h>

#include "pal/pal_thread.h"

// ==================================================
// Typedefs, enums and structs
// ==================================================

struct PalMutex {
    const PalAllocator* allocator;
    pthread_mutex_t handle;
};

struct PalCondVar {
    const PalAllocator* allocator;
    pthread_cond_t handle;
};

// ==================================================
// Internal API
// ==================================================


// ==================================================
// Public API
// ==================================================

// ==================================================
// Thread
// ==================================================

PalResult PAL_CALL palCreateThread(
    const PalThreadCreateInfo* info,
    PalThread** outThread)
{
    if (!info || !outThread) {
        return PAL_RESULT_NULL_POINTER;
    }

    if (info->allocator) {
        if (!info->allocator->allocate && !info->allocator->free) {
            return PAL_RESULT_INVALID_ALLOCATOR;
        }
    }

    pthread_t thread;
    if (info->stackSize == 0) {
        if (pthread_create(&thread, nullptr, info->entry, info->arg) != 0) {
            return PAL_RESULT_PLATFORM_FAILURE;
        }
    } else {
        pthread_attr_t attr;
        pthread_attr_init(&attr);
        pthread_attr_setstacksize(&attr, info->stackSize);

        if (pthread_create(&thread, nullptr, info->entry, info->arg) != 0) {
            return PAL_RESULT_PLATFORM_FAILURE;
        }
        pthread_attr_destroy(&attr);
    }
    
    *outThread = (PalThread*)thread;
    return PAL_RESULT_SUCCESS;
}

PalResult PAL_CALL palJoinThread(
    PalThread* thread,
    void* retval)
{
    if (!thread) {
        return PAL_RESULT_NULL_POINTER;
    }

    int ret = 0;
    if (retval) {
        ret = pthread_join((pthread_t)thread, &retval);
    } else {
        ret = pthread_join((pthread_t)thread, nullptr);
    }

    if (ret == 0) {
        return PAL_RESULT_SUCCESS;
    } else {
        return PAL_RESULT_PLATFORM_FAILURE;
    }
}

void PAL_CALL palDetachThread(PalThread* thread)
{
    if (thread) {
        pthread_detach((pthread_t)thread);
    }
}

void PAL_CALL palSleep(Uint64 milliseconds)
{
    usleep(milliseconds * 1000);
}

void PAL_CALL palYield()
{
    sched_yield();
}

PalThread* PAL_CALL palGetCurrentThread()
{
    return (PalThread*)pthread_self();
}

PalThreadFeatures PAL_CALL palGetThreadFeatures()
{
    PalThreadFeatures features = 0;
    features |= PAL_THREAD_FEATURE_STACK_SIZE;
    features |= PAL_THREAD_FEATURE_PRIORITY;
    features |= PAL_THREAD_FEATURE_AFFINITY;
    features |= PAL_THREAD_FEATURE_NAME;
    return features;
}

PalThreadPriority PAL_CALL palGetThreadPriority(PalThread* thread)
{
    if (!thread) {
        return 0;
    }

    int policy;
    struct sched_param param;
    if (pthread_getschedparam((pthread_t)thread, &policy, &param) != 0) {
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

Uint64 PAL_CALL palGetThreadAffinity(PalThread* thread)
{
    if (!thread) {
        return 0;
    }

    cpu_set_t cpuset;
    CPU_ZERO(&cpuset);
    if (pthread_getaffinity_np((pthread_t)thread, sizeof(cpuset), &cpuset) != 0) {
        return 0;
    }

    Uint64 mask = 0;
    for (int i = 0; i < 64; ++i) {
        if (CPU_ISSET(i, &cpuset)) {
            mask |= (1ULL << 1);
        }
    }
    return mask;
}

PalResult PAL_CALL palGetThreadName(
    PalThread* thread,
    Uint64 bufferSize,
    Uint64* outSize,
    char* outBuffer)
{
    if (!thread) {
        return PAL_RESULT_NULL_POINTER;
    }

    // see if user provided a buffer and write to it
    if (outBuffer && bufferSize > 0) {
        if (pthread_getname_np((pthread_t)thread, outBuffer, bufferSize) != 0) {
            return PAL_RESULT_INVALID_THREAD;
        }
    }

    return PAL_RESULT_SUCCESS;
}

PalResult PAL_CALL palSetThreadPriority(
    PalThread* thread,
    PalThreadPriority priority)
{
    if (!thread) {
        return PAL_RESULT_NULL_POINTER;
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
            int ret = pthread_setschedparam((pthread_t)thread, SCHED_FIFO, &param);
            if (ret == EPERM) {
                return PAL_RESULT_ACCESS_DENIED;
            }
        }
    }

    return PAL_RESULT_SUCCESS;
}

PalResult PAL_CALL palSetThreadAffinity(
    PalThread* thread,
    Uint64 mask)
{
    if (!thread) {
        return PAL_RESULT_NULL_POINTER;
    }

    cpu_set_t cpuset;
    CPU_ZERO(&cpuset);
    for (int i = 0; i < 64; ++i) {
        // check for all bits to see which CPUs will be used
        if (mask & (1ULL << i)) {
            CPU_SET(i, &cpuset);
        }
    }

    int ret = pthread_setaffinity_np((pthread_t)thread, sizeof(cpuset), &cpuset);
    if (ret == 0) {
        return  PAL_RESULT_SUCCESS;
    } else {
        return PAL_RESULT_INVALID_THREAD;
    }
}

PalResult PAL_CALL palSetThreadName(
    PalThread* thread,
    const char* name)
{
    if (!thread || !name) {
        return PAL_RESULT_NULL_POINTER;
    }

    int ret = pthread_setname_np((pthread_t)thread, name);
    if (ret == 0) {
        return PAL_RESULT_SUCCESS;
    } else {
        return PAL_RESULT_INVALID_THREAD;
    }
}

// ==================================================
// TLS
// ==================================================

PalTLSId PAL_CALL palCreateTLS(PaTlsDestructorFn destructor)
{
    pthread_key_t key;
    if (pthread_key_create(&key, destructor) != 0) {
        return 0;
    }
    return (PalTLSId)key;
}

void PAL_CALL palDestroyTLS(PalTLSId id)
{
    pthread_key_delete((pthread_key_t)id);
}

void* PAL_CALL palGetTLS(PalTLSId id)
{
    return pthread_getspecific((pthread_key_t)id);
}

void PAL_CALL palSetTLS(
    PalTLSId id,
    void* data)
{
    pthread_setspecific((pthread_key_t)id, data);
}

// ==================================================
// Mutex
// ==================================================

PalResult PAL_CALL palCreateMutex(
    const PalAllocator* allocator,
    PalMutex** outMutex)
{
    if (!outMutex) {
        return PAL_RESULT_NULL_POINTER;
    }

    if (allocator) {
        if (!allocator->allocate && !allocator->free) {
            return PAL_RESULT_INVALID_ALLOCATOR;
        }
    }

    PalMutex* mutex = palAllocate(allocator, sizeof(PalMutex), 0);
    if (!mutex) {
        return PAL_RESULT_OUT_OF_MEMORY;
    }

    pthread_mutex_init(&mutex->handle, nullptr);
    mutex->allocator = allocator;
    *outMutex = mutex;
    return PAL_RESULT_SUCCESS;
}

void PAL_CALL palDestroyMutex(PalMutex* mutex)
{
    if (mutex) {
        pthread_mutex_destroy(&mutex->handle);
        palFree(mutex->allocator, mutex);
    }
}

void PAL_CALL palLockMutex(PalMutex* mutex)
{
    if (mutex) {
        pthread_mutex_lock(&mutex->handle);
    }
}

void PAL_CALL palUnlockMutex(PalMutex* mutex)
{
    if (mutex) {
        pthread_mutex_unlock(&mutex->handle);
    }
}

// ==================================================
// Condition Variable
// ==================================================

PalResult PAL_CALL palCreateCondVar(
    const PalAllocator* allocator,
    PalCondVar** outCondVar)
{
    if (!outCondVar) {
        return PAL_RESULT_NULL_POINTER;
    }

    if (allocator) {
        if (!allocator->allocate && !allocator->free) {
            return PAL_RESULT_INVALID_ALLOCATOR;
        }
    }

    PalCondVar* condVar = palAllocate(allocator, sizeof(PalCondVar), 0);
    if (!condVar) {
        return PAL_RESULT_OUT_OF_MEMORY;
    }

    pthread_cond_init(&condVar->handle, nullptr);
    condVar->allocator = allocator;
    *outCondVar = condVar;
    return PAL_RESULT_SUCCESS;
}

void PAL_CALL palDestroyCondVar(PalCondVar* condVar)
{
    if (condVar) {
        pthread_cond_destroy(&condVar->handle);
        palFree(condVar->allocator, condVar);
    }
}

PalResult PAL_CALL palWaitCondVar(
    PalCondVar* condVar,
    PalMutex* mutex)
{
    if (!condVar || !mutex) {
        return PAL_RESULT_NULL_POINTER;
    }

    int ret = pthread_cond_wait(&condVar->handle, &mutex->handle);
    if (ret == 0) {
        return PAL_RESULT_SUCCESS;
    } else if (ret == ETIMEDOUT) {
        return PAL_RESULT_TIMEOUT;
    } else {
        return PAL_RESULT_PLATFORM_FAILURE;
    }
}

PalResult PAL_CALL palWaitCondVarTimeout(
    PalCondVar* condVar,
    PalMutex* mutex,
    Uint64 milliseconds)
{
    if (!condVar || !mutex) {
        return PAL_RESULT_NULL_POINTER;
    }

    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    ts.tv_sec += milliseconds / 1000;
    ts.tv_nsec += (milliseconds % 1000) * 1000000;

    if (ts.tv_nsec >= 1000000000) {
        ts.tv_sec++;
        ts.tv_nsec -= 1000000000;
    }

    if (pthread_cond_timedwait(&condVar->handle, &mutex->handle, &ts) != 0) {
        return PAL_RESULT_TIMEOUT;
    }
    return PAL_RESULT_SUCCESS;
}

void PAL_CALL palSignalCondVar(PalCondVar* condVar)
{
    if (condVar) {
        pthread_cond_signal(&condVar->handle);
    }
}

void PAL_CALL palBroadcastCondVar(PalCondVar* condVar)
{
    if (condVar) {
        pthread_cond_broadcast(&condVar->handle);
    }
}