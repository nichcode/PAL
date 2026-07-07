
/**
    PAL - Prime Abstraction Layer
    Copyright (C) 2025
    Licensed under the Zlib license. See LICENSE file in root.
 */

#if PAL_HAS_D3D12_BACKEND
#include "pal_d3d12.h"

#define align(v, a) (v + a - 1) & ~(a - 1)

PalResult PAL_CALL createBufferD3D12(
    PalDevice* device,
    const PalBufferCreateInfo* info,
    PalBuffer** outBuffer)
{
    HRESULT result;
    BufferD3D12* buffer = nullptr;
    DeviceD3D12* d3d12Device = (DeviceD3D12*)device;

    if (info->usages & PAL_BUFFER_USAGE_ACCELERATION_STRUCTURE) {
        if (!(d3d12Device->features & PAL_ADAPTER_FEATURE_RAY_TRACING)) {
            return PAL_RESULT_CODE_FEATURE_NOT_SUPPORTED;
        }

    } else if (info->usages & PAL_BUFFER_USAGE_DEVICE_ADDRESS) {
        if (!(d3d12Device->features & PAL_ADAPTER_FEATURE_BUFFER_DEVICE_ADDRESS)) {
            return PAL_RESULT_CODE_FEATURE_NOT_SUPPORTED;
        }
    }

    buffer = palAllocate(s_D3D12.allocator, sizeof(BufferD3D12), 0);
    if (!buffer) {
        return PAL_RESULT_CODE_OUT_OF_MEMORY;
    }

    memset(buffer, 0, sizeof(BufferD3D12));
    buffer->desc.Width = info->size;
    buffer->desc.Height = 1;
    buffer->desc.DepthOrArraySize = 1;
    buffer->desc.MipLevels = 1;
    buffer->desc.SampleDesc.Count = 1;
    buffer->desc.SampleDesc.Quality = 0;
    buffer->desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
    buffer->desc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

    if (info->usages & PAL_BUFFER_USAGE_STORAGE) {
        buffer->desc.Flags |= D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
    }

    if (info->usages & PAL_BUFFER_USAGE_ACCELERATION_STRUCTURE) {
        buffer->desc.Flags |= D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
    }

    if (info->usages & PAL_BUFFER_USAGE_ACCELERATION_STRUCTURE_SCRATCH) {
        buffer->desc.Flags |= D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
    }

    buffer->isMemoryManaged = PAL_FALSE;
    if (info->memoryUsage != PAL_BUFFER_MEMORY_USAGE_MANUAL) {

        D3D12_HEAP_PROPERTIES heapProps = {0};
        heapProps.Type = D3D12_HEAP_TYPE_DEFAULT;
        D3D12_RESOURCE_STATES state = D3D12_RESOURCE_STATE_COMMON;

        if (info->memoryUsage != PAL_BUFFER_MEMORY_USAGE_AUTO_CPU_UPLOAD) {
            heapProps.Type = D3D12_HEAP_TYPE_UPLOAD;
            state = D3D12_RESOURCE_STATE_GENERIC_READ;

        } else if (info->memoryUsage != PAL_BUFFER_MEMORY_USAGE_AUTO_CPU_READBACK) {
            heapProps.Type = D3D12_HEAP_TYPE_READBACK;
            state = D3D12_RESOURCE_STATE_COPY_DEST;
        }

        result = d3d12Device->handle->lpVtbl->CreateCommittedResource(
            d3d12Device->handle, 
            &heapProps, 
            0, 
            &buffer->desc,
            state,
            nullptr,
            &IID_Resource,
            (void**)&buffer->handle);

        if (FAILED(result)) {
            return makeResultD3D12(result);
        }

        buffer->isMemoryManaged = PAL_TRUE;
    }

    buffer->usages = info->usages;
    buffer->device = d3d12Device;
    buffer->size = info->size;
    buffer->reserved = PAL_BACKEND_KEY;
    *outBuffer = (PalBuffer*)buffer;
    return PAL_RESULT_SUCCESS;
}

void PAL_CALL destroyBufferD3D12(PalBuffer* buffer)
{
    BufferD3D12* d3d12Buffer = (BufferD3D12*)buffer;
    if (d3d12Buffer->isMemoryManaged) {
        d3d12Buffer->handle->lpVtbl->Release(d3d12Buffer->handle);
    }
    palFree(s_D3D12.allocator, d3d12Buffer);
}

