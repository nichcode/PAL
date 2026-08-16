
/**
    PAL - Prime Abstraction Layer
    Copyright (C) 2025
    Licensed under the Zlib license. See LICENSE file in root.
 */

#if PAL_HAS_D3D12_BACKEND
#include "pal_d3d12.h"

PalResult PAL_CALL createCommandPoolD3D12(
    PalDevice* device,
    PalQueue* queue,
    PalCommandPool** outPool)
{
    HRESULT result;
    CommandPoolD3D12* pool = nullptr;
    QueueD3D12* queueImpl = (QueueD3D12*)queue;

    pool = palAllocate(s_D3D12.allocator, sizeof(CommandPoolD3D12), 0);
    if (!pool) {
        return PAL_RESULT_CODE_OUT_OF_MEMORY;
    }

    switch (queueImpl->type) {
        case PAL_QUEUE_TYPE_COMPUTE: {
            pool->type = D3D12_COMMAND_LIST_TYPE_COMPUTE;
            break;
        }

        case PAL_QUEUE_TYPE_GRAPHICS: {
            pool->type = D3D12_COMMAND_LIST_TYPE_DIRECT;
            break;
        }

        case PAL_QUEUE_TYPE_COPY: {
            pool->type = D3D12_COMMAND_LIST_TYPE_COPY;
            break;
        }
    }

    *outPool = (PalCommandPool*)pool;
    return PAL_RESULT_SUCCESS;
}

void PAL_CALL destroyCommandPoolD3D12(PalCommandPool* pool)
{
    CommandPoolD3D12* cmdPool = (CommandPoolD3D12*)pool;
    palFree(s_D3D12.allocator, cmdPool);
}

PalResult PAL_CALL allocateCommandBufferD3D12(
    PalDevice* device,
    PalCommandPool* pool,
    PalCommandBufferType type,
    PalCommandBuffer** outCmdBuffer)
{
    HRESULT result;
    DeviceD3D12* deviceImpl = (DeviceD3D12*)device;
    CommandBufferD3D12* cmdBuffer = nullptr;
    CommandPoolD3D12* cmdPool = (CommandPoolD3D12*)pool;

    cmdBuffer = palAllocate(s_D3D12.allocator, sizeof(CommandBufferD3D12), 0);
    if (!cmdBuffer) {
        return PAL_RESULT_CODE_OUT_OF_MEMORY;
    }

    memset(cmdBuffer, 0, sizeof(CommandBufferD3D12));
    cmdBuffer->primary = PAL_TRUE;

    // allocate memory for the linear allocator. We first start with 4KB
    cmdBuffer->linearAllocator.memory = (uint8_t*)palAllocate(s_D3D12.allocator, 4096, 0);
    if (!cmdBuffer->linearAllocator.memory) {
        PAL_RESULT_CODE_OUT_OF_MEMORY;
    }

    cmdBuffer->linearAllocator.size = 4096;
    cmdBuffer->linearAllocator.offset = 0;
    D3D12_COMMAND_LIST_TYPE cmdBufferType = cmdPool->type;
    if (type == PAL_COMMAND_BUFFER_TYPE_SECONDARY) {
        cmdBufferType = D3D12_COMMAND_LIST_TYPE_BUNDLE;
        cmdBuffer->primary = PAL_FALSE;
    }

    // create an allocator
    result = deviceImpl->handle->lpVtbl->CreateCommandAllocator(
        deviceImpl->handle,
        cmdBufferType,
        &IID_CommandAllocator,
        (void**)&cmdBuffer->allocator);

    if (FAILED(result)) {
        pollMessagesD3D12(deviceImpl);
        return makeResultD3D12(result);
    }

    // create the command list
    ID3D12GraphicsCommandList* cmdList = nullptr;
    result = deviceImpl->handle->lpVtbl->CreateCommandList(
        deviceImpl->handle,
        0,
        cmdBufferType,
        cmdBuffer->allocator,
        nullptr,
        &IID_CommandList,
        (void**)&cmdList);

    if (FAILED(result)) {
        pollMessagesD3D12(deviceImpl);
        return makeResultD3D12(result);
    }

    // create a tmp gpu buffer
    D3D12_HEAP_PROPERTIES heapProps = {0};
    heapProps.Type = D3D12_HEAP_TYPE_DEFAULT;
    heapProps.VisibleNodeMask = 1;
    heapProps.CreationNodeMask = 1;

    D3D12_RESOURCE_DESC bufferDesc = {0};
    bufferDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
    bufferDesc.Width = sizeof(D3D12_DISPATCH_RAYS_DESC);
    bufferDesc.Height = 1;
    bufferDesc.DepthOrArraySize = 1;
    bufferDesc.MipLevels = 1;
    bufferDesc.SampleDesc.Count = 1;
    bufferDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

    result = deviceImpl->handle->lpVtbl->CreateCommittedResource(
        deviceImpl->handle,
        &heapProps,
        0,
        &bufferDesc,
        D3D12_RESOURCE_STATE_COMMON,
        nullptr,
        &IID_Resource,
        (void**)&cmdBuffer->buffer);

    if (FAILED(result)) {
        pollMessagesD3D12(deviceImpl);
        return makeResultD3D12(result);
    }

    // create staging buffer
    heapProps.Type = D3D12_HEAP_TYPE_UPLOAD;
    result = deviceImpl->handle->lpVtbl->CreateCommittedResource(
        deviceImpl->handle,
        &heapProps,
        0,
        &bufferDesc,
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        &IID_Resource,
        (void**)&cmdBuffer->stagingBuffer);

    if (FAILED(result)) {
        pollMessagesD3D12(deviceImpl);
        return makeResultD3D12(result);
    }

    result =
        cmdList->lpVtbl->QueryInterface(cmdList, &IID_CommandList6, (void**)&cmdBuffer->handle);

    if (FAILED(result)) {
        pollMessagesD3D12(deviceImpl);
        return makeResultD3D12(result);
    }

    cmdList->lpVtbl->Release(cmdList);
    cmdBuffer->handle->lpVtbl->Close(cmdBuffer->handle);

    cmdBuffer->device = deviceImpl;
    *outCmdBuffer = (PalCommandBuffer*)cmdBuffer;
    return PAL_RESULT_SUCCESS;
}

