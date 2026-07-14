
/**
 PAL - Prime Abstraction Layer
 Copyright (C) 2025
 Licensed under the Zlib license. See LICENSE file in root.
 */

#include "dumps.h"
#include "pal/pal_graphics.h"

static void adapterInfoDump(PalBool verbose)
{
    uint32_t xSize = 200;
    uint32_t xAlign = 8;
    uint32_t xOffset1 = 0;
    uint32_t xOffset2 = 8;
    uint32_t xOffset3 = 16;
    uint32_t xOffset4 = 20;
    uint32_t xOffset5 = 24;
    uint32_t xOffset6 = 28;
    uint32_t xOffset7 = 32;
    uint32_t xOffset8 = 36;
    uint32_t xOffset9 = 40;
    uint32_t xOffset10 = 168;
    uint32_t xPadding = 0;

    uint32_t ySize = sizeof(PalAdapterInfo);
    uint32_t yAlign = PAL_ALIGNOF(PalAdapterInfo);
    uint32_t yOffset1 = offsetof(PalAdapterInfo, vram);
    uint32_t yOffset2 = offsetof(PalAdapterInfo, sharedMemory);
    uint32_t yOffset3 = offsetof(PalAdapterInfo, vendorId);
    uint32_t yOffset4 = offsetof(PalAdapterInfo, deviceId);
    uint32_t yOffset5 = offsetof(PalAdapterInfo, driverVersion);
    uint32_t yOffset6 = offsetof(PalAdapterInfo, shaderFormats);
    uint32_t yOffset7 = offsetof(PalAdapterInfo, type);
    uint32_t yOffset8 = offsetof(PalAdapterInfo, apiType);
    uint32_t yOffset9 = offsetof(PalAdapterInfo, name);
    uint32_t yOffset10 = offsetof(PalAdapterInfo, backendName);
    uint32_t yPadding = (yAlign - (ySize % yAlign)) % yAlign;

    const char* result = s_FailedString;
    // clang-format off
    if (xSize == ySize            && 
        xAlign == yAlign          && 
        xOffset1 == yOffset1      && 
        xOffset2 == yOffset2      && 
        xOffset3 == yOffset3      &&
        xOffset4 == yOffset4      &&
        xOffset5 == yOffset5      &&
        xOffset6 == yOffset6      &&
        xOffset7 == yOffset7      &&
        xOffset8 == yOffset8      &&
        xOffset9 == yOffset9      &&
        xOffset10 == yOffset10    &&
        xPadding == yPadding) {
        result = s_PassedString;
    }
    // clang-format on

    palLog(nullptr, "struct: PalAdapterInfo");
    if (verbose) {
        palLog(nullptr, "===========================================");
        palLog(nullptr, "Field              Expected     Actual");
        palLog(nullptr, "===========================================");

        palLog(nullptr, "size               %u          %u", xSize, ySize);
        palLog(nullptr, "align              %u            %u", xAlign, yAlign);
        palLog(nullptr, "padding            %u            %u", xPadding, yPadding);
        palLog(nullptr, "vram @             %u            %u", xOffset1, yOffset1);
        palLog(nullptr, "sharedMemory @     %u            %u", xOffset2, yOffset2);
        palLog(nullptr, "vendorId @         %u           %u", xOffset3, yOffset3);
        palLog(nullptr, "deviceId @         %u           %u", xOffset4, yOffset4);
        palLog(nullptr, "driverVersion @    %u           %u", xOffset5, yOffset5);
        palLog(nullptr, "shaderFormats @    %u           %u", xOffset6, yOffset6);
        palLog(nullptr, "type @             %u           %u", xOffset7, yOffset7);
        palLog(nullptr, "apiType @          %u           %u", xOffset8, yOffset8);
        palLog(nullptr, "name @             %u           %u", xOffset9, yOffset9);
        palLog(nullptr, "backendName @      %u          %u", xOffset10, yOffset10);
        palLog(nullptr, "===========================================");
    }

    palLog(nullptr, "Status: %s", result);
    palLog(nullptr, "");
}

