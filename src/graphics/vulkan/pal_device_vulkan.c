
/**
    PAL - Prime Abstraction Layer
    Copyright (C) 2025
    Licensed under the Zlib license. See LICENSE file in root.
 */

#if PAL_HAS_VULKAN_BACKEND
#include "pal_vulkan.h"

static void fillDescriptorIndexingFeatures(
    VkPhysicalDeviceDescriptorIndexingFeaturesEXT* feature,
    VkPhysicalDevice phyDevice)
{
    VkPhysicalDeviceDescriptorIndexingFeaturesEXT desc = {0};
    desc.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_FEATURES_EXT;

    VkPhysicalDeviceFeatures2 features;
    features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
    features.pNext = &desc;
    s_Vk.getPhysicalDeviceFeatures2(phyDevice, &features);

    // clang-format off
    feature->runtimeDescriptorArray = PAL_TRUE;
    feature->descriptorBindingPartiallyBound = desc.descriptorBindingPartiallyBound;

    feature->descriptorBindingUpdateUnusedWhilePending = desc.descriptorBindingUpdateUnusedWhilePending;
    feature->shaderSampledImageArrayNonUniformIndexing = desc.shaderSampledImageArrayNonUniformIndexing;
    feature->descriptorBindingSampledImageUpdateAfterBind = desc.descriptorBindingSampledImageUpdateAfterBind;
    feature->shaderStorageImageArrayNonUniformIndexing = desc.shaderStorageImageArrayNonUniformIndexing;
    feature->descriptorBindingStorageImageUpdateAfterBind = desc.descriptorBindingStorageImageUpdateAfterBind;

    feature->shaderStorageBufferArrayNonUniformIndexing = desc.shaderStorageBufferArrayNonUniformIndexing;
    feature->descriptorBindingStorageBufferUpdateAfterBind = desc.descriptorBindingStorageBufferUpdateAfterBind;
    feature->shaderUniformBufferArrayNonUniformIndexing = desc.shaderUniformBufferArrayNonUniformIndexing;
    feature->descriptorBindingUniformBufferUpdateAfterBind = desc.descriptorBindingUniformBufferUpdateAfterBind;
    // clang-format on
}

