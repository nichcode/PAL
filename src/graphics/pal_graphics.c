
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

// ==================================================
// Includes
// ==================================================

#include "pal/pal_graphics.h"

// ==================================================
// Typedefs, enums and structs
// ==================================================

#define MAX_BACKENDS 32
#define BINARY_SEMAPHORE 4
#define TIMELINE_SEMAPHORE 5
#define GRAPHICS_PIPELINE 6
#define COMPUTE_PIPELINE 7
#define SWAPCHAIN_IMAGE 12

typedef enum {
    HANDLE_TYPE_NONE,
    HANDLE_TYPE_ADAPTER,
    HANDLE_TYPE_DEVICE,
    HANDLE_TYPE_IMAGE,
    HANDLE_TYPE_IMAGE_VIEW,
    HANDLE_TYPE_SWAPCHAIN,
    HANDLE_TYPE_RENDER_PASS,
    HANDLE_TYPE_COMMAND_POOL,
    HANDLE_TYPE_COMMAND_BUFFER,
    HANDLE_TYPE_QUEUE,
    HANDLE_TYPE_FENCE,
    HANDLE_TYPE_SEMAPHORE,
    HANDLE_TYPE_PIPELINE,
    HANDLE_TYPE_SHADER
} HandleType;

typedef struct {
    bool used;
    bool shouldFree;
    HandleType type;
    Uint32 data2;
    PalAdapterFeatures features;
    void* handle;
    void* data;
    const PalGraphicsBackend* backend;
} HandleData;

typedef struct {
    Int32 count;
    Int32 startIndex;
    const PalGraphicsBackend* base;
} BackendData;

typedef struct {
    bool initialized;
    Int32 backendCount;
    Int32 maxHandleData;
    const PalAllocator* allocator;
    HandleData* handleData;
    BackendData backends[MAX_BACKENDS];
} GraphicsLinux;

static GraphicsLinux s_Graphics = {0};

// ==================================================
// Internal API
// ==================================================

static HandleData* getFreeHandleData()
{
    for (int i = 0; i < s_Graphics.maxHandleData; ++i) {
        if (!s_Graphics.handleData[i].used) {
            s_Graphics.handleData[i].used = true;
            s_Graphics.handleData[i].shouldFree = false;
            s_Graphics.handleData[i].type = HANDLE_TYPE_NONE;
            return &s_Graphics.handleData[i];
        }
    }  

    // It will be rare to have more than 128 handles at the same time
    HandleData* data = nullptr;
    data = palAllocate(s_Graphics.allocator, sizeof(HandleData), 0);
    if (!data) {
        return nullptr;
    }

    data->used = true;
    data->shouldFree = true;
    data->type = HANDLE_TYPE_NONE;
    return data;
}

static void freeHandleData(HandleData* data)
{
    if (data->shouldFree) {
        palFree(s_Graphics.allocator, data);
    } else {
        data->used = false;
    }
    data->type = HANDLE_TYPE_NONE;
}

// ==================================================
// Vulkan API
// ==================================================

#if PAL_HAS_VULKAN

PalResult PAL_CALL initGraphicsVk(
    bool enableDebugLayer,
    const PalAllocator* allocator);

PalResult PAL_CALL shutdownGraphicsVk();

PalResult PAL_CALL enumerateVkAdapters(
    Int32* count,
    PalAdapter** outAdapters);

PalResult PAL_CALL getVkAdapterInfo(
    PalAdapter* adapter,
    PalAdapterInfo* info);

PalResult PAL_CALL getVkAdapterCapabilities(
    PalAdapter* adapter,
    PalAdapterCapabilities* caps);

PalAdapterFeatures PAL_CALL getVkAdapterFeatures(PalAdapter* adapter);

PalResult PAL_CALL createVkDevice(
    PalAdapter* adapter,
    PalAdapterFeatures features,
    PalDevice** outDevice);

void PAL_CALL destroyVkDevice(PalDevice* device);

PalResult PAL_CALL allocateVkMemory(
    PalDevice* device,
    PalMemoryType type,
    Uint64 size,
    PalMemory** outMemory);

void PAL_CALL freeVkMemory(
    PalDevice* device,
    PalMemory* memory);

PalResult PAL_CALL queryVkDepthStencilCapabilities(
    PalDevice* device,
    PalDepthStencilCapabilities* caps);

PalResult PAL_CALL queryVkFragmentShadingRateCapabilities(
    PalDevice* device,
    PalFragmentShadingRateCapabilities* caps);

PalResult PAL_CALL createVkQueue(
    PalDevice* device,
    PalQueueType type,
    PalQueue** outQueue);

void PAL_CALL destroyVkQueue(PalQueue* queue);

bool PAL_CALL canVkQueuePresent(
    PalQueue* queue, 
    PalGraphicsWindow* window);

PalResult PAL_CALL enumerateVkFormats(
    PalAdapter* adapter,
    Int32* count,
    PalFormatInfo* outFormats);

bool PAL_CALL isVkFormatSupported(
    PalAdapter* adapter,
    PalFormat format);

PalImageUsages PAL_CALL queryVkFormatImageUsages(
    PalAdapter* adapter,
    PalFormat format);

PalImageViewUsages PAL_CALL queryVkFormatImageViewUsages(
    PalAdapter* adapter,
    PalFormat format);

PalResult PAL_CALL createVkImage(
    PalDevice* device,
    const PalImageCreateInfo* info,
    PalImage** outImage);

void PAL_CALL destroyVkImage(PalImage* image);

PalResult PAL_CALL getVkImageInfo(
    PalImage* image,
    PalImageInfo* info);

PalResult PAL_CALL getVkImageMemoryRequirements(
    PalImage* image,
    PalMemoryRequirements* requirements);

PalResult PAL_CALL bindVkImageMemory(
    PalImage* image,
    PalMemory* memory,
    Uint64 offset);

PalResult PAL_CALL createVkImageView(
    PalDevice* device,
    PalImage* image,
    const PalImageViewCreateInfo* info,
    PalImageView** outImageView);

void PAL_CALL destroyVkImageView(PalImageView* imageView);

PalResult PAL_CALL queryVkSwapchainCapabilities(
    PalDevice* device,
    PalGraphicsWindow* window,
    PalSwapchainCapabilities* caps);

PalResult PAL_CALL createVkSwapchain(
    PalDevice* device,
    PalQueue* queue,
    PalGraphicsWindow* window,
    const PalSwapchainCreateInfo* info,
    PalSwapchain** outSwapchain);

void PAL_CALL destroyVkSwapchain(PalSwapchain* swapchain);

PalImage* PAL_CALL getVkSwapchainImage(
    PalSwapchain* swapchain,
    Int32 index);

PalImage* PAL_CALL getVkNextSwapchainImage(
    PalSwapchain* swapchain,
    PalNextImageInfo* info);

PalResult PAL_CALL presentVkSwapchain(
    PalSwapchain* swapchain, 
    PalPresentInfo* info);

PalResult PAL_CALL createVkShader(
    PalDevice* device,
    const PalShaderCreateInfo* info,
    PalShader** outShader);

void PAL_CALL destroyVkShader(PalShader* shader);

PalResult PAL_CALL createVkRenderPass(
    PalDevice* device,
    const PalRenderPassCreateInfo* info,
    PalRenderPass** outRenderPass);

void PAL_CALL destroyVkRenderPass(PalRenderPass* renderPass);

PalResult PAL_CALL createVkFence(
    PalDevice* device,
    PalFence** outFence);

void PAL_CALL destroyVkFence(PalFence* fence);

PalResult PAL_CALL waitVkFence(
    PalFence* fence, 
    Uint64 timeout);

PalResult PAL_CALL resetVkFence(PalFence* fence);

bool PAL_CALL isVkFenceSignaled(PalFence* fence);

PalResult PAL_CALL createVkSemaphore(
    PalDevice* device,
    PalSemaphore** outSemaphore);

void PAL_CALL destroyVkSemaphore(PalSemaphore* semaphore);

PalResult PAL_CALL waitVkSemaphore(
    PalSemaphore* semaphore,
    PalQueue* queue,
    Uint64 value,
    Uint64 timeout);

PalResult PAL_CALL signalVkSemaphore(
    PalSemaphore* semaphore, 
    PalQueue* queue,
    Uint64 value);

PalResult PAL_CALL getVkSemaphoreValue(
    PalSemaphore* semaphore, 
    Uint64* value);

PalResult PAL_CALL createVkCommandPool(
    PalDevice* device,
    const PalCommandPoolCreateInfo* info,
    PalCommandPool** outPool);

void PAL_CALL destroyVkCommandPool(PalCommandPool* pool);

PalResult PAL_CALL createVkCommandBuffer(
    PalDevice* device,
    PalCommandPool* pool,
    bool primary,
    PalCommandBuffer** outBuffer);

void PAL_CALL destroyVkCommandBuffer(PalCommandBuffer* buffer);

PalResult PAL_CALL executeCommandBufferVk(
    PalCommandBuffer* primaryCmdBuffer,
    PalCommandBuffer* secondaryCmdBuffer);