static void imageCapDump(PalBool verbose)
{
    uint32_t xSize = 20;
    uint32_t xAlign = 4;
    uint32_t xOffset1 = 0;
    uint32_t xOffset2 = 4;
    uint32_t xOffset3 = 8;
    uint32_t xOffset4 = 12;
    uint32_t xOffset5 = 16;
    uint32_t xPadding = 0;

    uint32_t ySize = sizeof(PalImageCapabilities);
    uint32_t yAlign = PAL_ALIGNOF(PalImageCapabilities);
    uint32_t yOffset1 = offsetof(PalImageCapabilities, maxWidth);
    uint32_t yOffset2 = offsetof(PalImageCapabilities, maxHeight);
    uint32_t yOffset3 = offsetof(PalImageCapabilities, maxDepth);
    uint32_t yOffset4 = offsetof(PalImageCapabilities, maxArrayLayers);
    uint32_t yOffset5 = offsetof(PalImageCapabilities, maxMipLevels);
    uint32_t yPadding = (yAlign - (ySize % yAlign)) % yAlign;

    const char* result = s_FailedString;
    // clang-format off
    if (xSize == ySize            && 
        xAlign == yAlign          && 
        xOffset1 == yOffset1      && 
        xOffset2 == yOffset2      && 
        xOffset3 == yOffset3      &&
        xOffset4 == yOffset4      &&
        xOffset5 == yOffset5      &&
        xPadding == yPadding) {
        result = s_PassedString;
    }
    // clang-format on

    palLog(nullptr, "struct: PalImageCapabilities");
    if (verbose) {
        palLog(nullptr, "===========================================");
        palLog(nullptr, "Field              Expected     Actual");
        palLog(nullptr, "===========================================");

        palLog(nullptr, "size               %u           %u", xSize, ySize);
        palLog(nullptr, "align              %u            %u", xAlign, yAlign);
        palLog(nullptr, "padding            %u            %u", xPadding, yPadding);
        palLog(nullptr, "maxWidth @         %u            %u", xOffset1, yOffset1);
        palLog(nullptr, "maxHeight @        %u            %u", xOffset2, yOffset2);
        palLog(nullptr, "maxDepth @         %u            %u", xOffset3, yOffset3);
        palLog(nullptr, "maxArrayLayers @   %u           %u", xOffset4, yOffset4);
        palLog(nullptr, "maxMipLevels @     %u           %u", xOffset5, yOffset5);
        palLog(nullptr, "===========================================");
    }

    palLog(nullptr, "Status: %s", result);
    palLog(nullptr, "");
}

