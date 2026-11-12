
#include "helpers.h"
#include "pal2/graphics/swapchain.h"

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