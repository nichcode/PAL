
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "pal/pal_graphics.h"

#if PAL_HAS_VULKAN
#include <vulkan/vulkan_core.h>

// HACK: Needed to determine display type if on linux
#ifdef _WIN32
#include <libloaderapi.h>
#define VK_LIB_NAME "vulkan-1.dll"
#elif defined(__linux__)
#include <dlfcn.h>
#define VK_LIB_NAME "libvulkan.so"
#else
// Android
#define VK_LIB_NAME ""
#endif // _WIN32

// ==================================================
// Typedefs, enums and structs
// ==================================================

#define MAX_ATTACHMENTS 32

#pragma region Video

struct wl_display;
struct wl_surface;
typedef struct _XDisplay Display;
typedef unsigned long Window;
typedef unsigned long VisualID;
typedef unsigned long Colormap;
typedef char *XPointer;
typedef struct _XGC *GC;

typedef struct _XExtData {
	int number;
	struct _XExtData *next;
	int (*free_private)(
	struct _XExtData *extension
	);
	XPointer private_data;
} XExtData;

typedef struct {
	XExtData *ext_data;
	VisualID visualid;
	int class;
	unsigned long red_mask, green_mask, blue_mask;
	int bits_per_rgb;
	int map_entries;
} Visual;

typedef struct {
	int depth;
	int nvisuals;
	Visual *visuals;
} Depth;

typedef struct {
	XExtData *ext_data;
	struct _XDisplay *display;
	Window root;
	int width, height;
	int mwidth, mheight;
	int ndepths;
	Depth *depths;
	int root_depth;
	Visual *root_visual;
	GC default_gc;
	Colormap cmap;
	unsigned long white_pixel;
	unsigned long black_pixel;
	int max_maps, min_maps;
	int backing_store;
	int save_unders;
	long root_input_mask;
} Screen;

typedef struct {
    int x, y;
    int width, height;
    int border_width;
    int depth;
    Visual *visual;
    Window root;
    int class;
    int bit_gravity;
    int win_gravity;
    int backing_store;
    unsigned long backing_planes;
    unsigned long backing_pixel;
    int save_under;
    Colormap colormap;
    int map_installed;
    int map_state;
    long all_event_masks;
    long your_event_mask;
    long do_not_propagate_mask;
    int override_redirect;
    Screen *screen;
} XWindowAttributes;

typedef int (*XGetWindowAttributesFn)(
    Display*,
    Window,
    XWindowAttributes*);

typedef VisualID (*XVisualIDFromVisualFn)(Visual*);

typedef struct xcb_connection_t xcb_connection_t;
typedef uint32_t xcb_window_t;
typedef uint32_t xcb_visualid_t;
typedef uint32_t xcb_colormap_t;

typedef struct xcb_get_window_attributes_cookie_t {
    unsigned int sequence;
} xcb_get_window_attributes_cookie_t;

typedef struct xcb_get_window_attributes_reply_t {
    uint8_t        response_type;
    uint8_t        backing_store;
    uint16_t       sequence;
    uint32_t       length;
    xcb_visualid_t visual;
    uint16_t       _class;
    uint8_t        bit_gravity;
    uint8_t        win_gravity;
    uint32_t       backing_planes;
    uint32_t       backing_pixel;
    uint8_t        save_under;
    uint8_t        map_is_installed;
    uint8_t        map_state;
    uint8_t        override_redirect;
    xcb_colormap_t colormap;
    uint32_t       all_event_masks;
    uint32_t       your_event_mask;
    uint16_t       do_not_propagate_mask;
    uint8_t        pad0[2];
} xcb_get_window_attributes_reply_t;

typedef struct {
    uint8_t   response_type;
    uint8_t   error_code;
    uint16_t sequence;
    uint32_t resource_id;
    uint16_t minor_code;
    uint8_t major_code;
    uint8_t pad0;
    uint32_t pad[5];
    uint32_t full_sequence;
} xcb_generic_error_t;

typedef xcb_get_window_attributes_cookie_t (*xcb_get_window_attributes_fn)(
    xcb_connection_t*,
    xcb_window_t);

typedef xcb_get_window_attributes_reply_t* (*xcb_get_window_attributes_reply_fn)(
    xcb_connection_t*,
    xcb_get_window_attributes_cookie_t,
    xcb_generic_error_t**);

typedef unsigned long DWORD;
typedef int WINBOOL;
typedef void *LPVOID;
typedef const wchar_t *LPCWSTR,*PCWSTR;
typedef void *HANDLE;
typedef struct HINSTANCE__ *HINSTANCE;
typedef struct HWND__ *HWND;
typedef struct HMONITOR__ *HMONITOR;

// only define the struct if not on windows
#ifndef _MINWINBASE_
typedef struct _SECURITY_ATTRIBUTES {
    DWORD nLength;
    LPVOID ipSecurityDescriptor;
    WINBOOL bInheritHandle;
} SECURITY_ATTRIBUTES;
#endif // _MINWINBASE_

#include <vulkan/vulkan_wayland.h>
#include <vulkan/vulkan_xlib.h>
#include <vulkan/vulkan_xcb.h>
#include <vulkan/vulkan_win32.h>

#pragma endregion

typedef struct {
    const PalGraphicsBackend* backend;

    VkPhysicalDevice handle;
} Adapter;

typedef struct {
    bool useCache;
    void* handle;
    Adapter* adapters;
    VkInstance instance;
    VkDebugUtilsMessengerEXT messenger;
    PalDebugCallback callback;

    void* libX;
    XGetWindowAttributesFn XGetWindowAttributes;
    XVisualIDFromVisualFn XVisualIDFromVisual;

    void* libXcb;
    xcb_get_window_attributes_fn xcbGetWindowAttributes;
    xcb_get_window_attributes_reply_fn xcbGetWindowAttributesReply;

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
    PFN_vkGetPhysicalDeviceImageFormatProperties getPhysicalDeviceImageFormatProperties;
    PFN_vkGetImageMemoryRequirements getImageMemoryRequirements;
    PFN_vkAllocateMemory allocateMemory;
    PFN_vkFreeMemory freeMemory;
    PFN_vkBindImageMemory bindImageMemory;

    PFN_vkCreateDevice createDevice;
    PFN_vkDestroyDevice destroyDevice;
    PFN_vkGetDeviceQueue getDeviceQueue;
    PFN_vkQueueSubmit queueSubmit;
    PFN_vkGetDeviceProcAddr getDeviceProcAddr;
    PFN_vkCreateImage createImage;
    PFN_vkDestroyImage destroyImage;
    PFN_vkCreateShaderModule createShader;
    PFN_vkDestroyShaderModule destroyShader;
    PFN_vkCreateSampler createSampler;
    PFN_vkDestroySampler destroySampler;

    PFN_vkCreateCommandPool createCommandPool;
    PFN_vkDestroyCommandPool destroyCommandPool;
    PFN_vkAllocateCommandBuffers allocateCommandBuffer;
    PFN_vkFreeCommandBuffers freeCommandBuffer;
    PFN_vkCreateFence createFence;
    PFN_vkDestroyFence destroyFence;
    PFN_vkResetFences resetFence;
    PFN_vkWaitForFences waitFence;
    PFN_vkGetFenceStatus isFenceSignaled;
    PFN_vkCreateSemaphore createSemaphore;
    PFN_vkDestroySemaphore destroySemaphore;

    PFN_vkBeginCommandBuffer cmdBegin;
    PFN_vkEndCommandBuffer cmdEnd;
    PFN_vkResetCommandPool resetCommandPool;
    PFN_vkResetCommandBuffer resetCommandBuffer;
    PFN_vkCmdExecuteCommands cmdExecuteCommandBuffer;
    PFN_vkCmdCopyBuffer cmdCopyBuffer;
    PFN_vkCmdCopyBufferToImage cmdCopyBufferToImage;
    PFN_vkCmdCopyImage cmdCopyImage;
    PFN_vkCmdCopyImageToBuffer cmdCopyImageToBuffer;
    PFN_vkCmdBindPipeline cmdBindPipeline;
    PFN_vkCmdSetViewport cmdSetViewports;
    PFN_vkCmdSetScissor cmdSetScissors;
    PFN_vkCmdBindVertexBuffers cmdBindVertexBuffers;
    PFN_vkCmdBindIndexBuffer cmdBindIndexBuffer;
    PFN_vkCmdDraw cmdDraw;
    PFN_vkCmdDrawIndirect cmdDrawIndirect;
    PFN_vkCmdDrawIndexed cmdDrawIndexed;
    PFN_vkCmdDrawIndexedIndirect cmdDrawIndexedIndirect;
    PFN_vkCmdDispatch cmdDispatch;
    PFN_vkCmdDispatchIndirect cmdDispatchIndirect;
    PFN_vkCmdBindDescriptorSets cmdBindDescriptorSets;
    PFN_vkCmdPushConstants cmdPushConstants;

    PFN_vkCreateWaylandSurfaceKHR createWaylandSurface;
    PFN_vkCreateXlibSurfaceKHR createXlibSurface;
    PFN_vkCreateXcbSurfaceKHR createXcbSurface;
    PFN_vkCreateWin32SurfaceKHR createWin32Surface;

    PFN_vkDestroySurfaceKHR destroySurface;
    PFN_vkGetPhysicalDeviceSurfaceCapabilitiesKHR getSurfaceCapabilities;
    PFN_vkGetPhysicalDeviceSurfaceFormatsKHR getSurfaceFormats;
    PFN_vkGetPhysicalDeviceSurfacePresentModesKHR getSurfacePresentModes;
    PFN_vkGetPhysicalDeviceSurfaceSupportKHR checkSurfaceSupport;

    PFN_vkCreateBuffer createBuffer;
    PFN_vkDestroyBuffer destroyBuffer;
    PFN_vkGetBufferMemoryRequirements getBufferMemoryRequirements;
    PFN_vkBindBufferMemory bindBufferMemory;
    PFN_vkMapMemory mapMemory;
    PFN_vkUnmapMemory unmapMemory;
    PFN_vkGetBufferDeviceAddress getBufferDeviceAddress;

    PFN_vkCreateDescriptorSetLayout createDescriptorSetLayout;
    PFN_vkDestroyDescriptorSetLayout destroyDescriptorSetLayout;
    PFN_vkCreateDescriptorPool createDescriptorPool;
    PFN_vkDestroyDescriptorPool destroyDescriptorPool;
    PFN_vkResetDescriptorPool resetDescriptorPool;
    PFN_vkAllocateDescriptorSets allocateDescriptorSet;
    PFN_vkUpdateDescriptorSets updateDescriptorSet;

    PFN_vkCreatePipelineLayout createPipelineLayout;
    PFN_vkDestroyPipelineLayout destroyPipelineLayout;
    PFN_vkCreateGraphicsPipelines createGraphicsPipeline;
    PFN_vkCreateComputePipelines createComputePipeline;
    PFN_vkDestroyPipeline destroyPipeline;
    PFN_vkCreateDebugUtilsMessengerEXT createMessenger;
    PFN_vkDestroyDebugUtilsMessengerEXT destroyMessenger;
    PFN_vkQueueWaitIdle waitQueue;

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
    Int32 phyQueueCount;
    Int32 phyQueueIndex;
    Int32 queueFamilyCount;
    Uint32 memoryClassMask[3];
    VkPhysicalDevice phyDevice;
    VkDevice handle;
    PhysicalQueue* phyQueues;
    PFN_vkGetBufferDeviceAddress getBufferrAddress;
    PFN_vkCmdDispatchBase cmdDispatchBase;

    // draw indirect
    PFN_vkCmdDrawIndirectCount cmdDrawIndirectCount;
    PFN_vkCmdDrawIndexedIndirectCount cmdDrawIndexedIndirectCount;

    // swapchain
    PFN_vkCreateSwapchainKHR createSwapchain;
    PFN_vkDestroySwapchainKHR destroySwapchain;
    PFN_vkGetSwapchainImagesKHR getSwapchainImages;
    PFN_vkAcquireNextImageKHR acquireNextImage;
    PFN_vkQueuePresentKHR queuePresent;

    // semaphore
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
    PFN_vkGetRayTracingShaderGroupHandlesKHR getRayTracingShaderGroupHandles;

    // dynamic rendering
    PFN_vkCmdBeginRendering cmdBeginRendering;
    PFN_vkCmdEndRendering cmdEndRendering;
    PFN_vkCmdPipelineBarrier2 cmdPipelineBarrier;
    PFN_vkQueueSubmit2 queueSubmit;

    // dynamic states
    PFN_vkCmdSetCullMode cmdSetCullMode;
    PFN_vkCmdSetFrontFace cmdSetFrontFace;
    PFN_vkCmdSetPrimitiveTopology cmdSetPrimitiveTopology;

    PFN_vkCmdSetDepthTestEnable cmdSetDepthTestEnable;
    PFN_vkCmdSetDepthWriteEnable cmdSetDepthWriteEnable;
    PFN_vkCmdSetStencilOp cmdSetStencilOp;
} Device;

typedef struct {
    const PalGraphicsBackend* backend;

    VkQueueFlags usage;
    Device* device;
    PhysicalQueue* phyQueue;
} Queue;

typedef struct {
    PalMemoryType type;
    VkDeviceMemory handle;
} Memory;

typedef struct {
    const PalGraphicsBackend* backend;

    bool belongsToSwapchain;
    Device* device;
    Memory* memory;
    VkImage handle;
    PalImageInfo info;
} Image;

typedef struct {
    const PalGraphicsBackend* backend;

    Uint32 layerCount;
    Device* device;
    Image* image;
    VkImageView handle;
} ImageView;

typedef struct {
    const PalGraphicsBackend* backend;

    Device* device;
    VkSurfaceKHR handle;
} Surface;

typedef struct {
    const PalGraphicsBackend* backend;

    Uint32 imageCount;
    Device* device;
    Queue* queue;
    VkSwapchainKHR handle;
    Image* images;
} Swapchain;

typedef struct {
    const PalGraphicsBackend* backend;

    Uint32 patchControlPoints;
    Device* device;
    VkShaderModule handle;
    VkPipelineShaderStageCreateInfo info;
} Shader;

typedef struct {
    const PalGraphicsBackend* backend;

    Device* device;
    VkCommandPool handle;
} CommandPool;

typedef struct {
    const PalGraphicsBackend* backend;

    Device* device;
    VkBuffer buffer;
    VkDeviceMemory bufferMemory;
    VkDeviceAddress baseAddress;

    VkStridedDeviceAddressRegionKHR raygenAddress;
    VkStridedDeviceAddressRegionKHR missAddress;
    VkStridedDeviceAddressRegionKHR hitAddress;
    VkStridedDeviceAddressRegionKHR callableAddress;
} ShaderBindingTable;

typedef struct {
    const PalGraphicsBackend* backend;

    bool primary;
    Device* device;
    CommandPool* pool;
    void* pipeline;
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

    PalBufferUsages usages;
    Memory* memory;
    Device* device;
    VkBuffer handle;
} Buffer;

typedef struct {
    const PalGraphicsBackend* backend;

    VkDeviceAddress address;
    VkBuffer buffer;
    VkDeviceMemory bufferMemory;
    VkDeviceAddress bufferAddress;
    Device* device;
    VkAccelerationStructureKHR handle;
} AccelerationStructure;

typedef struct {
    const PalGraphicsBackend* backend;

    Device* device;
    VkDescriptorSetLayout handle;
} DescriptorSetLayout;

typedef struct {
    const PalGraphicsBackend* backend;

    Device* device;
    VkDescriptorPool handle;
} DescriptorPool;

typedef struct {
    const PalGraphicsBackend* backend;

    Device* device;
    DescriptorPool* pool;
    VkDescriptorSet handle;
} DescriptorSet;

typedef struct {
    const PalGraphicsBackend* backend;

    Device* device;
    VkSampler handle;
} Sampler;

typedef struct {
    const PalGraphicsBackend* backend;

    Device* device;
    VkPipelineLayout handle;
} PipelineLayout;

typedef struct {
    const PalGraphicsBackend* backend;

    VkPipelineBindPoint bindPoint;
    Device* device;
    VkPipeline handle;
    VkPipelineLayout layout;
} Pipeline;

typedef struct {
    VkPipelineStageFlags2 stages;
    VkPipelineStageFlags2 dstStages;
    VkAccessFlags2 access;
    VkImageLayout layout;
} Barrier;

static Vulkan s_Vk = {0};

// ==================================================
// Helper Functions
// ==================================================

static void* loadLibrary(const char* name)
{
#ifdef __WIN32
    return LoadLibraryA(name);
#elif defined (__linux__)
    return dlopen(name, RTLD_LAZY);
#endif
}

static void freeLibrary(void* lib)
{
#ifdef __WIN32
    FreeLibrary((HMODULE)lib);
#elif defined (__linux__)
    dlclose(lib);
#endif
}

static void* loadProc(void* lib, const char* name)
{
#ifdef __WIN32
    return GetProcAddress((HMODULE)lib, name);
#elif defined (__linux__)
    return dlsym(lib, name);
#endif
}

static PalResult resultFromVk(VkResult result)
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
            return PAL_RESULT_PLATFORM_FAILURE;

        case VK_ERROR_INCOMPATIBLE_DRIVER:
            return PAL_RESULT_INVALID_DRIVER;

        case VK_ERROR_NATIVE_WINDOW_IN_USE_KHR: {
            return PAL_RESULT_INVALID_WINDOW;
        }

        case VK_TIMEOUT:
            return PAL_RESULT_TIMEOUT;

        case VK_ERROR_MEMORY_MAP_FAILED:
            return PAL_RESULT_MEMORY_MAP_FAILED;

        case VK_ERROR_DEVICE_LOST:
            return PAL_RESULT_DEVICE_LOST;

        case VK_ERROR_SURFACE_LOST_KHR:
            return PAL_RESULT_SURFACE_LOST;

        case VK_ERROR_OUT_OF_DATE_KHR:
            return PAL_RESULT_SWAPCHAIN_OUT_OF_DATE;

        default:
            return PAL_RESULT_PLATFORM_FAILURE;
    }
    return PAL_RESULT_PLATFORM_FAILURE;
}

static VkImageUsageFlags imageUsageToVk(PalImageUsages usages)
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

static VkFormat formatToVk(PalFormat format)
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

static PalSampleCount samplesFromVk(VkSampleCountFlags count)
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

static PalFormat formatFromVk(VkFormat format)
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

static PalImageUsages ImageUsageFromVk(VkFormatFeatureFlags flags)
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

static VkImageViewType imageViewTypeToVk(PalImageViewType type)
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

static VkExtent2D getShadingRateSizeVk(PalFragmentShadingRate rate)
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

static VkFormat vertexTypeToVk(PalVertexType type)
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

static VkBufferUsageFlags bufferUsageToVk(PalBufferUsages usages)
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

    if (usages & PAL_BUFFER_USAGE_ACCELERATION_STRUCTURE) {
        flags |= VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT;
        flags |= VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_BUILD_INPUT_READ_ONLY_BIT_KHR;
        flags |= VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_STORAGE_BIT_KHR;
    }

    if (usages & PAL_BUFFER_USAGE_DEVICE_ADDRESS) {
        flags |= VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT;
    }

    return flags;
}

static Uint32 getVertexTypeSizeVk(PalVertexType type)
{
    // count x sizeof type returned as size
    switch (type) {
        case PAL_VERTEX_TYPE_INT8_2:
        case PAL_VERTEX_TYPE_UINT8_2:
        case PAL_VERTEX_TYPE_INT8_2NORM:
        case PAL_VERTEX_TYPE_UINT8_2NORM: {
            return 2;
        }

        case PAL_VERTEX_TYPE_INT32:
        case PAL_VERTEX_TYPE_UINT32:
        case PAL_VERTEX_TYPE_INT8_4:
        case PAL_VERTEX_TYPE_INT8_4NORM:
        case PAL_VERTEX_TYPE_UINT8_4:
        case PAL_VERTEX_TYPE_UINT8_4NORM:
        case PAL_VERTEX_TYPE_INT16_2NORM:
        case PAL_VERTEX_TYPE_INT16_2:
        case PAL_VERTEX_TYPE_UINT16_2:
        case PAL_VERTEX_TYPE_UINT16_2NORM:
        case PAL_VERTEX_TYPE_FLOAT:
        case PAL_VERTEX_TYPE_HALF_FLOAT16_2: {
            return 4;
        }

        case PAL_VERTEX_TYPE_INT32_2:
        case PAL_VERTEX_TYPE_UINT32_2:
        case PAL_VERTEX_TYPE_INT16_4:
        case PAL_VERTEX_TYPE_UINT16_4:
        case PAL_VERTEX_TYPE_UINT16_4NORM:
        case PAL_VERTEX_TYPE_INT16_4NORM:
        case PAL_VERTEX_TYPE_FLOAT2:
        case PAL_VERTEX_TYPE_HALF_FLOAT16_4: {
            return 8;
        }

        case PAL_VERTEX_TYPE_INT32_3:
        case PAL_VERTEX_TYPE_UINT32_3:
        case PAL_VERTEX_TYPE_FLOAT3: {
            return 12;
        }

        case PAL_VERTEX_TYPE_INT32_4:
        case PAL_VERTEX_TYPE_UINT32_4:
        case PAL_VERTEX_TYPE_FLOAT4: {
            return 16;
        }
    }

    return 0;
}

static VkStencilOp stencilOpToVk(PalStencilOp op)
{
    switch (op) {
        case PAL_STENCIL_OP_KEEP:
            return VK_STENCIL_OP_KEEP;

        case PAL_STENCIL_OP_ZERO:
            return VK_STENCIL_OP_ZERO;

        case PAL_STENCIL_OP_REPLACE:
            return VK_STENCIL_OP_REPLACE;

        case PAL_STENCIL_OP_INCREMENT_AND_CLAMP:
            return VK_STENCIL_OP_INCREMENT_AND_CLAMP;

        case PAL_STENCIL_OP_DECREMENT_AND_CLAMP:
            return VK_STENCIL_OP_DECREMENT_AND_CLAMP;

        case PAL_STENCIL_OP_INVERT:
            return VK_STENCIL_OP_INVERT;

        case PAL_STENCIL_OP_INCREMENT_AND_WRAP:
            return VK_STENCIL_OP_INCREMENT_AND_WRAP;

        case PAL_STENCIL_OP_DECREMENT_AND_WRAP:
            return VK_STENCIL_OP_DECREMENT_AND_WRAP;
    }

    return VK_STENCIL_OP_KEEP;
}

static VkCompareOp compareOpToVk(PalCompareOp op)
{
    switch (op) {
        case PAL_COMPARE_OP_NEVER:
            return VK_COMPARE_OP_NEVER;

        case PAL_COMPARE_OP_LESS:
            return VK_COMPARE_OP_LESS;

        case PAL_COMPARE_OP_EQUAL:
            return VK_COMPARE_OP_EQUAL;

        case PAL_COMPARE_OP_LESS_OR_EQUAL:
            return VK_COMPARE_OP_LESS_OR_EQUAL;

        case PAL_COMPARE_OP_GREATER:
            return VK_COMPARE_OP_GREATER;

        case PAL_COMPARE_OP_NOT_EQUAL:
            return VK_COMPARE_OP_NOT_EQUAL;

        case PAL_COMPARE_OP_GREATER_OR_EQUAL:
            return VK_COMPARE_OP_GREATER_OR_EQUAL;

        case PAL_COMPARE_OP_ALWAYS:
            return VK_COMPARE_OP_ALWAYS;
    }

    return VK_COMPARE_OP_NEVER;
}

static VkBlendOp blendOpToVk(PalBlendOp op)
{
    switch (op) {
        case PAL_BLEND_OP_ADD:
            return VK_BLEND_OP_ADD;

        case PAL_BLEND_OP_SUBTRACT:
            return VK_BLEND_OP_SUBTRACT;

        case PAL_BLEND_OP_REVERSE_SUBTRACT:
            return VK_BLEND_OP_REVERSE_SUBTRACT;

        case PAL_BLEND_OP_MIN:
            return VK_BLEND_OP_MIN;

        case PAL_BLEND_OP_MAX:
            return VK_BLEND_OP_MAX;
    }

    return VK_BLEND_OP_ADD;
}

static VkBlendFactor blendFactorToVk(PalBlendFactor op)
{
    switch (op) {
        case PAL_BLEND_FACTOR_ZERO:
            return VK_BLEND_FACTOR_ZERO;

        case PAL_BLEND_FACTOR_ONE:
            return VK_BLEND_FACTOR_ONE;

        case PAL_BLEND_FACTOR_SRC_COLOR:
            return VK_BLEND_FACTOR_SRC_COLOR;

        case PAL_BLEND_FACTOR_ONE_MINUS_SRC_COLOR:
            return VK_BLEND_FACTOR_ONE_MINUS_SRC_COLOR;

        case PAL_BLEND_FACTOR_DST_COLOR:
            return VK_BLEND_FACTOR_DST_COLOR;

        case PAL_BLEND_FACTOR_ONE_MINUX_DST_COLOR:
            return VK_BLEND_FACTOR_ONE_MINUS_DST_COLOR;

        case PAL_BLEND_FACTOR_SRC_ALPHA:
            return VK_BLEND_FACTOR_SRC_ALPHA;

        case PAL_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA:
            return VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;

        case PAL_BLEND_FACTOR_DST_ALPHA:
            return VK_BLEND_FACTOR_DST_ALPHA;

        case PAL_BLEND_FACTOR_ONE_MINUS_DST_ALPHA:
            return VK_BLEND_FACTOR_ONE_MINUS_DST_ALPHA;

        case PAL_BLEND_FACTOR_CONSTANT_COLOR:
            return VK_BLEND_FACTOR_CONSTANT_COLOR;

        case PAL_BLEND_FACTOR_ONE_MINUS_CONSTANT_COLOR:
            return VK_BLEND_FACTOR_ONE_MINUS_CONSTANT_COLOR;

        case PAL_BLEND_FACTOR_CONSTANT_ALPHA:
            return VK_BLEND_FACTOR_CONSTANT_ALPHA;

        case PAL_BLEND_FACTOR_ONE_MINUS_CONSTANT_ALPHA:
            return VK_BLEND_FACTOR_ONE_MINUS_CONSTANT_ALPHA;
    }

    return VK_BLEND_FACTOR_ZERO;
}

static VkFragmentShadingRateCombinerOpKHR combinerOpsToVk(PalFragmentShadingRateCombinerOp op)
{
    switch (op) {
        case PAL_FRAGMENT_SHADING_RATE_COMBINER_OP_KEEP:
            return VK_FRAGMENT_SHADING_RATE_COMBINER_OP_KEEP_KHR;

        case PAL_FRAGMENT_SHADING_RATE_COMBINER_OP_REPLACE:
            return VK_FRAGMENT_SHADING_RATE_COMBINER_OP_REPLACE_KHR;

        case PAL_FRAGMENT_SHADING_RATE_COMBINER_OP_MIN:
            return VK_FRAGMENT_SHADING_RATE_COMBINER_OP_MIN_KHR;

        case PAL_FRAGMENT_SHADING_RATE_COMBINER_OP_MAX:
            return VK_FRAGMENT_SHADING_RATE_COMBINER_OP_MAX_KHR;

        case PAL_FRAGMENT_SHADING_RATE_COMBINER_OP_MUL:
            return VK_FRAGMENT_SHADING_RATE_COMBINER_OP_MUL_KHR;
    }

    return VK_FRAGMENT_SHADING_RATE_COMBINER_OP_KEEP_KHR;
}

static VkResolveModeFlags resolveModeToVk(PalResolveMode mode)
{
    switch (mode) {
        case PAL_RESOLVE_MODE_SAMPLE_ZERO:
            return VK_RESOLVE_MODE_SAMPLE_ZERO_BIT_KHR;

        case PAL_RESOLVE_MODE_AVERAGE:
            return VK_RESOLVE_MODE_SAMPLE_ZERO_BIT_KHR;

        case PAL_RESOLVE_MODE_MIN:
            return VK_RESOLVE_MODE_SAMPLE_ZERO_BIT_KHR;

        case PAL_RESOLVE_MODE_MAX:
            return VK_RESOLVE_MODE_SAMPLE_ZERO_BIT_KHR;
    }

    return VK_RESOLVE_MODE_NONE_KHR;
}

static VkPipelineStageFlags2 pipelineStageToVk(PalShaderStage stage)
{
    switch (stage) {
        case PAL_SHADER_STAGE_VERTEX:
            return VK_PIPELINE_STAGE_2_VERTEX_SHADER_BIT;

        case PAL_SHADER_STAGE_FRAGMENT:
            return VK_PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT;

        case PAL_SHADER_STAGE_COMPUTE:
            return VK_PIPELINE_STAGE_2_COMPUTE_SHADER_BIT;

        case PAL_SHADER_STAGE_GEOMETRY:
            return VK_PIPELINE_STAGE_2_GEOMETRY_SHADER_BIT;

        case PAL_SHADER_STAGE_MESH:
            return VK_PIPELINE_STAGE_2_MESH_SHADER_BIT_EXT;

        case PAL_SHADER_STAGE_TASK:
            return VK_PIPELINE_STAGE_2_TASK_SHADER_BIT_EXT;

        case PAL_SHADER_STAGE_TESSELLATION_CONTROL:
            return VK_PIPELINE_STAGE_2_TESSELLATION_CONTROL_SHADER_BIT;

        case PAL_SHADER_STAGE_TESSELLATION_EVALUATION:
            return VK_PIPELINE_STAGE_2_TESSELLATION_EVALUATION_SHADER_BIT;

        case PAL_SHADER_STAGE_RAYGEN:
        case PAL_SHADER_STAGE_CLOSEST_HIT:
        case PAL_SHADER_STAGE_ANY_HIT:
        case PAL_SHADER_STAGE_MISS:
        case PAL_SHADER_STAGE_INTERSECTION:
        case PAL_SHADER_STAGE_CALLABLE: {
            return VK_PIPELINE_STAGE_2_RAY_TRACING_SHADER_BIT_KHR;
        }
    }
    return 0;
}
     
static VkFilter filterToVk(PalFilterMode mode)
{
    switch (mode) {
        case PAL_FILTER_MODE_NEAREST: {
            return VK_FILTER_NEAREST;
        }

        case PAL_FILTER_MODE_LINEAR: {
            return VK_FILTER_LINEAR;
        }
    }
    return VK_FILTER_NEAREST;
}

static VkSamplerMipmapMode mipmapModeToVk(PalSamplerMipmapMode mode)
{
    switch (mode) {
        case PAL_SAMPLER_MIPMAP_MODE_NEAREST: {
            return VK_SAMPLER_MIPMAP_MODE_NEAREST;
        }

        case PAL_SAMPLER_MIPMAP_MODE_LINEAR: {
            return VK_SAMPLER_MIPMAP_MODE_LINEAR;
        }
    }
    return VK_SAMPLER_MIPMAP_MODE_NEAREST;
}

static VkSamplerAddressMode addressModeToVk(PalSamplerAddressMode mode)
{
    switch (mode) {
        case PAL_SAMPLER_ADDRESS_MODE_REPEAT: {
            return VK_SAMPLER_ADDRESS_MODE_REPEAT;
        }

        case PAL_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT: {
            return VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT;

        }
        case PAL_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE: {
            return VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;

        }
        case PAL_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER: {
            return VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
        }
    }
    return VK_SAMPLER_ADDRESS_MODE_REPEAT;
}

static VkBorderColor borderColorToVk(PalBorderColor color)
{
    switch (color) {
        case PAL_BORDER_COLOR_FLOAT_TRANSPARENT_BLACK: {
            return VK_BORDER_COLOR_FLOAT_TRANSPARENT_BLACK;
        }

        case PAL_BORDER_COLOR_INT_TRANSPARENT_BLACK: {
            return VK_BORDER_COLOR_INT_TRANSPARENT_BLACK;
        }

        case PAL_BORDER_COLOR_FLOAT_OPAQUE_BLACK: {
            return VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK;
        }

        case PAL_BORDER_COLOR_INT_OPAQUE_BLACK: {
            return VK_BORDER_COLOR_INT_OPAQUE_BLACK;
        }

        case PAL_BORDER_COLOR_FLOAT_OPAQUE_WHITE: {
            return VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;
        }

        case PAL_BORDER_COLOR_INT_OPAQUE_WHITE: {
            return VK_BORDER_COLOR_INT_OPAQUE_WHITE;
        }
    }
    return VK_BORDER_COLOR_FLOAT_TRANSPARENT_BLACK;
}

