
/**
 PAL - Prime Abstraction Layer
 Copyright (C) 2025
 Licensed under the Zlib license. See LICENSE file in root.
 */

#include "dumps.h"
#include "pal/pal_graphics.h"

static PalBool adapterDump(PalBool verbose)
{
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

    // clang-format off
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
    // clang-format on

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

    PalBool status = checkABI(&adapterInfo, verbose);
    if (status == PAL_FALSE) {
        return status;
    }

    status = checkABI(&imageCap, verbose);
    if (status == PAL_FALSE) {
        return status;
    }

    status = checkABI(&resourceCap, verbose);
    if (status == PAL_FALSE) {
        return status;
    }

    status = checkABI(&computeCap, verbose);
    if (status == PAL_FALSE) {
        return status;
    }

    status = checkABI(&viewportCap, verbose);
    if (status == PAL_FALSE) {
        return status;
    }

    return checkABI(&adapterCap, verbose);
}

PalBool graphicsABIDump(PalBool verbose)
{
    palLog(nullptr, "");
    palLog(nullptr, "===========================================");
    palLog(nullptr, "Graphics ABI Dump");
    palLog(nullptr, "===========================================");
    palLog(nullptr, "");

    PalBool status = adapterDump(verbose);
    if (status == PAL_FALSE) {
        return status;
    }

    return PAL_TRUE;
}