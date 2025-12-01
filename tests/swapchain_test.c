

#include "pal/pal_graphics.h"
#include "pal/pal_video.h"
#include "tests.h"

bool swapchainTest()
{
    palLog(nullptr, "");
    palLog(nullptr, "===========================================");
    palLog(nullptr, "Swapchain Test");
    palLog(nullptr, "===========================================");
    palLog(nullptr, "");

    // we need a window. We use PAL video system to create the window
    PalResult result = palInitVideo(nullptr, nullptr);
    if (result != PAL_RESULT_SUCCESS) {
        const char* error = palFormatResult(result);
        palLog(nullptr, "Failed to initialize video: %s", error);
        return false;
    }

    PalWindow* window = nullptr;
    PalWindowCreateInfo windowCreateInfo = {0};
    windowCreateInfo.height = 480;
    windowCreateInfo.width = 640;
    windowCreateInfo.show = true;

    // check if we support decorated windows (title bar, close etc)
    PalVideoFeatures64 features = palGetVideoFeaturesEx();
    if (!(features & PAL_VIDEO_FEATURE64_DECORATED_WINDOW)) {
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

    // enumerate all available GPUs from internal and custom backends
    Int32 count = 0;
    result = palEnumerateGPUAdapters(&count, nullptr);
    if (result != PAL_RESULT_SUCCESS) {
        const char* error = palFormatResult(result);
        palLog(nullptr, "Failed to get query Adapters (GPUs): %s", error);
        return false;
    }

    if (count == 0) {
        palLog(nullptr, "No Adapters found");
        return false;
    }
    palLog(nullptr, "Adapter (GPUs) Count: %d", count);

    // allocate an array of adapters or use a fixed array
    // Example: PalGPUAdapter* adapters[12];
    PalGPUAdapter** adapters = nullptr;
    adapters = palAllocate(nullptr, sizeof(PalGPUAdapter*) * count, 0);
    if (!adapters) {
        palLog(nullptr, "Failed to allocate memory");
        return false;
    }

    result = palEnumerateGPUAdapters(&count, adapters);
    if (result != PAL_RESULT_SUCCESS) {
        const char* error = palFormatResult(result);
        palLog(nullptr, "Failed to get query Adapters (GPUs): %s", error);
        return false;
    }

    // filter the adapters for Vulkan
    PalGPUAdapter* vulkanAdapter = nullptr;
    PalGPUAdapterInfo info = {0};

    for (Int32 i = 0; i < count; i++) {
        PalGPUAdapter* adapter = adapters[i];
        result = palGetGPUAdapterInfo(adapter, &info);
        if (result != PAL_RESULT_SUCCESS) {
            const char* error = palFormatResult(result);
            palLog(nullptr, "Failed to get adapter info: %s", error);
            palFree(nullptr, adapters);
            return false;
        }

        // check if its Vulkan
        if (info.apiType == PAL_GPU_API_TYPE_VULKAN) {
            vulkanAdapter = adapter;
            break;
        }
    }

    palFree(nullptr, adapters);
    if (!vulkanAdapter) {
        palLog(nullptr, "Failed to find a vulkan adapter");
        return false;
    }

    // get capabilities about the vulkan adapter and check if we support 
    // graphics command queues
    PalGPUAdapterCapabilities caps = {0};
    result = palGetGPUAdapterCapabilities(vulkanAdapter, &caps);
    if (result != PAL_RESULT_SUCCESS) {
        const char* error = palFormatResult(result);
        palLog(nullptr, "Failed to get adapter info: %s", error);
        return false;
    }

    // check if we support a graphics command queue
    const char* msg = "This Adapter (GPU) does not have any graphics queues";
    if (!(caps.features & PAL_GPU_FEATURE_SWAPCHAIN)) {
        palLog(nullptr, msg);
        return false;
    }

    if (!caps.maxGraphicsQueues) {
        palLog(nullptr, msg);
        return false;
    }

    // create a device and a graphics command queue with the vulkan adapter
    PalGPUDevice* device = nullptr;
    PalGPUFeatures GPUfeatures = PAL_GPU_FEATURE_SWAPCHAIN;

    result = palCreateGPUDevice(vulkanAdapter, GPUfeatures, &device);
    if (result != PAL_RESULT_SUCCESS) {
        const char* error = palFormatResult(result);
        palLog(nullptr, "Failed to create device: %s", error);
        return false;
    }

    // create a graphics command queue
    PalGPUCommandQueueType queueType = PAL_GPU_COMMAND_QUEUE_TYPE_GRAPHICS;
    PalGPUCommandQueue* graphicsQueue = nullptr;
    result = palCreateGPUCommandQueue(device, queueType, &graphicsQueue);
    if (result != PAL_RESULT_SUCCESS) {
        const char* error = palFormatResult(result);
        palLog(nullptr, "Failed to create command queue: %s", error);
        return false;
    }

    // create and retrive the native window handles
    PalGPUWindow gpuWindow = {0};
    PalWindowHandleInfo windowHandleInfo = {};
    windowHandleInfo = palGetWindowHandleInfo(window);
    gpuWindow.display = windowHandleInfo.nativeDisplay;
    gpuWindow.window = windowHandleInfo.nativeWindow;

    // check if the command queue supports presentation to your window
    bool canPresent = palCanCommandQueuePresent(graphicsQueue, &gpuWindow);
    if (!canPresent) {
        palLog(nullptr, "Command queue cannot present to provided window");
        // cleanup
        return false;
    }

    // query swapchain capabilities of the adapter and the window
    PalSwapchainCapabilities swapchainCaps = {0};
    result = palQuerySwapchainCapabilities(
        vulkanAdapter, 
        &gpuWindow, 
        &swapchainCaps);

    if (result != PAL_RESULT_SUCCESS) {
        const char* error = palFormatResult(result);
        palLog(nullptr, "Failed to query swapchain capabilities: %s", error);
        return false;
    }
    
    // log swapchain capabilities
    Uint32 bufferLayers = swapchainCaps.maxBufferArrayLayers;
    palLog(nullptr, "Swapchain capabilities:");
    palLog(nullptr, " Min buffer count: %d", swapchainCaps.minBufferCount);
    palLog(nullptr, " Max buffer count: %d", swapchainCaps.maxBufferCount);
    palLog(nullptr, " Max buffer array layers: %d", bufferLayers);

    palLog(nullptr, " Min width: %d", swapchainCaps.minWidth);
    palLog(nullptr, " Min height: %d", swapchainCaps.minHeight);
    palLog(nullptr, " Max width: %d", swapchainCaps.maxWidth);
    palLog(nullptr, " Max height: %d", swapchainCaps.maxHeight);

    palLog(nullptr, " Supported formats:");
    if (swapchainCaps.formats & PAL_SWAPCHAIN_FORMAT_BGRA8_SRGB_SRGB) {
        palLog(nullptr, "  BGRA8 SRGB SRGB");
    }

    if (swapchainCaps.formats & PAL_SWAPCHAIN_FORMAT_BGRA8_UNORM_SRGB) {
        palLog(nullptr, "  BGRA8 UNORM SRGB");
    }

    if (swapchainCaps.formats & PAL_SWAPCHAIN_FORMAT_RGBA16_FLOAT_HDR10) {
        palLog(nullptr, "  RGBA16 FLOAT HDR10");
    }

    if (swapchainCaps.formats & PAL_SWAPCHAIN_FORMAT_RGBA8_UNORM_SRGB) {
        palLog(nullptr, "  RGBA8 UNORM SRGB");
    }

    palLog(nullptr, " Supported present modes:");
    if (swapchainCaps.presentModes & PAL_PRESENT_MODE_FIFO) {
        palLog(nullptr, "  FIFO");
    }

    if (swapchainCaps.presentModes & PAL_PRESENT_MODE_IMMEDIATE) {
        palLog(nullptr, "  Immediate");
    }

    if (swapchainCaps.presentModes & PAL_PRESENT_MODE_MAILBOX) {
        palLog(nullptr, "  Mailbox");
    }

    palLog(nullptr, " Supported transforms:");
    if (swapchainCaps.transforms & PAL_SWAPCHAIN_TRANSFORM_LANDSCAPE) {
        palLog(nullptr, "  Landscape");
    }

    if (swapchainCaps.transforms & PAL_SWAPCHAIN_TRANSFORM_PORTRAIT) {
        palLog(nullptr, "  Portrait");
    }

    if (swapchainCaps.transforms & PAL_SWAPCHAIN_TRANSFORM_PORTRAIT_FLIPPED) {
        palLog(nullptr, "  Portrait flipped");
    }

    if (swapchainCaps.transforms & PAL_SWAPCHAIN_TRANSFORM_LANDSCAPE_FLIPPED) {
        palLog(nullptr, "  Landscape flipped");
    }

    palLog(nullptr, " Supported sharing modes:");
    if (swapchainCaps.sharingModes & PAL_SWAPCHAIN_SHARING_MODE_EXCLUSIVE) {
        palLog(nullptr, "  Exclusive");
    }

    if (swapchainCaps.sharingModes & PAL_SWAPCHAIN_SHARING_MODE_CONCURRENT) {
        palLog(nullptr, "  Concurrent");
    }

    palLog(nullptr, " Supported composite alphas:");
    if (swapchainCaps.compositeAlphas & PAL_COMPOSITE_ALPHA_OPAQUE) {
        palLog(nullptr, "  Opaque");
    }

    if (swapchainCaps.compositeAlphas & PAL_COMPOSITE_ALPHA_PRE_MULTIPLIED) {
        palLog(nullptr, "  Pre Multiplied");
    }

    if (swapchainCaps.compositeAlphas & PAL_COMPOSITE_ALPHA_POST_MULTIPLIED) {
        palLog(nullptr, "  Post Multiplied");
    }

    palLog(nullptr, " Supported usages:");
    if (swapchainCaps.usages & PAL_SWAPCHAIN_USAGE_SAMPLED) {
        palLog(nullptr, "  Sampled");
    }

    if (swapchainCaps.usages & PAL_SWAPCHAIN_USAGE_TRANSFER_DST) {
        palLog(nullptr, "  Transfer Dst");
    }

    if (swapchainCaps.usages & PAL_SWAPCHAIN_USAGE_TRANSFER_SRC) {
        palLog(nullptr, "  Transfer src");
    }

    if (swapchainCaps.usages & PAL_SWAPCHAIN_USAGE_COLOR_ATTACHEMENT) {
        palLog(nullptr, "  Color attachment");
    }

    // create a swapchain
    PalSwapchain* swapchain = nullptr;
    PalSwapchainCreateInfo swapchainCreateInfo = {0};
    swapchainCreateInfo.bufferArrayLayerCount = 1; // works on all systems
    
    // check max count to choose buffers but for this example
    //we just set it to the minimal supported
    swapchainCreateInfo.bufferCount = swapchainCaps.minBufferCount;
    swapchainCreateInfo.clipped = true;
    swapchainCreateInfo.compositeAlpha = PAL_COMPOSITE_ALPHA_OPAQUE;
    swapchainCreateInfo.format = PAL_SWAPCHAIN_FORMAT_RGBA8_UNORM_SRGB;

    // set size
    swapchainCreateInfo.width = 640;
    swapchainCreateInfo.height = 480;
    if (640 > swapchainCaps.maxWidth) {
        // we set it to the minimal to mak it work across systems
        swapchainCreateInfo.width = swapchainCaps.minWidth;
    }

    if (480 > swapchainCaps.maxHeight) {
        // we set it to the minimal to mak it work across systems
        swapchainCreateInfo.height = swapchainCaps.minHeight;
    }

    swapchainCreateInfo.presentMode = PAL_PRESENT_MODE_FIFO;
    swapchainCreateInfo.transform = PAL_SWAPCHAIN_TRANSFORM_LANDSCAPE;
    swapchainCreateInfo.usage = PAL_SWAPCHAIN_USAGE_COLOR_ATTACHEMENT;
    swapchainCreateInfo.sharingMode = PAL_SWAPCHAIN_SHARING_MODE_EXCLUSIVE;

    result = palCreateSwapchain(
        graphicsQueue, 
        &gpuWindow, 
        &swapchainCreateInfo,
        &swapchain);

    if (result != PAL_RESULT_SUCCESS) {
        const char* error = palFormatResult(result);
        palLog(nullptr, "Failed to create swapchain: %s", error);
        return false;
    }

    // get the number of buffers the swapchain has
    // this should match the buffers used to create the swapchain
    Uint32 swapchainBufferCount = palGetSwapchainBufferCount(swapchain);
    palLog(nullptr, "Swapchain buffer count: %d", swapchainBufferCount);

    // create a render target view for the first swapchain buffer
    PalRenderTargetView* rtv = nullptr;
    result = palCreateRenderTargetView(swapchain, 0, &rtv);
    if (result != PAL_RESULT_SUCCESS) {
        const char* error = palFormatResult(result);
        palLog(nullptr, "Failed to create render target view: %s", error);
        return false;
    }

    // destroy the render target view
    palDestroyRenderTargetView(rtv);

    // destroy the swapchain
    palDestroySwapchain(swapchain);

    // destroy command queue and gpu device
    palDestroyGPUCommandQueue(graphicsQueue);
    palDestroyGPUDevice(device);

    // destroy window and shutdown video system
    palDestroyWindow(window);
    palShutdownVideo();

    // shutdown the graphics system
    palShutdownGraphics();
    
    return true;
}