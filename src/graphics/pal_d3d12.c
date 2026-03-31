
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

typedef HRESULT (WINAPI* PFN_CreateDXGIFactory2)(
    UINT,
    REFIID, 
    void**);

typedef struct {
    const PalGraphicsBackend* backend;

    IDXGIAdapter4* handle;
} Adapter;

typedef struct {
    bool debugLayer;
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
    ID3D12Device* handle;
} Device;

static D3D12 s_D3D12 = {0};

// ==================================================
// Helper Functions
// ==================================================


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

    if (s_D3D12.adapters) {
        palFree(s_D3D12.allocator, s_D3D12.adapters);
    }

    while (s_D3D12.factory->lpVtbl->EnumAdapters(
        s_D3D12.factory, 
        adapterCount, 
        &adapter) != DXGI_ERROR_NOT_FOUND) {
        if (outAdapters) {
            IDXGIAdapter4* tmp = nullptr;
            if SUCCEEDED((adapter->lpVtbl->QueryInterface(adapter, &IID_Adapter, (void**)&tmp))) {
                dxAdapters[adapterCount] = tmp;
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
            outAdapters[i] = (PalAdapter*)tmp;
        }

    } else {
        *count = adapterCount;
    }

    return PAL_RESULT_SUCCESS;
}

PalResult PAL_CALL getAdapterInfoD3D12(
    PalAdapter* adapter,
    PalAdapterInfo* info)
{
    Adapter* d3dAdapter = (Adapter*)adapter;
    IDXGIAdapter4* adapterHandle = d3dAdapter->handle;
    DXGI_ADAPTER_DESC3 desc;
    D3D12_FEATURE_DATA_ARCHITECTURE1 arch = {0};
    ID3D12Device* device = nullptr;

    HRESULT result = adapterHandle->lpVtbl->GetDesc3(adapterHandle, &desc);
    if (FAILED(result)) {
        return PAL_RESULT_INVALID_ADAPTER;
    }

    D3D_FEATURE_LEVEL supportedLevel = D3D_FEATURE_LEVEL_11_0;
    D3D_FEATURE_LEVEL levels[] = {
        D3D_FEATURE_LEVEL_12_2,
        D3D_FEATURE_LEVEL_12_1,
        D3D_FEATURE_LEVEL_12_0,
        D3D_FEATURE_LEVEL_11_1,
        D3D_FEATURE_LEVEL_11_0
    };

    const char* levelStrings[] = {
        "12_2",
        "12_1",
        "12_0",
        "11_1",
        "11_0"
    };

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

    // create a temporary device to check the supported version
    Uint32 levelIndex = 0;
    for (int i = 0; i < 5; i++) {
        result = s_D3D12.createDevice(
            (IUnknown*)adapterHandle, 
            levels[i], 
            &IID_Device, 
            (void**)&device);
            
        if (SUCCEEDED(result)) {
            supportedLevel = levels[i];
            levelIndex = i;
            break;
        }
    }

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

    info->version = supportedLevel;
    strcpy(info->versionString, levelStrings[levelIndex]);

    device->lpVtbl->Release(device);
    return PAL_RESULT_SUCCESS;
}

