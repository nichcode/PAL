
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
#include <string.h>
#include <stdio.h>

// HACK: Needed to determine display type
struct wl_display;
struct wl_surface;
typedef struct _XDisplay Display;
typedef unsigned long Window;
typedef unsigned long VisualID;
typedef int (*wl_display_get_fd_fn)(struct wl_display*);

#include <vulkan/vulkan_xlib.h>
#include <vulkan/vulkan_wayland.h>

#endif // PAL_HAS_VULKAN

// ==================================================
// Typedefs, enums and structs
// ==================================================

#define MAX_BACKENDS 32

#if PAL_HAS_VULKAN

typedef struct {
    bool hasDebug;
    bool hasDynamicRendering;
    void* handle;
    VkInstance instance;
    
    // HACK: for display testing
    void* libWayland;
    wl_display_get_fd_fn getDisplayFd;
    
    PFN_vkEnumerateInstanceVersion enumerateInstanceVersion;
    PFN_vkEnumerateInstanceExtensionProperties enumerateInstanceExtensionProperties;
    PFN_vkDestroyInstance destroyInstance;
    PFN_vkCreateInstance createInstance;
    PFN_vkEnumeratePhysicalDevices enumeratePhysicalDevices;
    PFN_vkGetPhysicalDeviceProperties getPhysicalDeviceProperties;
    PFN_vkGetPhysicalDeviceMemoryProperties getPhysicalDeviceMemoryProperties;
    PFN_vkEnumerateInstanceLayerProperties enumerateInstanceLayerProperties;
    PFN_vkGetPhysicalDeviceQueueFamilyProperties getPhysicalDeviceQueueFamilyProperties;
    PFN_vkEnumerateDeviceExtensionProperties enumerateDeviceExtensionProperties;
    PFN_vkGetPhysicalDeviceFeatures getPhysicalDeviceFeatures;
    PFN_vkGetPhysicalDeviceFeatures2 getPhysicalDeviceFeatures2;
    PFN_vkGetPhysicalDeviceFeatures2KHR getPhysicalDeviceFeatures2KHR;
    PFN_vkGetInstanceProcAddr getInstanceProcAddr;
    PFN_vkCreateImageView createImageView;
    PFN_vkDestroyImageView destroyImageView;
    PFN_vkGetPhysicalDeviceProperties2 getPhysicalDeviceProperties2;
    PFN_vkGetPhysicalDeviceFormatProperties getPhysicalDeviceFormatProperties;
    PFN_vkGetImageMemoryRequirements getImageMemoryRequirements;
    PFN_vkAllocateMemory allocateMemory;
    PFN_vkFreeMemory freeMemory;
    PFN_vkBindImageMemory bindImageMemory;
    
    PFN_vkCreateDevice createDevice;
    PFN_vkDestroyDevice destroyDevice;
    PFN_vkGetDeviceQueue getDeviceQueue;
    PFN_vkGetDeviceProcAddr getDeviceProcAddr;
    PFN_vkCreateImage createImage;
    PFN_vkDestroyImage destroyImage;

    PFN_vkCreateWaylandSurfaceKHR createWaylandSurface;
    PFN_vkGetPhysicalDeviceWaylandPresentationSupportKHR checkWaylandPresentSupport;
    PFN_vkCreateXlibSurfaceKHR createXlibSurface;
    PFN_vkGetPhysicalDeviceXlibPresentationSupportKHR checkXlibPresentSupport;

    PFN_vkDestroySurfaceKHR destroySurface;
    PFN_vkGetPhysicalDeviceSurfaceCapabilitiesKHR getSurfaceCapabilities;
    PFN_vkGetPhysicalDeviceSurfaceFormatsKHR getSurfaceFormats;
    PFN_vkGetPhysicalDeviceSurfacePresentModesKHR getSurfacePresentModes;

    VkAllocationCallbacks allocator;
} Vulkan;

typedef struct {
    Int32 familyIndex;
    VkPhysicalDevice phyDevice;
    VkQueue handle;
    VkQueueFlags usages;
    VkQueueFlags usedUsages;
} PhysicalQueue;

typedef struct {
    Int32 queueCount;
    VkPhysicalDevice phyDevice;
    VkDevice handle;
    PhysicalQueue* phyQueues;
    PFN_vkCreateSwapchainKHR createSwapchain;
    PFN_vkDestroySwapchainKHR destroySwapchain;
    PFN_vkGetSwapchainImagesKHR getSwapchainImages;
    PFN_vkAcquireNextImageKHR acquireNextImage;
    PFN_vkQueuePresentKHR queuePresent;
    Int32 memoryTypeIndex[PAL_MEMORY_TYPE_MAX];
} Device;

typedef struct {
    VkQueueFlags usage;
    Device* device;
    PhysicalQueue* phyQueue;
} Queue;

typedef struct {
    bool belongsToSwapchain;
    Device* device;
    VkImage handle;
    PalImageInfo info;
} Image;

typedef struct {
    VkImageViewType type;
    PalImageViewUsages usages;
    Device* device;
    Image* image;
    VkImageView handle;
} ImageView;

typedef struct {
    Uint32 imageCount;
    Device* device;
    VkSurfaceKHR surface;
    VkSwapchainKHR handle;
    Image* images;
} Swapchain;

static Vulkan s_Vk = {0};

#endif // PAL_HAS_VULKAN

typedef struct {
    bool used;
    void* handle;
    const PalGPUBackend* backend;
} HandleData;

