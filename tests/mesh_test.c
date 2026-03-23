
#include "pal/pal_graphics.h"
#include "pal/pal_video.h"
#include "tests.h"

#include <stdio.h>

#define WINDOW_WIDTH 640
#define WINDOW_HEIGHT 480
#define MAX_FRAMES_IN_FLIGHT 2

static bool readFile(
    const char* filename,
    void* buffer,
    Uint64* size)
{
    FILE* file = fopen(filename, "rb");
    if (!file) {
        return false;
    }

    fseek(file, 0, SEEK_END);
    Uint64 tmpSize = ftell(file);
    fseek(file, 0, SEEK_SET);

    if (buffer) {
        tmpSize = *size;
        size_t read = fread(buffer, 1, tmpSize, file);
        if (read != tmpSize) {
            return false;
        }
    }

    fclose(file);
    *size = tmpSize;
    return true;
}

static void PAL_CALL onGraphicsDebug(
    void* userData,
    PalDebugMessageSeverity severity,
    PalDebugMessageType type,
    const char* msg)
{
    palLog(nullptr, msg);
}

bool meshTest()
{
    palLog(nullptr, "");
    palLog(nullptr, "===========================================");
    palLog(nullptr, "Mesh Test");
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

    PalPipelineLayout* pipelineLayout = nullptr;
    PalPipeline* pipeline = nullptr;
    PalShader* meshShader = nullptr;
    PalShader* fragmentShader = nullptr;

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
    PalAdapterFeatures adapterFeatures;
    bool hasGfxQueue = false;
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
            hasGfxQueue = true;
            adapterFeatures = palGetAdapterFeatures(adapter);
            if (adapterFeatures & PAL_ADAPTER_FEATURE_MESH_SHADER) {
                break;
            }
        }
    }

    palFree(nullptr, adapters);
    if (!adapter) {
        if (hasGfxQueue) {
            palLog(nullptr, "Failed to find an adapter that supports graphics queue");

        } else {
            palLog(nullptr, "Failed to find an adapter that supports mesh shader");
        }
        return false;
    }

    PalAdapterInfo adapterInfo = {0};
    result = palGetAdapterInfo(adapter, &adapterInfo);
    if (result != PAL_RESULT_SUCCESS) {
        const char* error = palFormatResult(result);
        palLog(nullptr, "Failed to get adapter info: %s", error);
        return false;
    }

    // create a device
    PalAdapterFeatures features = PAL_ADAPTER_FEATURE_SWAPCHAIN;
    features |= PAL_ADAPTER_FEATURE_MESH_SHADER;
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
    swapchainCreateInfo.height = WINDOW_HEIGHT;
    swapchainCreateInfo.width = WINDOW_WIDTH;
    swapchainCreateInfo.imageArrayLayerCount = 1;
    swapchainCreateInfo.presentMode = PAL_PRESENT_MODE_FIFO;

    swapchainCreateInfo.format = PAL_SWAPCHAIN_FORMAT_RGBA8_UNORM_SRGB;
    if (!swapchainCaps.formats[PAL_SWAPCHAIN_FORMAT_RGBA8_UNORM_SRGB]) {
        // the format is not supported. we default to BGRA
        // if component mapping is not supported, we have to remap the component
        // from the shader rather instead of mapping when creating the image views
        swapchainCreateInfo.format = PAL_SWAPCHAIN_FORMAT_BGRA8_UNORM_SRGB;
    }

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
    if (swapchainCreateInfo.imageCount == 1) {
        swapchainCreateInfo.imageCount = 2;
        if (swapchainCaps.maxImageCount < 2) {
            palLog(nullptr, "Swapchain does not support double buffers");
            return false;
        }
    }

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
    imageViewCreateInfo.type = PAL_IMAGE_VIEW_TYPE_2D;
    imageViewCreateInfo.usages = PAL_IMAGE_VIEW_USAGE_COLOR;
    imageViewCreateInfo.subresourceRange.layerArrayCount = 1;
    imageViewCreateInfo.subresourceRange.mipLevelCount = 1;
    imageViewCreateInfo.subresourceRange.startArrayLayer = 0;
    imageViewCreateInfo.subresourceRange.startMipLevel = 0;

    // check multiple BGRA formats
    if (swapchainCreateInfo.format == PAL_SWAPCHAIN_FORMAT_BGRA8_UNORM_SRGB) {
        if (adapterFeatures & PAL_ADAPTER_FEATURE_COMPONENT_MAPPING) {
            imageViewCreateInfo.mapping.r = PAL_COMPONENT_SWIZZLE_B;
            imageViewCreateInfo.mapping.g = PAL_COMPONENT_SWIZZLE_G;
            imageViewCreateInfo.mapping.b = PAL_COMPONENT_SWIZZLE_R;
            imageViewCreateInfo.mapping.a = PAL_COMPONENT_SWIZZLE_A;
        }
    }

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

    // create shaders
    Uint64 bytecodeSize = 0;
    void* bytecode = nullptr;
    PalShaderCreateInfo shaderCreateInfo = {0};

    const char* meshShaderPath = nullptr;
    const char* fragShaderPath = nullptr;
    if (adapterInfo.shaderFormats & PAL_SHADER_FORMAT_SPIRV) {
        meshShaderPath = "shaders/mesh.spv";
        fragShaderPath = "shaders/triangle_frag.spv";
    }

    if (!readFile(meshShaderPath, nullptr, &bytecodeSize)) {
        palLog(nullptr, "Failed to find shader file");
        return false;
    }

    bytecode = palAllocate(nullptr, bytecodeSize, 0);
    if (!bytecode) {
        palLog(nullptr, "Failed to allocate memory");
        return false;
    }

    readFile(meshShaderPath, bytecode, &bytecodeSize);
    shaderCreateInfo.bytecode = bytecode;
    shaderCreateInfo.bytecodeSize = bytecodeSize;
    shaderCreateInfo.stage = PAL_SHADER_STAGE_MESH;

    result = palCreateShader(device, &shaderCreateInfo, &meshShader);
    if (result != PAL_RESULT_SUCCESS) {
        const char* error = palFormatResult(result);
        palLog(nullptr, "Failed to create mesh shader: %s", error);
        return false;
    }

    // fragment shader
    bytecodeSize = 0;
    palFree(nullptr, bytecode);
    bytecode = nullptr;

    if (!readFile(fragShaderPath, nullptr, &bytecodeSize)) {
        palLog(nullptr, "Failed to find shader file");
        return false;
    }

    bytecode = palAllocate(nullptr, bytecodeSize, 0);
    if (!bytecode) {
        palLog(nullptr, "Failed to allocate memory");
        return false;
    }

    readFile(fragShaderPath, bytecode, &bytecodeSize);
    shaderCreateInfo.bytecode = bytecode;
    shaderCreateInfo.bytecodeSize = bytecodeSize;
    shaderCreateInfo.stage = PAL_SHADER_STAGE_FRAGMENT;

    result = palCreateShader(device, &shaderCreateInfo, &fragmentShader);
    if (result != PAL_RESULT_SUCCESS) {
        const char* error = palFormatResult(result);
        palLog(nullptr, "Failed to create fragment shader: %s", error);
        return false;
    }

    palFree(nullptr, bytecode);

    // create a pipeline layout
    PalPipelineLayoutCreateInfo pipelineLayoutCreateInfo = {0};
    result = palCreatePipelineLayout(device, &pipelineLayoutCreateInfo, &pipelineLayout);
    if (result != PAL_RESULT_SUCCESS) {
        const char* error = palFormatResult(result);
        palLog(nullptr, "Failed to create pipeline layout: %s", error);
        return false;
    }

    // the graphics pipeline needs the layout of the rendering
    // info it will be used with
    // we get the any image from the swapchain and get the format
    // on the image since our color attachment takes a swapchain image
    PalImage* image = palGetSwapchainImage(swapchain, 0);
    PalImageInfo imageInfo = {0};
    result = palGetImageInfo(image, &imageInfo);
    if (result != PAL_RESULT_SUCCESS) {
        const char* error = palFormatResult(result);
        palLog(nullptr, "Failed to get image info: %s", error);
        return false;
    }

    PalRenderingLayoutInfo renderingLayoutInfo = {0};
    renderingLayoutInfo.colorAttachentCount = 1;
    renderingLayoutInfo.colorAttachmentsFormat = &imageInfo.format;
    renderingLayoutInfo.multisampleCount = PAL_SAMPLE_COUNT_1;
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

    // multisample state
    PalMultisampleState multisampleState = {0};
    multisampleState.sampleCount = PAL_SAMPLE_COUNT_1;
    pipelineCreateInfo.multisampleState = &multisampleState;

    // shaders
    PalShader* shaders[2];
    shaders[0] = meshShader;
    shaders[1] = fragmentShader;
    pipelineCreateInfo.shaderCount = 2;
    pipelineCreateInfo.shaders = shaders;

    pipelineCreateInfo.topology = PAL_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    pipelineCreateInfo.pipelineLayout = pipelineLayout;
    pipelineCreateInfo.renderingLayout = &renderingLayoutInfo;

    result = palCreateGraphicsPipeline(device, &pipelineCreateInfo, &pipeline);
    if (result != PAL_RESULT_SUCCESS) {
        const char* error = palFormatResult(result);
        palLog(nullptr, "Failed to create graphics pipeline: %s", error);
        return false;
    }

    palDestroyShader(meshShader);
    palDestroyShader(fragmentShader);

    // main loop
    Uint32 currentFrame = 0;
    bool running = true;
    PalSemaphore* presentCompleteSemaphore = nullptr;
    PalSemaphore* renderFinishedSemaphore = nullptr;
    PalFence* fence = nullptr;
    PalCommandBuffer* cmdBuffer = nullptr;

    bool firstImageViewUse[8];
    memset(firstImageViewUse, 1, sizeof(bool) * 8);

    // we are not resizing for the viewport and scissor will not change
    PalViewport viewport = {0};
    viewport.height = (float)WINDOW_HEIGHT;
    viewport.width = (float)WINDOW_WIDTH;
    viewport.maxDepth = 1.0f;

    PalRect2D scissor = {0};
    scissor.height = WINDOW_HEIGHT;
    scissor.width = WINDOW_WIDTH;

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

        result = palCmdBegin(cmdBuffer, nullptr);
        if (result != PAL_RESULT_SUCCESS) {
            const char* error = palFormatResult(result);
            palLog(nullptr, "Failed to begin command buffer: %s", error);
            return false;
        }

        // change the state of the image view to make it renderable
        PalUsageStateInfo oldUsageStateInfo = {0};
        PalUsageStateInfo newUsageStateInfo = {0};
        newUsageStateInfo.usageState = PAL_USAGE_STATE_COLOR_ATTACHMENT_WRITE;

        if (firstImageViewUse[index]) {
            oldUsageStateInfo.usageState = PAL_USAGE_STATE_UNDEFINED;
        } else {
            oldUsageStateInfo.usageState = PAL_USAGE_STATE_PRESENT;
        }

        PalImageSubresourceRange imageRange = {0};
        imageRange.layerArrayCount = 1;
        imageRange.mipLevelCount = 1;
        imageRange.startArrayLayer = 0;
        imageRange.startMipLevel = 0;

        PalImage* image = palGetSwapchainImage(swapchain, index);
        result = palCmdImageBarrier(
            cmdBuffer,
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
        colorAttachment.imageView = imageViews[index];

        PalRenderingInfo renderingInfo = {0};
        renderingInfo.viewCount = 1;
        renderingInfo.colorAttachentCount = 1;
        renderingInfo.colorAttachments = &colorAttachment;
        renderingInfo.layerCount = 1;
        renderingInfo.multisampleCount = PAL_SAMPLE_COUNT_1;
        renderingInfo.renderArea.width = WINDOW_WIDTH;
        renderingInfo.renderArea.height = WINDOW_HEIGHT;

        result = palCmdBeginRendering(cmdBuffer, &renderingInfo);
        if (result != PAL_RESULT_SUCCESS) {
            const char* error = palFormatResult(result);
            palLog(nullptr, "Failed to begin rendering: %s", error);
            return false;
        }

        // bind pipeline
        result = palCmdBindPipeline(cmdBuffer, pipeline);
        if (result != PAL_RESULT_SUCCESS) {
            const char* error = palFormatResult(result);
            palLog(nullptr, "Failed to bind pipeline: %s", error);
            return false;
        }

        // set viewport and scissors
        result = palCmdSetViewport(cmdBuffer, 1, &viewport);
        if (result != PAL_RESULT_SUCCESS) {
            const char* error = palFormatResult(result);
            palLog(nullptr, "Failed to set viewport: %s", error);
            return false;
        }

        result = palCmdSetScissors(cmdBuffer, 1, &scissor);
        if (result != PAL_RESULT_SUCCESS) {
            const char* error = palFormatResult(result);
            palLog(nullptr, "Failed to set scissors: %s", error);
            return false;
        }

        // draw a single triangle with the mesh shader
        // palBuildWorkGroupInfo() is a helper to build
        // the workgroup count per axis using normal
        // workCount (image size, buffer size)
        result = palCmdDrawMeshTasks(cmdBuffer, 1, 1, 1);
        if (result != PAL_RESULT_SUCCESS) {
            const char* error = palFormatResult(result);
            palLog(nullptr, "Failed to issue draw command: %s", error);
            return false;
        }

        result = palCmdEndRendering(cmdBuffer);
        if (result != PAL_RESULT_SUCCESS) {
            const char* error = palFormatResult(result);
            palLog(nullptr, "Failed to end rendering: %s", error);
            return false;
        }

        // change the state of the image view to make it presentable
        oldUsageStateInfo = newUsageStateInfo;
        newUsageStateInfo.usageState = PAL_USAGE_STATE_PRESENT;
        result = palCmdImageBarrier(
            cmdBuffer,
            image,
            &imageRange,
            &oldUsageStateInfo,
            &newUsageStateInfo);

        if (result != PAL_RESULT_SUCCESS) {
            const char* error = palFormatResult(result);
            palLog(nullptr, "Failed to set image view barrier: %s", error);
            return false;
        }

        result = palCmdEnd(cmdBuffer);
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

    palDestroyPipeline(pipeline);
    palDestroyPipelineLayout(pipelineLayout);

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
