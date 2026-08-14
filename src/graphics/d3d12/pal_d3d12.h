
/**
    PAL - Prime Abstraction Layer
    Copyright (C) 2025
    Licensed under the Zlib license. See LICENSE file in root.
 */

#ifndef _PAL_D3D12_H
#define _PAL_D3D12_H

#if PAL_HAS_D3D12_BACKEND
#include "graphics/pal_linear_allocator.h"
#include "pal2/pal_graphics.h"
#include <d3d12.h>
#include <d3d12sdklayers.h>
#include <dxgi1_6.h>
#include <windows.h>

#define MAX_RTV 1024
#define MAX_DSV 512
#define TEXTURE_PITCH 256
#define MAX_ATTACHMENTS 8
#define GRAPHICS_PIPELINE 1220
#define COMPUTE_PIPELINE 1221
#define RAY_TRACING_PIPELINE 1222

#define DESC_TYPE_RTV 0
#define DESC_TYPE_DSV 1
#define DESC_TYPE_SRV 2
#define DESC_TYPE_UAV 3

typedef HRESULT(WINAPI* PFN_CreateDXGIFactory2)(
    UINT,
    REFIID,
    void**);

typedef HRESULT(__stdcall* PFN_D3D12SerializeVersionedRootSignature)(
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
    void* scratchBuffer;
    uint32_t shaderModel;
    PalBool canFenceReset;
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
    PalPipelineStages supportedStages;
    PalQueueType type;
    ID3D12Fence* fence;
    HANDLE fenceEvent;
    DeviceD3D12* device;
    ID3D12CommandQueue* handle;
} QueueD3D12;

typedef struct {
    void* reserved;
    PalMemoryType type;
    ID3D12Heap* handle;
} MemoryD3D12;

typedef struct {
    void* reserved;
    HWND handle;
} SurfaceD3D12;

typedef struct {
    void* reserved;
    PalBool isMemoryManaged;
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
    UINT presentFlags;
    SurfaceD3D12* surface;
    ImageD3D12* images;
    ID3D12CommandQueue* queue;
    DeviceD3D12* device;
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
    D3D12_COMMAND_LIST_TYPE type;
} CommandPoolD3D12;

typedef struct {
    void* reserved;
    PalBool primary;
    ID3D12Resource* stagingBuffer;
    ID3D12Resource* buffer;
    ID3D12CommandAllocator* allocator;
    PalLinearAllocator linearAllocator;
    void* pipeline;
    DeviceD3D12* device;
    ID3D12GraphicsCommandList6* handle;
} CommandBufferD3D12;

typedef struct {
    void* reserved;
    PalBool isMemoryManaged;
    PalBool canStateChange;
    PalBufferUsages usages;
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
    void* stagingPtr;
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
DXGI_FORMAT formatToD3D12(PalFormat format);
D3D12_COMPARISON_FUNC compareOpToD3D12(PalCompareOp op);

D3D12_SHADING_RATE_COMBINER combinerOpsToD3D12(PalFragmentShadingRateCombinerOp op);
D3D12_SHADING_RATE shadingRateToD3D12(PalFragmentShadingRate rate);
uint32_t getFormatSizeD3D12(PalFormat format);
DXGI_FORMAT vertexTypeToD3D12(PalVertexType type);

D3D12_RAYTRACING_INSTANCE_FLAGS instanceFlagsToD3D12(PalAccelerationStructureInstanceFlags flags);
uint32_t samplesToD3D12(PalSampleCount count);

void fillBuildInfoD3D12(
    PalBool getBuildSize,
    PalAccelerationStructureBuildInfo* info,
    D3D12_RAYTRACING_GEOMETRY_DESC* geometries,
    D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_DESC* buildInfo);

void getDescriptorTierLimitsD3D12(
    void* device,
    PalResourceCapabilities* caps,
    PalDescriptorIndexingCapabilities* descCaps);

void fillSubresourceD3D12(
    uint32_t descType,
    PalImageViewType type,
    const PalImageSubresourceRange* range,
    void* desc);

uint64_t getDescriptorHandleD3D12(
    uint32_t index,
    uint32_t size,
    uint64_t baseOffset);

void pollMessagesD3D12(DeviceD3D12* device);

// IIDs
extern IID IID_Device;
extern IID IID_Adapter;
extern IID IID_Factory;
extern IID IID_Debug;
extern IID IID_Debug1;
extern IID IID_InfoQueue;
extern IID IID_Heap;
extern IID IID_Queue;
extern IID IID_Swapchain;
extern IID IID_CommandAllocator;
extern IID IID_CommandList;
extern IID IID_CommandList6;
extern IID IID_CommandSignature;
extern IID IID_DescriptorHeap;
extern IID IID_RootSignature;
extern IID IID_Device5;
extern IID IID_PipelineState;
extern IID IID_StateObject;
extern IID IID_Resource;
extern IID IID_StateObjectProps;
extern IID IID_Fence;

extern D3D12 s_D3D12;

#endif // PAL_HAS_D3D12_BACKEND
#endif // _PAL_D3D12_H