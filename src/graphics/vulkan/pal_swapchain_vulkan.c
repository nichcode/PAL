
/**
    PAL - Prime Abstraction Layer
    Copyright (C) 2025
    Licensed under the Zlib license. See LICENSE file in root.
 */

#if PAL_HAS_VULKAN_BACKEND
#include "pal_vulkan.h"

PalResult PAL_CALL createSurfaceVk(
    PalDevice* device,
    void* window,
    void* windowInstance,
    PalWindowInstanceType instanceType,
    PalSurface** outSurface)
{
    VkResult result;
    SurfaceVk* surface = nullptr;
    DeviceVk* vkDevice = (DeviceVk*)device;
    VkSurfaceKHR tmp = nullptr;

    surface = palAllocate(s_Vk.allocator, sizeof(SurfaceVk), 0);
    if (!surface) {
        return PAL_RESULT_CODE_OUT_OF_MEMORY;
    }

#ifdef _WIN32
    if (instanceType != PAL_WINDOW_INSTANCE_TYPE_WIN32) {
        return PAL_RESULT_CODE_INVALID_ARGUMENT;
    }

    if (!s_Vk.createWin32Surface) {
        return PAL_RESULT_CODE_PLATFORM_FAILURE;
    }

    VkWin32SurfaceCreateInfoKHR cInfo = {0};
    cInfo.hinstance = windowInstance;
    cInfo.hwnd = window;
    cInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
    result = s_Vk.createWin32Surface(s_Vk.instance, &cInfo, &s_Vk.vkAllocator, &tmp);

#else
    if (instanceType == PAL_WINDOW_INSTANCE_TYPE_WIN32) {
        return PAL_RESULT_CODE_INVALID_ARGUMENT;
    }

    if (instanceType == PAL_WINDOW_INSTANCE_TYPE_WAYLAND) {
        if (!s_Vk.createWaylandSurface) {
            return PAL_RESULT_CODE_PLATFORM_FAILURE;
        }

        VkWaylandSurfaceCreateInfoKHR cInfo = {0};
        cInfo.display = windowInstance;
        cInfo.pNext = nullptr;
        cInfo.flags = 0;
        cInfo.sType = VK_STRUCTURE_TYPE_WAYLAND_SURFACE_CREATE_INFO_KHR;
        cInfo.surface = window;
        result = s_Vk.createWaylandSurface(s_Vk.instance, &cInfo, &s_Vk.vkAllocator, &tmp);

    } else if (instanceType == PAL_WINDOW_INSTANCE_TYPE_X11) {
        if (!s_Vk.createXlibSurface) {
            return PAL_RESULT_CODE_PLATFORM_FAILURE;
        }

        VkXlibSurfaceCreateInfoKHR cInfo = {0};
        cInfo.dpy = windowInstance;
        cInfo.window = (Window)(uintptr_t)(window);
        cInfo.sType = VK_STRUCTURE_TYPE_XLIB_SURFACE_CREATE_INFO_KHR;
        result = s_Vk.createXlibSurface(s_Vk.instance, &cInfo, &s_Vk.vkAllocator, &tmp);

    } else if (instanceType == PAL_WINDOW_INSTANCE_TYPE_XCB) {
        if (!s_Vk.createXcbSurface) {
            return PAL_RESULT_CODE_PLATFORM_FAILURE;
        }

        VkXcbSurfaceCreateInfoKHR cInfo = {0};
        cInfo.connection = windowInstance;
        cInfo.window = (xcb_window_t)(uintptr_t)(window);
        cInfo.sType = VK_STRUCTURE_TYPE_XCB_SURFACE_CREATE_INFO_KHR;
        result = s_Vk.createXcbSurface(s_Vk.instance, &cInfo, &s_Vk.vkAllocator, &tmp);
    }
#endif // _WIN32

    if (result != VK_SUCCESS) {
        palFree(s_Vk.allocator, surface);
        return makeResultVk(result);
    }

    surface->device = vkDevice;
    surface->handle = tmp;
    surface->reserved = PAL_BACKEND_KEY;
    *outSurface = (PalSurface*)surface;
    return PAL_RESULT_SUCCESS;
}

