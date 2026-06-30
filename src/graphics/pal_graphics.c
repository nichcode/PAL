
/**
    PAL - Prime Abstraction Layer
    Copyright (C) 2025
    Licensed under the Zlib license. See LICENSE file in root.
 */

#include "pal_graphics_backends.h"

#define MAX_BACKENDS (PAL_MAX_CUSTOM_BACKENDS + 2)
#define PAL_HANDLE(name)                                                                           \
    struct name {                                                                                  \
        const PalGraphicsVtable* backend;                                                          \
    };

PAL_HANDLE(PalAdapter)
PAL_HANDLE(PalDevice)
PAL_HANDLE(PalQueue)
PAL_HANDLE(PalSwapchain)
PAL_HANDLE(PalImage)
PAL_HANDLE(PalImageView)
PAL_HANDLE(PalShader)
PAL_HANDLE(PalBuffer)

PAL_HANDLE(PalFence)
PAL_HANDLE(PalSemaphore)
PAL_HANDLE(PalCommandPool)
PAL_HANDLE(PalCommandBuffer)
PAL_HANDLE(PalPipeline)
PAL_HANDLE(PalAccelerationStructure)
PAL_HANDLE(PalPipelineLayout)
PAL_HANDLE(PalDescriptorSetLayout)
PAL_HANDLE(PalDescriptorPool)
PAL_HANDLE(PalDescriptorSet)
PAL_HANDLE(PalSampler)
PAL_HANDLE(PalSurface)
PAL_HANDLE(PalShaderBindingTable)

typedef struct {
    int32_t count;
    int32_t startIndex;
    PalGraphicsVtable base;
} BackendData;

typedef struct {
    PalBool initialized;
    int32_t backendCount;
    const PalAllocator* allocator;
    BackendData backends[MAX_BACKENDS];
} Graphics;

static Graphics s_Graphics = {0};

static inline uint32_t _ceil(
    uint32_t a,
    uint32_t b)
{
    return (a + b - 1) / b;
}

static inline uint32_t _min(
    uint32_t a,
    uint32_t b)
{
    return (a < b) ? a : b;
}

static PalBool validateVtableVersion1(const PalGraphicsBackendVtable1* vtable1)
{
    // clang-format off
    if (!vtable1->enumerateAdapters                             ||
        !vtable1->getAdapterInfo                                ||
        !vtable1->getAdapterCapabilities                        ||
        !vtable1->getAdapterFeatures                            ||
        !vtable1->getHighestSupportedShaderTarget               ||

        // device
        !vtable1->createDevice                                  ||
        !vtable1->destroyDevice                                 ||

        // memory
        !vtable1->allocateMemory                                ||
        !vtable1->freeMemory                                    ||

        // extended adapter features
        !vtable1->querySamplerAnisotropyCapabilities            ||
        !vtable1->queryMultiViewCapabilities                    ||
        !vtable1->queryMultiViewportCapabilities                ||
        !vtable1->queryDepthStencilCapabilities                 ||
        !vtable1->queryFragmentShadingRateCapabilities          ||
        !vtable1->queryMeshShaderCapabilities                   ||
        !vtable1->queryRayTracingCapabilities                   ||
        !vtable1->queryDescriptorIndexingCapabilities           ||

        // queue
        !vtable1->createQueue                                   ||
        !vtable1->destroyQueue                                  ||
        !vtable1->waitQueue                                     ||
        !vtable1->canQueuePresent                               ||

        // formats
        !vtable1->enumerateFormats                              ||
        !vtable1->isFormatSupported                             ||
        !vtable1->queryFormatImageUsages                        ||
        !vtable1->queryFormatSampleCount                        ||

        // image
        !vtable1->createImage                                   ||
        !vtable1->destroyImage                                  ||
        !vtable1->getImageInfo                                  ||
        !vtable1->getImageMemoryRequirements                    ||
        !vtable1->bindImageMemory                               ||
        !vtable1->mapImageMemory                                ||
        !vtable1->unmapImageMemory                              ||

        // image view
        !vtable1->createImageView                               ||
        !vtable1->destroyImageView                              ||

        // sampler
        !vtable1->createSampler                                 ||
        !vtable1->destroySampler                                ||

        // surface
        !vtable1->createSurface                                 ||
        !vtable1->destroySurface                                ||
        !vtable1->getSurfaceCapabilities                        ||

        // swapchain
        !vtable1->createSwapchain                               ||
        !vtable1->destroySwapchain                              ||
        !vtable1->getSwapchainImage                             ||
        !vtable1->getNextSwapchainImage                         ||
        !vtable1->presentSwapchain                              ||
        !vtable1->resizeSwapchain                               ||

        // shader
        !vtable1->createShader                                  ||
        !vtable1->destroyShader                                 ||

        // fence
        !vtable1->createFence                                   ||
        !vtable1->destroyFence                                  ||
        !vtable1->waitFence                                     ||
        !vtable1->resetFence                                    ||
        !vtable1->isFenceSignaled                               ||

        // semaphore
        !vtable1->createSemaphore                               ||
        !vtable1->destroySemaphore                              ||
        !vtable1->waitSemaphore                                 ||
        !vtable1->signalSemaphore                               ||
        !vtable1->getSemaphoreValue                             ||

        // command pool and command buffer
        !vtable1->createCommandPool                             ||
        !vtable1->destroyCommandPool                            ||
        !vtable1->resetCommandPool                              ||
        !vtable1->allocateCommandBuffer                         ||
        !vtable1->freeCommandBuffer                             ||
        !vtable1->submitCommandBuffer                           ||

        // command recording
        !vtable1->cmdBegin                                      ||
        !vtable1->cmdEnd                                        ||
        !vtable1->resetCommandBuffer                            ||
        !vtable1->cmdExecuteCommandBuffer                       ||
        !vtable1->cmdSetFragmentShadingRate                     ||
        !vtable1->cmdDrawMeshTasks                              ||
        !vtable1->cmdDrawMeshTasksIndirect                      ||
        !vtable1->cmdDrawMeshTasksIndirectCount                 ||
        !vtable1->cmdBuildAccelerationStructure                 ||
        !vtable1->cmdBeginRendering                             ||
        !vtable1->cmdEndRendering                               ||
        !vtable1->cmdCopyBuffer                                 ||
        !vtable1->cmdCopyBufferToImage                          ||
        !vtable1->cmdCopyImage                                  ||
        !vtable1->cmdCopyImageToBuffer                          ||
        !vtable1->cmdBindPipeline                               ||
        !vtable1->cmdSetViewport                                ||
        !vtable1->cmdSetScissors                                ||
        !vtable1->cmdBindVertexBuffers                          ||
        !vtable1->cmdBindIndexBuffer                            ||
        !vtable1->cmdDraw                                       ||
        !vtable1->cmdDrawIndirect                               ||
        !vtable1->cmdDrawIndirectCount                          ||
        !vtable1->cmdDrawIndexed                                ||
        !vtable1->cmdDrawIndexedIndirect                        ||
        !vtable1->cmdDrawIndexedIndirectCount                   ||
        !vtable1->cmdAccelerationStructureBarrier               ||
        !vtable1->cmdImageBarrier                               ||
        !vtable1->cmdBufferBarrier                              ||
        !vtable1->cmdDispatch                                   ||
        !vtable1->cmdDispatchBase                               ||
        !vtable1->cmdDispatchIndirect                           ||
        !vtable1->cmdTraceRays                                  ||
        !vtable1->cmdTraceRaysIndirect                          ||
        !vtable1->cmdBindDescriptorSet                          ||
        !vtable1->cmdPushConstants                              ||
        !vtable1->cmdSetCullMode                                ||
        !vtable1->cmdSetFrontFace                               ||
        !vtable1->cmdSetPrimitiveTopology                       ||
        !vtable1->cmdSetDepthTestEnable                         ||
        !vtable1->cmdSetDepthWriteEnable                        ||
        !vtable1->cmdSetStencilOp                               ||

        // acceleration structure
        !vtable1->createAccelerationstructure                   ||
        !vtable1->destroyAccelerationstructure                  ||
        !vtable1->getAccelerationStructureBuildSize             ||

        // buffer
        !vtable1->createBuffer                                  ||
        !vtable1->destroyBuffer                                 ||
        !vtable1->getBufferMemoryRequirements                   ||
        !vtable1->computeInstanceBufferRequirements             ||
        !vtable1->computeImageCopyStagingBufferRequirements     ||
        !vtable1->writeToInstanceBuffer                         ||
        !vtable1->writeToImageCopyStagingBuffer                 ||
        !vtable1->bindBufferMemory                              ||
        !vtable1->getBufferDeviceAddress                        ||
        !vtable1->mapBufferMemory                               ||
        !vtable1->unmapBufferMemory                             ||

        // descriptors
        !vtable1->createDescriptorSetLayout                     ||
        !vtable1->destroyDescriptorSetLayout                    ||
        !vtable1->createDescriptorPool                          ||
        !vtable1->destroyDescriptorPool                         ||
        !vtable1->resetDescriptorPool                           ||
        !vtable1->allocateDescriptorSet                         ||
        !vtable1->updateDescriptorSet                           ||

        // pipeline layout
        !vtable1->createPipelineLayout                          ||
        !vtable1->destroyPipelineLayout                         ||

        // pipeline
        !vtable1->createGraphicsPipeline                        ||
        !vtable1->createComputePipeline                         ||
        !vtable1->createRayTracingPipeline                      ||
        !vtable1->destroyPipeline                               ||

        // shader binding table
        !vtable1->createShaderBindingTable                      ||
        !vtable1->destroyShaderBindingTable                     ||
        !vtable1->updateShaderBindingTable) {
        return PAL_FALSE;
    }
    // clang-format on
    return PAL_TRUE;
}