PalResult PAL_CALL beginRenderPassVk(
    PalCommandBuffer* cmdBuffer,
    PalRenderPass* renderPass,
    Int32 clearValueCount,
    PalClearValue* clearValues);

PalResult PAL_CALL endRenderPassVk(PalCommandBuffer* cmdBuffer);

PalResult PAL_CALL submitVkCommandBuffer(
    PalQueue* queue,
    PalSubmitInfo* info);

PalResult PAL_CALL createVkGraphicsPipeline(
    PalDevice* device,
    const PalGraphicsPipelineCreateInfo* info,
    PalPipeline** outPipeline);

void PAL_CALL destroyVkPipeline(PalPipeline* pipeline);

static PalGraphicsBackend s_VkBackend = {
    .enumerateAdapters = enumerateVkAdapters,
    .getAdapterInfo =  getVkAdapterInfo,
    .getAdapterCapabilities =  getVkAdapterCapabilities,
    .getAdapterFeatures = getVkAdapterFeatures,
    .createDevice =  createVkDevice,
    .destroyDevice =  destroyVkDevice,
    .allocateMemory =  allocateVkMemory,
    .freeMemory =  freeVkMemory,
    .queryDepthStencilCapabilities = queryVkDepthStencilCapabilities,
    .queryFragmentShadingRateCapabilities = queryVkFragmentShadingRateCapabilities,
    .createQueue =  createVkQueue,
    .destroyQueue =  destroyVkQueue,
    .canQueuePresent =  canVkQueuePresent,
    .enumerateFormats =  enumerateVkFormats,
    .isFormatSupported =  isVkFormatSupported,
    .queryFormatImageUsages =  queryVkFormatImageUsages,
    .queryFormatImageViewUsages =  queryVkFormatImageViewUsages,
    .createImage =  createVkImage,
    .destroyImage =  destroyVkImage,
    .getImageInfo =  getVkImageInfo,
    .getImageMemoryRequirements =  getVkImageMemoryRequirements,
    .bindImageMemory =  bindVkImageMemory,
    .createImageView =  createVkImageView,
    .destroyImageView =  destroyVkImageView,
    .querySwapchainCapabilities =  queryVkSwapchainCapabilities,
    .createSwapchain =  createVkSwapchain,
    .destroySwapchain =  destroyVkSwapchain,
    .getSwapchainImage =  getVkSwapchainImage,
    .getNextSwapchainImage =  getVkNextSwapchainImage,
    .presentSwapchain =  presentVkSwapchain,
    .createShader = createVkShader,
    .destroyShader = destroyVkShader,
    .createRenderPass = createVkRenderPass,
    .destroyRenderPass = destroyVkRenderPass,
    .createFence = createVkFence,
    .destroyFence = destroyVkFence,
    .waitFenceTimeout = waitVkFence,
    .resetFence = resetVkFence,
    .isFenceSignaled = isVkFenceSignaled,
    .createSemaphore = createVkSemaphore,
    .destroySemaphore = destroyVkSemaphore,
    .waitSemaphore = waitVkSemaphore,
    .signalSemaphore = signalVkSemaphore,
    .getSemaphoreValue = getVkSemaphoreValue,
    .createCommandPool = createVkCommandPool,
    .destroyCommandPool = destroyVkCommandPool,
    .createCommandBuffer = createVkCommandBuffer,
    .destroyCommandBuffer = destroyVkCommandBuffer,
    .executeCommandBuffer = executeCommandBufferVk,
    .beginRenderPass = beginRenderPassVk,
    .endRenderPass = endRenderPassVk,
    .submitCommandBuffer = submitVkCommandBuffer,
    .createGraphicsPipeline = createVkGraphicsPipeline,
    .destroyPipeline = destroyVkPipeline
};

#endif // PAL_HAS_VULKAN

// ==================================================
// D3D12 API
// ==================================================

// ==================================================
// Metal API
// ==================================================

// ==================================================
// Public API
// ==================================================

PalResult PAL_CALL palAddGraphicsBackend(const PalGraphicsBackend* backend)
{
    if (s_Graphics.initialized) {
        return PAL_RESULT_INVALID_BACKEND;
    }

#ifdef _WIN32
    // we reserve two slots for vulkan and d3d12
    if (s_Graphics.backendCount == MAX_BACKENDS - 2) {
        return PAL_RESULT_INVALID_BACKEND;
    }
#else
    // we reserve one slot for vulkan or metal depending on platform
    if (s_Graphics.backendCount == MAX_BACKENDS - 1) {
        return PAL_RESULT_INVALID_BACKEND;
    }
#endif // _WIN32

    // check if all the function pointers are set
    // clang-format off
    if (!backend->enumerateAdapters             || 
        !backend->getAdapterInfo                ||
        !backend->getAdapterCapabilities        ||
        !backend->queryDepthStencilCapabilities ||
        !backend->queryFragmentShadingRateCapabilities ||
        !backend->getAdapterFeatures            ||
        !backend->createDevice                  ||
        !backend->destroyDevice                 ||
        !backend->allocateMemory                ||
        !backend->freeMemory                    ||
        !backend->createQueue                   ||
        !backend->destroyQueue                  ||
        !backend->canQueuePresent               ||
        !backend->enumerateFormats              ||
        !backend->isFormatSupported             ||
        !backend->queryFormatImageUsages        ||
        !backend->queryFormatImageViewUsages    ||
        !backend->createImage                   ||
        !backend->destroyImage                  ||
        !backend->getImageInfo                  ||
        !backend->getImageMemoryRequirements    ||
        !backend->bindImageMemory               ||
        !backend->createImageView               ||
        !backend->destroyImageView              ||
        !backend->querySwapchainCapabilities    ||
        !backend->createSwapchain               ||
        !backend->destroySwapchain              ||
        !backend->getSwapchainImage             ||
        !backend->getNextSwapchainImage         ||
        !backend->presentSwapchain              ||
        !backend->createShader                  ||
        !backend->destroyShader                 ||
        !backend->createRenderPass              ||
        !backend->destroyRenderPass             ||
        !backend->createFence                   ||
        !backend->destroyFence                  ||
        !backend->waitFenceTimeout              ||
        !backend->resetFence                    ||
        !backend->isFenceSignaled               ||
        !backend->createSemaphore               ||
        !backend->destroySemaphore              ||
        !backend->waitSemaphore                 ||
        !backend->signalSemaphore               ||
        !backend->getSemaphoreValue             ||
        !backend->createCommandPool             ||
        !backend->destroyCommandPool            ||
        !backend->createCommandBuffer           ||
        !backend->destroyCommandBuffer          ||
        !backend->beginRenderPass               ||
        !backend->endRenderPass                 ||
        !backend->createGraphicsPipeline        ||
        !backend->destroyPipeline               ||
        !backend->submitCommandBuffer           ||
        !backend->submitCommandBuffer) {
        return PAL_RESULT_INVALID_BACKEND;
    }
    // clang-format on

    BackendData* attached = &s_Graphics.backends[s_Graphics.backendCount++];
    attached->base = backend;
    attached->startIndex = 0;
    attached->count = 0;

    return PAL_RESULT_SUCCESS;
}

PalResult PAL_CALL palInitGraphics(
    bool enableDebugLayer,
    const PalAllocator* allocator)
{
    if (s_Graphics.initialized) {
        return PAL_RESULT_SUCCESS;
    }

    if (allocator && (!allocator->allocate || !allocator->free)) {
        return PAL_RESULT_INVALID_ALLOCATOR;
    }

    s_Graphics.maxHandleData = 128;
    s_Graphics.handleData = palAllocate(
        s_Graphics.allocator, 
        sizeof(HandleData) * s_Graphics.maxHandleData, 
        0);

    if (!s_Graphics.handleData) {
        return PAL_RESULT_OUT_OF_MEMORY;
    }

#ifdef _WIN32
    // vulkan and d3d12
#elif defined(__linux__)
    // vulkan
#if PAL_HAS_VULKAN
    initGraphicsVk(enableDebugLayer, allocator);
    BackendData* attached = &s_Graphics.backends[s_Graphics.backendCount++];
    attached->base = &s_VkBackend;
    attached->startIndex = 0;
    attached->count = 0;
#endif // PAL_HAS_VULKAN
#else
    // metal or andriod
#endif // _WIN32

    s_Graphics.allocator = allocator;
    s_Graphics.initialized = true;
    return PAL_RESULT_SUCCESS;
}

void PAL_CALL palShutdownGraphics()
{
    if (!s_Graphics.initialized) {
        return;
    }

#ifdef _WIN32
// vulkan and d3d12
#elif defined(__linux__)
    // vulkan
    shutdownGraphicsVk();
#else
    // metal or andriod
#endif // _WIN32

    palFree(s_Graphics.allocator, s_Graphics.handleData);
    memset(&s_Graphics, 0, sizeof(s_Graphics));
    s_Graphics.initialized = false;
}