PalResult PAL_CALL getBufferMemoryRequirementsD3D12(
    PalBuffer* buffer,
    PalMemoryRequirements* requirements)
{
    BufferD3D12* d3d12Buffer = (BufferD3D12*)buffer;
    ID3D12Device5* device = d3d12Buffer->device->handle;

    D3D12_RESOURCE_ALLOCATION_INFO allocationInfo = {0};
    D3D12_RESOURCE_ALLOCATION_INFO __ret = {0};
    allocationInfo = *device->lpVtbl->GetResourceAllocationInfo(
        device,
        &__ret,
        0,
        1,
        &d3d12Buffer->desc);

    // d3d12 allows buffers to be used with all memory heap types
    requirements->supportedMemoryTypes |= (1u << PAL_MEMORY_TYPE_GPU_ONLY);
    requirements->supportedMemoryTypes |= (1u << PAL_MEMORY_TYPE_CPU_UPLOAD);
    requirements->supportedMemoryTypes |= (1u << PAL_MEMORY_TYPE_CPU_READBACK);

    requirements->alignment = allocationInfo.Alignment;
    requirements->size = allocationInfo.SizeInBytes;
    return PAL_RESULT_SUCCESS;
}

PalResult PAL_CALL computeInstanceBufferRequirementsD3D12(
    PalDevice* device,
    uint32_t instanceCount,
    uint64_t* outSize)
{
    *outSize = sizeof(D3D12_RAYTRACING_INSTANCE_DESC) * instanceCount;
    return PAL_RESULT_SUCCESS;
}

PalResult PAL_CALL computeImageCopyStagingBufferRequirementsD3D12(
    PalDevice* device,
    PalFormat imageFormat,
    PalBufferImageCopyInfo* copyInfo,
    uint32_t* outBufferRowLength,
    uint32_t* outBufferImageHeight,
    uint64_t* outSize)
{
    uint32_t imageFormatSize = getFormatSizeD3D12(imageFormat);
    uint32_t rowPitch = align((uint64_t)copyInfo->imageWidth * imageFormatSize, TEXTURE_PITCH);
    uint32_t bufferImageHeight = 0;

    if (copyInfo->bufferImageHeight) {
        bufferImageHeight = copyInfo->bufferImageHeight;
    } else {
        bufferImageHeight = copyInfo->imageHeight;
    }

    *outBufferRowLength = rowPitch;
    *outBufferImageHeight = bufferImageHeight;
    *outSize = (uint64_t)rowPitch * bufferImageHeight * copyInfo->imageDepth;
    return PAL_RESULT_SUCCESS;
}

PalResult PAL_CALL writeToInstanceBufferD3D12(
    PalDevice* device,
    void* ptr,
    PalAccelerationStructureInstance* instances,
    uint32_t instanceCount)
{
    D3D12_RAYTRACING_INSTANCE_DESC* data = ptr;
    for (int i = 0; i < instanceCount; i++) {
        PalAccelerationStructureInstance* src = &instances[i];
        D3D12_RAYTRACING_INSTANCE_DESC* dst = &data[i];
        AccelerationStructureD3D12* as = (AccelerationStructureD3D12*)src->blas;

        dst->InstanceMask = src->mask;
        dst->InstanceID = src->instanceId;
        dst->AccelerationStructure = as->address;
        dst->InstanceContributionToHitGroupIndex = src->hitGroupOffset;
        dst->Flags = instanceFlagsToD3D12(src->flags);
        memcpy(dst->Transform, src->transform, sizeof(float) * 12);
    }
    return PAL_RESULT_SUCCESS;
}

