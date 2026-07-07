
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

// on older SDKs, D3D_FEATURE_LEVEL_12_2 is not defined
#ifndef D3D_FEATURE_LEVEL_12_2
#define D3D_FEATURE_LEVEL_12_2 0xc200
#endif // D3D_FEATURE_LEVEL_12_2

#define MAX_ATTACHMENTS 32
#define TEXTURE_PITCH 256
#define MAX_MESSAGE_SIZE 4096

#define GRAPHICS_PIPELINE 1220
#define COMPUTE_PIPELINE 1221
#define RAY_TRACING_PIPELINE 1222

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

// clang-format off
// IIDS

const IID IID_Factory = {0xc1b6694f, 0xff09, 0x44a9, 0xb0,0x3c, 0x77,0x90,0x0a,0x0a,0x1d,0x17};
const IID IID_DebugController = {0x344488b7, 0x6846, 0x474b, 0xb9,0x89, 0xf0,0x27,0x44,0x82,0x45,0xe0};
const IID IID_DebugController1 = {0xaffaa4ca, 0x63fe, 0x4d8e, 0xb8,0xad, 0x15,0x90,0x00,0xaf,0x43,0x04};
const IID IID_InfoQueue = {0x0742a90b, 0xc387, 0x483f, 0xb9,0x46, 0x30,0xa7,0xe4,0xe6,0x14,0x58};
const IID IID_Heap = {0x6b3b2502, 0x6e51, 0x45b3, 0x90,0xee, 0x98,0x84,0x26,0x5e,0x8d,0xf3};
const IID IID_Queue = {0x0ec870a6, 0x5d7e, 0x4c22, 0x8c,0xfc, 0x5b,0xaa,0xe0,0x76,0x16,0xed};
const IID IID_Swapchain = {0x94d99bdb, 0xf1f8, 0x4ab0, 0xb2,0x36, 0x7d,0xa0,0x17,0x0e,0xda,0xb1};
const IID IID_CommandAllocator = {0x6102dee4, 0xaf59, 0x4b09, 0xb9,0x99, 0xb4,0x4d,0x73,0xf0,0x9b,0x24};
const IID IID_CommandList = {0x7116d91c, 0xe7e4, 0x47ce, 0xb8,0xc6, 0xec,0x81,0x68,0xf4,0x37,0xe5};
const IID IID_CommandList6 = {0xc3827890, 0xe548, 0x4cfa, 0x96,0xcf, 0x56,0x89,0xa9,0x37,0x0f,0x80};
const IID IID_CommandSignature = {0xc36a797c, 0xec80, 0x4f0a, 0x89,0x85, 0xa7,0xb2,0x47,0x50,0x82,0xd1};
const IID IID_DescriptorHeap = {0x8efb471d, 0x616c, 0x4f49, 0x90,0xf7, 0x12,0x7b,0xb7,0x63,0xfa,0x51};
const IID IID_RootSignature = {0xc54a6b66, 0x72df, 0x4ee8, 0x8b,0xe5, 0xa9,0x46,0xa1,0x42,0x92,0x14};
const IID IID_Device5 = {0x8b4f173b, 0x2fea, 0x4b80, 0x8f,0x58, 0x43,0x07,0x19,0x1a,0xb9,0x5d};
const IID IID_PipelineState = {0x765a30f3, 0xf624, 0x4c6f, 0xa8,0x28, 0xac,0xe9,0x48,0x62,0x24,0x45};
const IID IID_StateObject = {0x47016943, 0xfca8, 0x4594, 0x93,0xea, 0xaf,0x25,0x8b,0x55,0x34,0x6d};
// clang-format on

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



// ==================================================
// Device
// ==================================================