static void populateVtableVersion1(
    PalGraphicsVtable* vtable,
    const PalGraphicsBackendVtable1* vtable1)
{
    // clang-format off
    vtable->enumerateAdapters = vtable1->enumerateAdapters;
    vtable->getAdapterInfo = vtable1->getAdapterInfo;
    vtable->getAdapterCapabilities = vtable1->getAdapterCapabilities;
    vtable->getAdapterFeatures = vtable1->getAdapterFeatures;
    vtable->getHighestSupportedShaderTarget = vtable1->getHighestSupportedShaderTarget;

    // device
    vtable->createDevice = vtable1->createDevice;
    vtable->destroyDevice = vtable1->destroyDevice;

    // memory
    vtable->allocateMemory = vtable1->allocateMemory;
    vtable->freeMemory = vtable1->freeMemory;

    // extended adapter features
    vtable->querySamplerAnisotropyCapabilities = vtable1->querySamplerAnisotropyCapabilities;
    vtable->queryMultiViewCapabilities = vtable1->queryMultiViewCapabilities;
    vtable->queryMultiViewportCapabilities = vtable1->queryMultiViewportCapabilities;
    vtable->queryDepthStencilCapabilities = vtable1->queryDepthStencilCapabilities;
    vtable->queryFragmentShadingRateCapabilities = vtable1->queryFragmentShadingRateCapabilities;
    vtable->queryMeshShaderCapabilities = vtable1->queryMeshShaderCapabilities;
    vtable->queryRayTracingCapabilities = vtable1->queryRayTracingCapabilities;
    vtable->queryDescriptorIndexingCapabilities = vtable1->queryDescriptorIndexingCapabilities;

    // queue
    vtable->createQueue = vtable1->createQueue;
    vtable->destroyQueue = vtable1->destroyQueue;
    vtable->waitQueue = vtable1->waitQueue;
    vtable->canQueuePresent = vtable1->canQueuePresent;

    // formats
    vtable->enumerateFormats = vtable1->enumerateFormats;
    vtable->isFormatSupported = vtable1->isFormatSupported;
    vtable->queryFormatImageUsages = vtable1->queryFormatImageUsages;
    vtable->queryFormatSampleCount = vtable1->queryFormatSampleCount;

    // image
    vtable->createImage = vtable1->createImage;
    vtable->destroyImage = vtable1->destroyImage;
    vtable->getImageInfo = vtable1->getImageInfo;
    vtable->getImageMemoryRequirements = vtable1->getImageMemoryRequirements;
    vtable->bindImageMemory = vtable1->bindImageMemory;
    vtable->mapImageMemory = vtable1->mapImageMemory;
    vtable->unmapImageMemory = vtable1->unmapImageMemory;

    // image view
    vtable->createImageView = vtable1->createImageView;
    vtable->destroyImageView = vtable1->destroyImageView;

    // sampler
    vtable->createSampler = vtable1->createSampler;
    vtable->destroySampler = vtable1->destroySampler;

    // surface
    vtable->createSurface = vtable1->createSurface;
    vtable->destroySurface = vtable1->destroySurface;
    vtable->getSurfaceCapabilities = vtable1->getSurfaceCapabilities;

    // swapchain
    vtable->createSwapchain = vtable1->createSwapchain;
    vtable->destroySwapchain = vtable1->destroySwapchain;
    vtable->getSwapchainImage = vtable1->getSwapchainImage;
    vtable->getNextSwapchainImage = vtable1->getNextSwapchainImage;
    vtable->presentSwapchain = vtable1->presentSwapchain;
    vtable->resizeSwapchain = vtable1->resizeSwapchain;

    // shader
    vtable->createShader = vtable1->createShader;
    vtable->destroyShader = vtable1->destroyShader;

    // fence
    vtable->createFence = vtable1->createFence;
    vtable->destroyFence = vtable1->destroyFence;
    vtable->waitFence = vtable1->waitFence;
    vtable->resetFence = vtable1->resetFence;
    vtable->isFenceSignaled = vtable1->isFenceSignaled;

    // semaphore
    vtable->createSemaphore = vtable1->createSemaphore;
    vtable->destroySemaphore = vtable1->destroySemaphore;
    vtable->waitSemaphore = vtable1->waitSemaphore;
    vtable->signalSemaphore = vtable1->signalSemaphore;
    vtable->getSemaphoreValue = vtable1->getSemaphoreValue;

    // command pool and command buffer
    vtable->createCommandPool = vtable1->createCommandPool;
    vtable->destroyCommandPool = vtable1->destroyCommandPool;
    vtable->resetCommandPool = vtable1->resetCommandPool;
    vtable->allocateCommandBuffer = vtable1->allocateCommandBuffer;
    vtable->freeCommandBuffer = vtable1->freeCommandBuffer;
    vtable->submitCommandBuffer = vtable1->submitCommandBuffer;

    // command recording
    vtable->cmdBegin = vtable1->cmdBegin;
    vtable->cmdEnd = vtable1->cmdEnd;
    vtable->resetCommandBuffer = vtable1->resetCommandBuffer;
    vtable->cmdExecuteCommandBuffer = vtable1->cmdExecuteCommandBuffer;
    vtable->cmdSetFragmentShadingRate = vtable1->cmdSetFragmentShadingRate;
    vtable->cmdDrawMeshTasks = vtable1->cmdDrawMeshTasks;
    vtable->cmdDrawMeshTasksIndirect = vtable1->cmdDrawMeshTasksIndirect;
    vtable->cmdDrawMeshTasksIndirectCount = vtable1->cmdDrawMeshTasksIndirectCount;
    vtable->cmdBuildAccelerationStructure = vtable1->cmdBuildAccelerationStructure;
    vtable->cmdBeginRendering = vtable1->cmdBeginRendering;
    vtable->cmdEndRendering = vtable1->cmdEndRendering;
    vtable->cmdCopyBuffer = vtable1->cmdCopyBuffer;
    vtable->cmdCopyBufferToImage = vtable1->cmdCopyBufferToImage;
    vtable->cmdCopyImage = vtable1->cmdCopyImage;
    vtable->cmdCopyImageToBuffer = vtable1->cmdCopyImageToBuffer;
    vtable->cmdBindPipeline = vtable1->cmdBindPipeline;
    vtable->cmdSetViewport = vtable1->cmdSetViewport;
    vtable->cmdSetScissors = vtable1->cmdSetScissors;
    vtable->cmdBindVertexBuffers = vtable1->cmdBindVertexBuffers;
    vtable->cmdBindIndexBuffer = vtable1->cmdBindIndexBuffer;
    vtable->cmdDraw = vtable1->cmdDraw;
    vtable->cmdDrawIndirect = vtable1->cmdDrawIndirect;
    vtable->cmdDrawIndirectCount = vtable1->cmdDrawIndirectCount;
    vtable->cmdDrawIndexed = vtable1->cmdDrawIndexed;
    vtable->cmdDrawIndexedIndirect = vtable1->cmdDrawIndexedIndirect;
    vtable->cmdDrawIndexedIndirectCount = vtable1->cmdDrawIndexedIndirectCount;
    vtable->cmdAccelerationStructureBarrier = vtable1->cmdAccelerationStructureBarrier;
    vtable->cmdImageBarrier = vtable1->cmdImageBarrier;
    vtable->cmdBufferBarrier = vtable1->cmdBufferBarrier;
    vtable->cmdDispatch = vtable1->cmdDispatch;
    vtable->cmdDispatchBase = vtable1->cmdDispatchBase;
    vtable->cmdDispatchIndirect = vtable1->cmdDispatchIndirect;
    vtable->cmdTraceRays = vtable1->cmdTraceRays;
    vtable->cmdTraceRaysIndirect = vtable1->cmdTraceRaysIndirect;
    vtable->cmdBindDescriptorSet = vtable1->cmdBindDescriptorSet;
    vtable->cmdPushConstants = vtable1->cmdPushConstants;
    vtable->cmdSetCullMode = vtable1->cmdSetCullMode;
    vtable->cmdSetFrontFace = vtable1->cmdSetFrontFace;
    vtable->cmdSetPrimitiveTopology = vtable1->cmdSetPrimitiveTopology;
    vtable->cmdSetDepthTestEnable = vtable1->cmdSetDepthTestEnable;
    vtable->cmdSetDepthWriteEnable = vtable1->cmdSetDepthWriteEnable;
    vtable->cmdSetStencilOp = vtable1->cmdSetStencilOp;

    // acceleration structure
    vtable->createAccelerationstructure = vtable1->createAccelerationstructure;
    vtable->destroyAccelerationstructure = vtable1->destroyAccelerationstructure;
    vtable->getAccelerationStructureBuildSize = vtable1->getAccelerationStructureBuildSize;

    // buffer
    vtable->createBuffer = vtable1->createBuffer;
    vtable->destroyBuffer = vtable1->destroyBuffer;
    vtable->getBufferMemoryRequirements = vtable1->getBufferMemoryRequirements;
    vtable->computeInstanceBufferRequirements = vtable1->computeInstanceBufferRequirements;
    vtable->computeImageCopyStagingBufferRequirements = vtable1->computeImageCopyStagingBufferRequirements;
    vtable->writeToInstanceBuffer = vtable1->writeToInstanceBuffer;

    vtable->writeToImageCopyStagingBuffer = vtable1->writeToImageCopyStagingBuffer;
    vtable->bindBufferMemory = vtable1->bindBufferMemory;
    vtable->getBufferDeviceAddress = vtable1->getBufferDeviceAddress;
    vtable->mapBufferMemory = vtable1->mapBufferMemory;
    vtable->unmapBufferMemory = vtable1->unmapBufferMemory;

    // descriptors
    vtable->createDescriptorSetLayout = vtable1->createDescriptorSetLayout;
    vtable->destroyDescriptorSetLayout = vtable1->destroyDescriptorSetLayout;
    vtable->createDescriptorPool = vtable1->createDescriptorPool;
    vtable->destroyDescriptorPool = vtable1->destroyDescriptorPool;
    vtable->resetDescriptorPool = vtable1->resetDescriptorPool;
    vtable->allocateDescriptorSet = vtable1->allocateDescriptorSet;
    vtable->updateDescriptorSet = vtable1->updateDescriptorSet;

    // pipeline layout
    vtable->createPipelineLayout = vtable1->createPipelineLayout;
    vtable->destroyPipelineLayout = vtable1->destroyPipelineLayout;

    // pipeline
    vtable->createGraphicsPipeline = vtable1->createGraphicsPipeline;
    vtable->createComputePipeline = vtable1->createComputePipeline;
    vtable->createRayTracingPipeline = vtable1->createRayTracingPipeline;
    vtable->destroyPipeline = vtable1->destroyPipeline;

    // shader binding table
    vtable->createShaderBindingTable = vtable1->createShaderBindingTable;
    vtable->destroyShaderBindingTable = vtable1->destroyShaderBindingTable;
    vtable->updateShaderBindingTable = vtable1->updateShaderBindingTable;
    // clang-format on
}

static void addBackend(PalGraphicsBackendInfo* backendInfo)
{
    BackendData* backendData = &s_Graphics.backends[s_Graphics.backendCount++];
    backendData->startIndex = 0;
    backendData->count = 0;

    // populate our internal vtable
    if (backendInfo->version == PAL_GRAPHICS_BACKEND_VTABLE_VERSION_1) {
        // validate that all version 1 pointers are set
        const PalGraphicsBackendVtable1* vtable1 = (PalGraphicsBackendVtable1*)backendInfo->vtable;
        validateVtableVersion1(vtable1);

        memset(&backendData->base, 0, sizeof(PalGraphicsVtable));
        populateVtableVersion1(&backendData->base, vtable1);
    }
}

PalResult PAL_CALL palInitGraphics(
    const PalGraphicsDebugger* debugger,
    const PalAllocator* allocator,
    uint32_t customBackendCount,
    const PalGraphicsBackendInfo* customBackends)
{
    if (s_Graphics.initialized) {
        return PAL_RESULT_SUCCESS;
    }

    if (customBackendCount == 0) {
        return PAL_RESULT_CODE_PLATFORM_FAILURE;
    }

    if (allocator && (!allocator->allocate || !allocator->free)) {
        return PAL_RESULT_CODE_INVALID_ARGUMENT;
    }

    PalResult result;
    BackendData* attachedBackend = nullptr;
#ifdef _WIN32
    // vulkan
#if PAL_HAS_VULKAN_BACKEND
    // result = initGraphicsVk(debugger, allocator);
    // if (result != PAL_RESULT_SUCCESS) {
    //     return result;
    // }

    // attachedBackend = &s_Graphics.backends[s_Graphics.backendCount++];
    // attachedBackend->base = &s_VkBackend;
    // attachedBackend->startIndex = 0;
    // attachedBackend->count = 0;
#endif // PAL_HAS_VULKAN_BACKEND

    // D3D12
#if PAL_HAS_D3D12_BACKEND
    result = initGraphicsD3D12(debugger, allocator);
    if (result != PAL_RESULT_SUCCESS) {
        return result;
    }

    attachedBackend = &s_Graphics.backends[s_Graphics.backendCount++];
    attachedBackend->base = &s_D3D12Backend;
    attachedBackend->startIndex = 0;
    attachedBackend->count = 0;
#endif // PAL_HAS_D3D12_BACKEND

#elif defined(__linux__)
    // vulkan
#if PAL_HAS_VULKAN_BACKEND
    result = initGraphicsVk(debugger, allocator);
    if (result != PAL_RESULT_SUCCESS) {
        return result;
    }

    attachedBackend = &s_Graphics.backends[s_Graphics.backendCount++];
    attachedBackend->base = &s_VkBackend;
    attachedBackend->startIndex = 0;
    attachedBackend->count = 0;
#endif // PAL_HAS_VULKAN_BACKEND
#else
    // metal or andriod
#endif // _WIN32

    s_Graphics.allocator = allocator;
    s_Graphics.initialized = PAL_TRUE;
    return PAL_RESULT_SUCCESS;
}