static void loadFeatureProcs(
    PalAdapterFeatures features,
    DeviceVk* device)
{
    // clang-format off
    // swapchain procs
    if (features & PAL_ADAPTER_FEATURE_SWAPCHAIN) {
        device->acquireNextImage = (PFN_vkAcquireNextImageKHR)s_Vk.getDeviceProcAddr(
            device->handle,
            "vkAcquireNextImageKHR");

        device->createSwapchain = (PFN_vkCreateSwapchainKHR)s_Vk.getDeviceProcAddr(
            device->handle,
            "vkCreateSwapchainKHR");

        device->destroySwapchain = (PFN_vkDestroySwapchainKHR)s_Vk.getDeviceProcAddr(
            device->handle,
            "vkDestroySwapchainKHR");

        device->getSwapchainImages = (PFN_vkGetSwapchainImagesKHR)s_Vk.getDeviceProcAddr(
            device->handle,
            "vkGetSwapchainImagesKHR");

        device->queuePresent = (PFN_vkQueuePresentKHR)s_Vk.getDeviceProcAddr(
            device->handle,
            "vkQueuePresentKHR");
    }

    // semaphore procs
    if (features & PAL_ADAPTER_FEATURE_TIMELINE_SEMAPHORE) {
        device->waitSemaphore = (PFN_vkWaitSemaphores)s_Vk.getDeviceProcAddr(
            device->handle,
            "vkWaitSemaphores");

        device->signalSemaphore = (PFN_vkSignalSemaphore)s_Vk.getDeviceProcAddr(
            device->handle,
            "vkSignalSemaphore");

        device->getSemaphoreValue = (PFN_vkGetSemaphoreCounterValue)s_Vk.getDeviceProcAddr(
            device->handle,
            "vkGetSemaphoreCounterValue");

        if (!device->waitSemaphore) {
            device->waitSemaphore = (PFN_vkWaitSemaphoresKHR)s_Vk.getDeviceProcAddr(
                device->handle,
                "vkWaitSemaphoresKHR");

            device->signalSemaphore = (PFN_vkSignalSemaphoreKHR)s_Vk.getDeviceProcAddr(
                device->handle,
                "vkSignalSemaphoreKHR");

            device->getSemaphoreValue = (PFN_vkGetSemaphoreCounterValueKHR)s_Vk.getDeviceProcAddr(
                device->handle,
                "vkGetSemaphoreCounterValueKHR");
        }
    }

    // fragment shading rate procs
    if (features & PAL_ADAPTER_FEATURE_FRAGMENT_SHADING_RATE) {
        device->cmdSetFragmentShadingRate =
            (PFN_vkCmdSetFragmentShadingRateKHR)s_Vk.getDeviceProcAddr(
                device->handle,
                "vkCmdSetFragmentShadingRateKHR");
    }

    // mesh shader procs
    if (features & PAL_ADAPTER_FEATURE_MESH_SHADER) {
        device->cmdDrawMeshTask = (PFN_vkCmdDrawMeshTasksEXT)s_Vk.getDeviceProcAddr(
            device->handle,
            "vkCmdDrawMeshTasksEXT");

        device->cmdDrawMeshTaskIndirect =
            (PFN_vkCmdDrawMeshTasksIndirectEXT)s_Vk.getDeviceProcAddr(
                device->handle,
                "vkCmdDrawMeshTasksIndirectEXT");

        device->cmdDrawMeshTaskIndirectCount =
            (PFN_vkCmdDrawMeshTasksIndirectCountEXT)s_Vk.getDeviceProcAddr(
                device->handle,
                "vkCmdDrawMeshTasksIndirectCountEXT");
    }

    // ray tracing procs
    device->limits.maxPayloadSize = 0;
    if (features & PAL_ADAPTER_FEATURE_RAY_TRACING) {
        device->limits.maxPayloadSize = 64;

        device->createAccelerationStructure =
            (PFN_vkCreateAccelerationStructureKHR)s_Vk.getDeviceProcAddr(
                device->handle,
                "vkCreateAccelerationStructureKHR");

        device->destroyAccelerationStructure =
            (PFN_vkDestroyAccelerationStructureKHR)s_Vk.getDeviceProcAddr(
                device->handle,
                "vkDestroyAccelerationStructureKHR");

        device->getAccelerationBuildsize =
            (PFN_vkGetAccelerationStructureBuildSizesKHR)s_Vk.getDeviceProcAddr(
                device->handle,
                "vkGetAccelerationStructureBuildSizesKHR");

        device->cmdBuildAccelerationStructures =
            (PFN_vkCmdBuildAccelerationStructuresKHR)s_Vk.getDeviceProcAddr(
                device->handle,
                "vkCmdBuildAccelerationStructuresKHR");

        device->getAccelerationDeviceAddress =
            (PFN_vkGetAccelerationStructureDeviceAddressKHR)s_Vk.getDeviceProcAddr(
                device->handle,
                "vkGetAccelerationStructureDeviceAddressKHR");

        device->cmdTraceRays =
            (PFN_vkCmdTraceRaysKHR)s_Vk.getDeviceProcAddr(
                device->handle,
                "vkCmdTraceRaysKHR");

        device->createRayTracingPipeline =
            (PFN_vkCreateRayTracingPipelinesKHR)s_Vk.getDeviceProcAddr(
                device->handle,
                "vkCreateRayTracingPipelinesKHR");

        device->cmdTraceRaysIndirect =
            (PFN_vkCmdTraceRaysIndirectKHR)s_Vk.getDeviceProcAddr(
                device->handle,
                "vkCmdTraceRaysIndirectKHR");

        device->getRayTracingShaderGroupHandles =
            (PFN_vkGetRayTracingShaderGroupHandlesKHR)s_Vk.getDeviceProcAddr(
                device->handle,
                "vkGetRayTracingShaderGroupHandlesKHR");
    }

    // buffer address procs
    if (features & PAL_ADAPTER_FEATURE_BUFFER_DEVICE_ADDRESS) {
        device->getBufferrAddress =
            (PFN_vkGetBufferDeviceAddress)s_Vk.getDeviceProcAddr(
                device->handle,
                "vkGetBufferDeviceAddress");

        if (!device->getBufferrAddress) {
            device->getBufferrAddress =
                (PFN_vkGetBufferDeviceAddressKHR)s_Vk.getDeviceProcAddr(
                    device->handle,
                    "vkGetBufferDeviceAddressKHR");
        }
    }

    // indirect draw count procs
    if (features & PAL_ADAPTER_FEATURE_INDIRECT_DRAW_COUNT) {
        device->cmdDrawIndirectCount =
            (PFN_vkCmdDrawIndirectCount)s_Vk.getDeviceProcAddr(
                device->handle,
                "vkCmdDrawIndirectCount");

        device->cmdDrawIndexedIndirectCount =
                (PFN_vkCmdDrawIndexedIndirectCount)s_Vk.getDeviceProcAddr(
                    device->handle,
                    "vkCmdDrawIndexedIndirectCount");

        if (!device->cmdDrawIndirectCount) {
            device->cmdDrawIndirectCount =
                (PFN_vkCmdDrawIndirectCountKHR)s_Vk.getDeviceProcAddr(
                    device->handle,
                    "vkCmdDrawIndirectCountKHR");

            device->cmdDrawIndexedIndirectCount =
                    (PFN_vkCmdDrawIndexedIndirectCountKHR)s_Vk.getDeviceProcAddr(
                        device->handle,
                        "vkCmdDrawIndexedIndirectCountKHR");
        }
    }

    // dispatch base procs
    if (features & PAL_ADAPTER_FEATURE_FRAGMENT_SHADING_RATE) {
        device->cmdDispatchBase =
            (PFN_vkCmdDispatchBase)s_Vk.getDeviceProcAddr(
                device->handle,
                "vkCmdDispatchBase");

        if (!device->cmdDispatchBase) {
            device->cmdDispatchBase =
                (PFN_vkCmdDispatchBaseKHR)s_Vk.getDeviceProcAddr(
                    device->handle,
                    "vkCmdDispatchBaseKHR");
        }
    }

    // dynamic rendering procs
    device->cmdBeginRendering =
        (PFN_vkCmdBeginRendering)s_Vk.getDeviceProcAddr(
            device->handle,
            "vkCmdBeginRendering");

    device->cmdEndRendering =
        (PFN_vkCmdEndRendering)s_Vk.getDeviceProcAddr(
            device->handle,
            "vkCmdEndRendering");

    device->cmdPipelineBarrier =
        (PFN_vkCmdPipelineBarrier2)s_Vk.getDeviceProcAddr(
            device->handle,
            "vkCmdPipelineBarrier2");

    device->queueSubmit =
        (PFN_vkQueueSubmit2)s_Vk.getDeviceProcAddr(
            device->handle,
            "vkQueueSubmit2");

    if (!device->cmdBeginRendering) {
        device->cmdBeginRendering =
            (PFN_vkCmdBeginRenderingKHR)s_Vk.getDeviceProcAddr(
                device->handle,
                "vkCmdBeginRenderingKHR");

        device->cmdEndRendering =
            (PFN_vkCmdEndRenderingKHR)s_Vk.getDeviceProcAddr(
                device->handle,
                "vkCmdEndRenderingKHR");

        device->cmdPipelineBarrier =
        (PFN_vkCmdPipelineBarrier2KHR)s_Vk.getDeviceProcAddr(
            device->handle,
            "vkCmdPipelineBarrier2KHR");

        device->queueSubmit =
        (PFN_vkQueueSubmit2KHR)s_Vk.getDeviceProcAddr(
            device->handle,
            "vkQueueSubmit2KHR");
    }

    // dynamic states
    if (features & PAL_ADAPTER_FEATURE_DYNAMIC_CULL_MODE) {
        device->cmdSetCullMode =
            (PFN_vkCmdSetCullMode)s_Vk.getDeviceProcAddr(
                device->handle,
                "vkCmdSetCullMode");

        if (!device->cmdSetCullMode) {
            device->cmdSetCullMode =
                (PFN_vkCmdSetCullModeEXT)s_Vk.getDeviceProcAddr(
                    device->handle,
                    "vkCmdSetCullModeEXT");
        }
    }

    if (features & PAL_ADAPTER_FEATURE_DYNAMIC_FRONT_FACE) {
        device->cmdSetFrontFace =
            (PFN_vkCmdSetFrontFace)s_Vk.getDeviceProcAddr(
                device->handle,
                "vkcmdSetFrontFace");

        if (!device->cmdSetFrontFace) {
            device->cmdSetFrontFace =
                (PFN_vkCmdSetFrontFaceEXT)s_Vk.getDeviceProcAddr(
                    device->handle,
                    "vkcmdSetFrontFaceEXT");
        }
    }

    if (features & PAL_ADAPTER_FEATURE_DYNAMIC_PRIMITIVE_TOPOLOGY) {
        device->cmdSetPrimitiveTopology =
            (PFN_vkCmdSetPrimitiveTopology)s_Vk.getDeviceProcAddr(
                device->handle,
                "vkcmdSetPrimitiveTopology");

        if (!device->cmdSetPrimitiveTopology) {
            device->cmdSetPrimitiveTopology =
                (PFN_vkCmdSetPrimitiveTopologyEXT)s_Vk.getDeviceProcAddr(
                    device->handle,
                    "vkcmdSetPrimitiveTopologyEXT");
        }
    }

    if (features & PAL_ADAPTER_FEATURE_DYNAMIC_DEPTH_TEST_ENABLE) {
        device->cmdSetDepthTestEnable =
            (PFN_vkCmdSetDepthTestEnable)s_Vk.getDeviceProcAddr(
                device->handle,
                "vkcmdSetDepthTestEnable");

        if (!device->cmdSetDepthTestEnable) {
            device->cmdSetDepthTestEnable =
                (PFN_vkCmdSetDepthTestEnableEXT)s_Vk.getDeviceProcAddr(
                    device->handle,
                    "vkcmdSetDepthTestEnableEXT");
        }
    }

    if (features & PAL_ADAPTER_FEATURE_DYNAMIC_DEPTH_WRITE_ENABLE) {
        device->cmdSetDepthWriteEnable =
            (PFN_vkCmdSetDepthWriteEnable)s_Vk.getDeviceProcAddr(
                device->handle,
                "vkcmdSetDepthWriteEnable");

        if (!device->cmdSetDepthWriteEnable) {
            device->cmdSetDepthWriteEnable =
                (PFN_vkCmdSetDepthWriteEnableEXT)s_Vk.getDeviceProcAddr(
                    device->handle,
                    "vkcmdSetDepthWriteEnableEXT");
        }
    }

    if (features & PAL_ADAPTER_FEATURE_DYNAMIC_STENCIL_OP) {
        device->cmdSetStencilOp =
            (PFN_vkCmdSetStencilOp)s_Vk.getDeviceProcAddr(
                device->handle,
                "vkcmdSetStencilOp");

        if (!device->cmdSetStencilOp) {
            device->cmdSetStencilOp =
                (PFN_vkCmdSetStencilOpEXT)s_Vk.getDeviceProcAddr(
                    device->handle,
                    "vkcmdSetStencilOpEXT");
        }
    }

    // clang-format on
}

