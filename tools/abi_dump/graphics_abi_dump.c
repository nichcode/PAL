
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

    return checkABI(&descriptorIndexingCap, flags);
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

    return PAL_TRUE;
}