void PAL_CALL palShutdownGraphics()
{
    if (!s_Graphics.initialized) {
        return;
    }

#ifdef _WIN32
    // vulkan
#if PAL_HAS_VULKAN_BACKEND
    // shutdownGraphicsVk();
#endif // PAL_HAS_VULKAN_BACKEND

    // D3D12
#if PAL_HAS_D3D12_BACKEND
    shutdownGraphicsD3D12();
#endif // PAL_HAS_D3D12_BACKEND

#elif defined(__linux__)
    // vulkan
#if PAL_HAS_VULKAN_BACKEND
    shutdownGraphicsVk();
#endif // PAL_HAS_VULKAN_BACKEND

#else
    // metal or andriod
#endif // _WIN32

    memset(&s_Graphics, 0, sizeof(s_Graphics));
    s_Graphics.initialized = PAL_FALSE;
}

// ==================================================
// Adapter
// ==================================================

PalResult PAL_CALL palEnumerateAdapters(
    int32_t* count,
    PalAdapter** outAdapters)
{
    // enumerate all adapters for both custom and PAL backends
    if (!s_Graphics.initialized) {
        return PAL_RESULT_CODE_NOT_INITIALIZED;
    }

    if (!count || *count == 0 && outAdapters) {
        return PAL_RESULT_CODE_INVALID_ARGUMENT;
    }

    PalResult result;
    int totalCount = 0;
    int index = 0;
    int _count = 0;

    for (int i = 0; i < s_Graphics.backendCount; i++) {
        BackendData* backend = &s_Graphics.backends[i];
        if (outAdapters) {
            // offset into the array so all backends write at the correct index
            PalAdapter** adapters = &outAdapters[backend->startIndex];
            _count = backend->count;
            result = backend->base.enumerateAdapters(&_count, adapters);
            // break if a backend fails
            if (result != PAL_RESULT_SUCCESS) {
                return result;
            }

            for (int j = 0; j < _count; j++) {
                PalAdapter* tmp = adapters[j];
                adapters[j]->backend = &backend->base;
            }

        } else {
            result = backend->base.enumerateAdapters(&_count, nullptr);
            // break if a backend fails
            if (result != PAL_RESULT_SUCCESS) {
                return result;
            }

            backend->startIndex = totalCount;
            backend->count = _count;
            totalCount += _count;
            _count = 0;
        }
    }

    if (!outAdapters) {
        *count = totalCount;
    }
    return PAL_RESULT_SUCCESS;
}

PalResult PAL_CALL palGetAdapterInfo(
    PalAdapter* adapter,
    PalAdapterInfo* info)
{
    if (!s_Graphics.initialized) {
        return PAL_RESULT_CODE_NOT_INITIALIZED;
    }

    if (!adapter || !info) {
        return PAL_RESULT_CODE_INVALID_ARGUMENT;
    }

    return adapter->backend->getAdapterInfo(adapter, info);
}

PalResult PAL_CALL palGetAdapterCapabilities(
    PalAdapter* adapter,
    PalAdapterCapabilities* caps)
{
    if (!s_Graphics.initialized) {
        return PAL_RESULT_CODE_NOT_INITIALIZED;
    }

    if (!adapter || !caps) {
        return PAL_RESULT_CODE_INVALID_ARGUMENT;
    }

    return adapter->backend->getAdapterCapabilities(adapter, caps);
}

PalAdapterFeatures PAL_CALL palGetAdapterFeatures(PalAdapter* adapter)
{
    if (!s_Graphics.initialized || !adapter) {
        return 0;
    }
    return adapter->backend->getAdapterFeatures(adapter);
}

uint32_t PAL_CALL palGetHighestSupportedShaderTarget(
    PalAdapter* adapter,
    PalShaderFormats shaderFormat)
{
    if (!s_Graphics.initialized || !adapter) {
        return 0;
    }
    return adapter->backend->getHighestSupportedShaderTarget(adapter, shaderFormat);
}

// ==================================================
// Device
// ==================================================

PalResult PAL_CALL palCreateDevice(
    PalAdapter* adapter,
    PalAdapterFeatures features,
    PalDevice** outDevice)
{
    if (!s_Graphics.initialized) {
        return PAL_RESULT_CODE_NOT_INITIALIZED;
    }

    if (!outDevice) {
        return PAL_RESULT_CODE_INVALID_ARGUMENT;
    }

    PalDevice* device = nullptr;
    PalResult result;
    result = adapter->backend->createDevice(adapter, features, &device);
    if (result != PAL_RESULT_SUCCESS) {
        return result;
    }

    device->backend = adapter->backend;
    *outDevice = device;
    return PAL_RESULT_SUCCESS;
}

void PAL_CALL palDestroyDevice(PalDevice* device)
{
    if (s_Graphics.initialized && device) {
        device->backend->destroyDevice(device);
    }
}

PalResult PAL_CALL palAllocateMemory(
    PalDevice* device,
    PalMemoryType type,
    uint64_t memoryMask,
    uint64_t size,
    PalMemory** outMemory)
{
    if (!s_Graphics.initialized) {
        return PAL_RESULT_CODE_NOT_INITIALIZED;
    }

    if (!outMemory || !device) {
        return PAL_RESULT_CODE_INVALID_ARGUMENT;
    }

    return device->backend->allocateMemory(device, type, memoryMask, size, outMemory);
}

void PAL_CALL palFreeMemory(
    PalDevice* device,
    PalMemory* memory)
{
    if (s_Graphics.initialized && device && memory) {
        device->backend->freeMemory(device, memory);
    }
}

// ==================================================
// Extended Adapter Features
// ==================================================

PalResult PAL_CALL palQuerySamplerAnisotropyCapabilities(
    PalDevice* device,
    PalSamplerAnisotropyCapabilities* caps)
{
    if (!s_Graphics.initialized) {
        return PAL_RESULT_CODE_NOT_INITIALIZED;
    }

    if (!device || !caps) {
        return PAL_RESULT_CODE_INVALID_ARGUMENT;
    }

    return device->backend->querySamplerAnisotropyCapabilities(device, caps);
}

PalResult PAL_CALL palQueryMultiViewCapabilities(
    PalDevice* device,
    PalMultiViewCapabilities* caps)
{
    if (!s_Graphics.initialized) {
        return PAL_RESULT_CODE_NOT_INITIALIZED;
    }

    if (!device || !caps) {
        return PAL_RESULT_CODE_INVALID_ARGUMENT;
    }

    return device->backend->queryMultiViewCapabilities(device, caps);
}

PalResult PAL_CALL palQueryMultiViewportCapabilities(
    PalDevice* device,
    PalMultiViewportCapabilities* caps)
{
    if (!s_Graphics.initialized) {
        return PAL_RESULT_CODE_NOT_INITIALIZED;
    }

    if (!device || !caps) {
        return PAL_RESULT_CODE_INVALID_ARGUMENT;
    }

    return device->backend->queryMultiViewportCapabilities(device, caps);
}

PalResult PAL_CALL palQueryDepthStencilCapabilities(
    PalDevice* device,
    PalDepthStencilCapabilities* caps)
{
    if (!s_Graphics.initialized) {
        return PAL_RESULT_CODE_NOT_INITIALIZED;
    }

    if (!device || !caps) {
        return PAL_RESULT_CODE_INVALID_ARGUMENT;
    }

    return device->backend->queryDepthStencilCapabilities(device, caps);
}

PalResult PAL_CALL palQueryFragmentShadingRateCapabilities(
    PalDevice* device,
    PalFragmentShadingRateCapabilities* caps)
{
    if (!s_Graphics.initialized) {
        return PAL_RESULT_CODE_NOT_INITIALIZED;
    }

    if (!device || !caps) {
        return PAL_RESULT_CODE_INVALID_ARGUMENT;
    }

    return device->backend->queryFragmentShadingRateCapabilities(device, caps);
}

PalResult PAL_CALL palQueryMeshShaderCapabilities(
    PalDevice* device,
    PalMeshShaderCapabilities* caps)
{
    if (!s_Graphics.initialized) {
        return PAL_RESULT_CODE_NOT_INITIALIZED;
    }

    if (!device || !caps) {
        return PAL_RESULT_CODE_INVALID_ARGUMENT;
    }

    return device->backend->queryMeshShaderCapabilities(device, caps);
}

PalResult PAL_CALL palQueryRayTracingCapabilities(
    PalDevice* device,
    PalRayTracingCapabilities* caps)
{
    if (!s_Graphics.initialized) {
        return PAL_RESULT_CODE_NOT_INITIALIZED;
    }

    if (!device || !caps) {
        return PAL_RESULT_CODE_INVALID_ARGUMENT;
    }

    return device->backend->queryRayTracingCapabilities(device, caps);
}

PalResult PAL_CALL palQueryDescriptorIndexingCapabilities(
    PalDevice* device,
    PalDescriptorIndexingCapabilities* caps)
{
    if (!s_Graphics.initialized) {
        return PAL_RESULT_CODE_NOT_INITIALIZED;
    }

    if (!device || !caps) {
        return PAL_RESULT_CODE_INVALID_ARGUMENT;
    }

    return device->backend->queryDescriptorIndexingCapabilities(device, caps);
}

// ==================================================
// Queue
// ==================================================

PalResult PAL_CALL palCreateQueue(
    PalDevice* device,
    PalQueueType type,
    PalQueue** outQueue)
{
    if (!s_Graphics.initialized) {
        return PAL_RESULT_CODE_NOT_INITIALIZED;
    }

    if (!device || !outQueue) {
        return PAL_RESULT_CODE_INVALID_ARGUMENT;
    }

    PalQueue* queue = nullptr;
    PalResult result;
    result = device->backend->createQueue(device, type, &queue);
    if (result != PAL_RESULT_SUCCESS) {
        return result;
    }

    queue->backend = device->backend;
    *outQueue = queue;
    return PAL_RESULT_SUCCESS;
}

void PAL_CALL palDestroyQueue(PalQueue* queue)
{
    if (s_Graphics.initialized && queue) {
        queue->backend->destroyQueue(queue);
    }
}

PalBool PAL_CALL palCanQueuePresent(
    PalQueue* queue,
    PalSurface* surface)
{
    if (s_Graphics.initialized && queue) {
        return queue->backend->canQueuePresent(queue, surface);
    }
    return PAL_FALSE;
}

PalResult PAL_CALL palWaitQueue(PalQueue* queue)
{
    if (!s_Graphics.initialized) {
        return PAL_RESULT_CODE_NOT_INITIALIZED;
    }

    if (!queue) {
        return PAL_RESULT_CODE_INVALID_ARGUMENT;
    }

    return queue->backend->waitQueue(queue);
}

// ==================================================
// Format And Usages
// ==================================================

PalResult PAL_CALL palEnumerateFormats(
    PalAdapter* adapter,
    int32_t* count,
    PalFormatInfo* outFormats)
{
    if (!s_Graphics.initialized) {
        return PAL_RESULT_CODE_NOT_INITIALIZED;
    }

    if (!adapter || !count || *count == 0 && outFormats) {
        return PAL_RESULT_CODE_INVALID_ARGUMENT;
    }

    return adapter->backend->enumerateFormats(adapter, count, outFormats);
}

PalBool PAL_CALL palIsFormatSupported(
    PalAdapter* adapter,
    PalFormat format)
{
    if (!s_Graphics.initialized || !adapter) {
        return PAL_FALSE;
    }

    return adapter->backend->isFormatSupported(adapter, format);
}

PalImageUsages PAL_CALL palQueryFormatImageUsages(
    PalAdapter* adapter,
    PalFormat format)
{
    if (!s_Graphics.initialized || !adapter) {
        return PAL_IMAGE_USAGE_UNDEFINED;
    }

    return adapter->backend->queryFormatImageUsages(adapter, format);
}

