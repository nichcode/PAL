
/**
 PAL - Prime Abstraction Layer
 Copyright (C) 2025
 Licensed under the Zlib license. See LICENSE file in root.
 */

#include "dumps.h"
#include "pal/pal_graphics.h"

static PalBool adapterDump(uint32_t flags)
{
    // clang-format off
    FieldInfo adapterInfoFields[] = {
        { "vram", {0, 8}, FIELD(PalAdapterInfo, vram) },
        { "sharedMemory", {8, 8}, FIELD(PalAdapterInfo, sharedMemory) },
        { "vendorId", {16, 4}, FIELD(PalAdapterInfo, vendorId) },
        { "deviceId", {20, 4}, FIELD(PalAdapterInfo, deviceId) },
        { "driverVersion", {24, 4}, FIELD(PalAdapterInfo, driverVersion) },
        { "shaderFormats", {28, 4}, FIELD(PalAdapterInfo, shaderFormats) },
        { "type", {32, 4}, FIELD(PalAdapterInfo, type) },
        { "apiType", {36, 4}, FIELD(PalAdapterInfo, apiType) },
        { "name", {40, 128}, FIELD(PalAdapterInfo, name) },
        { "backendName", {168, 32}, FIELD(PalAdapterInfo, backendName) }
    };

    FieldInfo imageCapFields[] = {
        { "maxWidth", {0, 4}, FIELD(PalImageCapabilities, maxWidth) },
        { "maxHeight", {4, 4}, FIELD(PalImageCapabilities, maxHeight) },
        { "maxDepth", {8, 4}, FIELD(PalImageCapabilities, maxDepth) },
        { "maxArrayLayers", {12, 4}, FIELD(PalImageCapabilities, maxArrayLayers) },
        { "maxMipLevels", {16, 4}, FIELD(PalImageCapabilities, maxMipLevels) }
    };

    FieldInfo resourceCapFields[] = {
        { "maxPerStageSampledImages", {0, 4}, FIELD(PalResourceCapabilities, maxPerStageSampledImages) },
        { "maxPerSetSampledImages", {4, 4}, FIELD(PalResourceCapabilities, maxPerSetSampledImages) },
        { "maxPerStageStorageImages", {8, 4}, FIELD(PalResourceCapabilities, maxPerStageStorageImages) },
        { "maxPerSetStorageImages", {12, 4}, FIELD(PalResourceCapabilities, maxPerSetStorageImages) },
        { "maxPerStageSamplers", {16, 4}, FIELD(PalResourceCapabilities, maxPerStageSamplers) },
        { "maxPerSetSamplers", {20, 4}, FIELD(PalResourceCapabilities, maxPerSetSamplers) },
        { "maxPerStageStorageBuffers", {24, 4}, FIELD(PalResourceCapabilities, maxPerStageStorageBuffers) },
        { "maxPerSetStorageBuffers", {28, 4}, FIELD(PalResourceCapabilities, maxPerSetStorageBuffers) },
        { "maxPerStageUniformBuffers", {32, 4}, FIELD(PalResourceCapabilities, maxPerStageUniformBuffers) },
        { "maxPerSetUniformBuffers", {36, 4}, FIELD(PalResourceCapabilities, maxPerSetUniformBuffers) },
        { "maxPerStageAccelerationStructure", {40, 4}, FIELD(PalResourceCapabilities, maxPerStageAccelerationStructure) },
        { "maxPerSetAccelerationStructure", {44, 4}, FIELD(PalResourceCapabilities, maxPerSetAccelerationStructure) },
        { "maxBoundSets", {48, 4}, FIELD(PalResourceCapabilities, maxBoundSets) },
    };

    FieldInfo computeCapFields[] = {
        { "maxWorkGroupInvocations", {0, 4}, FIELD(PalComputeCapabilities, maxWorkGroupInvocations) },
        { "maxWorkGroupCount", {4, 12}, FIELD(PalComputeCapabilities, maxWorkGroupCount) },
        { "maxWorkGroupSize", {16, 12}, FIELD(PalComputeCapabilities, maxWorkGroupSize) }
    };
    
    FieldInfo viewportCapFields[] = {
        { "maxWidth", {0, 4}, FIELD(PalViewportCapabilities, maxWidth) },
        { "maxHeight", {4, 4}, FIELD(PalViewportCapabilities, maxHeight) },
        { "minBoundsRange", {8, 4}, FIELD(PalViewportCapabilities, minBoundsRange) },
        { "maxBoundsRange", {12, 4}, FIELD(PalViewportCapabilities, maxBoundsRange) }
    };

    FieldInfo adapterCapFields[] = {
        { "maxComputeQueues", {0, 4}, FIELD(PalAdapterCapabilities, maxComputeQueues) },
        { "maxGraphicsQueues", {4, 4}, FIELD(PalAdapterCapabilities, maxGraphicsQueues) },
        { "maxCopyQueues", {8, 4}, FIELD(PalAdapterCapabilities, maxCopyQueues) },
        { "maxColorAttachments", {12, 4}, FIELD(PalAdapterCapabilities, maxColorAttachments) },
        { "maxUniformBufferSize", {16, 4}, FIELD(PalAdapterCapabilities, maxUniformBufferSize) },
        { "maxStorageBufferSize", {20, 4}, FIELD(PalAdapterCapabilities, maxStorageBufferSize) },
        { "maxPushConstantSize", {24, 4}, FIELD(PalAdapterCapabilities, maxPushConstantSize) },
        { "maxVertexLayouts", {28, 4}, FIELD(PalAdapterCapabilities, maxVertexLayouts) },
        { "maxVertexAttributes", {32, 4}, FIELD(PalAdapterCapabilities, maxVertexAttributes) },
        { "maxTessellationPatchPoint", {36, 4}, FIELD(PalAdapterCapabilities, maxTessellationPatchPoint) },
        { "viewportCaps", {40, 16}, FIELD(PalAdapterCapabilities, viewportCaps) },
        { "imageCaps", {56, 20}, FIELD(PalAdapterCapabilities, imageCaps) },
        { "resourceCaps", {76, 52}, FIELD(PalAdapterCapabilities, resourceCaps) },
        { "computeCaps", {128, 28}, FIELD(PalAdapterCapabilities, computeCaps) }
    };

    FieldInfo formatInfoFields[] = {
        { "usages", {0, 4}, FIELD(PalFormatInfo, usages) },
        { "format", {4, 4}, FIELD(PalFormatInfo, format) },
        { "sampleCount", {8, 4}, FIELD(PalFormatInfo, sampleCount) }
    };
    // clang-format on

    StructInfo adapterInfo = {0};
    adapterInfo.name = "PalAdapterInfo";
    adapterInfo.fields = adapterInfoFields;
    adapterInfo.fieldCount = ARRAY_SIZE(adapterInfoFields);
    adapterInfo.expected.alignof = 8;
    adapterInfo.expected.size = 200;
    adapterInfo.expected.padding = 0;
    adapterInfo.actual = STRUCT(PalAdapterInfo);

    StructInfo imageCap = {0};
    imageCap.name = "PalImageCapabilities";
    imageCap.fields = imageCapFields;
    imageCap.fieldCount = ARRAY_SIZE(imageCapFields);
    imageCap.expected.alignof = 4;
    imageCap.expected.size = 20;
    imageCap.expected.padding = 0;
    imageCap.actual = STRUCT(PalImageCapabilities);

    StructInfo resourceCap = {0};
    resourceCap.name = "PalResourceCapabilities";
    resourceCap.fields = resourceCapFields;
    resourceCap.fieldCount = ARRAY_SIZE(resourceCapFields);
    resourceCap.expected.alignof = 4;
    resourceCap.expected.size = 52;
    resourceCap.expected.padding = 0;
    resourceCap.actual = STRUCT(PalResourceCapabilities);

    StructInfo computeCap = {0};
    computeCap.name = "PalComputeCapabilities";
    computeCap.fields = computeCapFields;
    computeCap.fieldCount = ARRAY_SIZE(computeCapFields);
    computeCap.expected.alignof = 4;
    computeCap.expected.size = 28;
    computeCap.expected.padding = 0;
    computeCap.actual = STRUCT(PalComputeCapabilities);

    StructInfo viewportCap = {0};
    viewportCap.name = "PalViewportCapabilities";
    viewportCap.fields = viewportCapFields;
    viewportCap.fieldCount = ARRAY_SIZE(viewportCapFields);
    viewportCap.expected.alignof = 4;
    viewportCap.expected.size = 16;
    viewportCap.expected.padding = 0;
    viewportCap.actual = STRUCT(PalViewportCapabilities);

    StructInfo adapterCap = {0};
    adapterCap.name = "PalAdapterCapabilities";
    adapterCap.fields = adapterCapFields;
    adapterCap.fieldCount = ARRAY_SIZE(adapterCapFields);
    adapterCap.expected.alignof = 4;
    adapterCap.expected.size = 156;
    adapterCap.expected.padding = 0;
    adapterCap.actual = STRUCT(PalAdapterCapabilities);

    StructInfo formatInfo = {0};
    formatInfo.name = "PalFormatInfo";
    formatInfo.fields = formatInfoFields;
    formatInfo.fieldCount = ARRAY_SIZE(formatInfoFields);
    formatInfo.expected.alignof = 4;
    formatInfo.expected.size = 12;
    formatInfo.expected.padding = 0;
    formatInfo.actual = STRUCT(PalFormatInfo);

    PalBool status = checkABI(&adapterInfo, flags);
    if (status == PAL_FALSE) {
        return status;
    }

    status = checkABI(&imageCap, flags);
    if (status == PAL_FALSE) {
        return status;
    }

    status = checkABI(&resourceCap, flags);
    if (status == PAL_FALSE) {
        return status;
    }

    status = checkABI(&computeCap, flags);
    if (status == PAL_FALSE) {
        return status;
    }

    status = checkABI(&viewportCap, flags);
    if (status == PAL_FALSE) {
        return status;
    }

    status = checkABI(&adapterCap, flags);
    if (status == PAL_FALSE) {
        return status;
    }

    return checkABI(&formatInfo, flags);
}

