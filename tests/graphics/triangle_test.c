
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

bool triangleTest()
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
    PalSemaphore* renderFinishedSemaphores[MAX_FRAMES_IN_FLIGHT];
    PalFence* inFlightFences[MAX_FRAMES_IN_FLIGHT];
    PalFence** inFlightImages; // count of swapchain images

    PalPipelineLayout* pipelineLayout = nullptr;
    PalPipeline* pipeline = nullptr;
    PalShader* vertexShader = nullptr;
    PalShader* fragmentShader = nullptr;

    PalBuffer* vertexBuffer = nullptr;
    PalBuffer* stagingBuffer = nullptr;
    PalMemory* vertexBufferMemory = nullptr;
    PalMemory* stagingBufferMemory = nullptr;

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
    windowCreateInfo.title = "Triangle Window";

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

    result = palInitGraphics(&debugger, nullptr);
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
    bool hasGraphicsQueue = false;
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
            hasGraphicsQueue = false;
            continue;

        } else {
            hasGraphicsQueue = true;
        }

        if (hasGraphicsQueue) {
            // We want an adapter that supports spirv 1.0 or dxil 6.0
            result = palGetAdapterInfo(adapter, &adapterInfo);
            if (result != PAL_RESULT_SUCCESS) {
                const char* error = palFormatResult(result);
                palLog(nullptr, "Failed to get adapter info: %s", error);
                return false;
            }

            // we prefer spirv first if an adapter supports multiple shader formats
            if (adapterInfo.shaderFormats & PAL_SHADER_FORMAT_SPIRV) {
                if (palIsShaderTargetSupported(adapter, PAL_SHADER_TARGET_SPIRV_1_0)) {
                    break;
                }
            }

            if (adapterInfo.shaderFormats & PAL_SHADER_FORMAT_DXIL) {
                if (palIsShaderTargetSupported(adapter, PAL_SHADER_TARGET_DXIL_6_0)) {
                    break;
                }
            }
        }
        adapter = nullptr;
    }

    palFree(nullptr, adapters);
    if (!adapter) {
        if (!hasGraphicsQueue) {
            palLog(nullptr, "Failed to find an adapter that supports graphics queue");

        } else {
            palLog(nullptr, "Failed to find an adapter that supports required shader target");
        }
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
    if (!imageViews || !inFlightImages) {
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
        result = palCreateSemaphore(device, &imageAvailableSemaphores[i]);
        if (result != PAL_RESULT_SUCCESS) {
            const char* error = palFormatResult(result);
            palLog(nullptr, "Failed to create semaphore: %s", error);
            return false;
        }

        result = palCreateSemaphore(device, &renderFinishedSemaphores[i]);
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

    // create vertex and staging buffer
    // clang-format off
    float vertices[] = {
        0.0f, 0.5f, 1.0f, 0.0f, 0.0f,
        0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
       -0.5f, -0.5f, 0.0f, 0.0f, 1.0f};
    // clang-format on

    PalBufferCreateInfo bufferCreateInfo = {0};
    bufferCreateInfo.size = sizeof(vertices);
    bufferCreateInfo.usages = PAL_BUFFER_USAGE_VERTEX;
    bufferCreateInfo.usages |= PAL_BUFFER_USAGE_TRANSFER_DST; // will recieve
    result = palCreateBuffer(device, &bufferCreateInfo, &vertexBuffer);
    if (result != PAL_RESULT_SUCCESS) {
        const char* error = palFormatResult(result);
        palLog(nullptr, "Failed to create vertex buffer: %s", error);
        return false;
    }

    bufferCreateInfo.usages = PAL_BUFFER_USAGE_TRANSFER_SRC; // will send
    result = palCreateBuffer(device, &bufferCreateInfo, &stagingBuffer);
    if (result != PAL_RESULT_SUCCESS) {
        const char* error = palFormatResult(result);
        palLog(nullptr, "Failed to create staging buffer: %s", error);
        return false;
    }

    // get buffer memory requirement and allocate memory
    PalMemoryRequirements vertexBufferMemReq = {0};
    PalMemoryRequirements stagingBufferMemReq = {0};

    result = palGetBufferMemoryRequirements(vertexBuffer, &vertexBufferMemReq);
    if (result != PAL_RESULT_SUCCESS) {
        const char* error = palFormatResult(result);
        palLog(nullptr, "Failed to get buffer memory requirement: %s", error);
        return false;
    }

    result = palGetBufferMemoryRequirements(stagingBuffer, &stagingBufferMemReq);
    if (result != PAL_RESULT_SUCCESS) {
        const char* error = palFormatResult(result);
        palLog(nullptr, "Failed to get buffer memory requirement: %s", error);
        return false;
    }

    // we need to check if the memory type we want are supported
    // but almost every GPU supports a GPU only memory
    // and CPU writable memory
    result = palAllocateMemory(
        device,
        PAL_MEMORY_TYPE_GPU_ONLY,
        vertexBufferMemReq.memoryMask,
        vertexBufferMemReq.size,
        &vertexBufferMemory);

    if (result != PAL_RESULT_SUCCESS) {
        const char* error = palFormatResult(result);
        palLog(nullptr, "Failed to allocate memory for buffer: %s", error);
        return false;
    }

    result = palAllocateMemory(
        device,
        PAL_MEMORY_TYPE_CPU_UPLOAD,
        stagingBufferMemReq.memoryMask,
        stagingBufferMemReq.size,
        &stagingBufferMemory);

    if (result != PAL_RESULT_SUCCESS) {
        const char* error = palFormatResult(result);
        palLog(nullptr, "Failed to allocate memory for buffer: %s", error);
        return false;
    }

    // bind memory
    result = palBindBufferMemory(vertexBuffer, vertexBufferMemory, 0);
    if (result != PAL_RESULT_SUCCESS) {
        const char* error = palFormatResult(result);
        palLog(nullptr, "Failed to bind memory: %s", error);
        return false;
    }

    result = palBindBufferMemory(stagingBuffer, stagingBufferMemory, 0);
    if (result != PAL_RESULT_SUCCESS) {
        const char* error = palFormatResult(result);
        palLog(nullptr, "Failed to bind memory: %s", error);
        return false;
    }

    // map the staging buffer and upload the vertices
    void* ptr = nullptr;
    result = palMapBufferMemory(stagingBuffer, 0, sizeof(vertices), &ptr);
    if (result != PAL_RESULT_SUCCESS) {
        const char* error = palFormatResult(result);
        palLog(nullptr, "Failed to map buffer memory: %s", error);
        return false;
    }

    memcpy(ptr, vertices, sizeof(vertices));
    palUnmapBufferMemory(stagingBuffer);

    PalFence* tmpFence = nullptr;
    result = palCreateFence(device, false, &tmpFence);
    if (result != PAL_RESULT_SUCCESS) {
        const char* error = palFormatResult(result);
        palLog(nullptr, "Failed to create fence: %s", error);
        return false;
    }

    // use the first command buffer to upload the copy
    // and reset it when done
    // set a fence and check at the last line before the main loop
    // to see if we have to wait for the copy to be executed
    result = palCmdBegin(cmdBuffers[0], nullptr);
    if (result != PAL_RESULT_SUCCESS) {
        const char* error = palFormatResult(result);
        palLog(nullptr, "Failed to begin command buffer: %s", error);
        return false;
    }

    PalBufferCopyInfo copyInfo = {0};
    copyInfo.size = sizeof(vertices);

    result = palCmdCopyBuffer(cmdBuffers[0], vertexBuffer, stagingBuffer, &copyInfo);
    if (result != PAL_RESULT_SUCCESS) {
        const char* error = palFormatResult(result);
        palLog(nullptr, "Failed to copy buffer: %s", error);
        return false;
    }

    PalShaderStage vertexShaderStage[] = { PAL_SHADER_STAGE_VERTEX };
    PalUsageStateInfo oldUsageStateInfo = {0};
    oldUsageStateInfo.usageState = PAL_USAGE_STATE_TRANSFER_WRITE;

    PalUsageStateInfo newUsageStateInfo = {0};
    newUsageStateInfo.shaderStageCount = 1;
    newUsageStateInfo.shaderStages = vertexShaderStage;
    newUsageStateInfo.usageState = PAL_USAGE_STATE_VERTEX_READ;

    result = palCmdBufferBarrier(cmdBuffers[0], vertexBuffer, &oldUsageStateInfo, &newUsageStateInfo);
    if (result != PAL_RESULT_SUCCESS) {
        const char* error = palFormatResult(result);
        palLog(nullptr, "Failed to set buffer barrier: %s", error);
        return false;
    }

    result = palCmdEnd(cmdBuffers[0]);
    if (result != PAL_RESULT_SUCCESS) {
        const char* error = palFormatResult(result);
        palLog(nullptr, "Failed to end command buffer: %s", error);
        return false;
    }

    PalCommandBufferSubmitInfo submitInfo = {0};
    submitInfo.cmdBuffer = cmdBuffers[0];
    submitInfo.fence = tmpFence;
    result = palSubmitCommandBuffer(queue, &submitInfo);
    if (result != PAL_RESULT_SUCCESS) {
        const char* error = palFormatResult(result);
        palLog(nullptr, "Failed to submit command buffer: %s", error);
        return false;
    }

    // create shaders
    Uint64 bytecodeSize = 0;
    void* bytecode = nullptr;
    PalShaderCreateInfo shaderCreateInfo = {0};

    const char* vertexShaderPath = nullptr;
    const char* fragShaderPath = nullptr;
    if (adapterInfo.shaderFormats & PAL_SHADER_FORMAT_SPIRV) {
        vertexShaderPath = "graphics/shaders/triangle_vert_shader.spv";
        fragShaderPath = "graphics/shaders/triangle_frag_shader.spv";

    } else if (adapterInfo.shaderFormats & PAL_SHADER_FORMAT_DXIL) {
        vertexShaderPath = "graphics/shaders/triangle_vert_shader.dxil";
        fragShaderPath = "graphics/shaders/triangle_frag_shader.dxil";
    }

    if (!readFile(vertexShaderPath, nullptr, &bytecodeSize)) {
        palLog(nullptr, "Failed to find shader file");
        return false;
    }

    bytecode = palAllocate(nullptr, bytecodeSize, 0);
    if (!bytecode) {
        palLog(nullptr, "Failed to allocate memory");
        return false;
    }

    readFile(vertexShaderPath, bytecode, &bytecodeSize);
    shaderCreateInfo.bytecode = bytecode;
    shaderCreateInfo.bytecodeSize = bytecodeSize;
    shaderCreateInfo.stage = PAL_SHADER_STAGE_VERTEX;

    result = palCreateShader(device, &shaderCreateInfo, &vertexShader);
    if (result != PAL_RESULT_SUCCESS) {
        const char* error = palFormatResult(result);
        palLog(nullptr, "Failed to create vertex shader: %s", error);
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

    PalRenderingLayoutInfo renderingLayoutInfo = {0};
    renderingLayoutInfo.colorAttachentCount = 1;
    renderingLayoutInfo.colorAttachmentsFormat = &imageInfo.format;
    renderingLayoutInfo.multisampleCount = PAL_SAMPLE_COUNT_1;
    renderingLayoutInfo.viewCount = 1;

    // create graphics pipeline
    PalGraphicsPipelineCreateInfo pipelineCreateInfo = {0};
    PalVertexLayout vertexLayout = {0};
    PalVertexAttribute vertexAttributes[2];

    // position
    vertexAttributes[0].semanticID = PAL_VERTEX_SEMANTIC_ID_POSITION;
    vertexAttributes[0].semanticName = nullptr; // use default
    vertexAttributes[0].type = PAL_VERTEX_TYPE_FLOAT2;

    // color
    vertexAttributes[1].semanticID = PAL_VERTEX_SEMANTIC_ID_COLOR;
    vertexAttributes[1].semanticName = nullptr; // use default
    vertexAttributes[1].type = PAL_VERTEX_TYPE_FLOAT3;

    vertexLayout.attributeCount = 2;
    vertexLayout.attributes = vertexAttributes;
    vertexLayout.binding = 0; // first vertex buffer binding slot
    vertexLayout.type = PAL_VERTEX_LAYOUT_TYPE_PER_VERTEX;

    pipelineCreateInfo.vertexLayoutCount = 1;
    pipelineCreateInfo.vertexLayouts = &vertexLayout;

    // color blend attachment
    PalColorBlendAttachment blendAttachment = {0};
    blendAttachment.colorWriteMask |= PAL_COLOR_MASK_RED;
    blendAttachment.colorWriteMask |= PAL_COLOR_MASK_GREEN;
    blendAttachment.colorWriteMask |= PAL_COLOR_MASK_BLUE;
    blendAttachment.colorWriteMask |= PAL_COLOR_MASK_ALPHA;

    pipelineCreateInfo.colorBlendAttachments = &blendAttachment;
    pipelineCreateInfo.colorBlendAttachmentCount = 1;

    // shaders
    PalShader* shaders[2];
    shaders[0] = vertexShader;
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

    palDestroyShader(vertexShader);
    palDestroyShader(fragmentShader);

    // wait for the vertices copy to be done
    result = palWaitFence(tmpFence, PAL_INFINITE);
    if (result != PAL_RESULT_SUCCESS) {
        const char* error = palFormatResult(result);
        palLog(nullptr, "Failed to wait for fence: %s", error);
        return false;
    }

    // the vertices have been copied
    palDestroyFence(tmpFence);
    palDestroyBuffer(stagingBuffer);
    palFreeMemory(device, stagingBufferMemory);

    // main loop
    Uint32 currentFrame = 0;
    bool running = true;

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

        // bind pipeline
        result = palCmdBindPipeline(cmdBuffers[currentFrame], pipeline);
        if (result != PAL_RESULT_SUCCESS) {
            const char* error = palFormatResult(result);
            palLog(nullptr, "Failed to bind pipeline: %s", error);
            return false;
        }

        // set viewport and scissors
        result = palCmdSetViewport(cmdBuffers[currentFrame], 1, &viewport);
        if (result != PAL_RESULT_SUCCESS) {
            const char* error = palFormatResult(result);
            palLog(nullptr, "Failed to set viewport: %s", error);
            return false;
        }

        result = palCmdSetScissors(cmdBuffers[currentFrame], 1, &scissor);
        if (result != PAL_RESULT_SUCCESS) {
            const char* error = palFormatResult(result);
            palLog(nullptr, "Failed to set scissors: %s", error);
            return false;
        }

        // bind vertex buffer
        Uint64 offset[] = {0};
        result = palCmdBindVertexBuffers(
            cmdBuffers[currentFrame], 
            0, 
            1,
            &vertexBuffer,
            offset);
            
        if (result != PAL_RESULT_SUCCESS) {
            const char* error = palFormatResult(result);
            palLog(nullptr, "Failed to bind vertex buffer: %s", error);
            return false;
        }

        result = palCmdDraw(cmdBuffers[currentFrame], 3, 1, 0, 0);
        if (result != PAL_RESULT_SUCCESS) {
            const char* error = palFormatResult(result);
            palLog(nullptr, "Failed to issue draw command: %s", error);
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
        submitInfo.signalSemaphore = renderFinishedSemaphores[currentFrame];

        result = palSubmitCommandBuffer(queue, &submitInfo);
        if (result != PAL_RESULT_SUCCESS) {
            const char* error = palFormatResult(result);
            palLog(nullptr, "Failed to submit command buffer: %s", error);
            return false;
        }

        // present
        PalSwapchainPresentInfo presentInfo = {0};
        presentInfo.imageIndex = imageIndex;
        presentInfo.waitSemaphore = renderFinishedSemaphores[currentFrame];
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

    palDestroyPipeline(pipeline);
    palDestroyPipelineLayout(pipelineLayout);

    for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        palDestroySemaphore(imageAvailableSemaphores[i]);
        palDestroySemaphore(renderFinishedSemaphores[i]);
        palDestroyFence(inFlightFences[i]);
        palFreeCommandBuffer(cmdBuffers[i]);
    }

    for (int i = 0; i < imageCount; i++) {
        palDestroyImageView(imageViews[i]);
    }

    palDestroyBuffer(vertexBuffer);
    palFreeMemory(device, vertexBufferMemory);

    palDestroyCommandPool(cmdPool);
    palDestroySwapchain(swapchain);
    palDestroySurface(surface);
    palDestroyQueue(queue);
    palDestroyDevice(device);
    palShutdownGraphics();
    palFree(nullptr, imageViews);

    palDestroyWindow(window);
    palShutdownVideo();
    palDestroyEventDriver(eventDriver);
    return true;
}
