
#include "helpers.h"
#include "pal2/graphics/sampler.h"

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