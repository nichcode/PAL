
#include "helpers.h"
#include "pal2/pal_graphics.h"

bool adapterInfoStruct(void)
{
    FieldInfo fields[] = {
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
        { "vtableVersion", {204, 4}, FIELD(PalAdapterInfo, vtableVersion) }
    };

    StructInfo info = {0};
    info.name = "PalAdapterInfo";
    info.fields = fields;
    info.fieldCount = ARRAY_SIZE(fields);
    info.expected.align = 8;
    info.expected.size = 208;
    info.expected.padding = 0;
    info.actual = STRUCT(PalAdapterInfo);

    return dumpStruct(&info);
}

bool imageCapabilitiesStruct(void)
{
    FieldInfo fields[] = {
        { "maxWidth", {0, 4}, FIELD(PalImageCapabilities, maxWidth) },
        { "maxHeight", {4, 4}, FIELD(PalImageCapabilities, maxHeight) },
        { "maxDepth", {8, 4}, FIELD(PalImageCapabilities, maxDepth) },
        { "maxArrayLayers", {12, 4}, FIELD(PalImageCapabilities, maxArrayLayers) },
        { "maxMipLevels", {16, 4}, FIELD(PalImageCapabilities, maxMipLevels) }
    };

    StructInfo info = {0};
    info.name = "PalImageCapabilities";
    info.fields = fields;
    info.fieldCount = ARRAY_SIZE(fields);
    info.expected.align = 4;
    info.expected.size = 20;
    info.expected.padding = 0;
    info.actual = STRUCT(PalImageCapabilities);

    return dumpStruct(&info);
}

