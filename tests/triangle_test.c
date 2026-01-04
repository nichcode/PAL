
#include "pal/pal_graphics.h"
#include "pal/pal_video.h"
#include "tests.h"

#include <stdio.h>

static bool readFile(const char* filename, void* buffer, Uint64* size)
{
    FILE* file = fopen(filename, "rb");
    if (!file) {
        return false;
    }

    fseek(file, 0, SEEK_END);
    Uint64 tmpSize = ftell(file);
    fseek(file, 0, SEEK_SET);

    if (buffer) {
        fread(buffer, 1, (size_t)size, file);
    }
    
    fclose(file);
    *size = tmpSize;
    return true;
}

static bool initVideo(
    PalWindow** outWindow, 
    PalEventDriver** outEventDriver)
{
    PalResult result;
    PalWindow* window = nullptr;
    PalEventDriver* eventDriver = nullptr;

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
    windowCreateInfo.title = "Triangle Window";

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

    *outWindow = window;
    *outEventDriver = eventDriver;
    return true;
}

static PalWindowHandleInfo getWindowInfo(
    PalWindow* window, 
    Uint32* width, 
    Uint32* height)
{
    *width = 640;
    *height = 480;
    return palGetWindowHandleInfo(window);
}

static bool shutdownVideo(
    PalWindow* window, 
    PalEventDriver* eventDriver)
{
    palDestroyWindow(window);
    palShutdownVideo();
    palDestroyEventDriver(eventDriver);
}

static void PAL_CALL onGraphicsDebug(
    void* userData,
    Uint32 severity,
    Uint32 type,
    const char* msg)
{
    palLog(nullptr, msg);
}

