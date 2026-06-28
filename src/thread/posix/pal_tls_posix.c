
/**
 PAL - Prime Abstraction Layer
 Copyright (C) 2025
 Licensed under the Zlib license. See LICENSE file in root.
 */

#include "pal_posix.h"

#if _PAL_ON_POSIX
#include "pal/pal_thread.h"
#include <pthread.h>

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

#endif // _PAL_ON_POSIX