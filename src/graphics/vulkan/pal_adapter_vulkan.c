/**
    PAL - Prime Abstraction Layer
    Copyright (C) 2025
    Licensed under the Zlib license. See LICENSE file in root.
 */

#if PAL_HAS_VULKAN_BACKEND
#include "pal_vulkan.h"

static PalSampleCount samplesFromVk(VkSampleCountFlags count)
{
    if (count & VK_SAMPLE_COUNT_2_BIT) {
        return PAL_SAMPLE_COUNT_2;

    } else if (count & VK_SAMPLE_COUNT_4_BIT) {
        return PAL_SAMPLE_COUNT_4;

    } else if (count & VK_SAMPLE_COUNT_8_BIT) {
        return PAL_SAMPLE_COUNT_8;

    } else if (count & VK_SAMPLE_COUNT_16_BIT) {
        return PAL_SAMPLE_COUNT_16;

    } else if (count & VK_SAMPLE_COUNT_32_BIT) {
        return PAL_SAMPLE_COUNT_32;

    } else if (count & VK_SAMPLE_COUNT_64_BIT) {
        return PAL_SAMPLE_COUNT_64;
    }

    return PAL_SAMPLE_COUNT_1;
}

static PalImageUsages imageUsageFromVk(VkFormatFeatureFlags flags)
{
    PalImageUsages usages = 0;
    if (flags & VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT) {
        usages |= PAL_IMAGE_USAGE_COLOR_ATTACHEMENT;
    }

    if (flags & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT) {
        usages |= PAL_IMAGE_USAGE_DEPTH_ATTACHEMENT;
    }

    if (flags & VK_FORMAT_FEATURE_TRANSFER_SRC_BIT) {
        usages |= PAL_IMAGE_USAGE_TRANSFER_SRC;
    }

    if (flags & VK_FORMAT_FEATURE_TRANSFER_DST_BIT) {
        usages |= PAL_IMAGE_USAGE_TRANSFER_DST;
    }

    if (flags & VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT) {
        usages |= VK_IMAGE_USAGE_STORAGE_BIT;
    }

    if (flags & VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT) {
        usages |= PAL_IMAGE_USAGE_SAMPLED;
    }

    return usages;
}

PalResult PAL_CALL enumerateAdaptersVk(
    uint32_t* count,
    PalAdapter** outAdapters)
{
    int deviceCount = 0;
    int adapterCount = 0;
    int extCount = 0;
    VkResult ret;
    VkExtensionProperties* exts = nullptr;
    VkPhysicalDeviceProperties props = {0};

    ret = s_Vk.enumeratePhysicalDevices(s_Vk.instance, &deviceCount, nullptr);
    if (ret != VK_SUCCESS) {
        return makeResultVk(ret);
    }

    if (s_Vk.adapters) {
        palFree(s_Vk.allocator, s_Vk.adapters);
    }

    VkPhysicalDevice* devices = nullptr;
    devices = palAllocate(s_Vk.allocator, sizeof(VkPhysicalDevice) * deviceCount, 0);
    s_Vk.adapters = palAllocate(s_Vk.allocator, sizeof(AdapterVk) * deviceCount, 0);
    if (!devices || !s_Vk.adapters) {
        return PAL_RESULT_CODE_OUT_OF_MEMORY;
    }

    s_Vk.enumeratePhysicalDevices(s_Vk.instance, &deviceCount, devices);
    for (int i = 0; i < deviceCount; i++) {
        VkPhysicalDevice phyDevice = devices[i];
        s_Vk.getPhysicalDeviceProperties(phyDevice, &props);
        if (props.apiVersion < VK_API_VERSION_1_3) {
            // check extension
            ret = s_Vk.enumerateDeviceExtensionProperties(phyDevice, nullptr, &extCount, nullptr);
            exts = palAllocate(s_Vk.allocator, sizeof(VkExtensionProperties) * extCount, 0);
            if (!exts) {
                return PAL_RESULT_CODE_OUT_OF_MEMORY;
            }

            PalBool found = PAL_FALSE;
            s_Vk.enumerateDeviceExtensionProperties(phyDevice, nullptr, &extCount, exts);

            for (int i = 0; i < extCount; i++) {
                const char* ext = exts[i].extensionName;
                if (strcmp(ext, "VK_KHR_dynamic_rendering") == 0) {
                    found = PAL_TRUE;
                    break;
                }
            }

            palFree(s_Vk.allocator, exts);
            if (!found) {
                continue;
            }
        }

        VkPhysicalDeviceDynamicRenderingFeaturesKHR required = {0};
        required.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DYNAMIC_RENDERING_FEATURES_KHR;

        VkPhysicalDeviceFeatures2KHR features = {0};
        features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2_KHR;
        features.pNext = &required;
        s_Vk.getPhysicalDeviceFeatures2(phyDevice, &features);

        if (!required.dynamicRendering) {
            continue;
        }

        if (outAdapters) {
            if (adapterCount < *count) {
                AdapterVk* tmp = &s_Vk.adapters[adapterCount];
                tmp->handle = devices[i];
                outAdapters[adapterCount] = (PalAdapter*)tmp;
            }
        }
        adapterCount++;
    }

    if (!outAdapters) {
        *count = adapterCount;
    }

    palFree(s_Vk.allocator, devices);
    return PAL_RESULT_SUCCESS;
}

