
/**
    PAL - Prime Abstraction Layer
    Copyright (C) 2025
    Licensed under the Zlib license. See LICENSE file in root.
 */

#if PAL_HAS_D3D12_BACKEND
#include "pal_d3d12.h"

// IIDS
IID IID_Device = {0xc4fec28f, 0x7966, 0x4e95, 0x9f,0x94, 0xf4,0x31,0xcb,0x56,0xc3,0xb8};
IID IID_Adapter = {0x3c8d99d1, 0x4fbf, 0x4181, 0xa8,0x2c, 0xaf,0x66,0xbf,0x7b,0xd2,0x4e};
IID IID_Factory = {0xc1b6694f, 0xff09, 0x44a9, 0xb0,0x3c, 0x77,0x90,0x0a,0x0a,0x1d,0x17};
IID IID_DebugController = {0x344488b7, 0x6846, 0x474b, 0xb9,0x89, 0xf0,0x27,0x44,0x82,0x45,0xe0};
IID IID_DebugController1 = {0xaffaa4ca, 0x63fe, 0x4d8e, 0xb8,0xad, 0x15,0x90,0x00,0xaf,0x43,0x04};
IID IID_InfoQueue1 = {0x2852dd88, 0xb484, 0x4c0c, 0xb6,0xb1, 0x67,0x16,0x85,0x00,0xe6,0x00};
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

D3D12 s_D3D12 = {0};

PalResult makeResultD3D12(HRESULT result)
{
    PalResultCode code = PAL_RESULT_CODE_PLATFORM_FAILURE;
    switch (result) {
        case E_NOTIMPL:
        case DXGI_ERROR_UNSUPPORTED: {
            code = PAL_RESULT_CODE_FEATURE_NOT_SUPPORTED;
            break;
        }

        case E_OUTOFMEMORY: {
            code = PAL_RESULT_CODE_OUT_OF_MEMORY;
            break;
        }

        case E_ACCESSDENIED: {
            code = PAL_RESULT_CODE_INVALID_OPERATION;
            break;
        }

        case DXGI_ERROR_DEVICE_HUNG:
        case DXGI_ERROR_DEVICE_REMOVED:
        case DXGI_ERROR_DEVICE_RESET: {
            code = PAL_RESULT_CODE_DEVICE_LOST;
            break;
        }

        case E_INVALIDARG: {
            code = PAL_RESULT_CODE_INVALID_ARGUMENT;
            break;
        }
    }

    return palMakeResult(code, PAL_RESULT_SOURCE_D3D12, (uint32_t)result);
}

DXGI_FORMAT formatToD3D12(PalFormat format)
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

uint32_t samplesToD3D12(PalSampleCount count)
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

D3D12_COMPARISON_FUNC compareOpToD3D12(PalCompareOp op)
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

D3D12_SHADING_RATE_COMBINER combinerOpsToD3D12(PalFragmentShadingRateCombinerOp op)
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

D3D12_SHADING_RATE shadingRateToD3D12(PalFragmentShadingRate rate)
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

uint32_t getFormatSizeD3D12(PalFormat format)
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

DXGI_FORMAT vertexTypeToD3D12(PalVertexType type)
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

D3D12_RAYTRACING_INSTANCE_FLAGS instanceFlagsToD3D12(PalAccelerationStructureInstanceFlags flags)
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

