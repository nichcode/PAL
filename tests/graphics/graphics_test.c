
#include "pal/pal_graphics.h"
#include "tests.h"

bool graphicsTest()
{
    // initialize the graphics system
    PalResult result = palInitGraphics(nullptr, nullptr);
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
    PalAdapterFeatures features = 0;
    for (Int32 i = 0; i < count; i++) {
        PalAdapter* adapter = adapters[i];
        result = palGetAdapterInfo(adapter, &info);
        if (result != PAL_RESULT_SUCCESS) {
            const char* error = palFormatResult(result);
            palLog(nullptr, "Failed to get adapter info: %s", error);
            palFree(nullptr, adapters);
            return false;
        }

        features = palGetAdapterFeatures(adapter);
        Uint32 vramMb = info.vram / (1024.0 * 1024.0);
        Uint32 sharedMemMb = info.sharedMemory / (1024.0 * 1024.0);

        palLog(nullptr, "GPU Name: %s", info.name);
        palLog(nullptr, " Backend Name: %s", info.backendName);
        palLog(nullptr, " Vendor Id: %d", info.vendorId);
        palLog(nullptr, " Device Id: %d", info.deviceId);
        palLog(nullptr, " Vram %dMB", vramMb);
        palLog(nullptr, " Shared Memory %dMB", sharedMemMb);
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
        palLog(nullptr, "");
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

        // features
        palLog(nullptr, "");
        palLog(nullptr, " Supported Features:");
        if (features & PAL_ADAPTER_FEATURE_SAMPLER_ANISOTROPY) {
            palLog(nullptr, "  Sampler Anisotropy");
        }

        if (features & PAL_ADAPTER_FEATURE_SAMPLE_RATE_SHADING) {
            palLog(nullptr, "  Sample rate shading");
        }

        if (features & PAL_ADAPTER_FEATURE_MULTI_VIEWPORT) {
            palLog(nullptr, "  Multi viewport");
        }

        if (features & PAL_ADAPTER_FEATURE_TIMELINE_SEMAPHORE) {
            palLog(nullptr, "  Timeline Semaphore");
        }

        if (features & PAL_ADAPTER_FEATURE_TESSELLATION_SHADER) {
            palLog(nullptr, "  Tesselation Shader");
        }

        if (features & PAL_ADAPTER_FEATURE_GEOMETRY_SHADER) {
            palLog(nullptr, "  Geometry shader");
        }

        if (features & PAL_ADAPTER_FEATURE_COMPUTE_SHADER) {
            palLog(nullptr, "  Compute shader");
        }

        if (features & PAL_ADAPTER_FEATURE_SHADER_FLOAT16) {
            palLog(nullptr, "  Shader float16");
        }

        if (features & PAL_ADAPTER_FEATURE_SHADER_FLOAT64) {
            palLog(nullptr, "  Shader float64");
        }

        if (features & PAL_ADAPTER_FEATURE_SHADER_INT16) {
            palLog(nullptr, "  Shader int16");
        }

        if (features & PAL_ADAPTER_FEATURE_SHADER_INT64) {
            palLog(nullptr, "  Shader int64");
        }

        if (features & PAL_ADAPTER_FEATURE_RAY_TRACING) {
            palLog(nullptr, "  Ray tracing");
        }

        if (features & PAL_ADAPTER_FEATURE_MESH_SHADER) {
            palLog(nullptr, "  Mesh and task shader");
        }

        if (features & PAL_ADAPTER_FEATURE_FRAGMENT_SHADING_RATE) {
            palLog(nullptr, "  Fragment shading rate");
        }

        if (features & PAL_ADAPTER_FEATURE_DESCRIPTOR_INDEXING) {
            palLog(nullptr, "  Descriptor indexing");
        }

        if (features & PAL_ADAPTER_FEATURE_SWAPCHAIN) {
            palLog(nullptr, "  Swapchain");
        }

        if (features & PAL_ADAPTER_FEATURE_MULTI_VIEW) {
            palLog(nullptr, "  Multiview");
        }

        if (features & PAL_ADAPTER_FEATURE_IMAGE_VIEW_CUBE_ARRAY) {
            palLog(nullptr, "  Image view type Cube array");
        }

        if (features & PAL_ADAPTER_FEATURE_FENCE_RESET) {
            palLog(nullptr, "  Resetting fence");
        }

        if (features & PAL_ADAPTER_FEATURE_POLYGON_MODE_LINE) {
            palLog(nullptr, "  Polygon mode line");
        }

        if (features & PAL_ADAPTER_FEATURE_DYNAMIC_CULL_MODE) {
            palLog(nullptr, "  Dynamic cull mode");
        }

        if (features & PAL_ADAPTER_FEATURE_DYNAMIC_FRONT_FACE) {
            palLog(nullptr, "  Dynamic front face");
        }

        if (features & PAL_ADAPTER_FEATURE_DYNAMIC_PRIMITIVE_TOPOLOGY) {
            palLog(nullptr, "  Dynamic primitive topology");
        }

        if (features & PAL_ADAPTER_FEATURE_DYNAMIC_DEPTH_TEST_ENABLE) {
            palLog(nullptr, "  Dynamic depth test enable");
        }

        if (features & PAL_ADAPTER_FEATURE_DYNAMIC_DEPTH_WRITE_ENABLE) {
            palLog(nullptr, "  Dynamic depth write enable");
        }

        if (features & PAL_ADAPTER_FEATURE_DYNAMIC_STENCIL_OP) {
            palLog(nullptr, "  Dynamic stencil op");
        }

        if (features & PAL_ADAPTER_FEATURE_DEPTH_STENCIL_RESOLVE) {
            palLog(nullptr, "  Depth stencil resolve");
        }

        if (features & PAL_ADAPTER_FEATURE_FRAGMENT_SHADING_RATE_ATTACHMENT) {
            palLog(nullptr, "  Fragment shading rate attachment");
        }

        if (features & PAL_ADAPTER_FEATURE_BUFFER_DEVICE_ADDRESS) {
            palLog(nullptr, "  Buffer device address");
        }

        if (features & PAL_ADAPTER_FEATURE_INDIRECT_DRAW) {
            palLog(nullptr, "  Indirect draw");
        }

        if (features & PAL_ADAPTER_FEATURE_INDIRECT_DRAW_COUNT) {
            palLog(nullptr, "  Indirect draw count");
        }

        if (features & PAL_ADAPTER_FEATURE_INDIRECT_DRAW_MESH) {
            palLog(nullptr, "  Indirect mesh draw");
        }

        if (features & PAL_ADAPTER_FEATURE_INDIRECT_DRAW_MESH_COUNT) {
            palLog(nullptr, "  Indirect mesh draw count");
        }

        if (features & PAL_ADAPTER_FEATURE_DISPATCH_BASE) {
            palLog(nullptr, "  Dispatch base");
        }

        palLog(nullptr, "");
    }

    // shutdown the graphics system
    palShutdownGraphics();

    palFree(nullptr, adapters);

    return true;
}