void PAL_CALL destroySurfaceVk(PalSurface* surface)
{
    SurfaceVk* vkSurface = (SurfaceVk*)surface;
    s_Vk.destroySurface(s_Vk.instance, vkSurface->handle, &s_Vk.vkAllocator);
    palFree(s_Vk.allocator, vkSurface);
}

PalResult PAL_CALL getSurfaceCapabilitiesVk(
    PalDevice* device,
    PalSurface* surface,
    PalSurfaceCapabilities* caps)
{
    int32_t formatCount = 0;
    int32_t modeCount = 0;
    SurfaceVk* vkSurface = (SurfaceVk*)surface;
    VkSurfaceFormatKHR* formats = nullptr;
    VkPresentModeKHR* modes = nullptr;

    DeviceVk* vkDevice = (DeviceVk*)device;
    VkPhysicalDevice phyDevice = (VkPhysicalDevice)vkDevice->phyDevice;
    if (!(vkDevice->features & PAL_ADAPTER_FEATURE_SWAPCHAIN)) {
        return PAL_RESULT_CODE_FEATURE_NOT_SUPPORTED;
    }

    memset(caps, 0, sizeof(PalSurfaceCapabilities));
    s_Vk.getSurfacePresentModes(phyDevice, vkSurface->handle, &modeCount, nullptr);
    s_Vk.getSurfaceFormats(phyDevice, vkSurface->handle, &formatCount, nullptr);

    modes = palAllocate(s_Vk.allocator, sizeof(VkPresentModeKHR) * modeCount, 0);
    formats = palAllocate(s_Vk.allocator, sizeof(VkSurfaceFormatKHR) * formatCount, 0);
    if (!modes || !formats) {
        return PAL_RESULT_CODE_OUT_OF_MEMORY;
    }

    s_Vk.getSurfacePresentModes(phyDevice, vkSurface->handle, &modeCount, modes);
    s_Vk.getSurfaceFormats(phyDevice, vkSurface->handle, &formatCount, formats);

    VkSurfaceCapabilitiesKHR surfaceCaps;
    s_Vk.getSurfaceCapabilities(phyDevice, vkSurface->handle, &surfaceCaps);
    caps->minImageWidth = surfaceCaps.minImageExtent.width;
    caps->minImageHeight = surfaceCaps.minImageExtent.height;
    caps->maxImageWidth = surfaceCaps.maxImageExtent.width;
    caps->maxImageHeight = surfaceCaps.maxImageExtent.height;

    caps->maxImageCount = surfaceCaps.maxImageCount;
    caps->minImageCount = surfaceCaps.minImageCount;
    caps->maxImageArrayLayers = surfaceCaps.maxImageArrayLayers;

    if (caps->maxImageCount == 0) {
        caps->maxImageCount = 8; // safe
    }

    // get supported composite alphas
    VkCompositeAlphaFlagsKHR alpha = surfaceCaps.supportedCompositeAlpha;
    caps->supportedCompositeAlphas |= (1u << PAL_COMPOSITE_ALPHA_OPAQUE);
    if (alpha & VK_COMPOSITE_ALPHA_POST_MULTIPLIED_BIT_KHR) {
        caps->supportedCompositeAlphas |= (1u << PAL_COMPOSITE_ALPHA_POST_MULTIPLIED);
    }

    if (alpha & VK_COMPOSITE_ALPHA_PRE_MULTIPLIED_BIT_KHR) {
        caps->supportedCompositeAlphas |= (1u << PAL_COMPOSITE_ALPHA_PRE_MULTIPLIED);
    }

    // present modes
    caps->supportedPresentModes |= (1u << PAL_PRESENT_MODE_FIFO);
    for (int i = 0; i < modeCount; i++) {
        if (modes[i] == VK_PRESENT_MODE_IMMEDIATE_KHR) {
            caps->supportedPresentModes |= (1u << PAL_PRESENT_MODE_IMMEDIATE);
        }

        if (modes[i] == VK_PRESENT_MODE_MAILBOX_KHR) {
            caps->supportedPresentModes |= (1u << PAL_PRESENT_MODE_MAILBOX);
        }
    }

    // get format and colorspace
    for (int i = 0; i < formatCount; i++) {
        VkSurfaceFormatKHR* fmt = &formats[i];
        if (fmt->format == VK_FORMAT_B8G8R8A8_UNORM) {
            // find its supported colorspace
            if (fmt->colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
                caps->supportedFormats |= (1u << PAL_SURFACE_FORMAT_BGRA8_UNORM_SRGB_NONLINEAR);
            }

        } else if (fmt->format == VK_FORMAT_B8G8R8A8_SRGB) {
            // find its supported colorspace
            if (fmt->colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
                caps->supportedFormats |= (1u << PAL_SURFACE_FORMAT_BGRA8_SRGB_NONLINEAR);
            }

        } else if (fmt->format == VK_FORMAT_R8G8B8A8_UNORM) {
            // find its supported colorspace
            if (fmt->colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
                caps->supportedFormats |= (1u << PAL_SURFACE_FORMAT_RGBA8_UNORM_SRGB_NONLINEAR);
            }

        } else if (fmt->format == VK_FORMAT_R16G16B16A16_SFLOAT) {
            // find its supported colorspace
            if (fmt->colorSpace == VK_COLOR_SPACE_HDR10_ST2084_EXT) {
                caps->supportedFormats |= (1u << PAL_SURFACE_FORMAT_RGBA16_FLOAT_HDR10);
            }
        }
    }

    palFree(s_Vk.allocator, formats);
    palFree(s_Vk.allocator, modes);
    return PAL_RESULT_SUCCESS;
}