void fillBuildInfoD3D12(
    PalBool getBuildSize,
    PalAccelerationStructureBuildInfo* info,
    D3D12_RAYTRACING_GEOMETRY_DESC* geometries,
    D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_DESC* buildInfo)
{
    static uint32_t maxInstanceCount = 1000000;
    static uint32_t maxPrimitiveCount = 10000000;
    static uint32_t maxGeometryCount = 100000;

    if (info->type == PAL_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL) {
        if (info->count > maxGeometryCount) {
            return;
        }

        for (int i = 0; i < info->count; i++) {
            if (info->geometries[i].primitiveCount > maxPrimitiveCount) {
                return;
            }

            D3D12_RAYTRACING_GEOMETRY_DESC* tmp = &geometries[i];
            tmp->Flags = 0;
            if (info->geometries[i].flags & PAL_GEOMETRY_FLAG_NO_DUPLICATE_ANYHIT) {
                tmp->Flags |= D3D12_RAYTRACING_GEOMETRY_FLAG_NO_DUPLICATE_ANYHIT_INVOCATION;
            }

            if (info->geometries[i].flags & PAL_GEOMETRY_FLAG_OPAQUE) {
                tmp->Flags |= D3D12_RAYTRACING_GEOMETRY_FLAG_OPAQUE;
            }

            if (info->geometries[i].type == PAL_GEOMETRY_TYPE_TRIANGLE) {
                tmp->Type = D3D12_RAYTRACING_GEOMETRY_TYPE_TRIANGLES;
                const PalGeometryDataTriangle* tmpData = info->geometries[i].data;

                tmp->Triangles.VertexBuffer.StartAddress = tmpData->vertexBufferAddress;
                tmp->Triangles.VertexBuffer.StrideInBytes = tmpData->vertexStride;
                tmp->Triangles.VertexCount = tmpData->vertexCount;
                tmp->Triangles.VertexFormat = vertexTypeToD3D12(tmpData->vertexType);

                tmp->Triangles.Transform3x4 = tmpData->transformBufferAddress;

                tmp->Triangles.IndexBuffer = tmpData->indexBufferAddress;
                tmp->Triangles.IndexCount = info->geometries[i].primitiveCount * 3;
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
                const PalGeometryDataAABBS* tmpData = info->geometries[i].data;

                tmp->AABBs.AABBCount = info->geometries[i].primitiveCount;
                tmp->AABBs.AABBs.StartAddress = tmpData->bufferAddress;
                tmp->AABBs.AABBs.StrideInBytes = tmpData->stride;
            }
        }

        buildInfo->Inputs.Type = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL;
        buildInfo->Inputs.NumDescs = info->count;
        buildInfo->Inputs.pGeometryDescs = geometries;

    } else {
        if (info->count > maxInstanceCount) {
            return;
        }

        buildInfo->Inputs.Type = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL;
        buildInfo->Inputs.NumDescs = info->count;
        buildInfo->Inputs.InstanceDescs = info->instanceBufferAddress;
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

    AccelerationStructureD3D12* dstAs = (AccelerationStructureD3D12*)info->dst;
    AccelerationStructureD3D12* srcAs = (AccelerationStructureD3D12*)info->src;

    D3D12_GPU_VIRTUAL_ADDRESS dstAsHandle = 0;
    if (dstAs) {
        dstAsHandle = dstAs->address;
    }

    D3D12_GPU_VIRTUAL_ADDRESS srcAsHandle = 0;
    if (srcAs) {
        srcAsHandle = srcAs->address;
    }

    buildInfo->Inputs.DescsLayout = D3D12_ELEMENTS_LAYOUT_ARRAY;
    buildInfo->DestAccelerationStructureData = dstAsHandle;
    buildInfo->SourceAccelerationStructureData = srcAsHandle;
    buildInfo->ScratchAccelerationStructureData = info->scratchBufferAddress;
}

void fillSubresourceD3D12(
    uint32_t descType,
    PalImageViewType type,
    const PalImageSubresourceRange* range,
    void* desc)
{
    if (descType == DESC_TYPE_RTV) {
        D3D12_RENDER_TARGET_VIEW_DESC* rtvDesc = desc;

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

    } else if (descType == DESC_TYPE_DSV) {
        D3D12_DEPTH_STENCIL_VIEW_DESC* dsvDesc = desc;

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

    } else if (descType == DESC_TYPE_SRV) {
        D3D12_SHADER_RESOURCE_VIEW_DESC* srvDesc = desc;

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

    } else if (descType == DESC_TYPE_UAV) {
        D3D12_UNORDERED_ACCESS_VIEW_DESC* uavDesc = desc;

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

uint64_t getDescriptorHandleD3D12(
    uint32_t index,
    uint32_t size,
    uint64_t baseOffset)
{
    return baseOffset + index * size;
}

void getDescriptorTierLimitsD3D12(
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

PalResult PAL_CALL initGraphicsD3D12(
    const PalGraphicsDebugger* debugger,
    const PalAllocator* allocator)
{
    // load d3d12
    s_D3D12.handle = LoadLibraryA("d3d12.dll");
    s_D3D12.dxgi = LoadLibraryA("dxgi.dll");
    if (!s_D3D12.handle || !s_D3D12.dxgi) {
        return palMakeResult(
            PAL_RESULT_CODE_PLATFORM_FAILURE, 
            PAL_RESULT_SOURCE_WIN32, 
            GetLastError());
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

    if (debugger) {
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
        return makeResultD3D12(result);
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