// ==================================================
// Adapter
// ==================================================

PalResult PAL_CALL palEnumerateAdapters(
    Int32* count,
    PalAdapter** outAdapters)
{
    // enumerate all adapters for both custom and PAL backends
    if (!s_Graphics.initialized) {
        return PAL_RESULT_GRAPHICS_NOT_INITIALIZED;
    }

    if (!count) {
        return PAL_RESULT_NULL_POINTER;
    }

    if (*count == 0 && outAdapters) {
        return PAL_RESULT_INSUFFICIENT_BUFFER;
    }

    PalResult result;
    int totalCount = 0;
    int index = 0;
    int _count = outAdapters ? *count : 0;

    for (int i = 0; i < s_Graphics.backendCount; i++) {
        BackendData* backend = &s_Graphics.backends[i];
        if (outAdapters) {
            // offset into the array so all backends write at the correct index
            PalAdapter** adapters = &outAdapters[backend->startIndex];
            result = backend->base->enumerateAdapters(&_count, adapters);

            for (int i = 0; i < backend->count; i++) {
                HandleData* data = getFreeHandleData();
                data->backend = backend->base;
                data->handle = adapters[i];
                data->type = HANDLE_TYPE_ADAPTER;

                // set the adapter handle into our index generated handle
                adapters[i] = (PalAdapter*)data;
            }

        } else {
            result = backend->base->enumerateAdapters(&_count, nullptr);
            backend->startIndex = totalCount;
            backend->count = _count;
            totalCount += _count;
            _count = 0;
        }
        
        // break if a backend fails
        if (result != PAL_RESULT_SUCCESS) {
            return result;
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
        return PAL_RESULT_GRAPHICS_NOT_INITIALIZED;
    }

    if (!adapter || !info) {
        return PAL_RESULT_NULL_POINTER;
    }

    HandleData* data = (HandleData*)adapter;
    if (data->type != HANDLE_TYPE_ADAPTER) {
        return PAL_RESULT_INVALID_ADAPTER;
    }

    return data->backend->getAdapterInfo(data->handle, info);
}

PalResult PAL_CALL palGetAdapterCapabilities(
    PalAdapter* adapter,
    PalAdapterCapabilities* caps)
{
    if (!s_Graphics.initialized) {
        return PAL_RESULT_GRAPHICS_NOT_INITIALIZED;
    }

    if (!adapter || !caps) {
        return PAL_RESULT_NULL_POINTER;
    }

    HandleData* data = (HandleData*)adapter;
    if (data->type != HANDLE_TYPE_ADAPTER) {
        return PAL_RESULT_INVALID_ADAPTER;
    }

    return data->backend->getAdapterCapabilities(data->handle, caps);
}

PalAdapterFeatures PAL_CALL palGetAdapterFeatures(PalAdapter* adapter)
{
    if (!s_Graphics.initialized) {
        return 0;
    }

    if (!adapter) {
        return 0;
    }

    HandleData* data = (HandleData*)adapter;
    if (data->type != HANDLE_TYPE_ADAPTER) {
        return 0;
    }

    return data->backend->getAdapterFeatures(data->handle);
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
        return PAL_RESULT_GRAPHICS_NOT_INITIALIZED;
    }

    if (!outDevice) {
        return PAL_RESULT_NULL_POINTER;
    }

    HandleData* adapterData = (HandleData*)adapter;
    if (adapterData->type != HANDLE_TYPE_ADAPTER) {
        return PAL_RESULT_INVALID_ADAPTER;
    }

    // create a slot for the device
    HandleData* deviceData = getFreeHandleData();
    if (!deviceData) {
        return PAL_RESULT_OUT_OF_MEMORY;
    }

    PalDevice* device = nullptr;
    PalResult ret;
    ret = adapterData->backend->createDevice(
        adapterData->handle, 
        features, 
        &device);

    if (ret != PAL_RESULT_SUCCESS) {
        return ret;
    }
    
    deviceData->backend = adapterData->backend;
    deviceData->handle = device;
    deviceData->type = HANDLE_TYPE_DEVICE;
    deviceData->features = features;

    *outDevice = (PalDevice*)deviceData;
    return PAL_RESULT_SUCCESS;
}

void PAL_CALL palDestroyDevice(PalDevice* device)
{
    if (s_Graphics.initialized && device) {
        HandleData* data = (HandleData*)device;
        if (data->type == HANDLE_TYPE_DEVICE) {
            data->backend->destroyDevice(data->handle);
            freeHandleData(data);
        }
    }
}

PalResult PAL_CALL palAllocateMemory(
    PalDevice* device,
    PalMemoryType type,
    Uint64 size,
    PalMemory** outMemory)
{
    if (!s_Graphics.initialized) {
        return PAL_RESULT_GRAPHICS_NOT_INITIALIZED;
    }

    if (!outMemory || !device) {
        return PAL_RESULT_NULL_POINTER;
    }

    HandleData* data = (HandleData*)device;
    if (data->type != HANDLE_TYPE_DEVICE) {
        return PAL_RESULT_INVALID_DEVICE;
    }

    return data->backend->allocateMemory(
        data->handle, 
        type, 
        size, 
        outMemory);
}

void PAL_CALL palFreeMemory(
    PalDevice* device,
    PalMemory* memory)
{
    if (s_Graphics.initialized && device && memory) {
        HandleData* data = (HandleData*)device;
        if (data->type == HANDLE_TYPE_DEVICE) {
            data->backend->freeMemory(data->handle, memory);
            freeHandleData(data);
        }
    }
}

PalResult PAL_CALL palQueryDepthStencilCapabilities(
    PalDevice* device,
    PalDepthStencilCapabilities* caps)
{
    if (!s_Graphics.initialized) {
        return PAL_RESULT_GRAPHICS_NOT_INITIALIZED;
    }

    if (!device || !caps) {
        return PAL_RESULT_NULL_POINTER;
    }

    HandleData* data = (HandleData*)device;
    if (data->type != HANDLE_TYPE_DEVICE) {
        return PAL_RESULT_INVALID_DEVICE;
    }

    if (!(data->features & PAL_ADAPTER_FEATURE_DEPTH_STENCIL_RESOLVE)) {
        return PAL_RESULT_ADAPTER_FEATURE_NOT_SUPPORTED;
    }

    return data->backend->queryDepthStencilCapabilities(data->handle, caps);
}

PalResult PAL_CALL palQueryFragmentShadingRateCapabilities(
    PalDevice* device,
    PalFragmentShadingRateCapabilities* caps)
{
    if (!s_Graphics.initialized) {
        return PAL_RESULT_GRAPHICS_NOT_INITIALIZED;
    }

    if (!device || !caps) {
        return PAL_RESULT_NULL_POINTER;
    }

    HandleData* data = (HandleData*)device;
    if (data->type != HANDLE_TYPE_DEVICE) {
        return PAL_RESULT_INVALID_DEVICE;
    }

    if (!(data->features & PAL_ADAPTER_FEATURE_FRAGMENT_SHADING_RATE)) {
        return PAL_RESULT_ADAPTER_FEATURE_NOT_SUPPORTED;
    }

    return data->backend->queryFragmentShadingRateCapabilities(
        data->handle, 
        caps);
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
        return PAL_RESULT_GRAPHICS_NOT_INITIALIZED;
    }

    if (!device || !outQueue) {
        return PAL_RESULT_NULL_POINTER;
    }

    HandleData* deviceData = (HandleData*)device;
    if (deviceData->type != HANDLE_TYPE_DEVICE) {
        return PAL_RESULT_INVALID_DEVICE;
    }

    // create a slot for the queue
    HandleData* queueData = getFreeHandleData();
    if (!queueData) {
        return PAL_RESULT_OUT_OF_MEMORY;
    }

    PalQueue* queue = nullptr;
    PalResult ret;
    ret = deviceData->backend->createQueue(
        deviceData->handle,
        type,
        &queue);

    if (ret != PAL_RESULT_SUCCESS) {
        return ret;
    }

    queueData->backend = deviceData->backend;
    queueData->handle = queue;
    queueData->type = HANDLE_TYPE_QUEUE;
    queueData->features = deviceData->features;

    *outQueue = (PalQueue*)queueData;
    return PAL_RESULT_SUCCESS;
}

void PAL_CALL palDestroyQueue(PalQueue* queue)
{
    if (s_Graphics.initialized && queue) {
        HandleData* data = (HandleData*)queue;
        if (data->type == HANDLE_TYPE_QUEUE) {
            data->backend->destroyQueue(data->handle);
            freeHandleData(data);
        }
    }
}

bool PAL_CALL palCanQueuePresent(
    PalQueue* queue, 
    PalGraphicsWindow* window)
{
    if (s_Graphics.initialized && queue) {
        HandleData* data = (HandleData*)queue;
        if (data->type == HANDLE_TYPE_QUEUE) {
            return data->backend->canQueuePresent(data->handle, window);
        }
    }
    return false;
}

// ==================================================
// Format And Usages
// ==================================================

