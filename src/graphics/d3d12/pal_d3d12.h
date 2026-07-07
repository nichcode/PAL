
/**
    PAL - Prime Abstraction Layer
    Copyright (C) 2025
    Licensed under the Zlib license. See LICENSE file in root.
 */

#ifndef _PAL_D3D12_H
#define _PAL_D3D12_H

#if PAL_HAS_D3D12_BACKEND
#define COBJMACROS
#include "pal/pal_graphics.h"
#include <windows.h>
#include <d3d12.h>
#include <dxgi1_6.h>
#include <d3d12sdklayers.h>

#define MAX_RTV 1024
#define MAX_DSV 512

typedef HRESULT (WINAPI* PFN_CreateDXGIFactory2)(
    UINT,
    REFIID,
    void**);

typedef HRESULT (__stdcall *PFN_D3D12SerializeVersionedRootSignature)(
    const D3D12_VERSIONED_ROOT_SIGNATURE_DESC*,
    ID3DBlob**,
    ID3DBlob**);

typedef struct {
    uint32_t incrementSize;
    uint32_t freeTop;
    uint64_t baseOffset;
    ID3D12DescriptorHeap* heap;
    uint32_t freeList[MAX_RTV];
} RTVHeapAllocator;

typedef struct {
    uint32_t incrementSize;
    uint32_t freeTop;
    uint64_t baseOffset;
    ID3D12DescriptorHeap* heap;
    uint32_t freeList[MAX_DSV];
} DSVHeapAllocator;

typedef struct {
    uint32_t freeComputeQueues;
    uint32_t freeGraphicsQueues;
    uint32_t freeCopyQueues;
    uint32_t maxVertexLayouts;
    uint32_t maxVertexAttributes;

    uint32_t maxAnisotropy;
    uint32_t maxPushConstantSize;
    uint32_t maxTessellationPatchPoint;

    uint32_t maxDescriptorSampledImages;
    uint32_t maxDescriptorStorageImages;
    uint32_t maxDescriptorSamplers;
    uint32_t maxDescriptorStorageBuffers;
    uint32_t maxDescriptorUniformBuffers;
    uint32_t maxBoundDescriptorSets;

    uint32_t maxRecursionDepth;
    uint32_t maxHitAttributeSize;
    uint32_t maxPayloadSize;
    uint32_t maxDispatchInvocations;
    uint32_t maxDescriptorAccelerationStructures;
} DeviceLimits;

typedef struct {
    uint32_t patchControlPoints;
    PalShaderStage stage;
    wchar_t entryName[PAL_SHADER_ENTRY_NAME_SIZE];
} ShaderEntry;

typedef struct {
    PalBool isHitGroup;
    PalShaderStage stage;
    wchar_t entryName[PAL_SHADER_ENTRY_NAME_SIZE];
} ShaderExport;

typedef struct {
    wchar_t entryName[PAL_SHADER_ENTRY_NAME_SIZE];
    D3D12_HIT_GROUP_DESC desc;
} RayHitGroup;

typedef struct {
    uint32_t raygenCount;
    uint32_t raygenDataSize;
    uint32_t missCount;
    uint32_t missDataSize;
    uint32_t hitCount;
    uint32_t hitDataSize;
    uint32_t callableCount;
    uint32_t callableDataSize;
} ShaderBindingTableInfo;

typedef struct {
    uint32_t incrementSize;
    uint32_t nextOffset;
    uint64_t cpuBase;
    uint64_t gpuBase;
    ID3D12DescriptorHeap* handle;
} DescriptorHeap;

typedef struct {
    uint32_t maxUniformBuffers;
    uint32_t maxSampledImages;
    uint32_t maxStorageBuffers;
    uint32_t maxSamplers;
    uint32_t maxStorageImages;
    uint32_t maxAs;

    uint32_t usedUniformBuffers;
    uint32_t usedSampledImages;
    uint32_t usedStorageBuffers;
    uint32_t usedSamplers;
    uint32_t usedStorageImages;
    uint32_t usedAs;
} DescriptorHeapLimits;

typedef struct {
    uint32_t incrementSize;
    uint32_t nextOffset;
    uint64_t cpuBase;
    uint64_t gpuBase;
    ID3D12DescriptorHeap* handle;
} DescriptorHeap;

