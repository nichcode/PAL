
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
 * Graphics PAL functionality such as Adapters, Device, Swapchains and more.
 *
 * @{
 */

#ifndef _PAL_GRAPHICS_H
#define _PAL_GRAPHICS_H

#include "pal_core.h"

#define PAL_ADAPTER_NAME_SIZE 128
#define PAL_ADAPTER_VERSION_SIZE 16
#define PAL_INFINITE (2147483647)
#define PAL_DEFAULT_MEMORY_OFFSET 0

typedef struct PalAdapter PalAdapter;
typedef struct PalDevice PalDevice;
typedef struct PalMemory PalMemory;
typedef struct PalQueue PalQueue;
typedef struct PalSwapchain PalSwapchain;
typedef struct PalImage PalImage;
typedef struct PalImageView PalImageView;
typedef struct PalRenderPass PalRenderPass;

typedef enum {
    PAL_ADAPTER_TYPE_UNKNOWN,
    PAL_ADAPTER_TYPE_DISCRETE,
    PAL_ADAPTER_TYPE_INTEGRATED,
    PAL_ADAPTER_TYPE_VIRTUAL,
    PAL_ADAPTER_TYPE_CPU
} PalAdapterType;

typedef enum {
    PAL_ADAPTER_API_TYPE_VULKAN,
    PAL_ADAPTER_API_TYPE_D3D12,
    PAL_ADAPTER_API_TYPE_METAL,
    PAL_ADAPTER_API_TYPE_OPENGL,
    PAL_ADAPTER_API_TYPE_GLES,
    PAL_ADAPTER_API_TYPE_D3D11,
    PAL_ADAPTER_API_TYPE_D3D9,
    PAL_ADAPTER_API_TYPE_PPM
} PalAdapterApiType;

typedef enum {
    PAL_QUEUE_TYPE_GRAPHICS,
    PAL_QUEUE_TYPE_COMPUTE,
    PAL_QUEUE_TYPE_COPY
} PalQueueType;

typedef enum {
    PAL_PRESENT_MODE_FIFO,
    PAL_PRESENT_MODE_IMMEDIATE,
    PAL_PRESENT_MODE_MAILBOX,

    PAL_PRESENT_MODE_MAX
} PalPresentMode;

typedef enum {
    PAL_COMPOSITE_ALPHA_OPAQUE,
    PAL_COMPOSITE_ALPHA_PRE_MULTIPLIED,
    PAL_COMPOSITE_ALPHA_POST_MULTIPLIED,

    PAL_COMPOSITE_ALPHA_MAX
} PalCompositeAplha;

typedef enum {
    PAL_FORMAT_UNDEFINED,

    PAL_FORMAT_R8_UNORM,
    PAL_FORMAT_R8_SNORM,
    PAL_FORMAT_R8_UINT,
    PAL_FORMAT_R8_SINT,
    PAL_FORMAT_R8_SRGB,
    PAL_FORMAT_R16_UNORM,
    PAL_FORMAT_R16_SNORM,
    PAL_FORMAT_R16_UINT,
    PAL_FORMAT_R16_SINT,
    PAL_FORMAT_R16_SFLOAT,
    PAL_FORMAT_R32_UINT,
    PAL_FORMAT_R32_SINT,
    PAL_FORMAT_R32_SFLOAT,
    PAL_FORMAT_R64_UINT,
    PAL_FORMAT_R64_SINT,
    PAL_FORMAT_R64_SFLOAT,
    PAL_FORMAT_R8G8_UNORM,
    PAL_FORMAT_R8G8_SNORM,
    PAL_FORMAT_R8G8_UINT,
    PAL_FORMAT_R8G8_SINT,
    PAL_FORMAT_R8G8_SRGB,
    PAL_FORMAT_R16G16_UNORM,
    PAL_FORMAT_R16G16_SNORM,
    PAL_FORMAT_R16G16_UINT,
    PAL_FORMAT_R16G16_SINT,
    PAL_FORMAT_R16G16_SFLOAT,
    PAL_FORMAT_R32G32_UINT,
    PAL_FORMAT_R32G32_SINT,
    PAL_FORMAT_R32G32_SFLOAT,
    PAL_FORMAT_R64G64_UINT,
    PAL_FORMAT_R64G64_SINT,
    PAL_FORMAT_R64G64_SFLOAT,
    PAL_FORMAT_R8G8B8_UNORM,
    PAL_FORMAT_R8G8B8_SNORM,
    PAL_FORMAT_R8G8B8_UINT,
    PAL_FORMAT_R8G8B8_SINT,
    PAL_FORMAT_R8G8B8_SRGB,
    PAL_FORMAT_R16G16B16_UNORM,
    PAL_FORMAT_R16G16B16_SNORM,
    PAL_FORMAT_R16G16B16_UINT,
    PAL_FORMAT_R16G16B16_SINT,
    PAL_FORMAT_R16G16B16_SFLOAT,
    PAL_FORMAT_R32G32B32_UINT,
    PAL_FORMAT_R32G32B32_SINT,
    PAL_FORMAT_R32G32B32_SFLOAT,
    PAL_FORMAT_R64G64B64_UINT,
    PAL_FORMAT_R64G64B64_SINT,
    PAL_FORMAT_R64G64B64_SFLOAT,
    PAL_FORMAT_B8G8R8_UNORM,
    PAL_FORMAT_B8G8R8_SNORM,
    PAL_FORMAT_B8G8R8_UINT,
    PAL_FORMAT_B8G8R8_SINT,
    PAL_FORMAT_B8G8R8_SRGB,
    PAL_FORMAT_R8G8B8A8_UNORM,
    PAL_FORMAT_R8G8B8A8_SNORM,
    PAL_FORMAT_R8G8B8A8_UINT,
    PAL_FORMAT_R8G8B8A8_SINT,
    PAL_FORMAT_R8G8B8A8_SRGB,
    PAL_FORMAT_R16G16B16A16_UNORM,
    PAL_FORMAT_R16G16B16A16_SNORM,
    PAL_FORMAT_R16G16B16A16_UINT,
    PAL_FORMAT_R16G16B16A16_SINT,
    PAL_FORMAT_R16G16B16A16_SFLOAT,
    PAL_FORMAT_R32G32B32A32_UINT,
    PAL_FORMAT_R32G32B32A32_SINT,
    PAL_FORMAT_R32G32B32A32_SFLOAT,
    PAL_FORMAT_R64G64B64A64_UINT,
    PAL_FORMAT_R64G64B64A64_SINT,
    PAL_FORMAT_R64G64B64A64_SFLOAT,
    PAL_FORMAT_B8G8R8A8_UNORM,
    PAL_FORMAT_B8G8R8A8_SNORM,
    PAL_FORMAT_B8G8R8A8_UINT,
    PAL_FORMAT_B8G8R8A8_SINT,
    PAL_FORMAT_B8G8R8A8_SRGB,
    PAL_FORMAT_S8_UINT,
    PAL_FORMAT_D16_UNORM,
    PAL_FORMAT_D32_SFLOAT,
    PAL_FORMAT_D32_SFLOAT_S8_UINT,
    PAL_FORMAT_D16_UNORM_S8_UINT,
    PAL_FORMAT_D24_UNORM_S8_UINT,

    PAL_FORMAT_MAX
} PalFormat;

typedef enum {
    PAL_IMAGE_USAGE_UNDEFINED = 0,

    PAL_IMAGE_USAGE_COLOR_ATTACHEMENT = PAL_BIT(0),
    PAL_IMAGE_USAGE_DEPTH_ATTACHEMENT = PAL_BIT(1),
    PAL_IMAGE_USAGE_TRANSFER_SRC = PAL_BIT(2),
    PAL_IMAGE_USAGE_TRANSFER_DST = PAL_BIT(3),
    PAL_IMAGE_USAGE_STORAGE = PAL_BIT(4),
    PAL_IMAGE_USAGE_SAMPLED = PAL_BIT(5)
} PalImageUsages;

typedef enum {
    PAL_IMAGE_VIEW_USAGE_UNDEFINED = 0,
    PAL_IMAGE_VIEW_USAGE_COLOR = PAL_BIT(0),
    PAL_IMAGE_VIEW_USAGE_DEPTH = PAL_BIT(1),
    PAL_IMAGE_VIEW_USAGE_STENCIL = PAL_BIT(2)
} PalImageViewUsages;

typedef enum {
    PAL_SHADER_FORMAT_SPIRV = PAL_BIT(0),
    PAL_SHADER_FORMAT_DXIL = PAL_BIT(1),
    PAL_SHADER_FORMAT_DXBC = PAL_BIT(2),
    PAL_SHADER_FORMAT_GLSL = PAL_BIT(3),
    PAL_SHADER_FORMAT_MSL = PAL_BIT(4),
    PAL_SHADER_FORMAT_PPM = PAL_BIT(5)
} PalShaderFormats;

typedef enum {
    PAL_ADAPTER_FEATURE_SAMPLER_ANISOTROPY = PAL_BIT64(0),
    PAL_ADAPTER_FEATURE_SAMPLE_RATE_SHADING = PAL_BIT64(1),
    PAL_ADAPTER_FEATURE_MULTI_VIEWPORT = PAL_BIT64(2),
    PAL_ADAPTER_FEATURE_TIMELINE_SEMAPHORE = PAL_BIT64(3),
    PAL_ADAPTER_FEATURE_TESSELLATION_SHADER = PAL_BIT64(4),
    PAL_ADAPTER_FEATURE_GEOMETRY_SHADER = PAL_BIT64(5),
    PAL_ADAPTER_FEATURE_SHADER_FLOAT16 = PAL_BIT64(6),
    PAL_ADAPTER_FEATURE_SHADER_FLOAT64 = PAL_BIT64(7),
    PAL_ADAPTER_FEATURE_SHADER_INT16 = PAL_BIT64(8),
    PAL_ADAPTER_FEATURE_SHADER_INT64 = PAL_BIT64(9),
    PAL_ADAPTER_FEATURE_DYNAMIC_RENDERING = PAL_BIT64(10),
    PAL_ADAPTER_FEATURE_RAY_TRACING = PAL_BIT64(11),
    PAL_ADAPTER_FEATURE_MESH_SHADER = PAL_BIT64(12),
    PAL_ADAPTER_FEATURE_VARIABLE_RATE_SHADING = PAL_BIT64(13),
    PAL_ADAPTER_FEATURE_DESCRIPTOR_INDEXING = PAL_BIT64(14),
    PAL_ADAPTER_FEATURE_SWAPCHAIN = PAL_BIT64(15),
    PAL_ADAPTER_FEATURE_MULTI_VIEW = PAL_BIT64(16),
    PAL_ADAPTER_FEATURE_CUBE_ARRAY_IMAGE_VIEW = PAL_BIT64(17)
} PalAdapterFeatures;

typedef enum {
    PAL_LOAD_OP_LOAD,
    PAL_LOAD_OP_CLEAR,
    PAL_LOAD_OP_DONT_CARE,
} PalLoadOp;

typedef enum {
    PAL_STORE_OP_STORE,
    PAL_STORE_OP_DONT_CARE
} PalStoreOp;

typedef enum {
    PAL_MEMORY_TYPE_GPU_ONLY,
    PAL_MEMORY_TYPE_CPU_UPLOAD,
    PAL_MEMORY_TYPE_CPU_READBACK,
    PAL_MEMORY_TYPE_MAX
} PalMemoryType;

typedef enum {
    PAL_IMAGE_TYPE_1D,
    PAL_IMAGE_TYPE_2D,
    PAL_IMAGE_TYPE_3D
} PalImageType;

typedef enum {
    PAL_IMAGE_VIEW_TYPE_1D,
    PAL_IMAGE_VIEW_TYPE_1D_ARRAY,
    PAL_IMAGE_VIEW_TYPE_2D,
    PAL_IMAGE_VIEW_TYPE_2D_ARRAY,
    PAL_IMAGE_VIEW_TYPE_3D,
    PAL_IMAGE_VIEW_TYPE_CUBE,
    PAL_IMAGE_VIEW_TYPE_CUBE_ARRAY,
} PalImageViewType;

typedef enum {
    PAL_SWAPCHAIN_FORMAT_BGRA8_UNORM_SRGB,
    PAL_SWAPCHAIN_FORMAT_BGRA8_SRGB_SRGB,
    PAL_SWAPCHAIN_FORMAT_RGBA8_UNORM_SRGB,
    PAL_SWAPCHAIN_FORMAT_RGBA16_FLOAT_HDR10,

    PAL_SWAPCHAIN_FORMAT_MAX // more pars will be added
} PalSwapchainFormat;

typedef struct {
    Uint32 vendorId;
    Uint32 deviceId;
    PalAdapterType type;
    PalAdapterApiType apiType;
    PalShaderFormats shaderFormats;
    Uint64 vram;
    Uint64 sharedMemory;
    Uint64 version;
    char versionString[PAL_ADAPTER_VERSION_SIZE];
    char name[PAL_ADAPTER_NAME_SIZE];
} PalAdapterInfo;

typedef struct {
    bool debugLayer;
    Uint32 maxComputeQueues;
    Uint32 maxGraphicsQueues;
    Uint32 maxCopyQueues;
    Uint32 maxImageWidth;
    Uint32 maxImageHeight;
    Uint32 maxImageDepth;
    Uint32 maxImageArrayLayers;
    Uint32 maxImageMipLevels;
    Uint32 maxColorSamples;
    Uint32 maxDepthSamples;
    Uint32 maxColorAttachments;
    Uint32 maxMultiViews;
    Uint32 maxViewports;
    Uint32 maxSamplers;
    Uint32 maxUniformBufferSize;
    Uint32 maxStorageBufferSize;
    Uint32 maxPushConstantSize;
    PalAdapterFeatures features;
} PalAdapterCapabilities;

typedef struct {
    bool presentModessAllowed[PAL_PRESENT_MODE_MAX];
    bool compositeAlphasAllowed[PAL_COMPOSITE_ALPHA_MAX];
    bool swapchainFormatsAllowed[PAL_SWAPCHAIN_FORMAT_MAX];
    Uint32 minImageCount;
    Uint32 maxImageCount;
    Uint32 minImageWidth;
    Uint32 minImageHeight;
    Uint32 maxImageWidth;
    Uint32 maxImageHeight;
    Uint32 maxImageArrayLayers;
} PalSwapchainCapabilities;

// typedef struct {
//     Uint32 maxMultiViews;
//     Uint32 maxColorAttachments;
// } PalRenderPassCapabilities;

// typedef struct {
//     Uint32 mipLevel;
//     Uint32 baseLayer;
//     Uint32 layerCount;
//     PalRenderTargetView* renderTargetView;
// } PalRenderPassResolveInfo;

// typedef struct {
//     Uint32 mipLevel;
//     Uint32 baseLayer;
//     Uint32 layerCount;
//     PalRenderPassLoadOp loadOp;
//     PalRenderPassStoreOp storeOp;
//     float depth;
//     float stencil;
//     PalRenderPassResolveInfo* resolve;
//     float color[4];
// } PalRenderPassAttachmentInfo;

// typedef struct {
//     Uint32 attachmentCount;
//     Uint32 multiViewCount;
//     PalRenderTargetView* renderTargetView;
//     PalRenderPassAttachmentInfo* attachments;
// } PalRenderPassCreateInfo;

typedef struct {
    PalFormat format;
    PalImageUsages usages;
    PalImageViewUsages viewUsages;
} PalFormatInfo;

typedef struct {
    Uint32 width;
    Uint32 height;
    Uint32 depthOrArraySize;
    Uint32 mipLevelCount;
    Uint32 samples;
    PalImageType type;
    PalFormatInfo format;
} PalImageInfo;

typedef struct {
    bool memoryTypeAllowed[PAL_MEMORY_TYPE_MAX];
    Uint64 size;
    Uint32 alignment;
} PalMemoryRequirements;

typedef struct {
    Uint32 width;
    Uint32 height;
    Uint32 depthOrArraySize;
    Uint32 mipLevelCount;
    Uint32 samples;
    PalImageViewType type;
    PalFormatInfo format;
} PalImageCreateInfo;

typedef struct {
    Uint32 startMipLevel;
    Uint32 mipLevelCount;
    Uint32 startArrayLayer;
    Uint32 layerArrayCount;
    PalImageViewType type;
    PalImageUsages usages;
} PalImageViewCreateInfo;

typedef struct {
    bool clipped;
    Uint32 width;
    Uint32 height;
    Uint32 imageCount;
    Uint32 imageArrayLayerCount;
    PalPresentMode presentMode;
    PalCompositeAplha compositeAlpha;
    PalSwapchainFormat format;
} PalSwapchainCreateInfo;

typedef struct {
    void* display;
    void* window;
} PalGfxWindow;

typedef struct {
    PalResult PAL_CALL (*enumerateAdapters)(
        Int32* count,
        PalAdapter** outAdapters);

    PalResult PAL_CALL (*getAdapterInfo)(
        PalAdapter* adapter,
        PalAdapterInfo* info);

    PalResult PAL_CALL (*getAdapterCapabilities)(
        PalAdapter* adapter,
        PalAdapterCapabilities* caps);

    PalResult PAL_CALL (*createDevice)(
        PalAdapter* adapter,
        PalAdapterFeatures features,
        PalDevice** outDevice);

    void PAL_CALL (*destroyDevice)(PalDevice* device);

    PalResult PAL_CALL (*createQueue)(
        PalDevice* device,
        PalQueueType type,
        PalQueue** outQueue);

    void PAL_CALL (*destroyQueue)(PalQueue* queue);

    bool PAL_CALL (*canQueuePresent)(
        PalQueue* queue, 
        PalGfxWindow* window);

    PalResult PAL_CALL (*createImage)(
        PalDevice* device,
        const PalImageCreateInfo* info,
        PalImage** outImage);

    void PAL_CALL (*destroyImage)(PalImage* image);

    PalResult PAL_CALL (*getImageInfo)(
        PalImage* image,
        PalImageInfo* info);

    PalResult PAL_CALL (*enumerateFormats)(
        PalAdapter* adapter,
        Int32* count,
        PalFormatInfo* outFormats);

    bool PAL_CALL (*isFormatSupported)(
        PalAdapter* adapter,
        PalFormat format);

    PalImageUsages PAL_CALL (*queryFormatUsages)(
        PalAdapter* adapter,
        PalFormat format);

    PalImageViewUsages PAL_CALL (*queryFormatViewUsages)(
        PalAdapter* adapter,
        PalFormat format);

    PalResult PAL_CALL (*getImageMemoryRequirements)(
        PalDevice* device,
        PalImage* image,
        PalMemoryRequirements* requirments);

    PalResult PAL_CALL (*allocate)(
        PalDevice* device,
        PalMemoryType type,
        Uint64 size,
        PalMemory** outMemory);

    void PAL_CALL (*free)(
        PalDevice* device,
        PalMemory* memory);

    PalResult PAL_CALL (*bindImageMemory)(
        PalDevice* device,
        PalImage* image,
        PalMemory* memory,
        Uint64 offset);

    PalResult PAL_CALL (*createImageView)(
        PalDevice* device,
        PalImage* image,
        const PalImageViewCreateInfo* info,
        PalImageView** outImageView);

    void PAL_CALL (*destroyImageView)(PalImageView* imageView);

    PalResult PAL_CALL (*querySwapchainCapabilities)(
        PalAdapter* adapter,
        PalGfxWindow* window,
        PalSwapchainCapabilities* caps);

    PalResult PAL_CALL (*createSwapchain)(
        PalDevice* device,
        PalQueue* queue,
        PalGfxWindow* window,
        const PalSwapchainCreateInfo* info,
        PalSwapchain** outSwapchain);

    void PAL_CALL (*destroySwapchain)(PalSwapchain* swapchain);

    Uint32 PAL_CALL (*getSwapchainImageCount)(PalSwapchain* swapchain);

    PalImage* PAL_CALL (*getSwapchainImage)(
        PalSwapchain* swapchain,
        Int32 index);

    // PalResult PAL_CALL (*queryRenderPassCapabilities)(
    //     PalSwapchain* swapchain,
    //     PalRenderPassCapabilities* caps);

    // PalResult PAL_CALL (*createRenderPass)(
    //     PalSwapchain* swapchain,
    //     PalRenderPassCreateInfo* info,
    //     PalRenderPass** outRenderPass);

    // void PAL_CALL (*destroyRenderPass)(PalRenderPass* renderPass);
} PalGPUBackend;

PAL_API PalResult PAL_CALL palInitGraphics(
    bool enableDebugLayer,
    const PalAllocator* allocator);

PAL_API void PAL_CALL palShutdownGraphics();

PAL_API PalResult PAL_CALL palEnumerateAdapters(
    Int32* count,
    PalAdapter** outAdapters);

PAL_API PalResult PAL_CALL palGetAdapterInfo(
    PalAdapter* adapter,
    PalAdapterInfo* info);

PAL_API PalResult PAL_CALL palGetAdapterCapabilities(
    PalAdapter* adapter,
    PalAdapterCapabilities* caps);

PAL_API PalResult PAL_CALL palAddGPUBackend(const PalGPUBackend* backend);

PAL_API PalResult PAL_CALL palCreateDevice(
    PalAdapter* adapter,
    PalAdapterFeatures features,
    PalDevice** outDevice);

PAL_API void PAL_CALL palDestroyDevice(PalDevice* device);

PAL_API PalResult PAL_CALL palCreateQueue(
    PalDevice* device,
    PalQueueType type,
    PalQueue** outQueue);

PAL_API void PAL_CALL palDestroyQueue(PalQueue* queue);

PAL_API bool PAL_CALL palCanQueuePresent(
    PalQueue* queue, 
    PalGfxWindow* window);

PAL_API PalResult PAL_CALL palCreateImage(
    PalDevice* device,
    const PalImageCreateInfo* info,
    PalImage** outImage);

PAL_API void PAL_CALL palDestroyImage(PalImage* image);

PAL_API PalResult PAL_CALL palGetImageInfo(
    PalImage* image,
    PalImageInfo* info);

PAL_API PalResult PAL_CALL palEnumerateFormats(
    PalAdapter* adapter,
    Int32* count,
    PalFormatInfo* outFormats);

PAL_API bool PAL_CALL palIsFormatSupported(
    PalAdapter* adapter,
    PalFormat format);

PAL_API PalImageUsages PAL_CALL palQueryFormatUsages(
    PalAdapter* adapter,
    PalFormat format);

PAL_API PalImageViewUsages PAL_CALL palQueryFormatViewUsages(
    PalAdapter* adapter,
    PalFormat format);

PAL_API PalResult PAL_CALL palGetImageMemoryRequirements(
    PalDevice* device,
    PalImage* image,
    PalMemoryRequirements* requirements);

PAL_API PalResult PAL_CALL palAllocateMemory(
    PalDevice* device,
    PalMemoryType type,
    Uint64 size,
    PalMemory** outMemory);

PAL_API void PAL_CALL palFreeMemory(
    PalDevice* device,
    PalMemory* memory);

PAL_API PalResult PAL_CALL palBindImageMemory(
    PalDevice* device,
    PalImage* image,
    PalMemory* memory,
    Uint64 offset);

PAL_API PalResult PAL_CALL palCreateImageView(
    PalDevice* device,
    PalImage* image,
    const PalImageViewCreateInfo* info,
    PalImageView** outImageView);

PAL_API void PAL_CALL palDestroyImageView(PalImageView* imageView);

PAL_API PalResult PAL_CALL palQuerySwapchainCapabilities(
    PalAdapter* adapter,
    PalGfxWindow* window,
    PalSwapchainCapabilities* caps);

PAL_API PalResult PAL_CALL palCreateSwapchain(
    PalDevice* device,
    PalQueue* queue,
    PalGfxWindow* window,
    const PalSwapchainCreateInfo* info,
    PalSwapchain** outSwapchain);

PAL_API void PAL_CALL palDestroySwapchain(PalSwapchain* swapchain);

PAL_API Uint32 PAL_CALL palGetSwapchainImageCount(PalSwapchain* swapchain);

PAL_API PalImage* PAL_CALL palGetSwapchainImage(
    PalSwapchain* swapchain,
    Int32 index);

// PAL_API PalResult PAL_CALL palQueryRenderPassCapabilities(
//     PalSwapchain* swapchain,
//     PalRenderPassCapabilities* caps);

// PAL_API PalResult PAL_CALL palCreateRenderPass(
//     PalSwapchain* swapchain,
//     PalRenderPassCreateInfo* info,
//     PalRenderPass** outRenderPass);

// PAL_API void PAL_CALL palDestroyRenderPass(PalRenderPass* renderPass);

/** @} */ // end of pal_graphics group

#endif // _PAL_GRAPHICS_H