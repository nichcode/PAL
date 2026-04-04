
/**

Copyright (C) 2025-2026 Nicholas Agbo <agbonicholas04@gmail.com>

This software is provided 'as-is', without any express or implied
warranty.  In no event will the authors be held liable for any damages
arising from the use of this software.

Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it
freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not
   claim that you wrote the original software. If you use this software
   in a product, an acknowledgment in the product documentation would be
   appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be
   misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.

 */

// ==================================================
// Includes
// ==================================================

#include "pal/pal_graphics.h"

#if PAL_HAS_D3D12

#ifdef __WIN32
#include <windows.h>
#include <d3d12.h>
#include <dxgi1_6.h>
#include <d3d12sdklayers.h>

// ==================================================
// Typedefs, enums and structs
// ==================================================

// on older SDKs, D3D_FEATURE_LEVEL_12_2 is not defined
#ifndef D3D_FEATURE_LEVEL_12_2
#define D3D_FEATURE_LEVEL_12_2 0xc200
#endif // D3D_FEATURE_LEVEL_12_2

// IIDS
const IID IID_Device = {0xc4fec28f, 0x7966, 0x4e95, 0x9f,0x94, 0xf4,0x31,0xcb,0x56,0xc3,0xb8};
const IID IID_Adapter = {0x3c8d99d1, 0x4fbf, 0x4181, 0xa8,0x2c, 0xaf,0x66,0xbf,0x7b,0xd2,0x4e};
const IID IID_Factory = {0xc1b6694f, 0xff09, 0x44a9, 0xb0,0x3c, 0x77,0x90,0x0a,0x0a,0x1d,0x17};
const IID IID_Debug = {0x344488b7, 0x6846, 0x474b, 0xb9,0x89, 0xf0,0x27,0x44,0x82,0x45,0xe0};
const IID IID_Debug1 = {0xaffaa4ca, 0x63fe, 0x4d8e, 0xb8,0xad, 0x15,0x90,0x00,0xaf,0x43,0x04};
const IID IID_InfoQueue = {0x0742a90b, 0xc387, 0x483f, 0xb9,0x46, 0x30,0xa7,0xe4,0xe6,0x14,0x58};
const IID IID_Heap = {0x6b3b2502, 0x6e51, 0x45b3, 0x90,0xee, 0x98,0x84,0x26,0x5e,0x8d,0xf3};
const IID IID_Queue = {0x0ec870a6, 0x5d7e, 0x4c22, 0x8c,0xfc, 0x5b,0xaa,0xe0,0x76,0x16,0xed};
const IID IID_Fence = {0x0a753dcf, 0xc4d8, 0x4b91, 0xad,0xf6, 0xbe,0x5a,0x60,0xd9,0x5a,0x76};
const IID IID_Resource = {0x696442be, 0xa72e, 0x4059, 0xbc,0x79, 0x5b,0x5c,0x98,0x04,0x0f,0xad};
const IID IID_Swapchain = {0x94d99bdb, 0xf1f8, 0x4ab0, 0xb2,0x36, 0x7d,0xa0,0x17,0x0e,0xda,0xb1};

typedef HRESULT (WINAPI* PFN_CreateDXGIFactory2)(
    UINT,
    REFIID, 
    void**);

typedef struct {
    const PalGraphicsBackend* backend;

    D3D_FEATURE_LEVEL level;
    ID3D12Device* tmpDevice;
    IDXGIAdapter4* handle;
} Adapter;

typedef struct {
    bool debugLayer;
    Uint32 adapterCount;
    HMODULE handle;
    HMODULE dxgi;
    Adapter* adapters;
    IDXGIFactory6* factory;
    ID3D12Debug* debugController;
    ID3D12Debug1* debugController1;

    PFN_D3D12_CREATE_DEVICE createDevice;
    PFN_CreateDXGIFactory2 createDXGIFactory;
    PFN_D3D12_GET_DEBUG_INTERFACE getDebugInterface;
  
    const PalAllocator* allocator;
} D3D12;

typedef struct {
    const PalGraphicsBackend* backend;

    PalAdapterFeatures features;
    IDXGIAdapter4* adapter;
    ID3D12InfoQueue* infoQueue;
    ID3D12CommandQueue* queue;
    ID3D12Device* handle;
} Device;

typedef struct {
    const PalGraphicsBackend* backend;

    PalQueueType type;
    ID3D12Fence* fence;
    ID3D12CommandQueue* handle;
} Queue;

typedef struct {
    const PalGraphicsBackend* backend;

    HWND handle;
} Surface;

typedef struct {
    const PalGraphicsBackend* backend;

    bool belongsToSwapchain;
    ID3D12Device* device;
    ID3D12Resource* handle;
    PalImageInfo info;
    D3D12_RESOURCE_DESC desc;
} Image;

typedef struct {
    const PalGraphicsBackend* backend;

    PalImageViewType type;
    PalImageViewUsages usages;
    Image* image;
    D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle;
    D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle;
} ImageView;

typedef struct {
    const PalGraphicsBackend* backend;

    D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle;
    D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle;
    D3D12_SAMPLER_DESC desc;
} Sampler;

typedef struct {
    const PalGraphicsBackend* backend;

    Uint32 imageCount;
    Uint32 syncInterval;
    Uint32 windowWidth;
    Uint32 windowHeight;
    Uint32 flags;
    DXGI_FORMAT format;
    DXGI_FEATURE presentFlags;
    Surface* surface;
    Image* images;
    ID3D12CommandQueue* queue;
    IDXGISwapChain3* handle;
} Swapchain;

typedef struct {
    const PalGraphicsBackend* backend;

    bool isTimeline;
    bool signaled;
    UINT64 value; // for timeline semaphores
    ID3D12Fence* handle;
} Fence, Semaphore;

typedef struct {
    const PalGraphicsBackend* backend;

    PalShaderStage stage;
    D3D12_SHADER_BYTECODE byteCode;
} Shader;

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

