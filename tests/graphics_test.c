
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

    // get information about all the adapters
    PalAdapterInfo info;
    PalAdapterCapabilities caps;
    for (Int32 i = 0; i < count; i++) {
        PalAdapter* adapter = adapters[i];
        result = palGetAdapterInfo(adapter, &info);
        if (result != PAL_RESULT_SUCCESS) {
            const char* error = palFormatResult(result);
            palLog(nullptr, "Failed to get adapter info: %s", error);
            palFree(nullptr, adapters);
            return false;
        }

        result = palGetAdapterCapabilities(adapter, &caps);
        if (result != PAL_RESULT_SUCCESS) {
            const char* error = palFormatResult(result);
            palLog(nullptr, "Failed to get adapter capabilities: %s", error);
            palFree(nullptr, adapters);
            return false;
        }

        Uint32 vramGb = info.vram / (1024.0 * 1024.0 * 1024.0);
        Uint32 sharedMemGb = info.sharedMemory / (1024.0 * 1024.0 * 1024.0);

        palLog(nullptr, "GPU Name: %s", info.name);
        palLog(nullptr, " Vendor Id: %d", info.vendorId);
        palLog(nullptr, " Device Id: %d", info.deviceId);
        palLog(nullptr, " Vram %dGB", vramGb);
        palLog(nullptr, " Shared Memory %dGB", sharedMemGb);
        palLog(nullptr, " API Version: %s", info.versionString);

        const char* typeString;
        switch (info.type) {
            case PAL_ADAPTER_TYPE_INTEGRATED: {
                typeString = "Integrated";
                break;
            }

            case PAL_ADAPTER_TYPE_VIRTUAL: {
                typeString = "Virtual";
                break;
            }

            case PAL_ADAPTER_TYPE_DISCRETE: {
                typeString = "Discrete";
                break;
            }

            case PAL_ADAPTER_TYPE_CPU: {
                typeString = "CPU";
                break;
            }
        }
        palLog(nullptr, " Type: %s", typeString);

        const char* apiTypeString;
        switch (info.apiType) {
            case PAL_ADAPTER_API_TYPE_D3D12: {
                apiTypeString = "D3D12";
                break;
            }

            case PAL_ADAPTER_API_TYPE_VULKAN: {
                apiTypeString = "Vulkan";
                break;
            }

            case PAL_ADAPTER_API_TYPE_METAL: {
                apiTypeString = "Metal";
                break;
            }

            case PAL_ADAPTER_API_TYPE_OPENGL: {
                apiTypeString = "OpenGL";
                break;
            }

            case PAL_ADAPTER_API_TYPE_GLES: {
                apiTypeString = "GLes";
                break;
            }

            case PAL_ADAPTER_API_TYPE_D3D11: {
                apiTypeString = "D3D11";
                break;
            }

            case PAL_ADAPTER_API_TYPE_D3D9: {
                apiTypeString = "D3D9";
                break;
            }

            case PAL_ADAPTER_API_TYPE_PPM: {
                apiTypeString = "PPM";
                break;
            }
        }
        palLog(nullptr, " API Type: %s", apiTypeString);

        // shader formats
        palLog(nullptr, " Supported Shader Formats:");
        if (info.shaderFormats & PAL_SHADER_FORMAT_SPIRV) {
            palLog(nullptr, "  SPIRV");
        }

        if (info.shaderFormats & PAL_SHADER_FORMAT_DXIL) {
            palLog(nullptr, "  DXIL");
        }

        if (info.shaderFormats & PAL_SHADER_FORMAT_DXBC) {
            palLog(nullptr, "  DXBC");
        }

        if (info.shaderFormats & PAL_SHADER_FORMAT_GLSL) {
            palLog(nullptr, "  GLSL");
        }

        if (info.shaderFormats & PAL_SHADER_FORMAT_MSL) {
            palLog(nullptr, "  MSL");
        }

        if (info.shaderFormats & PAL_SHADER_FORMAT_PPM) {
            palLog(nullptr, "  PPM");
        }

        const char* boolToString;
        if (caps.debugLayerSupported) {
            boolToString = "True";
        } else {
            boolToString = "False";
        }

        palLog(nullptr, " Debug Layer: %s", boolToString);

        // clang-format off

        Uint32 uniformBufferSize = caps.maxUniformBufferSize / 1024;
        Uint32 storageBufferSize = caps.maxStorageBufferSize / 1024;
        Uint32 pushConstantSize = caps.maxStorageBufferSize / 1024;

        palLog(nullptr, " Max compute queues: %d", caps.maxComputeQueues);
        palLog(nullptr, " Max graphics queues: %d", caps.maxGraphicsQueues);
        palLog(nullptr, " Max copy queues: %d", caps.maxCopyQueues);

        palLog(nullptr, " Max image width: %d", caps.maxImageWidth);
        palLog(nullptr, " Max image height: %d", caps.maxImageHeight);
        palLog(nullptr, " Max image depth: %d", caps.maxImageDepth);
        palLog(nullptr, " Max image array layers: %d", caps.maxImageArrayLayers);
        palLog(nullptr, " Max image mip levels: %d", caps.maxImageMipLevels);

        palLog(nullptr, " Max color samples: %d", caps.maxColorSamples);
        palLog(nullptr, " Max depth samples: %d", caps.maxDepthSamples);
        palLog(nullptr, " Max color attachment: %d", caps.maxColorAttachments);
        palLog(nullptr, " Max multi views: %d", caps.maxMultiViews);   
        palLog(nullptr, " Max viewports: %d", caps.maxViewports);
        palLog(nullptr, " Max samplers: %d", caps.maxSamplers);
        palLog(nullptr, " Max uniform buffer size: %dKB", uniformBufferSize);
        palLog(nullptr, " Max storage buffer size: %dKB", storageBufferSize); 
        palLog(nullptr, " Max push constant size: %dKB", pushConstantSize); 

        // clang-format on

        // features
        palLog(nullptr, " Supported Features:");
        if (caps.features & PAL_ADAPTER_FEATURE_SAMPLER_ANISOTROPY) {
            palLog(nullptr, "  Sampler Anisotropy");
        }

        if (caps.features & PAL_ADAPTER_FEATURE_SAMPLE_RATE_SHADING) {
            palLog(nullptr, "  Sample rate shading");
        }

        if (caps.features & PAL_ADAPTER_FEATURE_MULTI_VIEWPORT) {
            palLog(nullptr, "  Multi viewport");
        }

        if (caps.features & PAL_ADAPTER_FEATURE_TIMELINE_SEMAPHORE) {
            palLog(nullptr, "  Timeline Semaphore");
        }

        if (caps.features & PAL_ADAPTER_FEATURE_TESSELLATION_SHADER) {
            palLog(nullptr, "  Tesselation Shader");
        }

        if (caps.features & PAL_ADAPTER_FEATURE_GEOMETRY_SHADER) {
            palLog(nullptr, "  Geometry shader");
        }

        if (caps.features & PAL_ADAPTER_FEATURE_SHADER_FLOAT16) {
            palLog(nullptr, "  Shader float16");
        }

        if (caps.features & PAL_ADAPTER_FEATURE_SHADER_FLOAT64) {
            palLog(nullptr, "  Shader float64");
        }

        if (caps.features & PAL_ADAPTER_FEATURE_SHADER_INT16) {
            palLog(nullptr, "  Shader int16");
        }
        if (caps.features & PAL_ADAPTER_FEATURE_SHADER_INT64) {
            palLog(nullptr, "  Shader int64");
        }

        if (caps.features & PAL_ADAPTER_FEATURE_DYNAMIC_RENDERING) {
            palLog(nullptr, "  Dynamic rendering");
        }

        if (caps.features & PAL_ADAPTER_FEATURE_RAY_TRACING) {
            palLog(nullptr, "  Ray tracing");
        }

        if (caps.features & PAL_ADAPTER_FEATURE_MESH_SHADER) {
            palLog(nullptr, "  Mesh shader");
        }

        if (caps.features & PAL_ADAPTER_FEATURE_VARIABLE_RATE_SHADING) {
            palLog(nullptr, "  Variable rate rendering");
        }

        if (caps.features & PAL_ADAPTER_FEATURE_DESCRIPTOR_INDEXING) {
            palLog(nullptr, "  Descriptor indexing");
        }

        if (caps.features & PAL_ADAPTER_FEATURE_SWAPCHAIN) {
            palLog(nullptr, "  Swapchain");
        }

        if (caps.features & PAL_ADAPTER_FEATURE_MULTI_VIEW) {
            palLog(nullptr, "  Multiview");
        }
        
        palLog(nullptr, "");
    }

    // shutdown the graphics system
    palShutdownGraphics();

    palFree(nullptr, adapters);
    
    return true;
}