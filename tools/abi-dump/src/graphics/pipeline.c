
#include "helpers.h"
#include "pal2/graphics/pipeline.h"

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

bool rasterizerStateStruct(void)
{
    FieldInfo fields[] = {
        { "enableDepthClamp", {0, 4}, FIELD(PalRasterizerState, enableDepthClamp) },
        { "enableDepthBias", {4, 4}, FIELD(PalRasterizerState, enableDepthBias) },
        { "depthBiasConstant", {8, 4}, FIELD(PalRasterizerState, depthBiasConstant) },
        { "depthBiasSlope", {12, 4}, FIELD(PalRasterizerState, depthBiasSlope) },
        { "depthBiasClamp", {16, 4}, FIELD(PalRasterizerState, depthBiasClamp) },
        { "polygonMode", {20, 4}, FIELD(PalRasterizerState, polygonMode) },
        { "cullMode", {24, 4}, FIELD(PalRasterizerState, cullMode) },
        { "frontFace", {28, 4}, FIELD(PalRasterizerState, frontFace) }
    };

    StructInfo info = {0};
    info.name = "PalRasterizerState";
    info.fields = fields;
    info.fieldCount = ARRAY_SIZE(fields);
    info.expected.align = 4;
    info.expected.size = 32;
    info.expected.padding = 0;
    info.actual = STRUCT(PalRasterizerState);

    return dumpStruct(&info);
}

bool multisampleStateStruct(void)
{
    FieldInfo fields[] = {
        { "sampleMask", {0, 8}, FIELD(PalMultisampleState, sampleMask) },
        { "enableSampleShading", {8, 4}, FIELD(PalMultisampleState, enableSampleShading) },
        { "enableAlphaToCoverage", {12, 4}, FIELD(PalMultisampleState, enableAlphaToCoverage) },
        { "sampleCount", {16, 4}, FIELD(PalMultisampleState, sampleCount) },
        { "minSampleShading", {20, 4}, FIELD(PalMultisampleState, minSampleShading) }
    };

    StructInfo info = {0};
    info.name = "PalMultisampleState";
    info.fields = fields;
    info.fieldCount = ARRAY_SIZE(fields);
    info.expected.align = 8;
    info.expected.size = 24;
    info.expected.padding = 0;
    info.actual = STRUCT(PalMultisampleState);

    return dumpStruct(&info);
}

bool stencilOpStateStruct(void)
{
    FieldInfo fields[] = {
        { "failOp", {0, 4}, FIELD(PalStencilOpState, failOp) },
        { "passOp", {4, 4}, FIELD(PalStencilOpState, passOp) },
        { "depthFailOp", {8, 4}, FIELD(PalStencilOpState, depthFailOp) },
        { "compareOp", {12, 4}, FIELD(PalStencilOpState, compareOp) }
    };

    StructInfo info = {0};
    info.name = "PalStencilOpState";
    info.fields = fields;
    info.fieldCount = ARRAY_SIZE(fields);
    info.expected.align = 4;
    info.expected.size = 16;
    info.expected.padding = 0;
    info.actual = STRUCT(PalStencilOpState);

    return dumpStruct(&info);
}

bool depthStencilStateStruct(void)
{
    FieldInfo fields[] = {
        { "enableDepthTest", {0, 4}, FIELD(PalDepthStencilState, enableDepthTest) },
        { "enableDepthWrite", {4, 4}, FIELD(PalDepthStencilState, enableDepthWrite) },
        { "enableStencilTest", {8, 4}, FIELD(PalDepthStencilState, enableStencilTest) },
        { "compareOp", {12, 4}, FIELD(PalDepthStencilState, compareOp) },
        { "frontStencilOpState", {16, 16}, FIELD(PalDepthStencilState, frontStencilOpState) },
        { "backStencilOpState", {32, 16}, FIELD(PalDepthStencilState, backStencilOpState) },
    };

    StructInfo info = {0};
    info.name = "PalDepthStencilState";
    info.fields = fields;
    info.fieldCount = ARRAY_SIZE(fields);
    info.expected.align = 4;
    info.expected.size = 48;
    info.expected.padding = 0;
    info.actual = STRUCT(PalDepthStencilState);

    return dumpStruct(&info);
}

