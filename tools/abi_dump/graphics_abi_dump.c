
/**
 PAL - Prime Abstraction Layer
 Copyright (C) 2025
 Licensed under the Zlib license. See LICENSE file in root.
 */

#include "dumps.h"
#include "pal2/pal_graphics.h"

static PalBool adapterDump(uint32_t flags)
{
    // clang-format off
    FieldInfo adapterInfoFields[] = {
        { "vram", {0, 8}, FIELD(PalAdapterInfo, vram) },
        { "sharedMemory", {8, 8}, FIELD(PalAdapterInfo, sharedMemory) },
        { "driverVersion", {16, 8}, FIELD(PalAdapterInfo, driverVersion) },
        { "vendorId", {24, 4}, FIELD(PalAdapterInfo, vendorId) },
        { "deviceId", {28, 4}, FIELD(PalAdapterInfo, deviceId) },
        { "shaderFormats", {32, 4}, FIELD(PalAdapterInfo, shaderFormats) },
        { "type", {36, 4}, FIELD(PalAdapterInfo, type) },
        { "apiType", {40, 4}, FIELD(PalAdapterInfo, apiType) },
        { "name", {44, 128}, FIELD(PalAdapterInfo, name) },
        { "backendName", {172, 32}, FIELD(PalAdapterInfo, backendName) },
        { "reserved", {204, 4}, FIELD(PalAdapterInfo, reserved) }
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
    adapterInfo.expected.size = 208;
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

    FieldInfo shaderEntryInfoFields[] = {
        { "entryName", {0, 8}, FIELD(PalShaderEntryInfo, entryName) },
        { "stage", {8, 4}, FIELD(PalShaderEntryInfo, stage) },
        { "patchControlPoints", {12, 4}, FIELD(PalShaderEntryInfo, patchControlPoints) }
    };

    FieldInfo shaderCreateInfoFields[] = {
        { "bcode", {0, 8}, FIELD(PalShaderCreateInfo, code) },
        { "entries", {8, 8}, FIELD(PalShaderCreateInfo, entries) },
        { "codeSize", {16, 4}, FIELD(PalShaderCreateInfo, codeSize) },
        { "entryCount", {20, 4}, FIELD(PalShaderCreateInfo, entryCount) }
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

    StructInfo shaderEntryInfo = {0};
    shaderEntryInfo.name = "PalShaderEntryInfo";
    shaderEntryInfo.fields = shaderEntryInfoFields;
    shaderEntryInfo.fieldCount = ARRAY_SIZE(shaderEntryInfoFields);
    shaderEntryInfo.expected.alignof = 8;
    shaderEntryInfo.expected.size = 16;
    shaderEntryInfo.expected.padding = 0;
    shaderEntryInfo.actual = STRUCT(PalShaderEntryInfo);

    StructInfo shaderCreateInfo = {0};
    shaderCreateInfo.name = "PalShaderCreateInfo";
    shaderCreateInfo.fields = shaderCreateInfoFields;
    shaderCreateInfo.fieldCount = ARRAY_SIZE(shaderCreateInfoFields);
    shaderCreateInfo.expected.alignof = 8;
    shaderCreateInfo.expected.size = 24;
    shaderCreateInfo.expected.padding = 0;
    shaderCreateInfo.actual = STRUCT(PalShaderCreateInfo);

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

    status = checkABI(&memoryRequirement, flags);
    if (status == PAL_FALSE) {
        return status;
    }

    status = checkABI(&shaderEntryInfo, flags);
    if (status == PAL_FALSE) {
        return status;
    }

    return checkABI(&shaderCreateInfo, flags);
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

    FieldInfo drawIndirectDataFields[] = {
        { "vertexCount", {0, 4}, FIELD(PalDrawIndirectData, vertexCount) },
        { "instanceCount", {4, 4}, FIELD(PalDrawIndirectData, instanceCount) },
        { "firstVertex", {8, 4}, FIELD(PalDrawIndirectData, firstVertex) },
        { "firstInstance", {12, 4}, FIELD(PalDrawIndirectData, firstInstance) }
    };

    FieldInfo drawIndexedIndirectDataFields[] = {
        { "indexCount", {0, 4}, FIELD(PalDrawIndexedIndirectData, indexCount) },
        { "instanceCount", {4, 4}, FIELD(PalDrawIndexedIndirectData, instanceCount) },
        { "firstIndex", {8, 4}, FIELD(PalDrawIndexedIndirectData, firstIndex) },
        { "vertexOffset", {12, 4}, FIELD(PalDrawIndexedIndirectData, vertexOffset) },
        { "firstInstance", {16, 4}, FIELD(PalDrawIndexedIndirectData, firstInstance) }
    };

    FieldInfo dispatchIndirectDataFields[] = {
        { "groupCountXOrWidth", {0, 4}, FIELD(PalDispatchIndirectData, groupCountXOrWidth) },
        { "groupCountXOrHeight", {4, 4}, FIELD(PalDispatchIndirectData, groupCountXOrHeight) },
        { "groupCountXOrDepth", {8, 4}, FIELD(PalDispatchIndirectData, groupCountXOrDepth) }
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

    StructInfo drawIndirectData = {0};
    drawIndirectData.name = "PalDrawIndirectData";
    drawIndirectData.fields = drawIndirectDataFields;
    drawIndirectData.fieldCount = ARRAY_SIZE(drawIndirectDataFields);
    drawIndirectData.expected.alignof = 4;
    drawIndirectData.expected.size = 16;
    drawIndirectData.expected.padding = 0;
    drawIndirectData.actual = STRUCT(PalDrawIndirectData);

    StructInfo drawIndexedIndirectData = {0};
    drawIndexedIndirectData.name = "PalDrawIndexedIndirectData";
    drawIndexedIndirectData.fields = drawIndexedIndirectDataFields;
    drawIndexedIndirectData.fieldCount = ARRAY_SIZE(drawIndexedIndirectDataFields);
    drawIndexedIndirectData.expected.alignof = 4;
    drawIndexedIndirectData.expected.size = 20;
    drawIndexedIndirectData.expected.padding = 0;
    drawIndexedIndirectData.actual = STRUCT(PalDrawIndexedIndirectData);

    StructInfo dispatchIndirectData = {0};
    dispatchIndirectData.name = "PalDispatchIndirectData";
    dispatchIndirectData.fields = dispatchIndirectDataFields;
    dispatchIndirectData.fieldCount = ARRAY_SIZE(dispatchIndirectDataFields);
    dispatchIndirectData.expected.alignof = 4;
    dispatchIndirectData.expected.size = 12;
    dispatchIndirectData.expected.padding = 0;
    dispatchIndirectData.actual = STRUCT(PalDispatchIndirectData);

    PalBool status = checkABI(&stagingRequirement, flags);
    if (status == PAL_FALSE) {
        return status;
    }

    status = checkABI(&bufferCopyInfo, flags);
    if (status == PAL_FALSE) {
        return status;
    }

    status = checkABI(&bufferCreateInfo, flags);
    if (status == PAL_FALSE) {
        return status;
    }

    status = checkABI(&drawIndirectData, flags);
    if (status == PAL_FALSE) {
        return status;
    }

    status = checkABI(&drawIndexedIndirectData, flags);
    if (status == PAL_FALSE) {
        return status;
    }

    return checkABI(&dispatchIndirectData, flags);
}

static PalBool accelerationStructureDump(uint32_t flags)
{
    // clang-format off
    FieldInfo accelerationStructureInstanceFields[] = {
        { "blas", {0, 8}, FIELD(PalAccelerationStructureInstance, blas) },
        { "flags", {8, 4}, FIELD(PalAccelerationStructureInstance, flags) },
        { "mask", {12, 4}, FIELD(PalAccelerationStructureInstance, mask) },
        { "instanceId", {16, 4}, FIELD(PalAccelerationStructureInstance, instanceId) },
        { "hitGroupOffset", {20, 4}, FIELD(PalAccelerationStructureInstance, hitGroupOffset) },
        { "transform", {24, 48}, FIELD(PalAccelerationStructureInstance, transform) }
    };

    FieldInfo accelerationStructureBuildSizeFields[] = {
        { "accelerationStructureSize", {0, 8}, FIELD(PalAccelerationStructureBuildSize, accelerationStructureSize) },
        { "scratchBufferSize", {8, 8}, FIELD(PalAccelerationStructureBuildSize, scratchBufferSize) },
        { "updateScratchBufferSize", {16, 8}, FIELD(PalAccelerationStructureBuildSize, updateScratchBufferSize) }
    };

    FieldInfo geometryDataTriangleFields[] = {
        { "vertexBufferAddress", {0, 8}, FIELD(PalGeometryDataTriangle, vertexBufferAddress) },
        { "indexBufferAddress", {8, 8}, FIELD(PalGeometryDataTriangle, indexBufferAddress) },
        { "transformBufferAddress", {16, 8}, FIELD(PalGeometryDataTriangle, transformBufferAddress) },
        { "vertexType", {24, 4}, FIELD(PalGeometryDataTriangle, vertexType) },
        { "indexType", {28, 4}, FIELD(PalGeometryDataTriangle, indexType) },
        { "vertexCount", {32, 4}, FIELD(PalGeometryDataTriangle, vertexCount) },
        { "vertexStride", {36, 4}, FIELD(PalGeometryDataTriangle, vertexStride) }
    };

    FieldInfo geometryDataAABBSFields[] = {
        { "bufferAddress", {0, 8}, FIELD(PalGeometryDataAABBS, bufferAddress) },
        { "stride", {8, 8}, FIELD(PalGeometryDataAABBS, stride) }
    };

    FieldInfo geometryFields[] = {
        { "data", {0, 8}, FIELD(PalGeometry, data) },
        { "primitiveCount", {8, 8}, FIELD(PalGeometry, primitiveCount) },
        { "flags", {16, 4}, FIELD(PalGeometry, flags) },
        { "type", {20, 4}, FIELD(PalGeometry, type) },
    };

    FieldInfo accelerationStructureBuildInfoFields[] = {
        { "dst", {0, 8}, FIELD(PalAccelerationStructureBuildInfo, dst) },
        { "src", {8, 8}, FIELD(PalAccelerationStructureBuildInfo, src) },
        { "geometries", {16, 8}, FIELD(PalAccelerationStructureBuildInfo, geometries) },
        { "scratchBufferAddress", {24, 8}, FIELD(PalAccelerationStructureBuildInfo, scratchBufferAddress) },
        { "instanceBufferAddress", {32, 8}, FIELD(PalAccelerationStructureBuildInfo, instanceBufferAddress) },
        { "buildHints", {40, 4}, FIELD(PalAccelerationStructureBuildInfo, buildHints) },
        { "type", {44, 4}, FIELD(PalAccelerationStructureBuildInfo, type) },
        { "buildMode", {48, 4}, FIELD(PalAccelerationStructureBuildInfo, buildMode) },
        { "count", {52, 4}, FIELD(PalAccelerationStructureBuildInfo, count) },
    };

    FieldInfo accelerationStructureCreateInfoFields[] = {
        { "buffer", {0, 8}, FIELD(PalAccelerationStructureCreateInfo, buffer) },
        { "offset", {8, 8}, FIELD(PalAccelerationStructureCreateInfo, offset) },
        { "size", {16, 8}, FIELD(PalAccelerationStructureCreateInfo, size) },
        { "type", {24, 4}, FIELD(PalAccelerationStructureCreateInfo, type) },
        { "reserved", {28, 4}, FIELD(PalAccelerationStructureCreateInfo, reserved) }
    };
    // clang-format on

    StructInfo accelerationStructureInstance = {0};
    accelerationStructureInstance.name = "PalAccelerationStructureInstance";
    accelerationStructureInstance.fields = accelerationStructureInstanceFields;
    accelerationStructureInstance.fieldCount = ARRAY_SIZE(accelerationStructureInstanceFields);
    accelerationStructureInstance.expected.alignof = 8;
    accelerationStructureInstance.expected.size = 72;
    accelerationStructureInstance.expected.padding = 0;
    accelerationStructureInstance.actual = STRUCT(PalAccelerationStructureInstance);

    StructInfo accelerationStructureBuildSize = {0};
    accelerationStructureBuildSize.name = "PalAccelerationStructureBuildSize";
    accelerationStructureBuildSize.fields = accelerationStructureBuildSizeFields;
    accelerationStructureBuildSize.fieldCount = ARRAY_SIZE(accelerationStructureBuildSizeFields);
    accelerationStructureBuildSize.expected.alignof = 8;
    accelerationStructureBuildSize.expected.size = 24;
    accelerationStructureBuildSize.expected.padding = 0;
    accelerationStructureBuildSize.actual = STRUCT(PalAccelerationStructureBuildSize);

    StructInfo geometryDataTriangle = {0};
    geometryDataTriangle.name = "PalGeometryDataTriangle";
    geometryDataTriangle.fields = geometryDataTriangleFields;
    geometryDataTriangle.fieldCount = ARRAY_SIZE(geometryDataTriangleFields);
    geometryDataTriangle.expected.alignof = 8;
    geometryDataTriangle.expected.size = 40;
    geometryDataTriangle.expected.padding = 0;
    geometryDataTriangle.actual = STRUCT(PalGeometryDataTriangle);

    StructInfo geometryDataAABBS = {0};
    geometryDataAABBS.name = "PalGeometryDataAABBS";
    geometryDataAABBS.fields = geometryDataAABBSFields;
    geometryDataAABBS.fieldCount = ARRAY_SIZE(geometryDataAABBSFields);
    geometryDataAABBS.expected.alignof = 8;
    geometryDataAABBS.expected.size = 16;
    geometryDataAABBS.expected.padding = 0;
    geometryDataAABBS.actual = STRUCT(PalGeometryDataAABBS);

    StructInfo geometry = {0};
    geometry.name = "PalGeometry";
    geometry.fields = geometryFields;
    geometry.fieldCount = ARRAY_SIZE(geometryFields);
    geometry.expected.alignof = 8;
    geometry.expected.size = 24;
    geometry.expected.padding = 0;
    geometry.actual = STRUCT(PalGeometry);

    StructInfo accelerationStructureBuildInfo = {0};
    accelerationStructureBuildInfo.name = "PalAccelerationStructureBuildInfo";
    accelerationStructureBuildInfo.fields = accelerationStructureBuildInfoFields;
    accelerationStructureBuildInfo.fieldCount = ARRAY_SIZE(accelerationStructureBuildInfoFields);
    accelerationStructureBuildInfo.expected.alignof = 8;
    accelerationStructureBuildInfo.expected.size = 56;
    accelerationStructureBuildInfo.expected.padding = 0;
    accelerationStructureBuildInfo.actual = STRUCT(PalAccelerationStructureBuildInfo);

    StructInfo accelerationStructureCreateInfo = {0};
    accelerationStructureCreateInfo.name = "PalAccelerationStructureCreateInfo";
    accelerationStructureCreateInfo.fields = accelerationStructureCreateInfoFields;
    accelerationStructureCreateInfo.fieldCount = ARRAY_SIZE(accelerationStructureCreateInfoFields);
    accelerationStructureCreateInfo.expected.alignof = 8;
    accelerationStructureCreateInfo.expected.size = 32;
    accelerationStructureCreateInfo.expected.padding = 0;
    accelerationStructureCreateInfo.actual = STRUCT(PalAccelerationStructureCreateInfo);

    PalBool status = checkABI(&accelerationStructureInstance, flags);
    if (status == PAL_FALSE) {
        return status;
    }

    status = checkABI(&accelerationStructureBuildSize, flags);
    if (status == PAL_FALSE) {
        return status;
    }

    status = checkABI(&geometryDataTriangle, flags);
    if (status == PAL_FALSE) {
        return status;
    }

    status = checkABI(&geometryDataAABBS, flags);
    if (status == PAL_FALSE) {
        return status;
    }

    status = checkABI(&geometry, flags);
    if (status == PAL_FALSE) {
        return status;
    }

    status = checkABI(&accelerationStructureBuildInfo, flags);
    if (status == PAL_FALSE) {
        return status;
    }

    return checkABI(&accelerationStructureCreateInfo, flags);
}

static PalBool pipelineDump(uint32_t flags)
{
    // clang-format off
    FieldInfo vertexAttributeFields[] = {
        { "semanticID", {0, 4}, FIELD(PalVertexAttribute, semanticID) },
        { "type", {4, 4}, FIELD(PalVertexAttribute, type) }
    };

    FieldInfo vertexLayoutFields[] = {
        { "attributes", {0, 8}, FIELD(PalVertexLayout, attributes) },
        { "attributeCount", {8, 4}, FIELD(PalVertexLayout, attributeCount) },
        { "type", {12, 4}, FIELD(PalVertexLayout, type) },
        { "binding", {16, 4}, FIELD(PalVertexLayout, binding) },
        { "reserved", {20, 4}, FIELD(PalVertexLayout, reserved) }
    };

    FieldInfo rasterizerStateFields[] = {
        { "enableDepthClamp", {0, 4}, FIELD(PalRasterizerState, enableDepthClamp) },
        { "enableDepthBias", {4, 4}, FIELD(PalRasterizerState, enableDepthBias) },
        { "depthBiasConstant", {8, 4}, FIELD(PalRasterizerState, depthBiasConstant) },
        { "depthBiasSlope", {12, 4}, FIELD(PalRasterizerState, depthBiasSlope) },
        { "depthBiasClamp", {16, 4}, FIELD(PalRasterizerState, depthBiasClamp) },
        { "polygonMode", {20, 4}, FIELD(PalRasterizerState, polygonMode) },
        { "cullMode", {24, 4}, FIELD(PalRasterizerState, cullMode) },
        { "frontFace", {28, 4}, FIELD(PalRasterizerState, frontFace) }
    };

    FieldInfo multisampleStateFields[] = {
        { "sampleMask", {0, 8}, FIELD(PalMultisampleState, sampleMask) },
        { "enableSampleShading", {8, 4}, FIELD(PalMultisampleState, enableSampleShading) },
        { "enableAlphaToCoverage", {12, 4}, FIELD(PalMultisampleState, enableAlphaToCoverage) },
        { "sampleCount", {16, 4}, FIELD(PalMultisampleState, sampleCount) },
        { "minSampleShading", {20, 4}, FIELD(PalMultisampleState, minSampleShading) }
    };

    FieldInfo stencilOpStateFields[] = {
        { "failOp", {0, 4}, FIELD(PalStencilOpState, failOp) },
        { "passOp", {4, 4}, FIELD(PalStencilOpState, passOp) },
        { "depthFailOp", {8, 4}, FIELD(PalStencilOpState, depthFailOp) },
        { "compareOp", {12, 4}, FIELD(PalStencilOpState, compareOp) }
    };

    FieldInfo depthStencilStateFields[] = {
        { "enableDepthTest", {0, 4}, FIELD(PalDepthStencilState, enableDepthTest) },
        { "enableDepthWrite", {4, 4}, FIELD(PalDepthStencilState, enableDepthWrite) },
        { "enableStencilTest", {8, 4}, FIELD(PalDepthStencilState, enableStencilTest) },
        { "compareOp", {12, 4}, FIELD(PalDepthStencilState, compareOp) },
        { "frontStencilOpState", {16, 16}, FIELD(PalDepthStencilState, frontStencilOpState) },
        { "backStencilOpState", {32, 16}, FIELD(PalDepthStencilState, backStencilOpState) },
    };

    FieldInfo colorBlendAttachmentFields[] = {
        { "enableBlend", {0, 4}, FIELD(PalColorBlendAttachment, enableBlend) },
        { "colorWriteMask", {4, 4}, FIELD(PalColorBlendAttachment, colorWriteMask) },
        { "dstColorBlendFactor", {8, 4}, FIELD(PalColorBlendAttachment, dstColorBlendFactor) },
        { "srcColorBlendFactor", {12, 4}, FIELD(PalColorBlendAttachment, srcColorBlendFactor) },
        { "colorBlendOp", {16, 4}, FIELD(PalColorBlendAttachment, colorBlendOp) },
        { "dstAlphaBlendFactor", {20, 4}, FIELD(PalColorBlendAttachment, dstAlphaBlendFactor) },
        { "srcAlphaBlendFactor", {24, 4}, FIELD(PalColorBlendAttachment, srcAlphaBlendFactor) },
        { "alphaBlendOp", {28, 4}, FIELD(PalColorBlendAttachment, alphaBlendOp) }
    };

    FieldInfo fragmentShadingRateStateFields[] = {
        { "rate", {0, 4}, FIELD(PalFragmentShadingRateState, rate) },
        { "combinerOps", {4, 8}, FIELD(PalFragmentShadingRateState, combinerOps) }
    };

    FieldInfo pushConstantInfoFields[] = {
        { "offset", {0, 4}, FIELD(PalPushConstantInfo, offset) },
        { "size", {4, 4}, FIELD(PalPushConstantInfo, size) }
    };

    FieldInfo pipelineLayoutCreateInfoFields[] = {
        { "descriptorSetLayouts", {0, 8}, FIELD(PalPipelineLayoutCreateInfo, descriptorSetLayouts) },
        { "pushConstantInfo", {8, 8}, FIELD(PalPipelineLayoutCreateInfo, pushConstantInfo) },
        { "descriptorSetLayoutCount", {16, 4}, FIELD(PalPipelineLayoutCreateInfo, descriptorSetLayoutCount) },
        { "usePushConstant", {20, 4}, FIELD(PalPipelineLayoutCreateInfo, usePushConstant) }
    };

    FieldInfo graphicsPipelineCreateInfoFields[] = {
        { "pipelineLayout", {0, 8}, FIELD(PalGraphicsPipelineCreateInfo, pipelineLayout) },
        { "shaders", {8, 8}, FIELD(PalGraphicsPipelineCreateInfo, shaders) },
        { "vertexLayouts", {16, 8}, FIELD(PalGraphicsPipelineCreateInfo, vertexLayouts) },
        { "colorBlendAttachments", {24, 8}, FIELD(PalGraphicsPipelineCreateInfo, colorBlendAttachments) },
        { "rasterizerState", {32, 8}, FIELD(PalGraphicsPipelineCreateInfo, rasterizerState) },
        { "multisampleState", {40, 8}, FIELD(PalGraphicsPipelineCreateInfo, multisampleState) },
        { "depthStencilState", {48, 8}, FIELD(PalGraphicsPipelineCreateInfo, depthStencilState) },
        { "fragmentShadingRateState", {56, 8}, FIELD(PalGraphicsPipelineCreateInfo, fragmentShadingRateState) },
        { "renderingLayout", {64, 8}, FIELD(PalGraphicsPipelineCreateInfo, renderingLayout) },
        { "primitiveRestartEnable", {72, 4}, FIELD(PalGraphicsPipelineCreateInfo, primitiveRestartEnable) },
        { "vertexLayoutCount", {76, 4}, FIELD(PalGraphicsPipelineCreateInfo, vertexLayoutCount) },
        { "colorBlendAttachmentCount", {80, 4}, FIELD(PalGraphicsPipelineCreateInfo, colorBlendAttachmentCount) },
        { "shaderCount", {84, 4}, FIELD(PalGraphicsPipelineCreateInfo, shaderCount) },
        { "indexType", {88, 4}, FIELD(PalGraphicsPipelineCreateInfo, indexType) },
        { "topology", {92, 4}, FIELD(PalGraphicsPipelineCreateInfo, topology) }
    };

    FieldInfo computePipelineCreateInfoFields[] = {
        { "pipelineLayout", {0, 8}, FIELD(PalComputePipelineCreateInfo, pipelineLayout) },
        { "computeShader", {8, 8}, FIELD(PalComputePipelineCreateInfo, computeShader) }
    };

    FieldInfo rayTracingShaderGroupCreateInfoFields[] = {
        { "type", {0, 4}, FIELD(PalRayTracingShaderGroupCreateInfo, type) },
        { "anyHitShaderIndex", {4, 4}, FIELD(PalRayTracingShaderGroupCreateInfo, anyHitShaderIndex) },
        { "anyHitShaderEntryIndex", {8, 4}, FIELD(PalRayTracingShaderGroupCreateInfo, anyHitShaderEntryIndex) },
        { "closestHitShaderIndex", {12, 4}, FIELD(PalRayTracingShaderGroupCreateInfo, closestHitShaderIndex) },
        { "closestHitShaderEntryIndex", {16, 4}, FIELD(PalRayTracingShaderGroupCreateInfo, closestHitShaderEntryIndex) },
        { "generalShaderIndex", {20, 4}, FIELD(PalRayTracingShaderGroupCreateInfo, generalShaderIndex) },
        { "generalShaderEntryIndex", {24, 4}, FIELD(PalRayTracingShaderGroupCreateInfo, generalShaderEntryIndex) },
        { "intersectionShaderIndex", {28, 4}, FIELD(PalRayTracingShaderGroupCreateInfo, intersectionShaderIndex) },
        { "intersectionShaderEntryIndex", {32, 4}, FIELD(PalRayTracingShaderGroupCreateInfo, intersectionShaderEntryIndex) },
        { "maxDataSize", {36, 4}, FIELD(PalRayTracingShaderGroupCreateInfo, maxDataSize) }
    };

    FieldInfo rayTracingPipelineCreateInfoFields[] = {
        { "pipelineLayout", {0, 8}, FIELD(PalRayTracingPipelineCreateInfo, pipelineLayout) },
        { "shaderGroups", {8, 8}, FIELD(PalRayTracingPipelineCreateInfo, shaderGroups) },
        { "shaders", {16, 8}, FIELD(PalRayTracingPipelineCreateInfo, shaders) },
        { "shaderGroupCount", {24, 4}, FIELD(PalRayTracingPipelineCreateInfo, shaderGroupCount) },
        { "shaderCount", {28, 4}, FIELD(PalRayTracingPipelineCreateInfo, shaderCount) },
        { "maxRecursionDepth", {32, 4}, FIELD(PalRayTracingPipelineCreateInfo, maxRecursionDepth) },
        { "maxAttributeSize", {36, 4}, FIELD(PalRayTracingPipelineCreateInfo, maxAttributeSize) },
        { "maxPayloadSize", {40, 4}, FIELD(PalRayTracingPipelineCreateInfo, maxPayloadSize) },
        { "reserved", {44, 4}, FIELD(PalRayTracingPipelineCreateInfo, reserved) }
    };
    // clang-format on

    StructInfo vertexAttribute = {0};
    vertexAttribute.name = "PalVertexAttribute";
    vertexAttribute.fields = vertexAttributeFields;
    vertexAttribute.fieldCount = ARRAY_SIZE(vertexAttributeFields);
    vertexAttribute.expected.alignof = 4;
    vertexAttribute.expected.size = 8;
    vertexAttribute.expected.padding = 0;
    vertexAttribute.actual = STRUCT(PalVertexAttribute);

    StructInfo vertexLayout = {0};
    vertexLayout.name = "PalVertexLayout";
    vertexLayout.fields = vertexLayoutFields;
    vertexLayout.fieldCount = ARRAY_SIZE(vertexLayoutFields);
    vertexLayout.expected.alignof = 8;
    vertexLayout.expected.size = 24;
    vertexLayout.expected.padding = 0;
    vertexLayout.actual = STRUCT(PalVertexLayout);

    StructInfo rasterizerState = {0};
    rasterizerState.name = "PalRasterizerState";
    rasterizerState.fields = rasterizerStateFields;
    rasterizerState.fieldCount = ARRAY_SIZE(rasterizerStateFields);
    rasterizerState.expected.alignof = 4;
    rasterizerState.expected.size = 32;
    rasterizerState.expected.padding = 0;
    rasterizerState.actual = STRUCT(PalRasterizerState);

    StructInfo multisampleState = {0};
    multisampleState.name = "PalMultisampleState";
    multisampleState.fields = multisampleStateFields;
    multisampleState.fieldCount = ARRAY_SIZE(multisampleStateFields);
    multisampleState.expected.alignof = 8;
    multisampleState.expected.size = 24;
    multisampleState.expected.padding = 0;
    multisampleState.actual = STRUCT(PalMultisampleState);

    StructInfo stencilOpState = {0};
    stencilOpState.name = "PalStencilOpState";
    stencilOpState.fields = stencilOpStateFields;
    stencilOpState.fieldCount = ARRAY_SIZE(stencilOpStateFields);
    stencilOpState.expected.alignof = 4;
    stencilOpState.expected.size = 16;
    stencilOpState.expected.padding = 0;
    stencilOpState.actual = STRUCT(PalStencilOpState);

    StructInfo depthStencilState = {0};
    depthStencilState.name = "PalDepthStencilState";
    depthStencilState.fields = depthStencilStateFields;
    depthStencilState.fieldCount = ARRAY_SIZE(depthStencilStateFields);
    depthStencilState.expected.alignof = 4;
    depthStencilState.expected.size = 48;
    depthStencilState.expected.padding = 0;
    depthStencilState.actual = STRUCT(PalDepthStencilState);

    StructInfo colorBlendAttachment = {0};
    colorBlendAttachment.name = "PalColorBlendAttachment";
    colorBlendAttachment.fields = colorBlendAttachmentFields;
    colorBlendAttachment.fieldCount = ARRAY_SIZE(colorBlendAttachmentFields);
    colorBlendAttachment.expected.alignof = 4;
    colorBlendAttachment.expected.size = 32;
    colorBlendAttachment.expected.padding = 0;
    colorBlendAttachment.actual = STRUCT(PalColorBlendAttachment);

    StructInfo fragmentShadingRateState = {0};
    fragmentShadingRateState.name = "PalFragmentShadingRateState";
    fragmentShadingRateState.fields = fragmentShadingRateStateFields;
    fragmentShadingRateState.fieldCount = ARRAY_SIZE(fragmentShadingRateStateFields);
    fragmentShadingRateState.expected.alignof = 4;
    fragmentShadingRateState.expected.size = 12;
    fragmentShadingRateState.expected.padding = 0;
    fragmentShadingRateState.actual = STRUCT(PalFragmentShadingRateState);

    StructInfo pushConstantInfo = {0};
    pushConstantInfo.name = "PalPushConstantInfo";
    pushConstantInfo.fields = pushConstantInfoFields;
    pushConstantInfo.fieldCount = ARRAY_SIZE(pushConstantInfoFields);
    pushConstantInfo.expected.alignof = 4;
    pushConstantInfo.expected.size = 8;
    pushConstantInfo.expected.padding = 0;
    pushConstantInfo.actual = STRUCT(PalPushConstantInfo);

    StructInfo pipelineLayoutCreateInfo = {0};
    pipelineLayoutCreateInfo.name = "PalPipelineLayoutCreateInfo";
    pipelineLayoutCreateInfo.fields = pipelineLayoutCreateInfoFields;
    pipelineLayoutCreateInfo.fieldCount = ARRAY_SIZE(pipelineLayoutCreateInfoFields);
    pipelineLayoutCreateInfo.expected.alignof = 8;
    pipelineLayoutCreateInfo.expected.size = 24;
    pipelineLayoutCreateInfo.expected.padding = 0;
    pipelineLayoutCreateInfo.actual = STRUCT(PalPipelineLayoutCreateInfo);

    StructInfo graphicsPipelineCreateInfo = {0};
    graphicsPipelineCreateInfo.name = "PalGraphicsPipelineCreateInfo";
    graphicsPipelineCreateInfo.fields = graphicsPipelineCreateInfoFields;
    graphicsPipelineCreateInfo.fieldCount = ARRAY_SIZE(graphicsPipelineCreateInfoFields);
    graphicsPipelineCreateInfo.expected.alignof = 8;
    graphicsPipelineCreateInfo.expected.size = 96;
    graphicsPipelineCreateInfo.expected.padding = 0;
    graphicsPipelineCreateInfo.actual = STRUCT(PalGraphicsPipelineCreateInfo);

    StructInfo computePipelineCreateInfo = {0};
    computePipelineCreateInfo.name = "PalComputePipelineCreateInfo";
    computePipelineCreateInfo.fields = computePipelineCreateInfoFields;
    computePipelineCreateInfo.fieldCount = ARRAY_SIZE(computePipelineCreateInfoFields);
    computePipelineCreateInfo.expected.alignof = 8;
    computePipelineCreateInfo.expected.size = 16;
    computePipelineCreateInfo.expected.padding = 0;
    computePipelineCreateInfo.actual = STRUCT(PalComputePipelineCreateInfo);

    StructInfo rayTracingShaderGroupCreateInfo = {0};
    rayTracingShaderGroupCreateInfo.name = "PalRayTracingShaderGroupCreateInfo";
    rayTracingShaderGroupCreateInfo.fields = rayTracingShaderGroupCreateInfoFields;
    rayTracingShaderGroupCreateInfo.fieldCount = ARRAY_SIZE(rayTracingShaderGroupCreateInfoFields);
    rayTracingShaderGroupCreateInfo.expected.alignof = 4;
    rayTracingShaderGroupCreateInfo.expected.size = 40;
    rayTracingShaderGroupCreateInfo.expected.padding = 0;
    rayTracingShaderGroupCreateInfo.actual = STRUCT(PalRayTracingShaderGroupCreateInfo);

    StructInfo rayTracingPipelineCreateInfo = {0};
    rayTracingPipelineCreateInfo.name = "PalRayTracingPipelineCreateInfo";
    rayTracingPipelineCreateInfo.fields = rayTracingPipelineCreateInfoFields;
    rayTracingPipelineCreateInfo.fieldCount = ARRAY_SIZE(rayTracingPipelineCreateInfoFields);
    rayTracingPipelineCreateInfo.expected.alignof = 8;
    rayTracingPipelineCreateInfo.expected.size = 48;
    rayTracingPipelineCreateInfo.expected.padding = 0;
    rayTracingPipelineCreateInfo.actual = STRUCT(PalRayTracingPipelineCreateInfo);

    PalBool status = checkABI(&vertexAttribute, flags);
    if (status == PAL_FALSE) {
        return status;
    }

    status = checkABI(&vertexLayout, flags);
    if (status == PAL_FALSE) {
        return status;
    }

    status = checkABI(&rasterizerState, flags);
    if (status == PAL_FALSE) {
        return status;
    }

    status = checkABI(&multisampleState, flags);
    if (status == PAL_FALSE) {
        return status;
    }

    status = checkABI(&stencilOpState, flags);
    if (status == PAL_FALSE) {
        return status;
    }

    status = checkABI(&depthStencilState, flags);
    if (status == PAL_FALSE) {
        return status;
    }

    status = checkABI(&colorBlendAttachment, flags);
    if (status == PAL_FALSE) {
        return status;
    }

    status = checkABI(&fragmentShadingRateState, flags);
    if (status == PAL_FALSE) {
        return status;
    }

    status = checkABI(&pushConstantInfo, flags);
    if (status == PAL_FALSE) {
        return status;
    }

    status = checkABI(&pipelineLayoutCreateInfo, flags);
    if (status == PAL_FALSE) {
        return status;
    }

    status = checkABI(&graphicsPipelineCreateInfo, flags);
    if (status == PAL_FALSE) {
        return status;
    }

    status = checkABI(&computePipelineCreateInfo, flags);
    if (status == PAL_FALSE) {
        return status;
    }

    status = checkABI(&rayTracingShaderGroupCreateInfo, flags);
    if (status == PAL_FALSE) {
        return status;
    }

    return checkABI(&rayTracingPipelineCreateInfo, flags);
}

static PalBool descriptorDump(uint32_t flags)
{
    // clang-format off
    FieldInfo descriptorSetLayoutBindingFields[] = {
        { "descriptorCount", {0, 4}, FIELD(PalDescriptorSetLayoutBinding, descriptorCount) },
        { "descriptorType", {4, 4}, FIELD(PalDescriptorSetLayoutBinding, descriptorType) }
    };

    FieldInfo descriptorPoolBindingSizeFields[] = {
        { "bindingCount", {0, 4}, FIELD(PalDescriptorPoolBindingSize, bindingCount) },
        { "descriptorType", {4, 4}, FIELD(PalDescriptorPoolBindingSize, descriptorType) }
    };

    FieldInfo descriptorBufferInfoFields[] = {
        { "buffer", {0, 8}, FIELD(PalDescriptorBufferInfo, buffer) },
        { "offset", {8, 8}, FIELD(PalDescriptorBufferInfo, offset) },
        { "size", {16, 8}, FIELD(PalDescriptorBufferInfo, size) },
        { "stride", {24, 8}, FIELD(PalDescriptorBufferInfo, stride) }
    };

    FieldInfo descriptorImageViewInfoFields[] = {
        { "imageView", {0, 8}, FIELD(PalDescriptorImageViewInfo, imageView) }
    };

    FieldInfo descriptorSamplerInfoFields[] = {
        { "sampler", {0, 8}, FIELD(PalDescriptorSamplerInfo, sampler) }
    };

    FieldInfo descriptorTLASInfoFields[] = {
        { "tlas", {0, 8}, FIELD(PalDescriptorTLASInfo, tlas) }
    };

    FieldInfo descriptorSetWriteInfoFields[] = {
        { "descriptorSet", {0, 8}, FIELD(PalDescriptorSetWriteInfo, descriptorSet) },
        { "bufferInfos", {8, 8}, FIELD(PalDescriptorSetWriteInfo, bufferInfos) },
        { "imageViewInfos", {16, 8}, FIELD(PalDescriptorSetWriteInfo, imageViewInfos) },
        { "samplerInfos", {24, 8}, FIELD(PalDescriptorSetWriteInfo, samplerInfos) },
        { "tlasInfos", {32, 8}, FIELD(PalDescriptorSetWriteInfo, tlasInfos) },
        { "descriptorType", {40, 4}, FIELD(PalDescriptorSetWriteInfo, descriptorType) },
        { "layoutBindingIndex", {44, 4}, FIELD(PalDescriptorSetWriteInfo, layoutBindingIndex) },
        { "arrayElement", {48, 4}, FIELD(PalDescriptorSetWriteInfo, arrayElement) },
        { "descriptorCount", {52, 4}, FIELD(PalDescriptorSetWriteInfo, descriptorCount) }
    };

    FieldInfo descriptorSetLayoutCreateInfoFields[] = {
        { "bindings", {0, 8}, FIELD(PalDescriptorSetLayoutCreateInfo, bindings) },
        { "flags", {8, 4}, FIELD(PalDescriptorSetLayoutCreateInfo, flags) },
        { "bindingCount", {12, 4}, FIELD(PalDescriptorSetLayoutCreateInfo, bindingCount) }
    };

    FieldInfo descriptorPoolCreateInfoFields[] = {
        { "bindingSizes", {0, 8}, FIELD(PalDescriptorPoolCreateInfo, bindingSizes) },
        { "bindingSizeCount", {8, 4}, FIELD(PalDescriptorPoolCreateInfo, bindingSizeCount) },
        { "maxDescriptorSets", {12, 4}, FIELD(PalDescriptorPoolCreateInfo, maxDescriptorSets) },
        { "flags", {16, 4}, FIELD(PalDescriptorPoolCreateInfo, flags) },
        { "reserved", {20, 4}, FIELD(PalDescriptorPoolCreateInfo, reserved) }
    };
    // clang-format on

    StructInfo descriptorSetLayoutBinding = {0};
    descriptorSetLayoutBinding.name = "PalDescriptorSetLayoutBinding";
    descriptorSetLayoutBinding.fields = descriptorSetLayoutBindingFields;
    descriptorSetLayoutBinding.fieldCount = ARRAY_SIZE(descriptorSetLayoutBindingFields);
    descriptorSetLayoutBinding.expected.alignof = 4;
    descriptorSetLayoutBinding.expected.size = 8;
    descriptorSetLayoutBinding.expected.padding = 0;
    descriptorSetLayoutBinding.actual = STRUCT(PalDescriptorSetLayoutBinding);

    StructInfo descriptorPoolBindingSize = {0};
    descriptorPoolBindingSize.name = "PalDescriptorPoolBindingSize";
    descriptorPoolBindingSize.fields = descriptorPoolBindingSizeFields;
    descriptorPoolBindingSize.fieldCount = ARRAY_SIZE(descriptorPoolBindingSizeFields);
    descriptorPoolBindingSize.expected.alignof = 4;
    descriptorPoolBindingSize.expected.size = 8;
    descriptorPoolBindingSize.expected.padding = 0;
    descriptorPoolBindingSize.actual = STRUCT(PalDescriptorPoolBindingSize);

    StructInfo descriptorBufferInfo = {0};
    descriptorBufferInfo.name = "PalDescriptorBufferInfo";
    descriptorBufferInfo.fields = descriptorBufferInfoFields;
    descriptorBufferInfo.fieldCount = ARRAY_SIZE(descriptorBufferInfoFields);
    descriptorBufferInfo.expected.alignof = 8;
    descriptorBufferInfo.expected.size = 32;
    descriptorBufferInfo.expected.padding = 0;
    descriptorBufferInfo.actual = STRUCT(PalDescriptorBufferInfo);

    StructInfo descriptorImageViewInfo = {0};
    descriptorImageViewInfo.name = "PalDescriptorImageViewInfo";
    descriptorImageViewInfo.fields = descriptorImageViewInfoFields;
    descriptorImageViewInfo.fieldCount = ARRAY_SIZE(descriptorImageViewInfoFields);
    descriptorImageViewInfo.expected.alignof = 8;
    descriptorImageViewInfo.expected.size = 8;
    descriptorImageViewInfo.expected.padding = 0;
    descriptorImageViewInfo.actual = STRUCT(PalDescriptorImageViewInfo);

    StructInfo descriptorSamplerInfo = {0};
    descriptorSamplerInfo.name = "PalDescriptorSamplerInfo";
    descriptorSamplerInfo.fields = descriptorSamplerInfoFields;
    descriptorSamplerInfo.fieldCount = ARRAY_SIZE(descriptorSamplerInfoFields);
    descriptorSamplerInfo.expected.alignof = 8;
    descriptorSamplerInfo.expected.size = 8;
    descriptorSamplerInfo.expected.padding = 0;
    descriptorSamplerInfo.actual = STRUCT(PalDescriptorSamplerInfo);

    StructInfo descriptorTLASInfo = {0};
    descriptorTLASInfo.name = "PalDescriptorTLASInfo";
    descriptorTLASInfo.fields = descriptorSamplerInfoFields;
    descriptorTLASInfo.fieldCount = ARRAY_SIZE(descriptorSamplerInfoFields);
    descriptorTLASInfo.expected.alignof = 8;
    descriptorTLASInfo.expected.size = 8;
    descriptorTLASInfo.expected.padding = 0;
    descriptorTLASInfo.actual = STRUCT(PalDescriptorTLASInfo);

    StructInfo descriptorSetWriteInfo = {0};
    descriptorSetWriteInfo.name = "PalDescriptorSetWriteInfo";
    descriptorSetWriteInfo.fields = descriptorSetWriteInfoFields;
    descriptorSetWriteInfo.fieldCount = ARRAY_SIZE(descriptorSetWriteInfoFields);
    descriptorSetWriteInfo.expected.alignof = 8;
    descriptorSetWriteInfo.expected.size = 56;
    descriptorSetWriteInfo.expected.padding = 0;
    descriptorSetWriteInfo.actual = STRUCT(PalDescriptorSetWriteInfo);

    StructInfo descriptorSetLayoutCreateInfo = {0};
    descriptorSetLayoutCreateInfo.name = "PalDescriptorSetLayoutCreateInfo";
    descriptorSetLayoutCreateInfo.fields = descriptorSetLayoutCreateInfoFields;
    descriptorSetLayoutCreateInfo.fieldCount = ARRAY_SIZE(descriptorSetLayoutCreateInfoFields);
    descriptorSetLayoutCreateInfo.expected.alignof = 8;
    descriptorSetLayoutCreateInfo.expected.size = 16;
    descriptorSetLayoutCreateInfo.expected.padding = 0;
    descriptorSetLayoutCreateInfo.actual = STRUCT(PalDescriptorSetLayoutCreateInfo);

    StructInfo descriptorPoolCreateInfo = {0};
    descriptorPoolCreateInfo.name = "PalDescriptorPoolCreateInfo";
    descriptorPoolCreateInfo.fields = descriptorPoolCreateInfoFields;
    descriptorPoolCreateInfo.fieldCount = ARRAY_SIZE(descriptorPoolCreateInfoFields);
    descriptorPoolCreateInfo.expected.alignof = 8;
    descriptorPoolCreateInfo.expected.size = 24;
    descriptorPoolCreateInfo.expected.padding = 0;
    descriptorPoolCreateInfo.actual = STRUCT(PalDescriptorPoolCreateInfo);

    PalBool status = checkABI(&descriptorSetLayoutBinding, flags);
    if (status == PAL_FALSE) {
        return status;
    }

    status = checkABI(&descriptorPoolBindingSize, flags);
    if (status == PAL_FALSE) {
        return status;
    }

    status = checkABI(&descriptorBufferInfo, flags);
    if (status == PAL_FALSE) {
        return status;
    }

    status = checkABI(&descriptorImageViewInfo, flags);
    if (status == PAL_FALSE) {
        return status;
    }

    status = checkABI(&descriptorSamplerInfo, flags);
    if (status == PAL_FALSE) {
        return status;
    }

    status = checkABI(&descriptorTLASInfo, flags);
    if (status == PAL_FALSE) {
        return status;
    }

    status = checkABI(&descriptorSetWriteInfo, flags);
    if (status == PAL_FALSE) {
        return status;
    }

    status = checkABI(&descriptorSetLayoutCreateInfo, flags);
    if (status == PAL_FALSE) {
        return status;
    }

    return checkABI(&descriptorPoolCreateInfo, flags);
}

static PalBool sbtDump(uint32_t flags)
{
    // clang-format off
    FieldInfo shaderBindingTableRecordInfoFields[] = {
        { "localData", {0, 8}, FIELD(PalShaderBindingTableRecordInfo, localData) },
        { "groupIndex", {8, 4}, FIELD(PalShaderBindingTableRecordInfo, groupIndex) },
        { "localDataSize", {12, 4}, FIELD(PalShaderBindingTableRecordInfo, localDataSize) }
    };

    FieldInfo shaderBindingTableCreateInfoFields[] = {
        { "records", {0, 8}, FIELD(PalShaderBindingTableCreateInfo, records) },
        { "rayTracingPipeline", {8, 8}, FIELD(PalShaderBindingTableCreateInfo, rayTracingPipeline) },
        { "recordCount", {16, 4}, FIELD(PalShaderBindingTableCreateInfo, recordCount) },
        { "reserved", {20, 4}, FIELD(PalShaderBindingTableCreateInfo, reserved) }
    };
    // clang-format on

    StructInfo shaderBindingTableRecordInfo = {0};
    shaderBindingTableRecordInfo.name = "PalShaderBindingTableRecordInfo";
    shaderBindingTableRecordInfo.fields = shaderBindingTableRecordInfoFields;
    shaderBindingTableRecordInfo.fieldCount = ARRAY_SIZE(shaderBindingTableRecordInfoFields);
    shaderBindingTableRecordInfo.expected.alignof = 8;
    shaderBindingTableRecordInfo.expected.size = 16;
    shaderBindingTableRecordInfo.expected.padding = 0;
    shaderBindingTableRecordInfo.actual = STRUCT(PalShaderBindingTableRecordInfo);

    StructInfo shaderBindingTableCreateInfo = {0};
    shaderBindingTableCreateInfo.name = "PalShaderBindingTableCreateInfo";
    shaderBindingTableCreateInfo.fields = shaderBindingTableCreateInfoFields;
    shaderBindingTableCreateInfo.fieldCount = ARRAY_SIZE(shaderBindingTableCreateInfoFields);
    shaderBindingTableCreateInfo.expected.alignof = 8;
    shaderBindingTableCreateInfo.expected.size = 24;
    shaderBindingTableCreateInfo.expected.padding = 0;
    shaderBindingTableCreateInfo.actual = STRUCT(PalShaderBindingTableCreateInfo);

    PalBool status = checkABI(&shaderBindingTableRecordInfo, flags);
    if (status == PAL_FALSE) {
        return status;
    }

    return checkABI(&shaderBindingTableCreateInfo, flags);
}

static PalBool commandPoolDump(uint32_t flags)
{
    // clang-format off
    FieldInfo commandBufferSubmitInfoFields[] = {
        { "waitValue", {0, 8}, FIELD(PalCommandBufferSubmitInfo, waitValue) },
        { "signalValue", {8, 8}, FIELD(PalCommandBufferSubmitInfo, signalValue) },
        { "cmdBuffer", {16, 8}, FIELD(PalCommandBufferSubmitInfo, cmdBuffer) },
        { "waitSemaphore", {24, 8}, FIELD(PalCommandBufferSubmitInfo, waitSemaphore) },
        { "signalSemaphore", {32, 8}, FIELD(PalCommandBufferSubmitInfo, signalSemaphore) },
        { "fence", {40, 8}, FIELD(PalCommandBufferSubmitInfo, fence) },
        { "waitStages", {48, 4}, FIELD(PalCommandBufferSubmitInfo, waitStages) },
        { "signalStages", {52, 4}, FIELD(PalCommandBufferSubmitInfo, signalStages) }
    };
    // clang-format on

    StructInfo commandBufferSubmitInfo = {0};
    commandBufferSubmitInfo.name = "PalCommandBufferSubmitInfo";
    commandBufferSubmitInfo.fields = commandBufferSubmitInfoFields;
    commandBufferSubmitInfo.fieldCount = ARRAY_SIZE(commandBufferSubmitInfoFields);
    commandBufferSubmitInfo.expected.alignof = 8;
    commandBufferSubmitInfo.expected.size = 56;
    commandBufferSubmitInfo.expected.padding = 0;
    commandBufferSubmitInfo.actual = STRUCT(PalCommandBufferSubmitInfo);

    return checkABI(&commandBufferSubmitInfo, flags);
}

static PalBool commandsDump(uint32_t flags)
{
    // clang-format off
    FieldInfo clearValueFields[] = {
        { "color", {0, 16}, FIELD(PalClearValue, color) },
        { "depth", {16, 4}, FIELD(PalClearValue, depth) },
        { "stencil", {20, 4}, FIELD(PalClearValue, stencil) }
    };

    FieldInfo attachmentDescFields[] = {
        { "imageView", {0, 8}, FIELD(PalAttachmentDesc, imageView) },
        { "resolveImageView", {8, 8}, FIELD(PalAttachmentDesc, resolveImageView) },
        { "loadOp", {16, 4}, FIELD(PalAttachmentDesc, loadOp) },
        { "storeOp", {20, 4}, FIELD(PalAttachmentDesc, storeOp) },
        { "stencilLoadOp", {24, 4}, FIELD(PalAttachmentDesc, stencilLoadOp) },
        { "stencilStoreOp", {28, 4}, FIELD(PalAttachmentDesc, stencilStoreOp) },
        { "resolveMode", {32, 4}, FIELD(PalAttachmentDesc, resolveMode) },
        { "stencilResolveMode", {36, 4}, FIELD(PalAttachmentDesc, stencilResolveMode) },
        { "clearValue", {40, 24}, FIELD(PalAttachmentDesc, clearValue) },
    };

    FieldInfo viewportFields[] = {
        { "x", {0, 4}, FIELD(PalViewport, x) },
        { "y", {4, 4}, FIELD(PalViewport, y) },
        { "width", {8, 4}, FIELD(PalViewport, width) },
        { "height", {12, 4}, FIELD(PalViewport, height) },
        { "minDepth", {16, 4}, FIELD(PalViewport, minDepth) },
        { "maxDepth", {20, 4}, FIELD(PalViewport, maxDepth) }
    };

    FieldInfo rect2DFields[] = {
        { "x", {0, 4}, FIELD(PalRect2D, x) },
        { "y", {4, 4}, FIELD(PalRect2D, y) },
        { "width", {8, 4}, FIELD(PalRect2D, width) },
        { "height", {12, 4}, FIELD(PalRect2D, height) },
    };

    FieldInfo renderingInfoFields[] = {
        { "colorAttachments", {0, 8}, FIELD(PalRenderingInfo, colorAttachments) },
        { "depthStencilAttachment", {8, 8}, FIELD(PalRenderingInfo, depthStencilAttachment) },
        { "fragmentShadingRateImageView", {16, 8}, FIELD(PalRenderingInfo, fragmentShadingRateImageView) },
        { "renderArea", {24, 16}, FIELD(PalRenderingInfo, renderArea) },
        { "flags", {40, 4}, FIELD(PalRenderingInfo, flags) },
        { "fragmentShadingRateTexelWidth", {44, 4}, FIELD(PalRenderingInfo, fragmentShadingRateTexelWidth) },
        { "fragmentShadingRateTexelHeight", {48, 4}, FIELD(PalRenderingInfo, fragmentShadingRateTexelHeight) },
        { "viewCount", {52, 4}, FIELD(PalRenderingInfo, viewCount) },
        { "arrayLayerCount", {56, 4}, FIELD(PalRenderingInfo, arrayLayerCount) },
        { "colorAttachentCount", {60, 4}, FIELD(PalRenderingInfo, colorAttachentCount) }
    };

    FieldInfo renderingLayoutInfoFields[] = {
        { "colorAttachmentsFormat", {0, 8}, FIELD(PalRenderingLayoutInfo, colorAttachmentsFormat) },
        { "colorAttachentCount", {8, 4}, FIELD(PalRenderingLayoutInfo, colorAttachentCount) },
        { "viewCount", {12, 4}, FIELD(PalRenderingLayoutInfo, viewCount) },
        { "sampleCount", {16, 4}, FIELD(PalRenderingLayoutInfo, sampleCount) },
        { "flags", {20, 4}, FIELD(PalRenderingLayoutInfo, flags) },
        { "depthStencilAttachmentFormat", {24, 4}, FIELD(PalRenderingLayoutInfo, depthStencilAttachmentFormat) },
        { "fragmentShadingRateAttachmentFormat", {28, 4}, FIELD(PalRenderingLayoutInfo, fragmentShadingRateAttachmentFormat) }
    };

    FieldInfo workGroupBuildDataFields[] = {
        { "workCount", {0, 12}, FIELD(PalWorkGroupBuildData, workCount) },
        { "workGroupSize", {12, 12}, FIELD(PalWorkGroupBuildData, workGroupSize) },
        { "workGroupCount", {24, 12}, FIELD(PalWorkGroupBuildData, workGroupCount) }
    };

    FieldInfo workGroupInfoFields[] = {
        { "workGroupBase", {0, 12}, FIELD(PalWorkGroupInfo, workGroupBase) },
        { "workGroupCount", {12, 12}, FIELD(PalWorkGroupInfo, workGroupCount) }
    };

    FieldInfo barrierInfoFields[] = {
        { "oldState", {0, 4}, FIELD(PalBarrierInfo, oldState) },
        { "newState", {4, 4}, FIELD(PalBarrierInfo, newState) },
        { "srcStages", {8, 4}, FIELD(PalBarrierInfo, srcStages) },
        { "dstStages", {12, 4}, FIELD(PalBarrierInfo, dstStages) }
    };
    // clang-format on

    StructInfo clearValue = {0};
    clearValue.name = "PalClearValue";
    clearValue.fields = clearValueFields;
    clearValue.fieldCount = ARRAY_SIZE(clearValueFields);
    clearValue.expected.alignof = 4;
    clearValue.expected.size = 24;
    clearValue.expected.padding = 0;
    clearValue.actual = STRUCT(PalClearValue);

    StructInfo attachmentDesc = {0};
    attachmentDesc.name = "PalAttachmentDesc";
    attachmentDesc.fields = attachmentDescFields;
    attachmentDesc.fieldCount = ARRAY_SIZE(attachmentDescFields);
    attachmentDesc.expected.alignof = 8;
    attachmentDesc.expected.size = 64;
    attachmentDesc.expected.padding = 0;
    attachmentDesc.actual = STRUCT(PalAttachmentDesc);

    StructInfo viewport = {0};
    viewport.name = "PalViewport";
    viewport.fields = viewportFields;
    viewport.fieldCount = ARRAY_SIZE(viewportFields);
    viewport.expected.alignof = 4;
    viewport.expected.size = 24;
    viewport.expected.padding = 0;
    viewport.actual = STRUCT(PalViewport);

    StructInfo rect2D = {0};
    rect2D.name = "PalRect2D";
    rect2D.fields = rect2DFields;
    rect2D.fieldCount = ARRAY_SIZE(rect2DFields);
    rect2D.expected.alignof = 4;
    rect2D.expected.size = 16;
    rect2D.expected.padding = 0;
    rect2D.actual = STRUCT(PalRect2D);

    StructInfo renderingInfo = {0};
    renderingInfo.name = "PalRenderingInfo";
    renderingInfo.fields = renderingInfoFields;
    renderingInfo.fieldCount = ARRAY_SIZE(renderingInfoFields);
    renderingInfo.expected.alignof = 8;
    renderingInfo.expected.size = 64;
    renderingInfo.expected.padding = 0;
    renderingInfo.actual = STRUCT(PalRenderingInfo);

    StructInfo renderingLayoutInfo = {0};
    renderingLayoutInfo.name = "PalRenderingLayoutInfo";
    renderingLayoutInfo.fields = renderingLayoutInfoFields;
    renderingLayoutInfo.fieldCount = ARRAY_SIZE(renderingLayoutInfoFields);
    renderingLayoutInfo.expected.alignof = 8;
    renderingLayoutInfo.expected.size = 32;
    renderingLayoutInfo.expected.padding = 0;
    renderingLayoutInfo.actual = STRUCT(PalRenderingLayoutInfo);

    StructInfo workGroupBuildData = {0};
    workGroupBuildData.name = "PalWorkGroupBuildData";
    workGroupBuildData.fields = workGroupBuildDataFields;
    workGroupBuildData.fieldCount = ARRAY_SIZE(workGroupBuildDataFields);
    workGroupBuildData.expected.alignof = 4;
    workGroupBuildData.expected.size = 36;
    workGroupBuildData.expected.padding = 0;
    workGroupBuildData.actual = STRUCT(PalWorkGroupBuildData);

    StructInfo workGroupInfo = {0};
    workGroupInfo.name = "PalWorkGroupInfo";
    workGroupInfo.fields = workGroupInfoFields;
    workGroupInfo.fieldCount = ARRAY_SIZE(workGroupInfoFields);
    workGroupInfo.expected.alignof = 4;
    workGroupInfo.expected.size = 24;
    workGroupInfo.expected.padding = 0;
    workGroupInfo.actual = STRUCT(PalWorkGroupInfo);

    StructInfo barrierInfo = {0};
    barrierInfo.name = "PalBarrierInfo";
    barrierInfo.fields = barrierInfoFields;
    barrierInfo.fieldCount = ARRAY_SIZE(barrierInfoFields);
    barrierInfo.expected.alignof = 4;
    barrierInfo.expected.size = 16;
    barrierInfo.expected.padding = 0;
    barrierInfo.actual = STRUCT(PalBarrierInfo);

    PalBool status = checkABI(&clearValue, flags);
    if (status == PAL_FALSE) {
        return status;
    }

    status = checkABI(&attachmentDesc, flags);
    if (status == PAL_FALSE) {
        return status;
    }

    status = checkABI(&viewport, flags);
    if (status == PAL_FALSE) {
        return status;
    }

    status = checkABI(&rect2D, flags);
    if (status == PAL_FALSE) {
        return status;
    }

    status = checkABI(&renderingInfo, flags);
    if (status == PAL_FALSE) {
        return status;
    }

    status = checkABI(&renderingLayoutInfo, flags);
    if (status == PAL_FALSE) {
        return status;
    }

    status = checkABI(&workGroupBuildData, flags);
    if (status == PAL_FALSE) {
        return status;
    }

    status = checkABI(&workGroupInfo, flags);
    if (status == PAL_FALSE) {
        return status;
    }

    return checkABI(&barrierInfo, flags);
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

    // clang-format off
    FieldInfo graphicsDebuggerFields[] = {
        { "userData", {0, 8}, FIELD(PalGraphicsDebugger, userData) },
        { "callback", {8, 8}, FIELD(PalGraphicsDebugger, callback) },
        { "enableGPUValidation", {16, 4}, FIELD(PalGraphicsDebugger, enableGPUValidation) },
        { "denyGeneral", {20, 4}, FIELD(PalGraphicsDebugger, denyGeneral) },
        { "denyValidation", {24, 4}, FIELD(PalGraphicsDebugger, denyValidation) },
        { "denyPerformance", {28, 4}, FIELD(PalGraphicsDebugger, denyPerformance) },
        { "denyInfoSeverity", {32, 4}, FIELD(PalGraphicsDebugger, denyInfoSeverity) },
        { "denyWarningSeverity", {36, 4}, FIELD(PalGraphicsDebugger, denyWarningSeverity) },
        { "denyErrorSeverity", {40, 4}, FIELD(PalGraphicsDebugger, denyErrorSeverity) },
        { "reserved", {44, 4}, FIELD(PalGraphicsDebugger, reserved) }
    };

    FieldInfo graphicsBackendInfoFields[] = {
        { "vtable", {0, 8}, FIELD(PalGraphicsBackendInfo, vtable) },
        { "version", {8, 4}, FIELD(PalGraphicsBackendInfo, version) },
        { "reserved", {12, 4}, FIELD(PalGraphicsBackendInfo, reserved) }
    };
    // clang-format on

    StructInfo graphicsDebugger = {0};
    graphicsDebugger.name = "PalGraphicsDebugger";
    graphicsDebugger.fields = graphicsDebuggerFields;
    graphicsDebugger.fieldCount = ARRAY_SIZE(graphicsDebuggerFields);
    graphicsDebugger.expected.alignof = 8;
    graphicsDebugger.expected.size = 48;
    graphicsDebugger.expected.padding = 0;
    graphicsDebugger.actual = STRUCT(PalGraphicsDebugger);

    StructInfo graphicsBackendInfo = {0};
    graphicsBackendInfo.name = "PalGraphicsBackendInfo";
    graphicsBackendInfo.fields = graphicsBackendInfoFields;
    graphicsBackendInfo.fieldCount = ARRAY_SIZE(graphicsBackendInfoFields);
    graphicsBackendInfo.expected.alignof = 8;
    graphicsBackendInfo.expected.size = 16;
    graphicsBackendInfo.expected.padding = 0;
    graphicsBackendInfo.actual = STRUCT(PalGraphicsBackendInfo);

    PalBool status = checkABI(&graphicsDebugger, flags);
    if (status == PAL_FALSE) {
        return status;
    }

    status = checkABI(&graphicsBackendInfo, flags);
    if (status == PAL_FALSE) {
        return status;
    }

    status = adapterDump(flags);
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

    status = accelerationStructureDump(flags);
    if (status == PAL_FALSE) {
        return status;
    }

    status = pipelineDump(flags);
    if (status == PAL_FALSE) {
        return status;
    }

    status = descriptorDump(flags);
    if (status == PAL_FALSE) {
        return status;
    }

    status = sbtDump(flags);
    if (status == PAL_FALSE) {
        return status;
    }

    status = commandPoolDump(flags);
    if (status == PAL_FALSE) {
        return status;
    }

    return commandsDump(flags);
}