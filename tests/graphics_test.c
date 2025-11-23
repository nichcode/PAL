
#include "pal/pal_graphics.h"
#include "tests.h"

bool graphicsTest()
{
    palLog(nullptr, "");
    palLog(nullptr, "===========================================");
    palLog(nullptr, "Graphics Test");
    palLog(nullptr, "===========================================");
    palLog(nullptr, "");

    // initialize the video system
    PalResult result = palInitGraphics(nullptr);
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
    for (Int32 i = 0; i < count; i++) {
        PalGPUAdapter* adapter = adapters[i];
        result = palGetGPUAdapterInfo(adapter, &info);
        if (result != PAL_RESULT_SUCCESS) {
            const char* error = palFormatResult(result);
            palLog(nullptr, "Failed to get adapter info: %s", error);
            palFree(nullptr, adapters);
            return false;
        }

        Uint32 memoryGb = info.totalMemory / (1024.0 * 1024.0 * 1024.0);
        palLog(nullptr, "GPU Name: %s", info.name);
        palLog(nullptr, " Total Memory %dGB", memoryGb);
        palLog(nullptr, " API Version: %s", info.versionString);

        const char* typeString;
        switch (info.type) {
            case PAL_GPU_TYPE_INTEGRATED: {
                typeString = "Integrated";
                break;
            }

            case PAL_GPU_TYPE_VIRTUAL: {
                typeString = "Virtual";
                break;
            }

            case PAL_GPU_TYPE_DISCRETE: {
                typeString = "Discrete";
                break;
            }

            case PAL_GPU_TYPE_CPU: {
                typeString = "CPU";
                break;
            }
        }
        palLog(nullptr, " Type: %s", typeString);

        const char* apiTypeString;
        switch (info.apiType) {
            case PAL_GPU_API_D3D12: {
                apiTypeString = "D3D12";
                break;
            }

            case PAL_GPU_API_VULKAN: {
                apiTypeString = "Vulkan";
                break;
            }

            case PAL_GPU_API_METAL: {
                apiTypeString = "Metal";
                break;
            }

            case PAL_GPU_API_CUSTOM: {
                apiTypeString = "Custom";
                break;
            }
        }
        palLog(nullptr, " API Type: %s", apiTypeString);

        const char* boolToString;
        if (info.debugLayerSupported) {
            boolToString = "True";
        } else {
            boolToString = "False";
        }
        palLog(nullptr, " Debug Layer: %s", boolToString);
    }

    // shutdown the graphics system
    palShutdownGraphics();

    palFree(nullptr, adapters);
    
    return true;
}