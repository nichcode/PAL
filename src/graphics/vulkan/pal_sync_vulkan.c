
/**
    PAL - Prime Abstraction Layer
    Copyright (C) 2025
    Licensed under the Zlib license. See LICENSE file in root.
 */

#if PAL_HAS_VULKAN_BACKEND
#include "pal_vulkan.h"

PalResult PAL_CALL createFenceVk(
    PalDevice* device,
    PalBool signaled,
    PalFence** outFence)
{
    VkResult result;
    FenceVk* fence = nullptr;
    DeviceVk* vkDevice = (DeviceVk*)device;

    fence = palAllocate(s_Vk.allocator, sizeof(FenceVk), 0);
    if (!fence) {
        return PAL_RESULT_CODE_OUT_OF_MEMORY;
    }

    VkFenceCreateInfo createInfo = {0};
    createInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    if (signaled) {
        createInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
    }

    result = s_Vk.createFence(vkDevice->handle, &createInfo, &s_Vk.vkAllocator, &fence->handle);
    if (result != VK_SUCCESS) {
        palFree(s_Vk.allocator, fence);
        return makeResultVk(result);
    }

    fence->device = vkDevice;
    fence->reserved = PAL_BACKEND_KEY;
    *outFence = (PalFence*)fence;
    return PAL_RESULT_SUCCESS;
}

void PAL_CALL destroyFenceVk(PalFence* fence)
{
    FenceVk* vkFence = (FenceVk*)fence;
    s_Vk.destroyFence(vkFence->device->handle, vkFence->handle, &s_Vk.vkAllocator);
    palFree(s_Vk.allocator, vkFence);
}

PalResult PAL_CALL waitFenceVk(
    PalFence* fence,
    uint64_t timeout)
{
    FenceVk* vkFence = (FenceVk*)fence;
    VkResult result;
    uint64_t timeInNanoseconds = 0;
    if (timeout) {
        if (timeout == PAL_INFINITE) {
            timeInNanoseconds = UINT64_MAX;
        } else {
            timeInNanoseconds = timeout * 1000000;
        }
    }

    result = s_Vk.waitFence(vkFence->device->handle, 1, &vkFence->handle, PAL_TRUE, timeInNanoseconds);
    if (result != VK_SUCCESS) {
        return makeResultVk(result);
    }

    return PAL_RESULT_SUCCESS;
}

PalResult PAL_CALL resetFenceVk(PalFence* fence)
{
    FenceVk* vkFence = (FenceVk*)fence;
    if (!(vkFence->device->features & PAL_ADAPTER_FEATURE_FENCE_RESET)) {
        return PAL_RESULT_CODE_FEATURE_NOT_SUPPORTED;
    }

    VkResult result = s_Vk.resetFence(vkFence->device->handle, 1, &vkFence->handle);
    if (result != VK_SUCCESS) {
        return makeResultVk(result);
    }

    return PAL_RESULT_SUCCESS;
}

PalBool PAL_CALL isFenceSignaledVk(PalFence* fence)
{
    FenceVk* vkFence = (FenceVk*)fence;
    VkResult result = s_Vk.isFenceSignaled(vkFence->device->handle, vkFence->handle);
    if (result == VK_SUCCESS) {
        return PAL_TRUE;

    } else {
        return PAL_FALSE;
    }
}

