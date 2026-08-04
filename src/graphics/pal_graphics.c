
/**
    PAL - Prime Abstraction Layer
    Copyright (C) 2025
    Licensed under the Zlib license. See LICENSE file in root.
 */

#include "pal_graphics_backends.h"

#define ceil(a, b) (a + b - 1) / b
#define min(a, b) (a < b) ? a : b

#define MAX_BACKENDS (PAL_MAX_CUSTOM_BACKENDS + 2)
#define PAL_HANDLE(name)                                                                           \
    struct name {                                                                                  \
        PalGraphicsVtable backend;                                                                 \
    };

PAL_HANDLE(PalAdapter)
PAL_HANDLE(PalDevice)
PAL_HANDLE(PalQueue)
PAL_HANDLE(PalMemory)
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
    int32_t backendCount;
    const PalAllocator* allocator;
    PalGraphicsVtable backends[MAX_BACKENDS];
} Graphics;

static Graphics s_Graphics = {0};

static PalBool validateVtableVersion1(const PalGraphicsBackendVtable1* vtable1)
{
    // validate required functions
    // clang-format off
    if (!vtable1->enumerateAdapters                             ||
        !vtable1->getAdapterInfo                                ||
        !vtable1->getAdapterCapabilities                        ||
        !vtable1->getAdapterFeatures                            ||
        !vtable1->getHighestSupportedShaderTarget               ||

        // device
        !vtable1->createDevice                                  ||
        !vtable1->destroyDevice                                 ||
        !vtable1->getDeviceLostReason                           ||

        // memory
        !vtable1->allocateMemory                                ||
        !vtable1->freeMemory                                    ||

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

        // image view
        !vtable1->createImageView                               ||
        !vtable1->destroyImageView                              ||

        // sampler
        !vtable1->createSampler                                 ||
        !vtable1->destroySampler                                ||

        // shader
        !vtable1->createShader                                  ||
        !vtable1->destroyShader                                 ||

        // fence
        !vtable1->createFence                                   ||
        !vtable1->destroyFence                                  ||
        !vtable1->waitFence                                     ||
        !vtable1->isFenceSignaled                               ||

        // semaphore
        !vtable1->createSemaphore                               ||
        !vtable1->destroySemaphore                              ||

        // command pool and command buffer
        !vtable1->createCommandPool                             ||
        !vtable1->destroyCommandPool                            ||
        !vtable1->allocateCommandBuffer                         ||
        !vtable1->freeCommandBuffer                             ||
        !vtable1->resetCommandBuffer                            ||
        !vtable1->submitCommandBuffer                           ||

        // command recording
        !vtable1->cmdBegin                                      ||
        !vtable1->cmdEnd                                        ||
        !vtable1->cmdExecuteCommandBuffer                       ||
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
        !vtable1->cmdDrawIndexed                                ||
        !vtable1->cmdImageBarrier                               ||
        !vtable1->cmdBufferBarrier                              ||
        !vtable1->cmdDispatch                                   ||
        !vtable1->cmdBindDescriptorSet                          ||
        !vtable1->cmdPushConstants                              ||

        // buffer
        !vtable1->createBuffer                                  ||
        !vtable1->destroyBuffer                                 ||
        !vtable1->getBufferMemoryRequirements                   ||
        !vtable1->computeImageStagingRequirements               ||
        !vtable1->writeImageStaging                             ||
        !vtable1->bindBufferMemory                              ||
        !vtable1->mapBuffer                                     ||
        !vtable1->unmapBuffer                                   ||

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
        !vtable1->destroyPipeline) {
        return PAL_FALSE;
    }
    // clang-format on
    return PAL_TRUE;
}

static PalBool addBackend(
    const void* infoOrVtable,
    PalBool custom)
{
    PalGraphicsVtable* backend = &s_Graphics.backends[s_Graphics.backendCount++];
    memset(backend, 0, sizeof(PalGraphicsVtable));

    if (custom) {
        const PalGraphicsBackendInfo* info = infoOrVtable;
        if (info->version == PAL_GRAPHICS_BACKEND_VTABLE_VERSION_1) {
            // validate that all version 1 required pointers are set
            const PalGraphicsBackendVtable1* vtable1 = (PalGraphicsBackendVtable1*)info->vtable;
            if (!validateVtableVersion1(vtable1)) {
                return PAL_FALSE;
            }
            backend->vtbl1 = vtable1;
        }

    } else {
        backend->vtbl1 = infoOrVtable;
    }

    return PAL_TRUE;
}

PalResult PAL_CALL palInitGraphics(
    const PalGraphicsDebugger* debugger,
    const PalAllocator* allocator,
    uint32_t customBackendCount,
    const PalGraphicsBackendInfo* customBackends)
{
#if PAL_HAS_VULKAN_BACKEND
    if (initGraphicsVk(debugger, allocator)) {
        addBackend(&s_VkBackend1, PAL_FALSE);
    }
#endif // PAL_HAS_VULKAN_BACKEND

#if PAL_HAS_D3D12_BACKEND
    if (initGraphicsD3D12(debugger, allocator)) {
        addBackend(&s_D3D12Backend1, PAL_FALSE);
    }
#endif // PAL_HAS_D3D12_BACKEND

    // custom backends
    for (uint32_t i = 0; i < customBackendCount; i++) {
        if (!addBackend(&customBackends[i], PAL_TRUE)) {
            return PAL_RESULT_CODE_INVALID_ARGUMENT;
        }
    }

    s_Graphics.allocator = allocator;
    return PAL_RESULT_SUCCESS;
}

void PAL_CALL palShutdownGraphics()
{
#if PAL_HAS_VULKAN_BACKEND
    shutdownGraphicsVk();
#endif // PAL_HAS_VULKAN_BACKEND

#if PAL_HAS_D3D12_BACKEND
    shutdownGraphicsD3D12();
#endif // PAL_HAS_D3D12_BACKEND

    memset(&s_Graphics, 0, sizeof(s_Graphics));
}

// ==================================================
// Adapter
// ==================================================

