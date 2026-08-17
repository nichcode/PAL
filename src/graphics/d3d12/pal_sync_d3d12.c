
/**
    PAL - Prime Abstraction Layer
    Copyright (C) 2025
    Licensed under the Zlib license. See LICENSE file in root.
 */

#if PAL_HAS_D3D12_BACKEND
#include "pal_d3d12.h"

PalResult PAL_CALL createFenceD3D12(
    PalDevice* device,
    PalBool signaled,
    PalFence** outFence)
{
    HRESULT result;
    DeviceD3D12* deviceImpl = (DeviceD3D12*)device;
    FenceD3D12* fence = nullptr;

    fence = palAllocate(s_D3D12.allocator, sizeof(FenceD3D12), 0);
    if (!fence) {
        return PAL_RESULT_CODE_OUT_OF_MEMORY;
    }

    result = deviceImpl->handle->lpVtbl
                 ->CreateFence(deviceImpl->handle, 0, 0, &IID_Fence, (void**)&fence->handle);

    if (FAILED(result)) {
        palFree(s_D3D12.allocator, fence);
        return makeResultD3D12(result);
    }
    pollMessagesD3D12(deviceImpl);

    // create event
    fence->event = CreateEvent(nullptr, PAL_FALSE, PAL_FALSE, nullptr);
    if (!fence->event) {
        return palMakeResult(
            PAL_RESULT_CODE_PLATFORM_FAILURE,
            PAL_RESULT_SOURCE_WIN32,
            GetLastError());
    }

    fence->canReset = PAL_FALSE;
    if (deviceImpl->canFenceReset) {
        fence->canReset = PAL_TRUE;
    }

    fence->isTimeline = PAL_FALSE; // for semaphores
    fence->value = 0;
    *outFence = (PalFence*)fence;
    return PAL_RESULT_SUCCESS;
}

void PAL_CALL destroyFenceD3D12(PalFence* fence)
{
    FenceD3D12* fenceImpl = (FenceD3D12*)fence;
    fenceImpl->handle->lpVtbl->Release(fenceImpl->handle);
    CloseHandle(fenceImpl->event);
    palFree(s_D3D12.allocator, fenceImpl);
}

PalResult PAL_CALL waitFenceD3D12(
    PalFence* fence,
    uint64_t timeout)
{
    HRESULT result;
    FenceD3D12* fenceImpl = (FenceD3D12*)fence;
    DWORD ret = 0;
    uint64_t value = fenceImpl->value;
    HANDLE event = fenceImpl->event;

    if (fenceImpl->handle->lpVtbl->GetCompletedValue(fenceImpl->handle) < value) {
        result = fenceImpl->handle->lpVtbl->SetEventOnCompletion(fenceImpl->handle, value, event);
        if (FAILED(result)) {
            return makeResultD3D12(result);
        }

        if (timeout == PAL_INFINITE) {
            ret = WaitForSingleObject(event, INFINITE);
        } else {
            ret = WaitForSingleObject(event, (DWORD)timeout);
        }
    }

    if (ret == WAIT_TIMEOUT) {
        return PAL_RESULT_CODE_TIMEOUT;
    }

    return PAL_RESULT_SUCCESS;
}

PalResult PAL_CALL resetFenceD3D12(PalFence* fence)
{
    FenceD3D12* fenceImpl = (FenceD3D12*)fence;
    fenceImpl->handle->lpVtbl->Signal(fenceImpl->handle, 0);
    fenceImpl->value = 0;
    return PAL_RESULT_SUCCESS;
}

PalBool PAL_CALL isFenceSignaledD3D12(PalFence* fence)
{
    FenceD3D12* fenceImpl = (FenceD3D12*)fence;
    if (fenceImpl->handle->lpVtbl->GetCompletedValue(fenceImpl->handle) == 0) {
        return PAL_FALSE;
    }
    return PAL_TRUE;
}

