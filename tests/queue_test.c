
#include "pal/pal_graphics.h"
#include "pal/pal_video.h"
#include "tests.h"

bool queueTest()
{
    palLog(nullptr, "");
    palLog(nullptr, "===========================================");
    palLog(nullptr, "Queue Test");
    palLog(nullptr, "===========================================");
    palLog(nullptr, "");

    // initialize the graphics system
    PalResult result = palInitGraphics(false, nullptr);
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

    // filter the adapters for Vulkan
    PalAdapter* vulkanAdapter = nullptr;
    PalAdapterInfo info = {0};

    for (Int32 i = 0; i < count; i++) {
        PalAdapter* adapter = adapters[i];
        result = palGetAdapterInfo(adapter, &info);
        if (result != PAL_RESULT_SUCCESS) {
            const char* error = palFormatResult(result);
            palLog(nullptr, "Failed to get adapter info: %s", error);
            palFree(nullptr, adapters);
            return false;
        }

        // check if its Vulkan
        if (info.apiType == PAL_ADAPTER_API_TYPE_VULKAN) {
            vulkanAdapter = adapter;
            break;
        }
    }

    palFree(nullptr, adapters);
    if (!vulkanAdapter) {
        palLog(nullptr, "Failed to find a vulkan adapter");
        return false;
    }

    // get capabilities about the adapter
    PalAdapterCapabilities caps = {0};
    result = palGetAdapterCapabilities(vulkanAdapter, &caps);
    if (result != PAL_RESULT_SUCCESS) {
        const char* error = palFormatResult(result);
        palLog(nullptr, "Failed to get adapter info: %s", error);
        return false;
    }

    if (caps.maxGraphicsQueues == 0) {
        palLog(nullptr, "adapter does not support graphics queues");
        return false;
    }

    // create a device with the vulkan adapter
    PalDevice* device = nullptr;
    PalAdapterFeatures features = PAL_ADAPTER_FEATURE_SWAPCHAIN;
    
    // enable multi viewport if supported
    if (caps.features & PAL_ADAPTER_FEATURE_MULTI_VIEWPORT) {
        features |= PAL_ADAPTER_FEATURE_MULTI_VIEWPORT;
    }

    result = palCreateDevice(vulkanAdapter, features, &device);
    if (result != PAL_RESULT_SUCCESS) {
        const char* error = palFormatResult(result);
        palLog(nullptr, "Failed to create device: %s", error);
        return false;
    }

    // create a graphics queue
    PalQueue* gfxQueue = nullptr;
    result = palCreateQueue(device, PAL_QUEUE_TYPE_GRAPHICS, &gfxQueue);
    if (result != PAL_RESULT_SUCCESS) {
        const char* error = palFormatResult(result);
        palLog(nullptr, "Failed to create queue: %s", error);
        return false;
    }

    // check if the graphics queue we created is presentable
    // to the provided window. we need a window
    result = palInitVideo(nullptr, nullptr);
    if (result != PAL_RESULT_SUCCESS) {
        const char* error = palFormatResult(result);
        palLog(nullptr, "Failed to initialize video: %s", error);
        return false;
    }

    PalWindow* window = nullptr;
    PalWindowCreateInfo createInfo = {0};
    createInfo.height = 480;
    createInfo.width = 640;
    createInfo.show = true;

    // check if we support decorated windows (title bar, close etc)
    PalVideoFeatures64 videoFeatures = palGetVideoFeaturesEx();
    if (!(videoFeatures & PAL_VIDEO_FEATURE64_DECORATED_WINDOW)) {
        // if we dont support, we need to create a borderless window
        // and create the decorations ourselves
        createInfo.style |= PAL_WINDOW_STYLE_BORDERLESS;
    }

    result = palCreateWindow(&createInfo, &window);
    if (result != PAL_RESULT_SUCCESS) {
        const char* error = palFormatResult(result);
        palLog(nullptr, "Failed to create window: %s", error);
        return false;
    }

    PalGraphicsWindow gWindow = {0};
    PalWindowHandleInfo winInfo = palGetWindowHandleInfo(window);
    gWindow.display = winInfo.nativeDisplay;
    gWindow.window = winInfo.nativeWindow;

    bool canPresent = palCanQueuePresent(gfxQueue, &gWindow);
    const char* boolString = "True";
    if (!canPresent) {
        boolString = "False";
    }
    palLog(nullptr, "Graphics queue presentable: %s", boolString);

    palDestroyWindow(window);
    palShutdownVideo();

    // destroy the graphics queue
    palDestroyQueue(gfxQueue);

    // destroy the device
    palDestroyDevice(device);

    // shutdown the graphics system
    palShutdownGraphics();
    
    return true;
}