static PalBool deviceDump(uint32_t flags)
{
    // clang-format off
    FieldInfo samplerAnisotropyCapFields[] = {
        { "maxAnisotropy", {0, 4}, FIELD(PalSamplerAnisotropyCapabilities, maxAnisotropy) }
    };

    FieldInfo multiViewCapFields[] = {
        { "maxViewCount", {0, 4}, FIELD(PalMultiViewCapabilities, maxViewCount) }
    };

    FieldInfo multiViewportCapFields[] = {
        { "maxCount", {0, 4}, FIELD(PalMultiViewportCapabilities, maxCount) }
    };

    FieldInfo depthStencilCapFields[] = {
        { "supportedDepthResolveModes", {0, 4}, FIELD(PalDepthStencilCapabilities, supportedDepthResolveModes) },
        { "supportedStencilResolveModes", {4, 4}, FIELD(PalDepthStencilCapabilities, supportedStencilResolveModes) },
        { "supportsIndependentResolve", {8, 4}, FIELD(PalDepthStencilCapabilities, supportsIndependentResolve) },
        { "supportsIndependentResolveNone", {12, 4}, FIELD(PalDepthStencilCapabilities, supportsIndependentResolveNone) }
    };

    FieldInfo fragmentShadingRateCapFields[] = {
        { "supportedShadingRates", {0, 4}, FIELD(PalFragmentShadingRateCapabilities, supportedShadingRates) },
        { "supportedCombinerOps", {4, 4}, FIELD(PalFragmentShadingRateCapabilities, supportedCombinerOps) },
        { "minTexelWidth", {8, 4}, FIELD(PalFragmentShadingRateCapabilities, minTexelWidth) },
        { "minTexelHeight", {12, 4}, FIELD(PalFragmentShadingRateCapabilities, minTexelHeight) },
        { "maxTexelWidth", {16, 4}, FIELD(PalFragmentShadingRateCapabilities, maxTexelWidth) },
        { "maxTexelHeight", {20, 4}, FIELD(PalFragmentShadingRateCapabilities, maxTexelHeight) }
    };

    FieldInfo meshCapFields[] = {
        { "maxOutputPrimitives", {0, 4}, FIELD(PalMeshShaderCapabilities, maxOutputPrimitives) },
        { "maxOutputVertices", {4, 4}, FIELD(PalMeshShaderCapabilities, maxOutputVertices) },
        { "maxWorkGroupInvocations", {8, 4}, FIELD(PalMeshShaderCapabilities, maxWorkGroupInvocations) },
        { "maxTaskWorkGroupInvocations", {12, 4}, FIELD(PalMeshShaderCapabilities, maxTaskWorkGroupInvocations) },
        { "maxWorkGroupCount", {16, 12}, FIELD(PalMeshShaderCapabilities, maxWorkGroupCount) },
        { "maxTaskWorkGroupCount", {28, 12}, FIELD(PalMeshShaderCapabilities, maxTaskWorkGroupCount) }
    };

    FieldInfo rayTracingCapFields[] = {
        { "maxRecursionDepth", {0, 4}, FIELD(PalRayTracingCapabilities, maxRecursionDepth) },
        { "maxHitAttributeSize", {4, 4}, FIELD(PalRayTracingCapabilities, maxHitAttributeSize) },
        { "maxInstanceCount", {8, 4}, FIELD(PalRayTracingCapabilities, maxInstanceCount) },
        { "maxPrimitiveCount", {12, 4}, FIELD(PalRayTracingCapabilities, maxPrimitiveCount) },
        { "maxGeometryCount", {16, 4}, FIELD(PalRayTracingCapabilities, maxGeometryCount) },
        { "maxPayloadSize", {20, 4}, FIELD(PalRayTracingCapabilities, maxPayloadSize) },
        { "maxDispatchInvocations", {24, 4}, FIELD(PalRayTracingCapabilities, maxDispatchInvocations) }
    };

    FieldInfo descriptorIndexingCapFields[] = {
        { "flags", {0, 4}, FIELD(PalDescriptorIndexingCapabilities, flags) },
        { "maxPerStageSampledImages", {4, 4}, FIELD(PalDescriptorIndexingCapabilities, maxPerStageSampledImages) },
        { "maxPerSetSampledImages", {8, 4}, FIELD(PalDescriptorIndexingCapabilities, maxPerSetSampledImages) },
        { "maxPerStageStorageImages", {12, 4}, FIELD(PalDescriptorIndexingCapabilities, maxPerStageStorageImages) },
        { "maxPerSetStorageImages", {16, 4}, FIELD(PalDescriptorIndexingCapabilities, maxPerSetStorageImages) },
        { "maxPerStageSamplers", {20, 4}, FIELD(PalDescriptorIndexingCapabilities, maxPerStageSamplers) },
        { "maxPerSetSamplers", {24, 4}, FIELD(PalDescriptorIndexingCapabilities, maxPerSetSamplers) },
        { "maxPerStageStorageBuffers", {28, 4}, FIELD(PalDescriptorIndexingCapabilities, maxPerStageStorageBuffers) },
        { "maxPerSetStorageBuffers", {32, 4}, FIELD(PalDescriptorIndexingCapabilities, maxPerSetStorageBuffers) },
        { "maxPerStageUniformBuffers", {36, 4}, FIELD(PalDescriptorIndexingCapabilities, maxPerStageUniformBuffers) },
        { "maxPerSetUniformBuffers", {40, 4}, FIELD(PalDescriptorIndexingCapabilities, maxPerSetUniformBuffers) },
        { "maxPerStageAccelerationStructure", {44, 4}, FIELD(PalDescriptorIndexingCapabilities, maxPerStageAccelerationStructure) },
        { "maxPerSetAccelerationStructure", {48, 4}, FIELD(PalDescriptorIndexingCapabilities, maxPerSetAccelerationStructure) }
    };

    FieldInfo memoryRequirementFields[] = {
        { "size", {0, 8}, FIELD(PalMemoryRequirements, size) },
        { "alignment", {8, 8}, FIELD(PalMemoryRequirements, alignment) },
        { "memoryMask", {16, 8}, FIELD(PalMemoryRequirements, memoryMask) },
        { "supportedMemoryTypes", {24, 4}, FIELD(PalMemoryRequirements, supportedMemoryTypes) },
        { "reserved", {28, 4}, FIELD(PalMemoryRequirements, reserved) }
    };
    // clang-format on

    StructInfo samplerAnisotropyCap = {0};
    samplerAnisotropyCap.name = "PalSamplerAnisotropyCapabilities";
    samplerAnisotropyCap.fields = samplerAnisotropyCapFields;
    samplerAnisotropyCap.fieldCount = ARRAY_SIZE(samplerAnisotropyCapFields);
    samplerAnisotropyCap.expected.alignof = 4;
    samplerAnisotropyCap.expected.size = 4;
    samplerAnisotropyCap.expected.padding = 0;
    samplerAnisotropyCap.actual = STRUCT(PalSamplerAnisotropyCapabilities);

    StructInfo multiViewCap = {0};
    multiViewCap.name = "PalMultiViewCapabilities";
    multiViewCap.fields = multiViewCapFields;
    multiViewCap.fieldCount = ARRAY_SIZE(multiViewCapFields);
    multiViewCap.expected.alignof = 4;
    multiViewCap.expected.size = 4;
    multiViewCap.expected.padding = 0;
    multiViewCap.actual = STRUCT(PalMultiViewCapabilities);

    StructInfo multiViewportCap = {0};
    multiViewportCap.name = "PalMultiViewportCapabilities";
    multiViewportCap.fields = multiViewportCapFields;
    multiViewportCap.fieldCount = ARRAY_SIZE(multiViewportCapFields);
    multiViewportCap.expected.alignof = 4;
    multiViewportCap.expected.size = 4;
    multiViewportCap.expected.padding = 0;
    multiViewportCap.actual = STRUCT(PalMultiViewportCapabilities);

    StructInfo depthStencilCap = {0};
    depthStencilCap.name = "PalDepthStencilCapabilities";
    depthStencilCap.fields = depthStencilCapFields;
    depthStencilCap.fieldCount = ARRAY_SIZE(depthStencilCapFields);
    depthStencilCap.expected.alignof = 4;
    depthStencilCap.expected.size = 16;
    depthStencilCap.expected.padding = 0;
    depthStencilCap.actual = STRUCT(PalDepthStencilCapabilities);

    StructInfo fragmentShadingRateCap = {0};
    fragmentShadingRateCap.name = "PalFragmentShadingRateCapabilities";
    fragmentShadingRateCap.fields = fragmentShadingRateCapFields;
    fragmentShadingRateCap.fieldCount = ARRAY_SIZE(fragmentShadingRateCapFields);
    fragmentShadingRateCap.expected.alignof = 4;
    fragmentShadingRateCap.expected.size = 24;
    fragmentShadingRateCap.expected.padding = 0;
    fragmentShadingRateCap.actual = STRUCT(PalFragmentShadingRateCapabilities);

    StructInfo meshCap = {0};
    meshCap.name = "PalMeshShaderCapabilities";
    meshCap.fields = meshCapFields;
    meshCap.fieldCount = ARRAY_SIZE(meshCapFields);
    meshCap.expected.alignof = 4;
    meshCap.expected.size = 40;
    meshCap.expected.padding = 0;
    meshCap.actual = STRUCT(PalMeshShaderCapabilities);

    StructInfo rayTracingCap = {0};
    rayTracingCap.name = "PalRayTracingCapabilities";
    rayTracingCap.fields = rayTracingCapFields;
    rayTracingCap.fieldCount = ARRAY_SIZE(rayTracingCapFields);
    rayTracingCap.expected.alignof = 4;
    rayTracingCap.expected.size = 28;
    rayTracingCap.expected.padding = 0;
    rayTracingCap.actual = STRUCT(PalRayTracingCapabilities);

    StructInfo descriptorIndexingCap = {0};
    descriptorIndexingCap.name = "PalDescriptorIndexingCapabilities";
    descriptorIndexingCap.fields = descriptorIndexingCapFields;
    descriptorIndexingCap.fieldCount = ARRAY_SIZE(descriptorIndexingCapFields);
    descriptorIndexingCap.expected.alignof = 4;
    descriptorIndexingCap.expected.size = 52;
    descriptorIndexingCap.expected.padding = 0;
    descriptorIndexingCap.actual = STRUCT(PalDescriptorIndexingCapabilities);

    StructInfo memoryRequirement = {0};
    memoryRequirement.name = "PalMemoryRequirements";
    memoryRequirement.fields = memoryRequirementFields;
    memoryRequirement.fieldCount = ARRAY_SIZE(memoryRequirementFields);
    memoryRequirement.expected.alignof = 8;
    memoryRequirement.expected.size = 32;
    memoryRequirement.expected.padding = 0;
    memoryRequirement.actual = STRUCT(PalMemoryRequirements);

    PalBool status = checkABI(&samplerAnisotropyCap, flags);
    if (status == PAL_FALSE) {
        return status;
    }

    status = checkABI(&multiViewCap, flags);
    if (status == PAL_FALSE) {
        return status;
    }

    status = checkABI(&multiViewportCap, flags);
    if (status == PAL_FALSE) {
        return status;
    }

    status = checkABI(&depthStencilCap, flags);
    if (status == PAL_FALSE) {
        return status;
    }

    status = checkABI(&fragmentShadingRateCap, flags);
    if (status == PAL_FALSE) {
        return status;
    }

    status = checkABI(&meshCap, flags);
    if (status == PAL_FALSE) {
        return status;
    }

    status = checkABI(&rayTracingCap, flags);
    if (status == PAL_FALSE) {
        return status;
    }

    status = checkABI(&descriptorIndexingCap, flags);
    if (status == PAL_FALSE) {
        return status;
    }

    return checkABI(&memoryRequirement, flags);
}

