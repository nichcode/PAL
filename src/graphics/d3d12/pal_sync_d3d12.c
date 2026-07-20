
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
    DeviceD3D12* d3d12Device = (DeviceD3D12*)device;
    FenceD3D12* fence = nullptr;

    fence = palAllocate(s_D3D12.allocator, sizeof(FenceD3D12), 0);
    if (!fence) {
        return PAL_RESULT_CODE_OUT_OF_MEMORY;
    }

    result = d3d12Device->handle->lpVtbl
                 ->CreateFence(d3d12Device->handle, 0, 0, &IID_Fence, (void**)&fence->handle);

    if (FAILED(result)) {
        pollMessagesD3D12(d3d12Device);
        palFree(s_D3D12.allocator, fence);
        return makeResultD3D12(result);
    }

    // create event
    fence->event = CreateEvent(nullptr, PAL_FALSE, PAL_FALSE, nullptr);
    if (!fence->event) {
        return palMakeResult(
            PAL_RESULT_CODE_PLATFORM_FAILURE,
            PAL_RESULT_SOURCE_WIN32,
            GetLastError());
    }

    fence->canReset = PAL_FALSE;
    if (d3d12Device->canFenceReset) {
        fence->canReset = PAL_TRUE;
    }

    fence->isTimeline = PAL_FALSE; // for sempaphores
    fence->value = 0;
    *outFence = (PalFence*)fence;
    return PAL_RESULT_SUCCESS;
}

void PAL_CALL destroyFenceD3D12(PalFence* fence)
{
    FenceD3D12* d3d12Fence = (FenceD3D12*)fence;
    d3d12Fence->handle->lpVtbl->Release(d3d12Fence->handle);
    CloseHandle(d3d12Fence->event);
    palFree(s_D3D12.allocator, d3d12Fence);
}

PalResult PAL_CALL waitFenceD3D12(
    PalFence* fence,
    uint64_t timeout)
{
    HRESULT result;
    FenceD3D12* d3d12Fence = (FenceD3D12*)fence;
    DWORD ret = 0;
    uint64_t value = d3d12Fence->value;
    HANDLE event = d3d12Fence->event;

    if (d3d12Fence->handle->lpVtbl->GetCompletedValue(d3d12Fence->handle) < value) {
        result = d3d12Fence->handle->lpVtbl->SetEventOnCompletion(d3d12Fence->handle, value, event);

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
    FenceD3D12* d3d12Fence = (FenceD3D12*)fence;
    d3d12Fence->handle->lpVtbl->Signal(d3d12Fence->handle, 0);
    d3d12Fence->value = 0;
    return PAL_RESULT_SUCCESS;
}

PalBool PAL_CALL isFenceSignaledD3D12(PalFence* fence)
{
    FenceD3D12* d3d12Fence = (FenceD3D12*)fence;
    if (d3d12Fence->handle->lpVtbl->GetCompletedValue(d3d12Fence->handle) == 0) {
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
    DeviceD3D12* d3d12Device = (DeviceD3D12*)device;
    SemaphoreD3D12* semaphore = nullptr;

    semaphore = palAllocate(s_D3D12.allocator, sizeof(SemaphoreD3D12), 0);
    if (!semaphore) {
        return PAL_RESULT_CODE_OUT_OF_MEMORY;
    }

    if (enableTimeline) {
        semaphore->isTimeline = PAL_TRUE;
    }

    result = d3d12Device->handle->lpVtbl
                 ->CreateFence(d3d12Device->handle, 0, 0, &IID_Fence, (void**)&semaphore->handle);

    if (FAILED(result)) {
        pollMessagesD3D12(d3d12Device);
        palFree(s_D3D12.allocator, semaphore);
        return makeResultD3D12(result);
    }

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
    SemaphoreD3D12* d3d12Semaphore = (SemaphoreD3D12*)semaphore;
    d3d12Semaphore->handle->lpVtbl->Release(d3d12Semaphore->handle);
    CloseHandle(d3d12Semaphore->event);
    palFree(s_D3D12.allocator, d3d12Semaphore);
}

PalResult PAL_CALL waitSemaphoreD3D12(
    PalSemaphore* semaphore,
    uint64_t value,
    uint64_t timeout)
{
    HRESULT result;
    DWORD ret = 0;
    SemaphoreD3D12* d3d12Semaphore = (SemaphoreD3D12*)semaphore;

    HANDLE event = d3d12Semaphore->event;
    if (d3d12Semaphore->handle->lpVtbl->GetCompletedValue(d3d12Semaphore->handle) < value) {
        result = d3d12Semaphore->handle->lpVtbl->SetEventOnCompletion(
            d3d12Semaphore->handle,
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
    SemaphoreD3D12* d3d12Semaphore = (SemaphoreD3D12*)semaphore;
    HRESULT result = d3d12Semaphore->handle->lpVtbl->Signal(d3d12Semaphore->handle, value);
    if (FAILED(result)) {
        return makeResultD3D12(result);
    }

    return PAL_RESULT_SUCCESS;
}

PalResult PAL_CALL getSemaphoreValueD3D12(
    PalSemaphore* semaphore,
    uint64_t* value)
{
    SemaphoreD3D12* d3d12Semaphore = (SemaphoreD3D12*)semaphore;
    *value = d3d12Semaphore->handle->lpVtbl->GetCompletedValue(d3d12Semaphore->handle);
    return PAL_RESULT_SUCCESS;
}

#endif // PAL_HAS_D3D12_BACKEND