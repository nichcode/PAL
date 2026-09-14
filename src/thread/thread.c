
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

#include "shared.h"
#include "thread_platform.h"

PalResult PAL_CALL palCreateThread(
    const PalThreadCreateInfo* info,
    PalThread** thread)
{
    if (!info || !thread) {
        return PAL_RESULT_CODE_INVALID_ARGUMENT;
    }

    if (info->allocator) {
        if (!info->allocator->allocate && !info->allocator->free) {
            return PAL_RESULT_CODE_INVALID_ARGUMENT;
        }
    }

    return platformCreateThread(info, thread);
}

PalResult PAL_CALL palJoinThread(
    PalThread* thread,
    void** retval)
{
    ASSERT(thread != nullptr, "The specified thread is null");

    return platformJoinThread(thread, retval);
}

void PAL_CALL palDetachThread(PalThread* thread)
{
    ASSERT(thread != nullptr, "The specified thread is null");

    platformDetachThread(thread);
}

void PAL_CALL palSleep(uint64_t milliseconds)
{
    platformSleep(milliseconds);
}

void PAL_CALL palYield()
{
    platformYield();
}

PalThread* PAL_CALL palGetCurrentThread()
{
    return platformGetCurrentThread();
}

PalThreadFeatures PAL_CALL palGetThreadFeatures()
{
    return platformGetThreadFeatures();
}

PalThreadPriority PAL_CALL palGetThreadPriority(PalThread* thread)
{
    ASSERT(thread != nullptr, "The specified thread is null");

    return platformGetThreadPriority(thread);
}

uint64_t PAL_CALL palGetThreadAffinity(PalThread* thread)
{
    ASSERT(thread != nullptr, "The specified thread is null");

    return platformGetThreadAffinity(thread);
}

void PAL_CALL palGetThreadName(
    PalThread* thread,
    uint64_t bufferSize,
    uint64_t* size,
    char* buffer)
{
    ASSERT(thread != nullptr, "The specified thread is null");
    ASSERT(
        buffer != nullptr && bufferSize != 0, 
        "The specified thread is null");

    platformGetThreadName(thread, bufferSize, size, buffer);
}

PalResult PAL_CALL palSetThreadPriority(
    PalThread* thread,
    PalThreadPriority priority)
{
    ASSERT(thread != nullptr, "The specified thread is null");

    return platformSetThreadPriority(thread, priority);
}

PalResult PAL_CALL palSetThreadAffinity(
    PalThread* thread,
    uint64_t mask)
{
    ASSERT(thread != nullptr, "The specified thread is null");

    return platformSetThreadAffinity(thread, mask);
}

PalResult PAL_CALL palSetThreadName(
    PalThread* thread,
    const char* name)
{
    ASSERT(thread != nullptr, "The specified thread is null");

    return platformSetThreadName(thread, name);
}
