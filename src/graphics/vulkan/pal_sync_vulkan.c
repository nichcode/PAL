
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
    DeviceVk* deviceImpl = (DeviceVk*)device;

    fence = palAllocate(s_Vk.allocator, sizeof(FenceVk), 0);
    if (!fence) {
        return PAL_RESULT_CODE_OUT_OF_MEMORY;
    }

    VkFenceCreateInfo createInfo = {0};
    createInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    if (signaled) {
        createInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
    }

    result = s_Vk.createFence(deviceImpl->handle, &createInfo, &s_Vk.allocatorImpl, &fence->handle);
    if (result != VK_SUCCESS) {
        palFree(s_Vk.allocator, fence);
        return makeResultVk(result);
    }

    setDebugNameVk(deviceImpl, VK_OBJECT_TYPE_FENCE, fence->handle);
    fence->device = deviceImpl;
    *outFence = (PalFence*)fence;
    return PAL_RESULT_SUCCESS;
}

void PAL_CALL destroyFenceVk(PalFence* fence)
{
    FenceVk* fenceImpl = (FenceVk*)fence;
    s_Vk.destroyFence(fenceImpl->device->handle, fenceImpl->handle, &s_Vk.allocatorImpl);
    palFree(s_Vk.allocator, fenceImpl);
}

PalResult PAL_CALL waitFenceVk(
    PalFence* fence,
    uint64_t timeout)
{
    FenceVk* fenceImpl = (FenceVk*)fence;
    VkResult result;
    uint64_t timeInNano = 0;
    if (timeout) {
        if (timeout == PAL_INFINITE) {
            timeInNano = UINT64_MAX;
        } else {
            timeInNano = timeout * 1000000;
        }
    }

    result = s_Vk.waitFence(fenceImpl->device->handle, 1, &fenceImpl->handle, PAL_TRUE, timeInNano);
    if (result != VK_SUCCESS) {
        return makeResultVk(result);
    }

    return PAL_RESULT_SUCCESS;
}

PalResult PAL_CALL resetFenceVk(PalFence* fence)
{
    FenceVk* fenceImpl = (FenceVk*)fence;
    VkResult result = s_Vk.resetFence(fenceImpl->device->handle, 1, &fenceImpl->handle);
    if (result != VK_SUCCESS) {
        return makeResultVk(result);
    }

    return PAL_RESULT_SUCCESS;
}

PalBool PAL_CALL isFenceSignaledVk(PalFence* fence)
{
    FenceVk* fenceImpl = (FenceVk*)fence;
    VkResult result = s_Vk.isFenceSignaled(fenceImpl->device->handle, fenceImpl->handle);
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
    DeviceVk* deviceImpl = (DeviceVk*)device;

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
    if (enableTimeline) {
        next = &timelineCreateInfo;
        semaphore->isTimeline = PAL_TRUE;
    }

    createInfo.pNext = next;
    result = s_Vk.createSemaphore(
        deviceImpl->handle,
        &createInfo,
        &s_Vk.allocatorImpl,
        &semaphore->handle);

    if (result != VK_SUCCESS) {
        palFree(s_Vk.allocator, semaphore);
        return makeResultVk(result);
    }

    setDebugNameVk(deviceImpl, VK_OBJECT_TYPE_SEMAPHORE, semaphore->handle);
    semaphore->device = deviceImpl;
    *outSemaphore = (PalSemaphore*)semaphore;
    return PAL_RESULT_SUCCESS;
}

void PAL_CALL destroySemaphoreVk(PalSemaphore* semaphore)
{
    SemaphoreVk* semaphoreImpl = (SemaphoreVk*)semaphore;
    s_Vk.destroySemaphore(
        semaphoreImpl->device->handle,
        semaphoreImpl->handle,
        &s_Vk.allocatorImpl);
    palFree(s_Vk.allocator, semaphoreImpl);
}

PalResult PAL_CALL waitSemaphoreVk(
    PalSemaphore* semaphore,
    uint64_t value,
    uint64_t timeout)
{
    VkResult result;
    uint64_t timeInNano = 0;
    SemaphoreVk* semaphoreImpl = (SemaphoreVk*)semaphore;
    if (!semaphoreImpl->isTimeline) {
        return PAL_RESULT_CODE_FEATURE_NOT_SUPPORTED;
    }

    if (timeout) {
        if (timeout == PAL_INFINITE) {
            timeInNano = UINT64_MAX;
        } else {
            timeInNano = timeout * 1000000;
        }
    }

    VkSemaphoreWaitInfo waitInfo = {0};
    waitInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_WAIT_INFO;
    waitInfo.semaphoreCount = 1;
    waitInfo.pSemaphores = &semaphoreImpl->handle;
    waitInfo.pValues = &value;

    // clang-format off
    result = semaphoreImpl->device->waitSemaphore(
        semaphoreImpl->device->handle, 
        &waitInfo, 
        timeInNano);
    // clang-format on

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
    SemaphoreVk* semaphoreImpl = (SemaphoreVk*)semaphore;
    if (!semaphoreImpl->isTimeline) {
        return PAL_RESULT_CODE_FEATURE_NOT_SUPPORTED;
    }

    VkSemaphoreSignalInfo signalInfo = {0};
    signalInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_SIGNAL_INFO;
    signalInfo.semaphore = semaphoreImpl->handle;
    signalInfo.value = value;

    result = semaphoreImpl->device->signalSemaphore(semaphoreImpl->device->handle, &signalInfo);
    if (result != VK_SUCCESS) {
        return makeResultVk(result);
    }

    return PAL_RESULT_SUCCESS;
}

PalResult PAL_CALL getSemaphoreValueVk(
    PalSemaphore* semaphore,
    uint64_t* value)
{
    SemaphoreVk* semaphoreImpl = (SemaphoreVk*)semaphore;
    VkResult result = semaphoreImpl->device->getSemaphoreValue(
        semaphoreImpl->device->handle,
        semaphoreImpl->handle,
        value);

    if (result != VK_SUCCESS) {
        return makeResultVk(result);
    }

    return PAL_RESULT_SUCCESS;
}

#endif // PAL_HAS_VULKAN_BACKEND