typedef struct {
    Int32 count;
    Int32 startIndex;
    const PalGPUBackend* base;
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

// FIXME: might be replaced with hashmap for performance
static HandleData* getFreeHandleData()
{
    for (int i = 0; i < s_Graphics.maxHandleData; ++i) {
        if (!s_Graphics.handleData[i].used) {
            s_Graphics.handleData[i].used = true;
            return &s_Graphics.handleData[i];
        }
    }  

    // resize the data array
    HandleData* data = nullptr;
    int count = s_Graphics.maxHandleData * 2; // double the size
    int freeIndex = s_Graphics.maxHandleData + 1;
    data = palAllocate(s_Graphics.allocator, sizeof(HandleData) * count, 0);
    if (data) {
        memcpy(
            data,
            s_Graphics.handleData,
            s_Graphics.maxHandleData * sizeof(HandleData));

        palFree(s_Graphics.allocator, s_Graphics.handleData);
        s_Graphics.handleData = data;
        s_Graphics.maxHandleData = count;

        s_Graphics.handleData[freeIndex].used = true;
        return &s_Graphics.handleData[freeIndex];
    }
    return nullptr;
}

static HandleData* findHandleData(void* handle)
{
    for (int i = 0; i < s_Graphics.maxHandleData; ++i) {
        if (s_Graphics.handleData[i].used &&
            s_Graphics.handleData[i].handle == handle) {
            return &s_Graphics.handleData[i];
        }
    }
    return nullptr;
}

#if PAL_HAS_VULKAN

static bool vkOnWayland(struct wl_display* display) 
{
    if (!s_Vk.libWayland) {
        return false;
    }

    int fd = s_Vk.getDisplayFd(display);
    if (fd <= 0 || fd > 1024) { // fds are usaually 0-30 but this is fine
        return false;
    }
    return true;
}

static bool vkCreateSurface(
    PalGfxWindow* window, 
    VkSurfaceKHR* outSurface)
{
    if (vkOnWayland(window->display)) {
        if (!s_Vk.createWaylandSurface) {
            return false;
        }

        VkSurfaceKHR surface = nullptr;
        VkWaylandSurfaceCreateInfoKHR createInfo = {0};
        createInfo.display = window->display;
        createInfo.pNext = nullptr;
        createInfo.flags = 0;
        createInfo.sType = VK_STRUCTURE_TYPE_WAYLAND_SURFACE_CREATE_INFO_KHR;
        createInfo.surface = window->window;

        VkResult result = s_Vk.createWaylandSurface(
            s_Vk.instance, 
            &createInfo, 
            &s_Vk.allocator, 
            &surface);

        if (result != VK_SUCCESS) {
            return false;
        }
        *outSurface = surface;

    } else {
        // TODO: create surface for xlib
    }
    return true;
}

static PalResult vkResultToPal(VkResult result) 
{
    switch (result) {
        case VK_ERROR_FEATURE_NOT_PRESENT:
        case VK_ERROR_EXTENSION_NOT_PRESENT: {
            return PAL_RESULT_ADAPTER_FEATURE_NOT_SUPPORTED;
        }

        case VK_ERROR_OUT_OF_HOST_MEMORY:
        case VK_ERROR_TOO_MANY_OBJECTS:
        case VK_ERROR_OUT_OF_DEVICE_MEMORY: {
            return PAL_RESULT_OUT_OF_MEMORY;
        }

        case VK_ERROR_INITIALIZATION_FAILED: 
        case VK_ERROR_DEVICE_LOST: {
            return PAL_RESULT_PLATFORM_FAILURE;
        }

        case VK_ERROR_INCOMPATIBLE_DRIVER:
            return PAL_RESULT_INVALID_GRAPHICS_DRIVER;

        case VK_ERROR_NATIVE_WINDOW_IN_USE_KHR:
        case VK_ERROR_SURFACE_LOST_KHR: {
            return PAL_RESULT_INVALID_WINDOW;
        }

        case VK_TIMEOUT:
            return PAL_RESULT_TIMEOUT;

        default:
            return PAL_RESULT_PLATFORM_FAILURE;
    }
    return PAL_RESULT_PLATFORM_FAILURE;
}

static VkImageUsageFlags palUsageToVk(PalImageUsages usages) 
{
    VkImageUsageFlags flags = 0;
    if (usages & PAL_IMAGE_USAGE_COLOR_ATTACHEMENT) {
        flags |= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    }

    if (usages & PAL_IMAGE_USAGE_DEPTH_ATTACHEMENT) {
        flags |= VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
    }

    if (usages & PAL_IMAGE_USAGE_TRANSFER_SRC) {
        flags |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
    }

    if (usages & PAL_IMAGE_USAGE_TRANSFER_DST) {
        flags |= VK_IMAGE_USAGE_TRANSFER_DST_BIT;
    }

    if (usages & PAL_IMAGE_USAGE_STORAGE) {
        flags |= VK_IMAGE_USAGE_STORAGE_BIT;
    }
    
    if (usages & PAL_IMAGE_USAGE_SAMPLED) {
        flags |= VK_IMAGE_USAGE_SAMPLED_BIT;
    }

    return flags;
}

static VkFormat palFormatToVk(PalFormat format) 
{
    switch (format) {
        case PAL_FORMAT_R8_UNORM:
            return VK_FORMAT_R8_UNORM;

        case PAL_FORMAT_R8_SNORM:
            return VK_FORMAT_R8_SNORM;

        case PAL_FORMAT_R8_UINT:
            return VK_FORMAT_R8_UINT;

        case PAL_FORMAT_R8_SINT:
            return VK_FORMAT_R8_SINT;

        case PAL_FORMAT_R8_SRGB:
            return VK_FORMAT_R8_SRGB;

        case PAL_FORMAT_R16_UNORM:
            return VK_FORMAT_R16_UNORM;

        case PAL_FORMAT_R16_SNORM:
            return VK_FORMAT_R16_SNORM;

        case PAL_FORMAT_R16_UINT:
            return VK_FORMAT_R16_UINT;

        case PAL_FORMAT_R16_SINT:
            return VK_FORMAT_R16_SINT;

        case PAL_FORMAT_R16_SFLOAT:
            return VK_FORMAT_R16_SFLOAT;

        case PAL_FORMAT_R32_UINT:
            return VK_FORMAT_R32_UINT;

        case PAL_FORMAT_R32_SINT:
            return VK_FORMAT_R32_SINT;

        case PAL_FORMAT_R32_SFLOAT:
            return VK_FORMAT_R32_SFLOAT;

        case PAL_FORMAT_R64_UINT:
            return VK_FORMAT_R64_UINT;

        case PAL_FORMAT_R64_SINT:
            return VK_FORMAT_R64_SINT;

        case PAL_FORMAT_R64_SFLOAT:
            return VK_FORMAT_R64_SFLOAT;

        case PAL_FORMAT_R8G8_UNORM:
            return VK_FORMAT_R8G8_UNORM;

        case PAL_FORMAT_R8G8_SNORM:
            return VK_FORMAT_R8G8_SNORM;

        case PAL_FORMAT_R8G8_UINT:
            return VK_FORMAT_R8G8_UINT;

        case PAL_FORMAT_R8G8_SINT:
            return VK_FORMAT_R8G8_SINT;

        case PAL_FORMAT_R8G8_SRGB:
            return VK_FORMAT_R8G8_SRGB;

        case PAL_FORMAT_R16G16_UNORM:
            return VK_FORMAT_R16G16_UNORM;

        case PAL_FORMAT_R16G16_SNORM:
            return VK_FORMAT_R16G16_SNORM;

        case PAL_FORMAT_R16G16_UINT:
            return VK_FORMAT_R16G16_UINT;

        case PAL_FORMAT_R16G16_SINT:
            return VK_FORMAT_R16G16_SINT;

        case PAL_FORMAT_R16G16_SFLOAT:
            return VK_FORMAT_R16G16_SFLOAT;

        case PAL_FORMAT_R32G32_UINT:
            return VK_FORMAT_R32G32_UINT;

        case PAL_FORMAT_R32G32_SINT:
            return VK_FORMAT_R32G32_SINT;

        case PAL_FORMAT_R32G32_SFLOAT:
            return VK_FORMAT_R32G32_SFLOAT;

        case PAL_FORMAT_R64G64_UINT:
            return VK_FORMAT_R64G64_UINT;

        case PAL_FORMAT_R64G64_SINT:
            return VK_FORMAT_R64G64_SINT;

        case PAL_FORMAT_R64G64_SFLOAT:
            return VK_FORMAT_R64G64_SFLOAT;

        case PAL_FORMAT_R8G8B8_UNORM:
            return VK_FORMAT_R8G8B8_UNORM;

        case PAL_FORMAT_R8G8B8_SNORM:
            return VK_FORMAT_R8G8B8_SNORM;

        case PAL_FORMAT_R8G8B8_UINT:
            return VK_FORMAT_R8G8B8_UINT;

        case PAL_FORMAT_R8G8B8_SINT:
            return VK_FORMAT_R8G8B8_SINT;

        case PAL_FORMAT_R8G8B8_SRGB:
            return VK_FORMAT_R8G8B8_SRGB;

        case PAL_FORMAT_R16G16B16_UNORM:
            return VK_FORMAT_R16G16B16_UNORM;

        case PAL_FORMAT_R16G16B16_SNORM:
            return VK_FORMAT_R16G16B16_SNORM;

        case PAL_FORMAT_R16G16B16_UINT:
            return VK_FORMAT_R16G16B16_UINT;

        case PAL_FORMAT_R16G16B16_SINT:
            return VK_FORMAT_R16G16B16_SINT;

        case PAL_FORMAT_R16G16B16_SFLOAT:
            return VK_FORMAT_R16G16B16_SFLOAT;

        case PAL_FORMAT_R32G32B32_UINT:
            return VK_FORMAT_R32G32B32_UINT;

        case PAL_FORMAT_R32G32B32_SINT:
            return VK_FORMAT_R32G32B32_SINT;

        case PAL_FORMAT_R32G32B32_SFLOAT:
            return VK_FORMAT_R32G32B32_SFLOAT;

        case PAL_FORMAT_R64G64B64_UINT:
            return VK_FORMAT_R64G64B64_UINT;

        case PAL_FORMAT_R64G64B64_SINT:
            return VK_FORMAT_R64G64B64_SINT;

        case PAL_FORMAT_R64G64B64_SFLOAT:
            return VK_FORMAT_R64G64B64_SFLOAT;

        case PAL_FORMAT_B8G8R8_UNORM:
            return VK_FORMAT_B8G8R8_UNORM;

        case PAL_FORMAT_B8G8R8_SNORM:
            return VK_FORMAT_B8G8R8_SNORM;

        case PAL_FORMAT_B8G8R8_UINT:
            return VK_FORMAT_B8G8R8_UINT;

        case PAL_FORMAT_B8G8R8_SINT:
            return VK_FORMAT_B8G8R8_SINT;

        case PAL_FORMAT_B8G8R8_SRGB:
            return VK_FORMAT_B8G8R8_SRGB;

        case PAL_FORMAT_R8G8B8A8_UNORM:
            return VK_FORMAT_R8G8B8A8_UNORM;

        case PAL_FORMAT_R8G8B8A8_SNORM:
            return VK_FORMAT_R8G8B8A8_SNORM;

        case PAL_FORMAT_R8G8B8A8_UINT:
            return VK_FORMAT_R8G8B8A8_UINT;

        case PAL_FORMAT_R8G8B8A8_SINT:
            return VK_FORMAT_R8G8B8A8_SINT;

        case PAL_FORMAT_R8G8B8A8_SRGB:
            return VK_FORMAT_R8G8B8A8_SRGB;

        case PAL_FORMAT_R16G16B16A16_UNORM:
            return VK_FORMAT_R16G16B16A16_UNORM;

        case PAL_FORMAT_R16G16B16A16_SNORM:
            return VK_FORMAT_R16G16B16A16_SNORM;

        case PAL_FORMAT_R16G16B16A16_UINT:
            return VK_FORMAT_R16G16B16A16_UINT;

        case PAL_FORMAT_R16G16B16A16_SINT:
            return VK_FORMAT_R16G16B16A16_SINT;

        case PAL_FORMAT_R16G16B16A16_SFLOAT:
            return VK_FORMAT_R16G16B16A16_SFLOAT;

        case PAL_FORMAT_R32G32B32A32_UINT:
            return VK_FORMAT_R32G32B32A32_UINT;

        case PAL_FORMAT_R32G32B32A32_SINT:
            return VK_FORMAT_R32G32B32A32_SINT;

        case PAL_FORMAT_R32G32B32A32_SFLOAT:
            return VK_FORMAT_R32G32B32A32_SFLOAT;

        case PAL_FORMAT_R64G64B64A64_UINT:
            return VK_FORMAT_R64G64B64A64_UINT;

        case PAL_FORMAT_R64G64B64A64_SINT:
            return VK_FORMAT_R64G64B64A64_SINT;

        case PAL_FORMAT_R64G64B64A64_SFLOAT:
            return VK_FORMAT_R64G64B64A64_SFLOAT;

        case PAL_FORMAT_B8G8R8A8_UNORM:
            return VK_FORMAT_B8G8R8A8_UNORM;

        case PAL_FORMAT_B8G8R8A8_SNORM:
            return VK_FORMAT_B8G8R8A8_SNORM;

        case PAL_FORMAT_B8G8R8A8_UINT:
            return VK_FORMAT_B8G8R8A8_UINT;

        case PAL_FORMAT_B8G8R8A8_SINT:
            return VK_FORMAT_B8G8R8A8_SINT;

        case PAL_FORMAT_B8G8R8A8_SRGB:
            return VK_FORMAT_B8G8R8A8_SRGB;

        case PAL_FORMAT_S8_UINT:
            return VK_FORMAT_S8_UINT;

        case PAL_FORMAT_D16_UNORM:
            return VK_FORMAT_D16_UNORM;

        case PAL_FORMAT_D32_SFLOAT:
            return VK_FORMAT_D32_SFLOAT;

        case PAL_FORMAT_D32_SFLOAT_S8_UINT:
            return VK_FORMAT_D32_SFLOAT_S8_UINT;

        case PAL_FORMAT_D16_UNORM_S8_UINT:
            return VK_FORMAT_D16_UNORM_S8_UINT;

        case PAL_FORMAT_D24_UNORM_S8_UINT:
            return VK_FORMAT_D24_UNORM_S8_UINT;
    }

    return VK_FORMAT_UNDEFINED;
}

static VkSampleCountFlags samplesToVk(Uint32 samples)
{
    switch (samples) {
        case 2:
            return VK_SAMPLE_COUNT_2_BIT;
        
        case 4:
            return VK_SAMPLE_COUNT_4_BIT;

        case 8:
            return VK_SAMPLE_COUNT_8_BIT;

        case 16:
            return VK_SAMPLE_COUNT_16_BIT;

        case 32:
            return VK_SAMPLE_COUNT_32_BIT;

        case 64:
            return VK_SAMPLE_COUNT_64_BIT;
    }

    return VK_SAMPLE_COUNT_1_BIT;
}

static Uint32 vkSamplesToSamples(VkSampleCountFlags samples)
{
    if (samples & VK_SAMPLE_COUNT_2_BIT) {
        return 2;

    } else if (samples & VK_SAMPLE_COUNT_4_BIT) {
        return 4;

    } else if (samples & VK_SAMPLE_COUNT_8_BIT) {
        return 8;

    } else if (samples & VK_SAMPLE_COUNT_16_BIT) {
        return 16;

    } else if (samples & VK_SAMPLE_COUNT_32_BIT) {
        return 32;

    } else if (samples & VK_SAMPLE_COUNT_64_BIT) {
        return 64;
    }

    return 1;
}

static PalFormat vkFormatToPal(VkFormat format)
{
    switch (format) {
        case VK_FORMAT_R8_UNORM:
            return PAL_FORMAT_R8_UNORM;

        case VK_FORMAT_R8_SNORM:
            return PAL_FORMAT_R8_SNORM;

        case VK_FORMAT_R8_UINT:
            return PAL_FORMAT_R8_UINT;

        case VK_FORMAT_R8_SINT:
            return PAL_FORMAT_R8_SINT;

        case VK_FORMAT_R8_SRGB:
            return PAL_FORMAT_R8_SRGB;

        case VK_FORMAT_R16_UNORM:
            return PAL_FORMAT_R16_UNORM;

        case VK_FORMAT_R16_SNORM:
            return PAL_FORMAT_R16_SNORM;

        case VK_FORMAT_R16_UINT:
            return PAL_FORMAT_R16_UINT;

        case VK_FORMAT_R16_SINT:
            return PAL_FORMAT_R16_SINT;

        case VK_FORMAT_R16_SFLOAT:
            return PAL_FORMAT_R16_SFLOAT;

        case VK_FORMAT_R32_UINT:
            return PAL_FORMAT_R32_UINT;

        case VK_FORMAT_R32_SINT:
            return PAL_FORMAT_R32_SINT;

        case VK_FORMAT_R32_SFLOAT:
            return PAL_FORMAT_R32_SFLOAT;

        case VK_FORMAT_R64_UINT:
            return PAL_FORMAT_R64_UINT;

        case VK_FORMAT_R64_SINT:
            return PAL_FORMAT_R64_SINT;

        case VK_FORMAT_R64_SFLOAT:
            return PAL_FORMAT_R64_SFLOAT;

        case VK_FORMAT_R8G8_UNORM:
            return PAL_FORMAT_R8G8_UNORM;

        case VK_FORMAT_R8G8_SNORM:
            return PAL_FORMAT_R8G8_SNORM;

        case VK_FORMAT_R8G8_UINT:
            return PAL_FORMAT_R8G8_UINT;

        case VK_FORMAT_R8G8_SINT:
            return PAL_FORMAT_R8G8_SINT;

        case VK_FORMAT_R8G8_SRGB:
            return PAL_FORMAT_R8G8_SRGB;

        case VK_FORMAT_R16G16_UNORM:
            return PAL_FORMAT_R16G16_UNORM;

        case VK_FORMAT_R16G16_SNORM:
            return PAL_FORMAT_R16G16_SNORM;

        case VK_FORMAT_R16G16_UINT:
            return PAL_FORMAT_R16G16_UINT;

        case VK_FORMAT_R16G16_SINT:
            return PAL_FORMAT_R16G16_SINT;

        case VK_FORMAT_R16G16_SFLOAT:
            return PAL_FORMAT_R16G16_SFLOAT;

        case VK_FORMAT_R32G32_UINT:
            return PAL_FORMAT_R32G32_UINT;

        case VK_FORMAT_R32G32_SINT:
            return PAL_FORMAT_R32G32_SINT;

        case VK_FORMAT_R32G32_SFLOAT:
            return PAL_FORMAT_R32G32_SFLOAT;

        case VK_FORMAT_R64G64_UINT:
            return PAL_FORMAT_R64G64_UINT;

        case VK_FORMAT_R64G64_SINT:
            return PAL_FORMAT_R64G64_SINT;

        case VK_FORMAT_R64G64_SFLOAT:
            return PAL_FORMAT_R64G64_SFLOAT;

        case VK_FORMAT_R8G8B8_UNORM:
            return PAL_FORMAT_R8G8B8_UNORM;

        case VK_FORMAT_R8G8B8_SNORM:
            return PAL_FORMAT_R8G8B8_SNORM;

        case VK_FORMAT_R8G8B8_UINT:
            return PAL_FORMAT_R8G8B8_UINT;

        case VK_FORMAT_R8G8B8_SINT:
            return PAL_FORMAT_R8G8B8_SINT;

        case VK_FORMAT_R8G8B8_SRGB:
            return PAL_FORMAT_R8G8B8_SRGB;

        case VK_FORMAT_R16G16B16_UNORM:
            return PAL_FORMAT_R16G16B16_UNORM;

        case VK_FORMAT_R16G16B16_SNORM:
            return PAL_FORMAT_R16G16B16_SNORM;

        case VK_FORMAT_R16G16B16_UINT:
            return PAL_FORMAT_R16G16B16_UINT;

        case VK_FORMAT_R16G16B16_SINT:
            return PAL_FORMAT_R16G16B16_SINT;

        case VK_FORMAT_R16G16B16_SFLOAT:
            return PAL_FORMAT_R16G16B16_SFLOAT;

        case VK_FORMAT_R32G32B32_UINT:
            return PAL_FORMAT_R32G32B32_UINT;

        case VK_FORMAT_R32G32B32_SINT:
            return PAL_FORMAT_R32G32B32_SINT;

        case VK_FORMAT_R32G32B32_SFLOAT:
            return PAL_FORMAT_R32G32B32_SFLOAT;

        case VK_FORMAT_R64G64B64_UINT:
            return PAL_FORMAT_R64G64B64_UINT;

        case VK_FORMAT_R64G64B64_SINT:
            return PAL_FORMAT_R64G64B64_SINT;

        case VK_FORMAT_R64G64B64_SFLOAT:
            return PAL_FORMAT_R64G64B64_SFLOAT;

        case VK_FORMAT_B8G8R8_UNORM:
            return PAL_FORMAT_B8G8R8_UNORM;

        case VK_FORMAT_B8G8R8_SNORM:
            return PAL_FORMAT_B8G8R8_SNORM;

        case VK_FORMAT_B8G8R8_UINT:
            return PAL_FORMAT_B8G8R8_UINT;

        case VK_FORMAT_B8G8R8_SINT:
            return PAL_FORMAT_B8G8R8_SINT;

        case VK_FORMAT_B8G8R8_SRGB:
            return PAL_FORMAT_B8G8R8_SRGB;

        case VK_FORMAT_R8G8B8A8_UNORM:
            return PAL_FORMAT_R8G8B8A8_UNORM;

        case VK_FORMAT_R8G8B8A8_SNORM:
            return PAL_FORMAT_R8G8B8A8_SNORM;

        case VK_FORMAT_R8G8B8A8_UINT:
            return PAL_FORMAT_R8G8B8A8_UINT;

        case VK_FORMAT_R8G8B8A8_SINT:
            return PAL_FORMAT_R8G8B8A8_SINT;

        case VK_FORMAT_R8G8B8A8_SRGB:
            return PAL_FORMAT_R8G8B8A8_SRGB;

        case VK_FORMAT_R16G16B16A16_UNORM:
            return PAL_FORMAT_R16G16B16A16_UNORM;

        case VK_FORMAT_R16G16B16A16_SNORM:
            return PAL_FORMAT_R16G16B16A16_SNORM;

        case VK_FORMAT_R16G16B16A16_UINT:
            return PAL_FORMAT_R16G16B16A16_UINT;

        case VK_FORMAT_R16G16B16A16_SINT:
            return PAL_FORMAT_R16G16B16A16_SINT;

        case VK_FORMAT_R16G16B16A16_SFLOAT:
            return PAL_FORMAT_R16G16B16A16_SFLOAT;

        case VK_FORMAT_R32G32B32A32_UINT:
            return PAL_FORMAT_R32G32B32A32_UINT;

        case VK_FORMAT_R32G32B32A32_SINT:
            return PAL_FORMAT_R32G32B32A32_SINT;

        case VK_FORMAT_R32G32B32A32_SFLOAT:
            return PAL_FORMAT_R32G32B32A32_SFLOAT;

        case VK_FORMAT_R64G64B64A64_UINT:
            return PAL_FORMAT_R64G64B64A64_UINT;

        case VK_FORMAT_R64G64B64A64_SINT:
            return PAL_FORMAT_R64G64B64A64_SINT;

        case VK_FORMAT_R64G64B64A64_SFLOAT:
            return PAL_FORMAT_R64G64B64A64_SFLOAT;

        case VK_FORMAT_B8G8R8A8_UNORM:
            return PAL_FORMAT_B8G8R8A8_UNORM;

        case VK_FORMAT_B8G8R8A8_SNORM:
            return PAL_FORMAT_B8G8R8A8_SNORM;

        case VK_FORMAT_B8G8R8A8_UINT:
            return PAL_FORMAT_B8G8R8A8_UINT;

        case VK_FORMAT_B8G8R8A8_SINT:
            return PAL_FORMAT_B8G8R8A8_SINT;

        case VK_FORMAT_B8G8R8A8_SRGB:
            return PAL_FORMAT_B8G8R8A8_SRGB;

        case VK_FORMAT_S8_UINT:
            return PAL_FORMAT_S8_UINT;

        case VK_FORMAT_D16_UNORM:
            return PAL_FORMAT_D16_UNORM;

        case VK_FORMAT_D32_SFLOAT:
            return PAL_FORMAT_D32_SFLOAT;

        case VK_FORMAT_D32_SFLOAT_S8_UINT:
            return PAL_FORMAT_D32_SFLOAT_S8_UINT;

        case VK_FORMAT_D16_UNORM_S8_UINT:
            return PAL_FORMAT_D16_UNORM_S8_UINT;

        case VK_FORMAT_D24_UNORM_S8_UINT:
            return PAL_FORMAT_D24_UNORM_S8_UINT;
    }

    return PAL_FORMAT_UNDEFINED;
}

static PalImageUsages vkFeatureToPalUsage(VkFormatFeatureFlags flags)
{
    PalImageUsages usages = 0;
    if (flags & VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT) {
        usages |= PAL_IMAGE_USAGE_COLOR_ATTACHEMENT;
    }

    if (flags & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT) {
        usages |= PAL_IMAGE_USAGE_DEPTH_ATTACHEMENT;
    }

    if (flags & VK_FORMAT_FEATURE_TRANSFER_SRC_BIT) {
        usages |= PAL_IMAGE_USAGE_TRANSFER_SRC;
    }

    if (flags & VK_FORMAT_FEATURE_TRANSFER_DST_BIT) {
        usages |= PAL_IMAGE_USAGE_TRANSFER_DST;
    }

    if (flags & VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT) {
        usages |= VK_IMAGE_USAGE_STORAGE_BIT;
    }

    if (flags & VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT) {
        usages |= PAL_IMAGE_USAGE_SAMPLED;
    }

    return usages;
}

static VkImageType palImageTypeToVk(PalImageType type)
{
    switch (type) {
        case PAL_IMAGE_TYPE_1D:
            return VK_IMAGE_TYPE_1D;

        case PAL_IMAGE_TYPE_2D:
            return VK_IMAGE_TYPE_2D;

        case PAL_IMAGE_TYPE_3D:
            return VK_IMAGE_TYPE_3D;
    }

    return VK_IMAGE_TYPE_2D;
}

static VkImageViewType palImageViewTypeToVk(PalImageViewType type)
{
    switch (type) {
        case PAL_IMAGE_VIEW_TYPE_1D:
            return VK_IMAGE_VIEW_TYPE_1D;

        case PAL_IMAGE_VIEW_TYPE_1D_ARRAY:
            return VK_IMAGE_VIEW_TYPE_1D_ARRAY;

        case PAL_IMAGE_VIEW_TYPE_2D:
            return VK_IMAGE_VIEW_TYPE_2D;

        case PAL_IMAGE_VIEW_TYPE_2D_ARRAY:
            return VK_IMAGE_VIEW_TYPE_2D_ARRAY;

        case PAL_IMAGE_VIEW_TYPE_3D:
            return VK_IMAGE_VIEW_TYPE_3D;

        case PAL_IMAGE_VIEW_TYPE_CUBE:
            return VK_IMAGE_VIEW_TYPE_CUBE;

        case PAL_IMAGE_VIEW_TYPE_CUBE_ARRAY:
            return VK_IMAGE_VIEW_TYPE_CUBE_ARRAY;
    }

    return VK_IMAGE_VIEW_TYPE_2D;
}

static void* vkAlloc(
    void* pUserData,
    size_t size,
    size_t alignment,
    VkSystemAllocationScope allocationScope)
{
    return palAllocate(s_Graphics.allocator, size, alignment);
}

static void vkFree(
    void* pUserData,
    void* ptr)
{
    palFree(s_Graphics.allocator, ptr);
}

static void* vkRealloc(
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
        void* memory = palAllocate(s_Graphics.allocator, size, alignment);
        if (!memory) {
            free(block);
            return nullptr;
        }

        memcpy(memory, block, size);
        free(block);
        return memory;
    }
    return nullptr;
}