PalSampleCount PAL_CALL palQueryFormatSampleCount(
    PalAdapter* adapter,
    PalFormat format)
{
    if (!s_Graphics.initialized || !adapter) {
        return PAL_SAMPLE_COUNT_1;
    }

    return adapter->backend->queryFormatSampleCount(adapter, format);
}

// ==================================================
// Image
// ==================================================

PalResult PAL_CALL palCreateImage(
    PalDevice* device,
    const PalImageCreateInfo* info,
    PalImage** outImage)
{
    if (!s_Graphics.initialized) {
        return PAL_RESULT_CODE_NOT_INITIALIZED;
    }

    if (!device || !info || !outImage) {
        return PAL_RESULT_CODE_INVALID_ARGUMENT;
    }

    PalImage* image = nullptr;
    PalResult result;
    result = device->backend->createImage(device, info, &image);
    if (result != PAL_RESULT_SUCCESS) {
        return result;
    }

    image->backend = device->backend;
    *outImage = image;
    return PAL_RESULT_SUCCESS;
}

void PAL_CALL palDestroyImage(PalImage* image)
{
    if (s_Graphics.initialized && image) {
        image->backend->destroyImage(image);
    }
}

PalResult PAL_CALL palGetImageInfo(
    PalImage* image,
    PalImageInfo* info)
{
    if (!s_Graphics.initialized) {
        return PAL_RESULT_CODE_NOT_INITIALIZED;
    }

    if (!image || !info) {
        return PAL_RESULT_CODE_INVALID_ARGUMENT;
    }

    return image->backend->getImageInfo(image, info);
}

PalResult PAL_CALL palGetImageMemoryRequirements(
    PalImage* image,
    PalMemoryRequirements* requirements)
{
    if (!s_Graphics.initialized) {
        return PAL_RESULT_CODE_NOT_INITIALIZED;
    }

    if (!image) {
        return PAL_RESULT_CODE_INVALID_ARGUMENT;
    }

    return image->backend->getImageMemoryRequirements(image, requirements);
}

PalResult PAL_CALL palBindImageMemory(
    PalImage* image,
    PalMemory* memory,
    uint64_t offset)
{
    if (!s_Graphics.initialized) {
        return PAL_RESULT_CODE_NOT_INITIALIZED;
    }

    if (!image || !memory) {
        return PAL_RESULT_CODE_INVALID_ARGUMENT;
    }

    return image->backend->bindImageMemory(image, memory, offset);
}

PalResult PAL_CALL palMapImageMemory(
    PalImage* image,
    uint64_t offset,
    uint64_t size,
    void** outPtr)
{
    if (!s_Graphics.initialized) {
        return PAL_RESULT_CODE_NOT_INITIALIZED;
    }

    if (!image) {
        return PAL_RESULT_CODE_INVALID_ARGUMENT;
    }

    return image->backend->mapImageMemory(image, offset, size, outPtr);
}

void PAL_CALL palUnmapImageMemory(PalImage* image)
{
    if (s_Graphics.initialized && image) {
        image->backend->unmapImageMemory(image);
    }
}

// ==================================================
// Image View
// ==================================================

PalResult PAL_CALL palCreateImageView(
    PalDevice* device,
    PalImage* image,
    const PalImageViewCreateInfo* info,
    PalImageView** outImageView)
{
    if (!s_Graphics.initialized) {
        return PAL_RESULT_CODE_NOT_INITIALIZED;
    }

    if (!device || !image || !info || !outImageView) {
        return PAL_RESULT_CODE_INVALID_ARGUMENT;
    }

    PalImageView* imageView = nullptr;
    PalResult result;
    result = device->backend->createImageView(device, image, info, &imageView);
    if (result != PAL_RESULT_SUCCESS) {
        return result;
    }

    imageView->backend = device->backend;
    *outImageView = imageView;
    return PAL_RESULT_SUCCESS;
}

void PAL_CALL palDestroyImageView(PalImageView* imageView)
{
    if (s_Graphics.initialized && imageView) {
        imageView->backend->destroyImageView(imageView);
    }
}

// ==================================================
// Sampler
// ==================================================

PalResult PAL_CALL palCreateSampler(
    PalDevice* device,
    const PalSamplerCreateInfo* info,
    PalSampler** outSampler)
{
    if (!s_Graphics.initialized) {
        return PAL_RESULT_CODE_NOT_INITIALIZED;
    }

    if (!device || !info || !outSampler) {
        return PAL_RESULT_CODE_INVALID_ARGUMENT;
    }

    PalSampler* sampler = nullptr;
    PalResult result;
    result = device->backend->createSampler(device, info, &sampler);
    if (result != PAL_RESULT_SUCCESS) {
        return result;
    }

    sampler->backend = device->backend;
    *outSampler = sampler;
    return PAL_RESULT_SUCCESS;
}

void PAL_CALL palDestroySampler(PalSampler* sampler)
{
    if (s_Graphics.initialized && sampler) {
        sampler->backend->destroySampler(sampler);
    }
}

// ==================================================
// Surface
// ==================================================

PalResult PAL_CALL palCreateSurface(
    PalDevice* device,
    void* window,
    void* windowInstance,
    PalWindowInstanceType instanceType,
    PalSurface** outSurface)
{
    if (!s_Graphics.initialized) {
        return PAL_RESULT_CODE_NOT_INITIALIZED;
    }

    if (!device || !window || !outSurface) {
        return PAL_RESULT_CODE_INVALID_ARGUMENT;
    }

    PalSurface* surface = nullptr;
    PalResult result;
    result = device->backend->createSurface(device, window, windowInstance, instanceType, &surface);
    if (result != PAL_RESULT_SUCCESS) {
        return result;
    }

    surface->backend = device->backend;
    *outSurface = surface;
    return PAL_RESULT_SUCCESS;
}

void PAL_CALL palDestroySurface(PalSurface* surface)
{
    if (s_Graphics.initialized && surface) {
        surface->backend->destroySurface(surface);
    }
}

PalResult PAL_CALL palGetSurfaceCapabilities(
    PalDevice* device,
    PalSurface* surface,
    PalSurfaceCapabilities* caps)
{
    if (!s_Graphics.initialized) {
        return PAL_RESULT_CODE_NOT_INITIALIZED;
    }

    if (!device || !surface || !caps) {
        return PAL_RESULT_CODE_INVALID_ARGUMENT;
    }

    return device->backend->getSurfaceCapabilities(device, surface, caps);
}

// ==================================================
// Swapchain
// ==================================================

PalResult PAL_CALL palCreateSwapchain(
    PalDevice* device,
    PalQueue* queue,
    PalSurface* surface,
    const PalSwapchainCreateInfo* info,
    PalSwapchain** outSwapchain)
{
    if (!s_Graphics.initialized) {
        return PAL_RESULT_CODE_NOT_INITIALIZED;
    }

    if (!device || !queue || !surface || !info || !outSwapchain) {
        return PAL_RESULT_CODE_INVALID_ARGUMENT;
    }

    PalResult result;
    PalSwapchain* swapchain = nullptr;
    result = device->backend->createSwapchain(device, queue, surface, info, &swapchain);
    if (result != PAL_RESULT_SUCCESS) {
        return result;
    }

    // set the backend for all swapchain images
    for (int i = 0; i < info->imageCount; i++) {
        PalImage* image = device->backend->getSwapchainImage(swapchain, i);
        image->backend = device->backend;
    }

    swapchain->backend = device->backend;
    *outSwapchain = swapchain;
    return PAL_RESULT_SUCCESS;
}

void PAL_CALL palDestroySwapchain(PalSwapchain* swapchain)
{
    if (s_Graphics.initialized && swapchain) {
        swapchain->backend->destroySwapchain(swapchain);
    }
}

PalImage* PAL_CALL palGetSwapchainImage(
    PalSwapchain* swapchain,
    int32_t index)
{
    if (!s_Graphics.initialized || !swapchain || index < 0) {
        return nullptr;
    }

    return swapchain->backend->getSwapchainImage(swapchain, index);
}

PalResult PAL_CALL palGetNextSwapchainImage(
    PalSwapchain* swapchain,
    PalSwapchainNextImageInfo* info,
    uint32_t* outIndex)
{
    if (!s_Graphics.initialized) {
        return PAL_RESULT_CODE_NOT_INITIALIZED;
    }

    if (!swapchain || !info) {
        return PAL_RESULT_CODE_INVALID_ARGUMENT;
    }

    return swapchain->backend->getNextSwapchainImage(swapchain, info, outIndex);
}

PalResult PAL_CALL palPresentSwapchain(
    PalSwapchain* swapchain,
    PalSwapchainPresentInfo* info)
{
    if (!s_Graphics.initialized) {
        return PAL_RESULT_CODE_NOT_INITIALIZED;
    }

    if (!swapchain || !info) {
        return PAL_RESULT_CODE_INVALID_ARGUMENT;
    }

    return swapchain->backend->presentSwapchain(swapchain, info);
}

PalResult PAL_CALL palResizeSwapchain(
    PalSwapchain* swapchain,
    uint32_t newWidth,
    uint32_t newHeight)
{
    if (!s_Graphics.initialized) {
        return PAL_RESULT_CODE_NOT_INITIALIZED;
    }

    if (!swapchain) {
        return PAL_RESULT_CODE_INVALID_ARGUMENT;
    }

    return swapchain->backend->resizeSwapchain(swapchain, newWidth, newHeight);
}

// ==================================================
// Shader
// ==================================================

PalResult PAL_CALL palCreateShader(
    PalDevice* device,
    const PalShaderCreateInfo* info,
    PalShader** outShader)
{
    if (!s_Graphics.initialized) {
        return PAL_RESULT_CODE_NOT_INITIALIZED;
    }

    if (!device || !info || !outShader) {
        return PAL_RESULT_CODE_INVALID_ARGUMENT;
    }

    PalShader* shader = nullptr;
    PalResult result;
    result = device->backend->createShader(device, info, &shader);
    if (result != PAL_RESULT_SUCCESS) {
        return result;
    }

    shader->backend = device->backend;
    *outShader = shader;
    return PAL_RESULT_SUCCESS;
}

void PAL_CALL palDestroyShader(PalShader* shader)
{
    if (s_Graphics.initialized && shader) {
        shader->backend->destroyShader(shader);
    }
}

// ==================================================
// Fence
// ==================================================

PalResult PAL_CALL palCreateFence(
    PalDevice* device,
    PalBool signaled,
    PalFence** outFence)
{
    if (!s_Graphics.initialized) {
        return PAL_RESULT_CODE_NOT_INITIALIZED;
    }

    if (!device || !outFence) {
        return PAL_RESULT_CODE_INVALID_ARGUMENT;
    }

    PalFence* fence = nullptr;
    PalResult result;
    result = device->backend->createFence(device, signaled, &fence);
    if (result != PAL_RESULT_SUCCESS) {
        return result;
    }

    fence->backend = device->backend;
    *outFence = fence;
    return PAL_RESULT_SUCCESS;
}

void PAL_CALL palDestroyFence(PalFence* fence)
{
    if (s_Graphics.initialized && fence) {
        fence->backend->destroyFence(fence);
    }
}

PalResult PAL_CALL palWaitFence(
    PalFence* fence,
    uint64_t timeout)
{
    if (!s_Graphics.initialized) {
        return PAL_RESULT_CODE_NOT_INITIALIZED;
    }

    if (!fence) {
        return PAL_RESULT_CODE_INVALID_ARGUMENT;
    }

    return fence->backend->waitFence(fence, timeout);
}

PalResult PAL_CALL palResetFence(PalFence* fence)
{
    if (!s_Graphics.initialized) {
        return PAL_RESULT_CODE_NOT_INITIALIZED;
    }

    if (!fence) {
        return PAL_RESULT_CODE_INVALID_ARGUMENT;
    }

    return fence->backend->resetFence(fence);
}

PalBool PAL_CALL palIsFenceSignaled(PalFence* fence)
{
    if (s_Graphics.initialized && fence) {
        return fence->backend->isFenceSignaled(fence);
    }
    return PAL_FALSE;
}

