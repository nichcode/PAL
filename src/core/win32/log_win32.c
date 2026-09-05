
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

#ifdef _WIN32
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif // WIN32_LEAN_AND_MEAN

#include "core/log_priv.h"
#include <windows.h>

static volatile LONG s_TLSID = 0;

// Create the TLS using atomic operations to avoid thread race
void createLogTLS_(void)
{
    DWORD TLSIndex = FlsAlloc(destroyTLSData_);
    LONG prev = InterlockedCompareExchange(
        (volatile LONG*)&s_TLSID,
        (LONG)TLSIndex,
        0);

    if (prev != 0) {
        FlsFree(TLSIndex);
    }
}

LogTLSData* getLogTLSData_(void)
{
    return FlsGetValue((DWORD)s_TLSID);
}

void setLogTLSData_(LogTLSData* data)
{
    FlsSetValue(s_TLSID, data);
}

#endif // _WIN32