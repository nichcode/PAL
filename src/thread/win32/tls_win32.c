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

#include "thread/platform.h"
#include "pal2/thread/tls.h"

#ifdef _WIN32
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif // WIN32_LEAN_AND_MEAN

#include <windows.h>

PalTLSId platformCreateTLS(PaTlsDestructorFn destructor)
{
    DWORD tlsid = FlsAlloc((PFLS_CALLBACK_FUNCTION)(void*)destructor);
    if (tlsid == FLS_OUT_OF_INDEXES) {
        return 0;
    }

    // Offset by +1 so FLS index 0 does not collide with invalid PalTLSId (0).
    return (PalTLSId)(tlsid + 1);
}

void platformDestroyTLS(PalTLSId Tls)
{
    FlsFree((DWORD)(Tls - 1));
}

void* platformGetTLS(PalTLSId Tls)
{
    return FlsGetValue((DWORD)(Tls - 1));
}

void platformSetTLS(
    PalTLSId Tls,
    void* data)
{
    FlsSetValue((DWORD)(Tls - 1), data);
}

#endif // _WIN32