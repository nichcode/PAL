
/**
    PAL - Prime Abstraction Layer
    Copyright (C) 2025
    Licensed under the Zlib license. See LICENSE file in root.
 */

#if PAL_HAS_VULKAN_BACKEND
#include "pal_vulkan.h"

PalResult PAL_CALL createAccelerationstructureVk(
    PalDevice* device,
    const PalAccelerationStructureCreateInfo* info,
    PalAccelerationStructure** outAs)
{
    VkResult result;
    AccelerationStructureVk* as = nullptr;
    DeviceVk* vkDevice = (DeviceVk*)device;
    BufferVk* buffer = (BufferVk*)info->buffer;
    
    if (!(vkDevice->features & PAL_ADAPTER_FEATURE_RAY_TRACING)) {
        return PAL_RESULT_CODE_FEATURE_NOT_SUPPORTED;
    }

    as = palAllocate(s_Vk.allocator, sizeof(AccelerationStructureVk), 0);
    if (!as) {
        return PAL_RESULT_CODE_OUT_OF_MEMORY;
    }

    VkAccelerationStructureCreateInfoKHR createInfo = {0};
    createInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_CREATE_INFO_KHR;
    createInfo.offset = (VkDeviceSize)info->offset;
    createInfo.size = (VkDeviceSize)info->size;
    createInfo.buffer = buffer->handle;
    createInfo.type = VK_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL_KHR;

    if (info->type == PAL_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL) {
        createInfo.type = VK_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL_KHR;
    }

    result = vkDevice->createAccelerationStructure(
        vkDevice->handle,
        &createInfo,
        &s_Vk.vkAllocator,
        &as->handle);

    if (result != VK_SUCCESS) {
        return makeResultVk(result);
    }

    // get and cache address
    VkAccelerationStructureDeviceAddressInfoKHR addressInfo = {0};
    addressInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_DEVICE_ADDRESS_INFO_KHR;
    addressInfo.accelerationStructure = as->handle;
    as->address = vkDevice->getAccelerationDeviceAddress(vkDevice->handle, &addressInfo);

    as->device = vkDevice;
    as->reserved = PAL_BACKEND_KEY;
    *outAs = (PalAccelerationStructure*)as;
    return PAL_RESULT_SUCCESS;
}

void PAL_CALL destroyAccelerationstructureVk(PalAccelerationStructure* as)
{
    AccelerationStructureVk* vkAs = (AccelerationStructureVk*)as;
    vkAs->device->destroyAccelerationStructure(
        vkAs->device->handle,
        vkAs->handle,
        &s_Vk.vkAllocator);

    palFree(s_Vk.allocator, vkAs);
}

PalResult PAL_CALL getAccelerationStructureBuildSizeVk(
    PalDevice* device,
    PalAccelerationStructureBuildInfo* info,
    PalAccelerationStructureBuildSize* size)
{
    uint32_t* maxPrimities = nullptr;
    VkAccelerationStructureGeometryKHR* geometries = nullptr;
    DeviceVk* vkDevice = (DeviceVk*)device;
    VkAccelerationStructureBuildGeometryInfoKHR buildInfo = {0};

    if (!(vkDevice->features & PAL_ADAPTER_FEATURE_RAY_TRACING)) {
        return PAL_RESULT_CODE_FEATURE_NOT_SUPPORTED;
    }

    // cache these for top level as
    VkAccelerationStructureGeometryKHR cachedGeometries = {0};
    uint32_t cachedPrimitives = 0;
    uint32_t geometryCount = info->count;
    if (info->type == PAL_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL) {
        geometryCount = 1;
        geometries = &cachedGeometries;
        maxPrimities = &cachedPrimitives;
    }

    if (info->type == PAL_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL) {
        geometries = palAllocate(
            s_Vk.allocator,
            sizeof(VkAccelerationStructureGeometryKHR) * geometryCount,
            0);

        maxPrimities = palAllocate(s_Vk.allocator, sizeof(uint32_t) * geometryCount, 0);
        if (!maxPrimities || !geometries) {
            return PAL_RESULT_CODE_OUT_OF_MEMORY;
        }

        memset(geometries, 0, sizeof(VkAccelerationStructureGeometryKHR) * geometryCount);
        memset(maxPrimities, 0, sizeof(uint32_t) * geometryCount);
    }

    fillBuildInfoVk(
        geometryCount, 
        info, 
        maxPrimities, 
        geometries, 
        nullptr, 
        nullptr, 
        nullptr, 
        &buildInfo);

    VkAccelerationStructureBuildSizesInfoKHR sizeInfo = {0};
    sizeInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_SIZES_INFO_KHR;

    vkDevice->getAccelerationBuildsize(
        vkDevice->handle,
        VK_ACCELERATION_STRUCTURE_BUILD_TYPE_DEVICE_KHR,
        &buildInfo,
        maxPrimities,
        &sizeInfo);

    size->accelerationStructureSize = sizeInfo.accelerationStructureSize;
    size->scratchBufferSize = sizeInfo.buildScratchSize;
    size->updateScratchBufferSize = sizeInfo.updateScratchSize;

    if (info->type == PAL_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL) {
        palFree(s_Vk.allocator, geometries);
        palFree(s_Vk.allocator, maxPrimities);
    }
    return PAL_RESULT_SUCCESS;
}

#endif // PAL_HAS_VULKAN_BACKEND