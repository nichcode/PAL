
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

// HACK: Needed to determine display type if on linux
#ifdef _WIN32
#define VK_LIB_NAME ""

#elif defined(__linux__)
struct wl_display;
struct wl_surface;
typedef struct _XDisplay Display;
typedef unsigned long Window;
typedef unsigned long VisualID;
typedef int (*wl_display_get_fd_fn)(struct wl_display*);

#include <vulkan/vulkan_xlib.h>
#include <vulkan/vulkan_wayland.h>

#define VK_LIB_NAME "libvulkan.so"

#else
// Android
#define VK_LIB_NAME ""

#endif // _WIN32

// ==================================================
// Typedefs, enums and structs
// ==================================================

#define VK_WIN32_PLATFORM 1
#define VK_XLIB_PLATFORM 2
#define VK_WAYLAND_PLATFORM 3
#define MAX_ATTACHMENTS 32

typedef struct {
    bool hasDebug;
    void* handle;
    VkInstance instance;

#ifdef __linux__
    // HACK: for display testing
    void* libWayland;
    wl_display_get_fd_fn getDisplayFd;
#endif // __linux__
    
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
    PFN_vkCreateShaderModule createShader;
    PFN_vkDestroyShaderModule destroyShader;
    PFN_vkCreateRenderPass createRenderPass;
    PFN_vkDestroyRenderPass destroyRenderPass;
    PFN_vkCreateFramebuffer createFramebuffer;
    PFN_vkDestroyFramebuffer destroyFramebuffer;

    PFN_vkCreateCommandPool createCommandPool;
    PFN_vkDestroyCommandPool destroyCommandPool;
    PFN_vkAllocateCommandBuffers createCommandBuffer;
    PFN_vkFreeCommandBuffers destroyCommandBuffer;
    PFN_vkCreateFence createFence;
    PFN_vkDestroyFence destroyFence;
    PFN_vkResetFences resetFence;
    PFN_vkWaitForFences waitFence;
    PFN_vkGetFenceStatus isFenceSignaled;
    PFN_vkCreateSemaphore createSemaphore;
    PFN_vkDestroySemaphore destroySemaphore;
    PFN_vkWaitSemaphores waitSemaphores;
    PFN_vkSignalSemaphore signalSemaphore;
    PFN_vkGetSemaphoreCounterValue getSemaphoreValue;

    PFN_vkBeginCommandBuffer cmdBegin;
    PFN_vkEndCommandBuffer cmdEnd;
    PFN_vkCmdExecuteCommands cmdExecuteCommandBuffer;
    PFN_vkQueueSubmit queueSubmit;
    PFN_vkCmdBeginRenderPass cmdBeginRenderPass;
    PFN_vkCmdEndRenderPass cmdEndRenderPass;

    PFN_vkCreateWaylandSurfaceKHR createWaylandSurface;
    PFN_vkGetPhysicalDeviceWaylandPresentationSupportKHR checkWaylandPresentSupport;
    PFN_vkCreateXlibSurfaceKHR createXlibSurface;
    PFN_vkGetPhysicalDeviceXlibPresentationSupportKHR checkXlibPresentSupport;

    PFN_vkDestroySurfaceKHR destroySurface;
    PFN_vkGetPhysicalDeviceSurfaceCapabilitiesKHR getSurfaceCapabilities;
    PFN_vkGetPhysicalDeviceSurfaceFormatsKHR getSurfaceFormats;
    PFN_vkGetPhysicalDeviceSurfacePresentModesKHR getSurfacePresentModes;

    VkAllocationCallbacks vkAllocator;
    const PalAllocator* allocator;
} Vulkan;

typedef struct {
    Int32 familyIndex;
    VkPhysicalDevice phyDevice;
    VkQueue handle;
    VkQueueFlags usages;
    VkQueueFlags usedUsages;
} PhysicalQueue;

struct PalDevice {
    PalAdapterFeatures features;
    Int32 queueCount;
    Int32 gpuOnlyMemoryIndex;
    Int32 cpuUploadMemoryIndex;
    Int32 cpuReadbackMemoryIndex;
    VkPhysicalDevice phyDevice;
    VkDevice handle;
    PhysicalQueue* phyQueues;
    PFN_vkCreateSwapchainKHR createSwapchain;
    PFN_vkDestroySwapchainKHR destroySwapchain;
    PFN_vkGetSwapchainImagesKHR getSwapchainImages;
    PFN_vkAcquireNextImageKHR acquireNextImage;
    PFN_vkQueuePresentKHR queuePresent;
};

struct PalQueue {
    VkQueueFlags usage;
    PalDevice* device;
    PhysicalQueue* phyQueue;
};

struct PalImage {
    bool belongsToSwapchain;
    Int32 index;
    PalDevice* device;
    VkImage handle;
    PalImageInfo info;
};

struct PalImageView {
    VkImageViewType type;
    PalImageViewUsages usages;
    PalDevice* device;
    PalImage* image;
    VkImageView handle;
};

struct PalSwapchain {
    Uint32 imageCount;
    PalDevice* device;
    PalQueue* queue;
    VkSurfaceKHR surface;
    VkSwapchainKHR handle;
    PalImage* images;
};

struct PalShader {
    PalShaderType type;
    PalDevice* device;
    VkShaderModule handle;
    VkPipelineShaderStageCreateInfo info;
};

struct PalRenderPass {
    bool hasDepth;
    Uint32 attachmentCount;
    Uint32 width;
    Uint32 height;
    PalDevice* device;
    VkRenderPass handle;
    VkFramebuffer framebuffer;
};

struct PalCommandPool {
    PalDevice* device;
    VkCommandPool handle;
};

struct PalCommandBuffer {
    bool primary;
    PalDevice* device;
    VkCommandPool pool;
    VkCommandBuffer handle;
};

struct PalFence {
    PalDevice* device;
    VkFence handle;
};

struct PalSemaphore {
    bool isTimeline;
    PalDevice* device;
    VkSemaphore handle;
};

static Vulkan s_Vk = {0};

// ==================================================
// Helper Functions
// ==================================================

static Uint32 checkPlatform(struct wl_display* display) 
{
#ifdef _WIN32
#elif defined(__linux__)
    if (!s_Vk.libWayland) {
        return VK_XLIB_PLATFORM;
    }

    int fd = s_Vk.getDisplayFd(display);
    if (fd <= 0 || fd > 1024) { // fds are usaually 0-30 but this is fine
        return VK_XLIB_PLATFORM;
    }
    return VK_WAYLAND_PLATFORM;
#else
    // Android
#endif // _WIN32
}

static bool createSurface(
    PalGraphicsWindow* window, 
    VkSurfaceKHR* outSurface)
{
    Uint32 platform = checkPlatform(window->display);
    if (platform == VK_WIN32_PLATFORM) {

    } else if (platform == VK_WAYLAND_PLATFORM) {
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
            &s_Vk.vkAllocator, 
            &surface);

        if (result != VK_SUCCESS) {
            return false;
        }

        *outSurface = surface;
        return true;
        
    } else if (platform == VK_XLIB_PLATFORM) {
        
    }
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
            return PAL_RESULT_INVALID_DRIVER;

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

static VkSampleCountFlags samplesToVk(PalSampleCount count)
{
    switch (count) {
        case PAL_SAMPLE_COUNT_2:
            return VK_SAMPLE_COUNT_2_BIT;
        
        case PAL_SAMPLE_COUNT_4:
            return VK_SAMPLE_COUNT_4_BIT;

        case PAL_SAMPLE_COUNT_8:
            return VK_SAMPLE_COUNT_8_BIT;

        case PAL_SAMPLE_COUNT_16:
            return VK_SAMPLE_COUNT_16_BIT;

        case PAL_SAMPLE_COUNT_32:
            return VK_SAMPLE_COUNT_32_BIT;

        case PAL_SAMPLE_COUNT_64:
            return VK_SAMPLE_COUNT_64_BIT;
    }

    return VK_SAMPLE_COUNT_1_BIT;
}

static PalSampleCount vkSamplesToSamples(VkSampleCountFlags count)
{
    if (count & VK_SAMPLE_COUNT_2_BIT) {
        return PAL_SAMPLE_COUNT_2;

    } else if (count & VK_SAMPLE_COUNT_4_BIT) {
        return PAL_SAMPLE_COUNT_4;

    } else if (count & VK_SAMPLE_COUNT_8_BIT) {
        return PAL_SAMPLE_COUNT_8;

    } else if (count & VK_SAMPLE_COUNT_16_BIT) {
        return PAL_SAMPLE_COUNT_16;

    } else if (count & VK_SAMPLE_COUNT_32_BIT) {
        return PAL_SAMPLE_COUNT_32;

    } else if (count & VK_SAMPLE_COUNT_64_BIT) {
        return PAL_SAMPLE_COUNT_64;
    }

    return PAL_SAMPLE_COUNT_1;
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
    return palAllocate(s_Vk.allocator, size, alignment);
}