PalResult PAL_CALL palEnumerateFormats(
    PalAdapter* adapter,
    Int32* count,
    PalFormatInfo* outFormats)
{
    if (!s_Graphics.initialized) {
        return PAL_RESULT_GRAPHICS_NOT_INITIALIZED;
    }

    if (!adapter || !count) {
        return PAL_RESULT_NULL_POINTER;
    }

    if (*count == 0 && outFormats) {
        return PAL_RESULT_INSUFFICIENT_BUFFER;
    }

    HandleData* data = (HandleData*)adapter;
    if (data->type != HANDLE_TYPE_ADAPTER) {
        return PAL_RESULT_INVALID_ADAPTER;
    }

    return data->backend->enumerateFormats(data->handle, count, outFormats);
}

bool PAL_CALL palIsFormatSupported(
    PalAdapter* adapter,
    PalFormat format)
{
    if (!s_Graphics.initialized || !adapter) {
        return false;
    }

    HandleData* data = (HandleData*)adapter;
    if (data->type != HANDLE_TYPE_ADAPTER) {
        return false;
    }

    return data->backend->isFormatSupported(data->handle, format);
}

PalImageUsages PAL_CALL palQueryFormatImageUsages(
    PalAdapter* adapter,
    PalFormat format)
{
    if (!s_Graphics.initialized || !adapter) {
        return PAL_IMAGE_USAGE_UNDEFINED;
    }

    HandleData* data = (HandleData*)adapter;
    if (data->type != HANDLE_TYPE_ADAPTER) {
        return PAL_IMAGE_USAGE_UNDEFINED;
    }

    return data->backend->queryFormatImageUsages(data->handle, format);
}

PalImageViewUsages PAL_CALL palQueryFormatImageViewUsages(
    PalAdapter* adapter,
    PalFormat format)
{
    if (!s_Graphics.initialized || !adapter) {
        return PAL_IMAGE_VIEW_USAGE_UNDEFINED;
    }

    HandleData* data = (HandleData*)adapter;
    if (data->type == HANDLE_TYPE_ADAPTER) {
        return PAL_IMAGE_VIEW_USAGE_UNDEFINED;
    }

    return data->backend->queryFormatImageViewUsages(data->handle, format);
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
        return PAL_RESULT_GRAPHICS_NOT_INITIALIZED;
    }

    if (!device ||!info || !outImage) {
        return PAL_RESULT_NULL_POINTER;
    }

    HandleData* data = (HandleData*)device;
    if (data->type != HANDLE_TYPE_DEVICE) {
        return PAL_RESULT_INVALID_DEVICE;
    }

    // create a slot for the image
    HandleData* imageData = getFreeHandleData();
    if (!imageData) {
        return PAL_RESULT_OUT_OF_MEMORY;
    }

    PalImage* image = nullptr;
    PalResult ret;
    ret = data->backend->createImage(
        data->handle,
        info,
        &image);

    if (ret != PAL_RESULT_SUCCESS) {
        return ret;
    }

    imageData->backend = data->backend;
    imageData->handle = image;
    imageData->type = HANDLE_TYPE_IMAGE;
    imageData->data2 = 0;
    imageData->features = data->features;

    *outImage = (PalImage*)imageData;
    return PAL_RESULT_SUCCESS;
}

void PAL_CALL palDestroyImage(PalImage* image)
{
    if (s_Graphics.initialized && image) {
        HandleData* data = (HandleData*)image;
        if (data->type == HANDLE_TYPE_IMAGE) {
            if (data->data2 == SWAPCHAIN_IMAGE) {
                data->backend->destroyImage(data->handle);
                freeHandleData(data);
            }     
        }
    }
}

PalResult PAL_CALL palGetImageInfo(
    PalImage* image,
    PalImageInfo* info)
{
    if (!s_Graphics.initialized) {
        return PAL_RESULT_GRAPHICS_NOT_INITIALIZED;
    }

    if (!image || !info) {
        return PAL_RESULT_NULL_POINTER;
    }

    HandleData* data = (HandleData*)image;
    if (data->type != HANDLE_TYPE_IMAGE) {
        return PAL_RESULT_INVALID_IMAGE;
    }

    return data->backend->getImageInfo(data->handle, info);
}

PalResult PAL_CALL palGetImageMemoryRequirements(
    PalImage* image,
    PalMemoryRequirements* requirements)
{
    if (!s_Graphics.initialized) {
        return PAL_RESULT_GRAPHICS_NOT_INITIALIZED;
    }

    if (!image) {
        return PAL_RESULT_NULL_POINTER;
    }

    HandleData* data = (HandleData*)image;
    if (data->type != HANDLE_TYPE_IMAGE) {
        return PAL_RESULT_INVALID_IMAGE;
    }

    if (data->data2 == SWAPCHAIN_IMAGE) {
        return PAL_RESULT_INVALID_IMAGE;
    }

    return data->backend->getImageMemoryRequirements(
        data->handle, 
        requirements);
}

PalResult PAL_CALL palBindImageMemory(
    PalImage* image,
    PalMemory* memory,
    Uint64 offset)
{
    if (!s_Graphics.initialized) {
        return PAL_RESULT_GRAPHICS_NOT_INITIALIZED;
    }

    if (!image || !memory) {
        return PAL_RESULT_NULL_POINTER;
    }

    HandleData* data = (HandleData*)image;
    if (data->type != HANDLE_TYPE_IMAGE) {
        return PAL_RESULT_INVALID_IMAGE;
    }

    if (data->data2 == SWAPCHAIN_IMAGE) {
        return PAL_RESULT_INVALID_IMAGE;
    }

    return data->backend->bindImageMemory(
        data->handle, 
        memory, 
        offset);
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
        return PAL_RESULT_GRAPHICS_NOT_INITIALIZED;
    }

    if (!device ||!image || !info || !outImageView) {
        return PAL_RESULT_NULL_POINTER;
    }

    HandleData* deviceData = (HandleData*)device;
    HandleData* imageData = (HandleData*)image;
    if (deviceData->type != HANDLE_TYPE_DEVICE) {
        return PAL_RESULT_INVALID_DEVICE;
    }

    if (imageData->type != HANDLE_TYPE_IMAGE) {
        return PAL_RESULT_INVALID_IMAGE;
    }

    // create a slot for the image view
    HandleData* imageViewData = getFreeHandleData();
    if (!imageViewData) {
        return PAL_RESULT_OUT_OF_MEMORY;
    }

    PalImageView* imageView = nullptr;
    PalResult ret;
    ret = deviceData->backend->createImageView(
        deviceData->handle,
        imageData->handle,
        info,
        &imageView);

    if (ret != PAL_RESULT_SUCCESS) {
        return ret;
    }

    imageViewData->backend = deviceData->backend;
    imageViewData->handle = imageView;
    imageViewData->type = HANDLE_TYPE_IMAGE_VIEW;
    imageViewData->features = deviceData->features;

    *outImageView = (PalImageView*)imageViewData;
    return PAL_RESULT_SUCCESS;
}

void PAL_CALL palDestroyImageView(PalImageView* imageView)
{
    if (s_Graphics.initialized && imageView) {
        HandleData* data = (HandleData*)imageView;
        if (data->type == HANDLE_TYPE_IMAGE_VIEW) {
            data->backend->destroyImageView(data->handle);
            freeHandleData(data);
        }
    }
}

// ==================================================
// Swapchain
// ==================================================

PalResult PAL_CALL palQuerySwapchainCapabilities(
    PalDevice* device,
    PalGraphicsWindow* window,
    PalSwapchainCapabilities* caps)
{
    if (!s_Graphics.initialized) {
        return PAL_RESULT_GRAPHICS_NOT_INITIALIZED;
    }

    if (!device || !window || !caps) {
        return PAL_RESULT_NULL_POINTER;
    }

    HandleData* data = (HandleData*)device;
    if (data->type != HANDLE_TYPE_DEVICE) {
        return PAL_RESULT_INVALID_DEVICE;
    }

    if (!(data->features & PAL_ADAPTER_FEATURE_SWAPCHAIN)) {
        return PAL_RESULT_ADAPTER_FEATURE_NOT_SUPPORTED;
    }

    return data->backend->querySwapchainCapabilities(
        data->handle,
        window,
        caps);
}

