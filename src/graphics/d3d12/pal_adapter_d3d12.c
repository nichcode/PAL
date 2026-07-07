
/**
    PAL - Prime Abstraction Layer
    Copyright (C) 2025
    Licensed under the Zlib license. See LICENSE file in root.
 */

#if PAL_HAS_D3D12_BACKEND
#include "pal_d3d12.h"

// From Agility SDK
#ifndef D3D_SHADER_MODEL_6_8
#define D3D_SHADER_MODEL_6_8 0x68
#endif // D3D_SHADER_MODEL_6_8

#ifndef D3D_SHADER_MODEL_6_9
#define D3D_SHADER_MODEL_6_9 0x69
#endif // D3D_SHADER_MODEL_6_9

#ifndef D3D_SHADER_MODEL_6_10
#define D3D_SHADER_MODEL_6_10 0x6a
#endif // D3D_SHADER_MODEL_6_10

const IID IID_Device = {0xc4fec28f, 0x7966, 0x4e95, 0x9f,0x94, 0xf4,0x31,0xcb,0x56,0xc3,0xb8};
const IID IID_Adapter = {0x3c8d99d1, 0x4fbf, 0x4181, 0xa8,0x2c, 0xaf,0x66,0xbf,0x7b,0xd2,0x4e};

PalResult PAL_CALL enumerateAdaptersD3D12(
    int32_t* count,
    PalAdapter** outAdapters)
{
    uint32_t adapterCount = 0;
    IDXGIAdapter* adapter = nullptr;
    IDXGIAdapter4* dxAdapters[32];
    ID3D12Device* devices[32];
    D3D_FEATURE_LEVEL deviceLevels[32];

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

    while (SUCCEEDED(IDXGIFactory6_EnumAdapters(s_D3D12.factory, adapterCount, &adapter))) {
        if (outAdapters) {
            IDXGIAdapter4* tmp = nullptr;
            if (SUCCEEDED(IDXGIAdapter_QueryInterface(adapter, &IID_Adapter, (void**)&tmp))) {
                dxAdapters[adapterCount] = tmp;
            }

            // create a temp device for every adapter to use as an instance to check features.
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
        s_D3D12.adapters = palAllocate(s_D3D12.allocator, sizeof(AdapterD3D12) * adapterCount, 0);
        if (!s_D3D12.adapters) {
            return PAL_RESULT_CODE_OUT_OF_MEMORY;
        }

        // fill the array with AdapterD3D12 structs
        for (int i = 0; i < *count; i++) {
            AdapterD3D12* tmp = &s_D3D12.adapters[i];
            tmp->handle = dxAdapters[i];
            tmp->tmpDevice = devices[i];
            tmp->level = deviceLevels[i];
            tmp->reserved = PAL_BACKEND_KEY;
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
    AdapterD3D12* d3d12Adapter = (AdapterD3D12*)adapter;
    DXGI_ADAPTER_DESC3 desc;
    D3D12_FEATURE_DATA_ARCHITECTURE1 arch = {0};
    ID3D12Device* device = d3d12Adapter->tmpDevice;

    D3D12_FEATURE_DATA_SHADER_MODEL shaderModel = {0};
    shaderModel.HighestShaderModel = D3D_SHADER_MODEL_6_0;

    HRESULT result = IDXGIAdapter4_GetDesc3(d3d12Adapter->handle, &desc);
    if (FAILED(result)) {
        return makeResultD3D12(result);
    }

    result = ID3D12Device_CheckFeatureSupport(
        device, 
        D3D12_FEATURE_SHADER_MODEL, 
        &shaderModel, 
        sizeof(shaderModel));

    info->shaderFormats = PAL_SHADER_FORMAT_DXBC;
    if (shaderModel.HighestShaderModel >= D3D_SHADER_MODEL_6_0 && result == S_OK) {
        info->shaderFormats |= PAL_SHADER_FORMAT_DXIL;
    }

    info->vendorId = desc.VendorId;
    info->deviceId= desc.DeviceId;
    info->apiType = PAL_ADAPTER_API_TYPE_D3D12;
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

    result = ID3D12Device_CheckFeatureSupport(
        device, 
        D3D12_FEATURE_ARCHITECTURE1, 
        &arch, 
        sizeof(arch));

    if (arch.UMA == PAL_TRUE) {
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

    return PAL_RESULT_SUCCESS;
}

PalResult PAL_CALL getAdapterCapabilitiesD3D12(
    PalAdapter* adapter,
    PalAdapterCapabilities* caps)
{
    AdapterD3D12* d3d12Adapter = (AdapterD3D12*)adapter;
    if (!d3d12Adapter->handle) {
        return PAL_RESULT_CODE_INVALID_HANDLE;
    }

    PalViewportCapabilities* viewportCaps = &caps->viewportCaps;
    PalImageCapabilities* imageCaps = &caps->imageCaps;
    PalResourceCapabilities* resourceCaps = &caps->resourceCaps;
    PalComputeCapabilities* computeCaps = &caps->computeCaps;

    caps->maxComputeQueues = 2; // safe default
    caps->maxGraphicsQueues = 2; // safe default
    caps->maxCopyQueues = 2; // safe default

    caps->maxColorAttachments = D3D12_SIMULTANEOUS_RENDER_TARGET_COUNT;
    caps->maxUniformBufferSize = D3D12_REQ_IMMEDIATE_CONSTANT_BUFFER_ELEMENT_COUNT * 16;
    caps->maxStorageBufferSize = 2147483648; // 2 GIB
    caps->maxPushConstantSize = 256;

    caps->maxVertexLayouts = 32; // safe
    caps->maxVertexAttributes = 32; // safe
    caps->maxTessellationPatchPoint = 32;

    // viewport limits
    viewportCaps->maxWidth = D3D12_REQ_TEXTURE2D_U_OR_V_DIMENSION;
    viewportCaps->maxHeight = D3D12_REQ_TEXTURE2D_U_OR_V_DIMENSION;
    viewportCaps->minBoundsRange = (float)D3D12_VIEWPORT_BOUNDS_MIN;
    viewportCaps->maxBoundsRange = (float)D3D12_VIEWPORT_BOUNDS_MAX;

    // image limits
    imageCaps->maxWidth = D3D12_REQ_TEXTURE2D_U_OR_V_DIMENSION;
    imageCaps->maxHeight = D3D12_REQ_TEXTURE2D_U_OR_V_DIMENSION;
    imageCaps->maxDepth = D3D12_REQ_TEXTURE3D_U_V_OR_W_DIMENSION;
    imageCaps->maxArrayLayers = D3D12_REQ_TEXTURE2D_ARRAY_AXIS_DIMENSION;

    // d3d12 does not give this but we calculate from the max width and width
    uint32_t a = imageCaps->maxWidth;
    uint32_t b = imageCaps->maxHeight;
    uint32_t c = imageCaps->maxDepth;

    uint32_t tmp = a > b ? a : b;
    uint32_t size = tmp > c ? tmp : c;
    uint32_t levels = 0;
    while (size > 0) {
        // divide by two
        size = size / 2;
        levels++;
    }
    imageCaps->maxMipLevels = levels;

    // resource limits
    // always supported
    getDescriptorTierLimitsD3D12(d3d12Adapter->tmpDevice, resourceCaps, nullptr);
    resourceCaps->maxBoundSets = 32;

    // compute limits
    computeCaps->maxWorkGroupInvocations = D3D12_CS_THREAD_GROUP_MAX_THREADS_PER_GROUP;
    computeCaps->maxWorkGroupCount[0] = D3D12_CS_DISPATCH_MAX_THREAD_GROUPS_PER_DIMENSION;
    computeCaps->maxWorkGroupCount[1] = D3D12_CS_DISPATCH_MAX_THREAD_GROUPS_PER_DIMENSION;
    computeCaps->maxWorkGroupCount[2] = D3D12_CS_DISPATCH_MAX_THREAD_GROUPS_PER_DIMENSION;
    computeCaps->maxWorkGroupSize[0] = D3D12_CS_THREAD_GROUP_MAX_X;
    computeCaps->maxWorkGroupSize[1] = D3D12_CS_THREAD_GROUP_MAX_Y;
    computeCaps->maxWorkGroupSize[2] = D3D12_CS_THREAD_GROUP_MAX_Z;

    return PAL_RESULT_SUCCESS;
}

PalAdapterFeatures PAL_CALL getAdapterFeaturesD3D12(PalAdapter* adapter)
{
    HRESULT result;
    AdapterD3D12* d3d12Adapter = (AdapterD3D12*)adapter;
    PalAdapterFeatures features = 0;
    ID3D12Device* device = d3d12Adapter->tmpDevice;

    D3D12_FEATURE_DATA_D3D12_OPTIONS options = {0};
    D3D12_FEATURE_DATA_D3D12_OPTIONS3 options3 = {0};
    D3D12_FEATURE_DATA_D3D12_OPTIONS5 options5 = {0};
    D3D12_FEATURE_DATA_D3D12_OPTIONS6 options6 = {0};
    D3D12_FEATURE_DATA_D3D12_OPTIONS7 options7 = {0};
    D3D12_FEATURE_DATA_SHADER_MODEL shaderModel = {0};

    result = ID3D12Device_CheckFeatureSupport(
        device, 
        D3D12_FEATURE_D3D12_OPTIONS, 
        &options, 
        sizeof(options));

    result = ID3D12Device_CheckFeatureSupport(
        device, 
        D3D12_FEATURE_D3D12_OPTIONS3, 
        &options3, 
        sizeof(options3));

    result = ID3D12Device_CheckFeatureSupport(
        device, 
        D3D12_FEATURE_D3D12_OPTIONS5, 
        &options5, 
        sizeof(options5));

    result = ID3D12Device_CheckFeatureSupport(
        device, 
        D3D12_FEATURE_D3D12_OPTIONS6, 
        &options6, 
        sizeof(options6));

    result = ID3D12Device_CheckFeatureSupport(
        device, 
        D3D12_FEATURE_D3D12_OPTIONS7, 
        &options7, 
        sizeof(options7));

    shaderModel.HighestShaderModel = D3D_SHADER_MODEL_5_1;
    result = ID3D12Device_CheckFeatureSupport(
        device, 
        D3D12_FEATURE_SHADER_MODEL, 
        &shaderModel, 
        sizeof(shaderModel));

    if (shaderModel.HighestShaderModel >= D3D_SHADER_MODEL_5_1 && result == S_OK) {
        features |= PAL_ADAPTER_FEATURE_TESSELLATION_SHADER;
        features |= PAL_ADAPTER_FEATURE_SAMPLE_RATE_SHADING;
        features |= PAL_ADAPTER_FEATURE_SHADER_FLOAT64;
    }

    shaderModel.HighestShaderModel = D3D_SHADER_MODEL_6_2;
    result = device->lpVtbl->CheckFeatureSupport(
        device,
        D3D12_FEATURE_SHADER_MODEL,
        &shaderModel,
        sizeof(shaderModel));

    if (shaderModel.HighestShaderModel >= D3D_SHADER_MODEL_6_2 && result == S_OK) {
        features |= PAL_ADAPTER_FEATURE_SHADER_FLOAT16;
        features |= PAL_ADAPTER_FEATURE_SHADER_INT16;
    }

    shaderModel.HighestShaderModel = D3D_SHADER_MODEL_6_0;
    result = device->lpVtbl->CheckFeatureSupport(
        device,
        D3D12_FEATURE_SHADER_MODEL,
        &shaderModel,
        sizeof(shaderModel));

    if (shaderModel.HighestShaderModel >= D3D_SHADER_MODEL_6_0 && result == S_OK) {
        features |= PAL_ADAPTER_FEATURE_SHADER_INT64;
    }

    if (options3.ViewInstancingTier != D3D12_VIEW_INSTANCING_TIER_NOT_SUPPORTED) {
        features |= PAL_ADAPTER_FEATURE_MULTI_VIEW;
    }

    if (options5.RaytracingTier != D3D12_RAYTRACING_TIER_NOT_SUPPORTED) {
        features |= PAL_ADAPTER_FEATURE_RAY_TRACING;
        features |= PAL_ADAPTER_FEATURE_INDIRECT_RAY_TRACING;
    }

    if (options5.RaytracingTier >= D3D12_RAYTRACING_TIER_1_1) {
        features |= PAL_ADAPTER_FEATURE_RAY_QUERY;
    }

    if (options6.VariableShadingRateTier != D3D12_VARIABLE_SHADING_RATE_TIER_NOT_SUPPORTED) {
        features |= PAL_ADAPTER_FEATURE_FRAGMENT_SHADING_RATE;
        features |= PAL_ADAPTER_FEATURE_FRAGMENT_SHADING_RATE_ATTACHMENT;
    }

    if (options7.MeshShaderTier != D3D12_MESH_SHADER_TIER_NOT_SUPPORTED) {
        features |= PAL_ADAPTER_FEATURE_MESH_SHADER;
        features |= PAL_ADAPTER_FEATURE_INDIRECT_DRAW_MESH;
        features |= PAL_ADAPTER_FEATURE_INDIRECT_DRAW_MESH_COUNT;
    }

    if (!(options.ResourceBindingTier == D3D12_RESOURCE_BINDING_TIER_1)) {
        features |= PAL_ADAPTER_FEATURE_DESCRIPTOR_INDEXING;
    }

    // this features are supported on d3d12
    features |= PAL_ADAPTER_FEATURE_SAMPLER_ANISOTROPY;
    features |= PAL_ADAPTER_FEATURE_TIMELINE_SEMAPHORE;
    features |= PAL_ADAPTER_FEATURE_MULTI_VIEWPORT;
    features |= PAL_ADAPTER_FEATURE_GEOMETRY_SHADER;
    features |= PAL_ADAPTER_FEATURE_SWAPCHAIN;
    features |= PAL_ADAPTER_FEATURE_FENCE_RESET;
    features |= PAL_ADAPTER_FEATURE_POLYGON_MODE_LINE;
    features |= PAL_ADAPTER_FEATURE_BUFFER_DEVICE_ADDRESS;
    features |= PAL_ADAPTER_FEATURE_INDIRECT_DRAW;
    features |= PAL_ADAPTER_FEATURE_INDIRECT_DISPATCH;
    features |= PAL_ADAPTER_FEATURE_INDIRECT_DRAW_COUNT;
    features |= PAL_ADAPTER_FEATURE_IMAGE_VIEW_CUBE_ARRAY;
    features |= PAL_ADAPTER_FEATURE_NULL_DESCRIPTORS;

    if (d3d12Adapter->level >= D3D_FEATURE_LEVEL_12_0) {
        features |= PAL_ADAPTER_FEATURE_DEPTH_STENCIL_RESOLVE;
    }
    return features;
}

uint32_t PAL_CALL getHighestSupportedShaderTargetD3D12(
    PalAdapter* adapter, 
    PalShaderFormats shaderFormat)
{
    if (shaderFormat != PAL_SHADER_FORMAT_DXIL && shaderFormat != PAL_SHADER_FORMAT_DXBC) {
        return 0;
    }

    if (shaderFormat == PAL_SHADER_FORMAT_DXBC) {
        return PAL_MAKE_SHADER_TARGET(5, 1);
    }

    AdapterD3D12* d3d12Adapter = (AdapterD3D12*)adapter;
    D3D12_FEATURE_DATA_SHADER_MODEL shaderModel = {0};

    D3D_SHADER_MODEL models[11];
    models[0] = D3D_SHADER_MODEL_6_10;
    models[1] = D3D_SHADER_MODEL_6_9;
    models[2] = D3D_SHADER_MODEL_6_8;
    models[3] = D3D_SHADER_MODEL_6_7;
    models[4] = D3D_SHADER_MODEL_6_6;
    models[5] = D3D_SHADER_MODEL_6_5;
    models[6] = D3D_SHADER_MODEL_6_4;
    models[7] = D3D_SHADER_MODEL_6_3;
    models[8] = D3D_SHADER_MODEL_6_2;
    models[9] = D3D_SHADER_MODEL_6_1;
    models[10] = D3D_SHADER_MODEL_6_0;

    // find the highest supported shader model
    HRESULT result = 0;
    D3D_SHADER_MODEL highestModel = D3D_SHADER_MODEL_5_1;
    for (int i = 0; i < 11; i++) {
        shaderModel.HighestShaderModel = models[i];
        result = ID3D12Device_CheckFeatureSupport(
            d3d12Adapter->tmpDevice, 
            D3D12_FEATURE_SHADER_MODEL, 
            &shaderModel, 
            sizeof(shaderModel));

        if (shaderModel.HighestShaderModel >= models[i] && result == S_OK) {
            highestModel = models[i];
            break;
        }
    }

    if (highestModel == D3D_SHADER_MODEL_5_1) {
        return 0;
    }

    if (highestModel >= D3D_SHADER_MODEL_6_10) {
        return PAL_MAKE_SHADER_TARGET(6, 10);

    } else if (highestModel >= D3D_SHADER_MODEL_6_9) {
        return PAL_MAKE_SHADER_TARGET(6, 9);

    } else if (highestModel >= D3D_SHADER_MODEL_6_8) {
        return PAL_MAKE_SHADER_TARGET(6, 8);

    } else if (highestModel >= D3D_SHADER_MODEL_6_6) {
        return PAL_MAKE_SHADER_TARGET(6, 6);

    } else if (highestModel >= D3D_SHADER_MODEL_6_5) {
        return PAL_MAKE_SHADER_TARGET(6, 5);

    } else if (highestModel >= D3D_SHADER_MODEL_6_4) {
        return PAL_MAKE_SHADER_TARGET(6, 4);

    } else if (highestModel >= D3D_SHADER_MODEL_6_3) {
        return PAL_MAKE_SHADER_TARGET(6, 3);

    } else if (highestModel >= D3D_SHADER_MODEL_6_2) {
        return PAL_MAKE_SHADER_TARGET(6, 2);

    } else if (highestModel >= D3D_SHADER_MODEL_6_1) {
        return PAL_MAKE_SHADER_TARGET(6, 1);

    } else if (highestModel >= D3D_SHADER_MODEL_6_0) {
        return PAL_MAKE_SHADER_TARGET(6, 0);
    }

    return 0;
}

PalResult PAL_CALL enumerateFormatsD3D12(
    PalAdapter* adapter,
    int32_t* count,
    PalFormatInfo* outFormats)
{
    int32_t fmtCount = 0;
    HRESULT result;
    AdapterD3D12* d3d12Adapter = (AdapterD3D12*)adapter;
    ID3D12Device* device = d3d12Adapter->tmpDevice;
    D3D12_FEATURE_DATA_FORMAT_SUPPORT support = {0};

    for (int i = 0; i < PAL_FORMAT_COUNT; i++) {
        DXGI_FORMAT fmt = formatToD3D12((PalFormat)i);
        if (fmt == DXGI_FORMAT_UNKNOWN) {
            continue;
        }

        support.Format = fmt;
        result = ID3D12Device_CheckFeatureSupport(
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

PalBool PAL_CALL isFormatSupportedD3D12(
    PalAdapter* adapter,
    PalFormat format)
{
    HRESULT result;
    AdapterD3D12* d3d12Adapter = (AdapterD3D12*)adapter;
    ID3D12Device* device = d3d12Adapter->tmpDevice;
    D3D12_FEATURE_DATA_FORMAT_SUPPORT support = {0};

    DXGI_FORMAT fmt = formatToD3D12(format);
    if (fmt == DXGI_FORMAT_UNKNOWN) {
        return PAL_FALSE;
    }

    support.Format = fmt;
    result = ID3D12Device_CheckFeatureSupport(
        device, 
        D3D12_FEATURE_FORMAT_SUPPORT, 
        &support, 
        sizeof(support));

    if (FAILED(result) || (support.Support1 == 0 && support.Support2 == 0)) {
        return PAL_FALSE;
    }

    return PAL_TRUE;
}

PalImageUsages PAL_CALL queryFormatImageUsagesD3D12(
    PalAdapter* adapter,
    PalFormat format)
{
    HRESULT result;
    AdapterD3D12* d3d12Adapter = (AdapterD3D12*)adapter;
    ID3D12Device* device = d3d12Adapter->tmpDevice;
    D3D12_FEATURE_DATA_FORMAT_SUPPORT support = {0};

    DXGI_FORMAT fmt = formatToD3D12(format);
    if (fmt == DXGI_FORMAT_UNKNOWN) {
        return 0;
    }

    support.Format = fmt;
    result = ID3D12Device_CheckFeatureSupport(
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

PalSampleCount PAL_CALL queryFormatSampleCountD3D12(
    PalAdapter* adapter,
    PalFormat format)
{
    HRESULT result;
    AdapterD3D12* d3d12Adapter = (AdapterD3D12*)adapter;
    ID3D12Device* device = d3d12Adapter->tmpDevice;
    D3D12_FEATURE_DATA_FORMAT_SUPPORT support = {0};

    DXGI_FORMAT fmt = formatToD3D12(format);
    if (fmt == DXGI_FORMAT_UNKNOWN) {
        return PAL_FALSE;
    }

    support.Format = fmt;
    result = ID3D12Device_CheckFeatureSupport(
        device, 
        D3D12_FEATURE_FORMAT_SUPPORT, 
        &support, 
        sizeof(support));

    if (FAILED(result)) {
        return PAL_FALSE;
    }

    if (support.Support1 == 0 && support.Support2 == 0) {
        // format not supported
        return PAL_FALSE;
    }

    // check sample count
    UINT sampleCounts[] = {64, 32, 16, 8, 4, 2};
    D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS samples = {0};
    samples.Format = fmt;

    uint32_t tmp = 0;
    for (int i = 0; i < 6; i++) {
        samples.SampleCount = sampleCounts[i];
        result = ID3D12Device_CheckFeatureSupport(
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

#endif // PAL_HAS_D3D12_BACKEND