static PalResult vkInitGraphics(bool enableDebugLayer)
{
    s_Vk.libWayland = nullptr;
    s_Vk.libWayland = dlopen("libwayland-client.so.0", RTLD_LAZY);
    if (s_Vk.libWayland) {
        s_Vk.getDisplayFd = (wl_display_get_fd_fn)dlsym(
            s_Vk.libWayland, 
            "wl_display_get_fd");
    }

    // load vulkan
    s_Vk.handle = dlopen("libvulkan.so", RTLD_LAZY);
    if (!s_Vk.handle) {
        return PAL_RESULT_PLATFORM_FAILURE;
    }

    // clang-format off
    s_Vk.enumerateInstanceVersion = (PFN_vkEnumerateInstanceVersion)dlsym(
        s_Vk.handle, 
        "vkEnumerateInstanceVersion");

    s_Vk.enumerateInstanceExtensionProperties = (PFN_vkEnumerateInstanceExtensionProperties)dlsym(
        s_Vk.handle, 
        "vkEnumerateInstanceExtensionProperties");

    s_Vk.createInstance = (PFN_vkCreateInstance)dlsym(
        s_Vk.handle, 
        "vkCreateInstance");

    s_Vk.destroyInstance = (PFN_vkDestroyInstance)dlsym(
        s_Vk.handle, 
        "vkDestroyInstance");

    s_Vk.enumeratePhysicalDevices = (PFN_vkEnumeratePhysicalDevices)dlsym(
        s_Vk.handle, 
        "vkEnumeratePhysicalDevices");

    s_Vk.getPhysicalDeviceProperties = (PFN_vkGetPhysicalDeviceProperties)dlsym(
        s_Vk.handle, 
        "vkGetPhysicalDeviceProperties");

    s_Vk.getPhysicalDeviceMemoryProperties = (PFN_vkGetPhysicalDeviceMemoryProperties)dlsym(
        s_Vk.handle, 
        "vkGetPhysicalDeviceMemoryProperties");

    s_Vk.enumerateInstanceLayerProperties = (PFN_vkEnumerateInstanceLayerProperties)dlsym(
        s_Vk.handle, 
        "vkEnumerateInstanceLayerProperties");

    s_Vk.getPhysicalDeviceQueueFamilyProperties = (PFN_vkGetPhysicalDeviceQueueFamilyProperties)dlsym(
        s_Vk.handle, 
        "vkGetPhysicalDeviceQueueFamilyProperties");

    s_Vk.enumerateDeviceExtensionProperties = (PFN_vkEnumerateDeviceExtensionProperties)dlsym(
        s_Vk.handle, 
        "vkEnumerateDeviceExtensionProperties");

    s_Vk.getPhysicalDeviceFeatures = (PFN_vkGetPhysicalDeviceFeatures)dlsym(
        s_Vk.handle, 
        "vkGetPhysicalDeviceFeatures");

    s_Vk.getPhysicalDeviceFeatures2 = (PFN_vkGetPhysicalDeviceFeatures2)dlsym(
        s_Vk.handle, 
        "vkGetPhysicalDeviceFeatures2");

    s_Vk.getInstanceProcAddr = (PFN_vkGetInstanceProcAddr)dlsym(
        s_Vk.handle, 
        "vkGetInstanceProcAddr");

    s_Vk.createImage = (PFN_vkCreateImage)dlsym(
        s_Vk.handle, 
        "vkCreateImage");

    s_Vk.destroyImage = (PFN_vkDestroyImage)dlsym(
        s_Vk.handle, 
        "vkDestroyImage");

    s_Vk.createImageView = (PFN_vkCreateImageView)dlsym(
        s_Vk.handle, 
        "vkCreateImageView");

    s_Vk.destroyImageView = (PFN_vkDestroyImageView)dlsym(
        s_Vk.handle, 
        "vkDestroyImageView");

    s_Vk.getPhysicalDeviceProperties2 = (PFN_vkGetPhysicalDeviceProperties2)dlsym(
        s_Vk.handle, 
        "vkGetPhysicalDeviceProperties2");

    s_Vk.getPhysicalDeviceFormatProperties = (PFN_vkGetPhysicalDeviceFormatProperties)dlsym(
        s_Vk.handle, 
        "vkGetPhysicalDeviceFormatProperties");

    s_Vk.createDevice = (PFN_vkCreateDevice)dlsym(
        s_Vk.handle, 
        "vkCreateDevice");

    s_Vk.destroyDevice = (PFN_vkDestroyDevice)dlsym(
        s_Vk.handle, 
        "vkDestroyDevice");

    s_Vk.getDeviceQueue = (PFN_vkGetDeviceQueue)dlsym(
        s_Vk.handle, 
        "vkGetDeviceQueue");

    s_Vk.getDeviceProcAddr = (PFN_vkGetDeviceProcAddr)dlsym(
        s_Vk.handle, 
        "vkGetDeviceProcAddr");

    s_Vk.getImageMemoryRequirements = (PFN_vkGetImageMemoryRequirements)dlsym(
        s_Vk.handle, 
        "vkGetImageMemoryRequirements");

    s_Vk.allocateMemory = (PFN_vkAllocateMemory)dlsym(
        s_Vk.handle, 
        "vkAllocateMemory");

    s_Vk.freeMemory = (PFN_vkFreeMemory)dlsym(
        s_Vk.handle, 
        "vkFreeMemory");

    s_Vk.bindImageMemory = (PFN_vkBindImageMemory)dlsym(
        s_Vk.handle, 
        "vkBindImageMemory");

    // clang-format on

    // get version
    bool versionFallback = false;
    s_Vk.hasDynamicRendering = false;
    Uint32 version = 0;
    if (s_Vk.enumerateInstanceVersion) {
        s_Vk.enumerateInstanceVersion(&version);
        if (version <= VK_API_VERSION_1_0) {
            versionFallback = true;
        }

        if (version >= VK_API_VERSION_1_3) {
            s_Vk.hasDynamicRendering = true;
        }
    }

    VkResult ret;
    Uint32 layerCount = 0;
    bool hasValidationLayer = false;
    if (enableDebugLayer) {
        // layers
        ret = s_Vk.enumerateInstanceLayerProperties(
            &layerCount, 
            nullptr);

        if (ret != VK_SUCCESS) {
            s_Vk.hasDebug = false;
        }

        VkLayerProperties* props = nullptr;
        props = palAllocate(
            s_Graphics.allocator, 
            sizeof(VkLayerProperties) * layerCount,
            0);

        if (!props) {
            return PAL_RESULT_OUT_OF_MEMORY;
        }

        s_Vk.enumerateInstanceLayerProperties(&layerCount, props);
        for (int i = 0; i < layerCount; i++) {
            const char* name = props[i].layerName;
            if (strcmp(name, "VK_LAYER_KHRONOS_validation") == 0) {
                hasValidationLayer = true;
                break;
            }
        }

        palFree(s_Graphics.allocator, props);
    }

    // extensions
    Uint32 extCount = 0;
    const char* extensions[8];
    ret = s_Vk.enumerateInstanceExtensionProperties(
        nullptr,
        &extCount,
        nullptr);

    if (ret != VK_SUCCESS) {
        return PAL_RESULT_PLATFORM_FAILURE;
    }

    VkExtensionProperties* extensionProps = nullptr;
    extensionProps = palAllocate(
        s_Graphics.allocator, 
        sizeof(VkExtensionProperties) * extCount, 
        0);

    if (!extensionProps) {
        return PAL_RESULT_SUCCESS;
    }

    s_Vk.enumerateInstanceExtensionProperties(
        nullptr, 
        &extCount, 
        extensionProps);

    bool hasXlib = false;
    bool hasWayland = false;
    bool hasSurface = false;
    bool hasExtDebug = false;

    for (int i = 0; i < extCount; i++) {
        VkExtensionProperties* prop = &extensionProps[i];
        if (strcmp(prop->extensionName, "VK_KHR_xlib_surface") == 0) {
            hasXlib = true;

        } else if (strcmp(prop->extensionName, "VK_KHR_wayland_surface") == 0) {
            hasWayland = true;

        } else if (strcmp(prop->extensionName, "VK_KHR_surface") == 0) {
            hasSurface = true;

        } else if (strcmp(prop->extensionName, "VK_EXT_debug_utils") == 0) {
            hasExtDebug = true;
        }
    }

    palFree(s_Graphics.allocator, extensionProps);

    int extensionCount = 0;
    if (hasSurface) {
        extensions[extensionCount++] = "VK_KHR_surface";
        if (hasWayland) {
            extensions[extensionCount++] = "VK_KHR_wayland_surface";
        }

        if (hasXlib) {
            extensions[extensionCount++] = "VK_KHR_xlib_surface";
        }
    }

    const char* layers[2];
    layerCount = 0;
    if (hasValidationLayer && hasExtDebug) {
        s_Vk.hasDebug = true;
        extensions[extensionCount++] = "VK_EXT_debug_utils";
        layers[layerCount++] = "VK_LAYER_KHRONOS_validation";
    }

    if (versionFallback) {
        const char* name = "VK_KHR_get_physical_device_properties2";
        extensions[extensionCount++] = name;
    }

    VkApplicationInfo appInfo = {0};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.apiVersion = version;
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "Engine";
    appInfo.pApplicationName = "App";
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);

    VkInstanceCreateInfo instanceCreateInfo = {0};
    instanceCreateInfo.pApplicationInfo = &appInfo;
    instanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    instanceCreateInfo.enabledExtensionCount = extensionCount;
    instanceCreateInfo.enabledLayerCount = layerCount;

    instanceCreateInfo.ppEnabledExtensionNames = extensions;
    instanceCreateInfo.ppEnabledLayerNames = layers;

    // vk allocator
    s_Vk.allocator.pfnAllocation = vkAlloc;
    s_Vk.allocator.pfnFree = vkFree;
    s_Vk.allocator.pfnReallocation = vkRealloc;

    VkInstance instance = nullptr;
    VkResult result = s_Vk.createInstance(
        &instanceCreateInfo, 
        &s_Vk.allocator, 
        &instance);

    if (result != VK_SUCCESS) {
        return vkResultToPal(result);
    }

    // clang-format off
    if (versionFallback) {
        s_Vk.getPhysicalDeviceFeatures2KHR = nullptr;
        // load get physical device properties2 proc if we are on version 1.0
        s_Vk.getPhysicalDeviceFeatures2KHR = 
            (PFN_vkGetPhysicalDeviceFeatures2KHR)s_Vk.getInstanceProcAddr(
                s_Vk.handle, 
                "vkGetPhysicalDeviceFeatures2KHR");
    }

    // load surface creation function pointers
    s_Vk.createWaylandSurface = nullptr;
    s_Vk.createXlibSurface = nullptr;

    if (hasWayland) {
        s_Vk.createWaylandSurface = (PFN_vkCreateWaylandSurfaceKHR)s_Vk.getInstanceProcAddr(
            instance, 
            "vkCreateWaylandSurfaceKHR");

        s_Vk.checkWaylandPresentSupport = 
            (PFN_vkGetPhysicalDeviceWaylandPresentationSupportKHR)s_Vk.getInstanceProcAddr(
                instance, 
                "vkGetPhysicalDeviceWaylandPresentationSupportKHR");
    }

    if (hasXlib) {
        s_Vk.createXlibSurface = (PFN_vkCreateXlibSurfaceKHR)s_Vk.getInstanceProcAddr(
            instance, 
            "vkCreateXlibSurfaceKHR");

        s_Vk.checkXlibPresentSupport = 
            (PFN_vkGetPhysicalDeviceXlibPresentationSupportKHR)s_Vk.getInstanceProcAddr(
                instance, 
                "vkGetPhysicalDeviceXlibPresentationSupportKHR");
    }

    // remaining function procs
    s_Vk.destroySurface = (PFN_vkDestroySurfaceKHR)s_Vk.getInstanceProcAddr(
        instance,
        "vkDestroySurfaceKHR");

    s_Vk.getSurfaceCapabilities =
        (PFN_vkGetPhysicalDeviceSurfaceCapabilitiesKHR)s_Vk.getInstanceProcAddr(
            instance,
            "vkGetPhysicalDeviceSurfaceCapabilitiesKHR");

    s_Vk.getSurfacePresentModes = 
        (PFN_vkGetPhysicalDeviceSurfacePresentModesKHR)s_Vk.getInstanceProcAddr(
            instance,
           "vkGetPhysicalDeviceSurfacePresentModesKHR");

    s_Vk.getSurfaceFormats = (PFN_vkGetPhysicalDeviceSurfaceFormatsKHR)s_Vk.getInstanceProcAddr(
        instance,
        "vkGetPhysicalDeviceSurfaceFormatsKHR");
    // clang-format on

    s_Vk.instance = instance;
    return PAL_RESULT_SUCCESS;
}

