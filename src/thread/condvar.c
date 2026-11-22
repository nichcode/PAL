#include "platform.h"
#include "pal2/thread/condvar.h"

PalResult PAL_CALL palCreateCondVar(
    const PalAllocator* allocator,
    PalCondVar** condVar)
{
    if (!condVar) {
        return PAL_RESULT_CODE_INVALID_ARGUMENT;
    }

    return platformCreateCondVar(allocator, condVar);
}

void PAL_CALL palDestroyCondVar(PalCondVar* condVar)
{
    ASSERT(condVar != nullptr, "The specified condvar is null");

    platformDestroyCondVar(condVar);
}

PalResult PAL_CALL palWaitCondVar(
    PalCondVar* condVar,
    PalMutex* mutex)
{
    if (!condVar || !mutex) {
        return PAL_RESULT_CODE_INVALID_HANDLE;
    }

    return platformWaitCondVar(condVar, mutex);
}

PalResult PAL_CALL palWaitCondVarTimeout(
    PalCondVar* condVar,
    PalMutex* mutex,
    uint64_t milliseconds)
{
    if (!condVar || !mutex) {
        return PAL_RESULT_CODE_INVALID_HANDLE;
    }

    return platformWaitCondVarTimeout(condVar, mutex, milliseconds);
}

void PAL_CALL palSignalCondVar(PalCondVar* condVar)
{
    ASSERT(condVar != nullptr, "The specified condvar is null");

    platformSignalCondVar(condVar);
}

void PAL_CALL palBroadcastCondVar(PalCondVar* condVar)
{
    ASSERT(condVar != nullptr, "The specified condvar is null");

    platformBroadcastCondVar(condVar);
}