
/**
    PAL - Prime Abstraction Layer
    Copyright (C) 2025
    Licensed under the Zlib license. See LICENSE file in root.
 */

#if PAL_HAS_D3D12_BACKEND
#include "pal_d3d12.h"

#define align(v, a) (v + a - 1) & ~(a - 1)

#if INTPTR_MAX == INT64_MAX
#define PTR_SIZE 8
#else
#define PTR_SIZE 4
#endif // INTPTR_MAX

#if defined(_MSC_VER)
#define ALIGN_STREAM __declspec(align(PTR_SIZE))
#elif defined(__GNUC__) || defined(__clang__)
#define ALIGN_STREAM __attribute__((aligned(PTR_SIZE)))
#else
#define ALIGN_STREAM
#endif // _MSC_VER

typedef ALIGN_STREAM struct {
    D3D12_PIPELINE_STATE_SUBOBJECT_TYPE type;
    ID3D12RootSignature* root;
} RootSignatureStream;

typedef ALIGN_STREAM struct {
    D3D12_PIPELINE_STATE_SUBOBJECT_TYPE type;
    D3D12_INPUT_LAYOUT_DESC desc;
} InputLayoutStream;

typedef ALIGN_STREAM struct {
    D3D12_PIPELINE_STATE_SUBOBJECT_TYPE type;
    D3D12_PRIMITIVE_TOPOLOGY_TYPE topology;
} TopologyStream;

typedef ALIGN_STREAM struct {
    D3D12_PIPELINE_STATE_SUBOBJECT_TYPE type;
    D3D12_INDEX_BUFFER_STRIP_CUT_VALUE value;
} IBStripCutStream;

typedef ALIGN_STREAM struct {
    D3D12_PIPELINE_STATE_SUBOBJECT_TYPE type;
    D3D12_RASTERIZER_DESC desc;
} RasterizerStream;

typedef ALIGN_STREAM struct {
    D3D12_PIPELINE_STATE_SUBOBJECT_TYPE type;
    DXGI_SAMPLE_DESC desc;
} SampleDescStream;

typedef ALIGN_STREAM struct {
    D3D12_PIPELINE_STATE_SUBOBJECT_TYPE type;
    UINT mask;
} SampleMaskStream;

typedef ALIGN_STREAM struct {
    D3D12_PIPELINE_STATE_SUBOBJECT_TYPE type;
    D3D12_DEPTH_STENCIL_DESC desc;
} DepthStencilStream;

typedef ALIGN_STREAM struct {
    D3D12_PIPELINE_STATE_SUBOBJECT_TYPE type;
    D3D12_BLEND_DESC desc;
} BlendStream;

typedef ALIGN_STREAM struct {
    D3D12_PIPELINE_STATE_SUBOBJECT_TYPE type;
    struct D3D12_RT_FORMAT_ARRAY data;
} RTVStream;

typedef ALIGN_STREAM struct {
    D3D12_PIPELINE_STATE_SUBOBJECT_TYPE type;
    DXGI_FORMAT format;
} DSVStream;

typedef ALIGN_STREAM struct {
    D3D12_PIPELINE_STATE_SUBOBJECT_TYPE type;
    D3D12_SHADER_BYTECODE desc;
} ShaderStream;

typedef ALIGN_STREAM struct {
    D3D12_PIPELINE_STATE_SUBOBJECT_TYPE type;
    D3D12_VIEW_INSTANCING_DESC desc;
} ViewInstancingStream;

typedef struct {
    RootSignatureStream layout;
    InputLayoutStream inputLayout;
    TopologyStream topology;
    IBStripCutStream ibStripCut;
    RasterizerStream rasterizer;
    SampleDescStream sampleDesc;
    SampleMaskStream sampleMask;
    DepthStencilStream depthStencil;
    BlendStream blend;
    RTVStream RTV;
    DSVStream DSV;
    ViewInstancingStream viewInstancing;
    ShaderStream shaders[7]; // 7 shader types for graphics pipeline
} GraphicsPipelineStreamDesc;

static D3D12_STENCIL_OP stencilOpToD3D12(PalStencilOp op)
{
    switch (op) {
        case PAL_STENCIL_OP_KEEP:
            return D3D12_STENCIL_OP_KEEP;

        case PAL_STENCIL_OP_ZERO:
            return D3D12_STENCIL_OP_ZERO;

        case PAL_STENCIL_OP_REPLACE:
            return D3D12_STENCIL_OP_REPLACE;

        case PAL_STENCIL_OP_INCREMENT_AND_CLAMP:
            return D3D12_STENCIL_OP_INCR_SAT;

        case PAL_STENCIL_OP_DECREMENT_AND_CLAMP:
            return D3D12_STENCIL_OP_DECR_SAT;

        case PAL_STENCIL_OP_INVERT:
            return D3D12_STENCIL_OP_INVERT;

        case PAL_STENCIL_OP_INCREMENT_AND_WRAP:
            return D3D12_STENCIL_OP_INCR;

        case PAL_STENCIL_OP_DECREMENT_AND_WRAP:
            return D3D12_STENCIL_OP_DECR;
    }

    return D3D12_STENCIL_OP_KEEP;
}

static D3D12_BLEND_OP blendOpToD3D12(PalBlendOp op)
{
    switch (op) {
        case PAL_BLEND_OP_ADD:
            return D3D12_BLEND_OP_ADD;

        case PAL_BLEND_OP_SUBTRACT:
            return D3D12_BLEND_OP_SUBTRACT;

        case PAL_BLEND_OP_REVERSE_SUBTRACT:
            return D3D12_BLEND_OP_REV_SUBTRACT;

        case PAL_BLEND_OP_MIN:
            return D3D12_BLEND_OP_MIN;

        case PAL_BLEND_OP_MAX:
            return D3D12_BLEND_OP_MAX;
    }

    return D3D12_BLEND_OP_ADD;
}

static D3D12_BLEND blendFactorToD3D12(PalBlendFactor op)
{
    switch (op) {
        case PAL_BLEND_FACTOR_ZERO:
            return D3D12_BLEND_ZERO;

        case PAL_BLEND_FACTOR_ONE:
            return D3D12_BLEND_ONE;

        case PAL_BLEND_FACTOR_SRC_COLOR:
            return D3D12_BLEND_SRC_COLOR;

        case PAL_BLEND_FACTOR_ONE_MINUS_SRC_COLOR:
            return D3D12_BLEND_INV_SRC_COLOR;

        case PAL_BLEND_FACTOR_DST_COLOR:
            return D3D12_BLEND_DEST_COLOR;

        case PAL_BLEND_FACTOR_ONE_MINUS_DST_COLOR:
            return D3D12_BLEND_INV_DEST_COLOR;

        case PAL_BLEND_FACTOR_SRC_ALPHA:
            return D3D12_BLEND_SRC_ALPHA;

        case PAL_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA:
            return D3D12_BLEND_INV_SRC_ALPHA;

        case PAL_BLEND_FACTOR_DST_ALPHA:
            return D3D12_BLEND_DEST_ALPHA;

        case PAL_BLEND_FACTOR_ONE_MINUS_DST_ALPHA:
            return D3D12_BLEND_INV_DEST_ALPHA;

        case PAL_BLEND_FACTOR_CONSTANT_COLOR:
        case PAL_BLEND_FACTOR_CONSTANT_ALPHA:
            return D3D12_BLEND_BLEND_FACTOR;

        case PAL_BLEND_FACTOR_ONE_MINUS_CONSTANT_COLOR:
        case PAL_BLEND_FACTOR_ONE_MINUS_CONSTANT_ALPHA:
            return D3D12_BLEND_INV_BLEND_FACTOR;
    }

    return D3D12_BLEND_ZERO;
}