PalResult PAL_CALL palCreateSwapchain(
    PalDevice* device,
    PalQueue* queue,
    PalGraphicsWindow* window,
    const PalSwapchainCreateInfo* info,
    PalSwapchain** outSwapchain)
{
    if (!s_Graphics.initialized) {
        return PAL_RESULT_GRAPHICS_NOT_INITIALIZED;
    }

    if (!device || !queue || !window || !info || !outSwapchain) {
        return PAL_RESULT_NULL_POINTER;
    }

    HandleData* imagesData = nullptr;
    imagesData = palAllocate(
        s_Graphics.allocator, 
        sizeof(HandleData) * info->imageCount, 
        0);

    if (!imagesData) {
        return PAL_RESULT_OUT_OF_MEMORY;
    }

    HandleData* deviceData = (HandleData*)device;
    HandleData* queueData = (HandleData*)queue;
    if (deviceData->type != HANDLE_TYPE_DEVICE) {
        return PAL_RESULT_INVALID_DEVICE;
    }

    if (queueData->type != HANDLE_TYPE_QUEUE) {
        return PAL_RESULT_INVALID_QUEUE;
    }

    // create a slot for the swapchain
    HandleData* swapchainData = getFreeHandleData();
    if (!swapchainData) {
        return PAL_RESULT_OUT_OF_MEMORY;
    }

    PalResult ret;
    PalSwapchain* swapchain = nullptr;
    ret = deviceData->backend->createSwapchain(
        deviceData->handle, 
        queueData->handle, 
        window, 
        info, 
        &swapchain);

    if (ret != PAL_RESULT_SUCCESS) {
        return ret;
    }

    // cache the swapchain images so we dont create new handles
    // for them anytime they are queried
    for (int i = 0; i < info->imageCount; i++) {
        PalImage* image = deviceData->backend->getSwapchainImage(swapchain, i);
        HandleData* tmp = &imagesData[i];
        tmp->backend = swapchainData->backend;
        tmp->handle = image;
        tmp->used = true;
        tmp->shouldFree = false; // we free all at once
        tmp->data = nullptr;
        tmp->type = HANDLE_TYPE_IMAGE;
        tmp->data2 = SWAPCHAIN_IMAGE;
        tmp->features = deviceData->features;
    }
    
    swapchainData->backend = deviceData->backend;
    swapchainData->handle = swapchain;
    swapchainData->data = (void*)imagesData;
    swapchainData->data2 = info->imageCount;
    swapchainData->type = HANDLE_TYPE_SWAPCHAIN;
    swapchainData->features = deviceData->features;

    *outSwapchain = (PalSwapchain*)swapchainData;
    return PAL_RESULT_SUCCESS;
}

void PAL_CALL palDestroySwapchain(PalSwapchain* swapchain)
{
    if (s_Graphics.initialized && swapchain) {
        HandleData* data = (HandleData*)swapchain;
        if (data->type == HANDLE_TYPE_SWAPCHAIN) {
            data->backend->destroySwapchain(data->handle);
            palFree(s_Graphics.allocator, data->data);
            freeHandleData(data);
        }
    }
}

Uint32 PAL_CALL palGetSwapchainImageCount(PalSwapchain* swapchain)
{
    if (s_Graphics.initialized && swapchain) {
        HandleData* data = (HandleData*)swapchain;
        if (data->type == HANDLE_TYPE_SWAPCHAIN) {
            return data->data2;
        }
    }
    return 0;
}

PalImage* PAL_CALL palGetSwapchainImage(
    PalSwapchain* swapchain,
    Int32 index)
{
    if (!s_Graphics.initialized || !swapchain || index < 0) {
        return nullptr;
    }

    HandleData* data = (HandleData*)swapchain;
    if (data->type != HANDLE_TYPE_SWAPCHAIN) {
        return nullptr;
    }

    if (index > data->data2) {
        return nullptr;
    }

    HandleData* imagesData = data->data;
    return (PalImage*)&imagesData[index];
}

PalImage* PAL_CALL palGetNextSwapchainImage(
    PalSwapchain* swapchain,
    PalNextImageInfo* info)
{
    if (!s_Graphics.initialized || !swapchain || !info) {
        return nullptr;
    }

    HandleData* data = (HandleData*)swapchain;
    if (data->type != HANDLE_TYPE_SWAPCHAIN) {
        return nullptr;
    }

    void* FenceHandle = nullptr;
    void* signalSemaphoreHandle = nullptr;
    HandleData* tmp = (HandleData*)info->signalSemaphore;
    if (info->fence) {
        tmp = (HandleData*)info->signalSemaphore;
        if (tmp->type != HANDLE_TYPE_FENCE) {
            return nullptr;
        }
        FenceHandle = tmp->handle;
    }

    if (info->signalSemaphore) {
        tmp = (HandleData*)info->signalSemaphore;
        if (tmp->type != HANDLE_TYPE_SEMAPHORE) {
            return nullptr;
        }
        signalSemaphoreHandle = tmp->handle;
    }

    PalNextImageInfo nextInfo;
    nextInfo.fence = FenceHandle;
    nextInfo.signalSemaphore = signalSemaphoreHandle;
    nextInfo.signalValue = info->signalValue;
    nextInfo.timeout = info->timeout;

    PalImage* tmpImage = data->backend->getNextSwapchainImage(
        data->handle,
        &nextInfo);

    // loop through all our cache images and get the handle data
    // associated with the image
    HandleData* imagesData = data->data;
    HandleData* imageData = nullptr;
    for (int i = 0; i < data->data2; i++) {
        if (imagesData[i].handle == tmpImage) {
            // found our handle info
            imageData = &imagesData[i];
            break;
        }
    }

    return (PalImage*)imageData;
}

PalResult PAL_CALL palPresentSwapchain(
    PalSwapchain* swapchain,
    PalPresentInfo* info)
{
    if (!s_Graphics.initialized) {
        return PAL_RESULT_GRAPHICS_NOT_INITIALIZED;
    }

    if (!swapchain || !info) {
        return PAL_RESULT_NULL_POINTER;
    }

    HandleData* swapchainData = (HandleData*)swapchain;
    if (swapchainData->type != HANDLE_TYPE_SWAPCHAIN) {
        return PAL_RESULT_INVALID_SWAPCHAIN;
    }

    HandleData* imageData = (HandleData*)info->image;
    if (imageData->type != HANDLE_TYPE_IMAGE) {
        return PAL_RESULT_INVALID_IMAGE;
    }

    if (imageData->data2 != SWAPCHAIN_IMAGE) {
        return PAL_RESULT_INVALID_IMAGE;
    }

    void* waitSemaphoreHandle = nullptr;
    if (info->waitSemaphore) {
        HandleData* tmp = (HandleData*)info->waitSemaphore;
        if (tmp->type != HANDLE_TYPE_SEMAPHORE) {
            return PAL_RESULT_INVALID_SEMAPHORE;
        }
        waitSemaphoreHandle = tmp->handle;
    }

    PalPresentInfo presentInfo;
    presentInfo.image = imageData->handle;
    presentInfo.waitValue = info->waitValue;
    presentInfo.waitSemaphore = waitSemaphoreHandle;

    return swapchainData->backend->presentSwapchain(
        swapchainData->handle, 
        &presentInfo);
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
        return PAL_RESULT_GRAPHICS_NOT_INITIALIZED;
    }

    if (!device || !info || !outShader) {
        return PAL_RESULT_NULL_POINTER;
    }

    HandleData* data = (HandleData*)device;
    if (data->type != HANDLE_TYPE_DEVICE) {
        return PAL_RESULT_INVALID_DEVICE;
    }

    if (info->type == PAL_SHADER_TYPE_UNDEFINED) {
        return PAL_RESULT_INVALID_SHADER_TYPE;
    
    } else if (info->type == PAL_SHADER_TYPE_COMPUTE) {
        if (data->features & PAL_ADAPTER_FEATURE_COMPUTE_SHADER) {
            return PAL_RESULT_ADAPTER_FEATURE_NOT_SUPPORTED;
        }

    } else if (info->type == PAL_SHADER_TYPE_TESSELLATION_CONTROL) {
        if (data->features & PAL_ADAPTER_FEATURE_TESSELLATION_SHADER) {
            return PAL_RESULT_ADAPTER_FEATURE_NOT_SUPPORTED;
        }

    } else if (info->type == PAL_SHADER_TYPE_TESSELLATION_EVALUATION) {
        if (data->features & PAL_ADAPTER_FEATURE_TESSELLATION_SHADER) {
            return PAL_RESULT_ADAPTER_FEATURE_NOT_SUPPORTED;
        }

    } else if (info->type == PAL_SHADER_TYPE_MESH) {
        if (data->features & PAL_ADAPTER_FEATURE_MESH_SHADER) {
            return PAL_RESULT_ADAPTER_FEATURE_NOT_SUPPORTED;
        }

    } else if (info->type == PAL_SHADER_TYPE_TASK) {
        if (data->features & PAL_ADAPTER_FEATURE_MESH_SHADER) {
            return PAL_RESULT_ADAPTER_FEATURE_NOT_SUPPORTED;
        }
    }

    // create a slot for the shader
    HandleData* shaderData = getFreeHandleData();
    if (!shaderData) {
        return PAL_RESULT_OUT_OF_MEMORY;
    }

    PalShader* shader = nullptr;
    PalResult ret;
    ret = data->backend->createShader(
        data->handle,
        info,
        &shader);

    if (ret != PAL_RESULT_SUCCESS) {
        return ret;
    }

    shaderData->backend = data->backend;
    shaderData->handle = shader;
    shaderData->type = HANDLE_TYPE_SHADER;
    shaderData->data2 = (Uint32)info->type;
    shaderData->features = data->features;

    *outShader = (PalShader*)shaderData;
    return PAL_RESULT_SUCCESS;
}