PalResult PAL_CALL createSwapchainVk(
    PalDevice* device,
    PalQueue* queue,
    PalSurface* surface,
    const PalSwapchainCreateInfo* info,
    PalSwapchain** outSwapchain)
{
    PalFormat imageFormat = 0;
    SwapchainVk* swapchain = nullptr;
    VkImage* images = nullptr;

    DeviceVk* vkDevice = (DeviceVk*)device;
    QueueVk* vkQueue = (QueueVk*)queue;
    PhysicalQueue* phyQueue = vkQueue->phyQueue;
    SurfaceVk* vkSurface = (SurfaceVk*)surface;

    if (!(vkDevice->features & PAL_ADAPTER_FEATURE_SWAPCHAIN)) {
        return PAL_RESULT_CODE_FEATURE_NOT_SUPPORTED;
    }

    // check if the queue is a graphics queue before we check its family
    // index for presentation support.
    if (vkQueue->usage != VK_QUEUE_GRAPHICS_BIT) {
        return PAL_RESULT_CODE_INVALID_HANDLE;
    }

    swapchain = palAllocate(s_Vk.allocator, sizeof(SwapchainVk), 0);
    if (!swapchain) {
        return PAL_RESULT_CODE_OUT_OF_MEMORY;
    }
    memset(swapchain, 0, sizeof(SwapchainVk));

    VkSwapchainCreateInfoKHR createInfo = {0};
    createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.surface = vkSurface->handle;
    createInfo.imageArrayLayers = info->imageArrayLayerCount;
    createInfo.imageExtent.width = info->width;
    createInfo.imageExtent.height = info->height;
    createInfo.minImageCount = info->imageCount;
    createInfo.clipped = info->clipped;
    createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    createInfo.preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;

    // present modes
    createInfo.presentMode = VK_PRESENT_MODE_FIFO_KHR;
    if (info->presentMode == PAL_PRESENT_MODE_IMMEDIATE) {
        createInfo.presentMode = VK_PRESENT_MODE_IMMEDIATE_KHR;

    } else if (info->presentMode == PAL_PRESENT_MODE_MAILBOX) {
        createInfo.presentMode = VK_PRESENT_MODE_MAILBOX_KHR;
    }

    // composite alpha
    createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    if (info->compositeAlpha == PAL_COMPOSITE_ALPHA_POST_MULTIPLIED) {
        createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_POST_MULTIPLIED_BIT_KHR;

    } else if (info->compositeAlpha == PAL_COMPOSITE_ALPHA_PRE_MULTIPLIED) {
        createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_PRE_MULTIPLIED_BIT_KHR;
    }

    // format and colorspace
    createInfo.imageFormat = VK_FORMAT_B8G8R8A8_UNORM;
    createInfo.imageColorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
    imageFormat = PAL_FORMAT_B8G8R8A8_UNORM;

    if (info->format == PAL_SURFACE_FORMAT_BGRA8_SRGB_NONLINEAR) {
        createInfo.imageFormat = VK_FORMAT_B8G8R8A8_SRGB;
        createInfo.imageColorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
        imageFormat = PAL_FORMAT_B8G8R8A8_SRGB;

    } else if (info->format == PAL_SURFACE_FORMAT_RGBA8_UNORM_SRGB_NONLINEAR) {
        createInfo.imageFormat = VK_FORMAT_R8G8B8A8_UNORM;
        createInfo.imageColorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
        imageFormat = PAL_FORMAT_R8G8B8A8_UNORM;

    } else if (info->format == PAL_SURFACE_FORMAT_RGBA16_FLOAT_HDR10) {
        createInfo.imageFormat = VK_FORMAT_R16G16B16A16_SFLOAT;
        createInfo.imageColorSpace = VK_COLOR_SPACE_HDR10_ST2084_EXT;
        imageFormat = PAL_FORMAT_R16G16B16A16_SFLOAT;
    }

    // create swapchain
    VkResult result = vkDevice->createSwapchain(
        vkDevice->handle,
        &createInfo,
        &s_Vk.vkAllocator,
        &swapchain->handle);

    if (result != VK_SUCCESS) {
        palFree(s_Vk.allocator, swapchain);
        return makeResultVk(result);
    }

    // get and cache all images
    int32_t count = 0;
    result = vkDevice->getSwapchainImages(vkDevice->handle, swapchain->handle, &count, nullptr);

    swapchain->images = palAllocate(s_Vk.allocator, sizeof(ImageVk) * count, 0);
    images = palAllocate(s_Vk.allocator, sizeof(VkImage) * count, 0);
    if (!swapchain->images || !images) {
        vkDevice->destroySwapchain(vkDevice->handle, swapchain->handle, &s_Vk.vkAllocator);
        palFree(s_Vk.allocator, swapchain);
        return PAL_RESULT_CODE_OUT_OF_MEMORY;
    }
    vkDevice->getSwapchainImages(vkDevice->handle, swapchain->handle, &count, images);

    // fill all images with the creatio info
    for (int i = 0; i < count; i++) {
        ImageVk* image = &swapchain->images[i];
        image->device = vkDevice;
        image->handle = images[i];

        image->info.belongsToSwapchain = PAL_TRUE;
        image->info.arrayLayerCount = createInfo.imageArrayLayers;
        image->info.depth = 1;
        image->info.format = imageFormat;
        image->info.usages = PAL_IMAGE_USAGE_COLOR_ATTACHEMENT;
        image->info.height = createInfo.imageExtent.height;
        image->info.width = createInfo.imageExtent.width;
        image->info.mipLevelCount = 1;
        image->info.sampleCount = PAL_SAMPLE_COUNT_1; // swapchain images are not multisampled
        image->info.type = PAL_IMAGE_TYPE_2D;
    }
    palFree(s_Vk.allocator, images);

    swapchain->device = vkDevice;
    swapchain->queue = vkQueue;
    swapchain->imageCount = count;
    swapchain->reserved = PAL_BACKEND_KEY;
    *outSwapchain = (PalSwapchain*)swapchain;
    return PAL_RESULT_SUCCESS;
}