bool resourceCapabilitiesStruct(void)
{
    FieldInfo fields[] = {
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

    StructInfo info = {0};
    info.name = "PalResourceCapabilities";
    info.fields = fields;
    info.fieldCount = ARRAY_SIZE(fields);
    info.expected.align = 4;
    info.expected.size = 52;
    info.expected.padding = 0;
    info.actual = STRUCT(PalResourceCapabilities);

    return dumpStruct(&info);
}

bool computeCapabilitiesStruct(void)
{
    FieldInfo fields[] = {
        { "maxWorkGroupInvocations", {0, 4}, FIELD(PalComputeCapabilities, maxWorkGroupInvocations) },
        { "maxWorkGroupCount", {4, 12}, FIELD(PalComputeCapabilities, maxWorkGroupCount) },
        { "maxWorkGroupSize", {16, 12}, FIELD(PalComputeCapabilities, maxWorkGroupSize) }
    };

    StructInfo info = {0};
    info.name = "PalComputeCapabilities";
    info.fields = fields;
    info.fieldCount = ARRAY_SIZE(fields);
    info.expected.align = 4;
    info.expected.size = 28;
    info.expected.padding = 0;
    info.actual = STRUCT(PalComputeCapabilities);

    return dumpStruct(&info);
}

bool viewportCapabilitiesStruct(void)
{
    FieldInfo fields[] = {
        { "maxWidth", {0, 4}, FIELD(PalViewportCapabilities, maxWidth) },
        { "maxHeight", {4, 4}, FIELD(PalViewportCapabilities, maxHeight) },
        { "minBoundsRange", {8, 4}, FIELD(PalViewportCapabilities, minBoundsRange) },
        { "maxBoundsRange", {12, 4}, FIELD(PalViewportCapabilities, maxBoundsRange) }
    };
   
    StructInfo info = {0};
    info.name = "PalViewportCapabilities";
    info.fields = fields;
    info.fieldCount = ARRAY_SIZE(fields);
    info.expected.align = 4;
    info.expected.size = 16;
    info.expected.padding = 0;
    info.actual = STRUCT(PalViewportCapabilities);

    return dumpStruct(&info);
}

bool adapterCapabilitiesStruct(void)
{
    FieldInfo fields[] = {
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

    StructInfo info = {0};
    info.name = "PalAdapterCapabilities";
    info.fields = fields;
    info.fieldCount = ARRAY_SIZE(fields);
    info.expected.align = 4;
    info.expected.size = 156;
    info.expected.padding = 0;
    info.actual = STRUCT(PalAdapterCapabilities);

    return dumpStruct(&info);
}

bool formatInfoStruct(void)
{
    FieldInfo fields[] = {
        { "usages", {0, 4}, FIELD(PalFormatInfo, usages) },
        { "format", {4, 4}, FIELD(PalFormatInfo, format) },
        { "sampleCount", {8, 4}, FIELD(PalFormatInfo, sampleCount) }
    };

    StructInfo info = {0};
    info.name = "PalFormatInfo";
    info.fields = fields;
    info.fieldCount = ARRAY_SIZE(fields);
    info.expected.align = 4;
    info.expected.size = 12;
    info.expected.padding = 0;
    info.actual = STRUCT(PalFormatInfo);

    return dumpStruct(&info);
}

bool samplerAnisotropyCapabilitiesStruct(void)
{
    FieldInfo fields[] = {
        { "maxAnisotropy", {0, 4}, FIELD(PalSamplerAnisotropyCapabilities, maxAnisotropy) }
    };
    
    StructInfo info = {0};
    info.name = "PalSamplerAnisotropyCapabilities";
    info.fields = fields;
    info.fieldCount = ARRAY_SIZE(fields);
    info.expected.align = 4;
    info.expected.size = 4;
    info.expected.padding = 0;
    info.actual = STRUCT(PalSamplerAnisotropyCapabilities);

    return dumpStruct(&info);
}

bool multiViewCapabilitiesStruct(void)
{
    FieldInfo fields[] = {
        { "maxViewCount", {0, 4}, FIELD(PalMultiViewCapabilities, maxViewCount) }
    };

    StructInfo info = {0};
    info.name = "PalMultiViewCapabilities";
    info.fields = fields;
    info.fieldCount = ARRAY_SIZE(fields);
    info.expected.align = 4;
    info.expected.size = 4;
    info.expected.padding = 0;
    info.actual = STRUCT(PalMultiViewCapabilities);

    return dumpStruct(&info);
}

bool multiViewportCapabilitiesStruct(void)
{
    FieldInfo fields[] = {
        { "maxCount", {0, 4}, FIELD(PalMultiViewportCapabilities, maxCount) }
    };

    StructInfo info = {0};
    info.name = "PalMultiViewportCapabilities";
    info.fields = fields;
    info.fieldCount = ARRAY_SIZE(fields);
    info.expected.align = 4;
    info.expected.size = 4;
    info.expected.padding = 0;
    info.actual = STRUCT(PalMultiViewportCapabilities);

    return dumpStruct(&info);
}

bool depthStencilCapabilitiesStruct(void)
{
    FieldInfo fields[] = {
        { "supportedDepthResolveModes", {0, 4}, FIELD(PalDepthStencilCapabilities, supportedDepthResolveModes) },
        { "supportedStencilResolveModes", {4, 4}, FIELD(PalDepthStencilCapabilities, supportedStencilResolveModes) },
        { "supportsIndependentResolve", {8, 4}, FIELD(PalDepthStencilCapabilities, supportsIndependentResolve) },
        { "supportsIndependentResolveNone", {12, 4}, FIELD(PalDepthStencilCapabilities, supportsIndependentResolveNone) }
    };

    StructInfo info = {0};
    info.name = "PalDepthStencilCapabilities";
    info.fields = fields;
    info.fieldCount = ARRAY_SIZE(fields);
    info.expected.align = 4;
    info.expected.size = 16;
    info.expected.padding = 0;
    info.actual = STRUCT(PalDepthStencilCapabilities);

    return dumpStruct(&info);
}

bool fragmentShadingRateCapabilitiesStruct(void)
{
    FieldInfo fields[] = {
        { "supportedShadingRates", {0, 4}, FIELD(PalFragmentShadingRateCapabilities, supportedShadingRates) },
        { "supportedCombinerOps", {4, 4}, FIELD(PalFragmentShadingRateCapabilities, supportedCombinerOps) },
        { "minTexelWidth", {8, 4}, FIELD(PalFragmentShadingRateCapabilities, minTexelWidth) },
        { "minTexelHeight", {12, 4}, FIELD(PalFragmentShadingRateCapabilities, minTexelHeight) },
        { "maxTexelWidth", {16, 4}, FIELD(PalFragmentShadingRateCapabilities, maxTexelWidth) },
        { "maxTexelHeight", {20, 4}, FIELD(PalFragmentShadingRateCapabilities, maxTexelHeight) }
    };

    StructInfo info = {0};
    info.name = "PalFragmentShadingRateCapabilities";
    info.fields = fields;
    info.fieldCount = ARRAY_SIZE(fields);
    info.expected.align = 4;
    info.expected.size = 24;
    info.expected.padding = 0;
    info.actual = STRUCT(PalFragmentShadingRateCapabilities);

    return dumpStruct(&info);
}

bool meshCapabilitiesStruct(void)
{
    FieldInfo fields[] = {
        { "maxOutputPrimitives", {0, 4}, FIELD(PalMeshShaderCapabilities, maxOutputPrimitives) },
        { "maxOutputVertices", {4, 4}, FIELD(PalMeshShaderCapabilities, maxOutputVertices) },
        { "maxWorkGroupInvocations", {8, 4}, FIELD(PalMeshShaderCapabilities, maxWorkGroupInvocations) },
        { "maxTaskWorkGroupInvocations", {12, 4}, FIELD(PalMeshShaderCapabilities, maxTaskWorkGroupInvocations) },
        { "maxWorkGroupCount", {16, 12}, FIELD(PalMeshShaderCapabilities, maxWorkGroupCount) },
        { "maxTaskWorkGroupCount", {28, 12}, FIELD(PalMeshShaderCapabilities, maxTaskWorkGroupCount) }
    };

    StructInfo info = {0};
    info.name = "PalMeshShaderCapabilities";
    info.fields = fields;
    info.fieldCount = ARRAY_SIZE(fields);
    info.expected.align = 4;
    info.expected.size = 40;
    info.expected.padding = 0;
    info.actual = STRUCT(PalMeshShaderCapabilities);

    return dumpStruct(&info);
}

bool rayTracingCapabilitiesStruct(void)
{
    FieldInfo fields[] = {
        { "maxRecursionDepth", {0, 4}, FIELD(PalRayTracingCapabilities, maxRecursionDepth) },
        { "maxHitAttributeSize", {4, 4}, FIELD(PalRayTracingCapabilities, maxHitAttributeSize) },
        { "maxInstanceCount", {8, 4}, FIELD(PalRayTracingCapabilities, maxInstanceCount) },
        { "maxPrimitiveCount", {12, 4}, FIELD(PalRayTracingCapabilities, maxPrimitiveCount) },
        { "maxGeometryCount", {16, 4}, FIELD(PalRayTracingCapabilities, maxGeometryCount) },
        { "maxPayloadSize", {20, 4}, FIELD(PalRayTracingCapabilities, maxPayloadSize) },
        { "maxDispatchInvocations", {24, 4}, FIELD(PalRayTracingCapabilities, maxDispatchInvocations) }
    };

    StructInfo info = {0};
    info.name = "PalRayTracingCapabilities";
    info.fields = fields;
    info.fieldCount = ARRAY_SIZE(fields);
    info.expected.align = 4;
    info.expected.size = 28;
    info.expected.padding = 0;
    info.actual = STRUCT(PalRayTracingCapabilities);

    return dumpStruct(&info);
}

bool descriptorIndexingCapabilitiesStruct(void)
{
    FieldInfo fields[] = {
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

    StructInfo info = {0};
    info.name = "PalDescriptorIndexingCapabilities";
    info.fields = fields;
    info.fieldCount = ARRAY_SIZE(fields);
    info.expected.align = 4;
    info.expected.size = 52;
    info.expected.padding = 0;
    info.actual = STRUCT(PalDescriptorIndexingCapabilities);

    return dumpStruct(&info);
}

bool memoryRequirementsStruct(void)
{
    FieldInfo fields[] = {
        { "size", {0, 8}, FIELD(PalMemoryRequirements, size) },
        { "alignment", {8, 8}, FIELD(PalMemoryRequirements, alignment) },
        { "memoryMask", {16, 8}, FIELD(PalMemoryRequirements, memoryMask) },
        { "supportedMemoryTypes", {24, 4}, FIELD(PalMemoryRequirements, supportedMemoryTypes) },
        { "reserved", {28, 4}, FIELD(PalMemoryRequirements, reserved) }
    };

    StructInfo info = {0};
    info.name = "PalMemoryRequirements";
    info.fields = fields;
    info.fieldCount = ARRAY_SIZE(fields);
    info.expected.align = 8;
    info.expected.size = 32;
    info.expected.padding = 0;
    info.actual = STRUCT(PalMemoryRequirements);

    return dumpStruct(&info);
}

bool shaderEntryInfoStruct(void)
{
    FieldInfo fields[] = {
        { "entryName", {0, 8}, FIELD(PalShaderEntryInfo, entryName) },
        { "stage", {8, 4}, FIELD(PalShaderEntryInfo, stage) },
        { "patchControlPoints", {12, 4}, FIELD(PalShaderEntryInfo, patchControlPoints) }
    };

    StructInfo info = {0};
    info.name = "PalShaderEntryInfo";
    info.fields = fields;
    info.fieldCount = ARRAY_SIZE(fields);
    info.expected.align = 8;
    info.expected.size = 16;
    info.expected.padding = 0;
    info.actual = STRUCT(PalShaderEntryInfo);

    return dumpStruct(&info);
}

bool shaderCreateInfoStruct(void)
{
    FieldInfo fields[] = {
        { "bcode", {0, 8}, FIELD(PalShaderCreateInfo, code) },
        { "entries", {8, 8}, FIELD(PalShaderCreateInfo, entries) },
        { "codeSize", {16, 4}, FIELD(PalShaderCreateInfo, codeSize) },
        { "entryCount", {20, 4}, FIELD(PalShaderCreateInfo, entryCount) }
    };

    StructInfo info = {0};
    info.name = "PalShaderCreateInfo";
    info.fields = fields;
    info.fieldCount = ARRAY_SIZE(fields);
    info.expected.align = 8;
    info.expected.size = 24;
    info.expected.padding = 0;
    info.actual = STRUCT(PalShaderCreateInfo);

    return dumpStruct(&info);
}

bool surfaceCapabilitiesStruct(void)
{
    FieldInfo fields[] = {
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

    StructInfo info = {0};
    info.name = "PalSurfaceCapabilities";
    info.fields = fields;
    info.fieldCount = ARRAY_SIZE(fields);
    info.expected.align = 4;
    info.expected.size = 40;
    info.expected.padding = 0;
    info.actual = STRUCT(PalSurfaceCapabilities);

    return dumpStruct(&info);
}

bool swapchainNextImageInfoStruct(void)
{
    FieldInfo fields[] = {
        { "timeout", {0, 8}, FIELD(PalSwapchainNextImageInfo, timeout) },
        { "signalSemaphore", {8, 8}, FIELD(PalSwapchainNextImageInfo, signalSemaphore) },
        { "fence", {16, 8}, FIELD(PalSwapchainNextImageInfo, fence) }
    };

    StructInfo info = {0};
    info.name = "PalSwapchainNextImageInfo";
    info.fields = fields;
    info.fieldCount = ARRAY_SIZE(fields);
    info.expected.align = 8;
    info.expected.size = 24;
    info.expected.padding = 0;
    info.actual = STRUCT(PalSwapchainNextImageInfo);

    return dumpStruct(&info);
}

bool swapchainCreateInfoStruct(void)
{
    FieldInfo fields[] = {
        { "clipped", {0, 4}, FIELD(PalSwapchainCreateInfo, clipped) },
        { "width", {4, 4}, FIELD(PalSwapchainCreateInfo, width) },
        { "height", {8, 4}, FIELD(PalSwapchainCreateInfo, height) },
        { "imageCount", {12, 4}, FIELD(PalSwapchainCreateInfo, imageCount) },
        { "imageArrayLayerCount", {16, 4}, FIELD(PalSwapchainCreateInfo, imageArrayLayerCount) },
        { "presentMode", {20, 4}, FIELD(PalSwapchainCreateInfo, presentMode) },
        { "compositeAlpha", {24, 4}, FIELD(PalSwapchainCreateInfo, compositeAlpha) },
        { "format", {28, 4}, FIELD(PalSwapchainCreateInfo, format) }
    };

    StructInfo info = {0};
    info.name = "PalSwapchainCreateInfo";
    info.fields = fields;
    info.fieldCount = ARRAY_SIZE(fields);
    info.expected.align = 4;
    info.expected.size = 32;
    info.expected.padding = 0;
    info.actual = STRUCT(PalSwapchainCreateInfo);

    return dumpStruct(&info);
}

bool imageInfoStruct(void)
{
    FieldInfo fields[] = {
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

    StructInfo info = {0};
    info.name = "PalImageInfo";
    info.fields = fields;
    info.fieldCount = ARRAY_SIZE(fields);
    info.expected.align = 4;
    info.expected.size = 40;
    info.expected.padding = 0;
    info.actual = STRUCT(PalImageInfo);

    return dumpStruct(&info);
}

bool imageSubresourceRangeStruct(void)
{
    FieldInfo fields[] = {
        { "aspect", {0, 4}, FIELD(PalImageSubresourceRange, aspect) },
        { "startMipLevel", {4, 4}, FIELD(PalImageSubresourceRange, startMipLevel) },
        { "mipLevelCount", {8, 4}, FIELD(PalImageSubresourceRange, mipLevelCount) },
        { "startArrayLayer", {12, 4}, FIELD(PalImageSubresourceRange, startArrayLayer) },
        { "layerArrayCount", {16, 4}, FIELD(PalImageSubresourceRange, layerArrayCount) }
    };

    StructInfo info = {0};
    info.name = "PalImageSubresourceRange";
    info.fields = fields;
    info.fieldCount = ARRAY_SIZE(fields);
    info.expected.align = 4;
    info.expected.size = 20;
    info.expected.padding = 0;
    info.actual = STRUCT(PalImageSubresourceRange);

    return dumpStruct(&info);
}

bool bufferImageCopyInfoStruct(void)
{
    FieldInfo fields[] = {
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

    StructInfo info = {0};
    info.name = "PalBufferImageCopyInfo";
    info.fields = fields;
    info.fieldCount = ARRAY_SIZE(fields);
    info.expected.align = 8;
    info.expected.size = 56;
    info.expected.padding = 0;
    info.actual = STRUCT(PalBufferImageCopyInfo);

    return dumpStruct(&info);
}

bool imageCopyInfoStruct(void)
{
    FieldInfo fields[] = {
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

    StructInfo info = {0};
    info.name = "PalImageCopyInfo";
    info.fields = fields;
    info.fieldCount = ARRAY_SIZE(fields);
    info.expected.align = 4;
    info.expected.size = 60;
    info.expected.padding = 0;
    info.actual = STRUCT(PalImageCopyInfo);

    return dumpStruct(&info);
}

bool imageCreateInfoStruct(void)
{
    FieldInfo fields[] = {
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

    StructInfo info = {0};
    info.name = "PalImageCreateInfo";
    info.fields = fields;
    info.fieldCount = ARRAY_SIZE(fields);
    info.expected.align = 4;
    info.expected.size = 40;
    info.expected.padding = 0;
    info.actual = STRUCT(PalImageCreateInfo);

    return dumpStruct(&info);
}

bool imageViewCreateInfoStruct(void)
{
    FieldInfo fields[] = {
        { "format", {0, 4}, FIELD(PalImageViewCreateInfo, format) },
        { "type", {4, 4}, FIELD(PalImageViewCreateInfo, type) },
        { "subresourceRange", {8, 20}, FIELD(PalImageViewCreateInfo, subresourceRange) }
    };

    StructInfo info = {0};
    info.name = "PalImageViewCreateInfo";
    info.fields = fields;
    info.fieldCount = ARRAY_SIZE(fields);
    info.expected.align = 4;
    info.expected.size = 28;
    info.expected.padding = 0;
    info.actual = STRUCT(PalImageViewCreateInfo);

    return dumpStruct(&info);
}

bool samplerCreateInfoStruct(void)
{
    FieldInfo fields[] = {
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

    StructInfo info = {0};
    info.name = "PalSamplerCreateInfo";
    info.fields = fields;
    info.fieldCount = ARRAY_SIZE(fields);
    info.expected.align = 4;
    info.expected.size = 56;
    info.expected.padding = 0;
    info.actual = STRUCT(PalSamplerCreateInfo);

    return dumpStruct(&info);
}

bool imageStagingRequirementsStruct(void)
{
    FieldInfo fields[] = {
        { "bufferSize", {0, 8}, FIELD(PalImageStagingRequirements, bufferSize) },
        { "bufferRowLength", {8, 4}, FIELD(PalImageStagingRequirements, bufferRowLength) },
        { "bufferImageHeight", {12, 4}, FIELD(PalImageStagingRequirements, bufferImageHeight) }
    };

    StructInfo info = {0};
    info.name = "PalImageStagingRequirements";
    info.fields = fields;
    info.fieldCount = ARRAY_SIZE(fields);
    info.expected.align = 8;
    info.expected.size = 16;
    info.expected.padding = 0;
    info.actual = STRUCT(PalImageStagingRequirements);

    return dumpStruct(&info);
}

bool bufferCopyInfoStruct(void)
{
    FieldInfo fields[] = {
        { "size", {0, 8}, FIELD(PalBufferCopyInfo, size) },
        { "dstOffset", {8, 8}, FIELD(PalBufferCopyInfo, dstOffset) },
        { "srcOffset", {16, 8}, FIELD(PalBufferCopyInfo, srcOffset) }
    };

    StructInfo info = {0};
    info.name = "PalBufferCopyInfo";
    info.fields = fields;
    info.fieldCount = ARRAY_SIZE(fields);
    info.expected.align = 8;
    info.expected.size = 24;
    info.expected.padding = 0;
    info.actual = STRUCT(PalBufferCopyInfo);

    return dumpStruct(&info);
}

bool bufferCreateInfoStruct(void)
{
    FieldInfo fields[] = {
        { "size", {0, 8}, FIELD(PalBufferCreateInfo, size) },
        { "usages", {8, 4}, FIELD(PalBufferCreateInfo, usages) },
        { "memoryUsage", {12, 4}, FIELD(PalBufferCreateInfo, memoryUsage) }
    };

    StructInfo info = {0};
    info.name = "PalBufferCreateInfo";
    info.fields = fields;
    info.fieldCount = ARRAY_SIZE(fields);
    info.expected.align = 8;
    info.expected.size = 16;
    info.expected.padding = 0;
    info.actual = STRUCT(PalBufferCreateInfo);

    return dumpStruct(&info);
}

bool drawIndirectDataStruct(void)
{
    FieldInfo fields[] = {
        { "vertexCount", {0, 4}, FIELD(PalDrawIndirectData, vertexCount) },
        { "instanceCount", {4, 4}, FIELD(PalDrawIndirectData, instanceCount) },
        { "firstVertex", {8, 4}, FIELD(PalDrawIndirectData, firstVertex) },
        { "firstInstance", {12, 4}, FIELD(PalDrawIndirectData, firstInstance) }
    };

    StructInfo info = {0};
    info.name = "PalDrawIndirectData";
    info.fields = fields;
    info.fieldCount = ARRAY_SIZE(fields);
    info.expected.align = 4;
    info.expected.size = 16;
    info.expected.padding = 0;
    info.actual = STRUCT(PalDrawIndirectData);

    return dumpStruct(&info);
}

bool drawIndexedIndirectDataStruct(void)
{
    FieldInfo fields[] = {
        { "indexCount", {0, 4}, FIELD(PalDrawIndexedIndirectData, indexCount) },
        { "instanceCount", {4, 4}, FIELD(PalDrawIndexedIndirectData, instanceCount) },
        { "firstIndex", {8, 4}, FIELD(PalDrawIndexedIndirectData, firstIndex) },
        { "vertexOffset", {12, 4}, FIELD(PalDrawIndexedIndirectData, vertexOffset) },
        { "firstInstance", {16, 4}, FIELD(PalDrawIndexedIndirectData, firstInstance) }
    };

    StructInfo info = {0};
    info.name = "PalDrawIndexedIndirectData";
    info.fields = fields;
    info.fieldCount = ARRAY_SIZE(fields);
    info.expected.align = 4;
    info.expected.size = 20;
    info.expected.padding = 0;
    info.actual = STRUCT(PalDrawIndexedIndirectData);

    return dumpStruct(&info);
}

bool dispatchIndirectDataStruct(void)
{
    FieldInfo fields[] = {
        { "groupCountXOrWidth", {0, 4}, FIELD(PalDispatchIndirectData, groupCountXOrWidth) },
        { "groupCountXOrHeight", {4, 4}, FIELD(PalDispatchIndirectData, groupCountXOrHeight) },
        { "groupCountXOrDepth", {8, 4}, FIELD(PalDispatchIndirectData, groupCountXOrDepth) }
    };

    StructInfo info = {0};
    info.name = "PalDispatchIndirectData";
    info.fields = fields;
    info.fieldCount = ARRAY_SIZE(fields);
    info.expected.align = 4;
    info.expected.size = 12;
    info.expected.padding = 0;
    info.actual = STRUCT(PalDispatchIndirectData);

    return dumpStruct(&info);
}

bool accelerationStructureInstanceStruct(void)
{
    FieldInfo fields[] = {
        { "blas", {0, 8}, FIELD(PalAccelerationStructureInstance, blas) },
        { "flags", {8, 4}, FIELD(PalAccelerationStructureInstance, flags) },
        { "mask", {12, 4}, FIELD(PalAccelerationStructureInstance, mask) },
        { "instanceId", {16, 4}, FIELD(PalAccelerationStructureInstance, instanceId) },
        { "hitGroupOffset", {20, 4}, FIELD(PalAccelerationStructureInstance, hitGroupOffset) },
        { "transform", {24, 48}, FIELD(PalAccelerationStructureInstance, transform) }
    };

    StructInfo info = {0};
    info.name = "PalAccelerationStructureInstance";
    info.fields = fields;
    info.fieldCount = ARRAY_SIZE(fields);
    info.expected.align = 8;
    info.expected.size = 72;
    info.expected.padding = 0;
    info.actual = STRUCT(PalAccelerationStructureInstance);

    return dumpStruct(&info);
}

bool accelerationStructureBuildSizeStruct(void)
{
    FieldInfo fields[] = {
        { "accelerationStructureSize", {0, 8}, FIELD(PalAccelerationStructureBuildSize, accelerationStructureSize) },
        { "scratchBufferSize", {8, 8}, FIELD(PalAccelerationStructureBuildSize, scratchBufferSize) },
        { "updateScratchBufferSize", {16, 8}, FIELD(PalAccelerationStructureBuildSize, updateScratchBufferSize) }
    };

    StructInfo info = {0};
    info.name = "PalAccelerationStructureBuildSize";
    info.fields = fields;
    info.fieldCount = ARRAY_SIZE(fields);
    info.expected.align = 8;
    info.expected.size = 24;
    info.expected.padding = 0;
    info.actual = STRUCT(PalAccelerationStructureBuildSize);

    return dumpStruct(&info);
}

bool geometryDataTriangleStruct(void)
{
    FieldInfo fields[] = {
        { "vertexBufferAddress", {0, 8}, FIELD(PalGeometryDataTriangle, vertexBufferAddress) },
        { "indexBufferAddress", {8, 8}, FIELD(PalGeometryDataTriangle, indexBufferAddress) },
        { "transformBufferAddress", {16, 8}, FIELD(PalGeometryDataTriangle, transformBufferAddress) },
        { "vertexType", {24, 4}, FIELD(PalGeometryDataTriangle, vertexType) },
        { "indexType", {28, 4}, FIELD(PalGeometryDataTriangle, indexType) },
        { "vertexCount", {32, 4}, FIELD(PalGeometryDataTriangle, vertexCount) },
        { "vertexStride", {36, 4}, FIELD(PalGeometryDataTriangle, vertexStride) }
    };

    StructInfo info = {0};
    info.name = "PalGeometryDataTriangle";
    info.fields = fields;
    info.fieldCount = ARRAY_SIZE(fields);
    info.expected.align = 8;
    info.expected.size = 40;
    info.expected.padding = 0;
    info.actual = STRUCT(PalGeometryDataTriangle);

    return dumpStruct(&info);
}

bool geometryDataAABBSStruct(void)
{
    FieldInfo fields[] = {
        { "bufferAddress", {0, 8}, FIELD(PalGeometryDataAABBS, bufferAddress) },
        { "stride", {8, 8}, FIELD(PalGeometryDataAABBS, stride) }
    };

    StructInfo info = {0};
    info.name = "PalGeometryDataAABBS";
    info.fields = fields;
    info.fieldCount = ARRAY_SIZE(fields);
    info.expected.align = 8;
    info.expected.size = 16;
    info.expected.padding = 0;
    info.actual = STRUCT(PalGeometryDataAABBS);

    return dumpStruct(&info);
}

bool geometryStruct(void)
{
    FieldInfo fields[] = {
        { "data", {0, 8}, FIELD(PalGeometry, data) },
        { "primitiveCount", {8, 8}, FIELD(PalGeometry, primitiveCount) },
        { "flags", {16, 4}, FIELD(PalGeometry, flags) },
        { "type", {20, 4}, FIELD(PalGeometry, type) },
    };

    StructInfo info = {0};
    info.name = "PalGeometry";
    info.fields = fields;
    info.fieldCount = ARRAY_SIZE(fields);
    info.expected.align = 8;
    info.expected.size = 24;
    info.expected.padding = 0;
    info.actual = STRUCT(PalGeometry);

    return dumpStruct(&info);
}

bool accelerationStructureBuildInfoStruct(void)
{
    FieldInfo fields[] = {
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

    StructInfo info = {0};
    info.name = "PalAccelerationStructureBuildInfo";
    info.fields = fields;
    info.fieldCount = ARRAY_SIZE(fields);
    info.expected.align = 8;
    info.expected.size = 56;
    info.expected.padding = 0;
    info.actual = STRUCT(PalAccelerationStructureBuildInfo);

    return dumpStruct(&info);
}

bool accelerationStructureCreateInfoStruct(void)
{
    FieldInfo fields[] = {
        { "buffer", {0, 8}, FIELD(PalAccelerationStructureCreateInfo, buffer) },
        { "offset", {8, 8}, FIELD(PalAccelerationStructureCreateInfo, offset) },
        { "size", {16, 8}, FIELD(PalAccelerationStructureCreateInfo, size) },
        { "type", {24, 4}, FIELD(PalAccelerationStructureCreateInfo, type) },
        { "reserved", {28, 4}, FIELD(PalAccelerationStructureCreateInfo, reserved) }
    };

    StructInfo info = {0};
    info.name = "PalAccelerationStructureCreateInfo";
    info.fields = fields;
    info.fieldCount = ARRAY_SIZE(fields);
    info.expected.align = 8;
    info.expected.size = 32;
    info.expected.padding = 0;
    info.actual = STRUCT(PalAccelerationStructureCreateInfo);

    return dumpStruct(&info);
}

bool vertexAttributeStruct(void)
{
    FieldInfo fields[] = {
        { "semanticID", {0, 4}, FIELD(PalVertexAttribute, semanticID) },
        { "type", {4, 4}, FIELD(PalVertexAttribute, type) }
    };

    StructInfo info = {0};
    info.name = "PalVertexAttribute";
    info.fields = fields;
    info.fieldCount = ARRAY_SIZE(fields);
    info.expected.align = 4;
    info.expected.size = 8;
    info.expected.padding = 0;
    info.actual = STRUCT(PalVertexAttribute);

    return dumpStruct(&info);
}

bool vertexLayoutStruct(void)
{
    FieldInfo fields[] = {
        { "attributes", {0, 8}, FIELD(PalVertexLayout, attributes) },
        { "attributeCount", {8, 4}, FIELD(PalVertexLayout, attributeCount) },
        { "type", {12, 4}, FIELD(PalVertexLayout, type) },
        { "binding", {16, 4}, FIELD(PalVertexLayout, binding) },
        { "reserved", {20, 4}, FIELD(PalVertexLayout, reserved) }
    };

    StructInfo info = {0};
    info.name = "PalVertexLayout";
    info.fields = fields;
    info.fieldCount = ARRAY_SIZE(fields);
    info.expected.align = 8;
    info.expected.size = 24;
    info.expected.padding = 0;
    info.actual = STRUCT(PalVertexLayout);

    return dumpStruct(&info);
}

bool rasterizerStateStruct(void)
{
    FieldInfo fields[] = {
        { "enableDepthClamp", {0, 4}, FIELD(PalRasterizerState, enableDepthClamp) },
        { "enableDepthBias", {4, 4}, FIELD(PalRasterizerState, enableDepthBias) },
        { "depthBiasConstant", {8, 4}, FIELD(PalRasterizerState, depthBiasConstant) },
        { "depthBiasSlope", {12, 4}, FIELD(PalRasterizerState, depthBiasSlope) },
        { "depthBiasClamp", {16, 4}, FIELD(PalRasterizerState, depthBiasClamp) },
        { "polygonMode", {20, 4}, FIELD(PalRasterizerState, polygonMode) },
        { "cullMode", {24, 4}, FIELD(PalRasterizerState, cullMode) },
        { "frontFace", {28, 4}, FIELD(PalRasterizerState, frontFace) }
    };

    StructInfo info = {0};
    info.name = "PalRasterizerState";
    info.fields = fields;
    info.fieldCount = ARRAY_SIZE(fields);
    info.expected.align = 4;
    info.expected.size = 32;
    info.expected.padding = 0;
    info.actual = STRUCT(PalRasterizerState);

    return dumpStruct(&info);
}

bool multisampleStateStruct(void)
{
    FieldInfo fields[] = {
        { "sampleMask", {0, 8}, FIELD(PalMultisampleState, sampleMask) },
        { "enableSampleShading", {8, 4}, FIELD(PalMultisampleState, enableSampleShading) },
        { "enableAlphaToCoverage", {12, 4}, FIELD(PalMultisampleState, enableAlphaToCoverage) },
        { "sampleCount", {16, 4}, FIELD(PalMultisampleState, sampleCount) },
        { "minSampleShading", {20, 4}, FIELD(PalMultisampleState, minSampleShading) }
    };

    StructInfo info = {0};
    info.name = "PalMultisampleState";
    info.fields = fields;
    info.fieldCount = ARRAY_SIZE(fields);
    info.expected.align = 8;
    info.expected.size = 24;
    info.expected.padding = 0;
    info.actual = STRUCT(PalMultisampleState);

    return dumpStruct(&info);
}

bool stencilOpStateStruct(void)
{
    FieldInfo fields[] = {
        { "failOp", {0, 4}, FIELD(PalStencilOpState, failOp) },
        { "passOp", {4, 4}, FIELD(PalStencilOpState, passOp) },
        { "depthFailOp", {8, 4}, FIELD(PalStencilOpState, depthFailOp) },
        { "compareOp", {12, 4}, FIELD(PalStencilOpState, compareOp) }
    };

    StructInfo info = {0};
    info.name = "PalStencilOpState";
    info.fields = fields;
    info.fieldCount = ARRAY_SIZE(fields);
    info.expected.align = 4;
    info.expected.size = 16;
    info.expected.padding = 0;
    info.actual = STRUCT(PalStencilOpState);

    return dumpStruct(&info);
}

bool depthStencilStateStruct(void)
{
    FieldInfo fields[] = {
        { "enableDepthTest", {0, 4}, FIELD(PalDepthStencilState, enableDepthTest) },
        { "enableDepthWrite", {4, 4}, FIELD(PalDepthStencilState, enableDepthWrite) },
        { "enableStencilTest", {8, 4}, FIELD(PalDepthStencilState, enableStencilTest) },
        { "compareOp", {12, 4}, FIELD(PalDepthStencilState, compareOp) },
        { "frontStencilOpState", {16, 16}, FIELD(PalDepthStencilState, frontStencilOpState) },
        { "backStencilOpState", {32, 16}, FIELD(PalDepthStencilState, backStencilOpState) },
    };

    StructInfo info = {0};
    info.name = "PalDepthStencilState";
    info.fields = fields;
    info.fieldCount = ARRAY_SIZE(fields);
    info.expected.align = 4;
    info.expected.size = 48;
    info.expected.padding = 0;
    info.actual = STRUCT(PalDepthStencilState);

    return dumpStruct(&info);
}

bool colorBlendAttachmentStruct(void)
{
    FieldInfo fields[] = {
        { "enableBlend", {0, 4}, FIELD(PalColorBlendAttachment, enableBlend) },
        { "colorWriteMask", {4, 4}, FIELD(PalColorBlendAttachment, colorWriteMask) },
        { "dstColorBlendFactor", {8, 4}, FIELD(PalColorBlendAttachment, dstColorBlendFactor) },
        { "srcColorBlendFactor", {12, 4}, FIELD(PalColorBlendAttachment, srcColorBlendFactor) },
        { "colorBlendOp", {16, 4}, FIELD(PalColorBlendAttachment, colorBlendOp) },
        { "dstAlphaBlendFactor", {20, 4}, FIELD(PalColorBlendAttachment, dstAlphaBlendFactor) },
        { "srcAlphaBlendFactor", {24, 4}, FIELD(PalColorBlendAttachment, srcAlphaBlendFactor) },
        { "alphaBlendOp", {28, 4}, FIELD(PalColorBlendAttachment, alphaBlendOp) }
    };

    StructInfo info = {0};
    info.name = "PalColorBlendAttachment";
    info.fields = fields;
    info.fieldCount = ARRAY_SIZE(fields);
    info.expected.align = 4;
    info.expected.size = 32;
    info.expected.padding = 0;
    info.actual = STRUCT(PalColorBlendAttachment);

    return dumpStruct(&info);
}

bool fragmentShadingRateStateStruct(void)
{
    FieldInfo fields[] = {
        { "rate", {0, 4}, FIELD(PalFragmentShadingRateState, rate) },
        { "combinerOps", {4, 8}, FIELD(PalFragmentShadingRateState, combinerOps) }
    };

    StructInfo info = {0};
    info.name = "PalFragmentShadingRateState";
    info.fields = fields;
    info.fieldCount = ARRAY_SIZE(fields);
    info.expected.align = 4;
    info.expected.size = 12;
    info.expected.padding = 0;
    info.actual = STRUCT(PalFragmentShadingRateState);

    return dumpStruct(&info);
}

bool pushConstantInfoStruct(void)
{
    FieldInfo fields[] = {
        { "offset", {0, 4}, FIELD(PalPushConstantInfo, offset) },
        { "size", {4, 4}, FIELD(PalPushConstantInfo, size) }
    };

    StructInfo info = {0};
    info.name = "PalPushConstantInfo";
    info.fields = fields;
    info.fieldCount = ARRAY_SIZE(fields);
    info.expected.align = 4;
    info.expected.size = 8;
    info.expected.padding = 0;
    info.actual = STRUCT(PalPushConstantInfo);

    return dumpStruct(&info);
}

bool pipelineLayoutCreateInfoStruct(void)
{
    FieldInfo fields[] = {
        { "descriptorSetLayouts", {0, 8}, FIELD(PalPipelineLayoutCreateInfo, descriptorSetLayouts) },
        { "pushConstantInfo", {8, 8}, FIELD(PalPipelineLayoutCreateInfo, pushConstantInfo) },
        { "descriptorSetLayoutCount", {16, 4}, FIELD(PalPipelineLayoutCreateInfo, descriptorSetLayoutCount) },
        { "usePushConstant", {20, 4}, FIELD(PalPipelineLayoutCreateInfo, usePushConstant) }
    };

    StructInfo info = {0};
    info.name = "PalPipelineLayoutCreateInfo";
    info.fields = fields;
    info.fieldCount = ARRAY_SIZE(fields);
    info.expected.align = 8;
    info.expected.size = 24;
    info.expected.padding = 0;
    info.actual = STRUCT(PalPipelineLayoutCreateInfo);

    return dumpStruct(&info);
}

bool graphicsPipelineCreateInfoStruct(void)
{
    FieldInfo fields[] = {
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

    StructInfo info = {0};
    info.name = "PalGraphicsPipelineCreateInfo";
    info.fields = fields;
    info.fieldCount = ARRAY_SIZE(fields);
    info.expected.align = 8;
    info.expected.size = 96;
    info.expected.padding = 0;
    info.actual = STRUCT(PalGraphicsPipelineCreateInfo);

    return dumpStruct(&info);
}

bool computePipelineCreateInfoStruct(void)
{
    FieldInfo fields[] = {
        { "pipelineLayout", {0, 8}, FIELD(PalComputePipelineCreateInfo, pipelineLayout) },
        { "computeShader", {8, 8}, FIELD(PalComputePipelineCreateInfo, computeShader) }
    };

    StructInfo info = {0};
    info.name = "PalComputePipelineCreateInfo";
    info.fields = fields;
    info.fieldCount = ARRAY_SIZE(fields);
    info.expected.align = 8;
    info.expected.size = 16;
    info.expected.padding = 0;
    info.actual = STRUCT(PalComputePipelineCreateInfo);

    return dumpStruct(&info);
}

bool rayTracingShaderGroupCreateInfoStruct(void)
{
    FieldInfo fields[] = {
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

    StructInfo info = {0};
    info.name = "PalRayTracingShaderGroupCreateInfo";
    info.fields = fields;
    info.fieldCount = ARRAY_SIZE(fields);
    info.expected.align = 4;
    info.expected.size = 40;
    info.expected.padding = 0;
    info.actual = STRUCT(PalRayTracingShaderGroupCreateInfo);

    return dumpStruct(&info);
}

bool rayTracingPipelineCreateInfoStruct(void)
{
    FieldInfo fields[] = {
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

    StructInfo info = {0};
    info.name = "PalRayTracingPipelineCreateInfo";
    info.fields = fields;
    info.fieldCount = ARRAY_SIZE(fields);
    info.expected.align = 8;
    info.expected.size = 48;
    info.expected.padding = 0;
    info.actual = STRUCT(PalRayTracingPipelineCreateInfo);

    return dumpStruct(&info);
}

bool descriptorSetLayoutBindingStruct(void)
{
    FieldInfo fields[] = {
        { "descriptorCount", {0, 4}, FIELD(PalDescriptorSetLayoutBinding, descriptorCount) },
        { "descriptorType", {4, 4}, FIELD(PalDescriptorSetLayoutBinding, descriptorType) }
    };

    StructInfo info = {0};
    info.name = "PalDescriptorSetLayoutBinding";
    info.fields = fields;
    info.fieldCount = ARRAY_SIZE(fields);
    info.expected.align = 4;
    info.expected.size = 8;
    info.expected.padding = 0;
    info.actual = STRUCT(PalDescriptorSetLayoutBinding);

    return dumpStruct(&info);
}

bool descriptorPoolBindingSizeStruct(void)
{
    FieldInfo fields[] = {
        { "bindingCount", {0, 4}, FIELD(PalDescriptorPoolBindingSize, bindingCount) },
        { "descriptorType", {4, 4}, FIELD(PalDescriptorPoolBindingSize, descriptorType) }
    };

    StructInfo info = {0};
    info.name = "PalDescriptorPoolBindingSize";
    info.fields = fields;
    info.fieldCount = ARRAY_SIZE(fields);
    info.expected.align = 4;
    info.expected.size = 8;
    info.expected.padding = 0;
    info.actual = STRUCT(PalDescriptorPoolBindingSize);

    return dumpStruct(&info);
}

bool descriptorBufferInfoStruct(void)
{
    FieldInfo fields[] = {
        { "buffer", {0, 8}, FIELD(PalDescriptorBufferInfo, buffer) },
        { "offset", {8, 8}, FIELD(PalDescriptorBufferInfo, offset) },
        { "size", {16, 8}, FIELD(PalDescriptorBufferInfo, size) },
        { "stride", {24, 8}, FIELD(PalDescriptorBufferInfo, stride) }
    };

    StructInfo info = {0};
    info.name = "PalDescriptorBufferInfo";
    info.fields = fields;
    info.fieldCount = ARRAY_SIZE(fields);
    info.expected.align = 8;
    info.expected.size = 32;
    info.expected.padding = 0;
    info.actual = STRUCT(PalDescriptorBufferInfo);

    return dumpStruct(&info);
}

bool descriptorImageViewInfoStruct(void)
{
    FieldInfo fields[] = {
        { "imageView", {0, 8}, FIELD(PalDescriptorImageViewInfo, imageView) }
    };

    StructInfo info = {0};
    info.name = "PalDescriptorImageViewInfo";
    info.fields = fields;
    info.fieldCount = ARRAY_SIZE(fields);
    info.expected.align = 8;
    info.expected.size = 8;
    info.expected.padding = 0;
    info.actual = STRUCT(PalDescriptorImageViewInfo);

    return dumpStruct(&info);
}

bool descriptorSamplerInfoStruct(void)
{
    FieldInfo fields[] = {
        { "sampler", {0, 8}, FIELD(PalDescriptorSamplerInfo, sampler) }
    };

    StructInfo info = {0};
    info.name = "PalDescriptorSamplerInfo";
    info.fields = fields;
    info.fieldCount = ARRAY_SIZE(fields);
    info.expected.align = 8;
    info.expected.size = 8;
    info.expected.padding = 0;
    info.actual = STRUCT(PalDescriptorSamplerInfo);

    return dumpStruct(&info);
}

bool descriptorTLASInfoStruct(void)
{
    FieldInfo fields[] = {
        { "sampler", {0, 8}, FIELD(PalDescriptorSamplerInfo, sampler) }
    };

    StructInfo info = {0};
    info.name = "PalDescriptorTLASInfo";
    info.fields = fields;
    info.fieldCount = ARRAY_SIZE(fields);
    info.expected.align = 8;
    info.expected.size = 8;
    info.expected.padding = 0;
    info.actual = STRUCT(PalDescriptorTLASInfo);

    return dumpStruct(&info);
}

bool descriptorSetWriteInfoStruct(void)
{
    FieldInfo fields[] = {
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

    StructInfo info = {0};
    info.name = "PalDescriptorSetWriteInfo";
    info.fields = fields;
    info.fieldCount = ARRAY_SIZE(fields);
    info.expected.align = 8;
    info.expected.size = 56;
    info.expected.padding = 0;
    info.actual = STRUCT(PalDescriptorSetWriteInfo);

    return dumpStruct(&info);
}

bool descriptorSetLayoutCreateInfoStruct(void)
{
    FieldInfo fields[] = {
        { "bindings", {0, 8}, FIELD(PalDescriptorSetLayoutCreateInfo, bindings) },
        { "flags", {8, 4}, FIELD(PalDescriptorSetLayoutCreateInfo, flags) },
        { "bindingCount", {12, 4}, FIELD(PalDescriptorSetLayoutCreateInfo, bindingCount) }
    };

    StructInfo info = {0};
    info.name = "PalDescriptorSetLayoutCreateInfo";
    info.fields = fields;
    info.fieldCount = ARRAY_SIZE(fields);
    info.expected.align = 8;
    info.expected.size = 16;
    info.expected.padding = 0;
    info.actual = STRUCT(PalDescriptorSetLayoutCreateInfo);

    return dumpStruct(&info);
}

bool descriptorPoolCreateInfoStruct(void)
{
    FieldInfo fields[] = {
        { "bindingSizes", {0, 8}, FIELD(PalDescriptorPoolCreateInfo, bindingSizes) },
        { "bindingSizeCount", {8, 4}, FIELD(PalDescriptorPoolCreateInfo, bindingSizeCount) },
        { "maxDescriptorSets", {12, 4}, FIELD(PalDescriptorPoolCreateInfo, maxDescriptorSets) },
        { "flags", {16, 4}, FIELD(PalDescriptorPoolCreateInfo, flags) },
        { "reserved", {20, 4}, FIELD(PalDescriptorPoolCreateInfo, reserved) }
    };

    StructInfo info = {0};
    info.name = "PalDescriptorPoolCreateInfo";
    info.fields = fields;
    info.fieldCount = ARRAY_SIZE(fields);
    info.expected.align = 8;
    info.expected.size = 24;
    info.expected.padding = 0;
    info.actual = STRUCT(PalDescriptorPoolCreateInfo);

    return dumpStruct(&info);
}

bool shaderBindingTableRecordInfoStruct(void)
{
    FieldInfo fields[] = {
        { "localData", {0, 8}, FIELD(PalShaderBindingTableRecordInfo, localData) },
        { "groupIndex", {8, 4}, FIELD(PalShaderBindingTableRecordInfo, groupIndex) },
        { "localDataSize", {12, 4}, FIELD(PalShaderBindingTableRecordInfo, localDataSize) }
    };

    StructInfo info = {0};
    info.name = "PalShaderBindingTableRecordInfo";
    info.fields = fields;
    info.fieldCount = ARRAY_SIZE(fields);
    info.expected.align = 8;
    info.expected.size = 16;
    info.expected.padding = 0;
    info.actual = STRUCT(PalShaderBindingTableRecordInfo);

    return dumpStruct(&info);
}

bool shaderBindingTableCreateInfoStruct(void)
{
    FieldInfo fields[] = {
        { "records", {0, 8}, FIELD(PalShaderBindingTableCreateInfo, records) },
        { "rayTracingPipeline", {8, 8}, FIELD(PalShaderBindingTableCreateInfo, rayTracingPipeline) },
        { "recordCount", {16, 4}, FIELD(PalShaderBindingTableCreateInfo, recordCount) },
        { "reserved", {20, 4}, FIELD(PalShaderBindingTableCreateInfo, reserved) }
    };

    StructInfo info = {0};
    info.name = "PalShaderBindingTableCreateInfo";
    info.fields = fields;
    info.fieldCount = ARRAY_SIZE(fields);
    info.expected.align = 8;
    info.expected.size = 24;
    info.expected.padding = 0;
    info.actual = STRUCT(PalShaderBindingTableCreateInfo);

    return dumpStruct(&info);
}

bool commandBufferSubmitInfoStruct(void)
{
    FieldInfo fields[] = {
        { "waitValue", {0, 8}, FIELD(PalCommandBufferSubmitInfo, waitValue) },
        { "signalValue", {8, 8}, FIELD(PalCommandBufferSubmitInfo, signalValue) },
        { "cmdBuffer", {16, 8}, FIELD(PalCommandBufferSubmitInfo, cmdBuffer) },
        { "waitSemaphore", {24, 8}, FIELD(PalCommandBufferSubmitInfo, waitSemaphore) },
        { "signalSemaphore", {32, 8}, FIELD(PalCommandBufferSubmitInfo, signalSemaphore) },
        { "fence", {40, 8}, FIELD(PalCommandBufferSubmitInfo, fence) },
        { "waitStages", {48, 4}, FIELD(PalCommandBufferSubmitInfo, waitStages) },
        { "signalStages", {52, 4}, FIELD(PalCommandBufferSubmitInfo, signalStages) }
    };

    StructInfo info = {0};
    info.name = "PalCommandBufferSubmitInfo";
    info.fields = fields;
    info.fieldCount = ARRAY_SIZE(fields);
    info.expected.align = 8;
    info.expected.size = 56;
    info.expected.padding = 0;
    info.actual = STRUCT(PalCommandBufferSubmitInfo);

    return dumpStruct(&info);
}

bool clearValueStruct(void)
{
    FieldInfo fields[] = {
        { "color", {0, 16}, FIELD(PalClearValue, color) },
        { "depth", {16, 4}, FIELD(PalClearValue, depth) },
        { "stencil", {20, 4}, FIELD(PalClearValue, stencil) }
    };

    StructInfo info = {0};
    info.name = "PalClearValue";
    info.fields = fields;
    info.fieldCount = ARRAY_SIZE(fields);
    info.expected.align = 4;
    info.expected.size = 24;
    info.expected.padding = 0;
    info.actual = STRUCT(PalClearValue);

    return dumpStruct(&info);
}

bool attachmentDescStruct(void)
{
    FieldInfo fields[] = {
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

    StructInfo info = {0};
    info.name = "PalAttachmentDesc";
    info.fields = fields;
    info.fieldCount = ARRAY_SIZE(fields);
    info.expected.align = 8;
    info.expected.size = 64;
    info.expected.padding = 0;
    info.actual = STRUCT(PalAttachmentDesc);

    return dumpStruct(&info);
}

bool viewportStruct(void)
{
    FieldInfo fields[] = {
        { "x", {0, 4}, FIELD(PalViewport, x) },
        { "y", {4, 4}, FIELD(PalViewport, y) },
        { "width", {8, 4}, FIELD(PalViewport, width) },
        { "height", {12, 4}, FIELD(PalViewport, height) },
        { "minDepth", {16, 4}, FIELD(PalViewport, minDepth) },
        { "maxDepth", {20, 4}, FIELD(PalViewport, maxDepth) }
    };

    StructInfo info = {0};
    info.name = "PalViewport";
    info.fields = fields;
    info.fieldCount = ARRAY_SIZE(fields);
    info.expected.align = 4;
    info.expected.size = 24;
    info.expected.padding = 0;
    info.actual = STRUCT(PalViewport);

    return dumpStruct(&info);
}

bool rect2DStruct(void)
{
    FieldInfo fields[] = {
        { "x", {0, 4}, FIELD(PalRect2D, x) },
        { "y", {4, 4}, FIELD(PalRect2D, y) },
        { "width", {8, 4}, FIELD(PalRect2D, width) },
        { "height", {12, 4}, FIELD(PalRect2D, height) },
    };

    StructInfo info = {0};
    info.name = "PalRect2D";
    info.fields = fields;
    info.fieldCount = ARRAY_SIZE(fields);
    info.expected.align = 4;
    info.expected.size = 16;
    info.expected.padding = 0;
    info.actual = STRUCT(PalRect2D);

    return dumpStruct(&info);
}

bool renderingInfoStruct(void)
{
    FieldInfo fields[] = {
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

    StructInfo info = {0};
    info.name = "PalRenderingInfo";
    info.fields = fields;
    info.fieldCount = ARRAY_SIZE(fields);
    info.expected.align = 8;
    info.expected.size = 64;
    info.expected.padding = 0;
    info.actual = STRUCT(PalRenderingInfo);

    return dumpStruct(&info);
}

bool renderingLayoutInfoStruct(void)
{
    FieldInfo fields[] = {
        { "colorAttachmentsFormat", {0, 8}, FIELD(PalRenderingLayoutInfo, colorAttachmentsFormat) },
        { "colorAttachentCount", {8, 4}, FIELD(PalRenderingLayoutInfo, colorAttachentCount) },
        { "viewCount", {12, 4}, FIELD(PalRenderingLayoutInfo, viewCount) },
        { "sampleCount", {16, 4}, FIELD(PalRenderingLayoutInfo, sampleCount) },
        { "flags", {20, 4}, FIELD(PalRenderingLayoutInfo, flags) },
        { "depthStencilAttachmentFormat", {24, 4}, FIELD(PalRenderingLayoutInfo, depthStencilAttachmentFormat) },
        { "fragmentShadingRateAttachmentFormat", {28, 4}, FIELD(PalRenderingLayoutInfo, fragmentShadingRateAttachmentFormat) }
    };

    StructInfo info = {0};
    info.name = "PalRenderingLayoutInfo";
    info.fields = fields;
    info.fieldCount = ARRAY_SIZE(fields);
    info.expected.align = 8;
    info.expected.size = 32;
    info.expected.padding = 0;
    info.actual = STRUCT(PalRenderingLayoutInfo);

    return dumpStruct(&info);
}

bool workGroupBuildDataStruct(void)
{
    FieldInfo fields[] = {
        { "workCount", {0, 12}, FIELD(PalWorkGroupBuildData, workCount) },
        { "workGroupSize", {12, 12}, FIELD(PalWorkGroupBuildData, workGroupSize) },
        { "workGroupCount", {24, 12}, FIELD(PalWorkGroupBuildData, workGroupCount) }
    };

    StructInfo info = {0};
    info.name = "PalWorkGroupBuildData";
    info.fields = fields;
    info.fieldCount = ARRAY_SIZE(fields);
    info.expected.align = 4;
    info.expected.size = 36;
    info.expected.padding = 0;
    info.actual = STRUCT(PalWorkGroupBuildData);

    return dumpStruct(&info);
}

bool workGroupInfoStruct(void)
{
    FieldInfo fields[] = {
        { "workGroupBase", {0, 12}, FIELD(PalWorkGroupInfo, workGroupBase) },
        { "workGroupCount", {12, 12}, FIELD(PalWorkGroupInfo, workGroupCount) }
    };

    StructInfo info = {0};
    info.name = "PalWorkGroupInfo";
    info.fields = fields;
    info.fieldCount = ARRAY_SIZE(fields);
    info.expected.align = 4;
    info.expected.size = 24;
    info.expected.padding = 0;
    info.actual = STRUCT(PalWorkGroupInfo);

    return dumpStruct(&info);
}

bool barrierInfoStruct(void)
{
    FieldInfo fields[] = {
        { "oldState", {0, 4}, FIELD(PalBarrierInfo, oldState) },
        { "newState", {4, 4}, FIELD(PalBarrierInfo, newState) },
        { "srcStages", {8, 4}, FIELD(PalBarrierInfo, srcStages) },
        { "dstStages", {12, 4}, FIELD(PalBarrierInfo, dstStages) }
    };

    StructInfo info = {0};
    info.name = "PalBarrierInfo";
    info.fields = fields;
    info.fieldCount = ARRAY_SIZE(fields);
    info.expected.align = 4;
    info.expected.size = 16;
    info.expected.padding = 0;
    info.actual = STRUCT(PalBarrierInfo);

    return dumpStruct(&info);
}

bool graphicsBackendVtable1Struct(void)
{
    FieldInfo fields[] = {
        { "enumerateAdapters", {0, 8}, FIELD(PalGraphicsBackendVtable1, enumerateAdapters) },
        { "getAdapterInfo", {8, 8}, FIELD(PalGraphicsBackendVtable1, getAdapterInfo) },
        { "getAdapterCapabilities", {16, 8}, FIELD(PalGraphicsBackendVtable1, getAdapterCapabilities) },
        { "getAdapterFeatures", {24, 8}, FIELD(PalGraphicsBackendVtable1, getAdapterFeatures) },
        { "getHighestSupportedShaderTarget", {32, 8}, FIELD(PalGraphicsBackendVtable1, getHighestSupportedShaderTarget) },
        { "createDevice", {40, 8}, FIELD(PalGraphicsBackendVtable1, createDevice) },
        { "destroyDevice", {48, 8}, FIELD(PalGraphicsBackendVtable1, destroyDevice) },
        { "getDeviceLostReason", {56, 8}, FIELD(PalGraphicsBackendVtable1, getDeviceLostReason) },
        { "allocateMemory", {64, 8}, FIELD(PalGraphicsBackendVtable1, allocateMemory) },
        { "freeMemory", {72, 8}, FIELD(PalGraphicsBackendVtable1, freeMemory) },
        { "querySamplerAnisotropyCapabilities", {80, 8}, FIELD(PalGraphicsBackendVtable1, querySamplerAnisotropyCapabilities) },
        { "queryMultiViewCapabilities", {88, 8}, FIELD(PalGraphicsBackendVtable1, queryMultiViewCapabilities) },
        { "queryMultiViewportCapabilities", {96, 8}, FIELD(PalGraphicsBackendVtable1, queryMultiViewportCapabilities) },
        { "queryDepthStencilCapabilities", {104, 8}, FIELD(PalGraphicsBackendVtable1, queryDepthStencilCapabilities) },
        { "queryFragmentShadingRateCapabilities", {112, 8}, FIELD(PalGraphicsBackendVtable1, queryFragmentShadingRateCapabilities) },
        { "queryMeshShaderCapabilities", {120, 8}, FIELD(PalGraphicsBackendVtable1, queryMeshShaderCapabilities) },
        { "queryRayTracingCapabilities", {128, 8}, FIELD(PalGraphicsBackendVtable1, queryRayTracingCapabilities) },
        { "queryDescriptorIndexingCapabilities", {136, 8}, FIELD(PalGraphicsBackendVtable1, queryDescriptorIndexingCapabilities) },
        { "createQueue", {144, 8}, FIELD(PalGraphicsBackendVtable1, createQueue) },
        { "destroyQueue", {152, 8}, FIELD(PalGraphicsBackendVtable1, destroyQueue) },
        { "canQueuePresent", {160, 8}, FIELD(PalGraphicsBackendVtable1, canQueuePresent) },
        { "waitQueue", {168, 8}, FIELD(PalGraphicsBackendVtable1, waitQueue) },
        { "enumerateFormats", {176, 8}, FIELD(PalGraphicsBackendVtable1, enumerateFormats) },
        { "isFormatSupported", {184, 8}, FIELD(PalGraphicsBackendVtable1, isFormatSupported) },
        { "queryFormatImageUsages", {192, 8}, FIELD(PalGraphicsBackendVtable1, queryFormatImageUsages) },
        { "queryFormatSampleCount", {200, 8}, FIELD(PalGraphicsBackendVtable1, queryFormatSampleCount) },
        { "createImage", {208, 8}, FIELD(PalGraphicsBackendVtable1, createImage) },
        { "destroyImage", {216, 8}, FIELD(PalGraphicsBackendVtable1, destroyImage) },
        { "getImageInfo", {224, 8}, FIELD(PalGraphicsBackendVtable1, getImageInfo) },
        { "getImageMemoryRequirements", {232, 8}, FIELD(PalGraphicsBackendVtable1, getImageMemoryRequirements) },
        { "bindImageMemory", {240, 8}, FIELD(PalGraphicsBackendVtable1, bindImageMemory) },
        { "createImageView", {248, 8}, FIELD(PalGraphicsBackendVtable1, createImageView) },
        { "destroyImageView", {256, 8}, FIELD(PalGraphicsBackendVtable1, destroyImageView) },
        { "createSampler", {264, 8}, FIELD(PalGraphicsBackendVtable1, createSampler) },
        { "destroySampler", {272, 8}, FIELD(PalGraphicsBackendVtable1, destroySampler) },
        { "createSurface", {280, 8}, FIELD(PalGraphicsBackendVtable1, createSurface) },
        { "destroySurface", {288, 8}, FIELD(PalGraphicsBackendVtable1, destroySurface) },
        { "getSurfaceCapabilities", {296, 8}, FIELD(PalGraphicsBackendVtable1, getSurfaceCapabilities) },
        { "createSwapchain", {304, 8}, FIELD(PalGraphicsBackendVtable1, createSwapchain) },
        { "destroySwapchain", {312, 8}, FIELD(PalGraphicsBackendVtable1, destroySwapchain) },
        { "getSwapchainImage", {320, 8}, FIELD(PalGraphicsBackendVtable1, getSwapchainImage) },
        { "getNextSwapchainImage", {328, 8}, FIELD(PalGraphicsBackendVtable1, getNextSwapchainImage) },
        { "presentSwapchain", {336, 8}, FIELD(PalGraphicsBackendVtable1, presentSwapchain) },
        { "resizeSwapchain", {344, 8}, FIELD(PalGraphicsBackendVtable1, resizeSwapchain) },
        { "createShader", {352, 8}, FIELD(PalGraphicsBackendVtable1, createShader) },
        { "destroyShader", {360, 8}, FIELD(PalGraphicsBackendVtable1, destroyShader) },
        { "createFence", {368, 8}, FIELD(PalGraphicsBackendVtable1, createFence) },
        { "destroyFence", {376, 8}, FIELD(PalGraphicsBackendVtable1, destroyFence) },
        { "waitFence", {384, 8}, FIELD(PalGraphicsBackendVtable1, waitFence) },
        { "resetFence", {392, 8}, FIELD(PalGraphicsBackendVtable1, resetFence) },
        { "isFenceSignaled", {400, 8}, FIELD(PalGraphicsBackendVtable1, isFenceSignaled) },
        { "createSemaphore", {408, 8}, FIELD(PalGraphicsBackendVtable1, createSemaphore) },
        { "destroySemaphore", {416, 8}, FIELD(PalGraphicsBackendVtable1, destroySemaphore) },
        { "waitSemaphore", {424, 8}, FIELD(PalGraphicsBackendVtable1, waitSemaphore) },
        { "signalSemaphore", {432, 8}, FIELD(PalGraphicsBackendVtable1, signalSemaphore) },
        { "getSemaphoreValue", {440, 8}, FIELD(PalGraphicsBackendVtable1, getSemaphoreValue) },
        { "createCommandPool", {448, 8}, FIELD(PalGraphicsBackendVtable1, createCommandPool) },
        { "destroyCommandPool", {456, 8}, FIELD(PalGraphicsBackendVtable1, destroyCommandPool) },
        { "allocateCommandBuffer", {464, 8}, FIELD(PalGraphicsBackendVtable1, allocateCommandBuffer) },
        { "freeCommandBuffer", {472, 8}, FIELD(PalGraphicsBackendVtable1, freeCommandBuffer) },
        { "resetCommandBuffer", {480, 8}, FIELD(PalGraphicsBackendVtable1, resetCommandBuffer) },
        { "submitCommandBuffer", {488, 8}, FIELD(PalGraphicsBackendVtable1, submitCommandBuffer) },
        { "cmdBegin", {496, 8}, FIELD(PalGraphicsBackendVtable1, cmdBegin) },
        { "cmdEnd", {504, 8}, FIELD(PalGraphicsBackendVtable1, cmdEnd) },
        { "cmdExecuteCommandBuffer", {512, 8}, FIELD(PalGraphicsBackendVtable1, cmdExecuteCommandBuffer) },
        { "cmdSetFragmentShadingRate", {520, 8}, FIELD(PalGraphicsBackendVtable1, cmdSetFragmentShadingRate) },
        { "cmdDrawMeshTasks", {528, 8}, FIELD(PalGraphicsBackendVtable1, cmdDrawMeshTasks) },
        { "cmdDrawMeshTasksIndirect", {536, 8}, FIELD(PalGraphicsBackendVtable1, cmdDrawMeshTasksIndirect) },
        { "cmdDrawMeshTasksIndirectCount", {544, 8}, FIELD(PalGraphicsBackendVtable1, cmdDrawMeshTasksIndirectCount) },
        { "cmdBuildAccelerationStructure", {552, 8}, FIELD(PalGraphicsBackendVtable1, cmdBuildAccelerationStructure) },
        { "cmdBeginRendering", {560, 8}, FIELD(PalGraphicsBackendVtable1, cmdBeginRendering) },
        { "cmdEndRendering", {568, 8}, FIELD(PalGraphicsBackendVtable1, cmdEndRendering) },
        { "cmdCopyBuffer", {576, 8}, FIELD(PalGraphicsBackendVtable1, cmdCopyBuffer) },
        { "cmdCopyBufferToImage", {584, 8}, FIELD(PalGraphicsBackendVtable1, cmdCopyBufferToImage) },
        { "cmdCopyImage", {592, 8}, FIELD(PalGraphicsBackendVtable1, cmdCopyImage) },
        { "cmdCopyImageToBuffer", {600, 8}, FIELD(PalGraphicsBackendVtable1, cmdCopyImageToBuffer) },
        { "cmdBindPipeline", {608, 8}, FIELD(PalGraphicsBackendVtable1, cmdBindPipeline) },
        { "cmdSetViewport", {616, 8}, FIELD(PalGraphicsBackendVtable1, cmdSetViewport) },
        { "cmdSetScissors", {624, 8}, FIELD(PalGraphicsBackendVtable1, cmdSetScissors) },
        { "cmdBindVertexBuffers", {632, 8}, FIELD(PalGraphicsBackendVtable1, cmdBindVertexBuffers) },
        { "cmdBindIndexBuffer", {640, 8}, FIELD(PalGraphicsBackendVtable1, cmdBindIndexBuffer) },
        { "cmdDraw", {648, 8}, FIELD(PalGraphicsBackendVtable1, cmdDraw) },
        { "cmdDrawIndirect", {656, 8}, FIELD(PalGraphicsBackendVtable1, cmdDrawIndirect) },
        { "cmdDrawIndirectCount", {664, 8}, FIELD(PalGraphicsBackendVtable1, cmdDrawIndirectCount) },
        { "cmdDrawIndexed", {672, 8}, FIELD(PalGraphicsBackendVtable1, cmdDrawIndexed) },
        { "cmdDrawIndexedIndirect", {680, 8}, FIELD(PalGraphicsBackendVtable1, cmdDrawIndexedIndirect) },
        { "cmdDrawIndexedIndirectCount", {688, 8}, FIELD(PalGraphicsBackendVtable1, cmdDrawIndexedIndirectCount) },
        { "cmdAccelerationStructureBarrier", {696, 8}, FIELD(PalGraphicsBackendVtable1, cmdAccelerationStructureBarrier) },
        { "cmdImageBarrier", {704, 8}, FIELD(PalGraphicsBackendVtable1, cmdImageBarrier) },
        { "cmdBufferBarrier", {712, 8}, FIELD(PalGraphicsBackendVtable1, cmdBufferBarrier) },
        { "cmdDispatch", {720, 8}, FIELD(PalGraphicsBackendVtable1, cmdDispatch) },
        { "cmdDispatchBase", {728, 8}, FIELD(PalGraphicsBackendVtable1, cmdDispatchBase) },
        { "cmdDispatchIndirect", {736, 8}, FIELD(PalGraphicsBackendVtable1, cmdDispatchIndirect) },
        { "cmdTraceRays", {744, 8}, FIELD(PalGraphicsBackendVtable1, cmdTraceRays) },
        { "cmdTraceRaysIndirect", {752, 8}, FIELD(PalGraphicsBackendVtable1, cmdTraceRaysIndirect) },
        { "cmdBindDescriptorSet", {760, 8}, FIELD(PalGraphicsBackendVtable1, cmdBindDescriptorSet) },
        { "cmdPushConstants", {768, 8}, FIELD(PalGraphicsBackendVtable1, cmdPushConstants) },
        { "cmdSetCullMode", {776, 8}, FIELD(PalGraphicsBackendVtable1, cmdSetCullMode) },
        { "cmdSetFrontFace", {784, 8}, FIELD(PalGraphicsBackendVtable1, cmdSetFrontFace) },
        { "cmdSetPrimitiveTopology", {792, 8}, FIELD(PalGraphicsBackendVtable1, cmdSetPrimitiveTopology) },
        { "cmdSetDepthTestEnable", {800, 8}, FIELD(PalGraphicsBackendVtable1, cmdSetDepthTestEnable) },
        { "cmdSetDepthWriteEnable", {808, 8}, FIELD(PalGraphicsBackendVtable1, cmdSetDepthWriteEnable) },
        { "cmdSetStencilOp", {816, 8}, FIELD(PalGraphicsBackendVtable1, cmdSetStencilOp) },
        { "createAccelerationstructure", {824, 8}, FIELD(PalGraphicsBackendVtable1, createAccelerationstructure) },
        { "destroyAccelerationstructure", {832, 8}, FIELD(PalGraphicsBackendVtable1, destroyAccelerationstructure) },
        { "getAccelerationStructureBuildSize", {840, 8}, FIELD(PalGraphicsBackendVtable1, getAccelerationStructureBuildSize) },
        { "createBuffer", {848, 8}, FIELD(PalGraphicsBackendVtable1, createBuffer) },
        { "destroyBuffer", {856, 8}, FIELD(PalGraphicsBackendVtable1, destroyBuffer) },
        { "getBufferMemoryRequirements", {864, 8}, FIELD(PalGraphicsBackendVtable1, getBufferMemoryRequirements) },
        { "computeInstanceStagingSize", {872, 8}, FIELD(PalGraphicsBackendVtable1, computeInstanceStagingSize) },
        { "computeImageStagingRequirements", {880, 8}, FIELD(PalGraphicsBackendVtable1, computeImageStagingRequirements) },
        { "writeInstanceStaging", {888, 8}, FIELD(PalGraphicsBackendVtable1, writeInstanceStaging) },
        { "writeImageStaging", {896, 8}, FIELD(PalGraphicsBackendVtable1, writeImageStaging) },
        { "bindBufferMemory", {904, 8}, FIELD(PalGraphicsBackendVtable1, bindBufferMemory) },
        { "mapBuffer", {912, 8}, FIELD(PalGraphicsBackendVtable1, mapBuffer) },
        { "unmapBuffer", {920, 8}, FIELD(PalGraphicsBackendVtable1, unmapBuffer) },
        { "getBufferDeviceAddress", {928, 8}, FIELD(PalGraphicsBackendVtable1, getBufferDeviceAddress) },
        { "createDescriptorSetLayout", {936, 8}, FIELD(PalGraphicsBackendVtable1, createDescriptorSetLayout) },
        { "destroyDescriptorSetLayout", {944, 8}, FIELD(PalGraphicsBackendVtable1, destroyDescriptorSetLayout) },
        { "createDescriptorPool", {952, 8}, FIELD(PalGraphicsBackendVtable1, createDescriptorPool) },
        { "destroyDescriptorPool", {960, 8}, FIELD(PalGraphicsBackendVtable1, destroyDescriptorPool) },
        { "resetDescriptorPool", {968, 8}, FIELD(PalGraphicsBackendVtable1, resetDescriptorPool) },
        { "allocateDescriptorSet", {976, 8}, FIELD(PalGraphicsBackendVtable1, allocateDescriptorSet) },
        { "updateDescriptorSet", {984, 8}, FIELD(PalGraphicsBackendVtable1, updateDescriptorSet) },
        { "createPipelineLayout", {992, 8}, FIELD(PalGraphicsBackendVtable1, createPipelineLayout) },
        { "destroyPipelineLayout", {1000, 8}, FIELD(PalGraphicsBackendVtable1, destroyPipelineLayout) },
        { "createGraphicsPipeline", {1008, 8}, FIELD(PalGraphicsBackendVtable1, createGraphicsPipeline) },
        { "createComputePipeline", {1016, 8}, FIELD(PalGraphicsBackendVtable1, createComputePipeline) },
        { "createRayTracingPipeline", {1024, 8}, FIELD(PalGraphicsBackendVtable1, createRayTracingPipeline) },
        { "destroyPipeline", {1032, 8}, FIELD(PalGraphicsBackendVtable1, destroyPipeline) },
        { "createShaderBindingTable", {1040, 8}, FIELD(PalGraphicsBackendVtable1, createShaderBindingTable) },
        { "destroyShaderBindingTable", {1048, 8}, FIELD(PalGraphicsBackendVtable1, destroyShaderBindingTable) },
        { "updateShaderBindingTable", {1056, 8}, FIELD(PalGraphicsBackendVtable1, updateShaderBindingTable) }
    };

    StructInfo info = {0};
    info.name = "PalGraphicsBackendVtable1";
    info.fields = fields;
    info.fieldCount = ARRAY_SIZE(fields);
    info.expected.align = 8;
    info.expected.size = 1064;
    info.expected.padding = 0;
    info.actual = STRUCT(PalGraphicsBackendVtable1);

    return dumpStruct(&info);
}

bool graphicsBackendVtable2Struct(void)
{
    FieldInfo fields[] = {
        { "vtable1", {0, 8}, FIELD(PalGraphicsBackendVtable2, vtable1) },
        { "canQueueShareOwnership", {8, 8}, FIELD(PalGraphicsBackendVtable2, canQueueShareOwnership) },
        { "canQueueUseUsageState", {16, 8}, FIELD(PalGraphicsBackendVtable2, canQueueUseUsageState) },
        { "canQueueUsePipelineStages", {24, 8}, FIELD(PalGraphicsBackendVtable2, canQueueUsePipelineStages) },
        { "cmdImageOwnershipTransfer", {32, 8}, FIELD(PalGraphicsBackendVtable2, cmdImageOwnershipTransfer) },
        { "cmdBufferOwnershipTransfer", {40, 8}, FIELD(PalGraphicsBackendVtable2, cmdBufferOwnershipTransfer) },
    };

    StructInfo info = {0};
    info.name = "PalGraphicsBackendVtable2";
    info.fields = fields;
    info.fieldCount = ARRAY_SIZE(fields);
    info.expected.align = 8;
    info.expected.size = 48;
    info.expected.padding = 0;
    info.actual = STRUCT(PalGraphicsBackendVtable2);

    return dumpStruct(&info);
}

bool graphicsDebuggerStruct(void)
{
    FieldInfo fields[] = {
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

    StructInfo info = {0};
    info.name = "PalGraphicsDebugger";
    info.fields = fields;
    info.fieldCount = ARRAY_SIZE(fields);
    info.expected.align = 8;
    info.expected.size = 48;
    info.expected.padding = 0;
    info.actual = STRUCT(PalGraphicsDebugger);

    return dumpStruct(&info);
}

bool GraphicsBackendInfoStruct(void)
{
    FieldInfo fields[] = {
        { "vtable", {0, 8}, FIELD(PalGraphicsBackendInfo, vtable) },
        { "version", {8, 4}, FIELD(PalGraphicsBackendInfo, version) },
        { "reserved", {12, 4}, FIELD(PalGraphicsBackendInfo, reserved) }
    };

    StructInfo info = {0};
    info.name = "PalGraphicsBackendInfo";
    info.fields = fields;
    info.fieldCount = ARRAY_SIZE(fields);
    info.expected.align = 8;
    info.expected.size = 16;
    info.expected.padding = 0;
    info.actual = STRUCT(PalGraphicsBackendInfo);

    return dumpStruct(&info);
}

bool graphicsStructs(void)
{
    if (!(g_DumpFlags & ABI_DUMP_QUICK)) {
        palLog(nullptr, "");
        palLog(nullptr, "===========================================");
        palLog(nullptr, "Graphics ABI Dump");
        palLog(nullptr, "===========================================");
        palLog(nullptr, "");
    }

    bool status = adapterInfoStruct();
    if (status == PAL_FALSE) {
        return PAL_FALSE;
    }

    status = imageCapabilitiesStruct();
    if (status == PAL_FALSE) {
        return PAL_FALSE;
    }

    status = resourceCapabilitiesStruct();
    if (status == PAL_FALSE) {
        return PAL_FALSE;
    }

    status = computeCapabilitiesStruct();
    if (status == PAL_FALSE) {
        return PAL_FALSE;
    }

    status = viewportCapabilitiesStruct();
    if (status == PAL_FALSE) {
        return PAL_FALSE;
    }

    status = adapterCapabilitiesStruct();
    if (status == PAL_FALSE) {
        return PAL_FALSE;
    }

    status = formatInfoStruct();
    if (status == PAL_FALSE) {
        return PAL_FALSE;
    }

    status = samplerAnisotropyCapabilitiesStruct();
    if (status == PAL_FALSE) {
        return PAL_FALSE;
    }

    status = multiViewCapabilitiesStruct();
    if (status == PAL_FALSE) {
        return PAL_FALSE;
    }

    status = multiViewportCapabilitiesStruct();
    if (status == PAL_FALSE) {
        return PAL_FALSE;
    }

    status = depthStencilCapabilitiesStruct();
    if (status == PAL_FALSE) {
        return PAL_FALSE;
    }

    status = fragmentShadingRateCapabilitiesStruct();
    if (status == PAL_FALSE) {
        return PAL_FALSE;
    }

    status = meshCapabilitiesStruct();
    if (status == PAL_FALSE) {
        return PAL_FALSE;
    }

    status = rayTracingCapabilitiesStruct();
    if (status == PAL_FALSE) {
        return PAL_FALSE;
    }

    status = descriptorIndexingCapabilitiesStruct();
    if (status == PAL_FALSE) {
        return PAL_FALSE;
    }

    status = memoryRequirementsStruct();
    if (status == PAL_FALSE) {
        return PAL_FALSE;
    }

    status = shaderEntryInfoStruct();
    if (status == PAL_FALSE) {
        return PAL_FALSE;
    }

    status = shaderCreateInfoStruct();
    if (status == PAL_FALSE) {
        return PAL_FALSE;
    }

    status = surfaceCapabilitiesStruct();
    if (status == PAL_FALSE) {
        return PAL_FALSE;
    }

    status = swapchainNextImageInfoStruct();
    if (status == PAL_FALSE) {
        return PAL_FALSE;
    }

    status = swapchainCreateInfoStruct();
    if (status == PAL_FALSE) {
        return PAL_FALSE;
    }

    status = imageInfoStruct();
    if (status == PAL_FALSE) {
        return PAL_FALSE;
    }

    status = imageSubresourceRangeStruct();
    if (status == PAL_FALSE) {
        return PAL_FALSE;
    }

    status = bufferImageCopyInfoStruct();
    if (status == PAL_FALSE) {
        return PAL_FALSE;
    }

    status = imageCopyInfoStruct();
    if (status == PAL_FALSE) {
        return PAL_FALSE;
    }

    status = imageCreateInfoStruct();
    if (status == PAL_FALSE) {
        return PAL_FALSE;
    }

    status = imageViewCreateInfoStruct();
    if (status == PAL_FALSE) {
        return PAL_FALSE;
    }

    status = samplerCreateInfoStruct();
    if (status == PAL_FALSE) {
        return PAL_FALSE;
    }

    status = imageStagingRequirementsStruct();
    if (status == PAL_FALSE) {
        return PAL_FALSE;
    }

    status = bufferCopyInfoStruct();
    if (status == PAL_FALSE) {
        return PAL_FALSE;
    }

    status = drawIndirectDataStruct();
    if (status == PAL_FALSE) {
        return PAL_FALSE;
    }

    status = drawIndexedIndirectDataStruct();
    if (status == PAL_FALSE) {
        return PAL_FALSE;
    }

    status = dispatchIndirectDataStruct();
    if (status == PAL_FALSE) {
        return PAL_FALSE;
    }

    status = accelerationStructureInstanceStruct();
    if (status == PAL_FALSE) {
        return PAL_FALSE;
    }

    status = accelerationStructureBuildSizeStruct();
    if (status == PAL_FALSE) {
        return PAL_FALSE;
    }

    status = geometryDataTriangleStruct();
    if (status == PAL_FALSE) {
        return PAL_FALSE;
    }

    status = geometryDataAABBSStruct();
    if (status == PAL_FALSE) {
        return PAL_FALSE;
    }

    status = geometryStruct();
    if (status == PAL_FALSE) {
        return PAL_FALSE;
    }

    status = accelerationStructureBuildInfoStruct();
    if (status == PAL_FALSE) {
        return PAL_FALSE;
    }

    status = accelerationStructureCreateInfoStruct();
    if (status == PAL_FALSE) {
        return PAL_FALSE;
    }

    status = vertexAttributeStruct();
    if (status == PAL_FALSE) {
        return PAL_FALSE;
    }

    status = vertexLayoutStruct();
    if (status == PAL_FALSE) {
        return PAL_FALSE;
    }

    status = rasterizerStateStruct();
    if (status == PAL_FALSE) {
        return PAL_FALSE;
    }

    status = multisampleStateStruct();
    if (status == PAL_FALSE) {
        return PAL_FALSE;
    }

    status = stencilOpStateStruct();
    if (status == PAL_FALSE) {
        return PAL_FALSE;
    }

    status = depthStencilStateStruct();
    if (status == PAL_FALSE) {
        return PAL_FALSE;
    }

    status = colorBlendAttachmentStruct();
    if (status == PAL_FALSE) {
        return PAL_FALSE;
    }

    status = fragmentShadingRateStateStruct();
    if (status == PAL_FALSE) {
        return PAL_FALSE;
    }

    status = pushConstantInfoStruct();
    if (status == PAL_FALSE) {
        return PAL_FALSE;
    }

    status = pipelineLayoutCreateInfoStruct();
    if (status == PAL_FALSE) {
        return PAL_FALSE;
    }

    status = graphicsPipelineCreateInfoStruct();
    if (status == PAL_FALSE) {
        return PAL_FALSE;
    }

    status = computePipelineCreateInfoStruct();
    if (status == PAL_FALSE) {
        return PAL_FALSE;
    }

    status = rayTracingShaderGroupCreateInfoStruct();
    if (status == PAL_FALSE) {
        return PAL_FALSE;
    }

    status = rayTracingPipelineCreateInfoStruct();
    if (status == PAL_FALSE) {
        return PAL_FALSE;
    }

    status = descriptorSetLayoutBindingStruct();
    if (status == PAL_FALSE) {
        return PAL_FALSE;
    }

    status = descriptorPoolBindingSizeStruct();
    if (status == PAL_FALSE) {
        return PAL_FALSE;
    }

    status = descriptorBufferInfoStruct();
    if (status == PAL_FALSE) {
        return PAL_FALSE;
    }

    status = descriptorImageViewInfoStruct();
    if (status == PAL_FALSE) {
        return PAL_FALSE;
    }

    status = descriptorSamplerInfoStruct();
    if (status == PAL_FALSE) {
        return PAL_FALSE;
    }

    status = descriptorTLASInfoStruct();
    if (status == PAL_FALSE) {
        return PAL_FALSE;
    }

    status = descriptorSetWriteInfoStruct();
    if (status == PAL_FALSE) {
        return PAL_FALSE;
    }

    status = descriptorSetLayoutCreateInfoStruct();
    if (status == PAL_FALSE) {
        return PAL_FALSE;
    }

    status = descriptorPoolCreateInfoStruct();
    if (status == PAL_FALSE) {
        return PAL_FALSE;
    }

    status = shaderBindingTableRecordInfoStruct();
    if (status == PAL_FALSE) {
        return PAL_FALSE;
    }

    status = shaderBindingTableCreateInfoStruct();
    if (status == PAL_FALSE) {
        return PAL_FALSE;
    }

    status = commandBufferSubmitInfoStruct();
    if (status == PAL_FALSE) {
        return PAL_FALSE;
    }

    status = clearValueStruct();
    if (status == PAL_FALSE) {
        return PAL_FALSE;
    }

    status = attachmentDescStruct();
    if (status == PAL_FALSE) {
        return PAL_FALSE;
    }

    status = viewportStruct();
    if (status == PAL_FALSE) {
        return PAL_FALSE;
    }

    status = rect2DStruct();
    if (status == PAL_FALSE) {
        return PAL_FALSE;
    }

    status = renderingInfoStruct();
    if (status == PAL_FALSE) {
        return PAL_FALSE;
    }

    status = renderingLayoutInfoStruct();
    if (status == PAL_FALSE) {
        return PAL_FALSE;
    }

    status = workGroupBuildDataStruct();
    if (status == PAL_FALSE) {
        return PAL_FALSE;
    }

    status = workGroupInfoStruct();
    if (status == PAL_FALSE) {
        return PAL_FALSE;
    }

    status = barrierInfoStruct();
    if (status == PAL_FALSE) {
        return PAL_FALSE;
    }

    status = graphicsBackendVtable1Struct();
    if (status == PAL_FALSE) {
        return PAL_FALSE;
    }

    status = graphicsBackendVtable2Struct();
    if (status == PAL_FALSE) {
        return PAL_FALSE;
    }

    status = graphicsDebuggerStruct();
    if (status == PAL_FALSE) {
        return PAL_FALSE;
    }

    return GraphicsBackendInfoStruct();
}