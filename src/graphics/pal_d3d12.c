
/**
    PAL - Prime Abstraction Layer
    Copyright (C) 2025
    Licensed under the Zlib license. See LICENSE file in root.
 */

// ==================================================
// Includes
// ==================================================

#include "pal/pal_graphics.h"

#if PAL_HAS_D3D12_BACKEND

#ifdef _WIN32
#include <windows.h>
#include <d3d12.h>
#include <dxgi1_6.h>
#include <d3d12sdklayers.h>

#define MAX_MESSAGE_SIZE 4096

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

// IIDS
IID IID_Device = {0xc4fec28f, 0x7966, 0x4e95, 0x9f,0x94, 0xf4,0x31,0xcb,0x56,0xc3,0xb8};
IID IID_Adapter = {0x3c8d99d1, 0x4fbf, 0x4181, 0xa8,0x2c, 0xaf,0x66,0xbf,0x7b,0xd2,0x4e};
IID IID_Factory = {0xc1b6694f, 0xff09, 0x44a9, 0xb0,0x3c, 0x77,0x90,0x0a,0x0a,0x1d,0x17};
IID IID_DebugController = {0x344488b7, 0x6846, 0x474b, 0xb9,0x89, 0xf0,0x27,0x44,0x82,0x45,0xe0};
IID IID_DebugController1 = {0xaffaa4ca, 0x63fe, 0x4d8e, 0xb8,0xad, 0x15,0x90,0x00,0xaf,0x43,0x04};
IID IID_InfoQueue = {0x0742a90b, 0xc387, 0x483f, 0xb9,0x46, 0x30,0xa7,0xe4,0xe6,0x14,0x58};
IID IID_Heap = {0x6b3b2502, 0x6e51, 0x45b3, 0x90,0xee, 0x98,0x84,0x26,0x5e,0x8d,0xf3};
IID IID_Queue = {0x0ec870a6, 0x5d7e, 0x4c22, 0x8c,0xfc, 0x5b,0xaa,0xe0,0x76,0x16,0xed};
IID IID_Swapchain = {0x94d99bdb, 0xf1f8, 0x4ab0, 0xb2,0x36, 0x7d,0xa0,0x17,0x0e,0xda,0xb1};
IID IID_CommandAllocator = {0x6102dee4, 0xaf59, 0x4b09, 0xb9,0x99, 0xb4,0x4d,0x73,0xf0,0x9b,0x24};
IID IID_CommandList = {0x7116d91c, 0xe7e4, 0x47ce, 0xb8,0xc6, 0xec,0x81,0x68,0xf4,0x37,0xe5};
IID IID_CommandList6 = {0xc3827890, 0xe548, 0x4cfa, 0x96,0xcf, 0x56,0x89,0xa9,0x37,0x0f,0x80};
IID IID_CommandSignature = {0xc36a797c, 0xec80, 0x4f0a, 0x89,0x85, 0xa7,0xb2,0x47,0x50,0x82,0xd1};
IID IID_DescriptorHeap = {0x8efb471d, 0x616c, 0x4f49, 0x90,0xf7, 0x12,0x7b,0xb7,0x63,0xfa,0x51};
IID IID_RootSignature = {0xc54a6b66, 0x72df, 0x4ee8, 0x8b,0xe5, 0xa9,0x46,0xa1,0x42,0x92,0x14};
IID IID_Device5 = {0x8b4f173b, 0x2fea, 0x4b80, 0x8f,0x58, 0x43,0x07,0x19,0x1a,0xb9,0x5d};
IID IID_PipelineState = {0x765a30f3, 0xf624, 0x4c6f, 0xa8,0x28, 0xac,0xe9,0x48,0x62,0x24,0x45};
IID IID_StateObject = {0x47016943, 0xfca8, 0x4594, 0x93,0xea, 0xaf,0x25,0x8b,0x55,0x34,0x6d};
IID IID_Resource = {0x696442be, 0xa72e, 0x4059, 0xbc,0x79, 0x5b,0x5c,0x98,0x04,0x0f,0xad};
IID IID_StateObjectProps = {0xde5fa827, 0x9bf9, 0x4f26, 0x89,0xff, 0xd7,0xf5,0x6f,0xde,0x38,0x60};
IID IID_Fence = {0x0a753dcf, 0xc4d8, 0x4b91, 0xad,0xf6, 0xbe,0x5a,0x60,0xd9,0x5a,0x76};

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

