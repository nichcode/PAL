
/**
    PAL - Prime Abstraction Layer
    Copyright (C) 2025
    Licensed under the Zlib license. See LICENSE file in root.
 */

#if PAL_HAS_VULKAN_BACKEND
#include "pal_vulkan.h"

#define align(v, a) (v + a - 1) & ~(a - 1)

PalResult PAL_CALL createShaderBindingTableVk(
    PalDevice* device,
    const PalShaderBindingTableCreateInfo* info,
    PalShaderBindingTable** outSbt)
{
    VkResult result;
    DeviceVk* vkDevice = (DeviceVk*)device;
    ShaderBindingTableVk* sbt = nullptr;
    PipelineVk* pipeline = (PipelineVk*)info->rayTracingPipeline;
    ShaderBindingTableInfo* sbtInfo = &pipeline->sbtInfo;

    uint32_t totalGroups = sbtInfo->raygenCount + sbtInfo->hitCount;
    totalGroups += sbtInfo->missCount + sbtInfo->callableCount;
    if (info->recordCount != totalGroups) {
        return PAL_RESULT_CODE_INVALID_ARGUMENT;
    }

    sbt = palAllocate(s_Vk.allocator, sizeof(ShaderBindingTableVk), 0);
    if (!sbt) {
        return PAL_RESULT_CODE_OUT_OF_MEMORY;
    }

    // create SBT buffer
    VkPhysicalDeviceRayTracingPipelinePropertiesKHR rayProps = {0};
    rayProps.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_TRACING_PIPELINE_PROPERTIES_KHR;

    VkPhysicalDeviceProperties2KHR props = {0};
    props.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2_KHR;
    props.pNext = &rayProps;
    s_Vk.getPhysicalDeviceProperties2(vkDevice->phyDevice, &props);

    uint32_t groupHandleSize = rayProps.shaderGroupHandleSize;
    uint32_t groupHandleAlignment = rayProps.shaderGroupHandleAlignment;
    uint32_t groupBaseAlignment = rayProps.shaderGroupBaseAlignment;

    // get the max local data size
    for (int i = 0; i < info->recordCount; i++) {
        PalShaderBindingTableRecordInfo* record = &info->records[i];
        uint32_t index = record->groupIndex;

        if (index < sbtInfo->raygenCount) {
            // raygen group
            if (record->localDataSize > sbtInfo->raygenDataSize) {
                return PAL_RESULT_CODE_INVALID_ARGUMENT;
            }

        } else if (index < sbtInfo->raygenCount + sbtInfo->missCount) {
            // miss group
            if (record->localDataSize > sbtInfo->missDataSize) {
                return PAL_RESULT_CODE_INVALID_ARGUMENT;
            }

        } else if (index < sbtInfo->raygenCount + sbtInfo->missCount + sbtInfo->hitCount) {
            // hit group
            if (record->localDataSize > sbtInfo->hitDataSize) {
                return PAL_RESULT_CODE_INVALID_ARGUMENT;
            }

        } else {
            // callable group
            if (record->localDataSize > sbtInfo->callableDataSize) {
                return PAL_RESULT_CODE_INVALID_ARGUMENT;
            }
        }
    }

    // get strides
    uint32_t callableStride = 0;
    uint32_t raygenStride = align(groupHandleSize + sbtInfo->raygenDataSize, groupHandleAlignment);
    uint32_t missStride = align(groupHandleSize + sbtInfo->missDataSize, groupHandleAlignment);
    uint32_t hitStride = align(groupHandleSize + sbtInfo->hitDataSize, groupHandleAlignment);
    callableStride = align(groupHandleSize + sbtInfo->callableDataSize, groupHandleAlignment);

    // get region size
    uint32_t raygenRegionSize = raygenStride * sbtInfo->raygenCount;
    uint32_t missRegionSize = missStride * sbtInfo->missCount;
    uint32_t hitRegionSize = hitStride * sbtInfo->hitCount;
    uint32_t callableRegionSize = callableStride * sbtInfo->callableCount;

    // get offsets
    uint32_t offset = 0;
    uint32_t raygenOffset = 0;
    uint32_t missOffset = 0;
    uint32_t hitOffset = 0;
    uint32_t callableOffset = 0;

    raygenOffset = align(offset, groupBaseAlignment);
    offset = raygenOffset + raygenRegionSize;

    missOffset = align(offset, groupBaseAlignment);
    offset = missOffset + missRegionSize;

    hitOffset = align(offset, groupBaseAlignment);
    offset = hitOffset + hitRegionSize;

    callableOffset = align(offset, groupBaseAlignment);
    offset = callableOffset + callableRegionSize;

    uint32_t bufferSize = align(offset, groupBaseAlignment);

    // create gpu buffer
    VkBufferCreateInfo bufCreateInfo = {0};
    bufCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufCreateInfo.size = bufferSize;
    bufCreateInfo.usage = VK_BUFFER_USAGE_SHADER_BINDING_TABLE_BIT_KHR;
    bufCreateInfo.usage |= VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT_KHR;
    bufCreateInfo.usage |= VK_BUFFER_USAGE_TRANSFER_DST_BIT;
    bufCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    result = s_Vk.createBuffer(vkDevice->handle, &bufCreateInfo, &s_Vk.vkAllocator, &sbt->buffer);
    if (result != VK_SUCCESS) {
        return makeResultVk(result);
    }

    // create staging buffer
    bufCreateInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
    sbt->stagingBufferSize = bufferSize;
    result = s_Vk.createBuffer(
        vkDevice->handle, 
        &bufCreateInfo, 
        &s_Vk.vkAllocator, 
        &sbt->stagingBuffer);

    if (result != VK_SUCCESS) {
        return makeResultVk(result);
    }

    // allocate CPU upload memory and bind
    VkMemoryRequirements memReq = {0};
    s_Vk.getBufferMemoryRequirements(vkDevice->handle, sbt->buffer, &memReq);

    VkMemoryAllocateInfo allocateInfo = {0};
    allocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocateInfo.allocationSize = memReq.size;

    uint32_t mask = vkDevice->memoryClassMask[PAL_MEMORY_TYPE_GPU_ONLY] & memReq.memoryTypeBits;
    uint32_t memoryIndex = findBestMemoryIndexVk(vkDevice->phyDevice, mask);
    allocateInfo.memoryTypeIndex = memoryIndex;

    VkMemoryAllocateFlagsInfo allocateFlagsInfo = {0};
    allocateFlagsInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_FLAGS_INFO;
    allocateFlagsInfo.flags = VK_MEMORY_ALLOCATE_DEVICE_ADDRESS_BIT_KHR;
    allocateInfo.pNext = &allocateFlagsInfo;

    result = s_Vk.allocateMemory(
        vkDevice->handle,
        &allocateInfo,
        &s_Vk.vkAllocator,
        &sbt->bufferMemory);

    if (result != VK_SUCCESS) {
        return makeResultVk(result);
    }

    // allocate memory for staging buffer
    s_Vk.getBufferMemoryRequirements(vkDevice->handle, sbt->stagingBuffer, &memReq);
    allocateInfo.allocationSize = memReq.size;

    mask = vkDevice->memoryClassMask[PAL_MEMORY_TYPE_CPU_UPLOAD] & memReq.memoryTypeBits;
    memoryIndex = findBestMemoryIndexVk(vkDevice->phyDevice, mask);
    allocateInfo.memoryTypeIndex = memoryIndex;
    allocateInfo.pNext = nullptr; // we dont need the address

    result = s_Vk.allocateMemory(
        vkDevice->handle,
        &allocateInfo,
        &s_Vk.vkAllocator,
        &sbt->stagingBufferMemory);

    if (result != VK_SUCCESS) {
        return makeResultVk(result);
    }

    s_Vk.bindBufferMemory(vkDevice->handle, sbt->buffer, sbt->bufferMemory, 0);
    s_Vk.bindBufferMemory(vkDevice->handle, sbt->stagingBuffer, sbt->stagingBufferMemory, 0);

    // get shader group handles
    uint32_t handlesSize = totalGroups * groupHandleSize;
    uint8_t* handles = palAllocate(s_Vk.allocator, handlesSize, 0);
    if (!handles) {
        return PAL_RESULT_CODE_OUT_OF_MEMORY;
    }

    result = vkDevice->getRayTracingShaderGroupHandles(
        vkDevice->handle,
        pipeline->handle,
        0,
        totalGroups,
        handlesSize,
        handles);

    if (result != VK_SUCCESS) {
        return makeResultVk(result);
    }

    // copy handles into the buffer
    void* ptr = nullptr;
    result = s_Vk.mapMemory(vkDevice->handle, sbt->stagingBufferMemory, 0, VK_WHOLE_SIZE, 0, &ptr);
    if (result != VK_SUCCESS) {
        return makeResultVk(result);
    }

    offset = 0; // reuse variable
    uint8_t* srcPtr = (uint8_t*)handles;

    // raygen
    for (int i = 0; i < sbtInfo->raygenCount; i++) {
        uint8_t* dstPtr = (uint8_t*)ptr + (i * raygenStride);
        PalShaderBindingTableRecordInfo* record = &info->records[offset + i];

        memcpy(dstPtr, srcPtr + (i * groupHandleSize), groupHandleSize);
        if (record->localDataSize) {
            // this record has local data
            memcpy(dstPtr + groupHandleSize, record->localData, record->localDataSize);
        }
    }

    offset += sbtInfo->raygenCount;
    srcPtr += (groupHandleSize * sbtInfo->raygenCount);

    // miss
    for (int i = 0; i < sbtInfo->missCount; i++) {
        uint8_t* dstPtr = (uint8_t*)ptr + missOffset + (i * missStride);
        PalShaderBindingTableRecordInfo* record = &info->records[offset + i];

        memcpy(dstPtr, srcPtr + (i * groupHandleSize), groupHandleSize);
        if (record->localDataSize) {
            // this record has local data
            memcpy(dstPtr + groupHandleSize, record->localData, record->localDataSize);
        }
    }
    
    offset += sbtInfo->missCount;
    srcPtr += (groupHandleSize * sbtInfo->missCount);

    // hit
    for (int i = 0; i < sbtInfo->hitCount; i++) {
        uint8_t* dstPtr = (uint8_t*)ptr + hitOffset + (i * hitStride);
        PalShaderBindingTableRecordInfo* record = &info->records[offset + i];

        memcpy(dstPtr, srcPtr + (i * groupHandleSize), groupHandleSize);
        if (record->localDataSize) {
            // this record has local data
            memcpy(dstPtr + groupHandleSize, record->localData, record->localDataSize);
        }
    }
    
    offset += sbtInfo->hitCount;
    srcPtr += (groupHandleSize * sbtInfo->hitCount);

    // callable
    for (int i = 0; i < sbtInfo->callableCount; i++) {
        uint8_t* dstPtr = (uint8_t*)ptr + callableOffset + (i * callableStride);
        PalShaderBindingTableRecordInfo* record = &info->records[offset + i];

        memcpy(dstPtr, srcPtr + (i * groupHandleSize), groupHandleSize);
        if (record->localDataSize) {
            // this record has local data
            memcpy(dstPtr + groupHandleSize, record->localData, record->localDataSize);
        }
    }

    sbt->stagingPtr = ptr;
    // cache SBT fields and offsets address
    // we know the layout so we can prepare the strided address before we copy to the gpu buffer
    VkBufferDeviceAddressInfo bufferAddressInfo = {0};
    bufferAddressInfo.buffer = sbt->buffer;
    bufferAddressInfo.sType = VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO;
    sbt->baseAddress = s_Vk.getBufferDeviceAddress(vkDevice->handle, &bufferAddressInfo);

    // raygen
    sbt->raygen.region.deviceAddress = sbt->baseAddress;
    sbt->raygen.offset = 0; // always
    sbt->raygen.startIndex = 0; // always
    sbt->raygen.region.size = raygenRegionSize;
    sbt->raygen.region.stride = raygenStride;

    // miss
    sbt->miss.offset = missOffset;
    sbt->miss.startIndex = sbtInfo->raygenCount;
    sbt->miss.region.deviceAddress = sbt->baseAddress + missOffset;
    sbt->miss.region.size = missRegionSize;
    sbt->miss.region.stride = missStride;

    // hit
    sbt->hit.offset = hitOffset;
    sbt->hit.startIndex = sbtInfo->raygenCount + sbtInfo->missCount;
    sbt->hit.region.deviceAddress = sbt->baseAddress + hitOffset;
    sbt->hit.region.size = hitRegionSize;
    sbt->hit.region.stride = hitStride;

    // callable
    sbt->callable.offset = callableOffset;
    sbt->callable.startIndex = sbtInfo->raygenCount + sbtInfo->missCount + sbtInfo->hitCount;
    sbt->callable.region.deviceAddress = sbt->baseAddress + callableOffset;
    sbt->callable.region.size = callableRegionSize;
    sbt->callable.region.stride = callableStride;

    palFree(s_Vk.allocator, handles);
    sbt->device = vkDevice;
    sbt->handleSize = groupHandleSize;
    sbt->pipeline = pipeline;

    sbt->isDirty = PAL_TRUE; // we need to copy from the staging to the gpu buffer
    *outSbt = (PalShaderBindingTable*)sbt;
    return PAL_RESULT_SUCCESS;
}