void PAL_CALL palDestroyShader(PalShader* shader)
{
    if (s_Graphics.initialized && shader) {
        HandleData* data = (HandleData*)shader;
        if (data->type == HANDLE_TYPE_SHADER) {
            data->backend->destroyShader(data->handle);
            freeHandleData(data);
        }
    }
}

PalShaderType PAL_CALL palGetShaderType(PalShader* shader)
{
    if (s_Graphics.initialized && shader) {
        HandleData* data = (HandleData*)shader;
        if (data->type == HANDLE_TYPE_SHADER) {
            return (PalShaderType)data->data2;
        }
    }
    return PAL_SHADER_TYPE_UNDEFINED;
}

// ==================================================
// Render Pass
// ==================================================

PalResult PAL_CALL palCreateRenderPass(
    PalDevice* device,
    const PalRenderPassCreateInfo* info,
    PalRenderPass** outRenderPass)
{
    if (!s_Graphics.initialized) {
        return PAL_RESULT_GRAPHICS_NOT_INITIALIZED;
    }

    if (!device || !info || !outRenderPass) {
        return PAL_RESULT_NULL_POINTER;
    }

    if (info->attachmentCount == 0 && info->attachments) {
        return PAL_RESULT_INSUFFICIENT_BUFFER;
    }

    HandleData* data = (HandleData*)device;
    if (data->type != HANDLE_TYPE_DEVICE) {
        return PAL_RESULT_INVALID_DEVICE;
    }

    PalAttachmentDesc attachments[16]; // should be fine
    PalRenderPassCreateInfo createInfo = {0};
    createInfo.attachmentCount = info->attachmentCount;
    createInfo.attachments = attachments;
    createInfo.width = info->width;
    createInfo.height = info->height;

    HandleData* tmp = nullptr;
    for (int i = 0; i < info->attachmentCount; i++) {
        if (!info->attachments[i].target) {
            return PAL_RESULT_NULL_POINTER;
        }

        tmp = (HandleData*)info->attachments[i].target;
        if (tmp->type != HANDLE_TYPE_IMAGE_VIEW) {
            return PAL_RESULT_INVALID_IMAGE_VIEW;
        }

        attachments[i].target = tmp->handle;
        attachments[i].loadOp = info->attachments[i].loadOp;
        attachments[i].storeOp = info->attachments[i].storeOp;
        attachments[i].stencilLoadOp = info->attachments[i].stencilLoadOp;
        attachments[i].stencilStoreOp = info->attachments[i].stencilStoreOp;
        attachments[i].resolveMode = info->attachments[i].resolveMode ;
        attachments[i].type = info->attachments[i].type;
        attachments[i].resolveTarget = nullptr;
        attachments[i].stencilResolveMode = 
            info->attachments[i].stencilResolveMode;

        if (info->attachments[i].resolveTarget) {
            tmp = (HandleData*)info->attachments[i].resolveTarget;
            if (tmp->type != HANDLE_TYPE_IMAGE_VIEW) {
                return PAL_RESULT_INVALID_IMAGE_VIEW;
            }
            attachments[i].resolveTarget = tmp->handle;
        }
    }

    // create a slot for the renderpass
    HandleData* renderPassData = getFreeHandleData();
    if (!renderPassData) {
        return PAL_RESULT_OUT_OF_MEMORY;
    }

    PalRenderPass* renderpass = nullptr;
    PalResult ret;
    ret = data->backend->createRenderPass(
        data->handle,
        &createInfo,
        &renderpass);

    if (ret != PAL_RESULT_SUCCESS) {
        return ret;
    }

    renderPassData->backend = data->backend;
    renderPassData->handle = renderpass;
    renderPassData->type = HANDLE_TYPE_RENDER_PASS;
    renderPassData->features = data->features;

    *outRenderPass = (PalRenderPass*)renderPassData;
    return PAL_RESULT_SUCCESS;
}

void PAL_CALL palDestroyRenderPass(PalRenderPass* renderPass)
{
    if (s_Graphics.initialized && renderPass) {
        HandleData* data = (HandleData*)renderPass;
        if (data->type == HANDLE_TYPE_RENDER_PASS) {
            data->backend->destroyRenderPass(data->handle);
            freeHandleData(data);
        }
    }
}

// ==================================================
// Fence
// ==================================================

PalResult PAL_CALL palCreateFence(
    PalDevice* device,
    PalFence** outFence)
{
    if (!s_Graphics.initialized) {
        return PAL_RESULT_GRAPHICS_NOT_INITIALIZED;
    }

    if (!device || !outFence) {
        return PAL_RESULT_NULL_POINTER;
    }

    HandleData* data = (HandleData*)device;
    if (data->type != HANDLE_TYPE_DEVICE) {
        return PAL_RESULT_INVALID_DEVICE;
    }

    // create a slot for the fence
    HandleData* fenceData = getFreeHandleData();
    if (!fenceData) {
        return PAL_RESULT_OUT_OF_MEMORY;
    }

    PalFence* fence = nullptr;
    PalResult ret;
    ret = data->backend->createFence(data->handle, &fence);
    if (ret != PAL_RESULT_SUCCESS) {
        return ret;
    }

    fenceData->backend = data->backend;
    fenceData->handle = fence;
    fenceData->type = HANDLE_TYPE_FENCE;
    fenceData->features = data->features;

    *outFence = (PalFence*)fenceData;
    return PAL_RESULT_SUCCESS;
}

void PAL_CALL palDestroyFence(PalFence* fence)
{
    if (s_Graphics.initialized && fence) {
        HandleData* data = (HandleData*)fence;
        if (data->type == HANDLE_TYPE_FENCE) {
            data->backend->destroyFence(data->handle);
            freeHandleData(data);
        }
    }
}

PalResult PAL_CALL palWaitFence(
    PalFence* fence, 
    Uint64 timeout)
{
    if (!s_Graphics.initialized) {
        return PAL_RESULT_GRAPHICS_NOT_INITIALIZED;
    }

    if (!fence) {
        return PAL_RESULT_NULL_POINTER;
    }

    HandleData* data = (HandleData*)fence;
    if (data->type != HANDLE_TYPE_FENCE) {
        return PAL_RESULT_INVALID_FENCE;
    }

    return data->backend->waitFenceTimeout(data->handle, timeout);
}

PalResult PAL_CALL palResetFence(PalFence* fence)
{
    if (!s_Graphics.initialized) {
        return PAL_RESULT_GRAPHICS_NOT_INITIALIZED;
    }

    if (!fence) {
        return PAL_RESULT_NULL_POINTER;
    }

    HandleData* data = (HandleData*)fence;
    if (data->type != HANDLE_TYPE_FENCE) {
        return PAL_RESULT_INVALID_FENCE;
    }

    if (!(data->features & PAL_ADAPTER_FEATURE_FENCE_RESET)) {
        return PAL_RESULT_ADAPTER_FEATURE_NOT_SUPPORTED;
    }

    return data->backend->resetFence(data->handle);
}

bool PAL_CALL palIsFenceSignaled(PalFence* fence)
{
    if (s_Graphics.initialized && fence) {
        HandleData* data = (HandleData*)fence;
        if (data->type == HANDLE_TYPE_FENCE) {
            return data->backend->isFenceSignaled(data->handle);
        }
    }
    return false;
}

// ==================================================
// Semaphore
// ==================================================

PalResult PAL_CALL palCreateSemaphore(
    PalDevice* device,
    PalSemaphore** outSemaphore)
{
    if (!s_Graphics.initialized) {
        return PAL_RESULT_GRAPHICS_NOT_INITIALIZED;
    }

    if (!device || !outSemaphore) {
        return PAL_RESULT_NULL_POINTER;
    }

    HandleData* data = (HandleData*)device;
    if (data->type != HANDLE_TYPE_DEVICE) {
        return PAL_RESULT_INVALID_DEVICE;
    }

    // create a slot for the semaphore
    HandleData* semaphoreData = getFreeHandleData();
    if (!semaphoreData) {
        return PAL_RESULT_OUT_OF_MEMORY;
    }

    PalSemaphore* semaphore = nullptr;
    PalResult ret;
    ret = data->backend->createSemaphore(data->handle, &semaphore);
    if (ret != PAL_RESULT_SUCCESS) {
        return ret;
    }

    semaphoreData->backend = data->backend;
    semaphoreData->handle = semaphore;
    semaphoreData->type = HANDLE_TYPE_SEMAPHORE;
    semaphoreData->features = data->features;

    semaphoreData->data2 = BINARY_SEMAPHORE;
    if (data->features & PAL_ADAPTER_FEATURE_TIMELINE_SEMAPHORE) {
        semaphoreData->data2 = TIMELINE_SEMAPHORE;
    }

    *outSemaphore = (PalSemaphore*)semaphoreData;
    return PAL_RESULT_SUCCESS;
}

