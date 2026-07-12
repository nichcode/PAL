
#include "pal/pal_graphics.h"
#include "pal/pal_video.h"
#include "pal/pal_system.h"
#include "tests.h"

#define WINDOW_WIDTH 640
#define WINDOW_HEIGHT 480
#define MAX_FRAMES_IN_FLIGHT 2
#define TEXTURE_WIDTH 128
#define TEXTURE_HEIGHT 128

#define ARRAY_ELEMENT_0 17
#define ARRAY_ELEMENT_1 47
#define ARRAY_ELEMENT_2 55
#define ARRAY_ELEMENT_3 78

// layout must match shader
typedef struct {
    uint32_t textureIndices[4];
} PushConstant;

static void createFlatTexture(
    uint32_t* texture,
    uint32_t width,
    uint32_t height,
    uint8_t r,
    uint8_t g,
    uint8_t b)
{
    uint8_t* pixels = (uint8_t*)texture;
    for (int32_t y = 0; y < height; ++y) {
        for (int32_t x = 0; x < width; ++x) {
            int32_t i = (y * width + x) * 4;
            pixels[i + 0] = r;
            pixels[i + 1] = g;
            pixels[i + 2] = b;
            pixels[i + 3] = 255;
        }
    }
}

static void PAL_CALL onGraphicsDebug(
    void* userData,
    PalDebugMessageSeverity severity,
    PalDebugMessageType type,
    const char* msg)
{
    palLog(nullptr, msg);
}

