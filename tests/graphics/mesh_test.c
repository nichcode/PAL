
#include "graphics_helper.h"

static void PAL_CALL onGraphicsDebug(
    void* userData,
    PalDebugMessageSeverity severity,
    PalDebugMessageType type,
    const char* msg)
{
    palLog(nullptr, msg);
}

PalBool meshTest()
{
    PalResult result;
    PalWindow* window = nullptr;
    PalEventDriver* eventDriver = nullptr;

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
    PalFence** inFlightImages;               // count of swapchain images

    PalPipelineLayout* pipelineLayout = nullptr;
    PalPipeline* pipeline = nullptr;
    PalShader* shaders[2];

    eventDriver = helperCreateEventDriver(0, PAL_DISPATCH_MODE_POLL, nullptr);
    if (!eventDriver) {
        return PAL_FALSE;
    }

    window = gfxHelperCreateWindow("Mesh Window", eventDriver);
    if (!window) {
        return PAL_FALSE;
    }

    PalGraphicsDebugger debugger = {0};
    debugger.callback = onGraphicsDebug;
    debugger.userData = nullptr;

    result = palInitGraphics(nullptr, nullptr, 0, nullptr);
    if (result != PAL_RESULT_SUCCESS) {
        logResult(result, "Failed to initialize graphics");
        return PAL_FALSE;
    }

    // enumerate all available adapters
    uint32_t adapterCount = 0;
    result = palEnumerateAdapters(&adapterCount, nullptr);
    if (result != PAL_RESULT_SUCCESS) {
        logResult(result, "Failed to get adapters");
        return PAL_FALSE;
    }

    if (adapterCount == 0) {
        palLog(nullptr, "No adapters found");
        return PAL_FALSE;
    }
    palLog(nullptr, "Adapter count: %d", adapterCount);

    PalAdapter** adapters = nullptr;
    adapters = palAllocate(nullptr, sizeof(PalAdapter*) * adapterCount, 0);
    if (!adapters) {
        palLog(nullptr, "Failed to allocate memory");
        return PAL_FALSE;
    }

    result = palEnumerateAdapters(&adapterCount, adapters);
    if (result != PAL_RESULT_SUCCESS) {
        logResult(result, "Failed to get adapters");
        return PAL_FALSE;
    }

    PalAdapterCapabilities caps = {0};
    PalAdapterFeatures adapterFeatures = 0;
    PalAdapterInfo adapterInfo = {0};
    for (int32_t i = 0; i < adapterCount; i++) {
        adapter = adapters[i];
        palGetAdapterCapabilities(adapter, &caps);
        if (caps.maxGraphicsQueues == 0) {
            adapter = nullptr;
            continue;
        }

        adapterFeatures = palGetAdapterFeatures(adapter);
        if (!(adapterFeatures & PAL_ADAPTER_FEATURE_MESH_SHADER)) {
            adapter = nullptr;
            continue;
        }

        // We want an adapter that supports spirv 1.5 or dxil 6.5
        palGetAdapterInfo(adapter, &adapterInfo);

        // we prefer spirv first if an adapter supports multiple shader formats
        uint32_t target = 0;
        if (adapterInfo.shaderFormats & PAL_SHADER_FORMAT_SPIRV) {
            target = palGetHighestSupportedShaderTarget(adapter, PAL_SHADER_FORMAT_SPIRV);
            if (target >= PAL_MAKE_SHADER_TARGET(1, 5)) {
                break;
            }
        }

        if (adapterInfo.shaderFormats & PAL_SHADER_FORMAT_DXIL) {
            target = palGetHighestSupportedShaderTarget(adapter, PAL_SHADER_FORMAT_DXIL);
            if (target >= PAL_MAKE_SHADER_TARGET(6, 5)) {
                break;
            }
        }

        adapter = nullptr;
        continue;
    }

    palFree(nullptr, adapters);
    if (!adapter) {
        palLog(nullptr, "Failed to find a required adapter");
        return PAL_FALSE;
    }

    // create a device
    PalAdapterFeatures features = PAL_ADAPTER_FEATURE_SWAPCHAIN;
    features |= PAL_ADAPTER_FEATURE_MESH_SHADER;
    if (adapterFeatures & PAL_ADAPTER_FEATURE_FENCE_RESET) {
        features |= PAL_ADAPTER_FEATURE_FENCE_RESET;
    }

    result = palCreateDevice(adapter, features, &device);
    if (result != PAL_RESULT_SUCCESS) {
        logResult(result, "Failed to create device");
        return PAL_FALSE;
    }

    surface = gfxHelperCreateSurface(device, window);
    if (!surface) {
        return PAL_FALSE;
    }

    // create a graphics command queue and check if its supports presenting to the surface
    PalBool foundQueue = PAL_FALSE;
    for (int i = 0; i < caps.maxGraphicsQueues; i++) {
        result = palCreateQueue(device, PAL_QUEUE_TYPE_GRAPHICS, &queue);
        if (result != PAL_RESULT_SUCCESS) {
            logResult(result, "Failed to create queue");
            return PAL_FALSE;
        }

        if (!palCanQueuePresent(queue, surface)) {
            palDestroyQueue(queue);
            queue = nullptr;
        } else {
            // found a queue
            foundQueue = PAL_TRUE;
            break;
        }
    }

    if (!foundQueue) {
        palLog(nullptr, "Failed to find a queue that can present to the surface");
        return PAL_FALSE;
    }

    // create a swapchain with the graphics queue
    PalSurfaceCapabilities surfaceCaps = {0};
    palGetSurfaceCapabilities(device, surface, &surfaceCaps);

    PalSwapchainCreateInfo swapchainCreateInfo = {0};
    swapchainCreateInfo.clipped = PAL_TRUE;
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
            return PAL_FALSE;
        }
    }

    result = palCreateSwapchain(device, queue, surface, &swapchainCreateInfo, &swapchain);
    if (result != PAL_RESULT_SUCCESS) {
        logResult(result, "Failed to create swapchain");
        return PAL_FALSE;
    }

    // get all swapchain images and create image views for them
    uint32_t imageCount = swapchainCreateInfo.imageCount;
    imageViews = palAllocate(nullptr, sizeof(PalImageView*) * imageCount, 0);
    inFlightImages = palAllocate(nullptr, sizeof(PalFence*) * imageCount, 0);
    renderFinishedSemaphores = palAllocate(nullptr, sizeof(PalSemaphore*) * imageCount, 0);
    if (!imageViews || !inFlightImages || !renderFinishedSemaphores) {
        palLog(nullptr, "Failed to allocate memory");
        return PAL_FALSE;
    }

    PalImageInfo imageInfo;
    palGetImageInfo(palGetSwapchainImage(swapchain, 0), &imageInfo);

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
            logResult(result, "Failed to create image view");
            return PAL_FALSE;
        }

        // create render finished semaphores
        result = palCreateSemaphore(device, PAL_FALSE, &renderFinishedSemaphores[i]);
        if (result != PAL_RESULT_SUCCESS) {
            logResult(result, "Failed to create semaphore");
            return PAL_FALSE;
        }

        inFlightImages[i] = nullptr;
    }

    result = palCreateCommandPool(device, queue, &cmdPool);
    if (result != PAL_RESULT_SUCCESS) {
        logResult(result, "Failed to create command pool");
        return PAL_FALSE;
    }

    // create synchronization objects and command buffers
    for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        result = palCreateSemaphore(device, PAL_FALSE, &imageAvailableSemaphores[i]);
        if (result != PAL_RESULT_SUCCESS) {
            logResult(result, "Failed to create semaphore");
            return PAL_FALSE;
        }

        result = palCreateFence(device, PAL_TRUE, &inFlightFences[i]);
        if (result != PAL_RESULT_SUCCESS) {
            logResult(result, "Failed to create fence");
            return PAL_FALSE;
        }

        result = palAllocateCommandBuffer(
            device,
            cmdPool,
            PAL_COMMAND_BUFFER_TYPE_PRIMARY,
            &cmdBuffers[i]);

        if (result != PAL_RESULT_SUCCESS) {
            logResult(result, "Failed to allocate command buffer");
            return PAL_FALSE;
        }
    }

    // create shaders
    uint32_t bytecodeSize = 0;
    void* bytecode = nullptr;
    const char* sources[2];
    PalShaderEntryInfo entries[2];

    PalViewport viewport = {0};
    viewport.width = (float)WINDOW_WIDTH;
    viewport.maxDepth = 1.0f;

    PalShaderCreateInfo shaderCreateInfo = {0};
    if (adapterInfo.shaderFormats & PAL_SHADER_FORMAT_SPIRV) {
        sources[0] = "graphics/shaders/bin/spirv/mesh.spv";
        sources[1] = "graphics/shaders/bin/spirv/triangle_frag.spv";

        viewport.height = -(float)WINDOW_HEIGHT;
        viewport.y = (float)WINDOW_HEIGHT;

    } else if (adapterInfo.shaderFormats & PAL_SHADER_FORMAT_DXIL) {
        sources[0] = "graphics/shaders/bin/dxil/mesh.dxil";
        sources[1] = "graphics/shaders/bin/dxil/triangle_frag.dxil";

        viewport.height = (float)WINDOW_HEIGHT;
    }

    entries[0].stage = PAL_SHADER_STAGE_MESH;
    entries[0].entryName = "main";
    entries[0].patchControlPoints = 0;

    entries[1].stage = PAL_SHADER_STAGE_FRAGMENT;
    entries[1].entryName = "main";
    entries[1].patchControlPoints = 0;

    for (int i = 0; i < 2; i++) {
        // read file
        if (!readFile(sources[i], nullptr, &bytecodeSize)) {
            palLog(nullptr, "Failed to read shader file");
            return PAL_FALSE;
        }

        bytecode = palAllocate(nullptr, bytecodeSize, 0);
        if (!bytecode) {
            palLog(nullptr, "Failed to allocate memory");
            return PAL_FALSE;
        }

        readFile(sources[i], bytecode, &bytecodeSize);

        shaderCreateInfo.code = bytecode;
        shaderCreateInfo.codeSize = bytecodeSize;
        shaderCreateInfo.entries = &entries[i];
        shaderCreateInfo.entryCount = 1;

        result = palCreateShader(device, &shaderCreateInfo, &shaders[i]);
        if (result != PAL_RESULT_SUCCESS) {
            logResult(result, "Failed to create shader");
            return PAL_FALSE;
        }

        palFree(nullptr, bytecode);
    }

    // create a pipeline layout
    PalPipelineLayoutCreateInfo pipelineLayoutCreateInfo = {0};
    result = palCreatePipelineLayout(device, &pipelineLayoutCreateInfo, &pipelineLayout);
    if (result != PAL_RESULT_SUCCESS) {
        logResult(result, "Failed to create pipeline layout");
        return PAL_FALSE;
    }

    PalRenderingLayoutInfo renderingLayoutInfo = {0};
    renderingLayoutInfo.colorAttachentCount = 1;
    renderingLayoutInfo.colorAttachmentsFormat = &imageInfo.format;
    renderingLayoutInfo.sampleCount = PAL_SAMPLE_COUNT_1;
    renderingLayoutInfo.viewCount = 1;

    // create graphics pipeline
    PalGraphicsPipelineCreateInfo pipelineCreateInfo = {0};

    // color blend attachment
    PalColorBlendAttachment blendAttachment = {0};
    blendAttachment.colorWriteMask |= PAL_COLOR_MASK_RED;
    blendAttachment.colorWriteMask |= PAL_COLOR_MASK_GREEN;
    blendAttachment.colorWriteMask |= PAL_COLOR_MASK_BLUE;
    blendAttachment.colorWriteMask |= PAL_COLOR_MASK_ALPHA;

    pipelineCreateInfo.colorBlendAttachments = &blendAttachment;
    pipelineCreateInfo.colorBlendAttachmentCount = 1;

    // shaders
    pipelineCreateInfo.shaderCount = 2;
    pipelineCreateInfo.shaders = shaders;

    pipelineCreateInfo.topology = PAL_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    pipelineCreateInfo.pipelineLayout = pipelineLayout;
    pipelineCreateInfo.renderingLayout = &renderingLayoutInfo;

    result = palCreateGraphicsPipeline(device, &pipelineCreateInfo, &pipeline);
    if (result != PAL_RESULT_SUCCESS) {
        logResult(result, "Failed to create pipeline");
        return PAL_FALSE;
    }

    for (int i = 0; i < 2; i++) {
        palDestroyShader(shaders[i]);
    }

    // main loop
    uint32_t currentFrame = 0;
    PalBool running = PAL_TRUE;

    PalRect2D scissor = {0};
    scissor.height = WINDOW_HEIGHT;
    scissor.width = WINDOW_WIDTH;

    while (running) {
        // update the video system to push video events
        palUpdateVideo();

        PalEvent event;
        while (palPollEvent(eventDriver, &event)) {
            switch (event.type) {
                case PAL_EVENT_TYPE_WINDOW_CLOSE: {
                    running = PAL_FALSE;
                    break;
                }

                case PAL_EVENT_TYPE_KEYDOWN: {
                    PalKeycode keycode = 0;
                    palUnpackUint32(event.data, &keycode, nullptr);
                    if (keycode == PAL_KEYCODE_ESCAPE) {
                        running = PAL_FALSE;
                    }
                    break;
                }
            }
        }

        result = palWaitFence(inFlightFences[currentFrame], PAL_INFINITE);
        if (result != PAL_RESULT_SUCCESS) {
            logResult(result, "Failed to wait fence");
            return PAL_FALSE;
        }

        // get next swapchain image
        PalSwapchainNextImageInfo nextImageInfo = {0};
        nextImageInfo.fence = nullptr;
        nextImageInfo.signalSemaphore = imageAvailableSemaphores[currentFrame];
        nextImageInfo.timeout = PAL_INFINITE;

        uint32_t imageIndex = 0;
        result = palGetNextSwapchainImage(swapchain, &nextImageInfo, &imageIndex);
        if (result != PAL_RESULT_SUCCESS) {
            logResult(result, "Failed to get next swapchain image");
            return PAL_FALSE;
        }

        if (inFlightImages[imageIndex] != nullptr) {
            result = palWaitFence(inFlightImages[imageIndex], PAL_INFINITE);
            if (result != PAL_RESULT_SUCCESS) {
                logResult(result, "Failed to wait fence");
                return PAL_FALSE;
            }
        }

        inFlightImages[imageIndex] = inFlightFences[currentFrame];
        if (adapterFeatures & PAL_ADAPTER_FEATURE_FENCE_RESET) {
            result = palResetFence(inFlightFences[currentFrame]);
            if (result != PAL_RESULT_SUCCESS) {
                logResult(result, "Failed to wait fence");
                return PAL_FALSE;
            }

        } else {
            // recreate since we dont support fence resetting
            palDestroyFence(inFlightFences[currentFrame]);

            result = palCreateFence(device, PAL_FALSE, &inFlightFences[currentFrame]);
            if (result != PAL_RESULT_SUCCESS) {
                logResult(result, "Failed to wait fence");
                return PAL_FALSE;
            }
        }

        // reset the command buffer
        result = palResetCommandBuffer(cmdBuffers[currentFrame]);
        if (result != PAL_RESULT_SUCCESS) {
            logResult(result, "Failed to reset command buffer");
            return PAL_FALSE;
        }

        result = palCmdBegin(cmdBuffers[currentFrame], nullptr);
        if (result != PAL_RESULT_SUCCESS) {
            logResult(result, "Failed to begin command buffer");
            return PAL_FALSE;
        }

        // change the state of the image view to make it renderable
        PalBarrierInfo barrierInfo = {0};
        barrierInfo.newState = PAL_USAGE_STATE_COLOR_ATTACHMENT_WRITE;
        barrierInfo.dstStages = PAL_PIPELINE_STAGE_COLOR_ATTACHMENT;

        PalImageSubresourceRange imageRange = {0};
        imageRange.layerArrayCount = 1;
        imageRange.mipLevelCount = 1;
        imageRange.startArrayLayer = 0;
        imageRange.startMipLevel = 0;

        PalImage* image = palGetSwapchainImage(swapchain, imageIndex);
        palCmdImageBarrier(cmdBuffers[currentFrame], image, &imageRange, &barrierInfo);

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
        renderingInfo.arrayLayerCount = 1;
        renderingInfo.viewCount = 1;
        renderingInfo.renderArea.width = WINDOW_WIDTH;
        renderingInfo.renderArea.height = WINDOW_HEIGHT;

        palCmdBeginRendering(cmdBuffers[currentFrame], &renderingInfo);
        palCmdBindPipeline(cmdBuffers[currentFrame], pipeline);
        palCmdSetViewport(cmdBuffers[currentFrame], 1, &viewport);
        palCmdSetScissors(cmdBuffers[currentFrame], 1, &scissor);

        // draw a single triangle with the mesh shader
        // palBuildWorkGroupInfo() is a helper to build
        // the workgroup count per axis using normal
        // workCount (image size, buffer size)
        palCmdDrawMeshTasks(cmdBuffers[currentFrame], 1, 1, 1);
        palCmdEndRendering(cmdBuffers[currentFrame]);

        // change the state of the image view to make it presentable
        barrierInfo.oldState = barrierInfo.newState;
        barrierInfo.srcStages = barrierInfo.dstStages;
        barrierInfo.newState = PAL_USAGE_STATE_PRESENT;
        barrierInfo.dstStages = PAL_PIPELINE_STAGE_NONE;
        palCmdImageBarrier(cmdBuffers[currentFrame], image, &imageRange, &barrierInfo);

        result = palCmdEnd(cmdBuffers[currentFrame]);
        if (result != PAL_RESULT_SUCCESS) {
            logResult(result, "Failed to end command buffer");
            return PAL_FALSE;
        }

        // submit command buffer
        PalCommandBufferSubmitInfo submitInfo = {0};
        submitInfo.cmdBuffer = cmdBuffers[currentFrame];
        submitInfo.fence = inFlightFences[currentFrame];
        submitInfo.waitSemaphore = imageAvailableSemaphores[currentFrame];
        submitInfo.signalSemaphore = renderFinishedSemaphores[imageIndex];
        submitInfo.waitStages = PAL_PIPELINE_STAGE_COLOR_ATTACHMENT;

        result = palSubmitCommandBuffer(queue, &submitInfo);
        if (result != PAL_RESULT_SUCCESS) {
            logResult(result, "Failed to submit command buffer");
            return PAL_FALSE;
        }

        // present
        result = palPresentSwapchain(swapchain, imageIndex, renderFinishedSemaphores[imageIndex]);
        if (result != PAL_RESULT_SUCCESS) {
            logResult(result, "Failed to present swapchain");
            return PAL_FALSE;
        }

        currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
    }

    result = palWaitQueue(queue);
    if (result != PAL_RESULT_SUCCESS) {
        logResult(result, "Failed to wait queue");
        return PAL_FALSE;
    }

    palDestroyPipeline(pipeline);
    palDestroyPipelineLayout(pipelineLayout);

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

    gfxHelperDestroyWindow(window, eventDriver);
    return PAL_TRUE;
}
