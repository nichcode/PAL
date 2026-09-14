
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
#include "thread/thread_platform.h"

#if PLATFORM_POSIX
#include <pthread.h>

PalTLSId platformCreateTLS(PaTlsDestructorFn destructor)
{
    pthread_key_t key;
    if (pthread_key_create(&key, destructor) != 0) {
        return 0;
    }

    return (PalTLSId)key;
}

void platformDestroyTLS(PalTLSId Tls)
{
    pthread_key_delete((pthread_key_t)Tls);
}

void* platformGetTLS(PalTLSId Tls)
{
    return pthread_getspecific((pthread_key_t)Tls);
}

void platformSetTLS(
    PalTLSId Tls,
    void* data)
{
    pthread_setspecific((pthread_key_t)Tls, data);
}

#endif // PLATFORM_POSIX