void PAL_CALL freeCommandBufferD3D12(PalCommandBuffer* cmdBuffer)
{
    CommandBufferD3D12* cmdBufferImpl = (CommandBufferD3D12*)cmdBuffer;
    cmdBufferImpl->handle->lpVtbl->Release(cmdBufferImpl->handle);
    cmdBufferImpl->allocator->lpVtbl->Release(cmdBufferImpl->allocator);
    cmdBufferImpl->buffer->lpVtbl->Release(cmdBufferImpl->buffer);
    cmdBufferImpl->stagingBuffer->lpVtbl->Release(cmdBufferImpl->stagingBuffer);

    palFree(s_D3D12.allocator, (void*)cmdBufferImpl->linearAllocator.memory);
    palFree(s_D3D12.allocator, cmdBufferImpl);
}

PalResult PAL_CALL resetCommandBufferD3D12(PalCommandBuffer* cmdBuffer)
{
    HRESULT result;
    CommandBufferD3D12* cmdBufferImpl = (CommandBufferD3D12*)cmdBuffer;
    result = cmdBufferImpl->allocator->lpVtbl->Reset(cmdBufferImpl->allocator);
    if (FAILED(result)) {
        return makeResultD3D12(result);
    }

    result = cmdBufferImpl->handle->lpVtbl->Reset(
        cmdBufferImpl->handle,
        cmdBufferImpl->allocator,
        nullptr);

    if (FAILED(result)) {
        return makeResultD3D12(result);
    }

    result = cmdBufferImpl->handle->lpVtbl->Close(cmdBufferImpl->handle);
    if (FAILED(result)) {
        pollMessagesD3D12(cmdBufferImpl->device);
        return makeResultD3D12(result);
    }

    cmdBufferImpl->pipeline = nullptr;
    return PAL_RESULT_SUCCESS;
}

PalResult PAL_CALL submitCommandBufferD3D12(
    PalQueue* queue,
    PalCommandBufferSubmitInfo* info)
{
    HRESULT ret;
    QueueD3D12* queueImpl = (QueueD3D12*)queue;
    ID3D12CommandQueue* queueHandle = queueImpl->handle;
    CommandBufferD3D12* cmdBufferImpl = (CommandBufferD3D12*)info->cmdBuffer;

    // poll pending messages
    pollMessagesD3D12(cmdBufferImpl->device);

    // wait semaphore
    if (info->waitSemaphore) {
        UINT64 value = 0;
        SemaphoreD3D12* semaphore = (SemaphoreD3D12*)info->waitSemaphore;
        if (semaphore->isTimeline) {
            value = info->waitValue;
        } else {
            value = semaphore->value;
        }

        ret = queueHandle->lpVtbl->Wait(queueHandle, semaphore->handle, value);
        if (FAILED(ret)) {
            return makeResultD3D12(ret);
        }

        // reset the value if its a binary semaphore
        if (!semaphore->isTimeline) {
            semaphore->value = 0;
            ret = semaphore->handle->lpVtbl->Signal(semaphore->handle, 0);
            if (FAILED(ret)) {
                return makeResultD3D12(ret);
            }
        }
    }

    ID3D12CommandList* cmdLists[1] = {(ID3D12CommandList*)cmdBufferImpl->handle};
    queueHandle->lpVtbl->ExecuteCommandLists(queueHandle, 1, cmdLists);
    pollMessagesD3D12(cmdBufferImpl->device);

    // this is used for queue wait
    queueImpl->fenceValue++;
    ret = queueHandle->lpVtbl->Signal(queueHandle, queueImpl->fence, queueImpl->fenceValue);
    if (FAILED(ret)) {
        return makeResultD3D12(ret);
    }

    if (info->fence) {
        FenceD3D12* fence = (FenceD3D12*)info->fence;
        fence->value++;
        ret = queueHandle->lpVtbl->Signal(queueHandle, fence->handle, fence->value);
        if (FAILED(ret)) {
            return makeResultD3D12(ret);
        }
    }

    if (info->signalSemaphore) {
        UINT64 value = 0;
        SemaphoreD3D12* semaphore = (SemaphoreD3D12*)info->signalSemaphore;
        if (semaphore->isTimeline) {
            value = info->signalValue;
        } else {
            semaphore->value = 1;
            value = semaphore->value;
        }

        ret = queueHandle->lpVtbl->Signal(queueHandle, semaphore->handle, value);
        if (FAILED(ret)) {
            return makeResultD3D12(ret);
        }
    }

    return PAL_RESULT_SUCCESS;
}

#endif // PAL_HAS_D3D12_BACKEND