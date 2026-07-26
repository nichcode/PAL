
/**
    PAL - Prime Abstraction Layer
    Copyright (C) 2025
    Licensed under the Zlib license. See LICENSE file in root.
 */

#if PAL_HAS_D3D12_BACKEND
#include "pal_d3d12.h"

#define _ALIGN(v, a) (v + a - 1) & ~(a - 1)

static uint32_t getSupportedMemoryTypes(PalBufferUsages usages)
{
    uint32_t masks = 0;
    uint32_t gpuBit = (1u << PAL_MEMORY_TYPE_GPU_ONLY);
    uint32_t uploadBit = (1u << PAL_MEMORY_TYPE_CPU_UPLOAD);
    uint32_t readBackBit = (1u << PAL_MEMORY_TYPE_CPU_READBACK);

    masks |= gpuBit;
    masks |= uploadBit;
    masks |= readBackBit;

    if (usages & PAL_BUFFER_USAGE_STORAGE) {
        masks &= ~uploadBit;
        masks &= ~readBackBit;
    }

    if (usages & PAL_BUFFER_USAGE_ACCELERATION_STRUCTURE) {
        masks &= ~uploadBit;
        masks &= ~readBackBit;
    }

    if (usages & PAL_BUFFER_USAGE_ACCELERATION_STRUCTURE_SCRATCH) {
        masks &= ~uploadBit;
        masks &= ~readBackBit;
    }

    if (usages & PAL_BUFFER_USAGE_TRANSFER_DST) {
        masks |= (1u << gpuBit);
        masks &= ~uploadBit;
    }

    if (usages & PAL_BUFFER_USAGE_TRANSFER_SRC) {
        masks |= (1u << gpuBit);
        masks &= ~readBackBit;
    }

    return masks;
}

PalResult PAL_CALL createBufferD3D12(
    PalDevice* device,
    const PalBufferCreateInfo* info,
    PalBuffer** outBuffer)
{
    HRESULT result;
    BufferD3D12* buffer = nullptr;
    DeviceD3D12* deviceImpl = (DeviceD3D12*)device;

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
        buffer->canStateChange = PAL_TRUE;

        uint32_t masks = getSupportedMemoryTypes(info->usages);
        if (info->memoryUsage == PAL_BUFFER_MEMORY_USAGE_AUTO_GPU_ONLY) {
            if (!(masks & (1u << PAL_MEMORY_TYPE_GPU_ONLY))) {
                return PAL_RESULT_CODE_INVALID_ARGUMENT;
            }

        } else if (info->memoryUsage == PAL_BUFFER_MEMORY_USAGE_AUTO_CPU_UPLOAD) {
            heapProps.Type = D3D12_HEAP_TYPE_UPLOAD;
            state = D3D12_RESOURCE_STATE_GENERIC_READ;
            buffer->canStateChange = PAL_FALSE;

            if (!(masks & (1u << PAL_MEMORY_TYPE_CPU_UPLOAD))) {
                return PAL_RESULT_CODE_INVALID_ARGUMENT;
            }

        } else if (info->memoryUsage == PAL_BUFFER_MEMORY_USAGE_AUTO_CPU_READBACK) {
            heapProps.Type = D3D12_HEAP_TYPE_READBACK;
            state = D3D12_RESOURCE_STATE_COPY_DEST;
            buffer->canStateChange = PAL_FALSE;

            if (!(masks & (1u << PAL_MEMORY_TYPE_CPU_READBACK))) {
                return PAL_RESULT_CODE_INVALID_ARGUMENT;
            }
        }

        result = deviceImpl->handle->lpVtbl->CreateCommittedResource(
            deviceImpl->handle,
            &heapProps,
            0,
            &buffer->desc,
            state,
            nullptr,
            &IID_Resource,
            (void**)&buffer->handle);

        if (FAILED(result)) {
            pollMessagesD3D12(deviceImpl);
            return makeResultD3D12(result);
        }

        buffer->isMemoryManaged = PAL_TRUE;
    }

    buffer->device = deviceImpl;
    buffer->usages = info->usages;
    buffer->size = info->size;
    *outBuffer = (PalBuffer*)buffer;
    return PAL_RESULT_SUCCESS;
}

void PAL_CALL destroyBufferD3D12(PalBuffer* buffer)
{
    BufferD3D12* bufferImpl = (BufferD3D12*)buffer;
    if (bufferImpl->isMemoryManaged) {
        bufferImpl->handle->lpVtbl->Release(bufferImpl->handle);
    }
    palFree(s_D3D12.allocator, bufferImpl);
}

void PAL_CALL getBufferMemoryRequirementsD3D12(
    PalBuffer* buffer,
    PalMemoryRequirements* requirements)
{
    BufferD3D12* bufferImpl = (BufferD3D12*)buffer;
    ID3D12Device5* device = bufferImpl->device->handle;

    D3D12_RESOURCE_ALLOCATION_INFO allocationInfo = {0};
    D3D12_RESOURCE_ALLOCATION_INFO __ret = {0};
    allocationInfo =
        *device->lpVtbl->GetResourceAllocationInfo(device, &__ret, 0, 1, &bufferImpl->desc);

    requirements->supportedMemoryTypes = getSupportedMemoryTypes(bufferImpl->usages);
    requirements->alignment = allocationInfo.Alignment;
    requirements->size = allocationInfo.SizeInBytes;
}

