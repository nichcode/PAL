
#include "pal/pal_graphics.h"
#include "pal/pal_video.h"
#include "tests.h"

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
    PalAdapter* adapter = nullptr;
    PalDevice* device = nullptr;
    PalQueue* queue = nullptr;
    PalSwapchain* swapchain = nullptr;
    PalCommandPool* cmdPool = nullptr;
    PalImageView** imageViews = nullptr;
    PalCommandBuffer** cmdBuffers = nullptr;
    PalRenderPass** renderPasses = nullptr;

    // create an event driver
    PalEventDriverCreateInfo eventDriverCreateInfo = {0};
    result = palCreateEventDriver(&eventDriverCreateInfo, &eventDriver);
    if (result != PAL_RESULT_SUCCESS) {
        const char* error = palFormatResult(result);
        palLog(nullptr, "Failed to create event driver: %s", error);
        return false;
    }

    // we only need window close and keydown(escape) for borderless window
    palSetEventDispatchMode(
        eventDriver, 
        PAL_EVENT_WINDOW_CLOSE, 
        PAL_DISPATCH_POLL);

    palSetEventDispatchMode(
        eventDriver, 
        PAL_EVENT_KEYDOWN, 
        PAL_DISPATCH_POLL);

    // initialize the video system
    // you can use any library you want
    result = palInitVideo(nullptr, eventDriver);
    if (result != PAL_RESULT_SUCCESS) {
        const char* error = palFormatResult(result);
        palLog(nullptr, "Failed to initialize video: %s", error);
        return false;
    }

    // create a window
    PalWindowCreateInfo windowCreateInfo = {0};
    windowCreateInfo.height = 480;
    windowCreateInfo.width = 640;
    windowCreateInfo.show = true;
    windowCreateInfo.title = "Clear Color Window";

    // check if we support decorated windows (title bar, close etc)
    PalVideoFeatures64 videoFeatures = palGetVideoFeaturesEx();
    if (!(videoFeatures & PAL_VIDEO_FEATURE64_DECORATED_WINDOW)) {
        // if we dont support, we need to create a borderless window
        // and create the decorations ourselves
        windowCreateInfo.style |= PAL_WINDOW_STYLE_BORDERLESS;
    }

    result = palCreateWindow(&windowCreateInfo, &window);
    if (result != PAL_RESULT_SUCCESS) {
        const char* error = palFormatResult(result);
        palLog(nullptr, "Failed to create window: %s", error);
        return false;
    }

    // initialize the graphics system 
    result = palInitGraphics(false, nullptr);
    if (result != PAL_RESULT_SUCCESS) {
        const char* error = palFormatResult(result);
        palLog(nullptr, "Failed to initialize graphics: %s", error);
        return false;
    }

    // enumerate all available adapters
    Int32 count = 0;
    result = palEnumerateAdapters(&count, nullptr);
    if (result != PAL_RESULT_SUCCESS) {
        const char* error = palFormatResult(result);
        palLog(nullptr, "Failed to get query adapters: %s", error);
        return false;
    }

    if (count == 0) {
        palLog(nullptr, "No adapters found");
        return false;
    }
    palLog(nullptr, "Adapter count: %d", count);

    // allocate an array of adapters or use a fixed array
    // Example: PalAdapter* adapters[12];
    PalAdapter** adapters = nullptr;
    adapters = palAllocate(nullptr, sizeof(PalAdapter*) * count, 0);
    if (!adapters) {
        palLog(nullptr, "Failed to allocate memory");
        return false;
    }

    result = palEnumerateAdapters(&count, adapters);
    if (result != PAL_RESULT_SUCCESS) {
        const char* error = palFormatResult(result);
        palLog(nullptr, "Failed to get query adapters: %s", error);
        return false;
    }

    // get information about all the adapters and find the adapter that 
    // has graphics queue
    PalAdapterCapabilities caps;
    for (Int32 i = 0; i < count; i++) {
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

    // create a device
    PalAdapterFeatures features = PAL_ADAPTER_FEATURE_SWAPCHAIN;
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

    // check if the queue can present on our window
    PalWindowHandleInfo handleInfo;
    PalGraphicsWindow gfxWindow;
    handleInfo = palGetWindowHandleInfo(window);
    gfxWindow.window = handleInfo.nativeWindow;
    gfxWindow.display = handleInfo.nativeDisplay;

    if (!palCanQueuePresent(queue, &gfxWindow)) {
        palLog(nullptr, "Queue cannot present to window");
        return false;
    }

    // create a swapchain with the graphics queue
    PalSwapchainCapabilities swapchainCaps = {0};
    result = palQuerySwapchainCapabilities(
        device, 
        &gfxWindow, 
        &swapchainCaps);

    if (result != PAL_RESULT_SUCCESS) {
        const char* error = palFormatResult(result);
        palLog(nullptr, "Failed to query swapchain capabilities: %s", error);
        return false;
    }

    PalSwapchainCreateInfo swapchainCreateInfo = {0};
    swapchainCreateInfo.clipped = true;
    swapchainCreateInfo.compositeAlpha = PAL_COMPOSITE_ALPHA_OPAQUE;
    swapchainCreateInfo.format = PAL_SWAPCHAIN_FORMAT_RGBA8_UNORM_SRGB;
    swapchainCreateInfo.height = windowCreateInfo.height;
    swapchainCreateInfo.width = windowCreateInfo.width;
    swapchainCreateInfo.imageArrayLayerCount = 1;

    // rare but possible on andriod
    if (windowCreateInfo.width > swapchainCaps.maxImageWidth) {
        swapchainCreateInfo.width = swapchainCaps.maxImageWidth / 2;
    }

    if (windowCreateInfo.height > swapchainCaps.maxImageHeight) {
        swapchainCreateInfo.height = swapchainCaps.maxImageHeight / 2;
    }

    // check if the minimal image count is not good for you 
    // and increase it nut not pass the max count
    swapchainCreateInfo.imageCount = swapchainCaps.minImageCount;
    swapchainCreateInfo.presentMode = PAL_PRESENT_MODE_FIFO;

    result = palCreateSwapchain(
        device, 
        queue, 
        &gfxWindow, 
        &swapchainCreateInfo, 
        &swapchain);

    if (result != PAL_RESULT_SUCCESS) {
        const char* error = palFormatResult(result);
        palLog(nullptr, "Failed to create swapchain: %s", error);
        return false;
    }

    // create a command pool for the command buffers
    PalCommandPoolCreateInfo cmdPoolCreateInfo = {0};
    cmdPoolCreateInfo.queue = queue;
    result = palCreateCommandPool(device, &cmdPoolCreateInfo, &cmdPool);
    if (result != PAL_RESULT_SUCCESS) {
        const char* error = palFormatResult(result);
        palLog(nullptr, "Failed to create command pool: %s", error);
        return false;
    }

    // get all swapchain images and create command buffers and image views
    // for them. This is much faster than resetting and recording per frames
    Uint32 imageCount = swapchainCreateInfo.imageCount;
    imageViews = palAllocate(
        nullptr, 
        sizeof(PalImageView*) * imageCount,
        0);

    renderPasses = palAllocate(
        nullptr, 
        sizeof(PalRenderPass*) * imageCount,
        0);

    cmdBuffers = palAllocate(
        nullptr, 
        sizeof(PalCommandBuffer*) * imageCount,
        0);

    if (!imageViews || !renderPasses || !cmdBuffers) {
        palLog(nullptr, "Failed to allocate memory");
        palFree(nullptr, imageViews);
        palFree(nullptr, cmdBuffers);
        palFree(nullptr, renderPasses);
        return false;
    }

    for (int i = 0; i < imageCount; i++) {
        // get swapchain image
        PalImage* image = palGetSwapchainImage(swapchain, i);
        if (!image) {
            palLog(nullptr, "Failed to get swapchain image");
        }

        // create image view
        // swapchain images are 2D and they only supports 2D 
        // and 2D array view types
        PalImageView* imageView = nullptr;
        PalImageViewCreateInfo imageViewCreateInfo = {0};
        imageViewCreateInfo.layerArrayCount = 1;
        imageViewCreateInfo.mipLevelCount = 1;
        imageViewCreateInfo.startArrayLayer = 0;
        imageViewCreateInfo.startMipLevel = 0;
        imageViewCreateInfo.type = PAL_IMAGE_VIEW_TYPE_2D;
        imageViewCreateInfo.usages = PAL_IMAGE_VIEW_USAGE_COLOR;

        result = palCreateImageView(
            device, 
            image, 
            &imageViewCreateInfo, 
            &imageView);

        if (result != PAL_RESULT_SUCCESS) {
            const char* error = palFormatResult(result);
            palLog(nullptr, "Failed to create image view: %s", error);
            return false;
        }

        // create a render pass with the image view as a color attahcment
        PalAttachmentDesc colorAttachment = {0};
        colorAttachment.loadOp = PAL_LOAD_OP_CLEAR;
        colorAttachment.storeOp = PAL_STORE_OP_STORE;
        colorAttachment.type = PAL_ATTACHMENT_TYPE_COLOR;
        colorAttachment.target = imageView;

        PalRenderPassCreateInfo renderPasscreateInfo = {0};
        renderPasscreateInfo.attachmentCount = 1;
        renderPasscreateInfo.attachments = &colorAttachment;

        // render area
        renderPasscreateInfo.width = swapchainCreateInfo.width;
        renderPasscreateInfo.height = swapchainCreateInfo.height;

        PalRenderPass* renderPass = nullptr;
        result = palCreateRenderPass(
            device, 
            &renderPasscreateInfo, 
            &renderPass);

        if (result != PAL_RESULT_SUCCESS) {
            const char* error = palFormatResult(result);
            palLog(nullptr, "Failed to create render pass: %s", error);
            return false;
        }

        // create a command buffer for the image view
        PalCommandBuffer* cmdBuffer = nullptr;
        result = palCreateCommandBuffer(
            device, 
            cmdPool, 
            PAL_COMMAND_BUFFER_TYPE_PRIMARY, 
            &cmdBuffer);

        if (result != PAL_RESULT_SUCCESS) {
            const char* error = palFormatResult(result);
            palLog(nullptr, "Failed to create command buffer: %s", error);
            return false;
        }

        // record commands
        PalClearValue clearValue = {0};
        clearValue.color[0] = 0.2f;
        clearValue.color[1] = 0.2f;
        clearValue.color[2] = 0.2f;
        clearValue.color[3] = 1.0f;

        result = palBeginRenderPass(cmdBuffer, renderPass, 1, &clearValue);
        if (result != PAL_RESULT_SUCCESS) {
            const char* error = palFormatResult(result);
            palLog(nullptr, "Failed to begin render pass: %s", error);
            return false;
        }

        result = palEndRenderPass(cmdBuffer);
        if (result != PAL_RESULT_SUCCESS) {
            const char* error = palFormatResult(result);
            palLog(nullptr, "Failed to end render pass: %s", error);
            return false;
        }
        
        // cache everything so we can reference and destroy later
        renderPasses[i] = renderPass;
        cmdBuffers[i] = cmdBuffer;
        imageViews[i] = imageView;
    }

    // main loop
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

        // get the next image that we can render too
        PalImage* image = nullptr;
        PalNextImageInfo nextImageInfo = {0};
        nextImageInfo.timeout = UINT64_MAX;
        image = palGetNextSwapchainImage(swapchain, &nextImageInfo);

        // find the command buffer associated with the image
        // this is fast, the swapchain caches all it images internally
        PalCommandBuffer* cmdBuffer = nullptr;
        for (int i = 0; i < imageCount; i++) {
            if (image == palGetSwapchainImage(swapchain, i)) {
                cmdBuffer = cmdBuffers[i];
                break;
            }
        }

        // submit to the queue and present
        PalSubmitInfo submitInfo = {0};
        submitInfo.cmdBuffer = cmdBuffer;

        result = palSubmitCommandBuffer(queue, &submitInfo);
        if (result != PAL_RESULT_SUCCESS) {
            const char* error = palFormatResult(result);
            palLog(nullptr, "Failed to submit command buffer: %s", error);
            return false;
        }

        PalPresentInfo presentInfo = {0};
        presentInfo.image = image;

        result = palPresentSwapchain(swapchain, &presentInfo);
        if (result != PAL_RESULT_SUCCESS) {
            const char* error = palFormatResult(result);
            palLog(nullptr, "Failed to present swapchain: %s", error);
            return false;
        }
    }

    // since we dont use a fence or sempahore, there is no way to know
    // if the GPU is done presenting and we can now destroy the swapchain
    // so we just wait for a while
    // and we dont want to use thread system for this
    Int32 counter = 0;
    for (int i = 0; i < 30000; i++) {
        counter++;
    }

    // cleanup
    for (int i = 0; i < imageCount; i++) {
        palDestroyRenderPass(renderPasses[i]);
        palDestroyCommandBuffer(cmdBuffers[i]);
        palDestroyImageView(imageViews[i]);
    }

    palDestroyCommandPool(cmdPool);
    palDestroySwapchain(swapchain); 
    palDestroyQueue(queue);
    palDestroyDevice(device);

    palDestroyWindow(window);
    palShutdownVideo();

    palShutdownGraphics();

    palFree(nullptr, imageViews);
    palFree(nullptr, cmdBuffers);
    palFree(nullptr, renderPasses);

    return true;
}