static void resourceCapDump(PalBool verbose)
{
    uint32_t xSize = 52;
    uint32_t xAlign = 4;
    uint32_t xOffset1 = 0;
    uint32_t xOffset2 = 4;
    uint32_t xOffset3 = 8;
    uint32_t xOffset4 = 12;
    uint32_t xOffset5 = 16;
    uint32_t xOffset6 = 20;
    uint32_t xOffset7 = 24;
    uint32_t xOffset8 = 28;
    uint32_t xOffset9 = 32;
    uint32_t xOffset10 = 36;
    uint32_t xOffset11 = 40;
    uint32_t xOffset12 = 44;
    uint32_t xOffset13 = 48;
    uint32_t xPadding = 0;

    uint32_t ySize = sizeof(PalResourceCapabilities);
    uint32_t yAlign = PAL_ALIGNOF(PalResourceCapabilities);
    uint32_t yOffset1 = offsetof(PalResourceCapabilities, maxPerStageSampledImages);
    uint32_t yOffset2 = offsetof(PalResourceCapabilities, maxPerSetSampledImages);
    uint32_t yOffset3 = offsetof(PalResourceCapabilities, maxPerStageStorageImages);
    uint32_t yOffset4 = offsetof(PalResourceCapabilities, maxPerSetStorageImages);
    uint32_t yOffset5 = offsetof(PalResourceCapabilities, maxPerStageSamplers);
    uint32_t yOffset6 = offsetof(PalResourceCapabilities, maxPerSetSamplers);
    uint32_t yOffset7 = offsetof(PalResourceCapabilities, maxPerStageStorageBuffers);
    uint32_t yOffset8 = offsetof(PalResourceCapabilities, maxPerSetStorageBuffers);
    uint32_t yOffset9 = offsetof(PalResourceCapabilities, maxPerStageUniformBuffers);
    uint32_t yOffset10 = offsetof(PalResourceCapabilities, maxPerSetUniformBuffers);
    uint32_t yOffset11 = offsetof(PalResourceCapabilities, maxPerStageAccelerationStructure);
    uint32_t yOffset12 = offsetof(PalResourceCapabilities, maxPerSetAccelerationStructure);
    uint32_t yOffset13 = offsetof(PalResourceCapabilities, maxBoundSets);
    uint32_t yPadding = (yAlign - (ySize % yAlign)) % yAlign;

    const char* result = s_FailedString;
    // clang-format off
    if (xSize == ySize            && 
        xAlign == yAlign          && 
        xOffset1 == yOffset1      && 
        xOffset2 == yOffset2      && 
        xOffset3 == yOffset3      &&
        xOffset4 == yOffset4      &&
        xOffset5 == yOffset5      &&
        xOffset6 == yOffset6      &&
        xOffset7 == yOffset7      &&
        xOffset8 == yOffset8      &&
        xOffset9 == yOffset9      &&
        xOffset10 == yOffset10    &&
        xOffset11 == yOffset11    &&
        xOffset12 == yOffset12    &&
        xOffset13 == yOffset13    &&
        xPadding == yPadding) {
        result = s_PassedString;
    }

    palLog(nullptr, "struct: PalResourceCapabilities");
    if (verbose) {
        palLog(nullptr, "========================================================");
        palLog(nullptr, "Field                               Expected     Actual");
        palLog(nullptr, "========================================================");

        palLog(nullptr, "size                                %u           %u", xSize, ySize);
        palLog(nullptr, "align                               %u            %u", xAlign, yAlign);
        palLog(nullptr, "padding                             %u            %u", xPadding, yPadding);
        palLog(nullptr, "maxPerStageSampledImages @          %u            %u", xOffset1, yOffset1);
        palLog(nullptr, "maxPerSetSampledImages @            %u            %u", xOffset2, yOffset2);
        palLog(nullptr, "maxPerStageStorageImages @          %u            %u", xOffset3, yOffset3);
        palLog(nullptr, "maxPerSetStorageImages @            %u           %u", xOffset4, yOffset4);
        palLog(nullptr, "maxPerStageSamplers @               %u           %u", xOffset5, yOffset5);
        palLog(nullptr, "maxPerSetSamplers @                 %u           %u", xOffset6, yOffset6);
        palLog(nullptr, "maxPerStageStorageBuffers @         %u           %u", xOffset7, yOffset7);
        palLog(nullptr, "maxPerSetStorageBuffers @           %u           %u", xOffset8, yOffset8);
        palLog(nullptr, "maxPerStageUniformBuffers @         %u           %u", xOffset9, yOffset9);
        palLog(nullptr, "maxPerSetUniformBuffers @           %u           %u", xOffset10, yOffset10);
        palLog(nullptr, "maxPerStageAccelerationStructure @  %u           %u", xOffset11, yOffset11);
        palLog(nullptr, "maxPerSetAccelerationStructure @    %u           %u", xOffset12, yOffset12);
        palLog(nullptr, "maxBoundSets @                      %u           %u", xOffset13, yOffset13);
        palLog(nullptr, "===========================================");
    }
    // clang-format on

    palLog(nullptr, "Status: %s", result);
    palLog(nullptr, "");
}

