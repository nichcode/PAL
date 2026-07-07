
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
    QueueD3D12* d3d12Queue = (QueueD3D12*)queue;

    pool = palAllocate(s_D3D12.allocator, sizeof(CommandPoolD3D12), 0);
    if (!pool) {
        return PAL_RESULT_CODE_OUT_OF_MEMORY;
    }

    pool->size = 8; // initial size
    pool->cmdBuffersData = nullptr;
    pool->cmdBuffersData = palAllocate(
        s_D3D12.allocator, 
        sizeof(CommandBufferData) * pool->size, 
        0);

    if (!pool->cmdBuffersData) {
        return PAL_RESULT_CODE_OUT_OF_MEMORY;
    }

    switch (d3d12Queue->type) {
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

    pool->reserved = PAL_BACKEND_KEY;
    *outPool = (PalCommandPool*)pool;
    return PAL_RESULT_SUCCESS;
}

void PAL_CALL destroyCommandPoolD3D12(PalCommandPool* pool)
{
    CommandPoolD3D12* cmdPool = (CommandPoolD3D12*)pool;
    for (int i = 0; i < cmdPool->size; i++) {
        if (!cmdPool->cmdBuffersData[i].used) {
            continue;
        }

        CommandBufferD3D12* cmdBuffer = cmdPool->cmdBuffersData[i].cmdBuffer;
        cmdBuffer->handle->lpVtbl->Release(cmdBuffer->handle);
        cmdBuffer->allocator->lpVtbl->Release(cmdBuffer->allocator);
    }

    palFree(s_D3D12.allocator, cmdPool->cmdBuffersData);
    palFree(s_D3D12.allocator, cmdPool);
}

PalResult PAL_CALL resetCommandPoolD3D12(PalCommandPool* pool)
{
    CommandPoolD3D12* cmdPool = (CommandPoolD3D12*)pool;
    for (int i = 0; i < cmdPool->size; i++) {
        if (!cmdPool->cmdBuffersData[i].used) {
            continue;
        }

        HRESULT ret;
        CommandBufferD3D12* cmdBuffer = cmdPool->cmdBuffersData[i].cmdBuffer;
        ret = cmdBuffer->handle->lpVtbl->Reset(cmdBuffer->handle, cmdBuffer->allocator, nullptr);
        if (FAILED(ret)) {
            return makeResultD3D12(ret);
        }
    }
    return PAL_RESULT_SUCCESS;
}

