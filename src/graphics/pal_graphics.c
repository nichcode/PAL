
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
#define TO_PAL_HANDLE(type, value) ((type*)(UintPtr)(value))
#define FROM_PAL_HANDLE(handle) ((Uint64)(UintPtr)(handle))

typedef struct {
    bool used;
    void* handle;
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

static HandleData* getFreeHandleData(Uint64* outIndex)
{
    for (int i = 0; i < s_Graphics.maxHandleData; ++i) {
        if (!s_Graphics.handleData[i].used) {
            s_Graphics.handleData[i].used = true;
            *outIndex = i + 1;
            return &s_Graphics.handleData[i];
        }
    }  

    // resize the data array
    HandleData* data = nullptr;


    Uint32 newSize = s_Graphics.maxHandleData * 2; // double the size
    int freeIndex = s_Graphics.maxHandleData + 1;
    data = palAllocate(s_Graphics.allocator, sizeof(HandleData) * newSize, 0);
    if (data) {
        memset(data, 0, sizeof(HandleData) * newSize);
        for (int i = 0; i < s_Graphics.maxHandleData; i++) {
            // copy (shallow) old array into the new one
            data[i] = s_Graphics.handleData[i];
        }

        palFree(s_Graphics.allocator, s_Graphics.handleData);
        s_Graphics.handleData = data;
        s_Graphics.maxHandleData = newSize;

        s_Graphics.handleData[freeIndex].used = true;
        *outIndex = freeIndex;
        return &s_Graphics.handleData[freeIndex];
    }
    return nullptr;
}

static HandleData* findHandleData(Uint64 index)
{
    if (index < 1 || index > s_Graphics.maxHandleData) {
        return nullptr;
    }
    return &s_Graphics.handleData[index - 1];
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
    PalDevice* device,
    PalImage* image,
    PalMemoryRequirements* requirements);

PalResult PAL_CALL bindVkImageMemory(
    PalDevice* device,
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
    PalAdapter* adapter,
    PalGraphicsWindow* window,
    PalSwapchainCapabilities* caps);

PalResult PAL_CALL createVkSwapchain(
    PalDevice* device,
    PalQueue* queue,
    PalGraphicsWindow* window,
    const PalSwapchainCreateInfo* info,
    PalSwapchain** outSwapchain);

void PAL_CALL destroyVkSwapchain(PalSwapchain* swapchain);

Uint32 PAL_CALL getVkSwapchainImageCount(PalSwapchain* swapchain);

PalImage* PAL_CALL getVkSwapchainImage(
    PalSwapchain* swapchain,
    Int32 index);

PalResult PAL_CALL createVkShader(
    PalDevice* device,
    const PalShaderCreateInfo* info,
    PalShader** outShader);

void PAL_CALL destroyVkShader(PalShader* shader);

PalShaderType PAL_CALL getVkShaderType(PalShader* shader);

PalResult PAL_CALL createVkRenderPass(
    PalDevice* device,
    const PalRenderPassCreateInfo* info,
    PalRenderPass** outRenderPass);

void PAL_CALL destroyVkRenderPass(PalRenderPass* renderPass);

PalResult PAL_CALL createVkCommandPool(
    PalDevice* device,
    const PalCommandPoolCreateInfo* info,
    PalCommandPool** outPool);

void PAL_CALL destroyVkCommandPool(PalCommandPool* pool);

static PalGraphicsBackend s_VkBackend = {
    .enumerateAdapters = enumerateVkAdapters,
    .getAdapterInfo =  getVkAdapterInfo,
    .getAdapterCapabilities =  getVkAdapterCapabilities,
    .createDevice =  createVkDevice,
    .destroyDevice =  destroyVkDevice,
    .allocateMemory =  allocateVkMemory,
    .freeMemory =  freeVkMemory,
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
    .getSwapchainImageCount =  getVkSwapchainImageCount,
    .getSwapchainImage =  getVkSwapchainImage,
    .createShader = createVkShader,
    .destroyShader = destroyVkShader,
    .getShaderType = getVkShaderType,
    .createRenderPass = createVkRenderPass,
    .destroyRenderPass = destroyVkRenderPass,
    .createCommandPool = createVkCommandPool,
    .destroyCommandPool = destroyVkCommandPool
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
    if (!backend->enumerateAdapters            || 
        !backend->getAdapterInfo               ||
        !backend->getAdapterCapabilities       ||
        !backend->createDevice                 ||
        !backend->destroyDevice                ||
        !backend->createQueue                  ||
        !backend->destroyQueue                 ||
        !backend->canQueuePresent              ||
        !backend->createImage                  ||
        !backend->destroyImage                 ||
        !backend->getImageInfo                 ||
        !backend->enumerateFormats             ||
        !backend->isFormatSupported            ||
        !backend->queryFormatImageUsages       ||
        !backend->queryFormatImageViewUsages   ||
        !backend->getImageMemoryRequirements   ||
        !backend->allocateMemory               ||
        !backend->freeMemory                   ||
        !backend->bindImageMemory              ||
        !backend->createImageView              ||
        !backend->destroyImageView             ||
        !backend->querySwapchainCapabilities   ||
        !backend->createSwapchain              ||
        !backend->destroySwapchain             ||
        !backend->getSwapchainImageCount       ||
        !backend->getSwapchainImage            ||
        !backend->createCommandPool            ||
        !backend->destroyCommandPool) {
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

    s_Graphics.maxHandleData = 32;
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
                Uint64 adapterIndex = 0;
                HandleData* data = getFreeHandleData(&adapterIndex);
                data->backend = backend->base;
                data->handle = adapters[i];

                // reset the adapter handle into our index generated handle
                PalAdapter* tmp = TO_PAL_HANDLE(PalAdapter, adapterIndex);
                adapters[i] = tmp;
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

    Uint64 index = FROM_PAL_HANDLE(adapter);
    HandleData* data = findHandleData(index);
    if (!data) {
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

    Uint64 index = FROM_PAL_HANDLE(adapter);
    HandleData* data = findHandleData(index);
    if (!data) {
        return PAL_RESULT_INVALID_ADAPTER;
    }
    return data->backend->getAdapterCapabilities(data->handle, caps);
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

    // check if the adapter is from PAL (custom or internal backend)
    Uint64 index = FROM_PAL_HANDLE(adapter);
    HandleData* data = findHandleData(index);
    if (!data) {
        return PAL_RESULT_INVALID_ADAPTER;
    }

    // create a slot for the device
    Uint64 deviceIndex = 0;
    HandleData* deviceData = getFreeHandleData(&deviceIndex);
    if (!deviceData) {
        return PAL_RESULT_OUT_OF_MEMORY;
    }

    PalDevice* device = nullptr;
    PalResult ret;
    ret = data->backend->createDevice(data->handle, features, &device);
    if (ret != PAL_RESULT_SUCCESS) {
        return ret;
    }
    
    deviceData->backend = data->backend;
    deviceData->handle = device;

    *outDevice = TO_PAL_HANDLE(PalDevice, deviceIndex);
    return PAL_RESULT_SUCCESS;
}

void PAL_CALL palDestroyDevice(PalDevice* device)
{
    if (s_Graphics.initialized && device) {
        Uint64 index = FROM_PAL_HANDLE(device);
        HandleData* data = findHandleData(index);
        if (data) {
            data->backend->destroyDevice(data->handle);
            data->used = false;
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

    Uint64 index = FROM_PAL_HANDLE(device);
    HandleData* deviceData = findHandleData(index);
    if (!deviceData) {
        return PAL_RESULT_INVALID_DEVICE;
    }
    return deviceData->backend->allocateMemory(
        deviceData->handle, 
        type, 
        size, 
        outMemory);
}

void PAL_CALL palFreeMemory(
    PalDevice* device,
    PalMemory* memory)
{
    if (s_Graphics.initialized && device && memory) {
        Uint64 index = FROM_PAL_HANDLE(device);
        HandleData* data = findHandleData(index);
        if (data) {
            data->backend->freeMemory(data->handle, memory);
        }
    }
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

    Uint64 index = FROM_PAL_HANDLE(device);
    HandleData* data = findHandleData(index);
    if (!data) {
        return PAL_RESULT_INVALID_DEVICE;
    }

    // create a slot for the queue
    Uint64 queueIndex = 0;
    HandleData* queueData = getFreeHandleData(&queueIndex);
    if (!queueData) {
        return PAL_RESULT_OUT_OF_MEMORY;
    }

    PalQueue* queue = nullptr;
    PalResult ret;
    ret = data->backend->createQueue(
        data->handle,
        type,
        &queue);

    if (ret != PAL_RESULT_SUCCESS) {
        return ret;
    }

    queueData->backend = data->backend;
    queueData->handle = queue;

    *outQueue = TO_PAL_HANDLE(PalQueue, queueIndex);
    return PAL_RESULT_SUCCESS;
}

void PAL_CALL palDestroyQueue(PalQueue* queue)
{
    if (s_Graphics.initialized && queue) {
        Uint64 index = FROM_PAL_HANDLE(queue);
        HandleData* data = findHandleData(index);
        if (data) {
            data->backend->destroyQueue(data->handle);
            data->used = false;
        }
    }
}

bool PAL_CALL palCanQueuePresent(
    PalQueue* queue, 
    PalGraphicsWindow* window)
{
    if (s_Graphics.initialized && queue) {
        Uint64 index = FROM_PAL_HANDLE(queue);
        HandleData* data = findHandleData(index);
        if (!data) {
            return false;
        }
        return data->backend->canQueuePresent(data->handle, window);
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

    Uint64 index = FROM_PAL_HANDLE(adapter);
    HandleData* data = findHandleData(index);
    if (!data) {
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

    Uint64 index = FROM_PAL_HANDLE(adapter);
    HandleData* data = findHandleData(index);
    if (!data) {
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

    Uint64 index = FROM_PAL_HANDLE(adapter);
    HandleData* data = findHandleData(index);
    if (!data) {
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

    Uint64 index = FROM_PAL_HANDLE(adapter);
    HandleData* data = findHandleData(index);
    if (!data) {
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

    Uint64 index = FROM_PAL_HANDLE(device);
    HandleData* data = findHandleData(index);
    if (!data) {
        return PAL_RESULT_INVALID_DEVICE;
    }

    // create a slot for the image
    Uint64 imageIndex = 0;
    HandleData* imageData = getFreeHandleData(&imageIndex);
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

    *outImage = TO_PAL_HANDLE(PalImage, imageIndex);
    return PAL_RESULT_SUCCESS;
}

void PAL_CALL palDestroyImage(PalImage* image)
{
    if (s_Graphics.initialized && image) {
        Uint64 index = FROM_PAL_HANDLE(image);
        HandleData* data = findHandleData(index);
        if (data) {
            data->backend->destroyImage(data->handle);
            data->used = false;
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

    Uint64 index = FROM_PAL_HANDLE(image);
    HandleData* data = findHandleData(index);
    if (!data) {
        return PAL_RESULT_INVALID_IMAGE;
    }
    return data->backend->getImageInfo(data->handle, info);
}

PalResult PAL_CALL palGetImageMemoryRequirements(
    PalDevice* device,
    PalImage* image,
    PalMemoryRequirements* requirements)
{
    if (!s_Graphics.initialized) {
        return PAL_RESULT_GRAPHICS_NOT_INITIALIZED;
    }

    if (!device || !image) {
        return PAL_RESULT_NULL_POINTER;
    }

    Uint64 index = FROM_PAL_HANDLE(device);
    HandleData* data = findHandleData(index);

    index = FROM_PAL_HANDLE(image);
    HandleData* imageData = findHandleData(index);
    if (!data) {
        return PAL_RESULT_INVALID_DEVICE;
    }

    if (!imageData) {
        return PAL_RESULT_INVALID_IMAGE;
    }

    return data->backend->getImageMemoryRequirements(
        data->handle, 
        imageData->handle, 
        requirements);
}

PalResult PAL_CALL palBindImageMemory(
    PalDevice* device,
    PalImage* image,
    PalMemory* memory,
    Uint64 offset)
{
    if (!s_Graphics.initialized) {
        return PAL_RESULT_GRAPHICS_NOT_INITIALIZED;
    }

    if (!device || !image || !memory) {
        return PAL_RESULT_NULL_POINTER;
    }

    Uint64 index = FROM_PAL_HANDLE(device);
    HandleData* data = findHandleData(index);

    index = FROM_PAL_HANDLE(image);
    HandleData* imageData = findHandleData(index);
    if (!data) {
        return PAL_RESULT_INVALID_DEVICE;
    }

    if (!imageData) {
        return PAL_RESULT_INVALID_IMAGE;
    }

    return data->backend->bindImageMemory(
        data->handle, 
        imageData->handle, 
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

    Uint64 index = FROM_PAL_HANDLE(device);
    HandleData* data = findHandleData(index);

    index = FROM_PAL_HANDLE(image);
    HandleData* imageData = findHandleData(index);
    if (!data) {
        return PAL_RESULT_INVALID_DEVICE;
    }

    if (!imageData) {
        return PAL_RESULT_INVALID_IMAGE;
    }

    // create a slot for the image view
    Uint64 imageViewIndex = 0;
    HandleData* imageViewData = getFreeHandleData(&imageViewIndex);
    if (!imageViewData) {
        return PAL_RESULT_OUT_OF_MEMORY;
    }

    PalImageView* imageView = nullptr;
    PalResult ret;
    ret = data->backend->createImageView(
        data->handle,
        imageData->handle,
        info,
        &imageView);

    if (ret != PAL_RESULT_SUCCESS) {
        return ret;
    }

    imageViewData->backend = data->backend;
    imageViewData->handle = imageView;

    *outImageView = TO_PAL_HANDLE(PalImageView, imageViewIndex);
    return PAL_RESULT_SUCCESS;
}

void PAL_CALL palDestroyImageView(PalImageView* imageView)
{
    if (s_Graphics.initialized && imageView) {
        Uint64 index = FROM_PAL_HANDLE(imageView);
        HandleData* data = findHandleData(index);
        if (data) {
            data->backend->destroyImageView(data->handle);
            data->used = false;
        }
    }
}

// ==================================================
// Swapchain
// ==================================================

PalResult PAL_CALL palQuerySwapchainCapabilities(
    PalAdapter* adapter,
    PalGraphicsWindow* window,
    PalSwapchainCapabilities* caps)
{
    if (!s_Graphics.initialized) {
        return PAL_RESULT_GRAPHICS_NOT_INITIALIZED;
    }

    if (!adapter || !window || !caps) {
        return PAL_RESULT_NULL_POINTER;
    }

    Uint64 index = FROM_PAL_HANDLE(adapter);
    HandleData* data = findHandleData(index);
    if (!data) {
        return PAL_RESULT_INVALID_ADAPTER;
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

    Uint64 index = FROM_PAL_HANDLE(device);
    HandleData* data = findHandleData(index);

    index = FROM_PAL_HANDLE(queue);
    HandleData* queueData = findHandleData(index);
    if (!data) {
        return PAL_RESULT_INVALID_DEVICE;
    }

    if (!queueData) {
        return PAL_RESULT_INVALID_QUEUE;
    }

    // create a slot for the swapchain
    Uint64 swapchainIndex = 0;
    HandleData* swapchainData = getFreeHandleData(&swapchainIndex);
    if (!swapchainData) {
        return PAL_RESULT_OUT_OF_MEMORY;
    }

    PalResult ret;
    PalSwapchain* swapchain = nullptr;
    ret = data->backend->createSwapchain(
        data->handle, 
        queueData->handle, 
        window, 
        info, 
        &swapchain);

    if (ret != PAL_RESULT_SUCCESS) {
        return ret;
    }
    
    swapchainData->backend = data->backend;
    swapchainData->handle = swapchain;

    *outSwapchain = TO_PAL_HANDLE(PalSwapchain, swapchainIndex);
    return PAL_RESULT_SUCCESS;
}

void PAL_CALL palDestroySwapchain(PalSwapchain* swapchain)
{
    if (s_Graphics.initialized && swapchain) {
        Uint64 index = FROM_PAL_HANDLE(swapchain);
        HandleData* data = findHandleData(index);
        if (data) {
            data->backend->destroySwapchain(data->handle);
            data->used = false;
        }
    }
}

Uint32 PAL_CALL palGetSwapchainImageCount(PalSwapchain* swapchain)
{
    if (s_Graphics.initialized && swapchain) {
        Uint64 index = FROM_PAL_HANDLE(swapchain);
        HandleData* data = findHandleData(index);
        if (data) {
            return data->backend->getSwapchainImageCount(data->handle);
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

    Uint64 swapchainIndex = FROM_PAL_HANDLE(swapchain);
    HandleData* data = findHandleData(swapchainIndex);
    if (!data) {
        return nullptr;
    }

    PalImage* image = data->backend->getSwapchainImage(
        data->handle, 
        index);

    // check if the user has already requested for the image
    Uint64 imageIndex = FROM_PAL_HANDLE(image);
    HandleData* imageData = findHandleData(imageIndex);
    if (!imageData) {
        // create a new slot
        imageData = getFreeHandleData(&imageIndex);
        if (!imageData) {
            return nullptr;
        }
    }

    imageData->backend = data->backend;
    imageData->handle = image;

    return TO_PAL_HANDLE(PalImage, imageIndex);
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

    if (info->type == PAL_SHADER_TYPE_UNDEFINED) {
        return PAL_RESULT_INVALID_SHADER_TYPE;
    }

    Uint64 index = FROM_PAL_HANDLE(device);
    HandleData* data = findHandleData(index);
    if (!data) {
        return PAL_RESULT_INVALID_DEVICE;
    }

    // create a slot for the shader
    Uint64 shaderIndex = 0;
    HandleData* shaderData = getFreeHandleData(&shaderIndex);
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

    *outShader = TO_PAL_HANDLE(PalShader, shaderIndex);
    return PAL_RESULT_SUCCESS;
}

void PAL_CALL palDestroyShader(PalShader* shader)
{
    if (s_Graphics.initialized && shader) {
        Uint64 index = FROM_PAL_HANDLE(shader);
        HandleData* data = findHandleData(index);
        if (data) {
            data->backend->destroyShader(data->handle);
            data->used = false;
        }
    }
}

PalShaderType PAL_CALL palGetShaderType(PalShader* shader)
{
    if (s_Graphics.initialized && shader) {
        Uint64 index = FROM_PAL_HANDLE(shader);
        HandleData* data = findHandleData(index);
        if (data) {
            return data->backend->getShaderType(data->handle);
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

    Uint64 index = FROM_PAL_HANDLE(device);
    HandleData* data = findHandleData(index);
    if (!data) {
        return PAL_RESULT_INVALID_DEVICE;
    }

    PalAttachmentDesc attachments[16]; // should be fine
    PalRenderPassCreateInfo createInfo = {0};
    createInfo.attachmentCount = info->attachmentCount;
    createInfo.attachments = attachments;

    for (int i = 0; i < info->attachmentCount; i++) {
        if (!info->attachments[i].target) {
            return PAL_RESULT_NULL_POINTER;
        }

        index = FROM_PAL_HANDLE(info->attachments[i].target);
        HandleData* tmp = findHandleData(index);
        attachments[i].target = tmp->handle;
        attachments[i].loadOp = info->attachments[i].loadOp;
        attachments[i].storeOp = info->attachments[i].storeOp;
        attachments[i].type = info->attachments[i].type;
        attachments[i].resolveTarget = nullptr;

        if (info->attachments[i].resolveTarget) {
            index = FROM_PAL_HANDLE(info->attachments[i].target);
            tmp = findHandleData(index);
            attachments[i].resolveTarget = tmp->handle;
        }
    }

    // create a slot for the renderpass
    Uint64 renderPassIndex = 0;
    HandleData* renderPassData = getFreeHandleData(&renderPassIndex);
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

    *outRenderPass = TO_PAL_HANDLE(PalRenderPass, renderPassIndex);
    return PAL_RESULT_SUCCESS;
}

void PAL_CALL palDestroyRenderPass(PalRenderPass* renderPass)
{
    if (s_Graphics.initialized && renderPass) {
        Uint64 index = FROM_PAL_HANDLE(renderPass);
        HandleData* data = findHandleData(index);
        if (data) {
            data->backend->destroyRenderPass(data->handle);
            data->used = false;
        }
    }
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

    Uint64 index = FROM_PAL_HANDLE(device);
    HandleData* data = findHandleData(index);
    if (!data) {
        return PAL_RESULT_INVALID_DEVICE;
    }

    // create a slot for the command pool
    Uint64 poolIndex = 0;
    HandleData* poolData = getFreeHandleData(&poolIndex);
    if (!poolData) {
        return PAL_RESULT_OUT_OF_MEMORY;
    }

    PalCommandPool* pool = nullptr;
    PalResult ret;
    ret = data->backend->createCommandPool(
        data->handle,
        info,
        &pool);

    if (ret != PAL_RESULT_SUCCESS) {
        return ret;
    }

    poolData->backend = data->backend;
    poolData->handle = pool;

    *outPool = TO_PAL_HANDLE(PalCommandPool, poolIndex);
    return PAL_RESULT_SUCCESS;
}

void PAL_CALL palDestroyCommandPool(PalCommandPool* pool)
{
    if (s_Graphics.initialized && pool) {
        Uint64 index = FROM_PAL_HANDLE(pool);
        HandleData* data = findHandleData(index);
        if (data) {
            data->backend->destroyCommandPool(data->handle);
            data->used = false;
        }
    }
}