static VkShaderStageFlags shaderStageToVK(PalShaderStage stage)
{
    switch (stage) {
        case PAL_SHADER_STAGE_VERTEX:
            return VK_SHADER_STAGE_VERTEX_BIT;

        case PAL_SHADER_STAGE_FRAGMENT:
            return VK_SHADER_STAGE_FRAGMENT_BIT;

        case PAL_SHADER_STAGE_COMPUTE:
            return VK_SHADER_STAGE_COMPUTE_BIT;

        case PAL_SHADER_STAGE_GEOMETRY:
            return VK_SHADER_STAGE_GEOMETRY_BIT;

        case PAL_SHADER_STAGE_MESH:
            return VK_SHADER_STAGE_MESH_BIT_EXT;

        case PAL_SHADER_STAGE_TASK:
            return VK_SHADER_STAGE_TASK_BIT_EXT;

        case PAL_SHADER_STAGE_TESSELLATION_CONTROL:
            return VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT;

        case PAL_SHADER_STAGE_TESSELLATION_EVALUATION:
            return VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT;

        case PAL_SHADER_STAGE_RAYGEN:
            return VK_SHADER_STAGE_RAYGEN_BIT_KHR;

        case PAL_SHADER_STAGE_CLOSEST_HIT:
            return VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR;

        case PAL_SHADER_STAGE_ANY_HIT:
            return VK_SHADER_STAGE_ANY_HIT_BIT_KHR;

        case PAL_SHADER_STAGE_MISS:
            return VK_SHADER_STAGE_MISS_BIT_KHR;

        case PAL_SHADER_STAGE_INTERSECTION:
            return VK_SHADER_STAGE_INTERSECTION_BIT_KHR;

        case PAL_SHADER_STAGE_CALLABLE:
            return VK_SHADER_STAGE_CALLABLE_BIT_KHR;
    }

    return 0;
}

