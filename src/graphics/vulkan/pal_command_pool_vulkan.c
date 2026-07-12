
/**
    PAL - Prime Abstraction Layer
    Copyright (C) 2025
    Licensed under the Zlib license. See LICENSE file in root.
 */

#if PAL_HAS_VULKAN_BACKEND
#include "pal_vulkan.h"

PalResult PAL_CALL createCommandPoolVk(
    PalDevice* device,
    PalQueue* queue,
    PalCommandPool** outPool)
{
    VkResult result;
    CommandPoolVk* pool = nullptr;
    DeviceVk* vkDevice = (DeviceVk*)device;
    QueueVk* vkQueue = (QueueVk*)queue;
    PhysicalQueue* phyQueue = vkQueue->phyQueue;

    pool = palAllocate(s_Vk.allocator, sizeof(CommandPoolVk), 0);
    if (!pool) {
        return PAL_RESULT_CODE_OUT_OF_MEMORY;
    }

    VkCommandPoolCreateInfo cInfo = {0};
    cInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    cInfo.queueFamilyIndex = phyQueue->familyIndex;
    cInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    result = s_Vk.createCommandPool(vkDevice->handle, &cInfo, &s_Vk.vkAllocator, &pool->handle);
    if (result != VK_SUCCESS) {
        return makeResultVk(result);
    }

    pool->device = vkDevice;
    *outPool = (PalCommandPool*)pool;
    return PAL_RESULT_SUCCESS;
}

void PAL_CALL destroyCommandPoolVk(PalCommandPool* pool)
{
    CommandPoolVk* vkPool = (CommandPoolVk*)pool;
    s_Vk.destroyCommandPool(vkPool->device->handle, vkPool->handle, &s_Vk.vkAllocator);
    palFree(s_Vk.allocator, vkPool);
}

PalResult PAL_CALL resetCommandPoolVk(PalCommandPool* pool)
{
    CommandPoolVk* vkCmdPool = (CommandPoolVk*)pool;
    VkResult result = s_Vk.resetCommandPool(vkCmdPool->device->handle, vkCmdPool->handle, 0);
    if (result != VK_SUCCESS) {
        return makeResultVk(result);
    }

    return PAL_RESULT_SUCCESS;
}

PalResult PAL_CALL allocateCommandBufferVk(
    PalDevice* device,
    PalCommandPool* pool,
    PalCommandBufferType type,
    PalCommandBuffer** outCmdBuffer)
{
    VkResult result;
    CommandBufferVk* cmdBuffer = nullptr;
    DeviceVk* vkDevice = (DeviceVk*)device;
    CommandPoolVk* vkPool = (CommandPoolVk*)pool;

    cmdBuffer = palAllocate(s_Vk.allocator, sizeof(CommandBufferVk), 0);
    if (!cmdBuffer) {
        PAL_RESULT_CODE_OUT_OF_MEMORY;
    }

    // allocate memory for the linear allocator. We first start with 4KB
    cmdBuffer->allocator.memory = (uint8_t*)palAllocate(s_Vk.allocator, 4096, 0);
    if (!cmdBuffer->allocator.memory) {
        PAL_RESULT_CODE_OUT_OF_MEMORY;
    }

    cmdBuffer->allocator.size = 4096;
    cmdBuffer->allocator.offset = 0;

    VkCommandBufferAllocateInfo allocateInfo = {0};
    allocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocateInfo.commandBufferCount = 1;
    allocateInfo.commandPool = vkPool->handle;
    allocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;

    cmdBuffer->primary = PAL_TRUE;
    if (type == PAL_COMMAND_BUFFER_TYPE_SECONDARY) {
        allocateInfo.level = VK_COMMAND_BUFFER_LEVEL_SECONDARY;
        cmdBuffer->primary = PAL_FALSE;
    }

    result = s_Vk.allocateCommandBuffer(vkDevice->handle, &allocateInfo, &cmdBuffer->handle);
    if (result != VK_SUCCESS) {
        palFree(s_Vk.allocator, (void*)cmdBuffer->allocator.memory);
        palFree(s_Vk.allocator, cmdBuffer);
        return makeResultVk(result);
    }

    // create tmp buffer
    VkBufferCreateInfo bufCreateInfo = {0};
    bufCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufCreateInfo.size = sizeof(VkTraceRaysIndirectCommandKHR);
    bufCreateInfo.usage = VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT;
    bufCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    result = s_Vk.createBuffer(
        vkDevice->handle, 
        &bufCreateInfo, 
        &s_Vk.vkAllocator, 
        &cmdBuffer->buffer);

    if (result != VK_SUCCESS) {
        return makeResultVk(result);
    }

    // allocate CPU upload memory and bind
    VkMemoryRequirements memReq = {0};
    s_Vk.getBufferMemoryRequirements(vkDevice->handle, cmdBuffer->buffer, &memReq);

    VkMemoryAllocateInfo bufferAllocateInfo = {0};
    bufferAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    bufferAllocateInfo.allocationSize = memReq.size;

    uint32_t mask = vkDevice->memoryClassMask[PAL_MEMORY_TYPE_GPU_ONLY] & memReq.memoryTypeBits;
    uint32_t memoryIndex = findBestMemoryIndexVk(vkDevice->phyDevice, mask);
    bufferAllocateInfo.memoryTypeIndex = memoryIndex;

    result = s_Vk.allocateMemory(
        vkDevice->handle,
        &bufferAllocateInfo,
        &s_Vk.vkAllocator,
        &cmdBuffer->bufferMemory);

    if (result != VK_SUCCESS) {
        return makeResultVk(result);
    }

    s_Vk.bindBufferMemory(vkDevice->handle, cmdBuffer->buffer, cmdBuffer->bufferMemory, 0);

    cmdBuffer->device = vkDevice;
    cmdBuffer->pool = vkPool;
    *outCmdBuffer = (PalCommandBuffer*)cmdBuffer;
    return PAL_RESULT_SUCCESS;
}

