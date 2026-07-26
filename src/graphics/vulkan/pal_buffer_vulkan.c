
/**
    PAL - Prime Abstraction Layer
    Copyright (C) 2025
    Licensed under the Zlib license. See LICENSE file in root.
 */

#if PAL_HAS_VULKAN_BACKEND
#include "pal_vulkan.h"

static VkBufferUsageFlags bufferUsageToVk(PalBufferUsages usages)
{
    VkBufferUsageFlags flags = 0;
    if (usages & PAL_BUFFER_USAGE_VERTEX) {
        flags |= VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
    }

    if (usages & PAL_BUFFER_USAGE_INDEX) {
        flags |= VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
    }

    if (usages & PAL_BUFFER_USAGE_UNIFORM) {
        flags |= VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
    }

    if (usages & PAL_BUFFER_USAGE_STORAGE) {
        flags |= VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
    }

    if (usages & PAL_BUFFER_USAGE_TRANSFER_SRC) {
        flags |= VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
    }

    if (usages & PAL_BUFFER_USAGE_TRANSFER_DST) {
        flags |= VK_BUFFER_USAGE_TRANSFER_DST_BIT;
    }

    if (usages & PAL_BUFFER_USAGE_ACCELERATION_STRUCTURE) {
        flags |= VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_STORAGE_BIT_KHR;
    }

    if (usages & PAL_BUFFER_USAGE_ACCELERATION_STRUCTURE_SCRATCH) {
        flags |= VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
    }

    if (usages & PAL_BUFFER_USAGE_ACCELERATION_STRUCTURE_READ_ONLY_INPUT) {
        flags |= VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_BUILD_INPUT_READ_ONLY_BIT_KHR;
    }

    if (usages & PAL_BUFFER_USAGE_DEVICE_ADDRESS) {
        flags |= VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT;
    }

    if (usages & PAL_BUFFER_USAGE_INDIRECT) {
        flags |= VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT;
    }

    return flags;
}

