
/**

Copyright (C) 2025 Nicholas Agbo <agbonicholas04@gmail.com>

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

/**
 * @defgroup pal_graphics Graphics
 * Graphics PAL functionality such as GPUs, GPUDevices, swapchains and more.
 *
 * @{
 */

#ifndef _PAL_GRAPHICS_H
#define _PAL_GRAPHICS_H

#include "pal_core.h"

#define PAL_GPU_NAME_SIZE 128
#define PAL_GPU_VERSION_SIZE 16
#define PAL_INFINITE (2147483647)

typedef struct PalGPUAdapter PalGPUAdapter;
typedef struct PalGPUDevice PalGPUDevice;
typedef struct PalGPUCommandQueue PalGPUCommandQueue;
typedef struct PalSwapchain PalSwapchain;
typedef struct PalGPUBuffer PalGPUBuffer;
typedef struct PalRenderTargetView PalRenderTargetView;

typedef enum {
    PAL_GPU_TYPE_UNKNOWN,
    PAL_GPU_TYPE_DISCRETE,
    PAL_GPU_TYPE_INTEGRATED,
    PAL_GPU_TYPE_VIRTUAL,
    PAL_GPU_TYPE_CPU
} PalGPUType;

typedef enum {
    PAL_GPU_API_TYPE_VULKAN,
    PAL_GPU_API_TYPE_D3D12,
    PAL_GPU_API_TYPE_METAL,

    // for custom backends
    PAL_GPU_API_TYPE_OPENGL,
    PAL_GPU_API_TYPE_GLES,
    PAL_GPU_API_TYPE_D3D11,
    PAL_GPU_API_TYPE_D3D9,
    PAL_GPU_API_TYPE_PPM
} PalGPUApiType;

typedef enum {
    PAL_GPU_COMMAND_QUEUE_TYPE_GRAPHICS,
    PAL_GPU_COMMAND_QUEUE_TYPE_COMPUTE,
    PAL_GPU_COMMAND_QUEUE_TYPE_COPY
} PalGPUCommandQueueType;

typedef enum {
    PAL_PRESENT_MODE_FIFO = PAL_BIT(0),
    PAL_PRESENT_MODE_IMMEDIATE = PAL_BIT(1),
    PAL_PRESENT_MODE_MAILBOX = PAL_BIT(2)
} PalPresentModes;

typedef enum {
    PAL_COMPOSITE_ALPHA_OPAQUE = PAL_BIT(0),
    PAL_COMPOSITE_ALPHA_PRE_MULTIPLIED = PAL_BIT(1),
    PAL_COMPOSITE_ALPHA_POST_MULTIPLIED = PAL_BIT(2)
} PalCompositeAplhas;

typedef enum {
    PAL_SWAPCHAIN_FORMAT_BGRA8_UNORM_SRGB = PAL_BIT64(0),
    PAL_SWAPCHAIN_FORMAT_BGRA8_SRGB_SRGB = PAL_BIT64(1),
    PAL_SWAPCHAIN_FORMAT_RGBA8_UNORM_SRGB = PAL_BIT64(2),
    PAL_SWAPCHAIN_FORMAT_RGBA16_FLOAT_HDR10 = PAL_BIT64(3)
} PalSwapchainFormats;

typedef enum {
    PAL_SWAPCHAIN_SHARING_MODE_EXCLUSIVE = PAL_BIT(0),
    PAL_SWAPCHAIN_SHARING_MODE_CONCURRENT = PAL_BIT(1)
} PalSwapchainSharingModes;

typedef enum {
    PAL_SWAPCHAIN_TRANSFORM_LANDSCAPE = PAL_BIT(0),
    PAL_SWAPCHAIN_TRANSFORM_PORTRAIT = PAL_BIT(1),
    PAL_SWAPCHAIN_TRANSFORM_LANDSCAPE_FLIPPED = PAL_BIT(2),
    PAL_SWAPCHAIN_TRANSFORM_PORTRAIT_FLIPPED = PAL_BIT(3)
} PalSwapchainTransforms;

typedef enum {
    PAL_SWAPCHAIN_USAGE_COLOR_ATTACHEMENT = PAL_BIT(0),
    PAL_SWAPCHAIN_USAGE_TRANSFER_SRC = PAL_BIT(1),
    PAL_SWAPCHAIN_USAGE_TRANSFER_DST = PAL_BIT(2),
    PAL_SWAPCHAIN_USAGE_SAMPLED = PAL_BIT(3),
} PalSwapchainUsages;