PalResult PAL_CALL palEnumerateAdapters(
    uint32_t* count,
    PalAdapter** outAdapters)
{
    if (!count) {
        return PAL_RESULT_CODE_INVALID_ARGUMENT;
    }

    // enumerate all adapters for both custom and PAL backends
    PalResult result;
    uint32_t offset = 0;
    uint32_t adapterCount = 0;
    for (int i = 0; i < s_Graphics.backendCount; i++) {
        PalGraphicsVtable* backend = &s_Graphics.backends[i];
        uint32_t backendAdapterCount = 0;
        result = backend->vtbl1->enumerateAdapters(&backendAdapterCount, nullptr);
        if (result == PAL_RESULT_SUCCESS) {
            adapterCount += backendAdapterCount;
        }

        if (outAdapters) {
            // offset into the array so all backends write at the correct index
            PalAdapter** adapters = &outAdapters[offset];
            result = backend->vtbl1->enumerateAdapters(&backendAdapterCount, adapters);
            if (result == PAL_RESULT_SUCCESS) {
                for (int j = 0; j < backendAdapterCount; j++) {
                    PalAdapter* tmp = adapters[j];
                    tmp->backend.vtbl1 = backend->vtbl1;
                }

                // update offset since the backend provided adapters
                offset += backendAdapterCount;
            }
        }
    }

    if (!outAdapters) {
        *count = adapterCount;
    }
    return PAL_RESULT_SUCCESS;
}

void PAL_CALL palGetAdapterInfo(
    PalAdapter* adapter,
    PalAdapterInfo* info)
{
    adapter->backend.vtbl1->getAdapterInfo(adapter, info);
}

void PAL_CALL palGetAdapterCapabilities(
    PalAdapter* adapter,
    PalAdapterCapabilities* caps)
{
    adapter->backend.vtbl1->getAdapterCapabilities(adapter, caps);
}

PalAdapterFeatures PAL_CALL palGetAdapterFeatures(PalAdapter* adapter)
{
    return adapter->backend.vtbl1->getAdapterFeatures(adapter);
}

uint32_t PAL_CALL palGetHighestSupportedShaderTarget(
    PalAdapter* adapter,
    PalShaderFormats shaderFormat)
{
    return adapter->backend.vtbl1->getHighestSupportedShaderTarget(adapter, shaderFormat);
}

// ==================================================
// Device
// ==================================================

PalResult PAL_CALL palCreateDevice(
    PalAdapter* adapter,
    PalAdapterFeatures features,
    PalDevice** outDevice)
{
    if (!adapter || !outDevice) {
        return PAL_RESULT_CODE_INVALID_ARGUMENT;
    }

    PalDevice* device = nullptr;
    PalResult result;
    result = adapter->backend.vtbl1->createDevice(adapter, features, &device);
    if (result != PAL_RESULT_SUCCESS) {
        return result;
    }

    device->backend = adapter->backend;
    *outDevice = device;
    return PAL_RESULT_SUCCESS;
}

void PAL_CALL palDestroyDevice(PalDevice* device)
{
    device->backend.vtbl1->destroyDevice(device);
}

uint32_t PAL_CALL palGetDeviceLostReason(PalDevice* device)
{
    return (uint32_t)device->backend.vtbl1->getDeviceLostReason(device);
}

PalResult PAL_CALL palAllocateMemory(
    PalDevice* device,
    PalMemoryType type,
    uint64_t memoryMask,
    uint64_t size,
    PalMemory** outMemory)
{
    if (!device || !outMemory) {
        return PAL_RESULT_CODE_INVALID_ARGUMENT;
    }

    PalMemory* memory = nullptr;
    PalResult result;
    result = device->backend.vtbl1->allocateMemory(device, type, memoryMask, size, &memory);
    if (result != PAL_RESULT_SUCCESS) {
        return result;
    }

    memory->backend = device->backend;
    *outMemory = memory;
    return PAL_RESULT_SUCCESS;
}

void PAL_CALL palFreeMemory(PalMemory* memory)
{
    memory->backend.vtbl1->freeMemory(memory);
}

// ==================================================
// Extended Adapter Features
// ==================================================

void PAL_CALL palQuerySamplerAnisotropyCapabilities(
    PalDevice* device,
    PalSamplerAnisotropyCapabilities* caps)
{
    device->backend.vtbl1->querySamplerAnisotropyCapabilities(device, caps);
}

void PAL_CALL palQueryMultiViewCapabilities(
    PalDevice* device,
    PalMultiViewCapabilities* caps)
{
    device->backend.vtbl1->queryMultiViewCapabilities(device, caps);
}

void PAL_CALL palQueryMultiViewportCapabilities(
    PalDevice* device,
    PalMultiViewportCapabilities* caps)
{
    device->backend.vtbl1->queryMultiViewportCapabilities(device, caps);
}

void PAL_CALL palQueryDepthStencilCapabilities(
    PalDevice* device,
    PalDepthStencilCapabilities* caps)
{
    device->backend.vtbl1->queryDepthStencilCapabilities(device, caps);
}

void PAL_CALL palQueryFragmentShadingRateCapabilities(
    PalDevice* device,
    PalFragmentShadingRateCapabilities* caps)
{
    device->backend.vtbl1->queryFragmentShadingRateCapabilities(device, caps);
}

void PAL_CALL palQueryMeshShaderCapabilities(
    PalDevice* device,
    PalMeshShaderCapabilities* caps)
{
    device->backend.vtbl1->queryMeshShaderCapabilities(device, caps);
}

void PAL_CALL palQueryRayTracingCapabilities(
    PalDevice* device,
    PalRayTracingCapabilities* caps)
{
    device->backend.vtbl1->queryRayTracingCapabilities(device, caps);
}

void PAL_CALL palQueryDescriptorIndexingCapabilities(
    PalDevice* device,
    PalDescriptorIndexingCapabilities* caps)
{
    device->backend.vtbl1->queryDescriptorIndexingCapabilities(device, caps);
}

// ==================================================
// Queue
// ==================================================

PalResult PAL_CALL palCreateQueue(
    PalDevice* device,
    PalQueueType type,
    PalQueue** outQueue)
{
    if (!device || !outQueue) {
        return PAL_RESULT_CODE_INVALID_ARGUMENT;
    }

    PalQueue* queue = nullptr;
    PalResult result;
    result = device->backend.vtbl1->createQueue(device, type, &queue);
    if (result != PAL_RESULT_SUCCESS) {
        return result;
    }

    queue->backend = device->backend;
    *outQueue = queue;
    return PAL_RESULT_SUCCESS;
}