static void vkShutdownGraphics()
{
    s_Vk.destroyInstance(s_Vk.instance, &s_Vk.allocator);
    dlclose(s_Vk.handle);
    if (s_Vk.libWayland) {
        dlclose(s_Vk.libWayland);
    }
    memset(&s_Vk, 0, sizeof(s_Vk));
}

static PalResult _vkEnumerateAdapters(
    Int32* count, 
    PalAdapter** outAdapters)
{
    int _count = 0;
    int deviceCount = 0;
    int maxCount = outAdapters ? *count : 0;
    VkResult result;

    result = s_Vk.enumeratePhysicalDevices(s_Vk.instance, &_count, nullptr);
    if (result != VK_SUCCESS) {
        return PAL_RESULT_PLATFORM_FAILURE;
    }

    // PAL only supports supports dynamic rendering
    VkPhysicalDevice* devices = nullptr;
    devices = palAllocate(s_Graphics.allocator, 
        sizeof(VkPhysicalDevice) * _count, 
        0);

    if (!devices) {
        return PAL_RESULT_OUT_OF_MEMORY;
    }

    s_Vk.enumeratePhysicalDevices(s_Vk.instance, &_count, devices);
    for (int i = 0; i < _count; i++) {
        // check if the gpu supports dynamic rendering
        if (s_Vk.hasDynamicRendering) {
            if (outAdapters) {
                if (deviceCount < *count) {
                    PalAdapter* adapter = (PalAdapter*)devices[i];
                    outAdapters[deviceCount++] = adapter;
                }
                
            } else {
                deviceCount++;
            }

        } else {
            // check extension
            Uint32 extCount = 0;
            VkExtensionProperties* exts = nullptr;
            VkResult ret;
            ret = s_Vk.enumerateDeviceExtensionProperties(
                devices[i],
                nullptr, 
                &extCount, 
                nullptr);

            if (ret != VK_SUCCESS) {
                // skip
                continue;
            }

            exts = palAllocate(
                s_Graphics.allocator, 
                sizeof(VkExtensionProperties) * extCount, 
                0);

            if (!exts) {
                return PAL_RESULT_OUT_OF_MEMORY;
            }

            s_Vk.enumerateDeviceExtensionProperties(
                devices[i],
                nullptr, 
                &extCount, 
                exts);

            bool found = false;
            for (int j = 0; j < extCount; j++) {
                const char* name = exts[j].extensionName;
                if (strcmp(name, "VK_KHR_dynamic_rendering") == 0) {
                    found = true;
                }
            }

            palFree(s_Graphics.allocator, exts);
            if (!found) {
                // skip
                continue;
            }

            if (outAdapters) {
                if (deviceCount < *count) {
                    PalAdapter* adapter = (PalAdapter*)devices[i];
                    outAdapters[deviceCount++] = adapter;
                }
                
            } else {
                deviceCount++;
            }
        }
    }

    if (!outAdapters) {
        *count = deviceCount;
    }

    palFree(s_Graphics.allocator, devices);
    return PAL_RESULT_SUCCESS;
}

static PalResult PAL_CALL _vkGetAdapterInfo(
    PalAdapter* adapter,
    PalAdapterInfo* info)
{
    VkPhysicalDevice phyDevice = (VkPhysicalDevice)adapter;
    VkPhysicalDeviceProperties props = {0};
    VkPhysicalDeviceMemoryProperties memProps = {0};

    s_Vk.getPhysicalDeviceMemoryProperties(phyDevice, &memProps);
    s_Vk.getPhysicalDeviceProperties(phyDevice, &props);

    info->apiType = PAL_ADAPTER_API_TYPE_VULKAN;
    info->shaderFormats = PAL_SHADER_FORMAT_SPIRV;
    info->version = props.driverVersion;
    info->deviceId = props.deviceID;
    info->vendorId = props.vendorID;
    strcpy(info->name, props.deviceName);

    info->vram = 0;
    info->sharedMemory = 0;
    for (int i = 0; i < memProps.memoryHeapCount; i++) {
        if (memProps.memoryHeaps[i].flags & VK_MEMORY_HEAP_DEVICE_LOCAL_BIT) {
            info->vram += memProps.memoryHeaps[i].size;
        } else {
            info->sharedMemory += memProps.memoryHeaps[i].size;
        }
    }

    // get device type
    switch (props.deviceType) {
        case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU: {
            info->type = PAL_ADAPTER_TYPE_INTEGRATED;
            break;
        }

        case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU: {
            info->type = PAL_ADAPTER_TYPE_DISCRETE;
            break;
        }

        case VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU: {
            info->type = PAL_ADAPTER_TYPE_VIRTUAL;
            break;
        }

        case VK_PHYSICAL_DEVICE_TYPE_CPU: {
            info->type = PAL_ADAPTER_TYPE_CPU;
            break;
        }

        default: {
            info->type = PAL_ADAPTER_TYPE_UNKNOWN;
            break;
        }
    }

    // version string
    snprintf(
        info->versionString, 
        PAL_ADAPTER_VERSION_SIZE, 
        "%d.%d.%d",
        VK_VERSION_MAJOR(props.apiVersion),
        VK_VERSION_MINOR(props.apiVersion),
        VK_VERSION_PATCH(props.apiVersion));

    return PAL_RESULT_SUCCESS;
}

