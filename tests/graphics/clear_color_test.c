
#include "pal/pal_graphics.h"
#include "pal/pal_video.h"
#include "pal/pal_system.h"
#include "tests.h"

#define WINDOW_WIDTH 640
#define WINDOW_HEIGHT 480
#define MAX_FRAMES_IN_FLIGHT 2

static void PAL_CALL onGraphicsDebug(
    void* userData,
    PalDebugMessageSeverity severity,
    PalDebugMessageType type,
    const char* msg)
{
    palLog(nullptr, msg);
}

bool clearColorTest()
{
    PalResult result;
    PalWindow* window = nullptr;
    PalEventDriver* eventDriver = nullptr;
    PalGraphicsWindow gfxWindow;

    PalAdapter* adapter = nullptr;
    PalDevice* device = nullptr;
    PalSurface* surface = nullptr;
    PalQueue* queue = nullptr;
    PalSwapchain* swapchain = nullptr;
    PalCommandPool* cmdPool = nullptr;
    PalImageView** imageViews = nullptr;

    PalCommandBuffer* cmdBuffers[MAX_FRAMES_IN_FLIGHT];
    PalSemaphore* imageAvailableSemaphores[MAX_FRAMES_IN_FLIGHT];
    PalFence* inFlightFences[MAX_FRAMES_IN_FLIGHT];
    PalSemaphore** renderFinishedSemaphores; // count of swapchain images
    PalFence** inFlightImages; // count of swapchain images

    PalEventDriverCreateInfo eventDriverCreateInfo = {0};
    result = palCreateEventDriver(&eventDriverCreateInfo, &eventDriver);
    if (result != PAL_RESULT_SUCCESS) {
        const char* error = palFormatResult(result);
        palLog(nullptr, "Failed to create event driver: %s", error);
        return false;
    }

    palSetEventDispatchMode(eventDriver, PAL_EVENT_WINDOW_CLOSE, PAL_DISPATCH_POLL);
    palSetEventDispatchMode(eventDriver, PAL_EVENT_KEYDOWN, PAL_DISPATCH_POLL);

    result = palInitVideo(nullptr, eventDriver);
    if (result != PAL_RESULT_SUCCESS) {
        const char* error = palFormatResult(result);
        palLog(nullptr, "Failed to initialize video: %s", error);
        return false;
    }

    PalWindowCreateInfo windowCreateInfo = {0};
    windowCreateInfo.height = WINDOW_HEIGHT;
    windowCreateInfo.width = WINDOW_WIDTH;
    windowCreateInfo.show = true;
    windowCreateInfo.title = "Clear Color Window";

    PalVideoFeatures64 videoFeatures = palGetVideoFeaturesEx();
    if (!(videoFeatures & PAL_VIDEO_FEATURE64_DECORATED_WINDOW)) {
        windowCreateInfo.style |= PAL_WINDOW_STYLE_BORDERLESS;
    }

    result = palCreateWindow(&windowCreateInfo, &window);
    if (result != PAL_RESULT_SUCCESS) {
        const char* error = palFormatResult(result);
        palLog(nullptr, "Failed to create window: %s", error);
        return false;
    }

    PalWindowHandleInfo winHandle = palGetWindowHandleInfo(window);
    gfxWindow.display = winHandle.nativeDisplay;
    gfxWindow.window = winHandle.nativeWindow;

    // using pal_system.h will be easy to know the underlying windowing API
    // or use typedefs. We will use the pal_system module. This is needed
    // for systems which multiple windowing APIs (linux).
    PalPlatformInfo platformInfo = {0};
    result = palGetPlatformInfo(&platformInfo);
    if (result != PAL_RESULT_SUCCESS) {
        const char* error = palFormatResult(result);
        palLog(nullptr, "Failed to get platform information: %s", error);
        return false;
    }

    if (platformInfo.apiType == PAL_PLATFORM_API_WAYLAND) {
        gfxWindow.displayType = PAL_GRAPHICS_WINDOW_DISPLAY_TYPE_WAYLAND;

    } else if (platformInfo.apiType == PAL_PLATFORM_API_X11) {
        gfxWindow.displayType = PAL_GRAPHICS_WINDOW_DISPLAY_TYPE_X11;

    } else {
        // automatically this is xcb
        gfxWindow.displayType = PAL_GRAPHICS_WINDOW_DISPLAY_TYPE_XCB;
    }

    PalGraphicsDebugger debugger = {0};
    debugger.callback = onGraphicsDebug;
    debugger.userData = nullptr;

    result = palInitGraphics(nullptr, nullptr);
    if (result != PAL_RESULT_SUCCESS) {
        const char* error = palFormatResult(result);
        palLog(nullptr, "Failed to initialize graphics: %s", error);
        return false;
    }

    // enumerate all available adapters
    Int32 adapterCount = 0;
    result = palEnumerateAdapters(&adapterCount, nullptr);
    if (result != PAL_RESULT_SUCCESS) {
        const char* error = palFormatResult(result);
        palLog(nullptr, "Failed to get query adapters: %s", error);
        return false;
    }

    if (adapterCount == 0) {
        palLog(nullptr, "No adapters found");
        return false;
    }
    palLog(nullptr, "Adapter count: %d", adapterCount);

    PalAdapter** adapters = nullptr;
    adapters = palAllocate(nullptr, sizeof(PalAdapter*) * adapterCount, 0);
    if (!adapters) {
        palLog(nullptr, "Failed to allocate memory");
        return false;
    }

    result = palEnumerateAdapters(&adapterCount, adapters);
    if (result != PAL_RESULT_SUCCESS) {
        const char* error = palFormatResult(result);
        palLog(nullptr, "Failed to get query adapters: %s", error);
        return false;
    }

    PalAdapterCapabilities caps = {0};
    PalAdapterInfo adapterInfo = {0};
    for (Int32 i = 0; i < adapterCount; i++) {
        adapter = adapters[i];
        result = palGetAdapterCapabilities(adapter, &caps);
        if (result != PAL_RESULT_SUCCESS) {
            const char* error = palFormatResult(result);
            palLog(nullptr, "Failed to get adapter capabilities: %s", error);
            palFree(nullptr, adapters);
            return false;
        }

        if (caps.maxGraphicsQueues == 0) {
            continue;

        } else {
            break;
        }

        adapter = nullptr;
    }

    palFree(nullptr, adapters);
    if (!adapter) {
        palLog(nullptr, "Failed to find an adapter that supports graphics queue");
        return false;
    }

    // create a device
    PalAdapterFeatures adapterFeatures = palGetAdapterFeatures(adapter);
    PalAdapterFeatures features = PAL_ADAPTER_FEATURE_SWAPCHAIN;
    if (adapterFeatures & PAL_ADAPTER_FEATURE_FENCE_RESET) {
        features |= PAL_ADAPTER_FEATURE_FENCE_RESET;
    }

    result = palCreateDevice(adapter, features, &device);
    if (result != PAL_RESULT_SUCCESS) {
        const char* error = palFormatResult(result);
        palLog(nullptr, "Failed to create device: %s", error);
        return false;
    }

    // create surface
    result = palCreateSurface(device, &gfxWindow, &surface);
    if (result != PAL_RESULT_SUCCESS) {
        const char* error = palFormatResult(result);
        palLog(nullptr, "Failed to create surface: %s", error);
        return false;
    }

    // create a graphics command queue and check if its supports presenting to the surface
    bool foundQueue = false;
    for (int i = 0; i < caps.maxGraphicsQueues; i++) {
        result = palCreateQueue(device, PAL_QUEUE_TYPE_GRAPHICS, &queue);
        if (result != PAL_RESULT_SUCCESS) {
            const char* error = palFormatResult(result);
            palLog(nullptr, "Failed to create queue: %s", error);
            return false;
        }

        if (!palCanQueuePresent(queue, surface)) {
            palDestroyQueue(queue);
            queue = nullptr;
        }  else {
            // found a queue
            foundQueue = true;
            break;
        }
    }

    if (!foundQueue) {
        palLog(nullptr, "Failed to find a queue that can present to the surface");
        return false;
    }

    // create a swapchain with the graphics queue
    PalSurfaceCapabilities surfaceCaps = {0};
    result = palGetSurfaceCapabilities(device, surface, &surfaceCaps);
    if (result != PAL_RESULT_SUCCESS) {
        const char* error = palFormatResult(result);
        palLog(nullptr, "Failed to get surface capabilities: %s", error);
        return false;
    }

    PalSwapchainCreateInfo swapchainCreateInfo = {0};
    swapchainCreateInfo.clipped = true;
    swapchainCreateInfo.compositeAlpha = PAL_COMPOSITE_ALPHA_OPAQUE;
    swapchainCreateInfo.height = WINDOW_HEIGHT;
    swapchainCreateInfo.width = WINDOW_WIDTH;
    swapchainCreateInfo.imageArrayLayerCount = 1;
    swapchainCreateInfo.presentMode = PAL_PRESENT_MODE_FIFO;
    swapchainCreateInfo.format = PAL_SURFACE_FORMAT_BGRA8_UNORM_SRGB_NONLINEAR;

    // rare but possible on andriod
    if (WINDOW_WIDTH > surfaceCaps.maxImageWidth) {
        swapchainCreateInfo.width = surfaceCaps.maxImageWidth / 2;
    }

    if (WINDOW_HEIGHT > surfaceCaps.maxImageHeight) {
        swapchainCreateInfo.height = surfaceCaps.maxImageHeight / 2;
    }

    // check if the minimal image count is not good for you
    // and increase it but not pass the max count
    swapchainCreateInfo.imageCount = surfaceCaps.minImageCount;
    if (swapchainCreateInfo.imageCount == 1) {
        swapchainCreateInfo.imageCount++;
        if (surfaceCaps.maxImageCount < 2) {
            palLog(nullptr, "Surface does not support double buffers");
            return false;
        }
    }

    result = palCreateSwapchain(device, queue, surface, &swapchainCreateInfo, &swapchain);
    if (result != PAL_RESULT_SUCCESS) {
        const char* error = palFormatResult(result);
        palLog(nullptr, "Failed to create swapchain: %s", error);
        return false;
    }

    // get all swapchain images and create image views for them
    Uint32 imageCount = swapchainCreateInfo.imageCount;
    imageViews = palAllocate(nullptr, sizeof(PalImageView*) * imageCount, 0);
    inFlightImages = palAllocate(nullptr, sizeof(PalFence*) * imageCount, 0);
    renderFinishedSemaphores = palAllocate(nullptr, sizeof(PalSemaphore*) * imageCount, 0);
    if (!imageViews || !inFlightImages || !renderFinishedSemaphores) {
        palLog(nullptr, "Failed to allocate memory");
        return false;
    }

    PalImageInfo imageInfo;
    result = palGetImageInfo(palGetSwapchainImage(swapchain, 0), &imageInfo);
    if (result != PAL_RESULT_SUCCESS) {
        const char* error = palFormatResult(result);
        palLog(nullptr, "Failed to get image info: %s", error);
        return false;
    }

    PalImageViewCreateInfo imageViewCreateInfo = {0};
    imageViewCreateInfo.type = PAL_IMAGE_VIEW_TYPE_2D;
    imageViewCreateInfo.subresourceRange.layerArrayCount = 1;
    imageViewCreateInfo.subresourceRange.mipLevelCount = 1;
    imageViewCreateInfo.subresourceRange.startArrayLayer = 0;
    imageViewCreateInfo.subresourceRange.startMipLevel = 0;
    imageViewCreateInfo.format = imageInfo.format;

    for (int i = 0; i < imageCount; i++) {
        // get swapchain image
        // this is fast since the images are cache by PAL
        PalImage* image = palGetSwapchainImage(swapchain, i);
        if (!image) {
            palLog(nullptr, "Failed to get swapchain image");
        }

        result = palCreateImageView(device, image, &imageViewCreateInfo, &imageViews[i]);
        if (result != PAL_RESULT_SUCCESS) {
            const char* error = palFormatResult(result);
            palLog(nullptr, "Failed to create image view: %s", error);
            return false;
        }

        // create render finished semaphores
        result = palCreateSemaphore(device, false, &renderFinishedSemaphores[i]);
        if (result != PAL_RESULT_SUCCESS) {
            const char* error = palFormatResult(result);
            palLog(nullptr, "Failed to create semaphore: %s", error);
            return false;
        }

        inFlightImages[i] = nullptr;
    }

    result = palCreateCommandPool(device, queue, &cmdPool);
    if (result != PAL_RESULT_SUCCESS) {
        const char* error = palFormatResult(result);
        palLog(nullptr, "Failed to create command pool: %s", error);
        return false;
    }

    // create synchronization objects and command buffers
    for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        result = palCreateSemaphore(device, false, &imageAvailableSemaphores[i]);
        if (result != PAL_RESULT_SUCCESS) {
            const char* error = palFormatResult(result);
            palLog(nullptr, "Failed to create semaphore: %s", error);
            return false;
        }

        result = palCreateFence(device, true, &inFlightFences[i]);
        if (result != PAL_RESULT_SUCCESS) {
            const char* error = palFormatResult(result);
            palLog(nullptr, "Failed to create fence: %s", error);
            return false;
        }

        result = palAllocateCommandBuffer(
            device,
            cmdPool,
            PAL_COMMAND_BUFFER_TYPE_PRIMARY,
            &cmdBuffers[i]);

        if (result != PAL_RESULT_SUCCESS) {
            const char* error = palFormatResult(result);
            palLog(nullptr, "Failed to allocate command buffer: %s", error);
            return false;
        }
    }

    // main loop
    Uint32 currentFrame = 0;
    bool running = true;
    while (running) {
        // update the video system to push video events
        palUpdateVideo();

        PalEvent event;
        while (palPollEvent(eventDriver, &event)) {
            switch (event.type) {
                case PAL_EVENT_WINDOW_CLOSE: {
                    running = false;
                    break;
                }

                case PAL_EVENT_KEYDOWN: {
                    PalKeycode keycode = 0;
                    palUnpackUint32(event.data, &keycode, nullptr);
                    if (keycode == PAL_KEYCODE_ESCAPE) {
                        running = false;
                    }
                    break;
                }
            }
        }

        result = palWaitFence(inFlightFences[currentFrame], PAL_INFINITE);
        if (result != PAL_RESULT_SUCCESS) {
            const char* error = palFormatResult(result);
            palLog(nullptr, "Failed to wait fence: %s", error);
            return false;
        }

        // get next swapchain image
        PalSwapchainNextImageInfo nextImageInfo = {0};
        nextImageInfo.fence = nullptr;
        nextImageInfo.signalSemaphore = imageAvailableSemaphores[currentFrame];
        nextImageInfo.timeout = PAL_INFINITE;

        Uint32 imageIndex = 0;
        result = palGetNextSwapchainImage(swapchain, &nextImageInfo, &imageIndex);
        if (result != PAL_RESULT_SUCCESS) {
            const char* error = palFormatResult(result);
            palLog(nullptr, "Failed to get next swapchain image: %s", error);
            return false;
        }

        if (inFlightImages[imageIndex] != nullptr) {
            result = palWaitFence(inFlightImages[imageIndex], PAL_INFINITE);
            if (result != PAL_RESULT_SUCCESS) {
                const char* error = palFormatResult(result);
                palLog(nullptr, "Failed to wait fence: %s", error);
                return false;
            }
        }

        inFlightImages[imageIndex] = inFlightFences[currentFrame];
        if (adapterFeatures & PAL_ADAPTER_FEATURE_FENCE_RESET) {
            result = palResetFence(inFlightFences[currentFrame]);
            if (result != PAL_RESULT_SUCCESS) {
                const char* error = palFormatResult(result);
                palLog(nullptr, "Failed to wait fence: %s", error);
                return false;
            }

        } else {
            // recreate since we dont support fence resetting
            palDestroyFence(inFlightFences[currentFrame]);

            result = palCreateFence(device, false, &inFlightFences[currentFrame]);
            if (result != PAL_RESULT_SUCCESS) {
                const char* error = palFormatResult(result);
                palLog(nullptr, "Failed to wait fence: %s", error);
                return false;
            }
        }

        // reset the command buffer
        result = palResetCommandBuffer(cmdBuffers[currentFrame]);
        if (result != PAL_RESULT_SUCCESS) {
            const char* error = palFormatResult(result);
            palLog(nullptr, "Failed to reset command buffer: %s", error);
            return false;
        }

        result = palCmdBegin(cmdBuffers[currentFrame], nullptr);
        if (result != PAL_RESULT_SUCCESS) {
            const char* error = palFormatResult(result);
            palLog(nullptr, "Failed to begin command buffer: %s", error);
            return false;
        }

        // change the state of the image view to make it renderable
        PalUsageStateInfo oldUsageStateInfo = {0};
        PalUsageStateInfo newUsageStateInfo = {0};
        newUsageStateInfo.usageState = PAL_USAGE_STATE_COLOR_ATTACHMENT_WRITE;

        PalImageSubresourceRange imageRange = {0};
        imageRange.layerArrayCount = 1;
        imageRange.mipLevelCount = 1;
        imageRange.startArrayLayer = 0;
        imageRange.startMipLevel = 0;

        PalImage* image = palGetSwapchainImage(swapchain, imageIndex);
        result = palCmdImageBarrier(
            cmdBuffers[currentFrame],
            image,
            &imageRange,
            &oldUsageStateInfo,
            &newUsageStateInfo);

        if (result != PAL_RESULT_SUCCESS) {
            const char* error = palFormatResult(result);
            palLog(nullptr, "Failed to set image view barrier: %s", error);
            return false;
        }

        PalClearValue clearValue;
        clearValue.color[0] = 0.2f;
        clearValue.color[1] = 0.2f;
        clearValue.color[2] = 0.2f;
        clearValue.color[3] = 1.0f;

        PalAttachmentDesc colorAttachment = {0};
        colorAttachment.loadOp = PAL_LOAD_OP_CLEAR;
        colorAttachment.storeOp = PAL_STORE_OP_STORE;
        colorAttachment.clearValue = clearValue;
        colorAttachment.imageView = imageViews[imageIndex];

        PalRenderingInfo renderingInfo = {0};
        renderingInfo.viewCount = 1;
        renderingInfo.colorAttachentCount = 1;
        renderingInfo.colorAttachments = &colorAttachment;

        result = palCmdBeginRendering(cmdBuffers[currentFrame], &renderingInfo);
        if (result != PAL_RESULT_SUCCESS) {
            const char* error = palFormatResult(result);
            palLog(nullptr, "Failed to begin rendering: %s", error);
            return false;
        }

        result = palCmdEndRendering(cmdBuffers[currentFrame]);
        if (result != PAL_RESULT_SUCCESS) {
            const char* error = palFormatResult(result);
            palLog(nullptr, "Failed to end rendering: %s", error);
            return false;
        }

        // change the state of the image view to make it presentable
        oldUsageStateInfo = newUsageStateInfo;
        newUsageStateInfo.usageState = PAL_USAGE_STATE_PRESENT;
        result = palCmdImageBarrier(
            cmdBuffers[currentFrame],
            image,
            &imageRange,
            &oldUsageStateInfo,
            &newUsageStateInfo);

        if (result != PAL_RESULT_SUCCESS) {
            const char* error = palFormatResult(result);
            palLog(nullptr, "Failed to set image view barrier: %s", error);
            return false;
        }

        result = palCmdEnd(cmdBuffers[currentFrame]);
        if (result != PAL_RESULT_SUCCESS) {
            const char* error = palFormatResult(result);
            palLog(nullptr, "Failed to end command buffer: %s", error);
            return false;
        }

        // submit command buffer
        PalCommandBufferSubmitInfo submitInfo = {0};
        submitInfo.cmdBuffer = cmdBuffers[currentFrame];
        submitInfo.fence = inFlightFences[currentFrame];
        submitInfo.waitSemaphore = imageAvailableSemaphores[currentFrame];
        submitInfo.signalSemaphore = renderFinishedSemaphores[imageIndex];

        result = palSubmitCommandBuffer(queue, &submitInfo);
        if (result != PAL_RESULT_SUCCESS) {
            const char* error = palFormatResult(result);
            palLog(nullptr, "Failed to submit command buffer: %s", error);
            return false;
        }

        // present
        PalSwapchainPresentInfo presentInfo = {0};
        presentInfo.imageIndex = imageIndex;
        presentInfo.waitSemaphore = renderFinishedSemaphores[imageIndex];
        result = palPresentSwapchain(swapchain, &presentInfo);
        if (result != PAL_RESULT_SUCCESS) {
            const char* error = palFormatResult(result);
            palLog(nullptr, "Failed to present swapchain: %s", error);
            return false;
        }

        currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
    }

    result = palWaitQueue(queue);
    if (result != PAL_RESULT_SUCCESS) {
        const char* error = palFormatResult(result);
        palLog(nullptr, "Failed to wait for queue: %s", error);
        return false;
    }

    for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        palDestroySemaphore(imageAvailableSemaphores[i]);
        palDestroyFence(inFlightFences[i]);
        palFreeCommandBuffer(cmdBuffers[i]);
    }

    for (int i = 0; i < imageCount; i++) {
        palDestroySemaphore(renderFinishedSemaphores[i]);
        palDestroyImageView(imageViews[i]);   
    }

    palDestroyCommandPool(cmdPool);
    palDestroySwapchain(swapchain);
    palDestroySurface(surface);
    palDestroyQueue(queue);
    palDestroyDevice(device);
    palShutdownGraphics();

    palFree(nullptr, imageViews);
    palFree(nullptr, renderFinishedSemaphores);
    palFree(nullptr, inFlightImages);

    palDestroyWindow(window);
    palShutdownVideo();
    palDestroyEventDriver(eventDriver);
    return true;
}