PalResult PAL_CALL getAdapterCapabilitiesD3D12(
    PalAdapter* adapter,
    PalAdapterCapabilities* caps)
{
    caps->maxComputeQueues = PAL_INFINITE;
    caps->maxGraphicsQueues = PAL_INFINITE;
    caps->maxCopyQueues = PAL_INFINITE;

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
    caps->maxStorageBufferSize = PAL_INFINITE;
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
    ID3D12Device* device = nullptr;
    Adapter* d3dAdapter = (Adapter*)adapter;
    IDXGIAdapter4* adapterHandle = d3dAdapter->handle;
    PalAdapterFeatures features = 0;

    D3D12_FEATURE_DATA_D3D12_OPTIONS options = {0};
    D3D12_FEATURE_DATA_D3D12_OPTIONS3 options3 = {0};
    D3D12_FEATURE_DATA_D3D12_OPTIONS5 options5 = {0};
    D3D12_FEATURE_DATA_D3D12_OPTIONS6 options6 = {0};
    D3D12_FEATURE_DATA_D3D12_OPTIONS7 options7 = {0};
    D3D12_FEATURE_DATA_SHADER_MODEL shaderModel = {0};

    D3D_FEATURE_LEVEL levels[] = {
        D3D_FEATURE_LEVEL_12_2,
        D3D_FEATURE_LEVEL_12_1,
        D3D_FEATURE_LEVEL_12_0,
        D3D_FEATURE_LEVEL_11_1,
        D3D_FEATURE_LEVEL_11_0
    };

    for (int i = 0; i < 5; i++) {
        result = s_D3D12.createDevice(
            (IUnknown*)adapterHandle, 
            levels[i], 
            &IID_Device, 
            (void**)&device);
            
        if (SUCCEEDED(result)) {
            if (levels[i] >= D3D_FEATURE_LEVEL_12_0) {
                features |= PAL_ADAPTER_FEATURE_DEPTH_STENCIL_RESOLVE;
            }
            break;
        }
    }

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

    device->lpVtbl->Release(device);
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
    D3D_FEATURE_LEVEL levels[] = {
        D3D_FEATURE_LEVEL_12_2,
        D3D_FEATURE_LEVEL_12_1,
        D3D_FEATURE_LEVEL_12_0,
        D3D_FEATURE_LEVEL_11_1,
        D3D_FEATURE_LEVEL_11_0
    };

    for (int i = 0; i < 5; i++) {
        result = s_D3D12.createDevice(
            (IUnknown*)d3d12Adapter->handle, 
            levels[i], 
            &IID_Device, 
            (void**)&device->handle);
            
        if (SUCCEEDED(result)) {
            break;
        }
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

    device->adapter = d3d12Adapter->handle;
    device->features = features;
    *outDevice = (PalDevice*)device;
    return PAL_RESULT_SUCCESS;
}

void PAL_CALL destroyDeviceD3D12(PalDevice* device)
{
    Device* d3d12Device = (Device*)device;
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
    caps->maxHitAttributeSize = PAL_INFINITE;
    caps->maxInstanceCount = PAL_INFINITE;
    caps->maxPrimitiveCount = PAL_INFINITE;
    caps->maxGeometryCount = PAL_INFINITE;
    caps->maxPayloadSize = PAL_INFINITE;
    caps->maxDispatchInvocations = PAL_INFINITE;

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
    caps->maxImagesPerShaderStage = PAL_INFINITE;
    caps->maxImagesPerDescriptorSet = PAL_INFINITE;
    caps->maxSamplersPerShaderStage = 2048;
    caps->maxSamplersPerDescriptorSet = PAL_INFINITE;
    caps->maxStorageBuffersPerShaderStage = PAL_INFINITE;
    caps->maxUniformBuffersPerShaderStage = PAL_INFINITE;
    caps->maxStorageBuffersPerDescriptorSet = PAL_INFINITE;
    caps->maxUniformBuffersPerDescriptorSet = PAL_INFINITE;
    caps->maxDescriptors = PAL_INFINITE;

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

}

void PAL_CALL destroyQueueD3D12(PalQueue* queue)
{

}

PalResult PAL_CALL waitQueueD3D12(PalQueue* queue)
{

}

bool PAL_CALL canQueuePresentD3D12(
    PalQueue* queue,
    PalSurface* surface)
{

}

// ==================================================
// Formats
// ==================================================

PalResult PAL_CALL enumerateFormatsD3D12(
    PalAdapter* adapter,
    Int32* count,
    PalFormatInfo* outFormats)
{

}

bool PAL_CALL isFormatSupportedD3D12(
    PalAdapter* adapter,
    PalFormat format)
{

}

PalImageUsages PAL_CALL queryFormatImageUsagesD3D12(
    PalAdapter* adapter,
    PalFormat format)
{

}

PalImageViewUsages PAL_CALL queryFormatImageViewUsagesD3D12(
    PalAdapter* adapter,
    PalFormat format)
{

}

// ==================================================
// Image
// ==================================================

PalResult PAL_CALL createImageD3D12(
    PalDevice* device,
    const PalImageCreateInfo* info,
    PalImage** outImage)
{

}

void PAL_CALL destroyImageD3D12(PalImage* image)
{

}

PalResult PAL_CALL getImageInfoD3D12(
    PalImage* image,
    PalImageInfo* info)
{

}

PalResult PAL_CALL getImageMemoryRequirementsD3D12(
    PalImage* image,
    PalMemoryRequirements* requirements)
{

}

PalResult PAL_CALL bindImageMemoryD3D12(
    PalImage* image,
    PalMemory* memory,
    Uint64 offset)
{

}

PalResult PAL_CALL mapImageMemoryD3D12(
    PalImage* image,
    Uint64 offset,
    Uint64 size,
    void** outPtr)
{

}

void PAL_CALL unmapImageMemoryD3D12(PalImage* image)
{

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

}

void PAL_CALL destroyImageViewD3D12(PalImageView* imageView)
{

}

// ==================================================
// Sampler
// ==================================================

PalResult PAL_CALL createSamplerD3D12(
    PalDevice* device,
    const PalSamplerCreateInfo* info,
    PalSampler** outSampler)
{

}

void PAL_CALL destroySamplerD3D12(PalSampler* sampler)
{

}

// ==================================================
// Surface
// ==================================================

PalResult PAL_CALL createSurfaceD3D12(
    PalDevice* device,
    PalGraphicsWindow* window,
    PalSurface** outSurface)
{

}

void PAL_CALL destroySurfaceD3D12(PalSurface* surface)
{

}

PalResult PAL_CALL getSurfaceCapabilitiesD3D12(
    PalDevice* device,
    PalSurface* surface,
    PalSurfaceCapabilities* caps)
{

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

}

void PAL_CALL destroySwapchainD3D12(PalSwapchain* swapchain)
{

}

PalImage* PAL_CALL getSwapchainImageD3D12(
    PalSwapchain* swapchain,
    Int32 index)
{

}

PalResult PAL_CALL getNextSwapchainImageD3D12(
    PalSwapchain* swapchain,
    PalSwapchainNextImageInfo* info,
    Uint32* outIndex)
{

}

PalResult PAL_CALL presentSwapchainD3D12(
    PalSwapchain* swapchain,
    PalSwapchainPresentInfo* info)
{

}

// ==================================================
// Shader
// ==================================================

PalResult PAL_CALL createShaderD3D12(
    PalDevice* device,
    const PalShaderCreateInfo* info,
    PalShader** outShader)
{

}

void PAL_CALL destroyShaderD3D12(PalShader* shader)
{

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