typedef struct  {
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

static D3D12 s_D3D12 = {0};

// ==================================================
// Helper Functions
// ==================================================

static DXGI_FORMAT formatToD3D12(PalFormat format)
{
    switch (format) {
        case PAL_FORMAT_R8_UNORM:
            return DXGI_FORMAT_R8_UNORM;

        case PAL_FORMAT_R8_SNORM:
            return DXGI_FORMAT_R8_SNORM;

        case PAL_FORMAT_R8_UINT:
            return DXGI_FORMAT_R8_UINT;

        case PAL_FORMAT_R8_SINT:
            return DXGI_FORMAT_R8_SINT;

        case PAL_FORMAT_R8_SRGB:
            return DXGI_FORMAT_UNKNOWN;

        case PAL_FORMAT_R16_UNORM:
            return DXGI_FORMAT_R16_UNORM;

        case PAL_FORMAT_R16_SNORM:
            return DXGI_FORMAT_R16_SNORM;

        case PAL_FORMAT_R16_UINT:
            return DXGI_FORMAT_R16_UINT;

        case PAL_FORMAT_R16_SINT:
            return DXGI_FORMAT_R16_SINT;

        case PAL_FORMAT_R16_SFLOAT:
            return DXGI_FORMAT_R16_FLOAT;

        case PAL_FORMAT_R32_UINT:
            return DXGI_FORMAT_R32_UINT;

        case PAL_FORMAT_R32_SINT:
            return DXGI_FORMAT_R32_SINT;

        case PAL_FORMAT_R32_SFLOAT:
            return DXGI_FORMAT_R32_FLOAT;

        case PAL_FORMAT_R64_UINT:
            return DXGI_FORMAT_UNKNOWN;

        case PAL_FORMAT_R64_SINT:
            return DXGI_FORMAT_UNKNOWN;

        case PAL_FORMAT_R64_SFLOAT:
            return DXGI_FORMAT_UNKNOWN;

        case PAL_FORMAT_R8G8_UNORM:
            return DXGI_FORMAT_R8G8_UNORM;

        case PAL_FORMAT_R8G8_SNORM:
            return DXGI_FORMAT_R8G8_SNORM;

        case PAL_FORMAT_R8G8_UINT:
            return DXGI_FORMAT_R8G8_UINT;

        case PAL_FORMAT_R8G8_SINT:
            return DXGI_FORMAT_R8G8_SINT;

        case PAL_FORMAT_R8G8_SRGB:
            return DXGI_FORMAT_UNKNOWN;

        case PAL_FORMAT_R16G16_UNORM:
            return DXGI_FORMAT_R16G16_UNORM;

        case PAL_FORMAT_R16G16_SNORM:
            return DXGI_FORMAT_R16G16_SNORM;

        case PAL_FORMAT_R16G16_UINT:
            return DXGI_FORMAT_R16G16_UINT;

        case PAL_FORMAT_R16G16_SINT:
            return DXGI_FORMAT_R16G16_SINT;

        case PAL_FORMAT_R16G16_SFLOAT:
            return DXGI_FORMAT_R16G16_FLOAT;

        case PAL_FORMAT_R32G32_UINT:
            return DXGI_FORMAT_R32G32_UINT;

        case PAL_FORMAT_R32G32_SINT:
            return DXGI_FORMAT_R32G32_SINT;

        case PAL_FORMAT_R32G32_SFLOAT:
            return DXGI_FORMAT_R32G32_FLOAT;

        case PAL_FORMAT_R64G64_UINT:
            return DXGI_FORMAT_UNKNOWN;

        case PAL_FORMAT_R64G64_SINT:
            return DXGI_FORMAT_UNKNOWN;

        case PAL_FORMAT_R64G64_SFLOAT:
            return DXGI_FORMAT_UNKNOWN;

        case PAL_FORMAT_R8G8B8_UNORM:
            return DXGI_FORMAT_UNKNOWN;

        case PAL_FORMAT_R8G8B8_SNORM:
            return DXGI_FORMAT_UNKNOWN;

        case PAL_FORMAT_R8G8B8_UINT:
            return DXGI_FORMAT_UNKNOWN;

        case PAL_FORMAT_R8G8B8_SINT:
            return DXGI_FORMAT_UNKNOWN;

        case PAL_FORMAT_R8G8B8_SRGB:
            return DXGI_FORMAT_UNKNOWN;

        case PAL_FORMAT_R16G16B16_UNORM:
            return DXGI_FORMAT_UNKNOWN;

        case PAL_FORMAT_R16G16B16_SNORM:
            return DXGI_FORMAT_UNKNOWN;

        case PAL_FORMAT_R16G16B16_UINT:
            return DXGI_FORMAT_UNKNOWN;

        case PAL_FORMAT_R16G16B16_SINT:
            return DXGI_FORMAT_UNKNOWN;

        case PAL_FORMAT_R16G16B16_SFLOAT:
            return DXGI_FORMAT_UNKNOWN;

        case PAL_FORMAT_R32G32B32_UINT:
            return DXGI_FORMAT_R32G32B32_UINT;

        case PAL_FORMAT_R32G32B32_SINT:
            return DXGI_FORMAT_R32G32B32_SINT;

        case PAL_FORMAT_R32G32B32_SFLOAT:
            return DXGI_FORMAT_R32G32B32_FLOAT;

        case PAL_FORMAT_R64G64B64_UINT:
            return DXGI_FORMAT_UNKNOWN;

        case PAL_FORMAT_R64G64B64_SINT:
            return DXGI_FORMAT_UNKNOWN;

        case PAL_FORMAT_R64G64B64_SFLOAT:
            return DXGI_FORMAT_UNKNOWN;

        case PAL_FORMAT_B8G8R8_UNORM:
            return DXGI_FORMAT_UNKNOWN;

        case PAL_FORMAT_B8G8R8_SNORM:
            return DXGI_FORMAT_UNKNOWN;

        case PAL_FORMAT_B8G8R8_UINT:
            return DXGI_FORMAT_UNKNOWN;

        case PAL_FORMAT_B8G8R8_SINT:
            return DXGI_FORMAT_UNKNOWN;

        case PAL_FORMAT_B8G8R8_SRGB:
            return DXGI_FORMAT_UNKNOWN;

        case PAL_FORMAT_R8G8B8A8_UNORM:
            return DXGI_FORMAT_R8G8B8A8_UNORM;

        case PAL_FORMAT_R8G8B8A8_SNORM:
            return DXGI_FORMAT_R8G8B8A8_SNORM;

        case PAL_FORMAT_R8G8B8A8_UINT:
            return DXGI_FORMAT_R8G8B8A8_UINT;

        case PAL_FORMAT_R8G8B8A8_SINT:
            return DXGI_FORMAT_R8G8B8A8_SINT;

        case PAL_FORMAT_R8G8B8A8_SRGB:
            return DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;

        case PAL_FORMAT_R16G16B16A16_UNORM:
            return DXGI_FORMAT_R16G16B16A16_UNORM;

        case PAL_FORMAT_R16G16B16A16_SNORM:
            return DXGI_FORMAT_R16G16B16A16_SNORM;

        case PAL_FORMAT_R16G16B16A16_UINT:
            return DXGI_FORMAT_R16G16B16A16_UINT;

        case PAL_FORMAT_R16G16B16A16_SINT:
            return DXGI_FORMAT_R16G16B16A16_SINT;

        case PAL_FORMAT_R16G16B16A16_SFLOAT:
            return DXGI_FORMAT_R16G16B16A16_FLOAT;

        case PAL_FORMAT_R32G32B32A32_UINT:
            return DXGI_FORMAT_R32G32B32A32_UINT;

        case PAL_FORMAT_R32G32B32A32_SINT:
            return DXGI_FORMAT_R32G32B32A32_SINT;

        case PAL_FORMAT_R32G32B32A32_SFLOAT:
            return DXGI_FORMAT_R32G32B32A32_FLOAT;

        case PAL_FORMAT_R64G64B64A64_UINT:
            return DXGI_FORMAT_UNKNOWN;

        case PAL_FORMAT_R64G64B64A64_SINT:
            return DXGI_FORMAT_UNKNOWN;

        case PAL_FORMAT_R64G64B64A64_SFLOAT:
            return DXGI_FORMAT_UNKNOWN;

        case PAL_FORMAT_B8G8R8A8_UNORM:
            return DXGI_FORMAT_B8G8R8A8_UNORM;

        case PAL_FORMAT_B8G8R8A8_SNORM:
            return DXGI_FORMAT_UNKNOWN;

        case PAL_FORMAT_B8G8R8A8_UINT:
            return DXGI_FORMAT_UNKNOWN;

        case PAL_FORMAT_B8G8R8A8_SINT:
            return DXGI_FORMAT_UNKNOWN;

        case PAL_FORMAT_B8G8R8A8_SRGB:
            return DXGI_FORMAT_B8G8R8A8_UNORM_SRGB;

        case PAL_FORMAT_S8_UINT:
            return DXGI_FORMAT_UNKNOWN;

        case PAL_FORMAT_D16_UNORM:
            return DXGI_FORMAT_D16_UNORM;

        case PAL_FORMAT_D32_SFLOAT:
            return DXGI_FORMAT_D32_FLOAT;

        case PAL_FORMAT_D32_SFLOAT_S8_UINT:
            return DXGI_FORMAT_D32_FLOAT_S8X24_UINT;

        case PAL_FORMAT_D16_UNORM_S8_UINT:
            return DXGI_FORMAT_UNKNOWN;

        case PAL_FORMAT_D24_UNORM_S8_UINT:
            return DXGI_FORMAT_D24_UNORM_S8_UINT;
    }

    return DXGI_FORMAT_UNKNOWN;
}

static PalImageUsages ImageUsageFromD3D12(D3D12_FORMAT_SUPPORT1 flags)
{
    PalImageUsages usages = 0;
    if (flags & D3D12_FORMAT_SUPPORT1_RENDER_TARGET) {
        usages |= PAL_IMAGE_USAGE_COLOR_ATTACHEMENT;
    }

    if (flags & D3D12_FORMAT_SUPPORT1_DEPTH_STENCIL) {
        usages |= PAL_IMAGE_USAGE_DEPTH_ATTACHEMENT;
    }

    if (flags & D3D12_FORMAT_SUPPORT1_SHADER_SAMPLE) {
        usages |= PAL_IMAGE_USAGE_SAMPLED;
    }

    usages |= PAL_IMAGE_USAGE_TRANSFER_DST;
    usages |= PAL_IMAGE_USAGE_TRANSFER_SRC;
    return usages;
}

static uint32_t samplesToD3D12(PalSampleCount count)
{
    switch (count) {
        case PAL_SAMPLE_COUNT_2:
            return 2;

        case PAL_SAMPLE_COUNT_4:
            return 4;

        case PAL_SAMPLE_COUNT_8:
            return 8;

        case PAL_SAMPLE_COUNT_16:
            return 16;

        case PAL_SAMPLE_COUNT_32:
            return 32;

        case PAL_SAMPLE_COUNT_64:
            return 64;
    }

    return 1;
}

static D3D12_COMPARISON_FUNC compareOpToD3D12(PalCompareOp op)
{
    switch (op) {
        case PAL_COMPARE_OP_NEVER:
            return D3D12_COMPARISON_FUNC_NEVER;

        case PAL_COMPARE_OP_LESS:
            return D3D12_COMPARISON_FUNC_LESS;

        case PAL_COMPARE_OP_EQUAL:
            return D3D12_COMPARISON_FUNC_EQUAL;

        case PAL_COMPARE_OP_LESS_OR_EQUAL:
            return D3D12_COMPARISON_FUNC_LESS_EQUAL;

        case PAL_COMPARE_OP_GREATER:
            return D3D12_COMPARISON_FUNC_GREATER;

        case PAL_COMPARE_OP_NOT_EQUAL:
            return D3D12_COMPARISON_FUNC_NOT_EQUAL;

        case PAL_COMPARE_OP_GREATER_OR_EQUAL:
            return D3D12_COMPARISON_FUNC_GREATER_EQUAL;

        case PAL_COMPARE_OP_ALWAYS:
            return D3D12_COMPARISON_FUNC_ALWAYS;
    }

    return D3D12_COMPARISON_FUNC_NEVER;
}

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

        case PAL_BLEND_FACTOR_ONE_MINUX_DST_COLOR:
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

static D3D12_FILTER filterToD3D12(
    PalFilterMode minFilter,
    PalFilterMode magFilter,
    PalSamplerMipmapMode mode)
{
    // all the enums start with min so we start with min filter
    switch (minFilter) {
        case PAL_FILTER_MODE_NEAREST: {
            switch (magFilter) {
                case PAL_FILTER_MODE_NEAREST: {
                    // min and mag are nearest. Check sampler mipmap mode
                    if (mode == PAL_SAMPLER_MIPMAP_MODE_NEAREST) {
                        return D3D12_FILTER_MIN_MAG_MIP_POINT; // sampler mode nearest
                    } else {
                        return D3D12_FILTER_MIN_MAG_POINT_MIP_LINEAR; // sampler mode linear
                    }
                }

                case PAL_FILTER_MODE_LINEAR: {
                    // min is nearest, mag is linear . Check sampler mipmap mode
                    if (mode == PAL_SAMPLER_MIPMAP_MODE_NEAREST) {
                        return D3D12_FILTER_MIN_POINT_MAG_LINEAR_MIP_POINT; // sampler mode nearest
                    } else {
                        return D3D12_FILTER_MIN_POINT_MAG_MIP_LINEAR; // sampler mode linear
                    }
                }
            }
        }

        case PAL_FILTER_MODE_LINEAR: {
            switch (magFilter) {
                case PAL_FILTER_MODE_NEAREST: {
                    // min is linear, mag is nearest. Check sampler mipmap mode
                    if (mode == PAL_SAMPLER_MIPMAP_MODE_NEAREST) {
                        return D3D12_FILTER_MIN_LINEAR_MAG_MIP_POINT; // sampler mode nearest
                    } else {
                        return D3D12_FILTER_MIN_LINEAR_MAG_POINT_MIP_LINEAR; // sampler mode linear
                    }
                }

                case PAL_FILTER_MODE_LINEAR: {
                    // min and mag are linear. Check sampler mipmap mode
                    if (mode == PAL_SAMPLER_MIPMAP_MODE_NEAREST) {
                        return D3D12_FILTER_MIN_MAG_LINEAR_MIP_POINT; // sampler mode nearest
                    } else {
                        return D3D12_FILTER_MIN_MAG_MIP_LINEAR; // sampler mode linear
                    }
                }
            }
        }
    }

    return D3D12_FILTER_MIN_MAG_MIP_POINT;
}

static D3D12_TEXTURE_ADDRESS_MODE addressModeToD3D12(PalSamplerAddressMode mode)
{
    switch (mode) {
        case PAL_SAMPLER_ADDRESS_MODE_REPEAT: {
            return D3D12_TEXTURE_ADDRESS_MODE_WRAP;
        }

        case PAL_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT: {
            return D3D12_TEXTURE_ADDRESS_MODE_MIRROR;

        }
        case PAL_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE: {
            return D3D12_TEXTURE_ADDRESS_MODE_CLAMP;

        }
        case PAL_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER: {
            return D3D12_TEXTURE_ADDRESS_MODE_BORDER;
        }
    }
    return D3D12_TEXTURE_ADDRESS_MODE_WRAP;
}

static void borderColorToD3D12(PalBorderColor color, float outColor[4])
{
    switch (color) {
        case PAL_BORDER_COLOR_FLOAT_TRANSPARENT_BLACK:
        case PAL_BORDER_COLOR_INT_TRANSPARENT_BLACK: {
            outColor[0] = 0.0f;
            outColor[1] = 0.0f;
            outColor[2] = 0.0f;
            outColor[3] = 0.0f;
            break;
        }

        case PAL_BORDER_COLOR_FLOAT_OPAQUE_BLACK:
        case PAL_BORDER_COLOR_INT_OPAQUE_BLACK: {
            outColor[0] = 0.0f;
            outColor[1] = 0.0f;
            outColor[2] = 0.0f;
            outColor[3] = 1.0f;
            break;
        }

        case PAL_BORDER_COLOR_FLOAT_OPAQUE_WHITE:
        case PAL_BORDER_COLOR_INT_OPAQUE_WHITE: {
            outColor[0] = 1.0f;
            outColor[1] = 1.0f;
            outColor[2] = 1.0f;
            outColor[3] = 1.0f;
            break;
        }
    }

    outColor[0] = 0.0f;
    outColor[1] = 0.0f;
    outColor[2] = 0.0f;
    outColor[3] = 0.0f;
}

static CommandBufferData* getFreeCmdBufferData(CommandPool* pool)
{
    for (int i = 0; i < pool->size; ++i) {
        if (!pool->cmdBuffersData[i].used) {
            pool->cmdBuffersData[i].used = PAL_TRUE;
            return &pool->cmdBuffersData[i];
        }
    }

    // resize the data array
    CommandBufferData* data = nullptr;
    int count = pool->size * 2; // double the size
    int freeIndex = pool->size + 1;

    data = palAllocate(s_D3D12.allocator, sizeof(CommandBufferData) * count, 0);
    if (data) {
        memcpy(data, pool->cmdBuffersData, pool->size * sizeof(CommandBufferData));

        palFree(s_D3D12.allocator, pool->cmdBuffersData);
        pool->cmdBuffersData = data;
        pool->size = count;

        pool->cmdBuffersData[freeIndex].used = PAL_TRUE;
        return &pool->cmdBuffersData[freeIndex];
    }
    return nullptr;
}

static CommandBufferData* findCmdBufferData(
    CommandPool* pool,
    CommandBuffer* cmdBuffer)
{
    for (int i = 0; i < pool->size; ++i) {
        if (pool->cmdBuffersData[i].used && pool->cmdBuffersData[i].cmdBuffer == cmdBuffer) {
            return &pool->cmdBuffersData[i];
        }
    }
    return nullptr;
}

static D3D12_SHADING_RATE_COMBINER combinerOpsToD3D12(PalFragmentShadingRateCombinerOp op)
{
    switch (op) {
        case PAL_FRAGMENT_SHADING_RATE_COMBINER_OP_KEEP:
            return D3D12_SHADING_RATE_COMBINER_PASSTHROUGH;

        case PAL_FRAGMENT_SHADING_RATE_COMBINER_OP_REPLACE:
            return D3D12_SHADING_RATE_COMBINER_OVERRIDE;

        case PAL_FRAGMENT_SHADING_RATE_COMBINER_OP_MIN:
            return D3D12_SHADING_RATE_COMBINER_MIN;

        case PAL_FRAGMENT_SHADING_RATE_COMBINER_OP_MAX:
            return D3D12_SHADING_RATE_COMBINER_MAX;

        case PAL_FRAGMENT_SHADING_RATE_COMBINER_OP_MUL:
            return D3D12_SHADING_RATE_COMBINER_SUM;
    }

    return D3D12_SHADING_RATE_COMBINER_PASSTHROUGH;
}

static D3D12_SHADING_RATE shadingRateToD3D12(PalFragmentShadingRate rate)
{
    switch (rate) {
        case PAL_FRAGMENT_SHADING_RATE_1X1:
            return D3D12_SHADING_RATE_1X1;

        case PAL_FRAGMENT_SHADING_RATE_1X2:
            return D3D12_SHADING_RATE_1X2;

        case PAL_FRAGMENT_SHADING_RATE_2X1:
            return D3D12_SHADING_RATE_2X1;

        case PAL_FRAGMENT_SHADING_RATE_2X2:
            return D3D12_SHADING_RATE_2X2;

        case PAL_FRAGMENT_SHADING_RATE_2X4:
            return D3D12_SHADING_RATE_2X4;

        case PAL_FRAGMENT_SHADING_RATE_4X2:
            return D3D12_SHADING_RATE_4X2;

        case PAL_FRAGMENT_SHADING_RATE_4X4:
            return D3D12_SHADING_RATE_4X4;
    }

    return D3D12_SHADING_RATE_1X1;
}

static DXGI_FORMAT vertexTypeToD3D12(PalVertexType type)
{
    switch (type) {
        case PAL_VERTEX_TYPE_INT32:
            return DXGI_FORMAT_R32_SINT;

        case PAL_VERTEX_TYPE_INT32_2:
            return DXGI_FORMAT_R32G32_SINT;

        case PAL_VERTEX_TYPE_INT32_3:
            return DXGI_FORMAT_R32G32B32_SINT;

        case PAL_VERTEX_TYPE_INT32_4:
            return DXGI_FORMAT_R32G32B32A32_SINT;

        case PAL_VERTEX_TYPE_UINT32:
            return DXGI_FORMAT_R32_UINT;

        case PAL_VERTEX_TYPE_UINT32_2:
            return DXGI_FORMAT_R32G32_UINT;

        case PAL_VERTEX_TYPE_UINT32_3:
            return DXGI_FORMAT_R32G32B32_UINT;

        case PAL_VERTEX_TYPE_UINT32_4:
            return DXGI_FORMAT_R32G32B32A32_UINT;

        case PAL_VERTEX_TYPE_INT8_2:
            return DXGI_FORMAT_R8G8_SINT;

        case PAL_VERTEX_TYPE_INT8_4:
            return DXGI_FORMAT_R8G8B8A8_SINT;

        case PAL_VERTEX_TYPE_UINT8_2:
            return DXGI_FORMAT_R8G8_UINT;

        case PAL_VERTEX_TYPE_UINT8_4:
            return DXGI_FORMAT_R8G8B8A8_UINT;

        case PAL_VERTEX_TYPE_INT8_2NORM:
            return DXGI_FORMAT_R8G8_SNORM;

        case PAL_VERTEX_TYPE_INT8_4NORM:
            return DXGI_FORMAT_R8G8B8A8_SNORM;

        case PAL_VERTEX_TYPE_UINT8_2NORM:
            return DXGI_FORMAT_R8G8_UNORM;

        case PAL_VERTEX_TYPE_UINT8_4NORM:
            return DXGI_FORMAT_R8G8B8A8_UNORM;

        case PAL_VERTEX_TYPE_INT16_2:
            return DXGI_FORMAT_R16G16_SINT;

        case PAL_VERTEX_TYPE_INT16_4:
            return DXGI_FORMAT_R16G16B16A16_SINT;

        case PAL_VERTEX_TYPE_UINT16_2:
            return DXGI_FORMAT_R16G16_UINT;

        case PAL_VERTEX_TYPE_UINT16_4:
            return DXGI_FORMAT_R16G16B16A16_UINT;

        case PAL_VERTEX_TYPE_INT16_2NORM:
            return DXGI_FORMAT_R16G16_SNORM;

        case PAL_VERTEX_TYPE_INT16_4NORM:
            return DXGI_FORMAT_R16G16B16A16_SNORM;

        case PAL_VERTEX_TYPE_UINT16_2NORM:
            return DXGI_FORMAT_R16G16_UNORM;

        case PAL_VERTEX_TYPE_UINT16_4NORM:
            return DXGI_FORMAT_R16G16B16A16_UNORM;

        case PAL_VERTEX_TYPE_FLOAT:
            return DXGI_FORMAT_R32_FLOAT;

        case PAL_VERTEX_TYPE_FLOAT2:
            return DXGI_FORMAT_R32G32_FLOAT;

        case PAL_VERTEX_TYPE_FLOAT3:
            return DXGI_FORMAT_R32G32B32_FLOAT;

        case PAL_VERTEX_TYPE_FLOAT4:
            return DXGI_FORMAT_R32G32B32A32_FLOAT;

        case PAL_VERTEX_TYPE_HALF_FLOAT16_2:
            return DXGI_FORMAT_R16G16_FLOAT;

        case PAL_VERTEX_TYPE_HALF_FLOAT16_4:
            return DXGI_FORMAT_R16G16B16A16_FLOAT;
    }

    return DXGI_FORMAT_UNKNOWN;
}

static PalBool fillBuildInfoD3D12(
    PalAccelerationStructureBuildInfo* info,
    D3D12_RAYTRACING_GEOMETRY_DESC* geometries,
    D3D12_GPU_VIRTUAL_ADDRESS srcAs,
    D3D12_GPU_VIRTUAL_ADDRESS dstAs,
    D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_DESC* buildInfo)
{
    static uint32_t maxInstanceCount = 1000000;
    static uint32_t maxPrimitiveCount = 10000000;
    static uint32_t maxGeometryCount = 100000;

    if (info->geometryCount > maxGeometryCount) {
        return PAL_FALSE;
    }

    if (info->instanceCount > maxInstanceCount) {
        return PAL_FALSE;
    }

    for (int i = 0; i < info->geometryCount; i++) {
        if (info->geometries[i].primitiveCount > maxPrimitiveCount) {
            return PAL_FALSE;
        }

        D3D12_RAYTRACING_GEOMETRY_DESC* tmp = &geometries[i];
        tmp->Flags = D3D12_RAYTRACING_GEOMETRY_FLAG_OPAQUE;
        if (info->geometries[i].flags & PAL_GEOMETRY_FLAG_NO_DUPLICATE_ANYHIT) {
            tmp->Flags = D3D12_RAYTRACING_GEOMETRY_FLAG_NO_DUPLICATE_ANYHIT_INVOCATION;
        }

        if (info->geometries[i].type == PAL_GEOMETRY_TYPE_TRIANGLE) {
            tmp->Type = D3D12_RAYTRACING_GEOMETRY_TYPE_TRIANGLES;
            PalGeometryDataTriangle* tmpData = info->geometries[i].data;

            tmp->Triangles.VertexBuffer.StartAddress = tmpData->vertexBufferAddress;
            tmp->Triangles.VertexBuffer.StrideInBytes = tmpData->vertexStride;
            tmp->Triangles.VertexCount = tmpData->vertexCount;
            tmp->Triangles.VertexFormat = vertexTypeToD3D12(tmpData->vertexType);

            tmp->Triangles.IndexBuffer = tmpData->indexBufferAddress;
            tmp->Triangles.IndexCount = tmpData->indexCount;
            if (tmpData->indexType == PAL_INDEX_TYPE_UINT16) {
                if (tmp->Triangles.IndexBuffer) {
                    tmp->Triangles.IndexFormat = DXGI_FORMAT_R16_FLOAT;
                }
                
            } else {
                if (tmp->Triangles.IndexBuffer) {
                    tmp->Triangles.IndexFormat = DXGI_FORMAT_R32_FLOAT;
                }
            }

        } else if (info->geometries[i].type == PAL_GEOMETRY_TYPE_AABBS) {
            tmp->Type = D3D12_RAYTRACING_GEOMETRY_TYPE_PROCEDURAL_PRIMITIVE_AABBS;
            PalGeometryDataAABBS* tmpData = info->geometries[i].data;

            tmp->AABBs.AABBCount = info->geometries[i].primitiveCount;
            tmp->AABBs.AABBs.StartAddress = tmpData->bufferAddress;
            tmp->AABBs.AABBs.StrideInBytes = tmpData->stride;
        }
    }

    if (info->type == PAL_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL) {
        buildInfo->Inputs.NumDescs = info->instanceCount;
        buildInfo->Inputs.InstanceDescs = info->instanceBufferAddress;
    } else {
        buildInfo->Inputs.NumDescs = info->geometryCount;
        buildInfo->Inputs.pGeometryDescs = geometries;
    }

    buildInfo->Inputs.DescsLayout = D3D12_ELEMENTS_LAYOUT_ARRAY;
    if (info->type == PAL_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL) {
        buildInfo->Inputs.Type = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL;
    } else {
        buildInfo->Inputs.Type = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL;
    }

    // build mode
    buildInfo->Inputs.Flags = 0;
    if (info->buildMode == PAL_ACCELERATION_STRUCTURE_BUILD_MODE_UPDATE) {
        buildInfo->Inputs.Flags = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_ALLOW_UPDATE;
    }

    // build hints
    if (info->buildHints & PAL_ACCELERATION_STRUCTURE_BUILD_HINT_FAST_BUILD) {
        buildInfo->Inputs.Flags |= D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_PREFER_FAST_BUILD;
    }

    if (info->buildHints & PAL_ACCELERATION_STRUCTURE_BUILD_HINT_FAST_TRACE) {
        buildInfo->Inputs.Flags |= D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_PREFER_FAST_TRACE;
    }

    if (info->buildHints & PAL_ACCELERATION_STRUCTURE_BUILD_HINT_LOW_MEMORY) {
        buildInfo->Inputs.Flags |= D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_MINIMIZE_MEMORY;
    }

    buildInfo->SourceAccelerationStructureData = srcAs;
    buildInfo->DestAccelerationStructureData = dstAs;
    buildInfo->ScratchAccelerationStructureData = info->scratchBufferAddress;
    
    return PAL_TRUE;
}

static uint32_t getFormatSizeD3D12(PalFormat format)
{
    switch (format) {
        case PAL_FORMAT_R8_UNORM:
        case PAL_FORMAT_R8_SNORM:
        case PAL_FORMAT_R8_UINT:
        case PAL_FORMAT_R8_SINT:
        case PAL_FORMAT_R8_SRGB:
        case PAL_FORMAT_S8_UINT:
            return 1;

        case PAL_FORMAT_R16_UNORM:
        case PAL_FORMAT_R16_SNORM:
        case PAL_FORMAT_R16_UINT:
        case PAL_FORMAT_R16_SINT:
        case PAL_FORMAT_R16_SFLOAT:
        case PAL_FORMAT_R8G8_UNORM:
        case PAL_FORMAT_R8G8_SNORM:
        case PAL_FORMAT_R8G8_UINT:
        case PAL_FORMAT_R8G8_SINT:
        case PAL_FORMAT_R8G8_SRGB:
        case PAL_FORMAT_D16_UNORM:
            return 2;

        case PAL_FORMAT_R8G8B8_UNORM:
        case PAL_FORMAT_R8G8B8_SNORM:
        case PAL_FORMAT_R8G8B8_UINT:
        case PAL_FORMAT_R8G8B8_SINT:
        case PAL_FORMAT_R8G8B8_SRGB:
        case PAL_FORMAT_B8G8R8_UNORM:
        case PAL_FORMAT_B8G8R8_SNORM:
        case PAL_FORMAT_B8G8R8_UINT:
        case PAL_FORMAT_B8G8R8_SINT:
        case PAL_FORMAT_B8G8R8_SRGB:
        case PAL_FORMAT_D16_UNORM_S8_UINT:
            return 3;

        case PAL_FORMAT_R32_UINT:
        case PAL_FORMAT_R32_SINT:
        case PAL_FORMAT_R32_SFLOAT:
        case PAL_FORMAT_R16G16_UNORM:
        case PAL_FORMAT_R16G16_SNORM:
        case PAL_FORMAT_R16G16_UINT:
        case PAL_FORMAT_R16G16_SINT:
        case PAL_FORMAT_R16G16_SFLOAT:
        case PAL_FORMAT_R8G8B8A8_UNORM:
        case PAL_FORMAT_R8G8B8A8_SNORM:
        case PAL_FORMAT_R8G8B8A8_UINT:
        case PAL_FORMAT_R8G8B8A8_SINT:
        case PAL_FORMAT_R8G8B8A8_SRGB:
        case PAL_FORMAT_B8G8R8A8_UNORM:
        case PAL_FORMAT_B8G8R8A8_SNORM:
        case PAL_FORMAT_B8G8R8A8_UINT:
        case PAL_FORMAT_B8G8R8A8_SINT:
        case PAL_FORMAT_B8G8R8A8_SRGB:
        case PAL_FORMAT_D32_SFLOAT:
        case PAL_FORMAT_D24_UNORM_S8_UINT:
            return 4;

        case PAL_FORMAT_D32_SFLOAT_S8_UINT:
            return 5;

        case PAL_FORMAT_R16G16B16_UNORM:
        case PAL_FORMAT_R16G16B16_SNORM:
        case PAL_FORMAT_R16G16B16_UINT:
        case PAL_FORMAT_R16G16B16_SINT:
        case PAL_FORMAT_R16G16B16_SFLOAT:
            return 6;

        case PAL_FORMAT_R64_UINT:
        case PAL_FORMAT_R64_SINT:
        case PAL_FORMAT_R64_SFLOAT:
        case PAL_FORMAT_R32G32_UINT:
        case PAL_FORMAT_R32G32_SINT:
        case PAL_FORMAT_R32G32_SFLOAT:
        case PAL_FORMAT_R16G16B16A16_UNORM:
        case PAL_FORMAT_R16G16B16A16_SNORM:
        case PAL_FORMAT_R16G16B16A16_UINT:
        case PAL_FORMAT_R16G16B16A16_SINT:
        case PAL_FORMAT_R16G16B16A16_SFLOAT:
            return 8;

        case PAL_FORMAT_R32G32B32_UINT:
        case PAL_FORMAT_R32G32B32_SINT:
        case PAL_FORMAT_R32G32B32_SFLOAT:
            return 12;

        case PAL_FORMAT_R64G64_UINT:
        case PAL_FORMAT_R64G64_SINT:
        case PAL_FORMAT_R64G64_SFLOAT:
        case PAL_FORMAT_R32G32B32A32_UINT:
        case PAL_FORMAT_R32G32B32A32_SINT:
        case PAL_FORMAT_R32G32B32A32_SFLOAT:
            return 16;

        case PAL_FORMAT_R64G64B64_UINT:
        case PAL_FORMAT_R64G64B64_SINT:
        case PAL_FORMAT_R64G64B64_SFLOAT:
            return 24;

        case PAL_FORMAT_R64G64B64A64_UINT:
        case PAL_FORMAT_R64G64B64A64_SINT:
        case PAL_FORMAT_R64G64B64A64_SFLOAT:
            return 32;
    }

    return 0;
}

static inline uint32_t alignD3D12(
    uint32_t value,
    uint32_t alignment)
{
    return (value + alignment - 1) & ~(alignment - 1);
}

static D3D12_RESOURCE_STATES barrierToD3D12(
    uint32_t stageCount,
    PalUsageState state,
    PalShaderStage* shaderStages)
{
    switch (state) {
        case PAL_USAGE_STATE_UNDEFINED: {
            return D3D12_RESOURCE_STATE_COMMON;
        }

        case PAL_USAGE_STATE_PRESENT: {
            return D3D12_RESOURCE_STATE_PRESENT;
        }

        case PAL_USAGE_STATE_COLOR_ATTACHMENT_WRITE: {
            return D3D12_RESOURCE_STATE_RENDER_TARGET;
        }

        case PAL_USAGE_STATE_DEPTH_ATTACHMENT_READ:
        case PAL_USAGE_STATE_STENCIL_ATTACHMENT_READ: {
            return D3D12_RESOURCE_STATE_DEPTH_READ;
        }

        case PAL_USAGE_STATE_DEPTH_ATTACHMENT_WRITE:
        case PAL_USAGE_STATE_STENCIL_ATTACHMENT_WRITE: {
            return D3D12_RESOURCE_STATE_DEPTH_WRITE;
        }

        case PAL_USAGE_STATE_FRAGMENT_SHADING_RATE_ATTACHMENT_READ: {
            return D3D12_RESOURCE_STATE_SHADING_RATE_SOURCE;
        }

        case PAL_USAGE_STATE_TRANSFER_READ: {
            return D3D12_RESOURCE_STATE_COPY_SOURCE;
        }

        case PAL_USAGE_STATE_TRANSFER_WRITE:
        case PAL_USAGE_STATE_HOST_READ: {
            return D3D12_RESOURCE_STATE_COPY_DEST;
        }

        case PAL_USAGE_STATE_VERTEX_READ: {
            return D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER;
        }

        case PAL_USAGE_STATE_INDEX_READ: {
            return D3D12_RESOURCE_STATE_INDEX_BUFFER;
        }

        case PAL_USAGE_STATE_INDIRECT_READ: {
            return D3D12_RESOURCE_STATE_INDIRECT_ARGUMENT;
        }

        case PAL_USAGE_STATE_UNIFORM_READ: {
            return D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER;
        }

        case PAL_USAGE_STATE_SHADER_READ: {
            if (stageCount == 1) {
                if (shaderStages[0] == PAL_SHADER_STAGE_FRAGMENT) {
                    return D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
                } else {
                    return D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE;
                }
            } else {
                return D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE;
            }
        }

        case PAL_USAGE_STATE_STORAGE_READ:
        case PAL_USAGE_STATE_SHADER_WRITE:
        case PAL_USAGE_STATE_STORAGE_WRITE: {
            return D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
        }

        case PAL_USAGE_STATE_HOST_WRITE: {
            return D3D12_RESOURCE_STATE_GENERIC_READ;
        }

        case PAL_USAGE_STATE_ACCELERATION_STRUCTURE_READ:
        case PAL_USAGE_STATE_ACCELERATION_STRUCTURE_WRITE: {
            return D3D12_RESOURCE_STATE_RAYTRACING_ACCELERATION_STRUCTURE;
        }
    }

    return D3D12_RESOURCE_STATE_COMMON;
}

static void fillSubresourceD3D12(
    PalImageViewType type,
    const PalImageSubresourceRange* range,
    D3D12_RENDER_TARGET_VIEW_DESC* rtvDesc,
    D3D12_DEPTH_STENCIL_VIEW_DESC* dsvDesc,
    D3D12_SHADER_RESOURCE_VIEW_DESC* srvDesc,
    D3D12_UNORDERED_ACCESS_VIEW_DESC* uavDesc)
{
    if (rtvDesc) {
        if (type == PAL_IMAGE_VIEW_TYPE_1D) {
            rtvDesc->Texture1D.MipSlice = range->startMipLevel;
            rtvDesc->ViewDimension = D3D12_RTV_DIMENSION_TEXTURE1D;

        } else if (type == PAL_IMAGE_VIEW_TYPE_1D_ARRAY) {
            rtvDesc->Texture1DArray.MipSlice = range->startMipLevel;
            rtvDesc->Texture1DArray.FirstArraySlice = range->startArrayLayer;
            rtvDesc->Texture1DArray.ArraySize = range->layerArrayCount;
            rtvDesc->ViewDimension = D3D12_RTV_DIMENSION_TEXTURE1DARRAY;

        } else if (type == PAL_IMAGE_VIEW_TYPE_2D) {
            rtvDesc->Texture2D.MipSlice = range->startMipLevel;
            rtvDesc->ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;

        } else if (type == PAL_IMAGE_VIEW_TYPE_2D_ARRAY) {
            rtvDesc->Texture2DArray.MipSlice = range->startMipLevel;
            rtvDesc->Texture2DArray.FirstArraySlice = range->startArrayLayer;
            rtvDesc->Texture2DArray.ArraySize = range->layerArrayCount;
            rtvDesc->ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2DARRAY;

        } else if (type == PAL_IMAGE_VIEW_TYPE_3D) {
            rtvDesc->Texture3D.MipSlice = range->startMipLevel;
            rtvDesc->Texture3D.FirstWSlice = range->startArrayLayer;
            rtvDesc->Texture3D.WSize = range->layerArrayCount;
            rtvDesc->ViewDimension = D3D12_RTV_DIMENSION_TEXTURE3D;
        }
        return;

    } else if (dsvDesc) {
        if (range->aspect == PAL_IMAGE_ASPECT_DEPTH) {
            dsvDesc->Flags = D3D12_DSV_FLAG_READ_ONLY_DEPTH;
        } else if (range->aspect == PAL_IMAGE_ASPECT_STENCIL) {
            dsvDesc->Flags = D3D12_DSV_FLAG_READ_ONLY_STENCIL;
        }

        if (type == PAL_IMAGE_VIEW_TYPE_1D) {
            dsvDesc->Texture1D.MipSlice = range->startMipLevel;
            dsvDesc->ViewDimension = D3D12_DSV_DIMENSION_TEXTURE1D;

        } else if (type == PAL_IMAGE_VIEW_TYPE_1D_ARRAY) {
            dsvDesc->Texture1DArray.MipSlice = range->startMipLevel;
            dsvDesc->Texture1DArray.FirstArraySlice = range->startArrayLayer;
            dsvDesc->Texture1DArray.ArraySize = range->layerArrayCount;
            dsvDesc->ViewDimension = D3D12_DSV_DIMENSION_TEXTURE1DARRAY;

        } else if (type == PAL_IMAGE_VIEW_TYPE_2D) {
            dsvDesc->Texture2D.MipSlice = range->startMipLevel;
            dsvDesc->ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;

        } else if (type == PAL_IMAGE_VIEW_TYPE_2D_ARRAY) {
            dsvDesc->Texture2DArray.MipSlice = range->startMipLevel;
            dsvDesc->Texture2DArray.FirstArraySlice = range->startArrayLayer;
            dsvDesc->Texture2DArray.ArraySize = range->layerArrayCount;
            dsvDesc->ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2DARRAY;
        }
        return;

    } else if (srvDesc) {
        if (type == PAL_IMAGE_VIEW_TYPE_1D) {
            srvDesc->Texture1D.MipLevels = range->mipLevelCount;
            srvDesc->Texture1D.MostDetailedMip = range->startMipLevel;
            srvDesc->ViewDimension = D3D12_SRV_DIMENSION_TEXTURE1D;

        } else if (type == PAL_IMAGE_VIEW_TYPE_1D_ARRAY) {
            srvDesc->Texture1DArray.MipLevels = range->mipLevelCount;
            srvDesc->Texture1DArray.MostDetailedMip = range->startMipLevel;
            srvDesc->Texture1DArray.FirstArraySlice = range->startArrayLayer;
            srvDesc->Texture1DArray.ArraySize = range->layerArrayCount;
            srvDesc->ViewDimension = D3D12_SRV_DIMENSION_TEXTURE1DARRAY;

        } else if (type == PAL_IMAGE_VIEW_TYPE_2D) {
            srvDesc->Texture2D.MipLevels = range->mipLevelCount;
            srvDesc->Texture2D.MostDetailedMip = range->startMipLevel;
            srvDesc->ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;

        } else if (type == PAL_IMAGE_VIEW_TYPE_2D_ARRAY) {
            srvDesc->Texture2DArray.MipLevels = range->mipLevelCount;
            srvDesc->Texture2DArray.MostDetailedMip = range->startMipLevel;
            srvDesc->Texture2DArray.FirstArraySlice = range->startArrayLayer;
            srvDesc->Texture2DArray.ArraySize = range->layerArrayCount;
            srvDesc->ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2DARRAY;

        } else if (type == PAL_IMAGE_VIEW_TYPE_3D) {
            srvDesc->Texture3D.MipLevels = range->mipLevelCount;
            srvDesc->Texture3D.MostDetailedMip = range->startMipLevel;
            srvDesc->ViewDimension = D3D12_SRV_DIMENSION_TEXTURE3D;

        } else if (type == PAL_IMAGE_VIEW_TYPE_CUBE) {
            srvDesc->TextureCube.MipLevels = range->mipLevelCount;
            srvDesc->TextureCube.MostDetailedMip = range->startMipLevel;
            srvDesc->ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBE;

        } else if (type == PAL_IMAGE_VIEW_TYPE_CUBE_ARRAY) {
            srvDesc->TextureCubeArray.MipLevels = range->mipLevelCount;
            srvDesc->TextureCubeArray.MostDetailedMip = range->startMipLevel;
            srvDesc->TextureCubeArray.First2DArrayFace = range->startArrayLayer;
            srvDesc->TextureCubeArray.NumCubes = range->layerArrayCount;
            srvDesc->ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBEARRAY;
        }
        return;

    } else if (uavDesc) {
        if (type == PAL_IMAGE_VIEW_TYPE_1D) {
            uavDesc->Texture1D.MipSlice = range->startMipLevel;
            uavDesc->ViewDimension = D3D12_UAV_DIMENSION_TEXTURE1D;

        } else if (type == PAL_IMAGE_VIEW_TYPE_1D_ARRAY) {
            uavDesc->Texture1DArray.MipSlice = range->startMipLevel;
            uavDesc->Texture1DArray.FirstArraySlice = range->startArrayLayer;
            uavDesc->Texture1DArray.ArraySize = range->layerArrayCount;
            uavDesc->ViewDimension = D3D12_UAV_DIMENSION_TEXTURE1DARRAY;

        } else if (type == PAL_IMAGE_VIEW_TYPE_2D) {
            uavDesc->Texture2D.MipSlice = range->startMipLevel;
            uavDesc->ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2D;

        } else if (type == PAL_IMAGE_VIEW_TYPE_2D_ARRAY) {
            uavDesc->Texture2DArray.MipSlice = range->startMipLevel;
            uavDesc->Texture2DArray.FirstArraySlice = range->startArrayLayer;
            uavDesc->Texture2DArray.ArraySize = range->layerArrayCount;
            uavDesc->ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2DARRAY;

        } else if (type == PAL_IMAGE_VIEW_TYPE_3D) {
            uavDesc->Texture3D.MipSlice = range->startMipLevel;
            uavDesc->Texture3D.FirstWSlice = range->startArrayLayer;
            uavDesc->Texture3D.WSize = range->layerArrayCount;
            uavDesc->ViewDimension = D3D12_UAV_DIMENSION_TEXTURE3D;
        }
        return;
    }
}

static inline uint64_t getDescriptorHandleD3D12(
    uint32_t index,
    uint32_t size,
    uint64_t baseOffset)
{
    return baseOffset + index * size;
}

static D3D12_RAYTRACING_INSTANCE_FLAGS instanceFlagsToD3D12(
    PalAccelerationStructureInstanceFlags flags)
{
    D3D12_RAYTRACING_INSTANCE_FLAGS instanceFlags = 0;
    if (flags & PAL_ACCELERATION_STRUCTURE_INSTANCE_FLAG_FORCE_OPAQUE) {
        instanceFlags |= D3D12_RAYTRACING_INSTANCE_FLAG_FORCE_OPAQUE;
    }

    if (flags & PAL_ACCELERATION_STRUCTURE_INSTANCE_FLAG_FORCE_NO_OPAQUE) {
        instanceFlags |= D3D12_RAYTRACING_INSTANCE_FLAG_FORCE_NON_OPAQUE;
    }

    if (flags & PAL_ACCELERATION_STRUCTURE_INSTANCE_FLAG_TRIANGLE_FACING_CULL_DISABLE) {
        instanceFlags |= D3D12_RAYTRACING_INSTANCE_FLAG_TRIANGLE_CULL_DISABLE;
    }

    if (flags & PAL_ACCELERATION_STRUCTURE_INSTANCE_FLAG_TRIANGLE_FRONT_COUNTERCLOCKWISE) {
        instanceFlags |= D3D12_RAYTRACING_INSTANCE_FLAG_TRIANGLE_FRONT_COUNTERCLOCKWISE;
    }

    return instanceFlags;
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

static void convertToWcharD3D12(
    const char* src,
    wchar_t dst[PAL_SHADER_ENTRY_NAME_SIZE])
{
    int i = 0;
    for (; i < PAL_SHADER_ENTRY_NAME_SIZE - 1 && src[i]; i++) {
        dst[i] = (wchar_t)src[i];
    }
    dst[i] = L'\0';
}

static void getHitGroupNameD3D12(
    uint32_t index,
    wchar_t dst[PAL_SHADER_ENTRY_NAME_SIZE])
{
    wcscpy(dst, L"HitGroup");
    _itow(index, dst + 8, 10);
}

static void pollMessagesD3D12(Device* device)
{
    ID3D12InfoQueue* queue = device->infoQueue;
    if (!queue) {
        return;
    }

    UINT64 messageCount = queue->lpVtbl->GetNumStoredMessages(queue);
    for (int i = 0; i < messageCount; i++) {
        SIZE_T size = 0;
        queue->lpVtbl->GetMessage(queue, i, nullptr, &size);

        uint8_t* buffer = palAllocate(s_D3D12.allocator, size, 0);
        if (!buffer) {
            return;
        }

        queue->lpVtbl->GetMessage(queue, i, (D3D12_MESSAGE*)buffer, &size);
        const char* message = ((D3D12_MESSAGE*)buffer)->pDescription;
        D3D12_MESSAGE_CATEGORY category = ((D3D12_MESSAGE*)buffer)->Category;
        D3D12_MESSAGE_SEVERITY severity = ((D3D12_MESSAGE*)buffer)->Severity;

        PalDebugMessageSeverity msgSeverity = 0;
        PalDebugMessageType msgType = 0;
        switch (category) {
            case D3D12_MESSAGE_CATEGORY_INITIALIZATION:
            case D3D12_MESSAGE_CATEGORY_CLEANUP:
            case D3D12_MESSAGE_CATEGORY_COMPILATION: {
                msgType = PAL_DEBUG_MESSAGE_TYPE_GENERAL;
                break;
            }

            case D3D12_MESSAGE_CATEGORY_RESOURCE_MANIPULATION:
            case D3D12_MESSAGE_CATEGORY_EXECUTION:
            case D3D12_MESSAGE_CATEGORY_SHADER: {
                msgType = PAL_DEBUG_MESSAGE_TYPE_PERFORMANCE;
                break;
            }

            case D3D12_MESSAGE_CATEGORY_STATE_CREATION:
            case D3D12_MESSAGE_CATEGORY_STATE_GETTING:
            case D3D12_MESSAGE_CATEGORY_STATE_SETTING: {
                msgType = PAL_DEBUG_MESSAGE_TYPE_VALIDATION;
                break;
            }
        }

        switch (severity) {
            case D3D12_MESSAGE_SEVERITY_INFO: {
                msgSeverity = PAL_DEBUG_MESSAGE_SEVERITY_INFO;
                break;
            }

            case D3D12_MESSAGE_SEVERITY_WARNING: {
                msgSeverity = PAL_DEBUG_MESSAGE_SEVERITY_INFO;
                break;
            }

            case D3D12_MESSAGE_SEVERITY_ERROR:
            case D3D12_MESSAGE_SEVERITY_CORRUPTION: {
                msgSeverity = PAL_DEBUG_MESSAGE_SEVERITY_ERROR;
                break;
            }
        }

        s_D3D12.debugCallback(s_D3D12.debugUserData, msgSeverity, msgType, message);
        palFree(s_D3D12.allocator, buffer);
    }
    queue->lpVtbl->ClearStoredMessages(queue);
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

static void commitShaderbindingTableUpdateD3D12(
    CommandBuffer* cmdBuffer, 
    ShaderBindingTable* sbt)
{
    if (!sbt->isDirty) {
        return;
    }

    // begin upload buffer copy to gpu buffer
    cmdBuffer->handle->lpVtbl->CopyBufferRegion(
        cmdBuffer->handle,
        sbt->buffer,
        0,
        sbt->stagingBuffer,
        0,
        sbt->stagingBufferSize);

    // put a memory barrier
    D3D12_RESOURCE_BARRIER barrier = {0};
    barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
    barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE;
    barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
    barrier.Transition.pResource = sbt->buffer;

    cmdBuffer->handle->lpVtbl->ResourceBarrier(cmdBuffer->handle, 1, &barrier);
    sbt->isDirty = PAL_FALSE;
}

static void getDescriptorTierLimitsD3D12(
    void* device, 
    PalResourceCapabilities* caps, 
    PalDescriptorIndexingCapabilities* descCaps)
{
    D3D12_FEATURE_DATA_D3D12_OPTIONS options = {0};
    D3D12_FEATURE_DATA_D3D12_OPTIONS5 options5 = {0};
    ID3D12Device* handle = device;
    handle->lpVtbl->CheckFeatureSupport(
        handle,
        D3D12_FEATURE_D3D12_OPTIONS,
        &options,
        sizeof(options));

    handle->lpVtbl->CheckFeatureSupport(
        handle,
        D3D12_FEATURE_D3D12_OPTIONS5,
        &options5,
        sizeof(options5));

    uint32_t perStage = 0;
    if (options5.RaytracingTier != D3D12_RAYTRACING_TIER_NOT_SUPPORTED) {
        // add buckets for acceleration structure
        if (options.ResourceBindingTier == D3D12_RESOURCE_BINDING_TIER_1) {
            perStage = 8;
        } else {
            perStage = 5000;
        }
    }

    PalResourceCapabilities tmp = {0};
    if (options.ResourceBindingTier == D3D12_RESOURCE_BINDING_TIER_1) {
        tmp.maxPerStageSampledImages = 128 - perStage;
        tmp.maxPerStageStorageImages = 4;
        tmp.maxPerStageSamplers = 16;
        tmp.maxPerStageStorageBuffers = 4;
        tmp.maxPerStageUniformBuffers = 14;
        tmp.maxPerStageAccelerationStructure = perStage;
        
    } else {
        tmp.maxPerStageSampledImages = 705000 - perStage;
        tmp.maxPerStageStorageImages = 100000;
        tmp.maxPerStageSamplers = 2048;
        tmp.maxPerStageStorageBuffers = 100000;
        tmp.maxPerStageUniformBuffers = 95000;
        tmp.maxPerStageAccelerationStructure = perStage;
    }

    tmp.maxPerSetSampledImages = tmp.maxPerStageSampledImages;
    tmp.maxPerSetStorageImages = tmp.maxPerStageStorageImages;
    tmp.maxPerSetSamplers = tmp.maxPerStageSamplers;
    tmp.maxPerSetStorageBuffers = tmp.maxPerStageStorageBuffers;
    tmp.maxPerSetUniformBuffers = tmp.maxPerStageUniformBuffers;
    tmp.maxPerSetAccelerationStructure = tmp.maxPerStageAccelerationStructure;

    if (caps) {
        *caps = tmp;
    } else {
        descCaps->maxPerStageSampledImages = tmp.maxPerStageSampledImages;
        descCaps->maxPerSetSampledImages = tmp.maxPerSetSampledImages;
        descCaps->maxPerStageStorageImages = tmp.maxPerStageStorageImages;
        descCaps->maxPerSetStorageImages = tmp.maxPerSetStorageImages;
        descCaps->maxPerStageSamplers = tmp.maxPerStageSamplers;
        descCaps->maxPerSetSamplers = tmp.maxPerSetSamplers;
        descCaps->maxPerStageStorageBuffers = tmp.maxPerStageStorageBuffers;
        descCaps->maxPerSetStorageBuffers = tmp.maxPerSetStorageBuffers;
        descCaps->maxPerStageUniformBuffers = tmp.maxPerStageUniformBuffers;
        descCaps->maxPerSetUniformBuffers = tmp.maxPerSetUniformBuffers;
        descCaps->maxPerStageAccelerationStructure = tmp.maxPerStageAccelerationStructure;
        descCaps->maxPerSetAccelerationStructure = tmp.maxPerSetAccelerationStructure;
    }
}

// ==================================================
// Adapter
// ==================================================

PalResult PAL_CALL initGraphicsD3D12(
    const PalGraphicsDebugger* debugger,
    const PalAllocator* allocator)
{
    // load d3d12
    s_D3D12.handle = LoadLibraryA("d3d12.dll");
    s_D3D12.dxgi = LoadLibraryA("dxgi.dll");
    if (!s_D3D12.handle || !s_D3D12.dxgi) {
        return PAL_RESULT_PLATFORM_FAILURE;
    }

    // clang-format off
    s_D3D12.createDevice = (PFN_D3D12_CREATE_DEVICE)GetProcAddress(
        s_D3D12.handle,
        "D3D12CreateDevice");

    s_D3D12.createDXGIFactory = (PFN_CreateDXGIFactory2)GetProcAddress(
        s_D3D12.dxgi,
        "CreateDXGIFactory2");

    s_D3D12.serializeVersionedRootSignature = (PFN_D3D12SerializeVersionedRootSignature)GetProcAddress(
        s_D3D12.handle,
        "D3D12SerializeVersionedRootSignature");

    if (debugger && debugger->callback) {
        s_D3D12.getDebugInterface = (PFN_D3D12_GET_DEBUG_INTERFACE)GetProcAddress(
            s_D3D12.handle,
            "D3D12GetDebugInterface");

        if (s_D3D12.getDebugInterface) {
            HRESULT hr;
            ID3D12Debug* debugController = nullptr;
            ID3D12Debug1* debugController1 = nullptr;

            hr = s_D3D12.getDebugInterface(&IID_DebugController, (void**)&debugController);
            if (SUCCEEDED(hr)) {
                debugController->lpVtbl->EnableDebugLayer(debugController);
                debugController->lpVtbl->Release(debugController);
            }

            hr = s_D3D12.getDebugInterface(&IID_DebugController1, (void**)&debugController1);
            if (SUCCEEDED(hr)) {
                debugController1->lpVtbl->SetEnableGPUBasedValidation(
                    debugController1,
                    TRUE);

                debugController1->lpVtbl->Release(debugController1);

                // message types
                if (!debugger->denyGeneral) {
                    s_D3D12.categories[s_D3D12.categoryCount++] = D3D12_MESSAGE_CATEGORY_APPLICATION_DEFINED;
                    s_D3D12.categories[s_D3D12.categoryCount++] = D3D12_MESSAGE_CATEGORY_INITIALIZATION;
                    s_D3D12.categories[s_D3D12.categoryCount++] = D3D12_MESSAGE_CATEGORY_STATE_SETTING;
                    s_D3D12.categories[s_D3D12.categoryCount++] = D3D12_MESSAGE_CATEGORY_RESOURCE_MANIPULATION;
                }

                if (!debugger->denyPerformance) {
                    s_D3D12.categories[s_D3D12.categoryCount++] = D3D12_MESSAGE_CATEGORY_STATE_CREATION;
                    s_D3D12.categories[s_D3D12.categoryCount++] = D3D12_MESSAGE_CATEGORY_EXECUTION;
                    s_D3D12.categories[s_D3D12.categoryCount++] = D3D12_MESSAGE_CATEGORY_STATE_SETTING;
                }

                if (!debugger->denyValidation) {
                    s_D3D12.categories[s_D3D12.categoryCount++] = D3D12_MESSAGE_CATEGORY_SHADER;
                }

                // message severities
                if (!debugger->denyInfoSeverity) {
                    s_D3D12.severities[s_D3D12.severityCount++] = D3D12_MESSAGE_SEVERITY_INFO;
                }

                if (!debugger->denyWarningSeverity) {
                    s_D3D12.severities[s_D3D12.severityCount++] = D3D12_MESSAGE_SEVERITY_WARNING;
                }

                if (!debugger->denyErrorSeverity) {
                    s_D3D12.severities[s_D3D12.severityCount++] = D3D12_MESSAGE_SEVERITY_ERROR;
                    s_D3D12.severities[s_D3D12.severityCount++] = D3D12_MESSAGE_SEVERITY_CORRUPTION;
                }
            }
            s_D3D12.debugLayer = PAL_TRUE;
            s_D3D12.debugCallback = debugger->callback;
        }
    }
    // clang-format on

    s_D3D12.factory = nullptr;
    s_D3D12.adapters = nullptr;
    s_D3D12.adapterCount = 0;
    HRESULT result = s_D3D12.createDXGIFactory(0, &IID_Factory, (void**)&s_D3D12.factory);
    if (FAILED(result)) {
        return PAL_RESULT_PLATFORM_FAILURE;
    }

    s_D3D12.allocator = allocator;
    return PAL_RESULT_SUCCESS;
}

void PAL_CALL shutdownGraphicsD3D12()
{
    for (int i = 0; i < s_D3D12.adapterCount; i++) {
        s_D3D12.adapters[i].handle->lpVtbl->Release(s_D3D12.adapters[i].handle);
    }

    s_D3D12.factory->lpVtbl->Release(s_D3D12.factory);
    FreeLibrary(s_D3D12.handle);
    FreeLibrary(s_D3D12.dxgi);

    if (s_D3D12.adapters) {
        palFree(s_D3D12.allocator, s_D3D12.adapters);
    }
    memset(&s_D3D12, 0, sizeof(s_D3D12));
}

#endif // PAL_HAS_D3D12_BACKEND

#endif // _WIN32
