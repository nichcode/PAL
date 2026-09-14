
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

PalTLSId PAL_CALL palCreateTLS(PaTlsDestructorFn destructor)
{
    return platformCreateTLS(destructor);
}

void PAL_CALL palDestroyTLS(PalTLSId Tls)
{
    ASSERT(Tls != 0, "The specified TLS is invalid");

    platformDestroyTLS(Tls);
}

void* PAL_CALL palGetTLS(PalTLSId Tls)
{
    ASSERT(Tls != 0, "The specified TLS is invalid");

    return platformGetTLS(Tls);
}

void PAL_CALL palSetTLS(
    PalTLSId Tls,
    void* data)
{
    ASSERT(Tls != 0, "The specified TLS is invalid");
    ASSERT(data != nullptr, "The specified data is null");

    platformSetTLS(Tls, data);
}