static void vkFree(
    void* pUserData,
    void* ptr)
{
    palFree(s_Vk.allocator, ptr);
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
        void* memory = palAllocate(s_Vk.allocator, size, alignment);
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

// ==================================================
// Adapter
// ==================================================

PalResult PAL_CALL initGraphicsVk(
    bool enableDebugLayer,
    const PalAllocator* allocator)
{
    s_Vk.libWayland = nullptr;
    s_Vk.libWayland = dlopen("libwayland-client.so.0", RTLD_LAZY);
    if (s_Vk.libWayland) {
        s_Vk.getDisplayFd = (wl_display_get_fd_fn)dlsym(
            s_Vk.libWayland, 
            "wl_display_get_fd");
    }

    // load vulkan
    s_Vk.handle = dlopen(VK_LIB_NAME, RTLD_LAZY);
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

    s_Vk.createShader = (PFN_vkCreateShaderModule)dlsym(
        s_Vk.handle, 
        "vkCreateShaderModule");

    s_Vk.destroyShader = (PFN_vkDestroyShaderModule)dlsym(
        s_Vk.handle, 
        "vkDestroyShaderModule");

    s_Vk.createRenderPass = (PFN_vkCreateRenderPass)dlsym(
        s_Vk.handle, 
        "vkCreateRenderPass");

    s_Vk.destroyRenderPass = (PFN_vkDestroyRenderPass)dlsym(
        s_Vk.handle, 
        "vkDestroyRenderPass");

    s_Vk.createFramebuffer = (PFN_vkCreateFramebuffer)dlsym(
        s_Vk.handle, 
        "vkCreateFramebuffer");

    s_Vk.destroyFramebuffer = (PFN_vkDestroyFramebuffer)dlsym(
        s_Vk.handle, 
        "vkDestroyFramebuffer");

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

    s_Vk.createCommandPool = (PFN_vkCreateCommandPool)dlsym(
        s_Vk.handle, 
        "vkCreateCommandPool");

    s_Vk.destroyCommandPool = (PFN_vkDestroyCommandPool)dlsym(
        s_Vk.handle, 
        "vkDestroyCommandPool");

    s_Vk.createCommandBuffer = (PFN_vkAllocateCommandBuffers)dlsym(
        s_Vk.handle, 
        "vkAllocateCommandBuffers");

    s_Vk.destroyCommandBuffer = (PFN_vkFreeCommandBuffers)dlsym(
        s_Vk.handle, 
        "vkFreeCommandBuffers");

    s_Vk.createFence = (PFN_vkCreateFence)dlsym(
        s_Vk.handle, 
        "vkCreateFence");

    s_Vk.destroyFence = (PFN_vkDestroyFence)dlsym(
        s_Vk.handle, 
        "vkDestroyFence");

    s_Vk.resetFence = (PFN_vkResetFences)dlsym(
        s_Vk.handle, 
        "vkResetFences");

    s_Vk.waitFence = (PFN_vkWaitForFences)dlsym(
        s_Vk.handle, 
        "vkWaitForFences");

    s_Vk.isFenceSignaled = (PFN_vkGetFenceStatus)dlsym(
        s_Vk.handle, 
        "vkGetFenceStatus");

    s_Vk.createSemaphore = (PFN_vkCreateSemaphore)dlsym(
        s_Vk.handle, 
        "vkCreateSemaphore");

    s_Vk.destroySemaphore = (PFN_vkDestroySemaphore)dlsym(
        s_Vk.handle, 
        "vkDestroySemaphore");

    s_Vk.waitSemaphores = (PFN_vkWaitSemaphores)dlsym(
        s_Vk.handle, 
        "vkWaitSemaphores");

    s_Vk.signalSemaphore = (PFN_vkSignalSemaphore)dlsym(
        s_Vk.handle, 
        "vkSignalSemaphore");

    s_Vk.getSemaphoreValue = (PFN_vkGetSemaphoreCounterValue)dlsym(
        s_Vk.handle, 
        "vkGetSemaphoreCounterValue");

    s_Vk.cmdBegin = (PFN_vkBeginCommandBuffer)dlsym(
        s_Vk.handle, 
        "vkBeginCommandBuffer");

    s_Vk.cmdEnd = (PFN_vkEndCommandBuffer)dlsym(
        s_Vk.handle, 
        "vkEndCommandBuffer");

    s_Vk.cmdExecuteCommandBuffer = (PFN_vkCmdExecuteCommands)dlsym(
        s_Vk.handle, 
        "vkCmdExecuteCommands");

    s_Vk.cmdBeginRenderPass = (PFN_vkCmdBeginRenderPass)dlsym(
        s_Vk.handle, 
        "vkCmdBeginRenderPass");

    s_Vk.cmdEndRenderPass = (PFN_vkCmdEndRenderPass)dlsym(
        s_Vk.handle, 
        "vkCmdEndRenderPass");

    s_Vk.queueSubmit = (PFN_vkQueueSubmit)dlsym(
        s_Vk.handle, 
        "vkQueueSubmit");

    // clang-format on

    // get version
    bool versionFallback = false;
    Uint32 version = 0;
    if (s_Vk.enumerateInstanceVersion) {
        s_Vk.enumerateInstanceVersion(&version);
        if (version <= VK_API_VERSION_1_0) {
            versionFallback = true;
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
            s_Vk.allocator, 
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

        palFree(s_Vk.allocator, props);
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
        s_Vk.allocator, 
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

    palFree(s_Vk.allocator, extensionProps);

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
    s_Vk.vkAllocator.pfnAllocation = vkAlloc;
    s_Vk.vkAllocator.pfnFree = vkFree;
    s_Vk.vkAllocator.pfnReallocation = vkRealloc;

    VkInstance instance = nullptr;
    VkResult result = s_Vk.createInstance(
        &instanceCreateInfo, 
        &s_Vk.vkAllocator, 
        &instance);

    if (result != VK_SUCCESS) {
        return vkResultToPal(result);
    }

    // clang-format off
    if (versionFallback) {
        // load get physical device properties2 proc if we are on version 1.0
        s_Vk.getPhysicalDeviceFeatures2 = 
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

PalResult PAL_CALL shutdownGraphicsVk()
{
    s_Vk.destroyInstance(s_Vk.instance, &s_Vk.vkAllocator);
    dlclose(s_Vk.handle);
    if (s_Vk.libWayland) {
        dlclose(s_Vk.libWayland);
    }
    memset(&s_Vk, 0, sizeof(s_Vk));
}

PalResult PAL_CALL enumerateVkAdapters(
    Int32* count,
    PalAdapter** outAdapters)
{
    int deviceCount = 0;
    int maxCount = outAdapters ? *count : 0;
    VkResult result;

    result = s_Vk.enumeratePhysicalDevices(
        s_Vk.instance, 
        &deviceCount, 
        nullptr);

    if (result != VK_SUCCESS) {
        return PAL_RESULT_PLATFORM_FAILURE;
    }

    if (!outAdapters) {
        *count = deviceCount;
        return PAL_RESULT_SUCCESS;
    }

    VkPhysicalDevice* devices = nullptr;
    devices = palAllocate(s_Vk.allocator, 
        sizeof(VkPhysicalDevice) * deviceCount, 
        0);

    if (!devices) {
        return PAL_RESULT_OUT_OF_MEMORY;
    }

    s_Vk.enumeratePhysicalDevices(s_Vk.instance, &deviceCount, devices);
    for (int i = 0; i < *count && i < deviceCount; i++) {
        outAdapters[i] = (PalAdapter*)devices[i];
    }

    palFree(s_Vk.allocator, devices);
    return PAL_RESULT_SUCCESS;
}

PalResult PAL_CALL getVkAdapterInfo(
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
    strcpy(info->backendName, "PAL");

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

PalResult PAL_CALL getVkAdapterCapabilities(
    PalAdapter* adapter,
    PalAdapterCapabilities* caps)
{
    VkResult ret = VK_SUCCESS;
    VkPhysicalDevice phyDevice = (VkPhysicalDevice)adapter;
    VkPhysicalDeviceProperties props = {0};
    VkPhysicalDeviceMultiviewPropertiesKHR multiViewProps = {0};
    VkPhysicalDeviceProperties2 properties2 = {0};
    multiViewProps.sType = 
        VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MULTIVIEW_PROPERTIES_KHR;

    s_Vk.getPhysicalDeviceProperties(phyDevice, &props);
    properties2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
    properties2.pNext = &multiViewProps;
    s_Vk.getPhysicalDeviceProperties2(phyDevice, &properties2);

    caps->debugLayer = s_Vk.hasDebug;
    caps->maxColorAttachments = props.limits.maxColorAttachments;
    caps->maxImageWidth = props.limits.maxImageDimension2D;
    caps->maxImageHeight = props.limits.maxImageDimension2D;
    caps->maxImageDepth = props.limits.maxImageDimension3D;
    caps->maxImageArrayLayers = props.limits.maxImageArrayLayers;

    PalSampleCount tmp = PAL_SAMPLE_COUNT_1;
    tmp = vkSamplesToSamples(props.limits.framebufferColorSampleCounts);
    caps->maxColorSampleCount = tmp;
    tmp = vkSamplesToSamples(props.limits.framebufferDepthSampleCounts);
    caps->maxDepthSampleCount = tmp;

    caps->maxViewports = props.limits.maxViewports;
    caps->maxSamplers = props.limits.maxSamplerAllocationCount;
    caps->maxUniformBufferSize = props.limits.maxUniformBufferRange;
    caps->maxStorageBufferSize = props.limits.maxStorageBufferRange;
    caps->maxPushConstantSize = props.limits.maxPushConstantsSize;
    
    caps->maxMultiViews = multiViewProps.maxMultiviewViewCount;
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
        s_Vk.allocator, 
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

    palFree(s_Vk.allocator, queueProps);
    return PAL_RESULT_SUCCESS;
}

PalAdapterFeatures PAL_CALL getVkAdapterFeatures(PalAdapter* adapter)
{
    VkResult ret;
    PalAdapterFeatures adapterFeatures = 0;
    Uint32 extensionCount = 0;
    VkPhysicalDevice phyDevice = (VkPhysicalDevice)adapter;
    VkPhysicalDeviceProperties props = {0};

    // get supported extensions
    s_Vk.getPhysicalDeviceProperties(phyDevice, &props);
    ret = s_Vk.enumerateDeviceExtensionProperties(
        phyDevice,
        nullptr, 
        &extensionCount, 
        nullptr);

    if (ret != VK_SUCCESS) {
        // we just return without any modern features which is rare
        return 0;
    }

    VkExtensionProperties* extensionProps = nullptr;
    extensionProps = palAllocate(
        s_Vk.allocator, 
        sizeof(VkExtensionProperties) * extensionCount, 
        0);

    if (!extensionProps) {
        return 0;
    }

    s_Vk.enumerateDeviceExtensionProperties(
        phyDevice, 
        nullptr, 
        &extensionCount, 
        extensionProps);

    // check extensions
    bool rayTracingFound = false;
    bool accelerateFound = false;
    bool meshShader = false;
    bool fragmentRateShading = false;
    bool timelineSemaphore = false;
    bool descriptorIndexing = false;
    bool shaderFloat16 = false;
    bool multiiView = false;

    // clang-format off
    // check if the extensions are present
    for (int i = 0; i < extensionCount; i++) {
        VkExtensionProperties* props = &extensionProps[i];
        if (strcmp(props->extensionName, "VK_KHR_ray_tracing_pipeline") == 0) {
            rayTracingFound = true;

        } else if (strcmp(props->extensionName, "VK_KHR_acceleration_structure") == 0) {
            accelerateFound = true;

        } else if (strcmp(props->extensionName, "VK_EXT_mesh_shader") == 0) {
            meshShader = true;

        } else if (strcmp(props->extensionName, "VK_KHR_fragment_shading_rate") == 0) {
            fragmentRateShading = true;         

        } else if (strcmp(props->extensionName, "VK_EXT_descriptor_indexing") == 0) {
            descriptorIndexing = true;
          
        } else if (strcmp(props->extensionName, "VK_KHR_swapchain") == 0) {
            adapterFeatures |= PAL_ADAPTER_FEATURE_SWAPCHAIN;

        } else if (strcmp(props->extensionName, "VK_KHR_shader_float16_int8") == 0) {
            shaderFloat16 = true;

        } else if (strcmp(props->extensionName, "VK_KHR_timeline_semaphore") == 0) {
            timelineSemaphore = true;

        } else if (strcmp(props->extensionName, "VK_KHR_multiview") == 0) {
            multiiView = true;

        } else if (strcmp(props->extensionName, "VK_EXT_extended_dynamic_state") == 0) {
            adapterFeatures |= PAL_ADAPTER_FEATURE_DYNAMIC_CULL_MODE;
            adapterFeatures |= PAL_ADAPTER_FEATURE_DYNAMIC_FRONT_FACE;
            adapterFeatures |= PAL_ADAPTER_FEATURE_DYNAMIC_PRIMITIVE_TOPOLOGY;

        } else if (strcmp(props->extensionName, "VK_EXT_extended_dynamic_state2") == 0) {
            adapterFeatures |= PAL_ADAPTER_FEATURE_DYNAMIC_DEPTH_TEST_ENABLE;
            adapterFeatures |= PAL_ADAPTER_FEATURE_DYNAMIC_DEPTH_WRITE_ENABLE;
            adapterFeatures |= PAL_ADAPTER_FEATURE_DYNAMIC_STENCIL_OP;
        }
    }

    // features that require core and extension support
    // ray tracing is part of core 1.3
    if (props.apiVersion >= VK_API_VERSION_1_3 || 
        (rayTracingFound && accelerateFound)) {

        VkPhysicalDeviceRayTracingPipelineFeaturesKHR ray = {0};
        VkPhysicalDeviceAccelerationStructureFeaturesKHR acc = {0};

        ray.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_TRACING_PIPELINE_FEATURES_KHR;
        acc.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ACCELERATION_STRUCTURE_FEATURES_KHR;

        ray.pNext = &acc;
        VkPhysicalDeviceFeatures2 features;
        features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
        features.pNext = &ray;

        s_Vk.getPhysicalDeviceFeatures2(phyDevice, &features);
        if (ray.rayTracingPipeline && acc.accelerationStructure) {
            adapterFeatures |= PAL_ADAPTER_FEATURE_RAY_TRACING;
        }
    }

    // mesh shader is part of core 1.3
    if (props.apiVersion >= VK_API_VERSION_1_3 || meshShader) {
        VkPhysicalDeviceMeshShaderFeaturesEXT mesh = {0};
        mesh.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MESH_SHADER_FEATURES_EXT;

        VkPhysicalDeviceFeatures2 features;
        features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
        features.pNext = &mesh;

        s_Vk.getPhysicalDeviceFeatures2(phyDevice, &features);
        if (mesh.meshShader && mesh.taskShader) {
            adapterFeatures |= PAL_ADAPTER_FEATURE_MESH_SHADER;
        }
    }

    // fragment shading rate is part of core 1.3
    if (props.apiVersion >= VK_API_VERSION_1_3 || fragmentRateShading) {
        VkPhysicalDeviceFragmentShadingRateFeaturesKHR frag = {0};
        frag.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FRAGMENT_SHADING_RATE_FEATURES_KHR;

        VkPhysicalDeviceFeatures2 features;
        features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
        features.pNext = &frag;

        s_Vk.getPhysicalDeviceFeatures2(phyDevice, &features);
        if (frag.pipelineFragmentShadingRate) {
            adapterFeatures |= PAL_ADAPTER_FEATURE_FRAGMENT_SHADING_RATE;
        }
    }

    // descriptor indexing is part of core 1.2
    if (props.apiVersion >= VK_API_VERSION_1_2 || descriptorIndexing) {
        VkPhysicalDeviceDescriptorIndexingFeaturesEXT desc = {0};
        desc.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_FEATURES_EXT;

        VkPhysicalDeviceFeatures2 features;
        features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
        features.pNext = &desc;

        s_Vk.getPhysicalDeviceFeatures2(phyDevice, &features);
        if (desc.shaderSampledImageArrayNonUniformIndexing) {
            adapterFeatures |= PAL_ADAPTER_FEATURE_DESCRIPTOR_INDEXING;
        }
    }

    // timeline semaphore is part of core 1.2
    if (props.apiVersion >= VK_API_VERSION_1_2 || timelineSemaphore) {
        VkPhysicalDeviceTimelineSemaphoreFeaturesKHR timeline = {0};
        timeline.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_TIMELINE_SEMAPHORE_FEATURES_KHR;

        VkPhysicalDeviceFeatures2 features;
        features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
        features.pNext = &timeline;

        s_Vk.getPhysicalDeviceFeatures2(phyDevice, &features);
        if (timeline.timelineSemaphore) {
            adapterFeatures |= PAL_ADAPTER_FEATURE_TIMELINE_SEMAPHORE;
        }
    }

    // shader float 16 is part of core 1.2
    if (props.apiVersion >= VK_API_VERSION_1_2 || shaderFloat16) {
        VkPhysicalDeviceShaderFloat16Int8FeaturesKHR shader16 = {0};
        shader16.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_FLOAT16_INT8_FEATURES_KHR;

        VkPhysicalDeviceFeatures2 features;
        features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
        features.pNext = &shader16;

        s_Vk.getPhysicalDeviceFeatures2(phyDevice, &features);
        if (shader16.shaderFloat16) {
            adapterFeatures |= PAL_ADAPTER_FEATURE_SHADER_FLOAT16;
        }
    }

    // multi view is part of core 1.1
    if (props.apiVersion >= VK_API_VERSION_1_1 || multiiView) {
        VkPhysicalDeviceMultiviewFeaturesKHR multiView = {0};
        multiView.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MULTIVIEW_FEATURES_KHR;

        VkPhysicalDeviceFeatures2 features;
        features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
        features.pNext = &multiView;

        s_Vk.getPhysicalDeviceFeatures2(phyDevice, &features);
        if (multiView.multiview) {
            adapterFeatures |= PAL_ADAPTER_FEATURE_MULTI_VIEW;
        }
    }

    // clang-format on
    VkPhysicalDeviceFeatures features;
    s_Vk.getPhysicalDeviceFeatures(phyDevice, &features);

    // check for additional features
    if (features.multiViewport) {
        adapterFeatures |= PAL_ADAPTER_FEATURE_MULTI_VIEWPORT;
    }

    if (features.samplerAnisotropy) {
        adapterFeatures |= PAL_ADAPTER_FEATURE_SAMPLER_ANISOTROPY;
    }

    if (features.sampleRateShading) {
        adapterFeatures |= PAL_ADAPTER_FEATURE_SAMPLE_RATE_SHADING;
    }

    if (features.shaderFloat64) {
        adapterFeatures |= PAL_ADAPTER_FEATURE_SHADER_FLOAT64;
    }

    if (features.shaderInt64) {
        adapterFeatures |= PAL_ADAPTER_FEATURE_SHADER_INT64;
    }

    if (features.shaderInt16) {
        adapterFeatures |= PAL_ADAPTER_FEATURE_SHADER_INT16;
    }

    if (features.geometryShader) {
        adapterFeatures |= PAL_ADAPTER_FEATURE_GEOMETRY_SHADER;
    }

    if (features.tessellationShader) {
        adapterFeatures |= PAL_ADAPTER_FEATURE_TESSELLATION_SHADER;
    }

    if (features.fillModeNonSolid) {
        adapterFeatures |= PAL_ADAPTER_FEATURE_POLYGON_MODE_LINE;
    }

    // this features are supported on vulkan
    adapterFeatures |= PAL_ADAPTER_FEATURE_IMAGE_VIEW_CUBE_ARRAY;
    adapterFeatures |= PAL_ADAPTER_FEATURE_COMPUTE_SHADER;
    adapterFeatures |= PAL_ADAPTER_FEATURE_COMMAND_POOL_FLAG_RESETTABLE;
    adapterFeatures |= PAL_ADAPTER_FEATURE_COMMAND_POOL_FLAG_TRANSIENT;
    adapterFeatures |= PAL_ADAPTER_FEATURE_FENCE_RESET;
    adapterFeatures |= PAL_ADAPTER_FEATURE_FENCE_TIMEOUT;
    adapterFeatures |= PAL_ADAPTER_FEATURE_SEMAPHORE;

    palFree(s_Vk.allocator, extensionProps);
    return adapterFeatures;
}

// ==================================================
// Device
// ==================================================

PalResult PAL_CALL createVkDevice(
    PalAdapter* adapter,
    PalAdapterFeatures features,
    PalDevice** outDevice)
{
    float priority = 1.0f;
    Uint32 count = 0;
    VkResult ret = VK_SUCCESS;
    PalDevice* device = nullptr;
    VkPhysicalDeviceProperties props = {0};
    VkPhysicalDevice phyDevice = (VkPhysicalDevice)adapter;

    s_Vk.getPhysicalDeviceProperties(phyDevice, &props);

    VkQueueFamilyProperties* queueProps = nullptr;
    VkDeviceQueueCreateInfo* queueCreateInfos = nullptr;
    s_Vk.getPhysicalDeviceQueueFamilyProperties(
        phyDevice, 
        &count, 
        nullptr);

    queueProps = palAllocate(
        s_Vk.allocator, 
        sizeof(VkQueueFamilyProperties) * count, 
        0);

    queueCreateInfos = palAllocate(
        s_Vk.allocator, 
        sizeof(VkDeviceQueueCreateInfo) * count, 
        0);

    device = palAllocate(s_Vk.allocator, sizeof(PalDevice), 0);
    if (!queueProps || !queueCreateInfos || !device) {
        return PAL_RESULT_OUT_OF_MEMORY;
    }

    device->queueCount = count;
    device->phyDevice = phyDevice;
    
    device->phyQueues = palAllocate(
        s_Vk.allocator, 
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

    if (features & PAL_ADAPTER_FEATURE_SWAPCHAIN) {
        extensions[extCount++] = "VK_KHR_swapchain";
    }

    if (features & PAL_ADAPTER_FEATURE_TIMELINE_SEMAPHORE) {
        if (props.apiVersion < VK_API_VERSION_1_2) {
            extensions[extCount++] = "VK_KHR_timeline_semaphore";
        }

        timeline.timelineSemaphore = true;
        start = &timeline;
    }

    if (features & PAL_ADAPTER_FEATURE_SHADER_FLOAT16) {
        if (props.apiVersion < VK_API_VERSION_1_2) {
            extensions[extCount++] = "VK_KHR_shader_float16_int8";
        }

        shader16.shaderFloat16 = true;
        if (timeline.timelineSemaphore) {
            timeline.pNext = &shader16;
        }
        start = &shader16;
    }

    if (features & PAL_ADAPTER_FEATURE_RAY_TRACING) {
        if (props.apiVersion < VK_API_VERSION_1_3) {
            extensions[extCount++] = "VK_KHR_ray_tracing_pipeline";
            extensions[extCount++] = "VK_KHR_acceleration_structure";
        }

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
        if (props.apiVersion < VK_API_VERSION_1_3) {
            extensions[extCount++] = "VK_EXT_mesh_shader";
        }

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

    if (features & PAL_ADAPTER_FEATURE_FRAGMENT_SHADING_RATE) {
        if (props.apiVersion < VK_API_VERSION_1_3) {
            extensions[extCount++] = "VK_KHR_fragment_shading_rate";
        }

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
        if (props.apiVersion < VK_API_VERSION_1_2) {
            extensions[extCount++] = "VK_EXT_descriptor_indexing";
        }

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
        if (props.apiVersion < VK_API_VERSION_1_2) {
            extensions[extCount++] = "VK_KHR_multiview";
        }

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
        &s_Vk.vkAllocator, 
        &device->handle);

    if (ret != VK_SUCCESS) {
        palFree(s_Vk.allocator, queueProps);
        palFree(s_Vk.allocator, queueCreateInfos);
        palFree(s_Vk.allocator, device->phyQueues);
        palFree(s_Vk.allocator, device);
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
    for (int i = 0; i < memProps.memoryTypeCount; i++) {
        VkMemoryPropertyFlags prop = memProps.memoryTypes[i].propertyFlags;
        if (prop & VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT) {
            device->gpuOnlyMemoryIndex = i;
        }

        if ((prop & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT) && 
             prop & VK_MEMORY_PROPERTY_HOST_COHERENT_BIT) {
            device->cpuUploadMemoryIndex = i;
        }

        if ((prop & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT) && 
             prop & VK_MEMORY_PROPERTY_HOST_CACHED_BIT) {
            device->cpuReadbackMemoryIndex = i;
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

    device->features = features;
    palFree(s_Vk.allocator, queueProps);
    palFree(s_Vk.allocator, queueCreateInfos);

    *outDevice = device;
    return PAL_RESULT_SUCCESS;
}

void PAL_CALL destroyVkDevice(PalDevice* device)
{
    s_Vk.destroyDevice(device->handle, &s_Vk.vkAllocator);
    palFree(s_Vk.allocator, device->phyQueues);
    palFree(s_Vk.allocator, device);
}

// ==================================================
// Memory
// ==================================================

PalResult PAL_CALL allocateVkMemory(
    PalDevice* device,
    PalMemoryType type,
    Uint64 size,
    PalMemory** outMemory)
{
    VkMemoryAllocateInfo allocateInfo = {0};
    allocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocateInfo.allocationSize = (VkDeviceSize)size;
    if (type == PAL_MEMORY_TYPE_GPU_ONLY) {
        allocateInfo.memoryTypeIndex = device->gpuOnlyMemoryIndex;

    } else if (type == PAL_MEMORY_TYPE_CPU_UPLOAD) {
        allocateInfo.memoryTypeIndex = device->cpuUploadMemoryIndex;

    } else if (type == PAL_MEMORY_TYPE_CPU_READBACK) {
        allocateInfo.memoryTypeIndex = device->cpuReadbackMemoryIndex;
    }

    if (allocateInfo.memoryTypeIndex == -1) {
        // not supported
        return PAL_RESULT_MEMORY_TYPE_NOT_SUPPORTED;
    }

    VkDeviceMemory memory = nullptr;
    VkResult result = s_Vk.allocateMemory(
        device->handle, 
        &allocateInfo, 
        &s_Vk.vkAllocator, 
        &memory);

    if (result != VK_SUCCESS) {
        return vkResultToPal(result);
    }

    *outMemory = (PalMemory*)memory;
    return PAL_RESULT_SUCCESS;
}

void PAL_CALL freeVkMemory(
    PalDevice* device,
    PalMemory* memory)
{
    VkDeviceMemory mem = (VkDeviceMemory)memory;
    s_Vk.freeMemory(device->handle, mem, &s_Vk.vkAllocator);
}

// ==================================================
// Queue
// ==================================================

PalResult PAL_CALL createVkQueue(
    PalDevice* device,
    PalQueueType type,
    PalQueue** outQueue)
{
    VkQueueFlags queueFlag = 0;
    PalQueue* queue = nullptr;
    if (!device->handle) {
        return PAL_RESULT_INVALID_DEVICE;
    }

    if (device->queueCount == 0) {
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
    for (int i = 0; i < device->queueCount; i++) {
        PhysicalQueue* pq = &device->phyQueues[i];
        // check if the physical queue supports the requested operation
        // and if its not already used
        if (pq->usages & queueFlag && 
            pq->usedUsages != queueFlag) {
            pq->usedUsages |= queueFlag;
            phyQueue = pq;
            break;
        } 
    }

    if (!phyQueue) {
        return PAL_RESULT_OUT_OF_QUEUE;
    }

    queue = palAllocate(s_Vk.allocator, sizeof(PalQueue), 0);
    if (!queue) {
        return PAL_RESULT_OUT_OF_MEMORY;
    }

    queue->phyQueue = phyQueue;
    queue->usage = queueFlag;
    queue->device = device;

    *outQueue = queue;
    return PAL_RESULT_SUCCESS;
}

void PAL_CALL destroyVkQueue(PalQueue* queue)
{
    PhysicalQueue* phyQueue = queue->phyQueue;
    phyQueue->usedUsages &= ~queue->usage;
    palFree(s_Vk.allocator, queue);
}

bool PAL_CALL canVkQueuePresent(
    PalQueue* queue, 
    PalGraphicsWindow* window)
{
    // check if the queue is a graphics queue before we check its family 
    // index for presentation support.
    if (queue->usage != VK_QUEUE_GRAPHICS_BIT) {
        return false;
    }

    Uint32 platform = checkPlatform(window->display);
    PhysicalQueue* phyQueue = queue->phyQueue;
    if (platform == VK_WIN32_PLATFORM) {

    } else if (platform == VK_WAYLAND_PLATFORM) {
        if (s_Vk.checkWaylandPresentSupport(
            phyQueue->phyDevice, 
            phyQueue->familyIndex, window->display)) {
            return true;
        }

    } else if (platform == VK_XLIB_PLATFORM) {
        
    }
    return false;
}

// ==================================================
// Formats
// ==================================================

PalResult PAL_CALL enumerateVkFormats(
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

bool PAL_CALL isVkFormatSupported(
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

PalImageUsages PAL_CALL queryVkFormatImageUsages(
    PalAdapter* adapter,
    PalFormat format)
{
    VkPhysicalDevice phyDevice = (VkPhysicalDevice)adapter;
    VkFormatProperties props = {0};

    VkFormat fmt = palFormatToVk(format);
    s_Vk.getPhysicalDeviceFormatProperties(phyDevice, fmt, &props);
    if (props.optimalTilingFeatures == 0) {
        return PAL_IMAGE_USAGE_UNDEFINED;
    }
    return vkFeatureToPalUsage(props.optimalTilingFeatures);
}

PalImageViewUsages PAL_CALL queryVkFormatImageViewUsages(
    PalAdapter* adapter,
    PalFormat format)
{
    VkPhysicalDevice phyDevice = (VkPhysicalDevice)adapter;
    VkFormatProperties props = {0};

    VkFormat fmt = palFormatToVk(format);
    s_Vk.getPhysicalDeviceFormatProperties(phyDevice, fmt, &props);
    if (props.optimalTilingFeatures == 0) {
        return PAL_IMAGE_VIEW_USAGE_UNDEFINED;
    }

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

// ==================================================
// Image
// ==================================================

PalResult PAL_CALL createVkImage(
    PalDevice* device,
    const PalImageCreateInfo* info,
    PalImage** outImage)
{
    VkResult result;
    PalImage* image = nullptr;
    if (!device->handle) {
        return PAL_RESULT_INVALID_DEVICE;
    }

    image = palAllocate(s_Vk.allocator, sizeof(PalImage), 0);
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
    createInfo.samples = samplesToVk(info->sampleCount);
    createInfo.usage = palUsageToVk(info->usages);

    createInfo.arrayLayers = info->depthOrArraySize;
    createInfo.extent.depth = 1;
    createInfo.imageType = palImageTypeToVk(info->type);

    if (info->type == PAL_IMAGE_TYPE_3D) {
        createInfo.arrayLayers = 1;
        createInfo.extent.depth = info->depthOrArraySize;
    }

    result = s_Vk.createImage(
        device->handle, 
        &createInfo, 
        &s_Vk.vkAllocator, 
        &image->handle);

    if (result != VK_SUCCESS) {
        palFree(s_Vk.allocator, image);
        return vkResultToPal(result);
    }

    image->belongsToSwapchain = false;
    image->device = device;
    image->info.depthOrArraySize = info->depthOrArraySize;
    image->info.type = info->type;
    image->info.format = info->format;
    image->info.usages = info->usages;
    image->info.height = info->height;
    image->info.mipLevelCount = info->mipLevelCount;
    image->info.sampleCount = info->sampleCount;
    image->info.width = info->width;

    *outImage = image;
    return PAL_RESULT_SUCCESS;
}

void PAL_CALL destroyVkImage(PalImage* image)
{
    if (image->belongsToSwapchain) {
        return;
    }
    s_Vk.destroyImage(image->device->handle, image->handle, &s_Vk.vkAllocator);
    palFree(s_Vk.allocator, image);
}

PalResult PAL_CALL getVkImageInfo(
    PalImage* image,
    PalImageInfo* info)
{
    *info = image->info;
    return PAL_RESULT_SUCCESS;
}

PalResult PAL_CALL getVkImageMemoryRequirements(
    PalImage* image,
    PalMemoryRequirements* requirements)
{
    PalDevice* device = image->device;
    VkPhysicalDevice phyDevice = (VkPhysicalDevice)device->phyDevice;
    VkPhysicalDeviceMemoryProperties memProps = {0};
    s_Vk.getPhysicalDeviceMemoryProperties(phyDevice, &memProps);

    VkMemoryRequirements memReq = {0};
    s_Vk.getImageMemoryRequirements(device->handle, image->handle, &memReq);
    requirements->alignment = (Uint64)memReq.alignment;
    requirements->size = (Uint64)memReq.size;

    requirements->memoryTypes[PAL_MEMORY_TYPE_GPU_ONLY] = false;
    requirements->memoryTypes[PAL_MEMORY_TYPE_CPU_UPLOAD] = false;
    requirements->memoryTypes[PAL_MEMORY_TYPE_CPU_READBACK] = false;

    for (int i = 0; i < memProps.memoryTypeCount; i++) {
        if (!(memReq.memoryTypeBits & (1 << i))) {
            // memory type not supported
            continue;
        }

        VkMemoryPropertyFlags prop = memProps.memoryTypes[i].propertyFlags;
        if (prop & VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT) {
            requirements->memoryTypes[PAL_MEMORY_TYPE_GPU_ONLY] = true;
        }

        if ((prop & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT) && 
             prop & VK_MEMORY_PROPERTY_HOST_COHERENT_BIT) {
            requirements->memoryTypes[PAL_MEMORY_TYPE_CPU_UPLOAD] = true;
        }

        if ((prop & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT) && 
             prop & VK_MEMORY_PROPERTY_HOST_CACHED_BIT) {
            requirements->memoryTypes[PAL_MEMORY_TYPE_CPU_READBACK] = true;
        }
    }
    return PAL_RESULT_SUCCESS;
}

PalResult PAL_CALL bindVkImageMemory(
    PalImage* image,
    PalMemory* memory,
    Uint64 offset)
{
    if (image->belongsToSwapchain) {
        return PAL_RESULT_INVALID_OPERATION;
    }

    VkDeviceMemory mem = (VkDeviceMemory)memory;
    s_Vk.bindImageMemory(image->device->handle, image->handle, mem, offset);
}

// ==================================================
// Image View
// ==================================================

PalResult PAL_CALL createVkImageView(
    PalDevice* device,
    PalImage* image,
    const PalImageViewCreateInfo* info,
    PalImageView** outImageView)
{
    // mimic the actual requested features at device creation
    if (info->type == PAL_IMAGE_VIEW_TYPE_CUBE_ARRAY) {
        if (!(device->features & PAL_ADAPTER_FEATURE_IMAGE_VIEW_CUBE_ARRAY)) {
            return PAL_RESULT_ADAPTER_FEATURE_NOT_SUPPORTED;
        }
    }

    VkResult result = VK_SUCCESS;
    PalImageView* imageView = nullptr;
    imageView = palAllocate(s_Vk.allocator, sizeof(PalImageView), 0);
    if (!imageView) {
        return PAL_RESULT_OUT_OF_MEMORY;
    }

    VkImageViewCreateInfo createInfo = {0};
    createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    createInfo.format = palFormatToVk(image->info.format);
    createInfo.image = image->handle;

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
        device->handle, 
        &createInfo, 
        &s_Vk.vkAllocator, 
        &imageView->handle);

    if (result != VK_SUCCESS) {
        palFree(s_Vk.allocator, imageView);
        return vkResultToPal(result);
    }

    imageView->device = device;
    imageView->image = image;
    imageView->type = createInfo.viewType;
    imageView->usages = info->usages;

    *outImageView = imageView;
    return PAL_RESULT_SUCCESS;
}

void PAL_CALL destroyVkImageView(PalImageView* imageView)
{
    s_Vk.destroyImageView(
        imageView->device->handle, 
        imageView->handle, 
        &s_Vk.vkAllocator);

    palFree(s_Vk.allocator, imageView);
}

// ==================================================
// Swapchain
// ==================================================

PalResult PAL_CALL queryVkSwapchainCapabilities(
    PalAdapter* adapter,
    PalGraphicsWindow* window,
    PalSwapchainCapabilities* caps)
{
    Int32 formatCount = 0;
    Int32 modeCount = 0;
    VkSurfaceKHR surface = nullptr;
    VkSurfaceFormatKHR* formats = nullptr;
    VkPresentModeKHR* modes = nullptr;
    VkPhysicalDevice phyDevice = (VkPhysicalDevice)adapter;

    bool ret = createSurface(window, &surface);
    if (!ret) {
        return PAL_RESULT_INVALID_GRAPHICS_WINDOW;
    }

    s_Vk.getSurfacePresentModes(phyDevice, surface, &modeCount, nullptr);
    s_Vk.getSurfaceFormats(phyDevice, surface, &formatCount, nullptr);

    modes = palAllocate(
        s_Vk.allocator, 
        sizeof(VkPresentModeKHR) * modeCount, 
        0);

    formats = palAllocate(
        s_Vk.allocator, 
        sizeof(VkSurfaceFormatKHR) * formatCount, 
        0);

    if (!modes || !formats) {
        s_Vk.destroySurface(s_Vk.instance, surface, &s_Vk.vkAllocator);
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
        caps->maxImageCount = INT32_MAX;
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

    palFree(s_Vk.allocator, formats);
    palFree(s_Vk.allocator, modes);
    s_Vk.destroySurface(s_Vk.instance, surface, &s_Vk.vkAllocator);
    return PAL_RESULT_SUCCESS;
}

PalResult PAL_CALL createVkSwapchain(
    PalDevice* device,
    PalQueue* queue,
    PalGraphicsWindow* window,
    const PalSwapchainCreateInfo* info,
    PalSwapchain** outSwapchain)
{
    PalFormat imageFormat = 0;
    PalSwapchain* swapchain = nullptr;
    VkImage* images = nullptr;
    PhysicalQueue* phyQueue = queue->phyQueue;

    // check if we enabled swapchain feature
    if (!device->features & PAL_ADAPTER_FEATURE_SWAPCHAIN) {
        return PAL_RESULT_ADAPTER_FEATURE_NOT_SUPPORTED;
    }

    // check if the queue is a graphics queue before we check its family 
    // index for presentation support.
    if (queue->usage != VK_QUEUE_GRAPHICS_BIT) {
        PAL_RESULT_INVALID_QUEUE;
    }

    swapchain = palAllocate(s_Vk.allocator, sizeof(PalSwapchain), 0);
    if (!swapchain) {
        return PAL_RESULT_OUT_OF_MEMORY;
    }

    bool ret = createSurface(window, &swapchain->surface);
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
    VkResult result = device->createSwapchain(
        device->handle, 
        &createInfo, 
        &s_Vk.vkAllocator, 
        &swapchain->handle);

    if (result != VK_SUCCESS) {
        s_Vk.destroySurface(
            s_Vk.instance, 
            swapchain->surface, 
            &s_Vk.vkAllocator);

        palFree(s_Vk.allocator, swapchain);
        return vkResultToPal(result);
    }

    // get and cache all images
    Int32 count = 0;
    result = device->getSwapchainImages(
        device->handle,
        swapchain->handle, 
        &count,
        nullptr);

    swapchain->images = palAllocate(
        s_Vk.allocator, 
        sizeof(PalImage) * count, 
        0);

    images = palAllocate(
        s_Vk.allocator, 
        sizeof(VkImage) * count, 
        0);

    if (!swapchain->images || !images) {
        device->destroySwapchain(
            device->handle,
            swapchain->handle,
            &s_Vk.vkAllocator
        );

        s_Vk.destroySurface(
            s_Vk.instance, 
            swapchain->surface, 
            &s_Vk.vkAllocator);

        palFree(s_Vk.allocator, swapchain);
        return PAL_RESULT_OUT_OF_MEMORY;
    }
    
    device->getSwapchainImages(
        device->handle, 
        swapchain->handle, 
        &count,
        images);

    // fill all images with the creatio info
    for (int i = 0; i < count; i++) {
        PalImage* image = &swapchain->images[i];
        image->belongsToSwapchain = true;
        image->device = device;
        image->handle = images[i];
        image->index = -1;

        image->info.depthOrArraySize = createInfo.imageArrayLayers;
        image->info.format = imageFormat;
        image->info.usages = PAL_IMAGE_USAGE_COLOR_ATTACHEMENT;
        image->info.height = createInfo.imageExtent.height;
        image->info.width = createInfo.imageExtent.width;
        image->info.mipLevelCount = 1; 
        image->info.sampleCount = PAL_SAMPLE_COUNT_1; // swapchain images are not multisampled
        image->info.type = PAL_IMAGE_TYPE_2D;
    }
    
    swapchain->device = device;
    swapchain->queue = queue;
    swapchain->imageCount = count;

    *outSwapchain = swapchain;
    return PAL_RESULT_SUCCESS;
}

void PAL_CALL destroyVkSwapchain(PalSwapchain* swapchain)
{
    swapchain->device->destroySwapchain(
        swapchain->device->handle,
        swapchain->handle,
        &s_Vk.vkAllocator
    );

    s_Vk.destroySurface(s_Vk.instance, swapchain->surface, &s_Vk.vkAllocator);
    palFree(s_Vk.allocator, swapchain->images);
    palFree(s_Vk.allocator, swapchain);
}

PalImage* PAL_CALL getVkSwapchainImage(
    PalSwapchain* swapchain,
    Int32 index)
{
    if (index > swapchain->imageCount) {
        return nullptr;
    }
    return &swapchain->images[index];
}

PalImage* PAL_CALL getVkNextSwapchainImage(
    PalSwapchain* swapchain,
    PalNextImageInfo* info)
{
    VkResult result;
    Uint32 index = 0;
    VkFence fenceHandle = nullptr;
    VkSemaphore semaphoreHandle = nullptr;

    if (info->fence) {
        fenceHandle = info->fence->handle;
    }

    if (info->signalSemaphore) {
        semaphoreHandle = info->signalSemaphore->handle;
    }

    result = swapchain->device->acquireNextImage(
        swapchain->device->handle,
        swapchain->handle,
        info->timeout,
        semaphoreHandle,
        fenceHandle,
        &index);

    if (result != VK_SUCCESS) {
        return nullptr;
    }

    PalImage* image = &swapchain->images[index];
    image->index = index;
    return image;
}

PalResult PAL_CALL presentVkSwapchain(
    PalSwapchain* swapchain, 
    PalPresentInfo* info)
{
    if (info->image->index == -1) {
        // image was not the next image
        // this prevents UB
        return PAL_RESULT_INVALID_IMAGE;
    }

    Int32 semaphoreCount = 0;
    VkSemaphore semaphoreHandle = nullptr;
    if (info->waitSemaphore) {
        semaphoreHandle = info->waitSemaphore->handle;
        semaphoreCount = 1;
    }

    VkResult result;
    VkPresentInfoKHR presentInfo = {0};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = &swapchain->handle;
    presentInfo.pImageIndices = &info->image->index;
    presentInfo.pWaitSemaphores = &semaphoreHandle;
    presentInfo.waitSemaphoreCount = semaphoreCount;

    result = swapchain->device->queuePresent(
        swapchain->queue->phyQueue->handle, 
        &presentInfo);

    if (result != VK_SUCCESS) {
        return vkResultToPal(result);
    }

    info->image->index = -1;
    return PAL_RESULT_SUCCESS;
}

// ==================================================
// Shader
// ==================================================

PalResult PAL_CALL createVkShader(
    PalDevice* device,
    const PalShaderCreateInfo* info,
    PalShader** outShader)
{
    VkResult result;
    PalShader* shader = nullptr;
    VkShaderStageFlags stage = 0;

    if (info->type == PAL_SHADER_TYPE_VERTEX) {
        stage = VK_SHADER_STAGE_VERTEX_BIT;

    } else if (info->type == PAL_SHADER_TYPE_FRAGMENT) {
        stage = VK_SHADER_STAGE_FRAGMENT_BIT;

    } else if (info->type == PAL_SHADER_TYPE_COMPUTE) {
        if (device->features & PAL_ADAPTER_FEATURE_COMPUTE_SHADER) {
            return PAL_RESULT_ADAPTER_FEATURE_NOT_SUPPORTED;
        }
        stage = VK_SHADER_STAGE_COMPUTE_BIT;

    } else if (info->type == PAL_SHADER_TYPE_TESSELLATION_CONTROL) {
        if (device->features & PAL_ADAPTER_FEATURE_TESSELLATION_SHADER) {
            return PAL_RESULT_ADAPTER_FEATURE_NOT_SUPPORTED;
        }
        stage = VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT;

    } else if (info->type == PAL_SHADER_TYPE_TESSELLATION_EVALUATION) {
        if (device->features & PAL_ADAPTER_FEATURE_TESSELLATION_SHADER) {
            return PAL_RESULT_ADAPTER_FEATURE_NOT_SUPPORTED;
        }
        stage = VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT;

    } else if (info->type == PAL_SHADER_TYPE_MESH) {
        if (device->features & PAL_ADAPTER_FEATURE_MESH_SHADER) {
            return PAL_RESULT_ADAPTER_FEATURE_NOT_SUPPORTED;
        }
        stage = VK_SHADER_STAGE_MESH_BIT_EXT;

    } else if (info->type == PAL_SHADER_TYPE_TASK) {
        if (device->features & PAL_ADAPTER_FEATURE_MESH_SHADER) {
            return PAL_RESULT_ADAPTER_FEATURE_NOT_SUPPORTED;
        }
        stage = VK_SHADER_STAGE_TASK_BIT_EXT;
    }

    shader = palAllocate(s_Vk.allocator, sizeof(PalShader), 0);
    if (!shader) {
        return PAL_RESULT_OUT_OF_MEMORY;
    }

    VkShaderModuleCreateInfo createInfo = {0};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.codeSize = info->bytecodeSize;
    createInfo.pCode = (const Uint32*)info->bytecode;

    result = s_Vk.createShader(
        device->handle, 
        &createInfo, 
        &s_Vk.vkAllocator, 
        &shader->handle);
    
    if (result != VK_SUCCESS) {
        palFree(s_Vk.allocator, shader);
        return vkResultToPal(result);
    }

    shader->device = device;
    shader->type = info->type;
    shader->info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    shader->info.module = shader->handle;
    shader->info.pName = "main";
    shader->info.stage = stage;

    *outShader = shader;
    return PAL_RESULT_SUCCESS;
}

void PAL_CALL destroyVkShader(PalShader* shader)
{
    s_Vk.destroyShader(
        shader->device->handle, 
        shader->handle, 
        &s_Vk.vkAllocator);
    
    palFree(s_Vk.allocator, shader);
}

// ==================================================
// Render Pass
// ==================================================

PalResult PAL_CALL createVkRenderPass(
    PalDevice* device,
    const PalRenderPassCreateInfo* info,
    PalRenderPass** outRenderPass)
{
    VkResult result;
    PalRenderPass* renderpass = nullptr;
    renderpass = palAllocate(s_Vk.allocator, sizeof(PalRenderPass), 0);
    if (!renderpass) {
        return PAL_RESULT_OUT_OF_MEMORY;
    }

    VkAttachmentReference depthRef;
    VkAttachmentDescription attachments[MAX_ATTACHMENTS];
    VkAttachmentReference colorRefs[MAX_ATTACHMENTS];
    VkAttachmentReference resolveRefs[MAX_ATTACHMENTS];
    VkImageView views[MAX_ATTACHMENTS];

    renderpass->hasDepth = false;
    Uint32 colorRefCount = 0;
    Uint32 layers = 0;
    for (int i = 0; i < info->attachmentCount; i++) {
        PalAttachmentDesc* desc = &info->attachments[i];
        VkAttachmentDescription* rDesc = &attachments[i];

        rDesc->format = palFormatToVk(desc->target->image->info.format);
        rDesc->initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        rDesc->samples = vkSamplesToSamples(desc->target->image->info.sampleCount);
        rDesc->flags = 0;
        rDesc->stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        rDesc->stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;

        layers = desc->target->image->info.depthOrArraySize;
        if (desc->resolveTarget) {
            // on legacy rendering, this should make but PAL supports both
            // so we use the highest on legacy rendering
            layers = desc->resolveTarget->image->info.depthOrArraySize;
        }

        // load op
        if (desc->loadOp == PAL_LOAD_OP_CLEAR) {
            rDesc->loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;

        } else if (desc->loadOp == PAL_LOAD_OP_LOAD) {
            rDesc->loadOp = VK_ATTACHMENT_LOAD_OP_LOAD;

        } else if (desc->loadOp == PAL_LOAD_OP_DONT_CARE) {
            rDesc->loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        }

        // store op
        if (desc->storeOp == PAL_STORE_OP_STORE) {
            rDesc->storeOp = VK_ATTACHMENT_STORE_OP_STORE;

        } else if (desc->storeOp == PAL_STORE_OP_DONT_CARE) {
            rDesc->storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        }

        // add the image views from the attachments into a seperate array
        views[i] = desc->target->handle;

        if (desc->type == PAL_ATTACHMENT_TYPE_COLOR) {
            VkAttachmentReference* ref = &colorRefs[colorRefCount];
            rDesc->finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
            ref->attachment = i;
            ref->layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
            colorRefCount++;
            if (desc->target->image->belongsToSwapchain) {
                rDesc->finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
            }

        } else {
            rDesc->finalLayout = VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL;
            rDesc->stencilLoadOp = rDesc->loadOp;
            rDesc->stencilStoreOp = rDesc->storeOp;
            depthRef.attachment = i;
            depthRef.layout = VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL;
            renderpass->hasDepth = true;
        }
    }

    // subpass
    VkSubpassDescription subpassDesc = {0};
    subpassDesc.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpassDesc.colorAttachmentCount = colorRefCount;
    subpassDesc.pColorAttachments = colorRefs;
    if (renderpass->hasDepth) {
        subpassDesc.pDepthStencilAttachment = &depthRef;
    }

    // render pass
    VkRenderPassCreateInfo createInfo = {0};
    createInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    createInfo.attachmentCount = info->attachmentCount;
    createInfo.pAttachments = attachments;
    createInfo.subpassCount = 1;
    createInfo.pSubpasses = &subpassDesc;

    result = s_Vk.createRenderPass(
        device->handle, 
        &createInfo, 
        &s_Vk.vkAllocator, 
        &renderpass->handle);

    if (result != VK_SUCCESS) {
        palFree(s_Vk.allocator, renderpass);
        return vkResultToPal(result);
    }

    // create framebuffer
    VkFramebufferCreateInfo fbCreateInfo = {0};
    fbCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    fbCreateInfo.attachmentCount = info->attachmentCount;
    fbCreateInfo.pAttachments = views;
    fbCreateInfo.height = info->height;
    fbCreateInfo.width = info->width;
    fbCreateInfo.layers = layers;
    fbCreateInfo.renderPass = renderpass->handle;

    result = s_Vk.createFramebuffer(
        device->handle, 
        &fbCreateInfo, 
        &s_Vk.vkAllocator, 
        &renderpass->framebuffer);

    if (result != VK_SUCCESS) {
        s_Vk.destroyRenderPass(
            device->handle, 
            renderpass->handle, 
            &s_Vk.vkAllocator);

        palFree(s_Vk.allocator, renderpass);
        // legacy rendering needs all image views to have
        // the same layers/width/height
        return PAL_RESULT_ADAPTER_FEATURE_NOT_SUPPORTED;
    }

    renderpass->device = device; 
    renderpass->width = info->width;
    renderpass->height = info->height;
    renderpass->attachmentCount = info->attachmentCount;

    *outRenderPass = renderpass;
    return PAL_RESULT_SUCCESS;
}

void PAL_CALL destroyVkRenderPass(PalRenderPass* renderPass)
{
    s_Vk.destroyFramebuffer(
        renderPass->device->handle,
        renderPass->framebuffer, 
        &s_Vk.vkAllocator);

    s_Vk.destroyRenderPass(
        renderPass->device->handle, 
        renderPass->handle, 
        &s_Vk.vkAllocator);
    
    palFree(s_Vk.allocator, renderPass);
}

// ==================================================
// Fence
// ==================================================

PalResult PAL_CALL createVkFence(
    PalDevice* device,
    PalFence** outFence)
{
    VkResult result;
    PalFence* fence = nullptr;
    fence = palAllocate(s_Vk.allocator, sizeof(PalFence), 0);
    if (!fence) {
        return PAL_RESULT_OUT_OF_MEMORY;
    }

    VkFenceCreateInfo createInfo = {0};
    createInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    result = s_Vk.createFence(
        device->handle, 
        &createInfo, 
        &s_Vk.vkAllocator, 
        &fence->handle);

    if (result != VK_SUCCESS) {
        palFree(s_Vk.allocator, fence);
        return vkResultToPal(result);
    }

    fence->device = device;
    *outFence = fence;
    return PAL_RESULT_SUCCESS;
}

void PAL_CALL destroyVkFence(PalFence* fence)
{
    s_Vk.destroyFence(fence->device->handle, fence->handle, &s_Vk.vkAllocator);
    palFree(s_Vk.allocator, fence);
}

PalResult PAL_CALL waitVkFence(
    PalFence* fence, 
    Uint64 timeout)
{
    VkResult result = s_Vk.waitFence(
        fence->device->handle, 
        1, 
        &fence->handle, 
        true,
        timeout);

    if (result != VK_SUCCESS) {
        return vkResultToPal(result);
    }
    return PAL_RESULT_SUCCESS;
}

PalResult PAL_CALL resetVkFence(PalFence* fence)
{
    if (!(fence->device->features & PAL_ADAPTER_FEATURE_FENCE_RESET)) {
        return PAL_RESULT_ADAPTER_FEATURE_NOT_SUPPORTED;
    }

    VkResult ret = s_Vk.resetFence(fence->device->handle, 1, &fence->handle);
    if (ret != VK_SUCCESS) {
        return vkResultToPal(ret);
    }

    return PAL_RESULT_SUCCESS;
}

bool PAL_CALL isVkFenceSignaled(PalFence* fence)
{
    VkResult ret = s_Vk.isFenceSignaled(fence->device->handle, fence->handle);
    if (ret == VK_SUCCESS) {
        return true;
    } else {
        return false;
    }
}

// ==================================================
// Semaphore
// ==================================================

PalResult PAL_CALL createVkSemaphore(
    PalDevice* device,
    PalSemaphore** outSemaphore)
{
    VkResult result;
    PalSemaphore* semaphore = nullptr;
    semaphore = palAllocate(s_Vk.allocator, sizeof(PalSemaphore), 0);
    if (!semaphore) {
        return PAL_RESULT_OUT_OF_MEMORY;
    }

    VkSemaphoreTypeCreateInfo timelineCreateInfo = {0};
    timelineCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_TYPE_CREATE_INFO;
    timelineCreateInfo.semaphoreType = VK_SEMAPHORE_TYPE_TIMELINE;

    VkSemaphoreCreateInfo createInfo = {0};
    createInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    const void* next = nullptr;
    semaphore->isTimeline = false;
    if (device->features & PAL_ADAPTER_FEATURE_TIMELINE_SEMAPHORE) {
        next = &timelineCreateInfo;
        semaphore->isTimeline = true;
    }

    createInfo.pNext = next;
    result = s_Vk.createSemaphore(
        device->handle,
        &createInfo, 
        &s_Vk.vkAllocator, 
        &semaphore->handle);

    if (result != VK_SUCCESS) {
        palFree(s_Vk.allocator, semaphore);
        return vkResultToPal(result);
    }

    semaphore->device = device;
    *outSemaphore = semaphore;
    return PAL_RESULT_SUCCESS;
}

void PAL_CALL destroyVkSemaphore(PalSemaphore* semaphore)
{
    s_Vk.destroySemaphore(
        semaphore->device->handle, 
        semaphore->handle, 
        &s_Vk.vkAllocator);

    palFree(s_Vk.allocator, semaphore);
}

PalResult PAL_CALL waitVkSemaphore(
    PalSemaphore* semaphore, 
    PalQueue* queue,
    Uint64 value,
    Uint64 timeout)
{
    VkResult result;
    VkSemaphoreWaitInfo waitInfo = {0};
    waitInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_WAIT_INFO;
    waitInfo.semaphoreCount = 1;
    waitInfo.pSemaphores = &semaphore->handle;
    waitInfo.pValues = &value;

    result = s_Vk.waitSemaphores(
        semaphore->device->handle, 
        &waitInfo, 
        timeout);

    if (result != VK_SUCCESS) {
        return vkResultToPal(result);
    }

    return PAL_RESULT_SUCCESS;
}

PalResult PAL_CALL signalVkSemaphore(
    PalSemaphore* semaphore,
    PalQueue* queue,
    Uint64 value)
{
    VkResult result;
    VkSemaphoreSignalInfo signalInfo = {0};
    signalInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_SIGNAL_INFO;
    signalInfo.semaphore = semaphore->handle;
    signalInfo.value = value;

    result = s_Vk.signalSemaphore(semaphore->device->handle, &signalInfo);
    if (result != VK_SUCCESS) {
        return vkResultToPal(result);
    }

    return PAL_RESULT_SUCCESS;
}

PalResult PAL_CALL getVkSemaphoreValue(
    PalSemaphore* semaphore, 
    Uint64* value) 
{
    VkResult result = s_Vk.getSemaphoreValue(
        semaphore->device->handle, 
        semaphore->handle, 
        value);

    if (result != VK_SUCCESS) {
        return vkResultToPal(result);
    }

    return PAL_RESULT_SUCCESS;
}

// ==================================================
// Command Pool And Buffer
// ==================================================

PalResult PAL_CALL createVkCommandPool(
    PalDevice* device,
    const PalCommandPoolCreateInfo* info,
    PalCommandPool** outPool)
{
    VkResult result;
    PalCommandPool* pool = nullptr;
    PhysicalQueue* phyQueue = info->queue->phyQueue;

    pool = palAllocate(s_Vk.allocator, sizeof(PalCommandPool), 0);
    if (!pool) {
        return PAL_RESULT_OUT_OF_MEMORY;
    }

    VkCommandPoolCreateInfo createInfo = {0};
    createInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    createInfo.queueFamilyIndex = phyQueue->familyIndex;

    if (info->resettable) {
        createInfo.flags |= VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    }

    if (info->transient) {
        createInfo.flags |= VK_COMMAND_POOL_CREATE_TRANSIENT_BIT;
    }

    result = s_Vk.createCommandPool(
        device->handle, 
        &createInfo, 
        &s_Vk.vkAllocator, 
        &pool->handle);

    if (result != VK_SUCCESS) {
        palFree(s_Vk.allocator, pool);
        return vkResultToPal(result);
    }

    pool->device = device;
    *outPool = pool;
    return PAL_RESULT_SUCCESS;
}

void PAL_CALL destroyVkCommandPool(PalCommandPool* pool)
{
    s_Vk.destroyCommandPool(
        pool->device->handle, 
        pool->handle, 
        &s_Vk.vkAllocator);

    palFree(s_Vk.allocator, pool);
}

PalResult PAL_CALL createVkCommandBuffer(
    PalDevice* device,
    PalCommandPool* pool,
    bool primary,
    PalCommandBuffer** outCmdBuffer)
{
    VkResult result;
    PalCommandBuffer* cmdBuffer = nullptr;
    cmdBuffer = palAllocate(s_Vk.allocator, sizeof(PalCommandBuffer), 0);
    if (!cmdBuffer) {
        PAL_RESULT_OUT_OF_MEMORY;
    }

    VkCommandBufferAllocateInfo createInfo = {0};
    createInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    createInfo.commandBufferCount = 1;
    createInfo.commandPool = pool->handle;
    createInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    if (!primary) {
        createInfo.level = VK_COMMAND_BUFFER_LEVEL_SECONDARY;
    }

    result = s_Vk.createCommandBuffer(
        device->handle, 
        &createInfo, 
        &cmdBuffer->handle);

    if (result != VK_SUCCESS) {
        palFree(s_Vk.allocator, cmdBuffer);
        return vkResultToPal(result);
    }

    if (primary) {
        cmdBuffer->primary = true;
    } else {
        cmdBuffer->primary = false;
    }

    cmdBuffer->device = device;
    cmdBuffer->pool = pool->handle;
    *outCmdBuffer = cmdBuffer;
    return PAL_RESULT_SUCCESS;
}

void PAL_CALL destroyVkCommandBuffer(PalCommandBuffer* cmdBuffer)
{
    s_Vk.destroyCommandBuffer(
        cmdBuffer->device->handle,
        cmdBuffer->pool,
        1,
        &cmdBuffer->handle);

    palFree(s_Vk.allocator, cmdBuffer);
}

PalResult PAL_CALL executeCommandBufferVk(
    PalCommandBuffer* primaryCmdBuffer,
    PalCommandBuffer* secondaryCmdBuffer)
{
    // check if both are primary cmd buffers
    if (primaryCmdBuffer->primary && secondaryCmdBuffer->primary) {
        return PAL_RESULT_INVALID_OPERATION;
    }

    if (!primaryCmdBuffer->primary && !secondaryCmdBuffer->primary) {
        return PAL_RESULT_INVALID_OPERATION;
    }

    if (!primaryCmdBuffer->primary) {
        return PAL_RESULT_INVALID_OPERATION;
    }

    s_Vk.cmdExecuteCommandBuffer(
        primaryCmdBuffer->handle, 
        1,
        &secondaryCmdBuffer->handle);

    return PAL_RESULT_SUCCESS;
}

PalResult PAL_CALL beginRenderPassVk(
    PalCommandBuffer* cmdBuffer,
    PalRenderPass* renderPass,
    Int32 clearValueCount,
    PalClearValue* clearValues)
{
    if (clearValueCount != renderPass->attachmentCount) {
        return PAL_RESULT_INSUFFICIENT_BUFFER;
    }

    VkCommandBufferBeginInfo cmdBeginInfo = {0};
    cmdBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

    VkResult result = s_Vk.cmdBegin(cmdBuffer->handle, &cmdBeginInfo);
    if (result != VK_SUCCESS) {
        return vkResultToPal(result);
    }

    VkRenderPassBeginInfo renderPassBeginInfo = {0};
    renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    VkClearValue tmp[MAX_ATTACHMENTS];

    for (int i = 0; i < clearValueCount; i++) {
        PalClearValue* clearValue = &clearValues[i];
        if (clearValue->depth == 0 && clearValue->stencil == 0) {
            // color attachment
            tmp[i].color.float32[0] = clearValue->color[0];
            tmp[i].color.float32[1] = clearValue->color[1];
            tmp[i].color.float32[2] = clearValue->color[2];
            tmp[i].color.float32[3] = clearValue->color[3];

        } else {
            // depth/stencil attachment
            tmp[i].depthStencil.depth = clearValue->depth;
            tmp[i].depthStencil.stencil = clearValue->stencil;
        }
    }

    renderPassBeginInfo.clearValueCount = clearValueCount;
    renderPassBeginInfo.pClearValues = tmp;
    renderPassBeginInfo.framebuffer = renderPass->framebuffer;
    renderPassBeginInfo.renderPass = renderPass->handle;
    renderPassBeginInfo.renderArea.extent.width = renderPass->width;
    renderPassBeginInfo.renderArea.extent.height = renderPass->height;

    s_Vk.cmdBeginRenderPass(
        cmdBuffer->handle, 
        &renderPassBeginInfo, 
        VK_SUBPASS_CONTENTS_INLINE);

    return PAL_RESULT_SUCCESS;
}

PalResult PAL_CALL endRenderPassVk(PalCommandBuffer* cmdBuffer)
{
    VkResult result = s_Vk.cmdEnd(cmdBuffer->handle);
    if (result != VK_SUCCESS) {
        return vkResultToPal(result);
    }

    s_Vk.cmdEndRenderPass(cmdBuffer->handle);
    return PAL_RESULT_SUCCESS;
}

PalResult PAL_CALL submitVkCommandBuffer(
    PalQueue* queue,
    PalSubmitInfo* info)
{
    VkResult result;
    Int32 waitSemaphoreCount = 0;
    Int32 signalSemaphoreCount = 0;
    VkFence fenceHandle = nullptr;
    VkSemaphore waitSemaphoreHandle = nullptr;
    VkSemaphore signalSemaphoreHandle = nullptr;

    if (info->waitSemaphore) {
        waitSemaphoreHandle = info->waitSemaphore->handle;
        waitSemaphoreCount = 1;
    }

    if (info->signalSemaphore) {
        signalSemaphoreHandle = info->signalSemaphore->handle;
        signalSemaphoreCount = 1;
    }

    if (info->fence) {
        fenceHandle = info->fence->handle; 
    }

    VkSubmitInfo submitInfo = {0};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &info->cmdBuffer->handle;
    submitInfo.pSignalSemaphores = &signalSemaphoreHandle;
    submitInfo.pWaitSemaphores = &waitSemaphoreHandle;
    submitInfo.waitSemaphoreCount = waitSemaphoreCount;
    submitInfo.waitSemaphoreCount = signalSemaphoreCount;

    result = s_Vk.queueSubmit(
        queue->phyQueue->handle,
        1,
        &submitInfo, 
        fenceHandle);

    if (result != VK_SUCCESS) {
        return vkResultToPal(result);
    }

    return PAL_RESULT_SUCCESS;
}

// ==================================================
// Pipeline
// ==================================================

PalResult PAL_CALL createVkGraphicsPipeline(
    PalDevice* device,
    const PalGraphicsPipelineCreateInfo* info,
    PalPipeline** outPipeline)
{

}

void PAL_CALL destroyVkPipeline(PalPipeline* pipeline)
{

}

#endif // PAL_HAS_VULKAN