static PalBool swapchainDump(uint32_t flags)
{
    // clang-format off
    FieldInfo surfaceCapFields[] = {
        { "supportedPresentModes", {0, 4}, FIELD(PalSurfaceCapabilities, supportedPresentModes) },
        { "supportedCompositeAlphas", {4, 4}, FIELD(PalSurfaceCapabilities, supportedCompositeAlphas) },
        { "supportedFormats", {8, 4}, FIELD(PalSurfaceCapabilities, supportedFormats) },
        { "minImageCount", {12, 4}, FIELD(PalSurfaceCapabilities, minImageCount) },
        { "maxImageCount", {16, 4}, FIELD(PalSurfaceCapabilities, maxImageCount) },
        { "minImageWidth", {20, 4}, FIELD(PalSurfaceCapabilities, minImageWidth) },
        { "minImageHeight", {24, 4}, FIELD(PalSurfaceCapabilities, minImageHeight) },
        { "maxImageWidth", {28, 4}, FIELD(PalSurfaceCapabilities, maxImageWidth) },
        { "maxImageHeight", {32, 4}, FIELD(PalSurfaceCapabilities, maxImageHeight) },
        { "maxImageArrayLayers", {36, 4}, FIELD(PalSurfaceCapabilities, maxImageArrayLayers) }
    };

    FieldInfo swapchainNextImageInfoFields[] = {
        { "timeout", {0, 8}, FIELD(PalSwapchainNextImageInfo, timeout) },
        { "signalSemaphore", {8, 8}, FIELD(PalSwapchainNextImageInfo, signalSemaphore) },
        { "fence", {16, 8}, FIELD(PalSwapchainNextImageInfo, fence) }
    };

    FieldInfo swapchainCreateInfoFields[] = {
        { "clipped", {0, 4}, FIELD(PalSwapchainCreateInfo, clipped) },
        { "width", {4, 4}, FIELD(PalSwapchainCreateInfo, width) },
        { "height", {8, 4}, FIELD(PalSwapchainCreateInfo, height) },
        { "imageCount", {12, 4}, FIELD(PalSwapchainCreateInfo, imageCount) },
        { "imageArrayLayerCount", {16, 4}, FIELD(PalSwapchainCreateInfo, imageArrayLayerCount) },
        { "presentMode", {20, 4}, FIELD(PalSwapchainCreateInfo, presentMode) },
        { "compositeAlpha", {24, 4}, FIELD(PalSwapchainCreateInfo, compositeAlpha) },
        { "format", {28, 4}, FIELD(PalSwapchainCreateInfo, format) }
    };
    // clang-format on

    StructInfo surfaceCap = {0};
    surfaceCap.name = "PalSurfaceCapabilities";
    surfaceCap.fields = surfaceCapFields;
    surfaceCap.fieldCount = ARRAY_SIZE(surfaceCapFields);
    surfaceCap.expected.alignof = 4;
    surfaceCap.expected.size = 40;
    surfaceCap.expected.padding = 0;
    surfaceCap.actual = STRUCT(PalSurfaceCapabilities);

    StructInfo swapchainNextImageInfo = {0};
    swapchainNextImageInfo.name = "PalSwapchainNextImageInfo";
    swapchainNextImageInfo.fields = swapchainNextImageInfoFields;
    swapchainNextImageInfo.fieldCount = ARRAY_SIZE(swapchainNextImageInfoFields);
    swapchainNextImageInfo.expected.alignof = 8;
    swapchainNextImageInfo.expected.size = 24;
    swapchainNextImageInfo.expected.padding = 0;
    swapchainNextImageInfo.actual = STRUCT(PalSwapchainNextImageInfo);

    StructInfo swapchainCreateInfo = {0};
    swapchainCreateInfo.name = "PalSwapchainCreateInfo";
    swapchainCreateInfo.fields = swapchainCreateInfoFields;
    swapchainCreateInfo.fieldCount = ARRAY_SIZE(swapchainCreateInfoFields);
    swapchainCreateInfo.expected.alignof = 4;
    swapchainCreateInfo.expected.size = 32;
    swapchainCreateInfo.expected.padding = 0;
    swapchainCreateInfo.actual = STRUCT(PalSwapchainCreateInfo);

    PalBool status = checkABI(&surfaceCap, flags);
    if (status == PAL_FALSE) {
        return status;
    }

    status = checkABI(&swapchainNextImageInfo, flags);
    if (status == PAL_FALSE) {
        return status;
    }

    return checkABI(&swapchainCreateInfo, flags);
}