static uint32_t getFormatSize(PalFormat format)
{
    switch (format) {
        case PAL_FORMAT_R8_UNORM:
        case PAL_FORMAT_R8_SNORM:
        case PAL_FORMAT_R8_UINT:
        case PAL_FORMAT_R8_SINT:
        case PAL_FORMAT_R8_SRGB:
        case PAL_FORMAT_S8_UINT:
            return 1;

        case PAL_FORMAT_R16_UNORM:
        case PAL_FORMAT_R16_SNORM:
        case PAL_FORMAT_R16_UINT:
        case PAL_FORMAT_R16_SINT:
        case PAL_FORMAT_R16_SFLOAT:
        case PAL_FORMAT_R8G8_UNORM:
        case PAL_FORMAT_R8G8_SNORM:
        case PAL_FORMAT_R8G8_UINT:
        case PAL_FORMAT_R8G8_SINT:
        case PAL_FORMAT_R8G8_SRGB:
        case PAL_FORMAT_D16_UNORM:
            return 2;

        case PAL_FORMAT_R8G8B8_UNORM:
        case PAL_FORMAT_R8G8B8_SNORM:
        case PAL_FORMAT_R8G8B8_UINT:
        case PAL_FORMAT_R8G8B8_SINT:
        case PAL_FORMAT_R8G8B8_SRGB:
        case PAL_FORMAT_B8G8R8_UNORM:
        case PAL_FORMAT_B8G8R8_SNORM:
        case PAL_FORMAT_B8G8R8_UINT:
        case PAL_FORMAT_B8G8R8_SINT:
        case PAL_FORMAT_B8G8R8_SRGB:
        case PAL_FORMAT_D16_UNORM_S8_UINT:
            return 3;

        case PAL_FORMAT_R32_UINT:
        case PAL_FORMAT_R32_SINT:
        case PAL_FORMAT_R32_SFLOAT:
        case PAL_FORMAT_R16G16_UNORM:
        case PAL_FORMAT_R16G16_SNORM:
        case PAL_FORMAT_R16G16_UINT:
        case PAL_FORMAT_R16G16_SINT:
        case PAL_FORMAT_R16G16_SFLOAT:
        case PAL_FORMAT_R8G8B8A8_UNORM:
        case PAL_FORMAT_R8G8B8A8_SNORM:
        case PAL_FORMAT_R8G8B8A8_UINT:
        case PAL_FORMAT_R8G8B8A8_SINT:
        case PAL_FORMAT_R8G8B8A8_SRGB:
        case PAL_FORMAT_B8G8R8A8_UNORM:
        case PAL_FORMAT_B8G8R8A8_SNORM:
        case PAL_FORMAT_B8G8R8A8_UINT:
        case PAL_FORMAT_B8G8R8A8_SINT:
        case PAL_FORMAT_B8G8R8A8_SRGB:
        case PAL_FORMAT_D32_SFLOAT:
        case PAL_FORMAT_D24_UNORM_S8_UINT:
            return 4;

        case PAL_FORMAT_D32_SFLOAT_S8_UINT:
            return 5;

        case PAL_FORMAT_R16G16B16_UNORM:
        case PAL_FORMAT_R16G16B16_SNORM:
        case PAL_FORMAT_R16G16B16_UINT:
        case PAL_FORMAT_R16G16B16_SINT:
        case PAL_FORMAT_R16G16B16_SFLOAT:
            return 6;

        case PAL_FORMAT_R64_UINT:
        case PAL_FORMAT_R64_SINT:
        case PAL_FORMAT_R64_SFLOAT:
        case PAL_FORMAT_R32G32_UINT:
        case PAL_FORMAT_R32G32_SINT:
        case PAL_FORMAT_R32G32_SFLOAT:
        case PAL_FORMAT_R16G16B16A16_UNORM:
        case PAL_FORMAT_R16G16B16A16_SNORM:
        case PAL_FORMAT_R16G16B16A16_UINT:
        case PAL_FORMAT_R16G16B16A16_SINT:
        case PAL_FORMAT_R16G16B16A16_SFLOAT:
            return 8;

        case PAL_FORMAT_R32G32B32_UINT:
        case PAL_FORMAT_R32G32B32_SINT:
        case PAL_FORMAT_R32G32B32_SFLOAT:
            return 12;

        case PAL_FORMAT_R64G64_UINT:
        case PAL_FORMAT_R64G64_SINT:
        case PAL_FORMAT_R64G64_SFLOAT:
        case PAL_FORMAT_R32G32B32A32_UINT:
        case PAL_FORMAT_R32G32B32A32_SINT:
        case PAL_FORMAT_R32G32B32A32_SFLOAT:
            return 16;

        case PAL_FORMAT_R64G64B64_UINT:
        case PAL_FORMAT_R64G64B64_SINT:
        case PAL_FORMAT_R64G64B64_SFLOAT:
            return 24;

        case PAL_FORMAT_R64G64B64A64_UINT:
        case PAL_FORMAT_R64G64B64A64_SINT:
        case PAL_FORMAT_R64G64B64A64_SFLOAT:
            return 32;
    }

    return 0;
}

static VkGeometryInstanceFlagsKHR instanceFlagsToVk(PalAccelerationStructureInstanceFlags flags)
{
    VkGeometryInstanceFlagsKHR instanceFlags = 0;
    if (flags & PAL_ACCELERATION_STRUCTURE_INSTANCE_FLAG_FORCE_OPAQUE) {
        instanceFlags |= VK_GEOMETRY_INSTANCE_FORCE_OPAQUE_BIT_KHR;
    }

    if (flags & PAL_ACCELERATION_STRUCTURE_INSTANCE_FLAG_FORCE_NO_OPAQUE) {
        instanceFlags |= VK_GEOMETRY_INSTANCE_FORCE_NO_OPAQUE_BIT_KHR;
    }

    if (flags & PAL_ACCELERATION_STRUCTURE_INSTANCE_FLAG_TRIANGLE_FACING_CULL_DISABLE) {
        instanceFlags |= VK_GEOMETRY_INSTANCE_TRIANGLE_FACING_CULL_DISABLE_BIT_KHR;
    }

    if (flags & PAL_ACCELERATION_STRUCTURE_INSTANCE_FLAG_TRIANGLE_FRONT_COUNTERCLOCKWISE) {
        instanceFlags |= VK_GEOMETRY_INSTANCE_TRIANGLE_FRONT_COUNTERCLOCKWISE_BIT_KHR;
    }

    return instanceFlags;
}