static D3D_PRIMITIVE_TOPOLOGY getPatchTopology(uint32_t patch)
{
    switch (patch) {
        case 1:
            return D3D_PRIMITIVE_TOPOLOGY_1_CONTROL_POINT_PATCHLIST;

        case 2:
            return D3D_PRIMITIVE_TOPOLOGY_2_CONTROL_POINT_PATCHLIST;

        case 3:
            return D3D_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST;

        case 4:
            return D3D_PRIMITIVE_TOPOLOGY_4_CONTROL_POINT_PATCHLIST;

        case 5:
            return D3D_PRIMITIVE_TOPOLOGY_5_CONTROL_POINT_PATCHLIST;

        case 6:
            return D3D_PRIMITIVE_TOPOLOGY_6_CONTROL_POINT_PATCHLIST;

        case 7:
            return D3D_PRIMITIVE_TOPOLOGY_7_CONTROL_POINT_PATCHLIST;

        case 8:
            return D3D_PRIMITIVE_TOPOLOGY_8_CONTROL_POINT_PATCHLIST;

        case 9:
            return D3D_PRIMITIVE_TOPOLOGY_9_CONTROL_POINT_PATCHLIST;

        case 10:
            return D3D_PRIMITIVE_TOPOLOGY_10_CONTROL_POINT_PATCHLIST;

        case 11:
            return D3D_PRIMITIVE_TOPOLOGY_11_CONTROL_POINT_PATCHLIST;

        case 12:
            return D3D_PRIMITIVE_TOPOLOGY_12_CONTROL_POINT_PATCHLIST;

        case 13:
            return D3D_PRIMITIVE_TOPOLOGY_13_CONTROL_POINT_PATCHLIST;

        case 14:
            return D3D_PRIMITIVE_TOPOLOGY_14_CONTROL_POINT_PATCHLIST;

        case 15:
            return D3D_PRIMITIVE_TOPOLOGY_15_CONTROL_POINT_PATCHLIST;

        case 16:
            return D3D_PRIMITIVE_TOPOLOGY_16_CONTROL_POINT_PATCHLIST;

        case 17:
            return D3D_PRIMITIVE_TOPOLOGY_17_CONTROL_POINT_PATCHLIST;

        case 18:
            return D3D_PRIMITIVE_TOPOLOGY_18_CONTROL_POINT_PATCHLIST;

        case 19:
            return D3D_PRIMITIVE_TOPOLOGY_19_CONTROL_POINT_PATCHLIST;

        case 20:
            return D3D_PRIMITIVE_TOPOLOGY_20_CONTROL_POINT_PATCHLIST;

        case 21:
            return D3D_PRIMITIVE_TOPOLOGY_21_CONTROL_POINT_PATCHLIST;

        case 22:
            return D3D_PRIMITIVE_TOPOLOGY_22_CONTROL_POINT_PATCHLIST;

        case 23:
            return D3D_PRIMITIVE_TOPOLOGY_23_CONTROL_POINT_PATCHLIST;

        case 24:
            return D3D_PRIMITIVE_TOPOLOGY_24_CONTROL_POINT_PATCHLIST;

        case 25:
            return D3D_PRIMITIVE_TOPOLOGY_25_CONTROL_POINT_PATCHLIST;

        case 26:
            return D3D_PRIMITIVE_TOPOLOGY_26_CONTROL_POINT_PATCHLIST;

        case 27:
            return D3D_PRIMITIVE_TOPOLOGY_27_CONTROL_POINT_PATCHLIST;

        case 28:
            return D3D_PRIMITIVE_TOPOLOGY_28_CONTROL_POINT_PATCHLIST;

        case 29:
            return D3D_PRIMITIVE_TOPOLOGY_29_CONTROL_POINT_PATCHLIST;

        case 30:
            return D3D_PRIMITIVE_TOPOLOGY_30_CONTROL_POINT_PATCHLIST;
    }

    return D3D_PRIMITIVE_TOPOLOGY_1_CONTROL_POINT_PATCHLIST;
}

static uint32_t getVertexTypeSizeD3D12(PalVertexType type)
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

static void getHitGroupNameD3D12(
    uint32_t index,
    wchar_t dst[PAL_SHADER_ENTRY_NAME_SIZE])
{
    wcscpy(dst, L"HitGroup");
    _itow(index, dst + 8, 10);
}

static const char* semanticIDToStringD3D12(PalVertexSemanticID id)
{
    switch (id) {
        case PAL_VERTEX_SEMANTIC_ID_POSITION:
            return "POSITION";

        case PAL_VERTEX_SEMANTIC_ID_COLOR:
            return "COLOR";

        case PAL_VERTEX_SEMANTIC_ID_TEXCOORD:
            return "TEXCOORD";

        case PAL_VERTEX_SEMANTIC_ID_NORMAL:
            return "NORMAL";

        case PAL_VERTEX_SEMANTIC_ID_TANGENT:
            return "TANGENT";
    }
    return nullptr;
}