typedef enum {
    PAL_GPU_SHADER_FORMAT_SPIRV = PAL_BIT(0),
    PAL_GPU_SHADER_FORMAT_DXIL = PAL_BIT(1),
    PAL_GPU_SHADER_FORMAT_DXBC = PAL_BIT(2),
    PAL_GPU_SHADER_FORMAT_GLSL = PAL_BIT(3),
    PAL_GPU_SHADER_FORMAT_MSL = PAL_BIT(4),
    PAL_GPU_SHADER_FORMAT_PPM = PAL_BIT(5)
} PalGPUShaderFormats;

typedef enum {
    PAL_GPU_FEATURE_SAMPLER_ANISOTROPY = PAL_BIT64(0),
    PAL_GPU_FEATURE_SAMPLE_RATE_SHADING = PAL_BIT64(1),
    PAL_GPU_FEATURE_MULTI_VIEWPORT = PAL_BIT64(2),
    PAL_GPU_FEATURE_TIMELINE_SEMAPHORE = PAL_BIT64(3),
    PAL_GPU_FEATURE_TESSELLATION_SHADER = PAL_BIT64(4),
    PAL_GPU_FEATURE_GEOMETRY_SHADER = PAL_BIT64(5),
    PAL_GPU_FEATURE_SHADER_FLOAT16 = PAL_BIT64(6),
    PAL_GPU_FEATURE_SHADER_FLOAT64 = PAL_BIT64(7),
    PAL_GPU_FEATURE_SHADER_INT16 = PAL_BIT64(8),
    PAL_GPU_FEATURE_SHADER_INT64 = PAL_BIT64(9),
    PAL_GPU_FEATURE_DYNAMIC_RENDERING = PAL_BIT64(10),
    PAL_GPU_FEATURE_RAY_TRACING = PAL_BIT64(11),
    PAL_GPU_FEATURE_MESH_SHADER = PAL_BIT64(12),
    PAL_GPU_FEATURE_VARIABLE_RATE_SHADING = PAL_BIT64(13),
    PAL_GPU_FEATURE_DESCRIPTOR_INDEXING = PAL_BIT64(14),
    PAL_GPU_FEATURE_SWAPCHAIN = PAL_BIT64(15)
} PalGPUFeatures;

typedef struct {
    PalGPUType type;
    PalGPUApiType apiType;
    PalGPUShaderFormats shaderFormats;
    Uint64 totalMemory; // in bytes
    char versionString[PAL_GPU_VERSION_SIZE];
    char name[PAL_GPU_NAME_SIZE];
} PalGPUAdapterInfo;

typedef struct {
    bool debugLayerSupported;
    Uint32 maxComputeQueues;
    Uint32 maxGraphicsQueues;
    Uint32 maxCopyQueues;
    PalGPUFeatures features;
} PalGPUAdapterCapabilities;

typedef struct {
    Uint32 minBufferCount;
    Uint32 maxBufferCount;
    Uint32 minWidth;
    Uint32 minHeight;
    Uint32 maxWidth;
    Uint32 maxHeight;
    Uint32 maxBufferArrayLayers;
    PalSwapchainFormats formats;
    PalSwapchainUsages usages;
    PalPresentModes presentModes;
    PalCompositeAplhas compositeAlphas;
    PalSwapchainSharingModes sharingModes;
    PalSwapchainTransforms transforms;
} PalSwapchainCapabilities;

typedef struct {
    bool clipped;
    Uint32 width;
    Uint32 height;
    Uint32 bufferCount;
    Uint32 bufferArrayLayerCount;
    Uint32 concurrentQueueCount;
    PalPresentModes presentMode;
    PalSwapchainUsages usage;
    PalCompositeAplhas compositeAlpha;
    PalSwapchainFormats format;
    PalSwapchainSharingModes sharingMode;
    PalSwapchainTransforms transform;
    PalGPUCommandQueue** concurrentQueue;
} PalSwapchainCreateInfo;

typedef struct {
    void* display;
    void* window;
} PalGPUWindow;

