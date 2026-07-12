
/**
    PAL - Prime Abstraction Layer
    Copyright (C) 2025
    Licensed under the Zlib license. See LICENSE file in root.
 */

#if PAL_HAS_VULKAN_BACKEND
#include "pal_vulkan.h"

#define max(a, b) (a > b) ? a : b

static VkBlendOp blendOpToVk(PalBlendOp op)
{
    switch (op) {
        case PAL_BLEND_OP_ADD:
            return VK_BLEND_OP_ADD;

        case PAL_BLEND_OP_SUBTRACT:
            return VK_BLEND_OP_SUBTRACT;

        case PAL_BLEND_OP_REVERSE_SUBTRACT:
            return VK_BLEND_OP_REVERSE_SUBTRACT;

        case PAL_BLEND_OP_MIN:
            return VK_BLEND_OP_MIN;

        case PAL_BLEND_OP_MAX:
            return VK_BLEND_OP_MAX;
    }

    return VK_BLEND_OP_ADD;
}

static VkBlendFactor blendFactorToVk(PalBlendFactor op)
{
    switch (op) {
        case PAL_BLEND_FACTOR_ZERO:
            return VK_BLEND_FACTOR_ZERO;

        case PAL_BLEND_FACTOR_ONE:
            return VK_BLEND_FACTOR_ONE;

        case PAL_BLEND_FACTOR_SRC_COLOR:
            return VK_BLEND_FACTOR_SRC_COLOR;

        case PAL_BLEND_FACTOR_ONE_MINUS_SRC_COLOR:
            return VK_BLEND_FACTOR_ONE_MINUS_SRC_COLOR;

        case PAL_BLEND_FACTOR_DST_COLOR:
            return VK_BLEND_FACTOR_DST_COLOR;

        case PAL_BLEND_FACTOR_ONE_MINUS_DST_COLOR:
            return VK_BLEND_FACTOR_ONE_MINUS_DST_COLOR;

        case PAL_BLEND_FACTOR_SRC_ALPHA:
            return VK_BLEND_FACTOR_SRC_ALPHA;

        case PAL_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA:
            return VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;

        case PAL_BLEND_FACTOR_DST_ALPHA:
            return VK_BLEND_FACTOR_DST_ALPHA;

        case PAL_BLEND_FACTOR_ONE_MINUS_DST_ALPHA:
            return VK_BLEND_FACTOR_ONE_MINUS_DST_ALPHA;

        case PAL_BLEND_FACTOR_CONSTANT_COLOR:
            return VK_BLEND_FACTOR_CONSTANT_COLOR;

        case PAL_BLEND_FACTOR_ONE_MINUS_CONSTANT_COLOR:
            return VK_BLEND_FACTOR_ONE_MINUS_CONSTANT_COLOR;

        case PAL_BLEND_FACTOR_CONSTANT_ALPHA:
            return VK_BLEND_FACTOR_CONSTANT_ALPHA;

        case PAL_BLEND_FACTOR_ONE_MINUS_CONSTANT_ALPHA:
            return VK_BLEND_FACTOR_ONE_MINUS_CONSTANT_ALPHA;
    }

    return VK_BLEND_FACTOR_ZERO;
}

static uint32_t getVertexTypeSizeVk(PalVertexType type)
{
    // count x sizeof type returned as size
    switch (type) {
        case PAL_VERTEX_TYPE_INT8_2:
        case PAL_VERTEX_TYPE_UINT8_2:
        case PAL_VERTEX_TYPE_INT8_2NORM:
        case PAL_VERTEX_TYPE_UINT8_2NORM: {
            return 2;
        }

        case PAL_VERTEX_TYPE_INT32:
        case PAL_VERTEX_TYPE_UINT32:
        case PAL_VERTEX_TYPE_INT8_4:
        case PAL_VERTEX_TYPE_INT8_4NORM:
        case PAL_VERTEX_TYPE_UINT8_4:
        case PAL_VERTEX_TYPE_UINT8_4NORM:
        case PAL_VERTEX_TYPE_INT16_2NORM:
        case PAL_VERTEX_TYPE_INT16_2:
        case PAL_VERTEX_TYPE_UINT16_2:
        case PAL_VERTEX_TYPE_UINT16_2NORM:
        case PAL_VERTEX_TYPE_FLOAT:
        case PAL_VERTEX_TYPE_HALF_FLOAT16_2: {
            return 4;
        }

        case PAL_VERTEX_TYPE_INT32_2:
        case PAL_VERTEX_TYPE_UINT32_2:
        case PAL_VERTEX_TYPE_INT16_4:
        case PAL_VERTEX_TYPE_UINT16_4:
        case PAL_VERTEX_TYPE_UINT16_4NORM:
        case PAL_VERTEX_TYPE_INT16_4NORM:
        case PAL_VERTEX_TYPE_FLOAT2:
        case PAL_VERTEX_TYPE_HALF_FLOAT16_4: {
            return 8;
        }

        case PAL_VERTEX_TYPE_INT32_3:
        case PAL_VERTEX_TYPE_UINT32_3:
        case PAL_VERTEX_TYPE_FLOAT3: {
            return 12;
        }

        case PAL_VERTEX_TYPE_INT32_4:
        case PAL_VERTEX_TYPE_UINT32_4:
        case PAL_VERTEX_TYPE_FLOAT4: {
            return 16;
        }
    }

    return 0;
}

