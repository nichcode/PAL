
#include "pal/pal_graphics.h"
#include "pal/pal_video.h"
#include "pal/pal_system.h"
#include "tests.h"

#define WINDOW_WIDTH 640
#define WINDOW_HEIGHT 480
#define MAX_FRAMES_IN_FLIGHT 2
#define TEXTURE_WIDTH 128
#define TEXTURE_HEIGHT 128
#define CHECKER_SIZE 16

static void createCheckerboardTexture(
    Uint32* texture,
    Uint32 width,
    Uint32 height,
    Uint32 checkerSize)
{
    Uint8* pixels = (Uint8*)texture;
    for (Int32 y = 0; y < height; ++y) {
        for (Int32 x = 0; x < width; ++x) {
            Int32 i = (y * width + x) * 4;
            int checker = ((x / checkerSize) ^ (y / checkerSize)) & 1;
            if (checker) {
                pixels[i + 0] = 255; // Red bit
                pixels[i + 1] = 0;   // Green bit
                pixels[i + 2] = 0;   // Blue bit
                pixels[i + 3] = 255; // Alpha bit

            } else {
                pixels[i + 0] = 0;   // Red bit
                pixels[i + 1] = 255; // Green bit
                pixels[i + 2] = 0;   // Blue bit
                pixels[i + 3] = 255; // Alpha bit
            }
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

bool textureTest()
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
    PalBuffer* stagingBuffer = nullptr;
    PalMemory* vertexBufferMemory = nullptr;
    PalMemory* stagingBufferMemory = nullptr;

    PalDescriptorSetLayout* descriptorSetLayout = nullptr;
    PalDescriptorPool* descriptorPool = nullptr;
    PalDescriptorSet* descriptorSet = nullptr;

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
    windowCreateInfo.title = "Texture Window";

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
            // We want an adapter that supports spirv 1.0 or dxbc 5.1
            result = palGetAdapterInfo(adapter, &adapterInfo);
            if (result != PAL_RESULT_SUCCESS) {
                const char* error = palFormatResult(result);
                palLog(nullptr, "Failed to get adapter info: %s", error);
                return false;
            }

            // we prefer spirv first if an adapter supports multiple shader formats
            Uint32 target = 0;
            if (adapterInfo.shaderFormats & PAL_SHADER_FORMAT_SPIRV) {
                target = palGetHighestSupportedShaderTarget(adapter, PAL_SHADER_FORMAT_SPIRV);
                if (target >= PAL_MAKE_SHADER_TARGET(1, 0)) {
                    break;
                }
            }

            if (adapterInfo.shaderFormats & PAL_SHADER_FORMAT_DXBC) {
                target = palGetHighestSupportedShaderTarget(adapter, PAL_SHADER_FORMAT_DXBC);
                if (target >= PAL_MAKE_SHADER_TARGET(5, 1)) {
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

    PalFence* fence = nullptr;
    result = palCreateFence(device, false, &fence);
    if (result != PAL_RESULT_SUCCESS) {
        const char* error = palFormatResult(result);
        palLog(nullptr, "Failed to create fence: %s", error);
        return false;
    }

    // we dont want to load the texture from disk so we will create a 
    // checkerboard texture and use that rather
    Uint32 texture[TEXTURE_WIDTH * TEXTURE_HEIGHT];
    memset(texture, 0, TEXTURE_WIDTH * TEXTURE_HEIGHT);
    createCheckerboardTexture(texture, TEXTURE_WIDTH, TEXTURE_HEIGHT, CHECKER_SIZE);

    // create image for the texture
    PalImage* checkerboard = nullptr;
    PalImageCreateInfo imageCreateInfo = {0};
    imageCreateInfo.depthOrArraySize = 1;
    imageCreateInfo.format = PAL_FORMAT_R8G8B8A8_UNORM;
    imageCreateInfo.mipLevelCount = 1; // simple
    imageCreateInfo.sampleCount = PAL_SAMPLE_COUNT_1; // simple
    imageCreateInfo.type = PAL_IMAGE_TYPE_2D;
    imageCreateInfo.usages = PAL_IMAGE_USAGE_TRANSFER_DST | PAL_IMAGE_USAGE_SAMPLED;
    imageCreateInfo.width = TEXTURE_WIDTH;
    imageCreateInfo.height = TEXTURE_HEIGHT;

    result = palCreateImage(device, &imageCreateInfo, &checkerboard);
    if (result != PAL_RESULT_SUCCESS) {
        const char* error = palFormatResult(result);
        palLog(nullptr, "Failed to create image: %s", error);
        return false;
    }

    // allocate memory for the image
    PalMemoryRequirements imageMemReq = {0};
    result = palGetImageMemoryRequirements(checkerboard, &imageMemReq);
    if (result != PAL_RESULT_SUCCESS) {
        const char* error = palFormatResult(result);
        palLog(nullptr, "Failed to get image memory requirement: %s", error);
        return false;
    }

    PalMemory* checkerboardMemory = nullptr;
    result = palAllocateMemory(
        device, 
        PAL_MEMORY_TYPE_GPU_ONLY, 
        imageMemReq.memoryMask, 
        imageMemReq.size, 
        &checkerboardMemory);

    if (result != PAL_RESULT_SUCCESS) {
        const char* error = palFormatResult(result);
        palLog(nullptr, "Failed to allocate memory: %s", error);
        return false;
    }

    result = palBindImageMemory(checkerboard, checkerboardMemory, 0);
    if (result != PAL_RESULT_SUCCESS) {
        const char* error = palFormatResult(result);
        palLog(nullptr, "Failed to bind image memory: %s", error);
        return false;
    }

    // create staging buffer to transfer the data to the image
    PalBufferImageCopyInfo bufferImageCopyInfo = {0};
    bufferImageCopyInfo.ImageArrayLayerCount = 1;
    bufferImageCopyInfo.imageWidth = TEXTURE_WIDTH;
    bufferImageCopyInfo.imageHeight = TEXTURE_HEIGHT;
    bufferImageCopyInfo.imageDepth = 1; // 2D image

    Uint64 imageCopyStagingBufferSize = 0;
    Uint32 bufferRowLength = 0;
    Uint32 bufferImageHeight = 0;

    result = palComputeImageCopyStagingBufferRequirements(
        device, 
        imageCreateInfo.format, 
        &bufferImageCopyInfo, 
        &bufferRowLength, 
        &bufferImageHeight,
        &imageCopyStagingBufferSize);

    if (result != PAL_RESULT_SUCCESS) {
        const char* error = palFormatResult(result);
        palLog(nullptr, "Failed to compute image copy staging buffer info: %s", error);
        return false;
    }

    // update our copy with the required buffer row length and buffer image height
    bufferImageCopyInfo.bufferRowLength = bufferRowLength;
    bufferImageCopyInfo.bufferImageHeight = bufferImageHeight;

    // create staging buffer to transfer the data to the image
    PalBuffer* imageStagingBuffer = nullptr;
    PalBufferCreateInfo imageStagingBufferCreateInfo = {0};
    imageStagingBufferCreateInfo.size = imageCopyStagingBufferSize;
    imageStagingBufferCreateInfo.usages = PAL_BUFFER_USAGE_TRANSFER_SRC;

    result = palCreateBuffer(device, &imageStagingBufferCreateInfo, &imageStagingBuffer);
    if (result != PAL_RESULT_SUCCESS) {
        const char* error = palFormatResult(result);
        palLog(nullptr, "Failed to create image staging buffer: %s", error);
        return false;
    }

    PalMemoryRequirements imageStagingBufferMemReq = {0};
    result = palGetBufferMemoryRequirements(imageStagingBuffer, &imageStagingBufferMemReq);
    if (result != PAL_RESULT_SUCCESS) {
        const char* error = palFormatResult(result);
        palLog(nullptr, "Failed to get image staging buffer memory requirement: %s", error);
        return false;
    }

    PalMemory* imageStagingBufferMemory = nullptr;
    result = palAllocateMemory(
        device, 
        PAL_MEMORY_TYPE_CPU_UPLOAD, 
        imageStagingBufferMemReq.memoryMask, 
        imageStagingBufferMemReq.size, 
        &imageStagingBufferMemory);

    if (result != PAL_RESULT_SUCCESS) {
        const char* error = palFormatResult(result);
        palLog(nullptr, "Failed to allocate memory: %s", error);
        return false;
    }

    result = palBindBufferMemory(imageStagingBuffer, imageStagingBufferMemory, 0);
    if (result != PAL_RESULT_SUCCESS) {
        const char* error = palFormatResult(result);
        palLog(nullptr, "Failed to bind image staging buffer memory: %s", error);
        return false;
    }

    // copy data
    void* data = nullptr;
    result = palMapBufferMemory(
        imageStagingBuffer,
        0, 
        imageStagingBufferCreateInfo.size, 
        &data);

    if (result != PAL_RESULT_SUCCESS) {
        const char* error = palFormatResult(result);
        palLog(nullptr, "Failed to map buffer memory: %s", error);
        return false;
    }

    // write data to the mapped image copy staging buffer
    result = palWriteToImageCopyStagingBuffer(
        device, 
        data, 
        texture,
        imageCreateInfo.format,
        &bufferImageCopyInfo);

    if (result != PAL_RESULT_SUCCESS) {
        const char* error = palFormatResult(result);
        palLog(nullptr, "Failed to write to image copy staging buffer: %s", error);
        return false;
    }

    palUnmapBufferMemory(imageStagingBuffer);

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

    // copy image staging buffer to the checkerboard image
    // first the image must be in the correct layout
    PalUsageStateInfo oldImageUsageState = {0};
    PalUsageStateInfo newImageUsageState = {0};
    newImageUsageState.usageState = PAL_USAGE_STATE_TRANSFER_WRITE;

    // set a barrier on the image to transition it into transfer dst state
    PalImageSubresourceRange checkerboardRange = {0};
    checkerboardRange.startMipLevel = 0;
    checkerboardRange.startArrayLayer = 0;
    checkerboardRange.mipLevelCount = 1;
    checkerboardRange.layerArrayCount = 1;

    result = palCmdImageBarrier(
        cmdBuffers[0], 
        checkerboard, 
        &checkerboardRange, 
        &oldImageUsageState, 
        &newImageUsageState);

    if (result != PAL_RESULT_SUCCESS) {
        const char* error = palFormatResult(result);
        palLog(nullptr, "Failed to set image barrier: %s", error);
        return false;
    }

    result = palCmdCopyBufferToImage(
        cmdBuffers[0], 
        checkerboard, 
        imageStagingBuffer, 
        &bufferImageCopyInfo);

    if (result != PAL_RESULT_SUCCESS) {
        const char* error = palFormatResult(result);
        palLog(nullptr, "Failed to copy buffer to image: %s", error);
        return false;
    }

    // we should transition the image into a shader read state so we dont do that
    // in the main loop
    PalShaderStage fragmentShaderStage[] = { PAL_SHADER_STAGE_FRAGMENT };
    oldImageUsageState = newImageUsageState;
    newImageUsageState.usageState = PAL_USAGE_STATE_SHADER_READ;
    newImageUsageState.shaderStageCount = 1;
    newImageUsageState.shaderStages = fragmentShaderStage; // fragment shader will read

    result = palCmdImageBarrier(
        cmdBuffers[0], 
        checkerboard, 
        &checkerboardRange, 
        &oldImageUsageState, 
        &newImageUsageState);

    if (result != PAL_RESULT_SUCCESS) {
        const char* error = palFormatResult(result);
        palLog(nullptr, "Failed to set image barrier: %s", error);
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
    submitInfo.fence = fence;
    result = palSubmitCommandBuffer(queue, &submitInfo);
    if (result != PAL_RESULT_SUCCESS) {
        const char* error = palFormatResult(result);
        palLog(nullptr, "Failed to submit command buffer: %s", error);
        return false;
    }

    // now we have the checkerboard texture data in the image
    // we need an image view and a sampler
    PalImageView* checkerboardImageView = nullptr;
    PalImageViewCreateInfo checkerboardImageViewCreateInfo = {0};
    checkerboardImageViewCreateInfo.type = PAL_IMAGE_VIEW_TYPE_2D;
    checkerboardImageViewCreateInfo.subresourceRange = checkerboardRange;
    checkerboardImageViewCreateInfo.format = PAL_FORMAT_R8G8B8A8_UNORM;

    result = palCreateImageView(
        device, 
        checkerboard, 
        &checkerboardImageViewCreateInfo, 
        &checkerboardImageView);

    if (result != PAL_RESULT_SUCCESS) {
        const char* error = palFormatResult(result);
        palLog(nullptr, "Failed to create checkerboard image view: %s", error);
        return false;
    }

    PalSampler* sampler = nullptr;
    PalSamplerCreateInfo samplerCreateInfo = {0};
    samplerCreateInfo.addressModeU = PAL_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerCreateInfo.addressModeV = PAL_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerCreateInfo.addressModeW = PAL_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerCreateInfo.borderColor = PAL_BORDER_COLOR_INT_OPAQUE_BLACK;
    samplerCreateInfo.compareOp = PAL_COMPARE_OP_NEVER; // will not be used if its not enabled

    samplerCreateInfo.enableAnisotropy = false;
    samplerCreateInfo.enableCompare = false;
    samplerCreateInfo.magFilterMode = PAL_FILTER_MODE_LINEAR;
    samplerCreateInfo.minFilterMode = PAL_FILTER_MODE_LINEAR;
    samplerCreateInfo.maxAnisotropy = 1.0f;

    result = palCreateSampler(
        device, 
        &samplerCreateInfo,
        &sampler);

    if (result != PAL_RESULT_SUCCESS) {
        const char* error = palFormatResult(result);
        palLog(nullptr, "Failed to create sampler: %s", error);
        return false;
    }

    // create shaders
    Uint64 bytecodeSize = 0;
    void* bytecode = nullptr;
    const char* sources[2];
    PalShaderStage tmpShaderStages[2];

    PalShaderCreateInfo shaderCreateInfo = {0};
    if (adapterInfo.shaderFormats & PAL_SHADER_FORMAT_SPIRV) {
        sources[0] = "graphics/shaders/bin/spirv/texture_vert.spv";
        sources[1] = "graphics/shaders/bin/spirv/texture_frag.spv";

    } else if (adapterInfo.shaderFormats & PAL_SHADER_FORMAT_DXBC) {
        sources[0] = "graphics/shaders/bin/dxbc/texture_vert.dxbc";
        sources[1] = "graphics/shaders/bin/dxbc/texture_frag.dxbc";
    }

    tmpShaderStages[0] = PAL_SHADER_STAGE_VERTEX;
    tmpShaderStages[1] = PAL_SHADER_STAGE_FRAGMENT;

    for (int i = 0; i < 2; i++) {
        // read file
        if (!readFile(sources[i], nullptr, &bytecodeSize)) {
            palLog(nullptr, "Failed to read shader file");
            return false;
        }

        bytecode = palAllocate(nullptr, bytecodeSize, 0);
        if (!bytecode) {
            palLog(nullptr, "Failed to allocate memory");
            return false;
        }

        readFile(sources[i], bytecode, &bytecodeSize);

        shaderCreateInfo.bytecode = bytecode;
        shaderCreateInfo.bytecodeSize = bytecodeSize;
        shaderCreateInfo.entryName = "main";
        shaderCreateInfo.stage = tmpShaderStages[i];

        result = palCreateShader(device, &shaderCreateInfo, &shaders[i]);
        if (result != PAL_RESULT_SUCCESS) {
            const char* error = palFormatResult(result);
            palLog(nullptr, "Failed to create shader: %s", error);
            return false;
        }

        palFree(nullptr, bytecode);
    }

    // create descriptor set layout
    PalDescriptorSetLayoutBinding descriptorBindings[2];
    PalShaderStage shaderStages[] = { PAL_SHADER_STAGE_FRAGMENT };

    descriptorBindings[0].descriptorCount = 1; // not an array
    descriptorBindings[0].descriptorType = PAL_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
    descriptorBindings[0].shaderStageCount = 1;
    descriptorBindings[0].shaderStages = shaderStages;

    descriptorBindings[1].descriptorCount = 1; // not an array
    descriptorBindings[1].descriptorType = PAL_DESCRIPTOR_TYPE_SAMPLER;
    descriptorBindings[1].shaderStageCount = 1;
    descriptorBindings[1].shaderStages = shaderStages;

    PalDescriptorSetLayoutCreateInfo descriptorSetLayoutcreateInfo = {0};
    descriptorSetLayoutcreateInfo.bindingCount = 2;
    descriptorSetLayoutcreateInfo.bindings = descriptorBindings;

    result = palCreateDescriptorSetLayout(
        device,
        &descriptorSetLayoutcreateInfo,
        &descriptorSetLayout);

    if (result != PAL_RESULT_SUCCESS) {
        const char* error = palFormatResult(result);
        palLog(nullptr, "Failed to create descriptor set layout: %s", error);
        return false;
    }

    // create descriptor pool
    PalDescriptorPoolBindingSize storageBufferBindingsizes[2];
    storageBufferBindingsizes[0].bindingCount = 1;
    storageBufferBindingsizes[0].descriptorType = PAL_DESCRIPTOR_TYPE_SAMPLED_IMAGE;

    storageBufferBindingsizes[1].bindingCount = 1;
    storageBufferBindingsizes[1].descriptorType = PAL_DESCRIPTOR_TYPE_SAMPLER;

    PalDescriptorPoolCreateInfo descriptorPoolCreateInfo = {0};
    descriptorPoolCreateInfo.maxDescriptorSets = 1; // only one set
    descriptorPoolCreateInfo.maxDescriptorBindingSizes = 2;
    descriptorPoolCreateInfo.bindingSizes = storageBufferBindingsizes;

    result = palCreateDescriptorPool(device, &descriptorPoolCreateInfo, &descriptorPool);
    if (result != PAL_RESULT_SUCCESS) {
        const char* error = palFormatResult(result);
        palLog(nullptr, "Failed to create descriptor pool: %s", error);
        return false;
    }

    // allocate a single descriptor set from the descriptor pool
    // using the layout we created above
    result = palAllocateDescriptorSet(device, descriptorPool, descriptorSetLayout, &descriptorSet);
    if (result != PAL_RESULT_SUCCESS) {
        const char* error = palFormatResult(result);
        palLog(nullptr, "Failed to allocate descriptor set: %s", error);
        return false;
    }

    // write the inital data to the descriptor set since its created empty
    PalDescriptorImageViewInfo descriptorImageInfo = {0};
    descriptorImageInfo.imageView = checkerboardImageView;

    PalDescriptorSamplerInfo descriptorSamplerInfo = {0};
    descriptorSamplerInfo.sampler = sampler;

    PalDescriptorSetWriteInfo writeInfos[2];
    writeInfos[0].layoutBindingIndex = 0;
    writeInfos[0].imageViewInfos = &descriptorImageInfo;
    writeInfos[0].descriptorSet = descriptorSet;
    writeInfos[0].descriptorType = PAL_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
    writeInfos[0].descriptorCount = 1;

    writeInfos[0].arrayElement = 0;
    writeInfos[0].bufferInfos = nullptr;
    writeInfos[0].samplerInfos = nullptr;
    writeInfos[0].tlasInfos =  nullptr;

    writeInfos[1].layoutBindingIndex = 1;
    writeInfos[1].samplerInfos = &descriptorSamplerInfo;
    writeInfos[1].descriptorSet = descriptorSet;
    writeInfos[1].descriptorType = PAL_DESCRIPTOR_TYPE_SAMPLER;
    writeInfos[1].descriptorCount = 1;

    writeInfos[1].arrayElement = 0;
    writeInfos[1].imageViewInfos = nullptr;
    writeInfos[1].tlasInfos =  nullptr;
    writeInfos[1].bufferInfos = nullptr;

    result = palUpdateDescriptorSet(device, 2, writeInfos);
    if (result != PAL_RESULT_SUCCESS) {
        const char* error = palFormatResult(result);
        palLog(nullptr, "Failed to update descriptor set: %s", error);
        return false;
    }

    // create pipeline layout
    PalPipelineLayoutCreateInfo pipelineLayoutCreateInfo = {0};
    pipelineLayoutCreateInfo.descriptorSetLayoutCount = 1;
    pipelineLayoutCreateInfo.descriptorSetLayouts = &descriptorSetLayout;

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

    // texture coordinates
    vertexAttributes[1].semanticID = PAL_VERTEX_SEMANTIC_ID_TEXCOORD;
    vertexAttributes[1].semanticName = nullptr; // use default
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
        const char* error = palFormatResult(result);
        palLog(nullptr, "Failed to create graphics pipeline: %s", error);
        return false;
    }

    for (int i = 0; i < 2; i++) {
        palDestroyShader(shaders[i]);
    }

    // wait for the vertices copy to be done
    result = palWaitFence(fence, PAL_INFINITE);
    if (result != PAL_RESULT_SUCCESS) {
        const char* error = palFormatResult(result);
        palLog(nullptr, "Failed to wait for fence: %s", error);
        return false;
    }

    // the vertices have been copied
    palDestroyFence(fence);
    palDestroyBuffer(stagingBuffer);
    palFreeMemory(device, stagingBufferMemory);

    // we can destroy the image staging buffer
    palDestroyBuffer(imageStagingBuffer);
    palFreeMemory(device, imageStagingBufferMemory);

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

        result = palCmdBindDescriptorSet(cmdBuffers[currentFrame], 0, descriptorSet);
        if (result != PAL_RESULT_SUCCESS) {
            const char* error = palFormatResult(result);
            palLog(nullptr, "Failed to bind descriptor set: %s", error);
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

        result = palCmdDraw(cmdBuffers[currentFrame], 6, 1, 0, 0);
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
    palDestroyImageView(checkerboardImageView);
    palDestroyImage(checkerboard);
    palFreeMemory(device, checkerboardMemory);

    palDestroyBuffer(vertexBuffer);
    palFreeMemory(device, vertexBufferMemory);

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