static Barrier barrierToVk(
    Uint32 stageCount,
    PalUsageState state,
    PalShaderStage* shaderStages)
{
    Barrier barrier = {0};
    switch (state) {
        case PAL_USAGE_STATE_UNDEFINED: {
            barrier.stages = VK_PIPELINE_STAGE_2_TOP_OF_PIPE_BIT_KHR;
            barrier.access = 0;
            barrier.layout = VK_IMAGE_LAYOUT_UNDEFINED;
            return barrier;
        }

        case PAL_USAGE_STATE_PRESENT: {
            barrier.stages = VK_PIPELINE_STAGE_2_BOTTOM_OF_PIPE_BIT_KHR;
            barrier.access = 0;
            barrier.layout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
            return barrier;
        }

        case PAL_USAGE_STATE_COLOR_ATTACHMENT_WRITE: {
            barrier.stages = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT_KHR;
            barrier.access = VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT_KHR;
            barrier.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
            return barrier;
        }

        case PAL_USAGE_STATE_DEPTH_ATTACHMENT_READ: {
            barrier.stages = VK_PIPELINE_STAGE_2_EARLY_FRAGMENT_TESTS_BIT_KHR;
            barrier.stages |= VK_PIPELINE_STAGE_2_LATE_FRAGMENT_TESTS_BIT_KHR;
            barrier.access = VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_READ_BIT_KHR;
            barrier.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
            return barrier;
        }

        case PAL_USAGE_STATE_DEPTH_ATTACHMENT_WRITE: {
            barrier.stages = VK_PIPELINE_STAGE_2_EARLY_FRAGMENT_TESTS_BIT_KHR;
            barrier.stages |= VK_PIPELINE_STAGE_2_LATE_FRAGMENT_TESTS_BIT_KHR;
            barrier.access = VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT_KHR;
            barrier.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
            return barrier;
        }

        case PAL_USAGE_STATE_STENCIL_ATTACHMENT_READ: {
            barrier.stages = VK_PIPELINE_STAGE_2_EARLY_FRAGMENT_TESTS_BIT_KHR;
            barrier.stages |= VK_PIPELINE_STAGE_2_LATE_FRAGMENT_TESTS_BIT_KHR;
            barrier.access = VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_READ_BIT_KHR;
            barrier.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
            return barrier;
        }

        case PAL_USAGE_STATE_STENCIL_ATTACHMENT_WRITE: {
            barrier.stages = VK_PIPELINE_STAGE_2_EARLY_FRAGMENT_TESTS_BIT_KHR;
            barrier.stages |= VK_PIPELINE_STAGE_2_LATE_FRAGMENT_TESTS_BIT_KHR;
            barrier.access = VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT_KHR;
            barrier.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
            return barrier;
        }

        case PAL_USAGE_STATE_FRAGMENT_SHADING_RATE_ATTACHMENT_READ: {
            barrier.stages = VK_PIPELINE_STAGE_2_FRAGMENT_SHADING_RATE_ATTACHMENT_BIT_KHR;
            barrier.access = VK_ACCESS_2_FRAGMENT_SHADING_RATE_ATTACHMENT_READ_BIT_KHR;
            barrier.layout = VK_IMAGE_LAYOUT_FRAGMENT_SHADING_RATE_ATTACHMENT_OPTIMAL_KHR;
            return barrier;
        }

        case PAL_USAGE_STATE_TRANSFER_READ: {
            barrier.stages = VK_PIPELINE_STAGE_2_TRANSFER_BIT_KHR;
            barrier.access = VK_ACCESS_2_TRANSFER_READ_BIT_KHR;
            barrier.layout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
            return barrier;
        }

        case PAL_USAGE_STATE_TRANSFER_WRITE: {
            barrier.stages = VK_PIPELINE_STAGE_2_TRANSFER_BIT_KHR;
            barrier.access = VK_ACCESS_2_TRANSFER_WRITE_BIT_KHR;
            barrier.layout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
            return barrier;
        }

        case PAL_USAGE_STATE_VERTEX_READ: {
            barrier.stages = VK_PIPELINE_STAGE_2_VERTEX_INPUT_BIT_KHR;
            barrier.access = VK_ACCESS_2_VERTEX_ATTRIBUTE_READ_BIT_KHR;
            barrier.layout = VK_IMAGE_LAYOUT_UNDEFINED;
            return barrier;
        }

        case PAL_USAGE_STATE_INDEX_READ: {
            barrier.stages = VK_PIPELINE_STAGE_2_INDEX_INPUT_BIT_KHR;
            barrier.access = VK_ACCESS_2_INDEX_READ_BIT_KHR;
            barrier.layout = VK_IMAGE_LAYOUT_UNDEFINED;
            return barrier;
        }

        case PAL_USAGE_STATE_UNIFORM_READ: {
            for (int i = 0; i < stageCount; i++) {
                barrier.stages |= pipelineStageToVk(shaderStages[i]);
            }
            
            barrier.access = VK_ACCESS_2_UNIFORM_READ_BIT_KHR;
            barrier.layout = VK_IMAGE_LAYOUT_UNDEFINED;
            return barrier;
        }

        case PAL_USAGE_STATE_SHADER_READ: {
            for (int i = 0; i < stageCount; i++) {
                barrier.stages |= pipelineStageToVk(shaderStages[i]);
            }

            barrier.access = VK_ACCESS_2_SHADER_READ_BIT_KHR;
            barrier.layout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            return barrier;
        }

        case PAL_USAGE_STATE_SHADER_WRITE: {
            for (int i = 0; i < stageCount; i++) {
                barrier.stages |= pipelineStageToVk(shaderStages[i]);
            }

            barrier.access = VK_ACCESS_2_SHADER_WRITE_BIT_KHR;
            barrier.layout = VK_IMAGE_LAYOUT_GENERAL;
            return barrier;
        }

        case PAL_USAGE_STATE_STORAGE_READ: {
            for (int i = 0; i < stageCount; i++) {
                barrier.stages |= pipelineStageToVk(shaderStages[i]);
            }

            barrier.access = VK_ACCESS_2_SHADER_READ_BIT_KHR;
            barrier.layout = VK_IMAGE_LAYOUT_GENERAL;
            return barrier;
        }

        case PAL_USAGE_STATE_STORAGE_WRITE: {
            for (int i = 0; i < stageCount; i++) {
                barrier.stages |= pipelineStageToVk(shaderStages[i]);
            }

            barrier.access = VK_ACCESS_2_SHADER_WRITE_BIT_KHR;
            barrier.layout = VK_IMAGE_LAYOUT_GENERAL;
            return barrier;
        }

        case PAL_USAGE_STATE_HOST_READ: {
            barrier.stages = VK_PIPELINE_STAGE_2_HOST_BIT_KHR;
            barrier.access = VK_ACCESS_2_HOST_READ_BIT_KHR;
            barrier.layout = VK_IMAGE_LAYOUT_UNDEFINED;
            return barrier;
        }

        case PAL_USAGE_STATE_HOST_WRITE: {
            barrier.stages = VK_PIPELINE_STAGE_2_HOST_BIT_KHR;
            barrier.access = VK_ACCESS_2_HOST_WRITE_BIT_KHR;
            barrier.layout = VK_IMAGE_LAYOUT_UNDEFINED;
            return barrier;
        }

        case PAL_USAGE_STATE_ACCELERATION_STRUCTURE_READ: {
            barrier.stages = VK_PIPELINE_STAGE_2_ACCELERATION_STRUCTURE_BUILD_BIT_KHR;
            barrier.access = VK_ACCESS_2_ACCELERATION_STRUCTURE_READ_BIT_KHR;

            // HACK: small performance lost for the case of a single scratch buffer used for
            // BLAS and TLAS builds.
            barrier.access |= VK_ACCESS_2_ACCELERATION_STRUCTURE_WRITE_BIT_KHR;

            for (int i = 0; i < stageCount; i++) {
                if (i == 0) {
                    barrier.stages = 0;
                }

                barrier.stages |= pipelineStageToVk(shaderStages[i]);
                barrier.access = VK_ACCESS_2_ACCELERATION_STRUCTURE_READ_BIT_KHR;
            }

            barrier.layout = VK_IMAGE_LAYOUT_UNDEFINED;
            return barrier;
        }

        case PAL_USAGE_STATE_ACCELERATION_STRUCTURE_WRITE: {
            barrier.stages = VK_PIPELINE_STAGE_2_ACCELERATION_STRUCTURE_BUILD_BIT_KHR;
            barrier.access = VK_ACCESS_2_ACCELERATION_STRUCTURE_WRITE_BIT_KHR;
            barrier.layout = VK_IMAGE_LAYOUT_UNDEFINED;
            return barrier;
        }
    }

    barrier.stages = VK_PIPELINE_STAGE_2_TOP_OF_PIPE_BIT_KHR;
    barrier.access = 0;
    barrier.layout = VK_IMAGE_LAYOUT_UNDEFINED;
    return barrier;
}

static VkDescriptorType descriptortypeToVk(PalDescriptorType type)
{
    switch (type) {
        case PAL_DESCRIPTOR_TYPE_STORAGE_BUFFER:
            return VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;

        case PAL_DESCRIPTOR_TYPE_UNIFORM_BUFFER:
            return VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;

        case PAL_DESCRIPTOR_TYPE_SAMPLED_IMAGE:
            return VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;

        case PAL_DESCRIPTOR_TYPE_STORAGE_IMAGE:
            return VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;

        case PAL_DESCRIPTOR_TYPE_SAMPLER:
            return VK_DESCRIPTOR_TYPE_SAMPLER;

        case PAL_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE:
            return VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR;
    }

    return 0;
}

static VkShaderStageFlags shaderStageToVK(PalShaderStage stage)
{
    switch (stage) {
        case PAL_SHADER_STAGE_VERTEX:
            return VK_SHADER_STAGE_VERTEX_BIT;

        case PAL_SHADER_STAGE_FRAGMENT:
            return VK_SHADER_STAGE_FRAGMENT_BIT;

        case PAL_SHADER_STAGE_COMPUTE:
            return VK_SHADER_STAGE_COMPUTE_BIT;

        case PAL_SHADER_STAGE_GEOMETRY:
            return VK_SHADER_STAGE_GEOMETRY_BIT;

        case PAL_SHADER_STAGE_MESH:
            return VK_SHADER_STAGE_MESH_BIT_EXT;

        case PAL_SHADER_STAGE_TASK:
            return VK_SHADER_STAGE_TASK_BIT_EXT;

        case PAL_SHADER_STAGE_TESSELLATION_CONTROL:
            return VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT;

        case PAL_SHADER_STAGE_TESSELLATION_EVALUATION:
            return VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT;

        case PAL_SHADER_STAGE_RAYGEN:
            return VK_SHADER_STAGE_RAYGEN_BIT_KHR;

        case PAL_SHADER_STAGE_CLOSEST_HIT:
            return VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR;

        case PAL_SHADER_STAGE_ANY_HIT:
            return VK_SHADER_STAGE_ANY_HIT_BIT_KHR;

        case PAL_SHADER_STAGE_MISS:
            return VK_SHADER_STAGE_MISS_BIT_KHR;

        case PAL_SHADER_STAGE_INTERSECTION:
            return VK_SHADER_STAGE_INTERSECTION_BIT_KHR;

        case PAL_SHADER_STAGE_CALLABLE:
            return VK_SHADER_STAGE_CALLABLE_BIT_KHR;
    }

    return 0;
}

static inline void* alignedRealloc(
    void* memory,
    Uint64 size,
    Uint64 alignment)
{
#if defined(_MSC_VER) || defined(__MINGW32__)
    return _aligned_realloc(memory, size, alignment);
#else
    return realloc(memory, size);
#endif // _MSC_VER
}

static inline void alignedFree(void* ptr)
{
#if defined(_MSC_VER) || defined(__MINGW32__)
    _aligned_free(ptr);
#else
    free(ptr);
#endif // _MSC_VER
}

static void* VKAPI_CALL allocateVk(
    void* pUserData,
    size_t size,
    size_t alignment,
    VkSystemAllocationScope allocationScope)
{
    return palAllocate(s_Vk.allocator, size, alignment);
}

static void VKAPI_CALL freeVk(
    void* pUserData,
    void* ptr)
{
    palFree(s_Vk.allocator, ptr);
}

static void* VKAPI_CALL reallocVk(
    void* pUserData,
    void* pOriginal,
    size_t size,
    size_t alignment,
    VkSystemAllocationScope allocationScope)
{
    // Note: This is a hack which could cost performance but
    // realloc is not really called that much so it should be fine
    // this is because we dont know the old size
    void* block = alignedRealloc(pOriginal, size, alignment);
    if (block) {
        void* memory = palAllocate(s_Vk.allocator, size, alignment);
        if (!memory) {
            alignedFree(block);
            return nullptr;
        }

        memcpy(memory, block, size);
        alignedFree(block);
        return memory;
    }
    return nullptr;
}

VkBool32 VKAPI_CALL debugCallbackVk(
    VkDebugUtilsMessageSeverityFlagBitsEXT severity,
    VkDebugUtilsMessageTypeFlagBitsEXT type,
    const VkDebugUtilsMessengerCallbackDataEXT* data,
    void* userData)
{
    if (!s_Vk.callback) {
        return VK_FALSE;
    }

    PalDebugMessageSeverity debugSeverity = 0;
    PalDebugMessageType debugType = 0;
    if (type & VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT) {
        debugType = PAL_DEBUG_MESSAGE_TYPE_GENERAL;
    }

    if (type & VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT) {
        debugType = PAL_DEBUG_MESSAGE_TYPE_PERFORMANCE;
    }

    if (type & VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT) {
        debugType = PAL_DEBUG_MESSAGE_TYPE_VALIDATION;
    }

    if (type & VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT) {
        debugSeverity = PAL_DEBUG_MESSAGE_SEVERITY_INFO;
    }

    if (type & VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) {
        debugSeverity = PAL_DEBUG_MESSAGE_SEVERITY_WARNING;
    }

    if (type & VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT) {
        debugSeverity = PAL_DEBUG_MESSAGE_SEVERITY_ERROR;
    }

    s_Vk.callback(userData, debugSeverity, debugType, data->pMessage);
    return VK_FALSE;
}

static Uint32 findBestMemoryIndexVk(
    VkPhysicalDevice phyDevice,
    Uint32 memoryMask)
{
    int bestScore = -1;
    Uint32 bestIndex = UINT32_MAX;
    VkPhysicalDeviceMemoryProperties memProps = {0};
    s_Vk.getPhysicalDeviceMemoryProperties(phyDevice, &memProps);

    for (int i = 0; i < memProps.memoryTypeCount; i++) {
        if (!(memoryMask & (1u << i))) {
            continue;
        }

        int score = 0;
        VkMemoryPropertyFlags flags = memProps.memoryTypes[i].propertyFlags;

        // GPU only memory
        if (flags & VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT) {
            score += 100;
        }

        // CPU memory
        if (flags & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT) {
            score += 50;
        }

        if (flags & VK_MEMORY_PROPERTY_HOST_COHERENT_BIT) {
            score += 25;
        }

        if (flags & VK_MEMORY_PROPERTY_HOST_CACHED_BIT) {
            score += 10;
        }

        if (score > bestScore) {
            bestIndex = i;
        }
    }

    return bestIndex;
}

static inline Uint32 alignVk(
    Uint32 value,
    Uint32 alignment)
{
    return (value + alignment - 1) & ~(alignment - 1);
}

static inline Uint32 minVk(
    Uint32 a,
    Uint32 b)
{
    return (a < b) ? a : b;
}

static void fillBuildInfoVk(
    Uint32 count,
    PalAccelerationStructureBuildInfo* info,
    Uint32* maxPrimities,
    VkAccelerationStructureGeometryKHR* geometries,
    VkAccelerationStructureKHR srcAs,
    VkAccelerationStructureKHR dstAs,
    VkAccelerationStructureBuildRangeInfoKHR* rangeInfos,
    VkAccelerationStructureBuildGeometryInfoKHR* buildInfo)
{
    for (int i = 0; i < count; i++) {
        // fill vulkan geometry struct
        VkAccelerationStructureGeometryKHR* tmp = &geometries[i];
        tmp->sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_KHR;
        tmp->flags = 0;

        if (info->type == PAL_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL) {
            tmp->geometryType = VK_GEOMETRY_TYPE_INSTANCES_KHR;
            VkAccelerationStructureGeometryInstancesDataKHR* data = nullptr;
            data = &tmp->geometry.instances;
            data->sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_INSTANCES_DATA_KHR;
            data->arrayOfPointers = false;

            VkDeviceOrHostAddressConstKHR address = {0};
            address.deviceAddress = info->instanceBufferAddress;
            data->data = address;

            if (maxPrimities) {
                maxPrimities[i] = info->instanceCount;
            }

            // range info
            if (rangeInfos) {
                VkAccelerationStructureBuildRangeInfoKHR* rangeInfo = &rangeInfos[i];
                rangeInfo->primitiveCount = info->instanceCount;
                rangeInfo->firstVertex = 0;     // PAL does not allow setting this
                rangeInfo->primitiveOffset = 0; // PAL does not allow setting this
                rangeInfo->transformOffset = 0; // PAL does not allow setting this
            }
            break;

        } else {
            if (info->geometries[i].flags & PAL_GEOMETRY_FLAG_OPAQUE) {
                tmp->flags |= VK_GEOMETRY_OPAQUE_BIT_KHR;
            }

            if (info->geometries[i].flags & PAL_GEOMETRY_FLAG_NO_DUPLICATE_ANYHIT) {
                tmp->flags |= VK_GEOMETRY_OPAQUE_BIT_KHR;
            }

            if (maxPrimities) {
                maxPrimities[i] = info->geometries[i].primitiveCount;
            }

            // range info
            if (rangeInfos) {
                VkAccelerationStructureBuildRangeInfoKHR* rangeInfo = &rangeInfos[i];
                rangeInfo->primitiveCount = info->geometries[i].primitiveCount;
                rangeInfo->firstVertex = 0;     // PAL does not allow setting this
                rangeInfo->primitiveOffset = 0; // PAL does not allow setting this
                rangeInfo->transformOffset = 0; // PAL does not allow setting this
            }
        }

        if (info->geometries[i].type == PAL_GEOMETRY_TYPE_TRIANGLE) {
            tmp->geometryType = VK_GEOMETRY_TYPE_TRIANGLES_KHR;
            VkAccelerationStructureGeometryTrianglesDataKHR* data = &tmp->geometry.triangles;
            data->sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_TRIANGLES_DATA_KHR;

            VkDeviceOrHostAddressConstKHR vertexAddress = {0};
            VkDeviceOrHostAddressConstKHR indexAddress = {0};
            PalGeometryDataTriangle* tmpData = info->geometries[i].data;

            vertexAddress.deviceAddress = tmpData->vertexBufferAddress;
            data->vertexData = vertexAddress;
            data->maxVertex = tmpData->vertexCount - 1;
            data->vertexFormat = vertexTypeToVk(tmpData->vertexType);
            data->vertexStride = tmpData->vertexStride;

            indexAddress.deviceAddress = tmpData->indexBufferAddress;
            data->indexData = indexAddress;
            if (tmpData->indexType == PAL_INDEX_TYPE_UINT32) {
                data->indexType = VK_INDEX_TYPE_UINT32;
            } else {
                data->indexType = VK_INDEX_TYPE_UINT16;
            }

            // set to none if there is no index buffer address
            if (!tmpData->indexBufferAddress) {
                data->indexType = VK_INDEX_TYPE_NONE_KHR;
            }

        } else if (info->geometries[i].type == PAL_GEOMETRY_TYPE_AABBS) {
            tmp->geometryType = VK_GEOMETRY_TYPE_AABBS_KHR;
            VkAccelerationStructureGeometryAabbsDataKHR* data = &tmp->geometry.aabbs;
            data->sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_AABBS_DATA_KHR;

            VkDeviceOrHostAddressConstKHR address = {0};
            PalGeometryDataAABBS* tmpData = info->geometries[i].data;
            address.deviceAddress = tmpData->bufferAddress;
            data->data = address;
            data->stride = tmpData->stride;
        }
    }

    buildInfo->sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_GEOMETRY_INFO_KHR;
    if (info->type == PAL_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL) {
        buildInfo->type = VK_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL_KHR;
    } else {
        buildInfo->type = VK_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL_KHR;
    }

    // build mode
    if (info->buildMode == PAL_ACCELERATION_STRUCTURE_BUILD_MODE_BUILD) {
        buildInfo->mode = VK_BUILD_ACCELERATION_STRUCTURE_MODE_BUILD_KHR;
    } else {
        buildInfo->mode = VK_BUILD_ACCELERATION_STRUCTURE_MODE_UPDATE_KHR;
    }

    // build hints
    buildInfo->flags = 0;
    if (info->buildHints & PAL_ACCELERATION_STRUCTURE_BUILD_HINT_FAST_BUILD) {
        buildInfo->flags |= VK_BUILD_ACCELERATION_STRUCTURE_PREFER_FAST_BUILD_BIT_KHR;
    }

    if (info->buildHints & PAL_ACCELERATION_STRUCTURE_BUILD_HINT_FAST_TRACE) {
        buildInfo->flags |= VK_BUILD_ACCELERATION_STRUCTURE_PREFER_FAST_TRACE_BIT_KHR;
    }

    if (info->buildHints & PAL_ACCELERATION_STRUCTURE_BUILD_HINT_LOW_MEMORY) {
        buildInfo->flags |= VK_BUILD_ACCELERATION_STRUCTURE_LOW_MEMORY_BIT_KHR;
    }

    buildInfo->geometryCount = count;
    buildInfo->srcAccelerationStructure = srcAs;
    buildInfo->dstAccelerationStructure = dstAs;
    buildInfo->pGeometries = geometries;

    VkDeviceOrHostAddressKHR scratchData = {0};
    scratchData.deviceAddress = info->scratchBufferAddress;
    buildInfo->scratchData = scratchData;
}

static Uint32 getFormatSizeVk(PalFormat format)
{
    switch (format) {
        case PAL_FORMAT_R8_UNORM:
        case PAL_FORMAT_R8_SNORM:
        case PAL_FORMAT_R8_UINT:
        case PAL_FORMAT_R8_SINT:
        case PAL_FORMAT_R8_SRGB:
        case PAL_FORMAT_S8_UINT:
            return 1;

        case PAL_FORMAT_R16_UNORM:
        case PAL_FORMAT_R16_SNORM:
        case PAL_FORMAT_R16_UINT:
        case PAL_FORMAT_R16_SINT:
        case PAL_FORMAT_R16_SFLOAT:
        case PAL_FORMAT_R8G8_UNORM:
        case PAL_FORMAT_R8G8_SNORM:
        case PAL_FORMAT_R8G8_UINT:
        case PAL_FORMAT_R8G8_SINT:
        case PAL_FORMAT_R8G8_SRGB:
        case PAL_FORMAT_D16_UNORM:
            return 2;

        case PAL_FORMAT_R8G8B8_UNORM:
        case PAL_FORMAT_R8G8B8_SNORM:
        case PAL_FORMAT_R8G8B8_UINT:
        case PAL_FORMAT_R8G8B8_SINT:
        case PAL_FORMAT_R8G8B8_SRGB:
        case PAL_FORMAT_B8G8R8_UNORM:
        case PAL_FORMAT_B8G8R8_SNORM:
        case PAL_FORMAT_B8G8R8_UINT:
        case PAL_FORMAT_B8G8R8_SINT:
        case PAL_FORMAT_B8G8R8_SRGB:
        case PAL_FORMAT_D16_UNORM_S8_UINT:
            return 3;

        case PAL_FORMAT_R32_UINT:
        case PAL_FORMAT_R32_SINT:
        case PAL_FORMAT_R32_SFLOAT:
        case PAL_FORMAT_R16G16_UNORM:
        case PAL_FORMAT_R16G16_SNORM:
        case PAL_FORMAT_R16G16_UINT:
        case PAL_FORMAT_R16G16_SINT:
        case PAL_FORMAT_R16G16_SFLOAT:
        case PAL_FORMAT_R8G8B8A8_UNORM:
        case PAL_FORMAT_R8G8B8A8_SNORM:
        case PAL_FORMAT_R8G8B8A8_UINT:
        case PAL_FORMAT_R8G8B8A8_SINT:
        case PAL_FORMAT_R8G8B8A8_SRGB:
        case PAL_FORMAT_B8G8R8A8_UNORM:
        case PAL_FORMAT_B8G8R8A8_SNORM:
        case PAL_FORMAT_B8G8R8A8_UINT:
        case PAL_FORMAT_B8G8R8A8_SINT:
        case PAL_FORMAT_B8G8R8A8_SRGB:
        case PAL_FORMAT_D32_SFLOAT:
        case PAL_FORMAT_D24_UNORM_S8_UINT:
            return 4;

        case PAL_FORMAT_D32_SFLOAT_S8_UINT:
            return 5;

        case PAL_FORMAT_R16G16B16_UNORM:
        case PAL_FORMAT_R16G16B16_SNORM:
        case PAL_FORMAT_R16G16B16_UINT:
        case PAL_FORMAT_R16G16B16_SINT:
        case PAL_FORMAT_R16G16B16_SFLOAT:
            return 6;

        case PAL_FORMAT_R64_UINT:
        case PAL_FORMAT_R64_SINT:
        case PAL_FORMAT_R64_SFLOAT:
        case PAL_FORMAT_R32G32_UINT:
        case PAL_FORMAT_R32G32_SINT:
        case PAL_FORMAT_R32G32_SFLOAT:
        case PAL_FORMAT_R16G16B16A16_UNORM:
        case PAL_FORMAT_R16G16B16A16_SNORM:
        case PAL_FORMAT_R16G16B16A16_UINT:
        case PAL_FORMAT_R16G16B16A16_SINT:
        case PAL_FORMAT_R16G16B16A16_SFLOAT:
            return 8;

        case PAL_FORMAT_R32G32B32_UINT:
        case PAL_FORMAT_R32G32B32_SINT:
        case PAL_FORMAT_R32G32B32_SFLOAT:
            return 12;

        case PAL_FORMAT_R64G64_UINT:
        case PAL_FORMAT_R64G64_SINT:
        case PAL_FORMAT_R64G64_SFLOAT:
        case PAL_FORMAT_R32G32B32A32_UINT:
        case PAL_FORMAT_R32G32B32A32_SINT:
        case PAL_FORMAT_R32G32B32A32_SFLOAT:
            return 16;

        case PAL_FORMAT_R64G64B64_UINT:
        case PAL_FORMAT_R64G64B64_SINT:
        case PAL_FORMAT_R64G64B64_SFLOAT:
            return 24;

        case PAL_FORMAT_R64G64B64A64_UINT:
        case PAL_FORMAT_R64G64B64A64_SINT:
        case PAL_FORMAT_R64G64B64A64_SFLOAT:
            return 32;
    }

    return 0;
}

static VkImageAspectFlags imageAspectToVk(PalImageAspect aspect)
{
    switch (aspect) {
        case PAL_IMAGE_ASPECT_COLOR:
            return VK_IMAGE_ASPECT_COLOR_BIT;

        case PAL_IMAGE_ASPECT_DEPTH:
            return VK_IMAGE_ASPECT_DEPTH_BIT;

        case PAL_IMAGE_ASPECT_STENCIL:
            return VK_IMAGE_ASPECT_STENCIL_BIT;

        case PAL_IMAGE_ASPECT_DEPTH_STENCIL:
            return VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT;
    }

    return VK_IMAGE_ASPECT_COLOR_BIT;
}

static VkGeometryInstanceFlagsKHR instanceFlagsToVk(PalAccelerationStructureInstanceFlags flags)
{
    VkGeometryInstanceFlagsKHR instanceFlags = 0;
    if (flags & PAL_ACCELERATION_STRUCTURE_INSTANCE_FLAG_FORCE_OPAQUE) {
        instanceFlags |= VK_GEOMETRY_INSTANCE_FORCE_OPAQUE_BIT_KHR;
    }

    if (flags & PAL_ACCELERATION_STRUCTURE_INSTANCE_FLAG_FORCE_NO_OPAQUE) {
        instanceFlags |= VK_GEOMETRY_INSTANCE_FORCE_NO_OPAQUE_BIT_KHR;
    }

    if (flags & PAL_ACCELERATION_STRUCTURE_INSTANCE_FLAG_TRIANGLE_FACING_CULL_DISABLE) {
        instanceFlags |= VK_GEOMETRY_INSTANCE_TRIANGLE_FACING_CULL_DISABLE_BIT_KHR;
    }

    if (flags & PAL_ACCELERATION_STRUCTURE_INSTANCE_FLAG_TRIANGLE_FRONT_COUNTERCLOCKWISE) {
        instanceFlags |= VK_GEOMETRY_INSTANCE_TRIANGLE_FRONT_COUNTERCLOCKWISE_BIT_KHR;
    }

    return instanceFlags;
}

// ==================================================
// Adapter
// ==================================================