PalResult PAL_CALL writeToImageCopyStagingBufferD3D12(
    PalDevice* device,
    void* ptr,
    void* srcData,
    PalFormat imageFormat,
    PalBufferImageCopyInfo* copyInfo)
{
    uint32_t imageFormatSize = getFormatSizeD3D12(imageFormat);
    uint32_t srcRowPitch = copyInfo->imageWidth * imageFormatSize;
    const uint32_t dstSlicePitch = copyInfo->bufferRowLength * copyInfo->bufferImageHeight;
    const uint32_t srcSlicePitch = srcRowPitch * copyInfo->imageHeight;

    // manually offset the buffer with the provided offset
    uint8_t* dst = (uint8_t*)ptr + copyInfo->bufferOffset;
    const uint8_t* src = (const uint8_t*)srcData;

    // write to destination pointer
    for (uint32_t z = 0; z < copyInfo->imageDepth; z++) {
        for (uint32_t y = 0; y < copyInfo->imageHeight; y++) {
            memcpy(
                dst + z * dstSlicePitch + y * copyInfo->bufferRowLength,
                src + z * srcSlicePitch + y * srcRowPitch,
                srcRowPitch);
        }
    }

    return PAL_RESULT_SUCCESS;
}

PalResult PAL_CALL bindBufferMemoryD3D12(
    PalBuffer* buffer,
    PalMemory* memory,
    uint64_t offset)
{
    HRESULT result;
    BufferD3D12* d3d12Buffer = (BufferD3D12*)buffer;
    ID3D12Device5* device = d3d12Buffer->device->handle;
    MemoryD3D12* d3d12Memory = (MemoryD3D12*)memory;

    if (d3d12Buffer->isMemoryManaged) {
        return PAL_RESULT_CODE_INVALID_OPERATION;
    }

    D3D12_RESOURCE_STATES state = 0;
    d3d12Buffer->canStateChange = PAL_TRUE;
    if (d3d12Memory->type == PAL_MEMORY_TYPE_CPU_UPLOAD) {
        state = D3D12_RESOURCE_STATE_GENERIC_READ;
        d3d12Buffer->canStateChange = PAL_FALSE;

    } else if (d3d12Memory->type == PAL_MEMORY_TYPE_CPU_UPLOAD) {
        state = D3D12_RESOURCE_STATE_COPY_DEST;
        d3d12Buffer->canStateChange = PAL_FALSE;
    }

    if (d3d12Buffer->usages & PAL_BUFFER_USAGE_ACCELERATION_STRUCTURE) {
        state = D3D12_RESOURCE_STATE_RAYTRACING_ACCELERATION_STRUCTURE;
        d3d12Buffer->canStateChange = PAL_FALSE;
    }

    if (d3d12Buffer->usages & PAL_BUFFER_USAGE_ACCELERATION_STRUCTURE_SCRATCH) {
        state = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
    }

    result = device->lpVtbl->CreatePlacedResource(
        device,
        d3d12Memory->handle,
        offset,
        &d3d12Buffer->desc,
        state,
        nullptr,
        &IID_Resource,
        (void**)&d3d12Buffer->handle);

    if (FAILED(result)) {
        pollMessagesD3D12(d3d12Buffer->device);
        return makeResultD3D12(result);
    }

    return PAL_RESULT_SUCCESS;
}

PalResult PAL_CALL mapBufferD3D12(
    PalBuffer* buffer,
    uint64_t offset,
    uint64_t size,
    void** outPtr)
{

    BufferD3D12* d3d12Buffer = (BufferD3D12*)buffer;
    void* ptr = nullptr;
    HRESULT result = d3d12Buffer->handle->lpVtbl->Map(d3d12Buffer->handle, 0, nullptr, &ptr);
    if (FAILED(result)) {
        pollMessagesD3D12(d3d12Buffer->device);
        return makeResultD3D12(result);
    }

    *outPtr = (uint8_t*)ptr + offset;
    return PAL_RESULT_SUCCESS;
}

void PAL_CALL unmapBufferD3D12(PalBuffer* buffer)
{
    BufferD3D12* d3d12Buffer = (BufferD3D12*)buffer;
    d3d12Buffer->handle->lpVtbl->Unmap(d3d12Buffer->handle, 0, nullptr);
}

PalDeviceAddress PAL_CALL getBufferDeviceAddressD3D12(PalBuffer* buffer)
{
    BufferD3D12* d3d12Buffer = (BufferD3D12*)buffer;
    if (d3d12Buffer->usages & PAL_BUFFER_USAGE_DEVICE_ADDRESS) {
        return d3d12Buffer->handle->lpVtbl->GetGPUVirtualAddress(d3d12Buffer->handle);
    }
    return 0;
}

#endif // PAL_HAS_D3D12_BACKEND