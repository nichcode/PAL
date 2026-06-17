
#include "pal/pal_graphics.h"
#include "pal/pal_video.h"
#include "pal/pal_system.h"
#include "tests.h"

#define WINDOW_WIDTH 640
#define WINDOW_HEIGHT 480
#define MAX_FRAMES_IN_FLIGHT 2

static inline uint32_t align(
    uint32_t value,
    uint32_t alignment)
{
    return (value + alignment - 1) & ~(alignment - 1);
}

static void PAL_CALL onGraphicsDebug(
    void* userData,
    PalDebugMessageSeverity severity,
    PalDebugMessageType type,
    const char* msg)
{
    palLog(nullptr, msg);
}

PalBool indirectDrawTest()
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

    PalPipelineLayout* pipelineLayout = nullptr;
    PalPipeline* pipeline = nullptr;
    PalShader* shaders[2];

    PalBuffer* vertexBuffer = nullptr;
    PalBuffer* indexBuffer = nullptr;
    PalBuffer* indirectBuffer = nullptr;
    PalBuffer* stagingBuffer = nullptr;
    
    PalMemory* vertexBufferMemory = nullptr;
    PalMemory* indexBufferMemory = nullptr;
    PalMemory* indirectBufferMemory = nullptr;
    PalMemory* stagingBufferMemory = nullptr;

    PalEventDriverCreateInfo eventDriverCreateInfo = {0};
    result = palCreateEventDriver(&eventDriverCreateInfo, &eventDriver);
    if (result != PAL_RESULT_SUCCESS) {
        logResult(result, "Failed to create event driver");
        return PAL_FALSE;
    }

    palSetEventDispatchMode(eventDriver, PAL_EVENT_WINDOW_CLOSE, PAL_DISPATCH_POLL);
    palSetEventDispatchMode(eventDriver, PAL_EVENT_KEYDOWN, PAL_DISPATCH_POLL);

    result = palInitVideo(nullptr, eventDriver, nullptr);
    if (result != PAL_RESULT_SUCCESS) {
        logResult(result, "Failed to initialize video");
        return PAL_FALSE;
    }

    PalWindowCreateInfo windowCreateInfo = {0};
    windowCreateInfo.height = WINDOW_HEIGHT;
    windowCreateInfo.width = WINDOW_WIDTH;
    windowCreateInfo.show = PAL_TRUE;
    windowCreateInfo.title = "Indirect Draw Window";

    PalVideoFeatures videoFeatures = palGetVideoFeatures();
    if (!(videoFeatures & PAL_VIDEO_FEATURE_DECORATED_WINDOW)) {
        windowCreateInfo.style |= PAL_WINDOW_STYLE_BORDERLESS;
    }

    result = palCreateWindow(&windowCreateInfo, &window);
    if (result != PAL_RESULT_SUCCESS) {
        logResult(result, "Failed to create window");
        return PAL_FALSE;
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
        return PAL_FALSE;
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
        return PAL_FALSE;
    }

    // enumerate all available adapters
    int32_t adapterCount = 0;
    result = palEnumerateAdapters(&adapterCount, nullptr);
    if (result != PAL_RESULT_SUCCESS) {
        const char* error = palFormatResult(result);
        palLog(nullptr, "Failed to get query adapters: %s", error);
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
        const char* error = palFormatResult(result);
        palLog(nullptr, "Failed to get query adapters: %s", error);
        return PAL_FALSE;
    }

    PalAdapterCapabilities caps = {0};
    PalAdapterInfo adapterInfo = {0};
    PalAdapterFeatures adapterFeatures;
    for (int32_t i = 0; i < adapterCount; i++) {
        adapter = adapters[i];
        result = palGetAdapterCapabilities(adapter, &caps);
        if (result != PAL_RESULT_SUCCESS) {
            const char* error = palFormatResult(result);
            palLog(nullptr, "Failed to get adapter capabilities: %s", error);
            palFree(nullptr, adapters);
            return PAL_FALSE;
        }

        if (caps.maxGraphicsQueues == 0) {
            adapter = nullptr;
            continue;
        }

        adapterFeatures = palGetAdapterFeatures(adapter);
        if (!(adapterFeatures & PAL_ADAPTER_FEATURE_INDIRECT_DRAW)) {
            adapter = nullptr;
            continue;
        }

        // We want an adapter that supports spirv 1.0 or dxil 6.0
        result = palGetAdapterInfo(adapter, &adapterInfo);
        if (result != PAL_RESULT_SUCCESS) {
            const char* error = palFormatResult(result);
            palLog(nullptr, "Failed to get adapter info: %s", error);
            return PAL_FALSE;
        }   

        // we prefer spirv first if an adapter supports multiple shader formats
        uint32_t target = 0;
        if (adapterInfo.shaderFormats & PAL_SHADER_FORMAT_SPIRV) {
            target = palGetHighestSupportedShaderTarget(adapter, PAL_SHADER_FORMAT_SPIRV);
            if (target >= PAL_MAKE_SHADER_TARGET(1, 0)) {
                break;
            }
        }

        if (adapterInfo.shaderFormats & PAL_SHADER_FORMAT_DXIL) {
            target = palGetHighestSupportedShaderTarget(adapter, PAL_SHADER_FORMAT_DXIL);
            if (target >= PAL_MAKE_SHADER_TARGET(6, 0)) {
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
    features |= PAL_ADAPTER_FEATURE_INDIRECT_DRAW;
    if (adapterFeatures & PAL_ADAPTER_FEATURE_FENCE_RESET) {
        features |= PAL_ADAPTER_FEATURE_FENCE_RESET;
    }

    result = palCreateDevice(adapter, features, &device);
    if (result != PAL_RESULT_SUCCESS) {
        const char* error = palFormatResult(result);
        palLog(nullptr, "Failed to create device: %s", error);
        return PAL_FALSE;
    }

    // create surface
    result = palCreateSurface(device, &gfxWindow, &surface);
    if (result != PAL_RESULT_SUCCESS) {
        const char* error = palFormatResult(result);
        palLog(nullptr, "Failed to create surface: %s", error);
        return PAL_FALSE;
    }

    // create a graphics command queue and check if its supports presenting to the surface
    PalBool foundQueue = PAL_FALSE;
    for (int i = 0; i < caps.maxGraphicsQueues; i++) {
        result = palCreateQueue(device, PAL_QUEUE_TYPE_GRAPHICS, &queue);
        if (result != PAL_RESULT_SUCCESS) {
            const char* error = palFormatResult(result);
            palLog(nullptr, "Failed to create queue: %s", error);
            return PAL_FALSE;
        }

        if (!palCanQueuePresent(queue, surface)) {
            palDestroyQueue(queue);
            queue = nullptr;
        }  else {
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
    result = palGetSurfaceCapabilities(device, surface, &surfaceCaps);
    if (result != PAL_RESULT_SUCCESS) {
        const char* error = palFormatResult(result);
        palLog(nullptr, "Failed to get surface capabilities: %s", error);
        return PAL_FALSE;
    }

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
        const char* error = palFormatResult(result);
        palLog(nullptr, "Failed to create swapchain: %s", error);
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
    result = palGetImageInfo(palGetSwapchainImage(swapchain, 0), &imageInfo);
    if (result != PAL_RESULT_SUCCESS) {
        const char* error = palFormatResult(result);
        palLog(nullptr, "Failed to get image info: %s", error);
        return PAL_FALSE;
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
            return PAL_FALSE;
        }

        // create render finished semaphores
        result = palCreateSemaphore(device, PAL_FALSE, &renderFinishedSemaphores[i]);
        if (result != PAL_RESULT_SUCCESS) {
            const char* error = palFormatResult(result);
            palLog(nullptr, "Failed to create semaphore: %s", error);
            return PAL_FALSE;
        }

        inFlightImages[i] = nullptr;
    }

    result = palCreateCommandPool(device, queue, &cmdPool);
    if (result != PAL_RESULT_SUCCESS) {
        const char* error = palFormatResult(result);
        palLog(nullptr, "Failed to create command pool: %s", error);
        return PAL_FALSE;
    }

    // create synchronization objects and command buffers
    for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        result = palCreateSemaphore(device, PAL_FALSE, &imageAvailableSemaphores[i]);
        if (result != PAL_RESULT_SUCCESS) {
            const char* error = palFormatResult(result);
            palLog(nullptr, "Failed to create semaphore: %s", error);
            return PAL_FALSE;
        }

        result = palCreateFence(device, PAL_TRUE, &inFlightFences[i]);
        if (result != PAL_RESULT_SUCCESS) {
            const char* error = palFormatResult(result);
            palLog(nullptr, "Failed to create fence: %s", error);
            return PAL_FALSE;
        }

        result = palAllocateCommandBuffer(
            device,
            cmdPool,
            PAL_COMMAND_BUFFER_TYPE_PRIMARY,
            &cmdBuffers[i]);

        if (result != PAL_RESULT_SUCCESS) {
            const char* error = palFormatResult(result);
            palLog(nullptr, "Failed to allocate command buffer: %s", error);
            return PAL_FALSE;
        }
    }

    // create vertex, index, indirect and staging buffer
    // clang-format off
    float vertices[] = {
        -0.5, 0.5, 1.0f, 1.0f, 0.0f,
         0.5, 0.5, 1.0f, 1.0f, 0.0f,
         0.5,-0.5, 1.0f, 1.0f, 0.0f,
        -0.5,-0.5, 1.0f, 1.0f, 0.0f
    };
    // clang-format on

    uint32_t indices[6] = { 0, 1, 2, 2, 3, 0 };

    // vertex buffer
    PalBufferCreateInfo bufferCreateInfo = {0};
    bufferCreateInfo.size = sizeof(vertices);
    bufferCreateInfo.usages = PAL_BUFFER_USAGE_VERTEX;
    bufferCreateInfo.usages |= PAL_BUFFER_USAGE_TRANSFER_DST; // will recieve
    result = palCreateBuffer(device, &bufferCreateInfo, &vertexBuffer);
    if (result != PAL_RESULT_SUCCESS) {
        const char* error = palFormatResult(result);
        palLog(nullptr, "Failed to create buffer: %s", error);
        return PAL_FALSE;
    }

    // index buffer
    bufferCreateInfo.size = sizeof(indices);
    bufferCreateInfo.usages = PAL_BUFFER_USAGE_INDEX;
    bufferCreateInfo.usages |= PAL_BUFFER_USAGE_TRANSFER_DST; // will recieve
    result = palCreateBuffer(device, &bufferCreateInfo, &indexBuffer);
    if (result != PAL_RESULT_SUCCESS) {
        const char* error = palFormatResult(result);
        palLog(nullptr, "Failed to create buffer: %s", error);
        return PAL_FALSE;
    }

    // indirect buffer
    bufferCreateInfo.size = sizeof(PalDrawIndexedIndirectData);
    bufferCreateInfo.usages = PAL_BUFFER_USAGE_INDIRECT;
    bufferCreateInfo.usages |= PAL_BUFFER_USAGE_TRANSFER_DST; // will recieve
    result = palCreateBuffer(device, &bufferCreateInfo, &indirectBuffer);
    if (result != PAL_RESULT_SUCCESS) {
        const char* error = palFormatResult(result);
        palLog(nullptr, "Failed to create buffer: %s", error);
        return PAL_FALSE;
    }

    // staging buffer for vertex, index and indirect buffer
    uint32_t offset = 0;
    uint32_t indirectOffset = offset;
    offset += sizeof(PalDrawIndexedIndirectData);
    offset = align(offset, 16);

    uint32_t indexOffset = offset;
    offset += sizeof(indices);
    offset = align(offset, 16);

    uint32_t vertexOffset = offset;
    offset += sizeof(vertices);
    offset = align(offset, 16);

    uint32_t stagingBufferSize = offset;
    bufferCreateInfo.usages = PAL_BUFFER_USAGE_TRANSFER_SRC; // will send
    bufferCreateInfo.size = offset;
    result = palCreateBuffer(device, &bufferCreateInfo, &stagingBuffer);
    if (result != PAL_RESULT_SUCCESS) {
        const char* error = palFormatResult(result);
        palLog(nullptr, "Failed to create buffer: %s", error);
        return PAL_FALSE;
    }

    // get buffer memory requirement and allocate memory
    PalMemoryRequirements vertexBufferMemReq = {0};
    PalMemoryRequirements indexBufferMemReq = {0};
    PalMemoryRequirements indirectBufferMemReq = {0};
    PalMemoryRequirements stagingBufferMemReq = {0};

    // get vertex buffer memory requirement
    result = palGetBufferMemoryRequirements(vertexBuffer, &vertexBufferMemReq);
    if (result != PAL_RESULT_SUCCESS) {
        const char* error = palFormatResult(result);
        palLog(nullptr, "Failed to get buffer memory requirement: %s", error);
        return PAL_FALSE;
    }

    // get index buffer memory requirement
    result = palGetBufferMemoryRequirements(indexBuffer, &indexBufferMemReq);
    if (result != PAL_RESULT_SUCCESS) {
        const char* error = palFormatResult(result);
        palLog(nullptr, "Failed to get buffer memory requirement: %s", error);
        return PAL_FALSE;
    }

    // get indirect buffer memory requirement
    result = palGetBufferMemoryRequirements(indirectBuffer, &indirectBufferMemReq);
    if (result != PAL_RESULT_SUCCESS) {
        const char* error = palFormatResult(result);
        palLog(nullptr, "Failed to get buffer memory requirement: %s", error);
        return PAL_FALSE;
    }

    // get staging buffer memory requirement
    result = palGetBufferMemoryRequirements(stagingBuffer, &stagingBufferMemReq);
    if (result != PAL_RESULT_SUCCESS) {
        const char* error = palFormatResult(result);
        palLog(nullptr, "Failed to get buffer memory requirement: %s", error);
        return PAL_FALSE;
    }

    // allocate memory for vertex buffer
    result = palAllocateMemory(
        device,
        PAL_MEMORY_TYPE_GPU_ONLY,
        vertexBufferMemReq.memoryMask,
        vertexBufferMemReq.size,
        &vertexBufferMemory);

    if (result != PAL_RESULT_SUCCESS) {
        const char* error = palFormatResult(result);
        palLog(nullptr, "Failed to allocate memory for buffer: %s", error);
        return PAL_FALSE;
    }

    // allocate memory for index buffer
    result = palAllocateMemory(
        device,
        PAL_MEMORY_TYPE_GPU_ONLY,
        indexBufferMemReq.memoryMask,
        indexBufferMemReq.size,
        &indexBufferMemory);

    if (result != PAL_RESULT_SUCCESS) {
        const char* error = palFormatResult(result);
        palLog(nullptr, "Failed to allocate memory for buffer: %s", error);
        return PAL_FALSE;
    }

    // allocate memory for indirect buffer
    result = palAllocateMemory(
        device,
        PAL_MEMORY_TYPE_GPU_ONLY,
        indirectBufferMemReq.memoryMask,
        indirectBufferMemReq.size,
        &indirectBufferMemory);

    if (result != PAL_RESULT_SUCCESS) {
        const char* error = palFormatResult(result);
        palLog(nullptr, "Failed to allocate memory for buffer: %s", error);
        return PAL_FALSE;
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
        return PAL_FALSE;
    }

    // bind memory for vertex buffer
    result = palBindBufferMemory(vertexBuffer, vertexBufferMemory, 0);
    if (result != PAL_RESULT_SUCCESS) {
        const char* error = palFormatResult(result);
        palLog(nullptr, "Failed to bind memory: %s", error);
        return PAL_FALSE;
    }

    // bind memory for index buffer
    result = palBindBufferMemory(indexBuffer, indexBufferMemory, 0);
    if (result != PAL_RESULT_SUCCESS) {
        const char* error = palFormatResult(result);
        palLog(nullptr, "Failed to bind memory: %s", error);
        return PAL_FALSE;
    }

    // bind memory for indirect buffer
    result = palBindBufferMemory(indirectBuffer, indirectBufferMemory, 0);
    if (result != PAL_RESULT_SUCCESS) {
        const char* error = palFormatResult(result);
        palLog(nullptr, "Failed to bind memory: %s", error);
        return PAL_FALSE;
    }

    // bind memory for staging buffer
    result = palBindBufferMemory(stagingBuffer, stagingBufferMemory, 0);
    if (result != PAL_RESULT_SUCCESS) {
        const char* error = palFormatResult(result);
        palLog(nullptr, "Failed to bind memory: %s", error);
        return PAL_FALSE;
    }

    // map the staging buffer and upload the data
    void* ptr = nullptr;
    result = palMapBufferMemory(stagingBuffer, 0, stagingBufferSize, &ptr);
    if (result != PAL_RESULT_SUCCESS) {
        const char* error = palFormatResult(result);
        palLog(nullptr, "Failed to map buffer memory: %s", error);
        return PAL_FALSE;
    }

    // copy indirect data
    PalDrawIndexedIndirectData indirectData = {0};
    indirectData.firstIndex = 0;
    indirectData.firstInstance = 0;
    indirectData.indexCount = 6;
    indirectData.instanceCount = 1;
    indirectData.vertexOffset = 0;

    uint8_t* dst = (uint8_t*)ptr;
    memcpy(dst + indirectOffset, &indirectData, sizeof(PalDrawIndexedIndirectData));

    // copy vertices
    memcpy(dst + vertexOffset, vertices, sizeof(vertices));

    // copy indices
    memcpy(dst + indexOffset, indices, sizeof(indices));

    palUnmapBufferMemory(stagingBuffer);

    PalFence* fence = nullptr;
    result = palCreateFence(device, PAL_FALSE, &fence);
    if (result != PAL_RESULT_SUCCESS) {
        const char* error = palFormatResult(result);
        palLog(nullptr, "Failed to create fence: %s", error);
        return PAL_FALSE;
    }

    // use the first command buffer to upload the copy
    // and reset it when done
    // set a fence and check at the last line before the main loop
    // to see if we have to wait for the copy to be executed
    result = palCmdBegin(cmdBuffers[0], nullptr);
    if (result != PAL_RESULT_SUCCESS) {
        const char* error = palFormatResult(result);
        palLog(nullptr, "Failed to begin command buffer: %s", error);
        return PAL_FALSE;
    }

    PalUsageStateInfo oldUsageStateInfo = {0};
    oldUsageStateInfo.usageState = PAL_USAGE_STATE_TRANSFER_WRITE;

    PalUsageStateInfo newUsageStateInfo = {0};
    newUsageStateInfo.shaderStageCount = 0;
    newUsageStateInfo.shaderStages = nullptr;
    newUsageStateInfo.usageState = PAL_USAGE_STATE_INDIRECT_READ;

    // copy to indirect buffer
    PalBufferCopyInfo indirectCopyInfo = {0};
    indirectCopyInfo.dstOffset = 0;
    indirectCopyInfo.size = sizeof(PalDrawIndexedIndirectData);
    indirectCopyInfo.srcOffset = indirectOffset;

    result = palCmdCopyBuffer(cmdBuffers[0], indirectBuffer, stagingBuffer, &indirectCopyInfo);
    if (result != PAL_RESULT_SUCCESS) {
        const char* error = palFormatResult(result);
        palLog(nullptr, "Failed to copy buffer: %s", error);
        return PAL_FALSE;
    }

    result = palCmdBufferBarrier(
        cmdBuffers[0], 
        indirectBuffer, 
        &oldUsageStateInfo, 
        &newUsageStateInfo);

    if (result != PAL_RESULT_SUCCESS) {
        const char* error = palFormatResult(result);
        palLog(nullptr, "Failed to set buffer barrier: %s", error);
        return PAL_FALSE;
    }

    // copy to vertex buffer
    PalShaderStage vertexShaderStage[] = { PAL_SHADER_STAGE_VERTEX };

    newUsageStateInfo.shaderStageCount = 1;
    newUsageStateInfo.shaderStages = vertexShaderStage;
    newUsageStateInfo.usageState = PAL_USAGE_STATE_VERTEX_READ;

    PalBufferCopyInfo vertexCopyInfo = {0};
    vertexCopyInfo.dstOffset = 0;
    vertexCopyInfo.size = sizeof(vertices);
    vertexCopyInfo.srcOffset = vertexOffset;

    result = palCmdCopyBuffer(cmdBuffers[0], vertexBuffer, stagingBuffer, &vertexCopyInfo);
    if (result != PAL_RESULT_SUCCESS) {
        const char* error = palFormatResult(result);
        palLog(nullptr, "Failed to copy buffer: %s", error);
        return PAL_FALSE;
    }

    result = palCmdBufferBarrier(
        cmdBuffers[0], 
        vertexBuffer, 
        &oldUsageStateInfo, 
        &newUsageStateInfo);
        
    if (result != PAL_RESULT_SUCCESS) {
        const char* error = palFormatResult(result);
        palLog(nullptr, "Failed to set buffer barrier: %s", error);
        return PAL_FALSE;
    }

    // copy to index buffer
    newUsageStateInfo.shaderStageCount = 0;
    newUsageStateInfo.shaderStages = nullptr;
    newUsageStateInfo.usageState = PAL_USAGE_STATE_INDEX_READ;

    PalBufferCopyInfo indexCopyInfo = {0};
    indexCopyInfo.dstOffset = 0;
    indexCopyInfo.size = sizeof(indices);
    indexCopyInfo.srcOffset = indexOffset;

    result = palCmdCopyBuffer(cmdBuffers[0], indexBuffer, stagingBuffer, &indexCopyInfo);
    if (result != PAL_RESULT_SUCCESS) {
        const char* error = palFormatResult(result);
        palLog(nullptr, "Failed to copy buffer: %s", error);
        return PAL_FALSE;
    }

    result = palCmdBufferBarrier(
        cmdBuffers[0], 
        indexBuffer, 
        &oldUsageStateInfo, 
        &newUsageStateInfo);
        
    if (result != PAL_RESULT_SUCCESS) {
        const char* error = palFormatResult(result);
        palLog(nullptr, "Failed to set buffer barrier: %s", error);
        return PAL_FALSE;
    }

    result = palCmdEnd(cmdBuffers[0]);
    if (result != PAL_RESULT_SUCCESS) {
        const char* error = palFormatResult(result);
        palLog(nullptr, "Failed to end command buffer: %s", error);
        return PAL_FALSE;
    }

    PalCommandBufferSubmitInfo submitInfo = {0};
    submitInfo.cmdBuffer = cmdBuffers[0];
    submitInfo.fence = fence;
    result = palSubmitCommandBuffer(queue, &submitInfo);
    if (result != PAL_RESULT_SUCCESS) {
        const char* error = palFormatResult(result);
        palLog(nullptr, "Failed to submit command buffer: %s", error);
        return PAL_FALSE;
    }

    // create shaders
    uint64_t bytecodeSize = 0;
    void* bytecode = nullptr;
    const char* sources[2];
    PalShaderEntryInfo entries[2];

    PalViewport viewport = {0};
    viewport.width = (float)WINDOW_WIDTH;
    viewport.maxDepth = 1.0f;

    PalShaderCreateInfo shaderCreateInfo = {0};
    if (adapterInfo.shaderFormats & PAL_SHADER_FORMAT_SPIRV) {
        sources[0] = "graphics/shaders/bin/spirv/triangle_vert.spv";
        sources[1] = "graphics/shaders/bin/spirv/triangle_frag.spv";

        viewport.height = -(float)WINDOW_HEIGHT;
        viewport.y = (float)WINDOW_HEIGHT;

    } else if (adapterInfo.shaderFormats & PAL_SHADER_FORMAT_DXIL) {
        sources[0] = "graphics/shaders/bin/dxil/triangle_vert.dxil";
        sources[1] = "graphics/shaders/bin/dxil/triangle_frag.dxil";

        viewport.height = (float)WINDOW_HEIGHT;
    }

    entries[0].stage = PAL_SHADER_STAGE_VERTEX;
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

        shaderCreateInfo.bytecode = bytecode;
        shaderCreateInfo.bytecodeSize = bytecodeSize;
        shaderCreateInfo.entries = &entries[i];
        shaderCreateInfo.entryCount = 1;

        result = palCreateShader(device, &shaderCreateInfo, &shaders[i]);
        if (result != PAL_RESULT_SUCCESS) {
            const char* error = palFormatResult(result);
            palLog(nullptr, "Failed to create shader: %s", error);
            return PAL_FALSE;
        }

        palFree(nullptr, bytecode);
    }
    
    // create pipeline layout
    PalPipelineLayoutCreateInfo pipelineLayoutCreateInfo = {0};
    result = palCreatePipelineLayout(device, &pipelineLayoutCreateInfo, &pipelineLayout);
    if (result != PAL_RESULT_SUCCESS) {
        const char* error = palFormatResult(result);
        palLog(nullptr, "Failed to create pipeline layout: %s", error);
        return PAL_FALSE;
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

    // color coordinates
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
    pipelineCreateInfo.shaderCount = 2;
    pipelineCreateInfo.shaders = shaders;

    pipelineCreateInfo.topology = PAL_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    pipelineCreateInfo.pipelineLayout = pipelineLayout;
    pipelineCreateInfo.renderingLayout = &renderingLayoutInfo;

    result = palCreateGraphicsPipeline(device, &pipelineCreateInfo, &pipeline);
    if (result != PAL_RESULT_SUCCESS) {
        const char* error = palFormatResult(result);
        palLog(nullptr, "Failed to create graphics pipeline: %s", error);
        return PAL_FALSE;
    }

    for (int i = 0; i < 2; i++) {
        palDestroyShader(shaders[i]);
    }

    // wait for the vertices copy to be done
    result = palWaitFence(fence, PAL_INFINITE);
    if (result != PAL_RESULT_SUCCESS) {
        const char* error = palFormatResult(result);
        palLog(nullptr, "Failed to wait for fence: %s", error);
        return PAL_FALSE;
    }

    // the vertices have been copied
    palDestroyFence(fence);
    palDestroyBuffer(stagingBuffer);
    palFreeMemory(device, stagingBufferMemory);

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
                case PAL_EVENT_WINDOW_CLOSE: {
                    running = PAL_FALSE;
                    break;
                }

                case PAL_EVENT_KEYDOWN: {
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
            const char* error = palFormatResult(result);
            palLog(nullptr, "Failed to wait fence: %s", error);
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
            const char* error = palFormatResult(result);
            palLog(nullptr, "Failed to get next swapchain image: %s", error);
            return PAL_FALSE;
        }

        if (inFlightImages[imageIndex] != nullptr) {
            result = palWaitFence(inFlightImages[imageIndex], PAL_INFINITE);
            if (result != PAL_RESULT_SUCCESS) {
                const char* error = palFormatResult(result);
                palLog(nullptr, "Failed to wait fence: %s", error);
                return PAL_FALSE;
            }
        }

        inFlightImages[imageIndex] = inFlightFences[currentFrame];
        if (adapterFeatures & PAL_ADAPTER_FEATURE_FENCE_RESET) {
            result = palResetFence(inFlightFences[currentFrame]);
            if (result != PAL_RESULT_SUCCESS) {
                const char* error = palFormatResult(result);
                palLog(nullptr, "Failed to wait fence: %s", error);
                return PAL_FALSE;
            }

        } else {
            // recreate since we dont support fence resetting
            palDestroyFence(inFlightFences[currentFrame]);

            result = palCreateFence(device, PAL_FALSE, &inFlightFences[currentFrame]);
            if (result != PAL_RESULT_SUCCESS) {
                const char* error = palFormatResult(result);
                palLog(nullptr, "Failed to wait fence: %s", error);
                return PAL_FALSE;
            }
        }

        // reset the command buffer
        result = palResetCommandBuffer(cmdBuffers[currentFrame]);
        if (result != PAL_RESULT_SUCCESS) {
            const char* error = palFormatResult(result);
            palLog(nullptr, "Failed to reset command buffer: %s", error);
            return PAL_FALSE;
        }

        result = palCmdBegin(cmdBuffers[currentFrame], nullptr);
        if (result != PAL_RESULT_SUCCESS) {
            const char* error = palFormatResult(result);
            palLog(nullptr, "Failed to begin command buffer: %s", error);
            return PAL_FALSE;
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
            return PAL_FALSE;
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
            return PAL_FALSE;
        }

        // bind pipeline
        result = palCmdBindPipeline(cmdBuffers[currentFrame], pipeline);
        if (result != PAL_RESULT_SUCCESS) {
            const char* error = palFormatResult(result);
            palLog(nullptr, "Failed to bind pipeline: %s", error);
            return PAL_FALSE;
        }

        // set viewport and scissors
        result = palCmdSetViewport(cmdBuffers[currentFrame], 1, &viewport);
        if (result != PAL_RESULT_SUCCESS) {
            const char* error = palFormatResult(result);
            palLog(nullptr, "Failed to set viewport: %s", error);
            return PAL_FALSE;
        }

        result = palCmdSetScissors(cmdBuffers[currentFrame], 1, &scissor);
        if (result != PAL_RESULT_SUCCESS) {
            const char* error = palFormatResult(result);
            palLog(nullptr, "Failed to set scissors: %s", error);
            return PAL_FALSE;
        }

        // bind vertex buffer
        uint64_t offset[] = {0};
        result = palCmdBindVertexBuffers(
            cmdBuffers[currentFrame], 
            0, 
            1, 
            &vertexBuffer, 
            offset);
            
        if (result != PAL_RESULT_SUCCESS) {
            const char* error = palFormatResult(result);
            palLog(nullptr, "Failed to bind vertex buffer: %s", error);
            return PAL_FALSE;
        }

        // bind index buffer
        result = palCmdBindIndexBuffer(
            cmdBuffers[currentFrame], 
            indexBuffer,
            0,
            PAL_INDEX_TYPE_UINT32);
            
        if (result != PAL_RESULT_SUCCESS) {
            const char* error = palFormatResult(result);
            palLog(nullptr, "Failed to bind index buffer: %s", error);
            return PAL_FALSE;
        }

        result = palCmdDrawIndexedIndirect(cmdBuffers[currentFrame], indirectBuffer, 1);
        if (result != PAL_RESULT_SUCCESS) {
            const char* error = palFormatResult(result);
            palLog(nullptr, "Failed to issue draw indirect command: %s", error);
            return PAL_FALSE;
        }

        result = palCmdEndRendering(cmdBuffers[currentFrame]);
        if (result != PAL_RESULT_SUCCESS) {
            const char* error = palFormatResult(result);
            palLog(nullptr, "Failed to end rendering: %s", error);
            return PAL_FALSE;
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
            return PAL_FALSE;
        }

        result = palCmdEnd(cmdBuffers[currentFrame]);
        if (result != PAL_RESULT_SUCCESS) {
            const char* error = palFormatResult(result);
            palLog(nullptr, "Failed to end command buffer: %s", error);
            return PAL_FALSE;
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
            return PAL_FALSE;
        }

        // present
        PalSwapchainPresentInfo presentInfo = {0};
        presentInfo.imageIndex = imageIndex;
        presentInfo.waitSemaphore = renderFinishedSemaphores[imageIndex];
        result = palPresentSwapchain(swapchain, &presentInfo);
        if (result != PAL_RESULT_SUCCESS) {
            const char* error = palFormatResult(result);
            palLog(nullptr, "Failed to present swapchain: %s", error);
            return PAL_FALSE;
        }

        currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
    }

    result = palWaitQueue(queue);
    if (result != PAL_RESULT_SUCCESS) {
        const char* error = palFormatResult(result);
        palLog(nullptr, "Failed to wait for queue: %s", error);
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

    palDestroyBuffer(indirectBuffer);
    palDestroyBuffer(vertexBuffer);
    palDestroyBuffer(indexBuffer);

    palFreeMemory(device, indirectBufferMemory);
    palFreeMemory(device, vertexBufferMemory);
    palFreeMemory(device, indexBufferMemory);

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
    return PAL_TRUE;
}