void PAL_CALL palDestroyQueue(PalQueue* queue)
{
    queue->backend.vtbl1->destroyQueue(queue);
}

PalBool PAL_CALL palCanQueuePresent(
    PalQueue* queue,
    PalSurface* surface)
{
    return queue->backend.vtbl1->canQueuePresent(queue, surface);
}

PalResult PAL_CALL palWaitQueue(PalQueue* queue)
{
    return queue->backend.vtbl1->waitQueue(queue);
}

// ==================================================
// Format And Usages
// ==================================================

void PAL_CALL palEnumerateFormats(
    PalAdapter* adapter,
    uint32_t* count,
    PalFormatInfo* outFormats)
{
    adapter->backend.vtbl1->enumerateFormats(adapter, count, outFormats);
}

PalBool PAL_CALL palIsFormatSupported(
    PalAdapter* adapter,
    PalFormat format)
{
    return adapter->backend.vtbl1->isFormatSupported(adapter, format);
}

PalImageUsages PAL_CALL palQueryFormatImageUsages(
    PalAdapter* adapter,
    PalFormat format)
{
    return adapter->backend.vtbl1->queryFormatImageUsages(adapter, format);
}

PalSampleCount PAL_CALL palQueryFormatSampleCount(
    PalAdapter* adapter,
    PalFormat format)
{
    return adapter->backend.vtbl1->queryFormatSampleCount(adapter, format);
}

// ==================================================
// Image
// ==================================================

PalResult PAL_CALL palCreateImage(
    PalDevice* device,
    const PalImageCreateInfo* info,
    PalImage** outImage)
{
    if (!device || !info || !outImage) {
        return PAL_RESULT_CODE_INVALID_ARGUMENT;
    }

    PalImage* image = nullptr;
    PalResult result;
    result = device->backend.vtbl1->createImage(device, info, &image);
    if (result != PAL_RESULT_SUCCESS) {
        return result;
    }

    image->backend = device->backend;
    *outImage = image;
    return PAL_RESULT_SUCCESS;
}

void PAL_CALL palDestroyImage(PalImage* image)
{
    image->backend.vtbl1->destroyImage(image);
}

void PAL_CALL palGetImageInfo(
    PalImage* image,
    PalImageInfo* info)
{
    image->backend.vtbl1->getImageInfo(image, info);
}

void PAL_CALL palGetImageMemoryRequirements(
    PalImage* image,
    PalMemoryRequirements* requirements)
{
    image->backend.vtbl1->getImageMemoryRequirements(image, requirements);
}

PalResult PAL_CALL palBindImageMemory(
    PalImage* image,
    PalMemory* memory,
    uint64_t offset)
{
    return image->backend.vtbl1->bindImageMemory(image, memory, offset);
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
    if (!device || !image || !info || !outImageView) {
        return PAL_RESULT_CODE_INVALID_ARGUMENT;
    }

    PalImageView* imageView = nullptr;
    PalResult result;
    result = device->backend.vtbl1->createImageView(device, image, info, &imageView);
    if (result != PAL_RESULT_SUCCESS) {
        return result;
    }

    imageView->backend = device->backend;
    *outImageView = imageView;
    return PAL_RESULT_SUCCESS;
}

void PAL_CALL palDestroyImageView(PalImageView* imageView)
{
    imageView->backend.vtbl1->destroyImageView(imageView);
}

// ==================================================
// Sampler
// ==================================================

PalResult PAL_CALL palCreateSampler(
    PalDevice* device,
    const PalSamplerCreateInfo* info,
    PalSampler** outSampler)
{
    if (!device || !info || !outSampler) {
        return PAL_RESULT_CODE_INVALID_ARGUMENT;
    }

    PalSampler* sampler = nullptr;
    PalResult result;
    result = device->backend.vtbl1->createSampler(device, info, &sampler);
    if (result != PAL_RESULT_SUCCESS) {
        return result;
    }

    sampler->backend = device->backend;
    *outSampler = sampler;
    return PAL_RESULT_SUCCESS;
}

void PAL_CALL palDestroySampler(PalSampler* sampler)
{
    sampler->backend.vtbl1->destroySampler(sampler);
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
    if (!device || !window || !windowInstance || !outSurface) {
        return PAL_RESULT_CODE_INVALID_ARGUMENT;
    }

    PalSurface* surface = nullptr;
    PalResult ret;
    ret = device->backend.vtbl1
              ->createSurface(device, window, windowInstance, instanceType, &surface);
    if (ret != PAL_RESULT_SUCCESS) {
        return ret;
    }

    surface->backend = device->backend;
    *outSurface = surface;
    return PAL_RESULT_SUCCESS;
}

void PAL_CALL palDestroySurface(PalSurface* surface)
{
    surface->backend.vtbl1->destroySurface(surface);
}

void PAL_CALL palGetSurfaceCapabilities(
    PalDevice* device,
    PalSurface* surface,
    PalSurfaceCapabilities* caps)
{
    device->backend.vtbl1->getSurfaceCapabilities(device, surface, caps);
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
    if (!device || !queue || !surface || !info || !outSwapchain) {
        return PAL_RESULT_CODE_INVALID_ARGUMENT;
    }

    PalResult result;
    PalSwapchain* swapchain = nullptr;
    result = device->backend.vtbl1->createSwapchain(device, queue, surface, info, &swapchain);
    if (result != PAL_RESULT_SUCCESS) {
        return result;
    }

    // set the backend for all swapchain images
    for (int i = 0; i < info->imageCount; i++) {
        PalImage* image = device->backend.vtbl1->getSwapchainImage(swapchain, i);
        image->backend = device->backend;
    }

    swapchain->backend = device->backend;
    *outSwapchain = swapchain;
    return PAL_RESULT_SUCCESS;
}

void PAL_CALL palDestroySwapchain(PalSwapchain* swapchain)
{
    swapchain->backend.vtbl1->destroySwapchain(swapchain);
}