void PAL_CALL palDestroySemaphore(PalSemaphore* semaphore)
{
    if (s_Graphics.initialized && semaphore) {
        HandleData* data = (HandleData*)semaphore;
        if (data->type != HANDLE_TYPE_SEMAPHORE) {
            data->backend->destroySemaphore(data->handle);
            freeHandleData(data);
        }
    }
}

PalResult PAL_CALL palWaitSemaphore(
    PalSemaphore* semaphore, 
    PalQueue* queue,
    Uint64 value,
    Uint64 timeout)
{
    if (!s_Graphics.initialized) {
        return PAL_RESULT_GRAPHICS_NOT_INITIALIZED;
    }

    if (!semaphore || !queue) {
        return PAL_RESULT_NULL_POINTER;
    }

    HandleData* semaphoreData = (HandleData*)semaphore;
    if (semaphoreData->type != HANDLE_TYPE_SEMAPHORE) {
        return PAL_RESULT_INVALID_SEMAPHORE;
    }

    HandleData* queueData = (HandleData*)queue;
    if (queueData->type != HANDLE_TYPE_QUEUE) {
        return PAL_RESULT_INVALID_QUEUE;
    }

    return semaphoreData->backend->waitSemaphore(
        semaphoreData->handle, 
        queueData->handle,
        value,
        timeout);
}

PalResult PAL_CALL palSignalSemaphore(
    PalSemaphore* semaphore, 
    PalQueue* queue,
    Uint64 value)
{
    if (!s_Graphics.initialized) {
        return PAL_RESULT_GRAPHICS_NOT_INITIALIZED;
    }

    if (!semaphore || !queue) {
        return PAL_RESULT_NULL_POINTER;
    }

    HandleData* semaphoreData = (HandleData*)semaphore;
    if (semaphoreData->type != HANDLE_TYPE_SEMAPHORE) {
        return PAL_RESULT_INVALID_SEMAPHORE;
    }

    HandleData* queueData = (HandleData*)queue;
    if (queueData->type != HANDLE_TYPE_QUEUE) {
        return PAL_RESULT_INVALID_QUEUE;
    }

    return semaphoreData->backend->signalSemaphore(
        semaphoreData->handle, 
        queueData->handle,
        value);
}

PalResult PAL_CALL palGetSemaphoreValue(
    PalSemaphore* semaphore, 
    Uint64* value)
{
    if (!s_Graphics.initialized) {
        return PAL_RESULT_GRAPHICS_NOT_INITIALIZED;
    }

    if (!semaphore || !value) {
        return PAL_RESULT_NULL_POINTER;
    }

    HandleData* data = (HandleData*)semaphore;
    if (data->type != HANDLE_TYPE_SEMAPHORE) {
        return PAL_RESULT_INVALID_SEMAPHORE;
    }

    if (data->data2 == BINARY_SEMAPHORE) {
        return PAL_RESULT_INVALID_SEMAPHORE;
    }

    return data->backend->getSemaphoreValue(
        data->handle, 
        value);
}

bool PAL_CALL palIsTimelineSemaphore(PalSemaphore* semaphore)
{
    if (s_Graphics.initialized && semaphore) {
        HandleData* data = (HandleData*)semaphore;
        if (data->type == HANDLE_TYPE_SEMAPHORE) {
            if (data->data2 == TIMELINE_SEMAPHORE) {
                return true;
            }
        }
    }
    return false;
}

// ==================================================
// Command Pool And Buffer
// ==================================================

PalResult PAL_CALL palCreateCommandPool(
    PalDevice* device,
    const PalCommandPoolCreateInfo* info,
    PalCommandPool** outPool)
{
    if (!s_Graphics.initialized) {
        return PAL_RESULT_GRAPHICS_NOT_INITIALIZED;
    }

    if (!device || !info || !outPool) {
        return PAL_RESULT_NULL_POINTER;
    }

    if (!info->queue) {
        return PAL_RESULT_NULL_POINTER;
    }

    HandleData* deviceData = (HandleData*)device;
    if (deviceData->type != HANDLE_TYPE_DEVICE) {
        return PAL_RESULT_INVALID_DEVICE;
    }

    HandleData* queueData = (HandleData*)info->queue;
    if (queueData->type != HANDLE_TYPE_QUEUE) {
        return PAL_RESULT_INVALID_QUEUE;
    }

    PalCommandPoolCreateInfo createInfo = {0};
    createInfo.queue = queueData->handle;
    createInfo.resettable = info->resettable;
    createInfo.transient = info->transient;

    // create a slot for the command pool
    HandleData* poolData = getFreeHandleData();
    if (!poolData) {
        return PAL_RESULT_OUT_OF_MEMORY;
    }

    PalCommandPool* pool = nullptr;
    PalResult ret;
    ret = deviceData->backend->createCommandPool(
        deviceData->handle,
        &createInfo,
        &pool);

    if (ret != PAL_RESULT_SUCCESS) {
        return ret;
    }

    poolData->backend = deviceData->backend;
    poolData->handle = pool;
    poolData->type = HANDLE_TYPE_COMMAND_POOL;
    poolData->features = deviceData->features;

    *outPool = (PalCommandPool*)poolData;
    return PAL_RESULT_SUCCESS;
}

void PAL_CALL palDestroyCommandPool(PalCommandPool* pool)
{
    if (s_Graphics.initialized && pool) {
        HandleData* data = (HandleData*)pool;
        if (data->type == HANDLE_TYPE_COMMAND_POOL) {
            data->backend->destroyCommandPool(data->handle);
            freeHandleData(data);
        }
    }
}

PalResult PAL_CALL palCreateCommandBuffer(
    PalDevice* device,
    PalCommandPool* pool,
    bool primary,
    PalCommandBuffer** outCmdBuffer)
{
    if (!s_Graphics.initialized) {
        return PAL_RESULT_GRAPHICS_NOT_INITIALIZED;
    }

    if (!device || !pool || !outCmdBuffer) {
        return PAL_RESULT_NULL_POINTER;
    }

    HandleData* data = (HandleData*)device;
    if (data->type != HANDLE_TYPE_DEVICE) {
        return PAL_RESULT_INVALID_DEVICE;
    }

    HandleData* poolData = (HandleData*)pool;
    if (poolData->type != HANDLE_TYPE_COMMAND_POOL) {
        return PAL_RESULT_INVALID_COMMAND_POOL;
    }

    // create a slot for the command buffer
    HandleData* cmdBufferData = getFreeHandleData();
    if (!cmdBufferData) {
        return PAL_RESULT_OUT_OF_MEMORY;
    }

    PalCommandBuffer* cmdBuffer = nullptr;
    PalResult ret;
    ret = data->backend->createCommandBuffer(
        data->handle,
        poolData->handle,
        primary,
        &cmdBuffer);

    if (ret != PAL_RESULT_SUCCESS) {
        return ret;
    }

    cmdBufferData->backend = data->backend;
    cmdBufferData->handle = cmdBuffer;
    cmdBufferData->type = HANDLE_TYPE_COMMAND_BUFFER;
    cmdBufferData->features = data->features;

    *outCmdBuffer = (PalCommandBuffer*)cmdBufferData;
    return PAL_RESULT_SUCCESS;
}

void PAL_CALL palDestroyCommandBuffer(PalCommandBuffer* cmdBuffer)
{
    if (s_Graphics.initialized && cmdBuffer) {
        HandleData* data = (HandleData*)cmdBuffer;
        if (data->type == HANDLE_TYPE_COMMAND_BUFFER) {
            data->backend->destroyCommandBuffer(data->handle);
            freeHandleData(data);
        }
    }
}

PalResult PAL_CALL palExecuteCommandBuffer(
    PalCommandBuffer* primaryCmdBuffer,
    PalCommandBuffer* secondaryCmdBuffer)
{
    if (!s_Graphics.initialized) {
        return PAL_RESULT_GRAPHICS_NOT_INITIALIZED;
    }

    if (!primaryCmdBuffer || !secondaryCmdBuffer) {
        return PAL_RESULT_NULL_POINTER;
    }

    HandleData* primaryCmdBufferData = (HandleData*)primaryCmdBuffer;
    HandleData* secondaryCmdBufferData = (HandleData*)secondaryCmdBuffer;
    if (primaryCmdBufferData->type != HANDLE_TYPE_COMMAND_BUFFER) {
        return PAL_RESULT_INVALID_COMMAND_BUFFER;
    }

    if (secondaryCmdBufferData->type != HANDLE_TYPE_COMMAND_BUFFER) {
        return PAL_RESULT_INVALID_COMMAND_BUFFER;
    }

    return primaryCmdBufferData->backend->executeCommandBuffer(
        primaryCmdBufferData->handle, 
        secondaryCmdBufferData->handle);
}