// ==================================================
// Semaphore
// ==================================================

PalResult PAL_CALL palCreateSemaphore(
    PalDevice* device,
    PalBool enableTimeline,
    PalSemaphore** outSemaphore)
{
    if (!s_Graphics.initialized) {
        return PAL_RESULT_CODE_NOT_INITIALIZED;
    }

    if (!device || !outSemaphore) {
        return PAL_RESULT_CODE_INVALID_ARGUMENT;
    }

    PalSemaphore* semaphore = nullptr;
    PalResult result;
    result = device->backend->createSemaphore(device, enableTimeline, &semaphore);
    if (result != PAL_RESULT_SUCCESS) {
        return result;
    }

    semaphore->backend = device->backend;
    *outSemaphore = semaphore;
    return PAL_RESULT_SUCCESS;
}

void PAL_CALL palDestroySemaphore(PalSemaphore* semaphore)
{
    if (s_Graphics.initialized && semaphore) {
        semaphore->backend->destroySemaphore(semaphore);
    }
}

PalResult PAL_CALL palWaitSemaphore(
    PalSemaphore* semaphore,
    uint64_t value,
    uint64_t timeout)
{
    if (!s_Graphics.initialized) {
        return PAL_RESULT_CODE_NOT_INITIALIZED;
    }

    if (!semaphore) {
        return PAL_RESULT_CODE_INVALID_ARGUMENT;
    }

    return semaphore->backend->waitSemaphore(semaphore, value, timeout);
}

PalResult PAL_CALL palSignalSemaphore(
    PalSemaphore* semaphore,
    PalQueue* queue,
    uint64_t value)
{
    if (!s_Graphics.initialized) {
        return PAL_RESULT_CODE_NOT_INITIALIZED;
    }

    if (!semaphore || !queue) {
        return PAL_RESULT_CODE_INVALID_ARGUMENT;
    }

    return semaphore->backend->signalSemaphore(semaphore, queue, value);
}

PalResult PAL_CALL palGetSemaphoreValue(
    PalSemaphore* semaphore,
    uint64_t* outValue)
{
    if (!s_Graphics.initialized) {
        return PAL_RESULT_CODE_NOT_INITIALIZED;
    }

    if (!semaphore || !outValue) {
        return PAL_RESULT_CODE_INVALID_ARGUMENT;
    }

    return semaphore->backend->getSemaphoreValue(semaphore, outValue);
}

// ==================================================
// Command Pool And Buffer
// ==================================================

PalResult PAL_CALL palCreateCommandPool(
    PalDevice* device,
    PalQueue* queue,
    PalCommandPool** outPool)
{
    if (!s_Graphics.initialized) {
        return PAL_RESULT_CODE_NOT_INITIALIZED;
    }

    if (!device || !queue || !outPool) {
        return PAL_RESULT_CODE_INVALID_ARGUMENT;
    }

    PalCommandPool* pool = nullptr;
    PalResult result;
    result = device->backend->createCommandPool(device, queue, &pool);
    if (result != PAL_RESULT_SUCCESS) {
        return result;
    }

    pool->backend = device->backend;
    *outPool = pool;
    return PAL_RESULT_SUCCESS;
}

void PAL_CALL palDestroyCommandPool(PalCommandPool* pool)
{
    if (s_Graphics.initialized && pool) {
        pool->backend->destroyCommandPool(pool);
    }
}

PalResult PAL_CALL palResetCommandPool(PalCommandPool* pool)
{
    if (!s_Graphics.initialized) {
        return PAL_RESULT_CODE_NOT_INITIALIZED;
    }

    if (!pool) {
        return PAL_RESULT_CODE_INVALID_ARGUMENT;
    }

    return pool->backend->resetCommandPool(pool);
}

PalResult PAL_CALL palAllocateCommandBuffer(
    PalDevice* device,
    PalCommandPool* pool,
    PalCommandBufferType type,
    PalCommandBuffer** outCmdBuffer)
{
    if (!s_Graphics.initialized) {
        return PAL_RESULT_CODE_NOT_INITIALIZED;
    }

    if (!device || !pool || !outCmdBuffer) {
        return PAL_RESULT_CODE_INVALID_ARGUMENT;
    }

    PalCommandBuffer* cmdBuffer = nullptr;
    PalResult result;
    result = device->backend->allocateCommandBuffer(device, pool, type, &cmdBuffer);
    if (result != PAL_RESULT_SUCCESS) {
        return result;
    }

    cmdBuffer->backend = device->backend;
    *outCmdBuffer = cmdBuffer;
    return PAL_RESULT_SUCCESS;
}

void PAL_CALL palFreeCommandBuffer(PalCommandBuffer* cmdBuffer)
{
    if (s_Graphics.initialized && cmdBuffer) {
        cmdBuffer->backend->freeCommandBuffer(cmdBuffer);
    }
}

PalResult PAL_CALL palResetCommandBuffer(PalCommandBuffer* cmdBuffer)
{
    if (!s_Graphics.initialized) {
        return PAL_RESULT_CODE_NOT_INITIALIZED;
    }

    if (!cmdBuffer) {
        return PAL_RESULT_CODE_INVALID_ARGUMENT;
    }

    return cmdBuffer->backend->resetCommandBuffer(cmdBuffer);
}

PalResult PAL_CALL palSubmitCommandBuffer(
    PalQueue* queue,
    PalCommandBufferSubmitInfo* info)
{
    if (!s_Graphics.initialized) {
        return PAL_RESULT_CODE_NOT_INITIALIZED;
    }

    if (!queue || !info) {
        return PAL_RESULT_CODE_INVALID_ARGUMENT;
    }

    return queue->backend->submitCommandBuffer(queue, info);
}

// ==================================================
// Command Recording
// ==================================================

PalResult PAL_CALL palCmdBegin(
    PalCommandBuffer* cmdBuffer,
    PalRenderingLayoutInfo* info)
{
    if (!s_Graphics.initialized) {
        return PAL_RESULT_CODE_NOT_INITIALIZED;
    }

    if (!cmdBuffer) {
        return PAL_RESULT_CODE_INVALID_ARGUMENT;
    }

    return cmdBuffer->backend->cmdBegin(cmdBuffer, info);
}

PalResult PAL_CALL palCmdEnd(PalCommandBuffer* cmdBuffer)
{
    if (!s_Graphics.initialized) {
        return PAL_RESULT_CODE_NOT_INITIALIZED;
    }

    if (!cmdBuffer) {
        return PAL_RESULT_CODE_INVALID_ARGUMENT;
    }

    return cmdBuffer->backend->cmdEnd(cmdBuffer);
}

PalResult PAL_CALL palCmdExecuteCommandBuffer(
    PalCommandBuffer* primaryCmdBuffer,
    PalCommandBuffer* secondaryCmdBuffer)
{
    if (!s_Graphics.initialized) {
        return PAL_RESULT_CODE_NOT_INITIALIZED;
    }

    if (!primaryCmdBuffer || !secondaryCmdBuffer) {
        return PAL_RESULT_CODE_INVALID_ARGUMENT;
    }

    return primaryCmdBuffer->backend->cmdExecuteCommandBuffer(primaryCmdBuffer, secondaryCmdBuffer);
}

PalResult PAL_CALL palCmdSetFragmentShadingRate(
    PalCommandBuffer* cmdBuffer,
    PalFragmentShadingRateState* state)
{
    if (!s_Graphics.initialized) {
        return PAL_RESULT_CODE_NOT_INITIALIZED;
    }

    if (!cmdBuffer || !state) {
        return PAL_RESULT_CODE_INVALID_ARGUMENT;
    }

    return cmdBuffer->backend->cmdSetFragmentShadingRate(cmdBuffer, state);
}

PalResult PAL_CALL palCmdDrawMeshTasks(
    PalCommandBuffer* cmdBuffer,
    uint32_t groupCountX,
    uint32_t groupCountY,
    uint32_t groupCountZ)
{
    if (!s_Graphics.initialized) {
        return PAL_RESULT_CODE_NOT_INITIALIZED;
    }

    if (!cmdBuffer) {
        return PAL_RESULT_CODE_INVALID_ARGUMENT;
    }

    return cmdBuffer->backend->cmdDrawMeshTasks(cmdBuffer, groupCountX, groupCountY, groupCountZ);
}

PalResult PAL_CALL palCmdDrawMeshTasksIndirect(
    PalCommandBuffer* cmdBuffer,
    PalBuffer* buffer,
    uint32_t drawCount)
{
    if (!s_Graphics.initialized) {
        return PAL_RESULT_CODE_NOT_INITIALIZED;
    }

    if (!cmdBuffer || !buffer) {
        return PAL_RESULT_CODE_INVALID_ARGUMENT;
    }

    return cmdBuffer->backend->cmdDrawMeshTasksIndirect(cmdBuffer, buffer, drawCount);
}

PalResult PAL_CALL palCmdDrawMeshTasksIndirectCount(
    PalCommandBuffer* cmdBuffer,
    PalBuffer* buffer,
    PalBuffer* countBuffer,
    uint32_t maxDrawCount)
{
    if (!s_Graphics.initialized) {
        return PAL_RESULT_CODE_NOT_INITIALIZED;
    }

    if (!cmdBuffer || !buffer || !countBuffer) {
        return PAL_RESULT_CODE_INVALID_ARGUMENT;
    }

    return cmdBuffer->backend
        ->cmdDrawMeshTasksIndirectCount(cmdBuffer, buffer, countBuffer, maxDrawCount);
}

PalResult PAL_CALL palCmdBuildAccelerationStructure(
    PalCommandBuffer* cmdBuffer,
    PalAccelerationStructureBuildInfo* info)
{
    if (!s_Graphics.initialized) {
        return PAL_RESULT_CODE_NOT_INITIALIZED;
    }

    if (!cmdBuffer || !info) {
        return PAL_RESULT_CODE_INVALID_ARGUMENT;
    }

    if (!info->dst || info->scratchBufferAddress == 0) {
        return PAL_RESULT_CODE_INVALID_ARGUMENT;
    }

    return cmdBuffer->backend->cmdBuildAccelerationStructure(cmdBuffer, info);
}

PalResult PAL_CALL palCmdBeginRendering(
    PalCommandBuffer* cmdBuffer,
    PalRenderingInfo* info)
{
    if (!s_Graphics.initialized) {
        return PAL_RESULT_CODE_NOT_INITIALIZED;
    }

    if (!cmdBuffer || !info) {
        return PAL_RESULT_CODE_INVALID_ARGUMENT;
    }

    return cmdBuffer->backend->cmdBeginRendering(cmdBuffer, info);
}

PalResult PAL_CALL palCmdEndRendering(PalCommandBuffer* cmdBuffer)
{
    if (!s_Graphics.initialized) {
        return PAL_RESULT_CODE_NOT_INITIALIZED;
    }

    if (!cmdBuffer) {
        return PAL_RESULT_CODE_INVALID_ARGUMENT;
    }

    return cmdBuffer->backend->cmdEndRendering(cmdBuffer);
}

PalResult PAL_CALL palCmdCopyBuffer(
    PalCommandBuffer* cmdBuffer,
    PalBuffer* dst,
    PalBuffer* src,
    PalBufferCopyInfo* copyInfo)
{
    if (!s_Graphics.initialized) {
        return PAL_RESULT_CODE_NOT_INITIALIZED;
    }

    if (!cmdBuffer || !dst || !src || !copyInfo) {
        return PAL_RESULT_CODE_INVALID_ARGUMENT;
    }

    return cmdBuffer->backend->cmdCopyBuffer(cmdBuffer, dst, src, copyInfo);
}