PalResult PAL_CALL allocateCommandBufferD3D12(
    PalDevice* device,
    PalCommandPool* pool,
    PalCommandBufferType type,
    PalCommandBuffer** outCmdBuffer)
{
    HRESULT result;
    DeviceD3D12* d3d12Device = (DeviceD3D12*)device;
    CommandBufferD3D12* cmdBuffer = nullptr;
    CommandPoolD3D12* cmdPool = (CommandPoolD3D12*)pool;

    cmdBuffer = palAllocate(s_D3D12.allocator, sizeof(CommandBufferD3D12), 0);
    if (!cmdBuffer) {
        return PAL_RESULT_CODE_OUT_OF_MEMORY;
    }

    memset(cmdBuffer, 0, sizeof(CommandBufferD3D12));
    cmdBuffer->primary = PAL_TRUE;

    D3D12_COMMAND_LIST_TYPE cmdBufferType = cmdPool->type;
    if (type == PAL_COMMAND_BUFFER_TYPE_SECONDARY) {
        cmdBufferType = D3D12_COMMAND_LIST_TYPE_BUNDLE;
        cmdBuffer->primary = PAL_FALSE;
    }

    // create an allocator
    result = d3d12Device->handle->lpVtbl->CreateCommandAllocator(
        d3d12Device->handle,
        cmdBufferType,
        &IID_CommandAllocator,
        (void**)&cmdBuffer->allocator);

    if (FAILED(result)) {
        pollMessagesD3D12(d3d12Device);
        return makeResultD3D12(result);
    }

    // create the command list
    ID3D12GraphicsCommandList* cmdList = nullptr;
    result = d3d12Device->handle->lpVtbl->CreateCommandList(
        d3d12Device->handle,
        0,
        cmdBufferType,
        cmdBuffer->allocator,
        nullptr,
        &IID_CommandList,
        (void**)&cmdList);

    if (FAILED(result)) {
        pollMessagesD3D12(d3d12Device);
        return makeResultD3D12(result);
    }

    // we need a tmp staging and gpu buffer if ray tracing is enabled
    if (d3d12Device->features & PAL_ADAPTER_FEATURE_RAY_TRACING) {
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

        result = d3d12Device->handle->lpVtbl->CreateCommittedResource(
            d3d12Device->handle,
            &heapProps,
            0,
            &bufferDesc,
            D3D12_RESOURCE_STATE_COMMON,
            nullptr,
            &IID_Resource, 
            (void**)&cmdBuffer->buffer);

        if (FAILED(result)) {
            return makeResultD3D12(result);
        }

        // create staging buffer
        heapProps.Type = D3D12_HEAP_TYPE_UPLOAD;
        result = d3d12Device->handle->lpVtbl->CreateCommittedResource(
            d3d12Device->handle,
            &heapProps,
            0,
            &bufferDesc,
            D3D12_RESOURCE_STATE_GENERIC_READ,
            nullptr,
            &IID_Resource, 
            (void**)&cmdBuffer->stagingBuffer);

        if (FAILED(result)) {
            return makeResultD3D12(result);
        }
    }

    result = cmdList->lpVtbl->QueryInterface(
        cmdList,
        &IID_CommandList6,
        (void**)&cmdBuffer->handle);

    if (FAILED(result)) {
        return makeResultD3D12(result);
    }

    cmdList->lpVtbl->Release(cmdList);
    cmdBuffer->handle->lpVtbl->Close(cmdBuffer->handle);

    cmdBuffer->pool = cmdPool;
    cmdBuffer->device = d3d12Device;
    cmdBuffer->reserved = PAL_BACKEND_KEY;
    *outCmdBuffer = (PalCommandBuffer*)cmdBuffer;
    return PAL_RESULT_SUCCESS;
}

void PAL_CALL freeCommandBufferD3D12(PalCommandBuffer* cmdBuffer)
{
    CommandBufferD3D12* d3d12CmdBuffer = (CommandBufferD3D12*)cmdBuffer;
    CommandPoolD3D12* pool = d3d12CmdBuffer->pool;
    CommandBufferData* data = findCmdBufferData(pool, d3d12CmdBuffer);
    if (data) {
        d3d12CmdBuffer->handle->lpVtbl->Release(d3d12CmdBuffer->handle);
        d3d12CmdBuffer->allocator->lpVtbl->Release(d3d12CmdBuffer->allocator);

        if (d3d12CmdBuffer->device->features & PAL_ADAPTER_FEATURE_RAY_TRACING) {
            d3d12CmdBuffer->buffer->lpVtbl->Release(d3d12CmdBuffer->buffer);
            d3d12CmdBuffer->stagingBuffer->lpVtbl->Release(d3d12CmdBuffer->stagingBuffer);
        }

        palFree(s_D3D12.allocator, cmdBuffer);
        data->cmdBuffer = nullptr;
        data->used = PAL_FALSE;
    }
}