PalResult PAL_CALL createDeviceVk(
    PalAdapter* adapter,
    PalAdapterFeatures features,
    PalDevice** outDevice)
{
    float priority = 1.0f;
    uint32_t phyQueueCount = 0;
    uint32_t queueFamilyCount = 0;
    VkResult result = VK_SUCCESS;
    DeviceVk* device = nullptr;
    VkPhysicalDeviceProperties props = {0};

    AdapterVk* adapterImpl = (AdapterVk*)adapter;
    VkPhysicalDevice phyDevice = (VkPhysicalDevice)adapterImpl->handle;
    s_Vk.getPhysicalDeviceProperties(phyDevice, &props);

    VkQueueFamilyProperties* queueFamilyProps = nullptr;
    VkDeviceQueueCreateInfo* queueCreateInfos = nullptr;
    s_Vk.getPhysicalDeviceQueueFamilyProperties(phyDevice, &queueFamilyCount, nullptr);

    uint32_t queueFamilySize = sizeof(VkQueueFamilyProperties) * queueFamilyCount;
    uint32_t queueCreateInfosSize = sizeof(VkDeviceQueueCreateInfo) * queueFamilyCount;

    queueFamilyProps = palAllocate(s_Vk.allocator, queueFamilySize, 0);
    queueCreateInfos = palAllocate(s_Vk.allocator, queueCreateInfosSize, 0);
    device = palAllocate(s_Vk.allocator, sizeof(DeviceVk), 0);
    if (!queueFamilyProps || !queueCreateInfos || !device) {
        return PAL_RESULT_CODE_OUT_OF_MEMORY;
    }

    memset(device, 0, sizeof(DeviceVk));
    device->phyDevice = phyDevice;
    s_Vk.getPhysicalDeviceQueueFamilyProperties(phyDevice, &queueFamilyCount, queueFamilyProps);
    for (int i = 0; i < queueFamilyCount; i++) {
        queueCreateInfos[i].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfos[i].pNext = nullptr;
        queueCreateInfos[i].pQueuePriorities = &priority;
        queueCreateInfos[i].queueFamilyIndex = i;
        queueCreateInfos[i].queueCount = queueFamilyProps[i].queueCount;
        queueCreateInfos[i].flags = 0;

        // we need the total number of physical queues
        phyQueueCount += queueFamilyProps[i].queueCount;
    }

    device->phyQueues = palAllocate(s_Vk.allocator, sizeof(PhysicalQueue) * phyQueueCount, 0);
    if (!device->phyQueues) {
        return PAL_RESULT_CODE_OUT_OF_MEMORY;
    }

    // build features and extensions capabilities
    VkPhysicalDeviceFeatures phyDeviceFeatures = {0};
    s_Vk.getPhysicalDeviceFeatures(phyDevice, &phyDeviceFeatures);

    VkPhysicalDeviceFeatures coreFeatures = {0};
    if (features & PAL_ADAPTER_FEATURE_SAMPLER_ANISOTROPY) {
        coreFeatures.samplerAnisotropy = PAL_TRUE;
    }

    if (features & PAL_ADAPTER_FEATURE_SAMPLE_RATE_SHADING) {
        coreFeatures.sampleRateShading = PAL_TRUE;
    }

    if (features & PAL_ADAPTER_FEATURE_MULTI_VIEWPORT) {
        coreFeatures.multiViewport = PAL_TRUE;
    }

    if (features & PAL_ADAPTER_FEATURE_TESSELLATION_SHADER) {
        coreFeatures.tessellationShader = PAL_TRUE;
    }

    if (features & PAL_ADAPTER_FEATURE_GEOMETRY_SHADER) {
        coreFeatures.geometryShader = PAL_TRUE;
    }

    if (features & PAL_ADAPTER_FEATURE_SHADER_INT16) {
        coreFeatures.shaderInt16 = PAL_TRUE;
    }

    if (features & PAL_ADAPTER_FEATURE_SHADER_INT64) {
        coreFeatures.shaderInt64 = PAL_TRUE;
    }

    if (features & PAL_ADAPTER_FEATURE_SHADER_FLOAT64) {
        coreFeatures.shaderFloat64 = PAL_TRUE;
    }

    // extensions and features2
    void* next = nullptr;
    int extCount = 0;
    const char* extensions[32] = {0};

    VkPhysicalDeviceTimelineSemaphoreFeaturesKHR timeline = {0};
    timeline.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_TIMELINE_SEMAPHORE_FEATURES_KHR;

    VkPhysicalDeviceShaderFloat16Int8FeaturesKHR shader16 = {0};
    shader16.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_FLOAT16_INT8_FEATURES_KHR;

    VkPhysicalDeviceMeshShaderFeaturesEXT mesh = {0};
    mesh.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MESH_SHADER_FEATURES_EXT;

    VkPhysicalDeviceRayTracingPipelineFeaturesKHR ray = {0};
    VkPhysicalDeviceAccelerationStructureFeaturesKHR acc = {0};
    ray.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_TRACING_PIPELINE_FEATURES_KHR;
    acc.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ACCELERATION_STRUCTURE_FEATURES_KHR;

    VkPhysicalDeviceFragmentShadingRateFeaturesKHR fsr = {0};
    fsr.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FRAGMENT_SHADING_RATE_FEATURES_KHR;

    VkPhysicalDeviceDescriptorIndexingFeatures descIndex = {0};
    descIndex.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_FEATURES;

    VkPhysicalDeviceMultiviewFeatures multiView = {0};
    multiView.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MULTIVIEW_FEATURES;

    VkPhysicalDeviceExtendedDynamicStateFeaturesEXT dynamicState = {0};
    dynamicState.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_EXTENDED_DYNAMIC_STATE_FEATURES_EXT;

    VkPhysicalDeviceExtendedDynamicState2FeaturesEXT dynamicState2 = {0};
    dynamicState2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_EXTENDED_DYNAMIC_STATE_2_FEATURES_EXT;

    VkPhysicalDeviceBufferDeviceAddressFeaturesKHR bufferAddress = {0};
    bufferAddress.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_BUFFER_DEVICE_ADDRESS_FEATURES_KHR;

    VkPhysicalDeviceDynamicRenderingFeaturesKHR dynamicRendering = {0};
    dynamicRendering.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DYNAMIC_RENDERING_FEATURES_KHR;

    VkPhysicalDeviceSynchronization2FeaturesKHR sync2 = {0};
    sync2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SYNCHRONIZATION_2_FEATURES_KHR;

    VkPhysicalDeviceShaderDrawParametersFeatures drawParameters = {0};
    drawParameters.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_DRAW_PARAMETERS_FEATURES;

    VkPhysicalDeviceRobustness2FeaturesEXT nullDescriptors = {0};
    nullDescriptors.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ROBUSTNESS_2_FEATURES_EXT;

    if (props.apiVersion < VK_API_VERSION_1_3) {
        extensions[extCount++] = "VK_KHR_dynamic_rendering";
        extensions[extCount++] = "VK_KHR_synchronization2";
    }

    dynamicRendering.dynamicRendering = PAL_TRUE;
    sync2.synchronization2 = PAL_TRUE;

    dynamicRendering.pNext = next;
    sync2.pNext = &dynamicRendering;
    next = &sync2;

    if (features & PAL_ADAPTER_FEATURE_SWAPCHAIN) {
        extensions[extCount++] = "VK_KHR_swapchain";
    }

    if (features & PAL_ADAPTER_FEATURE_DEPTH_STENCIL_RESOLVE) {
        extensions[extCount++] = "VK_KHR_depth_stencil_resolve";
    }

    if (features & PAL_ADAPTER_FEATURE_TIMELINE_SEMAPHORE) {
        if (props.apiVersion < VK_API_VERSION_1_2) {
            extensions[extCount++] = "VK_KHR_timeline_semaphore";
        }
        timeline.timelineSemaphore = PAL_TRUE;

        timeline.pNext = next;
        next = &timeline;
    }

    if (features & PAL_ADAPTER_FEATURE_SHADER_FLOAT16) {
        if (props.apiVersion < VK_API_VERSION_1_2) {
            extensions[extCount++] = "VK_KHR_shader_float16_int8";
        }
        shader16.shaderFloat16 = PAL_TRUE;

        shader16.pNext = next;
        next = &shader16;
    }

    if (features & PAL_ADAPTER_FEATURE_RAY_TRACING) {
        extensions[extCount++] = "VK_KHR_ray_tracing_pipeline";
        extensions[extCount++] = "VK_KHR_acceleration_structure";
        extensions[extCount++] = "VK_KHR_deferred_host_operations";
        ray.rayTracingPipeline = PAL_TRUE;
        acc.accelerationStructure = PAL_TRUE;

        ray.pNext = next;
        acc.pNext = &ray;
        next = &acc;
    }

    if (features & PAL_ADAPTER_FEATURE_MESH_SHADER) {
        extensions[extCount++] = "VK_EXT_mesh_shader";
        mesh.meshShader = PAL_TRUE;
        mesh.taskShader = PAL_TRUE;

        // mesh shader needs geometry feature for primitives
        coreFeatures.geometryShader = PAL_TRUE;

        // msh draw indirect count
        if (features & PAL_ADAPTER_FEATURE_INDIRECT_DRAW_MESH_COUNT) {
            extensions[extCount++] = "VK_KHR_draw_mesh_tasks_indirect_count";
        }

        mesh.pNext = next;
        next = &mesh;
    }

    if (features & PAL_ADAPTER_FEATURE_INDIRECT_DRAW_COUNT) {
        if (props.apiVersion < VK_API_VERSION_1_2) {
            extensions[extCount++] = "VK_KHR_draw_indirect_count";
        }
    }

    if ((features & PAL_ADAPTER_FEATURE_FRAGMENT_SHADING_RATE) ||
        (features & PAL_ADAPTER_FEATURE_FRAGMENT_SHADING_RATE_ATTACHMENT)) {
        extensions[extCount++] = "VK_KHR_fragment_shading_rate";
        fsr.pipelineFragmentShadingRate = PAL_TRUE;

        // fragment shading rate needs this
        if (features & PAL_ADAPTER_FEATURE_FRAGMENT_SHADING_RATE_ATTACHMENT) {
            fsr.attachmentFragmentShadingRate = PAL_TRUE;
        }

        fsr.pNext = next;
        next = &fsr;
    }

    if (features & PAL_ADAPTER_FEATURE_DESCRIPTOR_INDEXING) {
        if (props.apiVersion < VK_API_VERSION_1_2) {
            extensions[extCount++] = "VK_EXT_descriptor_indexing";
        }

        fillDescriptorIndexingFeatures(&descIndex, phyDevice);
        descIndex.pNext = next;
        next = &descIndex;
    }

    if (features & PAL_ADAPTER_FEATURE_MULTI_VIEW) {
        if (props.apiVersion < VK_API_VERSION_1_2) {
            extensions[extCount++] = "VK_KHR_multiview";
        }
        multiView.multiview = PAL_TRUE;

        multiView.pNext = next;
        next = &multiView;
    }

    if (features & PAL_ADAPTER_FEATURE_DYNAMIC_CULL_MODE ||
        features & PAL_ADAPTER_FEATURE_DYNAMIC_FRONT_FACE ||
        features & PAL_ADAPTER_FEATURE_DYNAMIC_PRIMITIVE_TOPOLOGY) {
        if (props.apiVersion < VK_API_VERSION_1_3) {
            extensions[extCount++] = "VK_EXT_extended_dynamic_state";
        }
        dynamicState.extendedDynamicState = PAL_TRUE;

        dynamicState.pNext = next;
        next = &dynamicState;
    }

    if (features & PAL_ADAPTER_FEATURE_DYNAMIC_DEPTH_TEST_ENABLE ||
        features & PAL_ADAPTER_FEATURE_DYNAMIC_DEPTH_WRITE_ENABLE ||
        features & PAL_ADAPTER_FEATURE_DYNAMIC_STENCIL_OP) {
        extensions[extCount++] = "VK_EXT_extended_dynamic_state2";
        dynamicState2.extendedDynamicState2 = PAL_TRUE;

        dynamicState2.pNext = next;
        next = &dynamicState2;
    }

    if (features & PAL_ADAPTER_FEATURE_BUFFER_DEVICE_ADDRESS) {
        if (props.apiVersion < VK_API_VERSION_1_2) {
            extensions[extCount++] = "VK_KHR_buffer_device_address";
        }
        bufferAddress.bufferDeviceAddress = PAL_TRUE;

        bufferAddress.pNext = next;
        next = &bufferAddress;
    }

    if (features & PAL_ADAPTER_FEATURE_DISPATCH_BASE) {
        if (props.apiVersion < VK_API_VERSION_1_2) {
            extensions[extCount++] = "VK_KHR_shader_draw_parameters";
        }
        drawParameters.shaderDrawParameters = PAL_TRUE;

        drawParameters.pNext = next;
        next = &drawParameters;
    }

    if (features & PAL_ADAPTER_FEATURE_NULL_DESCRIPTORS) {
        extensions[extCount++] = "VK_EXT_robustness2";
        nullDescriptors.nullDescriptor = PAL_TRUE;

        nullDescriptors.pNext = next;
        next = &nullDescriptors;
    }

    VkDeviceCreateInfo createInfo = {0};
    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    createInfo.pEnabledFeatures = &coreFeatures;
    createInfo.enabledExtensionCount = extCount;
    createInfo.ppEnabledExtensionNames = extensions;
    createInfo.pQueueCreateInfos = queueCreateInfos;
    createInfo.queueCreateInfoCount = queueFamilyCount;
    createInfo.pNext = next;

    result = s_Vk.createDevice(phyDevice, &createInfo, &s_Vk.allocatorImpl, &device->handle);
    if (result != VK_SUCCESS) {
        palFree(s_Vk.allocator, queueFamilyProps);
        palFree(s_Vk.allocator, queueCreateInfos);
        palFree(s_Vk.allocator, device->phyQueues);
        palFree(s_Vk.allocator, device);
        return makeResultVk(result);
    }

    device->phyQueueIndex = 0;
    device->queueFamilyCount = queueFamilyCount;
    device->phyQueueCount = phyQueueCount;

    // get queues
    for (int i = 0; i < queueFamilyCount; i++) {
        VkQueueFamilyProperties* data = &queueFamilyProps[i];
        for (int j = 0; j < data->queueCount; j++) {
            PhysicalQueue* queue = &device->phyQueues[i];
            s_Vk.getDeviceQueue(device->handle, i, j, &queue->handle);
            queue->usages = data->queueFlags;
            queue->usedUsages = 0;
            queue->familyIndex = i;
            queue->phyDevice = phyDevice;
        }
    }

    // cache memory type indices
    VkPhysicalDeviceMemoryProperties memProps = {0};
    s_Vk.getPhysicalDeviceMemoryProperties(phyDevice, &memProps);

    memset(device->memoryClassMask, 0, sizeof(uint32_t) * 3);
    for (int i = 0; i < memProps.memoryTypeCount; i++) {
        VkMemoryPropertyFlags flags = memProps.memoryTypes[i].propertyFlags;

        uint32_t bit = (1u << i);
        if ((flags & VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT) &&
            !(flags & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT)) {
            device->memoryClassMask[PAL_MEMORY_TYPE_GPU_ONLY] |= bit;
        }

        if ((flags & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT) &&
            (flags & VK_MEMORY_PROPERTY_HOST_COHERENT_BIT)) {
            device->memoryClassMask[PAL_MEMORY_TYPE_CPU_UPLOAD] |= bit;
        }

        if ((flags & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT) &&
            (flags & VK_MEMORY_PROPERTY_HOST_CACHED_BIT)) {
            device->memoryClassMask[PAL_MEMORY_TYPE_CPU_READBACK] |= bit;
        }
    }

    // HACK: most CPU drivers dont have a vram so we set the vram to system memory
    if (device->memoryClassMask[PAL_MEMORY_TYPE_GPU_ONLY] == 0) {
        device->memoryClassMask[PAL_MEMORY_TYPE_GPU_ONLY] =
            device->memoryClassMask[PAL_MEMORY_TYPE_CPU_UPLOAD];
    }

    // cache shader stages
    device->shaderStages |= VK_SHADER_STAGE_VERTEX_BIT;
    device->shaderStages |= VK_SHADER_STAGE_FRAGMENT_BIT;
    device->shaderStages |= VK_SHADER_STAGE_COMPUTE_BIT;

    if (features & PAL_ADAPTER_FEATURE_GEOMETRY_SHADER) {
        device->shaderStages |= VK_SHADER_STAGE_GEOMETRY_BIT;
    }

    if (features & PAL_ADAPTER_FEATURE_TESSELLATION_SHADER) {
        device->shaderStages |= VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT;
        device->shaderStages |= VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT;
    }

    if (features & PAL_ADAPTER_FEATURE_MESH_SHADER) {
        device->shaderStages |= VK_SHADER_STAGE_TASK_BIT_EXT;
        device->shaderStages |= VK_SHADER_STAGE_MESH_BIT_EXT;
    }

    if (features & PAL_ADAPTER_FEATURE_RAY_TRACING) {
        device->shaderStages |= VK_SHADER_STAGE_RAYGEN_BIT_KHR;
        device->shaderStages |= VK_SHADER_STAGE_ANY_HIT_BIT_KHR;
        device->shaderStages |= VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR;
        device->shaderStages |= VK_SHADER_STAGE_MISS_BIT_KHR;
        device->shaderStages |= VK_SHADER_STAGE_INTERSECTION_BIT_KHR;
        device->shaderStages |= VK_SHADER_STAGE_CALLABLE_BIT_KHR;
    }

    loadFeatureProcs(features, device);
    palFree(s_Vk.allocator, queueFamilyProps);
    palFree(s_Vk.allocator, queueCreateInfos);

    setDebugNameVk(device, VK_OBJECT_TYPE_DEVICE, device->handle);
    device->features = features;
    *outDevice = (PalDevice*)device;
    return PAL_RESULT_SUCCESS;
}