PalResult PAL_CALL createSemaphoreD3D12(
    PalDevice* device,
    PalBool enableTimeline,
    PalSemaphore** outSemaphore)
{
    HRESULT result;
    DeviceD3D12* deviceImpl = (DeviceD3D12*)device;
    SemaphoreD3D12* semaphore = nullptr;

    semaphore = palAllocate(s_D3D12.allocator, sizeof(SemaphoreD3D12), 0);
    if (!semaphore) {
        return PAL_RESULT_CODE_OUT_OF_MEMORY;
    }

    if (enableTimeline) {
        semaphore->isTimeline = PAL_TRUE;
    }

    result = deviceImpl->handle->lpVtbl
                 ->CreateFence(deviceImpl->handle, 0, 0, &IID_Fence, (void**)&semaphore->handle);

    if (FAILED(result)) {
        palFree(s_D3D12.allocator, semaphore);
        return makeResultD3D12(result);
    }
    pollMessagesD3D12(deviceImpl);

    // create event
    semaphore->event = CreateEvent(nullptr, PAL_FALSE, PAL_FALSE, nullptr);
    if (!semaphore->event) {
        return palMakeResult(
            PAL_RESULT_CODE_PLATFORM_FAILURE,
            PAL_RESULT_SOURCE_WIN32,
            GetLastError());
    }

    semaphore->canReset = PAL_FALSE;
    semaphore->value = 0;
    *outSemaphore = (PalSemaphore*)semaphore;
    return PAL_RESULT_SUCCESS;
}

void PAL_CALL destroySemaphoreD3D12(PalSemaphore* semaphore)
{
    SemaphoreD3D12* semaphoreImpl = (SemaphoreD3D12*)semaphore;
    semaphoreImpl->handle->lpVtbl->Release(semaphoreImpl->handle);
    CloseHandle(semaphoreImpl->event);
    palFree(s_D3D12.allocator, semaphoreImpl);
}

PalResult PAL_CALL waitSemaphoreD3D12(
    PalSemaphore* semaphore,
    uint64_t value,
    uint64_t timeout)
{
    HRESULT result;
    DWORD ret = 0;
    SemaphoreD3D12* semaphoreImpl = (SemaphoreD3D12*)semaphore;

    HANDLE event = semaphoreImpl->event;
    if (semaphoreImpl->handle->lpVtbl->GetCompletedValue(semaphoreImpl->handle) < value) {
        result = semaphoreImpl->handle->lpVtbl->SetEventOnCompletion(
            semaphoreImpl->handle,
            value,
            event);

        if (FAILED(result)) {
            return makeResultD3D12(result);
        }

        if (timeout == PAL_INFINITE) {
            ret = WaitForSingleObject(event, INFINITE);
        } else {
            ret = WaitForSingleObject(event, (DWORD)timeout);
        }
    }

    if (ret == WAIT_TIMEOUT) {
        return PAL_RESULT_CODE_TIMEOUT;
    }

    return PAL_RESULT_SUCCESS;
}

PalResult PAL_CALL signalSemaphoreD3D12(
    PalSemaphore* semaphore,
    PalQueue* queue,
    uint64_t value)
{
    SemaphoreD3D12* semaphoreImpl = (SemaphoreD3D12*)semaphore;
    HRESULT result = semaphoreImpl->handle->lpVtbl->Signal(semaphoreImpl->handle, value);
    if (FAILED(result)) {
        return makeResultD3D12(result);
    }

    return PAL_RESULT_SUCCESS;
}

PalResult PAL_CALL getSemaphoreValueD3D12(
    PalSemaphore* semaphore,
    uint64_t* value)
{
    SemaphoreD3D12* semaphoreImpl = (SemaphoreD3D12*)semaphore;
    *value = semaphoreImpl->handle->lpVtbl->GetCompletedValue(semaphoreImpl->handle);
    return PAL_RESULT_SUCCESS;
}

#endif // PAL_HAS_D3D12_BACKEND