PalBool descriptorIndexingTest()
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
    PalFence** inFlightImages; // count of swapchain images

    PalPipelineLayout* pipelineLayout = nullptr;
    PalPipeline* pipeline = nullptr;
    PalShader* shaders[2];

    PalBuffer* vertexBuffer = nullptr;
    PalBuffer* stagingBuffer = nullptr;
    PalSampler* sampler = nullptr;
    PalImage* textures[4];
    PalImageView* textureViews[4];
    PalBuffer* imageStagingBuffers[4];

    PalDescriptorSetLayout* descriptorSetLayout = nullptr;
    PalDescriptorPool* descriptorPool = nullptr;
    PalDescriptorSet* descriptorSet = nullptr;

    PalEventDriverCreateInfo eventDriverCreateInfo = {0};
    result = palCreateEventDriver(&eventDriverCreateInfo, &eventDriver);
    if (result != PAL_RESULT_SUCCESS) {
        logResult(result, "Failed to create event driver");
        return PAL_FALSE;
    }

    palSetEventDispatchMode(eventDriver, PAL_EVENT_TYPE_WINDOW_CLOSE, PAL_DISPATCH_MODE_POLL);
    palSetEventDispatchMode(eventDriver, PAL_EVENT_TYPE_KEYDOWN, PAL_DISPATCH_MODE_POLL);

    result = palInitVideo(nullptr, eventDriver, nullptr);
    if (result != PAL_RESULT_SUCCESS) {
        logResult(result, "Failed to initialize video");
        return PAL_FALSE;
    }

    PalWindowCreateInfo windowCreateInfo = {0};
    windowCreateInfo.height = WINDOW_HEIGHT;
    windowCreateInfo.width = WINDOW_WIDTH;
    windowCreateInfo.show = PAL_TRUE;
    windowCreateInfo.title = "Descriptor Indexing Window";

    PalVideoFeatures videoFeatures = palGetVideoFeatures();
    if (!(videoFeatures & PAL_VIDEO_FEATURE_DECORATED_WINDOW)) {
        windowCreateInfo.style |= PAL_WINDOW_STYLE_BORDERLESS;
    }

    result = palCreateWindow(&windowCreateInfo, &window);
    if (result != PAL_RESULT_SUCCESS) {
        logResult(result, "Failed to create window");
        return PAL_FALSE;
    }

    // get window handle. You can use any window from any library
    // so long as you can get the window handle and display (if on X11, wayland)
    // If pal video system will not be used, there is no need to initialize it
    PalWindowHandleInfo winHandle = {0};
    palGetWindowHandleInfo(window, &winHandle);

    // using pal_system.h will be easy to know the underlying windowing API or use typedefs. 
    // We will use the pal_system module.
    PalPlatformInfo platformInfo = {0};
    palGetPlatformInfo(&platformInfo);

    PalWindowInstanceType windowInstanceType = PAL_WINDOW_INSTANCE_TYPE_XCB;
    if (platformInfo.apiType == PAL_PLATFORM_API_TYPE_WAYLAND) {
        windowInstanceType = PAL_WINDOW_INSTANCE_TYPE_WAYLAND;

    } else if (platformInfo.apiType == PAL_PLATFORM_API_TYPE_X11) {
        windowInstanceType = PAL_WINDOW_INSTANCE_TYPE_X11;

    } else if (platformInfo.apiType == PAL_PLATFORM_API_TYPE_WIN32) {
        windowInstanceType = PAL_WINDOW_INSTANCE_TYPE_WIN32;
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
    int32_t adapterCount = 0;
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
    PalAdapterInfo adapterInfo = {0};
    PalAdapterFeatures adapterFeatures;
    for (int32_t i = 0; i < adapterCount; i++) {
        adapter = adapters[i];
        palGetAdapterCapabilities(adapter, &caps);
        if (caps.maxGraphicsQueues == 0) {
            adapter = nullptr;
            continue;
        }

        adapterFeatures = palGetAdapterFeatures(adapter);
        if (!(adapterFeatures & PAL_ADAPTER_FEATURE_DESCRIPTOR_INDEXING)) {
            adapter = nullptr;
            continue;
        }

        // We want an adapter that supports spirv 1.4 or dxil 6.0
        palGetAdapterInfo(adapter, &adapterInfo);

        // we prefer spirv first if an adapter supports multiple shader formats
        uint32_t target = 0;
        if (adapterInfo.shaderFormats & PAL_SHADER_FORMAT_SPIRV) {
            target = palGetHighestSupportedShaderTarget(adapter, PAL_SHADER_FORMAT_SPIRV);
            if (target >= PAL_MAKE_SHADER_TARGET(1, 4)) {
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
    features |= PAL_ADAPTER_FEATURE_DESCRIPTOR_INDEXING;
    if (adapterFeatures & PAL_ADAPTER_FEATURE_FENCE_RESET) {
        features |= PAL_ADAPTER_FEATURE_FENCE_RESET;
    }

    if (adapterFeatures & PAL_ADAPTER_FEATURE_NULL_DESCRIPTORS) {
        features |= PAL_ADAPTER_FEATURE_NULL_DESCRIPTORS;
    }

    result = palCreateDevice(adapter, features, &device);
    if (result != PAL_RESULT_SUCCESS) {
        logResult(result, "Failed to create device");
        return PAL_FALSE;
    }

    // get descriptor indexing capabilities
    PalDescriptorIndexingCapabilities descriptorIndexingCaps = {0};
    palQueryDescriptorIndexingCapabilities(device, &descriptorIndexingCaps);

    // create surface
    result = palCreateSurface(
        device, 
        winHandle.nativeWindow, 
        winHandle.nativeInstance, 
        windowInstanceType, 
        &surface);
        
    if (result != PAL_RESULT_SUCCESS) {
        logResult(result, "Failed to create surface");
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

    // create vertex and staging buffer
    // clang-format off
    float vertices[] = {
       -0.5f,  0.5f, 0.0f, 0.0f,
        0.5f,  0.5f, 1.0f, 0.0f,
        0.5f, -0.5f, 1.0f, 1.0f,

       -0.5f,  0.5f, 0.0f, 0.0f,
        0.5f, -0.5f, 1.0f, 1.0f,
       -0.5f, -0.5f, 0.0f, 1.0f};
    // clang-format on

    PalBufferCreateInfo bufferCreateInfo = {0};
    bufferCreateInfo.size = sizeof(vertices);
    bufferCreateInfo.usages = PAL_BUFFER_USAGE_VERTEX;
    bufferCreateInfo.usages |= PAL_BUFFER_USAGE_TRANSFER_DST; // will recieve
    bufferCreateInfo.memoryUsage = PAL_BUFFER_MEMORY_USAGE_AUTO_GPU_ONLY;

    result = palCreateBuffer(device, &bufferCreateInfo, &vertexBuffer);
    if (result != PAL_RESULT_SUCCESS) {
        logResult(result, "Failed to create buffer");
        return PAL_FALSE;
    }

    bufferCreateInfo.usages = PAL_BUFFER_USAGE_TRANSFER_SRC; // will send
    bufferCreateInfo.memoryUsage = PAL_BUFFER_MEMORY_USAGE_AUTO_CPU_UPLOAD;
    result = palCreateBuffer(device, &bufferCreateInfo, &stagingBuffer);
    if (result != PAL_RESULT_SUCCESS) {
        logResult(result, "Failed to create buffer");
        return PAL_FALSE;
    }

    // map the staging buffer and upload the vertices
    void* ptr = nullptr;
    result = palMapBuffer(stagingBuffer, 0, sizeof(vertices), &ptr);
    if (result != PAL_RESULT_SUCCESS) {
        logResult(result, "Failed to map buffer");
        return PAL_FALSE;
    }

    memcpy(ptr, vertices, sizeof(vertices));
    palUnmapBuffer(stagingBuffer);

    PalFence* fence = nullptr;
    result = palCreateFence(device, PAL_FALSE, &fence);
    if (result != PAL_RESULT_SUCCESS) {
        logResult(result, "Failed to create fence");
        return PAL_FALSE;
    }

    // create image for the textures
    PalImageCreateInfo imageCreateInfo = {0};
    imageCreateInfo.arrayLayerCount = 1;
    imageCreateInfo.depth = 1;
    imageCreateInfo.format = PAL_FORMAT_R8G8B8A8_UNORM;
    imageCreateInfo.mipLevelCount = 1; // simple
    imageCreateInfo.sampleCount = PAL_SAMPLE_COUNT_1; // simple
    imageCreateInfo.type = PAL_IMAGE_TYPE_2D;
    imageCreateInfo.usages = PAL_IMAGE_USAGE_TRANSFER_DST | PAL_IMAGE_USAGE_SAMPLED;
    imageCreateInfo.width = TEXTURE_WIDTH;
    imageCreateInfo.height = TEXTURE_HEIGHT;
    imageCreateInfo.memoryUsage = PAL_IMAGE_MEMORY_USAGE_AUTO_GPU_ONLY;

    for (int i = 0; i < 4; i++) {
        result = palCreateImage(device, &imageCreateInfo, &textures[i]);
        if (result != PAL_RESULT_SUCCESS) {
            logResult(result, "Failed to create image");
            return PAL_FALSE;
        }
    }

    // create staging buffers to transfer the data to the images
    PalBufferImageCopyInfo bufferImageCopyInfo = {0};
    bufferImageCopyInfo.ImageArrayLayerCount = 1;
    bufferImageCopyInfo.imageWidth = TEXTURE_WIDTH;
    bufferImageCopyInfo.imageHeight = TEXTURE_HEIGHT;
    bufferImageCopyInfo.imageDepth = 1; // 2D image

    PalImageStagingRequirements stagingReq = {0};
    palComputeImageStagingRequirements(
        device, 
        imageCreateInfo.format,
        &bufferImageCopyInfo,
        &stagingReq);

    // update our copy with the required buffer row length and buffer image height
    bufferImageCopyInfo.bufferRowLength = stagingReq.bufferRowLength;
    bufferImageCopyInfo.bufferImageHeight = stagingReq.bufferImageHeight;

    // create staging buffers to transfer the data to the image
    uint32_t textureDatas[4][TEXTURE_WIDTH * TEXTURE_HEIGHT];
    createFlatTexture(textureDatas[0], TEXTURE_WIDTH, TEXTURE_HEIGHT, 255, 0, 0);
    createFlatTexture(textureDatas[1], TEXTURE_WIDTH, TEXTURE_HEIGHT, 0, 255, 0);
    createFlatTexture(textureDatas[2], TEXTURE_WIDTH, TEXTURE_HEIGHT, 0, 0, 255);
    createFlatTexture(textureDatas[3], TEXTURE_WIDTH, TEXTURE_HEIGHT, 255, 255, 0);    

    PalBufferCreateInfo imageStagingBufferCreateInfo = {0};
    imageStagingBufferCreateInfo.size = stagingReq.bufferSize;
    imageStagingBufferCreateInfo.usages = PAL_BUFFER_USAGE_TRANSFER_SRC;
    imageStagingBufferCreateInfo.memoryUsage = PAL_BUFFER_MEMORY_USAGE_AUTO_CPU_UPLOAD;

    for (int i = 0; i < 4; i++) {
        result = palCreateBuffer(device, &imageStagingBufferCreateInfo, &imageStagingBuffers[i]);
        if (result != PAL_RESULT_SUCCESS) {
            logResult(result, "Failed to create buffer");
            return PAL_FALSE;
        }

        // copy data
        void* data = nullptr;
        result = palMapBuffer(imageStagingBuffers[i], 0, stagingReq.bufferSize, &data);
        if (result != PAL_RESULT_SUCCESS) {
            logResult(result, "Failed to map buffer");
            return PAL_FALSE;
        }

        palWriteImageStaging(
            device,
            imageCreateInfo.format,
            &bufferImageCopyInfo,
            textureDatas[i],
            data);
            
        palUnmapBuffer(imageStagingBuffers[i]);
    }

    // use the first command buffer to upload the copy
    // and reset it when done
    // set a fence and check at the last line before the main loop
    // to see if we have to wait for the copy to be executed
    result = palCmdBegin(cmdBuffers[0], nullptr);
    if (result != PAL_RESULT_SUCCESS) {
        logResult(result, "Failed to begin command buffer");
        return PAL_FALSE;
    }

    PalBufferCopyInfo copyInfo = {0};
    copyInfo.size = sizeof(vertices);
    palCmdCopyBuffer(cmdBuffers[0], vertexBuffer, stagingBuffer, &copyInfo);

    PalBarrierInfo barrierInfo = {0};
    barrierInfo.oldState = PAL_USAGE_STATE_TRANSFER_WRITE;
    barrierInfo.srcStages = PAL_PIPELINE_STAGE_TRANSFER;
    barrierInfo.newState = PAL_USAGE_STATE_TRANSFER_READ;
    barrierInfo.dstStages = PAL_PIPELINE_STAGE_TRANSFER;
    palCmdBufferBarrier(cmdBuffers[0], vertexBuffer, &barrierInfo);

    // set a barrier on the image to transition it into transfer dst state
    PalImageSubresourceRange textureRange = {0};
    textureRange.startMipLevel = 0;
    textureRange.startArrayLayer = 0;
    textureRange.mipLevelCount = 1;
    textureRange.layerArrayCount = 1;

    for (int i = 0; i < 4; i++) {
        barrierInfo.oldState = PAL_USAGE_STATE_UNDEFINED;
        barrierInfo.srcStages = PAL_PIPELINE_STAGE_NONE;
        barrierInfo.newState = PAL_USAGE_STATE_TRANSFER_WRITE;
        barrierInfo.dstStages = PAL_PIPELINE_STAGE_TRANSFER;
        palCmdImageBarrier(cmdBuffers[0], textures[i], &textureRange, &barrierInfo);

        palCmdCopyBufferToImage(
            cmdBuffers[0], 
            textures[i], 
            imageStagingBuffers[i], 
            &bufferImageCopyInfo);

        // transition the image to shader read state
        barrierInfo.oldState = PAL_USAGE_STATE_TRANSFER_WRITE;
        barrierInfo.srcStages = PAL_PIPELINE_STAGE_TRANSFER;
        barrierInfo.newState = PAL_USAGE_STATE_SHADER_READ;
        barrierInfo.dstStages = PAL_PIPELINE_STAGE_FRAGMENT_SHADER;
        palCmdImageBarrier(cmdBuffers[0], textures[i], &textureRange, &barrierInfo);
    }

    result = palCmdEnd(cmdBuffers[0]);
    if (result != PAL_RESULT_SUCCESS) {
        logResult(result, "Failed to end command buffer");
        return PAL_FALSE;
    }

    PalCommandBufferSubmitInfo submitInfo = {0};
    submitInfo.cmdBuffer = cmdBuffers[0];
    submitInfo.fence = fence;
    submitInfo.waitStages = PAL_PIPELINE_STAGE_TRANSFER;

    result = palSubmitCommandBuffer(queue, &submitInfo);
    if (result != PAL_RESULT_SUCCESS) {
        logResult(result, "Failed to submit command buffer");
        return PAL_FALSE;
    }

    // now we have the checkerboard texture data in the image
    // we need an image view and a sampler
    PalImageViewCreateInfo checkerboardImageViewCreateInfo = {0};
    checkerboardImageViewCreateInfo.type = PAL_IMAGE_VIEW_TYPE_2D;
    checkerboardImageViewCreateInfo.subresourceRange = textureRange;
    checkerboardImageViewCreateInfo.format = PAL_FORMAT_R8G8B8A8_UNORM;

    for (int i = 0; i < 4; i++) {
        result = palCreateImageView(
            device, 
            textures[i], 
            &checkerboardImageViewCreateInfo, 
            &textureViews[i]);

        if (result != PAL_RESULT_SUCCESS) {
            logResult(result, "Failed to create image view");
            return PAL_FALSE;
        }
    }

    // create sampler
    PalSamplerCreateInfo samplerCreateInfo = {0};
    samplerCreateInfo.addressModeU = PAL_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerCreateInfo.addressModeV = PAL_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerCreateInfo.addressModeW = PAL_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerCreateInfo.borderColor = PAL_BORDER_COLOR_INT_OPAQUE_BLACK;
    samplerCreateInfo.compareOp = PAL_COMPARE_OP_NEVER; // will not be used if its not enabled

    samplerCreateInfo.enableAnisotropy = PAL_FALSE;
    samplerCreateInfo.enableCompare = PAL_FALSE;
    samplerCreateInfo.magFilterMode = PAL_FILTER_MODE_LINEAR;
    samplerCreateInfo.minFilterMode = PAL_FILTER_MODE_LINEAR;
    samplerCreateInfo.maxAnisotropy = 1.0f;

    result = palCreateSampler(
        device, 
        &samplerCreateInfo,
        &sampler);

    if (result != PAL_RESULT_SUCCESS) {
        logResult(result, "Failed to create sampler");
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
        sources[0] = "graphics/shaders/bin/spirv/texture_vert.spv";
        sources[1] = "graphics/shaders/bin/spirv/descriptor_indexing.spv";

        viewport.height = -(float)WINDOW_HEIGHT;
        viewport.y = (float)WINDOW_HEIGHT;

    } else if (adapterInfo.shaderFormats & PAL_SHADER_FORMAT_DXIL) {
        sources[0] = "graphics/shaders/bin/dxil/texture_vert.dxil";
        sources[1] = "graphics/shaders/bin/dxil/descriptor_indexing.dxil";

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
            logResult(result, "Failed to create shader");
            return PAL_FALSE;
        }

        palFree(nullptr, bytecode);
    }

    // create descriptor set layout
    // We use descriptor count of 100 and only use 4 slots
    PalDescriptorSetLayoutBinding descriptorBindings[2];
    descriptorBindings[0].descriptorCount = 100;
    descriptorBindings[0].descriptorType = PAL_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
    
    descriptorBindings[1].descriptorCount = 1; // not an array
    descriptorBindings[1].descriptorType = PAL_DESCRIPTOR_TYPE_SAMPLER;

    PalDescriptorSetLayoutCreateInfo descriptorSetLayoutcreateInfo = {0};
    descriptorSetLayoutcreateInfo.bindingCount = 2;
    descriptorSetLayoutcreateInfo.bindings = descriptorBindings;

    // we only need the partially bound feature if supported. We dont use the other features
    if (descriptorIndexingCaps.flags & PAL_DESCRIPTOR_INDEXING_FLAG_PARTIALLY_BOUND) {
        descriptorSetLayoutcreateInfo.flags |= PAL_DESCRIPTOR_INDEXING_FLAG_PARTIALLY_BOUND;
    }

    result = palCreateDescriptorSetLayout(
        device,
        &descriptorSetLayoutcreateInfo,
        &descriptorSetLayout);

    if (result != PAL_RESULT_SUCCESS) {
        logResult(result, "Failed to create descriptor set layout");
        return PAL_FALSE;
    }

    // create descriptor pool
    PalDescriptorPoolBindingSize storageBufferBindingsizes[2];
    storageBufferBindingsizes[0].bindingCount = 100;
    storageBufferBindingsizes[0].descriptorType = PAL_DESCRIPTOR_TYPE_SAMPLED_IMAGE;

    storageBufferBindingsizes[1].bindingCount = 1;
    storageBufferBindingsizes[1].descriptorType = PAL_DESCRIPTOR_TYPE_SAMPLER;

    PalDescriptorPoolCreateInfo descriptorPoolCreateInfo = {0};
    descriptorPoolCreateInfo.maxDescriptorSets = 1; // only one set
    descriptorPoolCreateInfo.bindingSizeCount = 2;
    descriptorPoolCreateInfo.bindingSizes = storageBufferBindingsizes;
    descriptorPoolCreateInfo.flags = descriptorSetLayoutcreateInfo.flags;

    result = palCreateDescriptorPool(device, &descriptorPoolCreateInfo, &descriptorPool);
    if (result != PAL_RESULT_SUCCESS) {
        logResult(result, "Failed to create descriptor pool");
        return PAL_FALSE;
    }

    // allocate a single descriptor set from the descriptor pool
    // using the layout we created above
    result = palAllocateDescriptorSet(device, descriptorPool, descriptorSetLayout, &descriptorSet);
    if (result != PAL_RESULT_SUCCESS) {
        logResult(result, "Failed to allocate descriptor set");
        return PAL_FALSE;
    }

    // check if null descriptors was enabled and partially bound was not supported
    PalDescriptorIndexingFlags flags = descriptorIndexingCaps.flags;
    PalBool hasPartiallyBound = flags & PAL_DESCRIPTOR_INDEXING_FLAG_PARTIALLY_BOUND;
    if (adapterFeatures & PAL_ADAPTER_FEATURE_NULL_DESCRIPTORS && !hasPartiallyBound) {
        // write null descriptors into the slots
        PalDescriptorSetWriteInfo writeInfo = {0};
        writeInfo.layoutBindingIndex = 0;
        writeInfo.descriptorSet = descriptorSet;
        writeInfo.descriptorType = PAL_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
        writeInfo.descriptorCount = 100;

        writeInfo.arrayElement = 0;
        writeInfo.samplerInfos = nullptr;
        writeInfo.imageViewInfos = nullptr;
        writeInfo.tlasInfos =  nullptr;
        writeInfo.bufferInfos = nullptr;

        result = palUpdateDescriptorSet(device, 1, &writeInfo);
        if (result != PAL_RESULT_SUCCESS) {
            logResult(result, "Failed to update descriptor set");
            return PAL_FALSE;
        }

    } else {
        // both null descriptors and partially bound are not supported
        // we write the first texture to all slots
        PalDescriptorImageViewInfo imageViewInfos[100];
        for (int i = 0; i < 100; i++) {
            imageViewInfos[i].imageView = textureViews[0];
        }

        PalDescriptorSetWriteInfo writeInfo = {0};
        writeInfo.layoutBindingIndex = 0;
        writeInfo.descriptorSet = descriptorSet;
        writeInfo.descriptorType = PAL_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
        writeInfo.descriptorCount = 100;

        writeInfo.arrayElement = 0;
        writeInfo.samplerInfos = nullptr;
        writeInfo.imageViewInfos = imageViewInfos;
        writeInfo.tlasInfos =  nullptr;
        writeInfo.bufferInfos = nullptr;

        result = palUpdateDescriptorSet(device, 1, &writeInfo);
        if (result != PAL_RESULT_SUCCESS) {
            logResult(result, "Failed to update descriptor set");
            return PAL_FALSE;
        }
    }


    // we only write 4 descriptors
    uint32_t arrElements[] = { ARRAY_ELEMENT_0, ARRAY_ELEMENT_1, ARRAY_ELEMENT_2, ARRAY_ELEMENT_3 };

    PalDescriptorImageViewInfo descriptorImageInfos[4];
    descriptorImageInfos[0].imageView = textureViews[0];
    descriptorImageInfos[1].imageView = textureViews[1];
    descriptorImageInfos[2].imageView = textureViews[2];
    descriptorImageInfos[3].imageView = textureViews[3];

    PalDescriptorSamplerInfo descriptorSamplerInfo = {0};
    descriptorSamplerInfo.sampler = sampler;

    PalDescriptorSetWriteInfo writeInfos[5];
    // sampler
    writeInfos[0].layoutBindingIndex = 1;
    writeInfos[0].samplerInfos = &descriptorSamplerInfo;
    writeInfos[0].descriptorSet = descriptorSet;
    writeInfos[0].descriptorType = PAL_DESCRIPTOR_TYPE_SAMPLER;
    writeInfos[0].descriptorCount = 1;

    writeInfos[0].arrayElement = 0;
    writeInfos[0].imageViewInfos = nullptr;
    writeInfos[0].tlasInfos =  nullptr;
    writeInfos[0].bufferInfos = nullptr;

    // textures
    for (int i = 0; i < 4; i++) {
        writeInfos[i + 1].layoutBindingIndex = 0;
        writeInfos[i + 1].imageViewInfos = &descriptorImageInfos[i];
        writeInfos[i + 1].descriptorSet = descriptorSet;
        writeInfos[i + 1].descriptorType = PAL_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
        writeInfos[i + 1].descriptorCount = 1;

        writeInfos[i + 1].arrayElement = arrElements[i];
        writeInfos[i + 1].bufferInfos = nullptr;
        writeInfos[i + 1].samplerInfos = nullptr;
        writeInfos[i + 1].tlasInfos =  nullptr;
    }

    result = palUpdateDescriptorSet(device, 5, writeInfos);
    if (result != PAL_RESULT_SUCCESS) {
        logResult(result, "Failed to update descriptor set");
        return PAL_FALSE;
    }

    // create pipeline layout
    PalPipelineLayoutCreateInfo pipelineLayoutCreateInfo = {0};
    pipelineLayoutCreateInfo.descriptorSetLayoutCount = 1;
    pipelineLayoutCreateInfo.descriptorSetLayouts = &descriptorSetLayout;
    pipelineLayoutCreateInfo.usePushConstant = PAL_TRUE;
    pipelineLayoutCreateInfo.pushConstantInfo.offset = 0;
    pipelineLayoutCreateInfo.pushConstantInfo.size = sizeof(PushConstant); // must match shader

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
    PalVertexLayout vertexLayout = {0};
    PalVertexAttribute vertexAttributes[2];

    // position
    vertexAttributes[0].semanticID = PAL_VERTEX_SEMANTIC_ID_POSITION;
    vertexAttributes[0].type = PAL_VERTEX_TYPE_FLOAT2;

    // texture coordinates
    vertexAttributes[1].semanticID = PAL_VERTEX_SEMANTIC_ID_TEXCOORD;
    vertexAttributes[1].type = PAL_VERTEX_TYPE_FLOAT2;

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
        logResult(result, "Failed to create pipeline");
        return PAL_FALSE;
    }

    for (int i = 0; i < 2; i++) {
        palDestroyShader(shaders[i]);
    }

    // wait for the vertices copy to be done
    result = palWaitFence(fence, PAL_INFINITE);
    if (result != PAL_RESULT_SUCCESS) {
        logResult(result, "Failed to wait fence");
        return PAL_FALSE;
    }

    // the vertices have been copied
    palDestroyFence(fence);
    palDestroyBuffer(stagingBuffer);

    // main loop
    uint32_t currentFrame = 0;
    PalBool running = PAL_TRUE;

    PalRect2D scissor = {0};
    scissor.height = WINDOW_HEIGHT;
    scissor.width = WINDOW_WIDTH;

    PushConstant pushConstant = {0};
    pushConstant.textureIndices[0] = ARRAY_ELEMENT_0;
    pushConstant.textureIndices[1] = ARRAY_ELEMENT_1;
    pushConstant.textureIndices[2] = ARRAY_ELEMENT_2;
    pushConstant.textureIndices[3] = ARRAY_ELEMENT_3;

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
        barrierInfo.oldState = PAL_USAGE_STATE_UNDEFINED;
        barrierInfo.srcStages = PAL_PIPELINE_STAGE_NONE;
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
        palCmdPushConstants(cmdBuffers[currentFrame], 0, sizeof(PushConstant), &pushConstant);
        palCmdBindDescriptorSet(cmdBuffers[currentFrame], 0, descriptorSet);
        palCmdSetViewport(cmdBuffers[currentFrame], 1, &viewport);
        palCmdSetScissors(cmdBuffers[currentFrame], 1, &scissor);
       
        uint64_t offset[] = {0};
        palCmdBindVertexBuffers(cmdBuffers[currentFrame], 0, 1, &vertexBuffer, offset);
        palCmdDraw(cmdBuffers[currentFrame], 6, 1, 0, 0);
        palCmdEndRendering(cmdBuffers[currentFrame]);

        // change the state of the image view to make it presentable
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

    palDestroyDescriptorPool(descriptorPool);
    palDestroyDescriptorSetLayout(descriptorSetLayout);

    palDestroySampler(sampler);
    for (int i = 0; i < 4; i++) {
        palDestroyImageView(textureViews[i]);
        palDestroyImage(textures[i]);
        palDestroyBuffer(imageStagingBuffers[i]);
    }

    palDestroyBuffer(vertexBuffer);
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