PalResult PAL_CALL palBeginRenderPass(
    PalCommandBuffer* cmdBuffer,
    PalRenderPass* renderPass,
    Int32 clearValueCount,
    PalClearValue* clearValues)
{
    if (!s_Graphics.initialized) {
        return PAL_RESULT_GRAPHICS_NOT_INITIALIZED;
    }

    if (!cmdBuffer || !renderPass) {
        return PAL_RESULT_NULL_POINTER;
    }

    if (clearValueCount == 0 && clearValues) {
        return PAL_RESULT_INSUFFICIENT_BUFFER;
    }

    HandleData* cmdBufferData = (HandleData*)cmdBuffer;
    if (cmdBufferData->type != HANDLE_TYPE_COMMAND_BUFFER) {
        return PAL_RESULT_INVALID_COMMAND_BUFFER;
    }

    HandleData* renderPassData = (HandleData*)renderPass;
    if (renderPassData->type != HANDLE_TYPE_RENDER_PASS) {
        return PAL_RESULT_INVALID_RENDER_PASS;
    }

    return cmdBufferData->backend->beginRenderPass(
        cmdBufferData->handle,
        renderPassData->handle,
        clearValueCount,
        clearValues);
}

PalResult PAL_CALL palEndRenderPass(PalCommandBuffer* cmdBuffer)
{
    if (!s_Graphics.initialized) {
        return PAL_RESULT_GRAPHICS_NOT_INITIALIZED;
    }

    if (!cmdBuffer) {
        return PAL_RESULT_NULL_POINTER;
    }

    HandleData* data = (HandleData*)cmdBuffer;
    if (data->type != HANDLE_TYPE_COMMAND_BUFFER) {
        return PAL_RESULT_INVALID_COMMAND_BUFFER;
    }

    return data->backend->endRenderPass(data->handle);
}

PalResult PAL_CALL palSubmitCommandBuffer(
    PalQueue* queue,
    PalSubmitInfo* info)
{
    if (!s_Graphics.initialized) {
        return PAL_RESULT_GRAPHICS_NOT_INITIALIZED;
    }

    if (!queue || !info) {
        return PAL_RESULT_NULL_POINTER;
    }

    HandleData* data = (HandleData*)queue;
    if (data->type != HANDLE_TYPE_QUEUE) {
        return PAL_RESULT_INVALID_QUEUE;
    }

    HandleData* cmdBufferData = (HandleData*)info->cmdBuffer;
    if (cmdBufferData->type != HANDLE_TYPE_COMMAND_BUFFER) {
        return PAL_RESULT_INVALID_COMMAND_BUFFER;
    }

    void* fenceHandle = nullptr;
    void* waitSemaphoreHandle = nullptr;
    void* signalSemaphoreHandle = nullptr;
    if (info->fence) {
        HandleData* tmp = (HandleData*)info->fence;
        if (tmp->type == HANDLE_TYPE_FENCE) {
            fenceHandle = tmp->handle;
        }
    }

    if (info->waitSemaphore) {
        HandleData* tmp = (HandleData*)info->waitSemaphore;
        if (tmp->type == HANDLE_TYPE_SEMAPHORE) {
            waitSemaphoreHandle = tmp->handle;
        }
    }

    if (info->signalSemaphore) {
        HandleData* tmp = (HandleData*)info->signalSemaphore;
        if (tmp->type == HANDLE_TYPE_SEMAPHORE) {
            signalSemaphoreHandle = tmp->handle;
        }
    }

    PalSubmitInfo submitInfo;
    submitInfo.cmdBuffer = cmdBufferData->handle;
    submitInfo.fence = fenceHandle;
    submitInfo.signalSemaphore = signalSemaphoreHandle;
    submitInfo.waitSemaphore = waitSemaphoreHandle;
    submitInfo.signalValue = info->signalValue;
    submitInfo.waitValue = info->waitValue;

    return data->backend->submitCommandBuffer(
        data->handle,
        &submitInfo);
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
        return PAL_RESULT_GRAPHICS_NOT_INITIALIZED;
    }

    if (!device || !info || !outPipeline) {
        return PAL_RESULT_NULL_POINTER;
    }

    if (!info->fragmentShader) {
        return PAL_RESULT_NULL_POINTER;
    }

    if (!info->meshShader && !info->vertexShader) {
        return PAL_RESULT_NULL_POINTER;
    }

    HandleData* deviceData = (HandleData*)device;
    if (deviceData->type != HANDLE_TYPE_DEVICE) {
        return PAL_RESULT_INVALID_DEVICE;
    }

    // shaders
    void* vShaderHandle = nullptr;
    void* fShaderHandle = nullptr;
    void* gShaderHandle = nullptr;
    void* mShaderHandle = nullptr;
    void* taskShaderHandle = nullptr;
    void* tessEShaderHandle = nullptr;
    void* tessCShaderHandle = nullptr;

    // create a slot for the pipeline
    HandleData* pipelineData = getFreeHandleData();
    if (!pipelineData) {
        return PAL_RESULT_OUT_OF_MEMORY;
    }

    HandleData* tmp = nullptr;
    // vertex shader path
    if (info->vertexShader) {
        tmp = (HandleData*)info->vertexShader;
        if (tmp->type == HANDLE_TYPE_SHADER && 
            tmp->data2 == PAL_SHADER_TYPE_VERTEX) {
            vShaderHandle = tmp->handle;
        }

        // tessellation control shader
        tmp = (HandleData*)info->tessellationControlShader;
        if (tmp->type == HANDLE_TYPE_SHADER && 
            tmp->data2 == PAL_SHADER_TYPE_TESSELLATION_CONTROL) {
            tessCShaderHandle = tmp->handle;
        }

        // tessellation evaluation shader
        tmp = (HandleData*)info->tessellationEvaluationShader;
        if (tmp->type == HANDLE_TYPE_SHADER && 
            tmp->data2 == PAL_SHADER_TYPE_TESSELLATION_EVALUATION) {
            tessEShaderHandle = tmp->handle;
        }

        // geometry shader
        tmp = (HandleData*)info->geometryShader;
        if (tmp->type == HANDLE_TYPE_SHADER && 
            tmp->data2 == PAL_SHADER_TYPE_GEOMETRY) {
            gShaderHandle = tmp->handle;
        }

    } else {
        // task shader
        tmp = (HandleData*)info->taskShader;
        if (tmp->type == HANDLE_TYPE_SHADER && 
            tmp->data2 == PAL_SHADER_TYPE_TASK) {
            taskShaderHandle = tmp->handle;
        }

        // mesh shader
        tmp = (HandleData*)info->meshShader;
        if (tmp->type == HANDLE_TYPE_SHADER && 
            tmp->data2 == PAL_SHADER_TYPE_MESH) {
            mShaderHandle = tmp->handle;
        }
    }

    // fragment shader
    tmp = (HandleData*)info->fragmentShader;
    if (tmp->type == HANDLE_TYPE_SHADER && 
        tmp->data2 == PAL_SHADER_TYPE_FRAGMENT) {
        fShaderHandle = tmp->handle;
    }

    PalGraphicsPipelineCreateInfo createInfo;
    createInfo.fragmentShader = fShaderHandle;
    createInfo.geometryShader = gShaderHandle;
    createInfo.meshShader = mShaderHandle;
    createInfo.taskShader = taskShaderHandle;
    createInfo.tessellationControlShader = tessCShaderHandle;
    createInfo.tessellationEvaluationShader = tessEShaderHandle;
    createInfo.vertexShader = vShaderHandle;

    createInfo.topology = info->topology;
    createInfo.blendAttachmentCount = info->blendAttachmentCount;
    createInfo.blendAttachments = info->blendAttachments;

    createInfo.depthStencilState = info->depthStencilState;
    createInfo.multisampleState = info->multisampleState;
    createInfo.rasterizerState = info->rasterizerState;
    
    createInfo.vertexLayoutCount = info->vertexLayoutCount;
    createInfo.vertexLayouts = info->vertexLayouts;

    PalResult ret;
    PalPipeline* pipeline = nullptr;
    ret = deviceData->backend->createGraphicsPipeline(
        deviceData->handle,
        &createInfo,
        &pipeline);

    if (ret != PAL_RESULT_SUCCESS) {
        return ret;
    }
    
    pipelineData->backend = deviceData->backend;
    pipelineData->handle = pipeline;
    pipelineData->type = HANDLE_TYPE_PIPELINE;
    pipelineData->data2 = GRAPHICS_PIPELINE;
    pipelineData->features = deviceData->features;

    *outPipeline = (PalPipeline*)pipelineData;
    return PAL_RESULT_SUCCESS;
}

void PAL_CALL palDestroyPipeline(PalPipeline* pipeline)
{
    if (s_Graphics.initialized && pipeline) {
        HandleData* data = (HandleData*)pipeline;
        if (data->type == HANDLE_TYPE_PIPELINE) {
            data->backend->destroyPipeline(data->handle);
            freeHandleData(data);
        }
    }
}

bool PAL_CALL palIsGraphicsPipeline(PalPipeline* pipeline)
{
    if (s_Graphics.initialized && pipeline) {
        HandleData* data = (HandleData*)pipeline;
        if (data->type == HANDLE_TYPE_PIPELINE) {
            if (data->data2 == GRAPHICS_PIPELINE) {
                return true;
            }
        }
    }
    return false;
}