PalImage* PAL_CALL palGetSwapchainImage(
    PalSwapchain* swapchain,
    uint32_t index)
{
    return swapchain->backend.vtbl1->getSwapchainImage(swapchain, index);
}

PalResult PAL_CALL palGetNextSwapchainImage(
    PalSwapchain* swapchain,
    PalSwapchainNextImageInfo* info,
    uint32_t* outIndex)
{
    return swapchain->backend.vtbl1->getNextSwapchainImage(swapchain, info, outIndex);
}

PalResult PAL_CALL palPresentSwapchain(
    PalSwapchain* swapchain,
    uint32_t imageIndex,
    PalSemaphore* waitSemaphore)
{
    return swapchain->backend.vtbl1->presentSwapchain(swapchain, imageIndex, waitSemaphore);
}

PalResult PAL_CALL palResizeSwapchain(
    PalSwapchain* swapchain,
    uint32_t newWidth,
    uint32_t newHeight)
{
    return swapchain->backend.vtbl1->resizeSwapchain(swapchain, newWidth, newHeight);
}

// ==================================================
// Shader
// ==================================================

PalResult PAL_CALL palCreateShader(
    PalDevice* device,
    const PalShaderCreateInfo* info,
    PalShader** outShader)
{
    if (!device || !info || !outShader) {
        return PAL_RESULT_CODE_INVALID_ARGUMENT;
    }

    PalShader* shader = nullptr;
    PalResult result;
    result = device->backend.vtbl1->createShader(device, info, &shader);
    if (result != PAL_RESULT_SUCCESS) {
        return result;
    }

    shader->backend = device->backend;
    *outShader = shader;
    return PAL_RESULT_SUCCESS;
}

void PAL_CALL palDestroyShader(PalShader* shader)
{
    shader->backend.vtbl1->destroyShader(shader);
}

// ==================================================
// Fence
// ==================================================

PalResult PAL_CALL palCreateFence(
    PalDevice* device,
    PalBool signaled,
    PalFence** outFence)
{
    if (!device || !outFence) {
        return PAL_RESULT_CODE_INVALID_ARGUMENT;
    }

    PalFence* fence = nullptr;
    PalResult result;
    result = device->backend.vtbl1->createFence(device, signaled, &fence);
    if (result != PAL_RESULT_SUCCESS) {
        return result;
    }

    fence->backend = device->backend;
    *outFence = fence;
    return PAL_RESULT_SUCCESS;
}

void PAL_CALL palDestroyFence(PalFence* fence)
{
    fence->backend.vtbl1->destroyFence(fence);
}

PalResult PAL_CALL palWaitFence(
    PalFence* fence,
    uint64_t timeout)
{
    return fence->backend.vtbl1->waitFence(fence, timeout);
}

PalResult PAL_CALL palResetFence(PalFence* fence)
{
    return fence->backend.vtbl1->resetFence(fence);
}

PalBool PAL_CALL palIsFenceSignaled(PalFence* fence)
{
    return fence->backend.vtbl1->isFenceSignaled(fence);
}

// ==================================================
// Semaphore
// ==================================================

PalResult PAL_CALL palCreateSemaphore(
    PalDevice* device,
    PalBool enableTimeline,
    PalSemaphore** outSemaphore)
{
    if (!device || !outSemaphore) {
        return PAL_RESULT_CODE_INVALID_ARGUMENT;
    }

    PalSemaphore* semaphore = nullptr;
    PalResult result;
    result = device->backend.vtbl1->createSemaphore(device, enableTimeline, &semaphore);
    if (result != PAL_RESULT_SUCCESS) {
        return result;
    }

    semaphore->backend = device->backend;
    *outSemaphore = semaphore;
    return PAL_RESULT_SUCCESS;
}

void PAL_CALL palDestroySemaphore(PalSemaphore* semaphore)
{
    semaphore->backend.vtbl1->destroySemaphore(semaphore);
}

PalResult PAL_CALL palWaitSemaphore(
    PalSemaphore* semaphore,
    uint64_t value,
    uint64_t timeout)
{
    return semaphore->backend.vtbl1->waitSemaphore(semaphore, value, timeout);
}

PalResult PAL_CALL palSignalSemaphore(
    PalSemaphore* semaphore,
    PalQueue* queue,
    uint64_t value)
{
    return semaphore->backend.vtbl1->signalSemaphore(semaphore, queue, value);
}

PalResult PAL_CALL palGetSemaphoreValue(
    PalSemaphore* semaphore,
    uint64_t* value)
{
    return semaphore->backend.vtbl1->getSemaphoreValue(semaphore, value);
}

// ==================================================
// Command Pool And Buffer
// ==================================================

PalResult PAL_CALL palCreateCommandPool(
    PalDevice* device,
    PalQueue* queue,
    PalCommandPool** outPool)
{
    if (!device || !queue || !outPool) {
        return PAL_RESULT_CODE_INVALID_ARGUMENT;
    }

    PalCommandPool* pool = nullptr;
    PalResult result;
    result = device->backend.vtbl1->createCommandPool(device, queue, &pool);
    if (result != PAL_RESULT_SUCCESS) {
        return result;
    }

    pool->backend = device->backend;
    *outPool = pool;
    return PAL_RESULT_SUCCESS;
}

void PAL_CALL palDestroyCommandPool(PalCommandPool* pool)
{
    pool->backend.vtbl1->destroyCommandPool(pool);
}

PalResult PAL_CALL palAllocateCommandBuffer(
    PalDevice* device,
    PalCommandPool* pool,
    PalCommandBufferType type,
    PalCommandBuffer** outCmdBuffer)
{
    if (!device || !pool || !outCmdBuffer) {
        return PAL_RESULT_CODE_INVALID_ARGUMENT;
    }

    PalCommandBuffer* cmdBuffer = nullptr;
    PalResult result;
    result = device->backend.vtbl1->allocateCommandBuffer(device, pool, type, &cmdBuffer);
    if (result != PAL_RESULT_SUCCESS) {
        return result;
    }

    cmdBuffer->backend = device->backend;
    *outCmdBuffer = cmdBuffer;
    return PAL_RESULT_SUCCESS;
}