bool colorBlendAttachmentStruct(void)
{
    FieldInfo fields[] = {
        { "enableBlend", {0, 4}, FIELD(PalColorBlendAttachment, enableBlend) },
        { "colorWriteMask", {4, 4}, FIELD(PalColorBlendAttachment, colorWriteMask) },
        { "dstColorBlendFactor", {8, 4}, FIELD(PalColorBlendAttachment, dstColorBlendFactor) },
        { "srcColorBlendFactor", {12, 4}, FIELD(PalColorBlendAttachment, srcColorBlendFactor) },
        { "colorBlendOp", {16, 4}, FIELD(PalColorBlendAttachment, colorBlendOp) },
        { "dstAlphaBlendFactor", {20, 4}, FIELD(PalColorBlendAttachment, dstAlphaBlendFactor) },
        { "srcAlphaBlendFactor", {24, 4}, FIELD(PalColorBlendAttachment, srcAlphaBlendFactor) },
        { "alphaBlendOp", {28, 4}, FIELD(PalColorBlendAttachment, alphaBlendOp) }
    };

    StructInfo info = {0};
    info.name = "PalColorBlendAttachment";
    info.fields = fields;
    info.fieldCount = ARRAY_SIZE(fields);
    info.expected.align = 4;
    info.expected.size = 32;
    info.expected.padding = 0;
    info.actual = STRUCT(PalColorBlendAttachment);

    return dumpStruct(&info);
}

bool fragmentShadingRateStateStruct(void)
{
    FieldInfo fields[] = {
        { "rate", {0, 4}, FIELD(PalFragmentShadingRateState, rate) },
        { "combinerOps", {4, 8}, FIELD(PalFragmentShadingRateState, combinerOps) }
    };

    StructInfo info = {0};
    info.name = "PalFragmentShadingRateState";
    info.fields = fields;
    info.fieldCount = ARRAY_SIZE(fields);
    info.expected.align = 4;
    info.expected.size = 12;
    info.expected.padding = 0;
    info.actual = STRUCT(PalFragmentShadingRateState);

    return dumpStruct(&info);
}

bool pushConstantInfoStruct(void)
{
    FieldInfo fields[] = {
        { "offset", {0, 4}, FIELD(PalPushConstantInfo, offset) },
        { "size", {4, 4}, FIELD(PalPushConstantInfo, size) }
    };

    StructInfo info = {0};
    info.name = "PalPushConstantInfo";
    info.fields = fields;
    info.fieldCount = ARRAY_SIZE(fields);
    info.expected.align = 4;
    info.expected.size = 8;
    info.expected.padding = 0;
    info.actual = STRUCT(PalPushConstantInfo);

    return dumpStruct(&info);
}

bool pipelineLayoutCreateInfoStruct(void)
{
    FieldInfo fields[] = {
        { "descriptorSetLayouts", {0, 8}, FIELD(PalPipelineLayoutCreateInfo, descriptorSetLayouts) },
        { "pushConstantInfo", {8, 8}, FIELD(PalPipelineLayoutCreateInfo, pushConstantInfo) },
        { "descriptorSetLayoutCount", {16, 4}, FIELD(PalPipelineLayoutCreateInfo, descriptorSetLayoutCount) },
        { "usePushConstant", {20, 4}, FIELD(PalPipelineLayoutCreateInfo, usePushConstant) }
    };

    StructInfo info = {0};
    info.name = "PalPipelineLayoutCreateInfo";
    info.fields = fields;
    info.fieldCount = ARRAY_SIZE(fields);
    info.expected.align = 8;
    info.expected.size = 24;
    info.expected.padding = 0;
    info.actual = STRUCT(PalPipelineLayoutCreateInfo);

    return dumpStruct(&info);
}