PalResult PAL_CALL createPipelineLayoutD3D12(
    PalDevice* device,
    const PalPipelineLayoutCreateInfo* info,
    PalPipelineLayout** outLayout)
{
    DeviceD3D12* d3d12Device = (DeviceD3D12*)device;
    PipelineLayoutD3D12* layout = nullptr;
    uint32_t resourceCount = 0;
    uint32_t samplerCount = 0;
    uint64_t pushConstantSize = 0;
    uint32_t sizeInBytes = sizeof(D3D12_DESCRIPTOR_RANGE1);

    uint32_t rangesOffset = 0;
    uint32_t samplerRangesOffset = 0;
    D3D12_DESCRIPTOR_RANGE1* ranges = nullptr;
    D3D12_DESCRIPTOR_RANGE1* samplerRanges = nullptr;

    uint32_t parameterCount = 0;
    D3D12_ROOT_PARAMETER1* parameters = nullptr;
    D3D12_ROOT_SIGNATURE_FLAGS rootFlags = 0;
    rootFlags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

    if (info->descriptorSetLayoutCount > d3d12Device->limits.maxBoundDescriptorSets) {
        return PAL_RESULT_CODE_INVALID_ARGUMENT;
    }

    if (d3d12Device->shaderModel >= PAL_MAKE_SHADER_TARGET(6, 6)) {
        rootFlags |= D3D12_ROOT_SIGNATURE_FLAG_CBV_SRV_UAV_HEAP_DIRECTLY_INDEXED;
        rootFlags |= D3D12_ROOT_SIGNATURE_FLAG_SAMPLER_HEAP_DIRECTLY_INDEXED;
    }

    // get the total resource and sampler ranges for all provided descriptor set layouts
    for (int i = 0; i < info->descriptorSetLayoutCount; i++) {
        DescriptorSetLayoutD3D12* tmp = (DescriptorSetLayoutD3D12*)info->descriptorSetLayouts[i];
        resourceCount += tmp->bindingCount - tmp->samplerCount;
        samplerCount += tmp->samplerCount;

        if (tmp->bindingCount - tmp->samplerCount >= 1) {
            parameterCount++;
        }

        if (tmp->samplerCount >= 1) {
            parameterCount++;
        }
    }

    if (info->usePushConstant) {
        pushConstantSize = info->pushConstantInfo.offset + info->pushConstantInfo.size;
        if (pushConstantSize > d3d12Device->limits.maxPushConstantSize) {
            return PAL_RESULT_CODE_INVALID_ARGUMENT;
        }

        if (pushConstantSize) {
            parameterCount++;
        }
    }

    layout = palAllocate(s_D3D12.allocator, sizeof(PipelineLayoutD3D12), 0);
    if (!layout) {
        return PAL_RESULT_CODE_OUT_OF_MEMORY;
    }

    if (parameterCount) {
        uint32_t paramtersSize = sizeof(D3D12_ROOT_PARAMETER1) * parameterCount;
        parameters = palAllocate(s_D3D12.allocator, paramtersSize, 0);
        if (!parameters) {
            return PAL_RESULT_CODE_OUT_OF_MEMORY;
        }

        memset(parameters, 0, paramtersSize);
    }

    if (resourceCount) {
        ranges = palAllocate(s_D3D12.allocator, sizeInBytes * resourceCount, 0);
        if (!ranges) {
            return PAL_RESULT_CODE_OUT_OF_MEMORY;
        }
    }

    if (samplerCount) {
        samplerRanges = palAllocate(s_D3D12.allocator, sizeInBytes * samplerCount, 0);
        if (!samplerRanges) {
            return PAL_RESULT_CODE_OUT_OF_MEMORY;
        }
    }

    // write root constant first if its provided
    parameterCount = 0; // reset and reuse the same variable
    layout->constantIndex = UINT32_MAX;
    if (pushConstantSize && info->usePushConstant) {
        D3D12_ROOT_PARAMETER1* parameter = &parameters[parameterCount];
        parameter->ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
        parameter->Constants.Num32BitValues = (UINT)pushConstantSize / 4;
        parameter->ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

        layout->constantIndex = parameterCount;
        parameterCount++;
    }

    uint32_t registerSpace = 0;
    for (int i = 0; i < info->descriptorSetLayoutCount; i++) {
        DescriptorSetLayoutD3D12* tmp = (DescriptorSetLayoutD3D12*)info->descriptorSetLayouts[i];
        D3D12_ROOT_PARAMETER1* parameter = nullptr;

        // reset and reuse same variable
        resourceCount = tmp->bindingCount - tmp->samplerCount;
        samplerCount = tmp->samplerCount;

        uint32_t samplerIndex = samplerRangesOffset;
        uint32_t rangeIndex = rangesOffset;

        // seperate the samplers from the remaining descriptors
        for (int j = 0; j < tmp->bindingCount; j++) {
            DescriptorSetBinding* binding = &tmp->bindings[j];
            D3D12_DESCRIPTOR_RANGE1* tmpRange = nullptr;

            if (binding->type == PAL_DESCRIPTOR_TYPE_SAMPLER) {
                tmpRange = &samplerRanges[samplerIndex++];
            } else {
                tmpRange = &ranges[rangeIndex++];
            }

            *tmpRange = binding->range;
            tmpRange->RegisterSpace = registerSpace;
        }

        // resource ranges
        if (resourceCount) {
            parameter = &parameters[parameterCount];
            parameter->ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
            parameter->DescriptorTable.NumDescriptorRanges = resourceCount;
            parameter->DescriptorTable.pDescriptorRanges = &ranges[rangesOffset];
            parameter->ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

            rangesOffset += resourceCount;
            parameterCount++;
        }

        // sampler ranges
        if (samplerCount) {
            parameter = &parameters[parameterCount];
            parameter->ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
            parameter->DescriptorTable.NumDescriptorRanges = samplerCount;
            parameter->DescriptorTable.pDescriptorRanges = &samplerRanges[samplerRangesOffset];
            parameter->ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

            samplerRangesOffset += samplerCount;
            parameterCount++;
        }

        registerSpace++;
    }

    // create root signature
    D3D12_VERSIONED_ROOT_SIGNATURE_DESC rootDesc = {0};
    rootDesc.Version = D3D_ROOT_SIGNATURE_VERSION_1_1;
    rootDesc.Desc_1_1.NumParameters = parameterCount;
    rootDesc.Desc_1_1.pParameters = parameters;
    rootDesc.Desc_1_1.Flags = rootFlags;

    ID3DBlob* blob = nullptr;
    HRESULT result = s_D3D12.serializeVersionedRootSignature(&rootDesc, &blob, nullptr);
    if (FAILED(result)) {
        pollMessagesD3D12(d3d12Device);
        return makeResultD3D12(result);
    }

    result = d3d12Device->handle->lpVtbl->CreateRootSignature(
        d3d12Device->handle,
        0,
        blob->lpVtbl->GetBufferPointer(blob),
        blob->lpVtbl->GetBufferSize(blob),
        &IID_RootSignature,
        (void**)&layout->handle);

    if (FAILED(result)) {
        pollMessagesD3D12(d3d12Device);
        return makeResultD3D12(result);
    }

    if (resourceCount) {
        palFree(s_D3D12.allocator, ranges);
    }

    if (samplerCount) {
        palFree(s_D3D12.allocator, samplerRanges);
    }

    if (parameterCount) {
        palFree(s_D3D12.allocator, parameters);
    }
    blob->lpVtbl->Release(blob);

    *outLayout = (PalPipelineLayout*)layout;
    return PAL_RESULT_SUCCESS;
}

void PAL_CALL destroyPipelineLayoutD3D12(PalPipelineLayout* layout)
{
    PipelineLayoutD3D12* d3d12Layout = (PipelineLayoutD3D12*)layout;
    d3d12Layout->handle->lpVtbl->Release(d3d12Layout->handle);
    palFree(s_D3D12.allocator, d3d12Layout);
}

