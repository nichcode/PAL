
#include "pal/pal_graphics.h"
#include "tests.h"

// a simple custom backend
// for simplicity we are not going to add that much functionality
// to it

typedef struct {
    PalGPUAdapterInfo adapterInfo;
    // add more fields if needed
} CustomGPUAdapter;

typedef struct {
    // we just have only two adapters for simplicity
    CustomGPUAdapter adapters[2];
} CustomGPUBackend;

static CustomGPUBackend s_CustomGPU;

// setup our state which we will use
// PAL does not need this call so we set it up
// before adding the backend to PAL
// you might need a shutdown function for the backend after PAL has shutdown
// if there is cleanup to do
static void initCustomBackend() {
    CustomGPUAdapter* adapter = &s_CustomGPU.adapters[0];
    adapter->adapterInfo.features = 0;
    adapter->adapterInfo.commands = 0;

    adapter->adapterInfo.apiType = PAL_GPU_API_D3D9;
    adapter->adapterInfo.debugLayerSupported = true;
    adapter->adapterInfo.type = PAL_GPU_TYPE_INTEGRATED;
    adapter->adapterInfo.version = 9; // combine into a single value
    
    // PAL needs it in bytes
    Uint64 byte = 1024 * 1024 * 1024;
    adapter->adapterInfo.totalMemory = byte * 4; // 4 GB

    strcpy(adapter->adapterInfo.versionString, "10_1");
    strcpy(adapter->adapterInfo.name, "Intel Arc A580");

    adapter->adapterInfo.commands |= PAL_GPU_COMMAND_GRAPHICS;
    adapter->adapterInfo.features |= PAL_GPU_FEATURE_RAY_TRACING;

    // second adapter
    adapter = &s_CustomGPU.adapters[1];
    adapter->adapterInfo.features = 0;
    adapter->adapterInfo.commands = 0;

    adapter->adapterInfo.apiType = PAL_GPU_API_OPENGL;
    adapter->adapterInfo.debugLayerSupported = true;
    adapter->adapterInfo.type = PAL_GPU_TYPE_DISCRETE;
    adapter->adapterInfo.version = 4; // combine into a single value
    
    // PAL needs it in bytes
    adapter->adapterInfo.totalMemory = byte * 6; // 6 GB

    strcpy(adapter->adapterInfo.versionString, "4.4");
    strcpy(adapter->adapterInfo.name, "AMD Radeon RX 7700 XT");

    adapter->adapterInfo.commands |= PAL_GPU_COMMAND_GRAPHICS;
    adapter->adapterInfo.commands |= PAL_GPU_COMMAND_COMPUTE;
    adapter->adapterInfo.features |= PAL_GPU_FEATURE_RAY_TRACING;
    adapter->adapterInfo.features |= PAL_GPU_FEATURE_MESH_SHADER;
}

static PalResult PAL_CALL customEnumerateGPUAdapters(
    Int32* count, 
    PalGPUAdapter** outAdapters)
{
    if (outAdapters) {
        for (int i = 0; i < 2 && i < *count; i++) {
            PalGPUAdapter* adapter = (PalGPUAdapter*)&s_CustomGPU.adapters[i];
            outAdapters[i] = adapter;
        }

    } else {
        *count = 2;
    }

    return PAL_RESULT_SUCCESS;
}

static PalResult PAL_CALL customGetGPUAdapterInfo(
    PalGPUAdapter* adapter,
    PalGPUAdapterInfo* info)
{
    for (int i = 0; i < 2; i++) {
        PalGPUAdapter* custom = (PalGPUAdapter*)&s_CustomGPU.adapters[i];
        if (custom == adapter) {
            // make a copy
            PalGPUAdapterInfo* gpuInfo = &s_CustomGPU.adapters[i].adapterInfo;
            info->apiType = gpuInfo->apiType;
            info->debugLayerSupported = gpuInfo->debugLayerSupported;
            info->totalMemory = gpuInfo->totalMemory;
            info->type = gpuInfo->type;
            info->version = gpuInfo->version;
            info->features = gpuInfo->features;
            info->commands = gpuInfo->commands;

            strcpy(info->name, gpuInfo->name);
            strcpy(info->versionString, gpuInfo->versionString);

            return PAL_RESULT_SUCCESS;
        }
    }
}

static PalGPUBackend s_CustomBackend = {
    .enumerateGPUAdapters = customEnumerateGPUAdapters,
    .getGPUAdapterInfo = customGetGPUAdapterInfo
};

bool customGraphicsBackendTest()
{
    palLog(nullptr, "");
    palLog(nullptr, "===========================================");
    palLog(nullptr, "Custom Graphics Backend Test");
    palLog(nullptr, "===========================================");
    palLog(nullptr, "");

    // do any initializtion before adding the backen to PAL
    initCustomBackend();

    // add the backend to the graphics system
    PalResult result = palAddGPUBackend(&s_CustomBackend);
    if (result != PAL_RESULT_SUCCESS) {
        const char* error = palFormatResult(result);
        palLog(nullptr, "Failed to add backend: %s", error);
        return false;
    }

    // initialize the video system
    result = palInitGraphics(nullptr);
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

            case PAL_GPU_API_OPENGL: {
                apiTypeString = "OpenGL";
                break;
            }

            case PAL_GPU_API_GLES: {
                apiTypeString = "GLes";
                break;
            }

            case PAL_GPU_API_D3D11: {
                apiTypeString = "D3D11";
                break;
            }

            case PAL_GPU_API_D3D9: {
                apiTypeString = "D3D9";
                break;
            }

            case PAL_GPU_API_PPM: {
                apiTypeString = "PPM";
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

        // commands
        palLog(nullptr, " Supported Commands:");
        if (info.commands & PAL_GPU_COMMAND_COMPUTE) {
            palLog(nullptr, "  Compute");
        }

        if (info.commands & PAL_GPU_COMMAND_GRAPHICS) {
            palLog(nullptr, "  Graphics");
        }

        if (info.commands & PAL_GPU_COMMAND_TRANSFER) {
            palLog(nullptr, "  Transfer");
        }

        // features
        palLog(nullptr, " Supported Features:");
        if (info.features & PAL_GPU_FEATURE_RAY_TRACING) {
            palLog(nullptr, "  Ray tracing");
        }

        if (info.features & PAL_GPU_FEATURE_MESH_SHADER) {
            palLog(nullptr, "  Mesh shading");
        }

        if (info.features & PAL_GPU_FEATURE_DESCRIPTOR_INDEXING) {
            palLog(nullptr, "  Descriptor indexing");
        }

        if (info.features & PAL_GPU_FEATURE_VARIABLE_RATE_SHADING) {
            palLog(nullptr, " Variable Rate Shading");
        }

        palLog(nullptr, "");
    }

    // shutdown the graphics system
    palShutdownGraphics();

    palFree(nullptr, adapters);
    
    return true;
}