void PAL_CALL destroyDeviceVk(PalDevice* device)
{
    DeviceVk* deviceImpl = (DeviceVk*)device;
    s_Vk.destroyDevice(deviceImpl->handle, &s_Vk.allocatorImpl);
    palFree(s_Vk.allocator, deviceImpl->phyQueues);
    palFree(s_Vk.allocator, deviceImpl);
}

uint32_t PAL_CALL getDeviceLostReasonVk(PalDevice* device)
{
    return 0;
}

PalResult PAL_CALL allocateMemoryVk(
    PalDevice* device,
    PalMemoryType type,
    uint64_t memoryMask,
    uint64_t size,
    PalMemory** outMemory)
{
    VkResult result;
    MemoryVk* memory = nullptr;
    DeviceVk* deviceImpl = (DeviceVk*)device;
    VkMemoryAllocateInfo allocateInfo = {0};
    allocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocateInfo.allocationSize = size;
    VkMemoryAllocateFlagsInfo allocateFlagsInfo = {0};

    memory = palAllocate(s_Vk.allocator, sizeof(MemoryVk), 0);
    if (!memory) {
        return PAL_RESULT_CODE_OUT_OF_MEMORY;
    }

    uint32_t memoryTypeMask = 0;
    uint32_t usages = 0;
    palUnpackUint32(memoryMask, &memoryTypeMask, &usages);
    uint32_t memoryClassMask = deviceImpl->memoryClassMask[type] & memoryTypeMask;

    // pick an index using the scoring system and check if the memory index is valid
    uint32_t memoryIndex = findBestMemoryIndexVk(deviceImpl->phyDevice, memoryClassMask);
    if (!(memoryClassMask & (1u << memoryIndex))) {
        return PAL_RESULT_CODE_PLATFORM_FAILURE;
    }

    allocateInfo.memoryTypeIndex = memoryIndex;
    if (usages & PAL_BUFFER_USAGE_DEVICE_ADDRESS) {
        allocateFlagsInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_FLAGS_INFO;
        allocateFlagsInfo.flags = VK_MEMORY_ALLOCATE_DEVICE_ADDRESS_BIT_KHR;
        allocateInfo.pNext = &allocateFlagsInfo;
    }

    result = s_Vk.allocateMemory(
        deviceImpl->handle,
        &allocateInfo,
        &s_Vk.allocatorImpl,
        &memory->handle);

    if (result != VK_SUCCESS) {
        return makeResultVk(result);
    }

    setDebugNameVk(deviceImpl, VK_OBJECT_TYPE_DEVICE_MEMORY, memory->handle);
    memory->device = deviceImpl;
    memory->type = type;
    *outMemory = (PalMemory*)memory;
    return PAL_RESULT_SUCCESS;
}

void PAL_CALL freeMemoryVk(PalMemory* memory)
{
    MemoryVk* memoryImpl = (MemoryVk*)memory;
    s_Vk.freeMemory(memoryImpl->device->handle, memoryImpl->handle, &s_Vk.allocatorImpl);
    palFree(s_Vk.allocator, memoryImpl);
}

void PAL_CALL querySamplerAnisotropyCapabilitiesVk(
    PalDevice* device,
    PalSamplerAnisotropyCapabilities* caps)
{
    DeviceVk* deviceImpl = (DeviceVk*)device;
    VkPhysicalDeviceProperties props = {0};
    s_Vk.getPhysicalDeviceProperties(deviceImpl->phyDevice, &props);
    caps->maxAnisotropy = props.limits.maxSamplerAnisotropy;
}

void PAL_CALL queryMultiViewCapabilitiesVk(
    PalDevice* device,
    PalMultiViewCapabilities* caps)
{
    DeviceVk* deviceImpl = (DeviceVk*)device;
    VkPhysicalDeviceMultiviewPropertiesKHR props = {0};
    VkPhysicalDeviceProperties2 properties2 = {0};
    props.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MULTIVIEW_PROPERTIES_KHR;
    properties2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
    properties2.pNext = &props;
    s_Vk.getPhysicalDeviceProperties2(deviceImpl->phyDevice, &properties2);

    caps->maxViewCount = props.maxMultiviewViewCount;
    if (caps->maxViewCount == 0) {
        caps->maxViewCount = 1;
    }
}

void PAL_CALL queryMultiViewportCapabilitiesVk(
    PalDevice* device,
    PalMultiViewportCapabilities* caps)
{
    DeviceVk* deviceImpl = (DeviceVk*)device;
    VkPhysicalDeviceProperties props = {0};
    s_Vk.getPhysicalDeviceProperties(deviceImpl->phyDevice, &props);
    caps->maxCount = props.limits.maxViewports;
}

