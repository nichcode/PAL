
/**
    PAL - Prime Abstraction Layer
    Copyright (C) 2025
    Licensed under the Zlib license. See LICENSE file in root.
 */

#ifndef _PAL_VULKAN_H
#define _PAL_VULKAN_H

#if PAL_HAS_VULKAN_BACKEND
#include "pal/pal_graphics.h"
#include <vulkan/vulkan_core.h>

typedef struct _XDisplay Display;
typedef unsigned long Window;
typedef struct xcb_connection_t xcb_connection_t;
typedef uint32_t xcb_window_t;
typedef struct HINSTANCE__ *HINSTANCE;
typedef struct HWND__ *HWND;

typedef VkFlags VkWaylandSurfaceCreateFlagsKHR;
typedef VkFlags VkXlibSurfaceCreateFlagsKHR;
typedef VkFlags VkXcbSurfaceCreateFlagsKHR;
typedef VkFlags VkWin32SurfaceCreateFlagsKHR;

typedef struct VkWaylandSurfaceCreateInfoKHR {
    VkStructureType                   sType;
    const void*                       pNext;
    VkWaylandSurfaceCreateFlagsKHR    flags;
    struct wl_display*                display;
    struct wl_surface*                surface;
} VkWaylandSurfaceCreateInfoKHR;

typedef struct VkXlibSurfaceCreateInfoKHR {
    VkStructureType                sType;
    const void*                    pNext;
    VkXlibSurfaceCreateFlagsKHR    flags;
    Display*                       dpy;
    Window                         window;
} VkXlibSurfaceCreateInfoKHR;

typedef struct VkXcbSurfaceCreateInfoKHR {
    VkStructureType               sType;
    const void*                   pNext;
    VkXcbSurfaceCreateFlagsKHR    flags;
    xcb_connection_t*             connection;
    xcb_window_t                  window;
} VkXcbSurfaceCreateInfoKHR;

typedef struct VkWin32SurfaceCreateInfoKHR {
    VkStructureType                 sType;
    const void*                     pNext;
    VkWin32SurfaceCreateFlagsKHR    flags;
    HINSTANCE                       hinstance;
    HWND                            hwnd;
} VkWin32SurfaceCreateInfoKHR;

typedef VkResult (VKAPI_PTR *PFN_vkCreateWaylandSurfaceKHR)(
    VkInstance, 
    const VkWaylandSurfaceCreateInfoKHR*, 
    const VkAllocationCallbacks*, 
    VkSurfaceKHR*);

typedef VkResult (VKAPI_PTR *PFN_vkCreateXlibSurfaceKHR)(
    VkInstance, 
    const VkXlibSurfaceCreateInfoKHR*, 
    const VkAllocationCallbacks*, 
    VkSurfaceKHR*);

typedef VkResult (VKAPI_PTR *PFN_vkCreateXcbSurfaceKHR)(
    VkInstance, 
    const VkXcbSurfaceCreateInfoKHR*, 
    const VkAllocationCallbacks*, 
    VkSurfaceKHR*);

typedef VkResult (VKAPI_PTR *PFN_vkCreateWin32SurfaceKHR)(
    VkInstance, 
    const VkWin32SurfaceCreateInfoKHR*, 
    const VkAllocationCallbacks*, 
    VkSurfaceKHR*);

typedef struct {
    int32_t familyIndex;
    VkPhysicalDevice phyDevice;
    VkQueue handle;
    VkQueueFlags usages;
    VkQueueFlags usedUsages;
} PhysicalQueue;

typedef struct {
    uint32_t maxPayloadSize;
} DeviceLimits;

typedef struct {
    uint32_t patchControlPoints;
    VkShaderStageFlagBits stage;
    char entryName[PAL_SHADER_ENTRY_NAME_SIZE];
} ShaderEntry;

typedef struct {
    uint32_t raygenCount;
    uint32_t raygenDataSize;
    uint32_t missCount;
    uint32_t missDataSize;
    uint32_t hitCount;
    uint32_t hitDataSize;
    uint32_t callableCount;
    uint32_t callableDataSize;
} ShaderBindingTableInfo;

typedef struct {
    uint32_t startIndex;
    uint32_t offset;
    VkStridedDeviceAddressRegionKHR region;
} AddressRegion;

typedef struct {
    VkAccessFlags2 access;
    VkImageLayout layout;
} Barrier;

typedef struct {
    void* reserved;
    VkPhysicalDevice handle;
} AdapterVk;

typedef struct {
    void* reserved;
    PalAdapterFeatures features;
    int32_t phyQueueCount;
    int32_t phyQueueIndex;
    int32_t queueFamilyCount;
    uint32_t memoryClassMask[3];
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

    DeviceLimits limits;
} DeviceVk;

typedef struct {
    void* reserved;
    VkQueueFlags usage;
    DeviceVk* device;
    PhysicalQueue* phyQueue;
} QueueVk;

typedef struct {
    void* reserved;
    PalMemoryType type;
    VkDeviceMemory handle;
} MemoryVk;

typedef struct {
    void* reserved;
    DeviceVk* device;
    MemoryVk* memory;
    VkImage handle;
    PalImageInfo info;
} ImageVk;

