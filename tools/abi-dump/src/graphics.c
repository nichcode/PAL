
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
}