void PAL_CALL getAdapterInfoVk(
    PalAdapter* adapter,
    PalAdapterInfo* info)
{
    AdapterVk* vkAdapter = (AdapterVk*)adapter;
    VkPhysicalDevice phyDevice = (VkPhysicalDevice)vkAdapter->handle;
    VkPhysicalDeviceProperties props = {0};
    VkPhysicalDeviceMemoryProperties memProps = {0};

    s_Vk.getPhysicalDeviceMemoryProperties(phyDevice, &memProps);
    s_Vk.getPhysicalDeviceProperties(phyDevice, &props);

    info->apiType = PAL_ADAPTER_API_TYPE_VULKAN;
    info->shaderFormats = PAL_SHADER_FORMAT_SPIRV;
    info->deviceId = props.deviceID;
    info->vendorId = props.vendorID;
    info->driverVersion = props.driverVersion;
    strcpy(info->name, props.deviceName);
    strcpy(info->backendName, "PAL");

    info->vram = 0;
    info->sharedMemory = 0;
    for (int i = 0; i < memProps.memoryHeapCount; i++) {
        if (memProps.memoryHeaps[i].flags & VK_MEMORY_HEAP_DEVICE_LOCAL_BIT) {
            info->vram += memProps.memoryHeaps[i].size;
        } else {
            info->sharedMemory += memProps.memoryHeaps[i].size;
        }
    }

    // get device type
    switch (props.deviceType) {
        case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU: {
            info->type = PAL_ADAPTER_TYPE_INTEGRATED;
            break;
        }

        case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU: {
            info->type = PAL_ADAPTER_TYPE_DISCRETE;
            break;
        }

        case VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU: {
            info->type = PAL_ADAPTER_TYPE_VIRTUAL;
            break;
        }

        case VK_PHYSICAL_DEVICE_TYPE_CPU: {
            info->type = PAL_ADAPTER_TYPE_CPU;
            break;
        }

        default: {
            info->type = PAL_ADAPTER_TYPE_UNKNOWN;
            break;
        }
    }
}

