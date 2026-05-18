
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

#define MAX_ATTACHMENTS 32
#define TEXTURE_PITCH 256
#define MAX_RTV 1024
#define MAX_DSV 512
#define MAX_SCRATCH_BUFFER_SIZE 65536 * sizeof(wchar_t)
#define MAX_MESSAGE_SIZE 4096

#define GRAPHICS_PIPELINE 1220
#define COMPUTE_PIPELINE 1221
#define RAY_TRACING_PIPELINE 1222

#if defined(_MSC_VER)
 #define ALIGN_STREAM __declspec(align(sizeof(void*)))
#elif defined(__GNUC__) || defined(__clang__)
 #define ALIGN_STREAM __attribute__((aligned(sizeof(void*))))
#else
 #define ALIGN_STREAM
#endif // _MSC_VER

// clang-format off
// IIDS
const IID IID_Device = {0xc4fec28f, 0x7966, 0x4e95, 0x9f,0x94, 0xf4,0x31,0xcb,0x56,0xc3,0xb8};
const IID IID_Adapter = {0x3c8d99d1, 0x4fbf, 0x4181, 0xa8,0x2c, 0xaf,0x66,0xbf,0x7b,0xd2,0x4e};
const IID IID_Factory = {0xc1b6694f, 0xff09, 0x44a9, 0xb0,0x3c, 0x77,0x90,0x0a,0x0a,0x1d,0x17};
const IID IID_DebugController = {0x344488b7, 0x6846, 0x474b, 0xb9,0x89, 0xf0,0x27,0x44,0x82,0x45,0xe0};
const IID IID_DebugController1 = {0xaffaa4ca, 0x63fe, 0x4d8e, 0xb8,0xad, 0x15,0x90,0x00,0xaf,0x43,0x04};
const IID IID_InfoQueue = {0x0742a90b, 0xc387, 0x483f, 0xb9,0x46, 0x30,0xa7,0xe4,0xe6,0x14,0x58};
const IID IID_Heap = {0x6b3b2502, 0x6e51, 0x45b3, 0x90,0xee, 0x98,0x84,0x26,0x5e,0x8d,0xf3};
const IID IID_Queue = {0x0ec870a6, 0x5d7e, 0x4c22, 0x8c,0xfc, 0x5b,0xaa,0xe0,0x76,0x16,0xed};
const IID IID_Fence = {0x0a753dcf, 0xc4d8, 0x4b91, 0xad,0xf6, 0xbe,0x5a,0x60,0xd9,0x5a,0x76};
const IID IID_Resource = {0x696442be, 0xa72e, 0x4059, 0xbc,0x79, 0x5b,0x5c,0x98,0x04,0x0f,0xad};
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
const IID IID_StateObjectProps = {0xde5fa827, 0x9bf9, 0x4f26, 0x89,0xff, 0xd7,0xf5,0x6f,0xde,0x38,0x60};
// clang-format on

typedef HRESULT (WINAPI* PFN_CreateDXGIFactory2)(
    UINT,
    REFIID,
    void**);

typedef HRESULT (__stdcall *PFN_D3D12SerializeVersionedRootSignature)(
    const D3D12_VERSIONED_ROOT_SIGNATURE_DESC*,
    ID3DBlob**,
    ID3DBlob**);

typedef struct {
    const PalGraphicsBackend* backend;

    D3D_FEATURE_LEVEL level;
    ID3D12Device* tmpDevice;
    IDXGIAdapter4* handle;
} Adapter;