static PalResult PAL_CALL _vkGetAdapterCapabilities(
    PalAdapter* adapter,
    PalAdapterCapabilities* caps)
{
    VkResult ret = VK_SUCCESS;
    VkPhysicalDevice phyDevice = (VkPhysicalDevice)adapter;
    VkPhysicalDeviceProperties props = {0};
    VkPhysicalDeviceMultiviewPropertiesKHR vProps = {0};
    VkPhysicalDeviceProperties2 properties2 = {0};
    vProps.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MULTIVIEW_PROPERTIES_KHR;

    s_Vk.getPhysicalDeviceProperties(phyDevice, &props);
    properties2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
    properties2.pNext = &vProps;
    s_Vk.getPhysicalDeviceProperties2(phyDevice, &properties2);

    caps->debugLayer = s_Vk.hasDebug;
    caps->maxColorAttachments = props.limits.maxColorAttachments;
    caps->maxImageWidth = props.limits.maxImageDimension2D;
    caps->maxImageHeight = props.limits.maxImageDimension2D;
    caps->maxImageDepth = props.limits.maxImageDimension3D;
    caps->maxImageArrayLayers = props.limits.maxImageArrayLayers;
    
    Uint32 tmp = vkSamplesToSamples(props.limits.framebufferColorSampleCounts);
    caps->maxColorSamples = tmp;
    tmp = vkSamplesToSamples(props.limits.framebufferDepthSampleCounts);
    caps->maxDepthSamples = tmp;

    caps->maxViewports = props.limits.maxViewports;
    caps->maxSamplers = props.limits.maxSamplerAllocationCount;
    caps->maxUniformBufferSize = props.limits.maxUniformBufferRange;
    caps->maxStorageBufferSize = props.limits.maxStorageBufferRange;
    caps->maxPushConstantSize = props.limits.maxPushConstantsSize;
    
    caps->maxMultiViews = vProps.maxMultiviewViewCount;
    if (caps->maxMultiViews == 0) {
        caps->maxMultiViews = 1;
    }

    // vulkan does not give this but we calculate from the max width and width
    Uint32 a = caps->maxImageWidth;
    Uint32 b = caps->maxImageHeight;
    Uint32 c = caps->maxImageDepth;

    tmp = a > b ? a : b;
    Uint32 size = tmp > c ? tmp : c;
    Uint32 levels = 0;
    while (size > 0) {
        // divide by two 
        size = size / 2;
        levels++;
    }
    caps->maxImageMipLevels = levels;

    // get supported queue commands
    Uint32 count;
    s_Vk.getPhysicalDeviceQueueFamilyProperties(
        phyDevice,
        &count, 
        nullptr);

    VkQueueFamilyProperties* queueProps = nullptr;
    queueProps = palAllocate(
        s_Graphics.allocator, 
        sizeof(VkQueueFamilyProperties) * count, 
        0);

    s_Vk.getPhysicalDeviceQueueFamilyProperties(
        phyDevice, 
        &count, 
        queueProps);

    caps->maxComputeQueues = 0;
    caps->maxGraphicsQueues = 0;
    caps->maxCopyQueues = 0;
    
    for (int i = 0; i < count; i++) {
        if (queueProps[i].queueFlags & VK_QUEUE_COMPUTE_BIT) {
            caps->maxComputeQueues += queueProps->queueCount;
        }

        if (queueProps[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            caps->maxGraphicsQueues += queueProps->queueCount;
        }

        if (queueProps[i].queueFlags & VK_QUEUE_TRANSFER_BIT) {
            caps->maxCopyQueues += queueProps->queueCount;
        }
    }

    palFree(s_Graphics.allocator, queueProps);

    // get supported extensions
    Uint32 extensionCount = 0;
    ret = s_Vk.enumerateDeviceExtensionProperties(
        phyDevice,
        nullptr, 
        &extensionCount, 
        nullptr);

    if (ret != VK_SUCCESS) {
        // we just return without any modern features which is rare
        return PAL_RESULT_SUCCESS;
    }

    VkExtensionProperties* extensionProps = nullptr;
    extensionProps = palAllocate(
        s_Graphics.allocator, 
        sizeof(VkExtensionProperties) * extensionCount, 
        0);

    if (!extensionProps) {
        return PAL_RESULT_SUCCESS;
    }

    s_Vk.enumerateDeviceExtensionProperties(
        phyDevice, 
        nullptr, 
        &extensionCount, 
        extensionProps);

    bool rayTracingFound = false;
    bool accelerateFound = false;
    caps->features = 0;

    // clang-format off
    for (int i = 0; i < extensionCount; i++) {
        VkExtensionProperties* props = &extensionProps[i];
        if (strcmp(props->extensionName, "VK_KHR_ray_tracing_pipeline") == 0) {
            rayTracingFound = true;

        } else if (strcmp(props->extensionName, "VK_KHR_acceleration_structure") == 0) {
            accelerateFound = true;

        } else if (strcmp(props->extensionName, "VK_EXT_mesh_shader") == 0) {
            // mesh shader
            VkPhysicalDeviceMeshShaderFeaturesEXT mesh = {0};
            mesh.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MESH_SHADER_FEATURES_EXT;

            VkPhysicalDeviceFeatures2 features;
            features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
            features.pNext = &mesh;

            if (s_Vk.getPhysicalDeviceFeatures2KHR) {
                s_Vk.getPhysicalDeviceFeatures2KHR(phyDevice, &features);

            } else {
                s_Vk.getPhysicalDeviceFeatures2(phyDevice, &features);
            }

            if (mesh.meshShader && mesh.taskShader) {
                caps->features |= PAL_ADAPTER_FEATURE_MESH_SHADER;
            }

        } else if (strcmp(props->extensionName, "VK_KHR_fragment_shading_rate") == 0) {
            // variable rate shading
            VkPhysicalDeviceFragmentShadingRateFeaturesKHR frag = {0};
            frag.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FRAGMENT_SHADING_RATE_FEATURES_KHR;

            VkPhysicalDeviceFeatures2 features;
            features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
            features.pNext = &frag;

            if (s_Vk.getPhysicalDeviceFeatures2KHR) {
                s_Vk.getPhysicalDeviceFeatures2KHR(phyDevice, &features);

            } else {
                s_Vk.getPhysicalDeviceFeatures2(phyDevice, &features);
            }

            if (frag.pipelineFragmentShadingRate) {
                caps->features |= PAL_ADAPTER_FEATURE_VARIABLE_RATE_SHADING;
            }

        } else if (strcmp(props->extensionName, "VK_EXT_descriptor_indexing") == 0) {
            // descriptor indexing
            VkPhysicalDeviceDescriptorIndexingFeatures desc = {0};
            desc.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_FEATURES;

            VkPhysicalDeviceFeatures2 features;
            features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
            features.pNext = &desc;

            if (s_Vk.getPhysicalDeviceFeatures2KHR) {
                s_Vk.getPhysicalDeviceFeatures2KHR(phyDevice, &features);

            } else {
                s_Vk.getPhysicalDeviceFeatures2(phyDevice, &features);
            }

            if (desc.shaderSampledImageArrayNonUniformIndexing) {
                caps->features |= PAL_ADAPTER_FEATURE_DESCRIPTOR_INDEXING;
            }

        } else if (strcmp(props->extensionName, "VK_KHR_swapchain") == 0) {
            // swapchain
            caps->features |= PAL_ADAPTER_FEATURE_SWAPCHAIN;

        } else if (strcmp(props->extensionName, "VK_KHR_shader_float16_int8") == 0) {
            // shader float16
            VkPhysicalDeviceShaderFloat16Int8FeaturesKHR shader16 = {0};
            shader16.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_FLOAT16_INT8_FEATURES_KHR;

            VkPhysicalDeviceFeatures2 features;
            features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
            features.pNext = &shader16;

            if (s_Vk.getPhysicalDeviceFeatures2KHR) {
                s_Vk.getPhysicalDeviceFeatures2KHR(phyDevice, &features);

            } else {
                s_Vk.getPhysicalDeviceFeatures2(phyDevice, &features);
            }

            if (shader16.shaderFloat16) {
                caps->features |= PAL_ADAPTER_FEATURE_SHADER_FLOAT16;
            }

        } else if (strcmp(props->extensionName, "VK_KHR_timeline_semaphore") == 0) {
            // timeline semaphore
            VkPhysicalDeviceTimelineSemaphoreFeatures timeline = {0};
            timeline.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_TIMELINE_SEMAPHORE_FEATURES;

            VkPhysicalDeviceFeatures2 features;
            features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
            features.pNext = &timeline;

            if (s_Vk.getPhysicalDeviceFeatures2KHR) {
                s_Vk.getPhysicalDeviceFeatures2KHR(phyDevice, &features);

            } else {
                s_Vk.getPhysicalDeviceFeatures2(phyDevice, &features);
            }

            if (timeline.timelineSemaphore) {
                caps->features |= PAL_ADAPTER_FEATURE_TIMELINE_SEMAPHORE;
            }

        } else if (strcmp(props->extensionName, "VK_KHR_multiview") == 0) {
            // multi view
            VkPhysicalDeviceMultiviewFeaturesKHR view = {0};
            view.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MULTIVIEW_FEATURES_KHR;

            VkPhysicalDeviceFeatures2 features;
            features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
            features.pNext = &view;

            if (s_Vk.getPhysicalDeviceFeatures2KHR) {
                s_Vk.getPhysicalDeviceFeatures2KHR(phyDevice, &features);

            } else {
                s_Vk.getPhysicalDeviceFeatures2(phyDevice, &features);
            }

            if (view.multiview) {
                caps->features |= PAL_ADAPTER_FEATURE_MULTI_VIEW;
            }
        }
    }

    if (accelerateFound && rayTracingFound) {
        // ray tracing
        VkPhysicalDeviceRayTracingPipelineFeaturesKHR ray = {0};
        VkPhysicalDeviceAccelerationStructureFeaturesKHR acc = {0};
        ray.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_TRACING_PIPELINE_FEATURES_KHR;
        acc.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ACCELERATION_STRUCTURE_FEATURES_KHR;

        ray.pNext = &acc;
        VkPhysicalDeviceFeatures2 features;
        features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
        features.pNext = &ray;

        if (s_Vk.getPhysicalDeviceFeatures2KHR) {
            s_Vk.getPhysicalDeviceFeatures2KHR(phyDevice, &features);

        } else {
            s_Vk.getPhysicalDeviceFeatures2(phyDevice, &features);
        }

        if (ray.rayTracingPipeline && acc.accelerationStructure) {
            caps->features |= PAL_ADAPTER_FEATURE_RAY_TRACING;
        }
    }
    // clang-format on
    VkPhysicalDeviceFeatures features;
    s_Vk.getPhysicalDeviceFeatures(phyDevice, &features);

    // check for additional features
    if (features.geometryShader) {
        caps->features |= PAL_ADAPTER_FEATURE_GEOMETRY_SHADER;
    }

    if (features.multiViewport) {
        caps->features |= PAL_ADAPTER_FEATURE_MULTI_VIEWPORT;
    }

    if (features.samplerAnisotropy) {
        caps->features |= PAL_ADAPTER_FEATURE_SAMPLER_ANISOTROPY;
    }

    if (features.sampleRateShading) {
        caps->features |= PAL_ADAPTER_FEATURE_SAMPLE_RATE_SHADING;
    }

    if (features.shaderFloat64) {
        caps->features |= PAL_ADAPTER_FEATURE_SHADER_FLOAT64;
    }

    if (features.shaderInt64) {
        caps->features |= PAL_ADAPTER_FEATURE_SHADER_INT64;
    }

    if (features.shaderInt16) {
        caps->features |= PAL_ADAPTER_FEATURE_SHADER_INT16;
    }

    if (features.tessellationShader) {
        caps->features |= PAL_ADAPTER_FEATURE_TESSELLATION_SHADER;
    }

    // this features are supported on vulkan
    caps->features |= PAL_ADAPTER_FEATURE_CUBE_ARRAY_IMAGE_VIEW;

    palFree(s_Graphics.allocator, extensionProps);
    return PAL_RESULT_SUCCESS;
}

static PalResult PAL_CALL _vkCreateDevice(
    PalAdapter* adapter,
    PalAdapterFeatures features,
    PalDevice** outDevice)
{ 
    float priority = 1.0f;
    Uint32 count = 0;
    VkResult ret = VK_SUCCESS;
    Device* device = nullptr;
    VkPhysicalDevice phyDevice = (VkPhysicalDevice)adapter;

    VkQueueFamilyProperties* queueProps = nullptr;
    VkDeviceQueueCreateInfo* queueCreateInfos = nullptr;
    s_Vk.getPhysicalDeviceQueueFamilyProperties(
        phyDevice, 
        &count, 
        nullptr);

    queueProps = palAllocate(
        s_Graphics.allocator, 
        sizeof(VkQueueFamilyProperties) * count, 
        0);

    queueCreateInfos = palAllocate(
        s_Graphics.allocator, 
        sizeof(VkDeviceQueueCreateInfo) * count, 
        0);

    device = palAllocate(s_Graphics.allocator, sizeof(Device), 0);
    if (!queueProps || !queueCreateInfos || !device) {
        return PAL_RESULT_OUT_OF_MEMORY;
    }

    device->queueCount = count;
    device->phyDevice = phyDevice;
    
    device->phyQueues = palAllocate(
        s_Graphics.allocator, 
        sizeof(PhysicalQueue) * count, 
        0);
        
    if (!device->phyQueues) {
        return PAL_RESULT_OUT_OF_MEMORY;
    }

    s_Vk.getPhysicalDeviceQueueFamilyProperties(
        phyDevice, 
        &count, 
        queueProps);
        
    for (int i = 0; i < count; i++) {
        queueCreateInfos[i].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfos[i].pNext = nullptr;
        queueCreateInfos[i].pQueuePriorities = &priority;
        queueCreateInfos[i].queueFamilyIndex = i;
        queueCreateInfos[i].queueCount = queueProps[i].queueCount;
        queueCreateInfos[i].flags = 0;
    }

    // build features and extensions capabilities
    VkPhysicalDeviceFeatures coreFeatures = {0};
    if (features & PAL_ADAPTER_FEATURE_SAMPLER_ANISOTROPY) {
        coreFeatures.samplerAnisotropy = true;
    }

    if (features & PAL_ADAPTER_FEATURE_SAMPLE_RATE_SHADING) {
        coreFeatures.sampleRateShading = true;
    }

    if (features & PAL_ADAPTER_FEATURE_MULTI_VIEWPORT) {
        coreFeatures.multiViewport = true;
    }

    if (features & PAL_ADAPTER_FEATURE_TESSELLATION_SHADER) {
        coreFeatures.tessellationShader = true;
    }

    if (features & PAL_ADAPTER_FEATURE_GEOMETRY_SHADER) {
        coreFeatures.geometryShader = true;
    }

    if (features & PAL_ADAPTER_FEATURE_SHADER_INT16) {
        coreFeatures.shaderInt16 = true;
    }

    if (features & PAL_ADAPTER_FEATURE_SHADER_INT64) {
        coreFeatures.shaderInt64 = true;
    }

    if (features & PAL_ADAPTER_FEATURE_SHADER_FLOAT64) {
        coreFeatures.shaderFloat64 = true;
    }

    // extensions and features2
    int extCount = 0;
    const char* extensions[16] = {0};

    // clang-format off

    const void* start = nullptr;
    VkPhysicalDeviceTimelineSemaphoreFeatures timeline = {0};
    timeline.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_TIMELINE_SEMAPHORE_FEATURES;

    VkPhysicalDeviceShaderFloat16Int8FeaturesKHR shader16 = {0};
    shader16.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_FLOAT16_INT8_FEATURES_KHR;

    VkPhysicalDeviceMeshShaderFeaturesEXT mesh = {0};
    mesh.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MESH_SHADER_FEATURES_EXT;

    VkPhysicalDeviceRayTracingPipelineFeaturesKHR ray = {0};
    VkPhysicalDeviceAccelerationStructureFeaturesKHR acc = {0};
    ray.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_TRACING_PIPELINE_FEATURES_KHR;
    acc.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ACCELERATION_STRUCTURE_FEATURES_KHR;

    VkPhysicalDeviceFragmentShadingRateFeaturesKHR vrs = {0};
    vrs.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FRAGMENT_SHADING_RATE_FEATURES_KHR;

    VkPhysicalDeviceDescriptorIndexingFeatures descIndex = {0};
    descIndex.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_FEATURES;

    VkPhysicalDeviceMultiviewFeatures multiView = {0};
    multiView.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MULTIVIEW_FEATURES;

    // clang-format on

    extensions[extCount++] = "VK_KHR_dynamic_rendering";
    if (features & PAL_ADAPTER_FEATURE_SWAPCHAIN) {
        extensions[extCount++] = "VK_KHR_swapchain";
    }

    if (features & PAL_ADAPTER_FEATURE_TIMELINE_SEMAPHORE) {
        extensions[extCount++] = "VK_KHR_timeline_semaphore";
        timeline.timelineSemaphore = true;
        start = &timeline;
    }

    if (features & PAL_ADAPTER_FEATURE_SHADER_FLOAT16) {
        extensions[extCount++] = "VK_KHR_shader_float16_int8";
        shader16.shaderFloat16 = true;

        if (timeline.timelineSemaphore) {
            timeline.pNext = &shader16;
        }
        start = &shader16;
    }

    if (features & PAL_ADAPTER_FEATURE_RAY_TRACING) {
        extensions[extCount++] = "VK_KHR_ray_tracing_pipeline";
        extensions[extCount++] = "VK_KHR_acceleration_structure";
        ray.rayTracingPipeline = true;
        acc.accelerationStructure = true;
       
        if (shader16.shaderFloat16) {
            shader16.pNext = &ray;

        } else if (timeline.timelineSemaphore) {
            // no shader16, check timeline
            timeline.pNext = &ray;
        }
        ray.pNext = &acc;
        start = &ray;
    }

    if (features & PAL_ADAPTER_FEATURE_MESH_SHADER) {
        extensions[extCount++] = "VK_EXT_mesh_shader";
        mesh.meshShader = true;
        mesh.taskShader = true;

        if (acc.accelerationStructure) {
            acc.pNext = &mesh;

        } else if (shader16.shaderFloat16) {
            shader16.pNext = &mesh;

        } else if (timeline.timelineSemaphore) {
            timeline.pNext = &mesh;
        }
        start = &mesh;
    }

    if (features & PAL_ADAPTER_FEATURE_VARIABLE_RATE_SHADING) {
        extensions[extCount++] = "VK_KHR_fragment_shading_rate";
        vrs.pipelineFragmentShadingRate = true;

        if (mesh.meshShader) {
            mesh.pNext = &vrs;

        } else if (acc.accelerationStructure) {
            acc.pNext = &vrs;

        } else if (shader16.shaderFloat16) {
            shader16.pNext = &vrs;

        } else if (timeline.timelineSemaphore) {
            timeline.pNext = &vrs;
        }
        start = &vrs;
    }

    if (features & PAL_ADAPTER_FEATURE_DESCRIPTOR_INDEXING) {
        extensions[extCount++] = "VK_EXT_descriptor_indexing";
        descIndex.shaderSampledImageArrayNonUniformIndexing = true;

        if (vrs.pipelineFragmentShadingRate) {
            vrs.pNext = &descIndex;

        } else if (mesh.meshShader) {
            mesh.pNext = &descIndex;

        } else if (acc.accelerationStructure) {
            acc.pNext = &descIndex;

        } else if (shader16.shaderFloat16) {
            shader16.pNext = &descIndex;

        } else if (timeline.timelineSemaphore) {
            timeline.pNext = &descIndex;
        }
        start = &descIndex;
    }

    if (features & PAL_ADAPTER_FEATURE_MULTI_VIEW) {
        extensions[extCount++] = "VK_KHR_multiview";
        multiView.multiview = true;

        if (descIndex.shaderSampledImageArrayNonUniformIndexing) {
            descIndex.pNext = &multiView;

        } else if (vrs.pipelineFragmentShadingRate) {
            vrs.pNext = &multiView;

        } else if (mesh.meshShader) {
            mesh.pNext = &multiView;

        } else if (acc.accelerationStructure) {
            acc.pNext = &multiView;

        } else if (shader16.shaderFloat16) {
            shader16.pNext = &multiView;

        } else if (timeline.timelineSemaphore) {
            timeline.pNext = &multiView;
        }
        start = &multiView;
    }

    VkDeviceCreateInfo createInfo = {0};
    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    createInfo.pEnabledFeatures = &coreFeatures;
    createInfo.enabledExtensionCount = extCount;
    createInfo.ppEnabledExtensionNames = extensions;
    createInfo.pQueueCreateInfos = queueCreateInfos;
    createInfo.queueCreateInfoCount = count;
    createInfo.pNext = start;

    ret = s_Vk.createDevice(
        phyDevice, 
        &createInfo, 
        &s_Vk.allocator, 
        &device->handle);

    if (ret != VK_SUCCESS) {
        palFree(s_Graphics.allocator, queueProps);
        palFree(s_Graphics.allocator, queueCreateInfos);
        palFree(s_Graphics.allocator, device->phyQueues);
        palFree(s_Graphics.allocator, device);
        return vkResultToPal(ret);
    }

    // get queues
    for (int i = 0; i < count; i++) {
        VkQueueFamilyProperties* data = &queueProps[i];
        for (int j = 0; j < data->queueCount; j++) {
            PhysicalQueue* queue = &device->phyQueues[i];
            s_Vk.getDeviceQueue(device->handle, i, j, &queue->handle);
            queue->usages = data->queueFlags;
            queue->usedUsages = 0;
            queue->familyIndex = i;
            queue->phyDevice = phyDevice;
        }        
    }

    // cache memory type indices
    VkPhysicalDeviceMemoryProperties memProps = {0};
    s_Vk.getPhysicalDeviceMemoryProperties(phyDevice, &memProps);
    device->memoryTypeIndex[PAL_MEMORY_TYPE_GPU_ONLY] = -1;
    device->memoryTypeIndex[PAL_MEMORY_TYPE_GPU_ONLY] = -1;
    device->memoryTypeIndex[PAL_MEMORY_TYPE_GPU_ONLY] = -1;

    for (int i = 0; i < memProps.memoryTypeCount; i++) {
        VkMemoryPropertyFlags prop = memProps.memoryTypes[i].propertyFlags;
        if (prop & VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT) {
            device->memoryTypeIndex[PAL_MEMORY_TYPE_GPU_ONLY] = i;
        }

        if ((prop & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT) && 
             prop & VK_MEMORY_PROPERTY_HOST_COHERENT_BIT) {
            device->memoryTypeIndex[PAL_MEMORY_TYPE_CPU_UPLOAD] = i;
        }

        if ((prop & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT) && 
             prop & VK_MEMORY_PROPERTY_HOST_CACHED_BIT) {
            device->memoryTypeIndex[PAL_MEMORY_TYPE_CPU_READBACK] = i;
        }
    }

    // load procs
    device->createSwapchain = nullptr;
    device->acquireNextImage = (PFN_vkAcquireNextImageKHR)s_Vk.getDeviceProcAddr(
        device->handle, 
        "vkAcquireNextImageKHR");

    device->createSwapchain = (PFN_vkCreateSwapchainKHR)s_Vk.getDeviceProcAddr(
        device->handle, 
        "vkCreateSwapchainKHR");

    device->destroySwapchain = (PFN_vkDestroySwapchainKHR)s_Vk.getDeviceProcAddr(
        device->handle, 
        "vkDestroySwapchainKHR");

    device->getSwapchainImages = (PFN_vkGetSwapchainImagesKHR)s_Vk.getDeviceProcAddr(
        device->handle, 
        "vkGetSwapchainImagesKHR");

    device->queuePresent = (PFN_vkQueuePresentKHR)s_Vk.getDeviceProcAddr(
        device->handle, 
        "vkQueuePresentKHR");

    palFree(s_Graphics.allocator, queueProps);
    palFree(s_Graphics.allocator, queueCreateInfos);

    *outDevice = (PalDevice*)device;
    return PAL_RESULT_SUCCESS;
}

static void PAL_CALL _vkDestroyDevice(PalDevice* device)
{
    Device* _device = (Device*)device;
    s_Vk.destroyDevice(_device->handle, &s_Vk.allocator);
    palFree(s_Graphics.allocator, _device->phyQueues);
    palFree(s_Graphics.allocator, _device);
}

static PalResult PAL_CALL _vkCreateQueue(
    PalDevice* device,
    PalQueueType type,
    PalQueue** outQueue)
{
    VkQueueFlags queueFlag = 0;
    Device* _device = (Device*)device;
    Queue* queue = nullptr;
    if (!_device->handle) {
        return PAL_RESULT_INVALID_GRAPHICS_DEVICE;
    }

    if (_device->queueCount == 0) {
        return PAL_RESULT_OUT_OF_QUEUE;
    }

    switch (type) {
        case PAL_QUEUE_TYPE_COMPUTE: {
            queueFlag = VK_QUEUE_COMPUTE_BIT;
            break;
        }

        case PAL_QUEUE_TYPE_GRAPHICS: {
            queueFlag = VK_QUEUE_GRAPHICS_BIT;
            break;
        }

        case PAL_QUEUE_TYPE_COPY: {
            queueFlag = VK_QUEUE_TRANSFER_BIT;
            break;
        }
    }

    PhysicalQueue* phyQueue = nullptr;
    for (int i = 0; i < _device->queueCount; i++) {
        PhysicalQueue* queue = &_device->phyQueues[i];
        // check if the physical queue supports the requested operation
        // and if its not already used
        if (queue->usages & queueFlag && 
            queue->usedUsages != queueFlag) {
            queue->usedUsages |= queueFlag;
            phyQueue = queue;
            break;
        } 
    }

    if (phyQueue) {
        queue = palAllocate(
            s_Graphics.allocator, 
            sizeof(Queue), 
            0);
        
        if (!queue) {
            return PAL_RESULT_OUT_OF_MEMORY;
        }

        queue->phyQueue = phyQueue;
        queue->usage = queueFlag;
        queue->device = _device;

        *outQueue = (PalQueue*)queue;
        return PAL_RESULT_SUCCESS;
    }

    return PAL_RESULT_OUT_OF_QUEUE;
}

static void PAL_CALL _vkDestroyQueue(PalQueue* queue)
{
    Queue* _queue = (Queue*)queue;
    PhysicalQueue* phyQueue = _queue->phyQueue;
    phyQueue->usedUsages &= ~_queue->usage;
    palFree(s_Graphics.allocator, _queue);
}

static bool PAL_CALL _vkCanQueuePresent(
    PalQueue* queue, 
    PalGfxWindow* window)
{
    // check if the queue is a graphics queue before we check its family 
    // index for presentation support.
    Queue* _queue = (Queue*)queue;
    if (_queue->usage != VK_QUEUE_GRAPHICS_BIT) {
        return false;
    }

    bool onWayland = vkOnWayland(window->display);
    PhysicalQueue* phyQueue = _queue->phyQueue;
    if (!s_Vk.checkWaylandPresentSupport(
        phyQueue->phyDevice, 
        phyQueue->familyIndex, window->display)) {
        return false;
    } else {
        // TODO: check presentation for xlib
    }

    return true;
}

static PalResult PAL_CALL _vkCreateImage(
    PalDevice* device,
    const PalImageCreateInfo* info,
    PalImage** outImage)
{
    VkResult result;
    Image* image = nullptr;
    Device* _device = (Device*)device;
    if (!_device->handle) {
        return PAL_RESULT_INVALID_GRAPHICS_DEVICE;
    }

    image = palAllocate(s_Graphics.allocator, sizeof(Image), 0);
    if (!image) {
        return PAL_RESULT_OUT_OF_MEMORY;
    }

    VkImageCreateInfo createInfo = {0};
    createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    createInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    createInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
    createInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    createInfo.extent.width = info->width;
    createInfo.extent.height = info->height;
    createInfo.mipLevels = info->mipLevelCount;

    createInfo.format = palFormatToVk(info->format);
    createInfo.samples = samplesToVk(info->samples);
    createInfo.usage = palUsageToVk(info->usages);

    createInfo.arrayLayers = info->depthOrArraySize;
    createInfo.extent.depth = 1;
    createInfo.imageType = palImageTypeToVk(info->type);

    if (info->type == PAL_IMAGE_TYPE_3D) {
        createInfo.arrayLayers = 1;
        createInfo.extent.depth = info->depthOrArraySize;
    }

    result = s_Vk.createImage(
        _device->handle, 
        &createInfo, 
        &s_Vk.allocator, 
        &image->handle);

    if (result != VK_SUCCESS) {
        palFree(s_Graphics.allocator, image);
        return vkResultToPal(result);
    }

    image->belongsToSwapchain = false;
    image->device = _device;
    image->info.depthOrArraySize = info->depthOrArraySize;
    image->info.type = info->type;
    image->info.format = info->format;
    image->info.usages = info->usages;
    image->info.height = info->height;
    image->info.mipLevelCount = info->mipLevelCount;
    image->info.samples = info->samples;
    image->info.width = info->width;

    *outImage = (PalImage*)image;
    return PAL_RESULT_SUCCESS;
}

static void PAL_CALL _vkDestroyImage(PalImage* image)
{
    Image* _image = (Image*)image;
    if (_image->belongsToSwapchain) {
        return;
    }

    s_Vk.destroyImage(_image->device->handle, _image->handle, &s_Vk.allocator);
    palFree(s_Graphics.allocator, _image);
}

static PalResult PAL_CALL _vkGetImageInfo(
    PalImage* image,
    PalImageInfo* info)
{
    Image* _image = (Image*)image;
    *info = _image->info;
    return PAL_RESULT_SUCCESS;
}

static PalResult PAL_CALL _vkEnumerateFormats(
    PalAdapter* adapter,
    Int32* count,
    PalFormatInfo* outFormats)
{
    Int32 fmtCount = 0;
    VkPhysicalDevice phyDevice = (VkPhysicalDevice)adapter;
    VkFormatProperties props = {0};

    for (int i = 0; i < PAL_FORMAT_MAX; i++) {
        VkFormat fmt = palFormatToVk((PalFormat)i);
        s_Vk.getPhysicalDeviceFormatProperties(phyDevice, fmt, &props);
        if (props.optimalTilingFeatures != 0) {
            // format supported
            if (outFormats) {
                if (fmtCount < *count) {
                    PalFormatInfo* fmtInfo = &outFormats[fmtCount++];
                    fmtInfo->format = (PalFormat)i;
                    fmtInfo->usages = 
                        vkFeatureToPalUsage(props.optimalTilingFeatures);

                    PalImageViewUsages usages = 0;
                    if (i == PAL_FORMAT_S8_UINT) {
                        usages |= PAL_IMAGE_VIEW_USAGE_STENCIL;
                    }

                    if (i == PAL_FORMAT_D16_UNORM || i == PAL_FORMAT_D32_SFLOAT) {
                        usages |= PAL_IMAGE_VIEW_USAGE_DEPTH;
                    }

                    if (i == PAL_FORMAT_D32_SFLOAT_S8_UINT || 
                        i == PAL_FORMAT_D16_UNORM_S8_UINT  || 
                        i == PAL_FORMAT_D24_UNORM_S8_UINT) {
                        usages |= PAL_IMAGE_VIEW_USAGE_DEPTH;
                        usages |= PAL_IMAGE_VIEW_USAGE_STENCIL;
                    }
                    
                    if (usages == 0) {
                        usages = PAL_IMAGE_VIEW_USAGE_COLOR;
                    }

                    fmtInfo->viewUsages = usages;
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

static bool PAL_CALL _vkIsFormatSupported(
    PalAdapter* adapter,
    PalFormat format)
{
    VkPhysicalDevice phyDevice = (VkPhysicalDevice)adapter;
    VkFormatProperties props = {0};

    VkFormat fmt = palFormatToVk(format);
    s_Vk.getPhysicalDeviceFormatProperties(phyDevice, fmt, &props);
    if (props.optimalTilingFeatures != 0) {
        return true;
    }

    return false;
}

static PalImageUsages PAL_CALL _vkQueryFormatImageUsages(
    PalAdapter* adapter,
    PalFormat format)
{
    VkPhysicalDevice phyDevice = (VkPhysicalDevice)adapter;
    VkFormatProperties props = {0};

    VkFormat fmt = palFormatToVk(format);
    s_Vk.getPhysicalDeviceFormatProperties(phyDevice, fmt, &props);
    if (props.optimalTilingFeatures != 0) {
        return vkFeatureToPalUsage(props.optimalTilingFeatures);
    }

    return PAL_IMAGE_USAGE_UNDEFINED;
}

static PalImageViewUsages PAL_CALL _vkQueryFormatImageViewUsages(
    PalAdapter* adapter,
    PalFormat format)
{
    VkPhysicalDevice phyDevice = (VkPhysicalDevice)adapter;
    VkFormatProperties props = {0};

    VkFormat fmt = palFormatToVk(format);
    s_Vk.getPhysicalDeviceFormatProperties(phyDevice, fmt, &props);
    if (props.optimalTilingFeatures != 0) {
        // format supported. check if we have any depth or stencil component
        // Note: this is a hack
        PalImageViewUsages usages = 0;
        if (format == PAL_FORMAT_S8_UINT) {
            usages |= PAL_IMAGE_VIEW_USAGE_STENCIL;
        }

        if (format == PAL_FORMAT_D16_UNORM || format == PAL_FORMAT_D32_SFLOAT) {
            usages |= PAL_IMAGE_VIEW_USAGE_DEPTH;
        }

        if (format == PAL_FORMAT_D32_SFLOAT_S8_UINT || 
            format == PAL_FORMAT_D16_UNORM_S8_UINT  || 
            format == PAL_FORMAT_D24_UNORM_S8_UINT) {
            usages |= PAL_IMAGE_VIEW_USAGE_DEPTH;
            usages |= PAL_IMAGE_VIEW_USAGE_STENCIL;
        }
        
        if (usages == 0) {
            usages = PAL_IMAGE_VIEW_USAGE_COLOR;
        }

        return usages;
    }

    return PAL_IMAGE_VIEW_USAGE_UNDEFINED;
}

static PalResult PAL_CALL _vkGetImageMemoryRequirements(
    PalDevice* device,
    PalImage* image,
    PalMemoryRequirements* requirments)
{
    Device* _device = (Device*)device;
    Image* _image = (Image*)image;
    VkPhysicalDevice phyDevice = (VkPhysicalDevice)_device->phyDevice;

    VkPhysicalDeviceMemoryProperties memProps = {0};
    s_Vk.getPhysicalDeviceMemoryProperties(phyDevice, &memProps);

    VkMemoryRequirements memReq = {0};
    s_Vk.getImageMemoryRequirements(_device->handle, _image->handle, &memReq);
    requirments->alignment = (Uint64)memReq.alignment;
    requirments->size = (Uint64)memReq.size;

    requirments->memoryTypes[PAL_MEMORY_TYPE_GPU_ONLY] = false;
    requirments->memoryTypes[PAL_MEMORY_TYPE_CPU_UPLOAD] = false;
    requirments->memoryTypes[PAL_MEMORY_TYPE_CPU_READBACK] = false;

    for (int i = 0; i < memProps.memoryTypeCount; i++) {
        if (!(memReq.memoryTypeBits & (1 << i))) {
            // memory type not supported
            continue;
        }

        VkMemoryPropertyFlags prop = memProps.memoryTypes[i].propertyFlags;
        if (prop & VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT) {
            requirments->memoryTypes[PAL_MEMORY_TYPE_GPU_ONLY] = true;
        }

        if ((prop & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT) && 
             prop & VK_MEMORY_PROPERTY_HOST_COHERENT_BIT) {
            requirments->memoryTypes[PAL_MEMORY_TYPE_CPU_UPLOAD] = true;
        }

        if ((prop & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT) && 
             prop & VK_MEMORY_PROPERTY_HOST_CACHED_BIT) {
            requirments->memoryTypes[PAL_MEMORY_TYPE_CPU_READBACK] = true;
        }
    }

    return PAL_RESULT_SUCCESS;
}

static PalResult PAL_CALL _vkAllocateMemory(
    PalDevice* device,
    PalMemoryType type,
    Uint64 size,
    PalMemory** outMemory)
{
    Device* _device = (Device*)device;
    VkMemoryAllocateInfo allocateInfo = {0};
    allocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocateInfo.allocationSize = (VkDeviceSize)size;
    allocateInfo.memoryTypeIndex = _device->memoryTypeIndex[type];

    if (allocateInfo.memoryTypeIndex == -1) {
        // not supported
        return PAL_RESULT_MEMORY_TYPE_NOT_SUPPORTED;
    }

    VkDeviceMemory memory = nullptr;
    VkResult result = s_Vk.allocateMemory(
        _device->handle, 
        &allocateInfo, 
        &s_Vk.allocator, 
        &memory);

    if (result != VK_SUCCESS) {
        return vkResultToPal(result);
    }

    *outMemory = (PalMemory*)memory;
    return PAL_RESULT_SUCCESS;
}

void PAL_CALL _vkFreeMemory(
    PalDevice* device,
    PalMemory* memory)
{
    Device* _device = (Device*)device;
    VkDeviceMemory mem = (VkDeviceMemory)memory;
    s_Vk.freeMemory(_device->handle, mem, &s_Vk.allocator);
}

static PalResult PAL_CALL _vkBindImageMemory(
    PalDevice* device,
    PalImage* image,
    PalMemory* memory,
    Uint64 offset)
{
    Image* _image = (Image*)image;
    if (_image->belongsToSwapchain) {
        return PAL_RESULT_INVALID_GRAPHICS_OPERATION;
    }

    Device* _device = (Device*)device;
    VkDeviceMemory mem = (VkDeviceMemory)memory;
    s_Vk.bindImageMemory(_device->handle, _image->handle, mem, offset);
}

static PalResult PAL_CALL _vkCreateImageView(
    PalDevice* device,
    PalImage* image,
    const PalImageViewCreateInfo* info,
    PalImageView** outImageView)
{
    VkResult result = VK_SUCCESS;
    ImageView* imageView = nullptr;
    Device* _device = (Device*)device;
    Image* _image = (Image*)image;

    imageView = palAllocate(s_Graphics.allocator, sizeof(ImageView), 0);
    if (!imageView) {
        return PAL_RESULT_OUT_OF_MEMORY;
    }

    VkImageViewCreateInfo createInfo = {0};
    createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    createInfo.format = palFormatToVk(_image->info.format);
    createInfo.image = _image->handle;

    createInfo.subresourceRange.baseArrayLayer = info->startArrayLayer;
    createInfo.subresourceRange.baseMipLevel = info->startMipLevel;
    createInfo.subresourceRange.levelCount = info->mipLevelCount;
    createInfo.subresourceRange.layerCount = info->layerArrayCount;
    createInfo.viewType = palImageViewTypeToVk(info->type);

    VkImageAspectFlags aspectFlags = 0;
    if (info->usages & PAL_IMAGE_VIEW_USAGE_DEPTH) {
        aspectFlags |= VK_IMAGE_ASPECT_DEPTH_BIT;
    }

    if (info->usages & PAL_IMAGE_VIEW_USAGE_STENCIL) {
        aspectFlags |= VK_IMAGE_ASPECT_STENCIL_BIT;
    }

    if (info->usages & PAL_IMAGE_VIEW_USAGE_COLOR) {
        aspectFlags |= VK_IMAGE_ASPECT_COLOR_BIT;
    }

    createInfo.subresourceRange .aspectMask = aspectFlags;
    result = s_Vk.createImageView(
        _device->handle, 
        &createInfo, 
        &s_Vk.allocator, 
        &imageView->handle);

    if (result != VK_SUCCESS) {
        palFree(s_Graphics.allocator, imageView);
        return vkResultToPal(result);
    }

    imageView->device = _device;
    imageView->image = _image;
    imageView->type = createInfo.viewType;
    imageView->usages = info->usages;

    *outImageView = (PalImageView*)imageView;
    return PAL_RESULT_SUCCESS;
}

static void PAL_CALL _vkDestroyImageView(PalImageView* imageView)
{
    ImageView* _imageView = (ImageView*)imageView;
    s_Vk.destroyImageView(
        _imageView->device->handle, 
        _imageView->handle, 
        &s_Vk.allocator);

    palFree(s_Graphics.allocator, _imageView);
}

static PalResult PAL_CALL _vkQuerySwapchainCapabilities(
    PalAdapter* adapter,
    PalGfxWindow* window,
    PalSwapchainCapabilities* caps)
{
    Int32 formatCount = 0;
    Int32 modeCount = 0;
    VkSurfaceKHR surface = nullptr;
    VkSurfaceFormatKHR* formats = nullptr;
    VkPresentModeKHR* modes = nullptr;
    VkPhysicalDevice phyDevice = (VkPhysicalDevice)adapter;

    bool ret = vkCreateSurface(window, &surface);
    if (!ret) {
        return PAL_RESULT_INVALID_GRAPHICS_WINDOW;
    }

    s_Vk.getSurfacePresentModes(phyDevice, surface, &modeCount, nullptr);
    s_Vk.getSurfaceFormats(phyDevice, surface, &formatCount, nullptr);

    modes = palAllocate(
        s_Graphics.allocator, 
        sizeof(VkPresentModeKHR) * modeCount, 
        0);

    formats = palAllocate(
        s_Graphics.allocator, 
        sizeof(VkSurfaceFormatKHR) * formatCount, 
        0);

    if (!modes || !formats) {
        s_Vk.destroySurface(s_Vk.instance, surface, &s_Vk.allocator);
        return PAL_RESULT_OUT_OF_MEMORY;
    }

    s_Vk.getSurfacePresentModes(phyDevice, surface, &modeCount, modes);
    s_Vk.getSurfaceFormats(phyDevice, surface, &formatCount, formats);

    VkSurfaceCapabilitiesKHR surfaceCaps;
    s_Vk.getSurfaceCapabilities(phyDevice, surface, &surfaceCaps);
    caps->minImageWidth = surfaceCaps.minImageExtent.width;
    caps->minImageHeight = surfaceCaps.minImageExtent.height;
    caps->maxImageWidth = surfaceCaps.maxImageExtent.width;
    caps->maxImageHeight = surfaceCaps.maxImageExtent.height;

    caps->maxImageCount = surfaceCaps.maxImageCount;
    caps->minImageCount = surfaceCaps.minImageCount;
    caps->maxImageArrayLayers = surfaceCaps.maxImageArrayLayers;

    if (caps->maxImageCount == 0) {
        caps->maxImageCount = PAL_INFINITE;
    }

    // get supported composite alphas
    VkCompositeAlphaFlagsKHR alpha = surfaceCaps.supportedCompositeAlpha;
    caps->compositeAlphas[PAL_COMPOSITE_ALPHA_OPAQUE] = true;
    caps->compositeAlphas[PAL_COMPOSITE_ALPHA_POST_MULTIPLIED] = false;
    caps->compositeAlphas[PAL_COMPOSITE_ALPHA_PRE_MULTIPLIED] = false;

    if (alpha & VK_COMPOSITE_ALPHA_POST_MULTIPLIED_BIT_KHR) {
        caps->compositeAlphas[PAL_COMPOSITE_ALPHA_POST_MULTIPLIED] = true;
    }

    if (alpha & VK_COMPOSITE_ALPHA_PRE_MULTIPLIED_BIT_KHR) {
        caps->compositeAlphas[PAL_COMPOSITE_ALPHA_PRE_MULTIPLIED] = true;
    }

    // present modes
    caps->presentModes[PAL_PRESENT_MODE_FIFO] = true;
    caps->presentModes[PAL_PRESENT_MODE_MAILBOX] = false;
    caps->presentModes[PAL_PRESENT_MODE_IMMEDIATE] = false;

    for (int i = 0; i < modeCount; i++) {
        if (modes[i] == VK_PRESENT_MODE_IMMEDIATE_KHR) {
            caps->presentModes[PAL_PRESENT_MODE_IMMEDIATE] = true;
        }

        if (modes[i] == VK_PRESENT_MODE_MAILBOX_KHR) {
            caps->presentModes[PAL_PRESENT_MODE_MAILBOX] = true;
        }
    }

    // clang-format off

    // get format and colorspace
    caps->formats[PAL_SWAPCHAIN_FORMAT_RGBA16_FLOAT_HDR10] = false;
    caps->formats[PAL_SWAPCHAIN_FORMAT_RGBA8_UNORM_SRGB] = false;
    caps->formats[PAL_SWAPCHAIN_FORMAT_BGRA8_SRGB_SRGB] = false;
    caps->formats[PAL_SWAPCHAIN_FORMAT_BGRA8_UNORM_SRGB] = false;

    for (int i = 0; i < formatCount; i++) {
        VkSurfaceFormatKHR* fmt = &formats[i];
        if (fmt->format == VK_FORMAT_B8G8R8A8_UNORM) {
            // find its supported colorspace
            if (fmt->colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
                caps->formats[PAL_SWAPCHAIN_FORMAT_BGRA8_UNORM_SRGB] = true;
            }

        } else if (fmt->format == VK_FORMAT_B8G8R8A8_SRGB) {
            // find its supported colorspace
            if (fmt->colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
                caps->formats[PAL_SWAPCHAIN_FORMAT_BGRA8_SRGB_SRGB] = true;
            }

        } else if (fmt->format == VK_FORMAT_R8G8B8A8_UNORM) {
            // find its supported colorspace
            if (fmt->colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
                caps->formats[PAL_SWAPCHAIN_FORMAT_RGBA8_UNORM_SRGB] = true;
            }

        } else if (fmt->format == VK_FORMAT_R16G16B16A16_SFLOAT) {
            // find its supported colorspace
            if (fmt->colorSpace == VK_COLOR_SPACE_HDR10_ST2084_EXT) {   
                caps->formats[PAL_SWAPCHAIN_FORMAT_RGBA16_FLOAT_HDR10] = true;
            }
        }
    }

    // clang-format on

    palFree(s_Graphics.allocator, formats);
    palFree(s_Graphics.allocator, modes);
    s_Vk.destroySurface(s_Vk.instance, surface, &s_Vk.allocator);

    return PAL_RESULT_SUCCESS;
}

static PalResult PAL_CALL _vkCreateSwapchain(
    PalDevice* device,
    PalQueue* queue,
    PalGfxWindow* window,
    const PalSwapchainCreateInfo* info,
    PalSwapchain** outSwapchain)
{
    PalFormat imageFormat = 0;
    Swapchain* swapchain = nullptr;
    VkImage* images = nullptr;

    Queue* _queue = (Queue*)queue;
    Device* _device = (Device*)device;
    PhysicalQueue* phyQueue = _queue->phyQueue;

    // check if we enabled swapchain feature
    if (!_device->createSwapchain) {
        return PAL_RESULT_ADAPTER_FEATURE_NOT_SUPPORTED;
    }

    // check if the queue is a graphics queue before we check its family 
    // index for presentation support.
    if (_queue->usage != VK_QUEUE_GRAPHICS_BIT) {
        PAL_RESULT_INVALID_QUEUE;
    }

    swapchain = palAllocate(s_Graphics.allocator, sizeof(Swapchain), 0);
    if (!swapchain) {
        return PAL_RESULT_OUT_OF_MEMORY;
    }

    bool ret = vkCreateSurface(window, &swapchain->surface);
    if (!ret) {
        return PAL_RESULT_INVALID_GRAPHICS_WINDOW;
    }

    VkSwapchainCreateInfoKHR createInfo = {0};
    createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.surface = swapchain->surface;
    createInfo.imageArrayLayers = info->imageArrayLayerCount;
    createInfo.imageExtent.width = info->width;
    createInfo.imageExtent.height = info->height;
    createInfo.minImageCount = info->imageCount;
    createInfo.clipped = (VkBool32)info->clipped;
    createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    createInfo.preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;

    // present modes
    createInfo.presentMode = VK_PRESENT_MODE_FIFO_KHR;
    if (info->presentMode == PAL_PRESENT_MODE_IMMEDIATE) {
        createInfo.presentMode = VK_PRESENT_MODE_IMMEDIATE_KHR;

    } else if (info->presentMode == PAL_PRESENT_MODE_MAILBOX) {
        createInfo.presentMode = VK_PRESENT_MODE_MAILBOX_KHR;
    }

    // composite alpha
    createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    if (info->compositeAlpha == PAL_COMPOSITE_ALPHA_POST_MULTIPLIED) {
        createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_POST_MULTIPLIED_BIT_KHR;

    } else if (info->compositeAlpha == PAL_COMPOSITE_ALPHA_PRE_MULTIPLIED) {
        createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_PRE_MULTIPLIED_BIT_KHR;
    }

    // format and colorspace
    createInfo.imageFormat = VK_FORMAT_B8G8R8A8_UNORM;
    createInfo.imageColorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
    imageFormat = PAL_FORMAT_B8G8R8A8_UNORM;

    if (info->format == PAL_SWAPCHAIN_FORMAT_BGRA8_SRGB_SRGB) {
        createInfo.imageFormat = VK_FORMAT_B8G8R8A8_SRGB;
        createInfo.imageColorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
        imageFormat = PAL_FORMAT_B8G8R8A8_SRGB;

    } else if (info->format == PAL_SWAPCHAIN_FORMAT_RGBA8_UNORM_SRGB) {
        createInfo.imageFormat = VK_FORMAT_R8G8B8A8_UNORM;
        createInfo.imageColorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
        imageFormat = PAL_FORMAT_R8G8B8A8_UNORM;

    } else if (info->format == PAL_SWAPCHAIN_FORMAT_RGBA16_FLOAT_HDR10) {
        createInfo.imageFormat = VK_FORMAT_R16G16B16A16_SFLOAT;
        createInfo.imageColorSpace = VK_COLOR_SPACE_HDR10_ST2084_EXT;
        imageFormat = PAL_FORMAT_R16G16B16A16_SFLOAT;
    }

    // create swapchain
    VkResult result = _device->createSwapchain(
        _device->handle, 
        &createInfo, 
        &s_Vk.allocator, 
        &swapchain->handle);

    if (result != VK_SUCCESS) {
        s_Vk.destroySurface(
            s_Vk.instance, 
            swapchain->surface, 
            &s_Vk.allocator);

        palFree(s_Graphics.allocator, swapchain);
        return vkResultToPal(result);
    }

    // get and cache all images
    Int32 count = 0;
    result = _device->getSwapchainImages(
        _device->handle,
        swapchain->handle, 
        &count,
        nullptr);

    swapchain->images = palAllocate(
        s_Graphics.allocator, 
        sizeof(Image) * count, 
        0);

    images = palAllocate(
        s_Graphics.allocator, 
        sizeof(VkImage) * count, 
        0);

    if (!swapchain->images || !images) {
        _device->destroySwapchain(
            _device->handle,
            swapchain->handle,
            &s_Vk.allocator
        );

        s_Vk.destroySurface(
            s_Vk.instance, 
            swapchain->surface, 
            &s_Vk.allocator);

        palFree(s_Graphics.allocator, swapchain);
        return PAL_RESULT_OUT_OF_MEMORY;
    }
    
    _device->getSwapchainImages(
        _device->handle, 
        swapchain->handle, 
        &count,
        images);

    // fill all images with the creatio info
    for (int i = 0; i < count; i++) {
        Image* image = &swapchain->images[i];
        image->belongsToSwapchain = true;
        image->device = _device;
        image->handle = images[i];

        image->info.depthOrArraySize = createInfo.imageArrayLayers;
        image->info.format = imageFormat;
        image->info.usages = PAL_IMAGE_USAGE_COLOR_ATTACHEMENT;
        image->info.height = createInfo.imageExtent.height;
        image->info.width = createInfo.imageExtent.width;
        image->info.mipLevelCount = 1; 
        image->info.samples = 1; // swapchain images are not multisampled
        image->info.type = PAL_IMAGE_TYPE_2D;
    }
    
    swapchain->device = _device;
    swapchain->imageCount = count;

    *outSwapchain = (PalSwapchain*)swapchain;
    return PAL_RESULT_SUCCESS;
}

static void PAL_CALL vkDestroySwapchain(PalSwapchain* swapchain)
{
    Swapchain* _swapchain = (Swapchain*)swapchain;
    _swapchain->device->destroySwapchain(
        _swapchain->device->handle,
        _swapchain->handle,
        &s_Vk.allocator
    );

    s_Vk.destroySurface(s_Vk.instance, _swapchain->surface, &s_Vk.allocator);
    palFree(s_Graphics.allocator, _swapchain->images);
    palFree(s_Graphics.allocator, _swapchain);
}

static Uint32 PAL_CALL _vkGetSwapchainImageCount(PalSwapchain* swapchain)
{
    Swapchain* _swapchain = (Swapchain*)swapchain;
    return _swapchain->imageCount;
}

static PalImage* PAL_CALL _vkGetSwapchainImage(
    PalSwapchain* swapchain,
    Int32 index)
{
    Swapchain* _swapchain = (Swapchain*)swapchain;
    if (index > _swapchain->imageCount) {
        return nullptr;
    }

    return (PalImage*)&_swapchain->images[index];
}

static PalGPUBackend s_VkBackend = {
    // adapter
    .enumerateAdapters = _vkEnumerateAdapters,
    .getAdapterInfo = _vkGetAdapterInfo,
    .getAdapterCapabilities = _vkGetAdapterCapabilities,

    // device
    .createDevice = _vkCreateDevice,
    .destroyDevice = _vkDestroyDevice,

    // queue
    .createQueue = _vkCreateQueue,
    .destroyQueue = _vkDestroyQueue,
    .canQueuePresent = _vkCanQueuePresent,

    // image
    .createImage = _vkCreateImage,
    .destroyImage = _vkDestroyImage,
    .getImageInfo = _vkGetImageInfo,
    .enumerateFormats = _vkEnumerateFormats,
    .isFormatSupported = _vkIsFormatSupported,
    .queryFormatUsages = _vkQueryFormatImageUsages,
    .queryFormatViewUsages = _vkQueryFormatImageViewUsages,
    .getImageMemoryRequirements = _vkGetImageMemoryRequirements,

    // memory
    .allocate = _vkAllocateMemory,
    .free = _vkFreeMemory,
    .bindImageMemory = _vkBindImageMemory,

    // image view
    .createImageView = _vkCreateImageView,
    .destroyImageView = _vkDestroyImageView,

    // swapchain
    .querySwapchainCapabilities = _vkQuerySwapchainCapabilities,
    .createSwapchain = _vkCreateSwapchain,
    .destroySwapchain = vkDestroySwapchain,
    .getSwapchainImageCount = _vkGetSwapchainImageCount,
    .getSwapchainImage = _vkGetSwapchainImage,
};

#endif // PAL_HAS_VULKAN

// ==================================================
// Public API
// ==================================================

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

    s_Graphics.allocator = allocator;
    s_Graphics.maxHandleData = 32;
    s_Graphics.handleData = palAllocate(
        s_Graphics.allocator, 
        sizeof(HandleData) * s_Graphics.maxHandleData, 
        0);

    if (!s_Graphics.handleData) {
        return PAL_RESULT_OUT_OF_MEMORY;
    }

#if PAL_HAS_VULKAN
    PalResult ret = vkInitGraphics(enableDebugLayer);
    if (ret != PAL_RESULT_SUCCESS) {
        return ret;
    }

    BackendData* backend = &s_Graphics.backends[s_Graphics.backendCount++];
    backend->base = &s_VkBackend;
    backend->count = 0;
    backend->startIndex = 0;
#endif // PAL_HAS_VULKAN

    s_Graphics.initialized = true;
    return PAL_RESULT_SUCCESS;
}

void PAL_CALL palShutdownGraphics()
{
    if (!s_Graphics.initialized) {
        return;
    }

#if PAL_HAS_VULKAN
    vkShutdownGraphics();
#endif // PAL_HAS_VULKAN

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

    HandleData* data = findHandleData(adapter);
    if (data) {
        return data->backend->getAdapterInfo(adapter, info);
    }

    return PAL_RESULT_INVALID_ADAPTER;
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

    HandleData* data = findHandleData(adapter);
    if (data) {
        return data->backend->getAdapterCapabilities(adapter, caps);
    }

    return PAL_RESULT_INVALID_ADAPTER;
}

PalResult PAL_CALL palAddGPUBackend(const PalGPUBackend* backend)
{
    if (s_Graphics.initialized) {
        return PAL_RESULT_INVALID_GRAPHICS_BACKEND;
    }

    // // check if all the function pointers are set
    // // clang-format off
    // if (!backend->enumerateGPUAdapters         || 
    //     !backend->getGPUAdapterInfo            ||
    //     !backend->getGPUAdapterCapabilities    ||
    //     !backend->createGPUDevice              ||
    //     !backend->destroyGPUDevice             ||
    //     !backend->createGPUCommandQueue        ||
    //     !backend->destroyGPUCommandQueue       ||
    //     !backend->canCommandQueuePresent       ||
    //     !backend->createSwapchain              ||
    //     !backend->destroySwapchain             ||
    //     !backend->querySwapchainCapabilities   ||
    //     !backend->getSwapchainBufferCount      ||
    //     !backend->createRenderTargetView       ||
    //     !backend->destroyRenderTargetView      ||
    //     !backend->queryRenderPassCapabilities  ||
    //     !backend->createRenderPass             ||
    //     !backend->destroyRenderPass) {
    //     return PAL_RESULT_INVALID_GPU_BACKEND;
    // }
    // // clang-format on

    BackendData* attached = &s_Graphics.backends[s_Graphics.backendCount++];
    attached->base = backend;
    attached->startIndex = 0;
    attached->count = 0;

    return PAL_RESULT_SUCCESS;
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
    HandleData* adapterData = findHandleData(adapter);
    if (!adapterData) {
        return PAL_RESULT_INVALID_ADAPTER;
    }

    PalDevice* device = nullptr;
    PalResult ret;
    ret = adapterData->backend->createDevice(adapter, features, &device);
    if (ret != PAL_RESULT_SUCCESS) {
        return ret;
    }

    // create a slot for the created device
    HandleData* deviceData = getFreeHandleData();
    if (!deviceData) {
        return PAL_RESULT_OUT_OF_MEMORY;
    }

    deviceData->backend = adapterData->backend;
    deviceData->handle = device;

    *outDevice = device;
    return PAL_RESULT_SUCCESS;
}

void PAL_CALL palDestroyDevice(PalDevice* device)
{
    if (s_Graphics.initialized && device) {
        HandleData* data = findHandleData(device);
        if (data) {
            data->backend->destroyDevice(device);
            data->used = false;
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

    HandleData* data = findHandleData(device);
    if (!data) {
        return PAL_RESULT_INVALID_GRAPHICS_DEVICE;
    }

    PalQueue* queue = nullptr;
    PalResult ret;
    ret = data->backend->createQueue(
        device,
        type,
        &queue);

    if (ret != PAL_RESULT_SUCCESS) {
        return ret;
    }

    // create a slot for the created queue
    HandleData* queueData = getFreeHandleData();
    if (!queueData) {
        return PAL_RESULT_OUT_OF_MEMORY;
    }

    queueData->backend = data->backend;
    queueData->handle = queue;

    *outQueue = queue;
    return PAL_RESULT_SUCCESS;
}

void PAL_CALL palDestroyQueue(PalQueue* queue)
{
    if (s_Graphics.initialized && queue) {
        HandleData* data = findHandleData(queue);
        if (data) {
            data->backend->destroyQueue(queue);
            data->used = false;
        }
    }
}

bool PAL_CALL palCanQueuePresent(
    PalQueue* queue, 
    PalGfxWindow* window)
{
    if (s_Graphics.initialized && queue) {
        HandleData* data = findHandleData(queue);
        if (data) {
            return data->backend->canQueuePresent(queue, window);
        }
        return false;
    }
    return false;
}

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

    HandleData* data = findHandleData(device);
    if (!data) {
        return PAL_RESULT_INVALID_GRAPHICS_DEVICE;
    }

    PalImage* image = nullptr;
    PalResult ret;
    ret = data->backend->createImage(
        device,
        info,
        &image);

    if (ret != PAL_RESULT_SUCCESS) {
        return ret;
    }

    // create a slot for the created image
    HandleData* imageData = getFreeHandleData();
    if (!imageData) {
        return PAL_RESULT_OUT_OF_MEMORY;
    }

    imageData->backend = data->backend;
    imageData->handle = image;

    *outImage = image;
    return PAL_RESULT_SUCCESS;
}

void PAL_CALL palDestroyImage(PalImage* image)
{
    if (s_Graphics.initialized && image) {
        HandleData* data = findHandleData(image);
        if (data) {
            data->backend->destroyImage(image);
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

    HandleData* data = findHandleData(image);
    if (!data) {
        return PAL_RESULT_INVALID_GRAPHICS_IMAGE;
    }

    return data->backend->getImageInfo(image, info);
}

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

    HandleData* adapterData = findHandleData(adapter);
    if (!adapterData) {
        return PAL_RESULT_INVALID_ADAPTER;
    }

    return adapterData->backend->enumerateFormats(adapter, count, outFormats);
}

bool PAL_CALL palIsFormatSupported(
    PalAdapter* adapter,
    PalFormat format)
{
    if (!s_Graphics.initialized || !adapter) {
        return false;
    }

    HandleData* adapterData = findHandleData(adapter);
    if (!adapterData) {
        return false;
    }

    return adapterData->backend->isFormatSupported(adapter, format);
}

PalImageUsages PAL_CALL palQueryFormatImageUsages(
    PalAdapter* adapter,
    PalFormat format)
{
    if (!s_Graphics.initialized || !adapter) {
        return PAL_IMAGE_USAGE_UNDEFINED;
    }

    HandleData* adapterData = findHandleData(adapter);
    if (!adapterData) {
        return PAL_IMAGE_USAGE_UNDEFINED;
    }

    return adapterData->backend->queryFormatUsages(adapter, format);
}

PalImageViewUsages PAL_CALL palQueryFormatImageViewUsages(
    PalAdapter* adapter,
    PalFormat format)
{
    if (!s_Graphics.initialized || !adapter) {
        return PAL_IMAGE_VIEW_USAGE_UNDEFINED;
    }

    HandleData* adapterData = findHandleData(adapter);
    if (!adapterData) {
        return PAL_IMAGE_VIEW_USAGE_UNDEFINED;
    }

    return adapterData->backend->queryFormatViewUsages(adapter, format);
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

    HandleData* deviceData = findHandleData(device);
    if (!deviceData) {
        return PAL_RESULT_INVALID_GRAPHICS_BACKEND;
    }

    return deviceData->backend->getImageMemoryRequirements(
        device, 
        image, 
        requirements);
}

// ==================================================
// Memory
// ==================================================

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

    HandleData* deviceData = findHandleData(device);
    if (!deviceData) {
        return PAL_RESULT_INVALID_GRAPHICS_DEVICE;
    }

    return deviceData->backend->allocate(device, type, size, outMemory);
}

void PAL_CALL palFreeMemory(
    PalDevice* device,
    PalMemory* memory)
{
    if (s_Graphics.initialized && device && memory) {
        HandleData* data = findHandleData(device);
        if (data) {
            data->backend->free(device, memory);
        }
    }
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

    HandleData* deviceData = findHandleData(device);
    if (!deviceData) {
        return PAL_RESULT_INVALID_GRAPHICS_DEVICE;
    }

    return deviceData->backend->bindImageMemory(
        device, 
        image, 
        memory, 
        offset);
}

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

    HandleData* data = findHandleData(device);
    if (!data) {
        return PAL_RESULT_INVALID_GRAPHICS_DEVICE;
    }

    PalImageView* imageView = nullptr;
    PalResult ret;
    ret = data->backend->createImageView(
        device,
        image,
        info,
        &imageView);

    if (ret != PAL_RESULT_SUCCESS) {
        return ret;
    }

    // create a slot for the created image view
    HandleData* imageViewData = getFreeHandleData();
    if (!imageViewData) {
        return PAL_RESULT_OUT_OF_MEMORY;
    }

    imageViewData->backend = data->backend;
    imageViewData->handle = imageView;

    *outImageView = imageView;
    return PAL_RESULT_SUCCESS;
}

void PAL_CALL palDestroyImageView(PalImageView* imageView)
{
    if (s_Graphics.initialized && imageView) {
        HandleData* data = findHandleData(imageView);
        if (data) {
            data->backend->destroyImageView(imageView);
            data->used = false;
        }
    }
}

// ==================================================
// Swapchain
// ==================================================

PalResult PAL_CALL palQuerySwapchainCapabilities(
    PalAdapter* adapter,
    PalGfxWindow* window,
    PalSwapchainCapabilities* caps)
{
    if (!s_Graphics.initialized) {
        return PAL_RESULT_GRAPHICS_NOT_INITIALIZED;
    }

    if (!adapter || !window || !caps) {
        return PAL_RESULT_NULL_POINTER;
    }

    HandleData* adapterData = findHandleData(adapter);
    if (!adapterData) {
        return PAL_RESULT_INVALID_ADAPTER;
    }

    return adapterData->backend->querySwapchainCapabilities(
        adapter,
        window,
        caps);
}

PalResult PAL_CALL palCreateSwapchain(
    PalDevice* device,
    PalQueue* queue,
    PalGfxWindow* window,
    const PalSwapchainCreateInfo* info,
    PalSwapchain** outSwapchain)
{
    if (!s_Graphics.initialized) {
        return PAL_RESULT_GRAPHICS_NOT_INITIALIZED;
    }

    if (!device || !queue || !window || !info || !outSwapchain) {
        return PAL_RESULT_NULL_POINTER;
    }

    HandleData* deviceData = findHandleData(device);
    if (!deviceData) {
        return PAL_RESULT_INVALID_QUEUE;
    }

    PalResult ret;
    PalSwapchain* swapchain = nullptr;
    ret = deviceData->backend->createSwapchain(
        device, 
        queue, 
        window, 
        info, 
        &swapchain);

    if (ret != PAL_RESULT_SUCCESS) {
        return ret;
    }

    // create a slot for the created swapchain
    HandleData* swapchainData = getFreeHandleData();
    if (!swapchainData) {
        return PAL_RESULT_OUT_OF_MEMORY;
    }
    swapchainData->backend = deviceData->backend;
    swapchainData->handle = swapchain;

    *outSwapchain = swapchain;
    return PAL_RESULT_SUCCESS;
}

void PAL_CALL palDestroySwapchain(PalSwapchain* swapchain)
{
    if (s_Graphics.initialized && swapchain) {
        HandleData* data = findHandleData(swapchain);
        if (data) {
            data->backend->destroySwapchain(swapchain);
            data->used = false;
        }
    }
}

Uint32 PAL_CALL palGetSwapchainImageCount(PalSwapchain* swapchain)
{
    if (!s_Graphics.initialized || !swapchain) {
        return 0;
    }

    HandleData* swapchainData = findHandleData(swapchain);
    if (!swapchainData) {
        return 0;
    }

    return swapchainData->backend->getSwapchainImageCount(swapchain);
}

PalImage* PAL_CALL palGetSwapchainImage(
    PalSwapchain* swapchain,
    Int32 index)
{
    if (!s_Graphics.initialized || !swapchain || index < 0) {
        return nullptr;
    }

    HandleData* swapchainData = findHandleData(swapchain);
    if (!swapchainData) {
        return nullptr;
    }

    PalImage* image = swapchainData->backend->getSwapchainImage(
        swapchain, 
        index);

    // check if the user has already requested for the image
    HandleData* imageData = findHandleData(image);
    if (!imageData) {
        // create a new slot
        imageData = getFreeHandleData();
        if (!imageData) {
            return nullptr;
        }
    }

    imageData->backend = swapchainData->backend;
    imageData->handle = image;
    return image;
}