PalResult PAL_CALL createPipelineLayoutVk(
    PalDevice* device,
    const PalPipelineLayoutCreateInfo* info,
    PalPipelineLayout** outLayout)
{
    VkResult result;
    DeviceVk* vkDevice = (DeviceVk*)device;
    PipelineLayoutVk* layout = nullptr;
    VkPushConstantRange pushConstantRange = {0};
    VkDescriptorSetLayout* descriptorLayouts = nullptr;
    
    layout = palAllocate(s_Vk.allocator, sizeof(PipelineLayoutVk), 0);
    if (!layout) {
        return PAL_RESULT_CODE_OUT_OF_MEMORY;
    }

    if (info->descriptorSetLayoutCount) {
        descriptorLayouts = palAllocate(
            s_Vk.allocator, 
            sizeof(VkDescriptorSetLayout) * info->descriptorSetLayoutCount, 
            0);

        if (!descriptorLayouts) {
            return PAL_RESULT_CODE_OUT_OF_MEMORY;
        }
    }

    for (int i = 0; i < info->descriptorSetLayoutCount; i++) {
        DescriptorSetLayoutVk* tmp = (DescriptorSetLayoutVk*)info->descriptorSetLayouts[i];
        descriptorLayouts[i] = tmp->handle;
    }

    VkPipelineLayoutCreateInfo createInfo = {0};
    createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    createInfo.setLayoutCount = info->descriptorSetLayoutCount;
    createInfo.pSetLayouts = descriptorLayouts;

    if (info->usePushConstant) {
        pushConstantRange.size = info->pushConstantInfo.size;
        pushConstantRange.stageFlags = vkDevice->shaderStages;
        pushConstantRange.offset = info->pushConstantInfo.offset;
        createInfo.pushConstantRangeCount = 1;
        createInfo.pPushConstantRanges = &pushConstantRange;
    }

    result = s_Vk.createPipelineLayout(
        vkDevice->handle,
        &createInfo,
        &s_Vk.vkAllocator,
        &layout->handle);

    if (info->descriptorSetLayoutCount) {
        palFree(s_Vk.allocator, descriptorLayouts);
    }

    if (result != VK_SUCCESS) {
        palFree(s_Vk.allocator, layout);
        return makeResultVk(result);
    }

    layout->device = vkDevice;
    *outLayout = (PalPipelineLayout*)layout;
    return PAL_RESULT_SUCCESS;
}

void PAL_CALL destroyPipelineLayoutVk(PalPipelineLayout* layout)
{
    PipelineLayoutVk* pipelineLayout = (PipelineLayoutVk*)layout;
    s_Vk.destroyPipelineLayout(
        pipelineLayout->device->handle,
        pipelineLayout->handle,
        &s_Vk.vkAllocator);

    palFree(s_Vk.allocator, layout);
}

