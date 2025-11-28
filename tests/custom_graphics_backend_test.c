
#include "pal/pal_graphics.h"
#include "tests.h"

// a simple custom backend
// for simplicity we are not going to add that much functionality
// to it
typedef struct {
    PalGPUAdapterInfo adapterInfo;
    PalGPUAdapterCapabilities caps;
    // add more fields if needed
} CustomGPUAdapter;

typedef struct {
    CustomGPUAdapter* adapter;
    // add more fields if needed
} CustomGPUDevice;

typedef struct {
    CustomGPUDevice* device;
    PalGPUCommandQueueType type;
    // add more fields if needed
} CustomGPUCommandQueue;

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
    // PAL needs the memory it in bytes
    Uint64 byte = 1024 * 1024 * 1024;

    CustomGPUAdapter* adapter = &s_CustomGPU.adapters[0];
    adapter->adapterInfo.apiType = PAL_GPU_API_TYPE_D3D9;
    adapter->adapterInfo.type = PAL_GPU_TYPE_INTEGRATED;
    adapter->adapterInfo.shaderFormats = PAL_GPU_SHADER_FORMAT_DXBC;
    adapter->adapterInfo.totalMemory = byte * 4; // 4 GB
    strcpy(adapter->adapterInfo.versionString, "10_1");
    strcpy(adapter->adapterInfo.name, "Intel Arc A580");

    adapter->caps.debugLayerSupported = true;
    adapter->caps.features = PAL_GPU_FEATURE_RAY_TRACING;
    adapter->caps.features |= PAL_GPU_FEATURE_SWAPCHAIN;
    adapter->caps.maxComputeQueues = 1;
    adapter->caps.maxGraphicsQueues = 1;
    adapter->caps.maxCopyQueues = 2;

    // second adapter
    adapter = &s_CustomGPU.adapters[1];
    adapter->adapterInfo.apiType = PAL_GPU_API_TYPE_OPENGL;
    adapter->adapterInfo.type = PAL_GPU_TYPE_DISCRETE;
    adapter->adapterInfo.shaderFormats = PAL_GPU_SHADER_FORMAT_SPIRV;
    adapter->adapterInfo.shaderFormats |= PAL_GPU_SHADER_FORMAT_GLSL;
    adapter->adapterInfo.totalMemory = byte * 6; // 6 GB
    strcpy(adapter->adapterInfo.versionString, "4.4");
    strcpy(adapter->adapterInfo.name, "AMD Radeon RX 7700 XT");

    adapter->caps.debugLayerSupported = true;
    adapter->caps.features = PAL_GPU_FEATURE_RAY_TRACING;
    adapter->caps.features |= PAL_GPU_FEATURE_SWAPCHAIN;
    adapter->caps.features |= PAL_GPU_FEATURE_MESH_SHADER;
    adapter->caps.maxComputeQueues = 1;
    adapter->caps.maxGraphicsQueues = 4;
    adapter->caps.maxCopyQueues = 4;
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
            info->totalMemory = gpuInfo->totalMemory;
            info->type = gpuInfo->type;
            info->shaderFormats = gpuInfo->shaderFormats;

            strcpy(info->name, gpuInfo->name);
            strcpy(info->versionString, gpuInfo->versionString);
            return PAL_RESULT_SUCCESS;
        }
    }

    return PAL_RESULT_INVALID_GPU_ADAPTER;
}

static PalResult PAL_CALL customGetGPUAdapterCapabilities(
    PalGPUAdapter* adapter,
    PalGPUAdapterCapabilities* caps)
{
    for (int i = 0; i < 2; i++) {
        PalGPUAdapter* custom = (PalGPUAdapter*)&s_CustomGPU.adapters[i];
        if (custom == adapter) {
            // make a copy
            *caps = s_CustomGPU.adapters[i].caps;
            return PAL_RESULT_SUCCESS;
        }
    }

    return PAL_RESULT_INVALID_GPU_ADAPTER;
}