PalResult PAL_CALL initGraphicsVk(
    const PalGraphicsDebugger* debugger,
    const PalAllocator* allocator)
{
    // load vulkan
    s_Vk.handle = loadLibrary(VK_LIB_NAME);
    if (!s_Vk.handle) {
        return PAL_RESULT_PLATFORM_FAILURE;
    }

    // clang-format off
    s_Vk.enumerateInstanceVersion = (PFN_vkEnumerateInstanceVersion)loadProc(
        s_Vk.handle,
        "vkEnumerateInstanceVersion");

    s_Vk.enumerateInstanceExtensionProperties = (PFN_vkEnumerateInstanceExtensionProperties)loadProc(
        s_Vk.handle,
        "vkEnumerateInstanceExtensionProperties");

    s_Vk.createInstance = (PFN_vkCreateInstance)loadProc(
        s_Vk.handle,
        "vkCreateInstance");

    s_Vk.destroyInstance = (PFN_vkDestroyInstance)loadProc(
        s_Vk.handle,
        "vkDestroyInstance");

    s_Vk.enumeratePhysicalDevices = (PFN_vkEnumeratePhysicalDevices)loadProc(
        s_Vk.handle,
        "vkEnumeratePhysicalDevices");

    s_Vk.getPhysicalDeviceProperties = (PFN_vkGetPhysicalDeviceProperties)loadProc(
        s_Vk.handle,
        "vkGetPhysicalDeviceProperties");

    s_Vk.getPhysicalDeviceMemoryProperties = (PFN_vkGetPhysicalDeviceMemoryProperties)loadProc(
        s_Vk.handle,
        "vkGetPhysicalDeviceMemoryProperties");

    s_Vk.enumerateInstanceLayerProperties = (PFN_vkEnumerateInstanceLayerProperties)loadProc(
        s_Vk.handle,
        "vkEnumerateInstanceLayerProperties");

    s_Vk.getPhysicalDeviceQueueFamilyProperties = (PFN_vkGetPhysicalDeviceQueueFamilyProperties)loadProc(
        s_Vk.handle,
        "vkGetPhysicalDeviceQueueFamilyProperties");

    s_Vk.enumerateDeviceExtensionProperties = (PFN_vkEnumerateDeviceExtensionProperties)loadProc(
        s_Vk.handle,
        "vkEnumerateDeviceExtensionProperties");

    s_Vk.getPhysicalDeviceFeatures = (PFN_vkGetPhysicalDeviceFeatures)loadProc(
        s_Vk.handle,
        "vkGetPhysicalDeviceFeatures");

    s_Vk.getPhysicalDeviceFeatures2 = (PFN_vkGetPhysicalDeviceFeatures2)loadProc(
        s_Vk.handle,
        "vkGetPhysicalDeviceFeatures2");

    s_Vk.getInstanceProcAddr = (PFN_vkGetInstanceProcAddr)loadProc(
        s_Vk.handle,
        "vkGetInstanceProcAddr");

    s_Vk.createImage = (PFN_vkCreateImage)loadProc(
        s_Vk.handle,
        "vkCreateImage");

    s_Vk.destroyImage = (PFN_vkDestroyImage)loadProc(
        s_Vk.handle,
        "vkDestroyImage");

    s_Vk.createImageView = (PFN_vkCreateImageView)loadProc(
        s_Vk.handle,
        "vkCreateImageView");

    s_Vk.destroyImageView = (PFN_vkDestroyImageView)loadProc(
        s_Vk.handle,
        "vkDestroyImageView");

    s_Vk.createShader = (PFN_vkCreateShaderModule)loadProc(
        s_Vk.handle,
        "vkCreateShaderModule");

    s_Vk.destroyShader = (PFN_vkDestroyShaderModule)loadProc(
        s_Vk.handle,
        "vkDestroyShaderModule");

    s_Vk.createSampler = (PFN_vkCreateSampler)loadProc(
        s_Vk.handle,
        "vkCreateSampler");

    s_Vk.destroySampler = (PFN_vkDestroySampler)loadProc(
        s_Vk.handle,
        "vkDestroySampler");

    s_Vk.getPhysicalDeviceProperties2 = (PFN_vkGetPhysicalDeviceProperties2)loadProc(
        s_Vk.handle,
        "vkGetPhysicalDeviceProperties2");

    s_Vk.getPhysicalDeviceFormatProperties = (PFN_vkGetPhysicalDeviceFormatProperties)loadProc(
        s_Vk.handle,
        "vkGetPhysicalDeviceFormatProperties");

    s_Vk.getPhysicalDeviceImageFormatProperties = (PFN_vkGetPhysicalDeviceImageFormatProperties)loadProc(
        s_Vk.handle,
        "vkGetPhysicalDeviceImageFormatProperties");

    s_Vk.createDevice = (PFN_vkCreateDevice)loadProc(
        s_Vk.handle,
        "vkCreateDevice");

    s_Vk.destroyDevice = (PFN_vkDestroyDevice)loadProc(
        s_Vk.handle,
        "vkDestroyDevice");

    s_Vk.getDeviceQueue = (PFN_vkGetDeviceQueue)loadProc(
        s_Vk.handle,
        "vkGetDeviceQueue");

    s_Vk.queueSubmit = (PFN_vkQueueSubmit)loadProc(
        s_Vk.handle,
        "vkQueueSubmit");

    s_Vk.getDeviceProcAddr = (PFN_vkGetDeviceProcAddr)loadProc(
        s_Vk.handle,
        "vkGetDeviceProcAddr");

    s_Vk.getImageMemoryRequirements = (PFN_vkGetImageMemoryRequirements)loadProc(
        s_Vk.handle,
        "vkGetImageMemoryRequirements");

    s_Vk.allocateMemory = (PFN_vkAllocateMemory)loadProc(
        s_Vk.handle,
        "vkAllocateMemory");

    s_Vk.freeMemory = (PFN_vkFreeMemory)loadProc(
        s_Vk.handle,
        "vkFreeMemory");

    s_Vk.bindImageMemory = (PFN_vkBindImageMemory)loadProc(
        s_Vk.handle,
        "vkBindImageMemory");

    s_Vk.createCommandPool = (PFN_vkCreateCommandPool)loadProc(
        s_Vk.handle,
        "vkCreateCommandPool");

    s_Vk.destroyCommandPool = (PFN_vkDestroyCommandPool)loadProc(
        s_Vk.handle,
        "vkDestroyCommandPool");

    s_Vk.allocateCommandBuffer = (PFN_vkAllocateCommandBuffers)loadProc(
        s_Vk.handle,
        "vkAllocateCommandBuffers");

    s_Vk.freeCommandBuffer = (PFN_vkFreeCommandBuffers)loadProc(
        s_Vk.handle,
        "vkFreeCommandBuffers");

    s_Vk.createFence = (PFN_vkCreateFence)loadProc(
        s_Vk.handle,
        "vkCreateFence");

    s_Vk.destroyFence = (PFN_vkDestroyFence)loadProc(
        s_Vk.handle,
        "vkDestroyFence");

    s_Vk.resetFence = (PFN_vkResetFences)loadProc(
        s_Vk.handle,
        "vkResetFences");

    s_Vk.waitFence = (PFN_vkWaitForFences)loadProc(
        s_Vk.handle,
        "vkWaitForFences");

    s_Vk.isFenceSignaled = (PFN_vkGetFenceStatus)loadProc(
        s_Vk.handle,
        "vkGetFenceStatus");

    s_Vk.createSemaphore = (PFN_vkCreateSemaphore)loadProc(
        s_Vk.handle,
        "vkCreateSemaphore");

    s_Vk.destroySemaphore = (PFN_vkDestroySemaphore)loadProc(
        s_Vk.handle,
        "vkDestroySemaphore");

    s_Vk.cmdBegin = (PFN_vkBeginCommandBuffer)loadProc(
        s_Vk.handle,
        "vkBeginCommandBuffer");

    s_Vk.cmdEnd = (PFN_vkEndCommandBuffer)loadProc(
        s_Vk.handle,
        "vkEndCommandBuffer");

    s_Vk.resetCommandPool = (PFN_vkResetCommandPool)loadProc(
        s_Vk.handle,
        "vkResetCommandPool");

    s_Vk.resetCommandBuffer = (PFN_vkResetCommandBuffer)loadProc(
        s_Vk.handle,
        "vkResetCommandBuffer");

    s_Vk.cmdExecuteCommandBuffer = (PFN_vkCmdExecuteCommands)loadProc(
        s_Vk.handle,
        "vkCmdExecuteCommands");

    s_Vk.cmdCopyBuffer = (PFN_vkCmdCopyBuffer)loadProc(
        s_Vk.handle,
        "vkCmdCopyBuffer");

    s_Vk.cmdCopyBufferToImage = (PFN_vkCmdCopyBufferToImage)loadProc(
        s_Vk.handle,
        "vkCmdCopyBufferToImage");

    s_Vk.cmdCopyImage = (PFN_vkCmdCopyImage)loadProc(
        s_Vk.handle,
        "vkCmdCopyImage");

    s_Vk.cmdCopyImageToBuffer = (PFN_vkCmdCopyImageToBuffer)loadProc(
        s_Vk.handle,
        "vkCmdCopyImageToBuffer");

    s_Vk.cmdBindPipeline = (PFN_vkCmdBindPipeline)loadProc(
        s_Vk.handle,
        "vkCmdBindPipeline");

    s_Vk.cmdSetViewports = (PFN_vkCmdSetViewport)loadProc(
        s_Vk.handle,
        "vkCmdSetViewport");

    s_Vk.cmdSetScissors = (PFN_vkCmdSetScissor)loadProc(
        s_Vk.handle,
        "vkCmdSetScissor");

    s_Vk.cmdBindVertexBuffers = (PFN_vkCmdBindVertexBuffers)loadProc(
        s_Vk.handle,
        "vkCmdBindVertexBuffers");

    s_Vk.cmdBindIndexBuffer = (PFN_vkCmdBindIndexBuffer)loadProc(
        s_Vk.handle,
        "vkCmdBindIndexBuffer");

    s_Vk.cmdDraw = (PFN_vkCmdDraw)loadProc(
        s_Vk.handle,
        "vkCmdDraw");

    s_Vk.cmdDrawIndirect = (PFN_vkCmdDrawIndirect)loadProc(
        s_Vk.handle,
        "vkCmdDrawIndirect");

    s_Vk.cmdDrawIndexed = (PFN_vkCmdDrawIndexed)loadProc(
        s_Vk.handle,
        "vkCmdDrawIndexed");

    s_Vk.cmdDrawIndexedIndirect = (PFN_vkCmdDrawIndexedIndirect)loadProc(
        s_Vk.handle,
        "vkCmdDrawIndexedIndirect");

    s_Vk.cmdDispatch = (PFN_vkCmdDispatch)loadProc(
        s_Vk.handle,
        "vkCmdDispatch");

    s_Vk.cmdDispatchIndirect = (PFN_vkCmdDispatchIndirect)loadProc(
        s_Vk.handle,
        "vkCmdDispatchIndirect");

    s_Vk.cmdBindDescriptorSets = (PFN_vkCmdBindDescriptorSets)loadProc(
        s_Vk.handle,
        "vkCmdBindDescriptorSets");

    s_Vk.cmdPushConstants = (PFN_vkCmdPushConstants)loadProc(
        s_Vk.handle,
        "vkCmdPushConstants");

    s_Vk.createBuffer = (PFN_vkCreateBuffer)loadProc(
        s_Vk.handle,
        "vkCreateBuffer");

    s_Vk.destroyBuffer = (PFN_vkDestroyBuffer)loadProc(
        s_Vk.handle,
        "vkDestroyBuffer");

    s_Vk.mapMemory = (PFN_vkMapMemory)loadProc(
        s_Vk.handle,
        "vkMapMemory");

    s_Vk.unmapMemory = (PFN_vkUnmapMemory)loadProc(
        s_Vk.handle,
        "vkUnmapMemory");

    s_Vk.getBufferDeviceAddress = (PFN_vkGetBufferDeviceAddress)loadProc(
        s_Vk.handle,
        "vkGetBufferDeviceAddress");

    s_Vk.getBufferMemoryRequirements = (PFN_vkGetBufferMemoryRequirements)loadProc(
        s_Vk.handle,
        "vkGetBufferMemoryRequirements");

    s_Vk.bindBufferMemory = (PFN_vkBindBufferMemory)loadProc(
        s_Vk.handle,
        "vkBindBufferMemory");

    s_Vk.createDescriptorSetLayout = (PFN_vkCreateDescriptorSetLayout)loadProc(
        s_Vk.handle,
        "vkCreateDescriptorSetLayout");

    s_Vk.destroyDescriptorSetLayout = (PFN_vkDestroyDescriptorSetLayout)loadProc(
        s_Vk.handle,
        "vkDestroyDescriptorSetLayout");

    s_Vk.createDescriptorPool = (PFN_vkCreateDescriptorPool)loadProc(
        s_Vk.handle,
        "vkCreateDescriptorPool");

    s_Vk.destroyDescriptorPool = (PFN_vkDestroyDescriptorPool)loadProc(
        s_Vk.handle,
        "vkDestroyDescriptorPool");

    s_Vk.resetDescriptorPool = (PFN_vkResetDescriptorPool)loadProc(
        s_Vk.handle,
        "vkResetDescriptorPool");

    s_Vk.allocateDescriptorSet = (PFN_vkAllocateDescriptorSets)loadProc(
        s_Vk.handle,
        "vkAllocateDescriptorSets");

    s_Vk.updateDescriptorSet = (PFN_vkUpdateDescriptorSets)loadProc(
        s_Vk.handle,
        "vkUpdateDescriptorSets");

    s_Vk.createPipelineLayout = (PFN_vkCreatePipelineLayout)loadProc(
        s_Vk.handle,
        "vkCreatePipelineLayout");

    s_Vk.destroyPipelineLayout = (PFN_vkDestroyPipelineLayout)loadProc(
        s_Vk.handle,
        "vkDestroyPipelineLayout");

    s_Vk.createGraphicsPipeline = (PFN_vkCreateGraphicsPipelines)loadProc(
        s_Vk.handle,
        "vkCreateGraphicsPipelines");

    s_Vk.createComputePipeline = (PFN_vkCreateComputePipelines)loadProc(
        s_Vk.handle,
        "vkCreateComputePipelines");

    s_Vk.destroyPipeline = (PFN_vkDestroyPipeline)loadProc(
        s_Vk.handle,
        "vkDestroyPipeline");

    s_Vk.waitQueue = (PFN_vkQueueWaitIdle)loadProc(
        s_Vk.handle,
        "vkQueueWaitIdle");
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

    VkResult result;
    Uint32 layerCount = 0;
    bool hasValidationLayer = false;
    s_Vk.messenger = nullptr;
    s_Vk.allocator = allocator;
    VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo = {0};
    debugCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;

    if (debugger && debugger->callback) {
        // layers
        result = s_Vk.enumerateInstanceLayerProperties(&layerCount, nullptr);
        if (result == VK_SUCCESS) {
            VkLayerProperties* props = nullptr;
            props = palAllocate(s_Vk.allocator, sizeof(VkLayerProperties) * layerCount, 0);
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

            // message types
            if (!debugger->denyGeneral) {
                debugCreateInfo.messageType |= VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT;
            }

            if (!debugger->denyPerformance) {
                debugCreateInfo.messageType |= VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
            }

            if (!debugger->denyValidation) {
                debugCreateInfo.messageType |= VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT;
            }

            // message severities
            if (!debugger->denyInfoSeverity) {
                debugCreateInfo.messageSeverity |= VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT;
            }

            if (!debugger->denyWarningSeverity) {
                debugCreateInfo.messageSeverity |= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT;
            }

            if (!debugger->denyErrorSeverity) {
                debugCreateInfo.messageSeverity |= VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
            }

            debugCreateInfo.pUserData = debugger->userData;
            debugCreateInfo.pfnUserCallback = debugCallbackVk;
            s_Vk.callback = debugger->callback;
        }
    }

    // extensions
    Uint32 extCount = 0;
    const char* extensions[8];
    result = s_Vk.enumerateInstanceExtensionProperties(nullptr, &extCount, nullptr);
    if (result != VK_SUCCESS) {
        return PAL_RESULT_PLATFORM_FAILURE;
    }

    VkExtensionProperties* extensionProps = nullptr;
    extensionProps = palAllocate(s_Vk.allocator, sizeof(VkExtensionProperties) * extCount, 0);
    if (!extensionProps) {
        return PAL_RESULT_SUCCESS;
    }

    bool hasXlib = false;
    bool hasXcb = false;
    bool hasWayland = false;
    bool hasWin32 = false;
    bool hasSurface = false;
    bool hasExtDebug = false;
    s_Vk.enumerateInstanceExtensionProperties(nullptr, &extCount, extensionProps);

    for (int i = 0; i < extCount; i++) {
        VkExtensionProperties* prop = &extensionProps[i];
        if (strcmp(prop->extensionName, "VK_KHR_xlib_surface") == 0) {
            hasXlib = true;

        } else if (strcmp(prop->extensionName, "VK_KHR_xcb_surface") == 0) {
            hasXcb = true;

        } else if (strcmp(prop->extensionName, "VK_KHR_wayland_surface") == 0) {
            hasWayland = true;

        } else if (strcmp(prop->extensionName, "VK_KHR_win32_surface") == 0) {
            hasWin32 = true;

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

        if (hasXcb) {
            extensions[extensionCount++] = "VK_KHR_xcb_surface";
        }

        if (hasWin32) {
            extensions[extensionCount++] = "VK_KHR_win32_surface";
        }
    }

    const char* layers[2];
    layerCount = 0;
    if (hasValidationLayer && hasExtDebug) {
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

    if (debugger && debugger->callback) {
        instanceCreateInfo.pNext = &debugCreateInfo;
    }

    // vk allocator
    s_Vk.vkAllocator.pfnAllocation = allocateVk;
    s_Vk.vkAllocator.pfnFree = freeVk;
    s_Vk.vkAllocator.pfnReallocation = reallocVk;

    VkInstance instance = nullptr;
    result = s_Vk.createInstance(&instanceCreateInfo, &s_Vk.vkAllocator, &instance);
    if (result != VK_SUCCESS) {
        return resultFromVk(result);
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
    s_Vk.createXcbSurface = nullptr;
    s_Vk.createWin32Surface = nullptr;

    if (hasWayland) {
        s_Vk.createWaylandSurface = (PFN_vkCreateWaylandSurfaceKHR)s_Vk.getInstanceProcAddr(
            instance,
            "vkCreateWaylandSurfaceKHR");
    }

    if (hasXlib) {
        s_Vk.libX = loadLibrary("libX11.so");
        if (s_Vk.libX) {
            s_Vk.XGetWindowAttributes = (XGetWindowAttributesFn)loadProc(
                s_Vk.libX, 
                "XGetWindowAttributes");

            s_Vk.XVisualIDFromVisual = (XVisualIDFromVisualFn)loadProc(
                s_Vk.libX, 
                "XVisualIDFromVisual");
        }

        s_Vk.createXlibSurface = (PFN_vkCreateXlibSurfaceKHR)s_Vk.getInstanceProcAddr(
            instance,
            "vkCreateXlibSurfaceKHR");
    }

    if (hasXcb) {
        s_Vk.libXcb = loadLibrary("libxcb.so.1");
        if (s_Vk.libXcb) {
            s_Vk.xcbGetWindowAttributes = (xcb_get_window_attributes_fn)loadProc(
                s_Vk.libXcb, 
                "xcb_get_window_attributes");

            s_Vk.xcbGetWindowAttributesReply = (xcb_get_window_attributes_reply_fn)loadProc(
                s_Vk.libXcb, 
                "xcb_get_window_attributes_reply");
        }

        s_Vk.createXcbSurface = (PFN_vkCreateXcbSurfaceKHR)s_Vk.getInstanceProcAddr(
            instance,
            "vkCreateXcbSurfaceKHR");
    }

    if (hasWin32) {
        s_Vk.createWin32Surface = (PFN_vkCreateWin32SurfaceKHR)s_Vk.getInstanceProcAddr(
            instance,
            "vkCreateWin32SurfaceKHR");
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

    s_Vk.checkSurfaceSupport = (PFN_vkGetPhysicalDeviceSurfaceSupportKHR)s_Vk.getInstanceProcAddr(
        instance,
        "vkGetPhysicalDeviceSurfaceSupportKHR");

    if (debugger) {
        s_Vk.createMessenger =
            (PFN_vkCreateDebugUtilsMessengerEXT)s_Vk.getInstanceProcAddr(
                instance,
                "vkCreateDebugUtilsMessengerEXT");

        s_Vk.destroyMessenger =
            (PFN_vkDestroyDebugUtilsMessengerEXT)s_Vk.getInstanceProcAddr(
                instance,
                "vkDestroyDebugUtilsMessengerEXT");

        s_Vk.createMessenger(instance, &debugCreateInfo, &s_Vk.vkAllocator, &s_Vk.messenger);
    }
    // clang-format on

    s_Vk.adapters = nullptr;
    s_Vk.instance = instance;
    return PAL_RESULT_SUCCESS;
}

void PAL_CALL shutdownGraphicsVk()
{
    if (s_Vk.messenger) {
        s_Vk.destroyMessenger(s_Vk.instance, s_Vk.messenger, &s_Vk.vkAllocator);
    }

    s_Vk.destroyInstance(s_Vk.instance, &s_Vk.vkAllocator);
    freeLibrary(s_Vk.handle);

    if (s_Vk.libX) {
        freeLibrary(s_Vk.libX);
    }

    if (s_Vk.libXcb) {
        freeLibrary(s_Vk.libXcb);
    }

    if (s_Vk.adapters) {
        palFree(s_Vk.allocator, s_Vk.adapters);
    }
    memset(&s_Vk, 0, sizeof(s_Vk));
}

PalResult PAL_CALL enumerateAdaptersVk(
    Int32* count,
    PalAdapter** outAdapters)
{
    int deviceCount = 0;
    int adapterCount = 0;
    int extCount = 0;
    VkResult ret;
    VkExtensionProperties* exts = nullptr;
    VkPhysicalDeviceProperties props = {0};

    ret = s_Vk.enumeratePhysicalDevices(s_Vk.instance, &deviceCount, nullptr);
    if (ret != VK_SUCCESS) {
        return PAL_RESULT_PLATFORM_FAILURE;
    }

    if (s_Vk.adapters) {
        palFree(s_Vk.allocator, s_Vk.adapters);
    }

    VkPhysicalDevice* devices = nullptr;
    devices = palAllocate(s_Vk.allocator, sizeof(VkPhysicalDevice) * deviceCount, 0);
    s_Vk.adapters = palAllocate(s_Vk.allocator, sizeof(Adapter) * deviceCount, 0);
    if (!devices || !s_Vk.adapters) {
        return PAL_RESULT_OUT_OF_MEMORY;
    }

    s_Vk.enumeratePhysicalDevices(s_Vk.instance, &deviceCount, devices);
    for (int i = 0; i < deviceCount; i++) {
        VkPhysicalDevice phyDevice = devices[i];
        s_Vk.getPhysicalDeviceProperties(phyDevice, &props);
        if (props.apiVersion < VK_API_VERSION_1_3) {
            // check extension
            ret = s_Vk.enumerateDeviceExtensionProperties(phyDevice, nullptr, &extCount, nullptr);
            exts = palAllocate(s_Vk.allocator, sizeof(VkExtensionProperties) * extCount, 0);
            if (!exts) {
                return PAL_RESULT_OUT_OF_MEMORY;
            }

            bool found = false;
            s_Vk.enumerateDeviceExtensionProperties(phyDevice, nullptr, &extCount, exts);

            for (int i = 0; i < extCount; i++) {
                const char* ext = exts[i].extensionName;
                if (strcmp(ext, "VK_KHR_dynamic_rendering") == 0) {
                    found = true;
                    break;
                }
            }

            palFree(s_Vk.allocator, exts);
            if (!found) {
                continue;
            }
        }

        VkPhysicalDeviceDynamicRenderingFeaturesKHR required = {0};
        required.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DYNAMIC_RENDERING_FEATURES_KHR;

        VkPhysicalDeviceFeatures2KHR features = {0};
        features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2_KHR;
        features.pNext = &required;
        s_Vk.getPhysicalDeviceFeatures2(phyDevice, &features);

        if (!required.dynamicRendering) {
            continue;
        }

        if (outAdapters) {
            if (adapterCount < *count) {
                Adapter* tmp = &s_Vk.adapters[adapterCount];
                tmp->handle = devices[i];
                outAdapters[adapterCount] = (PalAdapter*)tmp;
            }
        }
        adapterCount++;
    }

    if (!outAdapters) {
        *count = adapterCount;
    }

    palFree(s_Vk.allocator, devices);
    return PAL_RESULT_SUCCESS;
}

PalResult PAL_CALL getAdapterInfoVk(
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

    return PAL_RESULT_SUCCESS;
}

PalResult PAL_CALL getAdapterCapabilitiesVk(
    PalAdapter* adapter,
    PalAdapterCapabilities* caps)
{
    VkResult result = VK_SUCCESS;
    Adapter* vkAdapter = (Adapter*)adapter;
    VkPhysicalDevice phyDevice = (VkPhysicalDevice)vkAdapter->handle;

    VkPhysicalDeviceProperties props = {0};
    VkPhysicalDeviceMultiviewPropertiesKHR multiViewProps = {0};
    VkPhysicalDeviceProperties2 properties2 = {0};
    multiViewProps.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MULTIVIEW_PROPERTIES_KHR;

    s_Vk.getPhysicalDeviceProperties(phyDevice, &props);
    properties2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
    properties2.pNext = &multiViewProps;
    s_Vk.getPhysicalDeviceProperties2(phyDevice, &properties2);

    caps->maxColorAttachments = props.limits.maxColorAttachments;
    caps->maxImageWidth = props.limits.maxImageDimension2D;
    caps->maxImageHeight = props.limits.maxImageDimension2D;
    caps->maxImageDepth = props.limits.maxImageDimension3D;
    caps->maxImageArrayLayers = props.limits.maxImageArrayLayers;

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

    Uint32 tmp = a > b ? a : b;
    Uint32 size = tmp > c ? tmp : c;
    Uint32 levels = 0;
    while (size > 0) {
        // divide by two
        size = size / 2;
        levels++;
    }
    caps->maxImageMipLevels = levels;

    // get supported queue commands
    Uint32 count = 0;
    s_Vk.getPhysicalDeviceQueueFamilyProperties(phyDevice, &count, nullptr);

    VkQueueFamilyProperties* queueProps = nullptr;
    queueProps = palAllocate(s_Vk.allocator, sizeof(VkQueueFamilyProperties) * count, 0);
    s_Vk.getPhysicalDeviceQueueFamilyProperties(phyDevice, &count, queueProps);

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

    caps->maxVertexLayouts = props.limits.maxVertexInputBindings;
    caps->maxVertexAttributes = props.limits.maxVertexInputAttributes;
    caps->maxTessellationPatchPoint = props.limits.maxTessellationPatchSize;

    caps->maxComputeWorkGroupInvocations = props.limits.maxComputeWorkGroupInvocations;
    caps->maxComputeWorkGroupCount[0] = props.limits.maxComputeWorkGroupCount[0];
    caps->maxComputeWorkGroupCount[1] = props.limits.maxComputeWorkGroupCount[1];
    caps->maxComputeWorkGroupCount[2] = props.limits.maxComputeWorkGroupCount[2];
    caps->maxComputeWorkGroupSize[0] = props.limits.maxComputeWorkGroupSize[0];
    caps->maxComputeWorkGroupSize[1] = props.limits.maxComputeWorkGroupSize[1];
    caps->maxComputeWorkGroupSize[2] = props.limits.maxComputeWorkGroupSize[2];

    palFree(s_Vk.allocator, queueProps);
    return PAL_RESULT_SUCCESS;
}

PalAdapterFeatures PAL_CALL getAdapterFeaturesVk(PalAdapter* adapter)
{
    VkResult result;
    PalAdapterFeatures adapterFeatures = 0;
    Uint32 extensionCount = 0;
    VkPhysicalDeviceProperties props = {0};

    Adapter* vkAdapter = (Adapter*)adapter;
    VkPhysicalDevice phyDevice = (VkPhysicalDevice)vkAdapter->handle;

    // get supported extensions
    s_Vk.getPhysicalDeviceProperties(phyDevice, &props);
    result = s_Vk.enumerateDeviceExtensionProperties(phyDevice, nullptr, &extensionCount, nullptr);
    if (result != VK_SUCCESS) {
        // we just return without any modern features which is rare
        return 0;
    }

    VkExtensionProperties* extensionProps = nullptr;
    extensionProps = palAllocate(s_Vk.allocator, sizeof(VkExtensionProperties) * extensionCount, 0);
    if (!extensionProps) {
        return 0;
    }

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
    bool shaderParameters = false;
    s_Vk.enumerateDeviceExtensionProperties(phyDevice, nullptr, &extensionCount, extensionProps);

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
            adapterFeatures |= PAL_ADAPTER_FEATURE_INDIRECT_DRAW_COUNT;

        } else if (strcmp(props->extensionName, "VK_KHR_draw_mesh_tasks_indirect_count") == 0) {
            adapterFeatures |= PAL_ADAPTER_FEATURE_INDIRECT_DRAW_MESH_COUNT;

        } else if (strcmp(props->extensionName, "VK_KHR_buffer_device_address") == 0) {
            bufferDeviceAddress = true;

        }  else if (strcmp(props->extensionName, "VK_KHR_shader_draw_parameters") == 0) {
            shaderParameters = true;
        }
    }

    // features that require core and extension support
    // ray tracing is not part of core
    if (rayTracingFound && accelerateFound) {
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

    // mesh shader is not part of core
    if (meshShader) {
        VkPhysicalDeviceMeshShaderFeaturesEXT mesh = {0};
        mesh.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MESH_SHADER_FEATURES_EXT;

        VkPhysicalDeviceFeatures2 features;
        features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
        features.pNext = &mesh;

        s_Vk.getPhysicalDeviceFeatures2(phyDevice, &features);
        if (mesh.meshShader && mesh.taskShader) {
            adapterFeatures |= PAL_ADAPTER_FEATURE_MESH_SHADER;
            adapterFeatures |= PAL_ADAPTER_FEATURE_INDIRECT_DRAW_MESH;
        }
    }

    // fragment shading rate is not part of core
    if (fragmentRateShading) {
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
        if (desc.runtimeDescriptorArray &&
            desc.descriptorBindingPartiallyBound &&
            desc.descriptorBindingVariableDescriptorCount &&
            desc.shaderSampledImageArrayNonUniformIndexing &&
            desc.descriptorBindingSampledImageUpdateAfterBind) {
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

    // indirect draw count is part of core 1.2
    if (props.apiVersion >= VK_API_VERSION_1_2) {
        VkPhysicalDeviceVulkan12Features features12 = {0};
        features12.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES;

        VkPhysicalDeviceFeatures2 features;
        features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
        features.pNext = &features12;

        s_Vk.getPhysicalDeviceFeatures2(phyDevice, &features);
        if (features12.drawIndirectCount) {
            adapterFeatures |= PAL_ADAPTER_FEATURE_INDIRECT_DRAW_COUNT;
        }
    }

    // shader draw parameters is part of core 1.2
    if (props.apiVersion >= VK_API_VERSION_1_2 || shaderParameters) {
        VkPhysicalDeviceShaderDrawParametersFeatures drawParameters = {0};
        drawParameters.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_DRAW_PARAMETERS_FEATURES;

        VkPhysicalDeviceFeatures2 features;
        features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
        features.pNext = &drawParameters;

        s_Vk.getPhysicalDeviceFeatures2(phyDevice, &features);
        if (drawParameters.shaderDrawParameters) {
            adapterFeatures |= PAL_ADAPTER_FEATURE_DISPATCH_BASE;
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
    adapterFeatures |= PAL_ADAPTER_FEATURE_FENCE_RESET;
    adapterFeatures |= PAL_ADAPTER_FEATURE_INDIRECT_DRAW;

    palFree(s_Vk.allocator, extensionProps);
    return adapterFeatures;
}

bool PAL_CALL isShaderTargetSupportedVk(
    PalAdapter* adapter, 
    PalShaderTarget target)
{
    Adapter* vkAdapter = (Adapter*)adapter;
    VkPhysicalDeviceProperties props = {0};
    s_Vk.getPhysicalDeviceProperties(vkAdapter->handle, &props);

    switch (target) {
        case PAL_SHADER_TARGET_SPIRV_1_0:
        case PAL_SHADER_TARGET_SPIRV_1_1:
        case PAL_SHADER_TARGET_SPIRV_1_2: {
            return true;
        }

        case PAL_SHADER_TARGET_SPIRV_1_3:
        case PAL_SHADER_TARGET_SPIRV_1_4: {
            if (props.apiVersion >= VK_API_VERSION_1_1) {
                return true;
            }
        }

        case PAL_SHADER_TARGET_SPIRV_1_5: {
            if (props.apiVersion >= VK_API_VERSION_1_2) {
                return true;
            }
        }

        case PAL_SHADER_TARGET_SPIRV_1_6: {
            if (props.apiVersion >= VK_API_VERSION_1_3) {
                return true;
            }
        }
    }

    return false;
}

PalShaderTarget PAL_CALL getHighestSupportedShaderTargetVk(
    PalAdapter* adapter, 
    PalShaderFormats shaderFormat)
{
    if (shaderFormat != PAL_SHADER_FORMAT_SPIRV) {
        return PAL_SHADER_TARGET_UNKNOWN;
    }

    Adapter* vkAdapter = (Adapter*)adapter;
    VkPhysicalDeviceProperties props = {0};
    s_Vk.getPhysicalDeviceProperties(vkAdapter->handle, &props);

     if (props.apiVersion >= VK_API_VERSION_1_3) {
        return PAL_SHADER_TARGET_SPIRV_1_6;

    } else if (props.apiVersion >= VK_API_VERSION_1_2) {
        return PAL_SHADER_TARGET_SPIRV_1_5;

    } else if (props.apiVersion >= VK_API_VERSION_1_1) {
        return PAL_SHADER_TARGET_SPIRV_1_4;
        
    } else if (props.apiVersion >= VK_API_VERSION_1_0) {
        return PAL_SHADER_TARGET_SPIRV_1_2;
    }

    return PAL_SHADER_TARGET_UNKNOWN;
}

// ==================================================
// Device
// ==================================================

PalResult PAL_CALL createDeviceVk(
    PalAdapter* adapter,
    PalAdapterFeatures features,
    PalDevice** outDevice)
{
    float priority = 1.0f;
    Uint32 phyQueueCount = 0;
    Uint32 queueFamilyCount = 0;
    VkResult result = VK_SUCCESS;
    Device* device = nullptr;
    VkPhysicalDeviceProperties props = {0};

    Adapter* vkAdapter = (Adapter*)adapter;
    VkPhysicalDevice phyDevice = (VkPhysicalDevice)vkAdapter->handle;
    s_Vk.getPhysicalDeviceProperties(phyDevice, &props);

    VkQueueFamilyProperties* queueFamilyProps = nullptr;
    VkDeviceQueueCreateInfo* queueCreateInfos = nullptr;
    s_Vk.getPhysicalDeviceQueueFamilyProperties(phyDevice, &queueFamilyCount, nullptr);

    Uint32 queueFamilySize = sizeof(VkQueueFamilyProperties) * queueFamilyCount;
    Uint32 queueCreateInfosSize = sizeof(VkDeviceQueueCreateInfo) * queueFamilyCount;

    queueFamilyProps = palAllocate(s_Vk.allocator, queueFamilySize, 0);
    queueCreateInfos = palAllocate(s_Vk.allocator, queueCreateInfosSize, 0);
    device = palAllocate(s_Vk.allocator, sizeof(Device), 0);
    if (!queueFamilyProps || !queueCreateInfos || !device) {
        return PAL_RESULT_OUT_OF_MEMORY;
    }

    memset(device, 0, sizeof(Device));
    device->phyDevice = phyDevice;
    s_Vk.getPhysicalDeviceQueueFamilyProperties(phyDevice, &queueFamilyCount, queueFamilyProps);
    for (int i = 0; i < queueFamilyCount; i++) {
        queueCreateInfos[i].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfos[i].pNext = nullptr;
        queueCreateInfos[i].pQueuePriorities = &priority;
        queueCreateInfos[i].queueFamilyIndex = i;
        queueCreateInfos[i].queueCount = queueFamilyProps[i].queueCount;
        queueCreateInfos[i].flags = 0;

        // we need the total number of physical queues
        phyQueueCount += queueFamilyProps[i].queueCount;
    }

    device->phyQueues = palAllocate(s_Vk.allocator, sizeof(PhysicalQueue) * phyQueueCount, 0);
    if (!device->phyQueues) {
        return PAL_RESULT_OUT_OF_MEMORY;
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
    void* next = nullptr;
    int extCount = 0;
    const char* extensions[16] = {0};

    VkPhysicalDeviceTimelineSemaphoreFeaturesKHR timeline = {0};
    timeline.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_TIMELINE_SEMAPHORE_FEATURES_KHR;

    VkPhysicalDeviceShaderFloat16Int8FeaturesKHR shader16 = {0};
    shader16.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_FLOAT16_INT8_FEATURES_KHR;

    VkPhysicalDeviceMeshShaderFeaturesEXT mesh = {0};
    mesh.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MESH_SHADER_FEATURES_EXT;

    VkPhysicalDeviceRayTracingPipelineFeaturesKHR ray = {0};
    VkPhysicalDeviceAccelerationStructureFeaturesKHR acc = {0};
    ray.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_TRACING_PIPELINE_FEATURES_KHR;
    acc.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ACCELERATION_STRUCTURE_FEATURES_KHR;

    VkPhysicalDeviceFragmentShadingRateFeaturesKHR fsr = {0};
    fsr.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FRAGMENT_SHADING_RATE_FEATURES_KHR;

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

    VkPhysicalDeviceDynamicRenderingFeaturesKHR dynamicRendering = {0};
    dynamicRendering.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DYNAMIC_RENDERING_FEATURES_KHR;

    VkPhysicalDeviceSynchronization2FeaturesKHR sync2 = {0};
    sync2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SYNCHRONIZATION_2_FEATURES_KHR;

    VkPhysicalDeviceVulkan12Features features12 = {0};
    features12.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES;

    VkPhysicalDeviceShaderDrawParametersFeatures drawParameters = {0};
    drawParameters.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_DRAW_PARAMETERS_FEATURES;

    if (props.apiVersion < VK_API_VERSION_1_3) {
        extensions[extCount++] = "VK_KHR_dynamic_rendering";
        extensions[extCount++] = "VK_KHR_synchronization2";
    }

    dynamicRendering.dynamicRendering = true;
    sync2.synchronization2 = true;

    dynamicRendering.pNext = next;
    sync2.pNext = &dynamicRendering;
    next = &sync2;

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
        features12.timelineSemaphore = true;

        timeline.pNext = next;
        next = &timeline;
    }

    if (features & PAL_ADAPTER_FEATURE_SHADER_FLOAT16) {
        if (props.apiVersion < VK_API_VERSION_1_2) {
            extensions[extCount++] = "VK_KHR_shader_float16_int8";
        }
        shader16.shaderFloat16 = true;
        features12.shaderFloat16 = true;
        features12.shaderInt8 = true;

        shader16.pNext = next;
        next = &shader16;
    }

    if (features & PAL_ADAPTER_FEATURE_RAY_TRACING) {
        extensions[extCount++] = "VK_KHR_ray_tracing_pipeline";
        extensions[extCount++] = "VK_KHR_acceleration_structure";
        extensions[extCount++] = "VK_KHR_deferred_host_operations";
        ray.rayTracingPipeline = true;
        acc.accelerationStructure = true;

        // ray tracing needs buffer address feature enabled
        if (props.apiVersion < VK_API_VERSION_1_2) {
            extensions[extCount++] = "VK_KHR_buffer_device_address";
        }
        bufferAddress.bufferDeviceAddress = true;
        features12.bufferDeviceAddress = true;

        ray.pNext = next;
        acc.pNext = &ray;
        bufferAddress.pNext = &acc;
        next = &bufferAddress;
    }

    if (features & PAL_ADAPTER_FEATURE_MESH_SHADER) {
        extensions[extCount++] = "VK_EXT_mesh_shader";
        mesh.meshShader = true;
        mesh.taskShader = true;

        // mesh shader needs geometry feature for primitives
        coreFeatures.geometryShader = true;

        // msh draw indirect count
        if (features & PAL_ADAPTER_FEATURE_INDIRECT_DRAW_MESH_COUNT) {
            extensions[extCount++] = "VK_KHR_draw_mesh_tasks_indirect_count";
        }

        mesh.pNext = next;
        next = &mesh;
    }

    if (features & PAL_ADAPTER_FEATURE_INDIRECT_DRAW_COUNT) {
        if (props.apiVersion < VK_API_VERSION_1_2) {
            extensions[extCount++] = "VK_KHR_draw_indirect_count";
        }
        features12.drawIndirectCount = true;

        features12.pNext = next;
        next = &features12;
    }

    if ((features & PAL_ADAPTER_FEATURE_FRAGMENT_SHADING_RATE) ||
        (features & PAL_ADAPTER_FEATURE_FRAGMENT_SHADING_RATE_ATTACHMENT)) {
        extensions[extCount++] = "VK_KHR_fragment_shading_rate";
        fsr.pipelineFragmentShadingRate = true;

        // fragment shading rate attachment needs this
        if (features & PAL_ADAPTER_FEATURE_FRAGMENT_SHADING_RATE_ATTACHMENT) {
            fsr.attachmentFragmentShadingRate = true;
        }

        fsr.pNext = next;
        next = &fsr;
    }

    if (features & PAL_ADAPTER_FEATURE_DESCRIPTOR_INDEXING) {
        if (props.apiVersion < VK_API_VERSION_1_2) {
            extensions[extCount++] = "VK_EXT_descriptor_indexing";
        }

        descIndex.runtimeDescriptorArray = true;
        descIndex.descriptorBindingPartiallyBound = true;
        descIndex.shaderSampledImageArrayNonUniformIndexing = true;
        descIndex.descriptorBindingSampledImageUpdateAfterBind = true;
        descIndex.descriptorBindingVariableDescriptorCount = true;
        features12.descriptorIndexing = true;

        // check support for bindless storage and uniform buffers
        VkPhysicalDeviceDescriptorIndexingFeaturesEXT desc = {0};
        desc.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_FEATURES_EXT;

        VkPhysicalDeviceFeatures2 features;
        features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
        features.pNext = &desc;
        s_Vk.getPhysicalDeviceFeatures2(phyDevice, &features);

        if (desc.shaderStorageBufferArrayNonUniformIndexing &&
            desc.descriptorBindingStorageBufferUpdateAfterBind) {
            descIndex.shaderStorageBufferArrayNonUniformIndexing = true;
            descIndex.descriptorBindingStorageBufferUpdateAfterBind = true;
        }

        if (desc.shaderUniformBufferArrayNonUniformIndexing &&
            desc.descriptorBindingUniformBufferUpdateAfterBind) {
            descIndex.shaderUniformBufferArrayNonUniformIndexing = true;
            descIndex.descriptorBindingUniformBufferUpdateAfterBind = true;
        }

        descIndex.pNext = next;
        next = &descIndex;
    }

    if (features & PAL_ADAPTER_FEATURE_MULTI_VIEW) {
        if (props.apiVersion < VK_API_VERSION_1_2) {
            extensions[extCount++] = "VK_KHR_multiview";
        }
        multiView.multiview = true;

        multiView.pNext = next;
        next = &multiView;
    }

    if (features & PAL_ADAPTER_FEATURE_DYNAMIC_CULL_MODE ||
        features & PAL_ADAPTER_FEATURE_DYNAMIC_FRONT_FACE ||
        features & PAL_ADAPTER_FEATURE_DYNAMIC_PRIMITIVE_TOPOLOGY) {
        if (props.apiVersion < VK_API_VERSION_1_3) {
            extensions[extCount++] = "VK_EXT_extended_dynamic_state";
        }
        dynamicState.extendedDynamicState = true;

        dynamicState.pNext = next;
        next = &dynamicState;
    }

    if (features & PAL_ADAPTER_FEATURE_DYNAMIC_DEPTH_TEST_ENABLE ||
        features & PAL_ADAPTER_FEATURE_DYNAMIC_DEPTH_WRITE_ENABLE ||
        features & PAL_ADAPTER_FEATURE_DYNAMIC_STENCIL_OP) {
        extensions[extCount++] = "VK_EXT_extended_dynamic_state2";
        dynamicState2.extendedDynamicState2 = true;

        dynamicState2.pNext = next;
        next = &dynamicState2;
    }

    if (features & PAL_ADAPTER_FEATURE_BUFFER_DEVICE_ADDRESS) {
        if (props.apiVersion < VK_API_VERSION_1_2) {
            extensions[extCount++] = "VK_KHR_buffer_device_address";
        }
        bufferAddress.bufferDeviceAddress = true;
        features12.bufferDeviceAddress = true;

        bufferAddress.pNext = next;
        next = &bufferAddress;
    }

    if (features & PAL_ADAPTER_FEATURE_DISPATCH_BASE) {
        if (props.apiVersion < VK_API_VERSION_1_2) {
            extensions[extCount++] = "VK_KHR_shader_draw_parameters";
        }
        drawParameters.shaderDrawParameters = true;

        drawParameters.pNext = next;
        next = &drawParameters;
    }

    VkDeviceCreateInfo createInfo = {0};
    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    createInfo.pEnabledFeatures = &coreFeatures;
    createInfo.enabledExtensionCount = extCount;
    createInfo.ppEnabledExtensionNames = extensions;
    createInfo.pQueueCreateInfos = queueCreateInfos;
    createInfo.queueCreateInfoCount = queueFamilyCount;
    createInfo.pNext = next;

    result = s_Vk.createDevice(phyDevice, &createInfo, &s_Vk.vkAllocator, &device->handle);
    if (result != VK_SUCCESS) {
        palFree(s_Vk.allocator, queueFamilyProps);
        palFree(s_Vk.allocator, queueCreateInfos);
        palFree(s_Vk.allocator, device->phyQueues);
        palFree(s_Vk.allocator, device);
        return resultFromVk(result);
    }

    device->phyQueueIndex = 0;
    device->queueFamilyCount = queueFamilyCount;
    device->phyQueueCount = phyQueueCount;
    device->features = features;

    // get queues
    for (int i = 0; i < queueFamilyCount; i++) {
        VkQueueFamilyProperties* data = &queueFamilyProps[i];
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

    memset(device->memoryClassMask, 0, sizeof(Uint32) * 3);
    for (int i = 0; i < memProps.memoryTypeCount; i++) {
        VkMemoryPropertyFlags flags = memProps.memoryTypes[i].propertyFlags;

        Uint32 bit = (1u << i);
        if ((flags & VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT) &&
            !(flags & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT)) {
            device->memoryClassMask[PAL_MEMORY_TYPE_GPU_ONLY] |= bit;
        }

        if ((flags & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT) &&
            (flags & VK_MEMORY_PROPERTY_HOST_COHERENT_BIT)) {
            device->memoryClassMask[PAL_MEMORY_TYPE_CPU_UPLOAD] |= bit;
        }

        if ((flags & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT) &&
            (flags & VK_MEMORY_PROPERTY_HOST_CACHED_BIT)) {
            device->memoryClassMask[PAL_MEMORY_TYPE_CPU_READBACK] |= bit;
        }
    }

    // HACK: most CPU drivers dont have a vram so we set the vram to system memory
    if (device->memoryClassMask[PAL_MEMORY_TYPE_GPU_ONLY] == 0) {
        device->memoryClassMask[PAL_MEMORY_TYPE_GPU_ONLY] =
            device->memoryClassMask[PAL_MEMORY_TYPE_CPU_UPLOAD];
    }

    // clang-format off
    // swapchain procs
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

    // semaphore procs
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

    // fragment shading rate procs
    if (features & PAL_ADAPTER_FEATURE_FRAGMENT_SHADING_RATE) {
        device->cmdSetFragmentShadingRate =
            (PFN_vkCmdSetFragmentShadingRateKHR)s_Vk.getDeviceProcAddr(
                device->handle,
                "vkCmdSetFragmentShadingRateKHR");
    }

    // mesh shader procs
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

    // ray tracing procs
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

        device->getRayTracingShaderGroupHandles =
            (PFN_vkGetRayTracingShaderGroupHandlesKHR)s_Vk.getDeviceProcAddr(
                device->handle,
                "vkGetRayTracingShaderGroupHandlesKHR");

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
        device->features |= PAL_ADAPTER_FEATURE_BUFFER_DEVICE_ADDRESS;
    }

    // buffer address procs
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

    // indirect draw count procs
    if (features & PAL_ADAPTER_FEATURE_INDIRECT_DRAW_COUNT) {
        device->cmdDrawIndirectCount =
            (PFN_vkCmdDrawIndirectCount)s_Vk.getDeviceProcAddr(
                device->handle,
                "vkCmdDrawIndirectCount");

        device->cmdDrawIndexedIndirectCount =
                (PFN_vkCmdDrawIndexedIndirectCount)s_Vk.getDeviceProcAddr(
                    device->handle,
                    "vkCmdDrawIndexedIndirectCount");

        if (!device->cmdDrawIndirectCount) {
            device->cmdDrawIndirectCount =
                (PFN_vkCmdDrawIndirectCountKHR)s_Vk.getDeviceProcAddr(
                    device->handle,
                    "vkCmdDrawIndirectCountKHR");

            device->cmdDrawIndexedIndirectCount =
                    (PFN_vkCmdDrawIndexedIndirectCountKHR)s_Vk.getDeviceProcAddr(
                        device->handle,
                        "vkCmdDrawIndexedIndirectCountKHR");
        }
    }

    // dispatch base procs
    if (features & PAL_ADAPTER_FEATURE_FRAGMENT_SHADING_RATE) {
        device->cmdDispatchBase =
            (PFN_vkCmdDispatchBase)s_Vk.getDeviceProcAddr(
                device->handle,
                "vkCmdDispatchBase");

        if (!device->cmdDispatchBase) {
            device->cmdDispatchBase =
                (PFN_vkCmdDispatchBaseKHR)s_Vk.getDeviceProcAddr(
                    device->handle,
                    "vkCmdDispatchBaseKHR");
        }
    }

    // dynamic rendering procs
    device->cmdBeginRendering =
        (PFN_vkCmdBeginRendering)s_Vk.getDeviceProcAddr(
            device->handle,
            "vkCmdBeginRendering");

    device->cmdEndRendering =
        (PFN_vkCmdEndRendering)s_Vk.getDeviceProcAddr(
            device->handle,
            "vkCmdEndRendering");

    device->cmdPipelineBarrier =
        (PFN_vkCmdPipelineBarrier2)s_Vk.getDeviceProcAddr(
            device->handle,
            "vkCmdPipelineBarrier2");

    device->queueSubmit =
        (PFN_vkQueueSubmit2)s_Vk.getDeviceProcAddr(
            device->handle,
            "vkQueueSubmit2");

    if (!device->cmdBeginRendering) {
        device->cmdBeginRendering =
            (PFN_vkCmdBeginRenderingKHR)s_Vk.getDeviceProcAddr(
                device->handle,
                "vkCmdBeginRenderingKHR");

        device->cmdEndRendering =
            (PFN_vkCmdEndRenderingKHR)s_Vk.getDeviceProcAddr(
                device->handle,
                "vkCmdEndRenderingKHR");

        device->cmdPipelineBarrier =
        (PFN_vkCmdPipelineBarrier2KHR)s_Vk.getDeviceProcAddr(
            device->handle,
            "vkCmdPipelineBarrier2KHR");

        device->queueSubmit =
        (PFN_vkQueueSubmit2KHR)s_Vk.getDeviceProcAddr(
            device->handle,
            "vkQueueSubmit2KHR");
    }

    // dynamic states
    if (features & PAL_ADAPTER_FEATURE_DYNAMIC_CULL_MODE) {
        device->cmdSetCullMode =
            (PFN_vkCmdSetCullMode)s_Vk.getDeviceProcAddr(
                device->handle,
                "vkCmdSetCullMode");

        if (!device->cmdSetCullMode) {
            device->cmdSetCullMode =
                (PFN_vkCmdSetCullModeEXT)s_Vk.getDeviceProcAddr(
                    device->handle,
                    "vkCmdSetCullModeEXT");
        }
    }

    if (features & PAL_ADAPTER_FEATURE_DYNAMIC_FRONT_FACE) {
        device->cmdSetFrontFace =
            (PFN_vkCmdSetFrontFace)s_Vk.getDeviceProcAddr(
                device->handle,
                "vkcmdSetFrontFace");

        if (!device->cmdSetFrontFace) {
            device->cmdSetFrontFace =
                (PFN_vkCmdSetFrontFaceEXT)s_Vk.getDeviceProcAddr(
                    device->handle,
                    "vkcmdSetFrontFaceEXT");
        }
    }

    if (features & PAL_ADAPTER_FEATURE_DYNAMIC_PRIMITIVE_TOPOLOGY) {
        device->cmdSetPrimitiveTopology =
            (PFN_vkCmdSetPrimitiveTopology)s_Vk.getDeviceProcAddr(
                device->handle,
                "vkcmdSetPrimitiveTopology");

        if (!device->cmdSetPrimitiveTopology) {
            device->cmdSetPrimitiveTopology =
                (PFN_vkCmdSetPrimitiveTopologyEXT)s_Vk.getDeviceProcAddr(
                    device->handle,
                    "vkcmdSetPrimitiveTopologyEXT");
        }
    }

    if (features & PAL_ADAPTER_FEATURE_DYNAMIC_DEPTH_TEST_ENABLE) {
        device->cmdSetDepthTestEnable =
            (PFN_vkCmdSetDepthTestEnable)s_Vk.getDeviceProcAddr(
                device->handle,
                "vkcmdSetDepthTestEnable");

        if (!device->cmdSetDepthTestEnable) {
            device->cmdSetDepthTestEnable =
                (PFN_vkCmdSetDepthTestEnableEXT)s_Vk.getDeviceProcAddr(
                    device->handle,
                    "vkcmdSetDepthTestEnableEXT");
        }
    }

    if (features & PAL_ADAPTER_FEATURE_DYNAMIC_DEPTH_WRITE_ENABLE) {
        device->cmdSetDepthWriteEnable =
            (PFN_vkCmdSetDepthWriteEnable)s_Vk.getDeviceProcAddr(
                device->handle,
                "vkcmdSetDepthWriteEnable");

        if (!device->cmdSetDepthWriteEnable) {
            device->cmdSetDepthWriteEnable =
                (PFN_vkCmdSetDepthWriteEnableEXT)s_Vk.getDeviceProcAddr(
                    device->handle,
                    "vkcmdSetDepthWriteEnableEXT");
        }
    }

    if (features & PAL_ADAPTER_FEATURE_DYNAMIC_STENCIL_OP) {
        device->cmdSetStencilOp =
            (PFN_vkCmdSetStencilOp)s_Vk.getDeviceProcAddr(
                device->handle,
                "vkcmdSetStencilOp");

        if (!device->cmdSetStencilOp) {
            device->cmdSetStencilOp =
                (PFN_vkCmdSetStencilOpEXT)s_Vk.getDeviceProcAddr(
                    device->handle,
                    "vkcmdSetStencilOpEXT");
        }
    }

    // clang-format on

    palFree(s_Vk.allocator, queueFamilyProps);
    palFree(s_Vk.allocator, queueCreateInfos);

    *outDevice = (PalDevice*)device;
    return PAL_RESULT_SUCCESS;
}

void PAL_CALL destroyDeviceVk(PalDevice* device)
{
    Device* vkDevice = (Device*)device;
    s_Vk.destroyDevice(vkDevice->handle, &s_Vk.vkAllocator);
    palFree(s_Vk.allocator, vkDevice->phyQueues);
    palFree(s_Vk.allocator, vkDevice);
}

// ==================================================
// Memory
// ==================================================

PalResult PAL_CALL allocateMemoryVk(
    PalDevice* device,
    PalMemoryType type,
    Uint64 memoryMask,
    Uint64 size,
    PalMemory** outMemory)
{
    VkResult result;
    Memory* memory = nullptr;
    Device* vkDevice = (Device*)device;
    VkMemoryAllocateInfo allocateInfo = {0};
    allocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocateInfo.allocationSize = (VkDeviceSize)size;
    VkMemoryAllocateFlagsInfo allocateFlagsInfo = {0};

    memory = palAllocate(s_Vk.allocator, sizeof(Memory), 0);
    if (!memory) {
        return PAL_RESULT_NULL_POINTER;
    }

    Uint32 memoryTypeMask = 0;
    Uint32 usages = 0;
    palUnpackUint32(memoryMask, &memoryTypeMask, &usages);
    Uint32 memoryClassMask = vkDevice->memoryClassMask[type] & memoryTypeMask;
    if (memoryClassMask == 0) {
        return PAL_RESULT_MEMORY_TYPE_NOT_SUPPORTED;
    }

    // pick an index using the scoring system
    Uint32 memoryIndex = findBestMemoryIndexVk(vkDevice->phyDevice, memoryClassMask);
    if (memoryIndex == UINT32_MAX) {
        return PAL_RESULT_MEMORY_TYPE_NOT_SUPPORTED;
    }

    // check if the memory index is valid
    if (!(memoryClassMask & (1u << memoryIndex))) {
        return PAL_RESULT_MEMORY_TYPE_NOT_SUPPORTED;
    }

    allocateInfo.memoryTypeIndex = memoryIndex;
    if (usages & PAL_BUFFER_USAGE_DEVICE_ADDRESS) {
        allocateFlagsInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_FLAGS_INFO;
        allocateFlagsInfo.flags = VK_MEMORY_ALLOCATE_DEVICE_ADDRESS_BIT_KHR;
        allocateInfo.pNext = &allocateFlagsInfo;
    }

    result = s_Vk.allocateMemory(
        vkDevice->handle, 
        &allocateInfo, 
        &s_Vk.vkAllocator, 
        &memory->handle);

    if (result != VK_SUCCESS) {
        return resultFromVk(result);
    }

    memory->type = type;
    *outMemory = (PalMemory*)memory;
    return PAL_RESULT_SUCCESS;
}

void PAL_CALL freeMemoryVk(
    PalDevice* device,
    PalMemory* memory)
{
    Device* vkDevice = (Device*)device;
    Memory* vkMemory = (Memory*)memory;
    s_Vk.freeMemory(vkDevice->handle, vkMemory->handle, &s_Vk.vkAllocator);
    palFree(s_Vk.allocator, vkMemory);
}

// ==================================================
// Extended Adapter Features
// ==================================================

PalResult PAL_CALL querySamplerAnisotropyCapabilitiesVk(
    PalDevice* device,
    PalSamplerAnisotropyCapabilities* caps)
{
    Device* vkDevice = (Device*)device;
    if (!(vkDevice->features & PAL_ADAPTER_FEATURE_SAMPLER_ANISOTROPY)) {
        return PAL_RESULT_ADAPTER_FEATURE_NOT_SUPPORTED;
    }

    VkPhysicalDeviceProperties props = {0};
    s_Vk.getPhysicalDeviceProperties(vkDevice->phyDevice, &props);

    caps->maxAnisotropy = props.limits.maxSamplerAnisotropy;
    return PAL_RESULT_SUCCESS;
}

PalResult PAL_CALL queryDepthStencilCapabilitiesVk(
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
    props.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DEPTH_STENCIL_RESOLVE_PROPERTIES_KHR;

    properties2.pNext = &props;
    s_Vk.getPhysicalDeviceProperties2(vkDevice->phyDevice, &properties2);

    caps->independentDepthStencilResolve = props.independentResolve;
    if (props.supportedDepthResolveModes & VK_RESOLVE_MODE_AVERAGE_BIT_KHR) {
        caps->depthResolveModes[PAL_RESOLVE_MODE_AVERAGE] = true;
    }

    if (props.supportedDepthResolveModes & VK_RESOLVE_MODE_SAMPLE_ZERO_BIT_KHR) {
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

    if (props.supportedStencilResolveModes & VK_RESOLVE_MODE_SAMPLE_ZERO_BIT_KHR) {
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

PalResult PAL_CALL queryFragmentShadingRateCapabilitiesVk(
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
    props.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FRAGMENT_SHADING_RATE_PROPERTIES_KHR;

    properties2.pNext = &props;
    s_Vk.getPhysicalDeviceProperties2(vkDevice->phyDevice, &properties2);

    memset(caps, 0, sizeof(PalFragmentShadingRateCapabilities));
    for (int i = 0; i < PAL_FRAGMENT_SHADING_RATE_MAX; i++) {
        VkExtent2D size = getShadingRateSizeVk((PalFragmentShadingRate)i);

        // check against the max size
        if (size.width <= props.maxFragmentSize.width ||
            size.height <= props.maxFragmentSize.height) {
            caps->shadingRates[i] = true;
        }
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

PalResult PAL_CALL queryMeshShaderCapabilitiesVk(
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

PalResult PAL_CALL queryRayTracingCapabilitiesVk(
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
    props.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_TRACING_PIPELINE_PROPERTIES_KHR;

    VkPhysicalDeviceAccelerationStructurePropertiesKHR accProps = {0};
    accProps.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ACCELERATION_STRUCTURE_PROPERTIES_KHR;

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

    return PAL_RESULT_SUCCESS;
}

PalResult PAL_CALL queryDescriptorIndexingCapabilitiesVk(
    PalDevice* device,
    PalDescriptorIndexingCapabilities* caps)
{
    Device* vkDevice = (Device*)device;
    if (!(vkDevice->features & PAL_ADAPTER_FEATURE_DESCRIPTOR_INDEXING)) {
        return PAL_RESULT_ADAPTER_FEATURE_NOT_SUPPORTED;
    }

    caps->bindlessSamplers = true;
    VkPhysicalDeviceDescriptorIndexingFeaturesEXT desc = {0};
    desc.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_FEATURES_EXT;

    VkPhysicalDeviceProperties2 properties2 = {0};
    properties2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;

    VkPhysicalDeviceDescriptorIndexingPropertiesEXT props = {0};
    props.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_PROPERTIES_EXT;

    VkPhysicalDeviceFeatures2 features;
    features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
    
    features.pNext = &desc;
    properties2.pNext = &props;
    s_Vk.getPhysicalDeviceFeatures2(vkDevice->phyDevice, &features);
    s_Vk.getPhysicalDeviceProperties2(vkDevice->phyDevice, &properties2);

    // check for bindless storage buffers
    if (desc.shaderStorageBufferArrayNonUniformIndexing &&
        desc.descriptorBindingStorageBufferUpdateAfterBind) {
        caps->bindlessStorageBuffers = true;
    }

    // check for bindless uniform buffers
    if (desc.shaderUniformBufferArrayNonUniformIndexing &&
        desc.descriptorBindingUniformBufferUpdateAfterBind) {
        caps->bindlessUniformBuffers = true;
    }

    caps->maxImagesPerShaderStage = props.maxPerStageDescriptorUpdateAfterBindSampledImages;
    caps->maxImagesPerDescriptorSet = props.maxDescriptorSetUpdateAfterBindSampledImages;

    caps->maxSamplersPerShaderStage = props.maxPerStageDescriptorUpdateAfterBindSamplers;
    caps->maxSamplersPerDescriptorSet = props.maxDescriptorSetUpdateAfterBindSamplers;

    caps->maxStorageBuffersPerShaderStage =
        props.maxPerStageDescriptorUpdateAfterBindStorageBuffers;
    caps->maxStorageBuffersPerDescriptorSet = props.maxDescriptorSetUpdateAfterBindStorageBuffers;

    caps->maxUniformBuffersPerShaderStage =
        props.maxPerStageDescriptorUpdateAfterBindUniformBuffers;
    caps->maxUniformBuffersPerDescriptorSet = props.maxDescriptorSetUpdateAfterBindUniformBuffers;

    caps->maxDescriptors = props.maxUpdateAfterBindDescriptorsInAllPools;
    return PAL_RESULT_SUCCESS;
}

// ==================================================
// Queue
// ==================================================

PalResult PAL_CALL createQueueVk(
    PalDevice* device,
    PalQueueType type,
    PalQueue** outQueue)
{
    Device* vkDevice = (Device*)device;
    VkQueueFlags queueFlag = 0;
    Queue* queue = nullptr;

    if (vkDevice->phyQueueCount == 0) {
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

    // we index the for loop so we dont always start at the beginning, this way
    // we cycle through all queue families each time we create a queue
    PhysicalQueue* phyQueue = nullptr;
    for (int i = vkDevice->phyQueueIndex; i < vkDevice->phyQueueCount; i++) {
        PhysicalQueue* pq = &vkDevice->phyQueues[i];
        // check if the physical queue supports the requested operation
        // and if its not already used
        if (pq->usages & queueFlag && pq->usedUsages != queueFlag) {
            pq->usedUsages |= queueFlag;
            phyQueue = pq;
            break;
        }
    }

    if (!phyQueue) {
        // we didnt get any queue, we check if we started the loop at the beginning or mid way
        if (vkDevice->phyQueueIndex == 0) {
            // we searched all queue families
            return PAL_RESULT_OUT_OF_QUEUE;

        } else {
            // we start at the beginning and go through the queue families again
            vkDevice->phyQueueIndex = 0;
            for (int i = vkDevice->phyQueueIndex; i < vkDevice->phyQueueCount; i++) {
                PhysicalQueue* pq = &vkDevice->phyQueues[i];
                if (pq->usages & queueFlag && pq->usedUsages != queueFlag) {
                    pq->usedUsages |= queueFlag;
                    phyQueue = pq;
                    break;
                }
            }

            if (!phyQueue) {
                return PAL_RESULT_OUT_OF_QUEUE;
            }
        }
    }

    queue = palAllocate(s_Vk.allocator, sizeof(Queue), 0);
    if (!queue) {
        return PAL_RESULT_OUT_OF_MEMORY;
    }

    vkDevice->phyQueueIndex = (vkDevice->phyQueueIndex + 1) % vkDevice->phyQueueCount;
    queue->phyQueue = phyQueue;
    queue->usage = queueFlag;
    queue->device = vkDevice;

    *outQueue = (PalQueue*)queue;
    return PAL_RESULT_SUCCESS;
}

void PAL_CALL destroyQueueVk(PalQueue* queue)
{
    Queue* vkQueue = (Queue*)queue;
    PhysicalQueue* phyQueue = vkQueue->phyQueue;
    phyQueue->usedUsages &= ~vkQueue->usage;
    palFree(s_Vk.allocator, vkQueue);
}

PalResult PAL_CALL waitQueueVk(PalQueue* queue)
{
    Queue* vkQueue = (Queue*)queue;
    VkResult result = s_Vk.waitQueue(vkQueue->phyQueue->handle);
    if (result != VK_SUCCESS) {
        return resultFromVk(result);
    }

    return PAL_RESULT_SUCCESS;
}

bool PAL_CALL canQueuePresentVk(
    PalQueue* queue,
    PalSurface* surface)
{
    VkResult result;
    Queue* vkQueue = (Queue*)queue;
    PhysicalQueue* phyQueue = vkQueue->phyQueue;
    Surface* vkSurface = (Surface*)surface;

    // check if the queue is a graphics queue before we check its family
    // index for presentation support.
    if (vkQueue->usage != VK_QUEUE_GRAPHICS_BIT) {
        return false;
    }

    VkBool32 supported = false;
    result = s_Vk.checkSurfaceSupport(
        phyQueue->phyDevice, 
        phyQueue->familyIndex, 
        vkSurface->handle, 
        &supported);

    if (result == VK_SUCCESS && supported) {
        return true;
    }

    return false;
}

// ==================================================
// Formats
// ==================================================

PalResult PAL_CALL enumerateFormatsVk(
    PalAdapter* adapter,
    Int32* count,
    PalFormatInfo* outFormats)
{
    Int32 fmtCount = 0;
    Adapter* vkAdapter = (Adapter*)adapter;
    VkPhysicalDevice phyDevice = (VkPhysicalDevice)vkAdapter->handle;
    VkFormatProperties props = {0};

    for (int i = 0; i < PAL_FORMAT_MAX; i++) {
        VkFormat fmt = formatToVk((PalFormat)i);
        s_Vk.getPhysicalDeviceFormatProperties(phyDevice, fmt, &props);
        if (props.optimalTilingFeatures != 0) {
            // format supported
            if (outFormats) {
                if (fmtCount < *count) {
                    PalFormatInfo* fmtInfo = &outFormats[fmtCount++];
                    fmtInfo->format = (PalFormat)i;
                    fmtInfo->usages = ImageUsageFromVk(props.optimalTilingFeatures);
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

bool PAL_CALL isFormatSupportedVk(
    PalAdapter* adapter,
    PalFormat format)
{
    Adapter* vkAdapter = (Adapter*)adapter;
    VkPhysicalDevice phyDevice = (VkPhysicalDevice)vkAdapter->handle;
    VkFormatProperties props = {0};

    VkFormat fmt = formatToVk(format);
    s_Vk.getPhysicalDeviceFormatProperties(phyDevice, fmt, &props);
    if (props.optimalTilingFeatures != 0) {
        return true;
    }
    return false;
}

PalImageUsages PAL_CALL queryFormatImageUsagesVk(
    PalAdapter* adapter,
    PalFormat format)
{
    Adapter* vkAdapter = (Adapter*)adapter;
    VkPhysicalDevice phyDevice = (VkPhysicalDevice)vkAdapter->handle;
    VkFormatProperties props = {0};

    VkFormat fmt = formatToVk(format);
    s_Vk.getPhysicalDeviceFormatProperties(phyDevice, fmt, &props);
    if (props.optimalTilingFeatures == 0) {
        return PAL_IMAGE_USAGE_UNDEFINED;
    }

    return ImageUsageFromVk(props.optimalTilingFeatures);
}

PalSampleCount PAL_CALL queryFormatSampleCountVk(
    PalAdapter* adapter,
    PalFormat format)
{
    VkResult result;
    Adapter* vkAdapter = (Adapter*)adapter;
    VkFormatProperties props = {0};
    VkImageFormatProperties formatProps = {0};

    VkFormat fmt = formatToVk(format);
    s_Vk.getPhysicalDeviceFormatProperties(vkAdapter->handle, fmt, &props);
    if (props.optimalTilingFeatures == 0) {
        return PAL_SAMPLE_COUNT_1;
    }

    VkImageUsageFlags vkImageUsage = 0;
    PalImageUsages imageUsages = ImageUsageFromVk(props.optimalTilingFeatures);
    bool isDepth = (imageUsages & PAL_IMAGE_USAGE_DEPTH_ATTACHEMENT) != 0;
    if (isDepth) {
        vkImageUsage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
    } else {
        vkImageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    }

    result = s_Vk.getPhysicalDeviceImageFormatProperties(
        vkAdapter->handle,
        fmt, 
        VK_IMAGE_TYPE_2D, 
        VK_IMAGE_TILING_OPTIMAL,
        vkImageUsage,
        0,
        &formatProps);

    if (result != VK_SUCCESS) {
        return PAL_SAMPLE_COUNT_1;
    }

    return samplesFromVk(formatProps.sampleCounts);
}

// ==================================================
// Image
// ==================================================

PalResult PAL_CALL createImageVk(
    PalDevice* device,
    const PalImageCreateInfo* info,
    PalImage** outImage)
{
    VkResult result;
    Image* image = nullptr;
    Device* vkDevice = (Device*)device;

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

    createInfo.format = formatToVk(info->format);
    createInfo.samples = samplesToVk(info->sampleCount);
    createInfo.usage = imageUsageToVk(info->usages);
    createInfo.arrayLayers = info->depthOrArraySize;
    createInfo.extent.depth = 1;

    createInfo.imageType = VK_IMAGE_TYPE_2D;
    if (info->type == PAL_IMAGE_TYPE_3D) {
        createInfo.arrayLayers = 1;
        createInfo.extent.depth = info->depthOrArraySize;
        createInfo.imageType = VK_IMAGE_TYPE_3D;

    } else if (info->type == PAL_IMAGE_TYPE_1D) {
        createInfo.imageType = VK_IMAGE_TYPE_1D;
    }

    result = s_Vk.createImage(vkDevice->handle, &createInfo, &s_Vk.vkAllocator, &image->handle);
    if (result != VK_SUCCESS) {
        palFree(s_Vk.allocator, image);
        return resultFromVk(result);
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

    image->memory = nullptr;
    *outImage = (PalImage*)image;
    return PAL_RESULT_SUCCESS;
}

void PAL_CALL destroyImageVk(PalImage* image)
{
    Image* vkImage = (Image*)image;
    if (vkImage->belongsToSwapchain) {
        return;
    }

    s_Vk.destroyImage(vkImage->device->handle, vkImage->handle, &s_Vk.vkAllocator);
    palFree(s_Vk.allocator, vkImage);
}

PalResult PAL_CALL getImageInfoVk(
    PalImage* image,
    PalImageInfo* info)
{
    Image* vkImage = (Image*)image;
    *info = vkImage->info;
    return PAL_RESULT_SUCCESS;
}

PalResult PAL_CALL getImageMemoryRequirementsVk(
    PalImage* image,
    PalMemoryRequirements* requirements)
{
    Image* vkImage = (Image*)image;
    if (vkImage->belongsToSwapchain) {
        return PAL_RESULT_INVALID_OPERATION;
    }

    Device* device = vkImage->device;
    VkMemoryRequirements memReq = {0};
    s_Vk.getImageMemoryRequirements(device->handle, vkImage->handle, &memReq);
    requirements->alignment = (Uint64)memReq.alignment;
    requirements->size = (Uint64)memReq.size;
    requirements->memoryMask = palPackUint32(memReq.memoryTypeBits, 0);

    requirements->memoryTypes[PAL_MEMORY_TYPE_GPU_ONLY] = false;
    requirements->memoryTypes[PAL_MEMORY_TYPE_CPU_UPLOAD] = false;
    requirements->memoryTypes[PAL_MEMORY_TYPE_CPU_READBACK] = false;

    for (int i = 0; i < PAL_MEMORY_TYPE_MAX; i++) {
        requirements->memoryTypes[i] = (memReq.memoryTypeBits & device->memoryClassMask[i]) != 0;
    }

    return PAL_RESULT_SUCCESS;
}

PalResult PAL_CALL bindImageMemoryVk(
    PalImage* image,
    PalMemory* memory,
    Uint64 offset)
{
    Image* vkImage = (Image*)image;
    if (vkImage->belongsToSwapchain) {
        return PAL_RESULT_INVALID_OPERATION;
    }

    if (vkImage->memory) {
        return PAL_RESULT_INVALID_OPERATION;
    }

    Memory* vkMemory = (Memory*)memory;
    s_Vk.bindImageMemory(vkImage->device->handle, vkImage->handle, vkMemory->handle, offset);
    vkImage->memory = vkMemory;
    return PAL_RESULT_SUCCESS;
}

PalResult PAL_CALL mapImageMemoryVk(
    PalImage* image,
    Uint64 offset,
    Uint64 size,
    void** outPtr)
{
    VkResult result;
    Image* vkImage = (Image*)image;
    Device* device = vkImage->device;

    if (vkImage->memory->type == PAL_MEMORY_TYPE_GPU_ONLY) {
        return PAL_RESULT_MEMORY_MAP_FAILED;
    }

    result = s_Vk.mapMemory(device->handle, vkImage->memory->handle, offset, size, 0, outPtr);
    if (result != VK_SUCCESS) {
        return resultFromVk(result);
    }
    return PAL_RESULT_SUCCESS;
}

void PAL_CALL unmapImageMemoryVk(PalImage* image)
{
    Image* vkImage = (Image*)image;
    s_Vk.unmapMemory(vkImage->device->handle, vkImage->memory->handle);
}

// ==================================================
// Image View
// ==================================================

PalResult PAL_CALL createImageViewVk(
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
        if (!(vkDevice->features & PAL_ADAPTER_FEATURE_IMAGE_VIEW_CUBE_ARRAY)) {
            return PAL_RESULT_ADAPTER_FEATURE_NOT_SUPPORTED;
        }
    }

    imageView = palAllocate(s_Vk.allocator, sizeof(ImageView), 0);
    if (!imageView) {
        return PAL_RESULT_OUT_OF_MEMORY;
    }

    VkImageViewCreateInfo createInfo = {0};
    createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    createInfo.format = formatToVk(info->format);
    createInfo.image = vkImage->handle;
    createInfo.viewType = imageViewTypeToVk(info->type);

    createInfo.subresourceRange.aspectMask = imageAspectToVk(info->subresourceRange.aspect);
    createInfo.subresourceRange.baseArrayLayer = info->subresourceRange.startArrayLayer;
    createInfo.subresourceRange.baseMipLevel = info->subresourceRange.startMipLevel;
    createInfo.subresourceRange.levelCount = info->subresourceRange.mipLevelCount;
    createInfo.subresourceRange.layerCount = info->subresourceRange.layerArrayCount;

    result = s_Vk.createImageView(
        vkDevice->handle,
        &createInfo,
        &s_Vk.vkAllocator,
        &imageView->handle);

    if (result != VK_SUCCESS) {
        palFree(s_Vk.allocator, imageView);
        return resultFromVk(result);
    }

    imageView->device = vkDevice;
    imageView->image = vkImage;
    imageView->layerCount = createInfo.subresourceRange.layerCount;

    *outImageView = (PalImageView*)imageView;
    return PAL_RESULT_SUCCESS;
}

void PAL_CALL destroyImageViewVk(PalImageView* imageView)
{
    ImageView* vkImageView = (ImageView*)imageView;
    s_Vk.destroyImageView(vkImageView->device->handle, vkImageView->handle, &s_Vk.vkAllocator);

    palFree(s_Vk.allocator, vkImageView);
}

// ==================================================
// Sampler
// ==================================================

PalResult PAL_CALL createSamplerVk(
    PalDevice* device,
    const PalSamplerCreateInfo* info,
    PalSampler** outSampler)
{
    VkResult result = VK_SUCCESS;
    Sampler* sampler = nullptr;
    Device* vkDevice = (Device*)device;

    sampler = palAllocate(s_Vk.allocator, sizeof(Sampler), 0);
    if (!sampler) {
        return PAL_RESULT_OUT_OF_MEMORY;
    }
    
    VkSamplerCreateInfo createInfo = {0};
    createInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    createInfo.anisotropyEnable = info->enableAnisotropy;
    createInfo.compareEnable = info->enableCompare;

    createInfo.mipLodBias = info->mipLodBias;
    createInfo.minLod = info->minLod;
    createInfo.maxLod = info->maxLod;
    createInfo.maxAnisotropy = info->maxAnisotropy;
    createInfo.compareOp = compareOpToVk(info->compareOp);

    createInfo.minFilter = filterToVk(info->minFilterMode);
    createInfo.magFilter = filterToVk(info->magFilterMode);
    createInfo.mipmapMode = mipmapModeToVk(info->mipmapMode);

    createInfo.addressModeU = addressModeToVk(info->addressModeU);
    createInfo.addressModeV = addressModeToVk(info->addressModeV);
    createInfo.addressModeW = addressModeToVk(info->addressModeW);
    createInfo.borderColor = borderColorToVk(info->borderColor);

    result = s_Vk.createSampler(
        vkDevice->handle,
        &createInfo,
        &s_Vk.vkAllocator,
        &sampler->handle);

    if (result != VK_SUCCESS) {
        palFree(s_Vk.allocator, sampler);
        return resultFromVk(result);
    }

    sampler->device = vkDevice;
    *outSampler = (PalSampler*)sampler;
    return PAL_RESULT_SUCCESS;
}

void PAL_CALL destroySamplerVk(PalSampler* sampler)
{
    Sampler* vkSampler = (Sampler*)sampler;
    s_Vk.destroySampler(vkSampler->device->handle, vkSampler->handle, &s_Vk.vkAllocator);

    palFree(s_Vk.allocator, vkSampler);
}

// ==================================================
// Surface
// ==================================================

PalResult PAL_CALL createSurfaceVk(
    PalDevice* device,
    PalGraphicsWindow* window,
    PalSurface** outSurface)
{
    VkResult result;
    Surface* surface = nullptr;
    Device* vkDevice = (Device*)device;

    surface = palAllocate(s_Vk.allocator, sizeof(Surface), 0);
    if (!surface) {
        return PAL_RESULT_OUT_OF_MEMORY;
    }

#ifdef __WIN32
    if (!s_Vk.createWin32Surface) {
        return PAL_RESULT_ADAPTER_FEATURE_NOT_SUPPORTED;
    }

    VkWin32SurfaceCreateInfoKHR cInfo = {0};
    cInfo.hinstance = GetModuleHandle(nullptr);
    cInfo.hwnd = window->window;
    cInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;

    VkSurfaceKHR tmp = nullptr;
    result = s_Vk.createWin32Surface(s_Vk.instance, &cInfo, &s_Vk.vkAllocator, &tmp);
    if (result != VK_SUCCESS) {
        return resultFromVk(result);
    }

    surface->device = vkDevice;
    surface->handle = tmp;
    *outSurface = (PalSurface*)surface;
    return PAL_RESULT_SUCCESS;

#else
    if (window->displayType == PAL_GRAPHICS_WINDOW_DISPLAY_TYPE_WAYLAND) {
        if (!s_Vk.createWaylandSurface) {
            return PAL_RESULT_ADAPTER_FEATURE_NOT_SUPPORTED;
        }

        VkWaylandSurfaceCreateInfoKHR cInfo = {0};
        cInfo.display = window->display;
        cInfo.pNext = nullptr;
        cInfo.flags = 0;
        cInfo.sType = VK_STRUCTURE_TYPE_WAYLAND_SURFACE_CREATE_INFO_KHR;
        cInfo.surface = window->window;

        VkSurfaceKHR tmp = nullptr;
        result = s_Vk.createWaylandSurface(s_Vk.instance, &cInfo, &s_Vk.vkAllocator, &tmp);
        if (result != VK_SUCCESS) {
            return resultFromVk(result);
        }

        surface->device = vkDevice;
        surface->handle = tmp;
        *outSurface = (PalSurface*)surface;
        return PAL_RESULT_SUCCESS;

    } else if (window->displayType == PAL_GRAPHICS_WINDOW_DISPLAY_TYPE_X11) {
        if (!s_Vk.createXlibSurface) {
            return PAL_RESULT_ADAPTER_FEATURE_NOT_SUPPORTED;
        }

        VkXlibSurfaceCreateInfoKHR cInfo = {0};
        cInfo.dpy = window->display;
        cInfo.window = (Window)(UintPtr)(window->window);
        cInfo.sType = VK_STRUCTURE_TYPE_XLIB_SURFACE_CREATE_INFO_KHR;

        VkSurfaceKHR tmp = nullptr;
        result = s_Vk.createXlibSurface(s_Vk.instance, &cInfo, &s_Vk.vkAllocator, &tmp);
        if (result != VK_SUCCESS) {
            return resultFromVk(result);
        }

        surface->device = vkDevice;
        surface->handle = tmp;
        *outSurface = (PalSurface*)surface;
        return PAL_RESULT_SUCCESS;

    } else if (window->displayType == PAL_GRAPHICS_WINDOW_DISPLAY_TYPE_XCB) {
        if (!s_Vk.createXcbSurface) {
            return PAL_RESULT_ADAPTER_FEATURE_NOT_SUPPORTED;
        }

        VkXcbSurfaceCreateInfoKHR cInfo = {0};
        cInfo.connection = window->display;
        cInfo.sType = VK_STRUCTURE_TYPE_XCB_SURFACE_CREATE_INFO_KHR;

        VkSurfaceKHR tmp = nullptr;
        result = s_Vk.createXcbSurface(s_Vk.instance, &cInfo, &s_Vk.vkAllocator, &tmp);
        if (result != VK_SUCCESS) {
            return resultFromVk(result);
        }

        surface->device = vkDevice;
        surface->handle = tmp;
        *outSurface = (PalSurface*)surface;
        return PAL_RESULT_SUCCESS;
    }

#endif // __WIN32
}

void PAL_CALL destroySurfaceVk(PalSurface* surface)
{
    Surface* vkSurface = (Surface*)surface;
    s_Vk.destroySurface(s_Vk.instance, vkSurface->handle, &s_Vk.vkAllocator);

    palFree(s_Vk.allocator, vkSurface);
}

PalResult PAL_CALL getSurfaceCapabilitiesVk(
    PalDevice* device,
    PalSurface* surface,
    PalSurfaceCapabilities* caps)
{
    Int32 formatCount = 0;
    Int32 modeCount = 0;
    Surface* vkSurface = (Surface*)surface;
    VkSurfaceFormatKHR* formats = nullptr;
    VkPresentModeKHR* modes = nullptr;

    Device* vkDevice = (Device*)device;
    VkPhysicalDevice phyDevice = (VkPhysicalDevice)vkDevice->phyDevice;

    if (!(vkDevice->features & PAL_ADAPTER_FEATURE_SWAPCHAIN)) {
        return PAL_RESULT_ADAPTER_FEATURE_NOT_SUPPORTED;
    }

    s_Vk.getSurfacePresentModes(phyDevice, vkSurface->handle, &modeCount, nullptr);
    s_Vk.getSurfaceFormats(phyDevice, vkSurface->handle, &formatCount, nullptr);

    modes = palAllocate(s_Vk.allocator, sizeof(VkPresentModeKHR) * modeCount, 0);
    formats = palAllocate(s_Vk.allocator, sizeof(VkSurfaceFormatKHR) * formatCount, 0);
    if (!modes || !formats) {
        return PAL_RESULT_OUT_OF_MEMORY;
    }

    s_Vk.getSurfacePresentModes(phyDevice, vkSurface->handle, &modeCount, modes);
    s_Vk.getSurfaceFormats(phyDevice, vkSurface->handle, &formatCount, formats);

    VkSurfaceCapabilitiesKHR surfaceCaps;
    s_Vk.getSurfaceCapabilities(phyDevice, vkSurface->handle, &surfaceCaps);

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

    // get format and colorspace
    caps->formats[PAL_SURFACE_FORMAT_RGBA16_FLOAT_HDR10] = false;
    caps->formats[PAL_SURFACE_FORMAT_RGBA8_UNORM_SRGB_NONLINEAR] = false;
    caps->formats[PAL_SURFACE_FORMAT_BGRA8_SRGB_NONLINEAR] = false;
    caps->formats[PAL_SURFACE_FORMAT_BGRA8_UNORM_SRGB_NONLINEAR] = false;

    for (int i = 0; i < formatCount; i++) {
        VkSurfaceFormatKHR* fmt = &formats[i];
        if (fmt->format == VK_FORMAT_B8G8R8A8_UNORM) {
            // find its supported colorspace
            if (fmt->colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
                caps->formats[PAL_SURFACE_FORMAT_BGRA8_UNORM_SRGB_NONLINEAR] = true;
            }

        } else if (fmt->format == VK_FORMAT_B8G8R8A8_SRGB) {
            // find its supported colorspace
            if (fmt->colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
                caps->formats[PAL_SURFACE_FORMAT_BGRA8_SRGB_NONLINEAR] = true;
            }

        } else if (fmt->format == VK_FORMAT_R8G8B8A8_UNORM) {
            // find its supported colorspace
            if (fmt->colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
                caps->formats[PAL_SURFACE_FORMAT_RGBA8_UNORM_SRGB_NONLINEAR] = true;
            }

        } else if (fmt->format == VK_FORMAT_R16G16B16A16_SFLOAT) {
            // find its supported colorspace
            if (fmt->colorSpace == VK_COLOR_SPACE_HDR10_ST2084_EXT) {
                caps->formats[PAL_SURFACE_FORMAT_RGBA16_FLOAT_HDR10] = true;
            }
        }
    }

    palFree(s_Vk.allocator, formats);
    palFree(s_Vk.allocator, modes);
    return PAL_RESULT_SUCCESS;
}

// ==================================================
// Swapchain
// ==================================================

PalResult PAL_CALL createSwapchainVk(
    PalDevice* device,
    PalQueue* queue,
    PalSurface* surface,
    const PalSwapchainCreateInfo* info,
    PalSwapchain** outSwapchain)
{
    PalFormat imageFormat = 0;
    Swapchain* swapchain = nullptr;
    VkImage* images = nullptr;

    Device* vkDevice = (Device*)device;
    Queue* vkQueue = (Queue*)queue;
    PhysicalQueue* phyQueue = vkQueue->phyQueue;
    Surface* vkSurface = (Surface*)surface;

    if (!(vkDevice->features & PAL_ADAPTER_FEATURE_SWAPCHAIN)) {
        return PAL_RESULT_ADAPTER_FEATURE_NOT_SUPPORTED;
    }

    // check if the queue is a graphics queue before we check its family
    // index for presentation support.
    if (vkQueue->usage != VK_QUEUE_GRAPHICS_BIT) {
        return PAL_RESULT_INVALID_QUEUE;
    }

    swapchain = palAllocate(s_Vk.allocator, sizeof(Swapchain), 0);
    if (!swapchain) {
        return PAL_RESULT_OUT_OF_MEMORY;
    }
    memset(swapchain, 0, sizeof(Swapchain));

    VkSwapchainCreateInfoKHR createInfo = {0};
    createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.surface = vkSurface->handle;
    createInfo.imageArrayLayers = info->imageArrayLayerCount;
    createInfo.imageExtent.width = info->width;
    createInfo.imageExtent.height = info->height;
    createInfo.minImageCount = info->imageCount;
    createInfo.clipped = info->clipped;
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

    if (info->format == PAL_SURFACE_FORMAT_BGRA8_SRGB_NONLINEAR) {
        createInfo.imageFormat = VK_FORMAT_B8G8R8A8_SRGB;
        createInfo.imageColorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
        imageFormat = PAL_FORMAT_B8G8R8A8_SRGB;

    } else if (info->format == PAL_SURFACE_FORMAT_RGBA8_UNORM_SRGB_NONLINEAR) {
        createInfo.imageFormat = VK_FORMAT_R8G8B8A8_UNORM;
        createInfo.imageColorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
        imageFormat = PAL_FORMAT_R8G8B8A8_UNORM;

    } else if (info->format == PAL_SURFACE_FORMAT_RGBA16_FLOAT_HDR10) {
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
        palFree(s_Vk.allocator, swapchain);
        return resultFromVk(result);
    }

    // get and cache all images
    Int32 count = 0;
    result = vkDevice->getSwapchainImages(vkDevice->handle, swapchain->handle, &count, nullptr);

    swapchain->images = palAllocate(s_Vk.allocator, sizeof(Image) * count, 0);
    images = palAllocate(s_Vk.allocator, sizeof(VkImage) * count, 0);
    if (!swapchain->images || !images) {
        vkDevice->destroySwapchain(vkDevice->handle, swapchain->handle, &s_Vk.vkAllocator);
        palFree(s_Vk.allocator, swapchain);
        return PAL_RESULT_OUT_OF_MEMORY;
    }
    vkDevice->getSwapchainImages(vkDevice->handle, swapchain->handle, &count, images);

    // fill all images with the creatio info
    for (int i = 0; i < count; i++) {
        Image* image = &swapchain->images[i];
        image->belongsToSwapchain = true;
        image->device = vkDevice;
        image->handle = images[i];

        image->info.depthOrArraySize = createInfo.imageArrayLayers;
        image->info.format = imageFormat;
        image->info.usages = PAL_IMAGE_USAGE_COLOR_ATTACHEMENT;
        image->info.height = createInfo.imageExtent.height;
        image->info.width = createInfo.imageExtent.width;
        image->info.mipLevelCount = 1;
        image->info.sampleCount = PAL_SAMPLE_COUNT_1; // swapchain images are not multisampled
        image->info.type = PAL_IMAGE_TYPE_2D;
    }

    palFree(s_Vk.allocator, images);
    swapchain->device = vkDevice;
    swapchain->queue = vkQueue;
    swapchain->imageCount = count;

    *outSwapchain = (PalSwapchain*)swapchain;
    return PAL_RESULT_SUCCESS;
}

void PAL_CALL destroySwapchainVk(PalSwapchain* swapchain)
{
    Swapchain* vkSwapchain = (Swapchain*)swapchain;
    vkSwapchain->device->destroySwapchain(
        vkSwapchain->device->handle,
        vkSwapchain->handle,
        &s_Vk.vkAllocator);

    palFree(s_Vk.allocator, vkSwapchain->images);
    palFree(s_Vk.allocator, vkSwapchain);
}

PalImage* PAL_CALL getSwapchainImageVk(
    PalSwapchain* swapchain,
    Int32 index)
{
    Swapchain* vkSwapchain = (Swapchain*)swapchain;
    if (index > vkSwapchain->imageCount) {
        return nullptr;
    }
    return (PalImage*)&vkSwapchain->images[index];
}

PalResult PAL_CALL getNextSwapchainImageVk(
    PalSwapchain* swapchain,
    PalSwapchainNextImageInfo* info,
    Uint32* outIndex)
{
    VkResult result;
    Uint32 index = 0;
    Uint64 timeInNanoseconds = 0;
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

    if (info->timeout) {
        if (info->timeout == PAL_INFINITE) {
            timeInNanoseconds = UINT64_MAX;
        } else {
            timeInNanoseconds = info->timeout * 1000000;
        }
    }

    result = vkSwapchain->device->acquireNextImage(
        vkSwapchain->device->handle,
        vkSwapchain->handle,
        timeInNanoseconds,
        semaphoreHandle,
        fenceHandle,
        &index);

    if (result != VK_SUCCESS) {
        return resultFromVk(result);
    }

    *outIndex = index;
    return PAL_RESULT_SUCCESS;
}

PalResult PAL_CALL presentSwapchainVk(
    PalSwapchain* swapchain,
    PalSwapchainPresentInfo* info)
{
    Swapchain* vkSwapchain = (Swapchain*)swapchain;
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
    presentInfo.pImageIndices = &info->imageIndex;
    presentInfo.pWaitSemaphores = &semaphoreHandle;
    presentInfo.waitSemaphoreCount = semaphoreCount;

    result = vkSwapchain->device->queuePresent(vkSwapchain->queue->phyQueue->handle, &presentInfo);
    if (result != VK_SUCCESS) {
        return resultFromVk(result);
    }

    return PAL_RESULT_SUCCESS;
}

PalResult PAL_CALL resizeSwapchainVk(
    PalSwapchain* swapchain,
    Uint32 newWidth,
    Uint32 newHeight)
{
    VkResult result;
    Swapchain* vkSwapchain = (Swapchain*)swapchain;
    VkSwapchainKHR oldSwapchain = vkSwapchain->handle;
    Device* device = vkSwapchain->device;
    VkImage* images = nullptr;

    VkSwapchainCreateInfoKHR createInfo = {0};
    createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.oldSwapchain = oldSwapchain;
    createInfo.imageExtent.width = newWidth;
    createInfo.imageExtent.height = newHeight;

    result = device->createSwapchain(
        device->handle,
        &createInfo,
        &s_Vk.vkAllocator,
        &vkSwapchain->handle);

    if (result != VK_SUCCESS) {
        return resultFromVk(result);
    }

    Uint32 count = vkSwapchain->imageCount;
    images = palAllocate(s_Vk.allocator, sizeof(VkImage) * count, 0);
    if (!images) {
        return PAL_RESULT_OUT_OF_MEMORY;
    }

    device->destroySwapchain(device->handle, oldSwapchain, &s_Vk.vkAllocator);
    device->getSwapchainImages(device->handle, vkSwapchain->handle, &count, images);

    // fill all images with the creatio info
    for (int i = 0; i < count; i++) {
        Image* image = &vkSwapchain->images[i];
        image->handle = images[i];
        image->info.height = createInfo.imageExtent.height;
        image->info.width = createInfo.imageExtent.width;
    }

    palFree(s_Vk.allocator, images);
    return PAL_RESULT_SUCCESS;
}

// ==================================================
// Shader
// ==================================================

PalResult PAL_CALL createShaderVk(
    PalDevice* device,
    const PalShaderCreateInfo* info,
    PalShader** outShader)
{
    VkResult result;
    Shader* shader = nullptr;
    VkShaderStageFlags stage = 0;
    Device* vkDevice = (Device*)device;

    stage = shaderStageToVK(info->stage);
    if (info->stage == PAL_SHADER_STAGE_MESH || info->stage == PAL_SHADER_STAGE_TASK) {
        if (!(vkDevice->features & PAL_ADAPTER_FEATURE_MESH_SHADER)) {
            return PAL_RESULT_ADAPTER_FEATURE_NOT_SUPPORTED;
        }

    // clang-format off
    } else if (info->stage == PAL_SHADER_STAGE_RAYGEN ||
               info->stage == PAL_SHADER_STAGE_CLOSEST_HIT ||
               info->stage == PAL_SHADER_STAGE_ANY_HIT ||
               info->stage == PAL_SHADER_STAGE_MISS ||
               info->stage == PAL_SHADER_STAGE_INTERSECTION ||
               info->stage == PAL_SHADER_STAGE_CALLABLE) {
        if (!(vkDevice->features & PAL_ADAPTER_FEATURE_RAY_TRACING)) {
            return PAL_RESULT_ADAPTER_FEATURE_NOT_SUPPORTED;
        }
    }
    // clang-format on

    shader = palAllocate(s_Vk.allocator, sizeof(Shader), 0);
    if (!shader) {
        return PAL_RESULT_OUT_OF_MEMORY;
    }
    memset(shader, 0, sizeof(Shader));

    VkShaderModuleCreateInfo createInfo = {0};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.codeSize = info->bytecodeSize;
    createInfo.pCode = (const Uint32*)info->bytecode;

    result = s_Vk.createShader(vkDevice->handle, &createInfo, &s_Vk.vkAllocator, &shader->handle);
    if (result != VK_SUCCESS) {
        palFree(s_Vk.allocator, shader);
        return resultFromVk(result);
    }

    shader->device = vkDevice;
    shader->patchControlPoints = info->patchControlPoints;
    shader->info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    shader->info.module = shader->handle;
    shader->info.pName = "main";
    shader->info.stage = stage;

    *outShader = (PalShader*)shader;
    return PAL_RESULT_SUCCESS;
}

void PAL_CALL destroyShaderVk(PalShader* shader)
{
    Shader* vkShader = (Shader*)shader;
    s_Vk.destroyShader(vkShader->device->handle, vkShader->handle, &s_Vk.vkAllocator);

    palFree(s_Vk.allocator, vkShader);
}

// ==================================================
// Fence
// ==================================================

PalResult PAL_CALL createFenceVk(
    PalDevice* device,
    bool signaled,
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
    if (signaled) {
        createInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
    }

    result = s_Vk.createFence(vkDevice->handle, &createInfo, &s_Vk.vkAllocator, &fence->handle);
    if (result != VK_SUCCESS) {
        palFree(s_Vk.allocator, fence);
        return resultFromVk(result);
    }

    fence->device = vkDevice;
    *outFence = (PalFence*)fence;
    return PAL_RESULT_SUCCESS;
}

void PAL_CALL destroyFenceVk(PalFence* fence)
{
    Fence* vkFence = (Fence*)fence;
    s_Vk.destroyFence(vkFence->device->handle, vkFence->handle, &s_Vk.vkAllocator);

    palFree(s_Vk.allocator, vkFence);
}

PalResult PAL_CALL waitFenceVk(
    PalFence* fence,
    Uint64 timeout)
{
    Fence* vkFence = (Fence*)fence;
    VkResult result;
    Uint64 timeInNanoseconds = 0;

    if (timeout) {
        if (timeout == PAL_INFINITE) {
            timeInNanoseconds = UINT64_MAX;
        } else {
            timeInNanoseconds = timeout * 1000000;
        }
    }

    result = s_Vk.waitFence(vkFence->device->handle, 1, &vkFence->handle, true, timeInNanoseconds);
    if (result != VK_SUCCESS) {
        return resultFromVk(result);
    }

    return PAL_RESULT_SUCCESS;
}

PalResult PAL_CALL resetFenceVk(PalFence* fence)
{
    Fence* vkFence = (Fence*)fence;
    if (!(vkFence->device->features & PAL_ADAPTER_FEATURE_FENCE_RESET)) {
        return PAL_RESULT_ADAPTER_FEATURE_NOT_SUPPORTED;
    }

    VkResult result = s_Vk.resetFence(vkFence->device->handle, 1, &vkFence->handle);
    if (result != VK_SUCCESS) {
        return resultFromVk(result);
    }

    return PAL_RESULT_SUCCESS;
}

bool PAL_CALL isFenceSignaledVk(PalFence* fence)
{
    Fence* vkFence = (Fence*)fence;
    VkResult result = s_Vk.isFenceSignaled(vkFence->device->handle, vkFence->handle);
    if (result == VK_SUCCESS) {
        return true;
    } else {
        return false;
    }
}

// ==================================================
// Semaphore
// ==================================================

PalResult PAL_CALL createSemaphoreVk(
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
        return resultFromVk(result);
    }

    semaphore->device = vkDevice;
    *outSemaphore = (PalSemaphore*)semaphore;
    return PAL_RESULT_SUCCESS;
}

void PAL_CALL destroySemaphoreVk(PalSemaphore* semaphore)
{
    Semaphore* vkSemaphore = (Semaphore*)semaphore;
    s_Vk.destroySemaphore(vkSemaphore->device->handle, vkSemaphore->handle, &s_Vk.vkAllocator);

    palFree(s_Vk.allocator, vkSemaphore);
}

PalResult PAL_CALL waitSemaphoreVk(
    PalSemaphore* semaphore,
    Uint64 value,
    Uint64 timeout)
{
    VkResult result;
    Uint64 timeInNanoseconds = 0;
    Semaphore* vkSemaphore = (Semaphore*)semaphore;
    if (!(vkSemaphore->device->features & PAL_ADAPTER_FEATURE_TIMELINE_SEMAPHORE)) {
        return PAL_RESULT_ADAPTER_FEATURE_NOT_SUPPORTED;
    }

    if (timeout) {
        if (timeout == PAL_INFINITE) {
            timeInNanoseconds = UINT64_MAX;
        } else {
            timeInNanoseconds = timeout * 1000000;
        }
    }

    VkSemaphoreWaitInfo waitInfo = {0};
    waitInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_WAIT_INFO;
    waitInfo.semaphoreCount = 1;
    waitInfo.pSemaphores = &vkSemaphore->handle;
    waitInfo.pValues = &value;

    result = vkSemaphore->device->waitSemaphore(
        vkSemaphore->device->handle, 
        &waitInfo, 
        timeInNanoseconds);

    if (result != VK_SUCCESS) {
        return resultFromVk(result);
    }

    return PAL_RESULT_SUCCESS;
}

PalResult PAL_CALL signalSemaphoreVk(
    PalSemaphore* semaphore,
    PalQueue* queue,
    Uint64 value)
{
    VkResult result;
    Semaphore* vkSemaphore = (Semaphore*)semaphore;
    if (!(vkSemaphore->device->features & PAL_ADAPTER_FEATURE_TIMELINE_SEMAPHORE)) {
        return PAL_RESULT_ADAPTER_FEATURE_NOT_SUPPORTED;
    }

    VkSemaphoreSignalInfo signalInfo = {0};
    signalInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_SIGNAL_INFO;
    signalInfo.semaphore = vkSemaphore->handle;
    signalInfo.value = value;

    result = vkSemaphore->device->signalSemaphore(vkSemaphore->device->handle, &signalInfo);
    if (result != VK_SUCCESS) {
        return resultFromVk(result);
    }

    return PAL_RESULT_SUCCESS;
}

PalResult PAL_CALL getSemaphoreValueVk(
    PalSemaphore* semaphore,
    Uint64* outValue)
{
    Semaphore* vkSemaphore = (Semaphore*)semaphore;
    if (!(vkSemaphore->device->features & PAL_ADAPTER_FEATURE_TIMELINE_SEMAPHORE)) {
        return PAL_RESULT_ADAPTER_FEATURE_NOT_SUPPORTED;
    }

    VkResult result = vkSemaphore->device->getSemaphoreValue(
        vkSemaphore->device->handle,
        vkSemaphore->handle,
        outValue);

    if (result != VK_SUCCESS) {
        return resultFromVk(result);
    }

    return PAL_RESULT_SUCCESS;
}

// ==================================================
// Command Pool And Buffer
// ==================================================

PalResult PAL_CALL createCommandPoolVk(
    PalDevice* device,
    PalQueue* queue,
    PalCommandPool** outPool)
{
    VkResult result;
    CommandPool* pool = nullptr;
    Device* vkDevice = (Device*)device;
    Queue* vkQueue = (Queue*)queue;
    PhysicalQueue* phyQueue = vkQueue->phyQueue;

    pool = palAllocate(s_Vk.allocator, sizeof(CommandPool), 0);
    if (!pool) {
        return PAL_RESULT_OUT_OF_MEMORY;
    }

    VkCommandPoolCreateInfo cInfo = {0};
    cInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    cInfo.queueFamilyIndex = phyQueue->familyIndex;
    cInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

    result = s_Vk.createCommandPool(vkDevice->handle, &cInfo, &s_Vk.vkAllocator, &pool->handle);
    if (result != VK_SUCCESS) {
        palFree(s_Vk.allocator, pool);
        return resultFromVk(result);
    }

    pool->device = vkDevice;
    *outPool = (PalCommandPool*)pool;
    return PAL_RESULT_SUCCESS;
}

void PAL_CALL destroyCommandPoolVk(PalCommandPool* pool)
{
    CommandPool* vkPool = (CommandPool*)pool;
    s_Vk.destroyCommandPool(vkPool->device->handle, vkPool->handle, &s_Vk.vkAllocator);

    palFree(s_Vk.allocator, vkPool);
}

PalResult PAL_CALL resetCommandPoolVk(PalCommandPool* pool)
{
    CommandPool* vkCmdPool = (CommandPool*)pool;
    s_Vk.resetCommandPool(vkCmdPool->device->handle, vkCmdPool->handle, 0);
    return PAL_RESULT_SUCCESS;
}

PalResult PAL_CALL allocateCommandBufferVk(
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

    VkCommandBufferAllocateInfo allocateInfo = {0};
    allocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocateInfo.commandBufferCount = 1;
    allocateInfo.commandPool = vkPool->handle;

    allocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    cmdBuffer->primary = true;
    if (type == PAL_COMMAND_BUFFER_TYPE_SECONDARY) {
        allocateInfo.level = VK_COMMAND_BUFFER_LEVEL_SECONDARY;
        cmdBuffer->primary = false;
    }

    result = s_Vk.allocateCommandBuffer(vkDevice->handle, &allocateInfo, &cmdBuffer->handle);
    if (result != VK_SUCCESS) {
        palFree(s_Vk.allocator, cmdBuffer);
        return resultFromVk(result);
    }

    cmdBuffer->device = vkDevice;
    cmdBuffer->pool = vkPool;

    *outCmdBuffer = (PalCommandBuffer*)cmdBuffer;
    return PAL_RESULT_SUCCESS;
}

void PAL_CALL freeCommandBufferVk(PalCommandBuffer* cmdBuffer)
{
    CommandBuffer* vkCmdBuffer = (CommandBuffer*)cmdBuffer;
    s_Vk.freeCommandBuffer(
        vkCmdBuffer->device->handle,
        vkCmdBuffer->pool->handle,
        1,
        &vkCmdBuffer->handle);

    palFree(s_Vk.allocator, vkCmdBuffer);
}

PalResult PAL_CALL resetCommandBufferVk(PalCommandBuffer* cmdBuffer)
{
    CommandBuffer* vkCmdBuffer = (CommandBuffer*)cmdBuffer;
    s_Vk.resetCommandBuffer(vkCmdBuffer->handle, 0);
    return PAL_RESULT_SUCCESS;
}

PalResult PAL_CALL submitCommandBufferVk(
    PalQueue* queue,
    PalCommandBufferSubmitInfo* info)
{
    VkResult result;
    Int32 waitSemaphoreCount = 0;
    Int32 signalSemaphoreCount = 0;
    VkFence fenceHandle = nullptr;
    VkSemaphore waitSemaphoreHandle = nullptr;
    VkSemaphore signalSemaphoreHandle = nullptr;
    Queue* vkQueue = (Queue*)queue;
    CommandBuffer* vkCmdBuffer = (CommandBuffer*)info->cmdBuffer;
    PhysicalQueue* phyQueue = vkQueue->phyQueue;

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

    VkCommandBufferSubmitInfoKHR cmdBufferSubmitInfo = {0};
    cmdBufferSubmitInfo.commandBuffer = vkCmdBuffer->handle;
    cmdBufferSubmitInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO_KHR;

    VkSemaphoreSubmitInfoKHR waitSubmitInfo = {0};
    waitSubmitInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO_KHR;
    waitSubmitInfo.semaphore = waitSemaphoreHandle;
    waitSubmitInfo.stageMask = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT_KHR;

    VkSemaphoreSubmitInfoKHR signalSubmitInfo = {0};
    signalSubmitInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO_KHR;
    signalSubmitInfo.semaphore = signalSemaphoreHandle;
    signalSubmitInfo.stageMask = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT_KHR;

    if (vkCmdBuffer->device->features & PAL_ADAPTER_FEATURE_TIMELINE_SEMAPHORE) {
        waitSubmitInfo.value = info->waitValue;
        signalSubmitInfo.value = info->signalValue;
    }

    VkSubmitInfo2KHR submitInfo = {0};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO_2_KHR;
    submitInfo.commandBufferInfoCount = 1;
    submitInfo.pCommandBufferInfos = &cmdBufferSubmitInfo;
    submitInfo.pSignalSemaphoreInfos = &signalSubmitInfo;
    submitInfo.pWaitSemaphoreInfos = &waitSubmitInfo;
    submitInfo.waitSemaphoreInfoCount = waitSemaphoreCount;
    submitInfo.signalSemaphoreInfoCount = signalSemaphoreCount;

    result = vkCmdBuffer->device->queueSubmit(phyQueue->handle, 1, &submitInfo, fenceHandle);
    if (result != VK_SUCCESS) {
        return resultFromVk(result);
    }

    return PAL_RESULT_SUCCESS;
}

// ==================================================
// Command Recording
// ==================================================

PalResult PAL_CALL cmdBeginVk(
    PalCommandBuffer* cmdBuffer,
    PalRenderingLayoutInfo* info)
{
    CommandBuffer* vkCmdBuffer = (CommandBuffer*)cmdBuffer;
    VkCommandBufferBeginInfo beginInfo = {0};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

    VkCommandBufferInheritanceInfo inheritanceInfo = {0};
    inheritanceInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_INHERITANCE_INFO;
    VkCommandBufferInheritanceRenderingInfoKHR layout = {0};
    layout.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_INHERITANCE_RENDERING_INFO_KHR;

    VkFormat format = VK_FORMAT_UNDEFINED;
    VkFormat colorAttachments[MAX_ATTACHMENTS];

    if (!vkCmdBuffer->primary) {
        // secondary command buffer
        for (int i = 0; i < info->colorAttachentCount; i++) {
            format = formatToVk(info->colorAttachmentsFormat[i]);
            colorAttachments[i] = format;
        }
        layout.colorAttachmentCount = info->colorAttachentCount;
        layout.pColorAttachmentFormats = colorAttachments;

        // depth stencil attachment
        format = formatToVk(info->depthStencilAttachmentFormat);
        layout.depthAttachmentFormat = format;
        layout.stencilAttachmentFormat = format;

        layout.rasterizationSamples = samplesToVk(info->multisampleCount);
        if (info->viewCount == 1) {
            layout.viewMask = 0;
        } else {
            layout.viewMask = (1 << info->viewCount) - 1;
        }

        inheritanceInfo.pNext = &layout;
        beginInfo.pNext = &inheritanceInfo;
    }

    VkResult result = s_Vk.cmdBegin(vkCmdBuffer->handle, &beginInfo);
    if (result != VK_SUCCESS) {
        return resultFromVk(result);
    }

    return PAL_RESULT_SUCCESS;
}

PalResult PAL_CALL cmdEndVk(PalCommandBuffer* cmdBuffer)
{
    CommandBuffer* vkCmdBuffer = (CommandBuffer*)cmdBuffer;
    VkResult result = s_Vk.cmdEnd(vkCmdBuffer->handle);
    if (result != VK_SUCCESS) {
        return resultFromVk(result);
    }

    return PAL_RESULT_SUCCESS;
}

PalResult PAL_CALL cmdExecuteCommandBufferVk(
    PalCommandBuffer* primaryCmdBuffer,
    PalCommandBuffer* secondaryCmdBuffer)
{
    CommandBuffer* vkCmdBuffer = (CommandBuffer*)primaryCmdBuffer;
    CommandBuffer* vkCmdBuffer2 = (CommandBuffer*)secondaryCmdBuffer;

    s_Vk.cmdExecuteCommandBuffer(vkCmdBuffer->handle, 1, &vkCmdBuffer2->handle);
    return PAL_RESULT_SUCCESS;
}

PalResult PAL_CALL cmdSetFragmentShadingRateVk(
    PalCommandBuffer* cmdBuffer,
    PalFragmentShadingRateState* state)
{
    CommandBuffer* vkCmdBuffer = (CommandBuffer*)cmdBuffer;
    Device* device = vkCmdBuffer->device;

    if (!(device->features & PAL_ADAPTER_FEATURE_FRAGMENT_SHADING_RATE)) {
        return PAL_RESULT_ADAPTER_FEATURE_NOT_SUPPORTED;
    }

    VkExtent2D size = getShadingRateSizeVk(state->rate);
    VkFragmentShadingRateCombinerOpKHR combinerOps[2];
    for (int i = 0; i < 2; i++) {
        combinerOps[i] = combinerOpsToVk(state->combinerOps[i]);
    }

    device->cmdSetFragmentShadingRate(vkCmdBuffer->handle, &size, combinerOps);
    return PAL_RESULT_SUCCESS;
}

PalResult PAL_CALL cmdDrawMeshTasksVk(
    PalCommandBuffer* cmdBuffer,
    Uint32 groupCountX,
    Uint32 groupCountY,
    Uint32 groupCountZ)
{
    CommandBuffer* vkCmdBuffer = (CommandBuffer*)cmdBuffer;
    Device* device = vkCmdBuffer->device;

    if (!(device->features & PAL_ADAPTER_FEATURE_MESH_SHADER)) {
        return PAL_RESULT_ADAPTER_FEATURE_NOT_SUPPORTED;
    }

    device->cmdDrawMeshTask(vkCmdBuffer->handle, groupCountX, groupCountY, groupCountZ);
    return PAL_RESULT_SUCCESS;
}

PalResult PAL_CALL cmdDrawMeshTasksIndirectVk(
    PalCommandBuffer* cmdBuffer,
    PalBuffer* buffer,
    Uint32 drawCount)
{
    CommandBuffer* vkCmdBuffer = (CommandBuffer*)cmdBuffer;
    Device* device = vkCmdBuffer->device;
    Buffer* vkBuffer = (Buffer*)buffer;

    if (!(device->features & PAL_ADAPTER_FEATURE_INDIRECT_DRAW_MESH)) {
        return PAL_RESULT_ADAPTER_FEATURE_NOT_SUPPORTED;
    }

    Uint32 stride = sizeof(VkDrawMeshTasksIndirectCommandEXT);
    device->cmdDrawMeshTaskIndirect(
        vkCmdBuffer->handle, 
        vkBuffer->handle,
        0, 
        drawCount,
        stride);

    return PAL_RESULT_SUCCESS;
}

PalResult PAL_CALL cmdDrawMeshTasksIndirectCountVk(
    PalCommandBuffer* cmdBuffer,
    PalBuffer* buffer,
    PalBuffer* countBuffer,
    Uint32 maxDrawCount)
{
    CommandBuffer* vkCmdBuffer = (CommandBuffer*)cmdBuffer;
    Device* device = vkCmdBuffer->device;
    Buffer* vkBuffer = (Buffer*)buffer;
    Buffer* vkCountBuffer = (Buffer*)countBuffer;

    if (!(device->features & PAL_ADAPTER_FEATURE_INDIRECT_DRAW_MESH_COUNT)) {
        return PAL_RESULT_ADAPTER_FEATURE_NOT_SUPPORTED;
    }

    Uint32 stride = sizeof(VkDrawMeshTasksIndirectCommandEXT);
    device->cmdDrawMeshTaskIndirectCount(
        vkCmdBuffer->handle,
        vkBuffer->handle,
        0,
        vkCountBuffer->handle,
        0,
        maxDrawCount,
        stride);

    return PAL_RESULT_SUCCESS;
}

PalResult PAL_CALL cmdBuildAccelerationStructureVk(
    PalCommandBuffer* cmdBuffer,
    PalAccelerationStructureBuildInfo* info)
{
    CommandBuffer* vkCmdBuffer = (CommandBuffer*)cmdBuffer;
    Device* device = vkCmdBuffer->device;
    VkAccelerationStructureGeometryKHR* geometries = nullptr;
    VkAccelerationStructureBuildRangeInfoKHR* rangeInfos = nullptr;
    AccelerationStructure* tmpAs = (AccelerationStructure*)info->src;
    AccelerationStructure* dstAs = (AccelerationStructure*)info->dst;
    VkAccelerationStructureKHR srcAs = nullptr;
    VkAccelerationStructureBuildGeometryInfoKHR buildInfo = {0};

    if (!(device->features & PAL_ADAPTER_FEATURE_RAY_TRACING)) {
        return PAL_RESULT_ADAPTER_FEATURE_NOT_SUPPORTED;
    }

    // cache these for top level as
    VkAccelerationStructureBuildRangeInfoKHR cachedRangeInfo = {0};
    VkAccelerationStructureGeometryKHR cachedGeometries = {0};
    Uint32 geometryCount = info->geometryCount;
    if (info->type == PAL_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL) {
        geometryCount = 1;
        rangeInfos = &cachedRangeInfo;
        geometries = &cachedGeometries;
    }

    if (tmpAs) {
        srcAs = tmpAs->handle;
    }

    if (info->type == PAL_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL) {
        geometries = palAllocate(
            s_Vk.allocator,
            sizeof(VkAccelerationStructureGeometryKHR) * geometryCount,
            0);

        rangeInfos = palAllocate(
            s_Vk.allocator,
            sizeof(VkAccelerationStructureBuildRangeInfoKHR) * geometryCount,
            0);

        if (!rangeInfos || !geometries) {
            return PAL_RESULT_OUT_OF_MEMORY;
        }

        memset(geometries, 0, sizeof(VkAccelerationStructureGeometryKHR) * geometryCount);
        memset(rangeInfos, 0, sizeof(VkAccelerationStructureBuildRangeInfoKHR) * geometryCount);
    }

    fillBuildInfoVk(
        geometryCount, 
        info, 
        nullptr, 
        geometries, 
        srcAs, 
        dstAs->handle, 
        rangeInfos, 
        &buildInfo);

    const VkAccelerationStructureBuildRangeInfoKHR* tmp[1];
    tmp[0] = rangeInfos;
    vkCmdBuffer->device->cmdBuildAccelerationStructures(vkCmdBuffer->handle, 1, &buildInfo, tmp);

    if (info->type == PAL_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL) {
        palFree(s_Vk.allocator, geometries);
        palFree(s_Vk.allocator, rangeInfos);
    }
    return PAL_RESULT_SUCCESS;
}

PalResult PAL_CALL cmdBeginRenderingVk(
    PalCommandBuffer* cmdBuffer,
    PalRenderingInfo* info)
{
    CommandBuffer* vkCmdBuffer = (CommandBuffer*)cmdBuffer;
    VkRenderingInfoKHR rendering = {0};
    rendering.sType = VK_STRUCTURE_TYPE_RENDERING_INFO_KHR;

    VkRenderingAttachmentInfoKHR depthAttachment = {0};
    VkRenderingAttachmentInfoKHR stencilAttachment = {0};
    VkRenderingAttachmentInfoKHR colorAttachments[MAX_ATTACHMENTS];

    VkRenderingAttachmentInfoKHR* attachment = nullptr;
    PalAttachmentDesc* desc = nullptr;
    ImageView* imageView = nullptr;
    ImageView* resolveImageView = nullptr;
    VkImageLayout layout = VK_IMAGE_LAYOUT_GENERAL;

    VkRenderingFragmentShadingRateAttachmentInfoKHR fsrInfo = {0};
    fsrInfo.sType = VK_STRUCTURE_TYPE_RENDERING_FRAGMENT_SHADING_RATE_ATTACHMENT_INFO_KHR;

    Uint32 layerCount = UINT32_MAX;
    Uint32 renderWidth = UINT32_MAX;
    Uint32 renderHeight = UINT32_MAX;
    for (int i = 0; i < info->colorAttachentCount; i++) {
        attachment = &colorAttachments[i];
        desc = &info->colorAttachments[i];
        imageView = (ImageView*)desc->imageView;
        resolveImageView = (ImageView*)desc->resolveImageView;

        attachment->sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO_KHR;
        attachment->pNext = nullptr;
        layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        attachment->resolveImageView = nullptr;
        attachment->resolveImageLayout = VK_IMAGE_LAYOUT_UNDEFINED;

        attachment->clearValue.color.float32[0] = desc->clearValue.color[0];
        attachment->clearValue.color.float32[1] = desc->clearValue.color[1];
        attachment->clearValue.color.float32[2] = desc->clearValue.color[2];
        attachment->clearValue.color.float32[3] = desc->clearValue.color[3];

        attachment->imageView = imageView->handle;
        if (resolveImageView) {
            attachment->resolveImageView = resolveImageView->handle;
            attachment->resolveImageLayout = layout;
        }

        // load op
        if (desc->loadOp == PAL_LOAD_OP_CLEAR) {
            attachment->loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;

        } else if (desc->loadOp == PAL_LOAD_OP_LOAD) {
            attachment->loadOp = VK_ATTACHMENT_LOAD_OP_LOAD;

        } else if (desc->loadOp == PAL_LOAD_OP_DONT_CARE) {
            attachment->loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        }

        // store op
        if (desc->storeOp == PAL_STORE_OP_STORE) {
            attachment->storeOp = VK_ATTACHMENT_STORE_OP_STORE;

        } else if (desc->storeOp == PAL_STORE_OP_DONT_CARE) {
            attachment->storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        }

        attachment->resolveMode = resolveModeToVk(desc->resolveMode);
        attachment->imageLayout = layout;

        // compute layer count and render area
        layerCount = minVk(layerCount, imageView->layerCount);
        renderWidth = minVk(renderWidth, imageView->image->info.width);
        renderHeight = minVk(renderHeight, imageView->image->info.height);

        if (resolveImageView) {
            layerCount = minVk(layerCount, resolveImageView->layerCount);
            renderWidth = minVk(renderWidth, resolveImageView->image->info.width);
            renderHeight = minVk(renderHeight, resolveImageView->image->info.height);
        }
    }

    rendering.colorAttachmentCount = info->colorAttachentCount;
    rendering.pColorAttachments = colorAttachments;

    // depth attachment
    if (info->depthStencilAttachment) {
        attachment = &depthAttachment;
        desc = info->depthStencilAttachment;
        imageView = (ImageView*)desc->imageView;
        resolveImageView = (ImageView*)desc->resolveImageView;

        attachment->sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO_KHR;
        attachment->pNext = nullptr;
        attachment->resolveImageView = nullptr;
        attachment->resolveImageLayout = VK_IMAGE_LAYOUT_UNDEFINED;

        stencilAttachment.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO_KHR;
        stencilAttachment.pNext = nullptr;
        stencilAttachment.resolveImageView = nullptr;
        stencilAttachment.resolveImageLayout = VK_IMAGE_LAYOUT_UNDEFINED;

        attachment->clearValue.depthStencil.depth = desc->clearValue.depth;
        stencilAttachment.clearValue.depthStencil.stencil = desc->clearValue.stencil;
        
        attachment->imageView = imageView->handle;
        stencilAttachment.imageView = imageView->handle;
        if (resolveImageView) {
            attachment->resolveImageView = resolveImageView->handle;
            attachment->resolveImageLayout = layout;

            stencilAttachment.resolveImageView = resolveImageView->handle;
            stencilAttachment.resolveImageLayout = layout;
        }

        // load op
        if (desc->loadOp == PAL_LOAD_OP_CLEAR) {
            attachment->loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;

        } else if (desc->loadOp == PAL_LOAD_OP_LOAD) {
            attachment->loadOp = VK_ATTACHMENT_LOAD_OP_LOAD;

        } else if (desc->loadOp == PAL_LOAD_OP_DONT_CARE) {
            attachment->loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        }

        // store op
        if (desc->storeOp == PAL_STORE_OP_STORE) {
            attachment->storeOp = VK_ATTACHMENT_STORE_OP_STORE;

        } else if (desc->storeOp == PAL_STORE_OP_DONT_CARE) {
            attachment->storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        }

        // stencil load op
        if (desc->stencilLoadOp == PAL_LOAD_OP_CLEAR) {
            stencilAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;

        } else if (desc->stencilLoadOp == PAL_LOAD_OP_LOAD) {
            stencilAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_LOAD;

        } else if (desc->stencilLoadOp == PAL_LOAD_OP_DONT_CARE) {
            stencilAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        }

        // stencil store op
        if (desc->stencilStoreOp == PAL_STORE_OP_STORE) {
            stencilAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;

        } else if (desc->stencilStoreOp == PAL_STORE_OP_DONT_CARE) {
            stencilAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        }

        attachment->resolveMode = resolveModeToVk(desc->resolveMode);
        attachment->imageLayout = VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL;
        stencilAttachment.resolveMode = resolveModeToVk(desc->resolveMode);
        stencilAttachment.imageLayout = VK_IMAGE_LAYOUT_STENCIL_ATTACHMENT_OPTIMAL;

        rendering.pDepthAttachment = &depthAttachment;
        rendering.pStencilAttachment = &stencilAttachment;

        // compute layer count and render area
        layerCount = minVk(layerCount, imageView->layerCount);
        renderWidth = minVk(renderWidth, imageView->image->info.width);
        renderHeight = minVk(renderHeight, imageView->image->info.height);

        if (resolveImageView) {
            layerCount = minVk(layerCount, resolveImageView->layerCount);
            renderWidth = minVk(renderWidth, resolveImageView->image->info.width);
            renderHeight = minVk(renderHeight, resolveImageView->image->info.height);
        }
    }

    // fragment shading rate attachment
    if (info->fragmentShadingRateAttachment) {
        imageView = (ImageView*)info->fragmentShadingRateAttachment->imageView;
        fsrInfo.imageView = imageView->handle;

        fsrInfo.shadingRateAttachmentTexelSize.width =
            info->fragmentShadingRateAttachment->texelWidth;

        fsrInfo.shadingRateAttachmentTexelSize.height =
            info->fragmentShadingRateAttachment->texelHeight;

        fsrInfo.imageLayout = VK_IMAGE_LAYOUT_FRAGMENT_SHADING_RATE_ATTACHMENT_OPTIMAL_KHR;
        rendering.pNext = &fsrInfo;

        // compute layer count and render area
        layerCount = minVk(layerCount, imageView->layerCount);
        renderWidth = minVk(renderWidth, imageView->image->info.width);
        renderHeight = minVk(renderHeight, imageView->image->info.height);
    }

    rendering.layerCount = layerCount;
    rendering.renderArea.offset.x = 0;
    rendering.renderArea.offset.y = 0;
    rendering.renderArea.extent.width = renderWidth;
    rendering.renderArea.extent.height = renderHeight;

    if (info->viewCount == 1) {
        rendering.viewMask = 0;
    } else {
        rendering.viewMask = (1 << info->viewCount) - 1;
    }

    vkCmdBuffer->device->cmdBeginRendering(vkCmdBuffer->handle, &rendering);
    return PAL_RESULT_SUCCESS;
}

PalResult PAL_CALL cmdEndRenderingVk(PalCommandBuffer* cmdBuffer)
{
    CommandBuffer* vkCmdBuffer = (CommandBuffer*)cmdBuffer;
    vkCmdBuffer->device->cmdEndRendering(vkCmdBuffer->handle);
    return PAL_RESULT_SUCCESS;
}

PalResult PAL_CALL cmdCopyBufferVk(
    PalCommandBuffer* cmdBuffer,
    PalBuffer* dst,
    PalBuffer* src,
    PalBufferCopyInfo* copyInfo)
{
    CommandBuffer* vkCmdBuffer = (CommandBuffer*)cmdBuffer;
    Buffer* dstbuffer = (Buffer*)dst;
    Buffer* srcBuffer = (Buffer*)src;

    VkBufferCopy copyRegion = {0};
    copyRegion.size = copyInfo->size;
    copyRegion.dstOffset = copyInfo->dstOffset;
    copyRegion.srcOffset = copyInfo->srcOffset;
    s_Vk.cmdCopyBuffer(vkCmdBuffer->handle, srcBuffer->handle, dstbuffer->handle, 1, &copyRegion);

    return PAL_RESULT_SUCCESS;
}

PalResult PAL_CALL cmdCopyBufferToImageVk(
    PalCommandBuffer* cmdBuffer,
    PalImage* dstImage,
    PalBuffer* srcBuffer,
    PalBufferImageCopyInfo* copyInfo)
{
    CommandBuffer* vkCmdBuffer = (CommandBuffer*)cmdBuffer;
    Image* dst = (Image*)dstImage;
    Buffer* src = (Buffer*)srcBuffer;

    VkBufferImageCopy copyRegion = {0};
    copyRegion.bufferImageHeight = copyInfo->bufferImageHeight;
    copyRegion.bufferOffset = copyInfo->bufferOffset;
    copyRegion.bufferRowLength = copyInfo->bufferRowLength;

    copyRegion.imageOffset.x = copyInfo->imageOffsetX;
    copyRegion.imageOffset.y = copyInfo->imageOffsetY;
    copyRegion.imageOffset.z = copyInfo->imageOffsetZ;

    copyRegion.imageExtent.width = copyInfo->imageWidth;
    copyRegion.imageExtent.height = copyInfo->imageHeight;
    copyRegion.imageExtent.depth = copyInfo->imageDepth;

    copyRegion.imageSubresource.aspectMask = imageAspectToVk(copyInfo->imageAspect);
    copyRegion.imageSubresource.baseArrayLayer = copyInfo->ImageStartArrayLayer;
    copyRegion.imageSubresource.layerCount = copyInfo->ImageArrayLayerCount;
    copyRegion.imageSubresource.mipLevel = copyInfo->ImageMipLevel;
    
    s_Vk.cmdCopyBufferToImage(
        vkCmdBuffer->handle, 
        src->handle, 
        dst->handle,
        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
        1,
        &copyRegion);

    return PAL_RESULT_SUCCESS;
}

PalResult PAL_CALL cmdCopyImageVk(
    PalCommandBuffer* cmdBuffer,
    PalImage* dst,
    PalImage* src,
    PalImageCopyInfo* copyInfo)
{
    CommandBuffer* vkCmdBuffer = (CommandBuffer*)cmdBuffer;
    Image* dstImage = (Image*)dst;
    Image* srcImage = (Image*)src;

    VkImageCopy copyRegion = {0};
    copyRegion.dstOffset.x = copyInfo->dstOffsetX;
    copyRegion.dstOffset.y = copyInfo->dstOffsetY;
    copyRegion.dstOffset.z = copyInfo->dstOffsetZ;

    copyRegion.srcOffset.x = copyInfo->srcOffsetX;
    copyRegion.srcOffset.y = copyInfo->srcOffsetY;
    copyRegion.srcOffset.z = copyInfo->srcOffsetZ;

    copyRegion.extent.width = copyInfo->width;
    copyRegion.extent.height = copyInfo->height;
    copyRegion.extent.depth = copyInfo->depth;

    copyRegion.dstSubresource.aspectMask = imageAspectToVk(copyInfo->aspect);
    copyRegion.dstSubresource.baseArrayLayer = copyInfo->dstStartArrayLayer;
    copyRegion.dstSubresource.layerCount = copyInfo->arrayLayerCount;
    copyRegion.dstSubresource.mipLevel = copyInfo->dstMipLevel;

    copyRegion.srcSubresource.aspectMask = imageAspectToVk(copyInfo->aspect);
    copyRegion.srcSubresource.baseArrayLayer = copyInfo->srcStartArrayLayer;
    copyRegion.srcSubresource.layerCount = copyInfo->arrayLayerCount;
    copyRegion.srcSubresource.mipLevel = copyInfo->srcMipLevel;
    
    s_Vk.cmdCopyImage(
        vkCmdBuffer->handle, 
        srcImage->handle, 
        VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
        dstImage->handle,
        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
        1,
        &copyRegion);

    return PAL_RESULT_SUCCESS;
}
   
PalResult PAL_CALL cmdCopyImageToBufferVk(
    PalCommandBuffer* cmdBuffer,
    PalBuffer* dstBuffer,
    PalImage* srcImage,
    PalBufferImageCopyInfo* copyInfo)
{
    CommandBuffer* vkCmdBuffer = (CommandBuffer*)cmdBuffer;
    Buffer* dst = (Buffer*)dstBuffer;
    Image* src = (Image*)srcImage;

    VkBufferImageCopy copyRegion = {0};
    copyRegion.bufferImageHeight = copyInfo->bufferImageHeight;
    copyRegion.bufferOffset = copyInfo->bufferOffset;
    copyRegion.bufferRowLength = copyInfo->bufferRowLength;

    copyRegion.imageOffset.x = copyInfo->imageOffsetX;
    copyRegion.imageOffset.y = copyInfo->imageOffsetY;
    copyRegion.imageOffset.z = copyInfo->imageOffsetZ;

    copyRegion.imageExtent.width = copyInfo->imageWidth;
    copyRegion.imageExtent.height = copyInfo->imageHeight;
    copyRegion.imageExtent.depth = copyInfo->imageDepth;

    copyRegion.imageSubresource.aspectMask = imageAspectToVk(copyInfo->imageAspect);
    copyRegion.imageSubresource.baseArrayLayer = copyInfo->ImageStartArrayLayer;
    copyRegion.imageSubresource.layerCount = copyInfo->ImageArrayLayerCount;
    copyRegion.imageSubresource.mipLevel = copyInfo->ImageMipLevel;
    
    s_Vk.cmdCopyImageToBuffer(
        vkCmdBuffer->handle, 
        src->handle,
        VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, 
        dst->handle,
        1,
        &copyRegion);

    return PAL_RESULT_SUCCESS;
}

PalResult PAL_CALL cmdBindPipelineVk(
    PalCommandBuffer* cmdBuffer,
    PalPipeline* pipeline)
{
    CommandBuffer* vkCmdBuffer = (CommandBuffer*)cmdBuffer;
    Pipeline* vkPipeline = (Pipeline*)pipeline;
    s_Vk.cmdBindPipeline(vkCmdBuffer->handle, vkPipeline->bindPoint, vkPipeline->handle);

    vkCmdBuffer->pipeline = vkPipeline;
    return PAL_RESULT_SUCCESS;
}

PalResult PAL_CALL cmdSetViewportVk(
    PalCommandBuffer* cmdBuffer,
    Uint32 count,
    PalViewport* viewports)
{
    CommandBuffer* vkCmdBuffer = (CommandBuffer*)cmdBuffer;
    VkViewport cachedViewport;
    VkViewport* vkViewports = nullptr;

    if (count > 1) {
        vkViewports = palAllocate(s_Vk.allocator, sizeof(VkViewport) * count, 0);
        if (!vkViewports) {
            return PAL_RESULT_OUT_OF_MEMORY;
        }

    } else {
        vkViewports = &cachedViewport;
    }

    for (int i = 0; i < count; i++) {
        VkViewport* tmp = &vkViewports[i];
        tmp->x = viewports[i].x;
        tmp->y = viewports[i].y;
        tmp->width = viewports[i].width;
        tmp->height = viewports[i].height;
        tmp->minDepth = viewports[i].minDepth;
        tmp->maxDepth = viewports[i].maxDepth;
    }

    s_Vk.cmdSetViewports(vkCmdBuffer->handle, 0, count, vkViewports);
    if (count > 1) {
        palFree(s_Vk.allocator, vkViewports);
    }
    return PAL_RESULT_SUCCESS;
}

PalResult PAL_CALL cmdSetScissorsVk(
    PalCommandBuffer* cmdBuffer,
    Uint32 count,
    PalRect2D* scissors)
{
    CommandBuffer* vkCmdBuffer = (CommandBuffer*)cmdBuffer;
    VkRect2D cachedScissor;
    VkRect2D* vkScissors = nullptr;

    if (count > 1) {
        vkScissors = palAllocate(s_Vk.allocator, sizeof(VkRect2D) * count, 0);
        if (!vkScissors) {
            return PAL_RESULT_OUT_OF_MEMORY;
        }

    } else {
        vkScissors = &cachedScissor;
    }

    for (int i = 0; i < count; i++) {
        VkRect2D* tmp = &vkScissors[i];
        tmp->offset.x = scissors[i].x;
        tmp->offset.y = scissors[i].y;
        tmp->extent.width = scissors[i].width;
        tmp->extent.height = scissors[i].height;
    }

    s_Vk.cmdSetScissors(vkCmdBuffer->handle, 0, count, vkScissors);
    if (count > 1) {
        palFree(s_Vk.allocator, vkScissors);
    }
    return PAL_RESULT_SUCCESS;
}

PalResult PAL_CALL cmdBindVertexBuffersVk(
    PalCommandBuffer* cmdBuffer,
    Uint32 firstSlot,
    Uint32 count,
    PalBuffer** buffers,
    Uint64* offsets)
{
    CommandBuffer* vkCmdBuffer = (CommandBuffer*)cmdBuffer;
    VkBuffer cachedbuffer = nullptr;
    VkBuffer* vkBuffers = nullptr;

    if (count > 1) {
        vkBuffers = palAllocate(s_Vk.allocator, sizeof(VkBuffer) * count, 0);
        if (!vkBuffers) {
            return PAL_RESULT_OUT_OF_MEMORY;
        }

    } else {
        vkBuffers = &cachedbuffer;
    }

    for (int i = 0; i < count; i++) {
        Buffer* tmp = (Buffer*)buffers[i];
        vkBuffers[i] = tmp->handle;
    }

    s_Vk.cmdBindVertexBuffers(vkCmdBuffer->handle, firstSlot, count, vkBuffers, offsets);
    if (count > 1) {
        palFree(s_Vk.allocator, vkBuffers);
    }
    return PAL_RESULT_SUCCESS;
}

PalResult PAL_CALL cmdBindIndexBufferVk(
    PalCommandBuffer* cmdBuffer,
    PalBuffer* buffer,
    Uint64 offset,
    PalIndexType type)
{
    CommandBuffer* vkCmdBuffer = (CommandBuffer*)cmdBuffer;
    Buffer* vkBuffer = (Buffer*)buffer;
    VkIndexType bufferType = VK_INDEX_TYPE_UINT32;
    if (type == PAL_INDEX_TYPE_UINT16) {
        bufferType = VK_INDEX_TYPE_UINT16;
    }

    s_Vk.cmdBindIndexBuffer(vkCmdBuffer->handle, vkBuffer->handle, offset, bufferType);
    return PAL_RESULT_SUCCESS;
}

PalResult PAL_CALL cmdDrawVk(
    PalCommandBuffer* cmdBuffer,
    Uint32 vertexCount,
    Uint32 instanceCount,
    Uint32 firstVertex,
    Uint32 firstInstance)
{
    CommandBuffer* vkCmdBuffer = (CommandBuffer*)cmdBuffer;
    s_Vk.cmdDraw(vkCmdBuffer->handle, vertexCount, instanceCount, firstVertex, firstInstance);
    return PAL_RESULT_SUCCESS;
}

PalResult PAL_CALL cmdDrawIndirectVk(
    PalCommandBuffer* cmdBuffer,
    PalBuffer* buffer,
    Uint32 count)
{
    CommandBuffer* vkCmdBuffer = (CommandBuffer*)cmdBuffer;
    Buffer* vkBuffer = (Buffer*)buffer;
    Uint32 stride = sizeof(VkDrawIndirectCommand);

    s_Vk.cmdDrawIndirect(vkCmdBuffer->handle, vkBuffer->handle, 0, count, stride);
    return PAL_RESULT_SUCCESS;
}

PalResult PAL_CALL cmdDrawIndirectCountVk(
    PalCommandBuffer* cmdBuffer,
    PalBuffer* buffer,
    PalBuffer* countBuffer,
    Uint32 maxDrawCount)
{
    CommandBuffer* vkCmdBuffer = (CommandBuffer*)cmdBuffer;
    Buffer* vkBuffer = (Buffer*)buffer;
    Buffer* vkCountBuffer = (Buffer*)countBuffer;
    Device* device = vkCmdBuffer->device;

    if (!(device->features & PAL_ADAPTER_FEATURE_INDIRECT_DRAW_COUNT)) {
        return PAL_RESULT_ADAPTER_FEATURE_NOT_SUPPORTED;
    }

    Uint32 stride = sizeof(VkDrawIndirectCommand);
    device->cmdDrawIndirectCount(
        vkCmdBuffer->handle,
        vkBuffer->handle,
        0,
        vkCountBuffer->handle,
        0,
        maxDrawCount,
        stride);

    return PAL_RESULT_SUCCESS;
}

PalResult PAL_CALL cmdDrawIndexedVk(
    PalCommandBuffer* cmdBuffer,
    Uint32 indexCount,
    Uint32 instanceCount,
    Uint32 firstIndex,
    Int32 vertexOffset,
    Uint32 firstInstance)
{
    CommandBuffer* vkCmdBuffer = (CommandBuffer*)cmdBuffer;
    s_Vk.cmdDrawIndexed(
        vkCmdBuffer->handle,
        indexCount,
        instanceCount,
        firstIndex,
        vertexOffset,
        firstInstance);

    return PAL_RESULT_SUCCESS;
}

PalResult PAL_CALL cmdDrawIndexedIndirectVk(
    PalCommandBuffer* cmdBuffer,
    PalBuffer* buffer,
    Uint32 count)
{
    CommandBuffer* vkCmdBuffer = (CommandBuffer*)cmdBuffer;
    Buffer* vkBuffer = (Buffer*)buffer;
    Uint32 stride = sizeof(VkDrawIndexedIndirectCommand);

    s_Vk.cmdDrawIndexedIndirect(vkCmdBuffer->handle, vkBuffer->handle, 0, count, stride);
    return PAL_RESULT_SUCCESS;
}

PalResult PAL_CALL cmdDrawIndexedIndirectCountVk(
    PalCommandBuffer* cmdBuffer,
    PalBuffer* buffer,
    PalBuffer* countBuffer,
    Uint32 maxDrawCount)
{
    CommandBuffer* vkCmdBuffer = (CommandBuffer*)cmdBuffer;
    Buffer* vkBuffer = (Buffer*)buffer;
    Buffer* vkCountBuffer = (Buffer*)countBuffer;
    Device* device = vkCmdBuffer->device;

    if (!(device->features & PAL_ADAPTER_FEATURE_INDIRECT_DRAW_COUNT)) {
        return PAL_RESULT_ADAPTER_FEATURE_NOT_SUPPORTED;
    }

    Uint32 stride = sizeof(VkDrawIndexedIndirectCommand);
    device->cmdDrawIndexedIndirectCount(
        vkCmdBuffer->handle,
        vkBuffer->handle,
        0,
        vkCountBuffer->handle,
        0,
        maxDrawCount,
        stride);

    return PAL_RESULT_SUCCESS;
}

PalResult PAL_CALL cmdAccelerationStructureBarrierVk(
    PalCommandBuffer* cmdBuffer,
    PalAccelerationStructure* as,
    PalUsageStateInfo* oldUsageStateInfo,
    PalUsageStateInfo* newUsageStateInfo)
{
    CommandBuffer* vkCmdBuffer = (CommandBuffer*)cmdBuffer;
    if (!(vkCmdBuffer->device->features & PAL_ADAPTER_FEATURE_RAY_TRACING)) {
        return PAL_RESULT_ADAPTER_FEATURE_NOT_SUPPORTED;
    }

    VkMemoryBarrier2KHR barrier = {0};
    barrier.sType = VK_STRUCTURE_TYPE_MEMORY_BARRIER_2_KHR;

    Barrier old, new;
    old = barrierToVk(
        oldUsageStateInfo->shaderStageCount,
        oldUsageStateInfo->usageState, 
        oldUsageStateInfo->shaderStages);

    new = barrierToVk(
        newUsageStateInfo->shaderStageCount,
        newUsageStateInfo->usageState, 
        newUsageStateInfo->shaderStages);

    barrier.srcStageMask = old.stages;
    barrier.srcAccessMask = old.access;

    barrier.dstStageMask = new.stages;
    barrier.dstAccessMask = new.access;

    VkDependencyInfo dependencyInfo = {0};
    dependencyInfo.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO;
    dependencyInfo.memoryBarrierCount = 1;
    dependencyInfo.pMemoryBarriers = &barrier;

    vkCmdBuffer->device->cmdPipelineBarrier(vkCmdBuffer->handle, &dependencyInfo);
    return PAL_RESULT_SUCCESS;
}

PalResult PAL_CALL cmdImageBarrierVk(
    PalCommandBuffer* cmdBuffer,
    PalImage* image,
    PalImageSubresourceRange* subresourceRange,
    PalUsageStateInfo* oldUsageStateInfo,
    PalUsageStateInfo* newUsageStateInfo)
{
    CommandBuffer* vkCmdBuffer = (CommandBuffer*)cmdBuffer;
    Image* vkImage = (Image*)image;
    VkImageMemoryBarrier2KHR barrier = {0};
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2_KHR;

    Barrier old, new;
    old = barrierToVk(
        oldUsageStateInfo->shaderStageCount,
        oldUsageStateInfo->usageState, 
        oldUsageStateInfo->shaderStages);

    new = barrierToVk(
        newUsageStateInfo->shaderStageCount,
        newUsageStateInfo->usageState, 
        newUsageStateInfo->shaderStages);

    barrier.srcStageMask = old.stages;
    barrier.srcAccessMask = old.access;
    barrier.oldLayout = old.layout;

    barrier.dstStageMask = new.stages;
    barrier.dstAccessMask = new.access;
    barrier.newLayout = new.layout;

    barrier.image = vkImage->handle;
    barrier.subresourceRange.aspectMask = imageAspectToVk(subresourceRange->aspect);
    barrier.subresourceRange.baseArrayLayer = subresourceRange->startArrayLayer;
    barrier.subresourceRange.baseMipLevel = subresourceRange->startMipLevel;
    barrier.subresourceRange.layerCount = subresourceRange->layerArrayCount;
    barrier.subresourceRange.levelCount = subresourceRange->mipLevelCount;

    VkDependencyInfo dependencyInfo = {0};
    dependencyInfo.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO;
    dependencyInfo.imageMemoryBarrierCount = 1;
    dependencyInfo.pImageMemoryBarriers = &barrier;

    vkCmdBuffer->device->cmdPipelineBarrier(vkCmdBuffer->handle, &dependencyInfo);
    return PAL_RESULT_SUCCESS;
}

PalResult PAL_CALL cmdBufferBarrierVk(
    PalCommandBuffer* cmdBuffer,
    PalBuffer* buffer,
    PalUsageStateInfo* oldUsageStateInfo,
    PalUsageStateInfo* newUsageStateInfo)
{
    CommandBuffer* vkCmdBuffer = (CommandBuffer*)cmdBuffer;
    Buffer* vkBuffer = (Buffer*)buffer;
    VkBufferMemoryBarrier2KHR barrier = {0};
    barrier.sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER_2_KHR;

    Barrier old, new;
    old = barrierToVk(
        oldUsageStateInfo->shaderStageCount,
        oldUsageStateInfo->usageState, 
        oldUsageStateInfo->shaderStages);

    new = barrierToVk(
        newUsageStateInfo->shaderStageCount,
        newUsageStateInfo->usageState, 
        newUsageStateInfo->shaderStages);

    barrier.srcStageMask = old.stages;
    barrier.srcAccessMask = old.access;

    barrier.dstStageMask = new.stages;
    barrier.dstAccessMask = new.access;

    barrier.buffer = vkBuffer->handle;
    barrier.offset = 0;
    barrier.size = VK_WHOLE_SIZE;

    VkDependencyInfo dependencyInfo = {0};
    dependencyInfo.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO;
    dependencyInfo.bufferMemoryBarrierCount = 1;
    dependencyInfo.pBufferMemoryBarriers = &barrier;

    vkCmdBuffer->device->cmdPipelineBarrier(vkCmdBuffer->handle, &dependencyInfo);
    return PAL_RESULT_SUCCESS;
}

PalResult PAL_CALL cmdDispatchVk(
    PalCommandBuffer* cmdBuffer,
    Uint32 groupCountX,
    Uint32 groupCountY,
    Uint32 groupCountZ)
{
    CommandBuffer* vkCmdBuffer = (CommandBuffer*)cmdBuffer;
    s_Vk.cmdDispatch(vkCmdBuffer->handle, groupCountX, groupCountY, groupCountZ);
    return PAL_RESULT_SUCCESS;
}

PalResult PAL_CALL cmdDispatchBaseVk(
    PalCommandBuffer* cmdBuffer,
    Uint32 baseGroupX,
    Uint32 baseGroupY,
    Uint32 baseGroupZ,
    Uint32 groupCountX,
    Uint32 groupCountY,
    Uint32 groupCountZ)
{
    CommandBuffer* vkCmdBuffer = (CommandBuffer*)cmdBuffer;
    Device* device = vkCmdBuffer->device;

    if (!(device->features & PAL_ADAPTER_FEATURE_DISPATCH_BASE)) {
        return PAL_RESULT_ADAPTER_FEATURE_NOT_SUPPORTED;
    }

    device->cmdDispatchBase(
        vkCmdBuffer->handle,
        baseGroupX,
        baseGroupY,
        baseGroupZ,
        groupCountX,
        groupCountY,
        groupCountZ);

    return PAL_RESULT_SUCCESS;
}

PalResult PAL_CALL cmdDispatchIndirectVk(
    PalCommandBuffer* cmdBuffer,
    PalBuffer* buffer)
{
    CommandBuffer* vkCmdBuffer = (CommandBuffer*)cmdBuffer;
    if (!(vkCmdBuffer->device->features & PAL_ADAPTER_FEATURE_COMPUTE_SHADER)) {
        return PAL_RESULT_ADAPTER_FEATURE_NOT_SUPPORTED;
    }

    if (!(vkCmdBuffer->device->features & PAL_ADAPTER_FEATURE_INDIRECT_DRAW)) {
        return PAL_RESULT_ADAPTER_FEATURE_NOT_SUPPORTED;
    }

    Buffer* vkBuffer = (Buffer*)buffer;
    s_Vk.cmdDispatchIndirect(vkCmdBuffer->handle, vkBuffer->handle, 0);
    return PAL_RESULT_SUCCESS;
}

PalResult PAL_CALL cmdTraceRaysVk(
    PalCommandBuffer* cmdBuffer,
    PalShaderBindingTable* sbt,
    Uint32 raygenIndex,
    Uint32 width,
    Uint32 height,
    Uint32 depth)
{
    CommandBuffer* vkCmdBuffer = (CommandBuffer*)cmdBuffer;
    Device* device = vkCmdBuffer->device;
    ShaderBindingTable* vkSbt = (ShaderBindingTable*)sbt;

    if (!(device->features & PAL_ADAPTER_FEATURE_RAY_TRACING)) {
        return PAL_RESULT_ADAPTER_FEATURE_NOT_SUPPORTED;
    }

    VkStridedDeviceAddressRegionKHR raygenAddress = {0};
    raygenAddress.size = vkSbt->raygenAddress.size;
    raygenAddress.stride = vkSbt->raygenAddress.stride;
    raygenAddress.deviceAddress = vkSbt->baseAddress + raygenIndex * vkSbt->raygenAddress.stride;

    vkCmdBuffer->device->cmdTraceRays(
        vkCmdBuffer->handle,
        &raygenAddress,
        &vkSbt->missAddress,
        &vkSbt->hitAddress,
        &vkSbt->callableAddress,
        width,
        height,
        depth);

    return PAL_RESULT_SUCCESS;
}

PalResult PAL_CALL cmdTraceRaysIndirectVk(
    PalCommandBuffer* cmdBuffer,
    Uint32 raygenIndex,
    PalShaderBindingTable* sbt,
    PalBuffer* buffer)
{
    CommandBuffer* vkCmdBuffer = (CommandBuffer*)cmdBuffer;
    ShaderBindingTable* vkSbt = (ShaderBindingTable*)sbt;
    Buffer* vkBuffer = (Buffer*)buffer;

    if (!(vkCmdBuffer->device->features & PAL_ADAPTER_FEATURE_RAY_TRACING)) {
        return PAL_RESULT_ADAPTER_FEATURE_NOT_SUPPORTED;
    }

    if (vkBuffer->memory->type != PAL_MEMORY_TYPE_CPU_UPLOAD) {
        return PAL_RESULT_MEMORY_MAP_FAILED;
    }

    PalDeviceAddress address = vkSbt->baseAddress + raygenIndex * vkSbt->raygenAddress.stride;
    vkSbt->raygenAddress.deviceAddress = address;
    
    VkDeviceAddress bufferAddress = 0;
    VkBufferDeviceAddressInfoKHR bufferInfo = {0};
    bufferInfo.buffer = vkBuffer->handle;
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO_KHR;
    bufferAddress = vkBuffer->device->getBufferrAddress(vkBuffer->device->handle, &bufferInfo);

    vkCmdBuffer->device->cmdTraceRaysIndirect(
        vkCmdBuffer->handle,
        &vkSbt->raygenAddress,
        &vkSbt->missAddress,
        &vkSbt->hitAddress,
        &vkSbt->callableAddress,
        bufferAddress);

    return PAL_RESULT_SUCCESS;
}

PalResult PAL_CALL cmdBindDescriptorSetVk(
    PalCommandBuffer* cmdBuffer,
    Uint32 setIndex,
    PalDescriptorSet* set)
{
    CommandBuffer* vkCmdBuffer = (CommandBuffer*)cmdBuffer;
    Pipeline* pipeline = vkCmdBuffer->pipeline;
    DescriptorSet* vkSet = (DescriptorSet*)set;

    s_Vk.cmdBindDescriptorSets(
        vkCmdBuffer->handle,
        pipeline->bindPoint,
        pipeline->layout,
        setIndex,
        1,
        &vkSet->handle,
        0,
        nullptr);

    return PAL_RESULT_SUCCESS;
}

PalResult PAL_CALL cmdPushConstantsVk(
    PalCommandBuffer* cmdBuffer,
    Uint32 shaderStageCount,
    PalShaderStage* shaderStages,
    Uint64 offset,
    Uint64 size,
    const void* value)
{
    CommandBuffer* vkCmdBuffer = (CommandBuffer*)cmdBuffer;
    Pipeline* pipeline = vkCmdBuffer->pipeline;
    VkShaderStageFlags stages = 0;

    for (int i = 0; i < shaderStageCount; i++) {
        VkShaderStageFlagBits bit = shaderStageToVK(shaderStages[i]);
        stages |= bit;
    }

    s_Vk.cmdPushConstants(
        vkCmdBuffer->handle, 
        pipeline->layout, 
        stages, 
        offset, 
        size, 
        value);

    return PAL_RESULT_SUCCESS;
}

PalResult PAL_CALL cmdSetCullModeVk(
    PalCommandBuffer* cmdBuffer,
    PalCullMode cullMode)
{
    CommandBuffer* vkCmdBuffer = (CommandBuffer*)cmdBuffer;
    Device* device = vkCmdBuffer->device;

    if (!(device->features & PAL_ADAPTER_FEATURE_DYNAMIC_CULL_MODE)) {
        return PAL_RESULT_ADAPTER_FEATURE_NOT_SUPPORTED;
    }

    VkCullModeFlags vkCullMode = 0;
    switch (cullMode) {
        case PAL_CULL_MODE_BACK:
            vkCullMode = VK_CULL_MODE_BACK_BIT;

        case PAL_CULL_MODE_FRONT:
            vkCullMode = VK_CULL_MODE_FRONT_BIT;

        case PAL_CULL_MODE_NONE:
            vkCullMode = VK_CULL_MODE_NONE;
    }
    
    device->cmdSetCullMode(vkCmdBuffer->handle, vkCullMode);
    return PAL_RESULT_SUCCESS;
}

PalResult PAL_CALL cmdSetFrontFaceVk(
    PalCommandBuffer* cmdBuffer,
    PalFrontFace frontFace)
{
    CommandBuffer* vkCmdBuffer = (CommandBuffer*)cmdBuffer;
    Device* device = vkCmdBuffer->device;

    if (!(device->features & PAL_ADAPTER_FEATURE_DYNAMIC_FRONT_FACE)) {
        return PAL_RESULT_ADAPTER_FEATURE_NOT_SUPPORTED;
    }

    VkFrontFace vkFrontFace = 0;
    switch (frontFace) {
        case PAL_FRONT_FACE_CLOCKWISE:
            vkFrontFace = VK_FRONT_FACE_CLOCKWISE;

        case PAL_FRONT_FACE_COUNTER_CLOCKWISE:
            vkFrontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
    }

    device->cmdSetFrontFace(vkCmdBuffer->handle, vkFrontFace);
    return PAL_RESULT_SUCCESS;
}

PalResult PAL_CALL cmdSetPrimitiveTopologyVk(
    PalCommandBuffer* cmdBuffer,
    PalPrimitiveTopology topology)
{
    CommandBuffer* vkCmdBuffer = (CommandBuffer*)cmdBuffer;
    Device* device = vkCmdBuffer->device;

    if (!(device->features & PAL_ADAPTER_FEATURE_DYNAMIC_PRIMITIVE_TOPOLOGY)) {
        return PAL_RESULT_ADAPTER_FEATURE_NOT_SUPPORTED;
    }

    VkPrimitiveTopology vkTopology = 0;
    switch (topology) {
        case PAL_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST: {
            vkTopology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
            break;
        }

        case PAL_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP: {
            vkTopology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP;
            break;
        }

        case PAL_PRIMITIVE_TOPOLOGY_LINE_LIST: {
            vkTopology = VK_PRIMITIVE_TOPOLOGY_LINE_LIST;
            break;
        }

        case PAL_PRIMITIVE_TOPOLOGY_LINE_STRIP: {
            vkTopology = VK_PRIMITIVE_TOPOLOGY_LINE_STRIP;
            break;
        }

        case PAL_PRIMITIVE_TOPOLOGY_POINT_LIST: {
            vkTopology = VK_PRIMITIVE_TOPOLOGY_POINT_LIST;
            break;
        }
    }

    device->cmdSetPrimitiveTopology(vkCmdBuffer->handle, vkTopology);
    return PAL_RESULT_SUCCESS;
}

PalResult PAL_CALL cmdSetDepthTestEnableVk(
    PalCommandBuffer* cmdBuffer,
    bool enable)
{
    CommandBuffer* vkCmdBuffer = (CommandBuffer*)cmdBuffer;
    Device* device = vkCmdBuffer->device;

    if (!(device->features & PAL_ADAPTER_FEATURE_DYNAMIC_DEPTH_TEST_ENABLE)) {
        return PAL_RESULT_ADAPTER_FEATURE_NOT_SUPPORTED;
    }
    
    device->cmdSetDepthTestEnable(vkCmdBuffer->handle, enable);
    return PAL_RESULT_SUCCESS;
}

PalResult PAL_CALL cmdSetDepthWriteEnableVk(
    PalCommandBuffer* cmdBuffer,
    bool enable)
{
    CommandBuffer* vkCmdBuffer = (CommandBuffer*)cmdBuffer;
    Device* device = vkCmdBuffer->device;

    if (!(device->features & PAL_ADAPTER_FEATURE_DYNAMIC_DEPTH_WRITE_ENABLE)) {
        return PAL_RESULT_ADAPTER_FEATURE_NOT_SUPPORTED;
    }
    
    device->cmdSetDepthWriteEnable(vkCmdBuffer->handle, enable);
    return PAL_RESULT_SUCCESS;
}

PalResult PAL_CALL cmdSetStencilOpVk(
    PalCommandBuffer* cmdBuffer,
    PalStencilFaceFlags faceMask,
    PalStencilOp failOp,
    PalStencilOp passOp,
    PalStencilOp depthFailOp,
    PalCompareOp compareOp)
{
    CommandBuffer* vkCmdBuffer = (CommandBuffer*)cmdBuffer;
    Device* device = vkCmdBuffer->device;

    if (!(device->features & PAL_ADAPTER_FEATURE_DYNAMIC_STENCIL_OP)) {
        return PAL_RESULT_ADAPTER_FEATURE_NOT_SUPPORTED;
    }

    VkStencilFaceFlags faceFlags = 0;
    if (faceMask & PAL_STENCIL_FACE_BACK) {
        faceFlags |= VK_STENCIL_FACE_BACK_BIT;
    }

    if (faceMask & PAL_STENCIL_FACE_FRONT) {
        faceFlags |= VK_STENCIL_FACE_FRONT_BIT;
    }

    VkStencilOp vkFailOp = stencilOpToVk(failOp);
    VkStencilOp vkPassOp = stencilOpToVk(passOp);
    VkStencilOp vkDepthFailOp = stencilOpToVk(depthFailOp);
    VkCompareOp vkCompareOp = compareOpToVk(compareOp);
    
    device->cmdSetStencilOp(
        vkCmdBuffer->handle, 
        faceFlags, 
        failOp, 
        passOp, 
        depthFailOp, 
        compareOp);

    return PAL_RESULT_SUCCESS;
}

// ==================================================
// Acceleration Structure
// ==================================================

PalResult PAL_CALL createAccelerationstructureVk(
    PalDevice* device,
    const PalAccelerationStructureCreateInfo* info,
    PalAccelerationStructure** outAs)
{
    VkResult result;
    AccelerationStructure* as = nullptr;
    Device* vkDevice = (Device*)device;
    Buffer* buffer = (Buffer*)info->buffer;
    
    if (!(vkDevice->features & PAL_ADAPTER_FEATURE_RAY_TRACING)) {
        return PAL_RESULT_ADAPTER_FEATURE_NOT_SUPPORTED;
    }

    as = palAllocate(s_Vk.allocator, sizeof(AccelerationStructure), 0);
    if (!as) {
        return PAL_RESULT_OUT_OF_MEMORY;
    }

    VkAccelerationStructureCreateInfoKHR createInfo = {0};
    createInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_CREATE_INFO_KHR;
    createInfo.offset = (VkDeviceSize)info->offset;
    createInfo.size = (VkDeviceSize)info->size;
    createInfo.buffer = buffer->handle;
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
        return resultFromVk(result);
    }

    // get and cache address
    VkAccelerationStructureDeviceAddressInfoKHR addressInfo = {0};
    addressInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_DEVICE_ADDRESS_INFO_KHR;
    addressInfo.accelerationStructure = as->handle;
    as->address = vkDevice->getAccelerationDeviceAddress(vkDevice->handle, &addressInfo);

    as->device = vkDevice;
    *outAs = (PalAccelerationStructure*)as;
    return PAL_RESULT_SUCCESS;
}

void PAL_CALL destroyAccelerationstructureVk(PalAccelerationStructure* as)
{
    AccelerationStructure* vkAs = (AccelerationStructure*)as;
    vkAs->device->destroyAccelerationStructure(
        vkAs->device->handle,
        vkAs->handle,
        &s_Vk.vkAllocator);

    palFree(s_Vk.allocator, vkAs);
}

PalResult PAL_CALL getAccelerationStructureBuildSizeVk(
    PalDevice* device,
    PalAccelerationStructureBuildInfo* info,
    PalAccelerationStructureBuildSize* size)
{
    Uint32* maxPrimities = nullptr;
    VkAccelerationStructureGeometryKHR* geometries = nullptr;
    Device* vkDevice = (Device*)device;
    VkAccelerationStructureBuildGeometryInfoKHR buildInfo = {0};

    if (!(vkDevice->features & PAL_ADAPTER_FEATURE_RAY_TRACING)) {
        return PAL_RESULT_ADAPTER_FEATURE_NOT_SUPPORTED;
    }

    // cache these for top level as
    VkAccelerationStructureGeometryKHR cachedGeometries = {0};
    Uint32 cachedPrimitives = 0;
    Uint32 geometryCount = info->geometryCount;
    if (info->type == PAL_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL) {
        geometryCount = 1;
        geometries = &cachedGeometries;
        maxPrimities = &cachedPrimitives;
    }

    if (info->type == PAL_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL) {
        geometries = palAllocate(
            s_Vk.allocator,
            sizeof(VkAccelerationStructureGeometryKHR) * geometryCount,
            0);

        maxPrimities = palAllocate(s_Vk.allocator, sizeof(Uint32) * geometryCount, 0);
        if (!maxPrimities || !geometries) {
            return PAL_RESULT_OUT_OF_MEMORY;
        }

        memset(geometries, 0, sizeof(VkAccelerationStructureGeometryKHR) * geometryCount);
        memset(maxPrimities, 0, sizeof(Uint32) * geometryCount);
    }

    fillBuildInfoVk(
        geometryCount, 
        info, 
        maxPrimities, 
        geometries, 
        nullptr, 
        nullptr, 
        nullptr, 
        &buildInfo);

    VkAccelerationStructureBuildSizesInfoKHR sizeInfo = {0};
    sizeInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_SIZES_INFO_KHR;

    vkDevice->getAccelerationBuildsize(
        vkDevice->handle,
        VK_ACCELERATION_STRUCTURE_BUILD_TYPE_DEVICE_KHR,
        &buildInfo,
        maxPrimities,
        &sizeInfo);

    size->accelerationStructureSize = sizeInfo.accelerationStructureSize;
    size->scratchBufferSize = sizeInfo.buildScratchSize;
    size->updateScratchBufferSize = sizeInfo.updateScratchSize;

    if (info->type == PAL_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL) {
        palFree(s_Vk.allocator, geometries);
        palFree(s_Vk.allocator, maxPrimities);
    }
    return PAL_RESULT_SUCCESS;
}

// ==================================================
// Buffer
// ==================================================

PalResult PAL_CALL createBufferVk(
    PalDevice* device,
    const PalBufferCreateInfo* info,
    PalBuffer** outBuffer)
{
    VkResult result;
    Buffer* buffer = nullptr;
    Device* vkDevice = (Device*)device;

    if (info->usages & PAL_BUFFER_USAGE_ACCELERATION_STRUCTURE) {
        if (!(vkDevice->features & PAL_ADAPTER_FEATURE_RAY_TRACING)) {
            return PAL_RESULT_ADAPTER_FEATURE_NOT_SUPPORTED;
        }
        // buffer device address feature is supported if ray tracing is

    } else if (info->usages & PAL_BUFFER_USAGE_DEVICE_ADDRESS) {
        if (!(vkDevice->features & PAL_ADAPTER_FEATURE_BUFFER_DEVICE_ADDRESS)) {
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
    createInfo.usage = bufferUsageToVk(info->usages);

    result = s_Vk.createBuffer(vkDevice->handle, &createInfo, &s_Vk.vkAllocator, &buffer->handle);
    if (result != VK_SUCCESS) {
        palFree(s_Vk.allocator, buffer);
        return resultFromVk(result);
    }

    buffer->usages = info->usages;
    if (info->usages & PAL_BUFFER_USAGE_ACCELERATION_STRUCTURE) {
        buffer->usages |= PAL_BUFFER_USAGE_DEVICE_ADDRESS;
    }

    buffer->device = vkDevice;
    buffer->memory = nullptr;
    *outBuffer = (PalBuffer*)buffer;
    return PAL_RESULT_SUCCESS;
}

void PAL_CALL destroyBufferVk(PalBuffer* buffer)
{
    Buffer* vkBuffer = (Buffer*)buffer;
    s_Vk.destroyBuffer(vkBuffer->device->handle, vkBuffer->handle, &s_Vk.vkAllocator);
    palFree(s_Vk.allocator, buffer);
}

PalResult PAL_CALL getBufferMemoryRequirementsVk(
    PalBuffer* buffer,
    PalMemoryRequirements* requirements)
{
    Buffer* vkBuffer = (Buffer*)buffer;
    Device* device = vkBuffer->device;
    VkMemoryRequirements memReq = {0};
    s_Vk.getBufferMemoryRequirements(device->handle, vkBuffer->handle, &memReq);

    requirements->alignment = (Uint64)memReq.alignment;
    requirements->size = (Uint64)memReq.size;
    requirements->memoryMask = palPackUint32(memReq.memoryTypeBits, vkBuffer->usages);

    requirements->memoryTypes[PAL_MEMORY_TYPE_GPU_ONLY] = false;
    requirements->memoryTypes[PAL_MEMORY_TYPE_CPU_UPLOAD] = false;
    requirements->memoryTypes[PAL_MEMORY_TYPE_CPU_READBACK] = false;

    for (int i = 0; i < PAL_MEMORY_TYPE_MAX; i++) {
        requirements->memoryTypes[i] = (memReq.memoryTypeBits & device->memoryClassMask[i]) != 0;
    }
    return PAL_RESULT_SUCCESS;
}

PalResult PAL_CALL computeInstanceBufferRequirementsVk(
    PalDevice* device,
    Uint32 instanceCount,
    Uint64* outSize)
{
    *outSize = sizeof(VkAccelerationStructureInstanceKHR) * instanceCount;
    return PAL_RESULT_SUCCESS;
}

PalResult PAL_CALL computeImageCopyStagingBufferRequirementsVk(
    PalDevice* device,
    PalFormat imageFormat,
    PalBufferImageCopyInfo* copyInfo,
    Uint32* outBufferRowLength,
    Uint32* outBufferImageHeight,
    Uint64* outSize)
{
    Uint32 imageFormatSize = getFormatSizeVk(imageFormat);
    Uint32 length = 0;
    Uint32 height = 0;
    length = copyInfo->bufferRowLength ? copyInfo->bufferRowLength : copyInfo->imageWidth;
    height = copyInfo->bufferImageHeight ? copyInfo->bufferImageHeight : copyInfo->imageHeight;
    Uint32 rowPitch = length * imageFormatSize;

    *outBufferRowLength = length;
    *outBufferImageHeight = height;
    *outSize = (Uint64)rowPitch * length * copyInfo->imageDepth;
    return PAL_RESULT_SUCCESS;
}

PalResult PAL_CALL writeToInstanceBufferVk(
    PalDevice* device,
    void* ptr,
    PalAccelerationStructureInstance* instances,
    Uint32 instanceCount)
{
    VkAccelerationStructureInstanceKHR* data = ptr;
    for (int i = 0; i < instanceCount; i++) {
        PalAccelerationStructureInstance* src = &instances[i];
        VkAccelerationStructureInstanceKHR* dst = &data[i];
        AccelerationStructure* as = (AccelerationStructure*)src->blas;

        dst->mask = src->mask & 0xFF;
        dst->instanceCustomIndex = src->instanceId & 0xFFFFFF;
        dst->accelerationStructureReference = as->address;
        dst->instanceShaderBindingTableRecordOffset = src->hitGroupOffset & 0xFFFFFF;
        dst->flags = instanceFlagsToVk(src->flags);
        memcpy(dst->transform.matrix, src->transform, sizeof(float) * 12);
    }
    return PAL_RESULT_SUCCESS;
}

PalResult PAL_CALL writeToImageCopyStagingBufferVk(
    PalDevice* device,
    void* ptr,
    void* srcData,
    PalFormat imageFormat,
    PalBufferImageCopyInfo* copyInfo)
{
    Uint32 imageFormatSize = getFormatSizeVk(imageFormat);
    Uint32 dstRowPitch = copyInfo->bufferRowLength * imageFormatSize;
    Uint32 srcRowPitch = copyInfo->imageWidth * imageFormatSize;
    const Uint32 dstSlicePitch = dstRowPitch * copyInfo->bufferImageHeight;
    const Uint32 srcSlicePitch = srcRowPitch * copyInfo->imageHeight;

    // manually offset the buffer with the provided offset
    Uint8* dst = (Uint8*)ptr + copyInfo->bufferOffset;
    const Uint8* src = (const Uint8*)srcData;

    // write to destination pointer
    for (Uint32 z = 0; z < copyInfo->imageDepth; z++) {
        for (Uint32 y = 0; y < copyInfo->imageHeight; y++) {
            memcpy(
                dst + z * dstSlicePitch + y * dstRowPitch,
                src + z * srcSlicePitch + y * srcRowPitch,
                srcRowPitch);
        }
    }

    return PAL_RESULT_SUCCESS;
}

PalResult PAL_CALL bindBufferMemoryVk(
    PalBuffer* buffer,
    PalMemory* memory,
    Uint64 offset)
{
    VkResult result;
    Memory* vkMemory = (Memory*)memory;
    Buffer* vkBuffer = (Buffer*)buffer;

    if (vkBuffer->memory) {
        return PAL_RESULT_INVALID_OPERATION;
    }

    result = s_Vk.bindBufferMemory(
        vkBuffer->device->handle, 
        vkBuffer->handle, 
        vkMemory->handle, 
        offset);

    if (result != VK_SUCCESS) {
        return resultFromVk(result);
    }

    vkBuffer->memory = vkMemory;
    return PAL_RESULT_SUCCESS;
}

PalResult PAL_CALL mapBufferMemoryVk(
    PalBuffer* buffer,
    Uint64 offset,
    Uint64 size,
    void** outPtr)
{
    VkResult result;
    Buffer* vkBuffer = (Buffer*)buffer;
    Device* device = vkBuffer->device;

    if (vkBuffer->memory->type == PAL_MEMORY_TYPE_GPU_ONLY) {
        return PAL_RESULT_MEMORY_MAP_FAILED;
    }

    result = s_Vk.mapMemory(device->handle, vkBuffer->memory->handle, offset, size, 0, outPtr);
    if (result != VK_SUCCESS) {
        return resultFromVk(result);
    }
    return PAL_RESULT_SUCCESS;
}

void PAL_CALL unmapBufferMemoryVk(PalBuffer* buffer)
{
    Buffer* vkBuffer = (Buffer*)buffer;
    s_Vk.unmapMemory(vkBuffer->device->handle, vkBuffer->memory->handle);
}

PalDeviceAddress PAL_CALL getBufferDeviceAddressVk(PalBuffer* buffer)
{
    Buffer* vkBuffer = (Buffer*)buffer;
    if (!(vkBuffer->device->features & PAL_ADAPTER_FEATURE_BUFFER_DEVICE_ADDRESS)) {
        return 0;
    }

    VkBufferDeviceAddressInfoKHR bufferInfo = {0};
    bufferInfo.buffer = vkBuffer->handle;
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO_KHR;
    return vkBuffer->device->getBufferrAddress(vkBuffer->device->handle, &bufferInfo);
}

// ==================================================
// Descriptor Pool, Set and Layout
// ==================================================

PalResult PAL_CALL createDescriptorSetLayoutVk(
    PalDevice* device,
    const PalDescriptorSetLayoutCreateInfo* info,
    PalDescriptorSetLayout** outLayout)
{
    VkResult result;
    Device* vkDevice = (Device*)device;
    VkDescriptorSetLayoutBinding* bindings = nullptr;
    VkDescriptorSetLayoutBindingFlagsCreateInfoEXT bindingFlags = {0};
    DescriptorSetLayout* layout = nullptr;
    Uint32 count = info->bindingCount;

    layout = palAllocate(s_Vk.allocator, sizeof(DescriptorSetLayout), 0);
    bindings = palAllocate(s_Vk.allocator, sizeof(VkDescriptorSetLayoutBinding) * count, 0);
    if (!layout || !bindings) {
        return PAL_RESULT_OUT_OF_MEMORY;
    }

    VkDescriptorSetLayoutCreateInfo createInfo = {0};
    createInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    createInfo.bindingCount = count;
    createInfo.pBindings = bindings;

    Uint32 bindingIndex = 0;
    for (int i = 0; i < count; i++) {
        VkDescriptorSetLayoutBinding* binding = &bindings[i];
        binding->binding = bindingIndex++;
        binding->descriptorCount = info->bindings[i].descriptorCount;
        binding->descriptorType = descriptortypeToVk(info->bindings[i].descriptorType);

        binding->pImmutableSamplers = nullptr;
        binding->stageFlags = 0;
        for (int j = 0; j < info->bindings[i].shaderStageCount; j++) {
            VkShaderStageFlagBits bit = shaderStageToVK(info->bindings[i].shaderStages[j]);
            binding->stageFlags |= bit;
        }
    }

    if (vkDevice->features & PAL_ADAPTER_FEATURE_DESCRIPTOR_INDEXING) {
        bindingFlags.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_BINDING_FLAGS_CREATE_INFO_EXT;
        bindingFlags.bindingCount = count;

        VkDescriptorBindingFlags flags = VK_DESCRIPTOR_BINDING_PARTIALLY_BOUND_BIT_EXT;
        flags |= VK_DESCRIPTOR_BINDING_VARIABLE_DESCRIPTOR_COUNT_BIT_EXT;
        flags |= VK_DESCRIPTOR_BINDING_UPDATE_AFTER_BIND_BIT_EXT;
        bindingFlags.pBindingFlags = &flags;
        createInfo.pNext = &bindingFlags;
    }

    result = s_Vk.createDescriptorSetLayout(
        vkDevice->handle,
        &createInfo,
        &s_Vk.vkAllocator,
        &layout->handle);

    if (result != VK_SUCCESS) {
        palFree(s_Vk.allocator, layout);
        palFree(s_Vk.allocator, bindings);
        return resultFromVk(result);
    }

    layout->device = vkDevice;
    palFree(s_Vk.allocator, bindings);
    *outLayout = (PalDescriptorSetLayout*)layout;
    return PAL_RESULT_SUCCESS;
}

void PAL_CALL destroyDescriptorSetLayoutVk(PalDescriptorSetLayout* layout)
{
    DescriptorSetLayout* vkLayout = (DescriptorSetLayout*)layout;
    s_Vk.destroyDescriptorSetLayout(
        vkLayout->device->handle,
        vkLayout->handle,
        &s_Vk.vkAllocator);

    palFree(s_Vk.allocator, layout);
}

PalResult PAL_CALL createDescriptorPoolVk(
    PalDevice* device,
    const PalDescriptorPoolCreateInfo* info,
    PalDescriptorPool** outPool)
{
    VkResult result;
    Device* vkDevice = (Device*)device;
    DescriptorPool* pool = nullptr;
    VkDescriptorPoolSize* poolSizes = nullptr;
    Uint32 maxBindings = info->maxDescriptorBindingSizes;

    pool = palAllocate(s_Vk.allocator, sizeof(DescriptorPool), 0);
    poolSizes = palAllocate(s_Vk.allocator, sizeof(VkDescriptorPoolSize) * maxBindings, 0);
    if (!pool || !poolSizes) {
        return PAL_RESULT_OUT_OF_MEMORY;
    }

    for (int i = 0; i < maxBindings; i++) {
        VkDescriptorPoolSize* poolSize = &poolSizes[i];
        poolSize->descriptorCount = info->bindingSizes[i].bindingCount;
        poolSize->type = descriptortypeToVk(info->bindingSizes[i].descriptorType);
    }

    VkDescriptorPoolCreateInfo createInfo = {0};
    createInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    createInfo.maxSets = info->maxDescriptorSets;
    createInfo.poolSizeCount = maxBindings;
    createInfo.pPoolSizes = poolSizes;
    createInfo.flags = VK_DESCRIPTOR_POOL_CREATE_UPDATE_AFTER_BIND_BIT_EXT;

    result = s_Vk.createDescriptorPool(
        vkDevice->handle,
        &createInfo,
        &s_Vk.vkAllocator,
        &pool->handle);

    if (result != VK_SUCCESS) {
        palFree(s_Vk.allocator, pool);
        palFree(s_Vk.allocator, poolSizes);
        return resultFromVk(result);
    }

    pool->device = vkDevice;
    palFree(s_Vk.allocator, poolSizes);
    *outPool = (PalDescriptorPool*)pool;
    return PAL_RESULT_SUCCESS;
}

void PAL_CALL destroyDescriptorPoolVk(PalDescriptorPool* pool)
{
    DescriptorPool* vkPool = (DescriptorPool*)pool;
    s_Vk.destroyDescriptorPool(vkPool->device->handle, vkPool->handle, &s_Vk.vkAllocator);
    palFree(s_Vk.allocator, pool);
}

PalResult PAL_CALL resetDescriptorPoolVk(PalDescriptorPool* pool)
{
    DescriptorPool* vkPool = (DescriptorPool*)pool;
    s_Vk.resetDescriptorPool(vkPool->device->handle, vkPool->handle, 0);
    return PAL_RESULT_SUCCESS;
}

PalResult PAL_CALL allocateDescriptorSetVk(
    PalDevice* device,
    PalDescriptorPool* pool,
    PalDescriptorSetLayout* layout,
    PalDescriptorSet** outSet)
{
    VkResult result;
    Device* vkDevice = (Device*)device;
    DescriptorPool* vkPool = (DescriptorPool*)pool;
    DescriptorSetLayout* vkLayout = (DescriptorSetLayout*)layout;
    DescriptorSet* set = nullptr;

    set = palAllocate(s_Vk.allocator, sizeof(DescriptorSet), 0);
    if (!set) {
        return PAL_RESULT_OUT_OF_MEMORY;
    }

    VkDescriptorSetAllocateInfo allocateInfo = {0};
    allocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocateInfo.descriptorPool = vkPool->handle;
    allocateInfo.descriptorSetCount = 1;
    allocateInfo.pSetLayouts = &vkLayout->handle;

    result = s_Vk.allocateDescriptorSet(vkDevice->handle, &allocateInfo, &set->handle);
    if (result != VK_SUCCESS) {
        palFree(s_Vk.allocator, set);
        return resultFromVk(result);
    }

    set->pool = vkPool;
    set->device = vkDevice;
    *outSet = (PalDescriptorSet*)set;
    return PAL_RESULT_SUCCESS;
}

PalResult PAL_CALL updateDescriptorSetVk(
    PalDevice* device,
    Uint32 count,
    PalDescriptorSetWriteInfo* infos)
{
    VkResult result;
    Device* vkDevice = (Device*)device;
    VkWriteDescriptorSet* writes = nullptr;
    VkDescriptorBufferInfo* bufferInfos = nullptr;
    VkDescriptorImageInfo* imageInfos = nullptr;
    VkWriteDescriptorSetAccelerationStructureKHR* tlasInfos = nullptr;

    Uint32 bufferCount = 0;
    Uint32 imageCount = 0;
    Uint32 tlasCount = 0;
    Uint32 bufferIndex = 0;
    Uint32 imageIndex = 0;
    Uint32 tlasIndex = 0;

    for (int i = 0; i < count; i++) {
        if (infos[i].descriptorType == PAL_DESCRIPTOR_TYPE_STORAGE_BUFFER ||
            infos[i].descriptorType == PAL_DESCRIPTOR_TYPE_UNIFORM_BUFFER) {
            bufferCount++;

        } else if (infos[i].descriptorType == PAL_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE) {
            tlasCount++;

        } else {
            imageCount++;
        }
    }

    writes = palAllocate(s_Vk.allocator, sizeof(VkWriteDescriptorSet) * count, 0);
    if (!writes) {
        return PAL_RESULT_OUT_OF_MEMORY;
    }

    bufferInfos = palAllocate(s_Vk.allocator, sizeof(VkDescriptorBufferInfo) * bufferCount, 0);
    if (!bufferInfos && bufferCount) {
        return PAL_RESULT_OUT_OF_MEMORY;
    }

    imageInfos = palAllocate(s_Vk.allocator, sizeof(VkDescriptorImageInfo) * imageCount, 0);
    if (!imageInfos && imageCount) {
        return PAL_RESULT_OUT_OF_MEMORY;
    }

    Uint32 tlasInfoSize = sizeof(VkWriteDescriptorSetAccelerationStructureKHR) * tlasCount;
    tlasInfos = palAllocate(s_Vk.allocator, tlasInfoSize, 0);
    if (!tlasInfos && tlasCount) {
        return PAL_RESULT_OUT_OF_MEMORY;
    }

    for (int i = 0; i < count; i++) {
        VkWriteDescriptorSet* write = &writes[i];
        write->sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        write->pBufferInfo = nullptr;
        write->pImageInfo = nullptr;
        write->pTexelBufferView = nullptr;
        write->pNext = nullptr;

        write->dstArrayElement = infos[i].arrayElement;
        write->dstBinding = infos[i].binding;
        write->descriptorCount = infos[i].descriptorCount;
        write->descriptorType = descriptortypeToVk(infos[i].descriptorType);

        DescriptorSet* set = (DescriptorSet*)infos[i].descriptorSet;
        write->dstSet = set->handle;

        if (infos[i].descriptorType == PAL_DESCRIPTOR_TYPE_STORAGE_BUFFER ||
            infos[i].descriptorType == PAL_DESCRIPTOR_TYPE_UNIFORM_BUFFER) {
            VkDescriptorBufferInfo* bufferInfo = &bufferInfos[bufferIndex++];
            Buffer* vkBuffer = (Buffer*)infos[i].bufferInfo->buffer;
            bufferInfo->buffer = vkBuffer->handle;
            bufferInfo->offset = infos[i].bufferInfo->offset;
            bufferInfo->range = infos[i].bufferInfo->size;
            write->pBufferInfo = bufferInfo;

        } else if (infos[i].descriptorType == PAL_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE) {
            VkWriteDescriptorSetAccelerationStructureKHR* tlasInfo = &tlasInfos[tlasIndex++];
            AccelerationStructure* ac = (AccelerationStructure*)infos[i].tlasInfo->tlas;

            tlasInfo->sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET_ACCELERATION_STRUCTURE_KHR;
            tlasInfo->accelerationStructureCount = 1;
            tlasInfo->pAccelerationStructures = &ac->handle;
            tlasInfo->pNext = nullptr;
            write->pNext = tlasInfo;

        } else {
            VkDescriptorImageInfo* imageInfo = &imageInfos[imageIndex++];
            if (infos[i].descriptorType == PAL_DESCRIPTOR_TYPE_SAMPLER) {
                Sampler* vkSampler = (Sampler*)infos[i].samplerInfo->sampler;

                imageInfo->sampler = vkSampler->handle;
                imageInfo->imageLayout = VK_IMAGE_LAYOUT_UNDEFINED;
                imageInfo->imageView = nullptr;

            } else if (infos[i].descriptorType == PAL_DESCRIPTOR_TYPE_STORAGE_IMAGE) {
                ImageView* vkImageView = (ImageView*)infos[i].imageViewInfo->imageView;

                imageInfo->sampler = nullptr;
                imageInfo->imageLayout = VK_IMAGE_LAYOUT_GENERAL;
                imageInfo->imageView = vkImageView->handle;

            } else if (infos[i].descriptorType == PAL_DESCRIPTOR_TYPE_SAMPLED_IMAGE) {
                ImageView* vkImageView = (ImageView*)infos[i].imageViewInfo->imageView;

                imageInfo->sampler = nullptr;
                imageInfo->imageLayout = VK_IMAGE_LAYOUT_GENERAL;
                imageInfo->imageView = vkImageView->handle;
            }
            write->pImageInfo = imageInfo;
        }
    }

    s_Vk.updateDescriptorSet(vkDevice->handle, count, writes, 0, nullptr);
    palFree(s_Vk.allocator, writes);
    palFree(s_Vk.allocator, bufferInfos);
    palFree(s_Vk.allocator, imageInfos);
    palFree(s_Vk.allocator, tlasInfos);
    return PAL_RESULT_SUCCESS;
}

// ==================================================
// Pipeline Layout
// ==================================================

PalResult PAL_CALL createPipelineLayoutVk(
    PalDevice* device,
    const PalPipelineLayoutCreateInfo* info,
    PalPipelineLayout** outLayout)
{
    VkResult result;
    Device* vkDevice = (Device*)device;
    PipelineLayout* layout = nullptr;
    VkPushConstantRange* pushConstants = nullptr;
    VkDescriptorSetLayout* descriptorLayouts = nullptr;
    Uint32 pushConstantSize = sizeof(VkPushConstantRange) * info->pushConstantRangeCount;
    Uint32 setLayoutSize = sizeof(VkDescriptorSetLayout) * info->descriptorSetLayoutCount;

    layout = palAllocate(s_Vk.allocator, sizeof(PipelineLayout), 0);
    pushConstants = palAllocate(s_Vk.allocator, pushConstantSize, 0);
    descriptorLayouts = palAllocate(s_Vk.allocator, setLayoutSize, 0);
    if (!layout || !pushConstants || !descriptorLayouts) {
        return PAL_RESULT_OUT_OF_MEMORY;
    }

    for (int i = 0; i < info->descriptorSetLayoutCount; i++) {
        DescriptorSetLayout* tmp = (DescriptorSetLayout*)info->descriptorSetLayouts[i];
        descriptorLayouts[i] = tmp->handle;
    }

    for (int i = 0; i < info->pushConstantRangeCount; i++) {
        VkPushConstantRange* range = &pushConstants[i];
        range->offset = info->pushConstantRanges[i].offset;
        range->size = info->pushConstantRanges[i].size;
        range->stageFlags = 0;

        range->offset = info->pushConstantRanges[i].offset;
        for (int j = 0; j < info->pushConstantRanges[i].shaderStageCount; j++) {
            VkShaderStageFlags bit = shaderStageToVK(info->pushConstantRanges[i].shaderStages[j]);
            range->stageFlags |= bit;
        }
    }

    VkPipelineLayoutCreateInfo createInfo = {0};
    createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    createInfo.setLayoutCount = info->descriptorSetLayoutCount;
    createInfo.pSetLayouts = descriptorLayouts;
    createInfo.pPushConstantRanges = pushConstants;
    createInfo.pushConstantRangeCount = info->pushConstantRangeCount;

    result = s_Vk.createPipelineLayout(
        vkDevice->handle,
        &createInfo,
        &s_Vk.vkAllocator,
        &layout->handle);

    if (result != VK_SUCCESS) {
        palFree(s_Vk.allocator, descriptorLayouts);
        palFree(s_Vk.allocator, pushConstants);
        palFree(s_Vk.allocator, layout);
        return resultFromVk(result);
    }

    palFree(s_Vk.allocator, descriptorLayouts);
    palFree(s_Vk.allocator, pushConstants);
    layout->device = vkDevice;
    *outLayout = (PalPipelineLayout*)layout;
    return PAL_RESULT_SUCCESS;
}

void PAL_CALL destroyPipelineLayoutVk(PalPipelineLayout* layout)
{
    PipelineLayout* pipelineLayout = (PipelineLayout*)layout;
    s_Vk.destroyPipelineLayout(
        pipelineLayout->device->handle,
        pipelineLayout->handle,
        &s_Vk.vkAllocator);

    palFree(s_Vk.allocator, layout);
}

// ==================================================
// Pipeline
// ==================================================

PalResult PAL_CALL createGraphicsPipelineVk(
    PalDevice* device,
    const PalGraphicsPipelineCreateInfo* info,
    PalPipeline** outPipeline)
{
    VkResult result;
    Pipeline* pipeline = nullptr;
    Device* vkDevice = (Device*)device;
    PipelineLayout* layout = (PipelineLayout*)info->pipelineLayout;

    VkPipelineShaderStageCreateInfo shaderStages[7]; // 7 shader types for graphics pipeline
    VkDynamicState dynamicStates[16];
    VkVertexInputBindingDescription* bindingDescs = nullptr;
    VkVertexInputAttributeDescription* attribDescs = nullptr;
    VkPipelineColorBlendAttachmentState* blendattachments = nullptr;

    VkPipelineVertexInputStateCreateInfo vertexInputState = {0};
    vertexInputState.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;

    VkPipelineInputAssemblyStateCreateInfo inputAssemblyState = {0};
    inputAssemblyState.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;

    VkPipelineDynamicStateCreateInfo dynamicState = {0};
    dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;

    VkPipelineRasterizationStateCreateInfo rasterizerState = {0};
    rasterizerState.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;

    VkPipelineMultisampleStateCreateInfo multisampleState = {0};
    multisampleState.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;

    VkPipelineDepthStencilStateCreateInfo depthStencilState = {0};
    depthStencilState.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;

    VkPipelineColorBlendStateCreateInfo colorBlendState = {0};
    colorBlendState.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;

    VkPipelineTessellationStateCreateInfo tessellationState = {0};
    tessellationState.sType = VK_STRUCTURE_TYPE_PIPELINE_TESSELLATION_STATE_CREATE_INFO;

    VkPipelineViewportStateCreateInfo viewportState = {0};
    viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;

    VkPipelineFragmentShadingRateStateCreateInfoKHR fsrState = {0};
    fsrState.sType = VK_STRUCTURE_TYPE_PIPELINE_FRAGMENT_SHADING_RATE_STATE_CREATE_INFO_KHR;

    VkPipelineRenderingCreateInfoKHR dynRendering = {0};
    dynRendering.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO_KHR;

    VkGraphicsPipelineCreateInfo createInfo = {0};
    createInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    createInfo.renderPass = VK_NULL_HANDLE;
    createInfo.layout = layout->handle;

    pipeline = palAllocate(s_Vk.allocator, sizeof(Pipeline), 0);
    if (!pipeline) {
        return PAL_RESULT_OUT_OF_MEMORY;
    }

    // shaders
    for (int i = 0; i < info->shaderCount; i++) {
        Shader* tmp = (Shader*)info->shaders[i];
        shaderStages[i] = tmp->info;

        if (tmp->patchControlPoints) {
            tessellationState.patchControlPoints = tmp->patchControlPoints;
            createInfo.pTessellationState = &tessellationState;

            if (info->topology != PAL_PRIMITIVE_TOPOLOGY_PATCH) {
                palFree(s_Vk.allocator, pipeline);
                return PAL_RESULT_INVALID_OPERATION;
            }
        }
    }
    createInfo.stageCount = info->shaderCount;
    createInfo.pStages = shaderStages;

    // Vertex input state
    // get the max size of vertex attributes in all layouts
    Uint32 vertexCount = 0;
    for (int i = 0; i < info->vertexLayoutCount; i++) {
        PalVertexLayout* layout = &info->vertexLayouts[i];
        vertexCount += layout->attributeCount;
    }

    if (vertexCount) {
        Uint32 tmpBindingSize = sizeof(VkVertexInputBindingDescription) * info->vertexLayoutCount;
        Uint32 tmpAttribSize = sizeof(VkVertexInputAttributeDescription) * vertexCount;
        bindingDescs = palAllocate(s_Vk.allocator, tmpBindingSize, 0);
        attribDescs = palAllocate(s_Vk.allocator, tmpAttribSize, 0);
        if (!bindingDescs || !attribDescs) {
            palFree(s_Vk.allocator, pipeline);
            return PAL_RESULT_OUT_OF_MEMORY;
        }

        Uint32 location = 0;
        for (int i = 0; i < info->vertexLayoutCount; i++) {
            PalVertexLayout* layout = &info->vertexLayouts[i];
            VkVertexInputBindingDescription* bindingDesc = &bindingDescs[i];

            bindingDesc->binding = layout->binding;
            if (layout->type == PAL_VERTEX_LAYOUT_TYPE_PER_INSTANCE) {
                bindingDesc->inputRate = VK_VERTEX_INPUT_RATE_INSTANCE;
            } else {
                bindingDesc->inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
            }

            // find the stride and offset of the layout
            bindingDesc->stride = 0;
            Uint32 offset = 0;
            for (int j = 0; j < layout->attributeCount; j++) {
                PalVertexAttribute* vertexAttrib = &layout->attributes[j];
                VkVertexInputAttributeDescription* attribDesc = &attribDescs[j];

                attribDesc->format = vertexTypeToVk(vertexAttrib->type);
                attribDesc->binding = bindingDesc->binding;
                attribDesc->location = location++;

                // build offsets and stride
                Uint32 size = getVertexTypeSizeVk(vertexAttrib->type);
                attribDesc->offset = offset;
                offset += size;
                bindingDesc->stride += size;
            }
        }

        vertexInputState.pVertexAttributeDescriptions = attribDescs;
        vertexInputState.vertexAttributeDescriptionCount = vertexCount;
        vertexInputState.pVertexBindingDescriptions = bindingDescs;
        vertexInputState.vertexBindingDescriptionCount = info->vertexLayoutCount;
    }
    createInfo.pVertexInputState = &vertexInputState;

    // Input assembly
    VkPrimitiveTopology topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    switch (info->topology) {
        case PAL_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST: {
            topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
            break;
        }

        case PAL_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP: {
            topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP;
            break;
        }

        case PAL_PRIMITIVE_TOPOLOGY_LINE_LIST: {
            topology = VK_PRIMITIVE_TOPOLOGY_LINE_LIST;
            break;
        }

        case PAL_PRIMITIVE_TOPOLOGY_LINE_STRIP: {
            topology = VK_PRIMITIVE_TOPOLOGY_LINE_STRIP;
            break;
        }

        case PAL_PRIMITIVE_TOPOLOGY_POINT_LIST: {
            topology = VK_PRIMITIVE_TOPOLOGY_POINT_LIST;
            break;
        }
    }
    inputAssemblyState.topology = topology;
    inputAssemblyState.primitiveRestartEnable = info->primitiveRestartEnable;
    createInfo.pInputAssemblyState = &inputAssemblyState;

    // Dynamic states
    Uint32 dynCount = 0;
    dynamicStates[dynCount++] = VK_DYNAMIC_STATE_VIEWPORT;
    dynamicStates[dynCount++] = VK_DYNAMIC_STATE_SCISSOR;
    dynamicStates[dynCount++] = VK_DYNAMIC_STATE_LINE_WIDTH;
    dynamicStates[dynCount++] = VK_DYNAMIC_STATE_BLEND_CONSTANTS;
    dynamicStates[dynCount++] = VK_DYNAMIC_STATE_DEPTH_BIAS;
    dynamicStates[dynCount++] = VK_DYNAMIC_STATE_STENCIL_REFERENCE;

    if (vkDevice->features & PAL_ADAPTER_FEATURE_DYNAMIC_CULL_MODE) {
        dynamicStates[dynCount++] = VK_DYNAMIC_STATE_CULL_MODE_EXT;
    }

    if (vkDevice->features & PAL_ADAPTER_FEATURE_DYNAMIC_FRONT_FACE) {
        dynamicStates[dynCount++] = VK_DYNAMIC_STATE_FRONT_FACE_EXT;
    }

    if (vkDevice->features & PAL_ADAPTER_FEATURE_DYNAMIC_PRIMITIVE_TOPOLOGY) {
        dynamicStates[dynCount++] = VK_DYNAMIC_STATE_PRIMITIVE_TOPOLOGY_EXT;
    }

    if (vkDevice->features & PAL_ADAPTER_FEATURE_DYNAMIC_DEPTH_TEST_ENABLE) {
        dynamicStates[dynCount++] = VK_DYNAMIC_STATE_DEPTH_TEST_ENABLE_EXT;
    }

    if (vkDevice->features & PAL_ADAPTER_FEATURE_DYNAMIC_DEPTH_WRITE_ENABLE) {
        dynamicStates[dynCount++] = VK_DYNAMIC_STATE_DEPTH_WRITE_ENABLE_EXT;
    }

    if (vkDevice->features & PAL_ADAPTER_FEATURE_DYNAMIC_STENCIL_OP) {
        dynamicStates[dynCount++] = VK_DYNAMIC_STATE_STENCIL_OP_EXT;
    }

    if (info->fragmentShadingRateState) {
        dynamicStates[dynCount++] = VK_DYNAMIC_STATE_FRAGMENT_SHADING_RATE_KHR;
    }

    dynamicState.dynamicStateCount = dynCount;
    dynamicState.pDynamicStates = dynamicStates;
    createInfo.pDynamicState = &dynamicState;

    // Rasterizer state
    rasterizerState.frontFace = VK_FRONT_FACE_CLOCKWISE;
    if (info->rasterizerState) {
        PalRasterizerState* state = info->rasterizerState;
        if (state->cullMode == PAL_CULL_MODE_NONE) {
            rasterizerState.cullMode = VK_CULL_MODE_NONE;

        } else if (state->cullMode == PAL_CULL_MODE_BACK) {
            rasterizerState.cullMode = VK_CULL_MODE_BACK_BIT;

        } else if (state->cullMode == PAL_CULL_MODE_FRONT) {
            rasterizerState.cullMode = VK_CULL_MODE_FRONT_BIT;
        }

        if (state->polygonMode == PAL_POLYGON_MODE_FILL) {
            rasterizerState.polygonMode = VK_POLYGON_MODE_FILL;

        } else {
            rasterizerState.polygonMode = VK_POLYGON_MODE_LINE;
        }

        if (state->frontFace == PAL_FRONT_FACE_CLOCKWISE) {
            rasterizerState.frontFace = VK_FRONT_FACE_CLOCKWISE;

        } else {
            rasterizerState.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
        }

        rasterizerState.depthBiasEnable = state->enableDepthBias;
        rasterizerState.depthClampEnable = state->enableDepthClamp;
        rasterizerState.depthBiasConstantFactor = state->depthBiasConstant;
        rasterizerState.depthBiasSlopeFactor = state->depthBiasSlope;
        rasterizerState.depthBiasClamp = state->depthBiasClamp;
    }

    rasterizerState.lineWidth = 1.0f;
    createInfo.pRasterizationState = &rasterizerState;

    // Multisample state
    Uint32 sampleMask[2] = {0}; // PAL supports upto 64 samples
    multisampleState.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
    multisampleState.pSampleMask = nullptr;
    if (info->multisampleState) {
        PalMultisampleState* state = info->multisampleState;
        multisampleState.alphaToCoverageEnable = state->enableAlphaToCoverage;
        multisampleState.minSampleShading = state->minSampleShading;
        multisampleState.sampleShadingEnable = state->enableSampleShading;
        multisampleState.rasterizationSamples = samplesToVk(state->sampleCount);

        // clang-format off
        if (state->sampleMask) {
            if (info->multisampleState->sampleCount == PAL_SAMPLE_COUNT_1  || 
                info->multisampleState->sampleCount == PAL_SAMPLE_COUNT_2  ||
                info->multisampleState->sampleCount == PAL_SAMPLE_COUNT_4  ||
                info->multisampleState->sampleCount == PAL_SAMPLE_COUNT_8  ||
                info->multisampleState->sampleCount == PAL_SAMPLE_COUNT_16 ||
                info->multisampleState->sampleCount == PAL_SAMPLE_COUNT_32) {
                sampleMask[0] = (Uint32)(info->multisampleState->sampleMask & 0xFFFFFFFFULL);

            } else {
                sampleMask[0] = (Uint32)(info->multisampleState->sampleMask & 0xFFFFFFFFULL);
                sampleMask[1] = (Uint32)((info->multisampleState->sampleMask >> 32) & 0xFFFFFFFFULL);
            }
            multisampleState.pSampleMask = sampleMask;

        } else {
            multisampleState.pSampleMask = nullptr;
        }
        // clang-format on
    }
    createInfo.pMultisampleState = &multisampleState;

    // Depth stencil state
    if (info->depthStencilState) {
        PalDepthStencilState* state = info->depthStencilState;
        PalStencilOpState* back = &state->backStencilOpState;
        PalStencilOpState* front = &state->frontStencilOpState;

        VkStencilOpState* vkBack = &depthStencilState.back;
        VkStencilOpState* vkFront = &depthStencilState.front;

        vkBack->compareOp = compareOpToVk(back->compareOp);
        vkBack->depthFailOp = stencilOpToVk(back->depthFailOp);
        vkBack->failOp = stencilOpToVk(back->failOp);
        vkBack->passOp = stencilOpToVk(back->passOp);

        vkFront->compareOp = compareOpToVk(front->compareOp);
        vkFront->depthFailOp = stencilOpToVk(front->depthFailOp);
        vkFront->failOp = stencilOpToVk(front->failOp);
        vkFront->passOp = stencilOpToVk(front->passOp);

        depthStencilState.depthCompareOp = compareOpToVk(state->compareOp);
        depthStencilState.depthTestEnable = state->enableDepthTest;
        depthStencilState.depthWriteEnable = state->enableDepthWrite;
        depthStencilState.stencilTestEnable = state->enableStencilTest;
    }
    createInfo.pDepthStencilState = &depthStencilState;

    // Color blend state
    if (info->colorBlendAttachmentCount) {
        Uint32 count = info->colorBlendAttachmentCount;
        Uint32 size = sizeof(VkPipelineColorBlendAttachmentState) * count;
        blendattachments = palAllocate(s_Vk.allocator, size, 0);
        if (!blendattachments) {
            return PAL_RESULT_OUT_OF_MEMORY;
        }

        for (int i = 0; i < count; i++) {
            VkPipelineColorBlendAttachmentState* tmp = &blendattachments[i];
            PalColorBlendAttachment* desc = &info->colorBlendAttachments[i];

            tmp->blendEnable = desc->enableBlend;
            tmp->alphaBlendOp = blendOpToVk(desc->alphaBlendOp);
            tmp->colorBlendOp = blendOpToVk(desc->colorBlendOp);

            tmp->srcAlphaBlendFactor = blendFactorToVk(desc->srcAlphaBlendFactor);
            tmp->srcColorBlendFactor = blendFactorToVk(desc->srcColorBlendFactor);

            tmp->dstAlphaBlendFactor = blendFactorToVk(desc->dstAlphaBlendFactor);
            tmp->dstColorBlendFactor = blendFactorToVk(desc->dstColorBlendFactor);

            // blend color write mask
            tmp->colorWriteMask = 0;
            if (desc->colorWriteMask & PAL_COLOR_MASK_RED) {
                tmp->colorWriteMask |= VK_COLOR_COMPONENT_R_BIT;
            }

            if (desc->colorWriteMask & PAL_COLOR_MASK_GREEN) {
                tmp->colorWriteMask |= VK_COLOR_COMPONENT_G_BIT;
            }

            if (desc->colorWriteMask & PAL_COLOR_MASK_BLUE) {
                tmp->colorWriteMask |= VK_COLOR_COMPONENT_B_BIT;
            }

            if (desc->colorWriteMask & PAL_COLOR_MASK_ALPHA) {
                tmp->colorWriteMask |= VK_COLOR_COMPONENT_A_BIT;
            }
        }

        colorBlendState.attachmentCount = count;
        colorBlendState.pAttachments = blendattachments;
    }
    createInfo.pColorBlendState = &colorBlendState;

    // viewport state
    viewportState.viewportCount = 1;
    viewportState.scissorCount = 1;
    createInfo.pViewportState = &viewportState;

    // Fragment shading rate
    if (info->fragmentShadingRateState) {
        PalFragmentShadingRateState* state = info->fragmentShadingRateState;
        for (int i = 0; i < 2; i++) {
            VkFragmentShadingRateCombinerOpKHR combinerOp;
            combinerOp = combinerOpsToVk(state->combinerOps[i]);
            fsrState.combinerOps[i] = combinerOp;
        }

        VkExtent2D size = getShadingRateSizeVk(state->rate);
        fsrState.fragmentSize = size;
        createInfo.pNext = &fsrState;
    }

    // layout info
    VkFormat format = VK_FORMAT_UNDEFINED;
    VkFormat colorAttachments[MAX_ATTACHMENTS];
    PalRenderingLayoutInfo* renderingLayout = info->renderingLayout;

    // color attachments
    for (int i = 0; i < renderingLayout->colorAttachentCount; i++) {
        format = formatToVk(renderingLayout->colorAttachmentsFormat[i]);
        colorAttachments[i] = format;
    }
    dynRendering.colorAttachmentCount = renderingLayout->colorAttachentCount;
    dynRendering.pColorAttachmentFormats = colorAttachments;

    // depth stencil attachment
    format = formatToVk(renderingLayout->depthStencilAttachmentFormat);
    dynRendering.depthAttachmentFormat = format;
    dynRendering.stencilAttachmentFormat = format;

    if (renderingLayout->viewCount == 1) {
        dynRendering.viewMask = 0;
    } else {
        dynRendering.viewMask = (1 << renderingLayout->viewCount) - 1;
    }
    createInfo.pNext = &dynRendering;

    result = s_Vk.createGraphicsPipeline(
        vkDevice->handle,
        0,
        1,
        &createInfo,
        &s_Vk.vkAllocator,
        &pipeline->handle);

    if (result != VK_SUCCESS) {
        palFree(s_Vk.allocator, pipeline);
        return resultFromVk(result);
    }

    if (info->vertexLayoutCount) {
        palFree(s_Vk.allocator, bindingDescs);
        palFree(s_Vk.allocator, attribDescs);
    }

    if (info->colorBlendAttachmentCount) {
        palFree(s_Vk.allocator, blendattachments);
    }

    pipeline->bindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    pipeline->device = vkDevice;
    pipeline->layout = layout->handle;
    *outPipeline = (PalPipeline*)pipeline;
    return PAL_RESULT_SUCCESS;
}

PalResult PAL_CALL createComputePipelineVk(
    PalDevice* device,
    const PalComputePipelineCreateInfo* info,
    PalPipeline** outPipeline)
{
    Device* vkDevice = (Device*)device;
    PipelineLayout* layout = (PipelineLayout*)info->pipelineLayout;
    Shader* shader = (Shader*)info->computeShader;
    Pipeline* pipeline = nullptr;

    pipeline = palAllocate(s_Vk.allocator, sizeof(Pipeline), 0);
    if (!pipeline) {
        return PAL_RESULT_OUT_OF_MEMORY;
    }

    VkComputePipelineCreateInfo createInfo = {0};
    createInfo.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
    createInfo.layout = layout->handle;
    createInfo.stage = shader->info;

    VkResult result = s_Vk.createComputePipeline(
        vkDevice->handle,
        nullptr,
        1,
        &createInfo,
        &s_Vk.vkAllocator,
        &pipeline->handle);

    if (result != VK_SUCCESS) {
        palFree(s_Vk.allocator, pipeline);
        return resultFromVk(result);
    }

    pipeline->bindPoint = VK_PIPELINE_BIND_POINT_COMPUTE;
    pipeline->device = vkDevice;
    pipeline->layout = layout->handle;
    *outPipeline = (PalPipeline*)pipeline;
    return PAL_RESULT_SUCCESS;
}

PalResult PAL_CALL createRayTracingPipelineVk(
    PalDevice* device,
    const PalRayTracingPipelineCreateInfo* info,
    PalPipeline** outPipeline)
{
    VkResult result;
    Device* vkDevice = (Device*)device;
    PipelineLayout* layout = (PipelineLayout*)info->pipelineLayout;
    Pipeline* pipeline = nullptr;
    VkPipelineShaderStageCreateInfo* shaderStages = nullptr; 
    VkRayTracingShaderGroupCreateInfoKHR* groups = nullptr;
    Uint32 groupSize = sizeof(VkRayTracingShaderGroupCreateInfoKHR) * info->shaderGroupCount;
    Uint32 shaderSize = sizeof(VkPipelineShaderStageCreateInfo) * info->shaderCount;

    if (!(vkDevice->features & PAL_ADAPTER_FEATURE_RAY_TRACING)) {
        return PAL_RESULT_ADAPTER_FEATURE_NOT_SUPPORTED;
    }

    VkRayTracingPipelineCreateInfoKHR createInfo = {0};
    createInfo.sType = VK_STRUCTURE_TYPE_RAY_TRACING_PIPELINE_CREATE_INFO_KHR;

    pipeline = palAllocate(s_Vk.allocator, sizeof(Pipeline), 0);
    groups = palAllocate(s_Vk.allocator, groupSize, 0);
    shaderStages = palAllocate(s_Vk.allocator, shaderSize, 0);
    if (!pipeline || !groups || !shaderStages) {
        return PAL_RESULT_OUT_OF_MEMORY;
    }

    // shaders
    memset(groups, 0, sizeof(VkRayTracingShaderGroupCreateInfoKHR) * info->shaderGroupCount);
    for (int i = 0; i < info->shaderCount; i++) {
        Shader* tmp = (Shader*)info->shaders[i];
        shaderStages[i] = tmp->info;
    }

    // shader groups
    for (int i = 0; i < info->shaderGroupCount; i++) {
        VkRayTracingShaderGroupCreateInfoKHR* group = &groups[i];
        group->sType = VK_STRUCTURE_TYPE_RAY_TRACING_SHADER_GROUP_CREATE_INFO_KHR;
        if (info->shaderGroups[i].type == PAL_RAY_TRACING_SHADER_GROUP_TYPE_GENERAL) {
            group->type = VK_RAY_TRACING_SHADER_GROUP_TYPE_GENERAL_KHR;

        } else if (info->shaderGroups[i].type == PAL_RAY_TRACING_SHADER_GROUP_TYPE_TRIANGLES_HIT) {
            group->type = VK_RAY_TRACING_SHADER_GROUP_TYPE_TRIANGLES_HIT_GROUP_KHR;

        } else {
            group->type = VK_RAY_TRACING_SHADER_GROUP_TYPE_PROCEDURAL_HIT_GROUP_KHR;
        }

        group->anyHitShader = info->shaderGroups[i].anyHitShaderIndex;
        group->closestHitShader = info->shaderGroups[i].closestHitShaderIndex;
        group->generalShader = info->shaderGroups[i].generalShaderIndex;
        group->intersectionShader = info->shaderGroups[i].intersectionShaderIndex;
    }

    createInfo.stageCount = info->shaderCount;
    createInfo.pStages = shaderStages;
    createInfo.pGroups = groups;
    createInfo.groupCount = info->shaderGroupCount;
    createInfo.maxPipelineRayRecursionDepth = info->maxRecursionDepth;
    createInfo.layout = layout->handle;

    result = vkDevice->createRayTracingPipeline(
        vkDevice->handle,
        nullptr,
        nullptr,
        1,
        &createInfo,
        &s_Vk.vkAllocator,
        &pipeline->handle);

    palFree(s_Vk.allocator, groups);
    palFree(s_Vk.allocator, shaderStages);
    if (result != VK_SUCCESS) {
        palFree(s_Vk.allocator, pipeline);
        return resultFromVk(result);
    }

    pipeline->bindPoint = VK_PIPELINE_BIND_POINT_RAY_TRACING_KHR;
    pipeline->device = vkDevice;
    pipeline->layout = layout->handle;
    *outPipeline = (PalPipeline*)pipeline;
    return PAL_RESULT_SUCCESS;
}

void PAL_CALL destroyPipelineVk(PalPipeline* pipeline)
{
    Pipeline* vkPipeline = (Pipeline*)pipeline;
    s_Vk.destroyPipeline(vkPipeline->device->handle, vkPipeline->handle, &s_Vk.vkAllocator);

    palFree(s_Vk.allocator, pipeline);
}

// ==================================================
// Shader Binding Table
// ==================================================

PalResult PAL_CALL createShaderBindingTableVk(
    PalDevice* device,
    const PalShaderBindingTableCreateInfo* info,
    PalShaderBindingTable** outSbt)
{
    VkResult result;
    Device* vkDevice = (Device*)device;
    ShaderBindingTable* sbt = nullptr;
    Pipeline* pipeline = (Pipeline*)info->rayTracingPipeline;

    if (!(vkDevice->features & PAL_ADAPTER_FEATURE_RAY_TRACING)) {
        return PAL_RESULT_ADAPTER_FEATURE_NOT_SUPPORTED;
    }

    sbt = palAllocate(s_Vk.allocator, sizeof(ShaderBindingTable), 0);
    if (!sbt) {
        return PAL_RESULT_OUT_OF_MEMORY;
    }

    // create SBT buffer
    VkPhysicalDeviceRayTracingPipelinePropertiesKHR rayProps = {0};
    rayProps.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_TRACING_PIPELINE_PROPERTIES_KHR;

    VkPhysicalDeviceProperties2KHR props = {0};
    props.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2_KHR;
    props.pNext = &rayProps;
    s_Vk.getPhysicalDeviceProperties2(vkDevice->phyDevice, &props);

    Uint32 groupHandleSize = rayProps.shaderGroupHandleSize;
    Uint32 groupHandleAlignment = rayProps.shaderGroupHandleAlignment;
    Uint32 groupBaseAlignment = rayProps.shaderGroupBaseAlignment;
    Uint32 stride = alignVk(groupHandleSize, groupHandleAlignment);

    // get region size
    Uint32 raygenRegionSize = stride * info->raygenGroupCount;
    Uint32 missRegionSize = stride * info->missGroupCount;
    Uint32 hitRegionSize = stride * info->hitGroupCount;
    Uint32 callableRegionSize = stride * info->callableGroupCount;

    // get aligned region size
    Uint32 raygenAlignedRegionSize = alignVk(raygenRegionSize, groupBaseAlignment);
    Uint32 missAlignedRegionSize = alignVk(missRegionSize, groupBaseAlignment);
    Uint32 hitAlignedRegionSize = alignVk(hitRegionSize, groupBaseAlignment);
    Uint32 callableAligneRegionSize = alignVk(callableRegionSize, groupBaseAlignment);

    // get offsets
    Uint32 missOffset = raygenAlignedRegionSize;
    Uint32 hitOffset = missOffset + missAlignedRegionSize;
    Uint32 callableOffset = hitOffset + hitAlignedRegionSize;
    Uint32 bufferSize = callableOffset + callableAligneRegionSize;

    // create gpu buffer
    VkBufferCreateInfo bufCreateInfo = {0};
    bufCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufCreateInfo.size = bufferSize;
    bufCreateInfo.usage = VK_BUFFER_USAGE_SHADER_BINDING_TABLE_BIT_KHR;
    bufCreateInfo.usage |= VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT_KHR;
    bufCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    result = s_Vk.createBuffer(vkDevice->handle, &bufCreateInfo, &s_Vk.vkAllocator, &sbt->buffer);
    if (result != VK_SUCCESS) {
        palFree(s_Vk.allocator, sbt);
        return resultFromVk(result);
    }

    // allocate CPU upload memory and bind
    VkMemoryRequirements memReq = {0};
    s_Vk.getBufferMemoryRequirements(vkDevice->handle, sbt->buffer, &memReq);

    VkMemoryAllocateInfo allocateInfo = {0};
    allocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocateInfo.allocationSize = memReq.size;

    Uint32 mask = vkDevice->memoryClassMask[PAL_MEMORY_TYPE_CPU_UPLOAD] & memReq.memoryTypeBits;
    Uint32 memoryIndex = findBestMemoryIndexVk(vkDevice->phyDevice, mask);
    allocateInfo.memoryTypeIndex = memoryIndex;

    VkMemoryAllocateFlagsInfo allocateFlagsInfo = {0};
    allocateFlagsInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_FLAGS_INFO;
    allocateFlagsInfo.flags = VK_MEMORY_ALLOCATE_DEVICE_ADDRESS_BIT_KHR;
    allocateInfo.pNext = &allocateFlagsInfo;

    result = s_Vk.allocateMemory(
        vkDevice->handle,
        &allocateInfo,
        &s_Vk.vkAllocator,
        &sbt->bufferMemory);

    if (result != VK_SUCCESS) {
        palFree(s_Vk.allocator, sbt);
        return resultFromVk(result);
    }
    s_Vk.bindBufferMemory(vkDevice->handle, sbt->buffer, sbt->bufferMemory, 0);

    // get shader group handles
    Uint32 totalGroups = info->raygenGroupCount + info->hitGroupCount;
    totalGroups += info->missGroupCount + info->callableGroupCount;
    Uint32 sbtSize = totalGroups * groupHandleSize;
    Uint8* handles = palAllocate(s_Vk.allocator, sbtSize, 0);
    if (!handles) {
        palFree(s_Vk.allocator, sbt);
        return PAL_RESULT_OUT_OF_MEMORY;
    }

    result = vkDevice->getRayTracingShaderGroupHandles(
        vkDevice->handle,
        pipeline->handle,
        0,
        totalGroups,
        sbtSize,
        handles);

    if (result != VK_SUCCESS) {
        palFree(s_Vk.allocator, sbt);
        return resultFromVk(result);
    }

    // copy handles into the buffer
    void* ptr = nullptr;
    result = s_Vk.mapMemory(vkDevice->handle, sbt->bufferMemory, 0, memReq.size, 0, &ptr);
    if (result != VK_SUCCESS) {
        palFree(s_Vk.allocator, sbt);
        return resultFromVk(result);
    }

    Uint8* dstPtr = ptr;
    Uint8* srcPtr = handles;

    // raygen
    for (int i = 0; i < info->raygenGroupCount; i++) {
        memcpy(dstPtr + i * stride, srcPtr + i * groupHandleSize, groupHandleSize);
    }
    srcPtr += raygenRegionSize;

    // miss
    for (int i = 0; i < info->missGroupCount; i++) {
        memcpy(dstPtr + missOffset + i * stride, srcPtr + i * groupHandleSize, groupHandleSize);
    }
    srcPtr += missRegionSize;

    // hit
    for (int i = 0; i < info->hitGroupCount; i++) {
        memcpy(dstPtr + hitOffset + i * stride, srcPtr + i * groupHandleSize, groupHandleSize);
    }
    srcPtr += hitRegionSize;

    // callable
    for (int i = 0; i < info->callableGroupCount; i++) {
        memcpy(dstPtr + callableOffset + i * stride, srcPtr + i * groupHandleSize, groupHandleSize);
    }
    srcPtr += callableRegionSize;

    s_Vk.unmapMemory(vkDevice->handle, sbt->bufferMemory);

    // cache SBT fields and offsets address
    VkBufferDeviceAddressInfo bufferAddressInfo = {0};
    bufferAddressInfo.buffer = sbt->buffer;
    bufferAddressInfo.sType = VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO;
    sbt->baseAddress = s_Vk.getBufferDeviceAddress(vkDevice->handle, &bufferAddressInfo);

    // raygen
    sbt->raygenAddress.deviceAddress = sbt->baseAddress;
    sbt->raygenAddress.size = raygenAlignedRegionSize;
    sbt->raygenAddress.stride = stride;

    // miss
    sbt->missAddress.deviceAddress = sbt->baseAddress + missOffset;
    sbt->missAddress.size = missAlignedRegionSize;
    sbt->missAddress.stride = stride;

    // hit
    sbt->hitAddress.deviceAddress = sbt->baseAddress + hitOffset;
    sbt->hitAddress.size = hitAlignedRegionSize;
    sbt->hitAddress.stride = stride;

    // callable
    sbt->callableAddress.deviceAddress = sbt->baseAddress + callableOffset;
    sbt->callableAddress.size = callableAligneRegionSize;
    sbt->callableAddress.stride = stride;

    palFree(s_Vk.allocator, handles);
    sbt->device = vkDevice;
    *outSbt = (PalShaderBindingTable*)sbt;
    return PAL_RESULT_SUCCESS;
}

void PAL_CALL destroyShaderBindingTableVk(PalShaderBindingTable* sbt)
{
    ShaderBindingTable* vkSbt = (ShaderBindingTable*)sbt;
    Device* device = vkSbt->device;

    s_Vk.destroyBuffer(device->handle, vkSbt->buffer, &s_Vk.vkAllocator);
    s_Vk.freeMemory(device->handle, vkSbt->bufferMemory, &s_Vk.vkAllocator);
    palFree(s_Vk.allocator, vkSbt);
}

#endif // PAL_HAS_VULKAN