void PAL_CALL queryDepthStencilCapabilitiesVk(
    PalDevice* device,
    PalDepthStencilCapabilities* caps)
{
    DeviceVk* deviceImpl = (DeviceVk*)device;
    VkPhysicalDeviceProperties2 properties2 = {0};
    properties2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
    VkPhysicalDeviceDepthStencilResolvePropertiesKHR props = {0};
    props.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DEPTH_STENCIL_RESOLVE_PROPERTIES_KHR;
    properties2.pNext = &props;
    s_Vk.getPhysicalDeviceProperties2(deviceImpl->phyDevice, &properties2);

    caps->supportsIndependentResolve = props.independentResolve;
    caps->supportsIndependentResolveNone = props.independentResolveNone;

    // depth
    if (props.supportedDepthResolveModes & VK_RESOLVE_MODE_AVERAGE_BIT_KHR) {
        caps->supportedDepthResolveModes |= (1u << PAL_RESOLVE_MODE_AVERAGE);
    }

    if (props.supportedDepthResolveModes & VK_RESOLVE_MODE_SAMPLE_ZERO_BIT_KHR) {
        caps->supportedDepthResolveModes |= (1u << PAL_RESOLVE_MODE_SAMPLE_ZERO);
    }

    if (props.supportedDepthResolveModes & VK_RESOLVE_MODE_MIN_BIT_KHR) {
        caps->supportedDepthResolveModes |= (1u << PAL_RESOLVE_MODE_MIN);
    }

    if (props.supportedDepthResolveModes & VK_RESOLVE_MODE_MAX_BIT_KHR) {
        caps->supportedDepthResolveModes |= (1u << PAL_RESOLVE_MODE_MAX);
    }

    // stencil
    if (props.supportedStencilResolveModes & VK_RESOLVE_MODE_AVERAGE_BIT_KHR) {
        caps->supportedStencilResolveModes |= (1u << PAL_RESOLVE_MODE_AVERAGE);
    }

    if (props.supportedStencilResolveModes & VK_RESOLVE_MODE_SAMPLE_ZERO_BIT_KHR) {
        caps->supportedStencilResolveModes |= (1u << PAL_RESOLVE_MODE_SAMPLE_ZERO);
    }

    if (props.supportedStencilResolveModes & VK_RESOLVE_MODE_MIN_BIT_KHR) {
        caps->supportedStencilResolveModes |= (1u << PAL_RESOLVE_MODE_MIN);
    }

    if (props.supportedStencilResolveModes & VK_RESOLVE_MODE_MAX_BIT_KHR) {
        caps->supportedStencilResolveModes |= (1u << PAL_RESOLVE_MODE_MAX);
    }
}

void PAL_CALL queryFragmentShadingRateCapabilitiesVk(
    PalDevice* device,
    PalFragmentShadingRateCapabilities* caps)
{
    DeviceVk* deviceImpl = (DeviceVk*)device;
    VkPhysicalDeviceProperties2 properties2 = {0};
    properties2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
    VkPhysicalDeviceFragmentShadingRatePropertiesKHR props = {0};
    props.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FRAGMENT_SHADING_RATE_PROPERTIES_KHR;
    properties2.pNext = &props;
    s_Vk.getPhysicalDeviceProperties2(deviceImpl->phyDevice, &properties2);

    memset(caps, 0, sizeof(PalFragmentShadingRateCapabilities));
    for (int i = 0; i < PAL_FRAGMENT_SHADING_RATE_COUNT; i++) {
        VkExtent2D size = getShadingRateSizeVk((PalFragmentShadingRate)i);

        // check against the max size
        if (size.width <= props.maxFragmentSize.width ||
            size.height <= props.maxFragmentSize.height) {
            caps->supportedShadingRates |= (1u << i);
        }
    }

    caps->supportedShadingRates |= (1u << PAL_FRAGMENT_SHADING_RATE_COMBINER_OP_KEEP);
    caps->supportedShadingRates |= (1u << PAL_FRAGMENT_SHADING_RATE_COMBINER_OP_REPLACE);
    if (props.fragmentShadingRateNonTrivialCombinerOps) {
        caps->supportedShadingRates |= (1u << PAL_FRAGMENT_SHADING_RATE_COMBINER_OP_MIN);
        caps->supportedShadingRates |= (1u << PAL_FRAGMENT_SHADING_RATE_COMBINER_OP_MAX);
        caps->supportedShadingRates |= (1u << PAL_FRAGMENT_SHADING_RATE_COMBINER_OP_MUL);
    }

    VkExtent2D size = props.minFragmentShadingRateAttachmentTexelSize;
    caps->minTexelWidth = size.width;
    caps->minTexelHeight = size.height;
    size = props.maxFragmentShadingRateAttachmentTexelSize;
    caps->maxTexelWidth = size.width;
    caps->maxTexelHeight = size.height;
}

void PAL_CALL queryMeshShaderCapabilitiesVk(
    PalDevice* device,
    PalMeshShaderCapabilities* caps)
{
    DeviceVk* deviceImpl = (DeviceVk*)device;
    VkPhysicalDeviceProperties2 properties2 = {0};
    properties2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
    VkPhysicalDeviceMeshShaderPropertiesEXT props = {0};
    props.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MESH_SHADER_PROPERTIES_EXT;
    properties2.pNext = &props;
    s_Vk.getPhysicalDeviceProperties2(deviceImpl->phyDevice, &properties2);

    caps->maxOutputPrimitives = props.maxMeshOutputPrimitives;
    caps->maxOutputVertices = props.maxMeshOutputVertices;
    caps->maxTaskWorkGroupInvocations = props.maxTaskWorkGroupInvocations;
    caps->maxWorkGroupInvocations = props.maxMeshWorkGroupInvocations;

    caps->maxTaskWorkGroupCount[0] = props.maxTaskWorkGroupCount[0];
    caps->maxTaskWorkGroupCount[1] = props.maxTaskWorkGroupCount[1];
    caps->maxTaskWorkGroupCount[2] = props.maxTaskWorkGroupCount[2];

    caps->maxWorkGroupCount[0] = props.maxMeshWorkGroupCount[0];
    caps->maxWorkGroupCount[1] = props.maxMeshWorkGroupCount[1];
    caps->maxWorkGroupCount[2] = props.maxMeshWorkGroupCount[2];
}

void PAL_CALL queryRayTracingCapabilitiesVk(
    PalDevice* device,
    PalRayTracingCapabilities* caps)
{
    DeviceVk* deviceImpl = (DeviceVk*)device;
    VkPhysicalDeviceProperties2 properties2 = {0};
    properties2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
    VkPhysicalDeviceRayTracingPipelinePropertiesKHR props = {0};
    props.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_TRACING_PIPELINE_PROPERTIES_KHR;
    VkPhysicalDeviceAccelerationStructurePropertiesKHR accProps = {0};
    accProps.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ACCELERATION_STRUCTURE_PROPERTIES_KHR;
    props.pNext = &accProps;
    properties2.pNext = &props;
    s_Vk.getPhysicalDeviceProperties2(deviceImpl->phyDevice, &properties2);

    caps->maxRecursionDepth = props.maxRayRecursionDepth;
    caps->maxHitAttributeSize = props.maxRayHitAttributeSize;
    caps->maxInstanceCount = accProps.maxInstanceCount;
    caps->maxPrimitiveCount = accProps.maxPrimitiveCount;
    caps->maxGeometryCount = accProps.maxGeometryCount;

    caps->maxPayloadSize = 64; // safe
    caps->maxDispatchInvocations = props.maxRayDispatchInvocationCount;
}