PalResult PAL_CALL resetCommandBufferD3D12(PalCommandBuffer* cmdBuffer)
{
    HRESULT result;
    CommandBufferD3D12* d3d12CmdBuffer = (CommandBufferD3D12*)cmdBuffer;
    result = d3d12CmdBuffer->allocator->lpVtbl->Reset(d3d12CmdBuffer->allocator);
    if (FAILED(result)) {
        pollMessagesD3D12(d3d12CmdBuffer->device);
        return makeResultD3D12(result);
    }

    result = d3d12CmdBuffer->handle->lpVtbl->Reset(
        d3d12CmdBuffer->handle,
        d3d12CmdBuffer->allocator,
        nullptr);

    if (FAILED(result)) {
        pollMessagesD3D12(d3d12CmdBuffer->device);
        return makeResultD3D12(result);
    }

    result = d3d12CmdBuffer->handle->lpVtbl->Close(d3d12CmdBuffer->handle);
    if (FAILED(result)) {
        pollMessagesD3D12(d3d12CmdBuffer->device);
        return makeResultD3D12(result);
    }

    d3d12CmdBuffer->pipeline = nullptr;
    return PAL_RESULT_SUCCESS;
}

PalResult PAL_CALL submitCommandBufferD3D12(
    PalQueue* queue,
    PalCommandBufferSubmitInfo* info)
{
    HRESULT ret;
    QueueD3D12* d3d12Queue = (QueueD3D12*)queue;
    ID3D12CommandQueue* queueHandle = d3d12Queue->handle;
    CommandBufferD3D12* d3d12CmdBuffer = (CommandBufferD3D12*)info->cmdBuffer;

    // wait semaphore
    if (info->waitSemaphore) {
        SemaphoreD3D12* semaphore = (SemaphoreD3D12*)info->waitSemaphore;
        if (semaphore->isTimeline) {
            ret = queueHandle->lpVtbl->Wait(queueHandle, semaphore->handle, info->waitValue);
            if (FAILED(ret)) {
                pollMessagesD3D12(d3d12CmdBuffer->device);
                return makeResultD3D12(ret);
            }

        } else {
            queueHandle->lpVtbl->Wait(queueHandle, semaphore->handle, semaphore->value);
            if (FAILED(ret)) {
                pollMessagesD3D12(d3d12CmdBuffer->device);
                return makeResultD3D12(ret);
            }

            semaphore->handle->lpVtbl->Signal(semaphore->handle, 0);
            if (FAILED(ret)) {
                pollMessagesD3D12(d3d12CmdBuffer->device);
                return makeResultD3D12(ret);
            }

            semaphore->value = 0;
        }
    }

    ID3D12CommandList* cmdLists[1] = { (ID3D12CommandList*)d3d12CmdBuffer->handle };
    queueHandle->lpVtbl->ExecuteCommandLists(queueHandle, 1, cmdLists);
    
    d3d12Queue->fenceValue++;
    ret = queueHandle->lpVtbl->Signal(queueHandle, d3d12Queue->fence, d3d12Queue->fenceValue);
    if (FAILED(ret)) {
        pollMessagesD3D12(d3d12CmdBuffer->device);
        return makeResultD3D12(ret);
    }

    if (info->fence) {
        FenceD3D12* fence = (FenceD3D12*)info->fence;
        fence->value++;
        ret = queueHandle->lpVtbl->Signal(queueHandle, fence->handle, fence->value);
        if (FAILED(ret)) {
            pollMessagesD3D12(d3d12CmdBuffer->device);
            return makeResultD3D12(ret);
        }
    }

    if (info->signalSemaphore) {
        SemaphoreD3D12* semaphore = (SemaphoreD3D12*)info->signalSemaphore;
        if (semaphore->isTimeline) {
            ret = queueHandle->lpVtbl->Signal(queueHandle, semaphore->handle, info->signalValue);
            if (FAILED(ret)) {
                pollMessagesD3D12(d3d12CmdBuffer->device);
                return makeResultD3D12(ret);
            }
        } else {
            semaphore->value = 1;
            ret = queueHandle->lpVtbl->Signal(queueHandle, semaphore->handle, semaphore->value);
            if (FAILED(ret)) {
                pollMessagesD3D12(d3d12CmdBuffer->device);
                return makeResultD3D12(ret);
            }
        }
    }

    return PAL_RESULT_SUCCESS;
}

#endif // PAL_HAS_D3D12_BACKEND