PalResult PAL_CALL customCreateGPUDevice(
    PalGPUAdapter* adapter,
    PalGPUFeatures features,
    PalGPUDevice** outDevice)
{
    // very simple GPU device. Just an allocation
    // no need for checks eithe, PAL does that already for you
    // use any allocator you want but its best to use the same allocator
    // passed to the graphics system

    CustomGPUDevice* device = nullptr;
    device = palAllocate(nullptr, sizeof(CustomGPUDevice), 0);
    if (!device) {
        return PAL_RESULT_OUT_OF_MEMORY;
    }

    // if your backend has more than one adapter
    // check and create the device with that adapter
    for (int i = 0; i < 2; i++) {
        PalGPUAdapter* custom = (PalGPUAdapter*)&s_CustomGPU.adapters[i];
        if (adapter == custom) {
            device->adapter = &s_CustomGPU.adapters[i];
            // additional info for the adapter
            break;
        }
    }

    *outDevice = (PalGPUDevice*)device;
    return PAL_RESULT_SUCCESS;
}

void PAL_CALL customDestroyGPUDevice(PalGPUDevice* device)
{
    CustomGPUDevice* customDevice = (CustomGPUDevice*)device;
    palFree(nullptr, device);
}

static PalResult PAL_CALL customCreateGPUCommandQueue(
    PalGPUDevice* device,
    PalGPUCommandQueueType type,
    PalGPUCommandQueue** outQueue)
{
    // implement
}

void PAL_CALL customDestroyGPUCommandQueue(PalGPUCommandQueue* queue)
{
    // implement
}

static PalGPUBackend s_CustomBackend = {
    .enumerateGPUAdapters = customEnumerateGPUAdapters,
    .getGPUAdapterInfo = customGetGPUAdapterInfo,
    .getGPUAdapterCapabilities = customGetGPUAdapterCapabilities,
    .createGPUDevice = customCreateGPUDevice,
    .destroyGPUDevice = customDestroyGPUDevice,
    .createGPUCommandQueue = customCreateGPUCommandQueue,
    .destroyGPUCommandQueue = customDestroyGPUCommandQueue
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

    // get information about all the adapters
    PalGPUAdapterInfo info;
    PalGPUAdapter* d3d9Adapter = nullptr;

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
            case PAL_GPU_API_TYPE_D3D12: {
                apiTypeString = "D3D12";
                break;
            }

            case PAL_GPU_API_TYPE_VULKAN: {
                apiTypeString = "Vulkan";
                break;
            }

            case PAL_GPU_API_TYPE_METAL: {
                apiTypeString = "Metal";
                break;
            }

            case PAL_GPU_API_TYPE_OPENGL: {
                apiTypeString = "OpenGL";
                break;
            }

            case PAL_GPU_API_TYPE_GLES: {
                apiTypeString = "GLes";
                break;
            }

            case PAL_GPU_API_TYPE_D3D11: {
                apiTypeString = "D3D11";
                break;
            }

            case PAL_GPU_API_TYPE_D3D9: {
                apiTypeString = "D3D9";
                d3d9Adapter = adapter;
                break;
            }

            case PAL_GPU_API_TYPE_PPM: {
                apiTypeString = "PPM";
                break;
            }
        }
        palLog(nullptr, " API Type: %s", apiTypeString);

        // shader formats
        palLog(nullptr, " Supported Shader Formats:");
        if (info.shaderFormats & PAL_GPU_SHADER_FORMAT_SPIRV) {
            palLog(nullptr, "  SPIRV");
        }

        if (info.shaderFormats & PAL_GPU_SHADER_FORMAT_DXIL) {
            palLog(nullptr, "  DXIL");
        }

        if (info.shaderFormats & PAL_GPU_SHADER_FORMAT_DXBC) {
            palLog(nullptr, "  DXBC");
        }

        if (info.shaderFormats & PAL_GPU_SHADER_FORMAT_GLSL) {
            palLog(nullptr, "  GLSL");
        }

        if (info.shaderFormats & PAL_GPU_SHADER_FORMAT_MSL) {
            palLog(nullptr, "  MSL");
        }

        if (info.shaderFormats & PAL_GPU_SHADER_FORMAT_PPM) {
            palLog(nullptr, "  PPM");
        }

        palLog(nullptr, "");
    }

    // shutdown the graphics system
    palShutdownGraphics();

    palFree(nullptr, adapters);
    
    return true;
}