void PAL_CALL queryDescriptorIndexingCapabilitiesVk(
    PalDevice* device,
    PalDescriptorIndexingCapabilities* caps)
{
    DeviceVk* deviceImpl = (DeviceVk*)device;
    VkPhysicalDeviceDescriptorIndexingFeaturesEXT desc = {0};
    desc.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_FEATURES_EXT;

    VkPhysicalDeviceProperties2 properties2 = {0};
    properties2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;

    VkPhysicalDeviceDescriptorIndexingPropertiesEXT props = {0};
    props.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_PROPERTIES_EXT;

    VkPhysicalDeviceFeatures2 features;
    features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;

    VkPhysicalDeviceAccelerationStructurePropertiesKHR accProps = {0};
    accProps.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ACCELERATION_STRUCTURE_PROPERTIES_KHR;

    features.pNext = &desc;
    props.pNext = &accProps;
    properties2.pNext = &props;
    s_Vk.getPhysicalDeviceFeatures2(deviceImpl->phyDevice, &features);
    s_Vk.getPhysicalDeviceProperties2(deviceImpl->phyDevice, &properties2);

    caps->flags = 0;
    if (desc.descriptorBindingPartiallyBound) {
        caps->flags |= PAL_DESCRIPTOR_INDEXING_FLAG_PARTIALLY_BOUND;
    }

    // clang-format off
    if (desc.descriptorBindingUpdateUnusedWhilePending && 
        desc.descriptorBindingSampledImageUpdateAfterBind &&
        desc.descriptorBindingStorageImageUpdateAfterBind &&
        desc.descriptorBindingStorageBufferUpdateAfterBind &&
        desc.descriptorBindingUniformBufferUpdateAfterBind) {
        caps->flags |= PAL_DESCRIPTOR_INDEXING_FLAG_UPDATE_AFTER_BIND;
    }

    if (desc.shaderSampledImageArrayNonUniformIndexing && 
        desc.shaderStorageImageArrayNonUniformIndexing &&
        desc.shaderStorageBufferArrayNonUniformIndexing &&
        desc.shaderUniformBufferArrayNonUniformIndexing) {
        caps->flags |= PAL_DESCRIPTOR_INDEXING_FLAG_NON_UNIFORM_INDEXING;
    }
    // clang-format on

    caps->maxPerStageSampledImages = props.maxPerStageDescriptorUpdateAfterBindSampledImages;
    caps->maxPerSetSampledImages = props.maxDescriptorSetUpdateAfterBindSampledImages;
    caps->maxPerStageStorageImages = props.maxPerStageDescriptorUpdateAfterBindStorageImages;
    caps->maxPerSetStorageImages = props.maxDescriptorSetUpdateAfterBindStorageImages;

    caps->maxPerStageSamplers = props.maxPerStageDescriptorUpdateAfterBindSamplers;
    caps->maxPerSetSamplers = props.maxDescriptorSetUpdateAfterBindSamplers;
    caps->maxPerStageStorageBuffers = props.maxPerStageDescriptorUpdateAfterBindStorageBuffers;
    caps->maxPerSetStorageBuffers = props.maxDescriptorSetUpdateAfterBindStorageBuffers;

    caps->maxPerStageUniformBuffers = props.maxPerStageDescriptorUpdateAfterBindUniformBuffers;
    caps->maxPerSetUniformBuffers = props.maxDescriptorSetUpdateAfterBindUniformBuffers;

    uint32_t tmp = accProps.maxPerStageDescriptorUpdateAfterBindAccelerationStructures;
    uint32_t tmp2 = accProps.maxDescriptorSetUpdateAfterBindAccelerationStructures;
    caps->maxPerStageAccelerationStructure = tmp;
    caps->maxPerSetAccelerationStructure = tmp2;
}

PalResult PAL_CALL createQueueVk(
    PalDevice* device,
    PalQueueType type,
    PalQueue** outQueue)
{
    DeviceVk* deviceImpl = (DeviceVk*)device;
    VkQueueFlags queueFlag = 0;
    QueueVk* queue = nullptr;
    PalPipelineStages stages = 0;

    if (deviceImpl->phyQueueCount == 0) {
        return PAL_RESULT_CODE_OUT_OF_MEMORY;
    }

    switch (type) {
        case PAL_QUEUE_TYPE_COMPUTE: {
            queueFlag = VK_QUEUE_COMPUTE_BIT;
            stages = PAL_PIPELINE_STAGE_TRANSFER;
            stages |= PAL_PIPELINE_STAGE_COMPUTE_SHADER;
            stages |= PAL_PIPELINE_STAGE_INDIRECT_INPUT;

            stages |= PAL_PIPELINE_STAGE_HOST;
            stages |= PAL_PIPELINE_STAGE_RAY_TRACING_SHADER;
            stages |= PAL_PIPELINE_STAGE_ACCELERATION_STRUCTURE_BUILD;
            break;
        }

        case PAL_QUEUE_TYPE_GRAPHICS: {
            queueFlag = VK_QUEUE_GRAPHICS_BIT;
            stages = PAL_PIPELINE_STAGE_TRANSFER;
            stages |= PAL_PIPELINE_STAGE_VERTEX_SHADER;
            stages |= PAL_PIPELINE_STAGE_FRAGMENT_SHADER;

            stages |= PAL_PIPELINE_STAGE_GEOMETRY_SHADER;
            stages |= PAL_PIPELINE_STAGE_TESSELLATION_CONTROL_SHADER;
            stages |= PAL_PIPELINE_STAGE_TESSELLATION_EVALUATION_SHADER;
            stages |= PAL_PIPELINE_STAGE_TASK_SHADER;

            stages |= PAL_PIPELINE_STAGE_MESH_SHADER;
            stages |= PAL_PIPELINE_STAGE_VERTEX_INPUT;
            stages |= PAL_PIPELINE_STAGE_INDEX_INPUT;
            stages |= PAL_PIPELINE_STAGE_EARLY_DEPTH_STENCIL;

            stages |= PAL_PIPELINE_STAGE_LATE_DEPTH_STENCIL;
            stages |= PAL_PIPELINE_STAGE_COLOR_ATTACHMENT;
            stages |= PAL_PIPELINE_STAGE_FRAGMENT_SHADING_RATE_ATTACHMENT;
            stages |= PAL_PIPELINE_STAGE_INDIRECT_INPUT;

            stages |= PAL_PIPELINE_STAGE_HOST;
            stages |= PAL_PIPELINE_STAGE_COMPUTE_SHADER;
            stages |= PAL_PIPELINE_STAGE_RAY_TRACING_SHADER;
            stages |= PAL_PIPELINE_STAGE_ACCELERATION_STRUCTURE_BUILD;
            break;
        }

        case PAL_QUEUE_TYPE_COPY: {
            queueFlag = VK_QUEUE_TRANSFER_BIT;
            stages = PAL_PIPELINE_STAGE_TRANSFER;
            break;
        }
    }

    // we index the for loop so we dont always start at the beginning, this way
    // we cycle through all queue families each time we create a queue
    PhysicalQueue* phyQueue = nullptr;
    for (int i = deviceImpl->phyQueueIndex; i < deviceImpl->phyQueueCount; i++) {
        PhysicalQueue* pq = &deviceImpl->phyQueues[i];
        // check if the physical queue supports the requested operation
        // and if its not already used
        if (pq->usages & queueFlag && pq->usedUsages != queueFlag) {
            pq->usedUsages |= queueFlag;
            phyQueue = pq;
            break;
        }
    }

    if (!phyQueue) {
        // we didnt get any queue, we check if we started the loop at the beginning or mid way
        if (deviceImpl->phyQueueIndex == 0) {
            // we searched all queue families
            return PAL_RESULT_CODE_OUT_OF_MEMORY;

        } else {
            // we start at the beginning and go through the queue families again
            deviceImpl->phyQueueIndex = 0;
            for (int i = deviceImpl->phyQueueIndex; i < deviceImpl->phyQueueCount; i++) {
                PhysicalQueue* pq = &deviceImpl->phyQueues[i];
                if (pq->usages & queueFlag && pq->usedUsages != queueFlag) {
                    pq->usedUsages |= queueFlag;
                    phyQueue = pq;
                    break;
                }
            }

            if (!phyQueue) {
                return PAL_RESULT_CODE_OUT_OF_MEMORY;
            }
        }
    }

    queue = palAllocate(s_Vk.allocator, sizeof(QueueVk), 0);
    if (!queue) {
        return PAL_RESULT_CODE_OUT_OF_MEMORY;
    }

    setDebugNameVk(deviceImpl, VK_OBJECT_TYPE_QUEUE, queue->phyQueue->handle);
    deviceImpl->phyQueueIndex = (deviceImpl->phyQueueIndex + 1) % deviceImpl->phyQueueCount;
    queue->phyQueue = phyQueue;
    queue->usage = queueFlag;
    queue->device = deviceImpl;

    queue->supportedStages = stages;
    *outQueue = (PalQueue*)queue;
    return PAL_RESULT_SUCCESS;
}