PalResult PAL_CALL createBufferVk(
    PalDevice* device,
    const PalBufferCreateInfo* info,
    PalBuffer** outBuffer)
{
    VkResult result;
    BufferVk* buffer = nullptr;
    DeviceVk* deviceImpl = (DeviceVk*)device;
    MemoryVk* memory = nullptr;

    buffer = palAllocate(s_Vk.allocator, sizeof(BufferVk), 0);
    if (!buffer) {
        return PAL_RESULT_CODE_OUT_OF_MEMORY;
    }

    if (info->memoryUsage != PAL_BUFFER_MEMORY_USAGE_MANUAL) {
        memory = palAllocate(s_Vk.allocator, sizeof(MemoryVk), 0);
        if (!memory) {
            return PAL_RESULT_CODE_OUT_OF_MEMORY;
        }
    }

    VkBufferCreateInfo createInfo = {0};
    createInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    createInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    createInfo.size = info->size;
    createInfo.usage = bufferUsageToVk(info->usages);

    result =
        s_Vk.createBuffer(deviceImpl->handle, &createInfo, &s_Vk.allocatorImpl, &buffer->handle);
    if (result != VK_SUCCESS) {
        return makeResultVk(result);
    }

    buffer->isMemoryManaged = PAL_FALSE;
    if (info->memoryUsage != PAL_BUFFER_MEMORY_USAGE_MANUAL) {
        PalMemoryType memoryType = PAL_MEMORY_TYPE_GPU_ONLY;
        if (info->memoryUsage == PAL_BUFFER_MEMORY_USAGE_AUTO_CPU_UPLOAD) {
            memoryType = PAL_MEMORY_TYPE_CPU_UPLOAD;

        } else if (info->memoryUsage == PAL_BUFFER_MEMORY_USAGE_AUTO_CPU_READBACK) {
            memoryType = PAL_MEMORY_TYPE_CPU_READBACK;
        }

        // allocate and manage memory
        VkMemoryRequirements memReq = {0};
        s_Vk.getBufferMemoryRequirements(deviceImpl->handle, buffer->handle, &memReq);

        VkMemoryAllocateInfo allocateInfo = {0};
        allocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocateInfo.allocationSize = (VkDeviceSize)memReq.size;
        VkMemoryAllocateFlagsInfo allocateFlagsInfo = {0};

        uint32_t memoryMask = deviceImpl->memoryClassMask[memoryType] & memReq.memoryTypeBits;
        uint32_t memoryIndex = findBestMemoryIndexVk(deviceImpl->phyDevice, memoryMask);
        if (!(memoryMask & (1u << memoryIndex))) {
            return PAL_RESULT_CODE_PLATFORM_FAILURE;
        }

        allocateInfo.memoryTypeIndex = memoryIndex;
        if (info->usages & PAL_BUFFER_USAGE_DEVICE_ADDRESS) {
            allocateFlagsInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_FLAGS_INFO;
            allocateFlagsInfo.flags = VK_MEMORY_ALLOCATE_DEVICE_ADDRESS_BIT_KHR;
            allocateInfo.pNext = &allocateFlagsInfo;
        }

        result = s_Vk.allocateMemory(
            deviceImpl->handle,
            &allocateInfo,
            &s_Vk.allocatorImpl,
            &memory->handle);

        if (result != VK_SUCCESS) {
            return makeResultVk(result);
        }

        result = s_Vk.bindBufferMemory(deviceImpl->handle, buffer->handle, memory->handle, 0);
        if (result != VK_SUCCESS) {
            return makeResultVk(result);
        }

        memory->type = memoryType;
        buffer->isMemoryManaged = PAL_TRUE;
    }

    buffer->memory = memory;
    buffer->usages = info->usages;
    buffer->device = deviceImpl;
    *outBuffer = (PalBuffer*)buffer;
    return PAL_RESULT_SUCCESS;
}

