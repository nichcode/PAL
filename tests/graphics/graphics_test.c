
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
    palLog(nullptr, "Adapter count: %u", count);

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
        palLog(nullptr, " Vendor Id: %u", info.vendorId);
        palLog(nullptr, " Device Id: %u", info.deviceId);
        palLog(nullptr, " Vram %u MB", vramMb);
        palLog(nullptr, " Shared Memory %u MB", sharedMemMb);

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
        if (caps.sampledImageDynamicArrayIndexing) {
            palLog(nullptr, "  Sampled image dynamic array indexing: True");

        } else {
            palLog(nullptr, "  Sampled image dynamic array indexing: False");
        }

        if (caps.storageImageDynamicArrayIndexing) {
            palLog(nullptr, "  Storage image dynamic array indexing: True");

        } else {
            palLog(nullptr, "  Storage image dynamic array indexing: False");
        }

        if (caps.storageBufferDynamicArrayIndexing) {
            palLog(nullptr, "  Storage buffer dynamic array indexing: True");

        } else {
            palLog(nullptr, "  Storage buffer dynamic array indexing: False");
        }

        if (caps.uniformBufferDynamicArrayIndexing) {
            palLog(nullptr, "  Uniform buffer dynamic array indexing: True");

        } else {
            palLog(nullptr, "  Uniform buffer dynamic array indexing: False");
        }

        palLog(nullptr, "  Max compute queue: %u", caps.maxComputeQueues);
        palLog(nullptr, "  Max graphics queue: %u", caps.maxGraphicsQueues);
        palLog(nullptr, "  Max copy queue: %u", caps.maxCopyQueues);

        palLog(nullptr, "  Max image width: %u", caps.maxImageWidth);
        palLog(nullptr, "  Max image height: %u", caps.maxImageHeight);
        palLog(nullptr, "  Max image depth: %u", caps.maxImageDepth);
        palLog(nullptr, "  Max image array layers: %u", caps.maxImageArrayLayers);
        palLog(nullptr, "  Max image mip levels: %u", caps.maxImageMipLevels);

        palLog(nullptr, "  Max color attachments: %u", caps.maxColorAttachments);
        palLog(nullptr, "  Max uniform buffer size: %u Bytes", caps.maxUniformBufferSize);
        palLog(nullptr, "  Max storage buffer size: %u Bytes", caps.maxStorageBufferSize);
        palLog(nullptr, "  Max push constant size: %u Bytes", caps.maxPushConstantSize);

        palLog(nullptr, "  Max vertex layouts: %u", caps.maxVertexLayouts);
        palLog(nullptr, "  Max vertex attributes: %u", caps.maxVertexAttributes);
        palLog(nullptr, "  Max tessellation patch point: %u", caps.maxTessellationPatchPoint);

        // clang-format off
        palLog(nullptr, "  Max per stage descriptor sampled images: %u", caps.maxPerStageDescriptorSampledImages);
        palLog(nullptr, "  Max descriptor set sampled images: %u", caps.maxDescriptorSetSampledImages);
        palLog(nullptr, "  Max per stage descriptor storage images: %u", caps.maxPerStageDescriptorStorageImages);
        palLog(nullptr, "  Max descriptor set storage images: %u", caps.maxDescriptorSetStorageImages);
        
        palLog(nullptr, "  Max per stage descriptor samplers: %u", caps.maxPerStageDescriptorSamplers);
        palLog(nullptr, "  Max descriptor set samplers: %u", caps.maxDescriptorSetSamplers);
        palLog(nullptr, "  Max per stage descriptor storage buffers: %u", caps.maxPerStageDescriptorStorageBuffers);
        palLog(nullptr, "  Max descriptor set storage buffers: %u", caps.maxDescriptorSetStorageBuffers);

        palLog(nullptr, "  Max per stage descriptor uniform buffers: %u", caps.maxPerStageDescriptorUniformBuffers);
        palLog(nullptr, "  Max descriptor set uniform buffers: %u", caps.maxDescriptorSetUniformBuffers);
        palLog(nullptr, "  Max bound descriptor sets: %u", caps.maxBoundDescriptorSets);
        // clang-format on

        palLog(nullptr, "  Max compute invocations: %u", caps.maxComputeWorkGroupInvocations);
        palLog(nullptr, "  Max compute work group count[0]: %u", caps.maxComputeWorkGroupCount[0]);
        palLog(nullptr, "  Max compute work group count[1]: %u", caps.maxComputeWorkGroupCount[1]);
        palLog(nullptr, "  Max compute work group count[2]: %u", caps.maxComputeWorkGroupCount[2]);
        palLog(nullptr, "  Max compute work group size[0]: %u", caps.maxComputeWorkGroupSize[0]);
        palLog(nullptr, "  Max compute work group size[1]: %u", caps.maxComputeWorkGroupSize[1]);
        palLog(nullptr, "  Max compute work group size[2]: %u", caps.maxComputeWorkGroupSize[2]);

        // shader formats
        Uint32 target;
        Uint32 targetMajor = 0;
        Uint32 targetMinor = 0;

        palLog(nullptr, "");
        palLog(nullptr, " Supported Shader Formats:");
        if (info.shaderFormats & PAL_SHADER_FORMAT_SPIRV) {
            palLog(nullptr, "  SPIRV");
            target = palGetHighestSupportedShaderTarget(adapter, PAL_SHADER_FORMAT_SPIRV);
            targetMajor = PAL_SHADER_TARGET_MAJOR(target);
            targetMinor = PAL_SHADER_TARGET_MINOR(target);

            palLog(nullptr, "   Highest Spirv Target: %u.%u", targetMajor, targetMinor);
            palLog(nullptr, "");
        }

        if (info.shaderFormats & PAL_SHADER_FORMAT_DXBC) {
            palLog(nullptr, "  DXBC");
            target = palGetHighestSupportedShaderTarget(adapter, PAL_SHADER_FORMAT_DXBC);
            targetMajor = PAL_SHADER_TARGET_MAJOR(target);
            targetMinor = PAL_SHADER_TARGET_MINOR(target);

            palLog(nullptr, "   Highest Dxbc Target: %u.%u", targetMajor, targetMinor);
            palLog(nullptr, "");
        }

        if (info.shaderFormats & PAL_SHADER_FORMAT_DXIL) {
            palLog(nullptr, "  DXIL");
            target = palGetHighestSupportedShaderTarget(adapter, PAL_SHADER_FORMAT_DXIL);
            targetMajor = PAL_SHADER_TARGET_MAJOR(target);
            targetMinor = PAL_SHADER_TARGET_MINOR(target);

            palLog(nullptr, "   Highest Dxil Target: %u.%u", targetMajor, targetMinor);
            palLog(nullptr, "");
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

            palLog(nullptr, "   Max anisotropy: %u", tmp.maxAnisotropy);

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

            palLog(nullptr, "   Max viewports: %u", tmp.maxViewports);

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

            palLog(nullptr, "   Max recursion depth: %u", tmp.maxRecursionDepth);
            palLog(nullptr, "   Max hit attribute size: %u Bytes", tmp.maxHitAttributeSize);
            palLog(nullptr, "   Max instance count: %u", tmp.maxInstanceCount);
            palLog(nullptr, "   Max primitive count: %u", tmp.maxPrimitiveCount);
            palLog(nullptr, "   Max geometry count: %u", tmp.maxGeometryCount);
            palLog(nullptr, "   Max payload size: %u Bytes", tmp.maxPayloadSize);
            palLog(nullptr, "   Max dispatch invocations: %u", tmp.maxDispatchInvocations);
            palLog(nullptr, "   Max descriptor set acceleration structures: %u", tmp.maxDescriptorSetAccelerationStructures);
            palLog(nullptr, "   Max descriptor set bindless acceleration structure: %u", tmp.maxDescriptorSetBindlessAccelerationStructures);

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

            palLog(nullptr, "   Max mesh output primitives: %u", tmp.maxMeshOutputPrimitives);
            palLog(nullptr, "   Max mesh output vertices: %u", tmp.maxMeshOutputVertices);
            palLog(nullptr, "   Max mesh invocations: %u", tmp.maxMeshWorkGroupInvocations);
            palLog(nullptr, "   Max task invocations: %u", tmp.maxTaskWorkGroupInvocations);

            palLog(nullptr, "   Max mesh work group count[0]: %u", tmp.maxMeshWorkGroupCount[0]);
            palLog(nullptr, "   Max mesh work group count[1]: %u", tmp.maxMeshWorkGroupCount[1]);
            palLog(nullptr, "   Max mesh work group count[2]: %u", tmp.maxMeshWorkGroupCount[2]);
            palLog(nullptr, "   Max task work group count[0]: %u", tmp.maxTaskWorkGroupCount[0]);
            palLog(nullptr, "   Max task work group count[1]: %u", tmp.maxTaskWorkGroupCount[1]);
            palLog(nullptr, "   Max task work group count[2]: %u", tmp.maxTaskWorkGroupCount[2]);

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

            palLog(nullptr, "   Min texel width: %u", tmp.minTexelWidth);
            palLog(nullptr, "   Min texel height: %u", tmp.maxTexelWidth);
            palLog(nullptr, "   Max texel width: %u", tmp.minTexelHeight);
            palLog(nullptr, "   Max texel height: %u", tmp.maxTexelHeight);

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

            if (tmp.runtimeDescriptorArray) {
                palLog(nullptr, "   Runtime descriptor arrays: True");

            } else {
                palLog(nullptr, "   Runtime descriptor arrays: False");
            }

            if (tmp.variableDescriptorCount) {
                palLog(nullptr, "   Variable descriptor count: True");

            } else {
                palLog(nullptr, "   Variable descriptor count: False");
            }

            if (tmp.partiallyBoundDescriptors) {
                palLog(nullptr, "   Partially bound descriptors: True");

            } else {
                palLog(nullptr, "   Partially bound descriptors: False");
            }

            if (tmp.sampledImageNonUniformIndexing) {
                palLog(nullptr, "   Sampled image non uniform indexing: True");

            } else {
                palLog(nullptr, "   Sampled image non uniform indexing: False");
            }

            if (tmp.sampledImageUpdateAfterBind) {
                palLog(nullptr, "   Sampled image update after bind: True");
                
            } else {
                palLog(nullptr, "   Sampled image update after bind: False");
            }

            if (tmp.storageImageNonUniformIndexing) {
                palLog(nullptr, "   Storage image non uniform indexing: True");

            } else {
                palLog(nullptr, "   Storage image non uniform indexing: False");
            }

            if (tmp.storageImageUpdateAfterBind) {
                palLog(nullptr, "   Storage image update after bind: True");
                
            } else {
                palLog(nullptr, "   Storage image update after bind: False");
            }

            if (tmp.storageBufferNonUniformIndexing) {
                palLog(nullptr, "   Storage buffer non uniform indexing: True");

            } else {
                palLog(nullptr, "   Storage buffer non uniform indexing: False");
            }

            if (tmp.storageBufferUpdateAfterBind) {
                palLog(nullptr, "   Storage buffer update after bind: True");
                
            } else {
                palLog(nullptr, "   Storage buffer update after bind: False");
            }

            if (tmp.uniformBufferNonUniformIndexing) {
                palLog(nullptr, "   Uniform buffer non uniform indexing: True");

            } else {
                palLog(nullptr, "   Uniform buffer non uniform indexing: False");
            }

            if (tmp.uniformBufferUpdateAfterBind) {
                palLog(nullptr, "   Uniform buffer update after bind: True");
                
            } else {
                palLog(nullptr, "   Uniform buffer update after bind: False");
            }

            // clang-format off
            palLog(nullptr, "   Max per stage bindless descriptor sampled images: %u", tmp.maxPerStageBindlessDescriptorSampledImages);
            palLog(nullptr, "   Max descriptor set bindless sampled images: %u", tmp.maxDescriptorSetBindlessSampledImages);
            palLog(nullptr, "   Max per stage binding descriptor storage images: %u", tmp.maxPerStageBindlessDescriptorStorageImages);
            palLog(nullptr, "   Max descriptor set bindless storage images: %u", tmp.maxDescriptorSetBindlessStorageImages);
            
            palLog(nullptr, "   Max per stage binding descriptor samplers: %u", tmp.maxPerStageBindlessDescriptorSamplers);
            palLog(nullptr, "   Max descriptor set bindless samplers: %u", tmp.maxDescriptorSetBindlessSamplers);
            palLog(nullptr, "   Max per stage binding descriptor storage buffers: %u", tmp.maxPerStageBindlessDescriptorStorageBuffers);
            palLog(nullptr, "   Max descriptor set bindless storage buffers: %u", tmp.maxDescriptorSetBindlessStorageBuffers);

            palLog(nullptr, "   Max per stage binding descriptor uniform buffers: %u", tmp.maxPerStageBindlessDescriptorUniformBuffers);
            palLog(nullptr, "   Max descriptor set bindless uniform buffers: %u", tmp.maxDescriptorSetBindlessUniformBuffers);
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

            palLog(nullptr, "   Max multi views: %u", tmp.maxMultiViews);
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