void PAL_CALL destroyShaderBindingTableVk(PalShaderBindingTable* sbt)
{
    ShaderBindingTableVk* vkSbt = (ShaderBindingTableVk*)sbt;
    DeviceVk* device = vkSbt->device;
    s_Vk.unmapMemory(device->handle, vkSbt->stagingBufferMemory);

    s_Vk.destroyBuffer(device->handle, vkSbt->buffer, &s_Vk.vkAllocator);
    s_Vk.destroyBuffer(device->handle, vkSbt->stagingBuffer, &s_Vk.vkAllocator);
    s_Vk.freeMemory(device->handle, vkSbt->bufferMemory, &s_Vk.vkAllocator);
    s_Vk.freeMemory(device->handle, vkSbt->stagingBufferMemory, &s_Vk.vkAllocator);
    palFree(s_Vk.allocator, vkSbt);
}

void PAL_CALL updateShaderBindingTableVk(
    PalShaderBindingTable* sbt, 
    uint32_t count,
    PalShaderBindingTableRecordInfo* infos)
{
    ShaderBindingTableVk* vkSbt = (ShaderBindingTableVk*)sbt;
    DeviceVk* vkDevice = vkSbt->device;
    PipelineVk* pipeline = vkSbt->pipeline;
    ShaderBindingTableInfo* sbtInfo = &pipeline->sbtInfo;

    uint32_t stride = 0;
    uint32_t offset = 0;
    uint32_t startIndex = 0;

    for (int i = 0; i < count; i++) {
        PalShaderBindingTableRecordInfo* info = &infos[i];

        // find the group the record belongs to
        uint32_t index = info->groupIndex;
        if (index < sbtInfo->raygenCount) {
            // raygen group
            offset = 0;
            stride = vkSbt->raygen.region.stride;
            startIndex = vkSbt->raygen.startIndex;

        } else if (index < sbtInfo->raygenCount + sbtInfo->missCount) {
            // miss group
            offset = vkSbt->miss.offset;
            stride = vkSbt->miss.region.stride;
            startIndex = vkSbt->miss.startIndex;

        } else if (index < sbtInfo->raygenCount + sbtInfo->missCount + sbtInfo->hitCount) {
            // hit group
            offset = vkSbt->hit.offset;
            stride = vkSbt->hit.region.stride;
            startIndex = vkSbt->hit.startIndex;

        } else {
            // callable group
            offset = vkSbt->callable.offset;
            stride = vkSbt->callable.region.stride;
            startIndex = vkSbt->callable.startIndex;
        }

        // write payload
        uint32_t localIndex = index - startIndex;
        uint8_t* dst = (uint8_t*)vkSbt->stagingPtr + offset + (localIndex * stride);
        memcpy(dst + vkSbt->handleSize, info->localData, info->localDataSize);
    }

    vkSbt->isDirty = PAL_TRUE;
}

#endif // PAL_HAS_VULKAN_BACKEND