void PAL_CALL destroySwapchainVk(PalSwapchain* swapchain)
{
    SwapchainVk* vkSwapchain = (SwapchainVk*)swapchain;
    vkSwapchain->device->destroySwapchain(
        vkSwapchain->device->handle,
        vkSwapchain->handle,
        &s_Vk.vkAllocator);

    palFree(s_Vk.allocator, vkSwapchain->images);
    palFree(s_Vk.allocator, vkSwapchain);
}

PalImage* PAL_CALL getSwapchainImageVk(
    PalSwapchain* swapchain,
    uint32_t index)
{
    SwapchainVk* vkSwapchain = (SwapchainVk*)swapchain;
    if (index > vkSwapchain->imageCount) {
        return nullptr;
    }
    return (PalImage*)&vkSwapchain->images[index];
}

PalResult PAL_CALL getNextSwapchainImageVk(
    PalSwapchain* swapchain,
    PalSwapchainNextImageInfo* info,
    uint32_t* outIndex)
{
    VkResult result;
    uint32_t index = 0;
    uint64_t timeInNanoseconds = 0;
    VkFence fenceHandle = nullptr;
    VkSemaphore semaphoreHandle = nullptr;
    SwapchainVk* vkSwapchain = (SwapchainVk*)swapchain;

    if (info->fence) {
        FenceVk* vkFence = (FenceVk*)info->fence;
        fenceHandle = vkFence->handle;
    }

    if (info->signalSemaphore) {
        SemaphoreVk* vkSemaphore = (SemaphoreVk*)info->signalSemaphore;
        semaphoreHandle = vkSemaphore->handle;
    }

    if (info->timeout) {
        if (info->timeout == PAL_INFINITE) {
            timeInNanoseconds = UINT64_MAX;
        } else {
            timeInNanoseconds = info->timeout * 1000000;
        }
    }

    result = vkSwapchain->device->acquireNextImage(
        vkSwapchain->device->handle,
        vkSwapchain->handle,
        timeInNanoseconds,
        semaphoreHandle,
        fenceHandle,
        &index);

    if (result != VK_SUCCESS) {
        return makeResultVk(result);
    }

    *outIndex = index;
    return PAL_RESULT_SUCCESS;
}