typedef struct {
    bool debugLayer;
    Uint32 adapterCount;
    Uint32 severityCount;
    Uint32 categoryCount;
    HMODULE handle;
    HMODULE dxgi;
    Adapter* adapters;
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

typedef struct {
    Uint32 incrementSize;
    Uint32 freeTop;
    Uint64 baseOffset;
    ID3D12DescriptorHeap* heap;
    Uint32 freeList[MAX_RTV];
} RTVHeapAllocator;

typedef struct {
    Uint32 incrementSize;
    Uint32 freeTop;
    Uint64 baseOffset;
    ID3D12DescriptorHeap* heap;
    Uint32 freeList[MAX_DSV];
} DSVHeapAllocator;

// Limits we must enforce ourselves
typedef struct {
    Uint32 freeComputeQueues;
    Uint32 freeGraphicsQueues;
    Uint32 freeCopyQueues;
    Uint32 maxVertexLayouts;
    Uint32 maxVertexAttributes;

    Uint32 maxAnisotropy;
    Uint32 maxPushConstantSize;
    Uint32 maxTessellationPatchPoint;

    Uint32 maxDescriptorSampledImages;
    Uint32 maxDescriptorStorageImages;
    Uint32 maxDescriptorSamplers;
    Uint32 maxDescriptorStorageBuffers;
    Uint32 maxDescriptorUniformBuffers;
    Uint32 maxBoundDescriptorSets;

    Uint32 maxRecursionDepth;
    Uint32 maxHitAttributeSize;
    Uint32 maxPayloadSize;
    Uint32 maxDispatchInvocations;
    Uint32 maxDescriptorAccelerationStructures;
} DeviceLimits;

typedef struct {
    const PalGraphicsBackend* backend;

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
} Device;

typedef struct {
    const PalGraphicsBackend* backend;

    Uint32 fenceValue;
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
    Device* device;
    ID3D12Resource* handle;
    PalImageInfo info;
    D3D12_RESOURCE_DESC desc;
} Image;

typedef struct {
    const PalGraphicsBackend* backend;

    Uint32 heapIndex;
    PalImageViewType type;
    DXGI_FORMAT format;
    Image* image;
    Device* device;
    PalImageSubresourceRange range;
} ImageView;

typedef struct {
    const PalGraphicsBackend* backend;

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
    Device* device;
    ID3D12CommandQueue* queue;
    IDXGISwapChain3* handle;
} Swapchain;

typedef struct {
    const PalGraphicsBackend* backend;

    bool isTimeline;
    bool canReset;
    UINT64 value;
    ID3D12Fence* handle;
} Fence, Semaphore;

typedef struct {
    const PalGraphicsBackend* backend;

    Uint32 patchControlPoints;
    PalShaderStage stage;
    D3D12_SHADER_BYTECODE byteCode;
    wchar_t entryName[PAL_SHADER_ENTRY_NAME_SIZE];
} Shader;

typedef struct {
    const PalGraphicsBackend* backend;

    bool primary;
    void* pool; // CommandPool
    Device* device;
    ID3D12Resource* stagingBuffer;
    ID3D12Resource* buffer;
    ID3D12CommandAllocator* allocator;
    void* pipeline;
    ID3D12GraphicsCommandList6* handle;
} CommandBuffer;

typedef struct {
    bool used;
    CommandBuffer* cmdBuffer;
} CommandBufferData;

typedef struct {
    const PalGraphicsBackend* backend;

    Uint32 size;
    D3D12_COMMAND_LIST_TYPE type;
    CommandBufferData* cmdBuffersData;
} CommandPool;

typedef struct {
    const PalGraphicsBackend* backend;

    bool supportsAddress;
    bool canChangeState;
    bool hasIndirect;
    Uint64 size;
    ID3D12Resource* handle;
    Device* device;
    D3D12_RESOURCE_DESC desc;
} Buffer;

typedef struct {
    const PalGraphicsBackend* backend;

    PalAccelerationStructureType type;
    D3D12_GPU_VIRTUAL_ADDRESS address;
    ID3D12Resource* handle;
} AccelerationStructure;

typedef struct {
    const PalGraphicsBackend* backend;

    Uint32 constantIndex;
    ID3D12RootSignature* handle;
} PipelineLayout;

typedef struct {
    PalShaderStage stage;
    ID3D12RootSignature* localRoot;
    wchar_t entryName[PAL_SHADER_ENTRY_NAME_SIZE];
} ShaderExport;

typedef struct {
    wchar_t entryName[PAL_SHADER_ENTRY_NAME_SIZE];
    D3D12_HIT_GROUP_DESC desc;
} RayHitGroup;

typedef struct {
    D3D12_EXPORT_DESC export;
    D3D12_DXIL_LIBRARY_DESC library;
} RayShaderLibary;

typedef struct {
    Uint32 raygenCount;
    Uint32 raygenDataSize;
    Uint32 missCount;
    Uint32 missDataSize;
    Uint32 hitCount;
    Uint32 hitDataSize;
    Uint32 callableCount;
    Uint32 callableDataSize;
} ShaderBindingTableInfo;

typedef struct {
    const PalGraphicsBackend* backend;

    bool hasFsr;
    Uint32 type;
    D3D12_SHADING_RATE shadingRate;
    D3D_PRIMITIVE_TOPOLOGY topology;
    Uint32* strides;
    void* handle;
    ShaderExport* shaderExports;
    PipelineLayout* layout;
    ID3D12RootSignature* localRootSignature;
    D3D12_SHADING_RATE_COMBINER combinerOps[2];
    ShaderBindingTableInfo sbtInfo;
} Pipeline;

typedef struct {
    Uint32 startIndex;
    Uint32 offset;
    D3D12_GPU_VIRTUAL_ADDRESS_RANGE_AND_STRIDE region;
} AddressRegion;

typedef struct {
    const PalGraphicsBackend* backend;

    bool isDirty;
    Uint32 stagingBufferSize;
    Uint32 handleSize;
    ID3D12Resource* buffer;
    ID3D12Resource* stagingBuffer;
    D3D12_GPU_VIRTUAL_ADDRESS baseAddress;
    Pipeline* pipeline;

    AddressRegion raygen;
    AddressRegion miss;
    AddressRegion hit;
    AddressRegion callable;
} ShaderBindingTable;

typedef struct {
    PalDescriptorType type;
    D3D12_DESCRIPTOR_RANGE1 range;
} DescriptorSetBinding;

typedef struct {
    const PalGraphicsBackend* backend;

    Uint32 bindingCount;
    Uint32 samplerCount;
    DescriptorSetBinding* bindings;
} DescriptorSetLayout;

typedef struct {
    Uint32 incrementSize;
    Uint32 nextOffset;
    Uint64 cpuBase;
    Uint64 gpuBase;
    ID3D12DescriptorHeap* handle;
} DescriptorHeap;

typedef struct {
    Uint32 maxUniformBuffers;
    Uint32 maxSampledImages;
    Uint32 maxStorageBuffers;
    Uint32 maxSamplers;
    Uint32 maxStorageImages;
    Uint32 maxAs;

    Uint32 usedUniformBuffers;
    Uint32 usedSampledImages;
    Uint32 usedStorageBuffers;
    Uint32 usedSamplers;
    Uint32 usedStorageImages;
    Uint32 usedAs;
} DescriptorHeapLimits;

typedef struct {
    const PalGraphicsBackend* backend;

    Uint32 resourceOffset;
    Uint32 samplerOffset;
    DescriptorSetLayout* layout;
    void* pool; // DescriptorPool
} DescriptorSet;

typedef struct {
    const PalGraphicsBackend* backend;

    bool hasResourceHeap;
    bool hasSamplerHeap;
    Uint32 maxSets;
    Uint32 usedSets;
    DescriptorHeapLimits limits;
    DescriptorHeap resourceHeap;
    DescriptorHeap samplerHeap;
    DescriptorSet* sets;
} DescriptorPool;

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

static D3D12 s_D3D = {0};

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
            pool->cmdBuffersData[i].used = true;
            return &pool->cmdBuffersData[i];
        }
    }

    // resize the data array
    CommandBufferData* data = nullptr;
    int count = pool->size * 2; // double the size
    int freeIndex = pool->size + 1;

    data = palAllocate(s_D3D.allocator, sizeof(CommandBufferData) * count, 0);
    if (data) {
        memcpy(data, pool->cmdBuffersData, pool->size * sizeof(CommandBufferData));

        palFree(s_D3D.allocator, pool->cmdBuffersData);
        pool->cmdBuffersData = data;
        pool->size = count;

        pool->cmdBuffersData[freeIndex].used = true;
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

static bool fillBuildInfoD3D12(
    PalAccelerationStructureBuildInfo* info,
    D3D12_RAYTRACING_GEOMETRY_DESC* geometries,
    D3D12_GPU_VIRTUAL_ADDRESS srcAs,
    D3D12_GPU_VIRTUAL_ADDRESS dstAs,
    D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_DESC* buildInfo)
{
    static Uint32 maxInstanceCount = 1000000;
    static Uint32 maxPrimitiveCount = 10000000;
    static Uint32 maxGeometryCount = 100000;

    if (info->geometryCount > maxGeometryCount) {
        return false;
    }

    if (info->instanceCount > maxInstanceCount) {
        return false;
    }

    for (int i = 0; i < info->geometryCount; i++) {
        if (info->geometries[i].primitiveCount > maxPrimitiveCount) {
            return false;
        }

        D3D12_RAYTRACING_GEOMETRY_DESC* tmp = &geometries[i];
        tmp->Flags = 0;
        if (info->geometries[i].flags & PAL_GEOMETRY_FLAG_OPAQUE) {
            tmp->Flags |= D3D12_RAYTRACING_GEOMETRY_FLAG_OPAQUE;
        }

        if (info->geometries[i].flags & PAL_GEOMETRY_FLAG_NO_DUPLICATE_ANYHIT) {
            tmp->Flags |= D3D12_RAYTRACING_GEOMETRY_FLAG_NO_DUPLICATE_ANYHIT_INVOCATION;
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
                tmp->Triangles.IndexFormat = DXGI_FORMAT_R16_FLOAT;
            } else {
                tmp->Triangles.IndexFormat = DXGI_FORMAT_R32_FLOAT;
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
    
    return true;
}

static Uint32 getFormatSizeD3D12(PalFormat format)
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

static inline Uint32 alignD3D12(
    Uint32 value,
    Uint32 alignment)
{
    return (value + alignment - 1) & ~(alignment - 1);
}

static D3D12_RESOURCE_STATES barrierToD3D12(
    Uint32 stageCount,
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
            rtvDesc->ViewDimension = D3D12_SRV_DIMENSION_TEXTURE1D;

        } else if (type == PAL_IMAGE_VIEW_TYPE_1D_ARRAY) {
            rtvDesc->Texture1DArray.MipSlice = range->startMipLevel;
            rtvDesc->Texture1DArray.FirstArraySlice = range->startArrayLayer;
            rtvDesc->Texture1DArray.ArraySize = range->layerArrayCount;
            rtvDesc->ViewDimension = D3D12_SRV_DIMENSION_TEXTURE1DARRAY;

        } else if (type == PAL_IMAGE_VIEW_TYPE_2D) {
            rtvDesc->Texture2D.MipSlice = range->startMipLevel;
            rtvDesc->ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;

        } else if (type == PAL_IMAGE_VIEW_TYPE_2D_ARRAY) {
            rtvDesc->Texture2DArray.MipSlice = range->startMipLevel;
            rtvDesc->Texture2DArray.FirstArraySlice = range->startArrayLayer;
            rtvDesc->Texture2DArray.ArraySize = range->layerArrayCount;
            rtvDesc->ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2DARRAY;

        } else if (type == PAL_IMAGE_VIEW_TYPE_3D) {
            rtvDesc->Texture3D.MipSlice = range->startMipLevel;
            rtvDesc->Texture3D.FirstWSlice = range->startArrayLayer;
            rtvDesc->Texture3D.WSize = range->layerArrayCount;
            rtvDesc->ViewDimension = D3D12_SRV_DIMENSION_TEXTURE3D;
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
            dsvDesc->ViewDimension = D3D12_SRV_DIMENSION_TEXTURE1D;

        } else if (type == PAL_IMAGE_VIEW_TYPE_1D_ARRAY) {
            dsvDesc->Texture1DArray.MipSlice = range->startMipLevel;
            dsvDesc->Texture1DArray.FirstArraySlice = range->startArrayLayer;
            dsvDesc->Texture1DArray.ArraySize = range->layerArrayCount;
            dsvDesc->ViewDimension = D3D12_SRV_DIMENSION_TEXTURE1DARRAY;

        } else if (type == PAL_IMAGE_VIEW_TYPE_2D) {
            dsvDesc->Texture2D.MipSlice = range->startMipLevel;
            dsvDesc->ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;

        } else if (type == PAL_IMAGE_VIEW_TYPE_2D_ARRAY) {
            dsvDesc->Texture2DArray.MipSlice = range->startMipLevel;
            dsvDesc->Texture2DArray.FirstArraySlice = range->startArrayLayer;
            dsvDesc->Texture2DArray.ArraySize = range->layerArrayCount;
            dsvDesc->ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2DARRAY;
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
            uavDesc->ViewDimension = D3D12_SRV_DIMENSION_TEXTURE1D;

        } else if (type == PAL_IMAGE_VIEW_TYPE_1D_ARRAY) {
            uavDesc->Texture1DArray.MipSlice = range->startMipLevel;
            uavDesc->Texture1DArray.FirstArraySlice = range->startArrayLayer;
            uavDesc->Texture1DArray.ArraySize = range->layerArrayCount;
            uavDesc->ViewDimension = D3D12_SRV_DIMENSION_TEXTURE1DARRAY;

        } else if (type == PAL_IMAGE_VIEW_TYPE_2D) {
            uavDesc->Texture2D.MipSlice = range->startMipLevel;
            uavDesc->ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;

        } else if (type == PAL_IMAGE_VIEW_TYPE_2D_ARRAY) {
            uavDesc->Texture2DArray.MipSlice = range->startMipLevel;
            uavDesc->Texture2DArray.FirstArraySlice = range->startArrayLayer;
            uavDesc->Texture2DArray.ArraySize = range->layerArrayCount;
            uavDesc->ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2DARRAY;

        } else if (type == PAL_IMAGE_VIEW_TYPE_3D) {
            uavDesc->Texture3D.MipSlice = range->startMipLevel;
            uavDesc->Texture3D.FirstWSlice = range->startArrayLayer;
            uavDesc->Texture3D.WSize = range->layerArrayCount;
            uavDesc->ViewDimension = D3D12_SRV_DIMENSION_TEXTURE3D;
        }
        return;
    }
}

static inline Uint64 getDescriptorHandleD3D12(
    Uint32 index,
    Uint32 size,
    Uint64 baseOffset)
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

static D3D_PRIMITIVE_TOPOLOGY getPatchTopology(Uint32 patch)
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

static Uint32 getVertexTypeSizeD3D12(PalVertexType type)
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
    Uint32 index,
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

    Uint8* tmpBuffer[MAX_MESSAGE_SIZE];
    UINT64 messageCount = queue->lpVtbl->GetNumStoredMessages(queue);
    for (int i = 0; i < messageCount; i++) {
        SIZE_T size = 0;
        queue->lpVtbl->GetMessageA(queue, i, nullptr, &size);
        if (size > sizeof(tmpBuffer)) {
            size = sizeof(tmpBuffer);
        }

        queue->lpVtbl->GetMessageA(queue, i, (D3D12_MESSAGE*)tmpBuffer, &size);
        const char* message = ((D3D12_MESSAGE*)tmpBuffer)->pDescription;
        D3D12_MESSAGE_CATEGORY category = ((D3D12_MESSAGE*)tmpBuffer)->Category;
        D3D12_MESSAGE_SEVERITY severity = ((D3D12_MESSAGE*)tmpBuffer)->Severity;

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

        s_D3D.debugCallback(s_D3D.debugUserData, msgSeverity, msgType, message);
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
    sbt->isDirty = false;
}

static void getDescriptorLimitsD3D12(
    void* device, 
    PalAdapterCapabilities* caps, 
    PalDescriptorIndexingCapabilities* descCaps)
{
    D3D12_FEATURE_DATA_D3D12_OPTIONS options = {0};
    if (caps) {
        ID3D12Device* handle = device;
        handle->lpVtbl->CheckFeatureSupport(
            handle,
            D3D12_FEATURE_D3D12_OPTIONS,
            &options,
            sizeof(options));

    } else {
        ID3D12Device5* handle = device;
        handle->lpVtbl->CheckFeatureSupport(
            handle,
            D3D12_FEATURE_D3D12_OPTIONS,
            &options,
            sizeof(options));
    }

    if (options.ResourceBindingTier == D3D12_RESOURCE_BINDING_TIER_1) {
        // safe defaults
        if (caps) {
            caps->maxPerStageDescriptorSampledImages = 1024;
            caps->maxDescriptorSetSampledImages = 1024;
            caps->maxPerStageDescriptorStorageImages = 512;
            caps->maxDescriptorSetStorageImages = 512;

            caps->maxPerStageDescriptorSamplers = 256;
            caps->maxDescriptorSetSamplers = 256;
            caps->maxPerStageDescriptorStorageBuffers = 512;
            caps->maxDescriptorSetStorageBuffers = 512;

            caps->maxPerStageDescriptorUniformBuffers = 256;
            caps->maxDescriptorSetUniformBuffers = 256;
            caps->maxBoundDescriptorSets = 30;

        } else {
            descCaps->maxPerStageBindlessDescriptorSampledImages = 1024;
            descCaps->maxDescriptorSetBindlessSampledImages = 1024;
            descCaps->maxPerStageBindlessDescriptorStorageImages = 512;
            descCaps->maxDescriptorSetBindlessStorageImages = 512;

            descCaps->maxPerStageBindlessDescriptorSamplers = 256;
            descCaps->maxDescriptorSetBindlessSamplers = 256;
            descCaps->maxPerStageBindlessDescriptorStorageBuffers = 512;
            descCaps->maxDescriptorSetBindlessStorageBuffers = 512;

            descCaps->maxPerStageBindlessDescriptorUniformBuffers = 256;
            descCaps->maxDescriptorSetBindlessUniformBuffers = 256;
        }

    } else {
        // safe defaults
        if (caps) {
            caps->maxPerStageDescriptorSampledImages = 4096;
            caps->maxDescriptorSetSampledImages = 4096;
            caps->maxPerStageDescriptorStorageImages = 1024;
            caps->maxDescriptorSetStorageImages = 1024;

            caps->maxPerStageDescriptorSamplers = 512;
            caps->maxDescriptorSetSamplers = 512;
            caps->maxPerStageDescriptorStorageBuffers = 2048;
            caps->maxDescriptorSetStorageBuffers = 2048;

            caps->maxPerStageDescriptorUniformBuffers = 256;
            caps->maxDescriptorSetUniformBuffers = 256;
            caps->maxBoundDescriptorSets = 30;

        } else {
            descCaps->maxPerStageBindlessDescriptorSampledImages = 4096;
            descCaps->maxDescriptorSetBindlessSampledImages = 4096;
            descCaps->maxPerStageBindlessDescriptorStorageImages = 1024;
            descCaps->maxDescriptorSetBindlessStorageImages = 1024;

            descCaps->maxPerStageBindlessDescriptorSamplers = 512;
            descCaps->maxDescriptorSetBindlessSamplers = 512;
            descCaps->maxPerStageBindlessDescriptorStorageBuffers = 2048;
            descCaps->maxDescriptorSetBindlessStorageBuffers = 2048;

            descCaps->maxPerStageBindlessDescriptorUniformBuffers = 256;
            descCaps->maxDescriptorSetBindlessUniformBuffers = 256;
        }
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
    s_D3D.handle = LoadLibraryA("d3d12.dll");
    s_D3D.dxgi = LoadLibraryA("dxgi.dll");
    if (!s_D3D.handle || !s_D3D.dxgi) {
        return PAL_RESULT_PLATFORM_FAILURE;
    }

    // clang-format off
    s_D3D.createDevice = (PFN_D3D12_CREATE_DEVICE)GetProcAddress(
        s_D3D.handle,
        "D3D12CreateDevice");

    s_D3D.createDXGIFactory = (PFN_CreateDXGIFactory2)GetProcAddress(
        s_D3D.dxgi,
        "CreateDXGIFactory2");

    s_D3D.serializeVersionedRootSignature = (PFN_D3D12SerializeVersionedRootSignature)GetProcAddress(
        s_D3D.handle,
        "D3D12SerializeVersionedRootSignature");

    if (debugger && debugger->callback) {
        s_D3D.getDebugInterface = (PFN_D3D12_GET_DEBUG_INTERFACE)GetProcAddress(
            s_D3D.handle,
            "D3D12GetDebugInterface");

        if (s_D3D.getDebugInterface) {
            HRESULT hr;
            ID3D12Debug* debugController = nullptr;
            ID3D12Debug1* debugController1 = nullptr;

            hr = s_D3D.getDebugInterface(&IID_DebugController, (void**)&debugController);
            if (SUCCEEDED(hr)) {
                debugController->lpVtbl->EnableDebugLayer(debugController);
                debugController->lpVtbl->Release(debugController);
            }

            hr = s_D3D.getDebugInterface(&IID_DebugController1, (void**)&debugController1);
            if (SUCCEEDED(hr)) {
                debugController1->lpVtbl->SetEnableGPUBasedValidation(
                    debugController1,
                    TRUE);

                debugController1->lpVtbl->Release(debugController1);

                // message types
                if (!debugger->denyGeneral) {
                    s_D3D.categories[s_D3D.categoryCount++] = D3D12_MESSAGE_CATEGORY_INITIALIZATION;
                    s_D3D.categories[s_D3D.categoryCount++] = D3D12_MESSAGE_CATEGORY_CLEANUP;
                    s_D3D.categories[s_D3D.categoryCount++] = D3D12_MESSAGE_CATEGORY_COMPILATION;
                }

                if (!debugger->denyPerformance) {
                    s_D3D.categories[s_D3D.categoryCount++] = D3D12_MESSAGE_CATEGORY_RESOURCE_MANIPULATION;
                    s_D3D.categories[s_D3D.categoryCount++] = D3D12_MESSAGE_CATEGORY_EXECUTION;
                    s_D3D.categories[s_D3D.categoryCount++] = D3D12_MESSAGE_CATEGORY_SHADER;
                }

                if (!debugger->denyValidation) {
                    s_D3D.categories[s_D3D.categoryCount++] = D3D12_MESSAGE_CATEGORY_STATE_CREATION;
                    s_D3D.categories[s_D3D.categoryCount++] = D3D12_MESSAGE_CATEGORY_STATE_GETTING;
                    s_D3D.categories[s_D3D.categoryCount++] = D3D12_MESSAGE_CATEGORY_STATE_SETTING;
                }

                // message severities
                if (!debugger->denyInfoSeverity) {
                    s_D3D.severities[s_D3D.severityCount++] = D3D12_MESSAGE_SEVERITY_INFO;
                }

                if (!debugger->denyWarningSeverity) {
                    s_D3D.severities[s_D3D.severityCount++] = D3D12_MESSAGE_SEVERITY_WARNING;
                }

                if (!debugger->denyErrorSeverity) {
                    s_D3D.severities[s_D3D.severityCount++] = D3D12_MESSAGE_SEVERITY_ERROR;
                    s_D3D.severities[s_D3D.severityCount++] = D3D12_MESSAGE_SEVERITY_CORRUPTION;
                }
            }
            s_D3D.debugLayer = true;
            s_D3D.debugCallback = debugger->callback;
        }
    }
    // clang-format on

    s_D3D.factory = nullptr;
    s_D3D.adapters = nullptr;
    s_D3D.adapterCount = 0;
    HRESULT result = s_D3D.createDXGIFactory(0, &IID_Factory, (void**)&s_D3D.factory);
    if (FAILED(result)) {
        return PAL_RESULT_PLATFORM_FAILURE;
    }

    s_D3D.allocator = allocator;
    return PAL_RESULT_SUCCESS;
}

void PAL_CALL shutdownGraphicsD3D12()
{
    for (int i = 0; i < s_D3D.adapterCount; i++) {
        s_D3D.adapters[i].handle->lpVtbl->Release(s_D3D.adapters[i].handle);
    }

    s_D3D.factory->lpVtbl->Release(s_D3D.factory);
    FreeLibrary(s_D3D.handle);
    FreeLibrary(s_D3D.dxgi);

    if (s_D3D.adapters) {
        palFree(s_D3D.allocator, s_D3D.adapters);
    }
    memset(&s_D3D, 0, sizeof(s_D3D));
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

    if (s_D3D.adapters) {
        palFree(s_D3D.allocator, s_D3D.adapters);
    }

    while (SUCCEEDED(s_D3D.factory->lpVtbl->EnumAdapters(
        s_D3D.factory,
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
                HRESULT result = s_D3D.createDevice(
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
        s_D3D.adapters = palAllocate(s_D3D.allocator, sizeof(Adapter) * adapterCount, 0);
        if (!s_D3D.adapters) {
            return PAL_RESULT_OUT_OF_MEMORY;
        }

        // fill the array with Adapter structs
        for (int i = 0; i < *count; i++) {
            Adapter* tmp = &s_D3D.adapters[i];
            tmp->handle = dxAdapters[i];
            tmp->tmpDevice = devices[i];
            tmp->level = deviceLevels[i];
            outAdapters[i] = (PalAdapter*)tmp;
        }
        s_D3D.adapterCount = *count;

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
    ID3D12Device* device = d3dAdapter->tmpDevice;

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

    return PAL_RESULT_SUCCESS;
}

PalResult PAL_CALL getAdapterCapabilitiesD3D12(
    PalAdapter* adapter,
    PalAdapterCapabilities* caps)
{
    // always supported
    caps->sampledImageDynamicArrayIndexing = true;
    caps->storageImageDynamicArrayIndexing = true;
    caps->storageBufferDynamicArrayIndexing = true;
    caps->uniformBufferDynamicArrayIndexing = true;

    caps->maxComputeQueues = 2; // safe default
    caps->maxGraphicsQueues = 2; // safe default
    caps->maxCopyQueues = 2; // safe default

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
    caps->maxUniformBufferSize = D3D12_REQ_IMMEDIATE_CONSTANT_BUFFER_ELEMENT_COUNT * 16;
    caps->maxStorageBufferSize = 2147483648; // 2 GIB
    caps->maxPushConstantSize = 256;

    caps->maxVertexLayouts = 32; // safe
    caps->maxVertexAttributes = 32; // safe
    caps->maxTessellationPatchPoint = 32;

    caps->maxComputeWorkGroupInvocations = D3D12_CS_THREAD_GROUP_MAX_THREADS_PER_GROUP;
    caps->maxComputeWorkGroupCount[0] = D3D12_CS_DISPATCH_MAX_THREAD_GROUPS_PER_DIMENSION;
    caps->maxComputeWorkGroupCount[1] = D3D12_CS_DISPATCH_MAX_THREAD_GROUPS_PER_DIMENSION;
    caps->maxComputeWorkGroupCount[2] = D3D12_CS_DISPATCH_MAX_THREAD_GROUPS_PER_DIMENSION;

    caps->maxComputeWorkGroupSize[0] = D3D12_CS_THREAD_GROUP_MAX_X;
    caps->maxComputeWorkGroupSize[1] = D3D12_CS_THREAD_GROUP_MAX_Y;
    caps->maxComputeWorkGroupSize[2] = D3D12_CS_THREAD_GROUP_MAX_Z;

    Adapter* d3dAdapter = (Adapter*)adapter;
    getDescriptorLimitsD3D12(d3dAdapter->tmpDevice, caps, nullptr);
    return PAL_RESULT_SUCCESS;
}

PalAdapterFeatures PAL_CALL getAdapterFeaturesD3D12(PalAdapter* adapter)
{
    HRESULT result;
    Adapter* d3dAdapter = (Adapter*)adapter;
    IDXGIAdapter4* adapterHandle = d3dAdapter->handle;
    PalAdapterFeatures features = 0;
    ID3D12Device* device = d3dAdapter->tmpDevice;

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
        features |= PAL_ADAPTER_FEATURE_INDIRECT_DRAW_MESH;
        features |= PAL_ADAPTER_FEATURE_INDIRECT_DRAW_MESH_COUNT;
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
    features |= PAL_ADAPTER_FEATURE_INDIRECT_DRAW_COUNT;
    features |= PAL_ADAPTER_FEATURE_IMAGE_VIEW_CUBE_ARRAY;
    features |= PAL_ADAPTER_FEATURE_DESCRIPTOR_INDEXING;

    if (d3dAdapter->level >= D3D_FEATURE_LEVEL_12_0) {
        features |= PAL_ADAPTER_FEATURE_DEPTH_STENCIL_RESOLVE;
    }
    return features;
}

Uint32 PAL_CALL getHighestSupportedShaderTargetD3D12(
    PalAdapter* adapter, 
    PalShaderFormats shaderFormat)
{
    if (shaderFormat != PAL_SHADER_FORMAT_DXIL) {
        return 0;
    }

    Adapter* d3dAdapter = (Adapter*)adapter;
    D3D12_FEATURE_DATA_SHADER_MODEL shaderModel = {0};

    D3D_SHADER_MODEL models[8];
    models[0] = D3D_SHADER_MODEL_6_7;
    models[1] = D3D_SHADER_MODEL_6_6;
    models[2] = D3D_SHADER_MODEL_6_5;
    models[3] = D3D_SHADER_MODEL_6_4;
    models[4] = D3D_SHADER_MODEL_6_3;
    models[5] = D3D_SHADER_MODEL_6_2;
    models[6] = D3D_SHADER_MODEL_6_1;
    models[7] = D3D_SHADER_MODEL_6_0;

    // find the highest supported shader model
    HRESULT result = 0;
    D3D_SHADER_MODEL highestModel = D3D_SHADER_MODEL_5_1;
    for (int i = 0; i < 8; i++) {
        shaderModel.HighestShaderModel = models[i];
        result = d3dAdapter->tmpDevice->lpVtbl->CheckFeatureSupport(
            d3dAdapter->tmpDevice,
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

    if (highestModel >= D3D_SHADER_MODEL_6_7) {
        return PAL_MAKE_SHADER_TARGET(6, 7);

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
    bool valid = (adapterFeatures & features) == features;
    if (!valid) {
        return PAL_RESULT_ADAPTER_FEATURE_NOT_SUPPORTED;
    }

    device = palAllocate(s_D3D.allocator, sizeof(Device), 0);
    if (!device) {
        return PAL_RESULT_OUT_OF_MEMORY;
    }

    ID3D12Device* tmpDevice = nullptr;
    memset(device, 0, sizeof(Device));
    DeviceLimits* limits = &device->limits;

    result = s_D3D.createDevice(
        (IUnknown*)d3dAdapter->handle,
        d3dAdapter->level,
        &IID_Device,
        (void**)&tmpDevice);

    if (FAILED(result)) {
        palFree(s_D3D.allocator, device);
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
    if (s_D3D.debugLayer) {
        result = device->handle->lpVtbl->QueryInterface(
            device->handle,
            &IID_InfoQueue,
            (void**)&device->infoQueue);

        if (SUCCEEDED(result)) {
            D3D12_MESSAGE_ID denyIDs[] = { D3D12_MESSAGE_ID_MAP_INVALID_NULLRANGE };
            D3D12_INFO_QUEUE_FILTER filter = {0};
            filter.AllowList.NumSeverities = s_D3D.severityCount;
            filter.AllowList.pSeverityList = s_D3D.severities;
            filter.AllowList.NumCategories = s_D3D.categoryCount;
            filter.AllowList.pCategoryList = s_D3D.categories;
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
        // disptach indexed
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
    Device* d3dDevice = (Device*)device;
    if (d3dDevice->meshSignature) {
        d3dDevice->meshSignature->lpVtbl->Release(d3dDevice->meshSignature);
    }

    if (d3dDevice->raySignature) {
        d3dDevice->raySignature->lpVtbl->Release(d3dDevice->raySignature);
    }

    if (d3dDevice->dispatchSignature) {
        d3dDevice->dispatchSignature->lpVtbl->Release(d3dDevice->dispatchSignature);
    }

    if (d3dDevice->drawIndexedSignature) {
        d3dDevice->drawIndexedSignature->lpVtbl->Release(d3dDevice->drawIndexedSignature);
    }

    if (d3dDevice->drawSignature) {
        d3dDevice->drawSignature->lpVtbl->Release(d3dDevice->drawSignature);
    }

    d3dDevice->rtvAllocator.heap->lpVtbl->Release(d3dDevice->rtvAllocator.heap);
    d3dDevice->dsvAllocator.heap->lpVtbl->Release(d3dDevice->dsvAllocator.heap);

    d3dDevice->queue->lpVtbl->Release(d3dDevice->queue);
    d3dDevice->handle->lpVtbl->Release(d3dDevice->handle);
    if (d3dDevice->infoQueue) {
        d3dDevice->infoQueue->lpVtbl->Release(d3dDevice->infoQueue);
    }

    palFree(s_D3D.allocator, d3dDevice);
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
    Device* d3dDevice = (Device*)device;
    ID3D12Heap* memory = nullptr;

    D3D12_HEAP_DESC desc = {0};
    desc.SizeInBytes = size;

    desc.Properties.Type = D3D12_HEAP_TYPE_DEFAULT;
    if (type == PAL_MEMORY_TYPE_CPU_READBACK) {
        desc.Properties.Type = D3D12_HEAP_TYPE_READBACK;

    } else if (type == PAL_MEMORY_TYPE_CPU_UPLOAD) {
        desc.Properties.Type = D3D12_HEAP_TYPE_UPLOAD;
    }

    result = d3dDevice->handle->lpVtbl->CreateHeap(
        d3dDevice->handle,
        &desc,
        &IID_Heap,
        (void**)&memory);

    if (FAILED(result)) {
        pollMessagesD3D12(d3dDevice);
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
    Device* d3dDevice = (Device*)device;
    if (!(d3dDevice->features & PAL_ADAPTER_FEATURE_SAMPLER_ANISOTROPY)) {
        return PAL_RESULT_ADAPTER_FEATURE_NOT_SUPPORTED;
    }

    caps->maxAnisotropy = 16; // default on most d3d12 hardwares
    return PAL_RESULT_SUCCESS;
}

PalResult PAL_CALL queryMultiViewCapabilitiesD3D12(
    PalDevice* device,
    PalMultiViewCapabilities* caps)
{
    Device* d3dDevice = (Device*)device;
    if (!(d3dDevice->features & PAL_ADAPTER_FEATURE_SAMPLER_ANISOTROPY)) {
        return PAL_RESULT_ADAPTER_FEATURE_NOT_SUPPORTED;
    }

    caps->maxMultiViews = D3D12_MAX_VIEW_INSTANCE_COUNT;
    return PAL_RESULT_SUCCESS;
}

PalResult PAL_CALL queryMultiViewportCapabilitiesD3D12(
    PalDevice* device,
    PalMultiViewportCapabilities* caps)
{
    Device* d3dDevice = (Device*)device;
    if (!(d3dDevice->features & PAL_ADAPTER_FEATURE_SAMPLER_ANISOTROPY)) {
        return PAL_RESULT_ADAPTER_FEATURE_NOT_SUPPORTED;
    }

    caps->maxViewports = D3D12_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE;
    return PAL_RESULT_SUCCESS;
}

PalResult PAL_CALL queryDepthStencilCapabilitiesD3D12(
    PalDevice* device,
    PalDepthStencilCapabilities* caps)
{
    Device* d3dDevice = (Device*)device;
    if (!(d3dDevice->features & PAL_ADAPTER_FEATURE_SAMPLER_ANISOTROPY)) {
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
    Device* d3dDevice = (Device*)device;
    if (!(d3dDevice->features & PAL_ADAPTER_FEATURE_FRAGMENT_SHADING_RATE)) {
        return PAL_RESULT_ADAPTER_FEATURE_NOT_SUPPORTED;
    }

    // these are supported if fragment shading rate feature is
    caps->shadingRates[PAL_FRAGMENT_SHADING_RATE_1X1] = true;
    caps->shadingRates[PAL_FRAGMENT_SHADING_RATE_1X2] = true;
    caps->shadingRates[PAL_FRAGMENT_SHADING_RATE_2X1] = true;
    caps->shadingRates[PAL_FRAGMENT_SHADING_RATE_2X2] = true;

    D3D12_FEATURE_DATA_D3D12_OPTIONS6 options = {0};
    d3dDevice->handle->lpVtbl->CheckFeatureSupport(
        d3dDevice->handle,
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
    Device* d3dDevice = (Device*)device;
    if (!(d3dDevice->features & PAL_ADAPTER_FEATURE_MESH_SHADER)) {
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
    Device* d3dDevice = (Device*)device;
    if (!(d3dDevice->features & PAL_ADAPTER_FEATURE_RAY_TRACING)) {
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

    caps->maxDescriptorSetAccelerationStructures = 4;
    caps->maxDescriptorSetBindlessAccelerationStructures = 4;
    return PAL_RESULT_SUCCESS;
}

PalResult PAL_CALL queryDescriptorIndexingCapabilitiesD3D12(
    PalDevice* device,
    PalDescriptorIndexingCapabilities* caps)
{
    Device* d3dDevice = (Device*)device;
    if (!(d3dDevice->features & PAL_ADAPTER_FEATURE_DESCRIPTOR_INDEXING)) {
        return PAL_RESULT_ADAPTER_FEATURE_NOT_SUPPORTED;
    }

    caps->runtimeDescriptorArray = true; // always supported
    caps->variableDescriptorCount = true; // manually
    caps->partiallyBoundDescriptors = true; // always supported

    // always supported
    caps->sampledImageNonUniformIndexing = true;
    caps->sampledImageUpdateAfterBind = true;
    caps->storageImageNonUniformIndexing = true;
    caps->storageImageUpdateAfterBind = true;
    caps->storageBufferNonUniformIndexing = true;
    caps->storageBufferUpdateAfterBind = true;
    caps->uniformBufferNonUniformIndexing = true;
    caps->uniformBufferUpdateAfterBind = true;

    getDescriptorLimitsD3D12(d3dDevice->handle, nullptr, caps);
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
    Device* d3dDevice = (Device*)device;
    Queue* queue = nullptr;
    D3D12_COMMAND_QUEUE_DESC desc = {0};

    switch (type) {
        case PAL_QUEUE_TYPE_COMPUTE: {
            desc.Type = D3D12_COMMAND_LIST_TYPE_COMPUTE;

            if (!d3dDevice->limits.freeComputeQueues) {
                return PAL_RESULT_OUT_OF_QUEUE;
            }
            d3dDevice->limits.freeComputeQueues--;
            break;
        }

        case PAL_QUEUE_TYPE_GRAPHICS: {
            desc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;

            if (!d3dDevice->limits.freeGraphicsQueues) {
                return PAL_RESULT_OUT_OF_QUEUE;
            }
            d3dDevice->limits.freeGraphicsQueues--;
            break;
        }

        case PAL_QUEUE_TYPE_COPY: {
            desc.Type = D3D12_COMMAND_LIST_TYPE_COPY;

            if (!d3dDevice->limits.freeCopyQueues) {
                return PAL_RESULT_OUT_OF_QUEUE;
            }
            d3dDevice->limits.freeCopyQueues--;
            break;
        }
    }

    queue = palAllocate(s_D3D.allocator, sizeof(Queue), 0);
    if (!queue) {
        return PAL_RESULT_OUT_OF_MEMORY;
    }

    result = d3dDevice->handle->lpVtbl->CreateCommandQueue(
        d3dDevice->handle,
        &desc,
        &IID_Queue,
        (void**)&queue->handle);

    if (FAILED(result)) {
        pollMessagesD3D12(d3dDevice);
        palFree(s_D3D.allocator, queue);
        if (result == E_OUTOFMEMORY) {
            return PAL_RESULT_OUT_OF_MEMORY;
        }
        return PAL_RESULT_PLATFORM_FAILURE;
    }

    // create fence used for queue wait
    result = d3dDevice->handle->lpVtbl->CreateFence(
        d3dDevice->handle,
        0,
        0,
        &IID_Fence,
        (void**)&queue->fence);

    if (FAILED(result)) {
        pollMessagesD3D12(d3dDevice);
        palFree(s_D3D.allocator, queue);
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
    palFree(s_D3D.allocator, d3dQueue);
}

PalResult PAL_CALL waitQueueD3D12(PalQueue* queue)
{
    Queue* d3dQueue = (Queue*)queue;
    ID3D12Fence* fence = d3dQueue->fence;

    // wait on the fence if the submited work is not done
    if (fence->lpVtbl->GetCompletedValue(fence) < d3dQueue->fenceValue) {
        HANDLE event = CreateEvent(nullptr, FALSE, FALSE, nullptr);
        fence->lpVtbl->SetEventOnCompletion(fence, d3dQueue->fenceValue, event);
        WaitForSingleObject(event, INFINITE);
        CloseHandle(event);
    }
    return PAL_RESULT_SUCCESS;
}

bool PAL_CALL canQueuePresentD3D12(
    PalQueue* queue,
    PalSurface* surface)
{
    Queue* d3dQueue = (Queue*)queue;
    if (d3dQueue->type == PAL_QUEUE_TYPE_GRAPHICS) {
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
    Adapter* d3dAdapter = (Adapter*)adapter;
    ID3D12Device* device = d3dAdapter->tmpDevice;
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
    Adapter* d3dAdapter = (Adapter*)adapter;
    ID3D12Device* device = d3dAdapter->tmpDevice;
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
    Adapter* d3dAdapter = (Adapter*)adapter;
    ID3D12Device* device = d3dAdapter->tmpDevice;
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

PalSampleCount PAL_CALL queryFormatSampleCountD3D12(
    PalAdapter* adapter,
    PalFormat format)
{
    HRESULT result;
    Adapter* d3dAdapter = (Adapter*)adapter;
    ID3D12Device* device = d3dAdapter->tmpDevice;
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
    Device* d3dDevice = (Device*)device;

    image = palAllocate(s_D3D.allocator, sizeof(Image), 0);
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

    image->device = d3dDevice;
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
    palFree(s_D3D.allocator, d3dImage);
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
    Device* d3dDevice = (Device*)device;
    Image* d3dImage = (Image*)image;

    if (info->type == PAL_IMAGE_VIEW_TYPE_CUBE_ARRAY) {
        if (!(d3dDevice->features & PAL_ADAPTER_FEATURE_IMAGE_VIEW_CUBE_ARRAY)) {
            return PAL_RESULT_ADAPTER_FEATURE_NOT_SUPPORTED;
        }
    }

    imageView = palAllocate(s_D3D.allocator, sizeof(ImageView), 0);
    if (!imageView) {
        return PAL_RESULT_OUT_OF_MEMORY;
    }

    imageView->heapIndex = UINT32_MAX;
    bool hasRTV = (d3dImage->desc.Flags & D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET);
    bool hasDSV = (d3dImage->desc.Flags & D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL);

    imageView->format = formatToD3D12(info->format);
    if (info->subresourceRange.aspect == PAL_IMAGE_ASPECT_COLOR && hasRTV) {
        RTVHeapAllocator* allocator = &d3dDevice->rtvAllocator;
        Uint32 index = allocator->freeTop;
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
        d3dDevice->handle->lpVtbl->CreateRenderTargetView(
            d3dDevice->handle,
            d3dImage->handle,
            &desc,
            dst);

    } else if (info->subresourceRange.aspect != PAL_IMAGE_ASPECT_COLOR && hasDSV) {
        DSVHeapAllocator* allocator = &d3dDevice->dsvAllocator;
        Uint32 index = allocator->freeTop;
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
        d3dDevice->handle->lpVtbl->CreateDepthStencilView(
            d3dDevice->handle,
            d3dImage->handle,
            &desc,
            dst);
    }

    imageView->range = info->subresourceRange;
    imageView->type = info->type;
    imageView->image = d3dImage;

    imageView->device = d3dDevice;
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
    palFree(s_D3D.allocator, d3dImageView);
}

// ==================================================
// Sampler
// ==================================================

PalResult PAL_CALL createSamplerD3D12(
    PalDevice* device,
    const PalSamplerCreateInfo* info,
    PalSampler** outSampler)
{
    Device* d3dDevice = (Device*)device;
    if (info->maxAnisotropy > d3dDevice->limits.maxAnisotropy) {
        return PAL_RESULT_INVALID_ARGUMENT;
    }

    Sampler* sampler = nullptr;
    sampler = palAllocate(s_D3D.allocator, sizeof(Sampler), 0);
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
    Sampler* d3dSampler = (Sampler*)sampler;
    palFree(s_D3D.allocator, d3dSampler);
}

// ==================================================
// Surface
// ==================================================

PalResult PAL_CALL createSurfaceD3D12(
    PalDevice* device,
    PalGraphicsWindow* window,
    PalSurface** outSurface)
{
    Device* d3dDevice = (Device*)device;
    if (!(d3dDevice->features & PAL_ADAPTER_FEATURE_SWAPCHAIN)) {
        return PAL_RESULT_ADAPTER_FEATURE_NOT_SUPPORTED;
    }

    Surface* surface = nullptr;
    surface = palAllocate(s_D3D.allocator, sizeof(Surface), 0);
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
    palFree(s_D3D.allocator, d3dSurface);
}

PalResult PAL_CALL getSurfaceCapabilitiesD3D12(
    PalDevice* device,
    PalSurface* surface,
    PalSurfaceCapabilities* caps)
{
    HRESULT result;
    Surface* d3dSurface = (Surface*)surface;
    Device* d3dDevice = (Device*)device;
    bool supportHDR10 = false;
    IDXGISwapChain1* swapchain1 = nullptr;
    IDXGISwapChain3* swapchain3 = nullptr;

    if (!(d3dDevice->features & PAL_ADAPTER_FEATURE_SWAPCHAIN)) {
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

    result = s_D3D.factory->lpVtbl->CreateSwapChainForHwnd(
        s_D3D.factory,
        (IUnknown*)d3dDevice->queue,
        d3dSurface->handle,
        &desc,
        nullptr,
        nullptr,
        &swapchain1);

    if (FAILED(result)) {
        pollMessagesD3D12(d3dDevice);
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
    s_D3D.factory->lpVtbl->CheckFeatureSupport(
        s_D3D.factory,
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

    caps->compositeAlphas[PAL_COMPOSITE_ALPHA_OPAQUE] = true;
    caps->compositeAlphas[PAL_COMPOSITE_ALPHA_PRE_MULTIPLIED] = false;
    caps->compositeAlphas[PAL_COMPOSITE_ALPHA_POST_MULTIPLIED] = false;

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

    caps->formats[PAL_SURFACE_FORMAT_BGRA8_UNORM_SRGB_NONLINEAR] = false;
    caps->formats[PAL_SURFACE_FORMAT_BGRA8_SRGB_NONLINEAR] = false;
    caps->formats[PAL_SURFACE_FORMAT_RGBA8_UNORM_SRGB_NONLINEAR] = false;
    caps->formats[PAL_SURFACE_FORMAT_RGBA16_FLOAT_HDR10] = false;

    for (int i = 0; i < PAL_SURFACE_FORMAT_MAX; i++) {
        formatSupport.Format = baseFormats[i];
        result = d3dDevice->handle->lpVtbl->CheckFeatureSupport(
            d3dDevice->handle,
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
    Surface* d3dSurface = (Surface*)surface;
    Device* d3dDevice = (Device*)device;
    Queue* d3dQueue = (Queue*)queue;
    Swapchain* swapchain = nullptr;
    bool isHDRColorspace = false;

    if (!(d3dDevice->features & PAL_ADAPTER_FEATURE_SWAPCHAIN)) {
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

    swapchain = palAllocate(s_D3D.allocator, sizeof(Swapchain), 0);
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
    result = s_D3D.factory->lpVtbl->CreateSwapChainForHwnd(
        s_D3D.factory,
        (IUnknown*)d3dQueue->handle,
        d3dSurface->handle,
        &desc,
        nullptr,
        nullptr,
        &swapchain1);

    if (FAILED(result)) {
        pollMessagesD3D12(d3dDevice);
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
    swapchain->images = palAllocate(s_D3D.allocator, sizeof(Image) * info->imageCount, 0);
    if (!swapchain->imageCount) {
        return PAL_RESULT_OUT_OF_MEMORY;
    }

    // fill all images with the creatio info
    for (int i = 0; i < info->imageCount; i++) {
        ID3D12Resource* tmp = nullptr;
        swapchain->handle->lpVtbl->GetBuffer(swapchain->handle, i, &IID_Resource, (void**)&tmp);

        Image* image = &swapchain->images[i];
        image->belongsToSwapchain = true;
        image->device = d3dDevice;
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

    swapchain->device = d3dDevice;
    swapchain->queue = d3dQueue->handle;
    swapchain->imageCount = info->imageCount;
    *outSwapchain = (PalSwapchain*)swapchain;
    return PAL_RESULT_SUCCESS;
}

void PAL_CALL destroySwapchainD3D12(PalSwapchain* swapchain)
{
    Swapchain* d3dSwapchain = (Swapchain*)swapchain;
    d3dSwapchain->handle->lpVtbl->Release(d3dSwapchain->handle);
    palFree(s_D3D.allocator, d3dSwapchain->images);
    palFree(s_D3D.allocator, d3dSwapchain);
}

PalImage* PAL_CALL getSwapchainImageD3D12(
    PalSwapchain* swapchain,
    Int32 index)
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
    Uint32* outIndex)
{
    Uint32 index = 0;
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
    PalSwapchainPresentInfo* info)
{
    HRESULT result;
    Swapchain* d3dSwapchain = (Swapchain*)swapchain;
    ID3D12CommandQueue* queue = d3dSwapchain->queue;

    if (info->waitSemaphore) {
        Semaphore* semaphore = (Semaphore*)info->waitSemaphore;
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
        Uint32 w, h;
        bool ret = GetClientRect((HWND)d3dSwapchain->surface->handle, &windowRect);
        w = windowRect.right - windowRect.left;
        w = windowRect.bottom - windowRect.top;

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
    Uint32 newWidth,
    Uint32 newHeight)
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
    Device* d3dDevice = (Device*)device;
    void* bytecode = nullptr;

    shader = palAllocate(s_D3D.allocator, sizeof(Shader), 0);
    bytecode = palAllocate(s_D3D.allocator, info->bytecodeSize, 0);
    if (!shader || !bytecode) {
        return PAL_RESULT_OUT_OF_MEMORY;
    }

    // clang-format off
    if (info->stage == PAL_SHADER_STAGE_MESH || 
        info->stage == PAL_SHADER_STAGE_TASK) {
        if (!(d3dDevice->features & PAL_ADAPTER_FEATURE_MESH_SHADER)) {
            return PAL_RESULT_ADAPTER_FEATURE_NOT_SUPPORTED;
        }

    } else if (info->stage == PAL_SHADER_STAGE_GEOMETRY) {
        if (!(d3dDevice->features & PAL_ADAPTER_FEATURE_GEOMETRY_SHADER)) {
            return PAL_RESULT_ADAPTER_FEATURE_NOT_SUPPORTED;
        }

    } else if (info->stage == PAL_SHADER_STAGE_TESSELLATION_CONTROL || 
               info->stage == PAL_SHADER_STAGE_TESSELLATION_EVALUATION) {
        if (!(d3dDevice->features & PAL_ADAPTER_FEATURE_TESSELLATION_SHADER)) {
            return PAL_RESULT_ADAPTER_FEATURE_NOT_SUPPORTED;
        }

    } else if (info->stage == PAL_SHADER_STAGE_RAYGEN ||
            info->stage == PAL_SHADER_STAGE_CLOSEST_HIT ||
            info->stage == PAL_SHADER_STAGE_ANY_HIT ||
            info->stage == PAL_SHADER_STAGE_MISS ||
            info->stage == PAL_SHADER_STAGE_INTERSECTION ||
            info->stage == PAL_SHADER_STAGE_CALLABLE) {
        if (!(d3dDevice->features & PAL_ADAPTER_FEATURE_RAY_TRACING)) {
            return PAL_RESULT_ADAPTER_FEATURE_NOT_SUPPORTED;
        }
    }
    // clang-format on

    convertToWcharD3D12(info->entryName, shader->entryName);
    shader->patchControlPoints = info->patchControlPoints;
    shader->stage = info->stage;

    memcpy(bytecode, info->bytecode, info->bytecodeSize);
    shader->byteCode.pShaderBytecode = bytecode;
    shader->byteCode.BytecodeLength = info->bytecodeSize;
    
    *outShader = (PalShader*)shader;
    return PAL_RESULT_SUCCESS;
}

void PAL_CALL destroyShaderD3D12(PalShader* shader)
{
    Shader* d3dShader = (Shader*)shader;
    palFree(s_D3D.allocator, (void*)d3dShader->byteCode.pShaderBytecode);
    palFree(s_D3D.allocator, d3dShader);
}

// ==================================================
// Fence
// ==================================================

PalResult PAL_CALL createFenceD3D12(
    PalDevice* device,
    bool signaled,
    PalFence** outFence)
{
    HRESULT result;
    Device* d3dDevice = (Device*)device;
    Fence* fence = nullptr;

    fence = palAllocate(s_D3D.allocator, sizeof(Fence), 0);
    if (!fence) {
        return PAL_RESULT_OUT_OF_MEMORY;
    }

    result = d3dDevice->handle->lpVtbl->CreateFence(
        d3dDevice->handle,
        0,
        0,
        &IID_Fence,
        (void**)&fence->handle);

    if (FAILED(result)) {
        pollMessagesD3D12(d3dDevice);
        palFree(s_D3D.allocator, fence);
        if (result == E_OUTOFMEMORY) {
            return PAL_RESULT_OUT_OF_MEMORY;
        }
        return PAL_RESULT_PLATFORM_FAILURE;
    }

    fence->canReset = false;
    if (d3dDevice->features & PAL_ADAPTER_FEATURE_FENCE_RESET) {
        fence->canReset = true;
    }

    fence->isTimeline = false; // for sempaphores
    fence->value = 0;
    *outFence = (PalFence*)fence;
    return PAL_RESULT_SUCCESS;
}

void PAL_CALL destroyFenceD3D12(PalFence* fence)
{
    Fence* d3dFence = (Fence*)fence;
    d3dFence->handle->lpVtbl->Release(d3dFence->handle);
    palFree(s_D3D.allocator, d3dFence);
}

PalResult PAL_CALL waitFenceD3D12(
    PalFence* fence,
    Uint64 timeout)
{
    HRESULT result;
    Fence* d3dFence = (Fence*)fence;
    DWORD ret = 0;
    Uint64 value = d3dFence->value;

    if (d3dFence->handle->lpVtbl->GetCompletedValue(d3dFence->handle) < value) {
        HANDLE event = CreateEvent(nullptr, FALSE, FALSE, nullptr);
        result = d3dFence->handle->lpVtbl->SetEventOnCompletion(
            d3dFence->handle,
            value,
            event);

        if (FAILED(result)) {
            if (result == E_INVALIDARG) {
                return PAL_RESULT_INVALID_FENCE;
            }
            return PAL_RESULT_PLATFORM_FAILURE;
        }

        if (timeout == PAL_INFINITE) {
            ret = WaitForSingleObject(event, INFINITE);
        } else {
            ret = WaitForSingleObject(event, timeout);
        }
        CloseHandle(event);
    }

    if (ret == WAIT_TIMEOUT) {
        return PAL_RESULT_TIMEOUT;
    }

    return PAL_RESULT_SUCCESS;
}

PalResult PAL_CALL resetFenceD3D12(PalFence* fence)
{
    Fence* d3dFence = (Fence*)fence;
    if (!d3dFence->canReset) {
        return PAL_RESULT_ADAPTER_FEATURE_NOT_SUPPORTED;
    }

    d3dFence->handle->lpVtbl->Signal(d3dFence->handle, 0);
    d3dFence->value = 0;
    return PAL_RESULT_SUCCESS;
}

bool PAL_CALL isFenceSignaledD3D12(PalFence* fence)
{
    Fence* d3dFence = (Fence*)fence;
    if (d3dFence->handle->lpVtbl->GetCompletedValue(d3dFence->handle) == 0) {
        return false;
    }
    return true;
}

// ==================================================
// Semaphore
// ==================================================

PalResult PAL_CALL createSemaphoreD3D12(
    PalDevice* device,
    PalSemaphore** outSemaphore)
{
    HRESULT result;
    Device* d3dDevice = (Device*)device;
    Semaphore* semaphore = nullptr;

    semaphore = palAllocate(s_D3D.allocator, sizeof(Semaphore), 0);
    if (!semaphore) {
        return PAL_RESULT_OUT_OF_MEMORY;
    }

    result = d3dDevice->handle->lpVtbl->CreateFence(
        d3dDevice->handle,
        0,
        0,
        &IID_Fence,
        (void**)&semaphore->handle);

    if (FAILED(result)) {
        pollMessagesD3D12(d3dDevice);
        palFree(s_D3D.allocator, semaphore);
        if (result == E_OUTOFMEMORY) {
            return PAL_RESULT_OUT_OF_MEMORY;
        }
        return PAL_RESULT_PLATFORM_FAILURE;
    }

    semaphore->isTimeline = false;
    if (d3dDevice->features & PAL_ADAPTER_FEATURE_TIMELINE_SEMAPHORE) {
        semaphore->isTimeline = true;
    }

    semaphore->canReset = false;
    semaphore->value = 0;
    *outSemaphore = (PalSemaphore*)semaphore;
    return PAL_RESULT_SUCCESS;
}

void PAL_CALL destroySemaphoreD3D12(PalSemaphore* semaphore)
{
    Semaphore* d3dSemaphore = (Semaphore*)semaphore;
    d3dSemaphore->handle->lpVtbl->Release(d3dSemaphore->handle);
    palFree(s_D3D.allocator, d3dSemaphore);
}

PalResult PAL_CALL waitSemaphoreD3D12(
    PalSemaphore* semaphore,
    Uint64 value,
    Uint64 timeout)
{
    HRESULT result;
    DWORD ret = 0;
    Semaphore* d3dSemaphore = (Semaphore*)semaphore;
    if (!d3dSemaphore->isTimeline) {
        return PAL_RESULT_ADAPTER_FEATURE_NOT_SUPPORTED;
    }

    if (d3dSemaphore->handle->lpVtbl->GetCompletedValue(d3dSemaphore->handle) < value) {
        HANDLE event = CreateEvent(nullptr, FALSE, FALSE, nullptr);
        result = d3dSemaphore->handle->lpVtbl->SetEventOnCompletion(
            d3dSemaphore->handle,
            value,
            event);

        if (FAILED(result)) {
            if (result == E_INVALIDARG) {
                return PAL_RESULT_INVALID_SEMAPHORE;
            }
            return PAL_RESULT_PLATFORM_FAILURE;
        }

        if (timeout == PAL_INFINITE) {
            ret = WaitForSingleObject(event, INFINITE);
        } else {
            ret = WaitForSingleObject(event, timeout);
        }
        CloseHandle(event);
    }

    if (ret == WAIT_TIMEOUT) {
        return PAL_RESULT_TIMEOUT;
    }
    return PAL_RESULT_SUCCESS;
}

PalResult PAL_CALL signalSemaphoreD3D12(
    PalSemaphore* semaphore,
    PalQueue* queue,
    Uint64 value)
{
    Semaphore* d3dSemaphore = (Semaphore*)semaphore;
    if (!d3dSemaphore->isTimeline) {
        return PAL_RESULT_ADAPTER_FEATURE_NOT_SUPPORTED;
    }

    HRESULT result = d3dSemaphore->handle->lpVtbl->Signal(d3dSemaphore->handle, value);
    if (FAILED(result)) {
        if (result == E_INVALIDARG) {
            return PAL_RESULT_INVALID_SEMAPHORE;
        }
        return PAL_RESULT_PLATFORM_FAILURE;
    }
    return PAL_RESULT_SUCCESS;
}

PalResult PAL_CALL getSemaphoreValueD3D12(
    PalSemaphore* semaphore,
    Uint64* outValue)
{
    Semaphore* d3dSemaphore = (Semaphore*)semaphore;
    if (!d3dSemaphore->isTimeline) {
        return PAL_RESULT_ADAPTER_FEATURE_NOT_SUPPORTED;
    }

    UINT64 tmp = d3dSemaphore->handle->lpVtbl->GetCompletedValue(d3dSemaphore->handle);
    *outValue = tmp;
    return PAL_RESULT_SUCCESS;
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

    pool = palAllocate(s_D3D.allocator, sizeof(CommandPool), 0);
    if (!pool) {
        return PAL_RESULT_OUT_OF_MEMORY;
    }

    pool->size = 8;
    pool->cmdBuffersData = nullptr;
    Uint32 size =  sizeof(CommandBufferData) * pool->size;
    pool->cmdBuffersData = palAllocate(s_D3D.allocator,  size, 0);
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

    palFree(s_D3D.allocator, cmdPool->cmdBuffersData);
    palFree(s_D3D.allocator, cmdPool);
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
    Device* d3dDevice = (Device*)device;
    CommandBuffer* cmdBuffer = nullptr;
    CommandPool* cmdPool = (CommandPool*)pool;

    cmdBuffer = palAllocate(s_D3D.allocator, sizeof(CommandBuffer), 0);
    if (!cmdBuffer) {
        return PAL_RESULT_OUT_OF_MEMORY;
    }

    cmdBuffer->primary = true;
    D3D12_COMMAND_LIST_TYPE cmdBufferType = cmdPool->type;
    if (type == PAL_COMMAND_BUFFER_TYPE_SECONDARY) {
        cmdBufferType = D3D12_COMMAND_LIST_TYPE_BUNDLE;
        cmdBuffer->primary = false;
    }

    // create an allocator
    result = d3dDevice->handle->lpVtbl->CreateCommandAllocator(
        d3dDevice->handle,
        cmdBufferType,
        &IID_CommandAllocator,
        (void**)&cmdBuffer->allocator);

    if (FAILED(result)) {
        pollMessagesD3D12(d3dDevice);
        if (result == E_OUTOFMEMORY) {
            return PAL_RESULT_OUT_OF_MEMORY;
        }
        return PAL_RESULT_PLATFORM_FAILURE;
    }

    // create the command list
    ID3D12GraphicsCommandList* cmdList = nullptr;
    result = d3dDevice->handle->lpVtbl->CreateCommandList(
        d3dDevice->handle,
        0,
        cmdBufferType,
        cmdBuffer->allocator,
        nullptr,
        &IID_CommandList,
        (void**)&cmdList);

    if (FAILED(result)) {
        pollMessagesD3D12(d3dDevice);
        if (result == E_OUTOFMEMORY) {
            return PAL_RESULT_OUT_OF_MEMORY;
        }
        return PAL_RESULT_PLATFORM_FAILURE;
    }

    // we need a tmp staging and gpu buffer if ray tracing is enabled
    if (d3dDevice->features & PAL_ADAPTER_FEATURE_RAY_TRACING) {
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

        result = d3dDevice->handle->lpVtbl->CreateCommittedResource(
            d3dDevice->handle,
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
        result = d3dDevice->handle->lpVtbl->CreateCommittedResource(
            d3dDevice->handle,
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

    cmdList->lpVtbl->QueryInterface(
        cmdList,
        &IID_CommandList6,
        (void**)&cmdBuffer->handle);

    cmdList->lpVtbl->Release(cmdList);
    cmdBuffer->handle->lpVtbl->Close(cmdBuffer->handle);

    cmdBuffer->pool = cmdPool;
    cmdBuffer->device = d3dDevice;
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

        palFree(s_D3D.allocator, cmdBuffer);
        data->cmdBuffer = nullptr;
        data->used = false;
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

    ID3D12CommandList* tmp = (ID3D12CommandList*)d3dCmdBuffer->handle;
    queueHandle->lpVtbl->ExecuteCommandLists(queueHandle, 1, &tmp);
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
    Uint32 groupCountX,
    Uint32 groupCountY,
    Uint32 groupCountZ)
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
    Uint32 drawCount)
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
    Uint32 maxDrawCount)
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
            s_D3D.allocator,
            sizeof(D3D12_RAYTRACING_GEOMETRY_DESC) * info->geometryCount,
            0);

        if (!geometries) {
            return PAL_RESULT_OUT_OF_MEMORY;
        }

        memset(geometries, 0, sizeof(D3D12_RAYTRACING_GEOMETRY_DESC) * info->geometryCount);
        bool success = fillBuildInfoD3D12(
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

        palFree(s_D3D.allocator, geometries);

    } else {
        bool success = fillBuildInfoD3D12(
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
        Uint32 size = allocator->incrementSize;
        Uint64 base = allocator->baseOffset;
        colorAttachments[i].ptr = getDescriptorHandleD3D12(tmp->heapIndex, size, base);
    }

    if (info->depthStencilAttachment) {
        ImageView* tmp = (ImageView*)info->depthStencilAttachment->imageView;
        DSVHeapAllocator* allocator = &tmp->device->dsvAllocator;
        Uint32 size = allocator->incrementSize;
        Uint64 base = allocator->baseOffset;
        depthStencilAttachment.ptr = getDescriptorHandleD3D12(tmp->heapIndex, size, base);
        depthStencil = &depthStencilAttachment;
    }

    d3dCmdBuffer->handle->lpVtbl->OMSetRenderTargets(
        d3dCmdBuffer->handle,
        info->colorAttachentCount,
        colorAttachments,
        FALSE,
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

    Uint32 imageFormatSize = getFormatSizeD3D12(dst->info.format);
    Uint64 rowPitch = alignD3D12((Uint64)copyInfo->imageWidth * imageFormatSize, TEXTURE_PITCH);
    footPrint->Footprint.RowPitch = (UINT)rowPitch;

    Uint32 planeCount = 1;
    if (copyInfo->imageAspect == PAL_IMAGE_ASPECT_DEPTH_STENCIL) {
        planeCount = 2;
    }

    D3D12_BOX box = {0};
    box.right = copyInfo->imageWidth;
    box.bottom = copyInfo->imageHeight;
    box.back = copyInfo->imageDepth;

    Uint32 level = copyInfo->ImageMipLevel;
    Uint32 startLayer = copyInfo->ImageStartArrayLayer;
    Uint32 layerCount = copyInfo->ImageArrayLayerCount;
    Uint32 maxLayers = dst->info.depthOrArraySize;
    Uint32 maxLevels = dst->info.mipLevelCount;

    for (Uint32 plane = 0; plane < planeCount; plane++) {
        for (Uint32 layer = startLayer; layer < startLayer + layerCount; layer++) {
            Uint32 index = level + (layer * maxLevels) + (plane * maxLevels * maxLayers);

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

    Uint32 planeCount = 1;
    Uint32 layerCount = copyInfo->arrayLayerCount;
    if (copyInfo->aspect == PAL_IMAGE_ASPECT_DEPTH_STENCIL) {
        planeCount = 2;
    }

    Uint32 dstLevel = copyInfo->dstMipLevel;
    Uint32 dstStartLayer = copyInfo->dstStartArrayLayer;
    Uint32 dstMaxLayers = dstImage->info.depthOrArraySize;
    Uint32 dstMaxLevels = dstImage->info.mipLevelCount;

    Uint32 srcLevel = copyInfo->srcMipLevel;
    Uint32 srcStartLayer = copyInfo->srcStartArrayLayer;
    Uint32 srcMaxLayers = srcImage->info.depthOrArraySize;
    Uint32 srcMaxLevels = srcImage->info.mipLevelCount;

    for (Uint32 plane = 0; plane < planeCount; plane++) {
        for (Uint32 layer = 0; layer + layerCount; layer++) {
            // clang-format off
            Uint32 dstIndex = dstLevel + (dstStartLayer + layer * dstMaxLevels) + (plane * dstMaxLevels * dstMaxLayers);
            Uint32 srcIndex = srcLevel + (srcStartLayer + layer * srcMaxLevels) + (plane * srcMaxLevels * srcMaxLayers);
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

    Uint32 imageFormatSize = getFormatSizeD3D12(src->info.format);
    Uint64 rowPitch = alignD3D12((Uint64)copyInfo->imageWidth * imageFormatSize, TEXTURE_PITCH);
    footPrint->Footprint.RowPitch = (UINT)rowPitch;

    D3D12_BOX box = {0};
    box.left = copyInfo->imageOffsetX;
    box.top = copyInfo->imageOffsetY;
    box.front = copyInfo->imageOffsetZ;
    box.right = copyInfo->imageOffsetX + copyInfo->imageWidth;
    box.bottom = copyInfo->imageOffsetY + copyInfo->imageHeight;
    box.back = copyInfo->imageOffsetX + copyInfo->imageDepth;

    Uint32 planeCount = 1;
    if (copyInfo->imageAspect == PAL_IMAGE_ASPECT_DEPTH_STENCIL) {
        planeCount = 2;
    }

    Uint32 level = copyInfo->ImageMipLevel;
    Uint32 startLayer = copyInfo->ImageStartArrayLayer;
    Uint32 layerCount = copyInfo->ImageArrayLayerCount;
    Uint32 maxLayers = src->info.depthOrArraySize;
    Uint32 maxLevels = src->info.mipLevelCount;

    for (Uint32 plane = 0; plane < planeCount; plane++) {
        for (Uint32 layer = startLayer; layer < startLayer + layerCount; layer++) {
            Uint32 index = level + (layer * maxLevels) + (plane * maxLevels * maxLayers);

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

        } else if (d3dPipeline->type == COMPUTE_PIPELINE) {
            d3dCmdBuffer->handle->lpVtbl->SetComputeRootSignature(
                d3dCmdBuffer->handle, 
                d3dPipeline->layout->handle);
        }
    }

    d3dCmdBuffer->pipeline = d3dPipeline;
    return PAL_RESULT_SUCCESS;
}

PalResult PAL_CALL cmdSetViewportD3D12(
    PalCommandBuffer* cmdBuffer,
    Uint32 count,
    PalViewport* viewports)
{
    CommandBuffer* d3dCmdBuffer = (CommandBuffer*)cmdBuffer;
    D3D12_VIEWPORT cachedViewport;
    D3D12_VIEWPORT* d3dViewports = nullptr;

    if (count > 1) {
        d3dViewports = palAllocate(s_D3D.allocator, sizeof(D3D12_VIEWPORT) * count, 0);
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
        palFree(s_D3D.allocator, d3dViewports);
    }
    return PAL_RESULT_SUCCESS;
}

PalResult PAL_CALL cmdSetScissorsD3D12(
    PalCommandBuffer* cmdBuffer,
    Uint32 count,
    PalRect2D* scissors)
{
    CommandBuffer* d3dCmdBuffer = (CommandBuffer*)cmdBuffer;
    D3D12_RECT cachedScissor;
    D3D12_RECT* d3dScissors = nullptr;

    if (count > 1) {
        d3dScissors = palAllocate(s_D3D.allocator, sizeof(D3D12_RECT) * count, 0);
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
        palFree(s_D3D.allocator, d3dScissors);
    }
    return PAL_RESULT_SUCCESS;
}

PalResult PAL_CALL cmdBindVertexBuffersD3D12(
    PalCommandBuffer* cmdBuffer,
    Uint32 firstSlot,
    Uint32 count,
    PalBuffer** buffers,
    Uint64* offsets)
{
    CommandBuffer* d3dCmdBuffer = (CommandBuffer*)cmdBuffer;
    Pipeline* pipeline = d3dCmdBuffer->pipeline;
    D3D12_VERTEX_BUFFER_VIEW cachedView = {0};
    D3D12_VERTEX_BUFFER_VIEW* views = nullptr;

    if (!pipeline) {
        return PAL_RESULT_INVALID_OPERATION;
    }

    if (count > 1) {
        views = palAllocate(s_D3D.allocator, sizeof(D3D12_VERTEX_BUFFER_VIEW) * count, 0);
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
        views[i].SizeInBytes = tmp->size;
        views[i].StrideInBytes = pipeline->strides[i];
    }

    d3dCmdBuffer->handle->lpVtbl->IASetVertexBuffers(
        d3dCmdBuffer->handle,
        firstSlot,
        count,
        views);

    if (count > 1) {
        palFree(s_D3D.allocator, views);
    }
    return PAL_RESULT_SUCCESS;
}

PalResult PAL_CALL cmdBindIndexBufferD3D12(
    PalCommandBuffer* cmdBuffer,
    PalBuffer* buffer,
    Uint64 offset,
    PalIndexType type)
{
    CommandBuffer* d3dCmdBuffer = (CommandBuffer*)cmdBuffer;
    D3D12_INDEX_BUFFER_VIEW view = {0};
    Buffer* indexBuffer = (Buffer*)buffer;

    view.BufferLocation = indexBuffer->handle->lpVtbl->GetGPUVirtualAddress(indexBuffer->handle);
    view.BufferLocation = view.BufferLocation + offset;
    view.SizeInBytes = indexBuffer->size;
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
    Uint32 vertexCount,
    Uint32 instanceCount,
    Uint32 firstVertex,
    Uint32 firstInstance)
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
    Uint32 count)
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
    Uint32 maxDrawCount)
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
    Uint32 indexCount,
    Uint32 instanceCount,
    Uint32 firstIndex,
    Int32 vertexOffset,
    Uint32 firstInstance)
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
    Uint32 count)
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
    Uint32 maxDrawCount)
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

    AccelerationStructure* d3dAS = (AccelerationStructure*)as;
    D3D12_RESOURCE_STATES old, new;

    old = barrierToD3D12(
        oldUsageStateInfo->shaderStageCount,
        oldUsageStateInfo->usageState,
        oldUsageStateInfo->shaderStages);

    new = barrierToD3D12(
        newUsageStateInfo->shaderStageCount,
        newUsageStateInfo->usageState,
        newUsageStateInfo->shaderStages);

    D3D12_RESOURCE_BARRIER barrier = {0};
    barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_UAV;
    barrier.UAV.pResource = d3dAS->handle;

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

    Uint32 planeCount = 1; // for color or depth
    if (subresourceRange->aspect == PAL_IMAGE_ASPECT_DEPTH_STENCIL) {
        planeCount = 2;
    }

    D3D12_RESOURCE_BARRIER* barriers = nullptr;
    Uint32 levelCount = subresourceRange->mipLevelCount;
    Uint32 layerCount = subresourceRange->layerArrayCount;

    Uint32 startLevel = subresourceRange->startMipLevel;
    Uint32 startLayer = subresourceRange->startArrayLayer;
    Uint32 maxLevels = d3dImage->info.mipLevelCount;
    Uint32 maxLayers = d3dImage->info.depthOrArraySize;
    Uint32 barrierCount = layerCount * levelCount * planeCount;

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

    barriers = palAllocate(s_D3D.allocator, sizeof(D3D12_RESOURCE_BARRIER) * barrierCount, 0);
    if (!barriers) {
        return PAL_RESULT_OUT_OF_MEMORY;
    }

    Uint32 count = 0;
    for (Uint32 plane = 0; plane < planeCount; plane++) {
        for (Uint32 layer = startLayer; layer < startLayer + layerCount; layer++) {
            for (Uint32 level = startLevel; level < startLevel + levelCount; level++) {
                Uint32 index = level + (layer * maxLevels) + (plane * maxLevels * maxLayers);

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
    palFree(s_D3D.allocator, barriers);
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
    Uint32 groupCountX,
    Uint32 groupCountY,
    Uint32 groupCountZ)
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
    Uint32 baseGroupX,
    Uint32 baseGroupY,
    Uint32 baseGroupZ,
    Uint32 groupCountX,
    Uint32 groupCountY,
    Uint32 groupCountZ)
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
    if (!(device->features & PAL_ADAPTER_FEATURE_INDIRECT_DRAW)) {
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
    Uint32 raygenIndex,
    Uint32 width,
    Uint32 height,
    Uint32 depth)
{
    CommandBuffer* d3dCmdBuffer = (CommandBuffer*)cmdBuffer;
    ShaderBindingTable* d3dSbt = (ShaderBindingTable*)sbt;
    if (!(d3dCmdBuffer->device->features & PAL_ADAPTER_FEATURE_RAY_TRACING)) {
        return PAL_RESULT_ADAPTER_FEATURE_NOT_SUPPORTED;
    }

    Uint32 stride = d3dSbt->raygen.region.StrideInBytes;
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
    Uint32 raygenIndex,
    PalShaderBindingTable* sbt,
    PalBuffer* buffer)
{
    HRESULT result;
    CommandBuffer* d3dCmdBuffer = (CommandBuffer*)cmdBuffer;
    ShaderBindingTable* d3dSbt = (ShaderBindingTable*)sbt;
    Buffer* d3dBuffer = (Buffer*)buffer;
    Device* device = (Device*)d3dCmdBuffer->device;
    D3D12_DISPATCH_RAYS_DESC desc = {0};

    if (!(device->features & PAL_ADAPTER_FEATURE_RAY_TRACING)) {
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

    Uint32 stride = d3dSbt->raygen.region.StrideInBytes;
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
    Uint32 setIndex,
    PalDescriptorSet* set)
{
    CommandBuffer* d3dCmdBuffer = (CommandBuffer*)cmdBuffer;
    Pipeline* pipeline = d3dCmdBuffer->pipeline;
    DescriptorSet* d3dSet = (DescriptorSet*)set;
    DescriptorPool* pool = d3dSet->pool;

    // bind heaps
    Uint32 heapCount = 0;
    ID3D12DescriptorHeap* heaps[2];
    if (pool->hasResourceHeap) {
        heaps[heapCount++] = pool->resourceHeap.handle;
    }

    if (pool->hasSamplerHeap) {
        heaps[heapCount++] = pool->samplerHeap.handle;
    }
    d3dCmdBuffer->handle->lpVtbl->SetDescriptorHeaps(d3dCmdBuffer->handle, heapCount, heaps);

    // bind resource descriptor table
    Uint32 resourceCount = d3dSet->layout->bindingCount - d3dSet->layout->samplerCount;
    Uint32 baseIndex = setIndex;
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
    Uint32 shaderStageCount,
    PalShaderStage* shaderStages,
    Uint32 offset,
    Uint32 size,
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
    bool enable)
{
    return PAL_RESULT_ADAPTER_FEATURE_NOT_SUPPORTED;
}

PalResult PAL_CALL cmdSetDepthWriteEnableD3D12(
    PalCommandBuffer* cmdBuffer,
    bool enable)
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

PalResult PAL_CALL createAccelerationstructureD3D12(
    PalDevice* device,
    const PalAccelerationStructureCreateInfo* info,
    PalAccelerationStructure** outAs)
{
    HRESULT result;
    AccelerationStructure* as = nullptr;
    Device* d3dDevice = (Device*)device;
    Buffer* d3dBuffer = (Buffer*)info->buffer;

    if (!(d3dDevice->features & PAL_ADAPTER_FEATURE_RAY_TRACING)) {
        return PAL_RESULT_ADAPTER_FEATURE_NOT_SUPPORTED;
    }

    as = palAllocate(s_D3D.allocator, sizeof(AccelerationStructure), 0);
    if (!as) {
        return PAL_RESULT_OUT_OF_MEMORY;
    }

    as->type = info->type;
    as->handle = d3dBuffer->handle;
    as->address = as->handle->lpVtbl->GetGPUVirtualAddress(as->handle);
    as->address += info->offset;

    *outAs = (PalAccelerationStructure*)as;
    return PAL_RESULT_SUCCESS;
}

void PAL_CALL destroyAccelerationstructureD3D12(PalAccelerationStructure* as)
{
    AccelerationStructure* d3dAs = (AccelerationStructure*)as;
    palFree(s_D3D.allocator, d3dAs);
}

PalResult PAL_CALL getAccelerationStructureBuildSizeD3D12(
    PalDevice* device,
    PalAccelerationStructureBuildInfo* info,
    PalAccelerationStructureBuildSize* size)
{
    Device* d3dDevice = (Device*)device;
    if (!(d3dDevice->features & PAL_ADAPTER_FEATURE_RAY_TRACING)) {
        return PAL_RESULT_ADAPTER_FEATURE_NOT_SUPPORTED;
    }

    D3D12_RAYTRACING_GEOMETRY_DESC* geometries = nullptr;
    D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_DESC buildInfo = {0};
    D3D12_RAYTRACING_ACCELERATION_STRUCTURE_PREBUILD_INFO sizeInfo = {0};

    if (info->type == PAL_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL) {
        geometries = palAllocate(
            s_D3D.allocator,
            sizeof(D3D12_RAYTRACING_GEOMETRY_DESC) * info->geometryCount,
            0);

        if (!geometries) {
            return PAL_RESULT_OUT_OF_MEMORY;
        }

        memset(geometries, 0, sizeof(D3D12_RAYTRACING_GEOMETRY_DESC) * info->geometryCount);
        bool success = fillBuildInfoD3D12(
            info,
            geometries,
            0,
            0,
            &buildInfo);

        if (!success) {
            return PAL_RESULT_INVALID_ARGUMENT;
        }

        d3dDevice->handle->lpVtbl->GetRaytracingAccelerationStructurePrebuildInfo(
            d3dDevice->handle,
            &buildInfo.Inputs,
            &sizeInfo);

        palFree(s_D3D.allocator, geometries);

    } else {
        bool success = fillBuildInfoD3D12(
            info,
            nullptr,
            0,
            0,
            &buildInfo);

        if (!success) {
            return PAL_RESULT_INVALID_ARGUMENT;
        }

        d3dDevice->handle->lpVtbl->GetRaytracingAccelerationStructurePrebuildInfo(
            d3dDevice->handle,
            &buildInfo.Inputs,
            &sizeInfo);
    }

    size->accelerationStructureSize = sizeInfo.ResultDataMaxSizeInBytes;
    size->scratchBufferSize = sizeInfo.ScratchDataSizeInBytes;
    size->updateScratchBufferSize = sizeInfo.UpdateScratchDataSizeInBytes;
    return PAL_RESULT_SUCCESS;
}

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
    Device* d3dDevice = (Device*)device;

    buffer = palAllocate(s_D3D.allocator, sizeof(Buffer), 0);
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
        buffer->desc.Flags |= D3D12_RESOURCE_FLAG_RAYTRACING_ACCELERATION_STRUCTURE;
    }

    if (info->usages & PAL_BUFFER_USAGE_DEVICE_ADDRESS) {
        if (!(d3dDevice->features & PAL_ADAPTER_FEATURE_BUFFER_DEVICE_ADDRESS)) {
            return PAL_RESULT_ADAPTER_FEATURE_NOT_SUPPORTED;
        }
        buffer->supportsAddress = true;
    }

    if (info->usages & PAL_BUFFER_USAGE_INDIRECT) {
        buffer->hasIndirect = false;
    }

    buffer->device = d3dDevice;
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
    palFree(s_D3D.allocator, d3dBuffer);
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
    requirements->memoryTypes[PAL_MEMORY_TYPE_GPU_ONLY] = true;
    requirements->memoryTypes[PAL_MEMORY_TYPE_CPU_UPLOAD] = true;
    requirements->memoryTypes[PAL_MEMORY_TYPE_CPU_READBACK] = true;

    requirements->alignment = allocationInfo.Alignment;
    requirements->size = allocationInfo.SizeInBytes;
    return PAL_RESULT_SUCCESS;
}

PalResult PAL_CALL computeInstanceBufferRequirementsD3D12(
    PalDevice* device,
    Uint32 instanceCount,
    Uint64* outSize)
{
    *outSize = sizeof(D3D12_RAYTRACING_INSTANCE_DESC) * instanceCount;
    return PAL_RESULT_SUCCESS;
}

PalResult PAL_CALL computeImageCopyStagingBufferRequirementsD3D12(
    PalDevice* device,
    PalFormat imageFormat,
    PalBufferImageCopyInfo* copyInfo,
    Uint32* outBufferRowLength,
    Uint32* outBufferImageHeight,
    Uint64* outSize)
{
    Uint32 imageFormatSize = getFormatSizeD3D12(imageFormat);
    Uint32 rowPitch = alignD3D12((Uint64)copyInfo->imageWidth * imageFormatSize, TEXTURE_PITCH);
    Uint32 bufferImageHeight = 0;

    if (copyInfo->bufferImageHeight) {
        bufferImageHeight = copyInfo->bufferImageHeight;
    } else {
        bufferImageHeight = copyInfo->imageHeight;
    }

    *outBufferRowLength = rowPitch;
    *outBufferImageHeight = bufferImageHeight;
    *outSize = (Uint64)rowPitch * bufferImageHeight * copyInfo->imageDepth;
    return PAL_RESULT_SUCCESS;
}

PalResult PAL_CALL writeToInstanceBufferD3D12(
    PalDevice* device,
    void* ptr,
    PalAccelerationStructureInstance* instances,
    Uint32 instanceCount)
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
    Uint32 imageFormatSize = getFormatSizeD3D12(imageFormat);
    Uint32 srcRowPitch = copyInfo->imageWidth * imageFormatSize;
    const Uint32 dstSlicePitch = copyInfo->bufferRowLength * copyInfo->bufferImageHeight;
    const Uint32 srcSlicePitch = srcRowPitch * copyInfo->imageHeight;

    // manually offset the buffer with the provided offset
    Uint8* dst = (Uint8*)ptr + copyInfo->bufferOffset;
    const Uint8* src = (const Uint8*)srcData;

    // write to destination pointer
    for (Uint32 z = 0; z < copyInfo->imageDepth; z++) {
        for (Uint32 y = 0; y < copyInfo->imageHeight; y++) {
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
    Uint64 offset)
{
    HRESULT result;
    Buffer* d3dBuffer = (Buffer*)buffer;
    ID3D12Device5* device = d3dBuffer->device->handle;

    D3D12_RESOURCE_STATES state = 0;
    ID3D12Heap* mem = (ID3D12Heap*)memory;
    D3D12_HEAP_DESC __ret = {0};
    D3D12_HEAP_DESC heapProps = {0};
    heapProps = *mem->lpVtbl->GetDesc(mem, &__ret);

    d3dBuffer->canChangeState = true;
    if (heapProps.Properties.Type == D3D12_HEAP_TYPE_UPLOAD) {
        state = D3D12_RESOURCE_STATE_GENERIC_READ;
        d3dBuffer->canChangeState = false;

    } else if (heapProps.Properties.Type == D3D12_HEAP_TYPE_READBACK) {
        state = D3D12_RESOURCE_STATE_COPY_DEST;
        d3dBuffer->canChangeState = false;
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

PalResult PAL_CALL mapBufferMemoryD3D12(
    PalBuffer* buffer,
    Uint64 offset,
    Uint64 size,
    void** outPtr)
{
    void* ptr = nullptr;
    Buffer* d3dBuffer = (Buffer*)buffer;
    HRESULT result = d3dBuffer->handle->lpVtbl->Map(d3dBuffer->handle, 0, nullptr, &ptr);
    if (FAILED(result)) {
        pollMessagesD3D12(d3dBuffer->device);
        return PAL_RESULT_MEMORY_MAP_FAILED;
    }

    *outPtr = ptr + offset;
    return PAL_RESULT_SUCCESS;
}

void PAL_CALL unmapBufferMemoryD3D12(PalBuffer* buffer)
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
    Device* d3dDevice = (Device*)device;
    DescriptorSetLayout* layout = nullptr;
    DescriptorSetBinding* bindings = nullptr;
    Uint32 count = info->bindingCount;

    layout = palAllocate(s_D3D.allocator, sizeof(DescriptorSetLayout), 0);
    bindings = palAllocate(s_D3D.allocator, sizeof(DescriptorSetBinding) * count, 0);
    if (!layout || !bindings) {
        return PAL_RESULT_OUT_OF_MEMORY;
    }

    Uint32 resourceOffset = 0;
    Uint32 samplerOffset = 0;
    Uint32 SRVRegister = 0;
    Uint32 UAVRegister = 0;
    Uint32 CBVRegister = 0;

    Uint32 sampledImageCount = 0;
    Uint32 storageImageCount = 0;
    Uint32 storageBufferCount = 0;
    Uint32 uniformBufferCount = 0;
    Uint32 tlasCount = 0;
    Uint32 samplerCount = 0;

    for (int i = 0; i < count; i++) {
        DescriptorSetBinding* binding = &bindings[i];
        binding->type = info->bindings[i].descriptorType;

        binding->range.NumDescriptors = info->bindings[i].descriptorCount;
        binding->range.Flags = D3D12_DESCRIPTOR_RANGE_FLAG_NONE;
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
    if (sampledImageCount > d3dDevice->limits.maxDescriptorSampledImages) {
        return PAL_RESULT_INVALID_ARGUMENT;
    }

    if (storageImageCount > d3dDevice->limits.maxDescriptorStorageImages) {
        return PAL_RESULT_INVALID_ARGUMENT;
    }

    if (storageBufferCount > d3dDevice->limits.maxDescriptorStorageBuffers) {
        return PAL_RESULT_INVALID_ARGUMENT;
    }

    if (uniformBufferCount > d3dDevice->limits.maxDescriptorUniformBuffers) {
        return PAL_RESULT_INVALID_ARGUMENT;
    }

    if (tlasCount > d3dDevice->limits.maxDescriptorAccelerationStructures) {
        return PAL_RESULT_INVALID_ARGUMENT;
    }

    if (samplerCount > d3dDevice->limits.maxDescriptorSamplers) {
        return PAL_RESULT_INVALID_ARGUMENT;
    }

    layout->bindingCount = info->bindingCount;
    layout->samplerCount = samplerCount;
    layout->bindings = bindings;
    *outLayout = (PalDescriptorSetLayout*)layout;
    return PAL_RESULT_SUCCESS;
}

void PAL_CALL destroyDescriptorSetLayoutD3D12(PalDescriptorSetLayout* layout)
{
    DescriptorSetLayout* d3dLayout = (DescriptorSetLayout*)layout;
    palFree(s_D3D.allocator, d3dLayout->bindings);
    palFree(s_D3D.allocator, d3dLayout);
}

PalResult PAL_CALL createDescriptorPoolD3D12(
    PalDevice* device,
    const PalDescriptorPoolCreateInfo* info,
    PalDescriptorPool** outPool)
{
    HRESULT result;
    Device* d3dDevice = (Device*)device;
    DescriptorPool* pool = nullptr;
    pool = palAllocate(s_D3D.allocator, sizeof(DescriptorPool), 0);
    if (!pool) {
        return PAL_RESULT_OUT_OF_MEMORY;
    }

    memset(pool, 0, sizeof(DescriptorPool));
    pool->sets = palAllocate(s_D3D.allocator, sizeof(DescriptorSet) * info->maxDescriptorSets, 0);
    if (!pool->sets) {
        return PAL_RESULT_OUT_OF_MEMORY;
    }

    Uint32 resourceCount = 0;
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

        result = d3dDevice->handle->lpVtbl->CreateDescriptorHeap(
            d3dDevice->handle,
            &desc,
            &IID_DescriptorHeap,
            (void**)&heap->handle);

        if (FAILED(result)) {
            pollMessagesD3D12(d3dDevice);
            if (result == E_OUTOFMEMORY) {
                return PAL_RESULT_OUT_OF_MEMORY;
            }
            return PAL_RESULT_PLATFORM_FAILURE;
        }

        // get increment size
        heap->incrementSize = d3dDevice->handle->lpVtbl->GetDescriptorHandleIncrementSize(
            d3dDevice->handle,
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

        pool->hasResourceHeap = true;
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

        result = d3dDevice->handle->lpVtbl->CreateDescriptorHeap(
            d3dDevice->handle,
            &desc,
            &IID_DescriptorHeap,
            (void**)&heap->handle);

        if (FAILED(result)) {
            if (result == E_OUTOFMEMORY) {
                pollMessagesD3D12(d3dDevice);
                return PAL_RESULT_OUT_OF_MEMORY;
            }
            return PAL_RESULT_PLATFORM_FAILURE;
        }

        // get increment size
        heap->incrementSize = d3dDevice->handle->lpVtbl->GetDescriptorHandleIncrementSize(
            d3dDevice->handle,
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

        pool->hasSamplerHeap = true;
    }

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

    palFree(s_D3D.allocator, d3dPool->sets);
    palFree(s_D3D.allocator, d3dPool);
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
    Device* d3dDevice = (Device*)device;
    DescriptorPool* d3dPool = (DescriptorPool*)pool;
    DescriptorSetLayout* d3dLayout = (DescriptorSetLayout*)layout;
    DescriptorSet* set = nullptr;

    Uint32 storageImageCount = 0;
    Uint32 samplerCount = 0;
    Uint32 storageBufferCount = 0;
    Uint32 uniformBufferCount = 0;
    Uint32 sampledImageCount = 0;
    Uint32 tlasCount = 0;

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

    Uint32 totalDescriptors = tlasCount + storageBufferCount + uniformBufferCount;
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
    Uint32 count,
    PalDescriptorSetWriteInfo* infos)
{
    Device* d3dDevice = (Device*)device;
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
        Uint32 index = 0;
        Uint32 bindingOffset = binding->range.OffsetInDescriptorsFromTableStart;

        if (binding->type == PAL_DESCRIPTOR_TYPE_SAMPLER) {
            heap = &pool->samplerHeap;
            index = set->samplerOffset + bindingOffset + info->arrayElement;

        } else {
            heap = &pool->resourceHeap;
            index = set->resourceOffset + bindingOffset + info->arrayElement;
        }

        for (int j = 0; j < binding->range.NumDescriptors; j++) {
            D3D12_CPU_DESCRIPTOR_HANDLE dst;
            dst.ptr = getDescriptorHandleD3D12(index + j, heap->incrementSize, heap->cpuBase);

            if (info->descriptorType == PAL_DESCRIPTOR_TYPE_SAMPLER) {
                Sampler* sampler = (Sampler*)info->samplerInfos[j].sampler;
                d3dDevice->handle->lpVtbl->CreateSampler(d3dDevice->handle, &sampler->desc, dst);

            } else if (info->descriptorType == PAL_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE) {
                AccelerationStructure* tlas = nullptr;
                tlas = (AccelerationStructure*)info->tlasInfos[j].tlas;

                D3D12_SHADER_RESOURCE_VIEW_DESC desc = {0};
                desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
                desc.RaytracingAccelerationStructure.Location = tlas->address;
                desc.ViewDimension = D3D12_SRV_DIMENSION_RAYTRACING_ACCELERATION_STRUCTURE;

                d3dDevice->handle->lpVtbl->CreateShaderResourceView(
                    d3dDevice->handle,
                    nullptr,
                    &desc,
                    dst);

            } else if (info->descriptorType == PAL_DESCRIPTOR_TYPE_SAMPLED_IMAGE) {
                ImageView* imageView = (ImageView*)info->imageViewInfos[j].imageView;
                D3D12_SHADER_RESOURCE_VIEW_DESC desc = {0};
                desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
                desc.Format = imageView->format;

                fillSubresourceD3D12(
                    imageView->type,
                    &imageView->range,
                    nullptr,
                    nullptr,
                    &desc,
                    nullptr);

                d3dDevice->handle->lpVtbl->CreateShaderResourceView(
                    d3dDevice->handle,
                    imageView->image->handle,
                    &desc,
                    dst);

            } else if (info->descriptorType == PAL_DESCRIPTOR_TYPE_STORAGE_IMAGE) {
                ImageView* imageView = (ImageView*)info->imageViewInfos[j].imageView;
                D3D12_UNORDERED_ACCESS_VIEW_DESC desc = {0};
                desc.Format = imageView->format;

                fillSubresourceD3D12(
                    imageView->type,
                    &imageView->range,
                    nullptr,
                    nullptr,
                    nullptr,
                    &desc);

                d3dDevice->handle->lpVtbl->CreateUnorderedAccessView(
                    d3dDevice->handle,
                    imageView->image->handle,
                    nullptr,
                    &desc,
                    dst);

            } else if (info->descriptorType == PAL_DESCRIPTOR_TYPE_UNIFORM_BUFFER) {
                PalDescriptorBufferInfo* bufferInfo = &info->bufferInfos[j];
                Buffer* buffer = (Buffer*)bufferInfo->buffer;

                D3D12_CONSTANT_BUFFER_VIEW_DESC desc = {0};
                desc.SizeInBytes = bufferInfo->size;

                D3D12_GPU_VIRTUAL_ADDRESS address = 0;
                address = buffer->handle->lpVtbl->GetGPUVirtualAddress(buffer->handle);
                desc.BufferLocation = address + bufferInfo->offset;

                d3dDevice->handle->lpVtbl->CreateConstantBufferView(
                    d3dDevice->handle,
                    &desc,
                    dst);

            } else {
                // storage buffer
                PalDescriptorBufferInfo* bufferInfo = &info->bufferInfos[j];
                Buffer* buffer = (Buffer*)bufferInfo->buffer;
                
                D3D12_UNORDERED_ACCESS_VIEW_DESC desc = {0};
                Uint32 stride = 4;
                if (bufferInfo->stride) {
                    stride = bufferInfo->stride;
                    desc.Buffer.StructureByteStride = bufferInfo->stride;
                } else {
                    desc.Format = DXGI_FORMAT_R32_TYPELESS;
                    desc.Buffer.Flags = D3D12_BUFFER_UAV_FLAG_RAW;
                }

                desc.ViewDimension = D3D12_UAV_DIMENSION_BUFFER;
                desc.Buffer.FirstElement = bufferInfo->offset / stride;
                desc.Buffer.NumElements = bufferInfo->size / stride;

                d3dDevice->handle->lpVtbl->CreateUnorderedAccessView(
                    d3dDevice->handle,
                    buffer->handle,
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
    Device* d3dDevice = (Device*)device;
    PipelineLayout* layout = nullptr;
    Uint32 resourceCount = 0;
    Uint32 samplerCount = 0;
    Uint32 pushConstantSize = 0;
    Uint32 sizeInBytes = sizeof(D3D12_DESCRIPTOR_RANGE1);

    Uint32 rangesOffset = 0;
    Uint32 samplerRangesOffset = 0;
    D3D12_DESCRIPTOR_RANGE1* ranges = nullptr;
    D3D12_DESCRIPTOR_RANGE1* samplerRanges = nullptr;

    Uint32 parameterCount = 0;
    D3D12_ROOT_PARAMETER1* parameters = nullptr;

    if (info->descriptorSetLayoutCount > d3dDevice->limits.maxBoundDescriptorSets) {
        return PAL_RESULT_INVALID_ARGUMENT;
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

    if (pushConstantSize > d3dDevice->limits.maxPushConstantSize) {
        return PAL_RESULT_INVALID_ARGUMENT;
    }

    layout = palAllocate(s_D3D.allocator, sizeof(PipelineLayout), 0);
    if (!layout) {
        return PAL_RESULT_OUT_OF_MEMORY;
    }

    if (pushConstantSize) {
        parameterCount++;
    }

    if (parameterCount) {
        Uint32 paramtersSize = sizeof(D3D12_ROOT_PARAMETER1) * parameterCount;
        parameters = palAllocate(s_D3D.allocator, paramtersSize, 0);
        if (!parameters) {
            return PAL_RESULT_OUT_OF_MEMORY;
        }

        memset(parameters, 0, paramtersSize);
    }

    if (resourceCount) {
        ranges = palAllocate(s_D3D.allocator, sizeInBytes * resourceCount, 0);
        if (!ranges) {
            return PAL_RESULT_OUT_OF_MEMORY;
        }
    }

    if (samplerCount) {
        samplerRanges = palAllocate(s_D3D.allocator, sizeInBytes * samplerCount, 0);
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
        parameter->Constants.Num32BitValues = pushConstantSize / 4;
        parameter->ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

        layout->constantIndex = parameterCount;
        parameterCount++;
    }

    Uint32 registerSpace = 0;
    for (int i = 0; i < info->descriptorSetLayoutCount; i++) {
        DescriptorSetLayout* tmp = (DescriptorSetLayout*)info->descriptorSetLayouts[i];
        D3D12_ROOT_PARAMETER1* parameter = nullptr;

        // reset and reuse same variable
        resourceCount = tmp->bindingCount - tmp->samplerCount;
        samplerCount = tmp->samplerCount;

        Uint32 samplerIndex = samplerRangesOffset;
        Uint32 rangeIndex = rangesOffset;

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
    rootDesc.Desc_1_1.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

    ID3DBlob* blob = nullptr;
    HRESULT result = s_D3D.serializeVersionedRootSignature(&rootDesc, &blob, nullptr);
    if (FAILED(result)) {
        pollMessagesD3D12(d3dDevice);
        if (result == E_INVALIDARG) {
            return PAL_RESULT_INVALID_ARGUMENT;
        }
        return PAL_RESULT_PLATFORM_FAILURE;
    }

    result = d3dDevice->handle->lpVtbl->CreateRootSignature(
        d3dDevice->handle,
        0,
        blob->lpVtbl->GetBufferPointer(blob),
        blob->lpVtbl->GetBufferSize(blob),
        &IID_RootSignature,
        (void**)&layout->handle);

    if (FAILED(result)) {
        pollMessagesD3D12(d3dDevice);
        if (result == E_INVALIDARG) {
            return PAL_RESULT_INVALID_ARGUMENT;
        } else if (result == E_OUTOFMEMORY) {
            return PAL_RESULT_OUT_OF_MEMORY;
        }
        return PAL_RESULT_PLATFORM_FAILURE;
    }

    if (resourceCount) {
        palFree(s_D3D.allocator, ranges);
    }

    if (samplerCount) {
        palFree(s_D3D.allocator, samplerRanges);
    }

    if (parameterCount) {
        palFree(s_D3D.allocator, parameters);
    }

    blob->lpVtbl->Release(blob);
    *outLayout = (PalPipelineLayout*)layout;
    return PAL_RESULT_SUCCESS;
}

void PAL_CALL destroyPipelineLayoutD3D12(PalPipelineLayout* layout)
{
    PipelineLayout* d3dLayout = (PipelineLayout*)layout;
    d3dLayout->handle->lpVtbl->Release(d3dLayout->handle);
    palFree(s_D3D.allocator, d3dLayout);
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
    Uint32 patchControlPoints = 0;
    Uint32 totalSize = 0;
    bool alphaToCoverageEnable = false;
    Pipeline* pipeline = nullptr;
    Device* d3dDevice = (Device*)device;
    PipelineLayout* layout = (PipelineLayout*)info->pipelineLayout;

    D3D12_INPUT_ELEMENT_DESC* elementDescs = nullptr;
    D3D12_VIEW_INSTANCE_LOCATION* viewLocations = nullptr;

    GraphicsPipelineStreamDesc graphicsStreamDesc = {0};
    memset(&graphicsStreamDesc, 0, sizeof(GraphicsPipelineStreamDesc));

    pipeline = palAllocate(s_D3D.allocator, sizeof(Pipeline), 0);
    if (!pipeline) {
        return PAL_RESULT_OUT_OF_MEMORY;
    }
    memset(pipeline, 0, sizeof(Pipeline));

    if (info->renderingLayout->viewCount > 1) {
        viewLocations = palAllocate(
            s_D3D.allocator,
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

        if (tmp->patchControlPoints) {
            patchControlPoints = tmp->patchControlPoints;
            if (info->topology != PAL_PRIMITIVE_TOPOLOGY_PATCH) {
                palFree(s_D3D.allocator, pipeline);
                return PAL_RESULT_INVALID_OPERATION;
            }
        }

        if (tmp->stage == PAL_SHADER_STAGE_VERTEX) {
            type = D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_VS;

        } else if (tmp->stage == PAL_SHADER_STAGE_FRAGMENT) {
            type = D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_PS;

        } else if (tmp->stage == PAL_SHADER_STAGE_GEOMETRY) {
            type = D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_GS;

        } else if (tmp->stage == PAL_SHADER_STAGE_TESSELLATION_CONTROL) {
            type = D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_HS;

        } else if (tmp->stage == PAL_SHADER_STAGE_TESSELLATION_EVALUATION) {
            type = D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_DS;

        } else if (tmp->stage == PAL_SHADER_STAGE_TASK) {
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
    Uint32 vertexCount = 0;
    Uint32 vertexLayoutCount = info->vertexLayoutCount;
    for (int i = 0; i < vertexLayoutCount; i++) {
        PalVertexLayout* layout = &info->vertexLayouts[i];
        vertexCount += layout->attributeCount;
    }

    if (info->vertexLayoutCount > d3dDevice->limits.maxVertexLayouts) {
        return PAL_RESULT_INVALID_ARGUMENT;
    }

    if (vertexCount > d3dDevice->limits.maxVertexAttributes) {
        return PAL_RESULT_INVALID_ARGUMENT;
    }

    InputLayoutStream* inputLayoutStream = &graphicsStreamDesc.inputLayout;
    totalSize += sizeof(InputLayoutStream);
    inputLayoutStream->type = D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_INPUT_LAYOUT;

    pipeline->strides = nullptr;
    if (vertexCount) {
        pipeline->strides = palAllocate(s_D3D.allocator, sizeof(Uint32) * 8, 0);
        if (!pipeline->strides) {
            return PAL_RESULT_OUT_OF_MEMORY;
        }

        elementDescs = palAllocate(
            s_D3D.allocator,
            sizeof(D3D12_INPUT_ELEMENT_DESC) * vertexCount,
            0);

        if (!elementDescs) {
            palFree(s_D3D.allocator, elementDescs);
            return PAL_RESULT_OUT_OF_MEMORY;
        }

        Uint32 positionIndex = 0;
        Uint32 colorIndex = 0;
        Uint32 texCoordIndex = 0;
        Uint32 normalIndex = 0;
        Uint32 tangentIndex = 0;

        for (int i = 0; i < info->vertexLayoutCount; i++) {
            PalVertexLayout* layout = &info->vertexLayouts[i];
            Uint32 stride = 0;
            Uint32 offset = 0;

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
                Uint32 size = getVertexTypeSizeD3D12(vertexAttrib->type);
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
    if (info->primitiveRestartEnable == false) {
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
    rasterizerStream->desc.FrontCounterClockwise = FALSE;
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
            rasterizerStream->desc.FrontCounterClockwise = FALSE;

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
    depthStencilStream->desc.DepthEnable = FALSE;
    depthStencilStream->desc.StencilEnable = FALSE;

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
    pipeline->hasFsr = false;
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

    result = d3dDevice->handle->lpVtbl->CreatePipelineState(
        d3dDevice->handle,
        &streamDesc,
        &IID_PipelineState,
        &pipeline->handle);

    if (FAILED(result)) {
        pollMessagesD3D12(d3dDevice);
        if (result == E_INVALIDARG) {
            return PAL_RESULT_INVALID_ARGUMENT;
        } else if (result == E_OUTOFMEMORY) {
            return PAL_RESULT_OUT_OF_MEMORY;
        }
        return PAL_RESULT_PLATFORM_FAILURE;
    }

    if (info->vertexLayoutCount) {
        palFree(s_D3D.allocator, elementDescs);
    }

    if (info->renderingLayout->viewCount > 1) {
        palFree(s_D3D.allocator, viewLocations);
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
    Device* d3dDevice = (Device*)device;
    PipelineLayout* layout = (PipelineLayout*)info->pipelineLayout;
    Shader* shader = (Shader*)info->computeShader;
    Pipeline* pipeline = nullptr;

    pipeline = palAllocate(s_D3D.allocator, sizeof(Pipeline), 0);
    if (!pipeline) {
        return PAL_RESULT_OUT_OF_MEMORY;
    }

    D3D12_COMPUTE_PIPELINE_STATE_DESC desc = {0};
    desc.CS = shader->byteCode;
    desc.pRootSignature = layout->handle;

    HRESULT result = d3dDevice->handle->lpVtbl->CreateComputePipelineState(
        d3dDevice->handle,
        &desc,
        &IID_PipelineState,
        &pipeline->handle);

    if (FAILED(result)) {
        pollMessagesD3D12(d3dDevice);
        if (result == E_INVALIDARG) {
            return PAL_RESULT_INVALID_ARGUMENT;
        } else if (result == E_OUTOFMEMORY) {
            return PAL_RESULT_OUT_OF_MEMORY;
        }
        return PAL_RESULT_PLATFORM_FAILURE;
    }

    pipeline->type = COMPUTE_PIPELINE;
    pipeline->strides = nullptr;
    pipeline->hasFsr = false;
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
    Uint32 hitGroupIndex = 0;
    Uint32 subObjectIndex = 0;
    Uint32 localExportCount = 0;
    Uint32 localExportIndex = 0;

    // D3D12_STATE_SUBOBJECT_TYPE_RAYTRACING_PIPELINE_CONFIG 
    // D3D12_STATE_SUBOBJECT_TYPE_RAYTRACING_SHADER_CONFIG
    Uint32 subObjectCount = info->shaderCount + 2;

    Device* d3dDevice = (Device*)device;
    PipelineLayout* layout = (PipelineLayout*)info->pipelineLayout;
    Pipeline* pipeline = nullptr;

    RayShaderLibary* libraries = nullptr;
    RayHitGroup* groups = nullptr;
    D3D12_STATE_SUBOBJECT* subObjects = nullptr;
    ShaderExport* exports = nullptr;
    const wchar_t** localExports = nullptr;
    
    if (!(d3dDevice->features & PAL_ADAPTER_FEATURE_RAY_TRACING)) {
        return PAL_RESULT_ADAPTER_FEATURE_NOT_SUPPORTED;
    }

    if (info->maxAttributeSize > d3dDevice->limits.maxHitAttributeSize) {
        return PAL_RESULT_INVALID_ARGUMENT;
    }

    if (info->maxPayloadSize > d3dDevice->limits.maxPayloadSize) {
        return PAL_RESULT_INVALID_ARGUMENT;
    }

    if (info->maxRecursionDepth> d3dDevice->limits.maxRecursionDepth) {
        return PAL_RESULT_INVALID_ARGUMENT;
    }

    Uint32 localRootSize = 0;
    ShaderBindingTableInfo sbtInfo = {0};
    for (int i = 0; i < info->shaderGroupCount; i++) {
        PalRayTracingShaderGroupCreateInfo* tmp = &info->shaderGroups[i];
        if (tmp->type == PAL_RAY_TRACING_SHADER_GROUP_TYPE_GENERAL) {
            // check if its raygen, miss or callable
            Shader* shader = (Shader*)info->shaders[tmp->generalShaderIndex];
            switch (shader->stage) {
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
        }

        sbtInfo.hitDataSize = max(sbtInfo.hitDataSize, tmp->maxDataSize);
        if (tmp->maxDataSize) {
            localExportCount++;
        }

        // find the max data size across all shader groups
        localRootSize = max(localRootSize, sbtInfo.raygenDataSize);
        localRootSize = max(localRootSize, sbtInfo.missDataSize);
        localRootSize = max(localRootSize, sbtInfo.hitDataSize);
        localRootSize = max(localRootSize, sbtInfo.callableDataSize);

        sbtInfo.hitCount++;
        subObjectCount++;
    }

    if (localRootSize) {
        // D3D12_LOCAL_ROOT_SIGNATURE
        // D3D12_STATE_SUBOBJECT_TYPE_SUBOBJECT_TO_EXPORTS_ASSOCIATION
        subObjectCount += 2;
    }

    pipeline = palAllocate(s_D3D.allocator, sizeof(Pipeline), 0);
    if (!pipeline) {
        return PAL_RESULT_OUT_OF_MEMORY;
    }

    subObjects = palAllocate(s_D3D.allocator, sizeof(D3D12_STATE_SUBOBJECT) * subObjectCount, 0);
    groups = palAllocate(s_D3D.allocator, sizeof(RayHitGroup) * sbtInfo.hitCount, 0);
    libraries = palAllocate(s_D3D.allocator, sizeof(RayShaderLibary) * info->shaderCount, 0);
    exports = palAllocate(s_D3D.allocator, sizeof(ShaderExport) * info->shaderCount, 0);
    localExports = palAllocate(s_D3D.allocator, sizeof(wchar_t*) * localExportCount, 0);

    if (!exports || !groups || !subObjects || !libraries || !localExports) {
        return PAL_RESULT_OUT_OF_MEMORY;
    }

    // check if we need a local root signature
    D3D12_SUBOBJECT_TO_EXPORTS_ASSOCIATION localExportAssociation = {0};
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
        result = s_D3D.serializeVersionedRootSignature(&rootDesc, &blob, nullptr);
        if (FAILED(result)) {
            pollMessagesD3D12(d3dDevice);
            if (result == E_INVALIDARG) {
                return PAL_RESULT_INVALID_ARGUMENT;
            }
            return PAL_RESULT_PLATFORM_FAILURE;
        }

        result = d3dDevice->handle->lpVtbl->CreateRootSignature(
            d3dDevice->handle,
            0,
            blob->lpVtbl->GetBufferPointer(blob),
            blob->lpVtbl->GetBufferSize(blob),
            &IID_RootSignature,
            (void**)&pipeline->localRootSignature);

        if (FAILED(result)) {
            pollMessagesD3D12(d3dDevice);
            if (result == E_INVALIDARG) {
                return PAL_RESULT_INVALID_ARGUMENT;
            } else if (result == E_OUTOFMEMORY) {
                return PAL_RESULT_OUT_OF_MEMORY;
            }
            return PAL_RESULT_PLATFORM_FAILURE;
        }

        subObjects[subObjectIndex].Type = D3D12_STATE_SUBOBJECT_TYPE_LOCAL_ROOT_SIGNATURE;
        subObjects[subObjectIndex].pDesc = pipeline->localRootSignature;

        localExportAssociation.pSubobjectToAssociate = &subObjects[subObjectIndex];
        localExportAssociation.pExports = localExports;
        localExportAssociation.NumExports = localExportCount;
        subObjectIndex++;

        D3D12_STATE_SUBOBJECT_TYPE t = D3D12_STATE_SUBOBJECT_TYPE_SUBOBJECT_TO_EXPORTS_ASSOCIATION;
        subObjects[subObjectIndex].Type = t;
        subObjects[subObjectIndex].pDesc = &localExportAssociation;
        subObjectIndex++;
    }

    // shaders
    for (int i = 0; i < info->shaderCount; i++) {
        Shader* tmp = (Shader*)info->shaders[i];
        RayShaderLibary* shaderLibrary = &libraries[i];
        ShaderExport* export = &exports[i];

        export->stage = tmp->stage;
        wcscpy(export->entryName, tmp->entryName);

        shaderLibrary->export.Flags = D3D12_EXPORT_FLAG_NONE;
        shaderLibrary->export.ExportToRename = nullptr;
        shaderLibrary->export.Name = export->entryName; // reference

        shaderLibrary->library.DXILLibrary = tmp->byteCode;
        shaderLibrary->library.NumExports = 1;
        shaderLibrary->library.pExports = &shaderLibrary->export;

        subObjects[subObjectIndex].Type = D3D12_STATE_SUBOBJECT_TYPE_DXIL_LIBRARY;
        subObjects[subObjectIndex].pDesc = &shaderLibrary->library;
        subObjectIndex++;
    }

    // hit groups
    for (int i = 0; i < info->shaderGroupCount; i++) {
        const wchar_t* exportName = nullptr;
        PalRayTracingShaderGroupCreateInfo* tmp = &info->shaderGroups[i];
        if (tmp->type == PAL_RAY_TRACING_SHADER_GROUP_TYPE_GENERAL) {
            if (tmp->maxDataSize) {
                Shader* shader = (Shader*)info->shaders[tmp->generalShaderIndex];
                localExports[localExportIndex++] = shader->entryName;
            }

            continue;
        }

        D3D12_HIT_GROUP_DESC* group = &groups[hitGroupIndex].desc;
        getHitGroupNameD3D12(hitGroupIndex, groups[hitGroupIndex].entryName);

        group->AnyHitShaderImport = nullptr;
        group->ClosestHitShaderImport = nullptr;
        group->IntersectionShaderImport = nullptr;
        group->HitGroupExport = groups[hitGroupIndex].entryName;

        if (tmp->type == PAL_RAY_TRACING_SHADER_GROUP_TYPE_TRIANGLES_HIT) {
            group->Type = D3D12_HIT_GROUP_TYPE_TRIANGLES;

        } else if (tmp->type == PAL_RAY_TRACING_SHADER_GROUP_TYPE_PROCEDURAL_HIT) {
            group->Type = D3D12_HIT_GROUP_TYPE_PROCEDURAL_PRIMITIVE;
        }

        // Any hit shader
        if (tmp->anyHitShaderIndex != PAL_UNUSED_SHADER_INDEX) {
            Shader* shader = (Shader*)info->shaders[tmp->anyHitShaderIndex];
            group->AnyHitShaderImport = shader->entryName;

        } else {
            group->AnyHitShaderImport = nullptr;
        }

        // Closest hit shader
        if (tmp->closestHitShaderIndex != PAL_UNUSED_SHADER_INDEX) {
            Shader* shader = (Shader*)info->shaders[tmp->closestHitShaderIndex];
            group->AnyHitShaderImport = shader->entryName;

        } else {
            group->ClosestHitShaderImport = nullptr;
        }

        // IntersectionShader shader
        if (tmp->intersectionShaderIndex != PAL_UNUSED_SHADER_INDEX) {
            Shader* shader = (Shader*)info->shaders[tmp->intersectionShaderIndex];
            group->AnyHitShaderImport = shader->entryName;

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

    D3D12_STATE_OBJECT_DESC desc = {0};
    desc.Type = D3D12_STATE_OBJECT_TYPE_RAYTRACING_PIPELINE;
    desc.NumSubobjects = subObjectCount;
    desc.pSubobjects = subObjects;

    result = d3dDevice->handle->lpVtbl->CreateStateObject(
        d3dDevice->handle,
        &desc,
        &IID_StateObject,
        &pipeline->handle);

    if (FAILED(result)) {
        pollMessagesD3D12(d3dDevice);
        if (result == E_INVALIDARG) {
            return PAL_RESULT_INVALID_ARGUMENT;
        } else if (result == E_OUTOFMEMORY) {
            return PAL_RESULT_OUT_OF_MEMORY;
        }
        return PAL_RESULT_PLATFORM_FAILURE;
    }

    palFree(s_D3D.allocator, groups);
    palFree(s_D3D.allocator, subObjects);
    palFree(s_D3D.allocator, libraries);
    palFree(s_D3D.allocator, localExports);

    pipeline->type = RAY_TRACING_PIPELINE;
    pipeline->strides = nullptr;
    pipeline->hasFsr = false;
    pipeline->layout = layout;

    pipeline->shaderExports = exports;
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
        palFree(s_D3D.allocator, d3dPipeline->strides);
    }

    if (d3dPipeline->shaderExports) {
        palFree(s_D3D.allocator, d3dPipeline->shaderExports);
    }

    palFree(s_D3D.allocator, d3dPipeline);
}

// ==================================================
// Shader Binding Table
// ==================================================

PalResult PAL_CALL createShaderBindingTableD3D12(
    PalDevice* device,
    const PalShaderBindingTableCreateInfo* info,
    PalShaderBindingTable** outSbt)
{
    HRESULT result;
    Device* d3dDevice = (Device*)device;
    ShaderBindingTable* sbt = nullptr;
    Pipeline* pipeline = (Pipeline*)info->rayTracingPipeline;
    ShaderBindingTableInfo* sbtInfo = &pipeline->sbtInfo;

    if (!(d3dDevice->features & PAL_ADAPTER_FEATURE_RAY_TRACING)) {
        return PAL_RESULT_ADAPTER_FEATURE_NOT_SUPPORTED;
    }

    Uint32 totalGroups = sbtInfo->raygenCount + sbtInfo->hitCount;
    totalGroups += sbtInfo->missCount + sbtInfo->callableCount;
    if (info->recordCount != totalGroups) {
        return PAL_RESULT_INVALID_ARGUMENT;
    }

    void** raygenHandles = nullptr;
    void** missHandles = nullptr;
    void** hitHandles = nullptr;
    void** callableHandles = nullptr;

    sbt = palAllocate(s_D3D.allocator, sizeof(ShaderBindingTable), 0);
    raygenHandles = palAllocate(s_D3D.allocator, sizeof(void*) * sbtInfo->raygenCount, 0);
    missHandles = palAllocate(s_D3D.allocator, sizeof(void*) * sbtInfo->missCount, 0);
    hitHandles = palAllocate(s_D3D.allocator, sizeof(void*) * sbtInfo->hitCount, 0);
    callableHandles = palAllocate(s_D3D.allocator, sizeof(void*) * sbtInfo->callableCount, 0);

    if (!sbt || !raygenHandles || !missHandles || !hitHandles || !callableHandles) {
        return PAL_RESULT_OUT_OF_MEMORY;
    }

    // create SBT buffer
    ID3D12StateObject* pipelineHandle = pipeline->handle;
    ID3D12StateObjectProperties* props = NULL;
    result = pipelineHandle->lpVtbl->QueryInterface(
        pipelineHandle,
        &IID_StateObjectProps,
        (void**)&props);

    if (FAILED(result)) {
        pollMessagesD3D12(d3dDevice);
        if (result == E_INVALIDARG) {
            return PAL_RESULT_INVALID_PIPELINE;
        }
        return PAL_RESULT_PLATFORM_FAILURE;
    }

    Uint32 groupHandleSize = D3D12_SHADER_IDENTIFIER_SIZE_IN_BYTES;
    Uint32 groupHandleAlignment = D3D12_RAYTRACING_SHADER_RECORD_BYTE_ALIGNMENT;
    Uint32 groupBaseAlignment = D3D12_RAYTRACING_SHADER_TABLE_BYTE_ALIGNMENT;

    // get the max local data size
    for (int i = 0; i < info->recordCount; i++) {
        PalShaderBindingTableRecordInfo* record = &info->records[i];
        Uint32 index = record->groupIndex;

        if (index < sbtInfo->raygenCount) {
            // raygen group
            if (record->localDataSize > sbtInfo->raygenDataSize)  {
                return PAL_RESULT_INVALID_ARGUMENT;
            }

        } else if (index < sbtInfo->raygenCount + sbtInfo->missCount) {
            // miss group
            if (record->localDataSize > sbtInfo->missDataSize)  {
                return PAL_RESULT_INVALID_ARGUMENT;
            }

        } else if (index < sbtInfo->raygenCount + sbtInfo->missCount + sbtInfo->hitCount) {
            // hit group
            if (record->localDataSize > sbtInfo->hitDataSize) {
                return PAL_RESULT_INVALID_ARGUMENT;
            }

        } else {
            // callable group
            if (record->localDataSize > sbtInfo->callableDataSize) {
                return PAL_RESULT_INVALID_ARGUMENT;
            }
        }
    }

    // get strides
    Uint32 raygenStride = 0;
    Uint32 missStride = 0;
    Uint32 hitStride = 0;
    Uint32 callableStride = 0;

    raygenStride = alignD3D12(groupHandleSize + sbtInfo->raygenDataSize, groupHandleAlignment);
    missStride = alignD3D12(groupHandleSize + sbtInfo->missDataSize, groupHandleAlignment);
    hitStride = alignD3D12(groupHandleSize + sbtInfo->hitDataSize, groupHandleAlignment);
    callableStride = alignD3D12(groupHandleSize + sbtInfo->callableDataSize, groupHandleAlignment);

    // get region size
    Uint32 raygenRegionSize = raygenStride * sbtInfo->raygenCount;
    Uint32 missRegionSize = missStride * sbtInfo->missCount;
    Uint32 hitRegionSize = hitStride * sbtInfo->hitCount;
    Uint32 callableRegionSize = callableStride * sbtInfo->callableCount;

    // get offsets
    Uint32 offset = 0;
    Uint32 raygenOffset = 0;
    Uint32 missOffset = 0;
    Uint32 hitOffset = 0;
    Uint32 callableOffset = 0;

    raygenOffset = alignD3D12(offset, groupBaseAlignment);
    offset = raygenOffset + raygenRegionSize;

    missOffset = alignD3D12(offset, groupBaseAlignment);
    offset = missOffset + missRegionSize;

    hitOffset = alignD3D12(offset, groupBaseAlignment);
    offset = hitOffset + hitRegionSize;

    callableOffset = alignD3D12(offset, groupBaseAlignment);
    offset = callableOffset + callableRegionSize;

    Uint32 bufferSize = alignD3D12(offset, groupBaseAlignment);

    // create gpu buffer
    D3D12_HEAP_PROPERTIES heapProps = {0};
    heapProps.Type = D3D12_HEAP_TYPE_DEFAULT;
    heapProps.VisibleNodeMask = 1;
    heapProps.CreationNodeMask = 1;

    D3D12_RESOURCE_DESC bufferDesc = {0};
    bufferDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
    bufferDesc.Width = bufferSize;
    bufferDesc.Height = 1;
    bufferDesc.DepthOrArraySize = 1;
    bufferDesc.MipLevels = 1;
    bufferDesc.SampleDesc.Count = 1;
    bufferDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

    result = d3dDevice->handle->lpVtbl->CreateCommittedResource(
        d3dDevice->handle,
        &heapProps,
        0,
        &bufferDesc,
        D3D12_RESOURCE_STATE_COMMON,
        nullptr,
        &IID_Resource, 
        (void**)&sbt->buffer);

    if (FAILED(result)) {
        if (result == E_OUTOFMEMORY) {
            return PAL_RESULT_OUT_OF_MEMORY;
        }
        return PAL_RESULT_PLATFORM_FAILURE;
    }

    // create staging buffer
    heapProps.Type = D3D12_HEAP_TYPE_UPLOAD;
    result = d3dDevice->handle->lpVtbl->CreateCommittedResource(
        d3dDevice->handle,
        &heapProps,
        0,
        &bufferDesc,
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        &IID_Resource, 
        (void**)&sbt->stagingBuffer);

    if (FAILED(result)) {
        if (result == E_OUTOFMEMORY) {
            return PAL_RESULT_OUT_OF_MEMORY;
        }
        return PAL_RESULT_PLATFORM_FAILURE;
    }

    // get shader group handles
    Uint32 raygenIndex = 0;
    Uint32 missIndex = 0;
    Uint32 hitIndex = 0;
    Uint32 callableIndex = 0;

    for (int i = 0; i < totalGroups; i++) {
        ShaderExport* tmp = &pipeline->shaderExports[i];
        PalShaderStage stage = tmp->stage;
        void* handle = nullptr;

        if (stage == PAL_SHADER_STAGE_RAYGEN) {
            handle = raygenHandles[raygenIndex++];

        } else if (stage == PAL_SHADER_STAGE_MISS) {
            handle = missHandles[missIndex++];

        } else if (stage == PAL_SHADER_STAGE_ANY_HIT ||
                   stage == PAL_SHADER_STAGE_CLOSEST_HIT ||
                   stage == PAL_SHADER_STAGE_INTERSECTION) {
            handle = hitHandles[hitIndex++];

        } else if (stage == PAL_SHADER_STAGE_CALLABLE) {
            handle = callableHandles[callableIndex++];
        }

        handle = props->lpVtbl->GetShaderIdentifier(props, tmp->entryName);
    }

    // copy handles into the buffer
    offset = 0; // reuse variable
    void* ptr = nullptr;
    result = sbt->stagingBuffer->lpVtbl->Map(sbt->stagingBuffer, 0, nullptr, &ptr);
    if (FAILED(result)) {
        return PAL_RESULT_PLATFORM_FAILURE;
    }

    // raygen
    for (int i = 0; i < sbtInfo->raygenCount; i++) {
        Uint8* dstPtr = (Uint8*)ptr + (i * raygenStride);
        PalShaderBindingTableRecordInfo* record = &info->records[offset + i];

        memcpy(dstPtr, raygenHandles[i], groupHandleSize);
        if (record->localDataSize) {
            // this record has local data
            memcpy(dstPtr + groupHandleSize, record->localData, record->localDataSize);
        }
    }
    offset += sbtInfo->raygenCount;

    // miss
    for (int i = 0; i < sbtInfo->missCount; i++) {
        Uint8* dstPtr = (Uint8*)ptr + missOffset + (i * missStride);
        PalShaderBindingTableRecordInfo* record = &info->records[offset + i];

        memcpy(dstPtr, missHandles[i], groupHandleSize);
        if (record->localDataSize) {
            // this record has local data
            memcpy(dstPtr + groupHandleSize, record->localData, record->localDataSize);
        }
    }
    offset += sbtInfo->missCount;

    // hit group
    for (int i = 0; i < sbtInfo->hitCount; i++) {
        Uint8* dstPtr = (Uint8*)ptr + hitOffset + (i * hitStride);
        PalShaderBindingTableRecordInfo* record = &info->records[offset + i];

        memcpy(dstPtr, hitHandles[i], groupHandleSize);
        if (record->localDataSize) {
            // this record has local data
            memcpy(dstPtr + groupHandleSize, record->localData, record->localDataSize);
        }
    }
    offset += sbtInfo->hitCount;

    // callable
    for (int i = 0; i < sbtInfo->callableCount; i++) {
        Uint8* dstPtr = (Uint8*)ptr + callableOffset + (i * callableStride);
        PalShaderBindingTableRecordInfo* record = &info->records[offset + i];

        memcpy(dstPtr, callableHandles[i], groupHandleSize);
        if (record->localDataSize) {
            // this record has local data
            memcpy(dstPtr + groupHandleSize, record->localData, record->localDataSize);
        }
    }

    sbt->stagingBuffer->lpVtbl->Unmap(sbt->stagingBuffer, 0, nullptr);

    // cache SBT fields and offsets address
    sbt->baseAddress = sbt->buffer->lpVtbl->GetGPUVirtualAddress(sbt->buffer);

    // raygen
    sbt->raygen.region.StartAddress = sbt->baseAddress;
    sbt->raygen.offset = 0; // always
    sbt->raygen.startIndex = 0; // always
    sbt->raygen.region.SizeInBytes = raygenRegionSize;
    sbt->raygen.region.StrideInBytes = raygenStride;

    // miss
    sbt->miss.offset = missOffset;
    sbt->miss.startIndex = sbtInfo->raygenCount;
    sbt->miss.region.StartAddress = sbt->baseAddress + missOffset;
    sbt->miss.region.SizeInBytes = missRegionSize;
    sbt->miss.region.StrideInBytes = missStride;

    // hit
    sbt->hit.offset = hitOffset;
    sbt->hit.startIndex = sbtInfo->raygenCount + sbtInfo->missCount;
    sbt->hit.region.StartAddress = sbt->baseAddress + hitOffset;
    sbt->hit.region.SizeInBytes = hitRegionSize;
    sbt->hit.region.StrideInBytes = hitStride;

    // callable
    sbt->callable.offset = callableOffset;
    sbt->callable.startIndex = sbtInfo->raygenCount + sbtInfo->missCount + sbtInfo->hitCount;
    sbt->callable.region.StartAddress = sbt->baseAddress + callableOffset;
    sbt->callable.region.SizeInBytes = callableRegionSize;
    sbt->callable.region.StrideInBytes = callableStride;

    props->lpVtbl->Release(props);
    palFree(s_D3D.allocator, raygenHandles);
    palFree(s_D3D.allocator, missHandles);
    palFree(s_D3D.allocator, hitHandles);
    palFree(s_D3D.allocator, callableHandles);

    sbt->handleSize = groupHandleSize;
    sbt->pipeline = pipeline;

    sbt->isDirty = true; // we need to copy from the staging to the gpu buffer
    *outSbt = (PalShaderBindingTable*)sbt;
    return PAL_RESULT_SUCCESS;
}

void PAL_CALL destroyShaderBindingTableD3D12(PalShaderBindingTable* sbt)
{
    ShaderBindingTable* d3dSbt = (ShaderBindingTable*)sbt;
    d3dSbt->buffer->lpVtbl->Release(d3dSbt->buffer);
    d3dSbt->stagingBuffer->lpVtbl->Release(d3dSbt->stagingBuffer);
    palFree(s_D3D.allocator, d3dSbt);
}

PalResult PAL_CALL updateShaderBindingTableD3D12(
    PalShaderBindingTable* sbt, 
    Uint32 count,
    PalShaderBindingTableRecordInfo* infos)
{
    HRESULT result;
    ShaderBindingTable* d3dSbt = (ShaderBindingTable*)sbt;
    Pipeline* pipeline = d3dSbt->pipeline;
    ShaderBindingTableInfo* sbtInfo = &pipeline->sbtInfo;

    void* data = nullptr;
    result = d3dSbt->stagingBuffer->lpVtbl->Map(d3dSbt->stagingBuffer, 0, nullptr, &data);
    if (FAILED(result)) {
        return PAL_RESULT_PLATFORM_FAILURE;
    }

    Uint32 stride = 0;
    Uint32 offset = 0;
    Uint32 startIndex = 0;

    for (int i = 0; i < count; i++) {
        PalShaderBindingTableRecordInfo* info = &infos[i];
        if (!info->localDataSize) {
            return PAL_RESULT_INVALID_ARGUMENT;
        }

        // find the group the record belongs to
        Uint32 index = info->groupIndex;
        if (index < sbtInfo->raygenCount) {
            // raygen group
            offset = 0;
            stride = d3dSbt->raygen.region.StrideInBytes;
            startIndex = d3dSbt->raygen.startIndex;

        } else if (index < sbtInfo->raygenCount + sbtInfo->missCount) {
            // miss group
            offset = d3dSbt->miss.offset;
            stride = d3dSbt->miss.region.StrideInBytes;
            startIndex = d3dSbt->miss.startIndex;

        } else if (index < sbtInfo->raygenCount + sbtInfo->missCount + sbtInfo->hitCount) {
            // hit group
            offset = d3dSbt->hit.offset;
            stride = d3dSbt->hit.region.StrideInBytes;
            startIndex = d3dSbt->hit.startIndex;

        } else {
            // callable group
            offset = d3dSbt->callable.offset;
            stride = d3dSbt->callable.region.StrideInBytes;
            startIndex = d3dSbt->callable.startIndex;
        }

        // write payload
        Uint32 localIndex = index - startIndex;
        Uint8* dst = (Uint8*)data + offset + (localIndex * stride);
        memcpy(dst + d3dSbt->handleSize, info->localData, info->localDataSize);
    }

    d3dSbt->stagingBuffer->lpVtbl->Unmap(d3dSbt->stagingBuffer, 0, nullptr);
    d3dSbt->isDirty = true;
    return PAL_RESULT_SUCCESS;
}

#endif // PAL_HAS_D3D12

#endif // __WIN32