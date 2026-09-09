
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
#include "core/core_platform.h"

#if PLATFORM_POSIX_
#include <pthread.h>

static pthread_once_t s_TLSCreation = PTHREAD_ONCE_INIT;
static pthread_key_t s_TLSID = 0;

static void destroyTLSData(void* data)
{
    LogTLSData* tlsData = data;
    if (tlsData) {
        palFree(nullptr, tlsData);
    }
}

static void createTLSID(void)
{
    if (pthread_key_create(&s_TLSID, destroyTLSData) != 0) {
        return;
    }
}

void corePlatformCreateLogTLS(void)
{
    pthread_once(&s_TLSCreation, createTLSID);
}

LogTLSData* corePlatformGetLogTLSData(void)
{
    return pthread_getspecific(s_TLSID);
}

void corePlatformSetLogTLSData(LogTLSData* data)
{
    pthread_setspecific(s_TLSID, data);
}

#endif // PLATFORM_POSIX_