
#ifndef _PAL_THREAD_H
#define _PAL_THREAD_H

#include "pal_core.h"

typedef Uint32 PalTLSId;
typedef struct PalThread PalThread;
typedef struct PalMutex PalMutex;
typedef struct PalCondVar PalCondVar;

typedef void* (*PalThreadFn)(
    void* arg);

typedef void (*PaTlsDestructorFn)(
    void* userData);

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

PAL_API const PalAllocator* PAL_CALL palGetThreadAllocator();

PAL_API void PAL_CALL palSetThreadAllocator(
    const PalAllocator* allocator);

PAL_API PalResult PAL_CALL palCreateThread(
    const PalThreadCreateInfo* info,
    PalThread** outThread);

PAL_API PalResult PAL_CALL palJoinThread(
    PalThread* thread, 
    void* retval);

PAL_API void PAL_CALL palDetachThread(
    PalThread* thread);

PAL_API void PAL_CALL palSleep(
    Uint64 milliseconds);

PAL_API void PAL_CALL palYield();

PAL_API PalThread* PAL_CALL palGetCurrentThread();
PAL_API PalThreadFeatures PAL_CALL palGetThreadFeatures();

PAL_API PalThreadPriority PAL_CALL palGetThreadPriority(
    PalThread* thread);

PAL_API Uint64 PAL_CALL palGetThreadAffinity(
    PalThread* thread);

PAL_API char* PAL_CALL palGetThreadName(
    PalThread* thread);

PAL_API PalResult PAL_CALL palSetThreadPriority(
    PalThread* thread, 
    PalThreadPriority priority);

PAL_API PalResult PAL_CALL palSetThreadAffinity(
    PalThread* thread,
    Uint64 mask);

PAL_API PalResult PAL_CALL palSetThreadName(
    PalThread* thread, 
    const char* name);

PAL_API PalTLSId PAL_CALL palCreateTLS(
    PaTlsDestructorFn destructor);

PAL_API void PAL_CALL palDestroyTLS(
    PalTLSId id);

PAL_API void* PAL_CALL palGetTLS(
    PalTLSId id);

PAL_API void PAL_CALL palSetTLS(
    PalTLSId id, 
    void* data);

PAL_API PalResult PAL_CALL palCreateMutex(
    PalMutex** outMutex);

PAL_API void PAL_CALL palDestroyMutex(
    PalMutex* mutex);

PAL_API void PAL_CALL palLockMutex(
    PalMutex* mutex);

PAL_API void PAL_CALL palUnlockMutex(
    PalMutex* mutex);

PAL_API PalResult PAL_CALL palCreateCondVar(
    PalCondVar** outCondition);

PAL_API void PAL_CALL palDestroyCondVar(
    PalCondVar* condition);

PAL_API PalResult PAL_CALL palWaitCondVar(
    PalCondVar* condition,
    PalMutex* mutex);

PAL_API PalResult PAL_CALL palWaitCondVarTimeout(
    PalCondVar* condition,
    PalMutex* mutex,
    Uint64 milliseconds);

PAL_API void PAL_CALL palSignalCondVar(
    PalCondVar* condition);
    
PAL_API void PAL_CALL palBroadcastCondVar(
    PalCondVar* condition);

#endif // _PAL_THREAD_H