static PalBool imageDump(uint32_t flags)
{
    // clang-format off
    FieldInfo imageInfoFields[] = {
        { "usages", {0, 4}, FIELD(PalImageInfo, usages) },
        { "width", {4, 4}, FIELD(PalImageInfo, width) },
        { "height", {8, 4}, FIELD(PalImageInfo, height) },
        { "depth", {12, 4}, FIELD(PalImageInfo, depth) },
        { "arrayLayerCount", {16, 4}, FIELD(PalImageInfo, arrayLayerCount) },
        { "mipLevelCount", {20, 4}, FIELD(PalImageInfo, mipLevelCount) },
        { "sampleCount", {24, 4}, FIELD(PalImageInfo, sampleCount) },
        { "type", {28, 4}, FIELD(PalImageInfo, type) },
        { "format", {32, 4}, FIELD(PalImageInfo, format) },
        { "belongsToSwapchain", {36, 4}, FIELD(PalImageInfo, belongsToSwapchain) }
    };

    FieldInfo imageSubresourceRangeFields[] = {
        { "aspect", {0, 4}, FIELD(PalImageSubresourceRange, aspect) },
        { "startMipLevel", {4, 4}, FIELD(PalImageSubresourceRange, startMipLevel) },
        { "mipLevelCount", {8, 4}, FIELD(PalImageSubresourceRange, mipLevelCount) },
        { "startArrayLayer", {12, 4}, FIELD(PalImageSubresourceRange, startArrayLayer) },
        { "layerArrayCount", {16, 4}, FIELD(PalImageSubresourceRange, layerArrayCount) }
    };

    FieldInfo bufferImageCopyInfoFields[] = {
        { "bufferOffset", {0, 8}, FIELD(PalBufferImageCopyInfo, bufferOffset) },
        { "imageAspect", {8, 4}, FIELD(PalBufferImageCopyInfo, imageAspect) },
        { "bufferRowLength", {12, 4}, FIELD(PalBufferImageCopyInfo, bufferRowLength) },
        { "bufferImageHeight", {16, 4}, FIELD(PalBufferImageCopyInfo, bufferImageHeight) },
        { "ImageMipLevel", {20, 4}, FIELD(PalBufferImageCopyInfo, ImageMipLevel) },
        { "ImageStartArrayLayer", {24, 4}, FIELD(PalBufferImageCopyInfo, ImageStartArrayLayer) },
        { "ImageArrayLayerCount", {28, 4}, FIELD(PalBufferImageCopyInfo, ImageArrayLayerCount) },
        { "imageOffsetX", {32, 4}, FIELD(PalBufferImageCopyInfo, imageOffsetX) },
        { "imageOffsetY", {36, 4}, FIELD(PalBufferImageCopyInfo, imageOffsetY) },
        { "imageOffsetZ", {40, 4}, FIELD(PalBufferImageCopyInfo, imageOffsetZ) },
        { "imageWidth", {44, 4}, FIELD(PalBufferImageCopyInfo, imageWidth) },
        { "imageHeight", {48, 4}, FIELD(PalBufferImageCopyInfo, imageHeight) },
        { "imageDepth", {52, 4}, FIELD(PalBufferImageCopyInfo, imageDepth) }
    };

    FieldInfo imageCopyInfoFields[] = {
        { "aspect", {0, 4}, FIELD(PalImageCopyInfo, aspect) },
        { "dstMipLevel", {4, 4}, FIELD(PalImageCopyInfo, dstMipLevel) },
        { "srcMipLevel", {8, 4}, FIELD(PalImageCopyInfo, srcMipLevel) },
        { "dstStartArrayLayer", {12, 4}, FIELD(PalImageCopyInfo, dstStartArrayLayer) },
        { "srcStartArrayLayer", {16, 4}, FIELD(PalImageCopyInfo, srcStartArrayLayer) },
        { "arrayLayerCount", {20, 4}, FIELD(PalImageCopyInfo, arrayLayerCount) },
        { "dstOffsetX", {24, 4}, FIELD(PalImageCopyInfo, dstOffsetX) },
        { "srcOffsetX", {28, 4}, FIELD(PalImageCopyInfo, srcOffsetX) },
        { "dstOffsetY", {32, 4}, FIELD(PalImageCopyInfo, dstOffsetY) },
        { "srcOffsetY", {36, 4}, FIELD(PalImageCopyInfo, srcOffsetY) },
        { "dstOffsetZ", {40, 4}, FIELD(PalImageCopyInfo, dstOffsetZ) },
        { "srcOffsetZ", {44, 4}, FIELD(PalImageCopyInfo, srcOffsetZ) },
        { "width", {48, 4}, FIELD(PalImageCopyInfo, width) },
        { "height", {52, 4}, FIELD(PalImageCopyInfo, height) },
        { "depth", {56, 4}, FIELD(PalImageCopyInfo, depth) }
    };

    FieldInfo imageCreateInfoFields[] = {
        { "usages", {0, 4}, FIELD(PalImageCreateInfo, usages) },
        { "width", {4, 4}, FIELD(PalImageCreateInfo, width) },
        { "height", {8, 4}, FIELD(PalImageCreateInfo, height) },
        { "depth", {12, 4}, FIELD(PalImageCreateInfo, depth) },
        { "arrayLayerCount", {16, 4}, FIELD(PalImageCreateInfo, arrayLayerCount) },
        { "mipLevelCount", {20, 4}, FIELD(PalImageCreateInfo, mipLevelCount) },
        { "sampleCount", {24, 4}, FIELD(PalImageCreateInfo, sampleCount) },
        { "type", {28, 4}, FIELD(PalImageCreateInfo, type) },
        { "format", {32, 4}, FIELD(PalImageCreateInfo, format) },
        { "memoryUsage", {36, 4}, FIELD(PalImageCreateInfo, memoryUsage) }
    };

    FieldInfo imageViewCreateInfoFields[] = {
        { "format", {0, 4}, FIELD(PalImageViewCreateInfo, format) },
        { "type", {4, 4}, FIELD(PalImageViewCreateInfo, type) },
        { "subresourceRange", {8, 20}, FIELD(PalImageViewCreateInfo, subresourceRange) }
    };

    FieldInfo samplerCreateInfoFields[] = {
        { "enableCompare", {0, 4}, FIELD(PalSamplerCreateInfo, enableCompare) },
        { "enableAnisotropy", {4, 4}, FIELD(PalSamplerCreateInfo, enableAnisotropy) },
        { "mipLodBias", {8, 4}, FIELD(PalSamplerCreateInfo, mipLodBias) },
        { "minLod", {12, 4}, FIELD(PalSamplerCreateInfo, minLod) },
        { "maxLod", {16, 4}, FIELD(PalSamplerCreateInfo, maxLod) },
        { "maxAnisotropy", {20, 4}, FIELD(PalSamplerCreateInfo, maxAnisotropy) },
        { "minFilterMode", {24, 4}, FIELD(PalSamplerCreateInfo, minFilterMode) },
        { "magFilterMode", {28, 4}, FIELD(PalSamplerCreateInfo, magFilterMode) },
        { "mipmapMode", {32, 4}, FIELD(PalSamplerCreateInfo, mipmapMode) },
        { "addressModeU", {36, 4}, FIELD(PalSamplerCreateInfo, addressModeU) },
        { "addressModeV", {40, 4}, FIELD(PalSamplerCreateInfo, addressModeV) },
        { "addressModeW", {44, 4}, FIELD(PalSamplerCreateInfo, addressModeW) },
        { "compareOp", {48, 4}, FIELD(PalSamplerCreateInfo, compareOp) },
        { "borderColor", {52, 4}, FIELD(PalSamplerCreateInfo, borderColor) },
    };
    // clang-format on

    StructInfo imageInfo = {0};
    imageInfo.name = "PalImageInfo";
    imageInfo.fields = imageInfoFields;
    imageInfo.fieldCount = ARRAY_SIZE(imageInfoFields);
    imageInfo.expected.alignof = 4;
    imageInfo.expected.size = 40;
    imageInfo.expected.padding = 0;
    imageInfo.actual = STRUCT(PalImageInfo);

    StructInfo imageSubresourceRange = {0};
    imageSubresourceRange.name = "PalImageSubresourceRange";
    imageSubresourceRange.fields = imageSubresourceRangeFields;
    imageSubresourceRange.fieldCount = ARRAY_SIZE(imageSubresourceRangeFields);
    imageSubresourceRange.expected.alignof = 4;
    imageSubresourceRange.expected.size = 20;
    imageSubresourceRange.expected.padding = 0;
    imageSubresourceRange.actual = STRUCT(PalImageSubresourceRange);

    StructInfo bufferImageCopyInfo = {0};
    bufferImageCopyInfo.name = "PalBufferImageCopyInfo";
    bufferImageCopyInfo.fields = bufferImageCopyInfoFields;
    bufferImageCopyInfo.fieldCount = ARRAY_SIZE(bufferImageCopyInfoFields);
    bufferImageCopyInfo.expected.alignof = 8;
    bufferImageCopyInfo.expected.size = 56;
    bufferImageCopyInfo.expected.padding = 0;
    bufferImageCopyInfo.actual = STRUCT(PalBufferImageCopyInfo);

    StructInfo imageCopyInfo = {0};
    imageCopyInfo.name = "PalImageCopyInfo";
    imageCopyInfo.fields = imageCopyInfoFields;
    imageCopyInfo.fieldCount = ARRAY_SIZE(imageCopyInfoFields);
    imageCopyInfo.expected.alignof = 4;
    imageCopyInfo.expected.size = 60;
    imageCopyInfo.expected.padding = 0;
    imageCopyInfo.actual = STRUCT(PalImageCopyInfo);

    StructInfo imageCreateInfo = {0};
    imageCreateInfo.name = "PalImageCreateInfo";
    imageCreateInfo.fields = imageCreateInfoFields;
    imageCreateInfo.fieldCount = ARRAY_SIZE(imageCreateInfoFields);
    imageCreateInfo.expected.alignof = 4;
    imageCreateInfo.expected.size = 40;
    imageCreateInfo.expected.padding = 0;
    imageCreateInfo.actual = STRUCT(PalImageCreateInfo);

    StructInfo imageViewCreateInfo = {0};
    imageViewCreateInfo.name = "PalImageViewCreateInfo";
    imageViewCreateInfo.fields = imageViewCreateInfoFields;
    imageViewCreateInfo.fieldCount = ARRAY_SIZE(imageViewCreateInfoFields);
    imageViewCreateInfo.expected.alignof = 4;
    imageViewCreateInfo.expected.size = 28;
    imageViewCreateInfo.expected.padding = 0;
    imageViewCreateInfo.actual = STRUCT(PalImageViewCreateInfo);

    StructInfo samplerCreateInfo = {0};
    samplerCreateInfo.name = "PalSamplerCreateInfo";
    samplerCreateInfo.fields = samplerCreateInfoFields;
    samplerCreateInfo.fieldCount = ARRAY_SIZE(samplerCreateInfoFields);
    samplerCreateInfo.expected.alignof = 4;
    samplerCreateInfo.expected.size = 56;
    samplerCreateInfo.expected.padding = 0;
    samplerCreateInfo.actual = STRUCT(PalSamplerCreateInfo);

    PalBool status = checkABI(&imageInfo, flags);
    if (status == PAL_FALSE) {
        return status;
    }

    status = checkABI(&imageSubresourceRange, flags);
    if (status == PAL_FALSE) {
        return status;
    }

    status = checkABI(&bufferImageCopyInfo, flags);
    if (status == PAL_FALSE) {
        return status;
    }

    status = checkABI(&imageCopyInfo, flags);
    if (status == PAL_FALSE) {
        return status;
    }

    status = checkABI(&imageCreateInfo, flags);
    if (status == PAL_FALSE) {
        return status;
    }

    status = checkABI(&imageViewCreateInfo, flags);
    if (status == PAL_FALSE) {
        return status;
    }

    return checkABI(&samplerCreateInfo, flags);
}