typedef struct {
    uint32_t maxUniformBuffers;
    uint32_t maxSampledImages;
    uint32_t maxStorageBuffers;
    uint32_t maxSamplers;
    uint32_t maxStorageImages;
    uint32_t maxAs;

    uint32_t usedUniformBuffers;
    uint32_t usedSampledImages;
    uint32_t usedStorageBuffers;
    uint32_t usedSamplers;
    uint32_t usedStorageImages;
    uint32_t usedAs;
} DescriptorHeapLimits;

typedef struct {
    PalDescriptorType type;
    D3D12_DESCRIPTOR_RANGE1 range;
} DescriptorSetBinding;

typedef struct {
    uint32_t startIndex;
    uint32_t offset;
    D3D12_GPU_VIRTUAL_ADDRESS_RANGE_AND_STRIDE region;
} AddressRegion;

typedef struct {
    void* reserved;
    D3D_FEATURE_LEVEL level;
    ID3D12Device* tmpDevice;
    IDXGIAdapter4* handle;
} AdapterD3D12;

typedef struct {
    void* reserved;
    uint32_t shaderModel;
    PalAdapterFeatures features;
    IDXGIAdapter4* adapter;
    ID3D12CommandSignature* meshSignature;
    ID3D12CommandSignature* drawIndexedSignature;
    ID3D12CommandSignature* drawSignature;
    ID3D12CommandSignature* dispatchSignature;
    ID3D12CommandSignature* raySignature;
    ID3D12InfoQueue* infoQueue;
    ID3D12CommandQueue* queue;
    ID3D12Device5* handle;
    RTVHeapAllocator rtvAllocator;
    DSVHeapAllocator dsvAllocator;
    DeviceLimits limits;
} DeviceD3D12;

typedef struct {
    void* reserved;
    uint32_t fenceValue;
    PalQueueType type;
    ID3D12Fence* fence;
    ID3D12CommandQueue* handle;
} QueueD3D12;

typedef struct {
    void* reserved;
    HWND handle;
} SurfaceD3D12;

typedef struct {
    void* reserved;
    DeviceD3D12* device;
    ID3D12Resource* handle;
    PalImageInfo info;
    D3D12_RESOURCE_DESC desc;
} ImageD3D12;

typedef struct {
    void* reserved;
    uint32_t heapIndex;
    PalImageViewType type;
    DXGI_FORMAT format;
    ImageD3D12* image;
    DeviceD3D12* device;
    PalImageSubresourceRange range;
} ImageViewD3D12;

typedef struct {
    void* reserved;
    D3D12_SAMPLER_DESC desc;
} SamplerD3D12;

typedef struct {
    void* reserved;
    uint32_t imageCount;
    uint32_t syncInterval;
    uint32_t windowWidth;
    uint32_t windowHeight;
    uint32_t flags;
    DXGI_FORMAT format;
    DXGI_FEATURE presentFlags;
    SurfaceD3D12* surface;
    ImageD3D12* images;
    DeviceD3D12* device;
    ID3D12CommandQueue* queue;
    IDXGISwapChain3* handle;
} SwapchainD3D12;

typedef struct {
    void* reserved;
    PalBool isTimeline;
    PalBool canReset;
    UINT64 value;
    ID3D12Fence* handle;
    HANDLE event;
} FenceD3D12, SemaphoreD3D12;

typedef struct {
    void* reserved;
    uint32_t entryCount;
    D3D12_SHADER_BYTECODE byteCode;
    ShaderEntry* entries;
} ShaderD3D12;

typedef struct {
    void* reserved;
    PalBool primary;
    void* pool; // CommandPool
    DeviceD3D12* device;
    ID3D12Resource* stagingBuffer;
    ID3D12Resource* buffer;
    ID3D12CommandAllocator* allocator;
    void* pipeline;
    ID3D12GraphicsCommandList6* handle;
} CommandBufferD3D12;

typedef struct {
    PalBool used;
    CommandBufferD3D12* cmdBuffer;
} CommandBufferData;

typedef struct {
    void* reserved;
    uint32_t size;
    D3D12_COMMAND_LIST_TYPE type;
    CommandBufferData* cmdBuffersData;
} CommandPoolD3D12;