bool triangleTest()
{
    palLog(nullptr, "");
    palLog(nullptr, "===========================================");
    palLog(nullptr, "Triangle Test");
    palLog(nullptr, "===========================================");
    palLog(nullptr, "");

    // initialize video system
    PalWindow* window = nullptr;
    PalEventDriver* eventDriver = nullptr;
    Uint32 windowWidth = 0;
    Uint32 windowHeight = 0;
    PalWindowHandleInfo windowHandleInfo;

    if (!initVideo(&window, &eventDriver)) {
        palLog(nullptr, "Failed to initialize video");
        return false;
    }
    windowHandleInfo = getWindowInfo(window, &windowWidth, &windowHeight);

    PalAdapter* adapter = nullptr;
    PalDevice* device = nullptr;
    PalQueue* queue = nullptr;
    PalSwapchain* swapchain = nullptr;
    PalCommandPool* cmdPool = nullptr;
    PalImageView** imageViews = nullptr;
    PalCommandBuffer** cmdBuffers = nullptr;
    PalRenderPass* renderPass = nullptr;
    PalRenderPassView** renderPassViews = nullptr;

    PalMemory* vertexbufferMemory = nullptr;
    PalBuffer* vertexBuffer = nullptr;
    PalShader* vertexShader = nullptr;
    PalShader* fragmentShader = nullptr;
    PalPipelineLayout* pipelineLayout = nullptr;
    PalPipeline* pipeline = nullptr;
    PalFence** fences = nullptr;

    // initialize the graphics system 
    PalGraphicsDebugger gfxDebugger = {0};
    gfxDebugger.callback = onGraphicsDebug;

    PalResult result = palInitGraphics(nullptr, nullptr);
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
    PalAdapterFeatures adapterFeatures = palGetAdapterFeatures(adapter);

    // create a device
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

    // check if the queue can present on our window
    PalGraphicsWindow gfxWindow;
    gfxWindow.window = windowHandleInfo.nativeWindow;
    gfxWindow.display = windowHandleInfo.nativeDisplay;
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
    swapchainCreateInfo.height = windowHeight;
    swapchainCreateInfo.width = windowWidth;
    swapchainCreateInfo.imageArrayLayerCount = 1;

    // rare but possible on andriod
    if (windowWidth > swapchainCaps.maxImageWidth) {
        swapchainCreateInfo.width = swapchainCaps.maxImageWidth / 2;
    }

    if (windowHeight > swapchainCaps.maxImageHeight) {
        swapchainCreateInfo.height = swapchainCaps.maxImageHeight / 2;
    }

    // check if the minimal image count is not good for you 
    // and increase it but not pass the max count
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

    renderPassViews = palAllocate(
        nullptr, 
        sizeof(PalRenderPassView*) * imageCount,
        0);

    cmdBuffers = palAllocate(
        nullptr, 
        sizeof(PalCommandBuffer*) * imageCount,
        0);

    fences = palAllocate(
        nullptr, 
        sizeof(PalFence*) * imageCount + 1, // next image fence
        0);

    if (!imageViews || !renderPassViews || !cmdBuffers || !fences) {
        palLog(nullptr, "Failed to allocate memory");
        return false;
    }

    // create render pass
    PalAttachmentDesc presentAttachment = {0};
    presentAttachment.loadOp = PAL_LOAD_OP_CLEAR;
    presentAttachment.storeOp = PAL_STORE_OP_STORE;
    presentAttachment.type = PAL_ATTACHMENT_TYPE_PRESENT;
    presentAttachment.sampleCount = PAL_SAMPLE_COUNT_1;
    presentAttachment.format = palGetSwapchainFormat(swapchain);
    
    PalRenderPassCreateInfo renderPasscreateInfo = {0};
    renderPasscreateInfo.attachmentCount = 1;
    renderPasscreateInfo.attachments = &presentAttachment;

    result = palCreateRenderPass(
        device, 
        &renderPasscreateInfo, 
        &renderPass);

    if (result != PAL_RESULT_SUCCESS) {
        const char* error = palFormatResult(result);
        palLog(nullptr, "Failed to create render pass: %s", error);
        return false;
    }

    // create shaders
    Uint64 vertexShaderSize = 0;
    Uint64 fragmentShaderSize = 0;
    void* vertexShaderBytecode = nullptr;
    void* fragmentShaderBytecode = nullptr;
    readFile("shaders/triangle_vert.spv", nullptr, &vertexShaderSize);
    readFile("shaders/triangle_frag.spv", nullptr, &fragmentShaderSize);

    if (!vertexShaderSize && !fragmentShaderSize) {
        palLog(nullptr, "Failed to find shader files");
        return false;
    }

    vertexShaderBytecode = palAllocate(nullptr, vertexShaderSize, 0);
    fragmentShaderBytecode = palAllocate(nullptr, fragmentShaderSize, 0);
    if (!vertexShaderBytecode && !fragmentShaderBytecode) {
        palLog(nullptr, "Failed to allocate memory");
        return false;
    }

    readFile(
        "shaders/triangle_vert.spv", 
        vertexShaderBytecode,
        &vertexShaderSize);

    readFile(
        "shaders/triangle_frag.spv", 
        fragmentShaderBytecode, 
        &fragmentShaderSize);

    PalShaderCreateInfo shaderCreateInfo = {0};
    shaderCreateInfo.bytecode = vertexShaderBytecode;
    shaderCreateInfo.bytecodeSize = vertexShaderSize;
    shaderCreateInfo.stage = PAL_SHADER_STAGE_VERTEX;
    result = palCreateShader(device, &shaderCreateInfo, &vertexShader);
    if (result != PAL_RESULT_SUCCESS) {
        const char* error = palFormatResult(result);
        palLog(nullptr, "Failed to create vertex shader: %s", error);
        return false;
    }

    shaderCreateInfo.bytecode = fragmentShaderBytecode;
    shaderCreateInfo.bytecodeSize = fragmentShaderSize;
    shaderCreateInfo.stage = PAL_SHADER_STAGE_FRAGMENT;
    result = palCreateShader(device, &shaderCreateInfo, &fragmentShader);
    if (result != PAL_RESULT_SUCCESS) {
        const char* error = palFormatResult(result);
        palLog(nullptr, "Failed to create fragment shader: %s", error);
        return false;
    }

    palFree(nullptr, vertexShaderBytecode);
    palFree(nullptr, fragmentShaderBytecode);

    // create graphics pipeline
    // vertex attributes and vertex layout
    PalVertexAttribute attributes[2];
    PalVertexLayout layout;

    PalVertexAttribute* position = &attributes[0];
    position->location = 0;
    position->type = PAL_VERTEX_TYPE_FLOAT2;

    PalVertexAttribute* color = &attributes[1];
    color->location = 1;
    color->type = PAL_VERTEX_TYPE_FLOAT3; // no alpha

    layout.binding = 0;
    layout.type = PAL_VERTEX_LAYOUT_TYPE_PER_VERTEX;
    layout.attributeCount = 2;
    layout.attributes = attributes;

    PalGraphicsPipelineCreateInfo pipelineCreateInfo = {0};
    pipelineCreateInfo.vertexLayouts = &layout;
    pipelineCreateInfo.vertexLayoutCount = 1;

    // Input assembly
    pipelineCreateInfo.topology = PAL_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;

    // Color blend attachment
    // we need one for each attachment in our render pass
    PalBlendAttachment blendAttachment = {0};
    blendAttachment.colorWriteMask |= PAL_COLOR_MASK_RED;
    blendAttachment.colorWriteMask |= PAL_COLOR_MASK_GREEN;
    blendAttachment.colorWriteMask |= PAL_COLOR_MASK_BLUE;
    blendAttachment.colorWriteMask |= PAL_COLOR_MASK_ALPHA;

    pipelineCreateInfo.blendAttachmentCount = 1;
    pipelineCreateInfo.blendAttachments = &blendAttachment;

    // shaders
    PalShader* shaders[2]; // vertex and fragment
    shaders[0] = vertexShader;
    shaders[1] = fragmentShader;
    pipelineCreateInfo.shaders = shaders;
    pipelineCreateInfo.shaderCount = 2;

    // pipeline layput
    PalPipelineLayoutCreateInfo pipelineLayoutCreateInfo = {0};
    result = palCreatePipelineLayout(
        device, 
        &pipelineLayoutCreateInfo, 
        &pipelineLayout);

    if (result != PAL_RESULT_SUCCESS) {
        const char* error = palFormatResult(result);
        palLog(nullptr, "Failed to create pipeline layout: %s", error);
        return false;
    }

    pipelineCreateInfo.renderPass = renderPass;
    pipelineCreateInfo.pipelineLayout = pipelineLayout;
    result = palCreateGraphicsPipeline(device, &pipelineCreateInfo, &pipeline);
    if (result != PAL_RESULT_SUCCESS) {
        const char* error = palFormatResult(result);
        palLog(nullptr, "Failed to create pipeline: %s", error);
        return false;
    }

    // triangle vertices
    // float 2 for pos and float 3 for color
    float vertices[] = {
        0.0f, 0.5f, 1.0f, 0.0f, 0.0f,
        -0.5f,  -0.5f, 0.0f, 1.0f, 0.0f,
       0.5f,  -0.5f, 0.0f, 0.0f, 1.0f
    };

    PalBufferCreateInfo bufferCreateInfo = {0};
    bufferCreateInfo.size = sizeof(vertices);
    bufferCreateInfo.usages = PAL_BUFFER_USAGE_VERTEX;
    result = palCreateBuffer(device, &bufferCreateInfo, &vertexBuffer);
    if (result != PAL_RESULT_SUCCESS) {
        const char* error = palFormatResult(result);
        palLog(nullptr, "Failed to create buffer: %s", error);
        return false;
    }

    PalMemoryRequirements memReq = {0};
    result = palGetBufferMemoryRequirements(vertexBuffer, &memReq);
    if (result != PAL_RESULT_SUCCESS) {
        const char* error = palFormatResult(result);
        palLog(nullptr, "Failed to create buffer: %s", error);
        return false;
    }

    // check if the memory type is supported
    result = palAllocateMemory(
        device, 
        PAL_MEMORY_TYPE_CPU_UPLOAD, 
        memReq.size, 
        &vertexbufferMemory);

    if (result != PAL_RESULT_SUCCESS) {
        const char* error = palFormatResult(result);
        palLog(nullptr, "Failed to allocate gpu memory: %s", error);
        return false;
    }

    result = palBindBufferMemory(vertexBuffer, vertexbufferMemory, 0);
    if (result != PAL_RESULT_SUCCESS) {
        const char* error = palFormatResult(result);
        palLog(nullptr, "Failed to bind gpu memory: %s", error);
        return false;
    }

    // map the memory and fill with our vertices
    void* data = nullptr;
    result = palMapMemory(
        device, 
        vertexbufferMemory,
        0, 
        sizeof(vertices), 
        &data);

    if (result != PAL_RESULT_SUCCESS) {
        const char* error = palFormatResult(result);
        palLog(nullptr, "Failed to map gpu memory: %s", error);
        return false;
    }

    memcpy(data, vertices, sizeof(vertices));
    palUnmapMemory(device, vertexbufferMemory);

    // viewport and scissor
    PalViewport viewport = {0};
    viewport.width = (float)swapchainCreateInfo.width;
    viewport.height = (float)swapchainCreateInfo.height;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    PalScissor scissor = {0};
    scissor.width = swapchainCreateInfo.width;
    scissor.height = swapchainCreateInfo.height;

    result = palCreateFence(device, &fences[imageCount]);
    if (result != PAL_RESULT_SUCCESS) {
        const char* error = palFormatResult(result);
        palLog(nullptr, "Failed to create fence: %s", error);
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
        
        // create a command buffer
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

        // create fences
        PalFence* fence = nullptr;
        result = palCreateFence(device, &fence);
        if (result != PAL_RESULT_SUCCESS) {
            const char* error = palFormatResult(result);
            palLog(nullptr, "Failed to create fence: %s", error);
            return false;
        }

        // create a render pass view
        PalRenderPassView* renderPassView = nullptr;
        PalRenderPassViewCreateInfo renderPassViewCreateInfo = {0};
        renderPassViewCreateInfo.width = swapchainCreateInfo.width;
        renderPassViewCreateInfo.height = swapchainCreateInfo.height;
        renderPassViewCreateInfo.imageViewCount = 1;
        renderPassViewCreateInfo.imageViews = &imageView;

        result = palCreateRenderPassView(
            device,
            renderPass,
            &renderPassViewCreateInfo,
            &renderPassView);

        if (result != PAL_RESULT_SUCCESS) {
            const char* error = palFormatResult(result);
            palLog(nullptr, "Failed to create render pass view: %s", error);
            return false;
        }

        // begin command buffer recording
        // the optional render pass is used for secondary command buffer
        // which will be used with a render pass
        result = palBeginCommandBuffer(cmdBuffer, nullptr);
        if (result != PAL_RESULT_SUCCESS) {
            const char* error = palFormatResult(result);
            palLog(nullptr, "Failed to begin command buffer: %s", error);
            return false;
        }

        // record commands
        PalClearValue clearValue = {0};
        clearValue.color[0] = 0.2f;
        clearValue.color[1] = 0.2f;
        clearValue.color[2] = 0.2f;
        clearValue.color[3] = 1.0f;

        PalRenderPassBeginInfo renderPassBeginInfo = {0};
        renderPassBeginInfo.clearValueCount = 1;
        renderPassBeginInfo.clearValues = &clearValue;
        renderPassBeginInfo.renderPass = renderPass;
        renderPassBeginInfo.view = renderPassView;

        result = palBeginRenderPass(cmdBuffer, &renderPassBeginInfo);
        if (result != PAL_RESULT_SUCCESS) {
            const char* error = palFormatResult(result);
            palLog(nullptr, "Failed to begin render pass: %s", error);
            return false;
        }

        // bind pipeline and set scissors and viewports
        result = palBindPipeline(cmdBuffer, pipeline);
        if (result != PAL_RESULT_SUCCESS) {
            const char* error = palFormatResult(result);
            palLog(nullptr, "Failed to bind pipeline: %s", error);
            return false;
        }

        result = palSetViewport(cmdBuffer, 1, &viewport);
        if (result != PAL_RESULT_SUCCESS) {
            const char* error = palFormatResult(result);
            palLog(nullptr, "Failed to set viewport: %s", error);
            return false;
        }

        result = palSetScissors(cmdBuffer, 1, &scissor);
        if (result != PAL_RESULT_SUCCESS) {
            const char* error = palFormatResult(result);
            palLog(nullptr, "Failed to set scissor: %s", error);
            return false;
        }

        Uint64 offset[] = { 0 };
        result = palBindVertexBuffers(cmdBuffer, 0, 1, &vertexBuffer, offset);
        if (result != PAL_RESULT_SUCCESS) {
            const char* error = palFormatResult(result);
            palLog(nullptr, "Failed to bind vertex buffer: %s", error);
            return false;
        }

        PalDrawData drawData = {0};
        drawData.vertexCount = 3;
        drawData.instancecCount = 1;

        result = palDraw(cmdBuffer, &drawData);
        if (result != PAL_RESULT_SUCCESS) {
            const char* error = palFormatResult(result);
            palLog(nullptr, "Failed to draw vertices: %s", error);
            return false;
        }

        result = palEndRenderPass(cmdBuffer);
        if (result != PAL_RESULT_SUCCESS) {
            const char* error = palFormatResult(result);
            palLog(nullptr, "Failed to end render pass: %s", error);
            return false;
        }

        // end command buffer recording
        result = palEndCommandBuffer(cmdBuffer);
        if (result != PAL_RESULT_SUCCESS) {
            const char* error = palFormatResult(result);
            palLog(nullptr, "Failed to end command buffer: %s", error);
            return false;
        }
        
        // cache everything so we can reference and destroy later
        renderPassViews[i] = renderPassView;
        cmdBuffers[i] = cmdBuffer;
        imageViews[i] = imageView;
        fences[i] = fence;
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

        if (adapterFeatures & PAL_ADAPTER_FEATURE_FENCE_RESET) {
            for (int i = 0; i < imageCount + 1; i++) {
                result = palResetFence(fences[i]);
                if (result != PAL_RESULT_SUCCESS) {
                    const char* error = palFormatResult(result);
                    palLog(nullptr, "Failed to reset fence: %s", error);
                    return false;
                }
            }

        } else {
            for (int i = 0; i < imageCount + 1; i++) {
                palDestroyFence(fences[i]);

                result = palCreateFence(device, &fences[i]);
                if (result != PAL_RESULT_SUCCESS) {
                    const char* error = palFormatResult(result);
                    palLog(nullptr, "Failed to create fence: %s", error);
                    return false;
                }
            }
        }

        // get the next image that we can render too
        PalImage* image = nullptr;
        PalNextImageInfo nextImageInfo = {0};
        nextImageInfo.timeout = UINT64_MAX;
        nextImageInfo.fence = fences[imageCount];

        image = palGetNextSwapchainImage(swapchain, &nextImageInfo);

        // wait till the image is acquired
        result = palWaitFence(fences[imageCount], UINT64_MAX);
        if (result != PAL_RESULT_SUCCESS) {
            const char* error = palFormatResult(result);
            palLog(nullptr, "Failed to wait for fence: %s", error);
            return false;
        }

        // find the command buffer associated with the image
        // this is fast, the swapchain caches all it images internally
        PalCommandBuffer* cmdBuffer = nullptr;
        PalFence* fence = nullptr;
        for (int i = 0; i < imageCount; i++) {
            if (image == palGetSwapchainImage(swapchain, i)) {
                cmdBuffer = cmdBuffers[i];
                fence = fences[i];
                break;
            }
        }

        // submit to the queue and present
        PalSubmitInfo submitInfo = {0};
        submitInfo.cmdBuffer = cmdBuffer;
        submitInfo.fence = fence;

        result = palSubmitCommandBuffer(queue, &submitInfo);
        if (result != PAL_RESULT_SUCCESS) {
            const char* error = palFormatResult(result);
            palLog(nullptr, "Failed to submit command buffer: %s", error);
            return false;
        }

        // wait for the commands to be executed
        result = palWaitFence(fence, UINT64_MAX);
        if (result != PAL_RESULT_SUCCESS) {
            const char* error = palFormatResult(result);
            palLog(nullptr, "Failed to wait for fence: %s", error);
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

    // cleanup
    for (int i = 0; i < imageCount; i++) {
        palDestroyRenderPassView(renderPassViews[i]);
        palDestroyCommandBuffer(cmdBuffers[i]);
        palDestroyImageView(imageViews[i]);
        palDestroyFence(fences[i]);
    }

    palDestroyFence(fences[imageCount]);

    palDestroyPipeline(pipeline);
    palDestroyPipelineLayout(pipelineLayout);
    palDestroyBuffer(vertexBuffer);
    palFreeMemory(device, vertexbufferMemory);

    palDestroyRenderPass(renderPass);
    palDestroyCommandPool(cmdPool);
    palDestroyShader(vertexShader);
    palDestroyShader(fragmentShader);
    
    palDestroySwapchain(swapchain); 
    palDestroyQueue(queue);
    palDestroyDevice(device);
    palShutdownGraphics();

    palFree(nullptr, imageViews);
    palFree(nullptr, cmdBuffers);
    palFree(nullptr, renderPassViews);

    shutdownVideo(window, eventDriver);
    return true;
}