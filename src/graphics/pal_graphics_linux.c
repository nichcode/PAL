
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

#if PAL_HAS_VULKAN
#include <vulkan/vulkan_core.h>
#include <stdlib.h>
#include <dlfcn.h>
#endif // PAL_HAS_VULKAN

// ==================================================
// Typedefs, enums and structs
// ==================================================

#define PAL_MAX_BACKENDS 8 // should be fine for now

#if PAL_HAS_VULKAN
// VKAPI_PTR expands to nothing on linux

typedef VkResult (*vkCreateInstanceFn)(
    const VkInstanceCreateInfo*, 
    const VkAllocationCallbacks*, 
    VkInstance*);

typedef void (*vkDestroyInstanceFn)(
    VkInstance, 
    const VkAllocationCallbacks*);

typedef VkResult (*vkEnumeratePhysicalDevicesFn)(
    VkInstance, 
    uint32_t*, 
    VkPhysicalDevice*);

#endif // PAL_HAS_VULKAN

typedef struct {
    bool initialized;
    Int32 backendCount;
    const PalAllocator* allocator;
    void* instance;
    void* handle;

    void* destroyInstance;
    void* createInstance;
    void* enumeratePhysicalDevices;

    const PalGPUBackend* backends[PAL_MAX_BACKENDS];
} VkGPU;

static VkGPU s_VkGPU = {0};

// ==================================================
// Internal API
// ==================================================

#if PAL_HAS_VULKAN

// we dont want to fill this everytime we want to use
static VkAllocationCallbacks s_VkAllocator = {0};

void* vkAlloc(
    void* pUserData,
    size_t size,
    size_t alignment,
    VkSystemAllocationScope allocationScope)
{
    return palAllocate(s_VkGPU.allocator, size, alignment);
}

void vkFree(
    void* pUserData,
    void* ptr)
{
    palFree(s_VkGPU.allocator, ptr);
}

void* vkRealloc(
    void* pUserData,
    void* pOriginal,
    size_t size,
    size_t alignment,
    VkSystemAllocationScope allocationScope)
{
    // Note: This is a hack which could cost performance but 
    // realloc is not really called that much so it should be fine
    // this is because we dont know the old size
    void* block = realloc(pOriginal, size);
    if (block) {
        void* memory = palAllocate(s_VkGPU.allocator, size, alignment);
        if (!memory) {
            free(block);
            return nullptr;
        }

        memcpy(memory, block, size);
        return memory;
    }
    return nullptr;
}

PalResult vkInitGraphics()
{
    // load vulkan
    s_VkGPU.handle = dlopen("libvulkan.so", RTLD_LAZY);
    if (!s_VkGPU.handle) {
        return PAL_RESULT_PLATFORM_FAILURE;
    }

    s_VkGPU.createInstance = dlsym(s_VkGPU.handle, "vkCreateInstance");
    s_VkGPU.destroyInstance = dlsym(s_VkGPU.handle, "vkDestroyInstance");

    s_VkGPU.enumeratePhysicalDevices = dlsym(
        s_VkGPU.handle, 
        "vkEnumeratePhysicalDevices");

    // create a dummy instance
    VkApplicationInfo appInfo = {0};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.apiVersion = VK_API_VERSION_1_0; // for wider support
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "Engine";
    appInfo.pApplicationName = "App";
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);

    VkInstanceCreateInfo instanceCreateInfo = {0};
    instanceCreateInfo.pApplicationInfo = &appInfo;
    instanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;

    // vk allocator
    s_VkAllocator.pfnAllocation = vkAlloc;
    s_VkAllocator.pfnFree = vkFree;
    s_VkAllocator.pfnReallocation = vkRealloc;

    VkInstance instance = nullptr;
    vkCreateInstanceFn vkCreateInstancePtr = s_VkGPU.createInstance;
    VkResult result = vkCreateInstancePtr(
        &instanceCreateInfo, 
        &s_VkAllocator, 
        &instance);

    if (result != VK_SUCCESS) {
        return PAL_RESULT_PLATFORM_FAILURE;
    }

    s_VkGPU.instance = instance;
    return PAL_RESULT_SUCCESS;
}

void vkShutdownGraphics()
{
    if (s_VkGPU.instance) {
        vkDestroyInstanceFn vkDestroyInstancePtr = s_VkGPU.destroyInstance;
        vkDestroyInstancePtr(s_VkGPU.instance, &s_VkAllocator);
    }
}

PalResult vkEnumerateAdapters(
    Int32* count, 
    PalGPUAdapter** outAdapters)
{
    palLog(nullptr, "Vulkan GPU");
    return PAL_RESULT_SUCCESS;
}

static PalGPUBackend s_VkBackend = {
    .enumerateAdapters = vkEnumerateAdapters
};

#endif // PAL_HAS_VULKAN

// ==================================================
// Public API
// ==================================================

PalResult PAL_CALL palInitGraphics(const PalAllocator* allocator)
{
    if (s_VkGPU.initialized) {
        return PAL_RESULT_SUCCESS;
    }

    if (allocator && (!allocator->allocate || !allocator->free)) {
        return PAL_RESULT_INVALID_ALLOCATOR;
    }

    s_VkGPU.allocator = allocator;
#if PAL_HAS_VULKAN
    PalResult ret = vkInitGraphics();
    if (ret != PAL_RESULT_SUCCESS) {
        return ret;
    }
    s_VkGPU.backends[s_VkGPU.backendCount++] = &s_VkBackend;
#endif // PAL_HAS_VULKAN

    s_VkGPU.initialized = true;
    return PAL_RESULT_SUCCESS;
}

void PAL_CALL palShutdownGraphics()
{
    if (!s_VkGPU.initialized) {
        return;
    }

#if PAL_HAS_VULKAN
    vkShutdownGraphics();
#endif // PAL_HAS_VULKAN

    memset(&s_VkGPU, 0, sizeof(VkGPU));
    s_VkGPU.initialized = false; // just in case
}