void PAL_CALL palFreeCommandBuffer(PalCommandBuffer* cmdBuffer)
{
    cmdBuffer->backend.vtbl1->freeCommandBuffer(cmdBuffer);
}

PalResult PAL_CALL palResetCommandBuffer(PalCommandBuffer* cmdBuffer)
{
    return cmdBuffer->backend.vtbl1->resetCommandBuffer(cmdBuffer);
}

PalResult PAL_CALL palSubmitCommandBuffer(
    PalQueue* queue,
    PalCommandBufferSubmitInfo* info)
{
    return queue->backend.vtbl1->submitCommandBuffer(queue, info);
}

// ==================================================
// Command Recording
// ==================================================

PalResult PAL_CALL palCmdBegin(
    PalCommandBuffer* cmdBuffer,
    PalRenderingLayoutInfo* info)
{
    return cmdBuffer->backend.vtbl1->cmdBegin(cmdBuffer, info);
}

PalResult PAL_CALL palCmdEnd(PalCommandBuffer* cmdBuffer)
{
    return cmdBuffer->backend.vtbl1->cmdEnd(cmdBuffer);
}

void PAL_CALL palCmdExecuteCommandBuffer(
    PalCommandBuffer* primaryCmdBuffer,
    PalCommandBuffer* secondaryCmdBuffer)
{
    primaryCmdBuffer->backend.vtbl1->cmdExecuteCommandBuffer(primaryCmdBuffer, secondaryCmdBuffer);
}

void PAL_CALL palCmdSetFragmentShadingRate(
    PalCommandBuffer* cmdBuffer,
    PalFragmentShadingRateState* state)
{
    cmdBuffer->backend.vtbl1->cmdSetFragmentShadingRate(cmdBuffer, state);
}

void PAL_CALL palCmdDrawMeshTasks(
    PalCommandBuffer* cmdBuffer,
    uint32_t groupCountX,
    uint32_t groupCountY,
    uint32_t groupCountZ)
{
    cmdBuffer->backend.vtbl1->cmdDrawMeshTasks(cmdBuffer, groupCountX, groupCountY, groupCountZ);
}

void PAL_CALL palCmdDrawMeshTasksIndirect(
    PalCommandBuffer* cmdBuffer,
    PalBuffer* buffer,
    uint32_t drawCount)
{
    cmdBuffer->backend.vtbl1->cmdDrawMeshTasksIndirect(cmdBuffer, buffer, drawCount);
}

void PAL_CALL palCmdDrawMeshTasksIndirectCount(
    PalCommandBuffer* cmdBuffer,
    PalBuffer* buffer,
    PalBuffer* countBuffer,
    uint32_t maxDrawCount)
{
    cmdBuffer->backend.vtbl1
        ->cmdDrawMeshTasksIndirectCount(cmdBuffer, buffer, countBuffer, maxDrawCount);
}

void PAL_CALL palCmdBuildAccelerationStructure(
    PalCommandBuffer* cmdBuffer,
    PalAccelerationStructureBuildInfo* info)
{
    cmdBuffer->backend.vtbl1->cmdBuildAccelerationStructure(cmdBuffer, info);
}

void PAL_CALL palCmdBeginRendering(
    PalCommandBuffer* cmdBuffer,
    PalRenderingInfo* info)
{
    cmdBuffer->backend.vtbl1->cmdBeginRendering(cmdBuffer, info);
}

void PAL_CALL palCmdEndRendering(PalCommandBuffer* cmdBuffer)
{
    cmdBuffer->backend.vtbl1->cmdEndRendering(cmdBuffer);
}

void PAL_CALL palCmdCopyBuffer(
    PalCommandBuffer* cmdBuffer,
    PalBuffer* dst,
    PalBuffer* src,
    PalBufferCopyInfo* copyInfo)
{
    cmdBuffer->backend.vtbl1->cmdCopyBuffer(cmdBuffer, dst, src, copyInfo);
}

void PAL_CALL palCmdCopyBufferToImage(
    PalCommandBuffer* cmdBuffer,
    PalImage* dstImage,
    PalBuffer* srcBuffer,
    PalBufferImageCopyInfo* copyInfo)
{
    cmdBuffer->backend.vtbl1->cmdCopyBufferToImage(cmdBuffer, dstImage, srcBuffer, copyInfo);
}

void PAL_CALL palCmdCopyImage(
    PalCommandBuffer* cmdBuffer,
    PalImage* dst,
    PalImage* src,
    PalImageCopyInfo* copyInfo)
{
    cmdBuffer->backend.vtbl1->cmdCopyImage(cmdBuffer, dst, src, copyInfo);
}

void PAL_CALL palCmdCopyImageToBuffer(
    PalCommandBuffer* cmdBuffer,
    PalBuffer* dstBuffer,
    PalImage* srcImage,
    PalBufferImageCopyInfo* copyInfo)
{
    cmdBuffer->backend.vtbl1->cmdCopyImageToBuffer(cmdBuffer, dstBuffer, srcImage, copyInfo);
}

void PAL_CALL palCmdBindPipeline(
    PalCommandBuffer* cmdBuffer,
    PalPipeline* pipeline)
{
    cmdBuffer->backend.vtbl1->cmdBindPipeline(cmdBuffer, pipeline);
}

void PAL_CALL palCmdSetViewport(
    PalCommandBuffer* cmdBuffer,
    uint32_t count,
    PalViewport* viewports)
{
    cmdBuffer->backend.vtbl1->cmdSetViewport(cmdBuffer, count, viewports);
}

void PAL_CALL palCmdSetScissors(
    PalCommandBuffer* cmdBuffer,
    uint32_t count,
    PalRect2D* scissors)
{
    cmdBuffer->backend.vtbl1->cmdSetScissors(cmdBuffer, count, scissors);
}

void PAL_CALL palCmdBindVertexBuffers(
    PalCommandBuffer* cmdBuffer,
    uint32_t firstSlot,
    uint32_t count,
    PalBuffer** buffers,
    uint64_t* offsets)
{
    cmdBuffer->backend.vtbl1->cmdBindVertexBuffers(cmdBuffer, firstSlot, count, buffers, offsets);
}