PalResult PAL_CALL createGraphicsPipelineVk(
    PalDevice* device,
    const PalGraphicsPipelineCreateInfo* info,
    PalPipeline** outPipeline)
{
    VkResult result;
    PipelineVk* pipeline = nullptr;
    DeviceVk* vkDevice = (DeviceVk*)device;
    PipelineLayoutVk* layout = (PipelineLayoutVk*)info->pipelineLayout;

    VkPipelineShaderStageCreateInfo* shaderStages = nullptr;
    VkDynamicState dynamicStates[16];
    VkVertexInputBindingDescription* bindingDescs = nullptr;
    VkVertexInputAttributeDescription* attribDescs = nullptr;
    VkPipelineColorBlendAttachmentState* blendattachments = nullptr;

    VkPipelineVertexInputStateCreateInfo vertexInputState = {0};
    vertexInputState.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;

    VkPipelineInputAssemblyStateCreateInfo inputAssemblyState = {0};
    inputAssemblyState.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;

    VkPipelineDynamicStateCreateInfo dynamicState = {0};
    dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;

    VkPipelineRasterizationStateCreateInfo rasterizerState = {0};
    rasterizerState.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;

    VkPipelineMultisampleStateCreateInfo multisampleState = {0};
    multisampleState.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;

    VkPipelineDepthStencilStateCreateInfo depthStencilState = {0};
    depthStencilState.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;

    VkPipelineColorBlendStateCreateInfo colorBlendState = {0};
    colorBlendState.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;

    VkPipelineTessellationStateCreateInfo tessellationState = {0};
    tessellationState.sType = VK_STRUCTURE_TYPE_PIPELINE_TESSELLATION_STATE_CREATE_INFO;

    VkPipelineViewportStateCreateInfo viewportState = {0};
    viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;

    VkPipelineFragmentShadingRateStateCreateInfoKHR fsrState = {0};
    fsrState.sType = VK_STRUCTURE_TYPE_PIPELINE_FRAGMENT_SHADING_RATE_STATE_CREATE_INFO_KHR;

    VkPipelineRenderingCreateInfoKHR dynRendering = {0};
    dynRendering.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO_KHR;

    VkGraphicsPipelineCreateInfo createInfo = {0};
    createInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    createInfo.renderPass = VK_NULL_HANDLE;
    createInfo.layout = layout->handle;

    // find the total number of shader stages
    uint32_t stageCount = 0;
    for (int i = 0; i < info->shaderCount; i++) {
        ShaderVk* shader = (ShaderVk*)info->shaders[i];
        stageCount += shader->entryCount;
    }

    pipeline = palAllocate(s_Vk.allocator, sizeof(PipelineVk), 0);
    shaderStages = palAllocate(
        s_Vk.allocator, 
        sizeof(VkPipelineShaderStageCreateInfo) * stageCount, 
        0);

    if (!pipeline || !shaderStages) {
        return PAL_RESULT_CODE_OUT_OF_MEMORY;
    }

    // shaders
    uint32_t stageIndex = 0;
    memset(shaderStages, 0, sizeof(VkPipelineShaderStageCreateInfo) * stageCount);
    for (int i = 0; i < info->shaderCount; i++) {
        ShaderVk* tmp = (ShaderVk*)info->shaders[i];

        for (int j = 0; j < tmp->entryCount; j++) {
            ShaderEntry* entry = &tmp->entries[j];
            VkPipelineShaderStageCreateInfo* stageInfo = &shaderStages[stageIndex++];

            if (entry->patchControlPoints) {
                tessellationState.patchControlPoints = entry->patchControlPoints;
                createInfo.pTessellationState = &tessellationState;

                if (info->topology != PAL_PRIMITIVE_TOPOLOGY_PATCH) {
                    palFree(s_Vk.allocator, pipeline);
                    return PAL_RESULT_CODE_INVALID_OPERATION;
                }
            }

            stageInfo->sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
            stageInfo->module = tmp->handle;
            stageInfo->pName = entry->entryName;
            stageInfo->stage = entry->stage;
        }
    }

    createInfo.stageCount = stageCount;
    createInfo.pStages = shaderStages;

    // Vertex input state
    // get the max size of vertex attributes in all layouts
    uint32_t vertexCount = 0;
    for (int i = 0; i < info->vertexLayoutCount; i++) {
        PalVertexLayout* layout = &info->vertexLayouts[i];
        vertexCount += layout->attributeCount;
    }

    if (vertexCount) {
        uint32_t tmpBindingSize = sizeof(VkVertexInputBindingDescription) * info->vertexLayoutCount;
        uint32_t tmpAttribSize = sizeof(VkVertexInputAttributeDescription) * vertexCount;
        bindingDescs = palAllocate(s_Vk.allocator, tmpBindingSize, 0);
        attribDescs = palAllocate(s_Vk.allocator, tmpAttribSize, 0);
        if (!bindingDescs || !attribDescs) {
            palFree(s_Vk.allocator, pipeline);
            return PAL_RESULT_CODE_OUT_OF_MEMORY;
        }

        uint32_t location = 0;
        for (int i = 0; i < info->vertexLayoutCount; i++) {
            PalVertexLayout* layout = &info->vertexLayouts[i];
            VkVertexInputBindingDescription* bindingDesc = &bindingDescs[i];

            bindingDesc->binding = layout->binding;
            if (layout->type == PAL_VERTEX_LAYOUT_TYPE_PER_INSTANCE) {
                bindingDesc->inputRate = VK_VERTEX_INPUT_RATE_INSTANCE;
            } else {
                bindingDesc->inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
            }

            // find the stride and offset of the layout
            bindingDesc->stride = 0;
            uint32_t offset = 0;
            for (int j = 0; j < layout->attributeCount; j++) {
                PalVertexAttribute* vertexAttrib = &layout->attributes[j];
                VkVertexInputAttributeDescription* attribDesc = &attribDescs[j];

                attribDesc->format = vertexTypeToVk(vertexAttrib->type);
                attribDesc->binding = bindingDesc->binding;
                attribDesc->location = location++;

                // build offsets and stride
                uint32_t size = getVertexTypeSizeVk(vertexAttrib->type);
                attribDesc->offset = offset;
                offset += size;
                bindingDesc->stride += size;
            }
        }

        vertexInputState.pVertexAttributeDescriptions = attribDescs;
        vertexInputState.vertexAttributeDescriptionCount = vertexCount;
        vertexInputState.pVertexBindingDescriptions = bindingDescs;
        vertexInputState.vertexBindingDescriptionCount = info->vertexLayoutCount;
    }
    createInfo.pVertexInputState = &vertexInputState;

    // Input assembly
    VkPrimitiveTopology topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    switch (info->topology) {
        case PAL_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST: {
            topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
            break;
        }

        case PAL_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP: {
            topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP;
            break;
        }

        case PAL_PRIMITIVE_TOPOLOGY_LINE_LIST: {
            topology = VK_PRIMITIVE_TOPOLOGY_LINE_LIST;
            break;
        }

        case PAL_PRIMITIVE_TOPOLOGY_LINE_STRIP: {
            topology = VK_PRIMITIVE_TOPOLOGY_LINE_STRIP;
            break;
        }

        case PAL_PRIMITIVE_TOPOLOGY_POINT_LIST: {
            topology = VK_PRIMITIVE_TOPOLOGY_POINT_LIST;
            break;
        }
    }
    inputAssemblyState.topology = topology;
    inputAssemblyState.primitiveRestartEnable = info->primitiveRestartEnable;
    createInfo.pInputAssemblyState = &inputAssemblyState;

    // Dynamic states
    uint32_t dynCount = 0;
    dynamicStates[dynCount++] = VK_DYNAMIC_STATE_VIEWPORT;
    dynamicStates[dynCount++] = VK_DYNAMIC_STATE_SCISSOR;
    dynamicStates[dynCount++] = VK_DYNAMIC_STATE_LINE_WIDTH;
    dynamicStates[dynCount++] = VK_DYNAMIC_STATE_BLEND_CONSTANTS;
    dynamicStates[dynCount++] = VK_DYNAMIC_STATE_DEPTH_BIAS;
    dynamicStates[dynCount++] = VK_DYNAMIC_STATE_STENCIL_REFERENCE;

    if (vkDevice->features & PAL_ADAPTER_FEATURE_DYNAMIC_CULL_MODE) {
        dynamicStates[dynCount++] = VK_DYNAMIC_STATE_CULL_MODE_EXT;
    }

    if (vkDevice->features & PAL_ADAPTER_FEATURE_DYNAMIC_FRONT_FACE) {
        dynamicStates[dynCount++] = VK_DYNAMIC_STATE_FRONT_FACE_EXT;
    }

    if (vkDevice->features & PAL_ADAPTER_FEATURE_DYNAMIC_PRIMITIVE_TOPOLOGY) {
        dynamicStates[dynCount++] = VK_DYNAMIC_STATE_PRIMITIVE_TOPOLOGY_EXT;
    }

    if (vkDevice->features & PAL_ADAPTER_FEATURE_DYNAMIC_DEPTH_TEST_ENABLE) {
        dynamicStates[dynCount++] = VK_DYNAMIC_STATE_DEPTH_TEST_ENABLE_EXT;
    }

    if (vkDevice->features & PAL_ADAPTER_FEATURE_DYNAMIC_DEPTH_WRITE_ENABLE) {
        dynamicStates[dynCount++] = VK_DYNAMIC_STATE_DEPTH_WRITE_ENABLE_EXT;
    }

    if (vkDevice->features & PAL_ADAPTER_FEATURE_DYNAMIC_STENCIL_OP) {
        dynamicStates[dynCount++] = VK_DYNAMIC_STATE_STENCIL_OP_EXT;
    }

    if (info->fragmentShadingRateState) {
        dynamicStates[dynCount++] = VK_DYNAMIC_STATE_FRAGMENT_SHADING_RATE_KHR;
    }

    dynamicState.dynamicStateCount = dynCount;
    dynamicState.pDynamicStates = dynamicStates;
    createInfo.pDynamicState = &dynamicState;

    // Rasterizer state
    rasterizerState.frontFace = VK_FRONT_FACE_CLOCKWISE;
    if (info->rasterizerState) {
        PalRasterizerState* state = info->rasterizerState;
        if (state->cullMode == PAL_CULL_MODE_NONE) {
            rasterizerState.cullMode = VK_CULL_MODE_NONE;

        } else if (state->cullMode == PAL_CULL_MODE_BACK) {
            rasterizerState.cullMode = VK_CULL_MODE_BACK_BIT;

        } else if (state->cullMode == PAL_CULL_MODE_FRONT) {
            rasterizerState.cullMode = VK_CULL_MODE_FRONT_BIT;
        }

        if (state->polygonMode == PAL_POLYGON_MODE_FILL) {
            rasterizerState.polygonMode = VK_POLYGON_MODE_FILL;

        } else {
            rasterizerState.polygonMode = VK_POLYGON_MODE_LINE;
        }

        if (state->frontFace == PAL_FRONT_FACE_CLOCKWISE) {
            rasterizerState.frontFace = VK_FRONT_FACE_CLOCKWISE;

        } else {
            rasterizerState.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
        }

        rasterizerState.depthBiasEnable = state->enableDepthBias;
        rasterizerState.depthClampEnable = state->enableDepthClamp;
        rasterizerState.depthBiasConstantFactor = state->depthBiasConstant;
        rasterizerState.depthBiasSlopeFactor = state->depthBiasSlope;
        rasterizerState.depthBiasClamp = state->depthBiasClamp;
    }

    rasterizerState.lineWidth = 1.0f;
    createInfo.pRasterizationState = &rasterizerState;

    // Multisample state
    uint32_t sampleMask[2] = {0}; // PAL supports upto 64 samples
    multisampleState.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
    multisampleState.pSampleMask = nullptr;
    if (info->multisampleState) {
        PalMultisampleState* state = info->multisampleState;
        multisampleState.alphaToCoverageEnable = state->enableAlphaToCoverage;
        multisampleState.minSampleShading = state->minSampleShading;
        multisampleState.sampleShadingEnable = state->enableSampleShading;
        multisampleState.rasterizationSamples = samplesToVk(state->sampleCount);

        // clang-format off
        if (state->sampleMask) {
            if (state->sampleCount == PAL_SAMPLE_COUNT_1  || 
                state->sampleCount == PAL_SAMPLE_COUNT_2  ||
                state->sampleCount == PAL_SAMPLE_COUNT_4  ||
                state->sampleCount == PAL_SAMPLE_COUNT_8  ||
                state->sampleCount == PAL_SAMPLE_COUNT_16 ||
                state->sampleCount == PAL_SAMPLE_COUNT_32) {
                sampleMask[0] = (uint32_t)(state->sampleMask & 0xFFFFFFFFULL);

            } else {
                sampleMask[0] = (uint32_t)(state->sampleMask & 0xFFFFFFFFULL);
                sampleMask[1] = (uint32_t)((state->sampleMask >> 32) & 0xFFFFFFFFULL);
            }
            multisampleState.pSampleMask = sampleMask;

        } else {
            multisampleState.pSampleMask = nullptr;
        }
        // clang-format on
    }
    createInfo.pMultisampleState = &multisampleState;

    // Depth stencil state
    if (info->depthStencilState) {
        PalDepthStencilState* state = info->depthStencilState;
        PalStencilOpState* back = &state->backStencilOpState;
        PalStencilOpState* front = &state->frontStencilOpState;

        VkStencilOpState* vkBack = &depthStencilState.back;
        VkStencilOpState* vkFront = &depthStencilState.front;

        vkBack->compareOp = compareOpToVk(back->compareOp);
        vkBack->depthFailOp = stencilOpToVk(back->depthFailOp);
        vkBack->failOp = stencilOpToVk(back->failOp);
        vkBack->passOp = stencilOpToVk(back->passOp);

        vkFront->compareOp = compareOpToVk(front->compareOp);
        vkFront->depthFailOp = stencilOpToVk(front->depthFailOp);
        vkFront->failOp = stencilOpToVk(front->failOp);
        vkFront->passOp = stencilOpToVk(front->passOp);

        depthStencilState.depthCompareOp = compareOpToVk(state->compareOp);
        depthStencilState.depthTestEnable = state->enableDepthTest;
        depthStencilState.depthWriteEnable = state->enableDepthWrite;
        depthStencilState.stencilTestEnable = state->enableStencilTest;
    }
    createInfo.pDepthStencilState = &depthStencilState;

    // Color blend state
    if (info->colorBlendAttachmentCount) {
        uint32_t count = info->colorBlendAttachmentCount;
        uint32_t size = sizeof(VkPipelineColorBlendAttachmentState) * count;
        blendattachments = palAllocate(s_Vk.allocator, size, 0);
        if (!blendattachments) {
            return PAL_RESULT_CODE_OUT_OF_MEMORY;
        }

        for (int i = 0; i < count; i++) {
            VkPipelineColorBlendAttachmentState* tmp = &blendattachments[i];
            PalColorBlendAttachment* desc = &info->colorBlendAttachments[i];

            tmp->blendEnable = desc->enableBlend;
            tmp->alphaBlendOp = blendOpToVk(desc->alphaBlendOp);
            tmp->colorBlendOp = blendOpToVk(desc->colorBlendOp);

            tmp->srcAlphaBlendFactor = blendFactorToVk(desc->srcAlphaBlendFactor);
            tmp->srcColorBlendFactor = blendFactorToVk(desc->srcColorBlendFactor);

            tmp->dstAlphaBlendFactor = blendFactorToVk(desc->dstAlphaBlendFactor);
            tmp->dstColorBlendFactor = blendFactorToVk(desc->dstColorBlendFactor);

            // blend color write mask
            tmp->colorWriteMask = 0;
            if (desc->colorWriteMask & PAL_COLOR_MASK_RED) {
                tmp->colorWriteMask |= VK_COLOR_COMPONENT_R_BIT;
            }

            if (desc->colorWriteMask & PAL_COLOR_MASK_GREEN) {
                tmp->colorWriteMask |= VK_COLOR_COMPONENT_G_BIT;
            }

            if (desc->colorWriteMask & PAL_COLOR_MASK_BLUE) {
                tmp->colorWriteMask |= VK_COLOR_COMPONENT_B_BIT;
            }

            if (desc->colorWriteMask & PAL_COLOR_MASK_ALPHA) {
                tmp->colorWriteMask |= VK_COLOR_COMPONENT_A_BIT;
            }
        }

        colorBlendState.attachmentCount = count;
        colorBlendState.pAttachments = blendattachments;
    }
    createInfo.pColorBlendState = &colorBlendState;

    // viewport state
    viewportState.viewportCount = 1;
    viewportState.scissorCount = 1;
    createInfo.pViewportState = &viewportState;

    // Fragment shading rate
    if (info->fragmentShadingRateState) {
        PalFragmentShadingRateState* state = info->fragmentShadingRateState;
        for (int i = 0; i < 2; i++) {
            VkFragmentShadingRateCombinerOpKHR combinerOp;
            combinerOp = combinerOpsToVk(state->combinerOps[i]);
            fsrState.combinerOps[i] = combinerOp;
        }

        VkExtent2D size = getShadingRateSizeVk(state->rate);
        fsrState.fragmentSize = size;
        createInfo.pNext = &fsrState;
    }

    // layout info
    VkFormat format = VK_FORMAT_UNDEFINED;
    VkFormat* colorAttachments = nullptr;
    PalRenderingLayoutInfo* renderingLayout = info->renderingLayout;

    colorAttachments = palAllocate(
        s_Vk.allocator, 
        sizeof(VkFormat) * renderingLayout->colorAttachentCount, 
        0);

    if (!colorAttachments) {
        return PAL_RESULT_CODE_OUT_OF_MEMORY;
    }

    // color attachments
    for (int i = 0; i < renderingLayout->colorAttachentCount; i++) {
        format = formatToVk(renderingLayout->colorAttachmentsFormat[i]);
        colorAttachments[i] = format;
    }
    dynRendering.colorAttachmentCount = renderingLayout->colorAttachentCount;
    dynRendering.pColorAttachmentFormats = colorAttachments;

    // depth stencil attachment
    format = formatToVk(renderingLayout->depthStencilAttachmentFormat);
    dynRendering.depthAttachmentFormat = format;
    dynRendering.stencilAttachmentFormat = format;

    if (renderingLayout->viewCount == 1) {
        dynRendering.viewMask = 0;
    } else {
        dynRendering.viewMask = (1 << renderingLayout->viewCount) - 1;
    }
    createInfo.pNext = &dynRendering;

    result = s_Vk.createGraphicsPipeline(
        vkDevice->handle,
        0,
        1,
        &createInfo,
        &s_Vk.vkAllocator,
        &pipeline->handle);

    if (result != VK_SUCCESS) {
        palFree(s_Vk.allocator, pipeline);
        return makeResultVk(result);
    }

    palFree(s_Vk.allocator, colorAttachments);
    palFree(s_Vk.allocator, shaderStages);
    if (info->vertexLayoutCount) {
        palFree(s_Vk.allocator, bindingDescs);
        palFree(s_Vk.allocator, attribDescs);
    }

    if (info->colorBlendAttachmentCount) {
        palFree(s_Vk.allocator, blendattachments);
    }

    pipeline->bindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    pipeline->device = vkDevice;
    pipeline->layout = layout->handle;
    *outPipeline = (PalPipeline*)pipeline;
    return PAL_RESULT_SUCCESS;
}