PalResult PAL_CALL createGraphicsPipelineD3D12(
    PalDevice* device,
    const PalGraphicsPipelineCreateInfo* info,
    PalPipeline** outPipeline)
{
    HRESULT result;
    uint32_t patchControlPoints = 0;
    uint32_t totalSize = 0;
    PalBool alphaToCoverageEnable = PAL_FALSE;
    PipelineD3D12* pipeline = nullptr;
    DeviceD3D12* d3d12Device = (DeviceD3D12*)device;
    PipelineLayoutD3D12* layout = (PipelineLayoutD3D12*)info->pipelineLayout;

    D3D12_INPUT_ELEMENT_DESC* elementDescs = nullptr;
    D3D12_VIEW_INSTANCE_LOCATION* viewLocations = nullptr;

    GraphicsPipelineStreamDesc graphicsStreamDesc = {0};
    memset(&graphicsStreamDesc, 0, sizeof(GraphicsPipelineStreamDesc));

    pipeline = palAllocate(s_D3D12.allocator, sizeof(PipelineD3D12), 0);
    if (!pipeline) {
        return PAL_RESULT_CODE_OUT_OF_MEMORY;
    }
    memset(pipeline, 0, sizeof(PipelineD3D12));

    if (info->renderingLayout->viewCount > 1) {
        viewLocations = palAllocate(
            s_D3D12.allocator,
            sizeof(D3D12_VIEW_INSTANCE_LOCATION) * info->renderingLayout->viewCount,
            0);

        if (!viewLocations) {
            return PAL_RESULT_CODE_OUT_OF_MEMORY;
        }
    }

    // Root signature
    RootSignatureStream* rootSignatureStream = &graphicsStreamDesc.layout;
    totalSize += sizeof(RootSignatureStream);
    rootSignatureStream->type = D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_ROOT_SIGNATURE;
    rootSignatureStream->root = layout->handle;

    // shaders
    for (int i = 0; i < info->shaderCount; i++) {
        ShaderD3D12* tmp = (ShaderD3D12*)info->shaders[i];
        ShaderStream* shaderStream = &graphicsStreamDesc.shaders[i];
        totalSize += sizeof(ShaderStream);
        D3D12_PIPELINE_STATE_SUBOBJECT_TYPE type;

        ShaderEntry* entry = &tmp->entries[0];
        if (entry->patchControlPoints) {
            patchControlPoints = entry->patchControlPoints;
            if (info->topology != PAL_PRIMITIVE_TOPOLOGY_PATCH) {
                palFree(s_D3D12.allocator, pipeline);
                return PAL_RESULT_CODE_INVALID_OPERATION;
            }
        }

        if (entry->stage == PAL_SHADER_STAGE_VERTEX) {
            type = D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_VS;

        } else if (entry->stage == PAL_SHADER_STAGE_FRAGMENT) {
            type = D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_PS;

        } else if (entry->stage == PAL_SHADER_STAGE_GEOMETRY) {
            type = D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_GS;

        } else if (entry->stage == PAL_SHADER_STAGE_TESSELLATION_CONTROL) {
            type = D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_HS;

        } else if (entry->stage == PAL_SHADER_STAGE_TESSELLATION_EVALUATION) {
            type = D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_DS;

        } else if (entry->stage == PAL_SHADER_STAGE_TASK) {
            type = D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_AS;

        } else {
            // mesh shader
            type = D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_MS;
        }

        shaderStream->type = type;
        shaderStream->desc = tmp->byteCode;
    }

    // Vertex input state
    // get the max size of vertex attributes in all layouts
    uint32_t vertexCount = 0;
    uint32_t vertexLayoutCount = info->vertexLayoutCount;
    for (int i = 0; i < vertexLayoutCount; i++) {
        PalVertexLayout* layout = &info->vertexLayouts[i];
        vertexCount += layout->attributeCount;
    }

    if (info->vertexLayoutCount > d3d12Device->limits.maxVertexLayouts) {
        return PAL_RESULT_CODE_INVALID_ARGUMENT;
    }

    if (vertexCount > d3d12Device->limits.maxVertexAttributes) {
        return PAL_RESULT_CODE_INVALID_ARGUMENT;
    }

    InputLayoutStream* inputLayoutStream = &graphicsStreamDesc.inputLayout;
    totalSize += sizeof(InputLayoutStream);
    inputLayoutStream->type = D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_INPUT_LAYOUT;

    pipeline->strides = nullptr;
    if (vertexCount) {
        pipeline->strides = palAllocate(s_D3D12.allocator, sizeof(uint32_t) * 8, 0);
        if (!pipeline->strides) {
            return PAL_RESULT_CODE_OUT_OF_MEMORY;
        }

        elementDescs =
            palAllocate(s_D3D12.allocator, sizeof(D3D12_INPUT_ELEMENT_DESC) * vertexCount, 0);

        if (!elementDescs) {
            palFree(s_D3D12.allocator, elementDescs);
            return PAL_RESULT_CODE_OUT_OF_MEMORY;
        }

        uint32_t positionIndex = 0;
        uint32_t colorIndex = 0;
        uint32_t texCoordIndex = 0;
        uint32_t normalIndex = 0;
        uint32_t tangentIndex = 0;

        for (int i = 0; i < info->vertexLayoutCount; i++) {
            PalVertexLayout* layout = &info->vertexLayouts[i];
            uint32_t stride = 0;
            uint32_t offset = 0;

            for (int j = 0; j < layout->attributeCount; j++) {
                PalVertexAttribute* vertexAttrib = &layout->attributes[j];
                D3D12_INPUT_ELEMENT_DESC* elementDesc = &elementDescs[j];

                elementDesc->Format = vertexTypeToD3D12(vertexAttrib->type);
                elementDesc->InputSlot = layout->binding;
                elementDesc->SemanticName = semanticIDToStringD3D12(vertexAttrib->semanticID);

                if (vertexAttrib->semanticID == PAL_VERTEX_SEMANTIC_ID_POSITION) {
                    elementDesc->SemanticIndex = positionIndex++;

                } else if (vertexAttrib->semanticID == PAL_VERTEX_SEMANTIC_ID_COLOR) {
                    elementDesc->SemanticIndex = colorIndex++;

                } else if (vertexAttrib->semanticID == PAL_VERTEX_SEMANTIC_ID_TEXCOORD) {
                    elementDesc->SemanticIndex = texCoordIndex++;

                } else if (vertexAttrib->semanticID == PAL_VERTEX_SEMANTIC_ID_NORMAL) {
                    elementDesc->SemanticIndex = normalIndex++;

                } else {
                    // tangent
                    elementDesc->SemanticIndex = tangentIndex++;
                }

                if (layout->type == PAL_VERTEX_LAYOUT_TYPE_PER_INSTANCE) {
                    elementDesc->InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA;
                    elementDesc->InstanceDataStepRate = 1;
                } else {
                    elementDesc->InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
                    elementDesc->InstanceDataStepRate = 0;
                }

                // build offsets and stride
                uint32_t size = getVertexTypeSizeD3D12(vertexAttrib->type);
                elementDesc->AlignedByteOffset = offset;
                offset += size;
                stride += size;
            }

            // cache the computed stride to be used later by the vertex buffer
            pipeline->strides[i] = stride;
        }

        inputLayoutStream->desc.NumElements = vertexCount;
        inputLayoutStream->desc.pInputElementDescs = elementDescs;
    }

    // Primitive Topology
    D3D12_PRIMITIVE_TOPOLOGY_TYPE topologyType = 0;
    D3D_PRIMITIVE_TOPOLOGY topology = 0;
    switch (info->topology) {
        case PAL_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST: {
            topologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
            topology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
            break;
        }

        case PAL_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP: {
            topologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
            topology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;
            break;
        }

        case PAL_PRIMITIVE_TOPOLOGY_LINE_LIST: {
            topologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE;
            topology = D3D_PRIMITIVE_TOPOLOGY_LINELIST;
            break;
        }

        case PAL_PRIMITIVE_TOPOLOGY_LINE_STRIP: {
            topologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE;
            topology = D3D_PRIMITIVE_TOPOLOGY_LINESTRIP;
            break;
        }

        case PAL_PRIMITIVE_TOPOLOGY_POINT_LIST: {
            topologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT;
            topology = D3D_PRIMITIVE_TOPOLOGY_POINTLIST;
            break;
        }

        case PAL_PRIMITIVE_TOPOLOGY_PATCH: {
            topologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_PATCH;
            topology = getPatchTopology(patchControlPoints);
            break;
        }
    }

    TopologyStream* topologyStream = &graphicsStreamDesc.topology;
    totalSize += sizeof(TopologyStream);
    topologyStream->type = D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_PRIMITIVE_TOPOLOGY;
    topologyStream->topology = topologyType;

    // IB Strip Cut
    IBStripCutStream* inStripCutStream = &graphicsStreamDesc.ibStripCut;
    totalSize += sizeof(IBStripCutStream);
    inStripCutStream->type = D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_IB_STRIP_CUT_VALUE;
    if (info->primitiveRestartEnable == PAL_FALSE) {
        inStripCutStream->value = D3D12_INDEX_BUFFER_STRIP_CUT_VALUE_DISABLED;

    } else {
        if (info->indexType == PAL_INDEX_TYPE_UINT16) {
            inStripCutStream->value = D3D12_INDEX_BUFFER_STRIP_CUT_VALUE_0xFFFF;
        } else {
            inStripCutStream->value = D3D12_INDEX_BUFFER_STRIP_CUT_VALUE_0xFFFFFFFF;
        }
    }

    // Rasterizer
    RasterizerStream* rasterizerStream = &graphicsStreamDesc.rasterizer;
    totalSize += sizeof(RasterizerStream);
    rasterizerStream->type = D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_RASTERIZER;

    rasterizerStream->desc.CullMode = D3D12_CULL_MODE_NONE;
    rasterizerStream->desc.FillMode = D3D12_FILL_MODE_SOLID;
    rasterizerStream->desc.FrontCounterClockwise = PAL_FALSE;
    rasterizerStream->desc.DepthClipEnable = TRUE;

    if (info->rasterizerState) {
        PalRasterizerState* state = info->rasterizerState;
        if (state->cullMode == PAL_CULL_MODE_NONE) {
            rasterizerStream->desc.CullMode = D3D12_CULL_MODE_NONE;

        } else if (state->cullMode == PAL_CULL_MODE_BACK) {
            rasterizerStream->desc.CullMode = D3D12_CULL_MODE_BACK;

        } else if (state->cullMode == PAL_CULL_MODE_FRONT) {
            rasterizerStream->desc.CullMode = D3D12_CULL_MODE_FRONT;
        }

        if (state->polygonMode == PAL_POLYGON_MODE_FILL) {
            rasterizerStream->desc.FillMode = D3D12_FILL_MODE_SOLID;

        } else {
            rasterizerStream->desc.FillMode = D3D12_FILL_MODE_WIREFRAME;
        }

        if (state->frontFace == PAL_FRONT_FACE_CLOCKWISE) {
            rasterizerStream->desc.FrontCounterClockwise = PAL_FALSE;

        } else {
            rasterizerStream->desc.FrontCounterClockwise = TRUE;
        }

        rasterizerStream->desc.DepthClipEnable = !state->enableDepthClamp;
        rasterizerStream->desc.DepthBias = (INT)state->depthBiasConstant;
        rasterizerStream->desc.SlopeScaledDepthBias = state->depthBiasSlope;
        rasterizerStream->desc.DepthBiasClamp = state->depthBiasClamp;
    }

    // Sample Desc
    SampleDescStream* sampleDescStream = &graphicsStreamDesc.sampleDesc;
    totalSize += sizeof(SampleDescStream);
    sampleDescStream->type = D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_SAMPLE_DESC;
    sampleDescStream->desc.Quality = 0;
    sampleDescStream->desc.Count = 1;

    // Sample Mask
    SampleMaskStream* sampleMaskStream = &graphicsStreamDesc.sampleMask;
    totalSize += sizeof(SampleMaskStream);
    sampleMaskStream->type = D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_SAMPLE_MASK;
    sampleMaskStream->mask = UINT_MAX;

    if (info->multisampleState) {
        PalMultisampleState* state = info->multisampleState;
        if (state->sampleMask) {
            sampleMaskStream->mask = (UINT)state->sampleMask;
        }

        sampleDescStream->desc.Count = samplesToD3D12(state->sampleCount);
        alphaToCoverageEnable = state->enableAlphaToCoverage;
    }

    // Depth stencil
    DepthStencilStream* depthStencilStream = &graphicsStreamDesc.depthStencil;
    totalSize += sizeof(DepthStencilStream);
    depthStencilStream->type = D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_DEPTH_STENCIL;
    depthStencilStream->desc.DepthEnable = PAL_FALSE;
    depthStencilStream->desc.StencilEnable = PAL_FALSE;

    if (info->depthStencilState) {
        PalDepthStencilState* state = info->depthStencilState;
        PalStencilOpState* back = &state->backStencilOpState;
        PalStencilOpState* front = &state->frontStencilOpState;

        D3D12_DEPTH_STENCILOP_DESC* d3dBack = &depthStencilStream->desc.BackFace;
        D3D12_DEPTH_STENCILOP_DESC* d3dFront = &depthStencilStream->desc.FrontFace;

        d3dBack->StencilFunc = compareOpToD3D12(back->compareOp);
        d3dBack->StencilDepthFailOp = stencilOpToD3D12(back->depthFailOp);
        d3dBack->StencilFailOp = stencilOpToD3D12(back->failOp);
        d3dBack->StencilPassOp = stencilOpToD3D12(back->passOp);

        d3dFront->StencilFunc = compareOpToD3D12(front->compareOp);
        d3dFront->StencilDepthFailOp = stencilOpToD3D12(front->depthFailOp);
        d3dFront->StencilFailOp = stencilOpToD3D12(front->failOp);
        d3dFront->StencilPassOp = stencilOpToD3D12(front->passOp);

        depthStencilStream->desc.DepthFunc = compareOpToD3D12(state->compareOp);
        depthStencilStream->desc.DepthEnable = state->enableDepthTest;
        depthStencilStream->desc.StencilEnable = state->enableStencilTest;
        if (state->enableDepthWrite) {
            depthStencilStream->desc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
        } else {
            depthStencilStream->desc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
        }
    }

    // Blend
    BlendStream* blendStream = &graphicsStreamDesc.blend;
    totalSize += sizeof(BlendStream);
    blendStream->type = D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_BLEND;
    blendStream->desc.IndependentBlendEnable = TRUE;
    blendStream->desc.AlphaToCoverageEnable = alphaToCoverageEnable;

    if (info->colorBlendAttachmentCount) {
        for (int i = 0; i < info->colorBlendAttachmentCount; i++) {
            D3D12_RENDER_TARGET_BLEND_DESC* tmp = &blendStream->desc.RenderTarget[i];
            PalColorBlendAttachment* desc = &info->colorBlendAttachments[i];

            tmp->BlendEnable = desc->enableBlend;
            tmp->BlendOpAlpha = blendOpToD3D12(desc->alphaBlendOp);
            tmp->BlendOp = blendOpToD3D12(desc->colorBlendOp);

            tmp->SrcBlendAlpha = blendFactorToD3D12(desc->srcAlphaBlendFactor);
            tmp->SrcBlend = blendFactorToD3D12(desc->srcColorBlendFactor);

            tmp->DestBlendAlpha = blendFactorToD3D12(desc->dstAlphaBlendFactor);
            tmp->DestBlend = blendFactorToD3D12(desc->dstColorBlendFactor);

            // blend color write mask
            tmp->RenderTargetWriteMask = 0;
            if (desc->colorWriteMask & PAL_COLOR_MASK_RED) {
                tmp->RenderTargetWriteMask |= D3D12_COLOR_WRITE_ENABLE_RED;
            }

            if (desc->colorWriteMask & PAL_COLOR_MASK_GREEN) {
                tmp->RenderTargetWriteMask |= D3D12_COLOR_WRITE_ENABLE_GREEN;
            }

            if (desc->colorWriteMask & PAL_COLOR_MASK_BLUE) {
                tmp->RenderTargetWriteMask |= D3D12_COLOR_WRITE_ENABLE_BLUE;
            }

            if (desc->colorWriteMask & PAL_COLOR_MASK_ALPHA) {
                tmp->RenderTargetWriteMask |= D3D12_COLOR_WRITE_ENABLE_ALPHA;
            }
        }
    }

    // Fragment shading rate
    pipeline->hasFsr = PAL_FALSE;
    if (info->fragmentShadingRateState) {
        PalFragmentShadingRateState* state = info->fragmentShadingRateState;
        pipeline->shadingRate = shadingRateToD3D12(state->rate);
        for (int i = 0; i < 2; i++) {
            pipeline->combinerOps[i] = combinerOpsToD3D12(state->combinerOps[i]);
        }
    }

    // RTV Formats
    RTVStream* rtvStream = &graphicsStreamDesc.RTV;
    totalSize += sizeof(RTVStream);
    rtvStream->type = D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_RENDER_TARGET_FORMATS;

    DXGI_FORMAT format = DXGI_FORMAT_UNKNOWN;
    rtvStream->data.NumRenderTargets = info->renderingLayout->colorAttachentCount;
    for (int i = 0; i < info->renderingLayout->colorAttachentCount; i++) {
        format = formatToD3D12(info->renderingLayout->colorAttachmentsFormat[i]);
        rtvStream->data.RTFormats[i] = format;
    }

    // DSV Format
    DSVStream* dsvStream = &graphicsStreamDesc.DSV;
    totalSize += sizeof(DSVStream);
    dsvStream->type = D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_DEPTH_STENCIL_FORMAT;
    dsvStream->format = formatToD3D12(info->renderingLayout->depthStencilAttachmentFormat);

    // View Instancing
    ViewInstancingStream* viewInstacingStream = &graphicsStreamDesc.viewInstancing;
    totalSize += sizeof(ViewInstancingStream);
    viewInstacingStream->type = D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_VIEW_INSTANCING;

    viewInstacingStream->desc.ViewInstanceCount = 0;
    viewInstacingStream->desc.pViewInstanceLocations = nullptr;
    if (info->renderingLayout->viewCount > 1) {
        for (int i = 0; i < info->renderingLayout->viewCount; i++) {
            viewLocations[i].RenderTargetArrayIndex = i;
            viewLocations[i].ViewportArrayIndex = i;
        }

        viewInstacingStream->desc.ViewInstanceCount = info->renderingLayout->viewCount;
        viewInstacingStream->desc.pViewInstanceLocations = viewLocations;
    }

    D3D12_PIPELINE_STATE_STREAM_DESC streamDesc = {0};
    streamDesc.pPipelineStateSubobjectStream = &graphicsStreamDesc;
    streamDesc.SizeInBytes = totalSize;

    result = d3d12Device->handle->lpVtbl->CreatePipelineState(
        d3d12Device->handle,
        &streamDesc,
        &IID_PipelineState,
        &pipeline->handle);

    if (FAILED(result)) {
        pollMessagesD3D12(d3d12Device);
        return makeResultD3D12(result);
    }

    if (info->vertexLayoutCount) {
        palFree(s_D3D12.allocator, elementDescs);
    }

    if (info->renderingLayout->viewCount > 1) {
        palFree(s_D3D12.allocator, viewLocations);
    }

    pipeline->topology = topology;
    pipeline->type = GRAPHICS_PIPELINE;
    pipeline->layout = layout;
    pipeline->shaderExports = nullptr;
    pipeline->localRootSignature = nullptr;

    *outPipeline = (PalPipeline*)pipeline;
    return PAL_RESULT_SUCCESS;
}