void PAL_CALL palCmdBindIndexBuffer(
    PalCommandBuffer* cmdBuffer,
    PalBuffer* buffer,
    uint64_t offset,
    PalIndexType type)
{
    cmdBuffer->backend.vtbl1->cmdBindIndexBuffer(cmdBuffer, buffer, offset, type);
}

void PAL_CALL palCmdDraw(
    PalCommandBuffer* cmdBuffer,
    uint32_t vertexCount,
    uint32_t instanceCount,
    uint32_t firstVertex,
    uint32_t firstInstance)
{
    cmdBuffer->backend.vtbl1
        ->cmdDraw(cmdBuffer, vertexCount, instanceCount, firstVertex, firstInstance);
}

void PAL_CALL palCmdDrawIndirect(
    PalCommandBuffer* cmdBuffer,
    PalBuffer* buffer,
    uint32_t count)
{
    cmdBuffer->backend.vtbl1->cmdDrawIndirect(cmdBuffer, buffer, count);
}

void PAL_CALL palCmdDrawIndirectCount(
    PalCommandBuffer* cmdBuffer,
    PalBuffer* buffer,
    PalBuffer* countBuffer,
    uint32_t maxDrawCount)
{
    cmdBuffer->backend.vtbl1->cmdDrawIndirectCount(cmdBuffer, buffer, countBuffer, maxDrawCount);
}

void PAL_CALL palCmdDrawIndexed(
    PalCommandBuffer* cmdBuffer,
    uint32_t indexCount,
    uint32_t instanceCount,
    uint32_t firstIndex,
    int32_t vertexOffset,
    uint32_t firstInstance)
{
    cmdBuffer->backend.vtbl1->cmdDrawIndexed(
        cmdBuffer,
        indexCount,
        instanceCount,
        firstIndex,
        vertexOffset,
        firstInstance);
}

void PAL_CALL palCmdDrawIndexedIndirect(
    PalCommandBuffer* cmdBuffer,
    PalBuffer* buffer,
    uint32_t count)
{
    cmdBuffer->backend.vtbl1->cmdDrawIndexedIndirect(cmdBuffer, buffer, count);
}

void PAL_CALL palCmdDrawIndexedIndirectCount(
    PalCommandBuffer* cmdBuffer,
    PalBuffer* buffer,
    PalBuffer* countBuffer,
    uint32_t maxDrawCount)
{
    cmdBuffer->backend.vtbl1
        ->cmdDrawIndexedIndirectCount(cmdBuffer, buffer, countBuffer, maxDrawCount);
}

void PAL_CALL palCmdAccelerationStructureBarrier(
    PalCommandBuffer* cmdBuffer,
    PalAccelerationStructure* as,
    PalBarrierInfo* info)
{
    cmdBuffer->backend.vtbl1->cmdAccelerationStructureBarrier(cmdBuffer, as, info);
}

void PAL_CALL palCmdImageBarrier(
    PalCommandBuffer* cmdBuffer,
    PalImage* image,
    PalImageSubresourceRange* subresourceRange,
    PalBarrierInfo* info)
{
    cmdBuffer->backend.vtbl1->cmdImageBarrier(cmdBuffer, image, subresourceRange, info);
}

void PAL_CALL palCmdBufferBarrier(
    PalCommandBuffer* cmdBuffer,
    PalBuffer* buffer,
    PalBarrierInfo* info)
{
    cmdBuffer->backend.vtbl1->cmdBufferBarrier(cmdBuffer, buffer, info);
}

void PAL_CALL palCmdDispatch(
    PalCommandBuffer* cmdBuffer,
    uint32_t groupCountX,
    uint32_t groupCountY,
    uint32_t groupCountZ)
{
    cmdBuffer->backend.vtbl1->cmdDispatch(cmdBuffer, groupCountX, groupCountY, groupCountZ);
}

void PAL_CALL palCmdDispatchBase(
    PalCommandBuffer* cmdBuffer,
    uint32_t baseGroupX,
    uint32_t baseGroupY,
    uint32_t baseGroupZ,
    uint32_t groupCountX,
    uint32_t groupCountY,
    uint32_t groupCountZ)
{
    cmdBuffer->backend.vtbl1->cmdDispatchBase(
        cmdBuffer,
        baseGroupX,
        baseGroupY,
        baseGroupZ,
        groupCountX,
        groupCountY,
        groupCountZ);
}

void PAL_CALL palCmdDispatchIndirect(
    PalCommandBuffer* cmdBuffer,
    PalBuffer* buffer)
{
    cmdBuffer->backend.vtbl1->cmdDispatchIndirect(cmdBuffer, buffer);
}

void PAL_CALL palCmdTraceRays(
    PalCommandBuffer* cmdBuffer,
    PalShaderBindingTable* sbt,
    uint32_t raygenIndex,
    uint32_t width,
    uint32_t height,
    uint32_t depth)
{
    cmdBuffer->backend.vtbl1->cmdTraceRays(cmdBuffer, sbt, raygenIndex, width, height, depth);
}

void PAL_CALL palCmdTraceRaysIndirect(
    PalCommandBuffer* cmdBuffer,
    uint32_t raygenIndex,
    PalShaderBindingTable* sbt,
    PalBuffer* buffer)
{
    cmdBuffer->backend.vtbl1->cmdTraceRaysIndirect(cmdBuffer, raygenIndex, sbt, buffer);
}

void PAL_CALL palCmdBindDescriptorSet(
    PalCommandBuffer* cmdBuffer,
    uint32_t setIndex,
    PalDescriptorSet* set)
{
    cmdBuffer->backend.vtbl1->cmdBindDescriptorSet(cmdBuffer, setIndex, set);
}

void PAL_CALL palCmdPushConstants(
    PalCommandBuffer* cmdBuffer,
    uint32_t offset,
    uint32_t size,
    const void* value)
{
    cmdBuffer->backend.vtbl1->cmdPushConstants(cmdBuffer, offset, size, value);
}

void PAL_CALL palCmdSetCullMode(
    PalCommandBuffer* cmdBuffer,
    PalCullMode cullMode)
{
    cmdBuffer->backend.vtbl1->cmdSetCullMode(cmdBuffer, cullMode);
}

