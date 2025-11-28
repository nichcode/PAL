
#include "pal/pal_graphics.h"
#include "tests.h"

bool graphicsTest()
{
    palLog(nullptr, "");
    palLog(nullptr, "===========================================");
    palLog(nullptr, "Graphics Test");
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
                break;
            }

            case PAL_GPU_API_TYPE_PPM: {
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

        // command queue
        Int32 maxComputeQueues = info.commandQueuesInfo.maxComputeQueues;
        Int32 maxGraphicsQueues = info.commandQueuesInfo.maxGraphicsQueues;
        Int32 maxTransferQueues = info.commandQueuesInfo.maxTransferQueues;
        palLog(nullptr, " Max compute command queues: %d", maxComputeQueues);
        palLog(nullptr, " Max graphics command queues: %d", maxGraphicsQueues);
        palLog(nullptr, " Max transfer command queues: %d", maxTransferQueues);

        // features
        palLog(nullptr, " Supported Features:");
        if (info.features & PAL_GPU_FEATURE_SAMPLER_ANISOTROPY) {
            palLog(nullptr, "  Sampler Anisotropy");
        }

        if (info.features & PAL_GPU_FEATURE_SAMPLE_RATE_SHADING) {
            palLog(nullptr, "  Sample rate shading");
        }

        if (info.features & PAL_GPU_FEATURE_MULTI_VIEWPORT) {
            palLog(nullptr, "  Multi viewport");
        }

        if (info.features & PAL_GPU_FEATURE_TIMELINE_SEMAPHORE) {
            palLog(nullptr, "  Timeline Semaphore");
        }

        if (info.features & PAL_GPU_FEATURE_TESSELLATION_SHADER) {
            palLog(nullptr, "  Tesselation Shader");
        }

        if (info.features & PAL_GPU_FEATURE_GEOMETRY_SHADER) {
            palLog(nullptr, "  Geometry shader");
        }

        if (info.features & PAL_GPU_FEATURE_SHADER_FLOAT16) {
            palLog(nullptr, "  Shader float16");
        }

        if (info.features & PAL_GPU_FEATURE_SHADER_FLOAT64) {
            palLog(nullptr, "  Shader float64");
        }

        if (info.features & PAL_GPU_FEATURE_SHADER_INT16) {
            palLog(nullptr, "  Shader int16");
        }
        if (info.features & PAL_GPU_FEATURE_SHADER_INT64) {
            palLog(nullptr, "  Shader int64");
        }

        if (info.features & PAL_GPU_FEATURE_DYNAMIC_RENDERING) {
            palLog(nullptr, "  Dynamic rendering");
        }

        if (info.features & PAL_GPU_FEATURE_RAY_TRACING) {
            palLog(nullptr, "  Ray tracing");
        }

        if (info.features & PAL_GPU_FEATURE_MESH_SHADER) {
            palLog(nullptr, "  Mesh shader");
        }

        if (info.features & PAL_GPU_FEATURE_VARIABLE_RATE_SHADING) {
            palLog(nullptr, "  Variable rate rendering");
        }

        if (info.features & PAL_GPU_FEATURE_DESCRIPTOR_INDEXING) {
            palLog(nullptr, "  Descriptor indexing");
        }

        if (info.features & PAL_GPU_FEATURE_SWAPCHAIN) {
            palLog(nullptr, "  Swapchain");
        }

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