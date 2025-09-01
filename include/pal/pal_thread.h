
#ifndef _PAL_THREAD_H
#define _PAL_THREAD_H

#include "pal_core.h"

typedef Uint32 PalTlsId;
typedef void (*PaTlsDestructorFn)(void*);

typedef struct PalThread PalThread;
typedef void* (*PalThreadFn)(void*);

typedef enum {
    PAL_THREAD_FEATURE_STACK_SIZE = PAL_BIT(0),
    PAL_THREAD_FEATURE_DETACHED = PAL_BIT(1),
    PAL_THREAD_FEATURE_PRIORITY = PAL_BIT(2),
    PAL_THREAD_FEATURE_AFFINITY = PAL_BIT(3),
    PAL_THREAD_FEATURE_NAME = PAL_BIT(4)
} PalThreadFeatures;

typedef enum {
    PAL_THREAD_PRIORITY_LOW,
    PAL_THREAD_PRIORITY_MEDIUM,
    PAL_THREAD_PRIORITY_HIGH
} PalThreadPriority;

typedef struct {
    Uint64 stackSize;
    PalThreadFn entry;
    void* arg;
    const PalAllocator* allocator;
} PalThreadCreateInfo;

_PAPI PalResult _PCALL palCreateThread(
    const PalThreadCreateInfo* info,
    PalThread** outThread);

_PAPI void _PCALL palJoinThread(PalThread* thread);
_PAPI void _PCALL palDetachThread(PalThread* thread);
_PAPI void _PCALL palSleep(Uint64 milliseconds);

_PAPI PalThread* _PCALL palGetCurrentThread();
_PAPI PalThreadFeatures _PCALL palGetThreadFeatures();

_PAPI PalTlsId _PCALL palCreateTls(PaTlsDestructorFn destructor);
_PAPI void _PCALL palDestroyTls(PalTlsId id);
_PAPI void* _PCALL palGetTls(PalTlsId id);
_PAPI void _PCALL palSetTls(PalTlsId id, void* data);

#endif // _PAL_THREAD_H