void PAL_CALL palCmdSetFrontFace(
    PalCommandBuffer* cmdBuffer,
    PalFrontFace frontFace)
{
    cmdBuffer->backend.vtbl1->cmdSetFrontFace(cmdBuffer, frontFace);
}

void PAL_CALL palCmdSetPrimitiveTopology(
    PalCommandBuffer* cmdBuffer,
    PalPrimitiveTopology topology)
{
    cmdBuffer->backend.vtbl1->cmdSetPrimitiveTopology(cmdBuffer, topology);
}

void PAL_CALL palCmdSetDepthTestEnable(
    PalCommandBuffer* cmdBuffer,
    PalBool enable)
{
    cmdBuffer->backend.vtbl1->cmdSetDepthTestEnable(cmdBuffer, enable);
}

void PAL_CALL palCmdSetDepthWriteEnable(
    PalCommandBuffer* cmdBuffer,
    PalBool enable)
{
    cmdBuffer->backend.vtbl1->cmdSetDepthWriteEnable(cmdBuffer, enable);
}

void PAL_CALL palCmdSetStencilOp(
    PalCommandBuffer* cmdBuffer,
    PalStencilFaceFlags faceMask,
    PalStencilOp failOp,
    PalStencilOp passOp,
    PalStencilOp depthFailOp,
    PalCompareOp compareOp)
{
    cmdBuffer->backend.vtbl1
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
    if (!device || !info || !outAs) {
        return PAL_RESULT_CODE_INVALID_ARGUMENT;
    }

    PalResult result;
    PalAccelerationStructure* as = nullptr;
    result = device->backend.vtbl1->createAccelerationstructure(device, info, &as);
    if (result != PAL_RESULT_SUCCESS) {
        return result;
    }

    as->backend = device->backend;
    *outAs = as;
    return PAL_RESULT_SUCCESS;
}

void PAL_CALL palDestroyAccelerationStructure(PalAccelerationStructure* as)
{
    as->backend.vtbl1->destroyAccelerationstructure(as);
}

void PAL_CALL palGetAccelerationStructureBuildSize(
    PalDevice* device,
    PalAccelerationStructureBuildInfo* info,
    PalAccelerationStructureBuildSize* size)
{
    device->backend.vtbl1->getAccelerationStructureBuildSize(device, info, size);
}

// ==================================================
// Buffer
// ==================================================

PalResult PAL_CALL palCreateBuffer(
    PalDevice* device,
    const PalBufferCreateInfo* info,
    PalBuffer** outBuffer)
{
    if (!device || !info || !outBuffer) {
        return PAL_RESULT_CODE_INVALID_ARGUMENT;
    }

    PalResult result;
    PalBuffer* buffer = nullptr;
    result = device->backend.vtbl1->createBuffer(device, info, &buffer);
    if (result != PAL_RESULT_SUCCESS) {
        return result;
    }

    buffer->backend = device->backend;
    *outBuffer = buffer;
    return PAL_RESULT_SUCCESS;
}

void PAL_CALL palDestroyBuffer(PalBuffer* buffer)
{
    buffer->backend.vtbl1->destroyBuffer(buffer);
}

void PAL_CALL palGetBufferMemoryRequirements(
    PalBuffer* buffer,
    PalMemoryRequirements* requirements)
{
    buffer->backend.vtbl1->getBufferMemoryRequirements(buffer, requirements);
}

void PAL_CALL palComputeInstanceStagingSize(
    PalDevice* device,
    uint32_t instanceCount,
    uint64_t* outSize)
{
    device->backend.vtbl1->computeInstanceStagingSize(device, instanceCount, outSize);
}

void PAL_CALL palComputeImageStagingRequirements(
    PalDevice* device,
    PalFormat imageFormat,
    const PalBufferImageCopyInfo* copyInfo,
    PalImageStagingRequirements* requirements)
{
    device->backend.vtbl1
        ->computeImageStagingRequirements(device, imageFormat, copyInfo, requirements);
}

void PAL_CALL palWriteInstanceStaging(
    PalDevice* device,
    uint32_t instanceCount,
    PalAccelerationStructureInstance* instances,
    void* ptr)
{
    device->backend.vtbl1->writeInstanceStaging(device, instanceCount, instances, ptr);
}

void PAL_CALL palWriteImageStaging(
    PalDevice* device,
    PalFormat imageFormat,
    PalBufferImageCopyInfo* copyInfo,
    void* srcData,
    void* ptr)
{
    device->backend.vtbl1->writeImageStaging(device, imageFormat, copyInfo, srcData, ptr);
}

PalResult PAL_CALL palBindBufferMemory(
    PalBuffer* buffer,
    PalMemory* memory,
    uint64_t offset)
{
    return buffer->backend.vtbl1->bindBufferMemory(buffer, memory, offset);
}

PalResult PAL_CALL palMapBuffer(
    PalBuffer* buffer,
    uint64_t offset,
    uint64_t size,
    void** outPtr)
{
    return buffer->backend.vtbl1->mapBuffer(buffer, offset, size, outPtr);
}

void PAL_CALL palUnmapBuffer(PalBuffer* buffer)
{
    buffer->backend.vtbl1->unmapBuffer(buffer);
}

PalDeviceAddress PAL_CALL palGetBufferDeviceAddress(PalBuffer* buffer)
{
    return buffer->backend.vtbl1->getBufferDeviceAddress(buffer);
}

// ==================================================
// Descriptor Pool, Set and Layout
// ==================================================

PalResult PAL_CALL palCreateDescriptorSetLayout(
    PalDevice* device,
    const PalDescriptorSetLayoutCreateInfo* info,
    PalDescriptorSetLayout** outLayout)
{
    if (!device || !info || !outLayout) {
        return PAL_RESULT_CODE_INVALID_ARGUMENT;
    }

    PalResult result;
    PalDescriptorSetLayout* layout = nullptr;
    result = device->backend.vtbl1->createDescriptorSetLayout(device, info, &layout);
    if (result != PAL_RESULT_SUCCESS) {
        return result;
    }

    layout->backend = device->backend;
    *outLayout = layout;
    return PAL_RESULT_SUCCESS;
}