PalResult PAL_CALL palCmdCopyBufferToImage(
    PalCommandBuffer* cmdBuffer,
    PalImage* dstImage,
    PalBuffer* srcBuffer,
    PalBufferImageCopyInfo* copyInfo)
{
    if (!s_Graphics.initialized) {
        return PAL_RESULT_CODE_NOT_INITIALIZED;
    }

    if (!cmdBuffer || !dstImage || !srcBuffer || !copyInfo) {
        return PAL_RESULT_CODE_INVALID_ARGUMENT;
    }

    return cmdBuffer->backend->cmdCopyBufferToImage(cmdBuffer, dstImage, srcBuffer, copyInfo);
}

PalResult PAL_CALL palCmdCopyImage(
    PalCommandBuffer* cmdBuffer,
    PalImage* dst,
    PalImage* src,
    PalImageCopyInfo* copyInfo)
{
    if (!s_Graphics.initialized) {
        return PAL_RESULT_CODE_NOT_INITIALIZED;
    }

    if (!cmdBuffer || !dst || !src || !copyInfo) {
        return PAL_RESULT_CODE_INVALID_ARGUMENT;
    }

    return cmdBuffer->backend->cmdCopyImage(cmdBuffer, dst, src, copyInfo);
}

PalResult PAL_CALL palCmdCopyImageToBuffer(
    PalCommandBuffer* cmdBuffer,
    PalBuffer* dstBuffer,
    PalImage* srcImage,
    PalBufferImageCopyInfo* copyInfo)
{
    if (!s_Graphics.initialized) {
        return PAL_RESULT_CODE_NOT_INITIALIZED;
    }

    if (!cmdBuffer || !dstBuffer || !srcImage || !copyInfo) {
        return PAL_RESULT_CODE_INVALID_ARGUMENT;
    }

    return cmdBuffer->backend->cmdCopyImageToBuffer(cmdBuffer, dstBuffer, srcImage, copyInfo);
}

PalResult PAL_CALL palCmdBindPipeline(
    PalCommandBuffer* cmdBuffer,
    PalPipeline* pipeline)
{
    if (!s_Graphics.initialized) {
        return PAL_RESULT_CODE_NOT_INITIALIZED;
    }

    if (!cmdBuffer || !pipeline) {
        return PAL_RESULT_CODE_INVALID_ARGUMENT;
    }

    return cmdBuffer->backend->cmdBindPipeline(cmdBuffer, pipeline);
}

PalResult PAL_CALL palCmdSetViewport(
    PalCommandBuffer* cmdBuffer,
    uint32_t count,
    PalViewport* viewports)
{
    if (!s_Graphics.initialized) {
        return PAL_RESULT_CODE_NOT_INITIALIZED;
    }

    if (!cmdBuffer || !viewports || !count) {
        return PAL_RESULT_CODE_INVALID_ARGUMENT;
    }

    return cmdBuffer->backend->cmdSetViewport(cmdBuffer, count, viewports);
}

PalResult PAL_CALL palCmdSetScissors(
    PalCommandBuffer* cmdBuffer,
    uint32_t count,
    PalRect2D* scissors)
{
    if (!s_Graphics.initialized) {
        return PAL_RESULT_CODE_NOT_INITIALIZED;
    }

    if (!cmdBuffer || !scissors || !count) {
        return PAL_RESULT_CODE_INVALID_ARGUMENT;
    }

    return cmdBuffer->backend->cmdSetScissors(cmdBuffer, count, scissors);
}

PalResult PAL_CALL palCmdBindVertexBuffers(
    PalCommandBuffer* cmdBuffer,
    uint32_t firstSlot,
    uint32_t count,
    PalBuffer** buffers,
    uint64_t* offsets)
{
    if (!s_Graphics.initialized) {
        return PAL_RESULT_CODE_NOT_INITIALIZED;
    }

    if (!cmdBuffer || !buffers || !offsets) {
        return PAL_RESULT_CODE_INVALID_ARGUMENT;
    }

    return cmdBuffer->backend->cmdBindVertexBuffers(cmdBuffer, firstSlot, count, buffers, offsets);
}

PalResult PAL_CALL palCmdBindIndexBuffer(
    PalCommandBuffer* cmdBuffer,
    PalBuffer* buffer,
    uint64_t offset,
    PalIndexType type)
{
    if (!s_Graphics.initialized) {
        return PAL_RESULT_CODE_NOT_INITIALIZED;
    }

    if (!cmdBuffer || !buffer) {
        return PAL_RESULT_CODE_INVALID_ARGUMENT;
    }

    return cmdBuffer->backend->cmdBindIndexBuffer(cmdBuffer, buffer, offset, type);
}

PalResult PAL_CALL palCmdDraw(
    PalCommandBuffer* cmdBuffer,
    uint32_t vertexCount,
    uint32_t instanceCount,
    uint32_t firstVertex,
    uint32_t firstInstance)
{
    if (!s_Graphics.initialized) {
        return PAL_RESULT_CODE_NOT_INITIALIZED;
    }

    if (!cmdBuffer) {
        return PAL_RESULT_CODE_INVALID_ARGUMENT;
    }

    return cmdBuffer->backend
        ->cmdDraw(cmdBuffer, vertexCount, instanceCount, firstVertex, firstInstance);
}

PalResult PAL_CALL palCmdDrawIndirect(
    PalCommandBuffer* cmdBuffer,
    PalBuffer* buffer,
    uint32_t count)
{
    if (!s_Graphics.initialized) {
        return PAL_RESULT_CODE_NOT_INITIALIZED;
    }

    if (!cmdBuffer || !buffer) {
        return PAL_RESULT_CODE_INVALID_ARGUMENT;
    }

    return cmdBuffer->backend->cmdDrawIndirect(cmdBuffer, buffer, count);
}

PalResult PAL_CALL palCmdDrawIndirectCount(
    PalCommandBuffer* cmdBuffer,
    PalBuffer* buffer,
    PalBuffer* countBuffer,
    uint32_t maxDrawCount)
{
    if (!s_Graphics.initialized) {
        return PAL_RESULT_CODE_NOT_INITIALIZED;
    }

    if (!cmdBuffer || !buffer || !countBuffer) {
        return PAL_RESULT_CODE_INVALID_ARGUMENT;
    }

    return cmdBuffer->backend->cmdDrawIndirectCount(cmdBuffer, buffer, countBuffer, maxDrawCount);
}

PalResult PAL_CALL palCmdDrawIndexed(
    PalCommandBuffer* cmdBuffer,
    uint32_t indexCount,
    uint32_t instanceCount,
    uint32_t firstIndex,
    int32_t vertexOffset,
    uint32_t firstInstance)
{
    if (!s_Graphics.initialized) {
        return PAL_RESULT_CODE_NOT_INITIALIZED;
    }

    if (!cmdBuffer) {
        return PAL_RESULT_CODE_INVALID_ARGUMENT;
    }

    return cmdBuffer->backend->cmdDrawIndexed(
        cmdBuffer,
        indexCount,
        instanceCount,
        firstIndex,
        vertexOffset,
        firstInstance);
}

PalResult PAL_CALL palCmdDrawIndexedIndirect(
    PalCommandBuffer* cmdBuffer,
    PalBuffer* buffer,
    uint32_t count)
{
    if (!s_Graphics.initialized) {
        return PAL_RESULT_CODE_NOT_INITIALIZED;
    }

    if (!cmdBuffer || !buffer) {
        return PAL_RESULT_CODE_INVALID_ARGUMENT;
    }

    return cmdBuffer->backend->cmdDrawIndexedIndirect(cmdBuffer, buffer, count);
}

PalResult PAL_CALL palCmdDrawIndexedIndirectCount(
    PalCommandBuffer* cmdBuffer,
    PalBuffer* buffer,
    PalBuffer* countBuffer,
    uint32_t maxDrawCount)
{
    if (!s_Graphics.initialized) {
        return PAL_RESULT_CODE_NOT_INITIALIZED;
    }

    if (!cmdBuffer || !buffer || !countBuffer) {
        return PAL_RESULT_CODE_INVALID_ARGUMENT;
    }

    return cmdBuffer->backend
        ->cmdDrawIndexedIndirectCount(cmdBuffer, buffer, countBuffer, maxDrawCount);
}

PalResult PAL_CALL palCmdAccelerationStructureBarrier(
    PalCommandBuffer* cmdBuffer,
    PalAccelerationStructure* as,
    PalUsageState oldUsageState,
    PalUsageState newUsageState)
{
    if (!s_Graphics.initialized) {
        return PAL_RESULT_CODE_NOT_INITIALIZED;
    }

    if (!cmdBuffer || !as) {
        return PAL_RESULT_CODE_INVALID_ARGUMENT;
    }

    return cmdBuffer->backend
        ->cmdAccelerationStructureBarrier(cmdBuffer, as, oldUsageState, newUsageState);
}

PalResult PAL_CALL palCmdImageBarrier(
    PalCommandBuffer* cmdBuffer,
    PalImage* image,
    PalImageSubresourceRange* subresourceRange,
    PalUsageState oldUsageState,
    PalUsageState newUsageState)
{
    if (!s_Graphics.initialized) {
        return PAL_RESULT_CODE_NOT_INITIALIZED;
    }

    if (!cmdBuffer || !image || !subresourceRange) {
        return PAL_RESULT_CODE_INVALID_ARGUMENT;
    }

    return cmdBuffer->backend
        ->cmdImageBarrier(cmdBuffer, image, subresourceRange, oldUsageState, newUsageState);
}

PalResult PAL_CALL palCmdBufferBarrier(
    PalCommandBuffer* cmdBuffer,
    PalBuffer* buffer,
    PalUsageState oldUsageState,
    PalUsageState newUsageState)
{
    if (!s_Graphics.initialized) {
        return PAL_RESULT_CODE_NOT_INITIALIZED;
    }

    if (!cmdBuffer || !buffer) {
        return PAL_RESULT_CODE_INVALID_ARGUMENT;
    }

    return cmdBuffer->backend
        ->cmdBufferBarrier(cmdBuffer, buffer, oldUsageState, newUsageState);
}

PalResult PAL_CALL palCmdDispatch(
    PalCommandBuffer* cmdBuffer,
    uint32_t groupCountX,
    uint32_t groupCountY,
    uint32_t groupCountZ)
{
    if (!s_Graphics.initialized) {
        return PAL_RESULT_CODE_NOT_INITIALIZED;
    }

    if (!cmdBuffer) {
        return PAL_RESULT_CODE_INVALID_ARGUMENT;
    }

    return cmdBuffer->backend->cmdDispatch(cmdBuffer, groupCountX, groupCountY, groupCountZ);
}

PalResult PAL_CALL palCmdDispatchBase(
    PalCommandBuffer* cmdBuffer,
    uint32_t baseGroupX,
    uint32_t baseGroupY,
    uint32_t baseGroupZ,
    uint32_t groupCountX,
    uint32_t groupCountY,
    uint32_t groupCountZ)
{
    if (!s_Graphics.initialized) {
        return PAL_RESULT_CODE_NOT_INITIALIZED;
    }

    if (!cmdBuffer) {
        return PAL_RESULT_CODE_INVALID_ARGUMENT;
    }

    return cmdBuffer->backend->cmdDispatchBase(
        cmdBuffer,
        baseGroupX,
        baseGroupY,
        baseGroupZ,
        groupCountX,
        groupCountY,
        groupCountZ);
}

PalResult PAL_CALL palCmdDispatchIndirect(
    PalCommandBuffer* cmdBuffer,
    PalBuffer* buffer)
{
    if (!s_Graphics.initialized) {
        return PAL_RESULT_CODE_NOT_INITIALIZED;
    }

    if (!cmdBuffer || !buffer) {
        return PAL_RESULT_CODE_INVALID_ARGUMENT;
    }

    return cmdBuffer->backend->cmdDispatchIndirect(cmdBuffer, buffer);
}

