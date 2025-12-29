
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
    const PalGraphicsBackend* backend;
    VkPhysicalDevice handle;
} Adapter;

typedef struct {
    bool hasDebug;
    void* handle;
    Adapter* adapters;
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

    PFN_vkBeginCommandBuffer cmdBegin;
    PFN_vkEndCommandBuffer cmdEnd;
    PFN_vkCmdExecuteCommands cmdExecuteCommandBuffer;
    PFN_vkQueueSubmit queueSubmit;
    PFN_vkCmdBeginRenderPass cmdBeginRenderPass;
    PFN_vkCmdEndRenderPass cmdEndRenderPass;
    PFN_vkCmdCopyBuffer cmdCopyBuffer;

    PFN_vkCreateWaylandSurfaceKHR createWaylandSurface;
    PFN_vkGetPhysicalDeviceWaylandPresentationSupportKHR checkWaylandPresentSupport;
    PFN_vkCreateXlibSurfaceKHR createXlibSurface;
    PFN_vkGetPhysicalDeviceXlibPresentationSupportKHR checkXlibPresentSupport;

    PFN_vkDestroySurfaceKHR destroySurface;
    PFN_vkGetPhysicalDeviceSurfaceCapabilitiesKHR getSurfaceCapabilities;
    PFN_vkGetPhysicalDeviceSurfaceFormatsKHR getSurfaceFormats;
    PFN_vkGetPhysicalDeviceSurfacePresentModesKHR getSurfacePresentModes;

    PFN_vkCreateBuffer createBuffer;
    PFN_vkDestroyBuffer destroyBuffer;
    PFN_vkGetBufferMemoryRequirements getBufferMemoryRequirements;
    PFN_vkBindBufferMemory bindBufferMemory;
    PFN_vkMapMemory mapMemory;
    PFN_vkUnmapMemory unmapMemory;

    PFN_vkCreateGraphicsPipelines createGraphicsPipeline;
    PFN_vkDestroyPipeline destroyPipeline;

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

typedef struct {
    const PalGraphicsBackend* backend;

    PalAdapterFeatures features;
    Int32 queueCount;
    Int32 gpuOnlyMemoryIndex;
    Int32 cpuUploadMemoryIndex;
    Int32 cpuReadbackMemoryIndex;
    VkPhysicalDevice phyDevice;
    VkDevice handle;
    PhysicalQueue* phyQueues;
    PFN_vkCreateRenderPass2 createRenderPass2;
    PFN_vkGetBufferDeviceAddress getBufferrAddress;

    // swapchain
    PFN_vkCreateSwapchainKHR createSwapchain;
    PFN_vkDestroySwapchainKHR destroySwapchain;
    PFN_vkGetSwapchainImagesKHR getSwapchainImages;
    PFN_vkAcquireNextImageKHR acquireNextImage;
    PFN_vkQueuePresentKHR queuePresent;

    // semaphore
    PFN_vkCreateSemaphore createSemaphore;
    PFN_vkDestroySemaphore destroySemaphore;
    PFN_vkWaitSemaphores waitSemaphore;
    PFN_vkSignalSemaphore signalSemaphore;
    PFN_vkGetSemaphoreCounterValue getSemaphoreValue;

    // fragment shading rate
    PFN_vkCmdSetFragmentShadingRateKHR cmdSetFragmentShadingRate;

    // mesh shader
    PFN_vkCmdDrawMeshTasksEXT cmdDrawMeshTask;
    PFN_vkCmdDrawMeshTasksIndirectEXT cmdDrawMeshTaskIndirect;
    PFN_vkCmdDrawMeshTasksIndirectCountEXT cmdDrawMeshTaskIndirectCount;

    // ray tracing
    PFN_vkCreateAccelerationStructureKHR createAccelerationStructure;
    PFN_vkDestroyAccelerationStructureKHR destroyAccelerationStructure;
    PFN_vkGetAccelerationStructureBuildSizesKHR getAccelerationBuildsize;
    PFN_vkCmdBuildAccelerationStructuresKHR cmdBuildAccelerationStructures;
    PFN_vkGetAccelerationStructureDeviceAddressKHR getAccelerationDeviceAddress;

    PFN_vkCmdTraceRaysKHR cmdTraceRays;
    PFN_vkCreateRayTracingPipelinesKHR createRayTracingPipeline;
    PFN_vkCmdTraceRaysIndirectKHR cmdTraceRaysIndirect;
} Device;

typedef struct {
    const PalGraphicsBackend* backend;

    VkQueueFlags usage;
    PalDevice* device;
    PhysicalQueue* phyQueue;
} Queue;

typedef struct {
    const PalGraphicsBackend* backend;

    bool belongsToSwapchain;
    Int32 index;
    Device* device;
    VkImage handle;
    PalImageInfo info;
} Image;

typedef struct {
    const PalGraphicsBackend* backend;

    VkImageViewType type;
    PalImageViewUsages usages;
    Device* device;
    Image* image;
    VkImageView handle;
} ImageView;

typedef struct {
    const PalGraphicsBackend* backend;

    Uint32 imageCount;
    Device* device;
    Queue* queue;
    VkSurfaceKHR surface;
    VkSwapchainKHR handle;
    Image* images;
} Swapchain;

typedef struct {
    const PalGraphicsBackend* backend;

    Device* device;
    VkShaderModule handle;
    VkPipelineShaderStageCreateInfo info;
} Shader;

typedef struct {
    const PalGraphicsBackend* backend;
    
    Uint32 attachmentCount;
    Uint32 width;
    Uint32 height;
    Device* device;
    VkRenderPass handle;
    VkFramebuffer framebuffer;
} RenderPass;

typedef struct {
    const PalGraphicsBackend* backend;

    Device* device;
    VkCommandPool handle;
} CommandPool;

typedef struct {
    const PalGraphicsBackend* backend;

    bool primary;
    Device* device;
    VkCommandPool pool;
    VkCommandBuffer handle;
} CommandBuffer;

typedef struct {
    const PalGraphicsBackend* backend;

    Device* device;
    VkFence handle;
} Fence;

typedef struct {
    const PalGraphicsBackend* backend;

    bool isTimeline;
    Device* device;
    VkSemaphore handle;
} Semaphore;

typedef struct {
    const PalGraphicsBackend* backend;

    VkDeviceAddress address;
    Device* device;
    VkBuffer handle;
} Buffer;

typedef struct {
    const PalGraphicsBackend* backend;

    PalBuffer* buffer;
    Device* device;
    VkBufferView handle;
} BufferView;

typedef struct {
    const PalGraphicsBackend* backend;

    VkDeviceAddress address;
    Device* device;
    VkAccelerationStructureKHR handle;
} AccelerationStructure;

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

static VkImageUsageFlags palImageUsageToVk(PalImageUsages usages) 
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

static VkExtent2D getShadingRateSize(PalFragmentShadingRate rate)
{
    switch (rate) {
        case PAL_FRAGMENT_SHADING_RATE_1X1:
            return (VkExtent2D){1, 1};

        case PAL_FRAGMENT_SHADING_RATE_1X2:
            return (VkExtent2D){1, 2};

        case PAL_FRAGMENT_SHADING_RATE_2X1:
            return (VkExtent2D){2, 1};

        case PAL_FRAGMENT_SHADING_RATE_2X2:
            return (VkExtent2D){2, 2};

        case PAL_FRAGMENT_SHADING_RATE_2X4:
            return (VkExtent2D){2, 4};

        case PAL_FRAGMENT_SHADING_RATE_4X2:
            return (VkExtent2D){4, 2};

        case PAL_FRAGMENT_SHADING_RATE_4X4:
            return (VkExtent2D){4, 4};
    }

    return (VkExtent2D){0, 0};
}

static VkFormat vertexTypeToVkFormat(PalVertexType type)
{
    switch (type) {
        case PAL_VERTEX_TYPE_INT32:
            return VK_FORMAT_R32_SINT;

        case PAL_VERTEX_TYPE_INT32_2:
            return VK_FORMAT_R32G32_SINT;

        case PAL_VERTEX_TYPE_INT32_3:
            return VK_FORMAT_R32G32B32_SINT;

        case PAL_VERTEX_TYPE_INT32_4:
            return VK_FORMAT_R32G32B32A32_SINT;

        case PAL_VERTEX_TYPE_UINT32:
            return VK_FORMAT_R32_UINT;

        case PAL_VERTEX_TYPE_UINT32_2:
            return VK_FORMAT_R32G32_UINT;

        case PAL_VERTEX_TYPE_UINT32_3:
            return VK_FORMAT_R32G32B32_UINT;

        case PAL_VERTEX_TYPE_UINT32_4:
            return VK_FORMAT_R32G32B32A32_UINT;


        case PAL_VERTEX_TYPE_INT8_2:
            return VK_FORMAT_R8G8_SINT;

        case PAL_VERTEX_TYPE_INT8_4:
            return VK_FORMAT_R8G8B8A8_SINT;

        case PAL_VERTEX_TYPE_UINT8_2:
            return VK_FORMAT_R8G8_UINT;

        case PAL_VERTEX_TYPE_UINT8_4:
            return VK_FORMAT_R8G8B8A8_UINT;


        case PAL_VERTEX_TYPE_INT8_2NORM:
            return VK_FORMAT_R8G8_SNORM;

        case PAL_VERTEX_TYPE_INT8_4NORM:
            return VK_FORMAT_R8G8B8A8_SNORM;

        case PAL_VERTEX_TYPE_UINT8_2NORM:
            return VK_FORMAT_R8G8_UNORM;

        case PAL_VERTEX_TYPE_UINT8_4NORM:
            return VK_FORMAT_R8G8B8A8_UNORM;


        case PAL_VERTEX_TYPE_INT16_2:
            return VK_FORMAT_R16G16_SINT;

        case PAL_VERTEX_TYPE_INT16_4:
            return VK_FORMAT_R16G16B16A16_SINT;

        case PAL_VERTEX_TYPE_UINT16_2:
            return VK_FORMAT_R16G16_UINT;

        case PAL_VERTEX_TYPE_UINT16_4:
            return VK_FORMAT_R16G16B16A16_UINT;


        case PAL_VERTEX_TYPE_INT16_2NORM:
            return VK_FORMAT_R16G16_SNORM;

        case PAL_VERTEX_TYPE_INT16_4NORM:
            return VK_FORMAT_R16G16B16A16_SNORM;

        case PAL_VERTEX_TYPE_UINT16_2NORM:
            return VK_FORMAT_R16G16_UNORM;

        case PAL_VERTEX_TYPE_UINT16_4NORM:
            return VK_FORMAT_R16G16B16A16_UNORM;


        case PAL_VERTEX_TYPE_FLOAT:
            return VK_FORMAT_R32_SFLOAT;

        case PAL_VERTEX_TYPE_FLOAT2:
            return VK_FORMAT_R32G32_SFLOAT;

        case PAL_VERTEX_TYPE_FLOAT3:
            return VK_FORMAT_R32G32B32_SFLOAT;

        case PAL_VERTEX_TYPE_FLOAT4:
            return VK_FORMAT_R32G32B32A32_SFLOAT;


        case PAL_VERTEX_TYPE_HALF_FLOAT16_2:
            return VK_FORMAT_R16G16_SFLOAT;

        case PAL_VERTEX_TYPE_HALF_FLOAT16_4:
            return VK_FORMAT_R16G16B16A16_SFLOAT;
    }

    return VK_FORMAT_UNDEFINED;
}

static VkBufferUsageFlags palBufferUsageToVk(PalBufferUsages usages) 
{
    VkBufferUsageFlags flags = 0;
    if (usages & PAL_BUFFER_USAGE_VERTEX) {
        flags |= VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
    }

    if (usages & PAL_BUFFER_USAGE_INDEX) {
        flags |= VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
    }

    if (usages & PAL_BUFFER_USAGE_UNIFORM) {
        flags |= VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
    }

    if (usages & PAL_BUFFER_USAGE_STORAGE) {
        flags |= VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
    }

    if (usages & PAL_BUFFER_USAGE_TRANSFER_SRC) {
        flags |= VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
    }

    if (usages & PAL_BUFFER_USAGE_TRANSFER_DST) {
        flags |= VK_BUFFER_USAGE_TRANSFER_DST_BIT;
    }

    if (usages & PAL_BUFFER_USAGE_RAY_TRACING) {
        flags |= VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT;
        flags |= 
          VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_BUILD_INPUT_READ_ONLY_BIT_KHR;
    }
    
    if (usages & PAL_BUFFER_USAGE_DEVICE_ADDRESS) {
        flags |= VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT;
    }

    return flags;
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

    s_Vk.cmdCopyBuffer = (PFN_vkCmdCopyBuffer)dlsym(
        s_Vk.handle, 
        "vkCmdCopyBuffer");

    s_Vk.queueSubmit = (PFN_vkQueueSubmit)dlsym(
        s_Vk.handle, 
        "vkQueueSubmit");

    s_Vk.createBuffer = (PFN_vkCreateBuffer)dlsym(
        s_Vk.handle, 
        "vkCreateBuffer");

    s_Vk.destroyBuffer = (PFN_vkDestroyBuffer)dlsym(
        s_Vk.handle, 
        "vkDestroyBuffer");

    s_Vk.mapMemory = (PFN_vkMapMemory)dlsym(
        s_Vk.handle, 
        "vkMapMemory");

    s_Vk.unmapMemory = (PFN_vkUnmapMemory)dlsym(
        s_Vk.handle, 
        "vkUnmapMemory");

    s_Vk.getBufferMemoryRequirements = (PFN_vkGetBufferMemoryRequirements)dlsym(
        s_Vk.handle, 
        "vkGetBufferMemoryRequirements");

    s_Vk.bindBufferMemory = (PFN_vkBindBufferMemory)dlsym(
        s_Vk.handle, 
        "vkBindBufferMemory");

    s_Vk.createGraphicsPipeline = (PFN_vkCreateGraphicsPipelines)dlsym(
        s_Vk.handle, 
        "vkCreateGraphicsPipelines");

    s_Vk.destroyPipeline = (PFN_vkDestroyPipeline)dlsym(
        s_Vk.handle, 
        "vkDestroyPipeline");

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

    s_Vk.adapters = nullptr;
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

    if (s_Vk.adapters) {
        palFree(s_Vk.allocator, s_Vk.adapters);
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

    if (s_Vk.adapters) {
        // free the adapters and cache them again
        palFree(s_Vk.allocator, s_Vk.adapters);
    }

    s_Vk.adapters = palAllocate(
        s_Vk.allocator, 
        sizeof(Adapter) * deviceCount, 
        0);

    s_Vk.enumeratePhysicalDevices(s_Vk.instance, &deviceCount, devices);
    for (int i = 0; i < *count && i < deviceCount; i++) {
        Adapter* tmp = &s_Vk.adapters[i];
        tmp->handle = devices[i];
        outAdapters[i] = (PalAdapter*)tmp;
    }

    palFree(s_Vk.allocator, devices);
    return PAL_RESULT_SUCCESS;
}

PalResult PAL_CALL getVkAdapterInfo(
    PalAdapter* adapter,
    PalAdapterInfo* info)
{
    Adapter* vkAdapter = (Adapter*)adapter;
    VkPhysicalDevice phyDevice = (VkPhysicalDevice)vkAdapter->handle;
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
    Adapter* vkAdapter = (Adapter*)adapter;
    VkPhysicalDevice phyDevice = (VkPhysicalDevice)vkAdapter->handle;

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
    VkPhysicalDeviceProperties props = {0};

    Adapter* vkAdapter = (Adapter*)adapter;
    VkPhysicalDevice phyDevice = (VkPhysicalDevice)vkAdapter->handle;

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
    bool dynamicstate = false;
    bool bufferDeviceAddress = false;

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
            dynamicstate = true;

        } else if (strcmp(props->extensionName, "VK_EXT_extended_dynamic_state2") == 0) {
            VkPhysicalDeviceExtendedDynamicState2FeaturesEXT dynState2 = {0};
            dynState2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_EXTENDED_DYNAMIC_STATE_2_FEATURES_EXT;

            VkPhysicalDeviceFeatures2 features;
            features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
            features.pNext = &dynState2;

            s_Vk.getPhysicalDeviceFeatures2(phyDevice, &features);
            if (dynState2.extendedDynamicState2) {
                adapterFeatures |= PAL_ADAPTER_FEATURE_DYNAMIC_DEPTH_TEST_ENABLE;
                adapterFeatures |= PAL_ADAPTER_FEATURE_DYNAMIC_DEPTH_WRITE_ENABLE;
                adapterFeatures |= PAL_ADAPTER_FEATURE_DYNAMIC_STENCIL_OP;
            }

        } else if (strcmp(props->extensionName, "VK_KHR_depth_stencil_resolve") == 0) {
            adapterFeatures |= PAL_ADAPTER_FEATURE_DEPTH_STENCIL_RESOLVE;

        } else if (strcmp(props->extensionName, "VK_KHR_draw_indirect_count") == 0) {
            adapterFeatures |= PAL_ADAPTER_FEATURE_MESH_SHADER_INDIRECT_COUNT;

        } else if (strcmp(props->extensionName, "VK_KHR_buffer_device_address") == 0) {
            bufferDeviceAddress = true;
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

        if (frag.attachmentFragmentShadingRate) {
            adapterFeatures |= 
                PAL_ADAPTER_FEATURE_FRAGMENT_SHADING_RATE_ATTACHMENT;
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

    // dynamic state is part of core 1.3
    if (props.apiVersion >= VK_API_VERSION_1_3 || dynamicstate) {
        VkPhysicalDeviceExtendedDynamicStateFeaturesEXT dynState = {0};
        dynState.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_EXTENDED_DYNAMIC_STATE_FEATURES_EXT;

        VkPhysicalDeviceFeatures2 features;
        features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
        features.pNext = &dynState;

        s_Vk.getPhysicalDeviceFeatures2(phyDevice, &features);
        if (dynState.extendedDynamicState) {
            adapterFeatures |= PAL_ADAPTER_FEATURE_DYNAMIC_CULL_MODE;
            adapterFeatures |= PAL_ADAPTER_FEATURE_DYNAMIC_FRONT_FACE;
            adapterFeatures |= PAL_ADAPTER_FEATURE_DYNAMIC_PRIMITIVE_TOPOLOGY;
        }
    }

    // buffer device address is part of core 1.2
    // if ray tracing is supported, buffer device address will be supported as well
    if (adapterFeatures & PAL_ADAPTER_FEATURE_RAY_TRACING) {
        adapterFeatures |= PAL_ADAPTER_FEATURE_BUFFER_DEVICE_ADDRESS;
    } else {
        if (props.apiVersion >= VK_API_VERSION_1_2 || bufferDeviceAddress) {
            VkPhysicalDeviceBufferDeviceAddressFeaturesKHR bufferAddress = {0};
            bufferAddress.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_BUFFER_DEVICE_ADDRESS_FEATURES_KHR;

            VkPhysicalDeviceFeatures2 features;
            features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
            features.pNext = &bufferAddress;

            s_Vk.getPhysicalDeviceFeatures2(phyDevice, &features);
            if (bufferAddress.bufferDeviceAddress) {
                adapterFeatures |= PAL_ADAPTER_FEATURE_BUFFER_DEVICE_ADDRESS;
            }
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
    Device* device = nullptr;
    VkPhysicalDeviceProperties props = {0};

    Adapter* vkAdapter = (Adapter*)adapter;
    VkPhysicalDevice phyDevice = (VkPhysicalDevice)vkAdapter->handle;
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

    device = palAllocate(s_Vk.allocator, sizeof(Device), 0);
    if (!queueProps || !queueCreateInfos || !device) {
        return PAL_RESULT_OUT_OF_MEMORY;
    }

    memset(device, 0, sizeof(Device));
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

    VkPhysicalDeviceExtendedDynamicStateFeaturesEXT dynamicState = {0};
    dynamicState.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_EXTENDED_DYNAMIC_STATE_FEATURES_EXT;

    VkPhysicalDeviceExtendedDynamicState2FeaturesEXT dynamicState2 = {0};
    dynamicState2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_EXTENDED_DYNAMIC_STATE_2_FEATURES_EXT;

    VkPhysicalDeviceBufferDeviceAddressFeaturesKHR bufferAddress = {0};
    bufferAddress.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_BUFFER_DEVICE_ADDRESS_FEATURES_KHR;

    // clang-format on
    if (features & PAL_ADAPTER_FEATURE_SWAPCHAIN) {
        extensions[extCount++] = "VK_KHR_swapchain";
    }

    if (features & PAL_ADAPTER_FEATURE_DEPTH_STENCIL_RESOLVE) {
        extensions[extCount++] = "VK_KHR_depth_stencil_resolve";
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

    if ((features & PAL_ADAPTER_FEATURE_MESH_SHADER) || 
       (features & PAL_ADAPTER_FEATURE_MESH_SHADER_INDIRECT_COUNT)) {
        if (props.apiVersion < VK_API_VERSION_1_3) {
            extensions[extCount++] = "VK_EXT_mesh_shader";
        }

        if (features & PAL_ADAPTER_FEATURE_MESH_SHADER_INDIRECT_COUNT) {
            if (props.apiVersion < VK_API_VERSION_1_2) {
                extensions[extCount++] = "VK_KHR_draw_indirect_count";
            }
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

    if ((features & PAL_ADAPTER_FEATURE_FRAGMENT_SHADING_RATE) || 
       (features & PAL_ADAPTER_FEATURE_FRAGMENT_SHADING_RATE_ATTACHMENT)) {
        if (props.apiVersion < VK_API_VERSION_1_3) {
            extensions[extCount++] = "VK_KHR_fragment_shading_rate";
            vrs.pipelineFragmentShadingRate = true;
    
            // fragment shading rate attachment needs this
            if (features & PAL_ADAPTER_FEATURE_FRAGMENT_SHADING_RATE_ATTACHMENT) {
                if (props.apiVersion < VK_API_VERSION_1_2) {
                    extensions[extCount++] = "VK_KHR_create_renderpass2";
                }
                vrs.attachmentFragmentShadingRate = true;
            }
        }

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

    if (features & PAL_ADAPTER_FEATURE_DYNAMIC_CULL_MODE || 
        features & PAL_ADAPTER_FEATURE_DYNAMIC_FRONT_FACE || 
        features & PAL_ADAPTER_FEATURE_DYNAMIC_PRIMITIVE_TOPOLOGY) {
        if (props.apiVersion < VK_API_VERSION_1_3) {
            extensions[extCount++] = "VK_EXT_extended_dynamic_state";
        }

        dynamicState.extendedDynamicState = true;
        if (multiView.multiview) {
            multiView.pNext = &dynamicState;

        } else if (descIndex.shaderSampledImageArrayNonUniformIndexing) {
            descIndex.pNext = &dynamicState;

        } else if (vrs.pipelineFragmentShadingRate) {
            vrs.pNext = &dynamicState;

        } else if (mesh.meshShader) {
            mesh.pNext = &dynamicState;

        } else if (acc.accelerationStructure) {
            acc.pNext = &dynamicState;

        } else if (shader16.shaderFloat16) {
            shader16.pNext = &dynamicState;

        } else if (timeline.timelineSemaphore) {
            timeline.pNext = &dynamicState;
        }
        start = &dynamicState;
    }

    if (features & PAL_ADAPTER_FEATURE_DYNAMIC_DEPTH_TEST_ENABLE || 
        features & PAL_ADAPTER_FEATURE_DYNAMIC_DEPTH_WRITE_ENABLE || 
        features & PAL_ADAPTER_FEATURE_DYNAMIC_STENCIL_OP) {
        extensions[extCount++] = "VK_EXT_extended_dynamic_state2";
        dynamicState2.extendedDynamicState2 = true;

        if (dynamicState.extendedDynamicState) {
            dynamicState.pNext = &dynamicState2;
            
        } else if (multiView.multiview) {
            multiView.pNext = &dynamicState2;

        } else if (descIndex.shaderSampledImageArrayNonUniformIndexing) {
            descIndex.pNext = &dynamicState2;

        } else if (vrs.pipelineFragmentShadingRate) {
            vrs.pNext = &dynamicState2;

        } else if (mesh.meshShader) {
            mesh.pNext = &dynamicState2;

        } else if (acc.accelerationStructure) {
            acc.pNext = &dynamicState2;

        } else if (shader16.shaderFloat16) {
            shader16.pNext = &dynamicState2;

        } else if (timeline.timelineSemaphore) {
            timeline.pNext = &dynamicState2;
        }
        start = &dynamicState2;
    }

    if (features & PAL_ADAPTER_FEATURE_BUFFER_DEVICE_ADDRESS) {
        if (props.apiVersion < VK_API_VERSION_1_2) {
            extensions[extCount++] = "VK_KHR_buffer_device_address";
        }
        bufferAddress.bufferDeviceAddress = true;
        
        if (dynamicState2.extendedDynamicState2) {
            dynamicState2.pNext = &bufferAddress;

        } else if (dynamicState.extendedDynamicState) {
            dynamicState.pNext = &dynamicState2;
            
        } else if (multiView.multiview) {
            multiView.pNext = &dynamicState2;

        } else if (descIndex.shaderSampledImageArrayNonUniformIndexing) {
            descIndex.pNext = &dynamicState2;

        } else if (vrs.pipelineFragmentShadingRate) {
            vrs.pNext = &dynamicState2;

        } else if (mesh.meshShader) {
            mesh.pNext = &dynamicState2;

        } else if (acc.accelerationStructure) {
            acc.pNext = &dynamicState2;

        } else if (shader16.shaderFloat16) {
            shader16.pNext = &dynamicState2;

        } else if (timeline.timelineSemaphore) {
            timeline.pNext = &dynamicState2;
        }
        start = &dynamicState2;
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

    // load swapchain procs
    if (features & PAL_ADAPTER_FEATURE_SWAPCHAIN) {
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
    }

    // load semaphore procs
    if (features & PAL_ADAPTER_FEATURE_TIMELINE_SEMAPHORE) {
        device->waitSemaphore = (PFN_vkWaitSemaphores)s_Vk.getDeviceProcAddr(
            device->handle, 
            "vkWaitSemaphores");

        device->signalSemaphore = (PFN_vkSignalSemaphore)s_Vk.getDeviceProcAddr(
            device->handle, 
            "vkSignalSemaphore");

        device->getSemaphoreValue = (PFN_vkGetSemaphoreCounterValue)s_Vk.getDeviceProcAddr(
            device->handle, 
            "vkGetSemaphoreCounterValue");

        if (!device->waitSemaphore) {
            device->waitSemaphore = (PFN_vkWaitSemaphoresKHR)s_Vk.getDeviceProcAddr(
                device->handle, 
                "vkWaitSemaphoresKHR");

            device->signalSemaphore = (PFN_vkSignalSemaphoreKHR)s_Vk.getDeviceProcAddr(
                device->handle, 
                "vkSignalSemaphoreKHR");

            device->getSemaphoreValue = (PFN_vkGetSemaphoreCounterValueKHR)s_Vk.getDeviceProcAddr(
                device->handle, 
                "vkGetSemaphoreCounterValueKHR");
        }
    }

    // load fragment shading rate procs
    if (features & PAL_ADAPTER_FEATURE_FRAGMENT_SHADING_RATE) {
        device->cmdSetFragmentShadingRate = 
            (PFN_vkCmdSetFragmentShadingRateKHR)s_Vk.getDeviceProcAddr(
                device->handle, 
                "vkCmdSetFragmentShadingRateKHR");

        device->createRenderPass2 = (PFN_vkCreateRenderPass2)s_Vk.getDeviceProcAddr(
            device->handle, 
            "vkCreateRenderPass2");
    }

    // load fragment shading rate attachment procs
    if (features & PAL_ADAPTER_FEATURE_FRAGMENT_SHADING_RATE_ATTACHMENT) {
        device->createRenderPass2 = (PFN_vkCreateRenderPass2)s_Vk.getDeviceProcAddr(
            device->handle, 
            "vkCreateRenderPass2");

        if (!device->createRenderPass2) {
            device->createRenderPass2 = (PFN_vkCreateRenderPass2KHR)s_Vk.getDeviceProcAddr(
                device->handle, 
                "vkCreateRenderPass2KHR");
        }
    }

    // mesh shader
    if (features & PAL_ADAPTER_FEATURE_MESH_SHADER) {
        device->cmdDrawMeshTask = (PFN_vkCmdDrawMeshTasksEXT)s_Vk.getDeviceProcAddr(
            device->handle, 
            "vkCmdDrawMeshTasksEXT");

        device->cmdDrawMeshTaskIndirect = 
            (PFN_vkCmdDrawMeshTasksIndirectEXT)s_Vk.getDeviceProcAddr(
                device->handle, 
                "vkCmdDrawMeshTasksIndirectEXT");

        device->cmdDrawMeshTaskIndirectCount = 
            (PFN_vkCmdDrawMeshTasksIndirectCountEXT)s_Vk.getDeviceProcAddr(
                device->handle, 
                "vkCmdDrawMeshTasksIndirectCountEXT"); 
    }

    // ray tracing
    if (features & PAL_ADAPTER_FEATURE_RAY_TRACING) {
        device->createAccelerationStructure = 
            (PFN_vkCreateAccelerationStructureKHR)s_Vk.getDeviceProcAddr(
                device->handle, 
                "vkCreateAccelerationStructureKHR");

        device->destroyAccelerationStructure = 
            (PFN_vkDestroyAccelerationStructureKHR)s_Vk.getDeviceProcAddr(
                device->handle, 
                "vkDestroyAccelerationStructureKHR");

        device->getAccelerationBuildsize = 
            (PFN_vkGetAccelerationStructureBuildSizesKHR)s_Vk.getDeviceProcAddr(
                device->handle, 
                "vkGetAccelerationStructureBuildSizesKHR");

        device->cmdBuildAccelerationStructures = 
            (PFN_vkCmdBuildAccelerationStructuresKHR)s_Vk.getDeviceProcAddr(
                device->handle, 
                "vkCmdBuildAccelerationStructuresKHR");

        device->getAccelerationDeviceAddress = 
            (PFN_vkGetAccelerationStructureDeviceAddressKHR)s_Vk.getDeviceProcAddr(
                device->handle, 
                "vkGetAccelerationStructureDeviceAddressKHR");

        device->cmdTraceRays = 
            (PFN_vkCmdTraceRaysKHR)s_Vk.getDeviceProcAddr(
                device->handle, 
                "vkCmdTraceRaysKHR");

        device->createRayTracingPipeline = 
            (PFN_vkCreateRayTracingPipelinesKHR)s_Vk.getDeviceProcAddr(
                device->handle, 
                "vkCreateRayTracingPipelinesKHR");

        device->cmdTraceRaysIndirect = 
            (PFN_vkCmdTraceRaysIndirectKHR)s_Vk.getDeviceProcAddr(
                device->handle, 
                "vkCmdTraceRaysIndirectKHR");
    }

    // buffer address
    if (features & PAL_ADAPTER_FEATURE_BUFFER_DEVICE_ADDRESS) {
        device->getBufferrAddress = 
            (PFN_vkGetBufferDeviceAddress)s_Vk.getDeviceProcAddr(
                device->handle, 
                "vkGetBufferDeviceAddress");

        if (!device->getBufferrAddress) {
            device->getBufferrAddress = 
                (PFN_vkGetBufferDeviceAddressKHR)s_Vk.getDeviceProcAddr(
                    device->handle, 
                    "vkGetBufferDeviceAddressKHR");
        }
    }

    device->features = features;
    palFree(s_Vk.allocator, queueProps);
    palFree(s_Vk.allocator, queueCreateInfos);

    *outDevice = (PalDevice*)device;
    return PAL_RESULT_SUCCESS;
}

void PAL_CALL destroyVkDevice(PalDevice* device)
{
    Device* vkDevice = (Device*)device;
    s_Vk.destroyDevice(vkDevice->handle, &s_Vk.vkAllocator);
    palFree(s_Vk.allocator, vkDevice->phyQueues);
    palFree(s_Vk.allocator, vkDevice);
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
    Device* vkDevice = (Device*)device;
    VkMemoryAllocateInfo allocateInfo = {0};
    allocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocateInfo.allocationSize = (VkDeviceSize)size;

    if (type == PAL_MEMORY_TYPE_GPU_ONLY) {
        allocateInfo.memoryTypeIndex = vkDevice->gpuOnlyMemoryIndex;

    } else if (type == PAL_MEMORY_TYPE_CPU_UPLOAD) {
        allocateInfo.memoryTypeIndex = vkDevice->cpuUploadMemoryIndex;

    } else if (type == PAL_MEMORY_TYPE_CPU_READBACK) {
        allocateInfo.memoryTypeIndex = vkDevice->cpuReadbackMemoryIndex;
    }

    if (allocateInfo.memoryTypeIndex == -1) {
        // not supported
        return PAL_RESULT_MEMORY_TYPE_NOT_SUPPORTED;
    }

    VkDeviceMemory memory = nullptr;
    VkResult result = s_Vk.allocateMemory(
        vkDevice->handle, 
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
    Device* vkDevice = (Device*)device;
    VkDeviceMemory mem = (VkDeviceMemory)memory;
    s_Vk.freeMemory(vkDevice->handle, mem, &s_Vk.vkAllocator);
}

PalResult PAL_CALL queryVkDepthStencilCapabilities(
    PalDevice* device,
    PalDepthStencilCapabilities* caps)
{
    Device* vkDevice = (Device*)device;
    if (!(vkDevice->features & PAL_ADAPTER_FEATURE_DEPTH_STENCIL_RESOLVE)) {
        return PAL_RESULT_ADAPTER_FEATURE_NOT_SUPPORTED;
    }

    VkPhysicalDeviceProperties2 properties2 = {0};
    properties2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;

    VkPhysicalDeviceDepthStencilResolvePropertiesKHR props = {0};
    props.sType = 
        VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DEPTH_STENCIL_RESOLVE_PROPERTIES_KHR;

    properties2.pNext = &props;
    s_Vk.getPhysicalDeviceProperties2(vkDevice->phyDevice, &properties2);

    caps->independentDepthStencilResolve = props.independentResolve;

    if (props.supportedDepthResolveModes & VK_RESOLVE_MODE_AVERAGE_BIT_KHR) {
        caps->depthResolveModes[PAL_RESOLVE_MODE_AVERAGE] = true;
    }

    if (props.supportedDepthResolveModes & 
        VK_RESOLVE_MODE_SAMPLE_ZERO_BIT_KHR) {
        caps->depthResolveModes[PAL_RESOLVE_MODE_SAMPLE_ZERO] = true;
    }

    if (props.supportedDepthResolveModes & VK_RESOLVE_MODE_MIN_BIT_KHR) {
        caps->depthResolveModes[PAL_RESOLVE_MODE_MIN] = true;
    }

    if (props.supportedDepthResolveModes & VK_RESOLVE_MODE_MAX_BIT_KHR) {
        caps->depthResolveModes[PAL_RESOLVE_MODE_MAX] = true;
    }

    // stencil
    if (props.supportedStencilResolveModes & VK_RESOLVE_MODE_AVERAGE_BIT_KHR) {
        caps->stencilResolveModes[PAL_RESOLVE_MODE_AVERAGE] = true;
    }

    if (props.supportedStencilResolveModes & 
        VK_RESOLVE_MODE_SAMPLE_ZERO_BIT_KHR) {
        caps->stencilResolveModes[PAL_RESOLVE_MODE_SAMPLE_ZERO] = true;
    }

    if (props.supportedStencilResolveModes & VK_RESOLVE_MODE_MIN_BIT_KHR) {
        caps->stencilResolveModes[PAL_RESOLVE_MODE_MIN] = true;
    }

    if (props.supportedStencilResolveModes & VK_RESOLVE_MODE_MAX_BIT_KHR) {
        caps->stencilResolveModes[PAL_RESOLVE_MODE_MAX] = true;
    }

    return PAL_RESULT_SUCCESS;
}

PalResult PAL_CALL queryVkFragmentShadingRateCapabilities(
    PalDevice* device,
    PalFragmentShadingRateCapabilities* caps)
{
    Device* vkDevice = (Device*)device;
    if (!(vkDevice->features & PAL_ADAPTER_FEATURE_FRAGMENT_SHADING_RATE)) {
        return PAL_RESULT_ADAPTER_FEATURE_NOT_SUPPORTED;
    }

    VkPhysicalDeviceProperties2 properties2 = {0};
    properties2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;

    VkPhysicalDeviceFragmentShadingRatePropertiesKHR props = {0};
    props.sType = 
        VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FRAGMENT_SHADING_RATE_PROPERTIES_KHR;

    properties2.pNext = &props;
    s_Vk.getPhysicalDeviceProperties2(vkDevice->phyDevice, &properties2);

    memset(caps, 0, sizeof(PalFragmentShadingRateCapabilities));
    for (int i = 0; i < PAL_FRAGMENT_SHADING_RATE_MAX; i++) {
        VkExtent2D size = getShadingRateSize((PalFragmentShadingRate)i);

        // clang-format off
        // check against the max size
        if (size.width <= props.maxFragmentSize.width || 
            size.height <= props.maxFragmentSize.height) {
            caps->shadingRates[i] = true;
        }
        // clang-format on
    }

    caps->combinerOps[PAL_FRAGMENT_SHADING_RATE_COMBINER_OP_KEEP] = true;
    caps->combinerOps[PAL_FRAGMENT_SHADING_RATE_COMBINER_OP_REPLACE] = true;
    if (props.fragmentShadingRateNonTrivialCombinerOps) {
        caps->combinerOps[PAL_FRAGMENT_SHADING_RATE_COMBINER_OP_MIN] = true;
        caps->combinerOps[PAL_FRAGMENT_SHADING_RATE_COMBINER_OP_MAX] = true;
        caps->combinerOps[PAL_FRAGMENT_SHADING_RATE_COMBINER_OP_MUL] = true;
    }

    VkExtent2D size = props.minFragmentShadingRateAttachmentTexelSize;
    caps->minTexelWidth = size.width;
    caps->minTexelHeight = size.height;

    size = props.maxFragmentShadingRateAttachmentTexelSize;
    caps->maxTexelWidth = size.width;
    caps->maxTexelHeight = size.height;

    return PAL_RESULT_SUCCESS;
}

PalResult PAL_CALL queryVkMeshShaderCapabilities(
    PalDevice* device,
    PalMeshShaderCapabilities* caps)
{
    Device* vkDevice = (Device*)device;
    if (!(vkDevice->features & PAL_ADAPTER_FEATURE_MESH_SHADER)) {
        return PAL_RESULT_ADAPTER_FEATURE_NOT_SUPPORTED;
    }

    VkPhysicalDeviceProperties2 properties2 = {0};
    properties2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;

    VkPhysicalDeviceMeshShaderPropertiesEXT props = {0};
    props.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MESH_SHADER_PROPERTIES_EXT;
    properties2.pNext = &props;
    s_Vk.getPhysicalDeviceProperties2(vkDevice->phyDevice, &properties2);

    caps->maxMeshOutputPrimitives = props.maxMeshOutputPrimitives;
    caps->maxMeshOutputVertices = props.maxMeshOutputVertices;
    caps->maxTaskWorkGroupInvocations = props.maxTaskWorkGroupInvocations;
    caps->maxMeshWorkGroupInvocations = props.maxMeshWorkGroupInvocations;

    caps->maxTaskWorkGroupCount[0] = props.maxTaskWorkGroupCount[0];
    caps->maxTaskWorkGroupCount[1] = props.maxTaskWorkGroupCount[1];
    caps->maxTaskWorkGroupCount[2] = props.maxTaskWorkGroupCount[2];

    caps->maxMeshWorkGroupCount[0] = props.maxMeshWorkGroupCount[0];
    caps->maxMeshWorkGroupCount[1] = props.maxMeshWorkGroupCount[1];
    caps->maxMeshWorkGroupCount[2] = props.maxMeshWorkGroupCount[2];

    return PAL_RESULT_SUCCESS;
}

PalResult PAL_CALL queryVkRayTracingCapabilities(
    PalDevice* device,
    PalRayTracingCapabilities* caps)
{
    Device* vkDevice = (Device*)device;
    if (!(vkDevice->features & PAL_ADAPTER_FEATURE_RAY_TRACING)) {
        return PAL_RESULT_ADAPTER_FEATURE_NOT_SUPPORTED;
    }

    VkPhysicalDeviceProperties2 properties2 = {0};
    properties2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;

    VkPhysicalDeviceRayTracingPipelinePropertiesKHR props = {0};
    props.sType = 
       VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_TRACING_PIPELINE_PROPERTIES_KHR;

    VkPhysicalDeviceAccelerationStructurePropertiesKHR accProps = {0};
    accProps.sType = 
       VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ACCELERATION_STRUCTURE_PROPERTIES_KHR;

    props.pNext = &accProps;
    properties2.pNext = &props;
    s_Vk.getPhysicalDeviceProperties2(vkDevice->phyDevice, &properties2);

    caps->maxRecursionDepth = props.maxRayRecursionDepth;
    caps->maxHitAttributeSize = props.maxRayHitAttributeSize;
    caps->maxInstanceCount = accProps.maxInstanceCount;
    caps->maxPrimitiveCount = accProps.maxPrimitiveCount;
    caps->maxGeometryCount = accProps.maxGeometryCount;

    caps->maxPayloadSize = INT32_MAX; // depends on memory
    caps->maxDispatchInvocations = props.maxRayDispatchInvocationCount;
    caps->maxShaderGroupStride = props.maxShaderGroupStride;
    caps->shaderGroupHandleSize = props.shaderGroupHandleSize;
    caps->shaderGroupHandleAlignment = props.shaderGroupHandleAlignment;
    caps->shaderGroupBaseAlignment = props.shaderGroupBaseAlignment;

    return PAL_RESULT_SUCCESS;
}

// ==================================================
// Queue
// ==================================================

PalResult PAL_CALL createVkQueue(
    PalDevice* device,
    PalQueueType type,
    PalQueue** outQueue)
{
    Device* vkDevice = (Device*)device;
    VkQueueFlags queueFlag = 0;
    Queue* queue = nullptr;

    if (!vkDevice->handle) {
        return PAL_RESULT_INVALID_DEVICE;
    }

    if (vkDevice->queueCount == 0) {
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
    for (int i = 0; i < vkDevice->queueCount; i++) {
        PhysicalQueue* pq = &vkDevice->phyQueues[i];
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

    queue = palAllocate(s_Vk.allocator, sizeof(Queue), 0);
    if (!queue) {
        return PAL_RESULT_OUT_OF_MEMORY;
    }

    queue->phyQueue = phyQueue;
    queue->usage = queueFlag;
    queue->device = device;

    *outQueue = (PalQueue*)queue;
    return PAL_RESULT_SUCCESS;
}

void PAL_CALL destroyVkQueue(PalQueue* queue)
{
    Queue* vkQueue = (Queue*)queue;
    PhysicalQueue* phyQueue = vkQueue->phyQueue;
    phyQueue->usedUsages &= ~vkQueue->usage;
    palFree(s_Vk.allocator, vkQueue);
}

bool PAL_CALL canVkQueuePresent(
    PalQueue* queue, 
    PalGraphicsWindow* window)
{
    Queue* vkQueue = (Queue*)queue;
    // check if the queue is a graphics queue before we check its family 
    // index for presentation support.
    if (vkQueue->usage != VK_QUEUE_GRAPHICS_BIT) {
        return false;
    }

    Uint32 platform = checkPlatform(window->display);
    PhysicalQueue* phyQueue = vkQueue->phyQueue;
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
    Adapter* vkAdapter = (Adapter*)adapter;
    VkPhysicalDevice phyDevice = (VkPhysicalDevice)vkAdapter->handle;
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

                    // fragment shading rate
                    if (i == PAL_FORMAT_R8_UINT) {
                        usages |= PAL_IMAGE_VIEW_USAGE_FRAGMENT_SHADING_RATE;
                        usages |= PAL_IMAGE_VIEW_USAGE_COLOR;
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
    Adapter* vkAdapter = (Adapter*)adapter;
    VkPhysicalDevice phyDevice = (VkPhysicalDevice)vkAdapter->handle;
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
    Adapter* vkAdapter = (Adapter*)adapter;
    VkPhysicalDevice phyDevice = (VkPhysicalDevice)vkAdapter->handle;
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
    Adapter* vkAdapter = (Adapter*)adapter;
    VkPhysicalDevice phyDevice = (VkPhysicalDevice)vkAdapter->handle;
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

    // fragment shading rate
    if (format == PAL_FORMAT_R8_UINT) {
        usages |= PAL_IMAGE_VIEW_USAGE_FRAGMENT_SHADING_RATE;
        usages |= PAL_IMAGE_VIEW_USAGE_COLOR;
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
    Image* image = nullptr;
    Device* vkDevice = (Device*)device;

    if (!vkDevice->handle) {
        return PAL_RESULT_INVALID_DEVICE;
    }

    image = palAllocate(s_Vk.allocator, sizeof(Image), 0);
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
    createInfo.usage = palImageUsageToVk(info->usages);

    createInfo.arrayLayers = info->depthOrArraySize;
    createInfo.extent.depth = 1;
    createInfo.imageType = palImageTypeToVk(info->type);

    if (info->type == PAL_IMAGE_TYPE_3D) {
        createInfo.arrayLayers = 1;
        createInfo.extent.depth = info->depthOrArraySize;
    }

    result = s_Vk.createImage(
        vkDevice->handle, 
        &createInfo, 
        &s_Vk.vkAllocator, 
        &image->handle);

    if (result != VK_SUCCESS) {
        palFree(s_Vk.allocator, image);
        return vkResultToPal(result);
    }

    image->belongsToSwapchain = false;
    image->device = vkDevice;
    image->info.depthOrArraySize = info->depthOrArraySize;
    image->info.type = info->type;
    image->info.format = info->format;
    image->info.usages = info->usages;
    image->info.height = info->height;
    image->info.mipLevelCount = info->mipLevelCount;
    image->info.sampleCount = info->sampleCount;
    image->info.width = info->width;

    *outImage = (PalImage*)image;
    return PAL_RESULT_SUCCESS;
}

void PAL_CALL destroyVkImage(PalImage* image)
{
    Image* vkImage = (Image*)image;
    if (vkImage->belongsToSwapchain) {
        return;
    }

    s_Vk.destroyImage(
        vkImage->device->handle, 
        vkImage->handle, 
        &s_Vk.vkAllocator);

    palFree(s_Vk.allocator, vkImage);
}

PalResult PAL_CALL getVkImageInfo(
    PalImage* image,
    PalImageInfo* info)
{
    Image* vkImage = (Image*)image;
    *info = vkImage->info;
    return PAL_RESULT_SUCCESS;
}

PalResult PAL_CALL getVkImageMemoryRequirements(
    PalImage* image,
    PalMemoryRequirements* requirements)
{
    Image* vkImage = (Image*)image;
    if (vkImage->belongsToSwapchain) {
        return PAL_RESULT_INVALID_OPERATION;
    }

    Device* device = vkImage->device;
    VkPhysicalDevice phyDevice = (VkPhysicalDevice)device->phyDevice;
    VkPhysicalDeviceMemoryProperties memProps = {0};
    s_Vk.getPhysicalDeviceMemoryProperties(phyDevice, &memProps);

    VkMemoryRequirements memReq = {0};
    s_Vk.getImageMemoryRequirements(device->handle, vkImage->handle, &memReq);
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
    Image* vkImage = (Image*)image;
    if (vkImage->belongsToSwapchain) {
        return PAL_RESULT_INVALID_OPERATION;
    }

    VkDeviceMemory mem = (VkDeviceMemory)memory;
    s_Vk.bindImageMemory(
        vkImage->device->handle, 
        vkImage->handle,
        mem, 
        offset);
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
    VkResult result = VK_SUCCESS;
    ImageView* imageView = nullptr;
    Device* vkDevice = (Device*)device;
    Image* vkImage = (Image*)image;

    if (info->type == PAL_IMAGE_VIEW_TYPE_CUBE_ARRAY) {
        if (!(vkDevice->features & 
            PAL_ADAPTER_FEATURE_IMAGE_VIEW_CUBE_ARRAY)) {
            return PAL_RESULT_ADAPTER_FEATURE_NOT_SUPPORTED;
        }
    }

    imageView = palAllocate(s_Vk.allocator, sizeof(ImageView), 0);
    if (!imageView) {
        return PAL_RESULT_OUT_OF_MEMORY;
    }

    VkImageViewCreateInfo createInfo = {0};
    createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    createInfo.format = palFormatToVk(vkImage->info.format);
    createInfo.image = vkImage->handle;

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

    createInfo.subresourceRange.aspectMask = aspectFlags;
    result = s_Vk.createImageView(
        vkDevice->handle, 
        &createInfo, 
        &s_Vk.vkAllocator, 
        &imageView->handle);

    if (result != VK_SUCCESS) {
        palFree(s_Vk.allocator, imageView);
        return vkResultToPal(result);
    }

    imageView->device = vkDevice;
    imageView->image = vkImage;
    imageView->type = createInfo.viewType;
    imageView->usages = info->usages;

    *outImageView = (PalImageView*)imageView;
    return PAL_RESULT_SUCCESS;
}

void PAL_CALL destroyVkImageView(PalImageView* imageView)
{
    ImageView* vkImageView = (ImageView*)imageView;
    s_Vk.destroyImageView(
        vkImageView->device->handle, 
        vkImageView->handle, 
        &s_Vk.vkAllocator);

    palFree(s_Vk.allocator, vkImageView);
}

// ==================================================
// Swapchain
// ==================================================

PalResult PAL_CALL queryVkSwapchainCapabilities(
    PalDevice* device,
    PalGraphicsWindow* window,
    PalSwapchainCapabilities* caps)
{
    Int32 formatCount = 0;
    Int32 modeCount = 0;
    VkSurfaceKHR surface = nullptr;
    VkSurfaceFormatKHR* formats = nullptr;
    VkPresentModeKHR* modes = nullptr;

    Device* vkDevice = (Device*)device;
    VkPhysicalDevice phyDevice = (VkPhysicalDevice)vkDevice->phyDevice;

    if (!(vkDevice->features & PAL_ADAPTER_FEATURE_SWAPCHAIN)) {
        return PAL_RESULT_ADAPTER_FEATURE_NOT_SUPPORTED;
    }

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
    Swapchain* swapchain = nullptr;
    VkImage* images = nullptr;

    Device* vkDevice = (Device*)device;
    Queue* vkQueue = (Queue*)queue;
    PhysicalQueue* phyQueue = vkQueue->phyQueue;

    if (!(vkDevice->features & PAL_ADAPTER_FEATURE_SWAPCHAIN)) {
        return PAL_RESULT_ADAPTER_FEATURE_NOT_SUPPORTED;
    }

    // check if the queue is a graphics queue before we check its family 
    // index for presentation support.
    if (vkQueue->usage != VK_QUEUE_GRAPHICS_BIT) {
        PAL_RESULT_INVALID_QUEUE;
    }

    swapchain = palAllocate(s_Vk.allocator, sizeof(Swapchain), 0);
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
    VkResult result = vkDevice->createSwapchain(
        vkDevice->handle, 
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
    result = vkDevice->getSwapchainImages(
        vkDevice->handle,
        swapchain->handle, 
        &count,
        nullptr);

    swapchain->images = palAllocate(
        s_Vk.allocator, 
        sizeof(Image) * count, 
        0);

    images = palAllocate(
        s_Vk.allocator, 
        sizeof(VkImage) * count, 
        0);

    if (!swapchain->images || !images) {
        vkDevice->destroySwapchain(
            vkDevice->handle,
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
    
    vkDevice->getSwapchainImages(
        vkDevice->handle, 
        swapchain->handle, 
        &count,
        images);

    // fill all images with the creatio info
    for (int i = 0; i < count; i++) {
        Image* image = &swapchain->images[i];
        image->belongsToSwapchain = true;
        image->device = vkDevice;
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
    
    swapchain->device = vkDevice;
    swapchain->queue = vkQueue;
    swapchain->imageCount = count;

    *outSwapchain = (PalSwapchain*)swapchain;
    return PAL_RESULT_SUCCESS;
}

void PAL_CALL destroyVkSwapchain(PalSwapchain* swapchain)
{
    Swapchain* vkSwapchain = (Swapchain*)swapchain;
    vkSwapchain->device->destroySwapchain(
        vkSwapchain->device->handle,
        vkSwapchain->handle,
        &s_Vk.vkAllocator
    );

    s_Vk.destroySurface(
        s_Vk.instance, 
        vkSwapchain->surface, 
        &s_Vk.vkAllocator);

    palFree(s_Vk.allocator, vkSwapchain->images);
    palFree(s_Vk.allocator, vkSwapchain);
}

PalImage* PAL_CALL getVkSwapchainImage(
    PalSwapchain* swapchain,
    Int32 index)
{
    Swapchain* vkSwapchain = (Swapchain*)swapchain;
    if (index > vkSwapchain->imageCount) {
        return nullptr;
    }
    return (PalImage*)&vkSwapchain->images[index];
}

PalImage* PAL_CALL getVkNextSwapchainImage(
    PalSwapchain* swapchain,
    PalNextImageInfo* info)
{
    VkResult result;
    Uint32 index = 0;
    VkFence fenceHandle = nullptr;
    VkSemaphore semaphoreHandle = nullptr;
    Swapchain* vkSwapchain = (Swapchain*)swapchain;

    if (info->fence) {
        Fence* vkFence = (Fence*)info->fence;
        fenceHandle = vkFence->handle;
    }

    if (info->signalSemaphore) {
        Semaphore* vkSemaphore = (Semaphore*)info->signalSemaphore;
        semaphoreHandle = vkSemaphore->handle;
    }

    result = vkSwapchain->device->acquireNextImage(
        vkSwapchain->device->handle,
        vkSwapchain->handle,
        info->timeout,
        semaphoreHandle,
        fenceHandle,
        &index);

    if (result != VK_SUCCESS) {
        return nullptr;
    }

    Image* image = &vkSwapchain->images[index];
    image->index = index;
    return (PalImage*)image;
}

PalResult PAL_CALL presentVkSwapchain(
    PalSwapchain* swapchain, 
    PalPresentInfo* info)
{
    Swapchain* vkSwapchain = (Swapchain*)swapchain;
    Image* vkImage = (Image*)info->image;
    if (vkImage->index == -1) {
        // image was not the next image
        // this prevents UB
        return PAL_RESULT_INVALID_IMAGE;
    }

    Int32 semaphoreCount = 0;
    VkSemaphore semaphoreHandle = nullptr;
    if (info->waitSemaphore) {
        Semaphore* vkSemaphore = (Semaphore*)info->waitSemaphore;
        semaphoreHandle = vkSemaphore->handle;
        semaphoreCount = 1;
    }

    VkResult result;
    VkPresentInfoKHR presentInfo = {0};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = &vkSwapchain->handle;
    presentInfo.pImageIndices = &vkImage->index;
    presentInfo.pWaitSemaphores = &semaphoreHandle;
    presentInfo.waitSemaphoreCount = semaphoreCount;

    result = vkSwapchain->device->queuePresent(
        vkSwapchain->queue->phyQueue->handle, 
        &presentInfo);

    if (result != VK_SUCCESS) {
        return vkResultToPal(result);
    }

    vkImage->index = -1;
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
    Shader* shader = nullptr;
    VkShaderStageFlags stage = 0;
    Device* vkDevice = (Device*)device;

    if (info->type == PAL_SHADER_TYPE_VERTEX) {
        stage = VK_SHADER_STAGE_VERTEX_BIT;

    } else if (info->type == PAL_SHADER_TYPE_FRAGMENT) {
        stage = VK_SHADER_STAGE_FRAGMENT_BIT;

    } else if (info->type == PAL_SHADER_TYPE_COMPUTE) {
        if (!(vkDevice->features & PAL_ADAPTER_FEATURE_COMPUTE_SHADER)) {
            return PAL_RESULT_ADAPTER_FEATURE_NOT_SUPPORTED;
        }
        stage = VK_SHADER_STAGE_COMPUTE_BIT;

    } else if (info->type == PAL_SHADER_TYPE_TESSELLATION_CONTROL) {
        if (!(vkDevice->features & PAL_ADAPTER_FEATURE_TESSELLATION_SHADER)) {
            return PAL_RESULT_ADAPTER_FEATURE_NOT_SUPPORTED;
        }
        stage = VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT;

    } else if (info->type == PAL_SHADER_TYPE_TESSELLATION_EVALUATION) {
        if (!(vkDevice->features & PAL_ADAPTER_FEATURE_TESSELLATION_SHADER)) {
            return PAL_RESULT_ADAPTER_FEATURE_NOT_SUPPORTED;
        }
        stage = VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT;

    } else if (info->type == PAL_SHADER_TYPE_MESH) {
        if (!(vkDevice->features & PAL_ADAPTER_FEATURE_MESH_SHADER)) {
            return PAL_RESULT_ADAPTER_FEATURE_NOT_SUPPORTED;
        }
        stage = VK_SHADER_STAGE_MESH_BIT_EXT;

    } else if (info->type == PAL_SHADER_TYPE_TASK) {
        if (!(vkDevice->features & PAL_ADAPTER_FEATURE_MESH_SHADER)) {
            return PAL_RESULT_ADAPTER_FEATURE_NOT_SUPPORTED;
        }
        stage = VK_SHADER_STAGE_TASK_BIT_EXT;
    }

    shader = palAllocate(s_Vk.allocator, sizeof(Shader), 0);
    if (!shader) {
        return PAL_RESULT_OUT_OF_MEMORY;
    }

    VkShaderModuleCreateInfo createInfo = {0};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.codeSize = info->bytecodeSize;
    createInfo.pCode = (const Uint32*)info->bytecode;

    result = s_Vk.createShader(
        vkDevice->handle,
        &createInfo, 
        &s_Vk.vkAllocator, 
        &shader->handle);
    
    if (result != VK_SUCCESS) {
        palFree(s_Vk.allocator, shader);
        return vkResultToPal(result);
    }

    shader->device = vkDevice;
    shader->info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    shader->info.module = shader->handle;
    shader->info.pName = "main";
    shader->info.stage = stage;

    *outShader = (PalShader*)shader;
    return PAL_RESULT_SUCCESS;
}

void PAL_CALL destroyVkShader(PalShader* shader)
{
    Shader* vkShader = (Shader*)shader;
    s_Vk.destroyShader(
        vkShader->device->handle, 
        vkShader->handle, 
        &s_Vk.vkAllocator);
    
    palFree(s_Vk.allocator, vkShader);
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
    RenderPass* renderPass = nullptr;
    Device* vkDevice = (Device*)device;

    renderPass = palAllocate(s_Vk.allocator, sizeof(RenderPass), 0);
    if (!renderPass) {
        return PAL_RESULT_OUT_OF_MEMORY;
    }

    bool hasDepth = false;
    bool hasFsr;
    Uint32 colorRefCount = 0;
    Uint32 resolveRefCount = 0;
    Uint32 layers = 0;
    VkImageView views[MAX_ATTACHMENTS];

    // multi view
    Uint32 viewMask = (1 << info->multiViewCount) - 1;
    VkRenderPassMultiviewCreateInfo viewCreateInfo = {0};
    viewCreateInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_MULTIVIEW_CREATE_INFO;
    viewCreateInfo.pViewMasks = &viewMask;
    viewCreateInfo.subpassCount = 1;

    // clang-format off
    if (vkDevice->createRenderPass2) {
        VkAttachmentDescription2KHR attachments[MAX_ATTACHMENTS];
        VkAttachmentReference2KHR depthRef;
        VkAttachmentReference2KHR fsrRef;
        VkAttachmentReference2KHR colorRefs[MAX_ATTACHMENTS];
        VkAttachmentReference2KHR resolveRefs[MAX_ATTACHMENTS];
        VkFragmentShadingRateAttachmentInfoKHR fsrAttachment = {0};
        fsrAttachment.sType = VK_STRUCTURE_TYPE_FRAGMENT_SHADING_RATE_ATTACHMENT_INFO_KHR;

        for (int i = 0; i < info->attachmentCount; i++) {
            PalAttachmentDesc* desc = &info->attachments[i];
            VkAttachmentDescription2KHR* rDesc = &attachments[i];

            rDesc->sType = VK_STRUCTURE_TYPE_ATTACHMENT_DESCRIPTION_2_KHR;
            rDesc->flags = 0;
            rDesc->pNext = nullptr;
            ImageView* vkImageView = (ImageView*)desc->target;

            rDesc->format = palFormatToVk(vkImageView->image->info.format);
            rDesc->initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
            rDesc->samples = vkSamplesToSamples(vkImageView->image->info.sampleCount);
            rDesc->flags = 0;

            layers = vkImageView->image->info.depthOrArraySize;
            VkAttachmentReference2KHR* resolveRef = &resolveRefs[resolveRefCount];
            if (desc->resolveTarget) {
                // the layer count must be the same so to validate as well
                // we use the highest from the attachments
                ImageView* tmp = (ImageView*)desc->resolveTarget;
                layers = tmp->image->info.depthOrArraySize;
                resolveRef->attachment = i;
                resolveRef->pNext = 0;
                resolveRef->sType = VK_STRUCTURE_TYPE_ATTACHMENT_DESCRIPTION_2;
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
            views[i] = vkImageView->handle;

            if (desc->type == PAL_ATTACHMENT_TYPE_COLOR) {
                VkAttachmentReference2KHR* ref = &colorRefs[colorRefCount];
                ref->attachment = i;
                ref->layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
                ref->aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
                ref->pNext = 0;
                ref->sType = VK_STRUCTURE_TYPE_ATTACHMENT_DESCRIPTION_2;

                rDesc->finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
                colorRefCount++;
                if (vkImageView->image->belongsToSwapchain) {
                    rDesc->finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
                }

                if (resolveRef) {
                    resolveRef->layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
                    resolveRef->aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
                    resolveRefCount++;
                }

            } else {
                // fragment shading rate attachment
                if (desc->type == PAL_ATTACHMENT_TYPE_FRAGMENT_SHADING_RATE) {
                    VkExtent2D size;
                    size.width = desc->texelWidth;
                    size.height = desc->texelHeight;
                    fsrAttachment.shadingRateAttachmentTexelSize = size;

                    fsrRef.attachment = i;
                    rDesc->finalLayout = VK_IMAGE_LAYOUT_FRAGMENT_SHADING_RATE_ATTACHMENT_OPTIMAL_KHR;
                    fsrRef.layout = VK_IMAGE_LAYOUT_FRAGMENT_SHADING_RATE_ATTACHMENT_OPTIMAL_KHR;
                    fsrAttachment.pFragmentShadingRateAttachment = &fsrRef;
                    hasFsr = true;

                } else {
                    depthRef.attachment = i;
                    depthRef.layout = VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL;
                    depthRef.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
                    depthRef.pNext = 0;
                    depthRef.sType = VK_STRUCTURE_TYPE_ATTACHMENT_DESCRIPTION_2;
                    
                    rDesc->finalLayout = VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL;
                    hasDepth = true;
                    // stencil is only used with depth attachment
                    if (desc->stencilLoadOp == PAL_LOAD_OP_CLEAR) {
                        rDesc->stencilLoadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;

                    } else if (desc->stencilLoadOp == PAL_LOAD_OP_LOAD) {
                        rDesc->stencilLoadOp = VK_ATTACHMENT_LOAD_OP_LOAD;

                    } else if (desc->stencilLoadOp == PAL_LOAD_OP_DONT_CARE) {
                        rDesc->stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
                    }

                    if (desc->stencilStoreOp == PAL_STORE_OP_STORE) {
                        rDesc->stencilStoreOp = VK_ATTACHMENT_STORE_OP_STORE;

                    } else if (desc->stencilStoreOp == PAL_STORE_OP_DONT_CARE) {
                        rDesc->stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
                    }

                    if (resolveRef) {
                        resolveRef->layout = VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL;
                        resolveRef->aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
                        resolveRefCount++;
                    }
                }
            }
        }

        // subpass
        VkSubpassDescription2 subpassDesc = {0};
        subpassDesc.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
        subpassDesc.colorAttachmentCount = colorRefCount;
        subpassDesc.pColorAttachments = colorRefs;
        subpassDesc.pNext = &fsrAttachment;

        if (hasDepth) {
            subpassDesc.pDepthStencilAttachment = &depthRef;
        }

        if (resolveRefCount) {
            subpassDesc.pResolveAttachments = resolveRefs;
        }

        VkRenderPassCreateInfo2KHR createInfo = {0};
        createInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO_2_KHR;
        createInfo.attachmentCount = info->attachmentCount;
        createInfo.pAttachments = attachments;
        createInfo.subpassCount = 1;
        createInfo.pSubpasses = &subpassDesc;

        if (info->multiViewCount > 1) {
            createInfo.pNext = &viewCreateInfo;
        }

        result = vkDevice->createRenderPass2(
            vkDevice->handle,
            &createInfo, 
            &s_Vk.vkAllocator, 
            &renderPass->handle);

    } else {
        // legacy path
        VkAttachmentDescription attachments[MAX_ATTACHMENTS];
        VkAttachmentReference depthRef;
        VkAttachmentReference colorRefs[MAX_ATTACHMENTS];
        VkAttachmentReference resolveRefs[MAX_ATTACHMENTS];

        for (int i = 0; i < info->attachmentCount; i++) {
            PalAttachmentDesc* desc = &info->attachments[i];
            VkAttachmentDescription* rDesc = &attachments[i];

            ImageView* vkImageView = (ImageView*)desc->target;
            rDesc->format = palFormatToVk(vkImageView->image->info.format);
            rDesc->initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
            rDesc->samples = vkSamplesToSamples(vkImageView->image->info.sampleCount);
            rDesc->flags = 0;

            layers = vkImageView->image->info.depthOrArraySize;
            VkAttachmentReference* resolveRef = &resolveRefs[resolveRefCount];
            if (desc->resolveTarget) {
                // the layer count must be the same so to validate as well
                // we use the highest from the attachments
                ImageView* tmp = (ImageView*)desc->resolveTarget;
                layers = tmp->image->info.depthOrArraySize;
                resolveRef->attachment = i;
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
            views[i] = vkImageView->handle;

            if (desc->type == PAL_ATTACHMENT_TYPE_COLOR) {
                VkAttachmentReference* ref = &colorRefs[colorRefCount];
                ref->attachment = i;
                ref->layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

                rDesc->finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
                colorRefCount++;
                if (vkImageView->image->belongsToSwapchain) {
                    rDesc->finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
                }

                if (resolveRef) {
                    resolveRef->layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
                    resolveRefCount++;
                }

            } else {
                depthRef.attachment = i;
                depthRef.layout = VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL;
                
                rDesc->finalLayout = VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL;
                hasDepth = true;
                // stencil is only used with depth attachment
                if (desc->stencilLoadOp == PAL_LOAD_OP_CLEAR) {
                    rDesc->stencilLoadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;

                } else if (desc->stencilLoadOp == PAL_LOAD_OP_LOAD) {
                    rDesc->stencilLoadOp = VK_ATTACHMENT_LOAD_OP_LOAD;

                } else if (desc->stencilLoadOp == PAL_LOAD_OP_DONT_CARE) {
                    rDesc->stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
                }

                if (desc->stencilStoreOp == PAL_STORE_OP_STORE) {
                    rDesc->stencilStoreOp = VK_ATTACHMENT_STORE_OP_STORE;

                } else if (desc->stencilStoreOp == PAL_STORE_OP_DONT_CARE) {
                    rDesc->stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
                }

                if (resolveRef) {
                    resolveRef->layout = VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL;
                    resolveRefCount++;
                }
            }
        }

        // subpass legacy
        VkSubpassDescription subpassDesc = {0};
        subpassDesc.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
        subpassDesc.colorAttachmentCount = colorRefCount;
        subpassDesc.pColorAttachments = colorRefs;
        if (hasDepth) {
            subpassDesc.pDepthStencilAttachment = &depthRef;
        }

        if (resolveRefCount) {
            subpassDesc.pResolveAttachments = resolveRefs;
        }

        VkRenderPassCreateInfo createInfo = {0};
        createInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        createInfo.attachmentCount = info->attachmentCount;
        createInfo.pAttachments = attachments;
        createInfo.subpassCount = 1;
        createInfo.pSubpasses = &subpassDesc;

        if (info->multiViewCount > 1) {
            createInfo.pNext = &viewCreateInfo;
        }

        result = s_Vk.createRenderPass(
            vkDevice->handle, 
            &createInfo, 
            &s_Vk.vkAllocator, 
            &renderPass->handle);
    }

    if (result != VK_SUCCESS) {
        palFree(s_Vk.allocator, renderPass);
        return vkResultToPal(result);
    }

    // clamg-format on
    // create framebuffer
    VkFramebufferCreateInfo fbCreateInfo = {0};
    fbCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    fbCreateInfo.attachmentCount = info->attachmentCount;
    fbCreateInfo.pAttachments = views;
    fbCreateInfo.height = info->height;
    fbCreateInfo.width = info->width;
    fbCreateInfo.layers = layers;
    fbCreateInfo.renderPass = renderPass->handle;

    result = s_Vk.createFramebuffer(
        vkDevice->handle, 
        &fbCreateInfo, 
        &s_Vk.vkAllocator, 
        &renderPass->framebuffer);

    if (result != VK_SUCCESS) {
        s_Vk.destroyRenderPass(
            vkDevice->handle, 
            renderPass->handle, 
            &s_Vk.vkAllocator);

        palFree(s_Vk.allocator, renderPass);
        // all image views in a single render pass must have the same
        // width/height/layers
        return PAL_RESULT_INVALID_ARGUMENT;
    }

    renderPass->device = vkDevice; 
    renderPass->width = info->width;
    renderPass->height = info->height;
    renderPass->attachmentCount = info->attachmentCount;

    *outRenderPass = (PalRenderPass*)renderPass;
    return PAL_RESULT_SUCCESS;
}

void PAL_CALL destroyVkRenderPass(PalRenderPass* renderPass)
{
    RenderPass* vkRenderPass = (RenderPass*)renderPass;
    s_Vk.destroyFramebuffer(
        vkRenderPass->device->handle,
        vkRenderPass->framebuffer, 
        &s_Vk.vkAllocator);

    s_Vk.destroyRenderPass(
        vkRenderPass->device->handle, 
        vkRenderPass->handle, 
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
    Fence* fence = nullptr;
    Device* vkDevice = (Device*)device;

    fence = palAllocate(s_Vk.allocator, sizeof(Fence), 0);
    if (!fence) {
        return PAL_RESULT_OUT_OF_MEMORY;
    }

    VkFenceCreateInfo createInfo = {0};
    createInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    result = s_Vk.createFence(
        vkDevice->handle, 
        &createInfo, 
        &s_Vk.vkAllocator, 
        &fence->handle);

    if (result != VK_SUCCESS) {
        palFree(s_Vk.allocator, fence);
        return vkResultToPal(result);
    }

    fence->device = vkDevice;
    *outFence = (PalFence*)fence;
    return PAL_RESULT_SUCCESS;
}

void PAL_CALL destroyVkFence(PalFence* fence)
{
    Fence* vkFence = (Fence*)fence;
    s_Vk.destroyFence(
        vkFence->device->handle, 
        vkFence->handle, 
        &s_Vk.vkAllocator);

    palFree(s_Vk.allocator, vkFence);
}

PalResult PAL_CALL waitVkFence(
    PalFence* fence, 
    Uint64 timeout)
{
    Fence* vkFence = (Fence*)fence;
    if (timeout != UINT64_MAX) {
        if (!(vkFence->device->features & PAL_ADAPTER_FEATURE_FENCE_RESET)) {
            return PAL_RESULT_ADAPTER_FEATURE_NOT_SUPPORTED;
        }
    }

    VkResult result = s_Vk.waitFence(
        vkFence->device->handle, 
        1, 
        &vkFence->handle, 
        true,
        timeout);

    if (result != VK_SUCCESS) {
        return vkResultToPal(result);
    }

    return PAL_RESULT_SUCCESS;
}

PalResult PAL_CALL resetVkFence(PalFence* fence)
{
    Fence* vkFence = (Fence*)fence;
    if (!(vkFence->device->features & PAL_ADAPTER_FEATURE_FENCE_RESET)) {
        return PAL_RESULT_ADAPTER_FEATURE_NOT_SUPPORTED;
    }

    VkResult ret = s_Vk.resetFence(
        vkFence->device->handle, 
        1, 
        &vkFence->handle);

    if (ret != VK_SUCCESS) {
        return vkResultToPal(ret);
    }

    return PAL_RESULT_SUCCESS;
}

bool PAL_CALL isVkFenceSignaled(PalFence* fence)
{
    Fence* vkFence = (Fence*)fence;
    VkResult ret = s_Vk.isFenceSignaled(
        vkFence->device->handle, 
        vkFence->handle);

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
    Semaphore* semaphore = nullptr;
    Device* vkDevice = (Device*)device;

    semaphore = palAllocate(s_Vk.allocator, sizeof(Semaphore), 0);
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
    if (vkDevice->features & PAL_ADAPTER_FEATURE_TIMELINE_SEMAPHORE) {
        next = &timelineCreateInfo;
        semaphore->isTimeline = true;
    }

    createInfo.pNext = next;
    result = s_Vk.createSemaphore(
        vkDevice->handle,
        &createInfo, 
        &s_Vk.vkAllocator, 
        &semaphore->handle);

    if (result != VK_SUCCESS) {
        palFree(s_Vk.allocator, semaphore);
        return vkResultToPal(result);
    }

    semaphore->device = vkDevice;
    *outSemaphore = (PalSemaphore*)semaphore;
    return PAL_RESULT_SUCCESS;
}

void PAL_CALL destroyVkSemaphore(PalSemaphore* semaphore)
{
    Semaphore* vkSemaphore = (Semaphore*)semaphore;
    s_Vk.destroySemaphore(
        vkSemaphore->device->handle, 
        vkSemaphore->handle, 
        &s_Vk.vkAllocator);

    palFree(s_Vk.allocator, vkSemaphore);
}

PalResult PAL_CALL waitVkSemaphore(
    PalSemaphore* semaphore, 
    PalQueue* queue,
    Uint64 value,
    Uint64 timeout)
{
    VkResult result;
    Semaphore* vkSemaphore = (Semaphore*)semaphore;
    if (!(vkSemaphore->device->features & 
        PAL_ADAPTER_FEATURE_TIMELINE_SEMAPHORE)) {
        return PAL_RESULT_ADAPTER_FEATURE_NOT_SUPPORTED;
    }

    VkSemaphoreWaitInfo waitInfo = {0};
    waitInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_WAIT_INFO;
    waitInfo.semaphoreCount = 1;
    waitInfo.pSemaphores = &vkSemaphore->handle;
    waitInfo.pValues = &value;

    result = vkSemaphore->device->waitSemaphore(
        vkSemaphore->device->handle, 
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
    Semaphore* vkSemaphore = (Semaphore*)semaphore;
    if (!(vkSemaphore->device->features & 
        PAL_ADAPTER_FEATURE_TIMELINE_SEMAPHORE)) {
        return PAL_RESULT_ADAPTER_FEATURE_NOT_SUPPORTED;
    }

    VkSemaphoreSignalInfo signalInfo = {0};
    signalInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_SIGNAL_INFO;
    signalInfo.semaphore = vkSemaphore->handle;
    signalInfo.value = value;

    result = vkSemaphore->device->signalSemaphore(
        vkSemaphore->device->handle,
        &signalInfo);

    if (result != VK_SUCCESS) {
        return vkResultToPal(result);
    }

    return PAL_RESULT_SUCCESS;
}

PalResult PAL_CALL getVkSemaphoreValue(
    PalSemaphore* semaphore, 
    Uint64* value) 
{
    Semaphore* vkSemaphore = (Semaphore*)semaphore;
    if (!(vkSemaphore->device->features & 
        PAL_ADAPTER_FEATURE_TIMELINE_SEMAPHORE)) {
        return PAL_RESULT_ADAPTER_FEATURE_NOT_SUPPORTED;
    }

    VkResult result = vkSemaphore->device->getSemaphoreValue(
        vkSemaphore->device->handle, 
        vkSemaphore->handle, 
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
    CommandPool* pool = nullptr;
    Device* vkDevice = (Device*)device;
    Queue* queue = (Queue*)info->queue;
    PhysicalQueue* phyQueue = queue->phyQueue;

    pool = palAllocate(s_Vk.allocator, sizeof(CommandPool), 0);
    if (!pool) {
        return PAL_RESULT_OUT_OF_MEMORY;
    }

    VkCommandPoolCreateInfo createInfo = {0};
    createInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    createInfo.queueFamilyIndex = phyQueue->familyIndex;

    if (info->resettable) {
        if (!(vkDevice->features & 
            PAL_ADAPTER_FEATURE_COMMAND_POOL_FLAG_RESETTABLE)) {
            return PAL_RESULT_ADAPTER_FEATURE_NOT_SUPPORTED;
        }
        createInfo.flags |= VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    }

    if (info->transient) {
        if (!(vkDevice->features & 
            PAL_ADAPTER_FEATURE_COMMAND_POOL_FLAG_TRANSIENT)) {
            return PAL_RESULT_ADAPTER_FEATURE_NOT_SUPPORTED;
        }
        createInfo.flags |= VK_COMMAND_POOL_CREATE_TRANSIENT_BIT;
    }

    result = s_Vk.createCommandPool(
        vkDevice->handle, 
        &createInfo, 
        &s_Vk.vkAllocator, 
        &pool->handle);

    if (result != VK_SUCCESS) {
        palFree(s_Vk.allocator, pool);
        return vkResultToPal(result);
    }

    pool->device = vkDevice;
    *outPool = (PalCommandPool*)pool;
    return PAL_RESULT_SUCCESS;
}

void PAL_CALL destroyVkCommandPool(PalCommandPool* pool)
{
    CommandPool* vkPool = (CommandPool*)pool;
    s_Vk.destroyCommandPool(
        vkPool->device->handle, 
        vkPool->handle, 
        &s_Vk.vkAllocator);

    palFree(s_Vk.allocator, vkPool);
}

PalResult PAL_CALL createVkCommandBuffer(
    PalDevice* device,
    PalCommandPool* pool,
    PalCommandBufferType type,
    PalCommandBuffer** outCmdBuffer)
{
    VkResult result;
    CommandBuffer* cmdBuffer = nullptr;
    Device* vkDevice = (Device*)device;
    CommandPool* vkPool = (CommandPool*)pool;

    cmdBuffer = palAllocate(s_Vk.allocator, sizeof(CommandBuffer), 0);
    if (!cmdBuffer) {
        PAL_RESULT_OUT_OF_MEMORY;
    }

    VkCommandBufferAllocateInfo createInfo = {0};
    createInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    createInfo.commandBufferCount = 1;
    createInfo.commandPool = vkPool->handle;

    createInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    cmdBuffer->primary = true;
    if (type == PAL_COMMAND_BUFFER_TYPE_SECONDARY) {
        createInfo.level = VK_COMMAND_BUFFER_LEVEL_SECONDARY;
        cmdBuffer->primary = false;
    }

    result = s_Vk.createCommandBuffer(
        vkDevice->handle, 
        &createInfo, 
        &cmdBuffer->handle);

    if (result != VK_SUCCESS) {
        palFree(s_Vk.allocator, cmdBuffer);
        return vkResultToPal(result);
    }

    cmdBuffer->device = vkDevice;
    cmdBuffer->pool = vkPool->handle;

    *outCmdBuffer = (PalCommandBuffer*)cmdBuffer;
    return PAL_RESULT_SUCCESS;
}

void PAL_CALL destroyVkCommandBuffer(PalCommandBuffer* cmdBuffer)
{
    CommandBuffer* vkCmdBuffer = (CommandBuffer*)cmdBuffer;
    s_Vk.destroyCommandBuffer(
        vkCmdBuffer->device->handle,
        vkCmdBuffer->pool,
        1,
        &vkCmdBuffer->handle);

    palFree(s_Vk.allocator, vkCmdBuffer);
}

PalResult PAL_CALL beginVkCommandBuffer(
    PalCommandBuffer* cmdBuffer, 
    PalRenderPass* renderPass)
{
    CommandBuffer* vkCmdBuffer = (CommandBuffer*)cmdBuffer;
    RenderPass* vkRenderPass = (RenderPass*)renderPass;
    VkRenderPass renderPassHandle = nullptr;
    VkFramebuffer frambufferHandle = nullptr;

    VkCommandBufferBeginInfo beginInfo = {0};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

    VkCommandBufferInheritanceInfo inheritanceInfo = {0};
    inheritanceInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_INHERITANCE_INFO;

    if (!vkCmdBuffer->primary) {
        // secondary command buffer
        if (renderPass) {
            renderPassHandle = vkRenderPass->handle;
            frambufferHandle = vkRenderPass->framebuffer;
            inheritanceInfo.framebuffer = frambufferHandle;
            inheritanceInfo.renderPass = renderPassHandle;

            beginInfo.flags = VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT;
            beginInfo   .pInheritanceInfo = &inheritanceInfo;
        }
    }

    VkResult result = s_Vk.cmdBegin(vkCmdBuffer->handle, &beginInfo);
    if (result != VK_SUCCESS) {
        return vkResultToPal(result);
    }

    return PAL_RESULT_SUCCESS;
}

PalResult PAL_CALL endVkCommandBuffer(PalCommandBuffer* cmdBuffer)
{
    CommandBuffer* vkCmdBuffer = (CommandBuffer*)cmdBuffer;
    VkResult result = s_Vk.cmdEnd(vkCmdBuffer->handle);
    if (result != VK_SUCCESS) {
        return vkResultToPal(result);
    }

    return PAL_RESULT_SUCCESS;
}

PalResult PAL_CALL executeCommandBufferVk(
    PalCommandBuffer* primaryCmdBuffer,
    PalCommandBuffer* secondaryCmdBuffer)
{
    CommandBuffer* vkCmdBuffer = (CommandBuffer*)primaryCmdBuffer;
    CommandBuffer* vkCmdBuffer2 = (CommandBuffer*)secondaryCmdBuffer;
    s_Vk.cmdExecuteCommandBuffer(
        vkCmdBuffer->handle, 
        1,
        &vkCmdBuffer2->handle);

    return PAL_RESULT_SUCCESS;
}

PalResult PAL_CALL setVkFragmentShadingRate(
    PalCommandBuffer* cmdBuffer,
    PalFragmentShadingRateState* state)
{
    CommandBuffer* vkCmdBuffer = (CommandBuffer*)cmdBuffer;
    if (!(vkCmdBuffer->device->features & 
        PAL_ADAPTER_FEATURE_FRAGMENT_SHADING_RATE)) {
        return PAL_RESULT_ADAPTER_FEATURE_NOT_SUPPORTED;
    }

    VkExtent2D size = getShadingRateSize(state->rate);
    VkFragmentShadingRateCombinerOpKHR combinerOps[2];

    for (int i = 0; i < 2; i++) {
        switch (state->combinerOps[i]) {
            case PAL_FRAGMENT_SHADING_RATE_COMBINER_OP_KEEP: {
                combinerOps[i] = VK_FRAGMENT_SHADING_RATE_COMBINER_OP_KEEP_KHR;
                continue;
            }

            case PAL_FRAGMENT_SHADING_RATE_COMBINER_OP_REPLACE: {
                combinerOps[i] = 
                    VK_FRAGMENT_SHADING_RATE_COMBINER_OP_REPLACE_KHR;
                continue;
            }

            case PAL_FRAGMENT_SHADING_RATE_COMBINER_OP_MIN: {
                combinerOps[i] = VK_FRAGMENT_SHADING_RATE_COMBINER_OP_MIN_KHR;
                continue;
            }

            case PAL_FRAGMENT_SHADING_RATE_COMBINER_OP_MAX: {
                combinerOps[i] = VK_FRAGMENT_SHADING_RATE_COMBINER_OP_MAX_KHR;
                continue;
            }

            case PAL_FRAGMENT_SHADING_RATE_COMBINER_OP_MUL: {
                combinerOps[i] = VK_FRAGMENT_SHADING_RATE_COMBINER_OP_MUL_KHR;
                continue;
            }

            combinerOps[i] = VK_FRAGMENT_SHADING_RATE_COMBINER_OP_KEEP_KHR;
        }
    }

    vkCmdBuffer->device->cmdSetFragmentShadingRate(
        vkCmdBuffer->handle,
        &size,
        combinerOps);

    return PAL_RESULT_SUCCESS;
}

PalResult PAL_CALL drawVkMeshTasks(
    PalCommandBuffer* cmdBuffer,
    Uint32 groupCountX,
    Uint32 groupCountY,
    Uint32 groupCountZ)
{
    CommandBuffer* vkCmdBuffer = (CommandBuffer*)cmdBuffer;
    if (!(vkCmdBuffer->device->features & PAL_ADAPTER_FEATURE_MESH_SHADER)) {
        return PAL_RESULT_ADAPTER_FEATURE_NOT_SUPPORTED;
    }

    vkCmdBuffer->device->cmdDrawMeshTask(
        vkCmdBuffer->handle, 
        groupCountX, 
        groupCountY, 
        groupCountZ);

    return PAL_RESULT_SUCCESS;
}

PalResult PAL_CALL drawVkMeshTasksIndirect(
    PalCommandBuffer* cmdBuffer,
    PalBuffer* buffer,
    Uint64 offset,
    Uint32 drawCount,
    Uint32 stride)
{
    CommandBuffer* vkCmdBuffer = (CommandBuffer*)cmdBuffer;
    if (!(vkCmdBuffer->device->features & PAL_ADAPTER_FEATURE_MESH_SHADER)) {
        return PAL_RESULT_ADAPTER_FEATURE_NOT_SUPPORTED;
    }

    Buffer* vkBuffer = (Buffer*)buffer;
    vkCmdBuffer->device->cmdDrawMeshTaskIndirect(
        vkCmdBuffer->handle,
        vkBuffer->handle,
        offset,
        drawCount,
        stride);

    return PAL_RESULT_SUCCESS;
}

PalResult PAL_CALL drawVkMeshTasksIndirectCount(
    PalCommandBuffer* cmdBuffer,
    PalBuffer* buffer,
    PalBuffer* countBuffer,
    Uint64 offset,
    Uint64 countBufferOffset,
    Uint32 maxDrawCount,
    Uint32 stride)
{
    CommandBuffer* vkCmdBuffer = (CommandBuffer*)cmdBuffer;
    if (!(vkCmdBuffer->device->features & 
        PAL_ADAPTER_FEATURE_MESH_SHADER_INDIRECT_COUNT)) {
        return PAL_RESULT_ADAPTER_FEATURE_NOT_SUPPORTED;
    }

    Buffer* vkBuffer = (Buffer*)buffer;
    Buffer* vkCountBuffer = (Buffer*)countBuffer;

    vkCmdBuffer->device->cmdDrawMeshTaskIndirectCount(
        vkCmdBuffer->handle,
        vkBuffer->handle,
        offset,
        vkCountBuffer->handle,
        countBufferOffset,
        maxDrawCount,
        stride);

    return PAL_RESULT_SUCCESS;
}

PalResult PAL_CALL buildVkAccelerationStructure(
    PalCommandBuffer* cmdBuffer,
    PalAccelerationStructureBuildInfo* info)
{
    CommandBuffer* vkCmdBuffer = (CommandBuffer*)cmdBuffer;
    if (!(vkCmdBuffer->device->features & PAL_ADAPTER_FEATURE_RAY_TRACING)) {
        return PAL_RESULT_ADAPTER_FEATURE_NOT_SUPPORTED;
    }
    
    VkAccelerationStructureGeometryKHR* geometries = nullptr;
    VkAccelerationStructureBuildRangeInfoKHR* rangeInfos = nullptr;
    AccelerationStructure* as = (AccelerationStructure*)info->dst;
    Buffer* scratchBuffer = (Buffer*)info->scratchBuffer;

    geometries = palAllocate(
        s_Vk.allocator, 
        sizeof(VkAccelerationStructureGeometryKHR) * info->geometryCount, 
        0);

    rangeInfos = palAllocate(
        s_Vk.allocator, 
        sizeof(VkAccelerationStructureBuildRangeInfoKHR) * info->geometryCount,
        0);

    if (!rangeInfos || !geometries) {
        return PAL_RESULT_OUT_OF_MEMORY;
    }

    // clang-format off
    for (int i = 0; i < info->geometryCount; i++) {
        // fill vulkan geometry struct
        VkAccelerationStructureGeometryKHR* tmp = &geometries[i];
        tmp->sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_KHR;
        tmp->flags = VK_GEOMETRY_OPAQUE_BIT_KHR;

        if (info->geometries[i].type == PAL_GEOMETRY_TYPE_TRIANGLE) {
            tmp->geometryType = VK_GEOMETRY_TYPE_TRIANGLES_KHR;

            VkAccelerationStructureGeometryTrianglesDataKHR* data = &tmp->geometry.triangles;
            data->pNext = nullptr;
            data->sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_TRIANGLES_DATA_KHR;
            
            VkDeviceOrHostAddressConstKHR vertexAddress = {0};
            VkDeviceOrHostAddressConstKHR indexAddress = {0};
            PalGeometryDataTriangle* tmpData = info->geometries[i].data;
            Buffer* vkVertexBuffer = (Buffer*)tmpData->vertexBuffer;
            Buffer* vkIndexBuffer = (Buffer*)tmpData->indexBuffer;

            vertexAddress.deviceAddress = vkVertexBuffer->address + tmpData->vertexOffset;
            data->vertexData = vertexAddress;
            data->maxVertex = tmpData->vertexCount;
            data->vertexFormat = vertexTypeToVkFormat(tmpData->vertexType);
            data->vertexStride = tmpData->vertexStride;

            indexAddress.deviceAddress = vkIndexBuffer->address + tmpData->indexOffset;
            data->indexData = indexAddress;
            if (tmpData->indexType == PAL_INDEX_TYPE_UINT32) {
                data->indexType = VK_INDEX_TYPE_UINT32;
            } else {
                data->indexType = VK_INDEX_TYPE_UINT16;
            }
            
        } else if (info->geometries[i].type == PAL_GEOMETRY_TYPE_AABBS) {
            tmp->geometryType = VK_GEOMETRY_TYPE_AABBS_KHR;
            VkAccelerationStructureGeometryAabbsDataKHR* data = &tmp->geometry.aabbs;
            data->pNext = nullptr;
            data->sType = 
                VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_AABBS_DATA_KHR;
            
            VkDeviceOrHostAddressConstKHR address = {0};
            PalGeometryDataAABBS* tmpData = info->geometries[i].data;
            Buffer* vkBuffer = (Buffer*)tmpData->buffer;
            address.deviceAddress = vkBuffer->address + tmpData->offset;
            data->data = address;
            data->stride = tmpData->stride;

        } else if (info->geometries[i].type == PAL_GEOMETRY_TYPE_INSTANCE) {
            tmp->geometryType = VK_GEOMETRY_TYPE_INSTANCES_KHR;
            VkAccelerationStructureGeometryInstancesDataKHR* data = nullptr;
            data = &tmp->geometry.instances;
            data->pNext = nullptr;
            data->sType = 
                VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_INSTANCES_DATA_KHR;
            
            VkDeviceOrHostAddressConstKHR address = {0};
            PalGeometryDataInstance* tmpData = info->geometries[i].data;
            Buffer* vkBuffer = (Buffer*)tmpData->buffer;
            address.deviceAddress = vkBuffer->address + tmpData->offset;
            data->data = address;
        }

        // range info
        VkAccelerationStructureBuildRangeInfoKHR* rangeInfo = &rangeInfos[i];
        rangeInfo->primitiveCount = info->geometries[i].primitiveCount;
        rangeInfo->firstVertex = 0; // PAL does not allow setting this
        rangeInfo->primitiveOffset = 0; // PAL does not allow setting this
        rangeInfo->transformOffset = 0; // PAL does not allow setting this
    }
    // clang-format on

    VkAccelerationStructureBuildGeometryInfoKHR buildInfo = {0};
    buildInfo.sType = 
        VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_GEOMETRY_INFO_KHR;
    
    if (info->type == PAL_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL) {
        buildInfo.type = VK_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL_KHR;
    } else {
        buildInfo.type = VK_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL_KHR;
    }

    buildInfo.mode = VK_BUILD_ACCELERATION_STRUCTURE_MODE_BUILD_KHR;
    buildInfo.geometryCount = info->geometryCount;
    buildInfo.dstAccelerationStructure = as->handle;

    VkDeviceOrHostAddressKHR scratchData = {0};
    scratchData.deviceAddress = 
        scratchBuffer->address + info->scratchBufferOffset;
    buildInfo.scratchData = scratchData;

    buildInfo.flags = VK_BUILD_ACCELERATION_STRUCTURE_PREFER_FAST_BUILD_BIT_KHR;
    buildInfo.pGeometries = geometries;

    const VkAccelerationStructureBuildRangeInfoKHR* tmp[1];
    tmp[0] = rangeInfos;
    vkCmdBuffer->device->cmdBuildAccelerationStructures(
        vkCmdBuffer->handle,
        1,
        &buildInfo,
        tmp);

    palFree(s_Vk.allocator, geometries);
    palFree(s_Vk.allocator, rangeInfos);

    return PAL_RESULT_SUCCESS;
}

PalResult PAL_CALL beginRenderPassVk(
    PalCommandBuffer* cmdBuffer,
    PalRenderPass* renderPass,
    Int32 clearValueCount,
    PalClearValue* clearValues)
{
    CommandBuffer* vkCmdBuffer = (CommandBuffer*)cmdBuffer;
    RenderPass* vkRenderPass = (RenderPass*)renderPass;
    if (clearValueCount != vkRenderPass->attachmentCount) {
        return PAL_RESULT_INSUFFICIENT_BUFFER;
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
    renderPassBeginInfo.framebuffer = vkRenderPass->framebuffer;
    renderPassBeginInfo.renderPass = vkRenderPass->handle;
    renderPassBeginInfo.renderArea.extent.width = vkRenderPass->width;
    renderPassBeginInfo.renderArea.extent.height = vkRenderPass->height;

    s_Vk.cmdBeginRenderPass(
        vkCmdBuffer->handle, 
        &renderPassBeginInfo, 
        VK_SUBPASS_CONTENTS_INLINE);

    return PAL_RESULT_SUCCESS;
}

PalResult PAL_CALL endRenderPassVk(PalCommandBuffer* cmdBuffer)
{
    CommandBuffer* vkCmdBuffer = (CommandBuffer*)cmdBuffer;
    s_Vk.cmdEndRenderPass(vkCmdBuffer->handle);
    return PAL_RESULT_SUCCESS;
}

PalResult PAL_CALL copyVkBuffer(
    PalCommandBuffer* cmdBuffer,
    PalBuffer* dst,
    PalBuffer* src,
    Uint64 dstOffset,
    Uint64 srcOffset,
    Uint32 size)
{
    CommandBuffer* vkCmdBuffer = (CommandBuffer*)cmdBuffer;
    Buffer* dstbuffer = (Buffer*)dst;
    Buffer* srcBuffer = (Buffer*)src;

    VkBufferCopy copyRegion = {0};
    copyRegion.size = size;
    copyRegion.dstOffset = dstOffset;
    copyRegion.srcOffset = srcOffset;
    s_Vk.cmdCopyBuffer(
        vkCmdBuffer->handle, 
        srcBuffer->handle, 
        dstbuffer->handle, 
        1, 
        &copyRegion);

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
    Queue* vkQueue = (Queue*)queue;
    CommandBuffer* vkCmdBuffer = (CommandBuffer*)info->cmdBuffer;

    if (info->waitSemaphore) {
        Semaphore* tmp = (Semaphore*)info->waitSemaphore;
        waitSemaphoreHandle = tmp->handle;
        waitSemaphoreCount = 1;
    }

    if (info->signalSemaphore) {
        Semaphore* tmp = (Semaphore*)info->signalSemaphore;
        signalSemaphoreHandle = tmp->handle;
        signalSemaphoreCount = 1;
    }

    if (info->fence) {
        Fence* tmp = (Fence*)info->fence;
        fenceHandle = tmp->handle; 
    }

    VkSubmitInfo submitInfo = {0};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &vkCmdBuffer->handle;
    submitInfo.pSignalSemaphores = &signalSemaphoreHandle;
    submitInfo.pWaitSemaphores = &waitSemaphoreHandle;
    submitInfo.waitSemaphoreCount = waitSemaphoreCount;
    submitInfo.waitSemaphoreCount = signalSemaphoreCount;

    result = s_Vk.queueSubmit(
        vkQueue->phyQueue->handle,
        1,
        &submitInfo, 
        fenceHandle);

    if (result != VK_SUCCESS) {
        return vkResultToPal(result);
    }

    return PAL_RESULT_SUCCESS;
}

// ==================================================
// Ray Tracing Pipeline
// ==================================================

PalResult PAL_CALL createVkAccelerationstructure(
    PalDevice* device,
    const PalAccelerationStructureCreateInfo* info,
    PalAccelerationStructure** outAs)
{
    VkResult result;
    AccelerationStructure* as = nullptr;
    Buffer* vkBuffer = (Buffer*)info->buffer;
    Device* vkDevice = (Device*)device;

    if (!(vkDevice->features & PAL_ADAPTER_FEATURE_RAY_TRACING)) {
        return PAL_RESULT_ADAPTER_FEATURE_NOT_SUPPORTED;
    }

    if (!vkBuffer->address) {
        return PAL_RESULT_INVALID_BUFFER;
    }

    as = palAllocate(s_Vk.allocator, sizeof(AccelerationStructure), 0);
    if (!as) {
        return PAL_RESULT_OUT_OF_MEMORY;
    }

    VkAccelerationStructureCreateInfoKHR createInfo = {0};
    createInfo.sType = 
        VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_CREATE_INFO_KHR;

    createInfo.offset = (VkDeviceSize)info->offset;
    createInfo.size = (VkDeviceSize)info->size;
    createInfo.buffer = vkBuffer->handle;
    createInfo.type = VK_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL_KHR;

    if (info->type == PAL_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL) {
        createInfo.type = VK_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL_KHR;
    }

    result = vkDevice->createAccelerationStructure(
        vkDevice->handle,
        &createInfo,
        &s_Vk.vkAllocator,
        &as->handle);

    if (result != VK_SUCCESS) {
        palFree(s_Vk.allocator, as);
        return vkResultToPal(result);
    }

    // get and cache address
    VkAccelerationStructureDeviceAddressInfoKHR addressInfo = {0};
    addressInfo.sType = 
        VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_DEVICE_ADDRESS_INFO_KHR;
    addressInfo.accelerationStructure = as->handle;

    as->address = vkDevice->getAccelerationDeviceAddress(
        vkDevice->handle, 
        &addressInfo);

    as->device = vkDevice;
    *outAs = (PalAccelerationStructure*)as;
    return PAL_RESULT_SUCCESS;
}

void PAL_CALL destroyVkAccelerationstructure(
    PalAccelerationStructure* as)
{
    AccelerationStructure* vkAs = (AccelerationStructure*)as;
    vkAs->device->destroyAccelerationStructure(
        vkAs->device->handle, 
        vkAs->handle, 
        &s_Vk.vkAllocator);

    palFree(s_Vk.allocator, vkAs);
}

PalResult PAL_CALL getVkAccelerationStructureBuildSize(
    PalDevice* device,
    PalAccelerationStructureBuildInfo* info,
    PalAccelerationStructureBuildSize* size)
{
    Uint32* maxPrimities = nullptr;
    VkAccelerationStructureGeometryKHR* geometries = nullptr;

    Device* vkDevice = (Device*)device;
    AccelerationStructure* vkAs = (AccelerationStructure*)info->dst;
    Buffer* vkScratchBuffer = (Buffer*)info->scratchBuffer;

    if (!(vkDevice->features & PAL_ADAPTER_FEATURE_RAY_TRACING)) {
        return PAL_RESULT_ADAPTER_FEATURE_NOT_SUPPORTED;
    }

    maxPrimities = palAllocate(
        s_Vk.allocator, 
        sizeof(Uint32) * info->geometryCount, 
        0);

    geometries = palAllocate(
        s_Vk.allocator, 
        sizeof(VkAccelerationStructureGeometryKHR) * info->geometryCount, 
        0);

    if (!maxPrimities || !geometries) {
        return PAL_RESULT_OUT_OF_MEMORY;
    }

    // clang-format off
    for (int i = 0; i < info->geometryCount; i++) {
        maxPrimities[i] = info->geometries[i].primitiveCount;

        // fill vulkan geometry struct
        VkAccelerationStructureGeometryKHR* tmp = &geometries[i];
        tmp->sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_KHR;
        tmp->flags = VK_GEOMETRY_OPAQUE_BIT_KHR;

        if (info->geometries[i].type == PAL_GEOMETRY_TYPE_TRIANGLE) {
            tmp->geometryType = VK_GEOMETRY_TYPE_TRIANGLES_KHR;

            VkAccelerationStructureGeometryTrianglesDataKHR* data = &tmp->geometry.triangles;
            data->pNext = nullptr;
            data->sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_TRIANGLES_DATA_KHR;
            
            VkDeviceOrHostAddressConstKHR vertexAddress = {0};
            VkDeviceOrHostAddressConstKHR indexAddress = {0};
            PalGeometryDataTriangle* tmpData = info->geometries[i].data;
            Buffer* vkVertexBuffer = (Buffer*)tmpData->vertexBuffer;
            Buffer* vkIndexBuffer = (Buffer*)tmpData->indexBuffer;

            vertexAddress.deviceAddress = vkVertexBuffer->address + tmpData->vertexOffset;
            data->vertexData = vertexAddress;
            data->maxVertex = tmpData->vertexCount;
            data->vertexFormat = vertexTypeToVkFormat(tmpData->vertexType);
            data->vertexStride = tmpData->vertexStride;

            indexAddress.deviceAddress = vkIndexBuffer->address + tmpData->indexOffset;
            data->indexData = indexAddress;
            if (tmpData->indexType == PAL_INDEX_TYPE_UINT32) {
                data->indexType = VK_INDEX_TYPE_UINT32;
            } else {
                data->indexType = VK_INDEX_TYPE_UINT16;
            }
            
        } else if (info->geometries[i].type == PAL_GEOMETRY_TYPE_AABBS) {
            tmp->geometryType = VK_GEOMETRY_TYPE_AABBS_KHR;
            VkAccelerationStructureGeometryAabbsDataKHR* data = &tmp->geometry.aabbs;
            data->pNext = nullptr;
            data->sType = 
                VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_AABBS_DATA_KHR;
            
            VkDeviceOrHostAddressConstKHR address = {0};
            PalGeometryDataAABBS* tmpData = info->geometries[i].data;
            Buffer* vkBuffer = (Buffer*)tmpData->buffer;
            address.deviceAddress = vkBuffer->address + tmpData->offset;
            data->data = address;
            data->stride = tmpData->stride;

        } else if (info->geometries[i].type == PAL_GEOMETRY_TYPE_INSTANCE) {
            tmp->geometryType = VK_GEOMETRY_TYPE_INSTANCES_KHR;
            VkAccelerationStructureGeometryInstancesDataKHR* data = nullptr;
            data = &tmp->geometry.instances;
            data->pNext = nullptr;
            data->sType = 
                VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_INSTANCES_DATA_KHR;
            
            VkDeviceOrHostAddressConstKHR address = {0};
            PalGeometryDataInstance* tmpData = info->geometries[i].data;
            Buffer* vkBuffer = (Buffer*)tmpData->buffer;
            address.deviceAddress = vkBuffer->address + tmpData->offset;
            data->data = address;
        }
    }
    // clang-format on

    VkAccelerationStructureBuildGeometryInfoKHR buildInfo = {0};
    buildInfo.sType = 
        VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_GEOMETRY_INFO_KHR;
    
    if (info->type == PAL_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL) {
        buildInfo.type = VK_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL_KHR;
    } else {
        buildInfo.type = VK_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL_KHR;
    }

    buildInfo.mode = VK_BUILD_ACCELERATION_STRUCTURE_MODE_BUILD_KHR;
    buildInfo.geometryCount = info->geometryCount;
    buildInfo.dstAccelerationStructure = vkAs->handle;

    VkDeviceOrHostAddressKHR scratchData = {0};
    scratchData.deviceAddress = 
        vkScratchBuffer->address + info->scratchBufferOffset;
    buildInfo.scratchData = scratchData;

    buildInfo.flags = VK_BUILD_ACCELERATION_STRUCTURE_PREFER_FAST_BUILD_BIT_KHR;
    buildInfo.pGeometries = geometries;

    VkAccelerationStructureBuildSizesInfoKHR sizeInfo = {0};
    sizeInfo.sType = 
        VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_SIZES_INFO_KHR;

    vkDevice->getAccelerationBuildsize(
        vkDevice->handle, 
        VK_ACCELERATION_STRUCTURE_BUILD_TYPE_DEVICE_KHR, 
        &buildInfo,
        maxPrimities,
        &sizeInfo);

    size->accelerationStructureSize = sizeInfo.accelerationStructureSize;
    size->scratchBufferSize = sizeInfo.buildScratchSize;

    palFree(s_Vk.allocator, maxPrimities);
    palFree(s_Vk.allocator, geometries);
    
    return PAL_RESULT_SUCCESS;
}

// ==================================================
// Buffer
// ==================================================

PalResult PAL_CALL createVkBuffer(
    PalDevice* device,
    const PalBufferCreateInfo* info,
    PalBuffer** outBuffer)
{
    VkResult result;
    Buffer* buffer = nullptr;
    Device* vkDevice = (Device*)device;
    if (info->usages & PAL_BUFFER_USAGE_RAY_TRACING) {
        if (!(vkDevice->features & PAL_ADAPTER_FEATURE_RAY_TRACING)) {
            return PAL_RESULT_ADAPTER_FEATURE_NOT_SUPPORTED;
        }
        // buffer device address feature is supported if ray tracing is

    } else if (info->usages & PAL_BUFFER_USAGE_DEVICE_ADDRESS) {
        if (!(vkDevice->features & 
            PAL_ADAPTER_FEATURE_BUFFER_DEVICE_ADDRESS)) {
            return PAL_RESULT_ADAPTER_FEATURE_NOT_SUPPORTED;
        }
    }

    buffer = palAllocate(s_Vk.allocator, sizeof(Buffer), 0);
    if (!buffer) {
        return PAL_RESULT_OUT_OF_MEMORY;
    }

    VkBufferCreateInfo createInfo = {0};
    createInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    createInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    createInfo.size = info->size;
    createInfo.usage = palBufferUsageToVk(info->usages);
    
    result = s_Vk.createBuffer(
        vkDevice->handle, 
        &createInfo,
        &s_Vk.vkAllocator,
        &buffer->handle);

    if (result != VK_SUCCESS) {
        palFree(s_Vk.allocator, buffer);
        return vkResultToPal(result);
    }

    // get the address if the address usage is set
    buffer->address = 0;
    if (createInfo.usage & VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT) {
        VkBufferDeviceAddressInfoKHR bufferInfo = {0};
        bufferInfo.buffer = buffer->handle;
        bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO_KHR;
        buffer->address = 
            vkDevice->getBufferrAddress(vkDevice->handle, &bufferInfo);
    }

    buffer->device = vkDevice;
    *outBuffer = (PalBuffer*)buffer;
    return PAL_RESULT_SUCCESS;
}

void PAL_CALL destroyVkBuffer(PalBuffer* buffer)
{
    Buffer* vkBuffer = (Buffer*)buffer;
    s_Vk.destroyBuffer(
        vkBuffer->device->handle, 
        vkBuffer->handle, 
        &s_Vk.vkAllocator);

    palFree(s_Vk.allocator, buffer);
}

PalResult PAL_CALL getVkBufferMemoryRequirements(
    PalBuffer* buffer,
    PalMemoryRequirements* requirements)
{
    Buffer* vkBuffer = (Buffer*)buffer;
    Device* device = vkBuffer->device;
    VkPhysicalDevice phyDevice = (VkPhysicalDevice)device->phyDevice;
    VkPhysicalDeviceMemoryProperties memProps = {0};
    s_Vk.getPhysicalDeviceMemoryProperties(phyDevice, &memProps);

    VkMemoryRequirements memReq = {0};
    s_Vk.getBufferMemoryRequirements(
        device->handle, 
        vkBuffer->handle,
        &memReq);

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

PalResult PAL_CALL bindVkBufferMemory(
    PalBuffer* buffer,
    PalMemory* memory,
    Uint64 offset)
{
    VkResult result;
    VkDeviceMemory mem = (VkDeviceMemory)memory;
    Buffer* vkBuffer = (Buffer*)buffer;
    result = s_Vk.bindBufferMemory(
        vkBuffer->device->handle, 
        vkBuffer->handle, 
        mem, 
        offset);

    if (result != VK_SUCCESS) {
        return vkResultToPal(result);
    }
    return PAL_RESULT_SUCCESS;
}

PalResult PAL_CALL mapVkBuffer(
    PalBuffer* buffer,
    PalMemory* memory,
    Uint64 offset,
    Uint64 size,
    void** outPtr)
{
    VkResult result;
    VkDeviceMemory mem = (VkDeviceMemory)memory;
    Buffer* vkBuffer = (Buffer*)buffer;

    result = s_Vk.mapMemory(
        vkBuffer->device->handle, 
        mem, 
        offset, 
        size, 
        0, 
        outPtr);

    if (result != VK_SUCCESS) {
        return vkResultToPal(result);
    }
    return PAL_RESULT_SUCCESS; 
}

void PAL_CALL unmapVkBuffer(
    PalBuffer* buffer,
    PalMemory* memory)
{
    VkDeviceMemory mem = (VkDeviceMemory)memory;
    Buffer* vkBuffer = (Buffer*)buffer;
    s_Vk.unmapMemory(vkBuffer->device->handle, mem);
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