
#ifndef _PAL_THREAD_H
#define _PAL_THREAD_H

#include "pal_core.h"

typedef Uint32 PalTlsId;
typedef struct PalThread PalThread;
typedef struct PalMutex PalMutex;
typedef struct PalCondition PalCondition;

typedef void* (*PalThreadFn)(void*);
typedef void (*PaTlsDestructorFn)(void*);

typedef enum {
    PAL_THREAD_FEATURE_STACK_SIZE = PAL_BIT(0),
    PAL_THREAD_FEATURE_DETACHED = PAL_BIT(1),
    PAL_THREAD_FEATURE_PRIORITY = PAL_BIT(2),
    PAL_THREAD_FEATURE_AFFINITY = PAL_BIT(3),
    PAL_THREAD_FEATURE_NAME = PAL_BIT(4)
} PalThreadFeatures;

typedef enum {
    PAL_THREAD_PRIORITY_LOW,
    PAL_THREAD_PRIORITY_NORMAL,
    PAL_THREAD_PRIORITY_HIGH
} PalThreadPriority;

typedef struct {
    Uint64 stackSize;
    PalThreadFn entry;
    void* arg;
} PalThreadCreateInfo;

_PAPI const PalAllocator* _PCALL palGetThreadAllocator();
_PAPI void _PCALL palSetThreadAllocator(const PalAllocator* allocator);

_PAPI PalResult _PCALL palCreateThread(
    const PalThreadCreateInfo* info,
    PalThread** outThread);

_PAPI PalResult _PCALL palJoinThread(
    PalThread* thread, 
    void* retval);

_PAPI void _PCALL palDetachThread(PalThread* thread);
_PAPI void _PCALL palSleep(Uint64 milliseconds);
_PAPI void _PCALL palYield();

_PAPI PalThread* _PCALL palGetCurrentThread();
_PAPI PalThreadFeatures _PCALL palGetThreadFeatures();

_PAPI PalThreadPriority _PCALL palGetThreadPriority(PalThread* thread);
_PAPI Uint64 _PCALL palGetThreadAffinity(PalThread* thread);
_PAPI char* _PCALL palGetThreadName(PalThread* thread);

_PAPI PalResult _PCALL palSetThreadPriority(
    PalThread* thread, 
    PalThreadPriority priority);

_PAPI PalResult _PCALL palSetThreadAffinity(
    PalThread* thread,
    Uint64 mask);

_PAPI PalResult _PCALL palSetThreadName(
    PalThread* thread, 
    const char* name);

_PAPI PalTlsId _PCALL palCreateTls(PaTlsDestructorFn destructor);
_PAPI void _PCALL palDestroyTls(PalTlsId id);
_PAPI void* _PCALL palGetTls(PalTlsId id);
_PAPI void _PCALL palSetTls(PalTlsId id, void* data);

_PAPI PalResult _PCALL palCreateMutex(PalMutex** outMutex);
_PAPI void _PCALL palDestroyMutex(PalMutex* mutex);
_PAPI void _PCALL palLockMutex(PalMutex* mutex);
_PAPI void _PCALL palUnlockMutex(PalMutex* mutex);

_PAPI PalResult _PCALL palCreateCondition(PalCondition** outCondition);
_PAPI void _PCALL palDestroyCondition(PalCondition* condition);

_PAPI PalResult _PCALL palWaitCondition(
    PalCondition* condition,
    PalMutex* mutex);

_PAPI PalResult _PCALL palWaitConditionTimeout(
    PalCondition* condition,
    PalMutex* mutex,
    Uint64 milliseconds);

_PAPI void _PCALL palSignalCondition(PalCondition* condition);
_PAPI void _PCALL palBroadcastCondition(PalCondition* condition);

#endif // _PAL_THREAD_H