PalResult PAL_CALL presentSwapchainVk(
    PalSwapchain* swapchain, 
    uint32_t imageIndex, 
    PalSemaphore* waitSemaphore)
{
    SwapchainVk* vkSwapchain = (SwapchainVk*)swapchain;
    int32_t semaphoreCount = 0;
    VkSemaphore semaphoreHandle = nullptr;
    if (waitSemaphore) {
        SemaphoreVk* vkSemaphore = (SemaphoreVk*)waitSemaphore;
        semaphoreHandle = vkSemaphore->handle;
        semaphoreCount = 1;
    }

    VkResult result;
    VkPresentInfoKHR presentInfo = {0};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = &vkSwapchain->handle;
    presentInfo.pImageIndices = &imageIndex;
    presentInfo.pWaitSemaphores = &semaphoreHandle;
    presentInfo.waitSemaphoreCount = semaphoreCount;

    result = vkSwapchain->device->queuePresent(vkSwapchain->queue->phyQueue->handle, &presentInfo);
    if (result != VK_SUCCESS) {
        return makeResultVk(result);
    }

    return PAL_RESULT_SUCCESS;
}

PalResult PAL_CALL resizeSwapchainVk(
    PalSwapchain* swapchain,
    uint32_t newWidth,
    uint32_t newHeight)
{
    VkResult result;
    SwapchainVk* vkSwapchain = (SwapchainVk*)swapchain;
    VkSwapchainKHR oldSwapchain = vkSwapchain->handle;
    DeviceVk* device = vkSwapchain->device;
    VkImage* images = nullptr;

    VkSwapchainCreateInfoKHR createInfo = {0};
    createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.oldSwapchain = oldSwapchain;
    createInfo.imageExtent.width = newWidth;
    createInfo.imageExtent.height = newHeight;

    result = device->createSwapchain(
        device->handle,
        &createInfo,
        &s_Vk.vkAllocator,
        &vkSwapchain->handle);

    if (result != VK_SUCCESS) {
        return makeResultVk(result);
    }

    uint32_t count = vkSwapchain->imageCount;
    images = palAllocate(s_Vk.allocator, sizeof(VkImage) * count, 0);
    if (!images) {
        return PAL_RESULT_CODE_OUT_OF_MEMORY;
    }

    device->destroySwapchain(device->handle, oldSwapchain, &s_Vk.vkAllocator);
    device->getSwapchainImages(device->handle, vkSwapchain->handle, &count, images);
    // fill all images with the new create info
    for (int i = 0; i < count; i++) {
        ImageVk* image = &vkSwapchain->images[i];
        image->handle = images[i];
        image->info.height = createInfo.imageExtent.height;
        image->info.width = createInfo.imageExtent.width;
    }

    palFree(s_Vk.allocator, images);
    return PAL_RESULT_SUCCESS;
}

#endif // PAL_HAS_VULKAN_BACKEND