PalResult PAL_CALL createSemaphoreVk(
    PalDevice* device,
    PalBool enableTimeline,
    PalSemaphore** outSemaphore)
{
    VkResult result;
    SemaphoreVk* semaphore = nullptr;
    DeviceVk* vkDevice = (DeviceVk*)device;
    PalBool hasTimeline = vkDevice->features & PAL_ADAPTER_FEATURE_TIMELINE_SEMAPHORE;

    semaphore = palAllocate(s_Vk.allocator, sizeof(SemaphoreVk), 0);
    if (!semaphore) {
        return PAL_RESULT_CODE_OUT_OF_MEMORY;
    }

    VkSemaphoreTypeCreateInfo timelineCreateInfo = {0};
    timelineCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_TYPE_CREATE_INFO;
    timelineCreateInfo.semaphoreType = VK_SEMAPHORE_TYPE_TIMELINE;

    VkSemaphoreCreateInfo createInfo = {0};
    createInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    const void* next = nullptr;
    semaphore->isTimeline = PAL_FALSE;
    if (enableTimeline && !hasTimeline) {
        return PAL_RESULT_CODE_FEATURE_NOT_SUPPORTED;
    }

    if (enableTimeline) {
        next = &timelineCreateInfo;
        semaphore->isTimeline = PAL_TRUE;   
    }

    createInfo.pNext = next;
    result = s_Vk.createSemaphore(
        vkDevice->handle,
        &createInfo,
        &s_Vk.vkAllocator,
        &semaphore->handle);

    if (result != VK_SUCCESS) {
        palFree(s_Vk.allocator, semaphore);
        return makeResultVk(result);
    }

    semaphore->device = vkDevice;
    semaphore->reserved = PAL_BACKEND_KEY;
    *outSemaphore = (PalSemaphore*)semaphore;
    return PAL_RESULT_SUCCESS;
}

void PAL_CALL destroySemaphoreVk(PalSemaphore* semaphore)
{
    SemaphoreVk* vkSemaphore = (SemaphoreVk*)semaphore;
    s_Vk.destroySemaphore(vkSemaphore->device->handle, vkSemaphore->handle, &s_Vk.vkAllocator);
    palFree(s_Vk.allocator, vkSemaphore);
}

PalResult PAL_CALL waitSemaphoreVk(
    PalSemaphore* semaphore,
    uint64_t value,
    uint64_t timeout)
{
    VkResult result;
    uint64_t timeInNanoseconds = 0;
    SemaphoreVk* vkSemaphore = (SemaphoreVk*)semaphore;
    if (!vkSemaphore->isTimeline) {
        return PAL_RESULT_CODE_FEATURE_NOT_SUPPORTED;
    }

    if (timeout) {
        if (timeout == PAL_INFINITE) {
            timeInNanoseconds = UINT64_MAX;
        } else {
            timeInNanoseconds = timeout * 1000000;
        }
    }

    VkSemaphoreWaitInfo waitInfo = {0};
    waitInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_WAIT_INFO;
    waitInfo.semaphoreCount = 1;
    waitInfo.pSemaphores = &vkSemaphore->handle;
    waitInfo.pValues = &value;

    result = vkSemaphore->device->waitSemaphore(
        vkSemaphore->device->handle, 
        &waitInfo, 
        timeInNanoseconds);

    if (result != VK_SUCCESS) {
        return makeResultVk(result);
    }

    return PAL_RESULT_SUCCESS;
}

PalResult PAL_CALL signalSemaphoreVk(
    PalSemaphore* semaphore,
    PalQueue* queue,
    uint64_t value)
{
    VkResult result;
    SemaphoreVk* vkSemaphore = (SemaphoreVk*)semaphore;
    if (!vkSemaphore->isTimeline) {
        return PAL_RESULT_CODE_FEATURE_NOT_SUPPORTED;
    }

    VkSemaphoreSignalInfo signalInfo = {0};
    signalInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_SIGNAL_INFO;
    signalInfo.semaphore = vkSemaphore->handle;
    signalInfo.value = value;

    result = vkSemaphore->device->signalSemaphore(vkSemaphore->device->handle, &signalInfo);
    if (result != VK_SUCCESS) {
        return makeResultVk(result);
    }

    return PAL_RESULT_SUCCESS;
}

uint64_t PAL_CALL getSemaphoreValueVk(PalSemaphore* semaphore)
{
    SemaphoreVk* vkSemaphore = (SemaphoreVk*)semaphore;
    if (!vkSemaphore->isTimeline) {
        return PAL_RESULT_CODE_FEATURE_NOT_SUPPORTED;
    }

    VkResult result = vkSemaphore->device->getSemaphoreValue(
        vkSemaphore->device->handle,
        vkSemaphore->handle,
        outValue);

    if (result != VK_SUCCESS) {
        return makeResultVk(result);
    }

    return PAL_RESULT_SUCCESS;
}

#endif // PAL_HAS_VULKAN_BACKEND