typedef struct {
    void* reserved;
    uint32_t layerCount;
    DeviceVk* device;
    ImageVk* image;
    VkImageView handle;
} ImageViewVk;

typedef struct {
    void* reserved;
    DeviceVk* device;
    VkSurfaceKHR handle;
} SurfaceVk;

typedef struct {
    void* reserved;
    uint32_t imageCount;
    DeviceVk* device;
    QueueVk* queue;
    VkSwapchainKHR handle;
    ImageVk* images;
} SwapchainVk;

typedef struct {
    void* reserved;
    uint32_t entryCount;
    ShaderEntry* entries;
    DeviceVk* device;
    VkShaderModule handle;
} ShaderVk;

typedef struct {
    void* reserved;
    DeviceVk* device;
    VkCommandPool handle;
} CommandPoolVk;

typedef struct {
    void* reserved;
    PalBool primary;
    DeviceVk* device;
    CommandPoolVk* pool;
    void* pipeline;
    VkBuffer buffer;
    VkDeviceMemory bufferMemory;
    VkCommandBuffer handle;
} CommandBufferVk;

typedef struct {
    void* reserved;
    DeviceVk* device;
    VkFence handle;
} FenceVk;

typedef struct {
    void* reserved;
    PalBool isTimeline;
    DeviceVk* device;
    VkSemaphore handle;
} SemaphoreVk;

typedef struct {
    void* reserved;
    PalBool isMemoryManaged;
    PalBufferUsages usages;
    MemoryVk* memory;
    DeviceVk* device;
    VkBuffer handle;
} BufferVk;

typedef struct {
    void* reserved;
    VkDeviceAddress address;
    VkBuffer buffer;
    VkDeviceMemory bufferMemory;
    VkDeviceAddress bufferAddress;
    DeviceVk* device;
    VkAccelerationStructureKHR handle;
} AccelerationStructureVk;

typedef struct {
    void* reserved;
    PalDescriptorIndexingFlags flags;
    DeviceVk* device;
    VkDescriptorSetLayout handle;
} DescriptorSetLayoutVk;

typedef struct {
    void* reserved;
    PalDescriptorIndexingFlags flags;
    DeviceVk* device;
    VkDescriptorPool handle;
} DescriptorPoolVk;

typedef struct {
    void* reserved;
    DeviceVk* device;
    DescriptorPoolVk* pool;
    VkDescriptorSet handle;
} DescriptorSetVk;

typedef struct {
    void* reserved;
    DeviceVk* device;
    VkSampler handle;
} SamplerVk;

typedef struct {
    void* reserved;
    DeviceVk* device;
    VkPipelineLayout handle;
} PipelineLayoutVk;

typedef struct {
    void* reserved;
    VkPipelineBindPoint bindPoint;
    VkPipelineStageFlags2 stages;
    VkShaderStageFlags shaderStages;
    DeviceVk* device;
    VkPipeline handle;
    VkPipelineLayout layout;
    ShaderBindingTableInfo sbtInfo;
} PipelineVk;

typedef struct {
    void* reserved;
    PalBool isDirty;
    uint32_t stagingBufferSize;
    uint32_t handleSize;
    DeviceVk* device;
    VkBuffer buffer;
    VkBuffer stagingBuffer;
    VkDeviceMemory bufferMemory;
    VkDeviceMemory stagingBufferMemory;
    VkDeviceAddress baseAddress;
    PipelineVk* pipeline;
    AddressRegion raygen;
    AddressRegion miss;
    AddressRegion hit;
    AddressRegion callable;
} ShaderBindingTableVk;

typedef struct {
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

    void* handle;
    VkInstance instance;
    VkDebugUtilsMessengerEXT messenger;
    PalDebugCallback callback;
    const PalAllocator* allocator;
    AdapterVk* adapters;
    VkAllocationCallbacks vkAllocator;
} Vulkan;

extern Vulkan s_Vk;

PalResult makeResultVk(VkResult result);
VkFormat formatToVk(PalFormat format);
VkSampleCountFlags samplesToVk(PalSampleCount count);
VkExtent2D getShadingRateSizeVk(PalFragmentShadingRate rate);

VkFragmentShadingRateCombinerOpKHR combinerOpsToVk(PalFragmentShadingRateCombinerOp op);
VkImageAspectFlags imageAspectToVk(PalImageAspect aspect);
Barrier barrierToVk(PalUsageState state);
VkStencilOp stencilOpToVk(PalStencilOp op);
VkCompareOp compareOpToVk(PalCompareOp op);
VkRenderingFlags renderingFlagToVk(PalRenderingFlags flags);

uint32_t findBestMemoryIndexVk(
    VkPhysicalDevice phyDevice,
    uint32_t memoryMask);

void fillBuildInfoVk(
    uint32_t count,
    PalAccelerationStructureBuildInfo* info,
    uint32_t* maxPrimities,
    VkAccelerationStructureGeometryKHR* geometries,
    VkAccelerationStructureKHR srcAs,
    VkAccelerationStructureKHR dstAs,
    VkAccelerationStructureBuildRangeInfoKHR* rangeInfos,
    VkAccelerationStructureBuildGeometryInfoKHR* buildInfo);

#endif // PAL_HAS_VULKAN_BACKEND
#endif // _PAL_VULKAN_H