bool graphicsPipelineCreateInfoStruct(void)
{
    FieldInfo fields[] = {
        { "pipelineLayout", {0, 8}, FIELD(PalGraphicsPipelineCreateInfo, pipelineLayout) },
        { "shaders", {8, 8}, FIELD(PalGraphicsPipelineCreateInfo, shaders) },
        { "vertexLayouts", {16, 8}, FIELD(PalGraphicsPipelineCreateInfo, vertexLayouts) },
        { "colorBlendAttachments", {24, 8}, FIELD(PalGraphicsPipelineCreateInfo, colorBlendAttachments) },
        { "rasterizerState", {32, 8}, FIELD(PalGraphicsPipelineCreateInfo, rasterizerState) },
        { "multisampleState", {40, 8}, FIELD(PalGraphicsPipelineCreateInfo, multisampleState) },
        { "depthStencilState", {48, 8}, FIELD(PalGraphicsPipelineCreateInfo, depthStencilState) },
        { "fragmentShadingRateState", {56, 8}, FIELD(PalGraphicsPipelineCreateInfo, fragmentShadingRateState) },
        { "renderingLayout", {64, 8}, FIELD(PalGraphicsPipelineCreateInfo, renderingLayout) },
        { "primitiveRestartEnable", {72, 4}, FIELD(PalGraphicsPipelineCreateInfo, primitiveRestartEnable) },
        { "vertexLayoutCount", {76, 4}, FIELD(PalGraphicsPipelineCreateInfo, vertexLayoutCount) },
        { "colorBlendAttachmentCount", {80, 4}, FIELD(PalGraphicsPipelineCreateInfo, colorBlendAttachmentCount) },
        { "shaderCount", {84, 4}, FIELD(PalGraphicsPipelineCreateInfo, shaderCount) },
        { "indexType", {88, 4}, FIELD(PalGraphicsPipelineCreateInfo, indexType) },
        { "topology", {92, 4}, FIELD(PalGraphicsPipelineCreateInfo, topology) }
    };

    StructInfo info = {0};
    info.name = "PalGraphicsPipelineCreateInfo";
    info.fields = fields;
    info.fieldCount = ARRAY_SIZE(fields);
    info.expected.align = 8;
    info.expected.size = 96;
    info.expected.padding = 0;
    info.actual = STRUCT(PalGraphicsPipelineCreateInfo);

    return dumpStruct(&info);
}

bool computePipelineCreateInfoStruct(void)
{
    FieldInfo fields[] = {
        { "pipelineLayout", {0, 8}, FIELD(PalComputePipelineCreateInfo, pipelineLayout) },
        { "computeShader", {8, 8}, FIELD(PalComputePipelineCreateInfo, computeShader) }
    };

    StructInfo info = {0};
    info.name = "PalComputePipelineCreateInfo";
    info.fields = fields;
    info.fieldCount = ARRAY_SIZE(fields);
    info.expected.align = 8;
    info.expected.size = 16;
    info.expected.padding = 0;
    info.actual = STRUCT(PalComputePipelineCreateInfo);

    return dumpStruct(&info);
}