PalResult PAL_CALL palCmdTraceRays(
    PalCommandBuffer* cmdBuffer,
    PalShaderBindingTable* sbt,
    uint32_t raygenIndex,
    uint32_t width,
    uint32_t height,
    uint32_t depth)
{
    if (!s_Graphics.initialized) {
        return PAL_RESULT_CODE_NOT_INITIALIZED;
    }

    if (!cmdBuffer || !sbt) {
        return PAL_RESULT_CODE_INVALID_ARGUMENT;
    }

    return cmdBuffer->backend->cmdTraceRays(cmdBuffer, sbt, raygenIndex, width, height, depth);
}

PalResult PAL_CALL palCmdTraceRaysIndirect(
    PalCommandBuffer* cmdBuffer,
    uint32_t raygenIndex,
    PalShaderBindingTable* sbt,
    PalBuffer* buffer)
{
    if (!s_Graphics.initialized) {
        return PAL_RESULT_CODE_NOT_INITIALIZED;
    }

    if (!cmdBuffer || !sbt || !buffer) {
        return PAL_RESULT_CODE_INVALID_ARGUMENT;
    }

    return cmdBuffer->backend->cmdTraceRaysIndirect(cmdBuffer, raygenIndex, sbt, buffer);
}

PalResult PAL_CALL palCmdBindDescriptorSet(
    PalCommandBuffer* cmdBuffer,
    uint32_t setIndex,
    PalDescriptorSet* set)
{
    if (!s_Graphics.initialized) {
        return PAL_RESULT_CODE_NOT_INITIALIZED;
    }

    if (!cmdBuffer || !set) {
        return PAL_RESULT_CODE_INVALID_ARGUMENT;
    }

    return cmdBuffer->backend->cmdBindDescriptorSet(cmdBuffer, setIndex, set);
}

PalResult PAL_CALL palCmdPushConstants(
    PalCommandBuffer* cmdBuffer,
    uint32_t offset,
    uint32_t size,
    const void* value)
{
    if (!s_Graphics.initialized) {
        return PAL_RESULT_CODE_NOT_INITIALIZED;
    }

    if (!cmdBuffer || !value) {
        return PAL_RESULT_CODE_INVALID_ARGUMENT;
    }

    return cmdBuffer->backend->cmdPushConstants(cmdBuffer, offset, size, value);
}

PalResult PAL_CALL palCmdSetCullMode(
    PalCommandBuffer* cmdBuffer,
    PalCullMode cullMode)
{
    if (!s_Graphics.initialized) {
        return PAL_RESULT_CODE_NOT_INITIALIZED;
    }

    if (!cmdBuffer) {
        return PAL_RESULT_CODE_INVALID_ARGUMENT;
    }

    return cmdBuffer->backend->cmdSetCullMode(cmdBuffer, cullMode);
}

PalResult PAL_CALL palCmdSetFrontFace(
    PalCommandBuffer* cmdBuffer,
    PalFrontFace frontFace)
{
    if (!s_Graphics.initialized) {
        return PAL_RESULT_CODE_NOT_INITIALIZED;
    }

    if (!cmdBuffer) {
        return PAL_RESULT_CODE_INVALID_ARGUMENT;
    }

    return cmdBuffer->backend->cmdSetFrontFace(cmdBuffer, frontFace);
}

PalResult PAL_CALL palCmdSetPrimitiveTopology(
    PalCommandBuffer* cmdBuffer,
    PalPrimitiveTopology topology)
{
    if (!s_Graphics.initialized) {
        return PAL_RESULT_CODE_NOT_INITIALIZED;
    }

    if (!cmdBuffer) {
        return PAL_RESULT_CODE_INVALID_ARGUMENT;
    }

    return cmdBuffer->backend->cmdSetPrimitiveTopology(cmdBuffer, topology);
}

PalResult PAL_CALL palCmdSetDepthTestEnable(
    PalCommandBuffer* cmdBuffer,
    PalBool enable)
{
    if (!s_Graphics.initialized) {
        return PAL_RESULT_CODE_NOT_INITIALIZED;
    }

    if (!cmdBuffer) {
        return PAL_RESULT_CODE_INVALID_ARGUMENT;
    }

    return cmdBuffer->backend->cmdSetDepthTestEnable(cmdBuffer, enable);
}

PalResult PAL_CALL palCmdSetDepthWriteEnable(
    PalCommandBuffer* cmdBuffer,
    PalBool enable)
{
    if (!s_Graphics.initialized) {
        return PAL_RESULT_CODE_NOT_INITIALIZED;
    }

    if (!cmdBuffer) {
        return PAL_RESULT_CODE_INVALID_ARGUMENT;
    }

    return cmdBuffer->backend->cmdSetDepthWriteEnable(cmdBuffer, enable);
}

PalResult PAL_CALL palCmdSetStencilOp(
    PalCommandBuffer* cmdBuffer,
    PalStencilFaceFlags faceMask,
    PalStencilOp failOp,
    PalStencilOp passOp,
    PalStencilOp depthFailOp,
    PalCompareOp compareOp)
{
    if (!s_Graphics.initialized) {
        return PAL_RESULT_CODE_NOT_INITIALIZED;
    }

    if (!cmdBuffer) {
        return PAL_RESULT_CODE_INVALID_ARGUMENT;
    }

    return cmdBuffer->backend
        ->cmdSetStencilOp(cmdBuffer, faceMask, failOp, passOp, depthFailOp, compareOp);
}

// ==================================================
// Acceleration Structure
// ==================================================

PalResult PAL_CALL palCreateAccelerationstructure(
    PalDevice* device,
    const PalAccelerationStructureCreateInfo* info,
    PalAccelerationStructure** outAs)
{
    if (!s_Graphics.initialized) {
        return PAL_RESULT_CODE_NOT_INITIALIZED;
    }

    if (!device || !info || !outAs) {
        return PAL_RESULT_CODE_INVALID_ARGUMENT;
    }

    if (!info->buffer) {
        return PAL_RESULT_CODE_INVALID_ARGUMENT;
    }

    PalResult result;
    PalAccelerationStructure* as = nullptr;
    result = device->backend->createAccelerationstructure(device, info, &as);
    if (result != PAL_RESULT_SUCCESS) {
        return result;
    }

    as->backend = device->backend;
    *outAs = as;
    return PAL_RESULT_SUCCESS;
}

void PAL_CALL palDestroyAccelerationstructure(PalAccelerationStructure* as)
{
    if (s_Graphics.initialized && as) {
        as->backend->destroyAccelerationstructure(as);
    }
}

PalResult PAL_CALL palGetAccelerationStructureBuildSize(
    PalDevice* device,
    PalAccelerationStructureBuildInfo* info,
    PalAccelerationStructureBuildSize* size)
{
    if (!s_Graphics.initialized) {
        return PAL_RESULT_CODE_NOT_INITIALIZED;
    }

    if (!device || !info || !size) {
        return PAL_RESULT_CODE_INVALID_ARGUMENT;
    }

    return device->backend->getAccelerationStructureBuildSize(device, info, size);
}

// ==================================================
// Buffer
// ==================================================

PalResult PAL_CALL palCreateBuffer(
    PalDevice* device,
    const PalBufferCreateInfo* info,
    PalBuffer** outBuffer)
{
    if (!s_Graphics.initialized) {
        return PAL_RESULT_CODE_NOT_INITIALIZED;
    }

    if (!device || !info) {
        return PAL_RESULT_CODE_INVALID_ARGUMENT;
    }

    PalResult result;
    PalBuffer* buffer = nullptr;
    result = device->backend->createBuffer(device, info, &buffer);
    if (result != PAL_RESULT_SUCCESS) {
        return result;
    }

    buffer->backend = device->backend;
    *outBuffer = buffer;
    return PAL_RESULT_SUCCESS;
}

void PAL_CALL palDestroyBuffer(PalBuffer* buffer)
{
    if (s_Graphics.initialized && buffer) {
        buffer->backend->destroyBuffer(buffer);
    }
}

PalResult PAL_CALL palGetBufferMemoryRequirements(
    PalBuffer* buffer,
    PalMemoryRequirements* requirements)
{
    if (!s_Graphics.initialized) {
        return PAL_RESULT_CODE_NOT_INITIALIZED;
    }

    if (!buffer) {
        return PAL_RESULT_CODE_INVALID_ARGUMENT;
    }

    return buffer->backend->getBufferMemoryRequirements(buffer, requirements);
}

PalResult PAL_CALL palComputeInstanceBufferRequirements(
    PalDevice* device,
    uint32_t instanceCount,
    uint64_t* outSize)
{
    if (!s_Graphics.initialized) {
        return PAL_RESULT_CODE_NOT_INITIALIZED;
    }

    if (!device || !outSize) {
        return PAL_RESULT_CODE_INVALID_ARGUMENT;
    }

    return device->backend->computeInstanceBufferRequirements(device, instanceCount, outSize);
}

PalResult PAL_CALL palComputeImageCopyStagingBufferRequirements(
    PalDevice* device,
    PalFormat imageFormat,
    PalBufferImageCopyInfo* copyInfo,
    uint32_t* outBufferRowLength,
    uint32_t* outBufferImageHeight,
    uint64_t* outSize)
{
    if (!s_Graphics.initialized) {
        return PAL_RESULT_CODE_NOT_INITIALIZED;
    }

    if (!device || !outBufferRowLength || !outBufferImageHeight || !outSize) {
        return PAL_RESULT_CODE_INVALID_ARGUMENT;
    }

    if (copyInfo->bufferRowLength < copyInfo->imageWidth && copyInfo->bufferRowLength) {
        return PAL_RESULT_CODE_INVALID_ARGUMENT;
    }

    if (copyInfo->bufferImageHeight < copyInfo->imageHeight && copyInfo->bufferImageHeight) {
        return PAL_RESULT_CODE_INVALID_ARGUMENT;
    }

    return device->backend->computeImageCopyStagingBufferRequirements(
        device,
        imageFormat,
        copyInfo,
        outBufferRowLength,
        outBufferImageHeight,
        outSize);
}

PalResult PAL_CALL palWriteToInstanceBuffer(
    PalDevice* device,
    void* ptr,
    PalAccelerationStructureInstance* instances,
    uint32_t instanceCount)
{
    if (!s_Graphics.initialized) {
        return PAL_RESULT_CODE_NOT_INITIALIZED;
    }

    if (!device || !ptr || !instances || instanceCount == 0) {
        return PAL_RESULT_CODE_INVALID_ARGUMENT;
    }

    // since all tlas have backend pointer, we use the first one
    return device->backend->writeToInstanceBuffer(device, ptr, instances, instanceCount);
}

PalResult PAL_CALL palWriteToImageCopyStagingBuffer(
    PalDevice* device,
    void* ptr,
    void* srcData,
    PalFormat imageFormat,
    PalBufferImageCopyInfo* copyInfo)
{
    if (!s_Graphics.initialized) {
        return PAL_RESULT_CODE_NOT_INITIALIZED;
    }

    if (!device || !ptr || !srcData || !copyInfo) {
        return PAL_RESULT_CODE_INVALID_ARGUMENT;
    }

    if (copyInfo->bufferRowLength < copyInfo->imageWidth && copyInfo->bufferRowLength) {
        return PAL_RESULT_CODE_INVALID_ARGUMENT;
    }

    if (copyInfo->bufferImageHeight < copyInfo->imageHeight && copyInfo->bufferImageHeight) {
        return PAL_RESULT_CODE_INVALID_ARGUMENT;
    }

    return device->backend
        ->writeToImageCopyStagingBuffer(device, ptr, srcData, imageFormat, copyInfo);
}

PalResult PAL_CALL palBindBufferMemory(
    PalBuffer* buffer,
    PalMemory* memory,
    uint64_t offset)
{
    if (!s_Graphics.initialized) {
        return PAL_RESULT_CODE_NOT_INITIALIZED;
    }

    if (!buffer || !memory) {
        return PAL_RESULT_CODE_INVALID_ARGUMENT;
    }

    return buffer->backend->bindBufferMemory(buffer, memory, offset);
}