void PAL_CALL destroyBufferVk(PalBuffer* buffer)
{
    BufferVk* bufferImpl = (BufferVk*)buffer;
    s_Vk.destroyBuffer(bufferImpl->device->handle, bufferImpl->handle, &s_Vk.allocatorImpl);
    if (bufferImpl->isMemoryManaged) {
        s_Vk.freeMemory(
            bufferImpl->device->handle,
            bufferImpl->memory->handle,
            &s_Vk.allocatorImpl);
        palFree(s_Vk.allocator, bufferImpl->memory);
    }
    palFree(s_Vk.allocator, buffer);
}

void PAL_CALL getBufferMemoryRequirementsVk(
    PalBuffer* buffer,
    PalMemoryRequirements* requirements)
{
    BufferVk* bufferImpl = (BufferVk*)buffer;
    DeviceVk* device = bufferImpl->device;
    VkMemoryRequirements memReq = {0};
    s_Vk.getBufferMemoryRequirements(device->handle, bufferImpl->handle, &memReq);

    requirements->alignment = (uint64_t)memReq.alignment;
    requirements->size = (uint64_t)memReq.size;
    requirements->memoryMask = palPackUint32(memReq.memoryTypeBits, bufferImpl->usages);
    requirements->supportedMemoryTypes = 0;

    if ((memReq.memoryTypeBits & device->memoryClassMask[PAL_MEMORY_TYPE_GPU_ONLY]) != 0) {
        requirements->supportedMemoryTypes |= (1u << PAL_MEMORY_TYPE_GPU_ONLY);
    }

    if ((memReq.memoryTypeBits & device->memoryClassMask[PAL_MEMORY_TYPE_CPU_UPLOAD]) != 0) {
        requirements->supportedMemoryTypes |= (1u << PAL_MEMORY_TYPE_CPU_UPLOAD);
    }

    if ((memReq.memoryTypeBits & device->memoryClassMask[PAL_MEMORY_TYPE_CPU_READBACK]) != 0) {
        requirements->supportedMemoryTypes |= (1u << PAL_MEMORY_TYPE_CPU_READBACK);
    }
}

void PAL_CALL computeInstanceStagingSizeVk(
    PalDevice* device,
    uint32_t instanceCount,
    uint64_t* outSize)
{
    *outSize = sizeof(VkAccelerationStructureInstanceKHR) * instanceCount;
}

void PAL_CALL computeImageStagingRequirementsVk(
    PalDevice* device,
    PalFormat imageFormat,
    const PalBufferImageCopyInfo* copyInfo,
    PalImageStagingRequirements* requirements)
{
    uint32_t imageFormatSize = getFormatSize(imageFormat);
    uint32_t length = 0;
    uint32_t height = 0;
    length = copyInfo->bufferRowLength ? copyInfo->bufferRowLength : copyInfo->imageWidth;
    height = copyInfo->bufferImageHeight ? copyInfo->bufferImageHeight : copyInfo->imageHeight;
    uint32_t rowPitch = length * imageFormatSize;

    requirements->bufferRowLength = length;
    requirements->bufferImageHeight = height;
    requirements->bufferSize = (uint64_t)rowPitch * length * copyInfo->imageDepth;
}