void PAL_CALL destroyQueueVk(PalQueue* queue)
{
    QueueVk* queueImpl = (QueueVk*)queue;
    PhysicalQueue* phyQueue = queueImpl->phyQueue;
    phyQueue->usedUsages &= ~queueImpl->usage;
    palFree(s_Vk.allocator, queueImpl);
}

PalResult PAL_CALL waitQueueVk(PalQueue* queue)
{
    QueueVk* queueImpl = (QueueVk*)queue;
    VkResult result = s_Vk.waitQueue(queueImpl->phyQueue->handle);
    if (result != VK_SUCCESS) {
        return makeResultVk(result);
    }

    return PAL_RESULT_SUCCESS;
}

PalBool PAL_CALL canQueuePresentVk(
    PalQueue* queue,
    PalSurface* surface)
{
    VkResult result;
    QueueVk* queueImpl = (QueueVk*)queue;
    PhysicalQueue* phyQueue = queueImpl->phyQueue;
    SurfaceVk* surfaceImpl = (SurfaceVk*)surface;

    // check if the queue is a graphics queue before we check its family
    // index for presentation support.
    if (queueImpl->usage != VK_QUEUE_GRAPHICS_BIT) {
        return PAL_FALSE;
    }

    VkBool32 supported = PAL_FALSE;
    result = s_Vk.checkSurfaceSupport(
        phyQueue->phyDevice,
        phyQueue->familyIndex,
        surfaceImpl->handle,
        &supported);

    if (result == VK_SUCCESS && supported) {
        return PAL_TRUE;
    }

    return PAL_FALSE;
}

PalBool PAL_CALL canQueueShareOwnershipVk(
    PalQueue* a,
    PalQueue* b)
{
    QueueVk* queueImplA = (QueueVk*)a;
    QueueVk* queueImplB = (QueueVk*)b;
    return queueImplA->phyQueue->familyIndex == queueImplB->phyQueue->familyIndex;
}

PalBool PAL_CALL canQueueUseUsageStateVk(
    PalQueue* queue,
    PalUsageState state)
{
    QueueVk* queueImpl = (QueueVk*)queue;
    if (queueImpl->usage == VK_QUEUE_TRANSFER_BIT) {
        switch (state) {
            case PAL_USAGE_STATE_TRANSFER_READ:
            case PAL_USAGE_STATE_TRANSFER_WRITE: {
                return PAL_TRUE;
            }
        }
        return PAL_FALSE;

    } else if (queueImpl->usage == VK_QUEUE_COMPUTE_BIT) {
        switch (state) {
            case PAL_USAGE_STATE_TRANSFER_READ:
            case PAL_USAGE_STATE_TRANSFER_WRITE:
            case PAL_USAGE_STATE_HOST_READ:
            case PAL_USAGE_STATE_HOST_WRITE:
            case PAL_USAGE_STATE_INDIRECT_READ:
            case PAL_USAGE_STATE_UNIFORM_READ:
            case PAL_USAGE_STATE_SHADER_READ:
            case PAL_USAGE_STATE_SHADER_WRITE:
            case PAL_USAGE_STATE_STORAGE_READ:
            case PAL_USAGE_STATE_STORAGE_WRITE:
            case PAL_USAGE_STATE_ACCELERATION_STRUCTURE_READ:
            case PAL_USAGE_STATE_ACCELERATION_STRUCTURE_WRITE: {
                return PAL_TRUE;
            }
        }
        return PAL_FALSE;

    } else if (queueImpl->usage == VK_QUEUE_GRAPHICS_BIT) {
        switch (state) {
            case PAL_USAGE_STATE_TRANSFER_READ:
            case PAL_USAGE_STATE_TRANSFER_WRITE:
            case PAL_USAGE_STATE_HOST_READ:
            case PAL_USAGE_STATE_HOST_WRITE:
            case PAL_USAGE_STATE_INDIRECT_READ:
            case PAL_USAGE_STATE_UNIFORM_READ:
            case PAL_USAGE_STATE_SHADER_READ:
            case PAL_USAGE_STATE_SHADER_WRITE:
            case PAL_USAGE_STATE_STORAGE_READ:
            case PAL_USAGE_STATE_STORAGE_WRITE:
            case PAL_USAGE_STATE_ACCELERATION_STRUCTURE_READ:
            case PAL_USAGE_STATE_ACCELERATION_STRUCTURE_WRITE:
            case PAL_USAGE_STATE_PRESENT:
            case PAL_USAGE_STATE_COLOR_ATTACHMENT_WRITE:
            case PAL_USAGE_STATE_DEPTH_ATTACHMENT_READ:
            case PAL_USAGE_STATE_DEPTH_ATTACHMENT_WRITE:
            case PAL_USAGE_STATE_STENCIL_ATTACHMENT_READ:
            case PAL_USAGE_STATE_STENCIL_ATTACHMENT_WRITE:
            case PAL_USAGE_STATE_FRAGMENT_SHADING_RATE_ATTACHMENT_READ:
            case PAL_USAGE_STATE_VERTEX_READ:
            case PAL_USAGE_STATE_INDEX_READ: {
                return PAL_TRUE;
            }
        }
        return PAL_FALSE;
    }
    return PAL_FALSE;
}

PalBool PAL_CALL canQueueUsePipelineStagesVk(
    PalQueue* queue,
    PalPipelineStages stages)
{
    QueueVk* queueImpl = (QueueVk*)queue;
    return (queueImpl->supportedStages & stages) == stages;
}

PalResult PAL_CALL createShaderVk(
    PalDevice* device,
    const PalShaderCreateInfo* info,
    PalShader** outShader)
{
    VkResult result;
    ShaderVk* shader = nullptr;
    DeviceVk* deviceImpl = (DeviceVk*)device;

    shader = palAllocate(s_Vk.allocator, sizeof(ShaderVk), 0);
    if (!shader) {
        return PAL_RESULT_CODE_OUT_OF_MEMORY;
    }

    // allocate entries array
    shader->entries = palAllocate(s_Vk.allocator, sizeof(ShaderEntry) * info->entryCount, 0);
    if (!shader->entries) {
        return PAL_RESULT_CODE_OUT_OF_MEMORY;
    }

    for (int i = 0; i < info->entryCount; i++) {
        ShaderEntry* entry = &shader->entries[i];
        strncpy(entry->entryName, info->entries[i].entryName, PAL_SHADER_ENTRY_NAME_SIZE);
        entry->entryName[PAL_SHADER_ENTRY_NAME_SIZE - 1] = '\0';
        entry->patchControlPoints = info->entries[i].patchControlPoints;
        entry->stage = shaderStageToVK(info->entries[i].stage);
    }

    VkShaderModuleCreateInfo createInfo = {0};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.codeSize = info->codeSize;
    createInfo.pCode = (const uint32_t*)info->code;

    result =
        s_Vk.createShader(deviceImpl->handle, &createInfo, &s_Vk.allocatorImpl, &shader->handle);
    if (result != VK_SUCCESS) {
        palFree(s_Vk.allocator, shader);
        return makeResultVk(result);
    }

    setDebugNameVk(deviceImpl, VK_OBJECT_TYPE_SHADER_MODULE, shader->handle);
    shader->device = deviceImpl;
    shader->entryCount = info->entryCount;
    *outShader = (PalShader*)shader;
    return PAL_RESULT_SUCCESS;
}

void PAL_CALL destroyShaderVk(PalShader* shader)
{
    ShaderVk* shaderImpl = (ShaderVk*)shader;
    s_Vk.destroyShader(shaderImpl->device->handle, shaderImpl->handle, &s_Vk.allocatorImpl);
    palFree(s_Vk.allocator, shaderImpl->entries);
    palFree(s_Vk.allocator, shaderImpl);
}

#endif // PAL_HAS_VULKAN_BACKEND