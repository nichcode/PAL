
#include "pal/pal_graphics.h"
#include "pal/pal_video.h"
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
    palLog(nullptr, "");
    palLog(nullptr, "===========================================");
    palLog(nullptr, "Clear Color Test");
    palLog(nullptr, "===========================================");
    palLog(nullptr, "");

    PalResult result;
    PalWindow* window = nullptr;
    PalEventDriver* eventDriver = nullptr;
    PalGraphicsWindow gfxWindow;

    PalAdapter* adapter = nullptr;
    PalDevice* device = nullptr;
    PalQueue* queue = nullptr;
    PalSwapchain* swapchain = nullptr;
    PalCommandPool* cmdPool = nullptr;
    PalImageView** imageViews = nullptr;

    PalCommandBuffer* cmdBuffers[MAX_FRAMES_IN_FLIGHT];
    PalSemaphore* presentCompleteSemaphores[MAX_FRAMES_IN_FLIGHT];
    PalSemaphore** renderFinishedSemaphores; // count of swapchain images
    PalFence* inFlightFences[MAX_FRAMES_IN_FLIGHT];

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

    PalGraphicsDebugger debugger;
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

    PalAdapterCapabilities caps;
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

    // create a graphics command queue
    result = palCreateQueue(device, PAL_QUEUE_TYPE_GRAPHICS, &queue);
    if (result != PAL_RESULT_SUCCESS) {
        const char* error = palFormatResult(result);
        palLog(nullptr, "Failed to create queue: %s", error);
        return false;
    }

    if (!palCanQueuePresent(queue, &gfxWindow)) {
        palLog(nullptr, "Queue cannot present to window");
        return false;
    }

    // create a swapchain with the graphics queue
    PalSwapchainCapabilities swapchainCaps = {0};
    result = palQuerySwapchainCapabilities(device, &gfxWindow, &swapchainCaps);

    if (result != PAL_RESULT_SUCCESS) {
        const char* error = palFormatResult(result);
        palLog(nullptr, "Failed to query swapchain capabilities: %s", error);
        return false;
    }

    PalSwapchainCreateInfo swapchainCreateInfo = {0};
    swapchainCreateInfo.clipped = true;
    swapchainCreateInfo.compositeAlpha = PAL_COMPOSITE_ALPHA_OPAQUE;
    swapchainCreateInfo.format = PAL_SWAPCHAIN_FORMAT_RGBA8_UNORM_SRGB;
    swapchainCreateInfo.height = WINDOW_HEIGHT;
    swapchainCreateInfo.width = WINDOW_WIDTH;
    swapchainCreateInfo.imageArrayLayerCount = 1;

    // rare but possible on andriod
    if (WINDOW_WIDTH > swapchainCaps.maxImageWidth) {
        swapchainCreateInfo.width = swapchainCaps.maxImageWidth / 2;
    }

    if (WINDOW_HEIGHT > swapchainCaps.maxImageHeight) {
        swapchainCreateInfo.height = swapchainCaps.maxImageHeight / 2;
    }

    // check if the minimal image count is not good for you
    // and increase it but not pass the max count
    swapchainCreateInfo.imageCount = swapchainCaps.minImageCount;
    swapchainCreateInfo.presentMode = PAL_PRESENT_MODE_FIFO;

    result = palCreateSwapchain(device, queue, &gfxWindow, &swapchainCreateInfo, &swapchain);

    if (result != PAL_RESULT_SUCCESS) {
        const char* error = palFormatResult(result);
        palLog(nullptr, "Failed to create swapchain: %s", error);
        return false;
    }

    // get all swapchain images and create image views for them
    Uint32 imageCount = swapchainCreateInfo.imageCount;
    imageViews = palAllocate(nullptr, sizeof(PalImageView*) * imageCount, 0);

    renderFinishedSemaphores = palAllocate(nullptr, sizeof(PalSemaphore*) * imageCount, 0);

    if (!imageViews || !renderFinishedSemaphores) {
        palLog(nullptr, "Failed to allocate memory");
        return false;
    }

    PalImageViewCreateInfo imageViewCreateInfo = {0};
    imageViewCreateInfo.layerArrayCount = 1;
    imageViewCreateInfo.mipLevelCount = 1;
    imageViewCreateInfo.startArrayLayer = 0;
    imageViewCreateInfo.startMipLevel = 0;
    imageViewCreateInfo.type = PAL_IMAGE_VIEW_TYPE_2D;
    imageViewCreateInfo.usages = PAL_IMAGE_VIEW_USAGE_COLOR;

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
    }

    result = palCreateCommandPool(device, queue, &cmdPool);

    if (result != PAL_RESULT_SUCCESS) {
        const char* error = palFormatResult(result);
        palLog(nullptr, "Failed to create command pool: %s", error);
        return false;
    }

    // create synchronization objects and command buffers
    for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        result = palCreateSemaphore(device, &presentCompleteSemaphores[i]);
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

    // create synchronization objects
    for (int i = 0; i < imageCount; i++) {
        result = palCreateSemaphore(device, &renderFinishedSemaphores[i]);
        if (result != PAL_RESULT_SUCCESS) {
            const char* error = palFormatResult(result);
            palLog(nullptr, "Failed to create semaphore: %s", error);
            return false;
        }
    }

    // main loop
    Uint32 currentFrame = 0;
    bool running = true;
    PalSemaphore* presentCompleteSemaphore = nullptr;
    PalSemaphore* renderFinishedSemaphore = nullptr;
    PalFence* fence = nullptr;
    PalCommandBuffer* cmdBuffer = nullptr;

    bool firstImageViewUse[8];
    memset(firstImageViewUse, 1, sizeof(bool) * 8);

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

        fence = inFlightFences[currentFrame];
        presentCompleteSemaphore = presentCompleteSemaphores[currentFrame];
        cmdBuffer = cmdBuffers[currentFrame];

        result = palWaitFence(fence, UINT64_MAX);
        if (result != PAL_RESULT_SUCCESS) {
            const char* error = palFormatResult(result);
            palLog(nullptr, "Failed to wait fence: %s", error);
            return false;
        }

        if (adapterFeatures & PAL_ADAPTER_FEATURE_FENCE_RESET) {
            result = palResetFence(fence);
            if (result != PAL_RESULT_SUCCESS) {
                const char* error = palFormatResult(result);
                palLog(nullptr, "Failed to wait fence: %s", error);
                return false;
            }

        } else {
            // recreate since we dont support fence resetting
            palDestroyFence(fence);

            result = palCreateFence(device, false, &fence);
            if (result != PAL_RESULT_SUCCESS) {
                const char* error = palFormatResult(result);
                palLog(nullptr, "Failed to wait fence: %s", error);
                return false;
            }
        }

        // get next swapchain image
        PalSwapchainNextImageInfo nextImageInfo = {0};
        nextImageInfo.fence = nullptr;
        nextImageInfo.signalSemaphore = presentCompleteSemaphore;
        nextImageInfo.timeout = UINT64_MAX;

        Uint32 index = 0;
        result = palGetNextSwapchainImage(swapchain, &nextImageInfo, &index);
        if (result != PAL_RESULT_SUCCESS) {
            const char* error = palFormatResult(result);
            palLog(nullptr, "Failed to get next swapchain image: %s", error);
            return false;
        }

        renderFinishedSemaphore = renderFinishedSemaphores[index];

        // reset the command buffer
        result = palResetCommandBuffer(cmdBuffer);
        if (result != PAL_RESULT_SUCCESS) {
            const char* error = palFormatResult(result);
            palLog(nullptr, "Failed to reset command buffer: %s", error);
            return false;
        }

        result = palBeginCommandBuffer(cmdBuffer, nullptr);
        if (result != PAL_RESULT_SUCCESS) {
            const char* error = palFormatResult(result);
            palLog(nullptr, "Failed to begin command buffer: %s", error);
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
        colorAttachment.imageView = imageViews[index];

        PalRenderingInfo renderingInfo = {0};
        renderingInfo.viewCount = 1;
        renderingInfo.colorAttachentCount = 1;
        renderingInfo.colorAttachments = &colorAttachment;
        renderingInfo.layerCount = 1;
        renderingInfo.multisampleCount = PAL_SAMPLE_COUNT_1;
        renderingInfo.renderArea.width = WINDOW_WIDTH;
        renderingInfo.renderArea.height = WINDOW_HEIGHT;

        // change the state of the image view to make it renderable
        PalUsageStateInfo oldUsageStateInfo = {0};
        PalUsageStateInfo newUsageStateInfo = {0};
        newUsageStateInfo.usageState = PAL_USAGE_STATE_COLOR_ATTACHMENT_WRITE;

        if (firstImageViewUse[index]) {
            oldUsageStateInfo.usageState = PAL_USAGE_STATE_UNDEFINED;
        } else {
            oldUsageStateInfo.usageState = PAL_USAGE_STATE_PRESENT;
        }

        result = palImageViewBarrier(
            cmdBuffer,
            imageViews[index],
            &oldUsageStateInfo,
            &newUsageStateInfo);

        if (result != PAL_RESULT_SUCCESS) {
            const char* error = palFormatResult(result);
            palLog(nullptr, "Failed to set image view barrier: %s", error);
            return false;
        }

        if (result != PAL_RESULT_SUCCESS) {
            const char* error = palFormatResult(result);
            palLog(nullptr, "Failed to set image view barrier: %s", error);
            return false;
        }

        result = palBeginRendering(cmdBuffer, &renderingInfo);
        if (result != PAL_RESULT_SUCCESS) {
            const char* error = palFormatResult(result);
            palLog(nullptr, "Failed to begin rendering: %s", error);
            return false;
        }

        result = palEndRendering(cmdBuffer);
        if (result != PAL_RESULT_SUCCESS) {
            const char* error = palFormatResult(result);
            palLog(nullptr, "Failed to end rendering: %s", error);
            return false;
        }

        // change the state of the image view to make it presentable
        oldUsageStateInfo = newUsageStateInfo;
        newUsageStateInfo.usageState = PAL_USAGE_STATE_PRESENT;
        result = palImageViewBarrier(
            cmdBuffer,
            imageViews[index],
            &oldUsageStateInfo,
            &newUsageStateInfo);

        if (result != PAL_RESULT_SUCCESS) {
            const char* error = palFormatResult(result);
            palLog(nullptr, "Failed to set image view barrier: %s", error);
            return false;
        }

        result = palEndCommandBuffer(cmdBuffer);
        if (result != PAL_RESULT_SUCCESS) {
            const char* error = palFormatResult(result);
            palLog(nullptr, "Failed to end command buffer: %s", error);
            return false;
        }

        // submit command buffer
        PalCommandBufferSubmitInfo submitInfo = {0};
        submitInfo.cmdBuffer = cmdBuffer;
        submitInfo.fence = fence;
        submitInfo.waitSemaphore = presentCompleteSemaphore;
        submitInfo.signalSemaphore = renderFinishedSemaphore;

        result = palSubmitCommandBuffer(queue, &submitInfo);
        if (result != PAL_RESULT_SUCCESS) {
            const char* error = palFormatResult(result);
            palLog(nullptr, "Failed to submit command buffer: %s", error);
            return false;
        }

        // present
        PalSwapchainPresentInfo presentInfo = {0};
        presentInfo.imageIndex = index;
        presentInfo.waitSemaphore = renderFinishedSemaphore;
        result = palPresentSwapchain(swapchain, &presentInfo);
        if (result != PAL_RESULT_SUCCESS) {
            const char* error = palFormatResult(result);
            palLog(nullptr, "Failed to present swapchain: %s", error);
            return false;
        }

        currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
    }

    result = palWaitDevice(device);
    if (result != PAL_RESULT_SUCCESS) {
        const char* error = palFormatResult(result);
        palLog(nullptr, "Failed to wait for device: %s", error);
        return false;
    }

    for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        palDestroySemaphore(presentCompleteSemaphores[i]);
        palDestroyFence(inFlightFences[i]);
        palFreeCommandBuffer(cmdBuffers[i]);
    }

    for (int i = 0; i < imageCount; i++) {
        palDestroyImageView(imageViews[i]);
        palDestroySemaphore(renderFinishedSemaphores[i]);
    }

    palDestroyCommandPool(cmdPool);
    palDestroySwapchain(swapchain);
    palDestroyQueue(queue);
    palDestroyDevice(device);
    palShutdownGraphics();
    palFree(nullptr, imageViews);

    palDestroyWindow(window);
    palShutdownVideo();
    palDestroyEventDriver(eventDriver);
    return true;
}
