
#include "pal/pal_graphics.h"
#include "tests.h"

bool gpuDeviceTest()
{
    palLog(nullptr, "");
    palLog(nullptr, "===========================================");
    palLog(nullptr, "GPU Device Test");
    palLog(nullptr, "===========================================");
    palLog(nullptr, "");

    // initialize the graphics system
    PalResult result = palInitGraphics(true, nullptr);
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

    // get information about all the adapters
    PalGPUAdapterInfo info;
    PalGPUAdapter* vulkanAdapter = nullptr;
    PalGPUCommandQueues vulkanCommandQueues;
    PalGPUFeatures vulkanFeatures;

    for (Int32 i = 0; i < count; i++) {
        PalGPUAdapter* adapter = adapters[i];
        result = palGetGPUAdapterInfo(adapter, &info);
        if (result != PAL_RESULT_SUCCESS) {
            const char* error = palFormatResult(result);
            palLog(nullptr, "Failed to get adapter info: %s", error);
            palFree(nullptr, adapters);
            return false;
        }

        if (info.apiType == PAL_GPU_API_VULKAN) {
            vulkanAdapter = adapter;
            vulkanCommandQueues = info.commandQueues;
            vulkanFeatures = info.features;
            break;
        }
    }

    palFree(nullptr, adapters);
    if (!vulkanAdapter) {
        palLog(nullptr, "Failed to find a vulkan adapter");
        return false;
    }

    // create a device with the vulkan adapter
    PalGPUDevice* device = nullptr;
    PalGPUDeviceCreateInfo deviceCreateInfo = {0};

    // almost supported on all platforms
    if (vulkanCommandQueues & PAL_GPU_COMMAND_QUEUE_GRAPHICS) {
        deviceCreateInfo.commandQueues = PAL_GPU_COMMAND_QUEUE_GRAPHICS;
    }

    // enable swapchain and maybe multi viewport if supported
    if (vulkanFeatures & PAL_GPU_FEATURE_SWAPCHAIN) {
        deviceCreateInfo.features = PAL_GPU_FEATURE_SWAPCHAIN;
    }

    if (vulkanFeatures & PAL_GPU_FEATURE_MULTI_VIEWPORT) {
        deviceCreateInfo.features = PAL_GPU_FEATURE_MULTI_VIEWPORT;
    }

    result = palCreateGPUDevice(vulkanAdapter, &deviceCreateInfo, &device);
    if (result != PAL_RESULT_SUCCESS) {
        const char* error = palFormatResult(result);
        palLog(nullptr, "Failed to create device: %s", error);
        return false;
    }

    // destroy the device
    palDestroyGPUDevice(device);

    // shutdown the graphics system
    palShutdownGraphics();
    
    return true;
}