void PAL_CALL palDestroyDescriptorSetLayout(PalDescriptorSetLayout* layout)
{
    layout->backend.vtbl1->destroyDescriptorSetLayout(layout);
}

PalResult PAL_CALL palCreateDescriptorPool(
    PalDevice* device,
    const PalDescriptorPoolCreateInfo* info,
    PalDescriptorPool** outPool)
{
    if (!device || !info || !outPool) {
        return PAL_RESULT_CODE_INVALID_ARGUMENT;
    }

    PalResult result;
    PalDescriptorPool* pool = nullptr;
    result = device->backend.vtbl1->createDescriptorPool(device, info, &pool);
    if (result != PAL_RESULT_SUCCESS) {
        return result;
    }

    pool->backend = device->backend;
    *outPool = pool;
    return PAL_RESULT_SUCCESS;
}

void PAL_CALL palDestroyDescriptorPool(PalDescriptorPool* pool)
{
    pool->backend.vtbl1->destroyDescriptorPool(pool);
}

PalResult PAL_CALL palResetDescriptorPool(PalDescriptorPool* pool)
{
    return pool->backend.vtbl1->resetDescriptorPool(pool);
}

PalResult PAL_CALL palAllocateDescriptorSet(
    PalDevice* device,
    PalDescriptorPool* pool,
    PalDescriptorSetLayout* layout,
    PalDescriptorSet** outSet)
{
    if (!device || !pool || !layout || !outSet) {
        return PAL_RESULT_CODE_INVALID_ARGUMENT;
    }

    PalResult result;
    PalDescriptorSet* set = nullptr;
    result = device->backend.vtbl1->allocateDescriptorSet(device, pool, layout, &set);
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
    return device->backend.vtbl1->updateDescriptorSet(device, count, infos);
}

// ==================================================
// Pipeline Layout
// ==================================================

PalResult PAL_CALL palCreatePipelineLayout(
    PalDevice* device,
    const PalPipelineLayoutCreateInfo* info,
    PalPipelineLayout** outLayout)
{
    if (!device || !info || !outLayout) {
        return PAL_RESULT_CODE_INVALID_ARGUMENT;
    }

    PalResult result;
    PalPipelineLayout* layout = nullptr;
    result = device->backend.vtbl1->createPipelineLayout(device, info, &layout);
    if (result != PAL_RESULT_SUCCESS) {
        return result;
    }

    layout->backend = device->backend;
    *outLayout = layout;
    return PAL_RESULT_SUCCESS;
}

void PAL_CALL palDestroyPipelineLayout(PalPipelineLayout* layout)
{
    layout->backend.vtbl1->destroyPipelineLayout(layout);
}

// ==================================================
// Pipeline
// ==================================================

PalResult PAL_CALL palCreateGraphicsPipeline(
    PalDevice* device,
    const PalGraphicsPipelineCreateInfo* info,
    PalPipeline** outPipeline)
{
    if (!device || !info || !outPipeline) {
        return PAL_RESULT_CODE_INVALID_ARGUMENT;
    }

    PalResult result;
    PalPipeline* pipeline = nullptr;
    result = device->backend.vtbl1->createGraphicsPipeline(device, info, &pipeline);
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
    if (!device || !info || !outPipeline) {
        return PAL_RESULT_CODE_INVALID_ARGUMENT;
    }

    PalResult result;
    PalPipeline* pipeline = nullptr;
    result = device->backend.vtbl1->createComputePipeline(device, info, &pipeline);
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
    if (!device || !info || !outPipeline) {
        return PAL_RESULT_CODE_INVALID_ARGUMENT;
    }

    PalResult result;
    PalPipeline* pipeline = nullptr;
    result = device->backend.vtbl1->createRayTracingPipeline(device, info, &pipeline);
    if (result != PAL_RESULT_SUCCESS) {
        return result;
    }

    pipeline->backend = device->backend;
    *outPipeline = pipeline;
    return PAL_RESULT_SUCCESS;
}

void PAL_CALL palDestroyPipeline(PalPipeline* pipeline)
{
    pipeline->backend.vtbl1->destroyPipeline(pipeline);
}

// ==================================================
// Shader Binding Table
// ==================================================

PalResult PAL_CALL palCreateShaderBindingTable(
    PalDevice* device,
    const PalShaderBindingTableCreateInfo* info,
    PalShaderBindingTable** outSbt)
{
    if (!device || !info || !outSbt) {
        return PAL_RESULT_CODE_INVALID_ARGUMENT;
    }

    PalResult result;
    PalShaderBindingTable* sbt = nullptr;
    result = device->backend.vtbl1->createShaderBindingTable(device, info, &sbt);
    if (result != PAL_RESULT_SUCCESS) {
        return result;
    }

    sbt->backend = device->backend;
    *outSbt = sbt;
    return PAL_RESULT_SUCCESS;
}

void PAL_CALL palDestroyShaderBindingTable(PalShaderBindingTable* sbt)
{
    sbt->backend.vtbl1->destroyShaderBindingTable(sbt);
}

void PAL_CALL palUpdateShaderBindingTable(
    PalShaderBindingTable* sbt,
    uint32_t count,
    PalShaderBindingTableRecordInfo* infos)
{
    sbt->backend.vtbl1->updateShaderBindingTable(sbt, count, infos);
}

// ==================================================
// Utils
// ==================================================

void PAL_CALL palBuildWorkGroupInfo(
    const PalWorkGroupBuildData* data,
    uint32_t* count,
    PalWorkGroupInfo* infos)
{
    uint32_t workGroupCount[3];
    uint32_t groupInfoCount[3];
    for (int i = 0; i < 3; i++) {
        uint32_t tmp = ceil(data->workCount[i], data->workGroupSize[i]);
        workGroupCount[i] = tmp;
        groupInfoCount[i] = ceil(tmp, data->workGroupCount[i]);
    }

    if (!infos) {
        // total number of group build info on all axis
        *count = groupInfoCount[0] * groupInfoCount[1] * groupInfoCount[2];
        return;
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
            buildInfo->workGroupCount[j] = min(data->workGroupCount[j], tmp);
        }
    }
}