static void computeCapDump(PalBool verbose)
{
    uint32_t xSize = 28;
    uint32_t xAlign = 4;
    uint32_t xOffset1 = 0;
    uint32_t xOffset2 = 4;
    uint32_t xOffset3 = 16;
    uint32_t xPadding = 0;

    uint32_t ySize = sizeof(PalComputeCapabilities);
    uint32_t yAlign = PAL_ALIGNOF(PalComputeCapabilities);
    uint32_t yOffset1 = offsetof(PalComputeCapabilities, maxWorkGroupInvocations);
    uint32_t yOffset2 = offsetof(PalComputeCapabilities, maxWorkGroupCount);
    uint32_t yOffset3 = offsetof(PalComputeCapabilities, maxWorkGroupSize);
    uint32_t yPadding = (yAlign - (ySize % yAlign)) % yAlign;

    const char* result = s_FailedString;
    // clang-format off
    if (xSize == ySize            && 
        xAlign == yAlign          && 
        xOffset1 == yOffset1      && 
        xOffset2 == yOffset2      && 
        xOffset3 == yOffset3      &&
        xPadding == yPadding) {
        result = s_PassedString;
    }
    // clang-format on

    palLog(nullptr, "struct: PalComputeCapabilities");
    if (verbose) {
        palLog(nullptr, "=============================================");
        palLog(nullptr, "Field                      Expected     Actual");
        palLog(nullptr, "==============================================");

        palLog(nullptr, "size                       %u           %u", xSize, ySize);
        palLog(nullptr, "align                      %u            %u", xAlign, yAlign);
        palLog(nullptr, "padding                    %u            %u", xPadding, yPadding);
        palLog(nullptr, "maxWorkGroupInvocations @  %u            %u", xOffset1, yOffset1);
        palLog(nullptr, "maxWorkGroupCount[3] @     %u            %u", xOffset2, yOffset2);
        palLog(nullptr, "maxWorkGroupSize[3] @      %u           %u", xOffset3, yOffset3);
        palLog(nullptr, "===========================================");
    }

    palLog(nullptr, "Status: %s", result);
    palLog(nullptr, "");
}

static void viewportCapDump(PalBool verbose)
{
    uint32_t xSize = 16;
    uint32_t xAlign = 4;
    uint32_t xOffset1 = 0;
    uint32_t xOffset2 = 4;
    uint32_t xOffset3 = 8;
    uint32_t xOffset4 = 12;
    uint32_t xPadding = 0;

    uint32_t ySize = sizeof(PalViewportCapabilities);
    uint32_t yAlign = PAL_ALIGNOF(PalViewportCapabilities);
    uint32_t yOffset1 = offsetof(PalViewportCapabilities, maxWidth);
    uint32_t yOffset2 = offsetof(PalViewportCapabilities, maxHeight);
    uint32_t yOffset3 = offsetof(PalViewportCapabilities, minBoundsRange);
    uint32_t yOffset4 = offsetof(PalViewportCapabilities, maxBoundsRange);
    uint32_t yPadding = (yAlign - (ySize % yAlign)) % yAlign;

    const char* result = s_FailedString;
    // clang-format off
    if (xSize == ySize            && 
        xAlign == yAlign          && 
        xOffset1 == yOffset1      && 
        xOffset2 == yOffset2      && 
        xOffset3 == yOffset3      &&
        xOffset4 == yOffset4      &&
        xPadding == yPadding) {
        result = s_PassedString;
    }
    // clang-format on

    palLog(nullptr, "struct: PalViewportCapabilities");
    if (verbose) {
        palLog(nullptr, "===========================================");
        palLog(nullptr, "Field              Expected     Actual");
        palLog(nullptr, "===========================================");

        palLog(nullptr, "size              %u           %u", xSize, ySize);
        palLog(nullptr, "align             %u            %u", xAlign, yAlign);
        palLog(nullptr, "padding           %u            %u", xPadding, yPadding);
        palLog(nullptr, "maxWidth @        %u            %u", xOffset1, yOffset1);
        palLog(nullptr, "maxHeight @       %u            %u", xOffset2, yOffset2);
        palLog(nullptr, "minBoundsRange @  %u            %u", xOffset3, yOffset3);
        palLog(nullptr, "maxBoundsRange @  %u           %u", xOffset4, yOffset4);
        palLog(nullptr, "===========================================");
    }

    palLog(nullptr, "Status: %s", result);
    palLog(nullptr, "");
}

void graphicsABIDump(PalBool verbose)
{
    palLog(nullptr, "");
    palLog(nullptr, "===========================================");
    palLog(nullptr, "Graphics ABI Dump");
    palLog(nullptr, "===========================================");
    palLog(nullptr, "");

    adapterInfoDump(verbose);
    imageCapDump(verbose);
    resourceCapDump(verbose);
    computeCapDump(verbose);
    viewportCapDump(verbose);
}