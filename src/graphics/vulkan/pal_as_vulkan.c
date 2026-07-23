
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
    DeviceVk* deviceImpl = (DeviceVk*)device;
    BufferVk* buffer = (BufferVk*)info->buffer;

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

    result = deviceImpl->createAccelerationStructure(
        deviceImpl->handle,
        &createInfo,
        &s_Vk.allocatorImpl,
        &as->handle);

    if (result != VK_SUCCESS) {
        return makeResultVk(result);
    }

    // get and cache address
    VkAccelerationStructureDeviceAddressInfoKHR addressInfo = {0};
    addressInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_DEVICE_ADDRESS_INFO_KHR;
    addressInfo.accelerationStructure = as->handle;
    as->address = deviceImpl->getAccelerationDeviceAddress(deviceImpl->handle, &addressInfo);

    as->device = deviceImpl;
    *outAs = (PalAccelerationStructure*)as;
    return PAL_RESULT_SUCCESS;
}

void PAL_CALL destroyAccelerationstructureVk(PalAccelerationStructure* as)
{
    AccelerationStructureVk* asImpl = (AccelerationStructureVk*)as;
    asImpl->device->destroyAccelerationStructure(
        asImpl->device->handle,
        asImpl->handle,
        &s_Vk.allocatorImpl);

    palFree(s_Vk.allocator, asImpl);
}

void PAL_CALL getAccelerationStructureBuildSizeVk(
    PalDevice* device,
    PalAccelerationStructureBuildInfo* info,
    PalAccelerationStructureBuildSize* size)
{
    DeviceVk* deviceImpl = (DeviceVk*)device;
    VkAccelerationStructureGeometryKHR* geometries = nullptr;
    uint32_t* maxPrimities = nullptr;
    VkAccelerationStructureBuildGeometryInfoKHR buildInfo = {0};
    uint32_t geometriesSize = sizeof(VkAccelerationStructureGeometryKHR) * info->count;

    geometries = palAllocate(s_Vk.allocator, geometriesSize, 0);
    maxPrimities = palAllocate(s_Vk.allocator, sizeof(uint32_t) * info->count, 0);
    if (!maxPrimities || !geometries) {
        return;
    }

    memset(geometries, 0, geometriesSize);
    memset(maxPrimities, 0, sizeof(uint32_t) * info->count);
    fillBuildInfoVk(PAL_TRUE, info, geometries, &buildInfo, maxPrimities);

    VkAccelerationStructureBuildSizesInfoKHR sizeInfo = {0};
    sizeInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_SIZES_INFO_KHR;
    deviceImpl->getAccelerationBuildsize(
        deviceImpl->handle,
        VK_ACCELERATION_STRUCTURE_BUILD_TYPE_DEVICE_KHR,
        &buildInfo,
        maxPrimities,
        &sizeInfo);

    size->accelerationStructureSize = sizeInfo.accelerationStructureSize;
    size->scratchBufferSize = sizeInfo.buildScratchSize;
    size->updateScratchBufferSize = sizeInfo.updateScratchSize;

    palFree(s_Vk.allocator, geometries);
    palFree(s_Vk.allocator, maxPrimities);
}

#endif // PAL_HAS_VULKAN_BACKEND