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

#include "core/platform.h"
#include "pal2/core/log.h"
#include "pal2/core/memory.h"

#ifdef _WIN32
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif // WIN32_LEAN_AND_MEAN

#include <windows.h>

static volatile LONG s_TLSID = (LONG)FLS_OUT_OF_INDEXES;

static void WINAPI destroyTLSData(void* data)
{
    LogTLSData* tlsData = data;
    if (tlsData) {
        palFree(nullptr, tlsData);
    }
}

void platformCreateLogTLS(void)
{
    if (s_TLSID != (LONG)FLS_OUT_OF_INDEXES) {
        return;
    }

    /** Create the TLS using atomic operations to avoid thread race. */
    DWORD TLSIndex = FlsAlloc(destroyTLSData);
    if (TLSIndex == FLS_OUT_OF_INDEXES) {
        return;
    }

    LONG prev = InterlockedCompareExchange(
        &s_TLSID,
        (LONG)TLSIndex,
        (LONG)FLS_OUT_OF_INDEXES);

    if (prev != (LONG)FLS_OUT_OF_INDEXES) {
        FlsFree(TLSIndex);
    }
}

LogTLSData* platformGetLogTLSData(void)
{
    DWORD index = (DWORD)s_TLSID;
    if (index == FLS_OUT_OF_INDEXES) {
        return nullptr;
    }
    return FlsGetValue(index);
}

void platformSetLogTLSData(LogTLSData* data)
{
    DWORD index = (DWORD)s_TLSID;
    if (index == FLS_OUT_OF_INDEXES) {
        return;
    }
    FlsSetValue(index, data);
}

#endif // _WIN32