PalResult PAL_CALL createDeviceD3D12(
    PalAdapter* adapter,
    PalAdapterFeatures features,
    PalDevice** outDevice)
{
    HRESULT result;
    Device* device = nullptr;
    Adapter* d3dAdapter = (Adapter*)adapter;

    // check if any of the features are not supported
    PalAdapterFeatures adapterFeatures = getAdapterFeaturesD3D12(adapter);
    PalBool valid = (adapterFeatures & features) == features;
    if (!valid) {
        return PAL_RESULT_ADAPTER_FEATURE_NOT_SUPPORTED;
    }

    device = palAllocate(s_D3D12.allocator, sizeof(Device), 0);
    if (!device) {
        return PAL_RESULT_OUT_OF_MEMORY;
    }

    ID3D12Device* tmpDevice = nullptr;
    memset(device, 0, sizeof(Device));
    DeviceLimits* limits = &device->limits;

    // get and cache highest shader model
    device->shaderModel = getHighestSupportedShaderTargetD3D12(adapter, PAL_SHADER_FORMAT_DXIL);

    result = s_D3D12.createDevice(
        (IUnknown*)d3dAdapter->handle,
        d3dAdapter->level,
        &IID_Device,
        (void**)&tmpDevice);

    if (FAILED(result)) {
        palFree(s_D3D12.allocator, device);
        if (result == E_OUTOFMEMORY) {
            return PAL_RESULT_OUT_OF_MEMORY;
        }
        return PAL_RESULT_INVALID_DRIVER;
    }

    result = tmpDevice->lpVtbl->QueryInterface(
        tmpDevice,
        &IID_Device5,
        (void**)&device->handle);

    tmpDevice->lpVtbl->Release(tmpDevice);
    if (s_D3D12.debugLayer) {
        result = device->handle->lpVtbl->QueryInterface(
            device->handle,
            &IID_InfoQueue,
            (void**)&device->infoQueue);

        if (SUCCEEDED(result)) {
            D3D12_MESSAGE_ID denyIDs[] = { D3D12_MESSAGE_ID_MAP_INVALID_NULLRANGE };
            D3D12_INFO_QUEUE_FILTER filter = {0};
            filter.AllowList.NumSeverities = s_D3D12.severityCount;
            filter.AllowList.pSeverityList = s_D3D12.severities;
            filter.AllowList.NumCategories = s_D3D12.categoryCount;
            filter.AllowList.pCategoryList = s_D3D12.categories;
            filter.DenyList.NumIDs = 1;
            filter.DenyList.pIDList = denyIDs;

            device->infoQueue->lpVtbl->PushStorageFilter(device->infoQueue, &filter);
        }
    }

    // create a temporary graphics queue
    D3D12_COMMAND_QUEUE_DESC desc = {0};
    desc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;

    result = device->handle->lpVtbl->CreateCommandQueue(
        device->handle,
        &desc,
        &IID_Queue,
        (void**)&device->queue);

    if (FAILED(result)) {
        if (result == E_OUTOFMEMORY) {
            return PAL_RESULT_OUT_OF_MEMORY;
        }
        return PAL_RESULT_PLATFORM_FAILURE;
    }

    // create command signatures
    D3D12_INDIRECT_ARGUMENT_DESC argumentDesc = {0};
    D3D12_COMMAND_SIGNATURE_DESC signatureDesc = {0};
    signatureDesc.NumArgumentDescs = 1;
    signatureDesc.pArgumentDescs = &argumentDesc;

    if (features & PAL_ADAPTER_FEATURE_INDIRECT_DRAW_MESH) {
        argumentDesc.Type = D3D12_INDIRECT_ARGUMENT_TYPE_DISPATCH_MESH;
        signatureDesc.ByteStride = sizeof(D3D12_DISPATCH_MESH_ARGUMENTS);

        result = device->handle->lpVtbl->CreateCommandSignature(
            device->handle,
            &signatureDesc,
            nullptr,
            &IID_CommandSignature,
            (void**)&device->meshSignature);

        if (FAILED(result)) {
            if (result == E_OUTOFMEMORY) {
                return PAL_RESULT_OUT_OF_MEMORY;
            }
            return PAL_RESULT_PLATFORM_FAILURE;
        }
    }

    if (features & PAL_ADAPTER_FEATURE_RAY_TRACING) {
        argumentDesc.Type = D3D12_INDIRECT_ARGUMENT_TYPE_DISPATCH_RAYS;
        signatureDesc.ByteStride = sizeof(D3D12_DISPATCH_RAYS_DESC);

        result = device->handle->lpVtbl->CreateCommandSignature(
            device->handle,
            &signatureDesc,
            nullptr,
            &IID_CommandSignature,
            (void**)&device->raySignature);

        if (FAILED(result)) {
            if (result == E_OUTOFMEMORY) {
                return PAL_RESULT_OUT_OF_MEMORY;
            }
            return PAL_RESULT_PLATFORM_FAILURE;
        }
    }

    if (features & PAL_ADAPTER_FEATURE_INDIRECT_DRAW) {
        // draw indexed
        argumentDesc.Type = D3D12_INDIRECT_ARGUMENT_TYPE_DRAW_INDEXED;
        signatureDesc.ByteStride = sizeof(D3D12_DRAW_INDEXED_ARGUMENTS);

        result = device->handle->lpVtbl->CreateCommandSignature(
            device->handle,
            &signatureDesc,
            nullptr,
            &IID_CommandSignature,
            (void**)&device->drawIndexedSignature);

        if (FAILED(result)) {
            if (result == E_OUTOFMEMORY) {
                return PAL_RESULT_OUT_OF_MEMORY;
            }
            return PAL_RESULT_PLATFORM_FAILURE;
        }

        // draw
        argumentDesc.Type = D3D12_INDIRECT_ARGUMENT_TYPE_DRAW;
        signatureDesc.ByteStride = sizeof(D3D12_DRAW_ARGUMENTS);

        result = device->handle->lpVtbl->CreateCommandSignature(
            device->handle,
            &signatureDesc,
            nullptr,
            &IID_CommandSignature,
            (void**)&device->drawSignature);

        if (FAILED(result)) {
            if (result == E_OUTOFMEMORY) {
                return PAL_RESULT_OUT_OF_MEMORY;
            }
            return PAL_RESULT_PLATFORM_FAILURE;
        }
    }

    if (features & PAL_ADAPTER_FEATURE_INDIRECT_DISPATCH) {
        argumentDesc.Type = D3D12_INDIRECT_ARGUMENT_TYPE_DISPATCH;
        signatureDesc.ByteStride = sizeof(D3D12_DISPATCH_ARGUMENTS);

        result = device->handle->lpVtbl->CreateCommandSignature(
            device->handle,
            &signatureDesc,
            nullptr,
            &IID_CommandSignature,
            (void**)&device->dispatchSignature);

        if (FAILED(result)) {
            if (result == E_OUTOFMEMORY) {
                return PAL_RESULT_OUT_OF_MEMORY;
            }
            return PAL_RESULT_PLATFORM_FAILURE;
        }
    }

    // create an internal heap for RTV and DSV
    D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {0};
    heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
    heapDesc.NumDescriptors = MAX_RTV;

    result = device->handle->lpVtbl->CreateDescriptorHeap(
        device->handle,
        &heapDesc,
        &IID_DescriptorHeap,
        (void**)&device->rtvAllocator.heap);

    if (FAILED(result)) {
        if (result == E_OUTOFMEMORY) {
            return PAL_RESULT_OUT_OF_MEMORY;
        }
        return PAL_RESULT_PLATFORM_FAILURE;
    }

    heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
    heapDesc.NumDescriptors = MAX_DSV;
    result = device->handle->lpVtbl->CreateDescriptorHeap(
        device->handle,
        &heapDesc,
        &IID_DescriptorHeap,
        (void**)&device->dsvAllocator.heap);

    if (FAILED(result)) {
        if (result == E_OUTOFMEMORY) {
            return PAL_RESULT_OUT_OF_MEMORY;
        }
        return PAL_RESULT_PLATFORM_FAILURE;
    }

    for (int i = 0; i < MAX_RTV; i++) {
        device->rtvAllocator.freeList[i] = i + 1;
    }

    for (int i = 0; i < MAX_DSV; i++) {
        device->dsvAllocator.freeList[i] = i + 1;
    }

    device->rtvAllocator.freeTop = 0;
    device->rtvAllocator.incrementSize = device->handle->lpVtbl->GetDescriptorHandleIncrementSize(
        device->handle,
        D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

    device->dsvAllocator.freeTop = 0;
    device->dsvAllocator.incrementSize = device->handle->lpVtbl->GetDescriptorHandleIncrementSize(
        device->handle,
        D3D12_DESCRIPTOR_HEAP_TYPE_DSV);

    // get base CPU pointer
    D3D12_CPU_DESCRIPTOR_HANDLE __ret, dst;
    dst = *device->rtvAllocator.heap->lpVtbl->GetCPUDescriptorHandleForHeapStart(
        device->rtvAllocator.heap,
        &__ret
    );
    device->rtvAllocator.baseOffset = dst.ptr;

    dst = *device->dsvAllocator.heap->lpVtbl->GetCPUDescriptorHandleForHeapStart(
        device->dsvAllocator.heap,
        &__ret
    );
    device->dsvAllocator.baseOffset = dst.ptr;

    // API enforced limits
    limits->freeComputeQueues = 2;
    limits->freeGraphicsQueues = 2;
    limits->freeCopyQueues = 2;
    limits->maxVertexLayouts = 30;
    limits->maxVertexAttributes = 30;

    limits->maxAnisotropy = 16;
    limits->maxPushConstantSize = 256;
    limits->maxTessellationPatchPoint = 32;
    limits->maxBoundDescriptorSets = 30;

    if (features & PAL_ADAPTER_FEATURE_DESCRIPTOR_INDEXING) {
        limits->maxDescriptorSampledImages = 4096;
        limits->maxDescriptorStorageImages = 1024;
        limits->maxDescriptorSamplers = 512;
        limits->maxDescriptorStorageBuffers = 2048;
        limits->maxDescriptorUniformBuffers = 256;

    } else {
        limits->maxDescriptorSampledImages = 1024;
        limits->maxDescriptorStorageImages = 512;
        limits->maxDescriptorSamplers = 256;
        limits->maxDescriptorStorageBuffers = 512;
        limits->maxDescriptorUniformBuffers = 256;
        limits->maxBoundDescriptorSets = 30;
    }

    if (features & PAL_ADAPTER_FEATURE_RAY_TRACING) {
        limits->maxRecursionDepth = 31;
        limits->maxHitAttributeSize = 32;
        limits->maxPayloadSize = 64;
        limits->maxDispatchInvocations = 16000000;
        limits->maxDescriptorAccelerationStructures = 4;
    }

    device->adapter = d3dAdapter->handle;
    device->features = features;
    *outDevice = (PalDevice*)device;
    return PAL_RESULT_SUCCESS;
}

void PAL_CALL destroyDeviceD3D12(PalDevice* device)
{
    Device* d3d12Device = (Device*)device;
    if (d3d12Device->meshSignature) {
        d3d12Device->meshSignature->lpVtbl->Release(d3d12Device->meshSignature);
    }

    if (d3d12Device->raySignature) {
        d3d12Device->raySignature->lpVtbl->Release(d3d12Device->raySignature);
    }

    if (d3d12Device->dispatchSignature) {
        d3d12Device->dispatchSignature->lpVtbl->Release(d3d12Device->dispatchSignature);
    }

    if (d3d12Device->drawIndexedSignature) {
        d3d12Device->drawIndexedSignature->lpVtbl->Release(d3d12Device->drawIndexedSignature);
    }

    if (d3d12Device->drawSignature) {
        d3d12Device->drawSignature->lpVtbl->Release(d3d12Device->drawSignature);
    }

    d3d12Device->rtvAllocator.heap->lpVtbl->Release(d3d12Device->rtvAllocator.heap);
    d3d12Device->dsvAllocator.heap->lpVtbl->Release(d3d12Device->dsvAllocator.heap);

    d3d12Device->queue->lpVtbl->Release(d3d12Device->queue);
    d3d12Device->handle->lpVtbl->Release(d3d12Device->handle);
    if (d3d12Device->infoQueue) {
        d3d12Device->infoQueue->lpVtbl->Release(d3d12Device->infoQueue);
    }

    palFree(s_D3D12.allocator, d3d12Device);
}

// ==================================================
// Memory
// ==================================================

PalResult PAL_CALL allocateMemoryD3D12(
    PalDevice* device,
    PalMemoryType type,
    uint64_t memoryMask,
    uint64_t size,
    PalMemory** outMemory)
{
    HRESULT result;
    Device* d3d12Device = (Device*)device;
    ID3D12Heap* memory = nullptr;

    D3D12_HEAP_DESC desc = {0};
    desc.SizeInBytes = size;

    desc.Properties.Type = D3D12_HEAP_TYPE_DEFAULT;
    if (type == PAL_MEMORY_TYPE_CPU_READBACK) {
        desc.Properties.Type = D3D12_HEAP_TYPE_READBACK;

    } else if (type == PAL_MEMORY_TYPE_CPU_UPLOAD) {
        desc.Properties.Type = D3D12_HEAP_TYPE_UPLOAD;
    }

    result = d3d12Device->handle->lpVtbl->CreateHeap(
        d3d12Device->handle,
        &desc,
        &IID_Heap,
        (void**)&memory);

    if (FAILED(result)) {
        pollMessagesD3D12(d3d12Device);
        return PAL_RESULT_OUT_OF_MEMORY;
    }

    *outMemory = (PalMemory*)memory;
    return PAL_RESULT_SUCCESS;
}

void PAL_CALL freeMemoryD3D12(
    PalDevice* device,
    PalMemory* memory)
{
    ID3D12Heap* mem = (ID3D12Heap*)memory;
    mem->lpVtbl->Release(mem);
}

// ==================================================
// Extended Adapter Features
// ==================================================

PalResult PAL_CALL querySamplerAnisotropyCapabilitiesD3D12(
    PalDevice* device,
    PalSamplerAnisotropyCapabilities* caps)
{
    Device* d3d12Device = (Device*)device;
    if (!(d3d12Device->features & PAL_ADAPTER_FEATURE_SAMPLER_ANISOTROPY)) {
        return PAL_RESULT_ADAPTER_FEATURE_NOT_SUPPORTED;
    }

    caps->maxAnisotropy = 16; // default on most d3d12 hardwares
    return PAL_RESULT_SUCCESS;
}

PalResult PAL_CALL queryMultiViewCapabilitiesD3D12(
    PalDevice* device,
    PalMultiViewCapabilities* caps)
{
    Device* d3d12Device = (Device*)device;
    if (!(d3d12Device->features & PAL_ADAPTER_FEATURE_SAMPLER_ANISOTROPY)) {
        return PAL_RESULT_ADAPTER_FEATURE_NOT_SUPPORTED;
    }

    caps->maxViewCount = D3D12_MAX_VIEW_INSTANCE_COUNT;
    return PAL_RESULT_SUCCESS;
}

PalResult PAL_CALL queryMultiViewportCapabilitiesD3D12(
    PalDevice* device,
    PalMultiViewportCapabilities* caps)
{
    Device* d3d12Device = (Device*)device;
    if (!(d3d12Device->features & PAL_ADAPTER_FEATURE_SAMPLER_ANISOTROPY)) {
        return PAL_RESULT_ADAPTER_FEATURE_NOT_SUPPORTED;
    }

    caps->maxCount = D3D12_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE;
    return PAL_RESULT_SUCCESS;
}

PalResult PAL_CALL queryDepthStencilCapabilitiesD3D12(
    PalDevice* device,
    PalDepthStencilCapabilities* caps)
{
    Device* d3d12Device = (Device*)device;
    if (!(d3d12Device->features & PAL_ADAPTER_FEATURE_SAMPLER_ANISOTROPY)) {
        return PAL_RESULT_ADAPTER_FEATURE_NOT_SUPPORTED;
    }

    caps->depthResolves[PAL_RESOLVE_MODE_SAMPLE_ZERO] = PAL_TRUE;
    caps->depthResolves[PAL_RESOLVE_MODE_AVERAGE] = PAL_TRUE;
    caps->depthResolves[PAL_RESOLVE_MODE_MIN] = PAL_TRUE;
    caps->depthResolves[PAL_RESOLVE_MODE_MAX] = PAL_TRUE;

    caps->stencilResolves[PAL_RESOLVE_MODE_SAMPLE_ZERO] = PAL_TRUE;
    caps->stencilResolves[PAL_RESOLVE_MODE_AVERAGE] = PAL_FALSE;
    caps->stencilResolves[PAL_RESOLVE_MODE_MIN] = PAL_TRUE;
    caps->stencilResolves[PAL_RESOLVE_MODE_MAX] = PAL_TRUE;

    caps->independentResolve = PAL_TRUE;
    return PAL_RESULT_SUCCESS;
}

PalResult PAL_CALL queryFragmentShadingRateCapabilitiesD3D12(
    PalDevice* device,
    PalFragmentShadingRateCapabilities* caps)
{
    Device* d3d12Device = (Device*)device;
    if (!(d3d12Device->features & PAL_ADAPTER_FEATURE_FRAGMENT_SHADING_RATE)) {
        return PAL_RESULT_ADAPTER_FEATURE_NOT_SUPPORTED;
    }

    // these are supported if fragment shading rate feature is
    caps->shadingRates[PAL_FRAGMENT_SHADING_RATE_1X1] = PAL_TRUE;
    caps->shadingRates[PAL_FRAGMENT_SHADING_RATE_1X2] = PAL_TRUE;
    caps->shadingRates[PAL_FRAGMENT_SHADING_RATE_2X1] = PAL_TRUE;
    caps->shadingRates[PAL_FRAGMENT_SHADING_RATE_2X2] = PAL_TRUE;

    D3D12_FEATURE_DATA_D3D12_OPTIONS6 options = {0};
    d3d12Device->handle->lpVtbl->CheckFeatureSupport(
        d3d12Device->handle,
        D3D12_FEATURE_D3D12_OPTIONS6,
        &options,
        sizeof(options));

    if (options.AdditionalShadingRatesSupported) {
        caps->shadingRates[PAL_FRAGMENT_SHADING_RATE_2X4] = PAL_TRUE;
        caps->shadingRates[PAL_FRAGMENT_SHADING_RATE_4X2] = PAL_TRUE;
        caps->shadingRates[PAL_FRAGMENT_SHADING_RATE_4X4] = PAL_TRUE;

    } else {
        caps->shadingRates[PAL_FRAGMENT_SHADING_RATE_2X4] = PAL_FALSE;
        caps->shadingRates[PAL_FRAGMENT_SHADING_RATE_4X2] = PAL_FALSE;
        caps->shadingRates[PAL_FRAGMENT_SHADING_RATE_4X4] = PAL_FALSE;
    }

    // there are supported if fragment shading rate feature is
    caps->combinerOps[PAL_FRAGMENT_SHADING_RATE_COMBINER_OP_KEEP] = PAL_TRUE;
    caps->combinerOps[PAL_FRAGMENT_SHADING_RATE_COMBINER_OP_REPLACE] = PAL_TRUE;
    caps->combinerOps[PAL_FRAGMENT_SHADING_RATE_COMBINER_OP_MIN] = PAL_TRUE;
    caps->combinerOps[PAL_FRAGMENT_SHADING_RATE_COMBINER_OP_MAX] = PAL_TRUE;
    caps->combinerOps[PAL_FRAGMENT_SHADING_RATE_COMBINER_OP_MUL] = PAL_TRUE;

    caps->minTexelWidth = 1; // safe default
    caps->minTexelHeight = 1; // safe default
    caps->maxTexelWidth = options.ShadingRateImageTileSize;
    caps->maxTexelHeight = options.ShadingRateImageTileSize;

    return PAL_RESULT_SUCCESS;
}

PalResult PAL_CALL queryMeshShaderCapabilitiesD3D12(
    PalDevice* device,
    PalMeshShaderCapabilities* caps)
{
    Device* d3d12Device = (Device*)device;
    if (!(d3d12Device->features & PAL_ADAPTER_FEATURE_MESH_SHADER)) {
        return PAL_RESULT_ADAPTER_FEATURE_NOT_SUPPORTED;
    }

    // these are not exposed by d3d12. We use the offical mesh shader spec
    caps->maxOutputPrimitives = 256;
    caps->maxOutputVertices = 256;
    caps->maxWorkGroupInvocations = 128;
    caps->maxTaskWorkGroupInvocations = 128;

    caps->maxWorkGroupCount[0] = 65535;
    caps->maxWorkGroupCount[1] = 65535;
    caps->maxWorkGroupCount[2] = 65535;

    caps->maxTaskWorkGroupCount[0] = 65535;
    caps->maxTaskWorkGroupCount[1] = 65535;
    caps->maxTaskWorkGroupCount[2] = 65535;
    return PAL_RESULT_SUCCESS;
}

PalResult PAL_CALL queryRayTracingCapabilitiesD3D12(
    PalDevice* device,
    PalRayTracingCapabilities* caps)
{
    Device* d3d12Device = (Device*)device;
    if (!(d3d12Device->features & PAL_ADAPTER_FEATURE_RAY_TRACING)) {
        return PAL_RESULT_ADAPTER_FEATURE_NOT_SUPPORTED;
    }

    // these are safe defaults. D3d12 does not expose them.
    caps->maxRecursionDepth = 31;
    caps->maxHitAttributeSize = 32;
    caps->maxInstanceCount = 1000000;
    caps->maxPrimitiveCount = 10000000;
    caps->maxGeometryCount = 100000;
    caps->maxPayloadSize = 64;
    caps->maxDispatchInvocations = 16000000;

    return PAL_RESULT_SUCCESS;
}

PalResult PAL_CALL queryDescriptorIndexingCapabilitiesD3D12(
    PalDevice* device,
    PalDescriptorIndexingCapabilities* caps)
{
    Device* d3d12Device = (Device*)device;
    if (!(d3d12Device->features & PAL_ADAPTER_FEATURE_DESCRIPTOR_INDEXING)) {
        return PAL_RESULT_ADAPTER_FEATURE_NOT_SUPPORTED;
    }

    // always supported
    caps->sampledImageNonUniformIndexing = PAL_TRUE;
    caps->sampledImageUpdateAfterBind = PAL_TRUE;
    caps->storageImageNonUniformIndexing = PAL_TRUE;
    caps->storageImageUpdateAfterBind = PAL_TRUE;
    caps->storageBufferNonUniformIndexing = PAL_TRUE;
    caps->storageBufferUpdateAfterBind = PAL_TRUE;
    caps->uniformBufferNonUniformIndexing = PAL_TRUE;
    caps->uniformBufferUpdateAfterBind = PAL_TRUE;

    getDescriptorTierLimitsD3D12(d3d12Device->handle, nullptr, caps);
    return PAL_RESULT_SUCCESS;
}

// ==================================================
// Queue
// ==================================================

PalResult PAL_CALL createQueueD3D12(
    PalDevice* device,
    PalQueueType type,
    PalQueue** outQueue)
{
    HRESULT result;
    Device* d3d12Device = (Device*)device;
    Queue* queue = nullptr;
    D3D12_COMMAND_QUEUE_DESC desc = {0};

    switch (type) {
        case PAL_QUEUE_TYPE_COMPUTE: {
            desc.Type = D3D12_COMMAND_LIST_TYPE_COMPUTE;

            if (!d3d12Device->limits.freeComputeQueues) {
                return PAL_RESULT_OUT_OF_QUEUE;
            }
            d3d12Device->limits.freeComputeQueues--;
            break;
        }

        case PAL_QUEUE_TYPE_GRAPHICS: {
            desc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;

            if (!d3d12Device->limits.freeGraphicsQueues) {
                return PAL_RESULT_OUT_OF_QUEUE;
            }
            d3d12Device->limits.freeGraphicsQueues--;
            break;
        }

        case PAL_QUEUE_TYPE_COPY: {
            desc.Type = D3D12_COMMAND_LIST_TYPE_COPY;

            if (!d3d12Device->limits.freeCopyQueues) {
                return PAL_RESULT_OUT_OF_QUEUE;
            }
            d3d12Device->limits.freeCopyQueues--;
            break;
        }
    }

    queue = palAllocate(s_D3D12.allocator, sizeof(Queue), 0);
    if (!queue) {
        return PAL_RESULT_OUT_OF_MEMORY;
    }

    result = d3d12Device->handle->lpVtbl->CreateCommandQueue(
        d3d12Device->handle,
        &desc,
        &IID_Queue,
        (void**)&queue->handle);

    if (FAILED(result)) {
        pollMessagesD3D12(d3d12Device);
        palFree(s_D3D12.allocator, queue);
        if (result == E_OUTOFMEMORY) {
            return PAL_RESULT_OUT_OF_MEMORY;
        }
        return PAL_RESULT_PLATFORM_FAILURE;
    }

    // create fence used for queue wait
    result = d3d12Device->handle->lpVtbl->CreateFence(
        d3d12Device->handle,
        0,
        0,
        &IID_Fence,
        (void**)&queue->fence);

    if (FAILED(result)) {
        pollMessagesD3D12(d3d12Device);
        palFree(s_D3D12.allocator, queue);
        if (result == E_OUTOFMEMORY) {
            return PAL_RESULT_OUT_OF_MEMORY;
        }
        return PAL_RESULT_PLATFORM_FAILURE;
    }

    queue->fenceValue = 0;
    queue->type = type;
    *outQueue = (PalQueue*)queue;
    return PAL_RESULT_SUCCESS;
}

void PAL_CALL destroyQueueD3D12(PalQueue* queue)
{
    Queue* d3dQueue = (Queue*)queue;
    d3dQueue->fence->lpVtbl->Release(d3dQueue->fence);
    d3dQueue->handle->lpVtbl->Release(d3dQueue->handle);
    palFree(s_D3D12.allocator, d3dQueue);
}

PalResult PAL_CALL waitQueueD3D12(PalQueue* queue)
{
    Queue* d3dQueue = (Queue*)queue;
    ID3D12Fence* fence = d3dQueue->fence;

    // wait on the fence if the submited work is not done
    if (fence->lpVtbl->GetCompletedValue(fence) < d3dQueue->fenceValue) {
        HANDLE event = CreateEvent(nullptr, PAL_FALSE, PAL_FALSE, nullptr);
        fence->lpVtbl->SetEventOnCompletion(fence, d3dQueue->fenceValue, event);
        WaitForSingleObject(event, INFINITE);
        CloseHandle(event);
    }
    return PAL_RESULT_SUCCESS;
}

PalBool PAL_CALL canQueuePresentD3D12(
    PalQueue* queue,
    PalSurface* surface)
{
    Queue* d3dQueue = (Queue*)queue;
    if (d3dQueue->type == PAL_QUEUE_TYPE_GRAPHICS) {
        return PAL_TRUE; // all graphics queues support presentation
    }
    return PAL_FALSE;
}

// ==================================================
// Formats
// ==================================================



// ==================================================
// Image
// ==================================================

PalResult PAL_CALL createImageD3D12(
    PalDevice* device,
    const PalImageCreateInfo* info,
    PalImage** outImage)
{
    HRESULT result;
    Image* image = nullptr;
    Device* d3d12Device = (Device*)device;

    image = palAllocate(s_D3D12.allocator, sizeof(Image), 0);
    if (!image) {
        return PAL_RESULT_OUT_OF_MEMORY;
    }

    memset(image, 0, sizeof(Image));
    image->desc.Width = (UINT64)info->width;
    image->desc.Height = (UINT64)info->height;
    image->desc.DepthOrArraySize = (UINT16)info->depthOrArraySize;
    image->desc.MipLevels = (UINT16)info->mipLevelCount;
    image->desc.Format = formatToD3D12(info->format);
    image->desc.SampleDesc.Count = samplesToD3D12(info->sampleCount);
    image->desc.SampleDesc.Quality = 0;

    image->desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
    if (info->type == PAL_IMAGE_TYPE_3D) {
        image->desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;

    } else if (info->type == PAL_IMAGE_TYPE_1D) {
        image->desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE1D;
    }

    if (info->usages & PAL_IMAGE_USAGE_COLOR_ATTACHEMENT) {
        image->desc.Flags |= D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;
    }

    if (info->usages & PAL_IMAGE_USAGE_DEPTH_ATTACHEMENT) {
        image->desc.Flags |= D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;
    }

    if (info->usages & PAL_IMAGE_USAGE_STORAGE) {
        image->desc.Flags |= D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
    }

    image->belongsToSwapchain = PAL_FALSE;
    image->info.depthOrArraySize = info->depthOrArraySize;
    image->info.type = info->type;
    image->info.format = info->format;
    image->info.usages = info->usages;
    image->info.height = info->height;
    image->info.mipLevelCount = info->mipLevelCount;
    image->info.sampleCount = info->sampleCount;
    image->info.width = info->width;

    image->device = d3d12Device;
    *outImage = (PalImage*)image;
    return PAL_RESULT_SUCCESS;
}

void PAL_CALL destroyImageD3D12(PalImage* image)
{
    Image* d3dImage = (Image*)image;
    // check if memory has been attached to the image
    if (d3dImage->handle) {
        d3dImage->handle->lpVtbl->Release(d3dImage->handle);
    }
    palFree(s_D3D12.allocator, d3dImage);
}

PalResult PAL_CALL getImageInfoD3D12(
    PalImage* image,
    PalImageInfo* info)
{
    Image* d3dImage = (Image*)image;
    *info = d3dImage->info;
    return PAL_RESULT_SUCCESS;
}

PalResult PAL_CALL getImageMemoryRequirementsD3D12(
    PalImage* image,
    PalMemoryRequirements* requirements)
{
    Image* d3dImage = (Image*)image;
    ID3D12Device5* device = d3dImage->device->handle;
    if (d3dImage->belongsToSwapchain) {
        return PAL_RESULT_INVALID_OPERATION;
    }

    D3D12_RESOURCE_ALLOCATION_INFO allocationInfo = {0};
    D3D12_RESOURCE_ALLOCATION_INFO __ret = {0};
    allocationInfo = *device->lpVtbl->GetResourceAllocationInfo(
        device,
        &__ret,
        0,
        1,
        &d3dImage->desc);

    // d3d12 allows images to be used with only GPU only heap
    requirements->memoryTypes[PAL_MEMORY_TYPE_GPU_ONLY] = PAL_TRUE;
    requirements->memoryTypes[PAL_MEMORY_TYPE_CPU_UPLOAD] = PAL_FALSE;
    requirements->memoryTypes[PAL_MEMORY_TYPE_CPU_READBACK] = PAL_FALSE;

    requirements->alignment = allocationInfo.Alignment;
    requirements->size = allocationInfo.SizeInBytes;
    return PAL_RESULT_SUCCESS;
}

PalResult PAL_CALL bindImageMemoryD3D12(
    PalImage* image,
    PalMemory* memory,
    uint64_t offset)
{
    HRESULT result;
    Image* d3dImage = (Image*)image;
    ID3D12Device5* device = d3dImage->device->handle;
    if (d3dImage->belongsToSwapchain) {
        return PAL_RESULT_INVALID_OPERATION;
    }

    ID3D12Heap* mem = (ID3D12Heap*)memory;
    result = device->lpVtbl->CreatePlacedResource(
        device,
        mem,
        offset,
        &d3dImage->desc,
        0,
        nullptr,
        &IID_Resource,
        (void**)&d3dImage->handle);

    if (FAILED(result)) {
        pollMessagesD3D12(d3dImage->device);
        if (result == E_OUTOFMEMORY) {
            return PAL_RESULT_OUT_OF_MEMORY;
        } else if (result == E_INVALIDARG) {
            return PAL_RESULT_INVALID_ARGUMENT;
        }
        return PAL_RESULT_PLATFORM_FAILURE;
    }

    return PAL_RESULT_SUCCESS;
}

// ==================================================
// Image View
// ==================================================

PalResult PAL_CALL createImageViewD3D12(
    PalDevice* device,
    PalImage* image,
    const PalImageViewCreateInfo* info,
    PalImageView** outImageView)
{
    HRESULT result;
    ImageView* imageView = nullptr;
    Device* d3d12Device = (Device*)device;
    Image* d3dImage = (Image*)image;

    if (info->type == PAL_IMAGE_VIEW_TYPE_CUBE_ARRAY) {
        if (!(d3d12Device->features & PAL_ADAPTER_FEATURE_IMAGE_VIEW_CUBE_ARRAY)) {
            return PAL_RESULT_ADAPTER_FEATURE_NOT_SUPPORTED;
        }
    }

    imageView = palAllocate(s_D3D12.allocator, sizeof(ImageView), 0);
    if (!imageView) {
        return PAL_RESULT_OUT_OF_MEMORY;
    }

    imageView->heapIndex = UINT32_MAX;
    PalBool hasRTV = (d3dImage->desc.Flags & D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET);
    PalBool hasDSV = (d3dImage->desc.Flags & D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL);

    imageView->format = formatToD3D12(info->format);
    if (info->subresourceRange.aspect == PAL_IMAGE_ASPECT_COLOR && hasRTV) {
        RTVHeapAllocator* allocator = &d3d12Device->rtvAllocator;
        uint32_t index = allocator->freeTop;
        allocator->freeTop = allocator->freeList[index];

        D3D12_CPU_DESCRIPTOR_HANDLE dst;
        dst.ptr = getDescriptorHandleD3D12(index, allocator->incrementSize, allocator->baseOffset);

        D3D12_RENDER_TARGET_VIEW_DESC desc = {0};
        desc.Format = imageView->format;
        fillSubresourceD3D12(
            info->type, 
            &info->subresourceRange, 
            &desc, 
            nullptr, 
            nullptr, 
            nullptr);

        imageView->heapIndex = index;
        d3d12Device->handle->lpVtbl->CreateRenderTargetView(
            d3d12Device->handle,
            d3dImage->handle,
            &desc,
            dst);

    } else if (info->subresourceRange.aspect != PAL_IMAGE_ASPECT_COLOR && hasDSV) {
        DSVHeapAllocator* allocator = &d3d12Device->dsvAllocator;
        uint32_t index = allocator->freeTop;
        allocator->freeTop = allocator->freeList[index];

        D3D12_CPU_DESCRIPTOR_HANDLE dst;
        dst.ptr = getDescriptorHandleD3D12(index, allocator->incrementSize, allocator->baseOffset);

        D3D12_DEPTH_STENCIL_VIEW_DESC desc = {0};
        desc.Format = imageView->format;
        fillSubresourceD3D12(
            info->type, 
            &info->subresourceRange, 
            nullptr, 
            &desc, 
            nullptr, 
            nullptr);

        imageView->heapIndex = index;
        d3d12Device->handle->lpVtbl->CreateDepthStencilView(
            d3d12Device->handle,
            d3dImage->handle,
            &desc,
            dst);
    }

    imageView->range = info->subresourceRange;
    imageView->type = info->type;
    imageView->image = d3dImage;

    imageView->device = d3d12Device;
    *outImageView = (PalImageView*)imageView;
    return PAL_RESULT_SUCCESS;
}

void PAL_CALL destroyImageViewD3D12(PalImageView* imageView)
{
    ImageView* d3dImageView = (ImageView*)imageView;
    Device* device = d3dImageView->device;
    RTVHeapAllocator* allocator = nullptr;

    if (d3dImageView->heapIndex != UINT32_MAX) {
        if (d3dImageView->range.aspect == PAL_IMAGE_ASPECT_COLOR) {
            RTVHeapAllocator* allocator = &device->rtvAllocator;
            allocator->freeList[d3dImageView->heapIndex] = allocator->freeTop;
            allocator->freeTop = d3dImageView->heapIndex;

        } else {
            DSVHeapAllocator* allocator = &device->dsvAllocator;
            allocator->freeList[d3dImageView->heapIndex] = allocator->freeTop;
            allocator->freeTop = d3dImageView->heapIndex;
        }
    }
    palFree(s_D3D12.allocator, d3dImageView);
}

// ==================================================
// Sampler
// ==================================================

PalResult PAL_CALL createSamplerD3D12(
    PalDevice* device,
    const PalSamplerCreateInfo* info,
    PalSampler** outSampler)
{
    Device* d3d12Device = (Device*)device;
    if (info->maxAnisotropy > d3d12Device->limits.maxAnisotropy) {
        return PAL_RESULT_INVALID_ARGUMENT;
    }

    Sampler* sampler = nullptr;
    sampler = palAllocate(s_D3D12.allocator, sizeof(Sampler), 0);
    if (!sampler) {
        return PAL_RESULT_OUT_OF_MEMORY;
    }

    memset(sampler, 0, sizeof(Sampler));
    sampler->desc.MaxLOD = info->maxLod;
    sampler->desc.MinLOD = info->minLod;
    sampler->desc.MipLODBias = info->mipLodBias;

    sampler->desc.MaxAnisotropy = 1;
    if (info->enableAnisotropy) {
        sampler->desc.MaxAnisotropy = (UINT)info->maxAnisotropy;
    }

    sampler->desc.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
    if (info->enableCompare) {
        sampler->desc.ComparisonFunc = compareOpToD3D12(info->compareOp);
    }

    borderColorToD3D12(info->borderColor, sampler->desc.BorderColor);
    sampler->desc.AddressU = addressModeToD3D12(info->addressModeU);
    sampler->desc.AddressV = addressModeToD3D12(info->addressModeV);
    sampler->desc.AddressW = addressModeToD3D12(info->addressModeW);

    sampler->desc.Filter = filterToD3D12(
        info->minFilterMode,
        info->magFilterMode,
        info->mipmapMode);

    *outSampler = (PalSampler*)sampler;
    return PAL_RESULT_SUCCESS;
}

void PAL_CALL destroySamplerD3D12(PalSampler* sampler)
{
    Sampler* d3dSampler = (Sampler*)sampler;
    palFree(s_D3D12.allocator, d3dSampler);
}

// ==================================================
// Surface
// ==================================================

PalResult PAL_CALL createSurfaceD3D12(
    PalDevice* device,
    PalGraphicsWindow* window,
    PalSurface** outSurface)
{
    Device* d3d12Device = (Device*)device;
    if (!(d3d12Device->features & PAL_ADAPTER_FEATURE_SWAPCHAIN)) {
        return PAL_RESULT_ADAPTER_FEATURE_NOT_SUPPORTED;
    }

    Surface* surface = nullptr;
    surface = palAllocate(s_D3D12.allocator, sizeof(Surface), 0);
    if (!surface) {
        return PAL_RESULT_OUT_OF_MEMORY;
    }

    // validate if the window is valid
    if (!IsWindow((HWND)window->window)) {
        return PAL_RESULT_INVALID_GRAPHICS_WINDOW;
    }

    surface->handle = window->window;
    *outSurface = (PalSurface*)surface;
    return PAL_RESULT_SUCCESS;
}

void PAL_CALL destroySurfaceD3D12(PalSurface* surface)
{
    Surface* d3dSurface = (Surface*)surface;
    palFree(s_D3D12.allocator, d3dSurface);
}

PalResult PAL_CALL getSurfaceCapabilitiesD3D12(
    PalDevice* device,
    PalSurface* surface,
    PalSurfaceCapabilities* caps)
{
    HRESULT result;
    Surface* d3dSurface = (Surface*)surface;
    Device* d3d12Device = (Device*)device;
    PalBool supportHDR10 = PAL_FALSE;
    IDXGISwapChain1* swapchain1 = nullptr;
    IDXGISwapChain3* swapchain3 = nullptr;

    if (!(d3d12Device->features & PAL_ADAPTER_FEATURE_SWAPCHAIN)) {
        return PAL_RESULT_ADAPTER_FEATURE_NOT_SUPPORTED;
    }

    DXGI_SWAP_CHAIN_DESC1 desc = {0};
    desc.Width = 8;
    desc.Height = 8;
    desc.SampleDesc.Count = 1;
    desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    desc.BufferCount = 2;
    desc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
    desc.AlphaMode = DXGI_ALPHA_MODE_IGNORE;
    desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;

    result = s_D3D12.factory->lpVtbl->CreateSwapChainForHwnd(
        s_D3D12.factory,
        (IUnknown*)d3d12Device->queue,
        d3dSurface->handle,
        &desc,
        nullptr,
        nullptr,
        &swapchain1);

    if (FAILED(result)) {
        pollMessagesD3D12(d3d12Device);
        if (result == E_OUTOFMEMORY) {
            return PAL_RESULT_OUT_OF_MEMORY;
        }
        return PAL_RESULT_PLATFORM_FAILURE;
    }

    swapchain1->lpVtbl->QueryInterface(swapchain1, &IID_Swapchain, (void**)&swapchain3);
    swapchain1->lpVtbl->Release(swapchain1);

    // check for HDR10 color space support
    UINT flags = 0;
    result = swapchain3->lpVtbl->CheckColorSpaceSupport(
        swapchain3,
        DXGI_COLOR_SPACE_RGB_FULL_G2084_NONE_P2020,
        &flags);

    if (SUCCEEDED(result) && (flags & DXGI_SWAP_CHAIN_COLOR_SPACE_SUPPORT_FLAG_PRESENT)) {
        supportHDR10 = PAL_TRUE;
    }

    BOOL allowTearing = PAL_FALSE;
    s_D3D12.factory->lpVtbl->CheckFeatureSupport(
        s_D3D12.factory,
        DXGI_FEATURE_PRESENT_ALLOW_TEARING,
        &allowTearing,
        sizeof(allowTearing));

    caps->presentModes[PAL_PRESENT_MODE_FIFO] = PAL_TRUE;
    if (allowTearing) {
        caps->minImageCount = 3;
        caps->presentModes[PAL_PRESENT_MODE_IMMEDIATE] = PAL_TRUE;
        caps->presentModes[PAL_PRESENT_MODE_MAILBOX] = PAL_TRUE;

    } else {
        caps->minImageCount = 2;
        caps->presentModes[PAL_PRESENT_MODE_IMMEDIATE] = PAL_FALSE;
        caps->presentModes[PAL_PRESENT_MODE_MAILBOX] = PAL_FALSE;
    }

    caps->compositeAlphas[PAL_COMPOSITE_ALPHA_OPAQUE] = PAL_TRUE;
    caps->compositeAlphas[PAL_COMPOSITE_ALPHA_PRE_MULTIPLIED] = PAL_FALSE;
    caps->compositeAlphas[PAL_COMPOSITE_ALPHA_POST_MULTIPLIED] = PAL_FALSE;

    caps->maxImageCount = 8; // safe default
    caps->minImageWidth = 1;
    caps->minImageHeight = 1;
    caps->maxImageWidth = D3D12_REQ_TEXTURE2D_U_OR_V_DIMENSION;
    caps->maxImageHeight = D3D12_REQ_TEXTURE2D_U_OR_V_DIMENSION;
    caps->maxImageArrayLayers = 1;

    // check support for the base format
    D3D12_FEATURE_DATA_FORMAT_SUPPORT formatSupport = {0};
    DXGI_FORMAT baseFormats[PAL_SURFACE_FORMAT_MAX];
    baseFormats[0] = DXGI_FORMAT_B8G8R8A8_UNORM;
    baseFormats[1] = DXGI_FORMAT_B8G8R8A8_UNORM_SRGB;
    baseFormats[2] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
    baseFormats[3] = DXGI_FORMAT_R16G16B16A16_FLOAT;

    caps->formats[PAL_SURFACE_FORMAT_BGRA8_UNORM_SRGB_NONLINEAR] = PAL_FALSE;
    caps->formats[PAL_SURFACE_FORMAT_BGRA8_SRGB_NONLINEAR] = PAL_FALSE;
    caps->formats[PAL_SURFACE_FORMAT_RGBA8_UNORM_SRGB_NONLINEAR] = PAL_FALSE;
    caps->formats[PAL_SURFACE_FORMAT_RGBA16_FLOAT_HDR10] = PAL_FALSE;

    for (int i = 0; i < PAL_SURFACE_FORMAT_MAX; i++) {
        formatSupport.Format = baseFormats[i];
        result = d3d12Device->handle->lpVtbl->CheckFeatureSupport(
            d3d12Device->handle,
            D3D12_FEATURE_FORMAT_SUPPORT,
            &formatSupport,
            sizeof(formatSupport));

        if (SUCCEEDED(result) && (formatSupport.Support1 != 0 || formatSupport.Support2 != 0)) {
            if (baseFormats[i] == DXGI_FORMAT_B8G8R8A8_UNORM) {
                caps->formats[PAL_SURFACE_FORMAT_BGRA8_UNORM_SRGB_NONLINEAR] = PAL_TRUE;
            }

            if (baseFormats[i] == DXGI_FORMAT_B8G8R8A8_UNORM_SRGB) {
                caps->formats[PAL_SURFACE_FORMAT_BGRA8_SRGB_NONLINEAR] = PAL_TRUE;
            }

            if (baseFormats[i] == DXGI_FORMAT_R8G8B8A8_UNORM_SRGB) {
                caps->formats[PAL_SURFACE_FORMAT_RGBA8_UNORM_SRGB_NONLINEAR] = PAL_TRUE;
            }

            if (baseFormats[i] == DXGI_FORMAT_R16G16B16A16_FLOAT) {
                // check HDR10 color space
                if (supportHDR10) {
                    caps->formats[PAL_SURFACE_FORMAT_RGBA16_FLOAT_HDR10] = PAL_TRUE;
                }
            }
        }
    }

    swapchain3->lpVtbl->Release(swapchain3);
    return PAL_RESULT_SUCCESS;
}

// ==================================================
// Swapchain
// ==================================================

PalResult PAL_CALL createSwapchainD3D12(
    PalDevice* device,
    PalQueue* queue,
    PalSurface* surface,
    const PalSwapchainCreateInfo* info,
    PalSwapchain** outSwapchain)
{
    HRESULT result;
    Surface* d3dSurface = (Surface*)surface;
    Device* d3d12Device = (Device*)device;
    Queue* d3dQueue = (Queue*)queue;
    Swapchain* swapchain = nullptr;
    PalBool isHDRColorspace = PAL_FALSE;

    if (!(d3d12Device->features & PAL_ADAPTER_FEATURE_SWAPCHAIN)) {
        return PAL_RESULT_ADAPTER_FEATURE_NOT_SUPPORTED;
    }

    if (d3dQueue->type != PAL_QUEUE_TYPE_GRAPHICS) {
        return PAL_RESULT_INVALID_QUEUE;
    }

    if (info->compositeAlpha != PAL_COMPOSITE_ALPHA_OPAQUE) {
        return PAL_RESULT_INVALID_ARGUMENT;
    }

    if (info->presentMode == PAL_PRESENT_MODE_MAILBOX && info->imageCount < 3) {
        return PAL_RESULT_INVALID_ARGUMENT;
    }

    if (info->imageCount > 8) {
        return PAL_RESULT_INVALID_ARGUMENT;
    }

    swapchain = palAllocate(s_D3D12.allocator, sizeof(Swapchain), 0);
    if (!swapchain) {
        return PAL_RESULT_OUT_OF_MEMORY;
    }

    IDXGISwapChain1* swapchain1 = nullptr;
    DXGI_SWAP_CHAIN_DESC1 desc = {0};
    desc.Width = info->width;
    desc.Height = info->height;
    desc.SampleDesc.Count = 1;
    desc.BufferCount = info->imageCount;
    desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    desc.Stereo = PAL_FALSE;
    desc.AlphaMode = DXGI_ALPHA_MODE_IGNORE;
    desc.Scaling = DXGI_SCALING_NONE;

    desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
    if (info->presentMode == PAL_PRESENT_MODE_FIFO) {
        swapchain->presentFlags = 0;
        swapchain->syncInterval = 1;

    } else if (info->presentMode == PAL_PRESENT_MODE_IMMEDIATE) {
        desc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING;
        swapchain->presentFlags = DXGI_PRESENT_ALLOW_TEARING;
        swapchain->syncInterval = 0;

    } else {
        desc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING;
        swapchain->presentFlags = DXGI_PRESENT_ALLOW_TEARING;
        swapchain->syncInterval = 0;
    }

    PalFormat imageFormat;
    if (info->format == PAL_SURFACE_FORMAT_BGRA8_UNORM_SRGB_NONLINEAR) {
        desc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
        imageFormat = PAL_FORMAT_B8G8R8A8_UNORM;

    } else if (info->format == PAL_SURFACE_FORMAT_BGRA8_SRGB_NONLINEAR) {
        desc.Format = DXGI_FORMAT_B8G8R8A8_UNORM_SRGB;
        imageFormat = PAL_FORMAT_B8G8R8A8_SRGB;

    } else if (info->format == PAL_SURFACE_FORMAT_RGBA8_UNORM_SRGB_NONLINEAR) {
        desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
        imageFormat = PAL_FORMAT_R8G8B8A8_SRGB;

    } else if (info->format == PAL_SURFACE_FORMAT_RGBA16_FLOAT_HDR10) {
        desc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
        imageFormat = PAL_FORMAT_R16G16B16A16_SFLOAT;
        isHDRColorspace = PAL_TRUE;
    }

    swapchain->format = desc.Format;
    swapchain->flags = desc.Flags;
    result = s_D3D12.factory->lpVtbl->CreateSwapChainForHwnd(
        s_D3D12.factory,
        (IUnknown*)d3dQueue->handle,
        d3dSurface->handle,
        &desc,
        nullptr,
        nullptr,
        &swapchain1);

    if (FAILED(result)) {
        pollMessagesD3D12(d3d12Device);
        if (result == E_OUTOFMEMORY) {
            return PAL_RESULT_OUT_OF_MEMORY;
        } else if (result == E_INVALIDARG) {
            return PAL_RESULT_INVALID_ARGUMENT;
        }
        return PAL_RESULT_PLATFORM_FAILURE;
    }

    swapchain1->lpVtbl->QueryInterface(swapchain1, &IID_Swapchain, (void**)&swapchain->handle);
    swapchain1->lpVtbl->Release(swapchain1);

    if (isHDRColorspace) {
        swapchain->handle->lpVtbl->SetColorSpace1(
            swapchain->handle,
            DXGI_COLOR_SPACE_RGB_FULL_G2084_NONE_P2020);
    } else {
        swapchain->handle->lpVtbl->SetColorSpace1(
            swapchain->handle, DXGI_COLOR_SPACE_RGB_FULL_G22_NONE_P709);
    }

    // get and cache swapchain images
    swapchain->images = nullptr;
    swapchain->images = palAllocate(s_D3D12.allocator, sizeof(Image) * info->imageCount, 0);
    if (!swapchain->imageCount) {
        return PAL_RESULT_OUT_OF_MEMORY;
    }

    // fill all images with the creatio info
    for (int i = 0; i < info->imageCount; i++) {
        ID3D12Resource* tmp = nullptr;
        swapchain->handle->lpVtbl->GetBuffer(swapchain->handle, i, &IID_Resource, (void**)&tmp);

        Image* image = &swapchain->images[i];
        image->belongsToSwapchain = PAL_TRUE;
        image->device = d3d12Device;
        image->handle = tmp;

        image->info.depthOrArraySize = 1; // always 1
        image->info.format = imageFormat;
        image->info.usages = PAL_IMAGE_USAGE_COLOR_ATTACHEMENT;
        image->info.height = info->height;
        image->info.width = info->width;
        image->info.mipLevelCount = 1;
        image->info.sampleCount = PAL_SAMPLE_COUNT_1; // swapchain images are not multisampled
        image->info.type = PAL_IMAGE_TYPE_2D;
    }

    // get and cache window size for swapchain out of date error
    RECT windowRect;
    GetClientRect((HWND)d3dSurface->handle, &windowRect);
    swapchain->windowWidth = windowRect.right - windowRect.left;
    swapchain->windowWidth = windowRect.bottom - windowRect.top;

    swapchain->device = d3d12Device;
    swapchain->queue = d3dQueue->handle;
    swapchain->imageCount = info->imageCount;
    *outSwapchain = (PalSwapchain*)swapchain;
    return PAL_RESULT_SUCCESS;
}

void PAL_CALL destroySwapchainD3D12(PalSwapchain* swapchain)
{
    Swapchain* d3dSwapchain = (Swapchain*)swapchain;
    d3dSwapchain->handle->lpVtbl->Release(d3dSwapchain->handle);
    palFree(s_D3D12.allocator, d3dSwapchain->images);
    palFree(s_D3D12.allocator, d3dSwapchain);
}

PalImage* PAL_CALL getSwapchainImageD3D12(
    PalSwapchain* swapchain,
    uint32_t index)
{
    Swapchain* d3dSwapchain = (Swapchain*)swapchain;
    if (index > d3dSwapchain->imageCount) {
        return nullptr;
    }
    return (PalImage*)&d3dSwapchain->images[index];
}

PalResult PAL_CALL getNextSwapchainImageD3D12(
    PalSwapchain* swapchain,
    PalSwapchainNextImageInfo* info,
    uint32_t* outIndex)
{
    uint32_t index = 0;
    Swapchain* d3dSwapchain = (Swapchain*)swapchain;
    ID3D12CommandQueue* queue = d3dSwapchain->queue;

    index = d3dSwapchain->handle->lpVtbl->GetCurrentBackBufferIndex(d3dSwapchain->handle);
    if (info->fence) {
        Fence* fence = (Fence*)info->fence;
        fence->value++;
        queue->lpVtbl->Signal(queue, fence->handle, fence->value);
    }

    if (info->signalSemaphore) {
        Semaphore* semaphore = (Semaphore*)info->signalSemaphore;
        if (semaphore->isTimeline) {
            queue->lpVtbl->Signal(queue, semaphore->handle, info->signalValue);
        } else {
            semaphore->value = 1;
            queue->lpVtbl->Signal(queue, semaphore->handle, semaphore->value);
        }
    }

    *outIndex = index;
    return PAL_RESULT_SUCCESS;
}

PalResult PAL_CALL presentSwapchainD3D12(
    PalSwapchain* swapchain,
    uint32_t imageIndex, 
    PalSemaphore* waitSemaphore)
{
    HRESULT result;
    Swapchain* d3dSwapchain = (Swapchain*)swapchain;
    ID3D12CommandQueue* queue = d3dSwapchain->queue;

    if (waitSemaphore) {
        Semaphore* semaphore = (Semaphore*)waitSemaphore;
        if (semaphore->isTimeline) {
            queue->lpVtbl->Wait(queue, semaphore->handle, info->waitValue);
        } else {
            queue->lpVtbl->Wait(queue, semaphore->handle, semaphore->value);
            semaphore->handle->lpVtbl->Signal(semaphore->handle, 0);
            semaphore->value = 0;
        }
    }

    result = d3dSwapchain->handle->lpVtbl->Present(
        d3dSwapchain->handle,
        d3dSwapchain->syncInterval,
        d3dSwapchain->presentFlags);

    if (FAILED(result)) {
        pollMessagesD3D12(d3dSwapchain->device);
        if (result == DXGI_ERROR_DEVICE_REMOVED || result == DXGI_ERROR_DEVICE_RESET) {
            return PAL_RESULT_DEVICE_LOST;
        }

        // check if swapchain needs to be resize
        RECT windowRect;
        PalBool ret = GetClientRect((HWND)d3dSwapchain->surface->handle, &windowRect);
        uint32_t w = windowRect.right - windowRect.left;
        uint32_t h = windowRect.bottom - windowRect.top;

        if (!ret) {
            return PAL_RESULT_SURFACE_LOST;
        }

        if (w != d3dSwapchain->windowWidth || h != d3dSwapchain->windowHeight) {
            return PAL_RESULT_SWAPCHAIN_OUT_OF_DATE;
        }
        return PAL_RESULT_PLATFORM_FAILURE;
    }

    return PAL_RESULT_SUCCESS;
}

PalResult PAL_CALL resizeSwapchainD3D12(
    PalSwapchain* swapchain,
    uint32_t newWidth,
    uint32_t newHeight)
{
    HRESULT result;
    Swapchain* d3dSwapchain = (Swapchain*)swapchain;
    result = d3dSwapchain->handle->lpVtbl->ResizeBuffers(
        d3dSwapchain->handle,
        d3dSwapchain->imageCount,
        newWidth,
        newHeight,
        d3dSwapchain->format,
        d3dSwapchain->flags);

    if (FAILED(result)) {
        pollMessagesD3D12(d3dSwapchain->device);
        if (result == E_INVALIDARG) {
            return PAL_RESULT_INVALID_ARGUMENT;
        }
        return PAL_RESULT_PLATFORM_FAILURE;
    }

    // fill all images with the creatio info
    for (int i = 0; i < d3dSwapchain->imageCount; i++) {
        ID3D12Resource* tmp = nullptr;
        d3dSwapchain->handle->lpVtbl->GetBuffer(
            d3dSwapchain->handle,
            i,
            &IID_Resource,
            (void**)&tmp);

        Image* image = &d3dSwapchain->images[i];
        image->handle = tmp;
        image->info.height = newHeight;
        image->info.width = newWidth;
    }

    // get and cache window size for swapchain out of date error
    RECT windowRect;
    Surface* surface = d3dSwapchain->surface;
    if (!GetClientRect((HWND)surface->handle, &windowRect)) {
        return PAL_RESULT_SURFACE_LOST;
    }

    d3dSwapchain->windowWidth = windowRect.right - windowRect.left;
    d3dSwapchain->windowWidth = windowRect.bottom - windowRect.top;
    return PAL_RESULT_SUCCESS;
}

// ==================================================
// Shader
// ==================================================

PalResult PAL_CALL createShaderD3D12(
    PalDevice* device,
    const PalShaderCreateInfo* info,
    PalShader** outShader)
{
    Shader* shader = nullptr;
    Device* d3d12Device = (Device*)device;
    void* bytecode = nullptr;

    shader = palAllocate(s_D3D12.allocator, sizeof(Shader), 0);
    bytecode = palAllocate(s_D3D12.allocator, info->bytecodeSize, 0);
    if (!shader || !bytecode) {
        return PAL_RESULT_OUT_OF_MEMORY;
    }

    // allocate entries array
    shader->entries = palAllocate(s_D3D12.allocator, sizeof(ShaderEntry) * info->entryCount, 0);
    if (!shader->entries) {
        return PAL_RESULT_OUT_OF_MEMORY;
    }

    for (int i = 0; i < info->entryCount; i++) {
        ShaderEntry* entry = &shader->entries[i];

        // clang-format off
        if (info->entries[i].stage == PAL_SHADER_STAGE_MESH || 
            info->entries[i].stage == PAL_SHADER_STAGE_TASK) {
            if (!(d3d12Device->features & PAL_ADAPTER_FEATURE_MESH_SHADER)) {
                return PAL_RESULT_ADAPTER_FEATURE_NOT_SUPPORTED;
            }

        } else if (info->entries[i].stage == PAL_SHADER_STAGE_GEOMETRY) {
            if (!(d3d12Device->features & PAL_ADAPTER_FEATURE_GEOMETRY_SHADER)) {
                return PAL_RESULT_ADAPTER_FEATURE_NOT_SUPPORTED;
            }

        } else if (info->entries[i].stage == PAL_SHADER_STAGE_TESSELLATION_CONTROL || 
                info->entries[i].stage == PAL_SHADER_STAGE_TESSELLATION_EVALUATION) {
            if (!(d3d12Device->features & PAL_ADAPTER_FEATURE_TESSELLATION_SHADER)) {
                return PAL_RESULT_ADAPTER_FEATURE_NOT_SUPPORTED;
            }

        } else if (info->entries[i].stage == PAL_SHADER_STAGE_RAYGEN ||
                info->entries[i].stage == PAL_SHADER_STAGE_CLOSEST_HIT ||
                info->entries[i].stage == PAL_SHADER_STAGE_ANY_HIT ||
                info->entries[i].stage == PAL_SHADER_STAGE_MISS ||
                info->entries[i].stage == PAL_SHADER_STAGE_INTERSECTION ||
                info->entries[i].stage == PAL_SHADER_STAGE_CALLABLE) {
            if (!(d3d12Device->features & PAL_ADAPTER_FEATURE_RAY_TRACING)) {
                return PAL_RESULT_ADAPTER_FEATURE_NOT_SUPPORTED;
            }
        }
        // clang-format on

        convertToWcharD3D12(info->entries[i].entryName, entry->entryName);
        entry->patchControlPoints = info->entries[i].patchControlPoints;
        entry->stage = info->entries[i].stage; 
    }

    memcpy(bytecode, info->bytecode, info->bytecodeSize);
    shader->byteCode.pShaderBytecode = bytecode;
    shader->byteCode.BytecodeLength = info->bytecodeSize;

    shader->entryCount = info->entryCount;
    *outShader = (PalShader*)shader;
    return PAL_RESULT_SUCCESS;
}

void PAL_CALL destroyShaderD3D12(PalShader* shader)
{
    Shader* d3dShader = (Shader*)shader;
    palFree(s_D3D12.allocator, (void*)d3dShader->byteCode.pShaderBytecode);
    palFree(s_D3D12.allocator, d3dShader->entries);
    palFree(s_D3D12.allocator, d3dShader);
}

// ==================================================
// Command Pool And Buffer
// ==================================================

PalResult PAL_CALL createCommandPoolD3D12(
    PalDevice* device,
    PalQueue* queue,
    PalCommandPool** outPool)
{
    HRESULT result;
    CommandPool* pool = nullptr;
    Queue* d3dQueue = (Queue*)queue;

    pool = palAllocate(s_D3D12.allocator, sizeof(CommandPool), 0);
    if (!pool) {
        return PAL_RESULT_OUT_OF_MEMORY;
    }

    pool->size = 8;
    pool->cmdBuffersData = nullptr;
    uint32_t size =  sizeof(CommandBufferData) * pool->size;
    pool->cmdBuffersData = palAllocate(s_D3D12.allocator,  size, 0);
    if (!pool->cmdBuffersData) {
        return PAL_RESULT_OUT_OF_MEMORY;
    }

    switch (d3dQueue->type) {
        case PAL_QUEUE_TYPE_COMPUTE: {
            pool->type = D3D12_COMMAND_LIST_TYPE_COMPUTE;
            break;
        }

        case PAL_QUEUE_TYPE_GRAPHICS: {
            pool->type = D3D12_COMMAND_LIST_TYPE_DIRECT;
            break;
        }

        case PAL_QUEUE_TYPE_COPY: {
            pool->type = D3D12_COMMAND_LIST_TYPE_COPY;
            break;
        }
    }

    *outPool = (PalCommandPool*)pool;
    return PAL_RESULT_SUCCESS;
}

void PAL_CALL destroyCommandPoolD3D12(PalCommandPool* pool)
{
    CommandPool* cmdPool = (CommandPool*)pool;
    for (int i = 0; i < cmdPool->size; i++) {
        if (!cmdPool->cmdBuffersData[i].used) {
            continue;
        }

        CommandBuffer* cmdBuffer = cmdPool->cmdBuffersData[i].cmdBuffer;
        cmdBuffer->handle->lpVtbl->Release(cmdBuffer->handle);
        cmdBuffer->allocator->lpVtbl->Release(cmdBuffer->allocator);
    }

    palFree(s_D3D12.allocator, cmdPool->cmdBuffersData);
    palFree(s_D3D12.allocator, cmdPool);
}

PalResult PAL_CALL resetCommandPoolD3D12(PalCommandPool* pool)
{
    CommandPool* cmdPool = (CommandPool*)pool;
    for (int i = 0; i < cmdPool->size; i++) {
        if (!cmdPool->cmdBuffersData[i].used) {
            continue;
        }

        CommandBuffer* cmdBuffer = cmdPool->cmdBuffersData[i].cmdBuffer;
        cmdBuffer->handle->lpVtbl->Reset(cmdBuffer->handle, cmdBuffer->allocator, nullptr);
    }
    return PAL_RESULT_SUCCESS;
}

PalResult PAL_CALL allocateCommandBufferD3D12(
    PalDevice* device,
    PalCommandPool* pool,
    PalCommandBufferType type,
    PalCommandBuffer** outCmdBuffer)
{
    HRESULT result;
    Device* d3d12Device = (Device*)device;
    CommandBuffer* cmdBuffer = nullptr;
    CommandPool* cmdPool = (CommandPool*)pool;

    cmdBuffer = palAllocate(s_D3D12.allocator, sizeof(CommandBuffer), 0);
    if (!cmdBuffer) {
        return PAL_RESULT_OUT_OF_MEMORY;
    }

    memset(cmdBuffer, 0, sizeof(CommandBuffer));
    cmdBuffer->primary = PAL_TRUE;

    D3D12_COMMAND_LIST_TYPE cmdBufferType = cmdPool->type;
    if (type == PAL_COMMAND_BUFFER_TYPE_SECONDARY) {
        cmdBufferType = D3D12_COMMAND_LIST_TYPE_BUNDLE;
        cmdBuffer->primary = PAL_FALSE;
    }

    // create an allocator
    result = d3d12Device->handle->lpVtbl->CreateCommandAllocator(
        d3d12Device->handle,
        cmdBufferType,
        &IID_CommandAllocator,
        (void**)&cmdBuffer->allocator);

    if (FAILED(result)) {
        pollMessagesD3D12(d3d12Device);
        if (result == E_OUTOFMEMORY) {
            return PAL_RESULT_OUT_OF_MEMORY;
        }
        return PAL_RESULT_PLATFORM_FAILURE;
    }

    // create the command list
    ID3D12GraphicsCommandList* cmdList = nullptr;
    result = d3d12Device->handle->lpVtbl->CreateCommandList(
        d3d12Device->handle,
        0,
        cmdBufferType,
        cmdBuffer->allocator,
        nullptr,
        &IID_CommandList,
        (void**)&cmdList);

    if (FAILED(result)) {
        pollMessagesD3D12(d3d12Device);
        if (result == E_OUTOFMEMORY) {
            return PAL_RESULT_OUT_OF_MEMORY;
        }
        return PAL_RESULT_PLATFORM_FAILURE;
    }

    // we need a tmp staging and gpu buffer if ray tracing is enabled
    if (d3d12Device->features & PAL_ADAPTER_FEATURE_RAY_TRACING) {
        D3D12_HEAP_PROPERTIES heapProps = {0};
        heapProps.Type = D3D12_HEAP_TYPE_DEFAULT;
        heapProps.VisibleNodeMask = 1;
        heapProps.CreationNodeMask = 1;

        D3D12_RESOURCE_DESC bufferDesc = {0};
        bufferDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
        bufferDesc.Width = sizeof(D3D12_DISPATCH_RAYS_DESC);
        bufferDesc.Height = 1;
        bufferDesc.DepthOrArraySize = 1;
        bufferDesc.MipLevels = 1;
        bufferDesc.SampleDesc.Count = 1;
        bufferDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

        result = d3d12Device->handle->lpVtbl->CreateCommittedResource(
            d3d12Device->handle,
            &heapProps,
            0,
            &bufferDesc,
            D3D12_RESOURCE_STATE_COMMON,
            nullptr,
            &IID_Resource, 
            (void**)&cmdBuffer->buffer);

        if (FAILED(result)) {
            if (result == E_OUTOFMEMORY) {
                return PAL_RESULT_OUT_OF_MEMORY;
            }
            return PAL_RESULT_PLATFORM_FAILURE;
        }

        // create staging buffer
        heapProps.Type = D3D12_HEAP_TYPE_UPLOAD;
        result = d3d12Device->handle->lpVtbl->CreateCommittedResource(
            d3d12Device->handle,
            &heapProps,
            0,
            &bufferDesc,
            D3D12_RESOURCE_STATE_GENERIC_READ,
            nullptr,
            &IID_Resource, 
            (void**)&cmdBuffer->stagingBuffer);

        if (FAILED(result)) {
            if (result == E_OUTOFMEMORY) {
                return PAL_RESULT_OUT_OF_MEMORY;
            }
            return PAL_RESULT_PLATFORM_FAILURE;
        }
    }

    result = cmdList->lpVtbl->QueryInterface(
        cmdList,
        &IID_CommandList6,
        (void**)&cmdBuffer->handle);

    if (FAILED(result)) {
        return PAL_RESULT_PLATFORM_FAILURE;
    }

    cmdList->lpVtbl->Release(cmdList);
    cmdBuffer->handle->lpVtbl->Close(cmdBuffer->handle);

    cmdBuffer->pool = cmdPool;
    cmdBuffer->device = d3d12Device;
    *outCmdBuffer = (PalCommandBuffer*)cmdBuffer;
    return PAL_RESULT_SUCCESS;
}

void PAL_CALL freeCommandBufferD3D12(PalCommandBuffer* cmdBuffer)
{
    CommandBuffer* d3dCmdBuffer = (CommandBuffer*)cmdBuffer;
    CommandPool* pool = d3dCmdBuffer->pool;
    CommandBufferData* data = findCmdBufferData(pool, d3dCmdBuffer);
    if (data) {
        d3dCmdBuffer->handle->lpVtbl->Release(d3dCmdBuffer->handle);
        d3dCmdBuffer->allocator->lpVtbl->Release(d3dCmdBuffer->allocator);

        if (d3dCmdBuffer->device->features & PAL_ADAPTER_FEATURE_RAY_TRACING) {
            d3dCmdBuffer->buffer->lpVtbl->Release(d3dCmdBuffer->buffer);
            d3dCmdBuffer->stagingBuffer->lpVtbl->Release(d3dCmdBuffer->stagingBuffer);
        }

        palFree(s_D3D12.allocator, cmdBuffer);
        data->cmdBuffer = nullptr;
        data->used = PAL_FALSE;
    }
}

PalResult PAL_CALL resetCommandBufferD3D12(PalCommandBuffer* cmdBuffer)
{
    HRESULT result;
    CommandBuffer* d3dCmdBuffer = (CommandBuffer*)cmdBuffer;
    result = d3dCmdBuffer->allocator->lpVtbl->Reset(d3dCmdBuffer->allocator);
    if (FAILED(result)) {
        pollMessagesD3D12(d3dCmdBuffer->device);
        if (result == E_INVALIDARG) {
            return PAL_RESULT_INVALID_COMMAND_BUFFER;
        }
        return PAL_RESULT_PLATFORM_FAILURE;
    }

    d3dCmdBuffer->handle->lpVtbl->Reset(
        d3dCmdBuffer->handle,
        d3dCmdBuffer->allocator,
        nullptr);

    d3dCmdBuffer->handle->lpVtbl->Close(d3dCmdBuffer->handle);
    d3dCmdBuffer->pipeline = nullptr;
    return PAL_RESULT_SUCCESS;
}

PalResult PAL_CALL submitCommandBufferD3D12(
    PalQueue* queue,
    PalCommandBufferSubmitInfo* info)
{
    HRESULT result;
    Queue* d3dQueue = (Queue*)queue;
    ID3D12CommandQueue* queueHandle = d3dQueue->handle;
    CommandBuffer* d3dCmdBuffer = (CommandBuffer*)info->cmdBuffer;

    // wait semaphore
    if (info->waitSemaphore) {
        Semaphore* semaphore = (Semaphore*)info->waitSemaphore;
        if (semaphore->isTimeline) {
            queueHandle->lpVtbl->Wait(queueHandle, semaphore->handle, info->waitValue);
        } else {
            queueHandle->lpVtbl->Wait(queueHandle, semaphore->handle, semaphore->value);
            semaphore->handle->lpVtbl->Signal(semaphore->handle, 0);
            semaphore->value = 0;
        }
    }


    ID3D12CommandList* cmdLists[1] = { (ID3D12CommandList*)d3dCmdBuffer->handle };
    queueHandle->lpVtbl->ExecuteCommandLists(queueHandle, 1, cmdLists);
    d3dQueue->fenceValue++;
    queueHandle->lpVtbl->Signal(queueHandle, d3dQueue->fence, d3dQueue->fenceValue);

    if (info->fence) {
        Fence* fence = (Fence*)info->fence;
        fence->value++;
        queueHandle->lpVtbl->Signal(queueHandle, fence->handle, fence->value);
    }

    if (info->signalSemaphore) {
        Semaphore* semaphore = (Semaphore*)info->signalSemaphore;
        if (semaphore->isTimeline) {
            queueHandle->lpVtbl->Signal(queueHandle, semaphore->handle, info->signalValue);
        } else {
            semaphore->value = 1;
            queueHandle->lpVtbl->Signal(queueHandle, semaphore->handle, semaphore->value);
        }
    }

    return PAL_RESULT_SUCCESS;
}

// ==================================================
// Command Recording
// ==================================================

PalResult PAL_CALL cmdBeginD3D12(
    PalCommandBuffer* cmdBuffer,
    PalRenderingLayoutInfo* info)
{
    HRESULT result;
    CommandBuffer* d3dCmdBuffer = (CommandBuffer*)cmdBuffer;
    result = d3dCmdBuffer->allocator->lpVtbl->Reset(d3dCmdBuffer->allocator);
    if (FAILED(result)) {
        pollMessagesD3D12(d3dCmdBuffer->device);
        if (result == E_INVALIDARG) {
            return PAL_RESULT_INVALID_COMMAND_BUFFER;
        }
        return PAL_RESULT_PLATFORM_FAILURE;
    }

    d3dCmdBuffer->handle->lpVtbl->Reset(
        d3dCmdBuffer->handle,
        d3dCmdBuffer->allocator,
        nullptr);

    return PAL_RESULT_SUCCESS;
}

PalResult PAL_CALL cmdEndD3D12(PalCommandBuffer* cmdBuffer)
{
    CommandBuffer* d3dCmdBuffer = (CommandBuffer*)cmdBuffer;
    d3dCmdBuffer->handle->lpVtbl->Close(d3dCmdBuffer->handle);
    return PAL_RESULT_SUCCESS;
}

PalResult PAL_CALL cmdExecuteCommandBufferD3D12(
    PalCommandBuffer* primaryCmdBuffer,
    PalCommandBuffer* secondaryCmdBuffer)
{
    CommandBuffer* d3dPrimaryCmdBuffer = (CommandBuffer*)primaryCmdBuffer;
    CommandBuffer* d3dSecondaryCmdBuffer = (CommandBuffer*)secondaryCmdBuffer;

    d3dPrimaryCmdBuffer->handle->lpVtbl->ExecuteBundle(
        d3dPrimaryCmdBuffer->handle,
        (ID3D12GraphicsCommandList*)d3dSecondaryCmdBuffer->handle);

    return PAL_RESULT_SUCCESS;
}

PalResult PAL_CALL cmdSetFragmentShadingRateD3D12(
    PalCommandBuffer* cmdBuffer,
    PalFragmentShadingRateState* state)
{
    HRESULT result;
    CommandBuffer* d3dCmdBuffer = (CommandBuffer*)cmdBuffer;
    Device* device = d3dCmdBuffer->device;
    if (!(device->features & PAL_ADAPTER_FEATURE_FRAGMENT_SHADING_RATE)) {
        return PAL_RESULT_ADAPTER_FEATURE_NOT_SUPPORTED;
    }

    D3D12_SHADING_RATE shadingRate = shadingRateToD3D12(state->rate);
    D3D12_SHADING_RATE_COMBINER combinerOps[2];
    for (int i = 0; i < 2; i++) {
        combinerOps[i] = combinerOpsToD3D12(state->combinerOps[i]);
    }

    d3dCmdBuffer->handle->lpVtbl->RSSetShadingRate(
        d3dCmdBuffer->handle,
        shadingRate,
        combinerOps);

    return PAL_RESULT_SUCCESS;
}

PalResult PAL_CALL cmdDrawMeshTasksD3D12(
    PalCommandBuffer* cmdBuffer,
    uint32_t groupCountX,
    uint32_t groupCountY,
    uint32_t groupCountZ)
{
    CommandBuffer* d3dCmdBuffer = (CommandBuffer*)cmdBuffer;
    Device* device = d3dCmdBuffer->device;
    if (!(device->features & PAL_ADAPTER_FEATURE_MESH_SHADER)) {
        return PAL_RESULT_ADAPTER_FEATURE_NOT_SUPPORTED;
    }

    d3dCmdBuffer->handle->lpVtbl->DispatchMesh(
        d3dCmdBuffer->handle,
        groupCountX,
        groupCountY,
        groupCountZ);

    return PAL_RESULT_SUCCESS;
}

PalResult PAL_CALL cmdDrawMeshTasksIndirectD3D12(
    PalCommandBuffer* cmdBuffer,
    PalBuffer* buffer,
    uint32_t drawCount)
{
    CommandBuffer* d3dCmdBuffer = (CommandBuffer*)cmdBuffer;
    Device* device = d3dCmdBuffer->device;
    Buffer* d3dBuffer = (Buffer*)buffer;
    if (!(device->features & PAL_ADAPTER_FEATURE_INDIRECT_DRAW_MESH)) {
        return PAL_RESULT_ADAPTER_FEATURE_NOT_SUPPORTED;
    }

    if (!d3dBuffer->hasIndirect) {
        return PAL_RESULT_INVALID_BUFFER;
    }
    
    d3dCmdBuffer->handle->lpVtbl->ExecuteIndirect(
        d3dCmdBuffer->handle,
        device->meshSignature,
        drawCount,
        d3dBuffer->handle,
        0,
        nullptr,
        0);

    return PAL_RESULT_SUCCESS;
}

PalResult PAL_CALL cmdDrawMeshTasksIndirectCountD3D12(
    PalCommandBuffer* cmdBuffer,
    PalBuffer* buffer,
    PalBuffer* countBuffer,
    uint32_t maxDrawCount)
{
    CommandBuffer* d3dCmdBuffer = (CommandBuffer*)cmdBuffer;
    Device* device = d3dCmdBuffer->device;
    if (!(device->features & PAL_ADAPTER_FEATURE_INDIRECT_DRAW_MESH_COUNT)) {
        return PAL_RESULT_ADAPTER_FEATURE_NOT_SUPPORTED;
    }

    Buffer* d3dBuffer = (Buffer*)buffer;
    Buffer* d3dCountBuffer = (Buffer*)countBuffer;
    if (!d3dBuffer->hasIndirect || !d3dCountBuffer->hasIndirect) {
        return PAL_RESULT_INVALID_BUFFER;
    }

    d3dCmdBuffer->handle->lpVtbl->ExecuteIndirect(
        d3dCmdBuffer->handle,
        device->meshSignature,
        maxDrawCount,
        d3dBuffer->handle,
        0,
        d3dCountBuffer->handle,
        0);

    return PAL_RESULT_SUCCESS;
}

PalResult PAL_CALL cmdBuildAccelerationStructureD3D12(
    PalCommandBuffer* cmdBuffer,
    PalAccelerationStructureBuildInfo* info)
{
    CommandBuffer* d3dCmdBuffer = (CommandBuffer*)cmdBuffer;
    Device* device = d3dCmdBuffer->device;
    if (!(device->features & PAL_ADAPTER_FEATURE_RAY_TRACING)) {
        return PAL_RESULT_ADAPTER_FEATURE_NOT_SUPPORTED;
    }

    D3D12_RAYTRACING_GEOMETRY_DESC* geometries = nullptr;
    AccelerationStructure* tmpAs = (AccelerationStructure*)info->src;
    AccelerationStructure* dstAs = (AccelerationStructure*)info->dst;
    D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_DESC buildInfo = {0};
    D3D12_GPU_VIRTUAL_ADDRESS srcAsAddress = 0;

    if (tmpAs) {
        srcAsAddress = tmpAs->address;
    }

    if (info->type == PAL_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL) {
        geometries = palAllocate(
            s_D3D12.allocator,
            sizeof(D3D12_RAYTRACING_GEOMETRY_DESC) * info->geometryCount,
            0);

        if (!geometries) {
            return PAL_RESULT_OUT_OF_MEMORY;
        }

        memset(geometries, 0, sizeof(D3D12_RAYTRACING_GEOMETRY_DESC) * info->geometryCount);
        PalBool success = fillBuildInfoD3D12(
            info,
            geometries,
            srcAsAddress,
            dstAs->address,
            &buildInfo);

        if (!success) {
            return PAL_RESULT_INVALID_ARGUMENT;
        }

        d3dCmdBuffer->handle->lpVtbl->BuildRaytracingAccelerationStructure(
            d3dCmdBuffer->handle,
            &buildInfo,
            0,
            nullptr);

        palFree(s_D3D12.allocator, geometries);

    } else {
        PalBool success = fillBuildInfoD3D12(
            info,
            nullptr,
            srcAsAddress,
            dstAs->address,
            &buildInfo);

        if (!success) {
            return PAL_RESULT_INVALID_ARGUMENT;
        }

        d3dCmdBuffer->handle->lpVtbl->BuildRaytracingAccelerationStructure(
            d3dCmdBuffer->handle,
            &buildInfo,
            0,
            nullptr);
    }

    return PAL_RESULT_SUCCESS;
}

PalResult PAL_CALL cmdBeginRenderingD3D12(
    PalCommandBuffer* cmdBuffer,
    PalRenderingInfo* info)
{
    CommandBuffer* d3dCmdBuffer = (CommandBuffer*)cmdBuffer;
    D3D12_CPU_DESCRIPTOR_HANDLE colorAttachments[MAX_ATTACHMENTS];
    D3D12_CPU_DESCRIPTOR_HANDLE depthStencilAttachment;
    D3D12_CPU_DESCRIPTOR_HANDLE* depthStencil = nullptr;
    D3D12_CPU_DESCRIPTOR_HANDLE fsrAttachment;

    for (int i = 0; i < info->colorAttachentCount; i++) {
        ImageView* tmp = (ImageView*)info->colorAttachments[i].imageView;

        RTVHeapAllocator* allocator = &tmp->device->rtvAllocator;
        uint32_t size = allocator->incrementSize;
        uint64_t base = allocator->baseOffset;
        colorAttachments[i].ptr = getDescriptorHandleD3D12(tmp->heapIndex, size, base);
    }

    if (info->depthStencilAttachment) {
        ImageView* tmp = (ImageView*)info->depthStencilAttachment->imageView;
        DSVHeapAllocator* allocator = &tmp->device->dsvAllocator;
        uint32_t size = allocator->incrementSize;
        uint64_t base = allocator->baseOffset;
        depthStencilAttachment.ptr = getDescriptorHandleD3D12(tmp->heapIndex, size, base);
        depthStencil = &depthStencilAttachment;
    }

    d3dCmdBuffer->handle->lpVtbl->OMSetRenderTargets(
        d3dCmdBuffer->handle,
        info->colorAttachentCount,
        colorAttachments,
        PAL_FALSE,
        depthStencil);

    for (int i = 0; i < info->colorAttachentCount; i++) {
        if (info->colorAttachments[i].loadOp == PAL_LOAD_OP_CLEAR) {
            float color[4];
            color[0] = info->colorAttachments[i].clearValue.color[0];
            color[1] = info->colorAttachments[i].clearValue.color[1];
            color[2] = info->colorAttachments[i].clearValue.color[2];
            color[3] = info->colorAttachments[i].clearValue.color[3];

            d3dCmdBuffer->handle->lpVtbl->ClearRenderTargetView(
                d3dCmdBuffer->handle,
                colorAttachments[i], color, 0, nullptr);
        }
    }

    if (info->depthStencilAttachment) {
        D3D12_CLEAR_FLAGS clearFlags = 0;
        UINT8 stencil = 0;
        float depth = 0;

        if (info->depthStencilAttachment->loadOp == PAL_LOAD_OP_CLEAR) {
            depth = info->depthStencilAttachment->clearValue.depth;
            clearFlags |= D3D12_CLEAR_FLAG_DEPTH;
        }

        if (info->depthStencilAttachment->stencilLoadOp == PAL_LOAD_OP_CLEAR) {
            stencil = (UINT8)info->depthStencilAttachment->clearValue.stencil;
            clearFlags |= D3D12_CLEAR_FLAG_STENCIL;
        }

        d3dCmdBuffer->handle->lpVtbl->ClearDepthStencilView(
            d3dCmdBuffer->handle,
            depthStencilAttachment,
            clearFlags,
            depth,
            stencil,
            0,
            nullptr);
    }

    if (info->fragmentShadingRateAttachment) {
        ImageView* tmp = (ImageView*)info->fragmentShadingRateAttachment->imageView;
        d3dCmdBuffer->handle->lpVtbl->RSSetShadingRateImage(
            d3dCmdBuffer->handle,
            tmp->image->handle);
    }

    return PAL_RESULT_SUCCESS;
}

PalResult PAL_CALL cmdEndRenderingD3D12(PalCommandBuffer* cmdBuffer)
{
    return PAL_RESULT_SUCCESS;
}

PalResult PAL_CALL cmdCopyBufferD3D12(
    PalCommandBuffer* cmdBuffer,
    PalBuffer* dst,
    PalBuffer* src,
    PalBufferCopyInfo* copyInfo)
{
    CommandBuffer* d3dCmdBuffer = (CommandBuffer*)cmdBuffer;
    Buffer* dstbuffer = (Buffer*)dst;
    Buffer* srcBuffer = (Buffer*)src;

    d3dCmdBuffer->handle->lpVtbl->CopyBufferRegion(
        d3dCmdBuffer->handle,
        dstbuffer->handle,
        copyInfo->dstOffset,
        srcBuffer->handle,
        copyInfo->srcOffset,
        copyInfo->size);

    return PAL_RESULT_SUCCESS;
}

PalResult PAL_CALL cmdCopyBufferToImageD3D12(
    PalCommandBuffer* cmdBuffer,
    PalImage* dstImage,
    PalBuffer* srcBuffer,
    PalBufferImageCopyInfo* copyInfo)
{
    CommandBuffer* d3dCmdBuffer = (CommandBuffer*)cmdBuffer;
    Image* dst = (Image*)dstImage;
    Buffer* src = (Buffer*)srcBuffer;

    D3D12_TEXTURE_COPY_LOCATION dstLocation = {0};
    dstLocation.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
    dstLocation.pResource = dst->handle;

    D3D12_TEXTURE_COPY_LOCATION srcLocation = {0};
    srcLocation.pResource = src->handle;
    srcLocation.Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;
    D3D12_PLACED_SUBRESOURCE_FOOTPRINT* footPrint = &srcLocation.PlacedFootprint;

    footPrint->Offset = copyInfo->bufferOffset;
    footPrint->Footprint.Width = copyInfo->imageWidth;
    footPrint->Footprint.Height = copyInfo->imageHeight;
    footPrint->Footprint.Depth = copyInfo->imageDepth;
    footPrint->Footprint.Format = formatToD3D12(dst->info.format);

    uint32_t imageFormatSize = getFormatSizeD3D12(dst->info.format);
    uint64_t rowPitch = alignD3D12((uint64_t)copyInfo->imageWidth * imageFormatSize, TEXTURE_PITCH);
    footPrint->Footprint.RowPitch = (UINT)rowPitch;

    uint32_t planeCount = 1;
    if (copyInfo->imageAspect == PAL_IMAGE_ASPECT_DEPTH_STENCIL) {
        planeCount = 2;
    }

    D3D12_BOX box = {0};
    box.right = copyInfo->imageWidth;
    box.bottom = copyInfo->imageHeight;
    box.back = copyInfo->imageDepth;

    uint32_t level = copyInfo->ImageMipLevel;
    uint32_t startLayer = copyInfo->ImageStartArrayLayer;
    uint32_t layerCount = copyInfo->ImageArrayLayerCount;
    uint32_t maxLayers = dst->info.depthOrArraySize;
    uint32_t maxLevels = dst->info.mipLevelCount;

    for (uint32_t plane = 0; plane < planeCount; plane++) {
        for (uint32_t layer = startLayer; layer < startLayer + layerCount; layer++) {
            uint32_t index = level + (layer * maxLevels) + (plane * maxLevels * maxLayers);

            dstLocation.SubresourceIndex = index;
            d3dCmdBuffer->handle->lpVtbl->CopyTextureRegion(
                d3dCmdBuffer->handle,
                &dstLocation,
                copyInfo->imageOffsetX,
                copyInfo->imageOffsetY,
                copyInfo->imageOffsetZ,
                &srcLocation,
                &box);
        }
    }

    return PAL_RESULT_SUCCESS;
}

PalResult PAL_CALL cmdCopyImageD3D12(
    PalCommandBuffer* cmdBuffer,
    PalImage* dst,
    PalImage* src,
    PalImageCopyInfo* copyInfo)
{
    CommandBuffer* d3dCmdBuffer = (CommandBuffer*)cmdBuffer;
    Image* dstImage = (Image*)dst;
    Image* srcImage = (Image*)src;

    D3D12_TEXTURE_COPY_LOCATION dstLocation = {0};
    dstLocation.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
    dstLocation.pResource = dstImage->handle;

    D3D12_TEXTURE_COPY_LOCATION srcLocation = {0};
    srcLocation.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
    srcLocation.pResource = srcImage->handle;

    D3D12_BOX box = {0};
    box.left = copyInfo->srcOffsetX;
    box.top = copyInfo->srcOffsetY;
    box.front = copyInfo->srcOffsetZ;
    box.right = copyInfo->srcOffsetX + copyInfo->width;
    box.bottom = copyInfo->srcOffsetY + copyInfo->height;
    box.back = copyInfo->srcOffsetZ + copyInfo->depth;

    uint32_t planeCount = 1;
    uint32_t layerCount = copyInfo->arrayLayerCount;
    if (copyInfo->aspect == PAL_IMAGE_ASPECT_DEPTH_STENCIL) {
        planeCount = 2;
    }

    uint32_t dstLevel = copyInfo->dstMipLevel;
    uint32_t dstStartLayer = copyInfo->dstStartArrayLayer;
    uint32_t dstMaxLayers = dstImage->info.depthOrArraySize;
    uint32_t dstMaxLevels = dstImage->info.mipLevelCount;

    uint32_t srcLevel = copyInfo->srcMipLevel;
    uint32_t srcStartLayer = copyInfo->srcStartArrayLayer;
    uint32_t srcMaxLayers = srcImage->info.depthOrArraySize;
    uint32_t srcMaxLevels = srcImage->info.mipLevelCount;

    for (uint32_t plane = 0; plane < planeCount; plane++) {
        for (uint32_t layer = 0; layer + layerCount; layer++) {
            // clang-format off
            uint32_t dstIndex = dstLevel + (dstStartLayer + layer * dstMaxLevels) + (plane * dstMaxLevels * dstMaxLayers);
            uint32_t srcIndex = srcLevel + (srcStartLayer + layer * srcMaxLevels) + (plane * srcMaxLevels * srcMaxLayers);
            // clang-format on

            dstLocation.SubresourceIndex = dstIndex;
            srcLocation.SubresourceIndex = srcIndex;

            d3dCmdBuffer->handle->lpVtbl->CopyTextureRegion(
                d3dCmdBuffer->handle,
                &dstLocation,
                copyInfo->dstOffsetX,
                copyInfo->dstOffsetY,
                copyInfo->dstOffsetZ,
                &srcLocation,
                &box);
        }
    }

    return PAL_RESULT_SUCCESS;
}

PalResult PAL_CALL cmdCopyImageToBufferD3D12(
    PalCommandBuffer* cmdBuffer,
    PalBuffer* dstBuffer,
    PalImage* srcImage,
    PalBufferImageCopyInfo* copyInfo)
{
    CommandBuffer* d3dCmdBuffer = (CommandBuffer*)cmdBuffer;
    Buffer* dst = (Buffer*)dstBuffer;
    Image* src = (Image*)srcImage;

    D3D12_TEXTURE_COPY_LOCATION dstLocation = {0};
    dstLocation.pResource = dst->handle;
    dstLocation.Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;
    D3D12_PLACED_SUBRESOURCE_FOOTPRINT* footPrint = &dstLocation.PlacedFootprint;
    footPrint->Offset = copyInfo->bufferOffset;
    footPrint->Footprint.Width = copyInfo->imageWidth;
    footPrint->Footprint.Height = copyInfo->imageHeight;
    footPrint->Footprint.Depth = copyInfo->imageDepth;
    footPrint->Footprint.Format = formatToD3D12(src->info.format);

    D3D12_TEXTURE_COPY_LOCATION srcLocation = {0};
    srcLocation.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
    srcLocation.pResource = src->handle;

    uint32_t imageFormatSize = getFormatSizeD3D12(src->info.format);
    uint64_t rowPitch = alignD3D12((uint64_t)copyInfo->imageWidth * imageFormatSize, TEXTURE_PITCH);
    footPrint->Footprint.RowPitch = (UINT)rowPitch;

    D3D12_BOX box = {0};
    box.left = copyInfo->imageOffsetX;
    box.top = copyInfo->imageOffsetY;
    box.front = copyInfo->imageOffsetZ;
    box.right = copyInfo->imageOffsetX + copyInfo->imageWidth;
    box.bottom = copyInfo->imageOffsetY + copyInfo->imageHeight;
    box.back = copyInfo->imageOffsetX + copyInfo->imageDepth;

    uint32_t planeCount = 1;
    if (copyInfo->imageAspect == PAL_IMAGE_ASPECT_DEPTH_STENCIL) {
        planeCount = 2;
    }

    uint32_t level = copyInfo->ImageMipLevel;
    uint32_t startLayer = copyInfo->ImageStartArrayLayer;
    uint32_t layerCount = copyInfo->ImageArrayLayerCount;
    uint32_t maxLayers = src->info.depthOrArraySize;
    uint32_t maxLevels = src->info.mipLevelCount;

    for (uint32_t plane = 0; plane < planeCount; plane++) {
        for (uint32_t layer = startLayer; layer < startLayer + layerCount; layer++) {
            uint32_t index = level + (layer * maxLevels) + (plane * maxLevels * maxLayers);

            srcLocation.SubresourceIndex = index;
            d3dCmdBuffer->handle->lpVtbl->CopyTextureRegion(
                d3dCmdBuffer->handle,
                &dstLocation,
                0,
                0,
                0,
                &srcLocation,
                &box);
        }
    }

    return PAL_RESULT_SUCCESS;
}

PalResult PAL_CALL cmdBindPipelineD3D12(
    PalCommandBuffer* cmdBuffer,
    PalPipeline* pipeline)
{
    CommandBuffer* d3dCmdBuffer = (CommandBuffer*)cmdBuffer;
    Pipeline* d3dPipeline = (Pipeline*)pipeline;

    if (d3dPipeline->type == RAY_TRACING_PIPELINE) {
        d3dCmdBuffer->handle->lpVtbl->SetPipelineState1(
            d3dCmdBuffer->handle,
            d3dPipeline->handle);

        d3dCmdBuffer->handle->lpVtbl->SetComputeRootSignature(
            d3dCmdBuffer->handle, 
            d3dPipeline->layout->handle);

    } else {
        d3dCmdBuffer->handle->lpVtbl->SetPipelineState(
            d3dCmdBuffer->handle,
            d3dPipeline->handle);

        if (d3dPipeline->type == GRAPHICS_PIPELINE) {
            d3dCmdBuffer->handle->lpVtbl->IASetPrimitiveTopology(
                d3dCmdBuffer->handle,
                d3dPipeline->topology);

            d3dCmdBuffer->handle->lpVtbl->SetGraphicsRootSignature(
                d3dCmdBuffer->handle, 
                d3dPipeline->layout->handle);

            if (d3dPipeline->hasFsr) {
                d3dCmdBuffer->handle->lpVtbl->RSSetShadingRate(
                    d3dCmdBuffer->handle,
                    d3dPipeline->shadingRate,
                    d3dPipeline->combinerOps);
            }
        }
    }

    d3dCmdBuffer->pipeline = d3dPipeline;
    return PAL_RESULT_SUCCESS;
}

PalResult PAL_CALL cmdSetViewportD3D12(
    PalCommandBuffer* cmdBuffer,
    uint32_t count,
    PalViewport* viewports)
{
    CommandBuffer* d3dCmdBuffer = (CommandBuffer*)cmdBuffer;
    D3D12_VIEWPORT cachedViewport;
    D3D12_VIEWPORT* d3dViewports = nullptr;

    if (count > 1) {
        d3dViewports = palAllocate(s_D3D12.allocator, sizeof(D3D12_VIEWPORT) * count, 0);
        if (!d3dViewports) {
            return PAL_RESULT_OUT_OF_MEMORY;
        }

    } else {
        d3dViewports = &cachedViewport;
    }

    for (int i = 0; i < count; i++) {
        D3D12_VIEWPORT* tmp = &d3dViewports[i];
        tmp->TopLeftX = viewports[i].x;
        tmp->TopLeftY = viewports[i].y;
        tmp->Width = viewports[i].width;
        tmp->Height = viewports[i].height;
        tmp->MinDepth = viewports[i].minDepth;
        tmp->MaxDepth = viewports[i].maxDepth;
    }

    d3dCmdBuffer->handle->lpVtbl->RSSetViewports(d3dCmdBuffer->handle, count, d3dViewports);
    if (count > 1) {
        palFree(s_D3D12.allocator, d3dViewports);
    }
    return PAL_RESULT_SUCCESS;
}

PalResult PAL_CALL cmdSetScissorsD3D12(
    PalCommandBuffer* cmdBuffer,
    uint32_t count,
    PalRect2D* scissors)
{
    CommandBuffer* d3dCmdBuffer = (CommandBuffer*)cmdBuffer;
    D3D12_RECT cachedScissor;
    D3D12_RECT* d3dScissors = nullptr;

    if (count > 1) {
        d3dScissors = palAllocate(s_D3D12.allocator, sizeof(D3D12_RECT) * count, 0);
        if (!d3dScissors) {
            return PAL_RESULT_OUT_OF_MEMORY;
        }

    } else {
        d3dScissors = &cachedScissor;
    }

    for (int i = 0; i < count; i++) {
        D3D12_RECT* tmp = &d3dScissors[i];
        tmp->left = scissors[i].x;
        tmp->top = scissors[i].y;
        tmp->right = scissors[i].width;
        tmp->bottom = scissors[i].height;
    }

    d3dCmdBuffer->handle->lpVtbl->RSSetScissorRects(d3dCmdBuffer->handle, count, d3dScissors);
    if (count > 1) {
        palFree(s_D3D12.allocator, d3dScissors);
    }
    return PAL_RESULT_SUCCESS;
}

PalResult PAL_CALL cmdBindVertexBuffersD3D12(
    PalCommandBuffer* cmdBuffer,
    uint32_t firstSlot,
    uint32_t count,
    PalBuffer** buffers,
    uint64_t* offsets)
{
    CommandBuffer* d3dCmdBuffer = (CommandBuffer*)cmdBuffer;
    Pipeline* pipeline = d3dCmdBuffer->pipeline;
    D3D12_VERTEX_BUFFER_VIEW cachedView = {0};
    D3D12_VERTEX_BUFFER_VIEW* views = nullptr;

    if (!pipeline) {
        return PAL_RESULT_INVALID_OPERATION;
    }

    if (count > 1) {
        views = palAllocate(s_D3D12.allocator, sizeof(D3D12_VERTEX_BUFFER_VIEW) * count, 0);
        if (!views) {
            return PAL_RESULT_OUT_OF_MEMORY;
        }

    } else {
        views = &cachedView;
    }

    for (int i = 0; i < count; i++) {
        Buffer* tmp = (Buffer*)buffers[i];
        views[i].BufferLocation = tmp->handle->lpVtbl->GetGPUVirtualAddress(tmp->handle);
        views[i].BufferLocation = views[i].BufferLocation + offsets[i];
        views[i].SizeInBytes = (UINT)tmp->size;
        views[i].StrideInBytes = pipeline->strides[i];
    }

    d3dCmdBuffer->handle->lpVtbl->IASetVertexBuffers(
        d3dCmdBuffer->handle,
        firstSlot,
        count,
        views);

    if (count > 1) {
        palFree(s_D3D12.allocator, views);
    }
    return PAL_RESULT_SUCCESS;
}

PalResult PAL_CALL cmdBindIndexBufferD3D12(
    PalCommandBuffer* cmdBuffer,
    PalBuffer* buffer,
    uint64_t offset,
    PalIndexType type)
{
    CommandBuffer* d3dCmdBuffer = (CommandBuffer*)cmdBuffer;
    D3D12_INDEX_BUFFER_VIEW view = {0};
    Buffer* indexBuffer = (Buffer*)buffer;

    view.BufferLocation = indexBuffer->handle->lpVtbl->GetGPUVirtualAddress(indexBuffer->handle);
    view.BufferLocation = view.BufferLocation + offset;
    view.SizeInBytes = (UINT)indexBuffer->size;
    if (type == PAL_INDEX_TYPE_UINT16) {
        view.Format = DXGI_FORMAT_R16_UINT;
    } else {
        view.Format = DXGI_FORMAT_R32_UINT;
    }

    d3dCmdBuffer->handle->lpVtbl->IASetIndexBuffer(d3dCmdBuffer->handle, &view);
    return PAL_RESULT_SUCCESS;
}

PalResult PAL_CALL cmdDrawD3D12(
    PalCommandBuffer* cmdBuffer,
    uint32_t vertexCount,
    uint32_t instanceCount,
    uint32_t firstVertex,
    uint32_t firstInstance)
{
    CommandBuffer* d3dCmdBuffer = (CommandBuffer*)cmdBuffer;
    d3dCmdBuffer->handle->lpVtbl->DrawInstanced(
        d3dCmdBuffer->handle,
        vertexCount,
        instanceCount,
        firstVertex,
        firstInstance);

    return PAL_RESULT_SUCCESS;
}

PalResult PAL_CALL cmdDrawIndirectD3D12(
    PalCommandBuffer* cmdBuffer,
    PalBuffer* buffer,
    uint32_t count)
{
    CommandBuffer* d3dCmdBuffer = (CommandBuffer*)cmdBuffer;
    Device* device = d3dCmdBuffer->device;
    Buffer* d3dBuffer = (Buffer*)buffer;
    if (!(device->features & PAL_ADAPTER_FEATURE_INDIRECT_DRAW)) {
        return PAL_RESULT_ADAPTER_FEATURE_NOT_SUPPORTED;
    }

    if (!d3dBuffer->hasIndirect) {
        return PAL_RESULT_INVALID_BUFFER;
    }

    d3dCmdBuffer->handle->lpVtbl->ExecuteIndirect(
        d3dCmdBuffer->handle,
        device->drawSignature,
        count,
        d3dBuffer->handle,
        0,
        nullptr,
        0);

    return PAL_RESULT_SUCCESS;
}

PalResult PAL_CALL cmdDrawIndirectCountD3D12(
    PalCommandBuffer* cmdBuffer,
    PalBuffer* buffer,
    PalBuffer* countBuffer,
    uint32_t maxDrawCount)
{
    CommandBuffer* d3dCmdBuffer = (CommandBuffer*)cmdBuffer;
    Device* device = d3dCmdBuffer->device;
    if (!(device->features & PAL_ADAPTER_FEATURE_INDIRECT_DRAW_COUNT)) {
        return PAL_RESULT_ADAPTER_FEATURE_NOT_SUPPORTED;
    }

    Buffer* d3dBuffer = (Buffer*)buffer;
    Buffer* d3dCountBuffer = (Buffer*)countBuffer;
    if (!d3dBuffer->hasIndirect || !d3dCountBuffer->hasIndirect) {
        return PAL_RESULT_INVALID_BUFFER;
    }

    d3dCmdBuffer->handle->lpVtbl->ExecuteIndirect(
        d3dCmdBuffer->handle,
        device->drawSignature,
        maxDrawCount,
        d3dBuffer->handle,
        0,
        d3dCountBuffer->handle,
        0);

    return PAL_RESULT_SUCCESS;
}

PalResult PAL_CALL cmdDrawIndexedD3D12(
    PalCommandBuffer* cmdBuffer,
    uint32_t indexCount,
    uint32_t instanceCount,
    uint32_t firstIndex,
    int32_t vertexOffset,
    uint32_t firstInstance)
{
    CommandBuffer* d3dCmdBuffer = (CommandBuffer*)cmdBuffer;
    d3dCmdBuffer->handle->lpVtbl->DrawIndexedInstanced(
        d3dCmdBuffer->handle,
        indexCount,
        instanceCount,
        firstIndex,
        vertexOffset,
        firstInstance);

    return PAL_RESULT_SUCCESS;
}

PalResult PAL_CALL cmdDrawIndexedIndirectD3D12(
    PalCommandBuffer* cmdBuffer,
    PalBuffer* buffer,
    uint32_t count)
{
    CommandBuffer* d3dCmdBuffer = (CommandBuffer*)cmdBuffer;
    Device* device = d3dCmdBuffer->device;
    Buffer* d3dBuffer = (Buffer*)buffer;
    if (!(device->features & PAL_ADAPTER_FEATURE_INDIRECT_DRAW)) {
        return PAL_RESULT_ADAPTER_FEATURE_NOT_SUPPORTED;
    }

    if (!d3dBuffer->hasIndirect) {
        return PAL_RESULT_INVALID_BUFFER;
    }

    d3dCmdBuffer->handle->lpVtbl->ExecuteIndirect(
        d3dCmdBuffer->handle,
        device->drawIndexedSignature,
        count,
        d3dBuffer->handle,
        0,
        nullptr,
        0);

    return PAL_RESULT_SUCCESS;
}

PalResult PAL_CALL cmdDrawIndexedIndirectCountD3D12(
    PalCommandBuffer* cmdBuffer,
    PalBuffer* buffer,
    PalBuffer* countBuffer,
    uint32_t maxDrawCount)
{
    CommandBuffer* d3dCmdBuffer = (CommandBuffer*)cmdBuffer;
    Device* device = d3dCmdBuffer->device;
    if (!(device->features & PAL_ADAPTER_FEATURE_INDIRECT_DRAW_COUNT)) {
        return PAL_RESULT_ADAPTER_FEATURE_NOT_SUPPORTED;
    }

    Buffer* d3dBuffer = (Buffer*)buffer;
    Buffer* d3dCountBuffer = (Buffer*)countBuffer;
    if (!d3dBuffer->hasIndirect || !d3dCountBuffer->hasIndirect) {
        return PAL_RESULT_INVALID_BUFFER;
    }

    d3dCmdBuffer->handle->lpVtbl->ExecuteIndirect(
        d3dCmdBuffer->handle,
        device->drawIndexedSignature,
        maxDrawCount,
        d3dBuffer->handle,
        0,
        d3dCountBuffer->handle,
        0);

    return PAL_RESULT_SUCCESS;
}

PalResult PAL_CALL cmdAccelerationStructureBarrierD3D12(
    PalCommandBuffer* cmdBuffer,
    PalAccelerationStructure* as,
    PalUsageStateInfo* oldUsageStateInfo,
    PalUsageStateInfo* newUsageStateInfo)
{
    CommandBuffer* d3dCmdBuffer = (CommandBuffer*)cmdBuffer;
    if (!(d3dCmdBuffer->device->features & PAL_ADAPTER_FEATURE_RAY_TRACING)) {
        return PAL_RESULT_ADAPTER_FEATURE_NOT_SUPPORTED;
    }

    AccelerationStructure* d3dAs = (AccelerationStructure*)as;
    D3D12_RESOURCE_BARRIER barrier = {0};
    barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_UAV;
    barrier.UAV.pResource = d3dAs->handle;

    d3dCmdBuffer->handle->lpVtbl->ResourceBarrier(d3dCmdBuffer->handle, 1, &barrier);
    return PAL_RESULT_SUCCESS;
}

PalResult PAL_CALL cmdImageBarrierD3D12(
    PalCommandBuffer* cmdBuffer,
    PalImage* image,
    PalImageSubresourceRange* subresourceRange,
    PalUsageStateInfo* oldUsageStateInfo,
    PalUsageStateInfo* newUsageStateInfo)
{
    CommandBuffer* d3dCmdBuffer = (CommandBuffer*)cmdBuffer;
    Image* d3dImage = (Image*)image;
    D3D12_RESOURCE_STATES old, new;
    D3D12_RESOURCE_BARRIER barrier = {0};

    old = barrierToD3D12(
        oldUsageStateInfo->shaderStageCount,
        oldUsageStateInfo->usageState,
        oldUsageStateInfo->shaderStages);

    new = barrierToD3D12(
        newUsageStateInfo->shaderStageCount,
        newUsageStateInfo->usageState,
        newUsageStateInfo->shaderStages);

    // read/write barrier without transition
    if (old == new && old == D3D12_RESOURCE_STATE_UNORDERED_ACCESS) {
        barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_UAV;
        barrier.UAV.pResource = d3dImage->handle;
        d3dCmdBuffer->handle->lpVtbl->ResourceBarrier(d3dCmdBuffer->handle, 1, &barrier);
        return PAL_RESULT_SUCCESS;
    }

    uint32_t planeCount = 1; // for color or depth
    if (subresourceRange->aspect == PAL_IMAGE_ASPECT_DEPTH_STENCIL) {
        planeCount = 2;
    }

    D3D12_RESOURCE_BARRIER* barriers = nullptr;
    uint32_t levelCount = subresourceRange->mipLevelCount;
    uint32_t layerCount = subresourceRange->layerArrayCount;

    uint32_t startLevel = subresourceRange->startMipLevel;
    uint32_t startLayer = subresourceRange->startArrayLayer;
    uint32_t maxLevels = d3dImage->info.mipLevelCount;
    uint32_t maxLayers = d3dImage->info.depthOrArraySize;
    uint32_t barrierCount = layerCount * levelCount * planeCount;

    if (startLevel == 0 && levelCount == maxLevels && startLayer == 0 && layerCount == maxLayers) {
        // full resource
        barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
        barrier.Transition.pResource = d3dImage->handle;
        barrier.Transition.StateBefore = old;
        barrier.Transition.StateAfter = new;
        barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

        d3dCmdBuffer->handle->lpVtbl->ResourceBarrier(d3dCmdBuffer->handle, 1, &barrier);
        return PAL_RESULT_SUCCESS;
    }

    if (layerCount == 1 && layerCount == 1 && planeCount == 1) {
        // single plane
        barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
        barrier.Transition.pResource = d3dImage->handle;
        barrier.Transition.StateBefore = old;
        barrier.Transition.StateAfter = new;
        barrier.Transition.Subresource = startLevel + startLayer * maxLevels;

        d3dCmdBuffer->handle->lpVtbl->ResourceBarrier(d3dCmdBuffer->handle, 1, &barrier);
        return PAL_RESULT_SUCCESS;
    }

    barriers = palAllocate(s_D3D12.allocator, sizeof(D3D12_RESOURCE_BARRIER) * barrierCount, 0);
    if (!barriers) {
        return PAL_RESULT_OUT_OF_MEMORY;
    }

    uint32_t count = 0;
    for (uint32_t plane = 0; plane < planeCount; plane++) {
        for (uint32_t layer = startLayer; layer < startLayer + layerCount; layer++) {
            for (uint32_t level = startLevel; level < startLevel + levelCount; level++) {
                uint32_t index = level + (layer * maxLevels) + (plane * maxLevels * maxLayers);

                D3D12_RESOURCE_BARRIER* tmp = &barriers[count++];
                tmp->Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
                tmp->Transition.pResource = d3dImage->handle;
                tmp->Transition.StateBefore = old;
                tmp->Transition.StateAfter = new;
                tmp->Transition.Subresource = index;
            }
        }
    }

    d3dCmdBuffer->handle->lpVtbl->ResourceBarrier(d3dCmdBuffer->handle, barrierCount, barriers);
    palFree(s_D3D12.allocator, barriers);
    return PAL_RESULT_SUCCESS;
}

PalResult PAL_CALL cmdBufferBarrierD3D12(
    PalCommandBuffer* cmdBuffer,
    PalBuffer* buffer,
    PalUsageStateInfo* oldUsageStateInfo,
    PalUsageStateInfo* newUsageStateInfo)
{
    CommandBuffer* d3dCmdBuffer = (CommandBuffer*)cmdBuffer;
    Buffer* d3dBuffer = (Buffer*)buffer;
    D3D12_RESOURCE_STATES old, new;
    if (!d3dBuffer->canChangeState) {
        return PAL_RESULT_SUCCESS;
    }

    old = barrierToD3D12(
        oldUsageStateInfo->shaderStageCount,
        oldUsageStateInfo->usageState,
        oldUsageStateInfo->shaderStages);

    new = barrierToD3D12(
        newUsageStateInfo->shaderStageCount,
        newUsageStateInfo->usageState,
        newUsageStateInfo->shaderStages);

    D3D12_RESOURCE_BARRIER barrier = {0};
    if (old == new && D3D12_RESOURCE_STATE_UNORDERED_ACCESS) {
        barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_UAV;
        barrier.UAV.pResource = d3dBuffer->handle;
    } else {
        barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
        barrier.Transition.pResource = d3dBuffer->handle;
        barrier.Transition.StateBefore = old;
        barrier.Transition.StateAfter = new;
    }

    d3dCmdBuffer->handle->lpVtbl->ResourceBarrier(d3dCmdBuffer->handle, 1, &barrier);
    return PAL_RESULT_SUCCESS;
}

PalResult PAL_CALL cmdDispatchD3D12(
    PalCommandBuffer* cmdBuffer,
    uint32_t groupCountX,
    uint32_t groupCountY,
    uint32_t groupCountZ)
{
    CommandBuffer* d3dCmdBuffer = (CommandBuffer*)cmdBuffer;
    d3dCmdBuffer->handle->lpVtbl->Dispatch(
        d3dCmdBuffer->handle,
        groupCountX,
        groupCountY,
        groupCountZ);

    return PAL_RESULT_SUCCESS;
}

PalResult PAL_CALL cmdDispatchBaseD3D12(
    PalCommandBuffer* cmdBuffer,
    uint32_t baseGroupX,
    uint32_t baseGroupY,
    uint32_t baseGroupZ,
    uint32_t groupCountX,
    uint32_t groupCountY,
    uint32_t groupCountZ)
{
    return PAL_RESULT_ADAPTER_FEATURE_NOT_SUPPORTED;
}

PalResult PAL_CALL cmdDispatchIndirectD3D12(
    PalCommandBuffer* cmdBuffer,
    PalBuffer* buffer)
{
    CommandBuffer* d3dCmdBuffer = (CommandBuffer*)cmdBuffer;
    Device* device = d3dCmdBuffer->device;
    Buffer* d3dBuffer = (Buffer*)buffer;
    if (!(device->features & PAL_ADAPTER_FEATURE_INDIRECT_DISPATCH)) {
        return PAL_RESULT_ADAPTER_FEATURE_NOT_SUPPORTED;
    }

    if (!d3dBuffer->hasIndirect) {
        return PAL_RESULT_INVALID_BUFFER;
    }

    d3dCmdBuffer->handle->lpVtbl->ExecuteIndirect(
        d3dCmdBuffer->handle,
        device->dispatchSignature,
        1, // one dispatch
        d3dBuffer->handle,
        0,
        nullptr,
        0);

    return PAL_RESULT_SUCCESS;
}

PalResult PAL_CALL cmdTraceRaysD3D12(
    PalCommandBuffer* cmdBuffer,
    PalShaderBindingTable* sbt,
    uint32_t raygenIndex,
    uint32_t width,
    uint32_t height,
    uint32_t depth)
{
    CommandBuffer* d3dCmdBuffer = (CommandBuffer*)cmdBuffer;
    ShaderBindingTable* d3dSbt = (ShaderBindingTable*)sbt;
    if (!(d3dCmdBuffer->device->features & PAL_ADAPTER_FEATURE_RAY_TRACING)) {
        return PAL_RESULT_ADAPTER_FEATURE_NOT_SUPPORTED;
    }

    uint64_t stride = d3dSbt->raygen.region.StrideInBytes;
    D3D12_GPU_VIRTUAL_ADDRESS_RANGE raygenAddress = {0};
    raygenAddress.SizeInBytes = d3dSbt->raygen.region.SizeInBytes;
    raygenAddress.StartAddress = d3dSbt->baseAddress + raygenIndex * stride;

    // we need to make sure the SBT is up to date
    commitShaderbindingTableUpdateD3D12(d3dCmdBuffer, d3dSbt);

    D3D12_DISPATCH_RAYS_DESC desc = {0};
    desc.Width = width;
    desc.Height = height;
    desc.Depth = depth;
    desc.RayGenerationShaderRecord = raygenAddress;
    desc.HitGroupTable = d3dSbt->hit.region;
    desc.MissShaderTable = d3dSbt->miss.region;
    desc.CallableShaderTable = d3dSbt->callable.region;

    d3dCmdBuffer->handle->lpVtbl->DispatchRays(d3dCmdBuffer->handle, &desc);
    return PAL_RESULT_SUCCESS;
}

PalResult PAL_CALL cmdTraceRaysIndirectD3D12(
    PalCommandBuffer* cmdBuffer,
    uint32_t raygenIndex,
    PalShaderBindingTable* sbt,
    PalBuffer* buffer)
{
    HRESULT result;
    CommandBuffer* d3dCmdBuffer = (CommandBuffer*)cmdBuffer;
    ShaderBindingTable* d3dSbt = (ShaderBindingTable*)sbt;
    Buffer* d3dBuffer = (Buffer*)buffer;
    Device* device = (Device*)d3dCmdBuffer->device;
    D3D12_DISPATCH_RAYS_DESC desc = {0};

    if (!(device->features & PAL_ADAPTER_FEATURE_INDIRECT_RAY_TRACING)) {
        return PAL_RESULT_ADAPTER_FEATURE_NOT_SUPPORTED;
    }

    if (!d3dBuffer->hasIndirect) {
        return PAL_RESULT_INVALID_BUFFER;
    }

    // we need to make sure the SBT is up to date
    commitShaderbindingTableUpdateD3D12(d3dCmdBuffer, d3dSbt);

    // Buffer is mappable
    void* ptr = nullptr;
    result = d3dBuffer->handle->lpVtbl->Map(d3dBuffer->handle, 0, nullptr, &ptr);
    if (FAILED(result)) {
        return PAL_RESULT_MEMORY_MAP_FAILED;
    }

    // copy indirect parameters from the buffer
    PalDispatchIndirectData data = {0};
    memcpy(&data, ptr, sizeof(PalDispatchIndirectData));
    d3dBuffer->handle->lpVtbl->Unmap(d3dBuffer->handle, 0, nullptr);

    desc.Width = data.groupCountXOrWidth;
    desc.Height = data.groupCountXOrHeight;
    desc.Depth = data.groupCountXOrDepth;

    uint64_t stride = d3dSbt->raygen.region.StrideInBytes;
    D3D12_GPU_VIRTUAL_ADDRESS_RANGE raygenAddress = {0};
    raygenAddress.SizeInBytes = d3dSbt->raygen.region.SizeInBytes;
    raygenAddress.StartAddress = d3dSbt->baseAddress + raygenIndex * stride;

    desc.RayGenerationShaderRecord = raygenAddress;
    desc.HitGroupTable = d3dSbt->hit.region;
    desc.MissShaderTable = d3dSbt->miss.region;
    desc.CallableShaderTable = d3dSbt->callable.region;

    // fill the data into the tmp upload buffer of the command buffer
    ptr = nullptr;
    d3dCmdBuffer->stagingBuffer->lpVtbl->Map(d3dCmdBuffer->stagingBuffer, 0, nullptr, &ptr);
    memcpy(ptr, &desc, sizeof(D3D12_DISPATCH_RAYS_DESC));
    d3dCmdBuffer->stagingBuffer->lpVtbl->Unmap(d3dCmdBuffer->stagingBuffer, 0, nullptr);

    // copy to the gpu tmp buffer of the command buffer and execute with that
    d3dCmdBuffer->handle->lpVtbl->CopyBufferRegion(
        d3dCmdBuffer->handle,
        d3dCmdBuffer->buffer,
        0,
        d3dCmdBuffer->stagingBuffer,
        0,
        sizeof(D3D12_DISPATCH_RAYS_DESC));

    // put a memory barrier
    D3D12_RESOURCE_BARRIER barrier = {0};
    barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
    barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE;
    barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
    barrier.Transition.pResource = d3dCmdBuffer->buffer;
    d3dCmdBuffer->handle->lpVtbl->ResourceBarrier(d3dCmdBuffer->handle, 1, &barrier);

    d3dCmdBuffer->handle->lpVtbl->ExecuteIndirect(
        d3dCmdBuffer->handle,
        device->raySignature,
        1,
        d3dCmdBuffer->buffer,
        0,
        nullptr,
        0);

    return PAL_RESULT_SUCCESS;
}

PalResult PAL_CALL cmdBindDescriptorSetD3D12(
    PalCommandBuffer* cmdBuffer,
    uint32_t setIndex,
    PalDescriptorSet* set)
{
    CommandBuffer* d3dCmdBuffer = (CommandBuffer*)cmdBuffer;
    Pipeline* pipeline = d3dCmdBuffer->pipeline;
    DescriptorSet* d3dSet = (DescriptorSet*)set;
    DescriptorPool* pool = d3dSet->pool;

    // bind heaps
    uint32_t heapCount = 0;
    ID3D12DescriptorHeap* heaps[2];
    if (pool->hasResourceHeap) {
        heaps[heapCount++] = pool->resourceHeap.handle;
    }

    if (pool->hasSamplerHeap) {
        heaps[heapCount++] = pool->samplerHeap.handle;
    }
    d3dCmdBuffer->handle->lpVtbl->SetDescriptorHeaps(d3dCmdBuffer->handle, heapCount, heaps);

    // bind resource descriptor table
    uint32_t resourceCount = d3dSet->layout->bindingCount - d3dSet->layout->samplerCount;
    uint32_t baseIndex = setIndex;
    if (pipeline->layout->constantIndex != UINT32_MAX) {
        // If push constant was used to create the pipeline layout
        // slot 0 will be reserve for it
        baseIndex++;
    }

    if (resourceCount) {
        D3D12_GPU_DESCRIPTOR_HANDLE base;
        base.ptr = getDescriptorHandleD3D12(
            d3dSet->resourceOffset,
            pool->resourceHeap.incrementSize,
            pool->resourceHeap.gpuBase);

        if (pipeline->type == GRAPHICS_PIPELINE) {
            d3dCmdBuffer->handle->lpVtbl->SetGraphicsRootDescriptorTable(
                d3dCmdBuffer->handle,
                baseIndex, // base set index is resource first before sampler
                base);

        } else {
            // ray tracing uses the compute path
            d3dCmdBuffer->handle->lpVtbl->SetComputeRootDescriptorTable(
                d3dCmdBuffer->handle,
                baseIndex, // base set index is resource first before sampler
                base);
        }

        baseIndex++;
    }

    // bind sampler descriptor table
    if (d3dSet->layout->samplerCount) {
        D3D12_GPU_DESCRIPTOR_HANDLE base;
        base.ptr = getDescriptorHandleD3D12(
            d3dSet->samplerOffset,
            pool->samplerHeap.incrementSize,
            pool->samplerHeap.gpuBase);

        if (pipeline->type == GRAPHICS_PIPELINE) {
            d3dCmdBuffer->handle->lpVtbl->SetGraphicsRootDescriptorTable(
                d3dCmdBuffer->handle,
                baseIndex,
                base);

        } else {
            // ray tracing uses the compute path
            d3dCmdBuffer->handle->lpVtbl->SetComputeRootDescriptorTable(
                d3dCmdBuffer->handle,
                baseIndex,
                base);
        }
    }

    return PAL_RESULT_SUCCESS;
}

PalResult PAL_CALL cmdPushConstantsD3D12(
    PalCommandBuffer* cmdBuffer,
    uint32_t shaderStageCount,
    PalShaderStage* shaderStages,
    uint32_t offset,
    uint32_t size,
    const void* value)
{
    CommandBuffer* d3dCmdBuffer = (CommandBuffer*)cmdBuffer;
    Pipeline* pipeline = d3dCmdBuffer->pipeline;

    if (pipeline->layout->constantIndex != UINT32_MAX) {
        if (pipeline->type == GRAPHICS_PIPELINE) {
            d3dCmdBuffer->handle->lpVtbl->SetGraphicsRoot32BitConstants(
                d3dCmdBuffer->handle,
                pipeline->layout->constantIndex,
                size / 4,
                value,
                offset / 4);

        } else {
            // ray tracing uses the compute path
            d3dCmdBuffer->handle->lpVtbl->SetComputeRoot32BitConstants(
                d3dCmdBuffer->handle,
                pipeline->layout->constantIndex,
                size / 4,
                value,
                offset / 4);
        }
    }

    return PAL_RESULT_SUCCESS;
}

PalResult PAL_CALL cmdSetCullModeD3D12(
    PalCommandBuffer* cmdBuffer,
    PalCullMode cullMode)
{
    return PAL_RESULT_ADAPTER_FEATURE_NOT_SUPPORTED;
}

PalResult PAL_CALL cmdSetFrontFaceD3D12(
    PalCommandBuffer* cmdBuffer,
    PalFrontFace frontFace)
{
    return PAL_RESULT_ADAPTER_FEATURE_NOT_SUPPORTED;
}

PalResult PAL_CALL cmdSetPrimitiveTopologyD3D12(
    PalCommandBuffer* cmdBuffer,
    PalPrimitiveTopology topology)
{
    return PAL_RESULT_ADAPTER_FEATURE_NOT_SUPPORTED;
}

PalResult PAL_CALL cmdSetDepthTestEnableD3D12(
    PalCommandBuffer* cmdBuffer,
    PalBool enable)
{
    return PAL_RESULT_ADAPTER_FEATURE_NOT_SUPPORTED;
}

PalResult PAL_CALL cmdSetDepthWriteEnableD3D12(
    PalCommandBuffer* cmdBuffer,
    PalBool enable)
{
    return PAL_RESULT_ADAPTER_FEATURE_NOT_SUPPORTED;
}

PalResult PAL_CALL cmdSetStencilOpD3D12(
    PalCommandBuffer* cmdBuffer,
    PalStencilFaceFlags faceMask,
    PalStencilOp failOp,
    PalStencilOp passOp,
    PalStencilOp depthFailOp,
    PalCompareOp compareOp)
{
    return PAL_RESULT_ADAPTER_FEATURE_NOT_SUPPORTED;
}

// ==================================================
// Acceleration Structure
// ==================================================

// ==================================================
// Buffer
// ==================================================

PalResult PAL_CALL createBufferD3D12(
    PalDevice* device,
    const PalBufferCreateInfo* info,
    PalBuffer** outBuffer)
{
    HRESULT result;
    Buffer* buffer = nullptr;
    Device* d3d12Device = (Device*)device;

    if (info->usages & PAL_BUFFER_USAGE_ACCELERATION_STRUCTURE) {
        if (!(d3d12Device->features & PAL_ADAPTER_FEATURE_RAY_TRACING)) {
            return PAL_RESULT_ADAPTER_FEATURE_NOT_SUPPORTED;
        }

    } else if (info->usages & PAL_BUFFER_USAGE_DEVICE_ADDRESS) {
        if (!(d3d12Device->features & PAL_ADAPTER_FEATURE_BUFFER_DEVICE_ADDRESS)) {
            return PAL_RESULT_ADAPTER_FEATURE_NOT_SUPPORTED;
        }
    }

    buffer = palAllocate(s_D3D12.allocator, sizeof(Buffer), 0);
    if (!buffer) {
        return PAL_RESULT_OUT_OF_MEMORY;
    }

    memset(buffer, 0, sizeof(Buffer));
    buffer->desc.Width = info->size;
    buffer->desc.Height = 1;
    buffer->desc.DepthOrArraySize = 1;
    buffer->desc.MipLevels = 1;
    buffer->desc.SampleDesc.Count = 1;
    buffer->desc.SampleDesc.Quality = 0;
    buffer->desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
    buffer->desc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

    if (info->usages & PAL_BUFFER_USAGE_STORAGE) {
        buffer->desc.Flags |= D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
    }

    if (info->usages & PAL_BUFFER_USAGE_ACCELERATION_STRUCTURE) {
        buffer->desc.Flags |= D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
        buffer->isAccelerationStructure = PAL_TRUE;
    }

    if (info->usages & PAL_BUFFER_USAGE_ACCELERATION_STRUCTURE_SCRATCH) {
        buffer->desc.Flags |= D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
        buffer->isScratch = PAL_TRUE;
    }

    if (info->usages & PAL_BUFFER_USAGE_DEVICE_ADDRESS) {
        buffer->supportsAddress = PAL_TRUE;
    }

    if (info->usages & PAL_BUFFER_USAGE_INDIRECT) {
        buffer->hasIndirect = PAL_TRUE;
    }

    buffer->device = d3d12Device;
    buffer->size = info->size;
    *outBuffer = (PalBuffer*)buffer;
    return PAL_RESULT_SUCCESS;
}

void PAL_CALL destroyBufferD3D12(PalBuffer* buffer)
{
    Buffer* d3dBuffer = (Buffer*)buffer;
    // check if memory has been attached to the buffer
    if (d3dBuffer->handle) {
        d3dBuffer->handle->lpVtbl->Release(d3dBuffer->handle);
    }
    palFree(s_D3D12.allocator, d3dBuffer);
}

PalResult PAL_CALL getBufferMemoryRequirementsD3D12(
    PalBuffer* buffer,
    PalMemoryRequirements* requirements)
{
    Buffer* d3dBuffer = (Buffer*)buffer;
    ID3D12Device5* device = d3dBuffer->device->handle;

    D3D12_RESOURCE_ALLOCATION_INFO allocationInfo = {0};
    D3D12_RESOURCE_ALLOCATION_INFO __ret = {0};
    allocationInfo = *device->lpVtbl->GetResourceAllocationInfo(
        device,
        &__ret,
        0,
        1,
        &d3dBuffer->desc);

    // d3d12 allows buffers to be used with all memory heap types
    requirements->memoryTypes[PAL_MEMORY_TYPE_GPU_ONLY] = PAL_TRUE;
    requirements->memoryTypes[PAL_MEMORY_TYPE_CPU_UPLOAD] = PAL_TRUE;
    requirements->memoryTypes[PAL_MEMORY_TYPE_CPU_READBACK] = PAL_TRUE;

    requirements->alignment = allocationInfo.Alignment;
    requirements->size = allocationInfo.SizeInBytes;
    return PAL_RESULT_SUCCESS;
}

PalResult PAL_CALL computeInstanceBufferRequirementsD3D12(
    PalDevice* device,
    uint32_t instanceCount,
    uint64_t* outSize)
{
    *outSize = sizeof(D3D12_RAYTRACING_INSTANCE_DESC) * instanceCount;
    return PAL_RESULT_SUCCESS;
}

PalResult PAL_CALL computeImageCopyStagingBufferRequirementsD3D12(
    PalDevice* device,
    PalFormat imageFormat,
    PalBufferImageCopyInfo* copyInfo,
    uint32_t* outBufferRowLength,
    uint32_t* outBufferImageHeight,
    uint64_t* outSize)
{
    uint32_t imageFormatSize = getFormatSizeD3D12(imageFormat);
    uint32_t rowPitch = alignD3D12((uint64_t)copyInfo->imageWidth * imageFormatSize, TEXTURE_PITCH);
    uint32_t bufferImageHeight = 0;

    if (copyInfo->bufferImageHeight) {
        bufferImageHeight = copyInfo->bufferImageHeight;
    } else {
        bufferImageHeight = copyInfo->imageHeight;
    }

    *outBufferRowLength = rowPitch;
    *outBufferImageHeight = bufferImageHeight;
    *outSize = (uint64_t)rowPitch * bufferImageHeight * copyInfo->imageDepth;
    return PAL_RESULT_SUCCESS;
}

PalResult PAL_CALL writeToInstanceBufferD3D12(
    PalDevice* device,
    void* ptr,
    PalAccelerationStructureInstance* instances,
    uint32_t instanceCount)
{
    D3D12_RAYTRACING_INSTANCE_DESC* data = ptr;
    for (int i = 0; i < instanceCount; i++) {
        PalAccelerationStructureInstance* src = &instances[i];
        D3D12_RAYTRACING_INSTANCE_DESC* dst = &data[i];
        AccelerationStructure* as = (AccelerationStructure*)src->blas;

        dst->InstanceMask = src->mask;
        dst->InstanceID = src->instanceId;
        dst->AccelerationStructure = as->address;
        dst->InstanceContributionToHitGroupIndex = src->hitGroupOffset;
        dst->Flags = instanceFlagsToD3D12(src->flags);
        memcpy(dst->Transform, src->transform, sizeof(float) * 12);
    }
    return PAL_RESULT_SUCCESS;
}

PalResult PAL_CALL writeToImageCopyStagingBufferD3D12(
    PalDevice* device,
    void* ptr,
    void* srcData,
    PalFormat imageFormat,
    PalBufferImageCopyInfo* copyInfo)
{
    uint32_t imageFormatSize = getFormatSizeD3D12(imageFormat);
    uint32_t srcRowPitch = copyInfo->imageWidth * imageFormatSize;
    const uint32_t dstSlicePitch = copyInfo->bufferRowLength * copyInfo->bufferImageHeight;
    const uint32_t srcSlicePitch = srcRowPitch * copyInfo->imageHeight;

    // manually offset the buffer with the provided offset
    uint8_t* dst = (uint8_t*)ptr + copyInfo->bufferOffset;
    const uint8_t* src = (const uint8_t*)srcData;

    // write to destination pointer
    for (uint32_t z = 0; z < copyInfo->imageDepth; z++) {
        for (uint32_t y = 0; y < copyInfo->imageHeight; y++) {
            memcpy(
                dst + z * dstSlicePitch + y * copyInfo->bufferRowLength,
                src + z * srcSlicePitch + y * srcRowPitch,
                srcRowPitch);
        }
    }

    return PAL_RESULT_SUCCESS;
}

PalResult PAL_CALL bindBufferMemoryD3D12(
    PalBuffer* buffer,
    PalMemory* memory,
    uint64_t offset)
{
    HRESULT result;
    Buffer* d3dBuffer = (Buffer*)buffer;
    ID3D12Device5* device = d3dBuffer->device->handle;

    D3D12_RESOURCE_STATES state = 0;
    ID3D12Heap* mem = (ID3D12Heap*)memory;
    D3D12_HEAP_DESC __ret = {0};
    D3D12_HEAP_DESC heapProps = {0};
    heapProps = *mem->lpVtbl->GetDesc(mem, &__ret);

    d3dBuffer->canChangeState = PAL_TRUE;
    if (heapProps.Properties.Type == D3D12_HEAP_TYPE_UPLOAD) {
        state = D3D12_RESOURCE_STATE_GENERIC_READ;
        d3dBuffer->canChangeState = PAL_FALSE;

    } else if (heapProps.Properties.Type == D3D12_HEAP_TYPE_READBACK) {
        state = D3D12_RESOURCE_STATE_COPY_DEST;
        d3dBuffer->canChangeState = PAL_FALSE;
    }

    if (d3dBuffer->isAccelerationStructure) {
        d3dBuffer->canChangeState = PAL_FALSE;
        state = D3D12_RESOURCE_STATE_RAYTRACING_ACCELERATION_STRUCTURE;

    } else if (d3dBuffer->isScratch) {
        state = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
    }

    result = device->lpVtbl->CreatePlacedResource(
        device,
        mem,
        offset,
        &d3dBuffer->desc,
        state,
        nullptr,
        &IID_Resource,
        (void**)&d3dBuffer->handle);

    if (FAILED(result)) {
        pollMessagesD3D12(d3dBuffer->device);
        if (result == E_OUTOFMEMORY) {
            return PAL_RESULT_OUT_OF_MEMORY;
        } else if (result == E_INVALIDARG) {
            return PAL_RESULT_INVALID_ARGUMENT;
        }
        return PAL_RESULT_PLATFORM_FAILURE;
    }

    return PAL_RESULT_SUCCESS;
}

PalResult PAL_CALL mapBufferD3D12(
    PalBuffer* buffer,
    uint64_t offset,
    uint64_t size,
    void** outPtr)
{
    void* ptr = nullptr;
    Buffer* d3dBuffer = (Buffer*)buffer;
    HRESULT result = d3dBuffer->handle->lpVtbl->Map(d3dBuffer->handle, 0, nullptr, &ptr);
    if (FAILED(result)) {
        pollMessagesD3D12(d3dBuffer->device);
        return PAL_RESULT_MEMORY_MAP_FAILED;
    }

    *outPtr = (uint8_t*)ptr + offset;
    return PAL_RESULT_SUCCESS;
}

void PAL_CALL unmapBufferD3D12(PalBuffer* buffer)
{
    Buffer* d3dBuffer = (Buffer*)buffer;
    d3dBuffer->handle->lpVtbl->Unmap(d3dBuffer->handle, 0, nullptr);
}

PalDeviceAddress PAL_CALL getBufferDeviceAddressD3D12(PalBuffer* buffer)
{
    Buffer* d3dBuffer = (Buffer*)buffer;
    if (!d3dBuffer->supportsAddress) {
        return 0;
    }

    return d3dBuffer->handle->lpVtbl->GetGPUVirtualAddress(d3dBuffer->handle);
}

// ==================================================
// Descriptor Pool, Set and Layout
// ==================================================

PalResult PAL_CALL createDescriptorSetLayoutD3D12(
    PalDevice* device,
    const PalDescriptorSetLayoutCreateInfo* info,
    PalDescriptorSetLayout** outLayout)
{
    HRESULT result;
    Device* d3d12Device = (Device*)device;
    DescriptorSetLayout* layout = nullptr;
    DescriptorSetBinding* bindings = nullptr;
    uint32_t count = info->bindingCount;

    PalBool hasDescriptorIndexing = d3d12Device->features & PAL_ADAPTER_FEATURE_DESCRIPTOR_INDEXING;
    if (info->enableDescriptorIndexing && !hasDescriptorIndexing) {
        return PAL_RESULT_ADAPTER_FEATURE_NOT_SUPPORTED;
    }

    layout = palAllocate(s_D3D12.allocator, sizeof(DescriptorSetLayout), 0);
    bindings = palAllocate(s_D3D12.allocator, sizeof(DescriptorSetBinding) * count, 0);
    if (!layout || !bindings) {
        return PAL_RESULT_OUT_OF_MEMORY;
    }

    layout->visibility = 0;
    if (info->shaderStageCount > 1) {
        layout->visibility = D3D12_SHADER_VISIBILITY_ALL;

    } else {
        switch (info->shaderStages[0]) {
            case PAL_SHADER_STAGE_VERTEX: {
                layout->visibility = D3D12_SHADER_VISIBILITY_VERTEX;
                break;
            }

            case PAL_SHADER_STAGE_FRAGMENT: {
                layout->visibility = D3D12_SHADER_VISIBILITY_PIXEL;
                break;
            }

            case PAL_SHADER_STAGE_GEOMETRY: {
                layout->visibility = D3D12_SHADER_VISIBILITY_GEOMETRY;
                break;
            }

            case PAL_SHADER_STAGE_MESH: {
                layout->visibility = D3D12_SHADER_VISIBILITY_MESH;
                break;
            }

            case PAL_SHADER_STAGE_TASK: {
                layout->visibility = D3D12_SHADER_VISIBILITY_AMPLIFICATION;
                break;
            }

            case PAL_SHADER_STAGE_TESSELLATION_CONTROL: {
                layout->visibility = D3D12_SHADER_VISIBILITY_HULL;
                break;
            }

            case PAL_SHADER_STAGE_TESSELLATION_EVALUATION: {
                layout->visibility = D3D12_SHADER_VISIBILITY_DOMAIN;
                break;
            }

            case PAL_SHADER_STAGE_COMPUTE:
            case PAL_SHADER_STAGE_RAYGEN:
            case PAL_SHADER_STAGE_CLOSEST_HIT:
            case PAL_SHADER_STAGE_ANY_HIT:
            case PAL_SHADER_STAGE_MISS:
            case PAL_SHADER_STAGE_INTERSECTION:
            case PAL_SHADER_STAGE_CALLABLE: {
                layout->visibility = D3D12_SHADER_VISIBILITY_ALL;
                break;
            }

        }
    }

    D3D12_DESCRIPTOR_RANGE_FLAGS rangeFlags = D3D12_DESCRIPTOR_RANGE_FLAG_NONE;
    if (info->enableDescriptorIndexing) {
        rangeFlags = D3D12_DESCRIPTOR_RANGE_FLAG_DESCRIPTORS_VOLATILE;
    }

    uint32_t resourceOffset = 0;
    uint32_t samplerOffset = 0;
    uint32_t SRVRegister = 0;
    uint32_t UAVRegister = 0;
    uint32_t CBVRegister = 0;

    uint32_t sampledImageCount = 0;
    uint32_t storageImageCount = 0;
    uint32_t storageBufferCount = 0;
    uint32_t uniformBufferCount = 0;
    uint32_t tlasCount = 0;
    uint32_t samplerCount = 0;

    for (int i = 0; i < count; i++) {
        DescriptorSetBinding* binding = &bindings[i];
        binding->type = info->bindings[i].descriptorType;

        binding->range.NumDescriptors = info->bindings[i].descriptorCount;
        binding->range.Flags = rangeFlags;
        binding->range.RegisterSpace = 0;

        switch (info->bindings[i].descriptorType) {
            case PAL_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE: {
                binding->range.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
                binding->range.BaseShaderRegister = SRVRegister;

                binding->range.OffsetInDescriptorsFromTableStart = resourceOffset;
                resourceOffset += info->bindings[i].descriptorCount;
                tlasCount += info->bindings[i].descriptorCount;
                SRVRegister += info->bindings[i].descriptorCount;

                break;
            }

            case PAL_DESCRIPTOR_TYPE_SAMPLED_IMAGE: {
                binding->range.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
                binding->range.BaseShaderRegister = SRVRegister;

                binding->range.OffsetInDescriptorsFromTableStart = resourceOffset;
                resourceOffset += info->bindings[i].descriptorCount;
                sampledImageCount += info->bindings[i].descriptorCount;
                SRVRegister += info->bindings[i].descriptorCount;
                break;
            }

            case PAL_DESCRIPTOR_TYPE_SAMPLER: {
                binding->range.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER;
                binding->range.BaseShaderRegister = samplerCount;

                binding->range.OffsetInDescriptorsFromTableStart = samplerOffset;
                samplerOffset += info->bindings[i].descriptorCount;
                samplerCount += info->bindings[i].descriptorCount;
                break;
            }

            case PAL_DESCRIPTOR_TYPE_STORAGE_BUFFER: {
                binding->range.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_UAV;
                binding->range.BaseShaderRegister = UAVRegister;

                binding->range.OffsetInDescriptorsFromTableStart = resourceOffset;
                resourceOffset += info->bindings[i].descriptorCount;
                storageBufferCount += info->bindings[i].descriptorCount;
                UAVRegister += info->bindings[i].descriptorCount;
                break;
            }

            case PAL_DESCRIPTOR_TYPE_STORAGE_IMAGE: {
                binding->range.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_UAV;
                binding->range.BaseShaderRegister = UAVRegister;

                binding->range.OffsetInDescriptorsFromTableStart = resourceOffset;
                resourceOffset += info->bindings[i].descriptorCount;
                storageImageCount += info->bindings[i].descriptorCount;
                UAVRegister += info->bindings[i].descriptorCount;
                break;
            }

            case PAL_DESCRIPTOR_TYPE_UNIFORM_BUFFER: {
                binding->range.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
                binding->range.BaseShaderRegister = CBVRegister;

                binding->range.OffsetInDescriptorsFromTableStart = resourceOffset;
                resourceOffset += info->bindings[i].descriptorCount;
                uniformBufferCount += info->bindings[i].descriptorCount;
                CBVRegister += info->bindings[i].descriptorCount;
                break;
            }
        }
    }

    // check limits
    if (sampledImageCount > d3d12Device->limits.maxDescriptorSampledImages) {
        return PAL_RESULT_INVALID_ARGUMENT;
    }

    if (storageImageCount > d3d12Device->limits.maxDescriptorStorageImages) {
        return PAL_RESULT_INVALID_ARGUMENT;
    }

    if (storageBufferCount > d3d12Device->limits.maxDescriptorStorageBuffers) {
        return PAL_RESULT_INVALID_ARGUMENT;
    }

    if (uniformBufferCount > d3d12Device->limits.maxDescriptorUniformBuffers) {
        return PAL_RESULT_INVALID_ARGUMENT;
    }

    if (tlasCount > d3d12Device->limits.maxDescriptorAccelerationStructures) {
        return PAL_RESULT_INVALID_ARGUMENT;
    }

    if (samplerCount > d3d12Device->limits.maxDescriptorSamplers) {
        return PAL_RESULT_INVALID_ARGUMENT;
    }

    layout->hasDescriptorIndexing = info->enableDescriptorIndexing;
    layout->bindingCount = info->bindingCount;
    layout->samplerCount = samplerCount;
    layout->bindings = bindings;
    *outLayout = (PalDescriptorSetLayout*)layout;
    return PAL_RESULT_SUCCESS;
}

void PAL_CALL destroyDescriptorSetLayoutD3D12(PalDescriptorSetLayout* layout)
{
    DescriptorSetLayout* d3dLayout = (DescriptorSetLayout*)layout;
    palFree(s_D3D12.allocator, d3dLayout->bindings);
    palFree(s_D3D12.allocator, d3dLayout);
}

PalResult PAL_CALL createDescriptorPoolD3D12(
    PalDevice* device,
    const PalDescriptorPoolCreateInfo* info,
    PalDescriptorPool** outPool)
{
    HRESULT result;
    Device* d3d12Device = (Device*)device;
    DescriptorPool* pool = nullptr;

    PalBool hasDescriptorIndexing = d3d12Device->features & PAL_ADAPTER_FEATURE_DESCRIPTOR_INDEXING;
    if (info->enableDescriptorIndexing && !hasDescriptorIndexing) {
        return PAL_RESULT_ADAPTER_FEATURE_NOT_SUPPORTED;
    }

    pool = palAllocate(s_D3D12.allocator, sizeof(DescriptorPool), 0);
    if (!pool) {
        return PAL_RESULT_OUT_OF_MEMORY;
    }

    memset(pool, 0, sizeof(DescriptorPool));
    pool->sets = palAllocate(s_D3D12.allocator, sizeof(DescriptorSet) * info->maxDescriptorSets, 0);
    if (!pool->sets) {
        return PAL_RESULT_OUT_OF_MEMORY;
    }

    uint32_t resourceCount = 0;
    DescriptorHeapLimits* limits = &pool->limits;
    for (int i = 0; i < info->maxDescriptorBindingSizes; i++) {
        PalDescriptorPoolBindingSize* bindingSize = &info->bindingSizes[i];
        if (bindingSize->descriptorType == PAL_DESCRIPTOR_TYPE_SAMPLER) {
            limits->maxSamplers += bindingSize->bindingCount;

        } else if (bindingSize->descriptorType == PAL_DESCRIPTOR_TYPE_UNIFORM_BUFFER) {
            limits->maxUniformBuffers += bindingSize->bindingCount;
            resourceCount += bindingSize->bindingCount;

        } else if (bindingSize->descriptorType == PAL_DESCRIPTOR_TYPE_STORAGE_BUFFER) {
            limits->maxStorageBuffers += bindingSize->bindingCount;
            resourceCount += bindingSize->bindingCount;

        } else if (bindingSize->descriptorType == PAL_DESCRIPTOR_TYPE_SAMPLED_IMAGE) {
            limits->maxSampledImages += bindingSize->bindingCount;
            resourceCount += bindingSize->bindingCount;

        } else if (bindingSize->descriptorType == PAL_DESCRIPTOR_TYPE_STORAGE_IMAGE) {
            limits->maxStorageImages += bindingSize->bindingCount;
            resourceCount += bindingSize->bindingCount;

        } else if (bindingSize->descriptorType == PAL_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE) {
            limits->maxAs += bindingSize->bindingCount;
            resourceCount += bindingSize->bindingCount;
        }
    }

    // resource heap
    if (resourceCount) {
        DescriptorHeap* heap = &pool->resourceHeap;
        D3D12_CPU_DESCRIPTOR_HANDLE __ret, handle;
        D3D12_GPU_DESCRIPTOR_HANDLE __gpuRet, gpuHandle;

        D3D12_DESCRIPTOR_HEAP_DESC desc = {0};
        desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
        desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
        desc.NumDescriptors = resourceCount;

        result = d3d12Device->handle->lpVtbl->CreateDescriptorHeap(
            d3d12Device->handle,
            &desc,
            &IID_DescriptorHeap,
            (void**)&heap->handle);

        if (FAILED(result)) {
            pollMessagesD3D12(d3d12Device);
            if (result == E_OUTOFMEMORY) {
                return PAL_RESULT_OUT_OF_MEMORY;
            }
            return PAL_RESULT_PLATFORM_FAILURE;
        }

        // get increment size
        heap->incrementSize = d3d12Device->handle->lpVtbl->GetDescriptorHandleIncrementSize(
            d3d12Device->handle,
            D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

        // get base CPU and GPU base pointer
        handle = *heap->handle->lpVtbl->GetCPUDescriptorHandleForHeapStart(
            heap->handle,
            &__ret);
        heap->cpuBase = handle.ptr;

        gpuHandle = *heap->handle->lpVtbl->GetGPUDescriptorHandleForHeapStart(
            heap->handle,
            &__gpuRet);
        heap->gpuBase = gpuHandle.ptr;

        pool->hasResourceHeap = PAL_TRUE;
    }

    // sampler heap
    if (limits->maxSamplers) {
        DescriptorHeap* heap = &pool->samplerHeap;
        D3D12_CPU_DESCRIPTOR_HANDLE __ret, handle;
        D3D12_GPU_DESCRIPTOR_HANDLE __gpuRet, gpuHandle;

        D3D12_DESCRIPTOR_HEAP_DESC desc = {0};
        desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
        desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER;
        desc.NumDescriptors = limits->maxSamplers;

        result = d3d12Device->handle->lpVtbl->CreateDescriptorHeap(
            d3d12Device->handle,
            &desc,
            &IID_DescriptorHeap,
            (void**)&heap->handle);

        if (FAILED(result)) {
            if (result == E_OUTOFMEMORY) {
                pollMessagesD3D12(d3d12Device);
                return PAL_RESULT_OUT_OF_MEMORY;
            }
            return PAL_RESULT_PLATFORM_FAILURE;
        }

        // get increment size
        heap->incrementSize = d3d12Device->handle->lpVtbl->GetDescriptorHandleIncrementSize(
            d3d12Device->handle,
            D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER);

        // get base CPU and GPU base pointer
        handle = *heap->handle->lpVtbl->GetCPUDescriptorHandleForHeapStart(
            heap->handle,
            &__ret);
        heap->cpuBase = handle.ptr;

        gpuHandle = *heap->handle->lpVtbl->GetGPUDescriptorHandleForHeapStart(
            heap->handle,
            &__gpuRet);
        heap->gpuBase = gpuHandle.ptr;

        pool->hasSamplerHeap = PAL_TRUE;
    }

    pool->hasDescriptorIndexing = info->enableDescriptorIndexing;
    pool->maxSets = info->maxDescriptorSets;
    *outPool = (PalDescriptorPool*)pool;
    return PAL_RESULT_SUCCESS;
}

void PAL_CALL destroyDescriptorPoolD3D12(PalDescriptorPool* pool)
{
    DescriptorPool* d3dPool = (DescriptorPool*)pool;
    if (d3dPool->hasResourceHeap) {
        d3dPool->resourceHeap.handle->lpVtbl->Release(d3dPool->resourceHeap.handle);
    }

    if (d3dPool->hasSamplerHeap) {
        d3dPool->samplerHeap.handle->lpVtbl->Release(d3dPool->samplerHeap.handle);
    }

    palFree(s_D3D12.allocator, d3dPool->sets);
    palFree(s_D3D12.allocator, d3dPool);
}

PalResult PAL_CALL resetDescriptorPoolD3D12(PalDescriptorPool* pool)
{
    DescriptorPool* d3dPool = (DescriptorPool*)pool;
    d3dPool->resourceHeap.nextOffset = 0;
    d3dPool->samplerHeap.nextOffset = 0;
    d3dPool->usedSets = 0;

    d3dPool->limits.usedAs = 0;
    d3dPool->limits.usedSampledImages = 0;
    d3dPool->limits.usedSamplers = 0;
    d3dPool->limits.usedStorageBuffers = 0;
    d3dPool->limits.usedStorageImages = 0;
    d3dPool->limits.usedUniformBuffers = 0;
    return PAL_RESULT_SUCCESS;
}

PalResult PAL_CALL allocateDescriptorSetD3D12(
    PalDevice* device,
    PalDescriptorPool* pool,
    PalDescriptorSetLayout* layout,
    PalDescriptorSet** outSet)
{
    Device* d3d12Device = (Device*)device;
    DescriptorPool* d3dPool = (DescriptorPool*)pool;
    DescriptorSetLayout* d3dLayout = (DescriptorSetLayout*)layout;
    DescriptorSet* set = nullptr;

    uint32_t storageImageCount = 0;
    uint32_t samplerCount = 0;
    uint32_t storageBufferCount = 0;
    uint32_t uniformBufferCount = 0;
    uint32_t sampledImageCount = 0;
    uint32_t tlasCount = 0;

    if (d3dPool->hasDescriptorIndexing != d3dLayout->hasDescriptorIndexing) {
        return PAL_RESULT_INVALID_OPERATION;
    }

    // get requirements for the sets using the provided layout
    for (int i = 0; i < d3dLayout->bindingCount; i++) {
        DescriptorSetBinding* binding = &d3dLayout->bindings[i];

        if (binding->type == PAL_DESCRIPTOR_TYPE_SAMPLER) {
            samplerCount += binding->range.NumDescriptors;

        } else if (binding->type == PAL_DESCRIPTOR_TYPE_STORAGE_BUFFER) {
            storageBufferCount += binding->range.NumDescriptors;

        } else if (binding->type == PAL_DESCRIPTOR_TYPE_STORAGE_IMAGE) {
            storageImageCount += binding->range.NumDescriptors;

        } else if (binding->type == PAL_DESCRIPTOR_TYPE_UNIFORM_BUFFER) {
            uniformBufferCount += binding->range.NumDescriptors;

        } else if (binding->type == PAL_DESCRIPTOR_TYPE_SAMPLED_IMAGE) {
            sampledImageCount += binding->range.NumDescriptors;

        } else if (binding->type == PAL_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE) {
            tlasCount += binding->range.NumDescriptors;
        }
    }

    // validate descriptor sets limits
    if (d3dPool->usedSets + 1 > d3dPool->maxSets) {
        // all sets are used
        return PAL_RESULT_OUT_OF_MEMORY;
    }

    // check descriptor limits
    DescriptorHeapLimits* limits = &d3dPool->limits;
    if (limits->usedAs + tlasCount > limits->maxAs) {
        return PAL_RESULT_OUT_OF_MEMORY;
    }

    if (limits->usedSampledImages + sampledImageCount > limits->maxSampledImages) {
        return PAL_RESULT_OUT_OF_MEMORY;
    }

    if (limits->usedSamplers + samplerCount > limits->maxSamplers) {
        return PAL_RESULT_OUT_OF_MEMORY;
    }

    if (limits->usedStorageBuffers + storageBufferCount > limits->maxStorageBuffers) {
        return PAL_RESULT_OUT_OF_MEMORY;
    }

    if (limits->usedStorageImages + storageImageCount > limits->maxStorageImages) {
        return PAL_RESULT_OUT_OF_MEMORY;
    }

    if (limits->usedUniformBuffers + uniformBufferCount > limits->maxUniformBuffers) {
        return PAL_RESULT_OUT_OF_MEMORY;
    }

    // assign offset base to the set so we know where to start and end for each set.
    set = &d3dPool->sets[d3dPool->usedSets++];
    set->resourceOffset = d3dPool->resourceHeap.nextOffset;
    set->samplerOffset = d3dPool->samplerHeap.nextOffset;
    set->layout = d3dLayout;
    set->pool = d3dPool;

    uint32_t totalDescriptors = tlasCount + storageBufferCount + uniformBufferCount;
    totalDescriptors += sampledImageCount + storageImageCount;
    d3dPool->resourceHeap.nextOffset += totalDescriptors;
    d3dPool->samplerHeap.nextOffset += samplerCount;

    limits->usedAs += tlasCount;
    limits->usedSampledImages += sampledImageCount;
    limits->usedSamplers += samplerCount;
    limits->usedStorageBuffers += storageBufferCount;
    limits->usedUniformBuffers += uniformBufferCount;

    *outSet = (PalDescriptorSet*)set;
    return PAL_RESULT_SUCCESS;
}

PalResult PAL_CALL updateDescriptorSetD3D12(
    PalDevice* device,
    uint32_t count,
    PalDescriptorSetWriteInfo* infos)
{
    Device* d3d12Device = (Device*)device;
    for (int i = 0; i < count; i++) {
        PalDescriptorSetWriteInfo* info = &infos[i];
        DescriptorSet* set = (DescriptorSet*)info->descriptorSet;
        DescriptorPool* pool = set->pool;
        DescriptorSetLayout* layout = set->layout;

        if (info->layoutBindingIndex > layout->bindingCount) {
            return PAL_RESULT_INVALID_ARGUMENT;
        }

        DescriptorSetBinding* binding = &layout->bindings[info->layoutBindingIndex];
        DescriptorHeap* heap = nullptr;
        uint32_t index = 0;
        uint32_t bindingOffset = binding->range.OffsetInDescriptorsFromTableStart;

        if (binding->type == PAL_DESCRIPTOR_TYPE_SAMPLER) {
            heap = &pool->samplerHeap;
            index = set->samplerOffset + bindingOffset + info->arrayElement;

        } else {
            heap = &pool->resourceHeap;
            index = set->resourceOffset + bindingOffset + info->arrayElement;
        }

        for (int j = 0; j < info->descriptorCount; j++) {
            D3D12_CPU_DESCRIPTOR_HANDLE dst;
            dst.ptr = getDescriptorHandleD3D12(index + j, heap->incrementSize, heap->cpuBase);

            if (info->descriptorType == PAL_DESCRIPTOR_TYPE_SAMPLER) {
                if (info->samplerInfos) {
                    Sampler* sampler = (Sampler*)info->samplerInfos[j].sampler;
                    d3d12Device->handle->lpVtbl->CreateSampler(d3d12Device->handle, &sampler->desc, dst);

                } else {
                    if (!(d3d12Device->features & PAL_ADAPTER_FEATURE_NULL_DESCRIPTORS)) {
                        return PAL_RESULT_ADAPTER_FEATURE_NOT_SUPPORTED;
                    }

                    D3D12_SAMPLER_DESC desc = {0};
                    desc.MaxAnisotropy = 1;
                    desc.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
                    desc.BorderColor[3] = 1.0f;

                    desc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
                    desc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
                    desc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
                    desc.Filter = D3D12_FILTER_MIN_MAG_MIP_POINT;
                    d3d12Device->handle->lpVtbl->CreateSampler(d3d12Device->handle, &desc, dst);
                }

            } else if (info->descriptorType == PAL_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE) {
                D3D12_SHADER_RESOURCE_VIEW_DESC desc = {0};
                desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
                desc.ViewDimension = D3D12_SRV_DIMENSION_RAYTRACING_ACCELERATION_STRUCTURE;

                if (info->tlasInfos) {
                    AccelerationStructure* tlas = nullptr;
                    tlas = (AccelerationStructure*)info->tlasInfos[j].tlas;
                    desc.RaytracingAccelerationStructure.Location = tlas->address;

                } else {
                    if (!(d3d12Device->features & PAL_ADAPTER_FEATURE_NULL_DESCRIPTORS)) {
                        return PAL_RESULT_ADAPTER_FEATURE_NOT_SUPPORTED;
                    }
                }

                d3d12Device->handle->lpVtbl->CreateShaderResourceView(
                    d3d12Device->handle,
                    nullptr,
                    &desc,
                    dst);

            } else if (info->descriptorType == PAL_DESCRIPTOR_TYPE_SAMPLED_IMAGE) {
                D3D12_SHADER_RESOURCE_VIEW_DESC desc = {0};
                desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
            
                PalImageSubresourceRange range = {0};
                PalImageViewType type;
                ID3D12Resource* handle = nullptr;

                if (info->imageViewInfos) {
                    ImageView* imageView = (ImageView*)info->imageViewInfos[j].imageView;
                    desc.Format = imageView->format;
                    type = imageView->type;
                    range = imageView->range;
                    handle = imageView->image->handle;

                } else {
                    if (!(d3d12Device->features & PAL_ADAPTER_FEATURE_NULL_DESCRIPTORS)) {
                        return PAL_RESULT_ADAPTER_FEATURE_NOT_SUPPORTED;
                    }

                    desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
                    type = PAL_IMAGE_VIEW_TYPE_2D;
                    range.mipLevelCount = 1;
                    range.layerArrayCount = 1;
                    range.aspect = PAL_IMAGE_ASPECT_COLOR;
                }

                fillSubresourceD3D12(
                    type,
                    &range,
                    nullptr,
                    nullptr,
                    &desc,
                    nullptr);

                d3d12Device->handle->lpVtbl->CreateShaderResourceView(
                    d3d12Device->handle,
                    handle,
                    &desc,
                    dst);

            } else if (info->descriptorType == PAL_DESCRIPTOR_TYPE_STORAGE_IMAGE) {
                D3D12_UNORDERED_ACCESS_VIEW_DESC desc = {0};
                PalImageSubresourceRange range = {0};
                PalImageViewType type;
                ID3D12Resource* handle = nullptr;

                if (info->imageViewInfos) {
                    ImageView* imageView = (ImageView*)info->imageViewInfos[j].imageView;
                    desc.Format = imageView->format;
                    type = imageView->type;
                    range = imageView->range;
                    handle = imageView->image->handle;

                } else {
                    if (!(d3d12Device->features & PAL_ADAPTER_FEATURE_NULL_DESCRIPTORS)) {
                        return PAL_RESULT_ADAPTER_FEATURE_NOT_SUPPORTED;
                    }

                    desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
                    type = PAL_IMAGE_VIEW_TYPE_2D;
                    range.mipLevelCount = 1;
                    range.layerArrayCount = 1;
                    range.aspect = PAL_IMAGE_ASPECT_COLOR;
                }

                fillSubresourceD3D12(
                    type,
                    &range,
                    nullptr,
                    nullptr,
                    nullptr,
                    &desc);

                d3d12Device->handle->lpVtbl->CreateUnorderedAccessView(
                    d3d12Device->handle,
                    handle,
                    nullptr,
                    &desc,
                    dst);

            } else if (info->descriptorType == PAL_DESCRIPTOR_TYPE_UNIFORM_BUFFER) {
                D3D12_CONSTANT_BUFFER_VIEW_DESC desc = {0};
                D3D12_GPU_VIRTUAL_ADDRESS address = 0;

                if (info->bufferInfos) {
                    PalDescriptorBufferInfo* bufferInfo = &info->bufferInfos[j];
                    Buffer* buffer = (Buffer*)bufferInfo->buffer;
                    desc.SizeInBytes = bufferInfo->size;
                    address = buffer->handle->lpVtbl->GetGPUVirtualAddress(buffer->handle);
                    desc.BufferLocation = address + bufferInfo->offset;

                } else {
                    if (!(d3d12Device->features & PAL_ADAPTER_FEATURE_NULL_DESCRIPTORS)) {
                        return PAL_RESULT_ADAPTER_FEATURE_NOT_SUPPORTED;
                    }
                }

                d3d12Device->handle->lpVtbl->CreateConstantBufferView(
                    d3d12Device->handle,
                    &desc,
                    dst);

            } else {
                // storage buffer
                D3D12_UNORDERED_ACCESS_VIEW_DESC desc = {0};
                desc.ViewDimension = D3D12_UAV_DIMENSION_BUFFER;

                ID3D12Resource* handle = nullptr;
                if (info->bufferInfos) {
                    PalDescriptorBufferInfo* bufferInfo = &info->bufferInfos[j];
                    Buffer* buffer = (Buffer*)bufferInfo->buffer;

                    uint32_t stride = 4;
                    if (bufferInfo->stride) {
                        stride = bufferInfo->stride;
                        desc.Buffer.StructureByteStride = bufferInfo->stride;
                    } else {
                        desc.Format = DXGI_FORMAT_R32_TYPELESS;
                        desc.Buffer.Flags = D3D12_BUFFER_UAV_FLAG_RAW;
                    }

                    handle = buffer->handle;
                    desc.Buffer.FirstElement = bufferInfo->offset / stride;
                    desc.Buffer.NumElements = bufferInfo->size / stride;

                } else {
                    if (!(d3d12Device->features & PAL_ADAPTER_FEATURE_NULL_DESCRIPTORS)) {
                        return PAL_RESULT_ADAPTER_FEATURE_NOT_SUPPORTED;
                    }
                }            

                d3d12Device->handle->lpVtbl->CreateUnorderedAccessView(
                    d3d12Device->handle,
                    handle,
                    nullptr,
                    &desc,
                    dst);
            }
        }
    }
    return PAL_RESULT_SUCCESS;
}

// ==================================================
// Pipeline Layout
// ==================================================

PalResult PAL_CALL createPipelineLayoutD3D12(
    PalDevice* device,
    const PalPipelineLayoutCreateInfo* info,
    PalPipelineLayout** outLayout)
{
    Device* d3d12Device = (Device*)device;
    PipelineLayout* layout = nullptr;
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
        return PAL_RESULT_INVALID_ARGUMENT;
    }

    if (d3d12Device->shaderModel >= PAL_MAKE_SHADER_TARGET(6, 6)) {
        rootFlags |= D3D12_ROOT_SIGNATURE_FLAG_CBV_SRV_UAV_HEAP_DIRECTLY_INDEXED;
        rootFlags |= D3D12_ROOT_SIGNATURE_FLAG_SAMPLER_HEAP_DIRECTLY_INDEXED;
    }

    // get the total resource and sampler ranges for all provided descriptor set layouts
    for (int i = 0; i < info->descriptorSetLayoutCount; i++) {
        DescriptorSetLayout* tmp = (DescriptorSetLayout*)info->descriptorSetLayouts[i];
        resourceCount += tmp->bindingCount - tmp->samplerCount;
        samplerCount += tmp->samplerCount;

        if (tmp->bindingCount - tmp->samplerCount >= 1) {
            parameterCount++;
        }

        if (tmp->samplerCount >= 1) {
            parameterCount++;
        }
    }

    // get the total size needed for all provided push constant range
    for (int i = 0; i < info->pushConstantRangeCount; i++) {
        PalPushConstantRange* tmp = &info->pushConstantRanges[i];
        if (tmp->offset + tmp->size > pushConstantSize) {
            pushConstantSize = tmp->offset + tmp->size;
        }
    }

    if (pushConstantSize > d3d12Device->limits.maxPushConstantSize) {
        return PAL_RESULT_INVALID_ARGUMENT;
    }

    layout = palAllocate(s_D3D12.allocator, sizeof(PipelineLayout), 0);
    if (!layout) {
        return PAL_RESULT_OUT_OF_MEMORY;
    }

    if (pushConstantSize) {
        parameterCount++;
    }

    if (parameterCount) {
        uint32_t paramtersSize = sizeof(D3D12_ROOT_PARAMETER1) * parameterCount;
        parameters = palAllocate(s_D3D12.allocator, paramtersSize, 0);
        if (!parameters) {
            return PAL_RESULT_OUT_OF_MEMORY;
        }

        memset(parameters, 0, paramtersSize);
    }

    if (resourceCount) {
        ranges = palAllocate(s_D3D12.allocator, sizeInBytes * resourceCount, 0);
        if (!ranges) {
            return PAL_RESULT_OUT_OF_MEMORY;
        }
    }

    if (samplerCount) {
        samplerRanges = palAllocate(s_D3D12.allocator, sizeInBytes * samplerCount, 0);
        if (!samplerRanges) {
            return PAL_RESULT_OUT_OF_MEMORY;
        }
    }

    // write root constant first if its provided
    parameterCount = 0; // reset and reuse the same variable
    layout->constantIndex = UINT32_MAX;
    if (pushConstantSize) {
        D3D12_ROOT_PARAMETER1* parameter = &parameters[parameterCount];
        parameter->ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
        parameter->Constants.Num32BitValues = (UINT)pushConstantSize / 4;
        parameter->ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

        layout->constantIndex = parameterCount;
        parameterCount++;
    }

    uint32_t registerSpace = 0;
    for (int i = 0; i < info->descriptorSetLayoutCount; i++) {
        DescriptorSetLayout* tmp = (DescriptorSetLayout*)info->descriptorSetLayouts[i];
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
            parameter->ShaderVisibility = tmp->visibility;

            rangesOffset += resourceCount;
            parameterCount++;
        }

        // sampler ranges
        if (samplerCount) {
            parameter = &parameters[parameterCount];
            parameter->ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
            parameter->DescriptorTable.NumDescriptorRanges = samplerCount;
            parameter->DescriptorTable.pDescriptorRanges = &samplerRanges[samplerRangesOffset];
            parameter->ShaderVisibility = tmp->visibility;

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
        if (result == E_INVALIDARG) {
            return PAL_RESULT_INVALID_ARGUMENT;
        }
        return PAL_RESULT_PLATFORM_FAILURE;
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
        if (result == E_INVALIDARG) {
            return PAL_RESULT_INVALID_ARGUMENT;
        } else if (result == E_OUTOFMEMORY) {
            return PAL_RESULT_OUT_OF_MEMORY;
        }
        return PAL_RESULT_PLATFORM_FAILURE;
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
    PipelineLayout* d3dLayout = (PipelineLayout*)layout;
    d3dLayout->handle->lpVtbl->Release(d3dLayout->handle);
    palFree(s_D3D12.allocator, d3dLayout);
}

// ==================================================
// Pipeline
// ==================================================

PalResult PAL_CALL createGraphicsPipelineD3D12(
    PalDevice* device,
    const PalGraphicsPipelineCreateInfo* info,
    PalPipeline** outPipeline)
{
    HRESULT result;
    uint32_t patchControlPoints = 0;
    uint32_t totalSize = 0;
    PalBool alphaToCoverageEnable = PAL_FALSE;
    Pipeline* pipeline = nullptr;
    Device* d3d12Device = (Device*)device;
    PipelineLayout* layout = (PipelineLayout*)info->pipelineLayout;

    D3D12_INPUT_ELEMENT_DESC* elementDescs = nullptr;
    D3D12_VIEW_INSTANCE_LOCATION* viewLocations = nullptr;

    GraphicsPipelineStreamDesc graphicsStreamDesc = {0};
    memset(&graphicsStreamDesc, 0, sizeof(GraphicsPipelineStreamDesc));

    pipeline = palAllocate(s_D3D12.allocator, sizeof(Pipeline), 0);
    if (!pipeline) {
        return PAL_RESULT_OUT_OF_MEMORY;
    }
    memset(pipeline, 0, sizeof(Pipeline));

    if (info->renderingLayout->viewCount > 1) {
        viewLocations = palAllocate(
            s_D3D12.allocator,
            sizeof(D3D12_VIEW_INSTANCE_LOCATION) * info->renderingLayout->viewCount,
            0);

        if (!viewLocations) {
            return PAL_RESULT_OUT_OF_MEMORY;
        }
    }

    // Root signature
    RootSignatureStream* rootSignatureStream = &graphicsStreamDesc.layout;
    totalSize += sizeof(RootSignatureStream);
    rootSignatureStream->type = D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_ROOT_SIGNATURE;
    rootSignatureStream->root = layout->handle;
    
    // shaders
    for (int i = 0; i < info->shaderCount; i++) {
        Shader* tmp = (Shader*)info->shaders[i];
        ShaderStream* shaderStream = &graphicsStreamDesc.shaders[i];
        totalSize += sizeof(ShaderStream);
        D3D12_PIPELINE_STATE_SUBOBJECT_TYPE type;

        ShaderEntry* entry = &tmp->entries[0];
        if (entry->patchControlPoints) {
            patchControlPoints = entry->patchControlPoints;
            if (info->topology != PAL_PRIMITIVE_TOPOLOGY_PATCH) {
                palFree(s_D3D12.allocator, pipeline);
                return PAL_RESULT_INVALID_OPERATION;
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
        return PAL_RESULT_INVALID_ARGUMENT;
    }

    if (vertexCount > d3d12Device->limits.maxVertexAttributes) {
        return PAL_RESULT_INVALID_ARGUMENT;
    }

    InputLayoutStream* inputLayoutStream = &graphicsStreamDesc.inputLayout;
    totalSize += sizeof(InputLayoutStream);
    inputLayoutStream->type = D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_INPUT_LAYOUT;

    pipeline->strides = nullptr;
    if (vertexCount) {
        pipeline->strides = palAllocate(s_D3D12.allocator, sizeof(uint32_t) * 8, 0);
        if (!pipeline->strides) {
            return PAL_RESULT_OUT_OF_MEMORY;
        }

        elementDescs = palAllocate(
            s_D3D12.allocator,
            sizeof(D3D12_INPUT_ELEMENT_DESC) * vertexCount,
            0);

        if (!elementDescs) {
            palFree(s_D3D12.allocator, elementDescs);
            return PAL_RESULT_OUT_OF_MEMORY;
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
                if (vertexAttrib->semanticName) {
                    elementDesc->SemanticName = vertexAttrib->semanticName;
                } else {
                    elementDesc->SemanticName = semanticIDToStringD3D12(vertexAttrib->semanticID);
                }

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
            pipeline->combinerOps[i]  = combinerOpsToD3D12(state->combinerOps[i]);
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
        if (result == E_INVALIDARG) {
            return PAL_RESULT_INVALID_ARGUMENT;
        } else if (result == E_OUTOFMEMORY) {
            return PAL_RESULT_OUT_OF_MEMORY;
        }
        return PAL_RESULT_PLATFORM_FAILURE;
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
    Device* d3d12Device = (Device*)device;
    PipelineLayout* layout = (PipelineLayout*)info->pipelineLayout;
    Shader* shader = (Shader*)info->computeShader;
    Pipeline* pipeline = nullptr;

    pipeline = palAllocate(s_D3D12.allocator, sizeof(Pipeline), 0);
    if (!pipeline) {
        return PAL_RESULT_OUT_OF_MEMORY;
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
        if (result == E_INVALIDARG) {
            return PAL_RESULT_INVALID_ARGUMENT;
        } else if (result == E_OUTOFMEMORY) {
            return PAL_RESULT_OUT_OF_MEMORY;
        }
        return PAL_RESULT_PLATFORM_FAILURE;
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
    Device* d3d12Device = (Device*)device;
    PipelineLayout* layout = (PipelineLayout*)info->pipelineLayout;
    Pipeline* pipeline = nullptr;

    if (!(d3d12Device->features & PAL_ADAPTER_FEATURE_RAY_TRACING)) {
        return PAL_RESULT_ADAPTER_FEATURE_NOT_SUPPORTED;
    }

    if (info->maxAttributeSize > d3d12Device->limits.maxHitAttributeSize) {
        return PAL_RESULT_INVALID_ARGUMENT;
    }

    if (info->maxPayloadSize > d3d12Device->limits.maxPayloadSize) {
        return PAL_RESULT_INVALID_ARGUMENT;
    }

    if (info->maxRecursionDepth> d3d12Device->limits.maxRecursionDepth) {
        return PAL_RESULT_INVALID_ARGUMENT;
    }

    D3D12_EXPORT_DESC* exportDescs = nullptr;
    D3D12_DXIL_LIBRARY_DESC* libraryDescs = nullptr;
    RayHitGroup* hitGroups = nullptr;
    D3D12_STATE_SUBOBJECT* subObjects = nullptr;
    const wchar_t** localExports = nullptr;

    // find the total number of exports
    uint32_t exportCount = 0;
    for (int i = 0; i < info->shaderCount; i++) {
        Shader* shader = (Shader*)info->shaders[i];
        exportCount += shader->entryCount;
    }

    uint32_t subObjectCount = 0;
    uint32_t localExportCount = 0;
    ShaderBindingTableInfo sbtInfo = {0};

    for (int i = 0; i < info->shaderGroupCount; i++) {
        PalRayTracingShaderGroupCreateInfo* tmp = &info->shaderGroups[i];
        if (tmp->type == PAL_RAY_TRACING_SHADER_GROUP_TYPE_GENERAL) {
            // check if its raygen, miss or callable
            Shader* shader = (Shader*)info->shaders[tmp->generalShaderIndex];
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

    pipeline = palAllocate(s_D3D12.allocator, sizeof(Pipeline), 0);
    if (!pipeline) {
        return PAL_RESULT_OUT_OF_MEMORY;
    }

    pipeline->shaderExportCount = exportCount + sbtInfo.hitCount;
    subObjects = palAllocate(s_D3D12.allocator, sizeof(D3D12_STATE_SUBOBJECT) * subObjectCount, 0);
    hitGroups = palAllocate(s_D3D12.allocator, sizeof(RayHitGroup) * sbtInfo.hitCount, 0);
    if (!subObjects || !hitGroups) {
        return PAL_RESULT_OUT_OF_MEMORY;
    }

    libraryDescs = palAllocate(
        s_D3D12.allocator, 
        sizeof(D3D12_DXIL_LIBRARY_DESC) * info->shaderCount, 
        0);

    exportDescs = palAllocate(
        s_D3D12.allocator, 
        sizeof(D3D12_EXPORT_DESC) * exportCount, 
        0);

    pipeline->shaderExports = palAllocate(
        s_D3D12.allocator, 
        sizeof(ShaderExport) * pipeline->shaderExportCount, 
        0);

    localExports = palAllocate(
        s_D3D12.allocator, 
        sizeof(wchar_t*) * localExportCount, 
        0);

    if (!libraryDescs || !exportDescs || !pipeline->shaderExports || !localExports) {
        return PAL_RESULT_OUT_OF_MEMORY;
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
        Shader* tmp = (Shader*)info->shaders[i];
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
                Shader* shader = (Shader*)info->shaders[tmp->generalShaderIndex];
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
            Shader* shader = (Shader*)info->shaders[tmp->anyHitShaderIndex];
            ShaderEntry* entry = &shader->entries[tmp->anyHitShaderEntryIndex];
            group->AnyHitShaderImport = entry->entryName;

        } else {
            group->AnyHitShaderImport = nullptr;
        }

        // Closest hit shader
        if (tmp->closestHitShaderIndex != PAL_UNUSED_SHADER_INDEX) {
            Shader* shader = (Shader*)info->shaders[tmp->closestHitShaderIndex];
            ShaderEntry* entry = &shader->entries[tmp->closestHitShaderEntryIndex];
            group->ClosestHitShaderImport = entry->entryName;

        } else {
            group->ClosestHitShaderImport = nullptr;
        }

        // IntersectionShader shader
        if (tmp->intersectionShaderIndex != PAL_UNUSED_SHADER_INDEX) {
            Shader* shader = (Shader*)info->shaders[tmp->intersectionShaderIndex];
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
        parameter.Constants.Num32BitValues = alignD3D12(localRootSize, 4) / 4;
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
            if (result == E_INVALIDARG) {
                return PAL_RESULT_INVALID_ARGUMENT;
            }
            return PAL_RESULT_PLATFORM_FAILURE;
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
            if (result == E_INVALIDARG) {
                return PAL_RESULT_INVALID_ARGUMENT;
            } else if (result == E_OUTOFMEMORY) {
                return PAL_RESULT_OUT_OF_MEMORY;
            }
            return PAL_RESULT_PLATFORM_FAILURE;
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
        if (result == E_INVALIDARG) {
            return PAL_RESULT_INVALID_ARGUMENT;
        } else if (result == E_OUTOFMEMORY) {
            return PAL_RESULT_OUT_OF_MEMORY;
        }
        return PAL_RESULT_PLATFORM_FAILURE;
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
    Pipeline* d3dPipeline = (Pipeline*)pipeline;
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

// ==================================================
// Shader Binding Table
// ==================================================



#endif // PAL_HAS_D3D12_BACKEND

#endif // _WIN32