PalResult PAL_CALL createComputePipelineVk(
    PalDevice* device,
    const PalComputePipelineCreateInfo* info,
    PalPipeline** outPipeline)
{
    DeviceVk* vkDevice = (DeviceVk*)device;
    PipelineLayoutVk* layout = (PipelineLayoutVk*)info->pipelineLayout;
    ShaderVk* shader = (ShaderVk*)info->computeShader;
    PipelineVk* pipeline = nullptr;

    pipeline = palAllocate(s_Vk.allocator, sizeof(PipelineVk), 0);
    if (!pipeline) {
        return PAL_RESULT_CODE_OUT_OF_MEMORY;
    }

    VkComputePipelineCreateInfo createInfo = {0};
    createInfo.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
    createInfo.layout = layout->handle;

    createInfo.stage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    createInfo.stage.module = shader->handle;
    createInfo.stage.stage = shader->entries[0].stage;
    createInfo.stage.pName = shader->entries[0].entryName;

    VkResult result = s_Vk.createComputePipeline(
        vkDevice->handle,
        nullptr,
        1,
        &createInfo,
        &s_Vk.vkAllocator,
        &pipeline->handle);

    if (result != VK_SUCCESS) {
        palFree(s_Vk.allocator, pipeline);
        return makeResultVk(result);
    }

    pipeline->bindPoint = VK_PIPELINE_BIND_POINT_COMPUTE;
    pipeline->device = vkDevice;
    pipeline->layout = layout->handle;
    *outPipeline = (PalPipeline*)pipeline;
    return PAL_RESULT_SUCCESS;
}