PalResult PAL_CALL createComputePipelineD3D12(
    PalDevice* device,
    const PalComputePipelineCreateInfo* info,
    PalPipeline** outPipeline)
{
    DeviceD3D12* d3d12Device = (DeviceD3D12*)device;
    PipelineLayoutD3D12* layout = (PipelineLayoutD3D12*)info->pipelineLayout;
    ShaderD3D12* shader = (ShaderD3D12*)info->computeShader;
    PipelineD3D12* pipeline = nullptr;

    pipeline = palAllocate(s_D3D12.allocator, sizeof(PipelineD3D12), 0);
    if (!pipeline) {
        return PAL_RESULT_CODE_OUT_OF_MEMORY;
    }

    D3D12_COMPUTE_PIPELINE_STATE_DESC desc = {0};
    desc.CS = shader->byteCode;
    desc.pRootSignature = layout->handle;

    HRESULT result = d3d12Device->handle->lpVtbl->CreateComputePipelineState(
        d3d12Device->handle,
        &desc,
        &IID_PipelineState,
        &pipeline->handle);

    if (FAILED(result)) {
        pollMessagesD3D12(d3d12Device);
        return makeResultD3D12(result);
    }

    pipeline->type = COMPUTE_PIPELINE;
    pipeline->strides = nullptr;
    pipeline->hasFsr = PAL_FALSE;
    pipeline->layout = layout;
    pipeline->shaderExports = nullptr;
    pipeline->localRootSignature = nullptr;

    *outPipeline = (PalPipeline*)pipeline;
    return PAL_RESULT_SUCCESS;
}

