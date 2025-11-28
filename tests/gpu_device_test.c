
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
    PalResult result = palInitGraphics(false, nullptr);
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

    // get capabilities about the adapter
    PalGPUAdapterCapabilities caps = {0};
    result = palGetGPUAdapterCapabilities(vulkanAdapter, &caps);
    if (result != PAL_RESULT_SUCCESS) {
        const char* error = palFormatResult(result);
        palLog(nullptr, "Failed to get adapter info: %s", error);
        palFree(nullptr, adapters);
        return false;
    }

    // create a device with the vulkan adapter
    PalGPUDevice* device = nullptr;
    PalGPUFeatures features = 0;

    // enable swapchain and maybe multi viewport if supported
    if (caps.features & PAL_GPU_FEATURE_SWAPCHAIN) {
        features = PAL_GPU_FEATURE_SWAPCHAIN;
    }

    if (caps.features & PAL_GPU_FEATURE_MULTI_VIEWPORT) {
        features |= PAL_GPU_FEATURE_MULTI_VIEWPORT;
    }

    result = palCreateGPUDevice(vulkanAdapter, features, &device);
    if (result != PAL_RESULT_SUCCESS) {
        const char* error = palFormatResult(result);
        palLog(nullptr, "Failed to create device: %s", error);
        return false;
    }

    // check if we support a graphics command queue
    if (!caps.maxGraphicsQueues) {
        palLog(
            nullptr, 
            "This Adapter (GPU) does not have any graphics command queues");
        return false;
    }

    // create a graphics command queue
    PalGPUCommandQueue* graphicsQueue = nullptr;
    result = palCreateGPUCommandQueue(
        device, 
        PAL_GPU_COMMAND_QUEUE_TYPE_GRAPHICS, 
        &graphicsQueue);

    if (result != PAL_RESULT_SUCCESS) {
        const char* error = palFormatResult(result);
        palLog(nullptr, "Failed to create graphics command queue: %s", error);
        return false;
    }

    // destroy the command queue
    palDestroyGPUCommandQueue(graphicsQueue);

    // destroy the device
    palDestroyGPUDevice(device);

    // shutdown the graphics system
    palShutdownGraphics();
    
    return true;
}