PalResult PAL_CALL palMapBufferMemory(
    PalBuffer* buffer,
    uint64_t offset,
    uint64_t size,
    void** outPtr)
{
    if (!s_Graphics.initialized) {
        return PAL_RESULT_CODE_NOT_INITIALIZED;
    }

    if (!buffer) {
        return PAL_RESULT_CODE_INVALID_ARGUMENT;
    }

    return buffer->backend->mapBufferMemory(buffer, offset, size, outPtr);
}

void PAL_CALL palUnmapBufferMemory(PalBuffer* buffer)
{
    if (s_Graphics.initialized && buffer) {
        buffer->backend->unmapBufferMemory(buffer);
    }
}

PalDeviceAddress PAL_CALL palGetBufferDeviceAddress(PalBuffer* buffer)
{
    if (!s_Graphics.initialized || !buffer) {
        return 0;
    }
    return buffer->backend->getBufferDeviceAddress(buffer);
}

// ==================================================
// Descriptor Pool, Set and Layout
// ==================================================

PalResult PAL_CALL palCreateDescriptorSetLayout(
    PalDevice* device,
    const PalDescriptorSetLayoutCreateInfo* info,
    PalDescriptorSetLayout** outLayout)
{
    if (!s_Graphics.initialized) {
        return PAL_RESULT_CODE_NOT_INITIALIZED;
    }

    if (!device || !info || !outLayout) {
        return PAL_RESULT_CODE_INVALID_ARGUMENT;
    }

    PalResult result;
    PalDescriptorSetLayout* layout = nullptr;
    result = device->backend->createDescriptorSetLayout(device, info, &layout);
    if (result != PAL_RESULT_SUCCESS) {
        return result;
    }

    layout->backend = device->backend;
    *outLayout = layout;
    return PAL_RESULT_SUCCESS;
}

void PAL_CALL palDestroyDescriptorSetLayout(PalDescriptorSetLayout* layout)
{
    if (s_Graphics.initialized && layout) {
        layout->backend->destroyDescriptorSetLayout(layout);
    }
}

PalResult PAL_CALL palCreateDescriptorPool(
    PalDevice* device,
    const PalDescriptorPoolCreateInfo* info,
    PalDescriptorPool** outPool)
{
    if (!s_Graphics.initialized) {
        return PAL_RESULT_CODE_NOT_INITIALIZED;
    }

    if (!device || !info || !outPool) {
        return PAL_RESULT_CODE_INVALID_ARGUMENT;
    }

    if (!info->maxDescriptorSets) {
        return PAL_RESULT_CODE_INVALID_ARGUMENT;
    }

    PalResult result;
    PalDescriptorPool* pool = nullptr;
    result = device->backend->createDescriptorPool(device, info, &pool);
    if (result != PAL_RESULT_SUCCESS) {
        return result;
    }

    pool->backend = device->backend;
    *outPool = pool;
    return PAL_RESULT_SUCCESS;
}

void PAL_CALL palDestroyDescriptorPool(PalDescriptorPool* pool)
{
    if (s_Graphics.initialized && pool) {
        pool->backend->destroyDescriptorPool(pool);
    }
}

PalResult PAL_CALL palResetDescriptorPool(PalDescriptorPool* pool)
{
    if (!s_Graphics.initialized) {
        return PAL_RESULT_CODE_NOT_INITIALIZED;
    }

    if (!pool) {
        return PAL_RESULT_CODE_INVALID_ARGUMENT;
    }

    return pool->backend->resetDescriptorPool(pool);
}

PalResult PAL_CALL palAllocateDescriptorSet(
    PalDevice* device,
    PalDescriptorPool* pool,
    PalDescriptorSetLayout* layout,
    PalDescriptorSet** outSet)
{
    if (!s_Graphics.initialized) {
        return PAL_RESULT_CODE_NOT_INITIALIZED;
    }

    if (!device || !pool || !layout || !outSet) {
        return PAL_RESULT_CODE_INVALID_ARGUMENT;
    }

    PalResult result;
    PalDescriptorSet* set = nullptr;
    result = device->backend->allocateDescriptorSet(device, pool, layout, &set);
    if (result != PAL_RESULT_SUCCESS) {
        return result;
    }

    set->backend = device->backend;
    *outSet = set;
    return PAL_RESULT_SUCCESS;
}

PalResult PAL_CALL palUpdateDescriptorSet(
    PalDevice* device,
    uint32_t count,
    PalDescriptorSetWriteInfo* infos)
{
    if (!s_Graphics.initialized) {
        return PAL_RESULT_CODE_NOT_INITIALIZED;
    }

    if (!device || !infos || count == 0 && infos) {
        return PAL_RESULT_CODE_INVALID_ARGUMENT;
    }

    return device->backend->updateDescriptorSet(device, count, infos);
}

// ==================================================
// Pipeline Layout
// ==================================================

PalResult PAL_CALL palCreatePipelineLayout(
    PalDevice* device,
    const PalPipelineLayoutCreateInfo* info,
    PalPipelineLayout** outLayout)
{
    if (!s_Graphics.initialized) {
        return PAL_RESULT_CODE_NOT_INITIALIZED;
    }

    if (!device || !info || !outLayout) {
        return PAL_RESULT_CODE_INVALID_ARGUMENT;
    }

    PalResult result;
    PalPipelineLayout* layout = nullptr;
    result = device->backend->createPipelineLayout(device, info, &layout);
    if (result != PAL_RESULT_SUCCESS) {
        return result;
    }

    layout->backend = device->backend;
    *outLayout = layout;
    return PAL_RESULT_SUCCESS;
}

void PAL_CALL palDestroyPipelineLayout(PalPipelineLayout* layout)
{
    if (s_Graphics.initialized && layout) {
        layout->backend->destroyPipelineLayout(layout);
    }
}

// ==================================================
// Pipeline
// ==================================================

PalResult PAL_CALL palCreateGraphicsPipeline(
    PalDevice* device,
    const PalGraphicsPipelineCreateInfo* info,
    PalPipeline** outPipeline)
{
    if (!s_Graphics.initialized) {
        return PAL_RESULT_CODE_NOT_INITIALIZED;
    }

    if (!device || !info || !outPipeline) {
        return PAL_RESULT_CODE_INVALID_ARGUMENT;
    }

    if (!info->renderingLayout) {
        return PAL_RESULT_CODE_INVALID_ARGUMENT;
    }

    PalResult result;
    PalPipeline* pipeline = nullptr;
    result = device->backend->createGraphicsPipeline(device, info, &pipeline);
    if (result != PAL_RESULT_SUCCESS) {
        return result;
    }

    pipeline->backend = device->backend;
    *outPipeline = pipeline;
    return PAL_RESULT_SUCCESS;
}

PalResult PAL_CALL palCreateComputePipeline(
    PalDevice* device,
    const PalComputePipelineCreateInfo* info,
    PalPipeline** outPipeline)
{
    if (!s_Graphics.initialized) {
        return PAL_RESULT_CODE_NOT_INITIALIZED;
    }

    if (!device || !info || !outPipeline) {
        return PAL_RESULT_CODE_INVALID_ARGUMENT;
    }

    PalResult result;
    PalPipeline* pipeline = nullptr;
    result = device->backend->createComputePipeline(device, info, &pipeline);
    if (result != PAL_RESULT_SUCCESS) {
        return result;
    }

    pipeline->backend = device->backend;
    *outPipeline = pipeline;
    return PAL_RESULT_SUCCESS;
}

PalResult PAL_CALL palCreateRayTracingPipeline(
    PalDevice* device,
    const PalRayTracingPipelineCreateInfo* info,
    PalPipeline** outPipeline)
{
    if (!s_Graphics.initialized) {
        return PAL_RESULT_CODE_NOT_INITIALIZED;
    }

    if (!device || !info || !outPipeline) {
        return PAL_RESULT_CODE_INVALID_ARGUMENT;
    }

    PalResult result;
    PalPipeline* pipeline = nullptr;
    result = device->backend->createRayTracingPipeline(device, info, &pipeline);
    if (result != PAL_RESULT_SUCCESS) {
        return result;
    }

    pipeline->backend = device->backend;
    *outPipeline = pipeline;
    return PAL_RESULT_SUCCESS;
}

void PAL_CALL palDestroyPipeline(PalPipeline* pipeline)
{
    if (s_Graphics.initialized && pipeline) {
        pipeline->backend->destroyPipeline(pipeline);
    }
}

// ==================================================
// Shader Binding Table
// ==================================================

PalResult PAL_CALL palCreateShaderBindingTable(
    PalDevice* device,
    const PalShaderBindingTableCreateInfo* info,
    PalShaderBindingTable** outSbt)
{
    if (!s_Graphics.initialized) {
        return PAL_RESULT_CODE_NOT_INITIALIZED;
    }

    if (!device || !info || !outSbt) {
        return PAL_RESULT_CODE_INVALID_ARGUMENT;
    }

    if (!info->rayTracingPipeline) {
        return PAL_RESULT_CODE_INVALID_ARGUMENT;
    }

    PalResult result;
    PalShaderBindingTable* sbt = nullptr;
    result = device->backend->createShaderBindingTable(device, info, &sbt);
    if (result != PAL_RESULT_SUCCESS) {
        return result;
    }

    sbt->backend = device->backend;
    *outSbt = sbt;
    return PAL_RESULT_SUCCESS;
}

void PAL_CALL palDestroyShaderBindingTable(PalShaderBindingTable* sbt)
{
    if (s_Graphics.initialized && sbt) {
        sbt->backend->destroyShaderBindingTable(sbt);
    }
}

PalResult PAL_CALL palUpdateShaderBindingTable(
    PalShaderBindingTable* sbt,
    uint32_t count,
    PalShaderBindingTableRecordInfo* infos)
{
    if (!s_Graphics.initialized) {
        return PAL_RESULT_CODE_NOT_INITIALIZED;
    }

    if (!sbt || !infos || count == 0 && infos) {
        return PAL_RESULT_CODE_INVALID_ARGUMENT;
    }

    return sbt->backend->updateShaderBindingTable(sbt, count, infos);
}

// ==================================================
// Utils
// ==================================================

PalBool PAL_CALL palBuildWorkGroupInfo(
    const PalWorkGroupBuildData* data,
    int32_t* count,
    PalWorkGroupInfo* infos)
{
    if (!data) {
        return PAL_FALSE;
    }

    if (*count == 0 && infos) {
        return PAL_FALSE;
    }

    uint32_t workGroupCount[3];
    uint32_t groupInfoCount[3];
    for (int i = 0; i < 3; i++) {
        uint32_t tmp = _ceil(data->workCount[i], data->workGroupSize[i]);
        workGroupCount[i] = tmp;
        groupInfoCount[i] = _ceil(tmp, data->workGroupCount[i]);
    }

    if (!infos) {
        // total number of group build info on all axis
        *count = groupInfoCount[0] * groupInfoCount[1] * groupInfoCount[2];
        ;
        return PAL_TRUE;
    }

    for (int i = 0; i < *count; i++) {
        PalWorkGroupInfo* buildInfo = &infos[i];
        // find index
        uint32_t index[3];
        index[0] = i % groupInfoCount[0];
        index[1] = (i / groupInfoCount[0]) % groupInfoCount[1];
        index[2] = i / (groupInfoCount[0] * groupInfoCount[1]);

        // fill group build info
        for (int j = 0; j < 3; j++) {
            buildInfo->workGroupBase[j] = index[j] * data->workGroupCount[j];
            buildInfo->workGroupBase[j] = index[j] * data->workGroupCount[j];
            buildInfo->workGroupBase[j] = index[j] * data->workGroupCount[j];

            uint32_t tmp = workGroupCount[j] - buildInfo->workGroupBase[j];
            buildInfo->workGroupCount[j] = _min(data->workGroupCount[j], tmp);
        }
    }

    return PAL_TRUE;
}