void PAL_CALL computeInstanceStagingSizeD3D12(
    PalDevice* device,
    uint32_t instanceCount,
    uint64_t* outSize)
{
    *outSize = sizeof(D3D12_RAYTRACING_INSTANCE_DESC) * instanceCount;
}

void PAL_CALL computeImageStagingRequirementsD3D12(
    PalDevice* device,
    PalFormat imageFormat,
    const PalBufferImageCopyInfo* copyInfo,
    PalImageStagingRequirements* requirements)
{
    uint32_t imageFormatSize = getFormatSizeD3D12(imageFormat);
    uint32_t rowPitch = _ALIGN((uint64_t)copyInfo->imageWidth * imageFormatSize, TEXTURE_PITCH);
    uint32_t bufferImageHeight = 0;
    if (copyInfo->bufferImageHeight) {
        bufferImageHeight = copyInfo->bufferImageHeight;
    } else {
        bufferImageHeight = copyInfo->imageHeight;
    }

    requirements->bufferRowLength = rowPitch;
    requirements->bufferImageHeight = bufferImageHeight;
    requirements->bufferSize = (uint64_t)rowPitch * bufferImageHeight * copyInfo->imageDepth;
}

void PAL_CALL writeInstanceStagingD3D12(
    PalDevice* device,
    uint32_t instanceCount,
    PalAccelerationStructureInstance* instances,
    void* ptr)
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
}

void PAL_CALL writeImageStagingD3D12(
    PalDevice* device,
    PalFormat imageFormat,
    PalBufferImageCopyInfo* copyInfo,
    void* srcData,
    void* ptr)
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
}

PalResult PAL_CALL bindBufferMemoryD3D12(
    PalBuffer* buffer,
    PalMemory* memory,
    uint64_t offset)
{
    HRESULT result;
    BufferD3D12* bufferImpl = (BufferD3D12*)buffer;
    DeviceD3D12* device = bufferImpl->device;
    MemoryD3D12* memoryImpl = (MemoryD3D12*)memory;
    if (bufferImpl->isMemoryManaged) {
        return PAL_RESULT_CODE_INVALID_OPERATION;
    }

    D3D12_RESOURCE_STATES state = 0;
    bufferImpl->canStateChange = PAL_TRUE;
    if (memoryImpl->type == PAL_MEMORY_TYPE_CPU_UPLOAD) {
        state = D3D12_RESOURCE_STATE_GENERIC_READ;
        bufferImpl->canStateChange = PAL_FALSE;

    } else if (memoryImpl->type == PAL_MEMORY_TYPE_CPU_UPLOAD) {
        state = D3D12_RESOURCE_STATE_COPY_DEST;
        bufferImpl->canStateChange = PAL_FALSE;
    }

    if (bufferImpl->usages & PAL_BUFFER_USAGE_ACCELERATION_STRUCTURE) {
        state = D3D12_RESOURCE_STATE_RAYTRACING_ACCELERATION_STRUCTURE;
        bufferImpl->canStateChange = PAL_FALSE;
    }

    if (bufferImpl->usages & PAL_BUFFER_USAGE_ACCELERATION_STRUCTURE_SCRATCH) {
        state = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
    }

    result = device->handle->lpVtbl->CreatePlacedResource(
        device->handle,
        memoryImpl->handle,
        offset,
        &bufferImpl->desc,
        state,
        nullptr,
        &IID_Resource,
        (void**)&bufferImpl->handle);

    if (FAILED(result)) {
        pollMessagesD3D12(device);
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
    BufferD3D12* bufferImpl = (BufferD3D12*)buffer;
    void* ptr = nullptr;
    HRESULT result = bufferImpl->handle->lpVtbl->Map(bufferImpl->handle, 0, nullptr, &ptr);
    if (FAILED(result)) {
        return makeResultD3D12(result);
    }

    *outPtr = (uint8_t*)ptr + offset;
    return PAL_RESULT_SUCCESS;
}

void PAL_CALL unmapBufferD3D12(PalBuffer* buffer)
{
    BufferD3D12* bufferImpl = (BufferD3D12*)buffer;
    bufferImpl->handle->lpVtbl->Unmap(bufferImpl->handle, 0, nullptr);
}

PalDeviceAddress PAL_CALL getBufferDeviceAddressD3D12(PalBuffer* buffer)
{
    BufferD3D12* bufferImpl = (BufferD3D12*)buffer;
    if (bufferImpl->usages & PAL_BUFFER_USAGE_DEVICE_ADDRESS) {
        return bufferImpl->handle->lpVtbl->GetGPUVirtualAddress(bufferImpl->handle);
    }
    return 0;
}

#endif // PAL_HAS_D3D12_BACKEND