void PAL_CALL writeInstanceStagingVk(
    PalDevice* device,
    uint32_t instanceCount,
    PalAccelerationStructureInstance* instances,
    void* ptr)
{
    VkAccelerationStructureInstanceKHR* data = ptr;
    for (int i = 0; i < instanceCount; i++) {
        PalAccelerationStructureInstance* src = &instances[i];
        VkAccelerationStructureInstanceKHR* dst = &data[i];
        AccelerationStructureVk* as = (AccelerationStructureVk*)src->blas;

        dst->mask = src->mask & 0xFF;
        dst->instanceCustomIndex = src->instanceId & 0xFFFFFF;
        dst->accelerationStructureReference = as->address;
        dst->instanceShaderBindingTableRecordOffset = src->hitGroupOffset & 0xFFFFFF;
        dst->flags = instanceFlagsToVk(src->flags);
        memcpy(dst->transform.matrix, src->transform, sizeof(float) * 12);
    }
}

void PAL_CALL writeImageStagingVk(
    PalDevice* device,
    PalFormat imageFormat,
    PalBufferImageCopyInfo* copyInfo,
    void* srcData,
    void* ptr)
{
    uint32_t imageFormatSize = getFormatSize(imageFormat);
    uint32_t dstRowPitch = copyInfo->bufferRowLength * imageFormatSize;
    uint32_t srcRowPitch = copyInfo->imageWidth * imageFormatSize;
    const uint32_t dstSlicePitch = dstRowPitch * copyInfo->bufferImageHeight;
    const uint32_t srcSlicePitch = srcRowPitch * copyInfo->imageHeight;

    // manually offset the buffer with the provided offset
    uint8_t* dst = (uint8_t*)ptr + copyInfo->bufferOffset;
    const uint8_t* src = (const uint8_t*)srcData;

    // write to destination pointer
    for (uint32_t z = 0; z < copyInfo->imageDepth; z++) {
        for (uint32_t y = 0; y < copyInfo->imageHeight; y++) {
            memcpy(
                dst + z * dstSlicePitch + y * dstRowPitch,
                src + z * srcSlicePitch + y * srcRowPitch,
                srcRowPitch);
        }
    }
}

PalResult PAL_CALL bindBufferMemoryVk(
    PalBuffer* buffer,
    PalMemory* memory,
    uint64_t offset)
{
    VkResult result;
    MemoryVk* memoryImpl = (MemoryVk*)memory;
    BufferVk* bufferImpl = (BufferVk*)buffer;

    if (bufferImpl->memory) {
        return PAL_RESULT_CODE_INVALID_OPERATION;
    }

    result = s_Vk.bindBufferMemory(
        bufferImpl->device->handle,
        bufferImpl->handle,
        memoryImpl->handle,
        offset);

    if (result != VK_SUCCESS) {
        return makeResultVk(result);
    }

    bufferImpl->memory = memoryImpl;
    return PAL_RESULT_SUCCESS;
}

PalResult PAL_CALL mapBufferVk(
    PalBuffer* buffer,
    uint64_t offset,
    uint64_t size,
    void** outPtr)
{
    VkResult result;
    BufferVk* bufferImpl = (BufferVk*)buffer;
    DeviceVk* device = bufferImpl->device;

    result = s_Vk.mapMemory(device->handle, bufferImpl->memory->handle, offset, size, 0, outPtr);
    if (result != VK_SUCCESS) {
        return makeResultVk(result);
    }
    return PAL_RESULT_SUCCESS;
}

void PAL_CALL unmapBufferVk(PalBuffer* buffer)
{
    BufferVk* bufferImpl = (BufferVk*)buffer;
    s_Vk.unmapMemory(bufferImpl->device->handle, bufferImpl->memory->handle);
}

PalDeviceAddress PAL_CALL getBufferDeviceAddressVk(PalBuffer* buffer)
{
    BufferVk* bufferImpl = (BufferVk*)buffer;
    if (!(bufferImpl->usages & PAL_BUFFER_USAGE_DEVICE_ADDRESS)) {
        return 0;
    }

    VkBufferDeviceAddressInfoKHR bufferInfo = {0};
    bufferInfo.buffer = bufferImpl->handle;
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO_KHR;
    return bufferImpl->device->getBufferrAddress(bufferImpl->device->handle, &bufferInfo);
}

#endif // PAL_HAS_VULKAN_BACKEND