typedef struct {
    PalResult PAL_CALL (*enumerateGPUAdapters)(
        Int32* count,
        PalGPUAdapter** outAdapters);

    PalResult PAL_CALL (*getGPUAdapterInfo)(
        PalGPUAdapter* adapter,
        PalGPUAdapterInfo* info);

    PalResult PAL_CALL (*getGPUAdapterCapabilities)(
        PalGPUAdapter* adapter,
        PalGPUAdapterCapabilities* caps);

    PalResult PAL_CALL (*createGPUDevice)(
        PalGPUAdapter* adapter,
        PalGPUFeatures features,
        PalGPUDevice** outDevice);

    void PAL_CALL (*destroyGPUDevice)(PalGPUDevice* device);

    PalResult PAL_CALL (*createGPUCommandQueue)(
        PalGPUDevice* device,
        PalGPUCommandQueueType type,
        PalGPUCommandQueue** outQueue);

    void PAL_CALL (*destroyGPUCommandQueue)(PalGPUCommandQueue* queue);

    bool PAL_CALL (*canCommandQueuePresent)(
        PalGPUCommandQueue* queue, 
        PalGPUWindow* window);

    PalResult PAL_CALL (*getSwapchainCapabilities)(
        PalGPUAdapter* adapter,
        PalGPUWindow* window,
        PalSwapchainCapabilities* caps);

    PalResult PAL_CALL (*createSwapchain)(
        PalGPUCommandQueue* queue,
        PalGPUWindow* window,
        const PalSwapchainCreateInfo* info,
        PalSwapchain** outSwapchain);

    void PAL_CALL (*destroySwapchain)(PalSwapchain* swapchain);

    PalResult PAL_CALL (*getSwapchainBuffers)(
        PalSwapchain* swapchain,
        Int32* count,
        PalGPUBuffer** outBuffers);

    PalResult PAL_CALL (*createRenderTargetView)(
        PalSwapchain* swapchain,
        PalGPUBuffer* buffer,
        PalRenderTargetView** outRtv);

    void PAL_CALL (*destroyRenderTargetView)(PalRenderTargetView* rtv);
} PalGPUBackend;

PAL_API PalResult PAL_CALL palInitGraphics(
    bool enableDebugLayer,
    const PalAllocator* allocator);

PAL_API void PAL_CALL palShutdownGraphics();

PAL_API PalResult PAL_CALL palEnumerateGPUAdapters(
    Int32* count,
    PalGPUAdapter** outAdapters);

PAL_API PalResult PAL_CALL palGetGPUAdapterInfo(
    PalGPUAdapter* adapter,
    PalGPUAdapterInfo* info);

PAL_API PalResult PAL_CALL palGetGPUAdapterCapabilities(
    PalGPUAdapter* adapter,
    PalGPUAdapterCapabilities* caps);

PAL_API PalResult PAL_CALL palAddGPUBackend(const PalGPUBackend* backend);

PAL_API PalResult PAL_CALL palCreateGPUDevice(
    PalGPUAdapter* adapter,
    PalGPUFeatures features,
    PalGPUDevice** outDevice);

PAL_API void PAL_CALL palDestroyGPUDevice(PalGPUDevice* device);

PAL_API PalResult PAL_CALL palCreateGPUCommandQueue(
    PalGPUDevice* device,
    PalGPUCommandQueueType type,
    PalGPUCommandQueue** outQueue);

PAL_API void PAL_CALL palDestroyGPUCommandQueue(PalGPUCommandQueue* queue);

PAL_API bool PAL_CALL palCanCommandQueuePresent(
    PalGPUCommandQueue* queue, 
    PalGPUWindow* window);

PAL_API PalResult PAL_CALL palQuerySwapchainCapabilities(
    PalGPUAdapter* adapter,
    PalGPUWindow* window,
    PalSwapchainCapabilities* caps);

PAL_API PalResult PAL_CALL palCreateSwapchain(
    PalGPUCommandQueue* queue,
    PalGPUWindow* window,
    const PalSwapchainCreateInfo* info,
    PalSwapchain** outSwapchain);

PAL_API void PAL_CALL palDestroySwapchain(PalSwapchain* swapchain);

PAL_API PalResult PAL_CALL palGetSwapchainBuffers(
    PalSwapchain* swapchain,
    Int32* count,
    PalGPUBuffer** outBuffers);

PAL_API PalResult PAL_CALL palCreateRenderTargetView(
    PalSwapchain* swapchain,
    PalGPUBuffer* buffer,
    PalRenderTargetView** outRtv);

PAL_API void PAL_CALL palDestroyRenderTargetView(PalRenderTargetView* rtv);

/** @} */ // end of pal_graphics group

#endif // _PAL_GRAPHICS_H