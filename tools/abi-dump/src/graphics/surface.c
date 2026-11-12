
#include "helpers.h"
#include "pal2/graphics/surface.h"

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