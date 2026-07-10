
#include "pal/pal_graphics.h"
#include "tests.h"

PalBool graphicsTest()
{
    // initialize the graphics system
    PalResult result = palInitGraphics(nullptr, nullptr, 0, nullptr);
    if (result != PAL_RESULT_SUCCESS) {
        logResult(result, "Failed to initialize graphics");
        return PAL_FALSE;
    }

    // enumerate all available adapters
    uint32_t count = 0;
    result = palEnumerateAdapters(&count, nullptr);
    if (result != PAL_RESULT_SUCCESS) {
        logResult(result, "Failed to get query adapters");
        return PAL_FALSE;
    }

    if (count == 0) {
        palLog(nullptr, "No adapters found");
        return PAL_FALSE;
    }
    palLog(nullptr, "Adapter count: %u", count);

    // allocate an array of adapters or use a fixed array
    // Example: PalAdapter* adapters[12];
    PalAdapter** adapters = nullptr;
    adapters = palAllocate(nullptr, sizeof(PalAdapter*) * count, 0);
    if (!adapters) {
        palLog(nullptr, "Failed to allocate memory");
        return PAL_FALSE;
    }

    result = palEnumerateAdapters(&count, adapters);
    if (result != PAL_RESULT_SUCCESS) {
        logResult(result, "Failed to get query adapters");
        return PAL_FALSE;
    }

    // get information about all the adapters
    PalAdapterInfo info;
    PalAdapterCapabilities caps;
    PalAdapterFeatures features = 0;
    for (int32_t i = 0; i < count; i++) {
        PalAdapter* adapter = adapters[i];
        palGetAdapterInfo(adapter, &info);
        palGetAdapterCapabilities(adapter, &caps);

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
            logResult(result, "Failed to create device");
            palFree(nullptr, adapters);
            return PAL_FALSE;
        }
        
        uint32_t vramMb = (uint32_t)info.vram / (1024 * 1024);
        uint32_t sharedMemMb = (uint32_t)info.sharedMemory /(1024 * 1024);

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
        palLog(nullptr, "  Max compute queue: %u", caps.maxComputeQueues);
        palLog(nullptr, "  Max graphics queue: %u", caps.maxGraphicsQueues);
        palLog(nullptr, "  Max copy queue: %u", caps.maxCopyQueues);
        palLog(nullptr, "  Max color attachments: %u", caps.maxColorAttachments);
        palLog(nullptr, "  Max uniform buffer size: %u Bytes", caps.maxUniformBufferSize);

        palLog(nullptr, "  Max storage buffer size: %u Bytes", caps.maxStorageBufferSize);
        palLog(nullptr, "  Max push constant size: %u Bytes", caps.maxPushConstantSize);
        palLog(nullptr, "  Max vertex layouts: %u", caps.maxVertexLayouts);
        palLog(nullptr, "  Max vertex attributes: %u", caps.maxVertexAttributes);
        palLog(nullptr, "  Max tessellation patch point: %u", caps.maxTessellationPatchPoint);

        palLog(nullptr, "");
        palLog(nullptr, "  Viewport Capabilities:");
        palLog(nullptr, "   Max width: %u", caps.viewportCaps.maxWidth);
        palLog(nullptr, "   Max height: %u", caps.viewportCaps.maxHeight);
        palLog(nullptr, "   Min bounds range: %.1f", caps.viewportCaps.minBoundsRange);
        palLog(nullptr, "   Max bounds range: %.1f", caps.viewportCaps.maxBoundsRange);

        palLog(nullptr, "");
        palLog(nullptr, "  Image Capabilities:");
        palLog(nullptr, "   Max width: %u", caps.imageCaps.maxWidth);
        palLog(nullptr, "   Max height: %u", caps.imageCaps.maxHeight);
        palLog(nullptr, "   Max depth: %u", caps.imageCaps.maxDepth);
        palLog(nullptr, "   Max array layers: %u", caps.imageCaps.maxArrayLayers);
        palLog(nullptr, "   Max mip levels: %u", caps.imageCaps.maxMipLevels);

        palLog(nullptr, "");
        palLog(nullptr, "  Resource Capabilities:");
        PalResourceCapabilities* resourceCaps = &caps.resourceCaps;
       
        // clang-format off
        palLog(nullptr, "   Max per stage sampled images: %u", resourceCaps->maxPerStageSampledImages);
        palLog(nullptr, "   Max per set sampled images: %u", resourceCaps->maxPerSetSampledImages);
        palLog(nullptr, "   Max per stage storage images: %u", resourceCaps->maxPerStageStorageImages);
        palLog(nullptr, "   Max per set storage images: %u", resourceCaps->maxPerSetStorageImages);
        
        palLog(nullptr, "   Max per stage samplers: %u", resourceCaps->maxPerStageSamplers);
        palLog(nullptr, "   Max per set samplers: %u", resourceCaps->maxPerSetSamplers);
        palLog(nullptr, "   Max per stage storage buffers: %u", resourceCaps->maxPerStageStorageBuffers);
        palLog(nullptr, "   Max per set storage buffers: %u", resourceCaps->maxPerSetStorageBuffers);

        palLog(nullptr, "   Max per stage uniform buffers: %u", resourceCaps->maxPerStageUniformBuffers);
        palLog(nullptr, "   Max per set uniform buffers: %u", resourceCaps->maxPerSetUniformBuffers);
        palLog(nullptr, "   Max per stage acceleration structures: %u", resourceCaps->maxPerStageAccelerationStructure);
        palLog(nullptr, "   Max per set acceleration structures: %u", resourceCaps->maxPerSetAccelerationStructure);
        palLog(nullptr, "   Max bound sets: %u", resourceCaps->maxBoundSets);
        // clang-format on

        palLog(nullptr, "");
        palLog(nullptr, "  Compute Capabilities:");
        palLog(nullptr, "   Max invocations: %u", caps.computeCaps.maxWorkGroupInvocations);
        palLog(nullptr, "   Max work group count[0]: %u", caps.computeCaps.maxWorkGroupCount[0]);
        palLog(nullptr, "   Max work group count[1]: %u", caps.computeCaps.maxWorkGroupCount[1]);
        palLog(nullptr, "   Max work group count[2]: %u", caps.computeCaps.maxWorkGroupCount[2]);
        palLog(nullptr, "   Max work group size[0]: %u", caps.computeCaps.maxWorkGroupSize[0]);
        palLog(nullptr, "   Max work group size[1]: %u", caps.computeCaps.maxWorkGroupSize[1]);
        palLog(nullptr, "   Max work group size[2]: %u", caps.computeCaps.maxWorkGroupSize[2]);

        // shader formats
        uint32_t target;
        uint32_t targetMajor = 0;
        uint32_t targetMinor = 0;

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
            palQuerySamplerAnisotropyCapabilities(device, &tmp);

            palLog(nullptr, "   Max anisotropy: %u", tmp.maxAnisotropy);
            palLog(nullptr, "");
        }

        if (features & PAL_ADAPTER_FEATURE_MULTI_VIEWPORT) {
            palLog(nullptr, "  Multi viewport");

            PalMultiViewportCapabilities tmp;
            palQueryMultiViewportCapabilities(device, &tmp);

            palLog(nullptr, "   Max count: %u", tmp.maxCount);
            palLog(nullptr, "");
        }

        if (features & PAL_ADAPTER_FEATURE_RAY_TRACING) {
            palLog(nullptr, "  Ray tracing");

            PalRayTracingCapabilities tmp;
            palQueryRayTracingCapabilities(device, &tmp);

            palLog(nullptr, "   Max recursion depth: %u", tmp.maxRecursionDepth);
            palLog(nullptr, "   Max hit attribute size: %u Bytes", tmp.maxHitAttributeSize);
            palLog(nullptr, "   Max instance count: %u", tmp.maxInstanceCount);
            palLog(nullptr, "   Max primitive count: %u", tmp.maxPrimitiveCount);
            palLog(nullptr, "   Max geometry count: %u", tmp.maxGeometryCount);
            palLog(nullptr, "   Max payload size: %u Bytes", tmp.maxPayloadSize);
            palLog(nullptr, "   Max dispatch invocations: %u", tmp.maxDispatchInvocations);
            palLog(nullptr, "");
        }

        if (features & PAL_ADAPTER_FEATURE_MESH_SHADER) {
            palLog(nullptr, "  Mesh and task shader");

            PalMeshShaderCapabilities tmp;
            palQueryMeshShaderCapabilities(device, &tmp);

            palLog(nullptr, "   Max output primitives: %u", tmp.maxOutputPrimitives);
            palLog(nullptr, "   Max output vertices: %u", tmp.maxOutputVertices);
            palLog(nullptr, "   Max invocations: %u", tmp.maxWorkGroupInvocations);
            palLog(nullptr, "   Max task invocations: %u", tmp.maxTaskWorkGroupInvocations);

            palLog(nullptr, "   Max work group count[0]: %u", tmp.maxWorkGroupCount[0]);
            palLog(nullptr, "   Max work group count[1]: %u", tmp.maxWorkGroupCount[1]);
            palLog(nullptr, "   Max work group count[2]: %u", tmp.maxWorkGroupCount[2]);
            palLog(nullptr, "   Max task work group count[0]: %u", tmp.maxTaskWorkGroupCount[0]);
            palLog(nullptr, "   Max task work group count[1]: %u", tmp.maxTaskWorkGroupCount[1]);
            palLog(nullptr, "   Max task work group count[2]: %u", tmp.maxTaskWorkGroupCount[2]);
            palLog(nullptr, "");
        }

        if (features & PAL_ADAPTER_FEATURE_FRAGMENT_SHADING_RATE) {
            palLog(nullptr, "  Fragment shading rate");

            PalFragmentShadingRateCapabilities tmp;
            palQueryFragmentShadingRateCapabilities(device, &tmp);

            palLog(nullptr, "   Min texel width: %u", tmp.minTexelWidth);
            palLog(nullptr, "   Min texel height: %u", tmp.maxTexelWidth);
            palLog(nullptr, "   Max texel width: %u", tmp.minTexelHeight);
            palLog(nullptr, "   Max texel height: %u", tmp.maxTexelHeight);

            palLog(nullptr, "   Supported Shading Rates:");
            if (palIsSupported(tmp.supportedShadingRates, PAL_FRAGMENT_SHADING_RATE_1X1)) {
                palLog(nullptr, "    1 X 1");
            }

            if (palIsSupported(tmp.supportedShadingRates, PAL_FRAGMENT_SHADING_RATE_1X2)) {
                palLog(nullptr, "    1 X 2");
            }

            if (palIsSupported(tmp.supportedShadingRates, PAL_FRAGMENT_SHADING_RATE_2X1)) {
                palLog(nullptr, "    2 X 1");
            }

            if (palIsSupported(tmp.supportedShadingRates, PAL_FRAGMENT_SHADING_RATE_2X2)) {
                palLog(nullptr, "    2 X 2");
            }

            if (palIsSupported(tmp.supportedShadingRates, PAL_FRAGMENT_SHADING_RATE_2X4)) {
                palLog(nullptr, "    2 X 4");
            }

            if (palIsSupported(tmp.supportedShadingRates, PAL_FRAGMENT_SHADING_RATE_4X2)) {
                palLog(nullptr, "    4 X 2");

            }

            if (palIsSupported(tmp.supportedShadingRates, PAL_FRAGMENT_SHADING_RATE_4X4)) {
                palLog(nullptr, "    4 X 4");
            }

            palLog(nullptr, "   Supported Combiner operations:");
            // clang-format off
            if (palIsSupported(tmp.supportedCombinerOps, PAL_FRAGMENT_SHADING_RATE_COMBINER_OP_KEEP)) {
                palLog(nullptr, "    Keep");
            }

            if (palIsSupported(tmp.supportedCombinerOps, PAL_FRAGMENT_SHADING_RATE_COMBINER_OP_REPLACE)) {
                palLog(nullptr, "    Replace");
            }

            if (palIsSupported(tmp.supportedCombinerOps, PAL_FRAGMENT_SHADING_RATE_COMBINER_OP_MIN)) {
                palLog(nullptr, "    Min");
            }

            if (palIsSupported(tmp.supportedCombinerOps, PAL_FRAGMENT_SHADING_RATE_COMBINER_OP_MAX)) {
                palLog(nullptr, "    Max");
            }

            if (palIsSupported(tmp.supportedCombinerOps, PAL_FRAGMENT_SHADING_RATE_COMBINER_OP_MUL)) {
                palLog(nullptr, "    Mul");
            }
            // clang-format on

            palLog(nullptr, "");
        }

        if (features & PAL_ADAPTER_FEATURE_DESCRIPTOR_INDEXING) {
            palLog(nullptr, "  Descriptor indexing");

            PalDescriptorIndexingCapabilities tmp;
            palQueryDescriptorIndexingCapabilities(device, &tmp);

            palLog(nullptr, "   Supported flags:");
            if (tmp.flags & PAL_DESCRIPTOR_INDEXING_FLAG_UPDATE_AFTER_BIND) {
                palLog(nullptr, "    Update after bind: True");
            } else {
                palLog(nullptr, "    Update after bind: False");
            }

            if (tmp.flags & PAL_DESCRIPTOR_INDEXING_FLAG_PARTIALLY_BOUND) {
                palLog(nullptr, "    Partially bound: True");
            } else {
                palLog(nullptr, "    Partially bound: False");
            }

            if (tmp.flags & PAL_DESCRIPTOR_INDEXING_FLAG_NON_UNIFORM_INDEXING) {
                palLog(nullptr, "    Non uniform indexing: True");
            } else {
                palLog(nullptr, "    Non uniform indexing: False");
            }

            // clang-format off
            palLog(nullptr, "   Max per stage sampled images: %u", tmp.maxPerStageSampledImages);
            palLog(nullptr, "   Max per set sampled images: %u", tmp.maxPerSetSampledImages);
            palLog(nullptr, "   Max per stage storage images: %u", tmp.maxPerStageStorageImages);
            palLog(nullptr, "   Max per set storage images: %u", tmp.maxPerSetStorageImages);
            
            palLog(nullptr, "   Max per stage samplers: %u", tmp.maxPerStageSamplers);
            palLog(nullptr, "   Max per set samplers: %u", tmp.maxPerSetSamplers);
            palLog(nullptr, "   Max per stage storage buffers: %u", tmp.maxPerStageStorageBuffers);
            palLog(nullptr, "   Max per set storage buffers: %u", tmp.maxPerSetStorageBuffers);

            palLog(nullptr, "   Max per stage uniform buffers: %u", tmp.maxPerStageUniformBuffers);
            palLog(nullptr, "   Max per set uniform buffers: %u", tmp.maxPerSetUniformBuffers);
            palLog(nullptr, "   Max per stage acceleration structures: %u", tmp.maxPerStageAccelerationStructure);
            palLog(nullptr, "   Max per set acceleration structures: %u", tmp.maxPerSetAccelerationStructure);
            // clang-format on

            palLog(nullptr, "");
        }

        if (features & PAL_ADAPTER_FEATURE_MULTI_VIEW) {
            palLog(nullptr, "  Multiview");

            PalMultiViewCapabilities tmp;
            palQueryMultiViewCapabilities(device, &tmp);

            palLog(nullptr, "   Max view count: %u", tmp.maxViewCount);
            palLog(nullptr, "");
        }

        if (features & PAL_ADAPTER_FEATURE_DEPTH_STENCIL_RESOLVE) {
            palLog(nullptr, "  Depth stencil resolve");

            PalDepthStencilCapabilities tmp;
            palQueryDepthStencilCapabilities(device, &tmp);

            if (tmp.supportsIndependentResolve) {
                palLog(nullptr, "   Independent resolve: True");
            } else {
                palLog(nullptr, "   Independent resolve: False");
            }

            if (tmp.supportsIndependentResolveNone) {
                palLog(nullptr, "   Independent resolve none: True");
            } else {
                palLog(nullptr, "   Independent resolve none: False");
            }

            palLog(nullptr, "   Supported depth resolves modes:");
            if (palIsSupported(tmp.supportedDepthResolveModes, PAL_RESOLVE_MODE_SAMPLE_ZERO)) {
                palLog(nullptr, "    Zero");
            }

            if (palIsSupported(tmp.supportedDepthResolveModes, PAL_RESOLVE_MODE_AVERAGE)) {
                palLog(nullptr, "    Average");
            }

            if (palIsSupported(tmp.supportedDepthResolveModes, PAL_RESOLVE_MODE_MIN)) {
                palLog(nullptr, "    Min");
            }

            if (palIsSupported(tmp.supportedDepthResolveModes, PAL_RESOLVE_MODE_MAX)) {
                palLog(nullptr, "    Max");
            }

            palLog(nullptr, "   Supported stencil resolve modes:");
            if (palIsSupported(tmp.supportedStencilResolveModes, PAL_RESOLVE_MODE_SAMPLE_ZERO)) {
                palLog(nullptr, "    Zero");
            }

            if (palIsSupported(tmp.supportedStencilResolveModes, PAL_RESOLVE_MODE_AVERAGE)) {
                palLog(nullptr, "    Average");
            }

            if (palIsSupported(tmp.supportedStencilResolveModes, PAL_RESOLVE_MODE_MIN)) {
                palLog(nullptr, "    Min");
            }

            if (palIsSupported(tmp.supportedStencilResolveModes, PAL_RESOLVE_MODE_MAX)) {
                palLog(nullptr, "    Max");
            }

            palLog(nullptr, "");
        }

        if (features & PAL_ADAPTER_FEATURE_SWAPCHAIN) {
            palLog(nullptr, "  Swapchain");
        }

        if (features & PAL_ADAPTER_FEATURE_SAMPLE_RATE_SHADING) {
            palLog(nullptr, "  Sample rate shading");
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

        if (features & PAL_ADAPTER_FEATURE_NULL_DESCRIPTORS) {
            palLog(nullptr, "  Null descriptors");
        }

        if (features & PAL_ADAPTER_FEATURE_INDIRECT_RAY_TRACING) {
            palLog(nullptr, "  Indirect ray tracing");
        }

        if (features & PAL_ADAPTER_FEATURE_RAY_QUERY) {
            palLog(nullptr, "  Ray query");
        }

        palLog(nullptr, "");
        palDestroyDevice(device);
    }

    // shutdown the graphics system
    palShutdownGraphics();

    palFree(nullptr, adapters);

    return PAL_TRUE;
}