static PalBool bufferDump(uint32_t flags)
{
    // clang-format off
    FieldInfo stagingRequirementFields[] = {
        { "bufferSize", {0, 8}, FIELD(PalImageStagingRequirements, bufferSize) },
        { "bufferRowLength", {8, 4}, FIELD(PalImageStagingRequirements, bufferRowLength) },
        { "bufferImageHeight", {12, 4}, FIELD(PalImageStagingRequirements, bufferImageHeight) }
    };

    FieldInfo bufferCopyInfoFields[] = {
        { "size", {0, 8}, FIELD(PalBufferCopyInfo, size) },
        { "dstOffset", {8, 8}, FIELD(PalBufferCopyInfo, dstOffset) },
        { "srcOffset", {16, 8}, FIELD(PalBufferCopyInfo, srcOffset) }
    };

    FieldInfo bufferCreateInfoFields[] = {
        { "size", {0, 8}, FIELD(PalBufferCreateInfo, size) },
        { "usages", {8, 4}, FIELD(PalBufferCreateInfo, usages) },
        { "memoryUsage", {12, 4}, FIELD(PalBufferCreateInfo, memoryUsage) }
    };
    // clang-format on

    StructInfo stagingRequirement = {0};
    stagingRequirement.name = "PalImageStagingRequirements";
    stagingRequirement.fields = stagingRequirementFields;
    stagingRequirement.fieldCount = ARRAY_SIZE(stagingRequirementFields);
    stagingRequirement.expected.alignof = 8;
    stagingRequirement.expected.size = 16;
    stagingRequirement.expected.padding = 0;
    stagingRequirement.actual = STRUCT(PalImageStagingRequirements);

    StructInfo bufferCopyInfo = {0};
    bufferCopyInfo.name = "PalBufferCopyInfo";
    bufferCopyInfo.fields = bufferCopyInfoFields;
    bufferCopyInfo.fieldCount = ARRAY_SIZE(bufferCopyInfoFields);
    bufferCopyInfo.expected.alignof = 8;
    bufferCopyInfo.expected.size = 24;
    bufferCopyInfo.expected.padding = 0;
    bufferCopyInfo.actual = STRUCT(PalBufferCopyInfo);

    StructInfo bufferCreateInfo = {0};
    bufferCreateInfo.name = "PalBufferCreateInfo";
    bufferCreateInfo.fields = bufferCreateInfoFields;
    bufferCreateInfo.fieldCount = ARRAY_SIZE(bufferCreateInfoFields);
    bufferCreateInfo.expected.alignof = 8;
    bufferCreateInfo.expected.size = 16;
    bufferCreateInfo.expected.padding = 0;
    bufferCreateInfo.actual = STRUCT(PalBufferCreateInfo);

    PalBool status = checkABI(&stagingRequirement, flags);
    if (status == PAL_FALSE) {
        return status;
    }

    status = checkABI(&bufferCopyInfo, flags);
    if (status == PAL_FALSE) {
        return status;
    }

    return checkABI(&bufferCreateInfo, flags);
}

PalBool graphicsABIDump(uint32_t flags)
{
    if (!(flags & DUMP_FLAG_QUICK)) {
        palLog(nullptr, "");
        palLog(nullptr, "===========================================");
        palLog(nullptr, "Graphics ABI Dump");
        palLog(nullptr, "===========================================");
        palLog(nullptr, "");
    }

    PalBool status = adapterDump(flags);
    if (status == PAL_FALSE) {
        return status;
    }

    status = deviceDump(flags);
    if (status == PAL_FALSE) {
        return status;
    }

    status = swapchainDump(flags);
    if (status == PAL_FALSE) {
        return status;
    }

    status = imageDump(flags);
    if (status == PAL_FALSE) {
        return status;
    }

    status = bufferDump(flags);
    if (status == PAL_FALSE) {
        return status;
    }

    return PAL_TRUE;
}