
#include "helpers.h"
#include "pal2/graphics/descriptor.h"

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