static Uint32 samplesToD3D12(PalSampleCount count)
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

    if (debugger && debugger->callback) {
        s_D3D12.getDebugInterface = (PFN_D3D12_GET_DEBUG_INTERFACE)GetProcAddress(
            s_D3D12.handle,
            "D3D12GetDebugInterface");

        if (s_D3D12.getDebugInterface) {
            HRESULT hr;
            hr = s_D3D12.getDebugInterface(&IID_Debug, (void**)&s_D3D12.debugController);
            if (SUCCEEDED(hr)) {
                s_D3D12.debugController->lpVtbl->EnableDebugLayer(s_D3D12.debugController);
            }

            hr = s_D3D12.getDebugInterface(&IID_Debug1, (void**)&s_D3D12.debugController1);
            if (SUCCEEDED(hr)) {
                s_D3D12.debugController1->lpVtbl->SetEnableGPUBasedValidation(
                    s_D3D12.debugController1, 
                    TRUE);
            }

            s_D3D12.debugLayer = true;
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
    if (s_D3D12.debugController) {
        s_D3D12.debugController->lpVtbl->Release(s_D3D12.debugController);
    }

    if (s_D3D12.debugController1) {
        s_D3D12.debugController1->lpVtbl->Release(s_D3D12.debugController1);
    }

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

PalResult PAL_CALL enumerateAdaptersD3D12(
    Int32* count,
    PalAdapter** outAdapters)
{
    Uint32 adapterCount = 0;
    IDXGIAdapter* adapter = nullptr;
    IDXGIAdapter4* dxAdapters[32]; // should be more than enough
    ID3D12Device* devices[32]; // should be more than enough
    D3D_FEATURE_LEVEL deviceLevels[32]; // should be more than enough

    D3D_FEATURE_LEVEL levels[] = {
        D3D_FEATURE_LEVEL_12_2,
        D3D_FEATURE_LEVEL_12_1,
        D3D_FEATURE_LEVEL_12_0,
        D3D_FEATURE_LEVEL_11_1,
        D3D_FEATURE_LEVEL_11_0
    };

    if (s_D3D12.adapters) {
        palFree(s_D3D12.allocator, s_D3D12.adapters);
    }

    while (SUCCEEDED(s_D3D12.factory->lpVtbl->EnumAdapters(
        s_D3D12.factory, 
        adapterCount, 
        &adapter))) {
        if (outAdapters) {
            IDXGIAdapter4* tmp = nullptr;
            if SUCCEEDED((adapter->lpVtbl->QueryInterface(adapter, &IID_Adapter, (void**)&tmp))) {
                dxAdapters[adapterCount] = tmp;
            }

            // create a temp device for every adapter to use as an instance to check features, 
            // capabilities etc.
            ID3D12Device* device = nullptr;
            for (int i = 0; i < 5; i++) {
                HRESULT result = s_D3D12.createDevice(
                    (IUnknown*)tmp,
                    levels[i], 
                    &IID_Device, 
                    (void**)&device);
                    
                if (SUCCEEDED(result)) {
                    deviceLevels[adapterCount] = levels[i];
                    devices[adapterCount] = device;
                    break;
                }
            }
        }
        adapter->lpVtbl->Release(adapter);
        adapterCount++;
    }

    if (outAdapters) {
        s_D3D12.adapters = palAllocate(s_D3D12.allocator, sizeof(Adapter) * adapterCount, 0);
        if (!s_D3D12.adapters) {
            return PAL_RESULT_OUT_OF_MEMORY;
        }

        // fill the array with Adapter structs
        for (int i = 0; i < *count; i++) {
            Adapter* tmp = &s_D3D12.adapters[i];
            tmp->handle = dxAdapters[i];
            tmp->tmpDevice = devices[i];
            tmp->level = levels[i];
            outAdapters[i] = (PalAdapter*)tmp;
        }
        s_D3D12.adapterCount = *count;

    } else {
        *count = adapterCount;
    }

    return PAL_RESULT_SUCCESS;
}

PalResult PAL_CALL getAdapterInfoD3D12(
    PalAdapter* adapter,
    PalAdapterInfo* info)
{
    Adapter* d3d12Adapter = (Adapter*)adapter;
    IDXGIAdapter4* adapterHandle = d3d12Adapter->handle;
    DXGI_ADAPTER_DESC3 desc;
    D3D12_FEATURE_DATA_ARCHITECTURE1 arch = {0};
    ID3D12Device* device = d3d12Adapter->tmpDevice;

    HRESULT result = adapterHandle->lpVtbl->GetDesc3(adapterHandle, &desc);
    if (FAILED(result)) {
        return PAL_RESULT_INVALID_ADAPTER;
    }

    info->vendorId = desc.VendorId;
    info->deviceId= desc.DeviceId;
    info->apiType = PAL_ADAPTER_API_TYPE_D3D12;
    info->shaderFormats = PAL_SHADER_FORMAT_DXIL;
    info->sharedMemory = desc.SharedSystemMemory;
    strcpy(info->backendName, "PAL");

    WideCharToMultiByte(
        CP_UTF8, 
        0, 
        desc.Description, 
        -1, 
        info->name, 
        PAL_ADAPTER_NAME_SIZE, 
        nullptr, 
        nullptr);

    device->lpVtbl->CheckFeatureSupport(
        device, 
        D3D12_FEATURE_ARCHITECTURE1, 
        &arch, 
        sizeof(arch));

    if (arch.UMA == true) {
        info->type = PAL_ADAPTER_TYPE_INTEGRATED;

    } else {
        info->type = PAL_ADAPTER_TYPE_DISCRETE;
    }

    if (desc.Flags & DXGI_ADAPTER_FLAG3_SOFTWARE) {
        info->type = PAL_ADAPTER_TYPE_CPU;
    }

    if (desc.DedicatedVideoMemory > 0) {
        info->vram = desc.DedicatedVideoMemory;

    } else if (desc.DedicatedVideoMemory == 0 && desc.DedicatedSystemMemory > 0) {
        info->vram = desc.DedicatedSystemMemory;
    }

    info->version = d3d12Adapter->level;
    if (d3d12Adapter->level == D3D_FEATURE_LEVEL_12_2) {
        strcpy(info->versionString, "12_2");

    } else if (d3d12Adapter->level == D3D_FEATURE_LEVEL_12_1) {
        strcpy(info->versionString, "12_1");

    } else if (d3d12Adapter->level == D3D_FEATURE_LEVEL_12_0) {
        strcpy(info->versionString, "12_0");

    } else if (d3d12Adapter->level == D3D_FEATURE_LEVEL_11_1) {
        strcpy(info->versionString, "11_1");

    } else if (d3d12Adapter->level == D3D_FEATURE_LEVEL_11_0) {
        strcpy(info->versionString, "11_0");
    }

    return PAL_RESULT_SUCCESS;
}

PalResult PAL_CALL getAdapterCapabilitiesD3D12(
    PalAdapter* adapter,
    PalAdapterCapabilities* caps)
{
    caps->maxComputeQueues = PAL_LIMIT_UNKNOWN;
    caps->maxGraphicsQueues = PAL_LIMIT_UNKNOWN;
    caps->maxCopyQueues = PAL_LIMIT_UNKNOWN;

    caps->maxImageWidth = D3D12_REQ_TEXTURE2D_U_OR_V_DIMENSION;
    caps->maxImageHeight = D3D12_REQ_TEXTURE2D_U_OR_V_DIMENSION;
    caps->maxImageDepth = D3D12_REQ_TEXTURE3D_U_V_OR_W_DIMENSION;
    caps->maxImageArrayLayers = D3D12_REQ_TEXTURE2D_ARRAY_AXIS_DIMENSION;

    // d3d12 does not give this but we calculate from the max width and width
    Uint32 a = caps->maxImageWidth;
    Uint32 b = caps->maxImageHeight;
    Uint32 c = caps->maxImageDepth;

    Uint32 tmp = a > b ? a : b;
    Uint32 size = tmp > c ? tmp : c;
    Uint32 levels = 0;
    while (size > 0) {
        // divide by two
        size = size / 2;
        levels++;
    }

    caps->maxImageMipLevels = levels;
    caps->maxColorAttachments = D3D12_SIMULTANEOUS_RENDER_TARGET_COUNT;
    caps->maxViewports = D3D12_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE;
    caps->maxSamplers = D3D12_MAX_SHADER_VISIBLE_SAMPLER_HEAP_SIZE;

    caps->maxMultiViews = D3D12_MAX_VIEW_INSTANCE_COUNT;
    if (caps->maxMultiViews == 0) {
        caps->maxMultiViews = 1;
    }

    caps->maxUniformBufferSize = D3D12_REQ_IMMEDIATE_CONSTANT_BUFFER_ELEMENT_COUNT * 16;
    caps->maxStorageBufferSize = PAL_LIMIT_UNKNOWN;
    caps->maxPushConstantSize = D3D12_MAX_ROOT_COST * 4;

    caps->maxComputeWorkGroupInvocations = D3D12_CS_THREAD_GROUP_MAX_THREADS_PER_GROUP;
    caps->maxComputeWorkGroupCount[0] = D3D12_CS_DISPATCH_MAX_THREAD_GROUPS_PER_DIMENSION;
    caps->maxComputeWorkGroupCount[1] = D3D12_CS_DISPATCH_MAX_THREAD_GROUPS_PER_DIMENSION;
    caps->maxComputeWorkGroupCount[2] = D3D12_CS_DISPATCH_MAX_THREAD_GROUPS_PER_DIMENSION;

    caps->maxComputeWorkGroupSize[0] = D3D12_CS_THREAD_GROUP_MAX_X;
    caps->maxComputeWorkGroupSize[1] = D3D12_CS_THREAD_GROUP_MAX_Y;
    caps->maxComputeWorkGroupSize[2] = D3D12_CS_THREAD_GROUP_MAX_Z;

    return PAL_RESULT_SUCCESS;
}

PalAdapterFeatures PAL_CALL getAdapterFeaturesD3D12(PalAdapter* adapter)
{
    HRESULT result;
    Adapter* d3d12Adapter = (Adapter*)adapter;
    IDXGIAdapter4* adapterHandle = d3d12Adapter->handle;
    PalAdapterFeatures features = 0;
    ID3D12Device* device = d3d12Adapter->tmpDevice;

    D3D12_FEATURE_DATA_D3D12_OPTIONS options = {0};
    D3D12_FEATURE_DATA_D3D12_OPTIONS3 options3 = {0};
    D3D12_FEATURE_DATA_D3D12_OPTIONS5 options5 = {0};
    D3D12_FEATURE_DATA_D3D12_OPTIONS6 options6 = {0};
    D3D12_FEATURE_DATA_D3D12_OPTIONS7 options7 = {0};
    D3D12_FEATURE_DATA_SHADER_MODEL shaderModel = {0};

    device->lpVtbl->CheckFeatureSupport(
        device, 
        D3D12_FEATURE_D3D12_OPTIONS, 
        &options, 
        sizeof(options));

    device->lpVtbl->CheckFeatureSupport(
        device, 
        D3D12_FEATURE_D3D12_OPTIONS3, 
        &options3, 
        sizeof(options3));

    device->lpVtbl->CheckFeatureSupport(
        device, 
        D3D12_FEATURE_D3D12_OPTIONS5, 
        &options5, 
        sizeof(options5));

    device->lpVtbl->CheckFeatureSupport(
        device, 
        D3D12_FEATURE_D3D12_OPTIONS6, 
        &options6, 
        sizeof(options6));

    device->lpVtbl->CheckFeatureSupport(
        device, 
        D3D12_FEATURE_D3D12_OPTIONS7, 
        &options7, 
        sizeof(options7));

    shaderModel.HighestShaderModel = D3D_SHADER_MODEL_5_1;
    HRESULT hr = device->lpVtbl->CheckFeatureSupport(
        device, 
        D3D12_FEATURE_SHADER_MODEL, 
        &shaderModel, 
        sizeof(shaderModel));

    if (shaderModel.HighestShaderModel >= D3D_SHADER_MODEL_5_1 && hr == S_OK) {
        features |= PAL_ADAPTER_FEATURE_TESSELLATION_SHADER;
        features |= PAL_ADAPTER_FEATURE_SAMPLE_RATE_SHADING;
        features |= PAL_ADAPTER_FEATURE_SHADER_FLOAT64;
    }

    shaderModel.HighestShaderModel = D3D_SHADER_MODEL_6_2;
    hr = device->lpVtbl->CheckFeatureSupport(
        device, 
        D3D12_FEATURE_SHADER_MODEL, 
        &shaderModel, 
        sizeof(shaderModel));

    if (shaderModel.HighestShaderModel >= D3D_SHADER_MODEL_6_2 && hr == S_OK) {
        features |= PAL_ADAPTER_FEATURE_SHADER_FLOAT16;
        features |= PAL_ADAPTER_FEATURE_SHADER_INT16;
    }

    shaderModel.HighestShaderModel = D3D_SHADER_MODEL_6_0;
    hr = device->lpVtbl->CheckFeatureSupport(
        device, 
        D3D12_FEATURE_SHADER_MODEL, 
        &shaderModel, 
        sizeof(shaderModel));

    if (shaderModel.HighestShaderModel >= D3D_SHADER_MODEL_6_0 && hr == S_OK) {
        features |= PAL_ADAPTER_FEATURE_SHADER_INT64;
    }

    if (options3.ViewInstancingTier != D3D12_VIEW_INSTANCING_TIER_NOT_SUPPORTED) {
        features |= PAL_ADAPTER_FEATURE_MULTI_VIEW;
    }

    if (options5.RaytracingTier != D3D12_RAYTRACING_TIER_NOT_SUPPORTED) {
        features |= PAL_ADAPTER_FEATURE_RAY_TRACING;
    }

    if (options6.VariableShadingRateTier != D3D12_VARIABLE_SHADING_RATE_TIER_NOT_SUPPORTED) {
        features |= PAL_ADAPTER_FEATURE_FRAGMENT_SHADING_RATE;
        features |= PAL_ADAPTER_FEATURE_FRAGMENT_SHADING_RATE_ATTACHMENT;
    }

    if (options7.MeshShaderTier != D3D12_MESH_SHADER_TIER_NOT_SUPPORTED) {
        features |= PAL_ADAPTER_FEATURE_MESH_SHADER;
    }

    if (options.ResourceBindingTier == D3D12_RESOURCE_BINDING_TIER_3) {
        features |= PAL_ADAPTER_FEATURE_DESCRIPTOR_INDEXING;
    }

    // this features are supported on d3d12
    features |= PAL_ADAPTER_FEATURE_SAMPLER_ANISOTROPY;
    features |= PAL_ADAPTER_FEATURE_COMPUTE_SHADER;
    features |= PAL_ADAPTER_FEATURE_TIMELINE_SEMAPHORE;
    features |= PAL_ADAPTER_FEATURE_MULTI_VIEWPORT;
    features |= PAL_ADAPTER_FEATURE_GEOMETRY_SHADER;
    features |= PAL_ADAPTER_FEATURE_SWAPCHAIN;
    features |= PAL_ADAPTER_FEATURE_FENCE_RESET;
    features |= PAL_ADAPTER_FEATURE_POLYGON_MODE_LINE;
    features |= PAL_ADAPTER_FEATURE_DYNAMIC_PRIMITIVE_TOPOLOGY;
    features |= PAL_ADAPTER_FEATURE_BUFFER_DEVICE_ADDRESS;
    features |= PAL_ADAPTER_FEATURE_INDIRECT_DRAW;

    if (d3d12Adapter->level >= D3D_FEATURE_LEVEL_12_0) {
        features |= PAL_ADAPTER_FEATURE_DEPTH_STENCIL_RESOLVE;
    }
    return features;
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
    Adapter* d3d12Adapter = (Adapter*)adapter;

    device = palAllocate(s_D3D12.allocator, sizeof(Device), 0);
    if (!device) {
        return PAL_RESULT_OUT_OF_MEMORY;
    }

    memset(device, 0, sizeof(Device));
    result = s_D3D12.createDevice(
        (IUnknown*)d3d12Adapter->handle, 
        d3d12Adapter->level, 
        &IID_Device, 
        (void**)&device->handle);
        
    if (FAILED(result)) {
        palFree(s_D3D12.allocator, device);
        if (result == E_OUTOFMEMORY) {
            return PAL_RESULT_OUT_OF_MEMORY;
        }
        return PAL_RESULT_INVALID_DRIVER;
    }

    if (s_D3D12.debugLayer) {
        result = device->handle->lpVtbl->QueryInterface(
            device->handle, 
            &IID_InfoQueue, 
            (void**)&device->infoQueue);

        if (SUCCEEDED(result)) {
            D3D12_MESSAGE_SEVERITY severities[] = {
                D3D12_MESSAGE_SEVERITY_WARNING,
                D3D12_MESSAGE_SEVERITY_ERROR,
                D3D12_MESSAGE_SEVERITY_CORRUPTION
            };

            D3D12_MESSAGE_ID denyIDs[] = { D3D12_MESSAGE_ID_MAP_INVALID_NULLRANGE };

            D3D12_INFO_QUEUE_FILTER filter = {0};
            filter.AllowList.NumSeverities = 3;
            filter.AllowList.pSeverityList = severities;
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
        } else {
            return PAL_RESULT_PLATFORM_FAILURE;
        }
    }

    device->adapter = d3d12Adapter->handle;
    device->features = features;
    *outDevice = (PalDevice*)device;
    return PAL_RESULT_SUCCESS;
}

void PAL_CALL destroyDeviceD3D12(PalDevice* device)
{
    Device* d3d12Device = (Device*)device;
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
    Uint64 memoryMask,
    Uint64 size,
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

PalResult PAL_CALL queryDepthStencilCapabilitiesD3D12(
    PalDevice* device,
    PalDepthStencilCapabilities* caps)
{
    Device* d3d12Device = (Device*)device;
    if (!(d3d12Device->features & PAL_ADAPTER_FEATURE_SAMPLER_ANISOTROPY)) {
        return PAL_RESULT_ADAPTER_FEATURE_NOT_SUPPORTED;
    }

    caps->depthResolveModes[PAL_RESOLVE_MODE_SAMPLE_ZERO] = true;
    caps->depthResolveModes[PAL_RESOLVE_MODE_AVERAGE] = true;
    caps->depthResolveModes[PAL_RESOLVE_MODE_MIN] = true;
    caps->depthResolveModes[PAL_RESOLVE_MODE_MAX] = true;

    caps->stencilResolveModes[PAL_RESOLVE_MODE_SAMPLE_ZERO] = true;
    caps->stencilResolveModes[PAL_RESOLVE_MODE_AVERAGE] = false;
    caps->stencilResolveModes[PAL_RESOLVE_MODE_MIN] = true;
    caps->stencilResolveModes[PAL_RESOLVE_MODE_MAX] = true;

    caps->independentDepthStencilResolve = true;
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
    caps->shadingRates[PAL_FRAGMENT_SHADING_RATE_1X1] = true;
    caps->shadingRates[PAL_FRAGMENT_SHADING_RATE_1X2] = true;
    caps->shadingRates[PAL_FRAGMENT_SHADING_RATE_2X1] = true;
    caps->shadingRates[PAL_FRAGMENT_SHADING_RATE_2X2] = true;

    D3D12_FEATURE_DATA_D3D12_OPTIONS6 options = {0};
    d3d12Device->handle->lpVtbl->CheckFeatureSupport(
        d3d12Device->handle, 
        D3D12_FEATURE_D3D12_OPTIONS6, 
        &options, 
        sizeof(options));

    if (options.AdditionalShadingRatesSupported) {
        caps->shadingRates[PAL_FRAGMENT_SHADING_RATE_2X4] = true;
        caps->shadingRates[PAL_FRAGMENT_SHADING_RATE_4X2] = true;
        caps->shadingRates[PAL_FRAGMENT_SHADING_RATE_4X4] = true;

    } else {
        caps->shadingRates[PAL_FRAGMENT_SHADING_RATE_2X4] = false;
        caps->shadingRates[PAL_FRAGMENT_SHADING_RATE_4X2] = false;
        caps->shadingRates[PAL_FRAGMENT_SHADING_RATE_4X4] = false;
    }

    // there are supported if fragment shading rate feature is
    caps->combinerOps[PAL_FRAGMENT_SHADING_RATE_COMBINER_OP_KEEP] = true;
    caps->combinerOps[PAL_FRAGMENT_SHADING_RATE_COMBINER_OP_REPLACE] = true;
    caps->combinerOps[PAL_FRAGMENT_SHADING_RATE_COMBINER_OP_MIN] = true;
    caps->combinerOps[PAL_FRAGMENT_SHADING_RATE_COMBINER_OP_MAX] = true;
    caps->combinerOps[PAL_FRAGMENT_SHADING_RATE_COMBINER_OP_MUL] = true;

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
    caps->maxMeshOutputPrimitives = 256; 
    caps->maxMeshOutputVertices = 256;
    caps->maxTaskWorkGroupInvocations = 128;
    caps->maxMeshWorkGroupInvocations = 128;

    caps->maxTaskWorkGroupCount[0] = 65535;
    caps->maxTaskWorkGroupCount[1] = 65535;
    caps->maxTaskWorkGroupCount[2] = 65535;

    caps->maxMeshWorkGroupCount[0] = 65535;
    caps->maxMeshWorkGroupCount[1] = 65535;
    caps->maxMeshWorkGroupCount[2] = 65535;

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

    // these are only limited by memory. D3d12 does not expose them
    caps->maxRecursionDepth = 32; // 32 - 1;
    caps->maxHitAttributeSize = PAL_LIMIT_UNKNOWN;
    caps->maxInstanceCount = PAL_LIMIT_UNKNOWN;
    caps->maxPrimitiveCount = PAL_LIMIT_UNKNOWN;
    caps->maxGeometryCount = PAL_LIMIT_UNKNOWN;
    caps->maxPayloadSize = PAL_LIMIT_UNKNOWN;
    caps->maxDispatchInvocations = PAL_LIMIT_UNKNOWN;

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

    // these are supported if descriptor indexing is
    caps->bindlessStorageBuffers = true;
    caps->bindlessUniformBuffers = true;
    caps->bindlessSamplers = true;

    // these are not exposed by d3d12. We use the offical resource binding spec
    caps->maxImagesPerShaderStage = PAL_LIMIT_UNKNOWN;
    caps->maxImagesPerDescriptorSet = PAL_LIMIT_UNKNOWN;
    caps->maxSamplersPerShaderStage = 2048;
    caps->maxSamplersPerDescriptorSet = PAL_LIMIT_UNKNOWN;
    caps->maxStorageBuffersPerShaderStage = PAL_LIMIT_UNKNOWN;
    caps->maxUniformBuffersPerShaderStage = PAL_LIMIT_UNKNOWN;
    caps->maxStorageBuffersPerDescriptorSet = PAL_LIMIT_UNKNOWN;
    caps->maxUniformBuffersPerDescriptorSet = PAL_LIMIT_UNKNOWN;
    caps->maxDescriptors = PAL_LIMIT_UNKNOWN;

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
            break;
        }

        case PAL_QUEUE_TYPE_GRAPHICS: {
            desc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
            break;
        }

        case PAL_QUEUE_TYPE_COPY: {
            desc.Type = D3D12_COMMAND_LIST_TYPE_COPY;
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
        palFree(s_D3D12.allocator, queue);
        if (result == E_OUTOFMEMORY) {
            return PAL_RESULT_OUT_OF_MEMORY;
        } else {
            return PAL_RESULT_PLATFORM_FAILURE;
        }
    }

    // create fence used for queue wait
    result = d3d12Device->handle->lpVtbl->CreateFence(
        d3d12Device->handle, 
        0, 
        0, 
        &IID_Fence,
        (void**)&queue->fence);

    if (FAILED(result)) {
        palFree(s_D3D12.allocator, queue);
        if (result == E_OUTOFMEMORY) {
            return PAL_RESULT_OUT_OF_MEMORY;
        } else {
            return PAL_RESULT_PLATFORM_FAILURE;
        }
    }

    queue->type = type;
    *outQueue = (PalQueue*)queue;
    return PAL_RESULT_SUCCESS;
}

void PAL_CALL destroyQueueD3D12(PalQueue* queue)
{
    Queue* d3d12Queue = (Queue*)queue;
    d3d12Queue->fence->lpVtbl->Release(d3d12Queue->fence);
    d3d12Queue->handle->lpVtbl->Release(d3d12Queue->handle);
    palFree(s_D3D12.allocator, d3d12Queue);
}

PalResult PAL_CALL waitQueueD3D12(PalQueue* queue)
{
    Queue* d3d12Queue = (Queue*)queue;
    ID3D12Fence* fence = d3d12Queue->fence;
    fence->lpVtbl->Signal(fence, 1);

    // wait on the fence if the submited work is not done
    if (fence->lpVtbl->GetCompletedValue(fence) < 1) {
        HANDLE event = CreateEvent(nullptr, FALSE, FALSE, nullptr);
        fence->lpVtbl->SetEventOnCompletion(fence, 1, event);
        WaitForSingleObject(event, INFINITE);
        CloseHandle(event);
    }

    return PAL_RESULT_SUCCESS;
}

bool PAL_CALL canQueuePresentD3D12(
    PalQueue* queue,
    PalSurface* surface)
{
    Queue* d3d12Queue = (Queue*)queue;
    if (d3d12Queue->type == PAL_QUEUE_TYPE_GRAPHICS) {
        return true; // all graphics queues support presentation
    }
    return false;
}

// ==================================================
// Formats
// ==================================================

PalResult PAL_CALL enumerateFormatsD3D12(
    PalAdapter* adapter,
    Int32* count,
    PalFormatInfo* outFormats)
{
    Int32 fmtCount = 0;
    HRESULT result;
    Adapter* d3d12Adapter = (Adapter*)adapter;
    ID3D12Device* device = d3d12Adapter->tmpDevice;
    D3D12_FEATURE_DATA_FORMAT_SUPPORT support = {0};

    for (int i = 0; i < PAL_FORMAT_MAX; i++) {
        DXGI_FORMAT fmt = formatToD3D12((PalFormat)i);
        if (fmt == DXGI_FORMAT_UNKNOWN) {
            continue;
        }

        support.Format = fmt;
        result = device->lpVtbl->CheckFeatureSupport(
            device, 
            D3D12_FEATURE_FORMAT_SUPPORT, 
            &support, 
            sizeof(support));

        if (SUCCEEDED(result)) {
            if (support.Support1 == 0 && support.Support2 == 0) {
                // format not supported
                continue;
            }

            if (outFormats) {
                if (fmtCount < *count) {
                    PalFormatInfo* fmtInfo = &outFormats[fmtCount++];
                    fmtInfo->format = (PalFormat)i;
                    fmtInfo->usages = ImageUsageFromD3D12(support.Support1);

                    if (support.Support2 & D3D12_FORMAT_SUPPORT2_UAV_TYPED_STORE ||
                        support.Support2 & D3D12_FORMAT_SUPPORT2_UAV_TYPED_LOAD) {
                        fmtInfo->usages |= PAL_IMAGE_USAGE_STORAGE;
                    }

                    fmtInfo->viewUsages = 0;
                    if (fmtInfo->usages & PAL_IMAGE_USAGE_COLOR_ATTACHEMENT) {
                        fmtInfo->viewUsages |= PAL_IMAGE_VIEW_USAGE_COLOR;
                        if (i == PAL_FORMAT_R8_UINT) {
                            fmtInfo->viewUsages |= PAL_IMAGE_VIEW_USAGE_FRAGMENT_SHADING_RATE;
                        }
                    }

                    if (fmtInfo->usages & PAL_IMAGE_USAGE_DEPTH_ATTACHEMENT) {
                        if (i == PAL_FORMAT_S8_UINT) {
                            fmtInfo->viewUsages |= PAL_IMAGE_VIEW_USAGE_STENCIL;

                        } else if (i == PAL_FORMAT_D16_UNORM || i == PAL_FORMAT_D32_SFLOAT) {
                            fmtInfo->viewUsages |= PAL_IMAGE_VIEW_USAGE_DEPTH;

                        } else {
                            fmtInfo->viewUsages |= PAL_IMAGE_VIEW_USAGE_DEPTH;
                            fmtInfo->viewUsages |= PAL_IMAGE_VIEW_USAGE_STENCIL;
                        }
                    }
                }

            } else {
                fmtCount++;
            }
        }
    }
    if (!outFormats) {
        *count = fmtCount;
    }
    return PAL_RESULT_SUCCESS;
}

bool PAL_CALL isFormatSupportedD3D12(
    PalAdapter* adapter,
    PalFormat format)
{
    HRESULT result;
    Adapter* d3d12Adapter = (Adapter*)adapter;
    ID3D12Device* device = d3d12Adapter->tmpDevice;
    D3D12_FEATURE_DATA_FORMAT_SUPPORT support = {0};

    DXGI_FORMAT fmt = formatToD3D12(format);
    if (fmt == DXGI_FORMAT_UNKNOWN) {
        return false;
    }

    support.Format = fmt;
    result = device->lpVtbl->CheckFeatureSupport(
        device, 
        D3D12_FEATURE_FORMAT_SUPPORT, 
        &support, 
        sizeof(support));

    if (FAILED(result) || (support.Support1 == 0 && support.Support2 == 0)) {
        return false;
    }

    return true;
}

PalImageUsages PAL_CALL queryFormatImageUsagesD3D12(
    PalAdapter* adapter,
    PalFormat format)
{
    HRESULT result;
    Adapter* d3d12Adapter = (Adapter*)adapter;
    ID3D12Device* device = d3d12Adapter->tmpDevice;
    D3D12_FEATURE_DATA_FORMAT_SUPPORT support = {0};

    DXGI_FORMAT fmt = formatToD3D12(format);
    if (fmt == DXGI_FORMAT_UNKNOWN) {
        return 0;
    }

    support.Format = fmt;
    result = device->lpVtbl->CheckFeatureSupport(
        device, 
        D3D12_FEATURE_FORMAT_SUPPORT, 
        &support, 
        sizeof(support));

    if (FAILED(result)) {
        return 0;
    }

    if (support.Support1 == 0 && support.Support2 == 0) {
        // format not supported
        return 0;
    }

    PalImageUsages usages = ImageUsageFromD3D12(support.Support1);
    if (support.Support2 & D3D12_FORMAT_SUPPORT2_UAV_TYPED_STORE ||
        support.Support2 & D3D12_FORMAT_SUPPORT2_UAV_TYPED_LOAD) {
        usages |= PAL_IMAGE_USAGE_STORAGE;
    }

    return usages;
}

PalImageViewUsages PAL_CALL queryFormatImageViewUsagesD3D12(
    PalAdapter* adapter,
    PalFormat format)
{
    HRESULT result;
    Adapter* d3d12Adapter = (Adapter*)adapter;
    ID3D12Device* device = d3d12Adapter->tmpDevice;
    D3D12_FEATURE_DATA_FORMAT_SUPPORT support = {0};

    DXGI_FORMAT fmt = formatToD3D12(format);
    if (fmt == DXGI_FORMAT_UNKNOWN) {
        return 0;
    }

    support.Format = fmt;
    result = device->lpVtbl->CheckFeatureSupport(
        device, 
        D3D12_FEATURE_FORMAT_SUPPORT, 
        &support, 
        sizeof(support));

    if (FAILED(result)) {
        return 0;
    }

    if (support.Support1 == 0 && support.Support2 == 0) {
        // format not supported
        return 0;
    }

    PalImageUsages imageUsages = ImageUsageFromD3D12(support.Support1);
    if (support.Support2 & D3D12_FORMAT_SUPPORT2_UAV_TYPED_STORE ||
        support.Support2 & D3D12_FORMAT_SUPPORT2_UAV_TYPED_LOAD) {
        imageUsages |= PAL_IMAGE_USAGE_STORAGE;
    }

    PalImageViewUsages usages = 0;
    if (imageUsages & PAL_IMAGE_USAGE_COLOR_ATTACHEMENT) {
        usages |= PAL_IMAGE_VIEW_USAGE_COLOR;
        if (format == PAL_FORMAT_R8_UINT) {
            usages |= PAL_IMAGE_VIEW_USAGE_FRAGMENT_SHADING_RATE;
        }
    }

    if (imageUsages & PAL_IMAGE_USAGE_DEPTH_ATTACHEMENT) {
        if (format == PAL_FORMAT_D16_UNORM || format == PAL_FORMAT_D32_SFLOAT) {
            usages |= PAL_IMAGE_VIEW_USAGE_DEPTH;

        } else {
            usages |= PAL_IMAGE_VIEW_USAGE_DEPTH;
            usages |= PAL_IMAGE_VIEW_USAGE_STENCIL;
        }
    }
    return usages;
}

PalSampleCount PAL_CALL queryFormatSampleCountD3D12(
    PalAdapter* adapter,
    PalFormat format)
{
    HRESULT result;
    Adapter* d3d12Adapter = (Adapter*)adapter;
    ID3D12Device* device = d3d12Adapter->tmpDevice;
    D3D12_FEATURE_DATA_FORMAT_SUPPORT support = {0};

    DXGI_FORMAT fmt = formatToD3D12(format);
    if (fmt == DXGI_FORMAT_UNKNOWN) {
        return false;
    }

    support.Format = fmt;
    result = device->lpVtbl->CheckFeatureSupport(
        device, 
        D3D12_FEATURE_FORMAT_SUPPORT, 
        &support, 
        sizeof(support));

    if (FAILED(result)) {
        return false;
    }

    if (support.Support1 == 0 && support.Support2 == 0) {
        // format not supported
        return false;
    }

    // check sample count
    UINT sampleCounts[] = {64, 32, 16, 8, 4, 2};
    D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS samples = {0};
    samples.Format = fmt;

    Uint32 tmp = 0;
    for (int i = 0; i < 6; i++) {
        samples.SampleCount = sampleCounts[i];
        result = device->lpVtbl->CheckFeatureSupport(
            device, 
            D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS, 
            &samples, 
            sizeof(samples));

        if (SUCCEEDED(result) && samples.NumQualityLevels > 0) {
            tmp = samples.SampleCount;
            break;
        }
    }

    if (tmp == 64) {
        return PAL_SAMPLE_COUNT_64;
    } else if (tmp == 32) {
        return PAL_SAMPLE_COUNT_32;
    } else if (tmp == 16) {
        return PAL_SAMPLE_COUNT_16;
    } else if (tmp == 8) {
        return PAL_SAMPLE_COUNT_8;
    } else if (tmp == 4) {
        return PAL_SAMPLE_COUNT_4;
    } else if (tmp == 2) {
        return PAL_SAMPLE_COUNT_2;
    } else {
        return PAL_SAMPLE_COUNT_1;
    }
}

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

    image->belongsToSwapchain = false;
    image->info.depthOrArraySize = info->depthOrArraySize;
    image->info.type = info->type;
    image->info.format = info->format;
    image->info.usages = info->usages;
    image->info.height = info->height;
    image->info.mipLevelCount = info->mipLevelCount;
    image->info.sampleCount = info->sampleCount;
    image->info.width = info->width;

    image->device = d3d12Device->handle;
    *outImage = (PalImage*)image;
    return PAL_RESULT_SUCCESS;
}

void PAL_CALL destroyImageD3D12(PalImage* image)
{
    Image* d3d12Image = (Image*)image;
    // check if memory has been attached to the image
    if (d3d12Image->handle) {
        d3d12Image->handle->lpVtbl->Release(d3d12Image->handle);
    }
    palFree(s_D3D12.allocator, d3d12Image);
}

PalResult PAL_CALL getImageInfoD3D12(
    PalImage* image,
    PalImageInfo* info)
{
    Image* d3d12Image = (Image*)image;
    *info = d3d12Image->info;
    return PAL_RESULT_SUCCESS;
}

PalResult PAL_CALL getImageMemoryRequirementsD3D12(
    PalImage* image,
    PalMemoryRequirements* requirements)
{
    Image* d3d12Image = (Image*)image;
    if (d3d12Image->belongsToSwapchain) {
        return PAL_RESULT_INVALID_OPERATION;
    }

    D3D12_RESOURCE_ALLOCATION_INFO allocationInfo = {0};
    D3D12_RESOURCE_ALLOCATION_INFO __ret = {0};
    allocationInfo = *d3d12Image->device->lpVtbl->GetResourceAllocationInfo(
        d3d12Image->device,
        &__ret,
        0,
        1,
        &d3d12Image->desc);

    // d3d12 allows images to be used with only GPU only heap
    requirements->memoryTypes[PAL_MEMORY_TYPE_GPU_ONLY] = true;
    requirements->memoryTypes[PAL_MEMORY_TYPE_CPU_UPLOAD] = false;
    requirements->memoryTypes[PAL_MEMORY_TYPE_CPU_READBACK] = false;

    requirements->alignment = allocationInfo.Alignment;
    requirements->size = allocationInfo.SizeInBytes;
    return PAL_RESULT_SUCCESS;
}

PalResult PAL_CALL bindImageMemoryD3D12(
    PalImage* image,
    PalMemory* memory,
    Uint64 offset)
{
    HRESULT result;
    Image* d3d12Image = (Image*)image;
    if (d3d12Image->belongsToSwapchain) {
        return PAL_RESULT_INVALID_OPERATION;
    }

    ID3D12Heap* mem = (ID3D12Heap*)memory;
    result = d3d12Image->device->lpVtbl->CreatePlacedResource(
        d3d12Image->device, 
        mem, 
        offset, 
        &d3d12Image->desc, 
        0,
        nullptr, 
        &IID_Resource, 
        (void**)d3d12Image->handle);

    if (FAILED(result)) {
        if (result == E_OUTOFMEMORY) {
            return PAL_RESULT_OUT_OF_MEMORY;

        } else if (result == E_INVALIDARG) {
            return PAL_RESULT_INVALID_ARGUMENT;

        } else {
            return PAL_RESULT_PLATFORM_FAILURE;
        }
    }

    return PAL_RESULT_SUCCESS;
}

PalResult PAL_CALL mapImageMemoryD3D12(
    PalImage* image,
    Uint64 offset,
    Uint64 size,
    void** outPtr)
{
    return PAL_RESULT_MEMORY_MAP_FAILED;
}

void PAL_CALL unmapImageMemoryD3D12(PalImage* image)
{
    // do nothing.
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
    Image* d3d12Image = (Image*)image;

    if (info->type == PAL_IMAGE_VIEW_TYPE_CUBE_ARRAY) {
        if (!(d3d12Device->features & PAL_ADAPTER_FEATURE_IMAGE_VIEW_CUBE_ARRAY)) {
            return PAL_RESULT_ADAPTER_FEATURE_NOT_SUPPORTED;
        }
    }

    imageView = palAllocate(s_D3D12.allocator, sizeof(ImageView), 0);
    if (!imageView) {
        return PAL_RESULT_OUT_OF_MEMORY;
    }

    memset(imageView, 0, sizeof(ImageView));
    imageView->type = info->type;
    imageView->usages = info->usages;
    imageView->image = d3d12Image;

    *outImageView = (PalImageView*)imageView;
    return PAL_RESULT_SUCCESS;
}

void PAL_CALL destroyImageViewD3D12(PalImageView* imageView)
{
    ImageView* d3d12ImageView = (ImageView*)imageView;
    palFree(s_D3D12.allocator, d3d12ImageView);
}

// ==================================================
// Sampler
// ==================================================

PalResult PAL_CALL createSamplerD3D12(
    PalDevice* device,
    const PalSamplerCreateInfo* info,
    PalSampler** outSampler)
{
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
        sampler->desc.MaxAnisotropy = info->maxAnisotropy;
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
    Sampler* d3d12Sampler = (Sampler*)sampler;
    palFree(s_D3D12.allocator, d3d12Sampler);
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
    Surface* d3d12Surface = (Surface*)surface;
    palFree(s_D3D12.allocator, d3d12Surface);
}

PalResult PAL_CALL getSurfaceCapabilitiesD3D12(
    PalDevice* device,
    PalSurface* surface,
    PalSurfaceCapabilities* caps)
{
    HRESULT result;
    Surface* d3d12Surface = (Surface*)surface;
    Device* d3d12Device = (Device*)device;
    bool supportHDR10 = false;
    IDXGISwapChain1* swapchain1 = nullptr;
    IDXGISwapChain3* swapchain3 = nullptr;

    if (!(d3d12Device->features & PAL_ADAPTER_FEATURE_SWAPCHAIN)) {
        return PAL_RESULT_ADAPTER_FEATURE_NOT_SUPPORTED;
    }

    DXGI_SWAP_CHAIN_DESC1 desc = {0};
    desc.Width = 1;
    desc.Height = 1;
    desc.SampleDesc.Count = 1;
    desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    desc.BufferCount = 1;
    desc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;

    result = s_D3D12.factory->lpVtbl->CreateSwapChainForHwnd(
        s_D3D12.factory,
        (IUnknown*)d3d12Device->queue,
        d3d12Surface->handle,
        &desc,
        nullptr,
        nullptr,
        &swapchain1);

    if (FAILED(result)) {
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
        supportHDR10 = true;
    }

    BOOL allowTearing = FALSE;
    s_D3D12.factory->lpVtbl->CheckFeatureSupport(
        s_D3D12.factory, 
        DXGI_FEATURE_PRESENT_ALLOW_TEARING,
        &allowTearing,
        sizeof(allowTearing));

    caps->presentModes[PAL_PRESENT_MODE_FIFO] = true;
    if (allowTearing) {
        caps->minImageCount = 3;
        caps->presentModes[PAL_PRESENT_MODE_IMMEDIATE] = true;
        caps->presentModes[PAL_PRESENT_MODE_MAILBOX] = true;

    } else {
        caps->minImageCount = 2;
        caps->presentModes[PAL_PRESENT_MODE_IMMEDIATE] = false;
        caps->presentModes[PAL_PRESENT_MODE_MAILBOX] = false;
    }

    // FIXME: Add support for pre and post multplied composite alpha
    caps->compositeAlphas[PAL_COMPOSITE_ALPHA_OPAQUE] = true;
    caps->compositeAlphas[PAL_COMPOSITE_ALPHA_PRE_MULTIPLIED] = false;
    caps->compositeAlphas[PAL_COMPOSITE_ALPHA_POST_MULTIPLIED] = false;

    caps->maxImageCount = PAL_LIMIT_UNKNOWN;
    caps->minImageWidth = 1;
    caps->minImageHeight = 1;
    caps->maxImageWidth = PAL_LIMIT_UNKNOWN;
    caps->maxImageHeight = PAL_LIMIT_UNKNOWN;
    caps->maxImageArrayLayers = 1;

    // check support for the base format
    D3D12_FEATURE_DATA_FORMAT_SUPPORT formatSupport = {0};
    DXGI_FORMAT baseFormats[PAL_SURFACE_FORMAT_MAX];
    baseFormats[0] = DXGI_FORMAT_B8G8R8A8_UNORM;
    baseFormats[1] = DXGI_FORMAT_B8G8R8A8_UNORM_SRGB;
    baseFormats[2] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
    baseFormats[3] = DXGI_FORMAT_R16G16B16A16_FLOAT;

    caps->formats[PAL_SURFACE_FORMAT_BGRA8_UNORM_SRGB_NONLINEAR] = false;
    caps->formats[PAL_SURFACE_FORMAT_BGRA8_SRGB_NONLINEAR] = false;
    caps->formats[PAL_SURFACE_FORMAT_RGBA8_UNORM_SRGB_NONLINEAR] = false;
    caps->formats[PAL_SURFACE_FORMAT_RGBA16_FLOAT_HDR10] = false;

    for (int i = 0; i < PAL_SURFACE_FORMAT_MAX; i++) {
        formatSupport.Format = baseFormats[i];
        result = d3d12Device->handle->lpVtbl->CheckFeatureSupport(
            d3d12Device->handle, 
            D3D12_FEATURE_FORMAT_SUPPORT, 
            &formatSupport, 
            sizeof(formatSupport));

        if (SUCCEEDED(result) && (formatSupport.Support1 != 0 || formatSupport.Support2 != 0)) {
            if (baseFormats[i] == DXGI_FORMAT_B8G8R8A8_UNORM) {
                caps->formats[PAL_SURFACE_FORMAT_BGRA8_UNORM_SRGB_NONLINEAR] = true;
            }

            if (baseFormats[i] == DXGI_FORMAT_B8G8R8A8_UNORM_SRGB) {
                caps->formats[PAL_SURFACE_FORMAT_BGRA8_SRGB_NONLINEAR] = true;
            }

            if (baseFormats[i] == DXGI_FORMAT_R8G8B8A8_UNORM_SRGB) {
                caps->formats[PAL_SURFACE_FORMAT_RGBA8_UNORM_SRGB_NONLINEAR] = true;
            }

            if (baseFormats[i] == DXGI_FORMAT_R16G16B16A16_FLOAT) {
                // check HDR10 color space
                if (supportHDR10) {
                    caps->formats[PAL_SURFACE_FORMAT_RGBA16_FLOAT_HDR10] = true;
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
    Surface* d3d12Surface = (Surface*)surface;
    Device* d3d12Device = (Device*)device;
    Queue* d3d12Queue = (Queue*)queue;
    Swapchain* swapchain = nullptr;
    bool isHDRColorspace = false;

    if (!(d3d12Device->features & PAL_ADAPTER_FEATURE_SWAPCHAIN)) {
        return PAL_RESULT_ADAPTER_FEATURE_NOT_SUPPORTED;
    }

    if (d3d12Queue->type != PAL_QUEUE_TYPE_GRAPHICS) {
        return PAL_RESULT_INVALID_QUEUE;
    }

    // FIXME: Add support for pre and post multplied composite alpha
    if (info->compositeAlpha != PAL_COMPOSITE_ALPHA_OPAQUE) {
        return PAL_RESULT_INVALID_ARGUMENT;
    }

    if (info->presentMode == PAL_PRESENT_MODE_MAILBOX && info->imageCount < 3) {
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
    desc.Stereo = false;
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
        isHDRColorspace = true;
    }

    swapchain->format = desc.Format;
    swapchain->flags = desc.Flags;
    result = s_D3D12.factory->lpVtbl->CreateSwapChainForHwnd(
        s_D3D12.factory,
        (IUnknown*)d3d12Queue->handle,
        d3d12Surface->handle,
        &desc,
        nullptr,
        nullptr,
        &swapchain1);

    if (FAILED(result)) {
        if (result == E_OUTOFMEMORY) {
            return PAL_RESULT_OUT_OF_MEMORY;
        } else if (result == E_INVALIDARG) {
            return PAL_RESULT_INVALID_ARGUMENT;
        } else {
            return PAL_RESULT_PLATFORM_FAILURE;
        }
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
        image->belongsToSwapchain = true;
        image->device = d3d12Device->handle;
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
    GetClientRect((HWND)d3d12Surface->handle, &windowRect);
    swapchain->windowWidth = windowRect.right - windowRect.left;
    swapchain->windowWidth = windowRect.bottom - windowRect.top;

    swapchain->queue = d3d12Queue->handle;
    swapchain->imageCount = info->imageCount;
    *outSwapchain = (PalSwapchain*)swapchain;
    return PAL_RESULT_SUCCESS;
}

void PAL_CALL destroySwapchainD3D12(PalSwapchain* swapchain)
{
    Swapchain* d3d12Swapchain = (Swapchain*)swapchain;
    d3d12Swapchain->handle->lpVtbl->Release(d3d12Swapchain->handle);
    palFree(s_D3D12.allocator, d3d12Swapchain->images);
    palFree(s_D3D12.allocator, d3d12Swapchain);
}

PalImage* PAL_CALL getSwapchainImageD3D12(
    PalSwapchain* swapchain,
    Int32 index)
{
    Swapchain* d3d12Swapchain = (Swapchain*)swapchain;
    if (index > d3d12Swapchain->imageCount) {
        return nullptr;
    }
    return (PalImage*)&d3d12Swapchain->images[index];
}

PalResult PAL_CALL getNextSwapchainImageD3D12(
    PalSwapchain* swapchain,
    PalSwapchainNextImageInfo* info,
    Uint32* outIndex)
{
    Uint32 index = 0;
    Swapchain* d3d12Swapchain = (Swapchain*)swapchain;
    ID3D12CommandQueue* queue = d3d12Swapchain->queue;

    index = d3d12Swapchain->handle->lpVtbl->GetCurrentBackBufferIndex(d3d12Swapchain->handle);
    if (info->fence) {
        Fence* fence = (Fence*)info->fence;
        queue->lpVtbl->Signal(queue, fence->handle, 1);
        fence->signaled = true;
    }

    if (info->signalSemaphore) {
        Semaphore* semaphore = (Semaphore*)info->signalSemaphore;
        if (semaphore->isTimeline) {
            queue->lpVtbl->Signal(queue, semaphore->handle, info->signalValue);
        } else {
            queue->lpVtbl->Signal(queue, semaphore->handle, 1);
            semaphore->signaled = true;
        }
    }

    *outIndex = index;
    return PAL_RESULT_SUCCESS;
}

PalResult PAL_CALL presentSwapchainD3D12(
    PalSwapchain* swapchain,
    PalSwapchainPresentInfo* info)
{
    HRESULT result;
    Swapchain* d3d12Swapchain = (Swapchain*)swapchain;
    ID3D12CommandQueue* queue = d3d12Swapchain->queue;

    if (info->waitSemaphore) {
        Semaphore* semaphore = (Semaphore*)info->waitSemaphore;
        if (semaphore->isTimeline) {
            queue->lpVtbl->Wait(queue, semaphore->handle, info->waitValue);
        } else {
            queue->lpVtbl->Signal(queue, semaphore->handle, 1);
            semaphore->signaled = false;
        }
    }

    result = d3d12Swapchain->handle->lpVtbl->Present(
        d3d12Swapchain->handle, 
        d3d12Swapchain->syncInterval, 
        d3d12Swapchain->presentFlags);

    if (FAILED(result)) {
        if (result == DXGI_ERROR_DEVICE_REMOVED || result == DXGI_ERROR_DEVICE_RESET) {
            return PAL_RESULT_DEVICE_LOST;
        }

        // check if swapchain needs to be resize
        RECT windowRect;
        Uint32 w, h;
        bool ret = GetClientRect((HWND)d3d12Swapchain->surface->handle, &windowRect);
        w = windowRect.right - windowRect.left;
        w = windowRect.bottom - windowRect.top;

        if (!ret) {
            return PAL_RESULT_SURFACE_LOST;
        }

        if (w != d3d12Swapchain->windowWidth || h != d3d12Swapchain->windowHeight) {
            return PAL_RESULT_SWAPCHAIN_OUT_OF_DATE;
        }
        return PAL_RESULT_PLATFORM_FAILURE;
    }

    return PAL_RESULT_SUCCESS;
}

PalResult PAL_CALL resizeSwapchainD3D12(
    PalSwapchain* swapchain,
    Uint32 newWidth,
    Uint32 newHeight)
{
    HRESULT result;
    Swapchain* d3d12Swapchain = (Swapchain*)swapchain;
    result = d3d12Swapchain->handle->lpVtbl->ResizeBuffers(
        d3d12Swapchain->handle, 
        d3d12Swapchain->imageCount, 
        newWidth, 
        newHeight, 
        d3d12Swapchain->format, 
        d3d12Swapchain->flags);

    if (FAILED(result)) {
        if (result == E_INVALIDARG) {
            return PAL_RESULT_INVALID_ARGUMENT;
        }
        return PAL_RESULT_PLATFORM_FAILURE;
    }

    // fill all images with the creatio info
    for (int i = 0; i < d3d12Swapchain->imageCount; i++) {
        ID3D12Resource* tmp = nullptr;
        d3d12Swapchain->handle->lpVtbl->GetBuffer(
            d3d12Swapchain->handle, 
            i, 
            &IID_Resource, 
            (void**)&tmp);

        Image* image = &d3d12Swapchain->images[i];
        image->handle = tmp;
        image->info.height = newHeight;
        image->info.width = newWidth;
    }

    // get and cache window size for swapchain out of date error
    RECT windowRect;
    Surface* surface = d3d12Swapchain->surface;
    if (!GetClientRect((HWND)surface->handle, &windowRect)) {
        return PAL_RESULT_SURFACE_LOST;
    }

    d3d12Swapchain->windowWidth = windowRect.right - windowRect.left;
    d3d12Swapchain->windowWidth = windowRect.bottom - windowRect.top;
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
    Device* d3d12Device = (Device*)device;
    Shader* shader = nullptr;

    if (info->stage == PAL_SHADER_STAGE_MESH || info->stage == PAL_SHADER_STAGE_TASK) {
        if (!(d3d12Device->features & PAL_ADAPTER_FEATURE_MESH_SHADER)) {
            return PAL_RESULT_ADAPTER_FEATURE_NOT_SUPPORTED;
        }

    // clang-format off
    } else if (info->stage == PAL_SHADER_STAGE_RAYGEN ||
               info->stage == PAL_SHADER_STAGE_CLOSEST_HIT ||
               info->stage == PAL_SHADER_STAGE_ANY_HIT ||
               info->stage == PAL_SHADER_STAGE_MISS ||
               info->stage == PAL_SHADER_STAGE_INTERSECTION ||
               info->stage == PAL_SHADER_STAGE_CALLABLE) {
        if (!(d3d12Device->features & PAL_ADAPTER_FEATURE_RAY_TRACING)) {
            return PAL_RESULT_ADAPTER_FEATURE_NOT_SUPPORTED;
        }
    }
    // clang-format on

    shader = palAllocate(s_D3D12.allocator, sizeof(Shader), 0);
    if (!shader) {
        return PAL_RESULT_OUT_OF_MEMORY;
    }

    shader->byteCode.pShaderBytecode = info->bytecode;
    shader->byteCode.BytecodeLength = info->bytecodeSize;
    shader->stage = info->stage;
    *outShader = (Shader*)shader;
    return PAL_RESULT_SUCCESS;
}

void PAL_CALL destroyShaderD3D12(PalShader* shader)
{
    Shader* d3d12Shader = (Shader*)shader;
    palFree(s_D3D12.allocator, d3d12Shader);
}

// ==================================================
// Fence
// ==================================================

PalResult PAL_CALL createFenceD3D12(
    PalDevice* device,
    bool signaled,
    PalFence** outFence)
{

}

void PAL_CALL destroyFenceD3D12(PalFence* fence)
{

}

PalResult PAL_CALL waitFenceD3D12(
    PalFence* fence,
    Uint64 timeout)
{

}

PalResult PAL_CALL resetFenceD3D12(PalFence* fence)
{

}

bool PAL_CALL isFenceSignaledD3D12(PalFence* fence)
{

}

// ==================================================
// Semaphore
// ==================================================

PalResult PAL_CALL createSemaphoreD3D12(
    PalDevice* device,
    PalSemaphore** outSemaphore)
{

}

void PAL_CALL destroySemaphoreD3D12(PalSemaphore* semaphore)
{

}

PalResult PAL_CALL waitSemaphoreD3D12(
    PalSemaphore* semaphore,
    Uint64 value,
    Uint64 timeout)
{

}

PalResult PAL_CALL signalSemaphoreD3D12(
    PalSemaphore* semaphore,
    PalQueue* queue,
    Uint64 value)
{

}

PalResult PAL_CALL getSemaphoreValueD3D12(
    PalSemaphore* semaphore,
    Uint64* value)
{

}

// ==================================================
// Command Pool And Buffer
// ==================================================

PalResult PAL_CALL createCommandPoolD3D12(
    PalDevice* device,
    PalQueue* queue,
    PalCommandPool** outPool)
{

}

void PAL_CALL destroyCommandPoolD3D12(PalCommandPool* pool)
{

}

PalResult PAL_CALL resetCommandPoolD3D12(PalCommandPool* pool)
{

}

PalResult PAL_CALL allocateCommandBufferD3D12(
    PalDevice* device,
    PalCommandPool* pool,
    PalCommandBufferType type,
    PalCommandBuffer** outBuffer)
{

}

void PAL_CALL freeCommandBufferD3D12(PalCommandBuffer* buffer)
{

}

PalResult PAL_CALL resetCommandBufferD3D12(PalCommandBuffer* cmdBuffer)
{

}

PalResult PAL_CALL submitCommandBufferD3D12(
    PalQueue* queue,
    PalCommandBufferSubmitInfo* info)
{

}

// ==================================================
// Command Recording
// ==================================================

PalResult PAL_CALL cmdBeginD3D12(
    PalCommandBuffer* cmdBuffer,
    PalRenderingLayoutInfo* info)
{

}

PalResult PAL_CALL cmdEndD3D12(PalCommandBuffer* cmdBuffer)
{

}

PalResult PAL_CALL cmdExecuteCommandBufferD3D12(
    PalCommandBuffer* primaryCmdBuffer,
    PalCommandBuffer* secondaryCmdBuffer)
{

}

PalResult PAL_CALL cmdSetFragmentShadingRateD3D12(
    PalCommandBuffer* cmdBuffer,
    PalFragmentShadingRateState* state)
{

}

PalResult PAL_CALL cmdDrawMeshTasksD3D12(
    PalCommandBuffer* cmdBuffer,
    Uint32 groupCountX,
    Uint32 groupCountY,
    Uint32 groupCountZ)
{

}

PalResult PAL_CALL cmdDrawMeshTasksIndirectD3D12(
    PalCommandBuffer* cmdBuffer,
    PalBuffer* buffer,
    Uint64 offset,
    Uint32 drawCount,
    Uint32 stride)
{

}

PalResult PAL_CALL cmdDrawMeshTasksIndirectCountD3D12(
    PalCommandBuffer* cmdBuffer,
    PalBuffer* buffer,
    PalBuffer* countBuffer,
    Uint64 offset,
    Uint64 countBufferOffset,
    Uint32 maxDrawCount,
    Uint32 stride)
{

}

PalResult PAL_CALL cmdBuildAccelerationStructureD3D12(
    PalCommandBuffer* cmdBuffer,
    PalAccelerationStructureBuildInfo* info)
{

}

PalResult PAL_CALL cmdBeginRenderingD3D12(
    PalCommandBuffer* cmdBuffer,
    PalRenderingInfo* info)
{

}

PalResult PAL_CALL cmdEndRenderingD3D12(PalCommandBuffer* cmdBuffer)
{

}

PalResult PAL_CALL cmdCopyBufferD3D12(
    PalCommandBuffer* cmdBuffer,
    PalBuffer* dst,
    PalBuffer* src,
    PalBufferCopyInfo* copyInfo)
{

}

PalResult PAL_CALL cmdCopyBufferToImageD3D12(
    PalCommandBuffer* cmdBuffer,
    PalImage* dstImage,
    PalBuffer* srcBuffer,
    PalBufferImageCopyInfo* copyInfo)
{

}

PalResult PAL_CALL cmdCopyImageD3D12(
    PalCommandBuffer* cmdBuffer,
    PalImage* dst,
    PalImage* src,
    PalImageCopyInfo* copyInfo)
{

}

PalResult PAL_CALL cmdCopyImageToBufferD3D12(
    PalCommandBuffer* cmdBuffer,
    PalBuffer* dstBuffer,
    PalImage* srcImage,
    PalBufferImageCopyInfo* copyInfo)
{

}

PalResult PAL_CALL cmdBindPipelineD3D12(
    PalCommandBuffer* cmdBuffer,
    PalPipelineBindPoint bindPoint,
    PalPipeline* pipeline)
{

}

PalResult PAL_CALL cmdSetViewportD3D12(
    PalCommandBuffer* cmdBuffer,
    Uint32 count,
    PalViewport* viewports)
{

}

PalResult PAL_CALL cmdSetScissorsD3D12(
    PalCommandBuffer* cmdBuffer,
    Uint32 count,
    PalRect2D* scissors)
{

}

PalResult PAL_CALL cmdBindVertexBuffersD3D12(
    PalCommandBuffer* cmdBuffer,
    Uint32 firstSlot,
    Uint32 count,
    PalBuffer** buffers,
    Uint64* offsets)
{

}

PalResult PAL_CALL cmdBindIndexBufferD3D12(
    PalCommandBuffer* cmdBuffer,
    PalBuffer* buffer,
    Uint64 offset,
    PalIndexType type)
{

}

PalResult PAL_CALL cmdDrawD3D12(
    PalCommandBuffer* cmdBuffer,
    Uint32 vertexCount,
    Uint32 instanceCount,
    Uint32 firstVertex,
    Uint32 firstInstance)
{

}

PalResult PAL_CALL cmdDrawIndirectD3D12(
    PalCommandBuffer* cmdBuffer,
    PalBuffer* buffer,
    Uint64 offset,
    Uint32 count,
    Uint32 stride)
{

}

PalResult PAL_CALL cmdDrawIndirectCountD3D12(
    PalCommandBuffer* cmdBuffer,
    PalBuffer* buffer,
    PalBuffer* countBuffer,
    Uint64 offset,
    Uint64 countBufferOffset,
    Uint32 maxDrawCount,
    Uint32 stride)
{

}

PalResult PAL_CALL cmdDrawIndexedD3D12(
    PalCommandBuffer* cmdBuffer,
    Uint32 indexCount,
    Uint32 instanceCount,
    Uint32 firstIndex,
    Int32 vertexOffset,
    Uint32 firstInstance)
{

}

PalResult PAL_CALL cmdDrawIndexedIndirectD3D12(
    PalCommandBuffer* cmdBuffer,
    PalBuffer* buffer,
    Uint64 offset,
    Uint32 count,
    Uint32 stride)
{

}

PalResult PAL_CALL cmdDrawIndexedIndirectCountD3D12(
    PalCommandBuffer* cmdBuffer,
    PalBuffer* buffer,
    PalBuffer* countBuffer,
    Uint64 offset,
    Uint64 countBufferOffset,
    Uint32 maxDrawCount,
    Uint32 stride)
{

}

PalResult PAL_CALL cmdMemoryBarrierD3D12(
    PalCommandBuffer* cmdBuffer,
    PalUsageStateInfo* oldsUsageStateInfo,
    PalUsageStateInfo* newUsageStateInfo)
{

}

PalResult PAL_CALL cmdImageBarrierD3D12(
    PalCommandBuffer* cmdBuffer,
    PalImage* image,
    PalImageSubresourceRange* subresourceRange,
    PalUsageStateInfo* oldUsageStateInfo,
    PalUsageStateInfo* newUsageStateInfo)
{

}

PalResult PAL_CALL cmdBufferBarrierD3D12(
    PalCommandBuffer* cmdBuffer,
    PalBuffer* buffer,
    PalUsageStateInfo* oldUsageStateInfo,
    PalUsageStateInfo* newUsageStateInfo)
{

}

PalResult PAL_CALL cmdDispatchD3D12(
    PalCommandBuffer* cmdBuffer,
    Uint32 groupCountX,
    Uint32 groupCountY,
    Uint32 groupCountZ)
{

}

PalResult PAL_CALL cmdDispatchBaseD3D12(
    PalCommandBuffer* cmdBuffer,
    Uint32 baseGroupX,
    Uint32 baseGroupY,
    Uint32 baseGroupZ,
    Uint32 groupCountX,
    Uint32 groupCountY,
    Uint32 groupCountZ)
{

}

PalResult PAL_CALL cmdDispatchIndirectD3D12(
    PalCommandBuffer* cmdBuffer,
    PalBuffer* buffer,
    Uint64 offset)
{

}

PalResult PAL_CALL cmdTraceRaysD3D12(
    PalCommandBuffer* cmdBuffer,
    PalShaderBindingTable* sbt,
    Uint32 raygenIndex,
    Uint32 width,
    Uint32 height,
    Uint32 depth)
{

}

PalResult PAL_CALL cmdTraceRaysIndirectD3D12(
    PalCommandBuffer* cmdBuffer,
    Uint32 raygenIndex,
    PalShaderBindingTable* sbt,
    PalDeviceAddress bufferAddress)
{

}

PalResult PAL_CALL cmdBindDescriptorSetD3D12(
    PalCommandBuffer* cmdBuffer,
    PalPipelineBindPoint bindPoint,
    PalPipelineLayout* layout,
    Uint32 setIndex,
    PalDescriptorSet* set)
{

}

PalResult PAL_CALL cmdPushConstantsD3D12(
    PalCommandBuffer* cmdBuffer,
    PalPipelineLayout* layout,
    Uint32 shaderStageCount,
    PalShaderStage* shaderStages,
    Uint32 offset,
    Uint32 size,
    const void* value)
{

}

PalResult PAL_CALL cmdSetCullModeD3D12(
    PalCommandBuffer* cmdBuffer,
    PalCullMode cullMode)
{

}

PalResult PAL_CALL cmdSetFrontFaceD3D12(
    PalCommandBuffer* cmdBuffer,
    PalFrontFace frontFace)
{

}

PalResult PAL_CALL cmdSetPrimitiveTopologyD3D12(
    PalCommandBuffer* cmdBuffer,
    PalPrimitiveTopology topology)
{

}

PalResult PAL_CALL cmdSetDepthTestEnableD3D12(
    PalCommandBuffer* cmdBuffer,
    bool enable)
{

}

PalResult PAL_CALL cmdSetDepthWriteEnableD3D12(
    PalCommandBuffer* cmdBuffer,
    bool enable)
{

}

PalResult PAL_CALL cmdSetStencilOpD3D12(
    PalCommandBuffer* cmdBuffer,
    PalStencilFaceFlags faceMask,
    PalStencilOp failOp,
    PalStencilOp passOp,
    PalStencilOp depthFailOp,
    PalCompareOp compareOp)
{

}

// ==================================================
// Acceleration Structure
// ==================================================

PalResult PAL_CALL createAccelerationstructureD3D12(
    PalDevice* device,
    const PalAccelerationStructureCreateInfo* info,
    PalAccelerationStructure** outAs)
{

}

void PAL_CALL destroyAccelerationstructureD3D12(PalAccelerationStructure* as)
{

}

PalResult PAL_CALL getAccelerationStructureBuildSizeD3D12(
    PalDevice* device,
    PalAccelerationStructureBuildInfo* info,
    PalAccelerationStructureBuildSize* size)
{

}

// ==================================================
// Buffer
// ==================================================

PalResult PAL_CALL createBufferD3D12(
    PalDevice* device,
    const PalBufferCreateInfo* info,
    PalBuffer** outBuffer)
{

}

void PAL_CALL destroyBufferD3D12(PalBuffer* buffer)
{

}

PalResult PAL_CALL getBufferMemoryRequirementsD3D12(
    PalBuffer* buffer,
    PalMemoryRequirements* requirements)
{

}

PalResult PAL_CALL computeInstanceBufferRequirementsD3D12(
    PalDevice* device,
    PalInstanceBufferRequirements* requirements,
    Uint32 instanceCount)
{

}

PalResult PAL_CALL writeInstancesToMappedMemoryD3D12(
    PalDevice* device,
    void* ptr,
    PalAccelerationStructureInstance* instances,
    Uint32 instanceCount)
{

}

PalResult PAL_CALL bindBufferMemoryD3D12(
    PalBuffer* buffer,
    PalMemory* memory,
    Uint64 offset)
{

}

PalResult PAL_CALL mapBufferMemoryD3D12(
    PalBuffer* buffer,
    Uint64 offset,
    Uint64 size,
    void** outPtr)
{
    
}

void PAL_CALL unmapBufferMemoryD3D12(PalBuffer* buffer)
{

}

PalDeviceAddress PAL_CALL getBufferDeviceAddressD3D12(PalBuffer* buffer)
{

}

// ==================================================
// Descriptor Pool, Set and Layout
// ==================================================

PalResult PAL_CALL createDescriptorSetLayoutD3D12(
    PalDevice* device,
    const PalDescriptorSetLayoutCreateInfo* info,
    PalDescriptorSetLayout** outLayout)
{

}

void PAL_CALL destroyDescriptorSetLayoutD3D12(PalDescriptorSetLayout* layout)
{

}

PalResult PAL_CALL createDescriptorPoolD3D12(
    PalDevice* device,
    const PalDescriptorPoolCreateInfo* info,
    PalDescriptorPool** outPool)
{

}

void PAL_CALL destroyDescriptorPoolD3D12(PalDescriptorPool* pool)
{

}

PalResult PAL_CALL resetDescriptorPoolD3D12(PalDescriptorPool* pool)
{

}

PalResult PAL_CALL allocateDescriptorSetD3D12(
    PalDevice* device,
    PalDescriptorPool* pool,
    PalDescriptorSetLayout* layout,
    PalDescriptorSet** outSet)
{

}

void PAL_CALL freeDescriptorSetD3D12(PalDescriptorSet* set)
{

}

PalResult PAL_CALL updateDescriptorSetD3D12(
    PalDevice* device,
    Uint32 count,
    PalDescriptorSetWriteInfo* infos)
{

}

// ==================================================
// Pipeline Layout
// ==================================================

PalResult PAL_CALL createPipelineLayoutD3D12(
    PalDevice* device,
    const PalPipelineLayoutCreateInfo* info,
    PalPipelineLayout** outLayout)
{

}

void PAL_CALL destroyPipelineLayoutD3D12(PalPipelineLayout* layout)
{

}

// ==================================================
// Pipeline
// ==================================================

PalResult PAL_CALL createGraphicsPipelineD3D12(
    PalDevice* device,
    const PalGraphicsPipelineCreateInfo* info,
    PalPipeline** outPipeline)
{

}

PalResult PAL_CALL createComputePipelineD3D12(
    PalDevice* device,
    const PalComputePipelineCreateInfo* info,
    PalPipeline** outPipeline)
{

}

PalResult PAL_CALL createRayTracingPipelineD3D12(
    PalDevice* device,
    const PalRayTracingPipelineCreateInfo* info,
    PalPipeline** outPipeline)
{

}

void PAL_CALL destroyPipelineD3D12(PalPipeline* pipeline)
{

}

// ==================================================
// Shader Binding Table
// ==================================================

PalResult PAL_CALL createShaderBindingTableD3D12(
    PalDevice* device,
    const PalShaderBindingTableCreateInfo* info,
    PalShaderBindingTable** outSbt)
{

}

void PAL_CALL destroyShaderBindingTableD3D12(PalShaderBindingTable* sbt)
{

}

#endif // PAL_HAS_D3D12

#endif // __WIN32