PalResult PAL_CALL createRayTracingPipelineVk(
    PalDevice* device,
    const PalRayTracingPipelineCreateInfo* info,
    PalPipeline** outPipeline)
{
    VkResult result;
    DeviceVk* vkDevice = (DeviceVk*)device;
    PipelineLayoutVk* layout = (PipelineLayoutVk*)info->pipelineLayout;
    PipelineVk* pipeline = nullptr;
    VkPipelineShaderStageCreateInfo* shaderStages = nullptr; 
    VkRayTracingShaderGroupCreateInfoKHR* groups = nullptr;

    if (info->maxPayloadSize > vkDevice->limits.maxPayloadSize) {
        return PAL_RESULT_CODE_INVALID_ARGUMENT;
    }

    VkRayTracingPipelineCreateInfoKHR createInfo = {0};
    createInfo.sType = VK_STRUCTURE_TYPE_RAY_TRACING_PIPELINE_CREATE_INFO_KHR;

    // find the total number of shader stages
    uint32_t stageCount = 0;
    for (int i = 0; i < info->shaderCount; i++) {
        ShaderVk* shader = (ShaderVk*)info->shaders[i];
        stageCount += shader->entryCount;
    }

    pipeline = palAllocate(s_Vk.allocator, sizeof(PipelineVk), 0);
    groups = palAllocate(
        s_Vk.allocator, 
        sizeof(VkRayTracingShaderGroupCreateInfoKHR) * info->shaderGroupCount, 
        0);

    shaderStages = palAllocate(
        s_Vk.allocator, 
        sizeof(VkPipelineShaderStageCreateInfo) * stageCount, 
        0);

    if (!pipeline || !groups || !shaderStages) {
        return PAL_RESULT_CODE_OUT_OF_MEMORY;
    }

    memset(pipeline, 0, sizeof(PipelineVk));

    // shaders
    uint32_t stageIndex = 0;
    memset(shaderStages, 0, sizeof(VkPipelineShaderStageCreateInfo) * stageCount);
    for (int i = 0; i < info->shaderCount; i++) {
        ShaderVk* tmp = (ShaderVk*)info->shaders[i];

        for (int j = 0; j < tmp->entryCount; j++) {
            ShaderEntry* entry = &tmp->entries[j];
            VkPipelineShaderStageCreateInfo* stageInfo = &shaderStages[stageIndex++];

            stageInfo->sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
            stageInfo->module = tmp->handle;
            stageInfo->pName = entry->entryName;
            stageInfo->stage = entry->stage;
        }
    }

    createInfo.stageCount = stageCount;
    createInfo.pStages = shaderStages;

    ShaderBindingTableInfo* sbtInfo = &pipeline->sbtInfo;
    for (int i = 0; i < info->shaderGroupCount; i++) {
        VkRayTracingShaderGroupCreateInfoKHR* group = &groups[i];
        PalRayTracingShaderGroupCreateInfo* tmp = &info->shaderGroups[i];

        group->sType = VK_STRUCTURE_TYPE_RAY_TRACING_SHADER_GROUP_CREATE_INFO_KHR;
        group->pShaderGroupCaptureReplayHandle = nullptr;
        group->pNext = nullptr;

        if (tmp->type == PAL_RAY_TRACING_SHADER_GROUP_TYPE_GENERAL) {
            // check if its raygen, miss or callable
            ShaderVk* shader = (ShaderVk*)info->shaders[tmp->generalShaderIndex];
            ShaderEntry* entry = &shader->entries[tmp->generalShaderEntryIndex];

            switch (entry->stage) {
                case VK_SHADER_STAGE_RAYGEN_BIT_KHR: {
                    sbtInfo->raygenCount++;
                    sbtInfo->raygenDataSize = max(sbtInfo->raygenDataSize, tmp->maxDataSize);
                    break;
                }

                case VK_SHADER_STAGE_MISS_BIT_KHR: {
                    sbtInfo->missCount++;
                    sbtInfo->missDataSize = max(sbtInfo->missDataSize, tmp->maxDataSize);
                    break;
                }

                case VK_SHADER_STAGE_CALLABLE_BIT_KHR: {
                    sbtInfo->callableCount++;
                    sbtInfo->callableDataSize = max(sbtInfo->callableDataSize, tmp->maxDataSize);
                    break;
                }
            }

            group->type = VK_RAY_TRACING_SHADER_GROUP_TYPE_GENERAL_KHR;
            group->generalShader = tmp->generalShaderEntryIndex;
            group->anyHitShader = VK_SHADER_UNUSED_KHR;
            group->closestHitShader = VK_SHADER_UNUSED_KHR;
            group->intersectionShader = VK_SHADER_UNUSED_KHR;

        } else {
            sbtInfo->hitCount++;
            sbtInfo->hitDataSize = max(sbtInfo->hitDataSize, tmp->maxDataSize);

            group->generalShader = VK_SHADER_UNUSED_KHR;
            if (tmp->anyHitShaderIndex != PAL_UNUSED_SHADER_INDEX) {
                group->anyHitShader = tmp->anyHitShaderEntryIndex;

            } else {
                group->anyHitShader = VK_SHADER_UNUSED_KHR;
            }

            if (tmp->closestHitShaderIndex != PAL_UNUSED_SHADER_INDEX) {
                group->closestHitShader = tmp->closestHitShaderEntryIndex;

            } else {
                group->closestHitShader = VK_SHADER_UNUSED_KHR;
            }

            if (tmp->intersectionShaderIndex!= PAL_UNUSED_SHADER_INDEX) {
                group->intersectionShader = tmp->intersectionShaderEntryIndex;

            } else {
                group->intersectionShader = VK_SHADER_UNUSED_KHR;
            }

            if (tmp->type == PAL_RAY_TRACING_SHADER_GROUP_TYPE_TRIANGLES_HIT) {
                group->type = VK_RAY_TRACING_SHADER_GROUP_TYPE_TRIANGLES_HIT_GROUP_KHR;

            } else {
                group->type = VK_RAY_TRACING_SHADER_GROUP_TYPE_PROCEDURAL_HIT_GROUP_KHR;
            }
        }
    }

    createInfo.pGroups = groups;
    createInfo.groupCount = info->shaderGroupCount;
    createInfo.maxPipelineRayRecursionDepth = info->maxRecursionDepth;
    createInfo.layout = layout->handle;

    result = vkDevice->createRayTracingPipeline(
        vkDevice->handle,
        nullptr,
        nullptr,
        1,
        &createInfo,
        &s_Vk.vkAllocator,
        &pipeline->handle);

    palFree(s_Vk.allocator, groups);
    palFree(s_Vk.allocator, shaderStages);
    if (result != VK_SUCCESS) {
        palFree(s_Vk.allocator, pipeline);
        return makeResultVk(result);
    }

    pipeline->bindPoint = VK_PIPELINE_BIND_POINT_RAY_TRACING_KHR;
    pipeline->device = vkDevice;
    pipeline->layout = layout->handle;
    *outPipeline = (PalPipeline*)pipeline;
    return PAL_RESULT_SUCCESS;
}

void PAL_CALL destroyPipelineVk(PalPipeline* pipeline)
{
    PipelineVk* vkPipeline = (PipelineVk*)pipeline;
    s_Vk.destroyPipeline(vkPipeline->device->handle, vkPipeline->handle, &s_Vk.vkAllocator);
    palFree(s_Vk.allocator, pipeline);
}

#endif // PAL_HAS_VULKAN_BACKEND