bool rayTracingShaderGroupCreateInfoStruct(void)
{
    FieldInfo fields[] = {
        { "type", {0, 4}, FIELD(PalRayTracingShaderGroupCreateInfo, type) },
        { "anyHitShaderIndex", {4, 4}, FIELD(PalRayTracingShaderGroupCreateInfo, anyHitShaderIndex) },
        { "anyHitShaderEntryIndex", {8, 4}, FIELD(PalRayTracingShaderGroupCreateInfo, anyHitShaderEntryIndex) },
        { "closestHitShaderIndex", {12, 4}, FIELD(PalRayTracingShaderGroupCreateInfo, closestHitShaderIndex) },
        { "closestHitShaderEntryIndex", {16, 4}, FIELD(PalRayTracingShaderGroupCreateInfo, closestHitShaderEntryIndex) },
        { "generalShaderIndex", {20, 4}, FIELD(PalRayTracingShaderGroupCreateInfo, generalShaderIndex) },
        { "generalShaderEntryIndex", {24, 4}, FIELD(PalRayTracingShaderGroupCreateInfo, generalShaderEntryIndex) },
        { "intersectionShaderIndex", {28, 4}, FIELD(PalRayTracingShaderGroupCreateInfo, intersectionShaderIndex) },
        { "intersectionShaderEntryIndex", {32, 4}, FIELD(PalRayTracingShaderGroupCreateInfo, intersectionShaderEntryIndex) },
        { "maxDataSize", {36, 4}, FIELD(PalRayTracingShaderGroupCreateInfo, maxDataSize) }
    };

    StructInfo info = {0};
    info.name = "PalRayTracingShaderGroupCreateInfo";
    info.fields = fields;
    info.fieldCount = ARRAY_SIZE(fields);
    info.expected.align = 4;
    info.expected.size = 40;
    info.expected.padding = 0;
    info.actual = STRUCT(PalRayTracingShaderGroupCreateInfo);

    return dumpStruct(&info);
}

bool rayTracingPipelineCreateInfoStruct(void)
{
    FieldInfo fields[] = {
        { "pipelineLayout", {0, 8}, FIELD(PalRayTracingPipelineCreateInfo, pipelineLayout) },
        { "shaderGroups", {8, 8}, FIELD(PalRayTracingPipelineCreateInfo, shaderGroups) },
        { "shaders", {16, 8}, FIELD(PalRayTracingPipelineCreateInfo, shaders) },
        { "shaderGroupCount", {24, 4}, FIELD(PalRayTracingPipelineCreateInfo, shaderGroupCount) },
        { "shaderCount", {28, 4}, FIELD(PalRayTracingPipelineCreateInfo, shaderCount) },
        { "maxRecursionDepth", {32, 4}, FIELD(PalRayTracingPipelineCreateInfo, maxRecursionDepth) },
        { "maxAttributeSize", {36, 4}, FIELD(PalRayTracingPipelineCreateInfo, maxAttributeSize) },
        { "maxPayloadSize", {40, 4}, FIELD(PalRayTracingPipelineCreateInfo, maxPayloadSize) },
        { "reserved", {44, 4}, FIELD(PalRayTracingPipelineCreateInfo, reserved) }
    };

    StructInfo info = {0};
    info.name = "PalRayTracingPipelineCreateInfo";
    info.fields = fields;
    info.fieldCount = ARRAY_SIZE(fields);
    info.expected.align = 8;
    info.expected.size = 48;
    info.expected.padding = 0;
    info.actual = STRUCT(PalRayTracingPipelineCreateInfo);

    return dumpStruct(&info);
}

bool renderingLayoutInfoStruct(void)
{
    FieldInfo fields[] = {
        { "colorAttachmentsFormat", {0, 8}, FIELD(PalRenderingLayoutInfo, colorAttachmentsFormat) },
        { "colorAttachentCount", {8, 4}, FIELD(PalRenderingLayoutInfo, colorAttachentCount) },
        { "viewCount", {12, 4}, FIELD(PalRenderingLayoutInfo, viewCount) },
        { "sampleCount", {16, 4}, FIELD(PalRenderingLayoutInfo, sampleCount) },
        { "flags", {20, 4}, FIELD(PalRenderingLayoutInfo, flags) },
        { "depthStencilAttachmentFormat", {24, 4}, FIELD(PalRenderingLayoutInfo, depthStencilAttachmentFormat) },
        { "fragmentShadingRateAttachmentFormat", {28, 4}, FIELD(PalRenderingLayoutInfo, fragmentShadingRateAttachmentFormat) }
    };

    StructInfo info = {0};
    info.name = "PalRenderingLayoutInfo";
    info.fields = fields;
    info.fieldCount = ARRAY_SIZE(fields);
    info.expected.align = 8;
    info.expected.size = 32;
    info.expected.padding = 0;
    info.actual = STRUCT(PalRenderingLayoutInfo);

    return dumpStruct(&info);
}