void PAL_CALL freeCommandBufferVk(PalCommandBuffer* cmdBuffer)
{
    CommandBufferVk* vkCmdBuffer = (CommandBufferVk*)cmdBuffer;
    s_Vk.freeCommandBuffer(
        vkCmdBuffer->device->handle,
        vkCmdBuffer->pool->handle,
        1,
        &vkCmdBuffer->handle);

    s_Vk.destroyBuffer(vkCmdBuffer->device->handle, vkCmdBuffer->buffer, &s_Vk.vkAllocator);
    s_Vk.freeMemory(vkCmdBuffer->device->handle, vkCmdBuffer->bufferMemory, &s_Vk.vkAllocator);

    palFree(s_Vk.allocator, (void*)vkCmdBuffer->allocator.memory);
    palFree(s_Vk.allocator, vkCmdBuffer);
}

PalResult PAL_CALL resetCommandBufferVk(PalCommandBuffer* cmdBuffer)
{
    CommandBufferVk* vkCmdBuffer = (CommandBufferVk*)cmdBuffer;
    VkResult result = s_Vk.resetCommandBuffer(vkCmdBuffer->handle, 0);
    if (result != VK_SUCCESS) {
        return makeResultVk(result);
    }
    
    return PAL_RESULT_SUCCESS;
}

PalResult PAL_CALL submitCommandBufferVk(
    PalQueue* queue,
    PalCommandBufferSubmitInfo* info)
{
    VkResult result;
    int32_t waitSemaphoreCount = 0;
    int32_t signalSemaphoreCount = 0;
    VkFence fenceHandle = nullptr;
    VkSemaphore waitSemaphoreHandle = nullptr;
    VkSemaphore signalSemaphoreHandle = nullptr;
    QueueVk* vkQueue = (QueueVk*)queue;
    CommandBufferVk* vkCmdBuffer = (CommandBufferVk*)info->cmdBuffer;
    PhysicalQueue* phyQueue = vkQueue->phyQueue;

    if (info->waitSemaphore) {
        SemaphoreVk* tmp = (SemaphoreVk*)info->waitSemaphore;
        waitSemaphoreHandle = tmp->handle;
        waitSemaphoreCount = 1;
    }

    if (info->signalSemaphore) {
        SemaphoreVk* tmp = (SemaphoreVk*)info->signalSemaphore;
        signalSemaphoreHandle = tmp->handle;
        signalSemaphoreCount = 1;
    }

    if (info->fence) {
        FenceVk* tmp = (FenceVk*)info->fence;
        fenceHandle = tmp->handle;
    }

    VkCommandBufferSubmitInfoKHR cmdBufferSubmitInfo = {0};
    cmdBufferSubmitInfo.commandBuffer = vkCmdBuffer->handle;
    cmdBufferSubmitInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO_KHR;

    VkSemaphoreSubmitInfoKHR waitSubmitInfo = {0};
    waitSubmitInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO_KHR;
    waitSubmitInfo.semaphore = waitSemaphoreHandle;
    waitSubmitInfo.stageMask = pipelineStagesToVk(info->waitStages);
    waitSubmitInfo.value = info->waitValue;

    VkSemaphoreSubmitInfoKHR signalSubmitInfo = {0};
    signalSubmitInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO_KHR;
    signalSubmitInfo.semaphore = signalSemaphoreHandle;
    signalSubmitInfo.stageMask = pipelineStagesToVk(info->signalStages);
    signalSubmitInfo.value = info->signalValue;

    VkSubmitInfo2KHR submitInfo = {0};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO_2_KHR;
    submitInfo.commandBufferInfoCount = 1;
    submitInfo.pCommandBufferInfos = &cmdBufferSubmitInfo;
    submitInfo.pSignalSemaphoreInfos = &signalSubmitInfo;
    submitInfo.pWaitSemaphoreInfos = &waitSubmitInfo;
    submitInfo.waitSemaphoreInfoCount = waitSemaphoreCount;
    submitInfo.signalSemaphoreInfoCount = signalSemaphoreCount;

    result = vkCmdBuffer->device->queueSubmit(phyQueue->handle, 1, &submitInfo, fenceHandle);
    if (result != VK_SUCCESS) {
        return makeResultVk(result);
    }

    return PAL_RESULT_SUCCESS;
}

#endif // PAL_HAS_VULKAN_BACKEND