PalResult PAL_CALL createRayTracingPipelineD3D12(
    PalDevice* device,
    const PalRayTracingPipelineCreateInfo* info,
    PalPipeline** outPipeline)
{
    HRESULT result;
    DeviceD3D12* d3d12Device = (DeviceD3D12*)device;
    PipelineLayoutD3D12* layout = (PipelineLayoutD3D12*)info->pipelineLayout;
    PipelineD3D12* pipeline = nullptr;

    if (info->maxAttributeSize > d3d12Device->limits.maxHitAttributeSize) {
        return PAL_RESULT_CODE_INVALID_ARGUMENT;
    }

    if (info->maxPayloadSize > d3d12Device->limits.maxPayloadSize) {
        return PAL_RESULT_CODE_INVALID_ARGUMENT;
    }

    if (info->maxRecursionDepth > d3d12Device->limits.maxRecursionDepth) {
        return PAL_RESULT_CODE_INVALID_ARGUMENT;
    }

    D3D12_EXPORT_DESC* exportDescs = nullptr;
    D3D12_DXIL_LIBRARY_DESC* libraryDescs = nullptr;
    RayHitGroup* hitGroups = nullptr;
    D3D12_STATE_SUBOBJECT* subObjects = nullptr;
    const wchar_t** localExports = nullptr;

    // find the total number of exports
    uint32_t exportCount = 0;
    for (int i = 0; i < info->shaderCount; i++) {
        ShaderD3D12* shader = (ShaderD3D12*)info->shaders[i];
        exportCount += shader->entryCount;
    }

    uint32_t subObjectCount = 0;
    uint32_t localExportCount = 0;
    ShaderBindingTableInfo sbtInfo = {0};

    for (int i = 0; i < info->shaderGroupCount; i++) {
        PalRayTracingShaderGroupCreateInfo* tmp = &info->shaderGroups[i];
        if (tmp->type == PAL_RAY_TRACING_SHADER_GROUP_TYPE_GENERAL) {
            // check if its raygen, miss or callable
            ShaderD3D12* shader = (ShaderD3D12*)info->shaders[tmp->generalShaderIndex];
            ShaderEntry* entry = &shader->entries[tmp->generalShaderEntryIndex];
            switch (entry->stage) {
                case PAL_SHADER_STAGE_RAYGEN: {
                    sbtInfo.raygenCount++;
                    sbtInfo.raygenDataSize = max(sbtInfo.raygenDataSize, tmp->maxDataSize);
                    break;
                }

                case PAL_SHADER_STAGE_MISS: {
                    sbtInfo.missCount++;
                    sbtInfo.missDataSize = max(sbtInfo.missDataSize, tmp->maxDataSize);
                    break;
                }

                case PAL_SHADER_STAGE_CALLABLE: {
                    sbtInfo.callableCount++;
                    sbtInfo.callableDataSize = max(sbtInfo.callableDataSize, tmp->maxDataSize);
                    break;
                }
            }

        } else {
            sbtInfo.hitDataSize = max(sbtInfo.hitDataSize, tmp->maxDataSize);
            sbtInfo.hitCount++;
        }

        if (tmp->maxDataSize) {
            localExportCount++;
        }
    }

    // find the max data size across all shader groups
    uint32_t localRootSize = 0;
    localRootSize = max(localRootSize, sbtInfo.raygenDataSize);
    localRootSize = max(localRootSize, sbtInfo.missDataSize);
    localRootSize = max(localRootSize, sbtInfo.hitDataSize);
    localRootSize = max(localRootSize, sbtInfo.callableDataSize);

    // // D3D12_STATE_SUBOBJECT_TYPE_GLOBAL_ROOT_SIGNATURE
    // // D3D12_STATE_SUBOBJECT_TYPE_RAYTRACING_PIPELINE_CONFIG
    // // D3D12_STATE_SUBOBJECT_TYPE_RAYTRACING_SHADER_CONFIG
    subObjectCount += info->shaderCount + 3;

    subObjectCount += sbtInfo.hitCount;
    if (localRootSize) {
        // D3D12_STATE_SUBOBJECT_TYPE_LOCAL_ROOT_SIGNATURE
        // D3D12_STATE_SUBOBJECT_TYPE_SUBOBJECT_TO_EXPORTS_ASSOCIATION
        subObjectCount += 2;
    }

    pipeline = palAllocate(s_D3D12.allocator, sizeof(PipelineD3D12), 0);
    if (!pipeline) {
        return PAL_RESULT_CODE_OUT_OF_MEMORY;
    }

    pipeline->shaderExportCount = exportCount + sbtInfo.hitCount;
    subObjects = palAllocate(s_D3D12.allocator, sizeof(D3D12_STATE_SUBOBJECT) * subObjectCount, 0);
    hitGroups = palAllocate(s_D3D12.allocator, sizeof(RayHitGroup) * sbtInfo.hitCount, 0);
    if (!subObjects || !hitGroups) {
        return PAL_RESULT_CODE_OUT_OF_MEMORY;
    }

    libraryDescs =
        palAllocate(s_D3D12.allocator, sizeof(D3D12_DXIL_LIBRARY_DESC) * info->shaderCount, 0);

    exportDescs = palAllocate(s_D3D12.allocator, sizeof(D3D12_EXPORT_DESC) * exportCount, 0);

    pipeline->shaderExports =
        palAllocate(s_D3D12.allocator, sizeof(ShaderExport) * pipeline->shaderExportCount, 0);

    localExports = palAllocate(s_D3D12.allocator, sizeof(wchar_t*) * localExportCount, 0);

    if (!libraryDescs || !exportDescs || !pipeline->shaderExports || !localExports) {
        return PAL_RESULT_CODE_OUT_OF_MEMORY;
    }

    // global root signature
    uint32_t subObjectIndex = 0;
    D3D12_GLOBAL_ROOT_SIGNATURE globalRootSignature = {0};
    globalRootSignature.pGlobalRootSignature = layout->handle;

    subObjects[subObjectIndex].Type = D3D12_STATE_SUBOBJECT_TYPE_GLOBAL_ROOT_SIGNATURE;
    subObjects[subObjectIndex].pDesc = &globalRootSignature;
    subObjectIndex++;

    // ray tracing config
    D3D12_RAYTRACING_SHADER_CONFIG shaderConfig = {0};
    shaderConfig.MaxAttributeSizeInBytes = info->maxAttributeSize;
    shaderConfig.MaxPayloadSizeInBytes = info->maxPayloadSize;
    subObjects[subObjectIndex].Type = D3D12_STATE_SUBOBJECT_TYPE_RAYTRACING_SHADER_CONFIG;
    subObjects[subObjectIndex].pDesc = &shaderConfig;
    subObjectIndex++;

    D3D12_RAYTRACING_PIPELINE_CONFIG pipelineConfig = {0};
    pipelineConfig.MaxTraceRecursionDepth = info->maxRecursionDepth;
    subObjects[subObjectIndex].Type = D3D12_STATE_SUBOBJECT_TYPE_RAYTRACING_PIPELINE_CONFIG;
    subObjects[subObjectIndex].pDesc = &pipelineConfig;
    subObjectIndex++;

    // shaders
    uint32_t exportsOffset = 0;
    for (int i = 0; i < info->shaderCount; i++) {
        ShaderD3D12* tmp = (ShaderD3D12*)info->shaders[i];
        D3D12_DXIL_LIBRARY_DESC* libraryDesc = &libraryDescs[i];
        libraryDesc->DXILLibrary = tmp->byteCode;

        for (int j = 0; j < tmp->entryCount; j++) {
            D3D12_EXPORT_DESC* exportDesc = &exportDescs[exportsOffset + j];
            ShaderExport* shaderExport = &pipeline->shaderExports[exportsOffset + j];
            ShaderEntry* entry = &tmp->entries[j];

            shaderExport->isHitGroup = PAL_FALSE;
            shaderExport->stage = entry->stage;
            wcscpy(shaderExport->entryName, entry->entryName);

            exportDesc->ExportToRename = nullptr;
            exportDesc->Flags = D3D12_EXPORT_FLAG_NONE;
            exportDesc->Name = shaderExport->entryName;
        }

        libraryDesc->pExports = &exportDescs[exportsOffset];
        libraryDesc->NumExports = tmp->entryCount;

        subObjects[subObjectIndex].Type = D3D12_STATE_SUBOBJECT_TYPE_DXIL_LIBRARY;
        subObjects[subObjectIndex].pDesc = libraryDesc;
        subObjectIndex++;
        exportsOffset += tmp->entryCount;
    }

    // hit groups
    uint32_t localExportIndex = 0;
    uint32_t hitGroupIndex = 0;
    for (int i = 0; i < info->shaderGroupCount; i++) {
        const wchar_t* exportName = nullptr;
        PalRayTracingShaderGroupCreateInfo* tmp = &info->shaderGroups[i];
        if (tmp->type == PAL_RAY_TRACING_SHADER_GROUP_TYPE_GENERAL) {
            if (tmp->maxDataSize) {
                ShaderD3D12* shader = (ShaderD3D12*)info->shaders[tmp->generalShaderIndex];
                ShaderEntry* entry = &shader->entries[tmp->generalShaderEntryIndex];
                localExports[localExportIndex++] = entry->entryName;
            }

            continue;
        }

        D3D12_HIT_GROUP_DESC* group = &hitGroups[hitGroupIndex].desc;
        getHitGroupNameD3D12(hitGroupIndex, hitGroups[hitGroupIndex].entryName);

        ShaderExport* hitGroupExport = &pipeline->shaderExports[exportCount++];
        wcscpy(hitGroupExport->entryName, hitGroups[hitGroupIndex].entryName);
        hitGroupExport->isHitGroup = PAL_TRUE;
        hitGroupExport->stage = PAL_SHADER_STAGE_CLOSEST_HIT; // to identify

        group->AnyHitShaderImport = nullptr;
        group->ClosestHitShaderImport = nullptr;
        group->IntersectionShaderImport = nullptr;
        group->HitGroupExport = hitGroups[hitGroupIndex].entryName;

        if (tmp->type == PAL_RAY_TRACING_SHADER_GROUP_TYPE_TRIANGLES_HIT) {
            group->Type = D3D12_HIT_GROUP_TYPE_TRIANGLES;

        } else if (tmp->type == PAL_RAY_TRACING_SHADER_GROUP_TYPE_PROCEDURAL_HIT) {
            group->Type = D3D12_HIT_GROUP_TYPE_PROCEDURAL_PRIMITIVE;
        }

        // Any hit shader
        if (tmp->anyHitShaderIndex != PAL_UNUSED_SHADER_INDEX) {
            ShaderD3D12* shader = (ShaderD3D12*)info->shaders[tmp->anyHitShaderIndex];
            ShaderEntry* entry = &shader->entries[tmp->anyHitShaderEntryIndex];
            group->AnyHitShaderImport = entry->entryName;

        } else {
            group->AnyHitShaderImport = nullptr;
        }

        // Closest hit shader
        if (tmp->closestHitShaderIndex != PAL_UNUSED_SHADER_INDEX) {
            ShaderD3D12* shader = (ShaderD3D12*)info->shaders[tmp->closestHitShaderIndex];
            ShaderEntry* entry = &shader->entries[tmp->closestHitShaderEntryIndex];
            group->ClosestHitShaderImport = entry->entryName;

        } else {
            group->ClosestHitShaderImport = nullptr;
        }

        // IntersectionShader shader
        if (tmp->intersectionShaderIndex != PAL_UNUSED_SHADER_INDEX) {
            ShaderD3D12* shader = (ShaderD3D12*)info->shaders[tmp->intersectionShaderIndex];
            ShaderEntry* entry = &shader->entries[tmp->intersectionShaderEntryIndex];
            group->IntersectionShaderImport = entry->entryName;

        } else {
            group->IntersectionShaderImport = nullptr;
        }

        if (tmp->maxDataSize) {
            localExports[localExportIndex] = group->HitGroupExport;
        }

        subObjects[subObjectIndex].Type = D3D12_STATE_SUBOBJECT_TYPE_HIT_GROUP;
        subObjects[subObjectIndex].pDesc = group;
        subObjectIndex++;
        hitGroupIndex++;
        localExportIndex++;
    }

    // check if we need a local root signature
    D3D12_SUBOBJECT_TO_EXPORTS_ASSOCIATION localExportAssociation = {0};
    D3D12_LOCAL_ROOT_SIGNATURE localRootSignature = {0};
    if (localRootSize) {
        D3D12_ROOT_PARAMETER1 parameter = {0};
        parameter.ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
        parameter.Constants.Num32BitValues = align(localRootSize, 4) / 4;
        parameter.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

        D3D12_VERSIONED_ROOT_SIGNATURE_DESC rootDesc = {0};
        rootDesc.Version = D3D_ROOT_SIGNATURE_VERSION_1_1;
        rootDesc.Desc_1_1.NumParameters = 1;
        rootDesc.Desc_1_1.pParameters = &parameter;
        rootDesc.Desc_1_1.Flags = D3D12_ROOT_SIGNATURE_FLAG_LOCAL_ROOT_SIGNATURE;

        ID3DBlob* blob = nullptr;
        result = s_D3D12.serializeVersionedRootSignature(&rootDesc, &blob, nullptr);
        if (FAILED(result)) {
            pollMessagesD3D12(d3d12Device);
            return makeResultD3D12(result);
        }

        result = d3d12Device->handle->lpVtbl->CreateRootSignature(
            d3d12Device->handle,
            0,
            blob->lpVtbl->GetBufferPointer(blob),
            blob->lpVtbl->GetBufferSize(blob),
            &IID_RootSignature,
            (void**)&pipeline->localRootSignature);

        if (FAILED(result)) {
            pollMessagesD3D12(d3d12Device);
            return makeResultD3D12(result);
        }

        localRootSignature.pLocalRootSignature = pipeline->localRootSignature;
        subObjects[subObjectIndex].Type = D3D12_STATE_SUBOBJECT_TYPE_LOCAL_ROOT_SIGNATURE;
        subObjects[subObjectIndex].pDesc = &localRootSignature;

        localExportAssociation.pSubobjectToAssociate = &subObjects[subObjectIndex];
        localExportAssociation.pExports = localExports;
        localExportAssociation.NumExports = localExportCount;
        subObjectIndex++;

        D3D12_STATE_SUBOBJECT_TYPE t = D3D12_STATE_SUBOBJECT_TYPE_SUBOBJECT_TO_EXPORTS_ASSOCIATION;
        subObjects[subObjectIndex].Type = t;
        subObjects[subObjectIndex].pDesc = &localExportAssociation;
        subObjectIndex++;
    }

    D3D12_STATE_OBJECT_DESC desc = {0};
    desc.Type = D3D12_STATE_OBJECT_TYPE_RAYTRACING_PIPELINE;
    desc.NumSubobjects = subObjectCount;
    desc.pSubobjects = subObjects;

    result = d3d12Device->handle->lpVtbl->CreateStateObject(
        d3d12Device->handle,
        &desc,
        &IID_StateObject,
        &pipeline->handle);

    if (FAILED(result)) {
        pollMessagesD3D12(d3d12Device);
        return makeResultD3D12(result);
    }

    palFree(s_D3D12.allocator, hitGroups);
    palFree(s_D3D12.allocator, subObjects);
    palFree(s_D3D12.allocator, libraryDescs);
    palFree(s_D3D12.allocator, exportDescs);
    palFree(s_D3D12.allocator, localExports);

    pipeline->type = RAY_TRACING_PIPELINE;
    pipeline->strides = nullptr;
    pipeline->hasFsr = PAL_FALSE;
    pipeline->layout = layout;

    pipeline->sbtInfo = sbtInfo;
    *outPipeline = (PalPipeline*)pipeline;
    return PAL_RESULT_SUCCESS;
}

void PAL_CALL destroyPipelineD3D12(PalPipeline* pipeline)
{
    PipelineD3D12* d3dPipeline = (PipelineD3D12*)pipeline;
    if (d3dPipeline->type == RAY_TRACING_PIPELINE) {
        ID3D12StateObject* handle = d3dPipeline->handle;
        handle->lpVtbl->Release(handle);

    } else {
        ID3D12PipelineState* handle = d3dPipeline->handle;
        handle->lpVtbl->Release(handle);
    }

    if (d3dPipeline->localRootSignature) {
        d3dPipeline->localRootSignature->lpVtbl->Release(d3dPipeline->localRootSignature);
    }

    if (d3dPipeline->strides) {
        palFree(s_D3D12.allocator, d3dPipeline->strides);
    }

    if (d3dPipeline->shaderExports) {
        palFree(s_D3D12.allocator, d3dPipeline->shaderExports);
    }

    palFree(s_D3D12.allocator, d3dPipeline);
}

#endif // PAL_HAS_D3D12_BACKEND