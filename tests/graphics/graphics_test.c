
#include "pal/pal_graphics.h"
#include "tests.h"

const char* shaderTargetToString(PalShaderTarget target) 
{
    switch (target) {
        case PAL_SHADER_TARGET_SPIRV_1_0:
            return "1.0";

        case PAL_SHADER_TARGET_SPIRV_1_1:
            return "1.1";
            
        case PAL_SHADER_TARGET_SPIRV_1_2:
            return "1.2";

        case PAL_SHADER_TARGET_SPIRV_1_3:
            return "1.3";

        case PAL_SHADER_TARGET_SPIRV_1_4:
            return "1.4";

        case PAL_SHADER_TARGET_SPIRV_1_5:
            return "1.5";

        case PAL_SHADER_TARGET_SPIRV_1_6:
            return "1.6";

        case PAL_SHADER_TARGET_DXIL_5_1:
            return "5.1";

        case PAL_SHADER_TARGET_DXIL_6_0:
            return "6.0";

        case PAL_SHADER_TARGET_DXIL_6_1:
            return "6.1";

        case PAL_SHADER_TARGET_DXIL_6_2:
           return "6.2";

        case PAL_SHADER_TARGET_DXIL_6_3:
            return "6.3";

        case PAL_SHADER_TARGET_DXIL_6_4:
            return "6.4";

        case PAL_SHADER_TARGET_DXIL_6_5:
            return "6.5";

        case PAL_SHADER_TARGET_DXIL_6_6:
            return "6.6";

        case PAL_SHADER_TARGET_DXIL_6_7:
            return "6.7";
    }
    return nullptr;
}

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
    PalAdapterCapabilities caps;
    PalAdapterFeatures features = 0;
    for (Int32 i = 0; i < count; i++) {
        PalAdapter* adapter = adapters[i];
        result = palGetAdapterInfo(adapter, &info);
        if (result != PAL_RESULT_SUCCESS) {
            const char* error = palFormatResult(result);
            palLog(nullptr, "Failed to get adapter information: %s", error);
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

        // create a device
        features = palGetAdapterFeatures(adapter);
        PalDevice* device = nullptr;
        PalAdapterFeatures deviceFeatures = 0;
        if (features & PAL_ADAPTER_FEATURE_SAMPLER_ANISOTROPY) {
            deviceFeatures |= PAL_ADAPTER_FEATURE_SAMPLER_ANISOTROPY;
        }

        if (features & PAL_ADAPTER_FEATURE_DEPTH_STENCIL_RESOLVE) {
            deviceFeatures |= PAL_ADAPTER_FEATURE_DEPTH_STENCIL_RESOLVE;
        }

        if (features & PAL_ADAPTER_FEATURE_FRAGMENT_SHADING_RATE) {
            deviceFeatures |= PAL_ADAPTER_FEATURE_FRAGMENT_SHADING_RATE;
        }

        if (features & PAL_ADAPTER_FEATURE_MESH_SHADER) {
            deviceFeatures |= PAL_ADAPTER_FEATURE_MESH_SHADER;
        }

        if (features & PAL_ADAPTER_FEATURE_RAY_TRACING) {
            deviceFeatures |= PAL_ADAPTER_FEATURE_RAY_TRACING;
        }

        if (features & PAL_ADAPTER_FEATURE_DESCRIPTOR_INDEXING) {
            deviceFeatures |= PAL_ADAPTER_FEATURE_DESCRIPTOR_INDEXING;
        }

        result = palCreateDevice(adapter, deviceFeatures, &device);
        if (result != PAL_RESULT_SUCCESS) {
            const char* error = palFormatResult(result);
            palLog(nullptr, "Failed to create device: %s", error);
            palFree(nullptr, adapters);
            return false;
        }
        
        Uint32 vramMb = info.vram / (1024.0 * 1024.0);
        Uint32 sharedMemMb = info.sharedMemory / (1024.0 * 1024.0);

        palLog(nullptr, "GPU Name: %s", info.name);
        palLog(nullptr, " Backend Name: %s", info.backendName);
        palLog(nullptr, " Vendor Id: %d", info.vendorId);
        palLog(nullptr, " Device Id: %d", info.deviceId);
        palLog(nullptr, " Vram %dMB", vramMb);
        palLog(nullptr, " Shared Memory %dMB", sharedMemMb);

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
        }
        palLog(nullptr, " API Type: %s", apiTypeString);

        palLog(nullptr, "");
        palLog(nullptr, " Capabilities:");
        palLog(nullptr, "  Max compute queue: %d", caps.maxComputeQueues);
        palLog(nullptr, "  Max graphics queue: %d", caps.maxGraphicsQueues);
        palLog(nullptr, "  Max copy queue: %d", caps.maxCopyQueues);

        palLog(nullptr, "  Max image width: %d", caps.maxImageWidth);
        palLog(nullptr, "  Max image height: %d", caps.maxImageHeight);
        palLog(nullptr, "  Max image depth: %d", caps.maxImageDepth);
        palLog(nullptr, "  Max image array layers: %d", caps.maxImageArrayLayers);
        palLog(nullptr, "  Max image mip levels: %d", caps.maxImageMipLevels);

        palLog(nullptr, "  Max color attachments: %d", caps.maxColorAttachments);
        palLog(nullptr, "  Max uniform buffer size: %d Bytes", caps.maxUniformBufferSize);
        palLog(nullptr, "  Max storage buffer size: %d Bytes", caps.maxStorageBufferSize);
        palLog(nullptr, "  Max push constant size: %d Bytes", caps.maxPushConstantSize);

        palLog(nullptr, "  Max vertex layouts: %d", caps.maxVertexLayouts);
        palLog(nullptr, "  Max vertex attributes: %d", caps.maxVertexAttributes);
        palLog(nullptr, "  Max tessellation patch point: %d", caps.maxTessellationPatchPoint);

        // clang-format off
        palLog(nullptr, "  Max per stage descriptor sampled images: %d", caps.maxPerStageDescriptorSampledImages);
        palLog(nullptr, "  Max descriptor set sampled images: %d", caps.maxDescriptorSetSampledImages);
        palLog(nullptr, "  Max per stage descriptor storage images: %d", caps.maxPerStageDescriptorStorageImages);
        palLog(nullptr, "  Max descriptor set storage images: %d", caps.maxDescriptorSetStorageImages);
        
        palLog(nullptr, "  Max per stage descriptor samplers: %d", caps.maxPerStageDescriptorSamplers);
        palLog(nullptr, "  Max descriptor set samplers: %d", caps.maxDescriptorSetSamplers);
        palLog(nullptr, "  Max per stage descriptor storage buffers: %d", caps.maxPerStageDescriptorStorageBuffers);
        palLog(nullptr, "  Max descriptor set storage buffers: %d", caps.maxDescriptorSetStorageBuffers);

        palLog(nullptr, "  Max per stage descriptor uniform buffers: %d", caps.maxPerStageDescriptorUniformBuffers);
        palLog(nullptr, "  Max descriptor set uniform buffers: %d", caps.maxDescriptorSetUniformBuffers);
        palLog(nullptr, "  Max bound descriptor sets: %d", caps.maxBoundDescriptorSets);
        // clang-format on

        palLog(nullptr, "  Max compute invocations: %d", caps.maxComputeWorkGroupInvocations);
        palLog(nullptr, "  Max compute work group count[0]: %d", caps.maxComputeWorkGroupCount[0]);
        palLog(nullptr, "  Max compute work group count[1]: %d", caps.maxComputeWorkGroupCount[1]);
        palLog(nullptr, "  Max compute work group count[2]: %d", caps.maxComputeWorkGroupCount[2]);
        palLog(nullptr, "  Max compute work group size[0]: %d", caps.maxComputeWorkGroupSize[0]);
        palLog(nullptr, "  Max compute work group size[1]: %d", caps.maxComputeWorkGroupSize[1]);
        palLog(nullptr, "  Max compute work group size[2]: %d", caps.maxComputeWorkGroupSize[2]);

        // shader formats
        PalShaderTarget target;
        palLog(nullptr, "");
        palLog(nullptr, " Supported Shader Formats:");
        if (info.shaderFormats & PAL_SHADER_FORMAT_SPIRV) {
            palLog(nullptr, "  SPIRV");

            target = palGetHighestSupportedShaderTarget(adapter, PAL_SHADER_FORMAT_SPIRV);
            palLog(nullptr, "  Highest Spirv Target %s:", shaderTargetToString(target));
        }

        if (info.shaderFormats & PAL_SHADER_FORMAT_DXIL) {
            palLog(nullptr, "  DXIL");

            target = palGetHighestSupportedShaderTarget(adapter, PAL_SHADER_FORMAT_DXIL);
            palLog(nullptr, "  Highest Dxil Target %s:", shaderTargetToString(target));
        }

        // features
        palLog(nullptr, "");
        palLog(nullptr, " Supported Features:");
        if (features & PAL_ADAPTER_FEATURE_SAMPLER_ANISOTROPY) {
            palLog(nullptr, "  Sampler Anisotropy");

            PalSamplerAnisotropyCapabilities tmp;
            result = palQuerySamplerAnisotropyCapabilities(device, &tmp);
            if (result != PAL_RESULT_SUCCESS) {
                const char* error = palFormatResult(result);
                palLog(nullptr, "Failed to get sampler anisotropy capabilities: %s", error);
                return false;
            }

            palLog(nullptr, "   Max anisotropy: %d", tmp.maxAnisotropy);

            palLog(nullptr, "");
        }

        if (features & PAL_ADAPTER_FEATURE_SAMPLE_RATE_SHADING) {
            palLog(nullptr, "  Sample rate shading");
        }

        if (features & PAL_ADAPTER_FEATURE_MULTI_VIEWPORT) {
            palLog(nullptr, "  Multi viewport");

            PalMultiViewportCapabilities tmp;
            result = palQueryMultiViewportCapabilities(device, &tmp);
            if (result != PAL_RESULT_SUCCESS) {
                const char* error = palFormatResult(result);
                palLog(nullptr, "Failed to get multi viewport capabilities: %s", error);
                return false;
            }

            palLog(nullptr, "   Max viewports: %d", tmp.maxViewports);

            palLog(nullptr, "");
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

            PalRayTracingCapabilities tmp;
            result = palQueryRayTracingCapabilities(device, &tmp);
            if (result != PAL_RESULT_SUCCESS) {
                const char* error = palFormatResult(result);
                palLog(nullptr, "Failed to get ray tracing capabilities: %s", error);
                return false;
            }

            palLog(nullptr, "   Max recursion depth: %d", tmp.maxRecursionDepth);
            palLog(nullptr, "   Max hit attribute size: %d Bytes", tmp.maxHitAttributeSize);
            palLog(nullptr, "   Max instance count: %d", tmp.maxInstanceCount);
            palLog(nullptr, "   Max primitive count: %d", tmp.maxPrimitiveCount);
            palLog(nullptr, "   Max geometry count: %d", tmp.maxGeometryCount);
            palLog(nullptr, "   Max payload size: %d Bytes", tmp.maxPayloadSize);
            palLog(nullptr, "   Max dispatch invocations: %d", tmp.maxDispatchInvocations);

            palLog(nullptr, "");
        }

        if (features & PAL_ADAPTER_FEATURE_MESH_SHADER) {
            palLog(nullptr, "  Mesh and task shader");

            PalMeshShaderCapabilities tmp;
            result = palQueryMeshShaderCapabilities(device, &tmp);
            if (result != PAL_RESULT_SUCCESS) {
                const char* error = palFormatResult(result);
                palLog(nullptr, "Failed to get mesh shader capabilities: %s", error);
                return false;
            }

            palLog(nullptr, "   Max mesh output primitives: %d", tmp.maxMeshOutputPrimitives);
            palLog(nullptr, "   Max mesh output vertices: %d", tmp.maxMeshOutputVertices);
            palLog(nullptr, "   Max mesh invocations: %d", tmp.maxMeshWorkGroupInvocations);
            palLog(nullptr, "   Max task invocations: %d", tmp.maxTaskWorkGroupInvocations);

            palLog(nullptr, "   Max mesh work group count[0]: %d", tmp.maxMeshWorkGroupCount[0]);
            palLog(nullptr, "   Max mesh work group count[1]: %d", tmp.maxMeshWorkGroupCount[1]);
            palLog(nullptr, "   Max mesh work group count[2]: %d", tmp.maxMeshWorkGroupCount[2]);
            palLog(nullptr, "   Max task work group count[0]: %d", tmp.maxTaskWorkGroupCount[0]);
            palLog(nullptr, "   Max task work group count[1]: %d", tmp.maxTaskWorkGroupCount[1]);
            palLog(nullptr, "   Max task work group count[2]: %d", tmp.maxTaskWorkGroupCount[2]);

            palLog(nullptr, "");
        }

        if (features & PAL_ADAPTER_FEATURE_FRAGMENT_SHADING_RATE) {
            palLog(nullptr, "  Fragment shading rate");

            PalFragmentShadingRateCapabilities tmp;
            result = palQueryFragmentShadingRateCapabilities(device, &tmp);
            if (result != PAL_RESULT_SUCCESS) {
                const char* error = palFormatResult(result);
                palLog(nullptr, "Failed to get FSR capabilities: %s", error);
                return false;
            }

            palLog(nullptr, "   Min texel width: %d", tmp.minTexelWidth);
            palLog(nullptr, "   Min texel height: %d", tmp.maxTexelWidth);
            palLog(nullptr, "   Max texel width: %d", tmp.minTexelHeight);
            palLog(nullptr, "   Max texel height: %d", tmp.maxTexelHeight);

            palLog(nullptr, "   Support Shading Rates:");
            if (tmp.shadingRates[PAL_FRAGMENT_SHADING_RATE_1X1]) {
                palLog(nullptr, "    1 X 1");
            }

            if (tmp.shadingRates[PAL_FRAGMENT_SHADING_RATE_1X2]) {
                palLog(nullptr, "    1 X 2");
            }

            if (tmp.shadingRates[PAL_FRAGMENT_SHADING_RATE_2X1]) {
                palLog(nullptr, "    2 X 1");
            }

            if (tmp.shadingRates[PAL_FRAGMENT_SHADING_RATE_2X2]) {
                palLog(nullptr, "    2 X 2");
            }

            if (tmp.shadingRates[PAL_FRAGMENT_SHADING_RATE_2X4]) {
                palLog(nullptr, "    2 X 4");
            }

            if (tmp.shadingRates[PAL_FRAGMENT_SHADING_RATE_4X2]) {
                palLog(nullptr, "    4 X 2");

            }

            if (tmp.shadingRates[PAL_FRAGMENT_SHADING_RATE_4X4]) {
                palLog(nullptr, "    4 X 4");
            }

            palLog(nullptr, "   Support Combiner operations:");
            if (tmp.combinerOps[PAL_FRAGMENT_SHADING_RATE_COMBINER_OP_KEEP]) {
                palLog(nullptr, "    Keep");
            }

            if (tmp.combinerOps[PAL_FRAGMENT_SHADING_RATE_COMBINER_OP_REPLACE]) {
                palLog(nullptr, "    Replace");
            }

            if (tmp.combinerOps[PAL_FRAGMENT_SHADING_RATE_COMBINER_OP_MIN]) {
                palLog(nullptr, "    Min");
            }

            if (tmp.combinerOps[PAL_FRAGMENT_SHADING_RATE_COMBINER_OP_MAX]) {
                palLog(nullptr, "    Max");
            }

            if (tmp.combinerOps[PAL_FRAGMENT_SHADING_RATE_COMBINER_OP_MUL]) {
                palLog(nullptr, "    Mul");
            }

            palLog(nullptr, "");
        }

        if (features & PAL_ADAPTER_FEATURE_DESCRIPTOR_INDEXING) {
            palLog(nullptr, "  Descriptor indexing");

            PalDescriptorIndexingCapabilities tmp;
            result = palQueryDescriptorIndexingCapabilities(device, &tmp);
            if (result != PAL_RESULT_SUCCESS) {
                const char* error = palFormatResult(result);
                palLog(nullptr, "Failed to get descriptor indexing capabilities: %s", error);
                return false;
            }

            if (tmp.bindlessSampledImages) {
                palLog(nullptr, "   Bindless sampled images: True");
            } else {
                palLog(nullptr, "   Bindless sampled images: False");
            }

            if (tmp.bindlessStorageImages) {
                palLog(nullptr, "   Bindless storage images: True");
            } else {
                palLog(nullptr, "   Bindless storage images: False");
            }

            if (tmp.bindlessSamplers) {
                palLog(nullptr, "   Bindless samplers: True");
            } else {
                palLog(nullptr, "   Bindless samplers: False");
            }

            if (tmp.bindlessStorageBuffers) {
                palLog(nullptr, "   Bindless storage buffers: True");
            } else {
                palLog(nullptr, "   Bindless storage buffers: False");
            }

            if (tmp.bindlessUniformBuffers) {
                palLog(nullptr, "   Bindless uniform buffers: True");
            } else {
                palLog(nullptr, "   Bindless uniform buffers: False");
            }

            // clang-format off
            palLog(nullptr, "   Max per stage bindless descriptor sampled images: %d", tmp.maxPerStageBindlessDescriptorSampledImages);
            palLog(nullptr, "   Max descriptor set bindless sampled images: %d", tmp.maxDescriptorSetBindlessSampledImages);
            palLog(nullptr, "   Max per stage binding descriptor storage images: %d", tmp.maxPerStageBindlessDescriptorStorageImages);
            palLog(nullptr, "   Max descriptor set bindless storage images: %d", tmp.maxDescriptorSetBindlessStorageImages);
            
            palLog(nullptr, "   Max per stage binding descriptor samplers: %d", tmp.maxPerStageBindlessDescriptorSamplers);
            palLog(nullptr, "   Max descriptor set bindless samplers: %d", tmp.maxDescriptorSetBindlessSamplers);
            palLog(nullptr, "   Max per stage binding descriptor storage buffers: %d", tmp.maxPerStageBindlessDescriptorStorageBuffers);
            palLog(nullptr, "   Max descriptor set bindless storage buffers: %d", tmp.maxDescriptorSetBindlessStorageBuffers);

            palLog(nullptr, "   Max per stage binding descriptor uniform buffers: %d", tmp.maxPerStageBindlessDescriptorUniformBuffers);
            palLog(nullptr, "   Max descriptor set bindless uniform buffers: %d", tmp.maxDescriptorSetBindlessUniformBuffers);
            // clang-format on

            palLog(nullptr, "");
        }

        if (features & PAL_ADAPTER_FEATURE_SWAPCHAIN) {
            palLog(nullptr, "  Swapchain");
        }

        if (features & PAL_ADAPTER_FEATURE_MULTI_VIEW) {
            palLog(nullptr, "  Multiview");

            PalMultiViewCapabilities tmp;
            result = palQueryMultiViewCapabilities(device, &tmp);
            if (result != PAL_RESULT_SUCCESS) {
                const char* error = palFormatResult(result);
                palLog(nullptr, "Failed to get multi view capabilities: %s", error);
                return false;
            }

            palLog(nullptr, "   Max multi views: %d", tmp.maxMultiViews);
            palLog(nullptr, "");
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

            PalDepthStencilCapabilities tmp;
            result = palQueryDepthStencilCapabilities(device, &tmp);
            if (result != PAL_RESULT_SUCCESS) {
                const char* error = palFormatResult(result);
                palLog(nullptr, "Failed to get depth stencil capabilities: %s", error);
                return false;
            }

            if (tmp.independentDepthStencilResolve) {
                palLog(nullptr, "   Independent depth stencil resource: True");
            } else {
                palLog(nullptr, "   Independent depth stencil resource: False");
            }

            palLog(nullptr, "   Supported Depth Resolve Modes:");
            if (tmp.depthResolveModes[PAL_RESOLVE_MODE_SAMPLE_ZERO]) {
                palLog(nullptr, "    Zero");
            }

            if (tmp.depthResolveModes[PAL_RESOLVE_MODE_AVERAGE]) {
                palLog(nullptr, "    Average");
            }

            if (tmp.depthResolveModes[PAL_RESOLVE_MODE_MIN]) {
                palLog(nullptr, "    Min");
            }

            if (tmp.depthResolveModes[PAL_RESOLVE_MODE_MAX]) {
                palLog(nullptr, "    Max");
            }

            palLog(nullptr, "   Supported Stencil Resolve Modes:");
            if (tmp.stencilResolveModes[PAL_RESOLVE_MODE_SAMPLE_ZERO]) {
                palLog(nullptr, "    Zero");
            }

            if (tmp.stencilResolveModes[PAL_RESOLVE_MODE_AVERAGE]) {
                palLog(nullptr, "    Average");
            }

            if (tmp.stencilResolveModes[PAL_RESOLVE_MODE_MIN]) {
                palLog(nullptr, "    Min");
            }

            if (tmp.stencilResolveModes[PAL_RESOLVE_MODE_MAX]) {
                palLog(nullptr, "    Max");
            }

            palLog(nullptr, "");
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
        palDestroyDevice(device);
    }

    // shutdown the graphics system
    palShutdownGraphics();

    palFree(nullptr, adapters);

    return true;
}