typedef struct {
    void* reserved;
    PalBool supportsAddress;
    PalBool canChangeState;
    PalBool hasIndirect;
    PalBool isAccelerationStructure;
    PalBool isScratch;
    uint64_t size;
    ID3D12Resource* handle;
    DeviceD3D12* device;
    D3D12_RESOURCE_DESC desc;
} BufferD3D12;

typedef struct {
    void* reserved;
    PalAccelerationStructureType type;
    D3D12_GPU_VIRTUAL_ADDRESS address;
    ID3D12Resource* handle;
} AccelerationStructureD3D12;

typedef struct {
    void* reserved;
    uint32_t constantIndex;
    ID3D12RootSignature* handle;
} PipelineLayoutD3D12;

typedef struct {
    void* reserved;
    PalBool hasFsr;
    uint32_t type;
    uint32_t shaderExportCount;
    D3D12_SHADING_RATE shadingRate;
    D3D_PRIMITIVE_TOPOLOGY topology;
    uint32_t* strides;
    void* handle;
    ShaderExport* shaderExports;
    PipelineLayoutD3D12* layout;
    ID3D12RootSignature* localRootSignature;
    D3D12_SHADING_RATE_COMBINER combinerOps[2];
    ShaderBindingTableInfo sbtInfo;
} PipelineD3D12;

typedef struct {
    void* reserved;
    PalBool isDirty;
    uint32_t stagingBufferSize;
    uint32_t handleSize;
    ID3D12Resource* buffer;
    ID3D12Resource* stagingBuffer;
    D3D12_GPU_VIRTUAL_ADDRESS baseAddress;
    PipelineD3D12* pipeline;

    AddressRegion raygen;
    AddressRegion miss;
    AddressRegion hit;
    AddressRegion callable;
} ShaderBindingTableD3D12;

typedef struct {
    void* reserved;
    PalDescriptorIndexingFlags flags;
    uint32_t bindingCount;
    uint32_t samplerCount;
    DescriptorSetBinding* bindings;
} DescriptorSetLayoutD3D12;

typedef struct {
    void* reserved;
    uint32_t resourceOffset;
    uint32_t samplerOffset;
    DescriptorSetLayoutD3D12* layout;
    void* pool; // DescriptorPool
} DescriptorSetD3D12;

typedef struct {
    void* reserved;
    PalDescriptorIndexingFlags flags;
    PalBool hasResourceHeap;
    PalBool hasSamplerHeap;
    uint32_t maxSets;
    uint32_t usedSets;
    DescriptorHeapLimits limits;
    DescriptorHeap resourceHeap;
    DescriptorHeap samplerHeap;
    DescriptorSetD3D12* sets;
} DescriptorPoolD3D12;

typedef struct {
    PalBool debugLayer;
    uint32_t adapterCount;
    uint32_t severityCount;
    uint32_t categoryCount;
    HMODULE handle;
    HMODULE dxgi;
    AdapterD3D12* adapters;
    IDXGIFactory6* factory;

    PFN_D3D12_CREATE_DEVICE createDevice;
    PFN_CreateDXGIFactory2 createDXGIFactory;
    PFN_D3D12_GET_DEBUG_INTERFACE getDebugInterface;
    PFN_D3D12SerializeVersionedRootSignature serializeVersionedRootSignature;

    PalDebugCallback debugCallback;
    void* debugUserData;
    const PalAllocator* allocator;
    D3D12_MESSAGE_SEVERITY severities[6];
    D3D12_MESSAGE_CATEGORY categories[12];
} D3D12;

PalResult makeResultD3D12(HRESULT result);
void pollMessagesD3D12(DeviceD3D12* device);
DXGI_FORMAT formatToD3D12(PalFormat format);
PalImageUsages ImageUsageFromD3D12(D3D12_FORMAT_SUPPORT1 flags);

void fillBuildInfoD3D12(
    PalBool getBuildSize,
    PalAccelerationStructureBuildInfo* info,
    D3D12_RAYTRACING_GEOMETRY_DESC* geometries,
    D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_DESC* buildInfo);

void getDescriptorTierLimitsD3D12(
    void* device, 
    PalResourceCapabilities* caps, 
    PalDescriptorIndexingCapabilities* descCaps);

extern D3D12 s_D3D12;

#endif // PAL_HAS_D3D12_BACKEND
#endif // _PAL_D3D12_H