
#include "helpers.h"
#include "pal2/graphics/adapter.h"

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