void PAL_CALL getAdapterCapabilitiesVk(
    PalAdapter* adapter,
    PalAdapterCapabilities* caps)
{
    AdapterVk* vkAdapter = (AdapterVk*)adapter;
    VkPhysicalDevice phyDevice = (VkPhysicalDevice)vkAdapter->handle;

    VkPhysicalDeviceProperties props = {0};
    s_Vk.getPhysicalDeviceProperties(phyDevice, &props);
    VkPhysicalDeviceLimits* limits = &props.limits;

    VkPhysicalDeviceProperties2 properties2 = {0};
    properties2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;

    VkPhysicalDeviceAccelerationStructurePropertiesKHR accProps = {0};
    accProps.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ACCELERATION_STRUCTURE_PROPERTIES_KHR;
    properties2.pNext = &accProps;
    s_Vk.getPhysicalDeviceProperties2(phyDevice, &properties2);

    PalViewportCapabilities* viewportCaps = &caps->viewportCaps;
    PalImageCapabilities* imageCaps = &caps->imageCaps;
    PalResourceCapabilities* resourceCaps = &caps->resourceCaps;
    PalComputeCapabilities* computeCaps = &caps->computeCaps;

    // get supported queue commands
    uint32_t count = 0;
    s_Vk.getPhysicalDeviceQueueFamilyProperties(phyDevice, &count, nullptr);

    VkQueueFamilyProperties* queueProps = nullptr;
    queueProps = palAllocate(s_Vk.allocator, sizeof(VkQueueFamilyProperties) * count, 0);
    s_Vk.getPhysicalDeviceQueueFamilyProperties(phyDevice, &count, queueProps);

    caps->maxComputeQueues = 0;
    caps->maxGraphicsQueues = 0;
    caps->maxCopyQueues = 0;

    for (int i = 0; i < count; i++) {
        if (queueProps[i].queueFlags & VK_QUEUE_COMPUTE_BIT) {
            caps->maxComputeQueues += queueProps->queueCount;
        }

        if (queueProps[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            caps->maxGraphicsQueues += queueProps->queueCount;
        }

        if (queueProps[i].queueFlags & VK_QUEUE_TRANSFER_BIT) {
            caps->maxCopyQueues += queueProps->queueCount;
        }
    }

    caps->maxColorAttachments = limits->maxColorAttachments;
    caps->maxUniformBufferSize = limits->maxUniformBufferRange;
    caps->maxStorageBufferSize = limits->maxStorageBufferRange;
    caps->maxPushConstantSize = limits->maxPushConstantsSize;

    caps->maxVertexLayouts = limits->maxVertexInputBindings;
    caps->maxVertexAttributes = limits->maxVertexInputAttributes;
    caps->maxTessellationPatchPoint = limits->maxTessellationPatchSize;

    // viewport limits
    viewportCaps->maxWidth = limits->maxViewportDimensions[0];
    viewportCaps->maxHeight = limits->maxViewportDimensions[1];
    viewportCaps->minBoundsRange = limits->viewportBoundsRange[0];
    viewportCaps->maxBoundsRange = limits->viewportBoundsRange[1];

    // image limits
    imageCaps->maxWidth = limits->maxImageDimension2D;
    imageCaps->maxHeight = limits->maxImageDimension2D;
    imageCaps->maxDepth = limits->maxImageDimension3D;
    imageCaps->maxArrayLayers = limits->maxImageArrayLayers;

    // vulkan does not give this but we calculate from the max width and width
    uint32_t a = imageCaps->maxWidth;
    uint32_t b = imageCaps->maxHeight;
    uint32_t c = imageCaps->maxDepth;

    uint32_t tmp = a > b ? a : b;
    uint32_t size = tmp > c ? tmp : c;
    uint32_t levels = 0;
    while (size > 0) {
        // divide by two
        size = size / 2;
        levels++;
    }
    imageCaps->maxMipLevels = levels;

    // resource limits
    resourceCaps->maxPerStageSampledImages = limits->maxPerStageDescriptorSampledImages;
    resourceCaps->maxPerSetSampledImages = limits->maxDescriptorSetSampledImages;
    resourceCaps->maxPerStageStorageImages = limits->maxPerStageDescriptorStorageImages;
    resourceCaps->maxPerSetStorageImages = limits->maxDescriptorSetStorageImages;

    resourceCaps->maxPerStageSamplers = limits->maxPerStageDescriptorSamplers;
    resourceCaps->maxPerSetSamplers = limits->maxDescriptorSetSamplers;
    resourceCaps->maxPerStageStorageBuffers = limits->maxPerStageDescriptorStorageBuffers;
    resourceCaps->maxPerSetStorageBuffers = limits->maxDescriptorSetStorageBuffers;

    resourceCaps->maxPerStageUniformBuffers = limits->maxPerStageDescriptorUniformBuffers;
    resourceCaps->maxPerSetUniformBuffers = limits->maxDescriptorSetUniformBuffers;

    tmp = accProps.maxPerStageDescriptorAccelerationStructures;
    resourceCaps->maxPerStageAccelerationStructure = tmp;
    resourceCaps->maxPerSetAccelerationStructure = accProps.maxDescriptorSetAccelerationStructures;
    resourceCaps->maxBoundSets = limits->maxBoundDescriptorSets;

    // compute limits
    computeCaps->maxWorkGroupInvocations = limits->maxComputeWorkGroupInvocations;
    computeCaps->maxWorkGroupCount[0] = limits->maxComputeWorkGroupCount[0];
    computeCaps->maxWorkGroupCount[1] = limits->maxComputeWorkGroupCount[1];
    computeCaps->maxWorkGroupCount[2] = limits->maxComputeWorkGroupCount[2];
    computeCaps->maxWorkGroupSize[0] = limits->maxComputeWorkGroupSize[0];
    computeCaps->maxWorkGroupSize[1] = limits->maxComputeWorkGroupSize[1];
    computeCaps->maxWorkGroupSize[2] = limits->maxComputeWorkGroupSize[2];

    palFree(s_Vk.allocator, queueProps);
}

PalAdapterFeatures PAL_CALL getAdapterFeaturesVk(PalAdapter* adapter)
{
    VkResult result;
    PalAdapterFeatures adapterFeatures = 0;
    uint32_t extensionCount = 0;
    VkPhysicalDeviceProperties props = {0};

    AdapterVk* vkAdapter = (AdapterVk*)adapter;
    VkPhysicalDevice phyDevice = (VkPhysicalDevice)vkAdapter->handle;

    // get supported extensions
    s_Vk.getPhysicalDeviceProperties(phyDevice, &props);
    result = s_Vk.enumerateDeviceExtensionProperties(phyDevice, nullptr, &extensionCount, nullptr);
    if (result != VK_SUCCESS) {
        // we just return without any modern features which is rare
        return 0;
    }

    VkExtensionProperties* extensionProps = nullptr;
    extensionProps = palAllocate(s_Vk.allocator, sizeof(VkExtensionProperties) * extensionCount, 0);
    if (!extensionProps) {
        return 0;
    }

    // check extensions
    PalBool hasRayTracing = PAL_FALSE;
    PalBool hasAccelerationStructure = PAL_FALSE;
    PalBool hasMeshShader = PAL_FALSE;
    PalBool hasFragmentRateShading = PAL_FALSE;
    PalBool hasTimelineSemaphore = PAL_FALSE;
    PalBool hasDescriptorIndexing = PAL_FALSE;
    PalBool hasShaderFloat16 = PAL_FALSE;
    PalBool hasMultiiView = PAL_FALSE;
    PalBool hasDynamicstate = PAL_FALSE;
    PalBool hasBufferDeviceAddress = PAL_FALSE;
    PalBool hasShaderParameters = PAL_FALSE;
    PalBool hasNullDescriptors = PAL_FALSE;
    PalBool hasRayQuery = PAL_FALSE;
    s_Vk.enumerateDeviceExtensionProperties(phyDevice, nullptr, &extensionCount, extensionProps);

    // clang-format off
    // check if the extensions are present
    for (int i = 0; i < extensionCount; i++) {
        VkExtensionProperties* props = &extensionProps[i];
        if (strcmp(props->extensionName, "VK_KHR_ray_tracing_pipeline") == 0) {
            hasRayTracing = PAL_TRUE;

        } else if (strcmp(props->extensionName, "VK_KHR_acceleration_structure") == 0) {
            hasAccelerationStructure = PAL_TRUE;

        } else if (strcmp(props->extensionName, "VK_EXT_mesh_shader") == 0) {
            hasMeshShader = PAL_TRUE;

        } else if (strcmp(props->extensionName, "VK_KHR_fragment_shading_rate") == 0) {
            hasFragmentRateShading = PAL_TRUE;

        } else if (strcmp(props->extensionName, "VK_EXT_descriptor_indexing") == 0) {
            hasDescriptorIndexing = PAL_TRUE;

        } else if (strcmp(props->extensionName, "VK_KHR_swapchain") == 0) {
            adapterFeatures |= PAL_ADAPTER_FEATURE_SWAPCHAIN;

        } else if (strcmp(props->extensionName, "VK_KHR_shader_float16_int8") == 0) {
            hasShaderFloat16 = PAL_TRUE;

        } else if (strcmp(props->extensionName, "VK_KHR_timeline_semaphore") == 0) {
            hasTimelineSemaphore = PAL_TRUE;

        } else if (strcmp(props->extensionName, "VK_KHR_multiview") == 0) {
            hasMultiiView = PAL_TRUE;

        } else if (strcmp(props->extensionName, "VK_EXT_extended_dynamic_state") == 0) {
            hasDynamicstate = PAL_TRUE;

        } else if (strcmp(props->extensionName, "VK_EXT_extended_dynamic_state2") == 0) {
            VkPhysicalDeviceExtendedDynamicState2FeaturesEXT dynState2 = {0};
            dynState2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_EXTENDED_DYNAMIC_STATE_2_FEATURES_EXT;

            VkPhysicalDeviceFeatures2 features;
            features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
            features.pNext = &dynState2;

            s_Vk.getPhysicalDeviceFeatures2(phyDevice, &features);
            if (dynState2.extendedDynamicState2) {
                adapterFeatures |= PAL_ADAPTER_FEATURE_DYNAMIC_DEPTH_TEST_ENABLE;
                adapterFeatures |= PAL_ADAPTER_FEATURE_DYNAMIC_DEPTH_WRITE_ENABLE;
                adapterFeatures |= PAL_ADAPTER_FEATURE_DYNAMIC_STENCIL_OP;
            }

        } else if (strcmp(props->extensionName, "VK_KHR_depth_stencil_resolve") == 0) {
            adapterFeatures |= PAL_ADAPTER_FEATURE_DEPTH_STENCIL_RESOLVE;

        } else if (strcmp(props->extensionName, "VK_KHR_draw_indirect_count") == 0) {
            adapterFeatures |= PAL_ADAPTER_FEATURE_INDIRECT_DRAW_COUNT;

        } else if (strcmp(props->extensionName, "VK_KHR_draw_mesh_tasks_indirect_count") == 0) {
            adapterFeatures |= PAL_ADAPTER_FEATURE_INDIRECT_DRAW_MESH_COUNT;

        } else if (strcmp(props->extensionName, "VK_KHR_buffer_device_address") == 0) {
            hasBufferDeviceAddress = PAL_TRUE;

        }  else if (strcmp(props->extensionName, "VK_KHR_shader_draw_parameters") == 0) {
            hasShaderParameters = PAL_TRUE;

        } else if (strcmp(props->extensionName, "VK_EXT_robustness2") == 0) {
            hasNullDescriptors = PAL_TRUE;
        }
    }

    // features that require additional checks
    if (hasRayTracing && hasAccelerationStructure) {
        VkPhysicalDeviceRayTracingPipelineFeaturesKHR ray = {0};
        VkPhysicalDeviceAccelerationStructureFeaturesKHR acc = {0};

        ray.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_TRACING_PIPELINE_FEATURES_KHR;
        acc.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ACCELERATION_STRUCTURE_FEATURES_KHR;

        ray.pNext = &acc;
        VkPhysicalDeviceFeatures2 features;
        features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
        features.pNext = &ray;

        s_Vk.getPhysicalDeviceFeatures2(phyDevice, &features);
        if (ray.rayTracingPipeline && acc.accelerationStructure) {
            adapterFeatures |= PAL_ADAPTER_FEATURE_RAY_TRACING;
        }

        if (ray.rayTracingPipelineTraceRaysIndirect) {
            adapterFeatures |= PAL_ADAPTER_FEATURE_INDIRECT_RAY_TRACING;
        }
    }

    if (hasMeshShader) {
        VkPhysicalDeviceMeshShaderFeaturesEXT mesh = {0};
        mesh.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MESH_SHADER_FEATURES_EXT;

        VkPhysicalDeviceFeatures2 features;
        features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
        features.pNext = &mesh;

        s_Vk.getPhysicalDeviceFeatures2(phyDevice, &features);
        if (mesh.meshShader && mesh.taskShader) {
            adapterFeatures |= PAL_ADAPTER_FEATURE_MESH_SHADER;
            adapterFeatures |= PAL_ADAPTER_FEATURE_INDIRECT_DRAW_MESH;
        }
    }

    if (hasFragmentRateShading) {
        VkPhysicalDeviceFragmentShadingRateFeaturesKHR frag = {0};
        frag.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FRAGMENT_SHADING_RATE_FEATURES_KHR;

        VkPhysicalDeviceFeatures2 features;
        features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
        features.pNext = &frag;

        s_Vk.getPhysicalDeviceFeatures2(phyDevice, &features);
        if (frag.pipelineFragmentShadingRate) {
            adapterFeatures |= PAL_ADAPTER_FEATURE_FRAGMENT_SHADING_RATE;
        }

        if (frag.attachmentFragmentShadingRate) {
            adapterFeatures |=
                PAL_ADAPTER_FEATURE_FRAGMENT_SHADING_RATE_ATTACHMENT;
        }
    }

    if (props.apiVersion >= VK_API_VERSION_1_2 || hasDescriptorIndexing) {
        VkPhysicalDeviceDescriptorIndexingFeaturesEXT desc = {0};
        desc.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_FEATURES_EXT;

        VkPhysicalDeviceFeatures2 features;
        features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
        features.pNext = &desc;

        s_Vk.getPhysicalDeviceFeatures2(phyDevice, &features);
        if (desc.runtimeDescriptorArray) {
            adapterFeatures |= PAL_ADAPTER_FEATURE_DESCRIPTOR_INDEXING;
        }
    }

    if (props.apiVersion >= VK_API_VERSION_1_2 || hasTimelineSemaphore) {
        VkPhysicalDeviceTimelineSemaphoreFeaturesKHR timeline = {0};
        timeline.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_TIMELINE_SEMAPHORE_FEATURES_KHR;

        VkPhysicalDeviceFeatures2 features;
        features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
        features.pNext = &timeline;

        s_Vk.getPhysicalDeviceFeatures2(phyDevice, &features);
        if (timeline.timelineSemaphore) {
            adapterFeatures |= PAL_ADAPTER_FEATURE_TIMELINE_SEMAPHORE;
        }
    }

    if (props.apiVersion >= VK_API_VERSION_1_2 || hasShaderFloat16) {
        VkPhysicalDeviceShaderFloat16Int8FeaturesKHR shader16 = {0};
        shader16.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_FLOAT16_INT8_FEATURES_KHR;

        VkPhysicalDeviceFeatures2 features;
        features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
        features.pNext = &shader16;

        s_Vk.getPhysicalDeviceFeatures2(phyDevice, &features);
        if (shader16.shaderFloat16) {
            adapterFeatures |= PAL_ADAPTER_FEATURE_SHADER_FLOAT16;
        }
    }

    if (props.apiVersion >= VK_API_VERSION_1_1 || hasMultiiView) {
        VkPhysicalDeviceMultiviewFeaturesKHR multiView = {0};
        multiView.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MULTIVIEW_FEATURES_KHR;

        VkPhysicalDeviceFeatures2 features;
        features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
        features.pNext = &multiView;

        s_Vk.getPhysicalDeviceFeatures2(phyDevice, &features);
        if (multiView.multiview) {
            adapterFeatures |= PAL_ADAPTER_FEATURE_MULTI_VIEW;
        }
    }

    if (props.apiVersion >= VK_API_VERSION_1_3 || hasDynamicstate) {
        VkPhysicalDeviceExtendedDynamicStateFeaturesEXT dynState = {0};
        dynState.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_EXTENDED_DYNAMIC_STATE_FEATURES_EXT;

        VkPhysicalDeviceFeatures2 features;
        features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
        features.pNext = &dynState;

        s_Vk.getPhysicalDeviceFeatures2(phyDevice, &features);
        if (dynState.extendedDynamicState) {
            adapterFeatures |= PAL_ADAPTER_FEATURE_DYNAMIC_CULL_MODE;
            adapterFeatures |= PAL_ADAPTER_FEATURE_DYNAMIC_FRONT_FACE;
            adapterFeatures |= PAL_ADAPTER_FEATURE_DYNAMIC_PRIMITIVE_TOPOLOGY;
        }
    }

    if (props.apiVersion >= VK_API_VERSION_1_2 || hasBufferDeviceAddress) {
        VkPhysicalDeviceBufferDeviceAddressFeaturesKHR bufferAddress = {0};
        bufferAddress.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_BUFFER_DEVICE_ADDRESS_FEATURES_KHR;

        VkPhysicalDeviceFeatures2 features;
        features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
        features.pNext = &bufferAddress;

        s_Vk.getPhysicalDeviceFeatures2(phyDevice, &features);
        if (bufferAddress.bufferDeviceAddress) {
            adapterFeatures |= PAL_ADAPTER_FEATURE_BUFFER_DEVICE_ADDRESS;
        }
    }

    if (props.apiVersion >= VK_API_VERSION_1_2) {
        VkPhysicalDeviceVulkan12Features features12 = {0};
        features12.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES;

        VkPhysicalDeviceFeatures2 features;
        features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
        features.pNext = &features12;

        s_Vk.getPhysicalDeviceFeatures2(phyDevice, &features);
        if (features12.drawIndirectCount) {
            adapterFeatures |= PAL_ADAPTER_FEATURE_INDIRECT_DRAW_COUNT;
        }
    }

    if (props.apiVersion >= VK_API_VERSION_1_2 || hasShaderParameters) {
        VkPhysicalDeviceShaderDrawParametersFeatures drawParameters = {0};
        drawParameters.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_DRAW_PARAMETERS_FEATURES;

        VkPhysicalDeviceFeatures2 features;
        features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
        features.pNext = &drawParameters;

        s_Vk.getPhysicalDeviceFeatures2(phyDevice, &features);
        if (drawParameters.shaderDrawParameters) {
            adapterFeatures |= PAL_ADAPTER_FEATURE_DISPATCH_BASE;
        }
    }

    if (hasNullDescriptors) {
        VkPhysicalDeviceRobustness2FeaturesEXT nullDescriptors = {0};
        nullDescriptors.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ROBUSTNESS_2_FEATURES_EXT;

        VkPhysicalDeviceFeatures2 features;
        features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
        features.pNext = &nullDescriptors;

        s_Vk.getPhysicalDeviceFeatures2(phyDevice, &features);
        if (nullDescriptors.nullDescriptor) {
            adapterFeatures |= PAL_ADAPTER_FEATURE_NULL_DESCRIPTORS;
        }
    }

    if (hasRayQuery) {
        VkPhysicalDeviceRayQueryFeaturesKHR query = {0};
        query.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_QUERY_FEATURES_KHR;

        VkPhysicalDeviceFeatures2 features;
        features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
        features.pNext = &query;

        s_Vk.getPhysicalDeviceFeatures2(phyDevice, &features);
        if (query.rayQuery) {
            adapterFeatures |= PAL_ADAPTER_FEATURE_RAY_QUERY;
        }
    }
    // clang-format on

    VkPhysicalDeviceFeatures features;
    s_Vk.getPhysicalDeviceFeatures(phyDevice, &features);

    // check for additional features
    if (features.multiViewport) {
        adapterFeatures |= PAL_ADAPTER_FEATURE_MULTI_VIEWPORT;
    }

    if (features.samplerAnisotropy) {
        adapterFeatures |= PAL_ADAPTER_FEATURE_SAMPLER_ANISOTROPY;
    }

    if (features.sampleRateShading) {
        adapterFeatures |= PAL_ADAPTER_FEATURE_SAMPLE_RATE_SHADING;
    }

    if (features.shaderFloat64) {
        adapterFeatures |= PAL_ADAPTER_FEATURE_SHADER_FLOAT64;
    }

    if (features.shaderInt64) {
        adapterFeatures |= PAL_ADAPTER_FEATURE_SHADER_INT64;
    }

    if (features.shaderInt16) {
        adapterFeatures |= PAL_ADAPTER_FEATURE_SHADER_INT16;
    }

    if (features.geometryShader) {
        adapterFeatures |= PAL_ADAPTER_FEATURE_GEOMETRY_SHADER;
    }

    if (features.tessellationShader) {
        adapterFeatures |= PAL_ADAPTER_FEATURE_TESSELLATION_SHADER;
    }

    if (features.fillModeNonSolid) {
        adapterFeatures |= PAL_ADAPTER_FEATURE_POLYGON_MODE_LINE;
    }

    // this features are supported on vulkan
    adapterFeatures |= PAL_ADAPTER_FEATURE_IMAGE_VIEW_CUBE_ARRAY;
    adapterFeatures |= PAL_ADAPTER_FEATURE_FENCE_RESET;
    adapterFeatures |= PAL_ADAPTER_FEATURE_INDIRECT_DRAW;
    adapterFeatures |= PAL_ADAPTER_FEATURE_INDIRECT_DISPATCH;

    palFree(s_Vk.allocator, extensionProps);
    return adapterFeatures;
}

uint32_t PAL_CALL getHighestSupportedShaderTargetVk(
    PalAdapter* adapter,
    PalShaderFormats shaderFormat)
{
    if (shaderFormat != PAL_SHADER_FORMAT_SPIRV) {
        return 0;
    }

    AdapterVk* vkAdapter = (AdapterVk*)adapter;
    VkPhysicalDeviceProperties props = {0};
    s_Vk.getPhysicalDeviceProperties(vkAdapter->handle, &props);

    if (props.apiVersion >= VK_API_VERSION_1_3) {
        return PAL_MAKE_SHADER_TARGET(1, 6);

    } else if (props.apiVersion >= VK_API_VERSION_1_2) {
        return PAL_MAKE_SHADER_TARGET(1, 5);

    } else if (props.apiVersion >= VK_API_VERSION_1_1) {
        return PAL_MAKE_SHADER_TARGET(1, 4);

    } else if (props.apiVersion >= VK_API_VERSION_1_0) {
        return PAL_MAKE_SHADER_TARGET(1, 2);
    }

    return 0;
}

void PAL_CALL enumerateFormatsVk(
    PalAdapter* adapter,
    uint32_t* count,
    PalFormatInfo* outFormats)
{
    int32_t fmtCount = 0;
    AdapterVk* vkAdapter = (AdapterVk*)adapter;
    VkPhysicalDevice phyDevice = (VkPhysicalDevice)vkAdapter->handle;
    VkFormatProperties props = {0};

    for (int i = 0; i < PAL_FORMAT_COUNT; i++) {
        VkFormat fmt = formatToVk((PalFormat)i);
        s_Vk.getPhysicalDeviceFormatProperties(phyDevice, fmt, &props);
        if (props.optimalTilingFeatures != 0) {
            // format supported
            if (outFormats) {
                if (fmtCount < *count) {
                    PalFormatInfo* fmtInfo = &outFormats[fmtCount++];
                    fmtInfo->format = (PalFormat)i;
                    fmtInfo->usages = imageUsageFromVk(props.optimalTilingFeatures);
                }

            } else {
                fmtCount++;
            }
        }
    }
    if (!outFormats) {
        *count = fmtCount;
    }
}

PalBool PAL_CALL isFormatSupportedVk(
    PalAdapter* adapter,
    PalFormat format)
{
    AdapterVk* vkAdapter = (AdapterVk*)adapter;
    VkPhysicalDevice phyDevice = (VkPhysicalDevice)vkAdapter->handle;
    VkFormatProperties props = {0};

    VkFormat fmt = formatToVk(format);
    s_Vk.getPhysicalDeviceFormatProperties(phyDevice, fmt, &props);
    if (props.optimalTilingFeatures != 0) {
        return PAL_TRUE;
    }
    return PAL_FALSE;
}

PalImageUsages PAL_CALL queryFormatImageUsagesVk(
    PalAdapter* adapter,
    PalFormat format)
{
    AdapterVk* vkAdapter = (AdapterVk*)adapter;
    VkPhysicalDevice phyDevice = (VkPhysicalDevice)vkAdapter->handle;
    VkFormatProperties props = {0};

    VkFormat fmt = formatToVk(format);
    s_Vk.getPhysicalDeviceFormatProperties(phyDevice, fmt, &props);
    if (props.optimalTilingFeatures == 0) {
        return PAL_IMAGE_USAGE_UNDEFINED;
    }

    return imageUsageFromVk(props.optimalTilingFeatures);
}

PalSampleCount PAL_CALL queryFormatSampleCountVk(
    PalAdapter* adapter,
    PalFormat format)
{
    VkResult result;
    AdapterVk* vkAdapter = (AdapterVk*)adapter;
    VkFormatProperties props = {0};
    VkImageFormatProperties formatProps = {0};

    VkFormat fmt = formatToVk(format);
    s_Vk.getPhysicalDeviceFormatProperties(vkAdapter->handle, fmt, &props);
    if (props.optimalTilingFeatures == 0) {
        return PAL_SAMPLE_COUNT_1;
    }

    VkImageUsageFlags vkImageUsage = 0;
    PalImageUsages imageUsages = imageUsageFromVk(props.optimalTilingFeatures);
    PalBool isDepth = (imageUsages & PAL_IMAGE_USAGE_DEPTH_ATTACHEMENT) != 0;
    if (isDepth) {
        vkImageUsage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
    } else {
        vkImageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    }

    result = s_Vk.getPhysicalDeviceImageFormatProperties(
        vkAdapter->handle,
        fmt,
        VK_IMAGE_TYPE_2D,
        VK_IMAGE_TILING_OPTIMAL,
        vkImageUsage,
        0,
        &formatProps);

    if (result != VK_SUCCESS) {
        return PAL_SAMPLE_COUNT_1;
    }

    return samplesFromVk(formatProps.sampleCounts);
}

#endif // PAL_HAS_VULKAN_BACKEND