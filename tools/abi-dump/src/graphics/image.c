
#include "helpers.h"
#include "pal2/graphics/image.h"

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