
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

// ==================================================
// Typedefs, enums and structs
// ==================================================

#define MAX_BACKENDS 18 // 16 for users
#define PAL_HANDLE(name)                                                                           \
    struct name {                                                                                  \
        const PalGraphicsBackend* backend;                                                         \
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
    Int32 count;
    Int32 startIndex;
    const PalGraphicsBackend* base;
} BackendData;

typedef struct {
    bool initialized;
    Int32 backendCount;
    const PalAllocator* allocator;
    BackendData backends[MAX_BACKENDS];
} GraphicsLinux;

static GraphicsLinux s_Graphics = {0};

// ==================================================
// Internal API
// ==================================================

static inline Uint32 _ceil(
    Uint32 a,
    Uint32 b)
{
    return (a + b - 1) / b;
}

static inline Uint32 _min(
    Uint32 a,
    Uint32 b)
{
    return (a < b) ? a : b;
}

// ==================================================
// Vulkan API
// ==================================================

#if PAL_HAS_VULKAN

// ==================================================
// Adapter
// ==================================================

PalResult PAL_CALL initGraphicsVk(
    const PalGraphicsDebugger* debugger,
    const PalAllocator* allocator);

void PAL_CALL shutdownGraphicsVk();

PalResult PAL_CALL enumerateAdaptersVk(
    Int32* count,
    PalAdapter** outAdapters);

PalResult PAL_CALL getAdapterInfoVk(
    PalAdapter* adapter,
    PalAdapterInfo* info);

PalResult PAL_CALL getAdapterCapabilitiesVk(
    PalAdapter* adapter,
    PalAdapterCapabilities* caps);

PalAdapterFeatures PAL_CALL getAdapterFeaturesVk(PalAdapter* adapter);

// ==================================================
// Device
// ==================================================

PalResult PAL_CALL createDeviceVk(
    PalAdapter* adapter,
    PalAdapterFeatures features,
    PalDevice** outDevice);

void PAL_CALL destroyDeviceVk(PalDevice* device);

PalResult PAL_CALL waitDeviceVk(PalDevice* device);

// ==================================================
// Memory
// ==================================================

PalResult PAL_CALL allocateMemoryVk(
    PalDevice* device,
    PalMemoryType type,
    Uint64 memoryMask,
    Uint64 size,
    PalMemory** outMemory);

void PAL_CALL freeMemoryVk(
    PalDevice* device,
    PalMemory* memory);

// ==================================================
// Extended Adapter Features
// ==================================================

PalResult PAL_CALL querySamplerAnisotropyCapabilitiesVk(
    PalDevice* device,
    PalSamplerAnisotropyCapabilities* caps);

PalResult PAL_CALL queryDepthStencilCapabilitiesVk(
    PalDevice* device,
    PalDepthStencilCapabilities* caps);

PalResult PAL_CALL queryFragmentShadingRateCapabilitiesVk(
    PalDevice* device,
    PalFragmentShadingRateCapabilities* caps);

PalResult PAL_CALL queryMeshShaderCapabilitiesVk(
    PalDevice* device,
    PalMeshShaderCapabilities* caps);

PalResult PAL_CALL queryRayTracingCapabilitiesVk(
    PalDevice* device,
    PalRayTracingCapabilities* caps);

PalResult PAL_CALL queryDescriptorIndexingCapabilitiesVk(
    PalDevice* device,
    PalDescriptorIndexingCapabilities* caps);

// ==================================================
// Queue
// ==================================================

PalResult PAL_CALL createQueueVk(
    PalDevice* device,
    PalQueueType type,
    PalQueue** outQueue);

void PAL_CALL destroyQueueVk(PalQueue* queue);

PalResult PAL_CALL waitQueueVk(PalQueue* queue);

bool PAL_CALL canQueuePresentVk(
    PalQueue* queue,
    PalSurface* surface);

// ==================================================
// Formats
// ==================================================

PalResult PAL_CALL enumerateFormatsVk(
    PalAdapter* adapter,
    Int32* count,
    PalFormatInfo* outFormats);

bool PAL_CALL isFormatSupportedVk(
    PalAdapter* adapter,
    PalFormat format);

PalImageUsages PAL_CALL queryFormatImageUsagesVk(
    PalAdapter* adapter,
    PalFormat format);

PalImageViewUsages PAL_CALL queryFormatImageViewUsagesVk(
    PalAdapter* adapter,
    PalFormat format);

PalSampleCount PAL_CALL queryFormatSampleCountVk(
    PalAdapter* adapter,
    PalFormat format);

// ==================================================
// Image
// ==================================================

PalResult PAL_CALL createImageVk(
    PalDevice* device,
    const PalImageCreateInfo* info,
    PalImage** outImage);

void PAL_CALL destroyImageVk(PalImage* image);

PalResult PAL_CALL getImageInfoVk(
    PalImage* image,
    PalImageInfo* info);

PalResult PAL_CALL getImageMemoryRequirementsVk(
    PalImage* image,
    PalMemoryRequirements* requirements);

PalResult PAL_CALL bindImageMemoryVk(
    PalImage* image,
    PalMemory* memory,
    Uint64 offset);

PalResult PAL_CALL mapImageMemoryVk(
    PalImage* image,
    Uint64 offset,
    Uint64 size,
    void** outPtr);

void PAL_CALL unmapImageMemoryVk(PalImage* image);

// ==================================================
// Image View
// ==================================================

PalResult PAL_CALL createImageViewVk(
    PalDevice* device,
    PalImage* image,
    const PalImageViewCreateInfo* info,
    PalImageView** outImageView);

void PAL_CALL destroyImageViewVk(PalImageView* imageView);

// ==================================================
// Sampler
// ==================================================

PalResult PAL_CALL createSamplerVk(
    PalDevice* device,
    const PalSamplerCreateInfo* info,
    PalSampler** outSampler);

void PAL_CALL destroySamplerVk(PalSampler* sampler);

// ==================================================
// Surface
// ==================================================

PalResult PAL_CALL createSurfaceVk(
    PalDevice* device,
    PalGraphicsWindow* window,
    PalSurface** outSurface);

void PAL_CALL destroySurfaceVk(PalSurface* surface);

PalResult PAL_CALL getSurfaceCapabilitiesVk(
    PalDevice* device,
    PalSurface* surface,
    PalSurfaceCapabilities* caps);

// ==================================================
// Swapchain
// ==================================================

PalResult PAL_CALL createSwapchainVk(
    PalDevice* device,
    PalQueue* queue,
    PalSurface* surface,
    const PalSwapchainCreateInfo* info,
    PalSwapchain** outSwapchain);

void PAL_CALL destroySwapchainVk(PalSwapchain* swapchain);

PalImage* PAL_CALL getSwapchainImageVk(
    PalSwapchain* swapchain,
    Int32 index);

PalResult PAL_CALL getNextSwapchainImageVk(
    PalSwapchain* swapchain,
    PalSwapchainNextImageInfo* info,
    Uint32* outIndex);

PalResult PAL_CALL presentSwapchainVk(
    PalSwapchain* swapchain,
    PalSwapchainPresentInfo* info);

PalResult PAL_CALL resizeSwapchainVk(
    PalSwapchain* swapchain,
    Uint32 newWidth,
    Uint32 newHeight);

// ==================================================
// Shader
// ==================================================

PalResult PAL_CALL createShaderVk(
    PalDevice* device,
    const PalShaderCreateInfo* info,
    PalShader** outShader);

void PAL_CALL destroyShaderVk(PalShader* shader);

// ==================================================
// Fence
// ==================================================

PalResult PAL_CALL createFenceVk(
    PalDevice* device,
    bool signaled,
    PalFence** outFence);

void PAL_CALL destroyFenceVk(PalFence* fence);

PalResult PAL_CALL waitFenceVk(
    PalFence* fence,
    Uint64 timeout);

PalResult PAL_CALL resetFenceVk(PalFence* fence);

bool PAL_CALL isFenceSignaledVk(PalFence* fence);

// ==================================================
// Semaphore
// ==================================================

PalResult PAL_CALL createSemaphoreVk(
    PalDevice* device,
    PalSemaphore** outSemaphore);

void PAL_CALL destroySemaphoreVk(PalSemaphore* semaphore);

PalResult PAL_CALL waitSemaphoreVk(
    PalSemaphore* semaphore,
    Uint64 value,
    Uint64 timeout);

PalResult PAL_CALL signalSemaphoreVk(
    PalSemaphore* semaphore,
    PalQueue* queue,
    Uint64 value);

PalResult PAL_CALL getSemaphoreValueVk(
    PalSemaphore* semaphore,
    Uint64* outValue);

// ==================================================
// Command Pool And Buffer
// ==================================================

PalResult PAL_CALL createCommandPoolVk(
    PalDevice* device,
    PalQueue* queue,
    PalCommandPool** outPool);

void PAL_CALL destroyCommandPoolVk(PalCommandPool* pool);

PalResult PAL_CALL resetCommandPoolVk(PalCommandPool* pool);

PalResult PAL_CALL allocateCommandBufferVk(
    PalDevice* device,
    PalCommandPool* pool,
    PalCommandBufferType type,
    PalCommandBuffer** outBuffer);

void PAL_CALL freeCommandBufferVk(PalCommandBuffer* buffer);

PalResult PAL_CALL resetCommandBufferVk(PalCommandBuffer* cmdBuffer);

PalResult PAL_CALL submitCommandBufferVk(
    PalQueue* queue,
    PalCommandBufferSubmitInfo* info);

// ==================================================
// Command Recording
// ==================================================

PalResult PAL_CALL cmdBeginVk(
    PalCommandBuffer* cmdBuffer,
    PalRenderingLayoutInfo* info);

PalResult PAL_CALL cmdEndVk(PalCommandBuffer* cmdBuffer);

PalResult PAL_CALL cmdExecuteCommandBufferVk(
    PalCommandBuffer* primaryCmdBuffer,
    PalCommandBuffer* secondaryCmdBuffer);

PalResult PAL_CALL cmdSetFragmentShadingRateVk(
    PalCommandBuffer* cmdBuffer,
    PalFragmentShadingRateState* state);

PalResult PAL_CALL cmdDrawMeshTasksVk(
    PalCommandBuffer* cmdBuffer,
    Uint32 groupCountX,
    Uint32 groupCountY,
    Uint32 groupCountZ);

PalResult PAL_CALL cmdDrawMeshTasksIndirectVk(
    PalCommandBuffer* cmdBuffer,
    PalBuffer* buffer,
    Uint64 offset,
    Uint32 drawCount,
    Uint32 stride);

PalResult PAL_CALL cmdDrawMeshTasksIndirectCountVk(
    PalCommandBuffer* cmdBuffer,
    PalBuffer* buffer,
    PalBuffer* countBuffer,
    Uint64 offset,
    Uint64 countBufferOffset,
    Uint32 maxDrawCount,
    Uint32 stride);

PalResult PAL_CALL cmdBuildAccelerationStructureVk(
    PalCommandBuffer* cmdBuffer,
    PalAccelerationStructureBuildInfo* info);

PalResult PAL_CALL cmdBeginRenderingVk(
    PalCommandBuffer* cmdBuffer,
    PalRenderingInfo* info);

PalResult PAL_CALL cmdEndRenderingVk(PalCommandBuffer* cmdBuffer);

PalResult PAL_CALL cmdCopyBufferVk(
    PalCommandBuffer* cmdBuffer,
    PalBuffer* dst,
    PalBuffer* src,
    PalBufferCopyInfo* copyInfo);

PalResult PAL_CALL cmdCopyBufferToImageVk(
    PalCommandBuffer* cmdBuffer,
    PalImage* dstImage,
    PalBuffer* srcBuffer,
    PalBufferImageCopyInfo* copyInfo);

PalResult PAL_CALL cmdCopyImageVk(
    PalCommandBuffer* cmdBuffer,
    PalImage* dst,
    PalImage* src,
    PalImageCopyInfo* copyInfo);

PalResult PAL_CALL cmdCopyImageToBufferVk(
    PalCommandBuffer* cmdBuffer,
    PalBuffer* dstBuffer,
    PalImage* srcImage,
    PalBufferImageCopyInfo* copyInfo);

PalResult PAL_CALL cmdBindPipelineVk(
    PalCommandBuffer* cmdBuffer,
    PalPipelineBindPoint bindPoint,
    PalPipeline* pipeline);

PalResult PAL_CALL cmdSetViewportVk(
    PalCommandBuffer* cmdBuffer,
    Uint32 count,
    PalViewport* viewports);

PalResult PAL_CALL cmdSetScissorsVk(
    PalCommandBuffer* cmdBuffer,
    Uint32 count,
    PalRect2D* scissors);

PalResult PAL_CALL cmdBindVertexBuffersVk(
    PalCommandBuffer* cmdBuffer,
    Uint32 firstSlot,
    Uint32 count,
    PalBuffer** buffers,
    Uint64* offsets);

PalResult PAL_CALL cmdBindIndexBufferVk(
    PalCommandBuffer* cmdBuffer,
    PalBuffer* buffer,
    Uint64 offset,
    PalIndexType type);

PalResult PAL_CALL cmdDrawVk(
    PalCommandBuffer* cmdBuffer,
    Uint32 vertexCount,
    Uint32 instanceCount,
    Uint32 firstVertex,
    Uint32 firstInstance);

PalResult PAL_CALL cmdDrawIndirectVk(
    PalCommandBuffer* cmdBuffer,
    PalBuffer* buffer,
    Uint64 offset,
    Uint32 count,
    Uint32 stride);

PalResult PAL_CALL cmdDrawIndirectCountVk(
    PalCommandBuffer* cmdBuffer,
    PalBuffer* buffer,
    PalBuffer* countBuffer,
    Uint64 offset,
    Uint64 countBufferOffset,
    Uint32 maxDrawCount,
    Uint32 stride);

PalResult PAL_CALL cmdDrawIndexedVk(
    PalCommandBuffer* cmdBuffer,
    Uint32 indexCount,
    Uint32 instanceCount,
    Uint32 firstIndex,
    Int32 vertexOffset,
    Uint32 firstInstance);

PalResult PAL_CALL cmdDrawIndexedIndirectVk(
    PalCommandBuffer* cmdBuffer,
    PalBuffer* buffer,
    Uint64 offset,
    Uint32 count,
    Uint32 stride);

PalResult PAL_CALL cmdDrawIndexedIndirectCountVk(
    PalCommandBuffer* cmdBuffer,
    PalBuffer* buffer,
    PalBuffer* countBuffer,
    Uint64 offset,
    Uint64 countBufferOffset,
    Uint32 maxDrawCount,
    Uint32 stride);

PalResult PAL_CALL cmdMemoryBarrierVk(
    PalCommandBuffer* cmdBuffer,
    PalUsageStateInfo* oldsUsageStateInfo,
    PalUsageStateInfo* newUsageStateInfo);

PalResult PAL_CALL cmdImageBarrierVk(
    PalCommandBuffer* cmdBuffer,
    PalImage* image,
    PalImageSubresourceRange* subresourceRange,
    PalUsageStateInfo* oldUsageStateInfo,
    PalUsageStateInfo* newUsageStateInfo);

PalResult PAL_CALL cmdBufferBarrierVk(
    PalCommandBuffer* cmdBuffer,
    PalBuffer* buffer,
    PalUsageStateInfo* oldUsageStateInfo,
    PalUsageStateInfo* newUsageStateInfo);

PalResult PAL_CALL cmdDispatchVk(
    PalCommandBuffer* cmdBuffer,
    Uint32 groupCountX,
    Uint32 groupCountY,
    Uint32 groupCountZ);

PalResult PAL_CALL cmdDispatchBaseVk(
    PalCommandBuffer* cmdBuffer,
    Uint32 baseGroupX,
    Uint32 baseGroupY,
    Uint32 baseGroupZ,
    Uint32 groupCountX,
    Uint32 groupCountY,
    Uint32 groupCountZ);

PalResult PAL_CALL cmdDispatchIndirectVk(
    PalCommandBuffer* cmdBuffer,
    PalBuffer* buffer,
    Uint64 offset);

PalResult PAL_CALL cmdTraceRaysVk(
    PalCommandBuffer* cmdBuffer,
    PalShaderBindingTable* sbt,
    Uint32 raygenIndex,
    Uint32 width,
    Uint32 height,
    Uint32 depth);

PalResult PAL_CALL cmdTraceRaysIndirectVk(
    PalCommandBuffer* cmdBuffer,
    Uint32 raygenIndex,
    PalShaderBindingTable* sbt,
    PalDeviceAddress bufferAddress);

PalResult PAL_CALL cmdBindDescriptorSetVk(
    PalCommandBuffer* cmdBuffer,
    PalPipelineBindPoint bindPoint,
    PalPipelineLayout* layout,
    Uint32 setIndex,
    PalDescriptorSet* set);

PalResult PAL_CALL cmdPushConstantsVk(
    PalCommandBuffer* cmdBuffer,
    PalPipelineLayout* layout,
    Uint32 shaderStageCount,
    PalShaderStage* shaderStages,
    Uint32 offset,
    Uint32 size,
    const void* value);

PalResult PAL_CALL cmdSetCullModeVk(
    PalCommandBuffer* cmdBuffer,
    PalCullMode cullMode);

PalResult PAL_CALL cmdSetFrontFaceVk(
    PalCommandBuffer* cmdBuffer,
    PalFrontFace frontFace);

PalResult PAL_CALL cmdSetPrimitiveTopologyVk(
    PalCommandBuffer* cmdBuffer,
    PalPrimitiveTopology topology);

PalResult PAL_CALL cmdSetDepthTestEnableVk(
    PalCommandBuffer* cmdBuffer,
    bool enable);

PalResult PAL_CALL cmdSetDepthWriteEnableVk(
    PalCommandBuffer* cmdBuffer,
    bool enable);

PalResult PAL_CALL cmdSetStencilOpVk(
    PalCommandBuffer* cmdBuffer,
    PalStencilFaceFlags faceMask,
    PalStencilOp failOp,
    PalStencilOp passOp,
    PalStencilOp depthFailOp,
    PalCompareOp compareOp);

// ==================================================
// Acceleration Structure
// ==================================================

PalResult PAL_CALL createAccelerationstructureVk(
    PalDevice* device,
    const PalAccelerationStructureCreateInfo* info,
    PalAccelerationStructure** outAs);

void PAL_CALL destroyAccelerationstructureVk(PalAccelerationStructure* as);

PalResult PAL_CALL getAccelerationStructureBuildSizeVk(
    PalDevice* device,
    PalAccelerationStructureBuildInfo* info,
    PalAccelerationStructureBuildSize* size);

// ==================================================
// Buffer
// ==================================================

PalResult PAL_CALL createBufferVk(
    PalDevice* device,
    const PalBufferCreateInfo* info,
    PalBuffer** outBuffer);

void PAL_CALL destroyBufferVk(PalBuffer* buffer);

PalResult PAL_CALL getBufferMemoryRequirementsVk(
    PalBuffer* buffer,
    PalMemoryRequirements* requirements);

PalResult PAL_CALL computeInstanceBufferRequirementsVk(
    PalDevice* device,
    PalInstanceBufferRequirements* requirements,
    Uint32 instanceCount);

PalResult PAL_CALL writeInstancesToMappedMemoryVk(
    PalDevice* device,
    void* ptr,
    PalAccelerationStructureInstance* instances,
    Uint32 instanceCount);

PalResult PAL_CALL bindBufferMemoryVk(
    PalBuffer* buffer,
    PalMemory* memory,
    Uint64 offset);

PalResult PAL_CALL mapBufferMemoryVk(
    PalBuffer* buffer,
    Uint64 offset,
    Uint64 size,
    void** outPtr);

void PAL_CALL unmapBufferMemoryVk(PalBuffer* buffer);

PalDeviceAddress PAL_CALL getBufferDeviceAddressVk(PalBuffer* buffer);

// ==================================================
// Descriptor Pool, Set and Layout
// ==================================================

PalResult PAL_CALL createDescriptorSetLayoutVk(
    PalDevice* device,
    const PalDescriptorSetLayoutCreateInfo* info,
    PalDescriptorSetLayout** outLayout);

void PAL_CALL destroyDescriptorSetLayoutVk(PalDescriptorSetLayout* layout);

PalResult PAL_CALL createDescriptorPoolVk(
    PalDevice* device,
    const PalDescriptorPoolCreateInfo* info,
    PalDescriptorPool** outPool);

void PAL_CALL destroyDescriptorPoolVk(PalDescriptorPool* pool);

PalResult PAL_CALL resetDescriptorPoolVk(PalDescriptorPool* pool);

PalResult PAL_CALL allocateDescriptorSetVk(
    PalDevice* device,
    PalDescriptorPool* pool,
    PalDescriptorSetLayout* layout,
    PalDescriptorSet** outSet);

void PAL_CALL freeDescriptorSetVk(PalDescriptorSet* set);

PalResult PAL_CALL updateDescriptorSetVk(
    PalDevice* device,
    Uint32 count,
    PalDescriptorSetWriteInfo* infos);

// ==================================================
// Pipeline Layout
// ==================================================

PalResult PAL_CALL createPipelineLayoutVk(
    PalDevice* device,
    const PalPipelineLayoutCreateInfo* info,
    PalPipelineLayout** outLayout);

void PAL_CALL destroyPipelineLayoutVk(PalPipelineLayout* layout);

// ==================================================
// Pipeline
// ==================================================

PalResult PAL_CALL createGraphicsPipelineVk(
    PalDevice* device,
    const PalGraphicsPipelineCreateInfo* info,
    PalPipeline** outPipeline);

PalResult PAL_CALL createComputePipelineVk(
    PalDevice* device,
    const PalComputePipelineCreateInfo* info,
    PalPipeline** outPipeline);

PalResult PAL_CALL createRayTracingPipelineVk(
    PalDevice* device,
    const PalRayTracingPipelineCreateInfo* info,
    PalPipeline** outPipeline);

void PAL_CALL destroyPipelineVk(PalPipeline* pipeline);

// ==================================================
// Shader Binding Table
// ==================================================

PalResult PAL_CALL createShaderBindingTableVk(
    PalDevice* device,
    const PalShaderBindingTableCreateInfo* info,
    PalShaderBindingTable** outSbt);

void PAL_CALL destroyShaderBindingTableVk(PalShaderBindingTable* sbt);

static PalGraphicsBackend s_VkBackend = {
    // adapter
    .enumerateAdapters = enumerateAdaptersVk,
    .getAdapterInfo = getAdapterInfoVk,
    .getAdapterCapabilities = getAdapterCapabilitiesVk,
    .getAdapterFeatures = getAdapterFeaturesVk,

    // device
    .createDevice = createDeviceVk,
    .destroyDevice = destroyDeviceVk,

    // memory
    .allocateMemory = allocateMemoryVk,
    .freeMemory = freeMemoryVk,

    // extended adapter features
    .querySamplerAnisotropyCapabilities = querySamplerAnisotropyCapabilitiesVk,
    .queryDepthStencilCapabilities = queryDepthStencilCapabilitiesVk,
    .queryFragmentShadingRateCapabilities = queryFragmentShadingRateCapabilitiesVk,
    .queryMeshShaderCapabilities = queryMeshShaderCapabilitiesVk,
    .queryRayTracingCapabilities = queryRayTracingCapabilitiesVk,
    .queryDescriptorIndexingCapabilities = queryDescriptorIndexingCapabilitiesVk,

    // queue
    .createQueue = createQueueVk,
    .destroyQueue = destroyQueueVk,
    .waitQueue = waitQueueVk,
    .canQueuePresent = canQueuePresentVk,

    // format
    .enumerateFormats = enumerateFormatsVk,
    .isFormatSupported = isFormatSupportedVk,
    .queryFormatImageUsages = queryFormatImageUsagesVk,
    .queryFormatImageViewUsages = queryFormatImageViewUsagesVk,
    .queryFormatSampleCount = queryFormatSampleCountVk,

    // image
    .createImage = createImageVk,
    .destroyImage = destroyImageVk,
    .getImageInfo = getImageInfoVk,
    .getImageMemoryRequirements = getImageMemoryRequirementsVk,
    .bindImageMemory = bindImageMemoryVk,
    .mapImageMemory = mapImageMemoryVk,
    .unmapImageMemory = unmapImageMemoryVk,

    // image view
    .createImageView = createImageViewVk,
    .destroyImageView = destroyImageViewVk,

    // sampler
    .createSampler = createSamplerVk,
    .destroySampler = destroySamplerVk,

    // surface
    .createSurface = createSurfaceVk,
    .destroySurface = destroySurfaceVk,
    .getSurfaceCapabilities = getSurfaceCapabilitiesVk,

    // swapchain
    .createSwapchain = createSwapchainVk,
    .destroySwapchain = destroySwapchainVk,
    .getSwapchainImage = getSwapchainImageVk,
    .getNextSwapchainImage = getNextSwapchainImageVk,
    .presentSwapchain = presentSwapchainVk,
    .resizeSwapchain = resizeSwapchainVk,

    // shader
    .createShader = createShaderVk,
    .destroyShader = destroyShaderVk,

    // fence
    .createFence = createFenceVk,
    .destroyFence = destroyFenceVk,
    .waitFence = waitFenceVk,
    .resetFence = resetFenceVk,
    .isFenceSignaled = isFenceSignaledVk,

    // semaphore
    .createSemaphore = createSemaphoreVk,
    .destroySemaphore = destroySemaphoreVk,
    .waitSemaphore = waitSemaphoreVk,
    .signalSemaphore = signalSemaphoreVk,
    .getSemaphoreValue = getSemaphoreValueVk,

    // command pool and command buffer
    .createCommandPool = createCommandPoolVk,
    .destroyCommandPool = destroyCommandPoolVk,
    .resetCommandPool = resetCommandPoolVk,
    .allocateCommandBuffer = allocateCommandBufferVk,
    .freeCommandBuffer = freeCommandBufferVk,
    .resetCommandBuffer = resetCommandBufferVk,
    .submitCommandBuffer = submitCommandBufferVk,

    // command recording
    .cmdBegin = cmdBeginVk,
    .cmdEnd = cmdEndVk,
    .cmdExecuteCommandBuffer = cmdExecuteCommandBufferVk,
    .cmdSetFragmentShadingRate = cmdSetFragmentShadingRateVk,
    .cmdDrawMeshTasks = cmdDrawMeshTasksVk,
    .cmdDrawMeshTasksIndirect = cmdDrawMeshTasksIndirectVk,
    .cmdDrawMeshTasksIndirectCount = cmdDrawMeshTasksIndirectCountVk,
    .cmdBuildAccelerationStructure = cmdBuildAccelerationStructureVk,
    .cmdBeginRendering = cmdBeginRenderingVk,
    .cmdEndRendering = cmdEndRenderingVk,
    .cmdCopyBuffer = cmdCopyBufferVk,
    .cmdCopyBufferToImage = cmdCopyBufferToImageVk,
    .cmdCopyImage = cmdCopyImageVk,
    .cmdCopyImageToBuffer = cmdCopyImageToBufferVk,
    .cmdBindPipeline = cmdBindPipelineVk,
    .cmdSetViewport = cmdSetViewportVk,
    .cmdSetScissors = cmdSetScissorsVk,
    .cmdBindVertexBuffers = cmdBindVertexBuffersVk,
    .cmdBindIndexBuffer = cmdBindIndexBufferVk,
    .cmdDraw = cmdDrawVk,
    .cmdDrawIndirect = cmdDrawIndirectVk,
    .cmdDrawIndirectCount = cmdDrawIndirectCountVk,
    .cmdDrawIndexed = cmdDrawIndexedVk,
    .cmdDrawIndexedIndirect = cmdDrawIndexedIndirectVk,
    .cmdDrawIndexedIndirectCount = cmdDrawIndexedIndirectCountVk,
    .cmdMemoryBarrier = cmdMemoryBarrierVk,
    .cmdImageBarrier = cmdImageBarrierVk,
    .cmdBufferBarrier = cmdBufferBarrierVk,
    .cmdDispatch = cmdDispatchVk,
    .cmdDispatchBase = cmdDispatchBaseVk,
    .cmdDispatchIndirect = cmdDispatchIndirectVk,
    .cmdTraceRays = cmdTraceRaysVk,
    .cmdTraceRaysIndirect = cmdTraceRaysIndirectVk,
    .cmdBindDescriptorSet = cmdBindDescriptorSetVk,
    .cmdPushConstants = cmdPushConstantsVk,
    .cmdSetCullMode = cmdSetCullModeVk,
    .cmdSetFrontFace = cmdSetFrontFaceVk,
    .cmdSetPrimitiveTopology = cmdSetPrimitiveTopologyVk,
    .cmdSetDepthTestEnable = cmdSetDepthTestEnableVk,
    .cmdSetDepthWriteEnable = cmdSetDepthWriteEnableVk,
    .cmdSetStencilOp = cmdSetStencilOpVk,

    // acceleration structure
    .createAccelerationstructure = createAccelerationstructureVk,
    .destroyAccelerationstructure = destroyAccelerationstructureVk,
    .getAccelerationStructureBuildSize = getAccelerationStructureBuildSizeVk,

    // buffer
    .createBuffer = createBufferVk,
    .destroyBuffer = destroyBufferVk,
    .getBufferMemoryRequirements = getBufferMemoryRequirementsVk,
    .computeInstanceBufferRequirements = computeInstanceBufferRequirementsVk,
    .writeInstancesToMappedMemory = writeInstancesToMappedMemoryVk,
    .bindBufferMemory = bindBufferMemoryVk,
    .getBufferDeviceAddress = getBufferDeviceAddressVk,
    .mapBufferMemory = mapBufferMemoryVk,
    .unmapBufferMemory = unmapBufferMemoryVk,

    // descriptor set layout, descriptor pool and descriptor set
    .createDescriptorSetLayout = createDescriptorSetLayoutVk,
    .destroyDescriptorSetLayout = destroyDescriptorSetLayoutVk,
    .createDescriptorPool = createDescriptorPoolVk,
    .destroyDescriptorPool = destroyDescriptorPoolVk,
    .resetDescriptorPool = resetDescriptorPoolVk,
    .allocateDescriptorSet = allocateDescriptorSetVk,
    .updateDescriptorSet = updateDescriptorSetVk,

    // pipeline layout
    .createPipelineLayout = createPipelineLayoutVk,
    .destroyPipelineLayout = destroyPipelineLayoutVk,

    // pipeline
    .createGraphicsPipeline = createGraphicsPipelineVk,
    .createComputePipeline = createComputePipelineVk,
    .createRayTracingPipeline = createRayTracingPipelineVk,
    .destroyPipeline = destroyPipelineVk,

    // shader binding table
    .createShaderBindingTable = createShaderBindingTableVk,
    .destroyShaderBindingTable = destroyShaderBindingTableVk};

#endif // PAL_HAS_VULKAN

// ==================================================
// D3D12 API
// ==================================================

#if PAL_HAS_D3D12

// ==================================================
// Adapter
// ==================================================

PalResult PAL_CALL initGraphicsD3D12(
    const PalGraphicsDebugger* debugger,
    const PalAllocator* allocator);

void PAL_CALL shutdownGraphicsD3D12();

PalResult PAL_CALL enumerateAdaptersD3D12(
    Int32* count,
    PalAdapter** outAdapters);

PalResult PAL_CALL getAdapterInfoD3D12(
    PalAdapter* adapter,
    PalAdapterInfo* info);

PalResult PAL_CALL getAdapterCapabilitiesD3D12(
    PalAdapter* adapter,
    PalAdapterCapabilities* caps);

PalAdapterFeatures PAL_CALL getAdapterFeaturesD3D12(PalAdapter* adapter);

// ==================================================
// Device
// ==================================================

PalResult PAL_CALL createDeviceD3D12(
    PalAdapter* adapter,
    PalAdapterFeatures features,
    PalDevice** outDevice);

void PAL_CALL destroyDeviceD3D12(PalDevice* device);

PalResult PAL_CALL waitDeviceD3D12(PalDevice* device);

// ==================================================
// Memory
// ==================================================

PalResult PAL_CALL allocateMemoryD3D12(
    PalDevice* device,
    PalMemoryType type,
    Uint64 memoryMask,
    Uint64 size,
    PalMemory** outMemory);

void PAL_CALL freeMemoryD3D12(
    PalDevice* device,
    PalMemory* memory);

// ==================================================
// Extended Adapter Features
// ==================================================

PalResult PAL_CALL querySamplerAnisotropyCapabilitiesD3D12(
    PalDevice* device,
    PalSamplerAnisotropyCapabilities* caps);

PalResult PAL_CALL queryDepthStencilCapabilitiesD3D12(
    PalDevice* device,
    PalDepthStencilCapabilities* caps);

PalResult PAL_CALL queryFragmentShadingRateCapabilitiesD3D12(
    PalDevice* device,
    PalFragmentShadingRateCapabilities* caps);

PalResult PAL_CALL queryMeshShaderCapabilitiesD3D12(
    PalDevice* device,
    PalMeshShaderCapabilities* caps);

PalResult PAL_CALL queryRayTracingCapabilitiesD3D12(
    PalDevice* device,
    PalRayTracingCapabilities* caps);

PalResult PAL_CALL queryDescriptorIndexingCapabilitiesD3D12(
    PalDevice* device,
    PalDescriptorIndexingCapabilities* caps);

// ==================================================
// Queue
// ==================================================

PalResult PAL_CALL createQueueD3D12(
    PalDevice* device,
    PalQueueType type,
    PalQueue** outQueue);

void PAL_CALL destroyQueueD3D12(PalQueue* queue);

PalResult PAL_CALL waitQueueD3D12(PalQueue* queue);

bool PAL_CALL canQueuePresentD3D12(
    PalQueue* queue,
    PalSurface* surface);

// ==================================================
// Formats
// ==================================================

PalResult PAL_CALL enumerateFormatsD3D12(
    PalAdapter* adapter,
    Int32* count,
    PalFormatInfo* outFormats);

bool PAL_CALL isFormatSupportedD3D12(
    PalAdapter* adapter,
    PalFormat format);

PalImageUsages PAL_CALL queryFormatImageUsagesD3D12(
    PalAdapter* adapter,
    PalFormat format);

PalImageViewUsages PAL_CALL queryFormatImageViewUsagesD3D12(
    PalAdapter* adapter,
    PalFormat format);

PalSampleCount PAL_CALL queryFormatSampleCountD3D12(
    PalAdapter* adapter,
    PalFormat format);

// ==================================================
// Image
// ==================================================

PalResult PAL_CALL createImageD3D12(
    PalDevice* device,
    const PalImageCreateInfo* info,
    PalImage** outImage);

void PAL_CALL destroyImageD3D12(PalImage* image);

PalResult PAL_CALL getImageInfoD3D12(
    PalImage* image,
    PalImageInfo* info);

PalResult PAL_CALL getImageMemoryRequirementsD3D12(
    PalImage* image,
    PalMemoryRequirements* requirements);

PalResult PAL_CALL bindImageMemoryD3D12(
    PalImage* image,
    PalMemory* memory,
    Uint64 offset);

PalResult PAL_CALL mapImageMemoryD3D12(
    PalImage* image,
    Uint64 offset,
    Uint64 size,
    void** outPtr);

void PAL_CALL unmapImageMemoryD3D12(PalImage* image);

// ==================================================
// Image View
// ==================================================

PalResult PAL_CALL createImageViewD3D12(
    PalDevice* device,
    PalImage* image,
    const PalImageViewCreateInfo* info,
    PalImageView** outImageView);

void PAL_CALL destroyImageViewD3D12(PalImageView* imageView);

// ==================================================
// Sampler
// ==================================================

PalResult PAL_CALL createSamplerD3D12(
    PalDevice* device,
    const PalSamplerCreateInfo* info,
    PalSampler** outSampler);

void PAL_CALL destroySamplerD3D12(PalSampler* sampler);

// ==================================================
// Surface
// ==================================================

PalResult PAL_CALL createSurfaceD3D12(
    PalDevice* device,
    PalGraphicsWindow* window,
    PalSurface** outSurface);

void PAL_CALL destroySurfaceD3D12(PalSurface* surface);

PalResult PAL_CALL getSurfaceCapabilitiesD3D12(
    PalDevice* device,
    PalSurface* surface,
    PalSurfaceCapabilities* caps);

// ==================================================
// Swapchain
// ==================================================

PalResult PAL_CALL createSwapchainD3D12(
    PalDevice* device,
    PalQueue* queue,
    PalSurface* surface,
    const PalSwapchainCreateInfo* info,
    PalSwapchain** outSwapchain);

void PAL_CALL destroySwapchainD3D12(PalSwapchain* swapchain);

PalImage* PAL_CALL getSwapchainImageD3D12(
    PalSwapchain* swapchain,
    Int32 index);

PalResult PAL_CALL getNextSwapchainImageD3D12(
    PalSwapchain* swapchain,
    PalSwapchainNextImageInfo* info,
    Uint32* outIndex);

PalResult PAL_CALL presentSwapchainD3D12(
    PalSwapchain* swapchain,
    PalSwapchainPresentInfo* info);

PalResult PAL_CALL resizeSwapchainD3D12(
    PalSwapchain* swapchain,
    Uint32 newWidth,
    Uint32 newHeight);

// ==================================================
// Shader
// ==================================================

PalResult PAL_CALL createShaderD3D12(
    PalDevice* device,
    const PalShaderCreateInfo* info,
    PalShader** outShader);

void PAL_CALL destroyShaderD3D12(PalShader* shader);

// ==================================================
// Fence
// ==================================================

PalResult PAL_CALL createFenceD3D12(
    PalDevice* device,
    bool signaled,
    PalFence** outFence);

void PAL_CALL destroyFenceD3D12(PalFence* fence);

PalResult PAL_CALL waitFenceD3D12(
    PalFence* fence,
    Uint64 timeout);

PalResult PAL_CALL resetFenceD3D12(PalFence* fence);

bool PAL_CALL isFenceSignaledD3D12(PalFence* fence);

// ==================================================
// Semaphore
// ==================================================

PalResult PAL_CALL createSemaphoreD3D12(
    PalDevice* device,
    PalSemaphore** outSemaphore);

void PAL_CALL destroySemaphoreD3D12(PalSemaphore* semaphore);

PalResult PAL_CALL waitSemaphoreD3D12(
    PalSemaphore* semaphore,
    Uint64 value,
    Uint64 timeout);

PalResult PAL_CALL signalSemaphoreD3D12(
    PalSemaphore* semaphore,
    PalQueue* queue,
    Uint64 value);

PalResult PAL_CALL getSemaphoreValueD3D12(
    PalSemaphore* semaphore,
    Uint64* outValue);

// ==================================================
// Command Pool And Buffer
// ==================================================

PalResult PAL_CALL createCommandPoolD3D12(
    PalDevice* device,
    PalQueue* queue,
    PalCommandPool** outPool);

void PAL_CALL destroyCommandPoolD3D12(PalCommandPool* pool);

PalResult PAL_CALL resetCommandPoolD3D12(PalCommandPool* pool);

PalResult PAL_CALL allocateCommandBufferD3D12(
    PalDevice* device,
    PalCommandPool* pool,
    PalCommandBufferType type,
    PalCommandBuffer** outBuffer);

void PAL_CALL freeCommandBufferD3D12(PalCommandBuffer* buffer);

PalResult PAL_CALL resetCommandBufferD3D12(PalCommandBuffer* cmdBuffer);

PalResult PAL_CALL submitCommandBufferD3D12(
    PalQueue* queue,
    PalCommandBufferSubmitInfo* info);

// ==================================================
// Command Recording
// ==================================================

PalResult PAL_CALL cmdBeginD3D12(
    PalCommandBuffer* cmdBuffer,
    PalRenderingLayoutInfo* info);

PalResult PAL_CALL cmdEndD3D12(PalCommandBuffer* cmdBuffer);

PalResult PAL_CALL cmdExecuteCommandBufferD3D12(
    PalCommandBuffer* primaryCmdBuffer,
    PalCommandBuffer* secondaryCmdBuffer);

PalResult PAL_CALL cmdSetFragmentShadingRateD3D12(
    PalCommandBuffer* cmdBuffer,
    PalFragmentShadingRateState* state);

PalResult PAL_CALL cmdDrawMeshTasksD3D12(
    PalCommandBuffer* cmdBuffer,
    Uint32 groupCountX,
    Uint32 groupCountY,
    Uint32 groupCountZ);

PalResult PAL_CALL cmdDrawMeshTasksIndirectD3D12(
    PalCommandBuffer* cmdBuffer,
    PalBuffer* buffer,
    Uint64 offset,
    Uint32 drawCount,
    Uint32 stride);

PalResult PAL_CALL cmdDrawMeshTasksIndirectCountD3D12(
    PalCommandBuffer* cmdBuffer,
    PalBuffer* buffer,
    PalBuffer* countBuffer,
    Uint64 offset,
    Uint64 countBufferOffset,
    Uint32 maxDrawCount,
    Uint32 stride);

PalResult PAL_CALL cmdBuildAccelerationStructureD3D12(
    PalCommandBuffer* cmdBuffer,
    PalAccelerationStructureBuildInfo* info);

PalResult PAL_CALL cmdBeginRenderingD3D12(
    PalCommandBuffer* cmdBuffer,
    PalRenderingInfo* info);

PalResult PAL_CALL cmdEndRenderingD3D12(PalCommandBuffer* cmdBuffer);

PalResult PAL_CALL cmdCopyBufferD3D12(
    PalCommandBuffer* cmdBuffer,
    PalBuffer* dst,
    PalBuffer* src,
    PalBufferCopyInfo* copyInfo);

PalResult PAL_CALL cmdCopyBufferToImageD3D12(
    PalCommandBuffer* cmdBuffer,
    PalImage* dstImage,
    PalBuffer* srcBuffer,
    PalBufferImageCopyInfo* copyInfo);

PalResult PAL_CALL cmdCopyImageD3D12(
    PalCommandBuffer* cmdBuffer,
    PalImage* dst,
    PalImage* src,
    PalImageCopyInfo* copyInfo);

PalResult PAL_CALL cmdCopyImageToBufferD3D12(
    PalCommandBuffer* cmdBuffer,
    PalBuffer* dstBuffer,
    PalImage* srcImage,
    PalBufferImageCopyInfo* copyInfo);

PalResult PAL_CALL cmdBindPipelineD3D12(
    PalCommandBuffer* cmdBuffer,
    PalPipelineBindPoint bindPoint,
    PalPipeline* pipeline);

PalResult PAL_CALL cmdSetViewportD3D12(
    PalCommandBuffer* cmdBuffer,
    Uint32 count,
    PalViewport* viewports);

PalResult PAL_CALL cmdSetScissorsD3D12(
    PalCommandBuffer* cmdBuffer,
    Uint32 count,
    PalRect2D* scissors);

PalResult PAL_CALL cmdBindVertexBuffersD3D12(
    PalCommandBuffer* cmdBuffer,
    Uint32 firstSlot,
    Uint32 count,
    PalBuffer** buffers,
    Uint64* offsets);

PalResult PAL_CALL cmdBindIndexBufferD3D12(
    PalCommandBuffer* cmdBuffer,
    PalBuffer* buffer,
    Uint64 offset,
    PalIndexType type);

PalResult PAL_CALL cmdDrawD3D12(
    PalCommandBuffer* cmdBuffer,
    Uint32 vertexCount,
    Uint32 instanceCount,
    Uint32 firstVertex,
    Uint32 firstInstance);

PalResult PAL_CALL cmdDrawIndirectD3D12(
    PalCommandBuffer* cmdBuffer,
    PalBuffer* buffer,
    Uint64 offset,
    Uint32 count,
    Uint32 stride);

PalResult PAL_CALL cmdDrawIndirectCountD3D12(
    PalCommandBuffer* cmdBuffer,
    PalBuffer* buffer,
    PalBuffer* countBuffer,
    Uint64 offset,
    Uint64 countBufferOffset,
    Uint32 maxDrawCount,
    Uint32 stride);

PalResult PAL_CALL cmdDrawIndexedD3D12(
    PalCommandBuffer* cmdBuffer,
    Uint32 indexCount,
    Uint32 instanceCount,
    Uint32 firstIndex,
    Int32 vertexOffset,
    Uint32 firstInstance);

PalResult PAL_CALL cmdDrawIndexedIndirectD3D12(
    PalCommandBuffer* cmdBuffer,
    PalBuffer* buffer,
    Uint64 offset,
    Uint32 count,
    Uint32 stride);

PalResult PAL_CALL cmdDrawIndexedIndirectCountD3D12(
    PalCommandBuffer* cmdBuffer,
    PalBuffer* buffer,
    PalBuffer* countBuffer,
    Uint64 offset,
    Uint64 countBufferOffset,
    Uint32 maxDrawCount,
    Uint32 stride);

PalResult PAL_CALL cmdMemoryBarrierD3D12(
    PalCommandBuffer* cmdBuffer,
    PalUsageStateInfo* oldsUsageStateInfo,
    PalUsageStateInfo* newUsageStateInfo);

PalResult PAL_CALL cmdImageBarrierD3D12(
    PalCommandBuffer* cmdBuffer,
    PalImage* image,
    PalImageSubresourceRange* subresourceRange,
    PalUsageStateInfo* oldUsageStateInfo,
    PalUsageStateInfo* newUsageStateInfo);

PalResult PAL_CALL cmdBufferBarrierD3D12(
    PalCommandBuffer* cmdBuffer,
    PalBuffer* buffer,
    PalUsageStateInfo* oldUsageStateInfo,
    PalUsageStateInfo* newUsageStateInfo);

PalResult PAL_CALL cmdDispatchD3D12(
    PalCommandBuffer* cmdBuffer,
    Uint32 groupCountX,
    Uint32 groupCountY,
    Uint32 groupCountZ);

PalResult PAL_CALL cmdDispatchBaseD3D12(
    PalCommandBuffer* cmdBuffer,
    Uint32 baseGroupX,
    Uint32 baseGroupY,
    Uint32 baseGroupZ,
    Uint32 groupCountX,
    Uint32 groupCountY,
    Uint32 groupCountZ);

PalResult PAL_CALL cmdDispatchIndirectD3D12(
    PalCommandBuffer* cmdBuffer,
    PalBuffer* buffer,
    Uint64 offset);

PalResult PAL_CALL cmdTraceRaysD3D12(
    PalCommandBuffer* cmdBuffer,
    PalShaderBindingTable* sbt,
    Uint32 raygenIndex,
    Uint32 width,
    Uint32 height,
    Uint32 depth);

PalResult PAL_CALL cmdTraceRaysIndirectD3D12(
    PalCommandBuffer* cmdBuffer,
    Uint32 raygenIndex,
    PalShaderBindingTable* sbt,
    PalDeviceAddress bufferAddress);

PalResult PAL_CALL cmdBindDescriptorSetD3D12(
    PalCommandBuffer* cmdBuffer,
    PalPipelineBindPoint bindPoint,
    PalPipelineLayout* layout,
    Uint32 setIndex,
    PalDescriptorSet* set);

PalResult PAL_CALL cmdPushConstantsD3D12(
    PalCommandBuffer* cmdBuffer,
    PalPipelineLayout* layout,
    Uint32 shaderStageCount,
    PalShaderStage* shaderStages,
    Uint32 offset,
    Uint32 size,
    const void* value);

PalResult PAL_CALL cmdSetCullModeD3D12(
    PalCommandBuffer* cmdBuffer,
    PalCullMode cullMode);

PalResult PAL_CALL cmdSetFrontFaceD3D12(
    PalCommandBuffer* cmdBuffer,
    PalFrontFace frontFace);

PalResult PAL_CALL cmdSetPrimitiveTopologyD3D12(
    PalCommandBuffer* cmdBuffer,
    PalPrimitiveTopology topology);

PalResult PAL_CALL cmdSetDepthTestEnableD3D12(
    PalCommandBuffer* cmdBuffer,
    bool enable);

PalResult PAL_CALL cmdSetDepthWriteEnableD3D12(
    PalCommandBuffer* cmdBuffer,
    bool enable);

PalResult PAL_CALL cmdSetStencilOpD3D12(
    PalCommandBuffer* cmdBuffer,
    PalStencilFaceFlags faceMask,
    PalStencilOp failOp,
    PalStencilOp passOp,
    PalStencilOp depthFailOp,
    PalCompareOp compareOp);

// ==================================================
// Acceleration Structure
// ==================================================

PalResult PAL_CALL createAccelerationstructureD3D12(
    PalDevice* device,
    const PalAccelerationStructureCreateInfo* info,
    PalAccelerationStructure** outAs);

void PAL_CALL destroyAccelerationstructureD3D12(PalAccelerationStructure* as);

PalResult PAL_CALL getAccelerationStructureBuildSizeD3D12(
    PalDevice* device,
    PalAccelerationStructureBuildInfo* info,
    PalAccelerationStructureBuildSize* size);

// ==================================================
// Buffer
// ==================================================

PalResult PAL_CALL createBufferD3D12(
    PalDevice* device,
    const PalBufferCreateInfo* info,
    PalBuffer** outBuffer);

void PAL_CALL destroyBufferD3D12(PalBuffer* buffer);

PalResult PAL_CALL getBufferMemoryRequirementsD3D12(
    PalBuffer* buffer,
    PalMemoryRequirements* requirements);

PalResult PAL_CALL computeInstanceBufferRequirementsD3D12(
    PalDevice* device,
    PalInstanceBufferRequirements* requirements,
    Uint32 instanceCount);

PalResult PAL_CALL writeInstancesToMappedMemoryD3D12(
    PalDevice* device,
    void* ptr,
    PalAccelerationStructureInstance* instances,
    Uint32 instanceCount);

PalResult PAL_CALL bindBufferMemoryD3D12(
    PalBuffer* buffer,
    PalMemory* memory,
    Uint64 offset);

PalResult PAL_CALL mapBufferMemoryD3D12(
    PalBuffer* buffer,
    Uint64 offset,
    Uint64 size,
    void** outPtr);

void PAL_CALL unmapBufferMemoryD3D12(PalBuffer* buffer);

PalDeviceAddress PAL_CALL getBufferDeviceAddressD3D12(PalBuffer* buffer);

// ==================================================
// Descriptor Pool, Set and Layout
// ==================================================

PalResult PAL_CALL createDescriptorSetLayoutD3D12(
    PalDevice* device,
    const PalDescriptorSetLayoutCreateInfo* info,
    PalDescriptorSetLayout** outLayout);

void PAL_CALL destroyDescriptorSetLayoutD3D12(PalDescriptorSetLayout* layout);

PalResult PAL_CALL createDescriptorPoolD3D12(
    PalDevice* device,
    const PalDescriptorPoolCreateInfo* info,
    PalDescriptorPool** outPool);

void PAL_CALL destroyDescriptorPoolD3D12(PalDescriptorPool* pool);

PalResult PAL_CALL resetDescriptorPoolD3D12(PalDescriptorPool* pool);

PalResult PAL_CALL allocateDescriptorSetD3D12(
    PalDevice* device,
    PalDescriptorPool* pool,
    PalDescriptorSetLayout* layout,
    PalDescriptorSet** outSet);

void PAL_CALL freeDescriptorSetD3D12(PalDescriptorSet* set);

PalResult PAL_CALL updateDescriptorSetD3D12(
    PalDevice* device,
    Uint32 count,
    PalDescriptorSetWriteInfo* infos);

// ==================================================
// Pipeline Layout
// ==================================================

PalResult PAL_CALL createPipelineLayoutD3D12(
    PalDevice* device,
    const PalPipelineLayoutCreateInfo* info,
    PalPipelineLayout** outLayout);

void PAL_CALL destroyPipelineLayoutD3D12(PalPipelineLayout* layout);

// ==================================================
// Pipeline
// ==================================================

PalResult PAL_CALL createGraphicsPipelineD3D12(
    PalDevice* device,
    const PalGraphicsPipelineCreateInfo* info,
    PalPipeline** outPipeline);

PalResult PAL_CALL createComputePipelineD3D12(
    PalDevice* device,
    const PalComputePipelineCreateInfo* info,
    PalPipeline** outPipeline);

PalResult PAL_CALL createRayTracingPipelineD3D12(
    PalDevice* device,
    const PalRayTracingPipelineCreateInfo* info,
    PalPipeline** outPipeline);

void PAL_CALL destroyPipelineD3D12(PalPipeline* pipeline);

// ==================================================
// Shader Binding Table
// ==================================================

PalResult PAL_CALL createShaderBindingTableD3D12(
    PalDevice* device,
    const PalShaderBindingTableCreateInfo* info,
    PalShaderBindingTable** outSbt);

void PAL_CALL destroyShaderBindingTableD3D12(PalShaderBindingTable* sbt);

static PalGraphicsBackend s_D3D12Backend = {
    // adapter
    .enumerateAdapters = enumerateAdaptersD3D12,
    .getAdapterInfo = getAdapterInfoD3D12,
    .getAdapterCapabilities = getAdapterCapabilitiesD3D12,
    .getAdapterFeatures = getAdapterFeaturesD3D12,

    // device
    .createDevice = createDeviceD3D12,
    .destroyDevice = destroyDeviceD3D12,

    // memory
    .allocateMemory = allocateMemoryD3D12,
    .freeMemory = freeMemoryD3D12,

    // extended adapter features
    .querySamplerAnisotropyCapabilities = querySamplerAnisotropyCapabilitiesD3D12,
    .queryDepthStencilCapabilities = queryDepthStencilCapabilitiesD3D12,
    .queryFragmentShadingRateCapabilities = queryFragmentShadingRateCapabilitiesD3D12,
    .queryMeshShaderCapabilities = queryMeshShaderCapabilitiesD3D12,
    .queryRayTracingCapabilities = queryRayTracingCapabilitiesD3D12,
    .queryDescriptorIndexingCapabilities = queryDescriptorIndexingCapabilitiesD3D12,

    // queue
    .createQueue = createQueueD3D12,
    .destroyQueue = destroyQueueD3D12,
    .waitQueue = waitQueueD3D12,
    .canQueuePresent = canQueuePresentD3D12,

    // format
    .enumerateFormats = enumerateFormatsD3D12,
    .isFormatSupported = isFormatSupportedD3D12,
    .queryFormatImageUsages = queryFormatImageUsagesD3D12,
    .queryFormatImageViewUsages = queryFormatImageViewUsagesD3D12,
    .queryFormatSampleCount = queryFormatSampleCountD3D12,

    // image
    .createImage = createImageD3D12,
    .destroyImage = destroyImageD3D12,
    .getImageInfo = getImageInfoD3D12,
    .getImageMemoryRequirements = getImageMemoryRequirementsD3D12,
    .bindImageMemory = bindImageMemoryD3D12,
    .mapImageMemory = mapImageMemoryD3D12,
    .unmapImageMemory = unmapImageMemoryD3D12,

    // image view
    .createImageView = createImageViewD3D12,
    .destroyImageView = destroyImageViewD3D12,

    // sampler
    .createSampler = createSamplerD3D12,
    .destroySampler = destroySamplerD3D12,

    // surface
    .createSurface = createSurfaceD3D12,
    .destroySurface = destroySurfaceD3D12,
    .getSurfaceCapabilities = getSurfaceCapabilitiesD3D12,

    // swapchain
    .createSwapchain = createSwapchainD3D12,
    .destroySwapchain = destroySwapchainD3D12,
    .getSwapchainImage = getSwapchainImageD3D12,
    .getNextSwapchainImage = getNextSwapchainImageD3D12,
    .presentSwapchain = presentSwapchainD3D12,
    .resizeSwapchain = resizeSwapchainD3D12,

    // shader
    .createShader = createShaderD3D12,
    .destroyShader = destroyShaderD3D12,

    // fence
    .createFence = createFenceD3D12,
    .destroyFence = destroyFenceD3D12,
    .waitFence = waitFenceD3D12,
    .resetFence = resetFenceD3D12,
    .isFenceSignaled = isFenceSignaledD3D12,

    // semaphore
    .createSemaphore = createSemaphoreD3D12,
    .destroySemaphore = destroySemaphoreD3D12,
    .waitSemaphore = waitSemaphoreD3D12,
    .signalSemaphore = signalSemaphoreD3D12,
    .getSemaphoreValue = getSemaphoreValueD3D12,

    // command pool and command buffer
    .createCommandPool = createCommandPoolD3D12,
    .destroyCommandPool = destroyCommandPoolD3D12,
    .resetCommandPool = resetCommandPoolD3D12,
    .allocateCommandBuffer = allocateCommandBufferD3D12,
    .freeCommandBuffer = freeCommandBufferD3D12,
    .resetCommandBuffer = resetCommandBufferD3D12,
    .submitCommandBuffer = submitCommandBufferD3D12,

    // command recording
    .cmdBegin = cmdBeginD3D12,
    .cmdEnd = cmdEndD3D12,
    .cmdExecuteCommandBuffer = cmdExecuteCommandBufferD3D12,
    .cmdSetFragmentShadingRate = cmdSetFragmentShadingRateD3D12,
    .cmdDrawMeshTasks = cmdDrawMeshTasksD3D12,
    .cmdDrawMeshTasksIndirect = cmdDrawMeshTasksIndirectD3D12,
    .cmdDrawMeshTasksIndirectCount = cmdDrawMeshTasksIndirectCountD3D12,
    .cmdBuildAccelerationStructure = cmdBuildAccelerationStructureD3D12,
    .cmdBeginRendering = cmdBeginRenderingD3D12,
    .cmdEndRendering = cmdEndRenderingD3D12,
    .cmdCopyBuffer = cmdCopyBufferD3D12,
    .cmdCopyBufferToImage = cmdCopyBufferToImageD3D12,
    .cmdCopyImage = cmdCopyImageD3D12,
    .cmdCopyImageToBuffer = cmdCopyImageToBufferD3D12,
    .cmdBindPipeline = cmdBindPipelineD3D12,
    .cmdSetViewport = cmdSetViewportD3D12,
    .cmdSetScissors = cmdSetScissorsD3D12,
    .cmdBindVertexBuffers = cmdBindVertexBuffersD3D12,
    .cmdBindIndexBuffer = cmdBindIndexBufferD3D12,
    .cmdDraw = cmdDrawD3D12,
    .cmdDrawIndirect = cmdDrawIndirectD3D12,
    .cmdDrawIndirectCount = cmdDrawIndirectCountD3D12,
    .cmdDrawIndexed = cmdDrawIndexedD3D12,
    .cmdDrawIndexedIndirect = cmdDrawIndexedIndirectD3D12,
    .cmdDrawIndexedIndirectCount = cmdDrawIndexedIndirectCountD3D12,
    .cmdMemoryBarrier = cmdMemoryBarrierD3D12,
    .cmdImageBarrier = cmdImageBarrierD3D12,
    .cmdBufferBarrier = cmdBufferBarrierD3D12,
    .cmdDispatch = cmdDispatchD3D12,
    .cmdDispatchBase = cmdDispatchBaseD3D12,
    .cmdDispatchIndirect = cmdDispatchIndirectD3D12,
    .cmdTraceRays = cmdTraceRaysD3D12,
    .cmdTraceRaysIndirect = cmdTraceRaysIndirectD3D12,
    .cmdBindDescriptorSet = cmdBindDescriptorSetD3D12,
    .cmdPushConstants = cmdPushConstantsD3D12,
    .cmdSetCullMode = cmdSetCullModeD3D12,
    .cmdSetFrontFace = cmdSetFrontFaceD3D12,
    .cmdSetPrimitiveTopology = cmdSetPrimitiveTopologyD3D12,
    .cmdSetDepthTestEnable = cmdSetDepthTestEnableD3D12,
    .cmdSetDepthWriteEnable = cmdSetDepthWriteEnableD3D12,
    .cmdSetStencilOp = cmdSetStencilOpD3D12,

    // acceleration structure
    .createAccelerationstructure = createAccelerationstructureD3D12,
    .destroyAccelerationstructure = destroyAccelerationstructureD3D12,
    .getAccelerationStructureBuildSize = getAccelerationStructureBuildSizeD3D12,

    // buffer
    .createBuffer = createBufferD3D12,
    .destroyBuffer = destroyBufferD3D12,
    .getBufferMemoryRequirements = getBufferMemoryRequirementsD3D12,
    .computeInstanceBufferRequirements = computeInstanceBufferRequirementsD3D12,
    .writeInstancesToMappedMemory = writeInstancesToMappedMemoryD3D12,
    .bindBufferMemory = bindBufferMemoryD3D12,
    .getBufferDeviceAddress = getBufferDeviceAddressD3D12,
    .mapBufferMemory = mapBufferMemoryD3D12,
    .unmapBufferMemory = unmapBufferMemoryD3D12,

    // descriptor set layout, descriptor pool and descriptor set
    .createDescriptorSetLayout = createDescriptorSetLayoutD3D12,
    .destroyDescriptorSetLayout = destroyDescriptorSetLayoutD3D12,
    .createDescriptorPool = createDescriptorPoolD3D12,
    .destroyDescriptorPool = destroyDescriptorPoolD3D12,
    .resetDescriptorPool = resetDescriptorPoolD3D12,
    .allocateDescriptorSet = allocateDescriptorSetD3D12,
    .updateDescriptorSet = updateDescriptorSetD3D12,

    // pipeline layout
    .createPipelineLayout = createPipelineLayoutD3D12,
    .destroyPipelineLayout = destroyPipelineLayoutD3D12,

    // pipeline
    .createGraphicsPipeline = createGraphicsPipelineD3D12,
    .createComputePipeline = createComputePipelineD3D12,
    .createRayTracingPipeline = createRayTracingPipelineD3D12,
    .destroyPipeline = destroyPipelineD3D12,

    // shader binding table
    .createShaderBindingTable = createShaderBindingTableD3D12,
    .destroyShaderBindingTable = destroyShaderBindingTableD3D12};

#endif // PAL_HAS_D3D12

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
    // adapter
    if (!backend->enumerateAdapters                     ||
        !backend->getAdapterInfo                        ||
        !backend->getAdapterCapabilities                ||
        !backend->getAdapterFeatures                    ||

        // device
        !backend->createDevice                          ||
        !backend->destroyDevice                         ||

        // memory
        !backend->allocateMemory                        ||
        !backend->freeMemory                            ||

        // extended adapter features
        !backend->querySamplerAnisotropyCapabilities    ||
        !backend->queryDepthStencilCapabilities         ||
        !backend->queryFragmentShadingRateCapabilities  ||
        !backend->queryMeshShaderCapabilities           ||
        !backend->queryRayTracingCapabilities           ||
        !backend->queryDescriptorIndexingCapabilities   ||

        // queue
        !backend->createQueue                           ||
        !backend->destroyQueue                          ||
        !backend->waitQueue                             ||
        !backend->canQueuePresent                       ||

        // formats
        !backend->enumerateFormats                      ||
        !backend->isFormatSupported                     ||
        !backend->queryFormatImageUsages                ||
        !backend->queryFormatImageViewUsages            ||
        !backend->queryFormatSampleCount                ||

        // image
        !backend->createImage                           ||
        !backend->destroyImage                          ||
        !backend->getImageInfo                          ||
        !backend->getImageMemoryRequirements            ||
        !backend->bindImageMemory                       ||
        !backend->mapImageMemory                        ||
        !backend->unmapImageMemory                      ||

        // image view
        !backend->createImageView                       ||
        !backend->destroyImageView                      ||

        // sampler
        !backend->createSampler                         ||
        !backend->destroySampler                        ||

        // surface
        !backend->createSurface                         ||
        !backend->destroySurface                        ||
        !backend->getSurfaceCapabilities                ||

        // swapchain
        !backend->createSwapchain                       ||
        !backend->destroySwapchain                      ||
        !backend->getSwapchainImage                     ||
        !backend->getNextSwapchainImage                 ||
        !backend->presentSwapchain                      ||
        !backend->resizeSwapchain                       ||

        // shader
        !backend->createShader                          ||
        !backend->destroyShader                         ||

        // fence
        !backend->createFence                           ||
        !backend->destroyFence                          ||
        !backend->waitFence                             ||
        !backend->resetFence                            ||
        !backend->isFenceSignaled                       ||

        // semaphore
        !backend->createSemaphore                       ||
        !backend->destroySemaphore                      ||
        !backend->waitSemaphore                         ||
        !backend->signalSemaphore                       ||
        !backend->getSemaphoreValue                     ||

        // command pool and command buffer
        !backend->createCommandPool                     ||
        !backend->destroyCommandPool                    ||
        !backend->resetCommandPool                      ||
        !backend->allocateCommandBuffer                 ||
        !backend->freeCommandBuffer                     ||
        !backend->submitCommandBuffer                   ||

        // command recording
        !backend->cmdBegin                              ||
        !backend->cmdEnd                                ||
        !backend->resetCommandBuffer                    ||
        !backend->cmdExecuteCommandBuffer               ||
        !backend->cmdSetFragmentShadingRate             ||
        !backend->cmdDrawMeshTasks                      ||
        !backend->cmdDrawMeshTasksIndirect              ||
        !backend->cmdDrawMeshTasksIndirectCount         ||
        !backend->cmdBuildAccelerationStructure         ||
        !backend->cmdBeginRendering                     ||
        !backend->cmdEndRendering                       ||
        !backend->cmdCopyBuffer                         ||
        !backend->cmdCopyBufferToImage                  ||
        !backend->cmdCopyImage                          ||
        !backend->cmdCopyImageToBuffer                  ||
        !backend->cmdBindPipeline                       ||
        !backend->cmdSetViewport                        ||
        !backend->cmdSetScissors                        ||
        !backend->cmdBindVertexBuffers                  ||
        !backend->cmdBindIndexBuffer                    ||
        !backend->cmdDraw                               ||
        !backend->cmdDrawIndirect                       ||
        !backend->cmdDrawIndirectCount                  ||
        !backend->cmdDrawIndexed                        ||
        !backend->cmdDrawIndexedIndirect                ||
        !backend->cmdDrawIndexedIndirectCount           ||
        !backend->cmdMemoryBarrier                      ||
        !backend->cmdImageBarrier                       ||
        !backend->cmdBufferBarrier                      ||
        !backend->cmdDispatch                           ||
        !backend->cmdDispatchBase                       ||
        !backend->cmdDispatchIndirect                   ||
        !backend->cmdTraceRays                          ||
        !backend->cmdTraceRaysIndirect                  ||
        !backend->cmdBindDescriptorSet                  ||
        !backend->cmdPushConstants                      ||
        !backend->cmdSetCullMode                        ||
        !backend->cmdSetFrontFace                       ||
        !backend->cmdSetPrimitiveTopology               ||
        !backend->cmdSetDepthTestEnable                 ||
        !backend->cmdSetDepthWriteEnable                ||
        !backend->cmdSetStencilOp                       ||

        // acceleration structure
        !backend->createAccelerationstructure           ||
        !backend->destroyAccelerationstructure          ||
        !backend->getAccelerationStructureBuildSize     ||

        // buffer
        !backend->createBuffer                          ||
        !backend->destroyBuffer                         ||
        !backend->getBufferMemoryRequirements           ||
        !backend->computeInstanceBufferRequirements     ||
        !backend->writeInstancesToMappedMemory          ||
        !backend->bindBufferMemory                      ||
        !backend->getBufferDeviceAddress                ||
        !backend->mapBufferMemory                       ||
        !backend->unmapBufferMemory                     ||

        // descriptor set layout, descriptor pool and descriptor set
        !backend->createDescriptorSetLayout             ||
        !backend->destroyDescriptorSetLayout            ||
        !backend->createDescriptorPool                  ||
        !backend->destroyDescriptorPool                 ||
        !backend->resetDescriptorPool                   ||
        !backend->allocateDescriptorSet                 ||
        !backend->updateDescriptorSet                   ||

        // pipeline layout
        !backend->createPipelineLayout                  ||
        !backend->destroyPipelineLayout                 ||

        // pipeline
        !backend->createGraphicsPipeline                ||
        !backend->createComputePipeline                 ||
        !backend->createRayTracingPipeline              ||
        !backend->destroyPipeline                       ||

        // shader binding table
        !backend->createShaderBindingTable              ||
        !backend->destroyShaderBindingTable) {
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
    const PalGraphicsDebugger* debugger,
    const PalAllocator* allocator)
{
    if (s_Graphics.initialized) {
        return PAL_RESULT_SUCCESS;
    }

    if (allocator && (!allocator->allocate || !allocator->free)) {
        return PAL_RESULT_INVALID_ALLOCATOR;
    }

    PalResult result;
    BackendData* attachedBackend = nullptr;
#ifdef _WIN32
    // vulkan
#if PAL_HAS_VULKAN
    result = initGraphicsVk(debugger, allocator);
    if (result != PAL_RESULT_SUCCESS) {
        return result;
    }

    attachedBackend = &s_Graphics.backends[s_Graphics.backendCount++];
    attachedBackend->base = &s_VkBackend;
    attachedBackend->startIndex = 0;
    attachedBackend->count = 0;
#endif // PAL_HAS_VULKAN

    // D3D12
#if PAL_HAS_D3D12
    result = initGraphicsD3D12(debugger, allocator);
    if (result != PAL_RESULT_SUCCESS) {
        return result;
    }

    attachedBackend = &s_Graphics.backends[s_Graphics.backendCount++];
    attachedBackend->base = &s_D3D12Backend;
    attachedBackend->startIndex = 0;
    attachedBackend->count = 0;
#endif // PAL_HAS_D3D12

#elif defined(__linux__)
    // vulkan
#if PAL_HAS_VULKAN
    result = initGraphicsVk(debugger, allocator);
    if (result != PAL_RESULT_SUCCESS) {
        return result;
    }

    attachedBackend = &s_Graphics.backends[s_Graphics.backendCount++];
    attachedBackend->base = &s_VkBackend;
    attachedBackend->startIndex = 0;
    attachedBackend->count = 0;
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
    // vulkan
#if PAL_HAS_VULKAN
    shutdownGraphicsVk();
#endif // PAL_HAS_VULKAN

    // D3D12
#if PAL_HAS_D3D12
    shutdownGraphicsD3D12();
#endif // PAL_HAS_D3D12

#elif defined(__linux__)
    // vulkan
#if PAL_HAS_VULKAN
    shutdownGraphicsVk();
#endif // PAL_HAS_VULKAN

#else
    // metal or andriod
#endif // _WIN32

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
    int _count = 0;

    for (int i = 0; i < s_Graphics.backendCount; i++) {
        BackendData* backend = &s_Graphics.backends[i];
        if (outAdapters) {
            // offset into the array so all backends write at the correct index
            PalAdapter** adapters = &outAdapters[backend->startIndex];
            _count = backend->count;
            result = backend->base->enumerateAdapters(&_count, adapters);
            // break if a backend fails
            if (result != PAL_RESULT_SUCCESS) {
                return result;
            }

            for (int j = 0; j < _count; j++) {
                PalAdapter* tmp = adapters[j];
                adapters[j]->backend = backend->base;
            }

        } else {
            result = backend->base->enumerateAdapters(&_count, nullptr);
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
        return PAL_RESULT_GRAPHICS_NOT_INITIALIZED;
    }

    if (!adapter || !info) {
        return PAL_RESULT_NULL_POINTER;
    }

    return adapter->backend->getAdapterInfo(adapter, info);
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

    return adapter->backend->getAdapterCapabilities(adapter, caps);
}

PalAdapterFeatures PAL_CALL palGetAdapterFeatures(PalAdapter* adapter)
{
    if (!s_Graphics.initialized || !adapter) {
        return 0;
    }

    return adapter->backend->getAdapterFeatures(adapter);
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
    Uint64 memoryMask,
    Uint64 size,
    PalMemory** outMemory)
{
    if (!s_Graphics.initialized) {
        return PAL_RESULT_GRAPHICS_NOT_INITIALIZED;
    }

    if (!outMemory || !device) {
        return PAL_RESULT_NULL_POINTER;
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
        return PAL_RESULT_GRAPHICS_NOT_INITIALIZED;
    }

    if (!device || !caps) {
        return PAL_RESULT_NULL_POINTER;
    }

    return device->backend->querySamplerAnisotropyCapabilities(device, caps);
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

    return device->backend->queryDepthStencilCapabilities(device, caps);
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

    return device->backend->queryFragmentShadingRateCapabilities(device, caps);
}

PalResult PAL_CALL palQueryMeshShaderCapabilities(
    PalDevice* device,
    PalMeshShaderCapabilities* caps)
{
    if (!s_Graphics.initialized) {
        return PAL_RESULT_GRAPHICS_NOT_INITIALIZED;
    }

    if (!device || !caps) {
        return PAL_RESULT_NULL_POINTER;
    }

    return device->backend->queryMeshShaderCapabilities(device, caps);
}

PalResult PAL_CALL palQueryRayTracingCapabilities(
    PalDevice* device,
    PalRayTracingCapabilities* caps)
{
    if (!s_Graphics.initialized) {
        return PAL_RESULT_GRAPHICS_NOT_INITIALIZED;
    }

    if (!device || !caps) {
        return PAL_RESULT_NULL_POINTER;
    }

    return device->backend->queryRayTracingCapabilities(device, caps);
}

PalResult PAL_CALL palQueryDescriptorIndexingCapabilities(
    PalDevice* device,
    PalDescriptorIndexingCapabilities* caps)
{
    if (!s_Graphics.initialized) {
        return PAL_RESULT_GRAPHICS_NOT_INITIALIZED;
    }

    if (!device || !caps) {
        return PAL_RESULT_NULL_POINTER;
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
        return PAL_RESULT_GRAPHICS_NOT_INITIALIZED;
    }

    if (!device || !outQueue) {
        return PAL_RESULT_NULL_POINTER;
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

bool PAL_CALL palCanQueuePresent(
    PalQueue* queue,
    PalSurface* surface)
{
    if (s_Graphics.initialized && queue) {
        return queue->backend->canQueuePresent(queue, surface);
    }
    return false;
}

PalResult PAL_CALL palWaitQueue(PalQueue* queue)
{
    if (!s_Graphics.initialized) {
        return PAL_RESULT_GRAPHICS_NOT_INITIALIZED;
    }

    if (!queue) {
        return PAL_RESULT_NULL_POINTER;
    }

    return queue->backend->waitQueue(queue);
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

    return adapter->backend->enumerateFormats(adapter, count, outFormats);
}

bool PAL_CALL palIsFormatSupported(
    PalAdapter* adapter,
    PalFormat format)
{
    if (!s_Graphics.initialized || !adapter) {
        return false;
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

PalImageViewUsages PAL_CALL palQueryFormatImageViewUsages(
    PalAdapter* adapter,
    PalFormat format)
{
    if (!s_Graphics.initialized || !adapter) {
        return PAL_IMAGE_VIEW_USAGE_UNDEFINED;
    }

    return adapter->backend->queryFormatImageViewUsages(adapter, format);
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
        return PAL_RESULT_GRAPHICS_NOT_INITIALIZED;
    }

    if (!device || !info || !outImage) {
        return PAL_RESULT_NULL_POINTER;
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
        return PAL_RESULT_GRAPHICS_NOT_INITIALIZED;
    }

    if (!image || !info) {
        return PAL_RESULT_NULL_POINTER;
    }

    return image->backend->getImageInfo(image, info);
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

    return image->backend->getImageMemoryRequirements(image, requirements);
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

    return image->backend->bindImageMemory(image, memory, offset);
}

PalResult PAL_CALL palMapImageMemory(
    PalImage* image,
    Uint64 offset,
    Uint64 size,
    void** outPtr)
{
    if (!s_Graphics.initialized) {
        return PAL_RESULT_GRAPHICS_NOT_INITIALIZED;
    }

    if (!image) {
        return PAL_RESULT_NULL_POINTER;
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
        return PAL_RESULT_GRAPHICS_NOT_INITIALIZED;
    }

    if (!device || !image || !info || !outImageView) {
        return PAL_RESULT_NULL_POINTER;
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
        return PAL_RESULT_GRAPHICS_NOT_INITIALIZED;
    }

    if (!device || !info || !outSampler) {
        return PAL_RESULT_NULL_POINTER;
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
    PalGraphicsWindow* window,
    PalSurface** outSurface)
{
    if (!s_Graphics.initialized) {
        return PAL_RESULT_GRAPHICS_NOT_INITIALIZED;
    }

    if (!device || !window || !outSurface) {
        return PAL_RESULT_NULL_POINTER;
    }

    PalSurface* surface = nullptr;
    PalResult result;
    result = device->backend->createSurface(device, window, &surface);
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
        return PAL_RESULT_GRAPHICS_NOT_INITIALIZED;
    }

    if (!device || !surface || !caps) {
        return PAL_RESULT_NULL_POINTER;
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
        return PAL_RESULT_GRAPHICS_NOT_INITIALIZED;
    }

    if (!device || !queue || !surface || !info || !outSwapchain) {
        return PAL_RESULT_NULL_POINTER;
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
    Int32 index)
{
    if (!s_Graphics.initialized || !swapchain || index < 0) {
        return nullptr;
    }

    return swapchain->backend->getSwapchainImage(swapchain, index);
}

PalResult PAL_CALL palGetNextSwapchainImage(
    PalSwapchain* swapchain,
    PalSwapchainNextImageInfo* info,
    Uint32* outIndex)
{
    if (!s_Graphics.initialized) {
        return PAL_RESULT_GRAPHICS_NOT_INITIALIZED;
    }

    if (!swapchain || !info) {
        return PAL_RESULT_NULL_POINTER;
    }

    return swapchain->backend->getNextSwapchainImage(swapchain, info, outIndex);
}

PalResult PAL_CALL palPresentSwapchain(
    PalSwapchain* swapchain,
    PalSwapchainPresentInfo* info)
{
    if (!s_Graphics.initialized) {
        return PAL_RESULT_GRAPHICS_NOT_INITIALIZED;
    }

    if (!swapchain || !info) {
        return PAL_RESULT_NULL_POINTER;
    }

    return swapchain->backend->presentSwapchain(swapchain, info);
}

PalResult PAL_CALL palResizeSwapchain(
    PalSwapchain* swapchain,
    Uint32 newWidth,
    Uint32 newHeight)
{
    if (!s_Graphics.initialized) {
        return PAL_RESULT_GRAPHICS_NOT_INITIALIZED;
    }

    if (!swapchain) {
        return PAL_RESULT_NULL_POINTER;
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
        return PAL_RESULT_GRAPHICS_NOT_INITIALIZED;
    }

    if (!device || !info || !outShader) {
        return PAL_RESULT_NULL_POINTER;
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
    bool signaled,
    PalFence** outFence)
{
    if (!s_Graphics.initialized) {
        return PAL_RESULT_GRAPHICS_NOT_INITIALIZED;
    }

    if (!device || !outFence) {
        return PAL_RESULT_NULL_POINTER;
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
    Uint64 timeout)
{
    if (!s_Graphics.initialized) {
        return PAL_RESULT_GRAPHICS_NOT_INITIALIZED;
    }

    if (!fence) {
        return PAL_RESULT_NULL_POINTER;
    }

    return fence->backend->waitFence(fence, timeout);
}

PalResult PAL_CALL palResetFence(PalFence* fence)
{
    if (!s_Graphics.initialized) {
        return PAL_RESULT_GRAPHICS_NOT_INITIALIZED;
    }

    if (!fence) {
        return PAL_RESULT_NULL_POINTER;
    }

    return fence->backend->resetFence(fence);
}

bool PAL_CALL palIsFenceSignaled(PalFence* fence)
{
    if (s_Graphics.initialized && fence) {
        return fence->backend->isFenceSignaled(fence);
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

    PalSemaphore* semaphore = nullptr;
    PalResult result;
    result = device->backend->createSemaphore(device, &semaphore);
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
    Uint64 value,
    Uint64 timeout)
{
    if (!s_Graphics.initialized) {
        return PAL_RESULT_GRAPHICS_NOT_INITIALIZED;
    }

    if (!semaphore) {
        return PAL_RESULT_NULL_POINTER;
    }

    return semaphore->backend->waitSemaphore(semaphore, value, timeout);
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

    return semaphore->backend->signalSemaphore(semaphore, queue, value);
}

PalResult PAL_CALL palGetSemaphoreValue(
    PalSemaphore* semaphore,
    Uint64* outValue)
{
    if (!s_Graphics.initialized) {
        return PAL_RESULT_GRAPHICS_NOT_INITIALIZED;
    }

    if (!semaphore || !outValue) {
        return PAL_RESULT_NULL_POINTER;
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
        return PAL_RESULT_GRAPHICS_NOT_INITIALIZED;
    }

    if (!device || !queue || !outPool) {
        return PAL_RESULT_NULL_POINTER;
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
        return PAL_RESULT_GRAPHICS_NOT_INITIALIZED;
    }

    if (!pool) {
        return PAL_RESULT_NULL_POINTER;
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
        return PAL_RESULT_GRAPHICS_NOT_INITIALIZED;
    }

    if (!device || !pool || !outCmdBuffer) {
        return PAL_RESULT_NULL_POINTER;
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
        return PAL_RESULT_GRAPHICS_NOT_INITIALIZED;
    }

    if (!cmdBuffer) {
        return PAL_RESULT_NULL_POINTER;
    }

    return cmdBuffer->backend->resetCommandBuffer(cmdBuffer);
}

PalResult PAL_CALL palSubmitCommandBuffer(
    PalQueue* queue,
    PalCommandBufferSubmitInfo* info)
{
    if (!s_Graphics.initialized) {
        return PAL_RESULT_GRAPHICS_NOT_INITIALIZED;
    }

    if (!queue || !info) {
        return PAL_RESULT_NULL_POINTER;
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
        return PAL_RESULT_GRAPHICS_NOT_INITIALIZED;
    }

    if (!cmdBuffer) {
        return PAL_RESULT_NULL_POINTER;
    }

    return cmdBuffer->backend->cmdBegin(cmdBuffer, info);
}

PalResult PAL_CALL palCmdEnd(PalCommandBuffer* cmdBuffer)
{
    if (!s_Graphics.initialized) {
        return PAL_RESULT_GRAPHICS_NOT_INITIALIZED;
    }

    if (!cmdBuffer) {
        return PAL_RESULT_NULL_POINTER;
    }

    return cmdBuffer->backend->cmdEnd(cmdBuffer);
}

PalResult PAL_CALL palCmdExecuteCommandBuffer(
    PalCommandBuffer* primaryCmdBuffer,
    PalCommandBuffer* secondaryCmdBuffer)
{
    if (!s_Graphics.initialized) {
        return PAL_RESULT_GRAPHICS_NOT_INITIALIZED;
    }

    if (!primaryCmdBuffer || !secondaryCmdBuffer) {
        return PAL_RESULT_NULL_POINTER;
    }

    return primaryCmdBuffer->backend->cmdExecuteCommandBuffer(primaryCmdBuffer, secondaryCmdBuffer);
}

PalResult PAL_CALL palCmdSetFragmentShadingRate(
    PalCommandBuffer* cmdBuffer,
    PalFragmentShadingRateState* state)
{
    if (!s_Graphics.initialized) {
        return PAL_RESULT_GRAPHICS_NOT_INITIALIZED;
    }

    if (!cmdBuffer || !state) {
        return PAL_RESULT_NULL_POINTER;
    }

    return cmdBuffer->backend->cmdSetFragmentShadingRate(cmdBuffer, state);
}

PalResult PAL_CALL palCmdDrawMeshTasks(
    PalCommandBuffer* cmdBuffer,
    Uint32 groupCountX,
    Uint32 groupCountY,
    Uint32 groupCountZ)
{
    if (!s_Graphics.initialized) {
        return PAL_RESULT_GRAPHICS_NOT_INITIALIZED;
    }

    if (!cmdBuffer) {
        return PAL_RESULT_NULL_POINTER;
    }

    return cmdBuffer->backend->cmdDrawMeshTasks(cmdBuffer, groupCountX, groupCountY, groupCountZ);
}

PalResult PAL_CALL palCmdDrawMeshTasksIndirect(
    PalCommandBuffer* cmdBuffer,
    PalBuffer* buffer,
    Uint64 offset,
    Uint32 drawCount,
    Uint32 stride)
{
    if (!s_Graphics.initialized) {
        return PAL_RESULT_GRAPHICS_NOT_INITIALIZED;
    }

    if (!cmdBuffer || !buffer) {
        return PAL_RESULT_NULL_POINTER;
    }

    return cmdBuffer->backend
        ->cmdDrawMeshTasksIndirect(cmdBuffer, buffer, offset, drawCount, stride);
}

PalResult PAL_CALL palCmdDrawMeshTasksIndirectCount(
    PalCommandBuffer* cmdBuffer,
    PalBuffer* buffer,
    PalBuffer* countBuffer,
    Uint64 offset,
    Uint64 countBufferOffset,
    Uint32 maxDrawCount,
    Uint32 stride)
{
    if (!s_Graphics.initialized) {
        return PAL_RESULT_GRAPHICS_NOT_INITIALIZED;
    }

    if (!cmdBuffer || !buffer || !countBuffer) {
        return PAL_RESULT_NULL_POINTER;
    }

    return cmdBuffer->backend->cmdDrawMeshTasksIndirectCount(
        cmdBuffer,
        buffer,
        countBuffer,
        offset,
        countBufferOffset,
        maxDrawCount,
        stride);
}

PalResult PAL_CALL palCmdBuildAccelerationStructure(
    PalCommandBuffer* cmdBuffer,
    PalAccelerationStructureBuildInfo* info)
{
    if (!s_Graphics.initialized) {
        return PAL_RESULT_GRAPHICS_NOT_INITIALIZED;
    }

    if (!cmdBuffer) {
        return PAL_RESULT_NULL_POINTER;
    }

    if (!info->dst || info->scratchBufferAddress == 0) {
        return PAL_RESULT_NULL_POINTER;
    }

    return cmdBuffer->backend->cmdBuildAccelerationStructure(cmdBuffer, info);
}

PalResult PAL_CALL palCmdBeginRendering(
    PalCommandBuffer* cmdBuffer,
    PalRenderingInfo* info)
{
    if (!s_Graphics.initialized) {
        return PAL_RESULT_GRAPHICS_NOT_INITIALIZED;
    }

    if (!cmdBuffer || !info) {
        return PAL_RESULT_NULL_POINTER;
    }

    return cmdBuffer->backend->cmdBeginRendering(cmdBuffer, info);
}

PalResult PAL_CALL palCmdEndRendering(PalCommandBuffer* cmdBuffer)
{
    if (!s_Graphics.initialized) {
        return PAL_RESULT_GRAPHICS_NOT_INITIALIZED;
    }

    if (!cmdBuffer) {
        return PAL_RESULT_NULL_POINTER;
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
        return PAL_RESULT_GRAPHICS_NOT_INITIALIZED;
    }

    if (!cmdBuffer || !dst || !src || !copyInfo) {
        return PAL_RESULT_NULL_POINTER;
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
        return PAL_RESULT_GRAPHICS_NOT_INITIALIZED;
    }

    if (!cmdBuffer || !dstImage || !srcBuffer || !copyInfo) {
        return PAL_RESULT_NULL_POINTER;
    }

    return cmdBuffer->backend->cmdCopyBufferToImage(
        cmdBuffer, 
        dstImage, 
        srcBuffer,
        copyInfo);
}

PalResult PAL_CALL palCmdCopyImage(
    PalCommandBuffer* cmdBuffer,
    PalImage* dst,
    PalImage* src,
    PalImageCopyInfo* copyInfo)
{
    if (!s_Graphics.initialized) {
        return PAL_RESULT_GRAPHICS_NOT_INITIALIZED;
    }

    if (!cmdBuffer || !dst || !src || !copyInfo) {
        return PAL_RESULT_NULL_POINTER;
    }

    return cmdBuffer->backend->cmdCopyImage(
        cmdBuffer, 
        dst, 
        src,
        copyInfo);
}
   
PalResult PAL_CALL palCmdCopyImageToBuffer(
    PalCommandBuffer* cmdBuffer,
    PalBuffer* dstBuffer,
    PalImage* srcImage,
    PalBufferImageCopyInfo* copyInfo)
{
    if (!s_Graphics.initialized) {
        return PAL_RESULT_GRAPHICS_NOT_INITIALIZED;
    }

    if (!cmdBuffer || !dstBuffer || !srcImage || !copyInfo) {
        return PAL_RESULT_NULL_POINTER;
    }

    return cmdBuffer->backend->cmdCopyImageToBuffer(
        cmdBuffer, 
        dstBuffer, 
        srcImage,
        copyInfo);
}

PalResult PAL_CALL palCmdBindPipeline(
    PalCommandBuffer* cmdBuffer,
    PalPipelineBindPoint bindPoint,
    PalPipeline* pipeline)
{
    if (!s_Graphics.initialized) {
        return PAL_RESULT_GRAPHICS_NOT_INITIALIZED;
    }

    if (!cmdBuffer || !pipeline) {
        return PAL_RESULT_NULL_POINTER;
    }

    return cmdBuffer->backend->cmdBindPipeline(cmdBuffer, bindPoint, pipeline);
}

PalResult PAL_CALL palCmdSetViewport(
    PalCommandBuffer* cmdBuffer,
    Uint32 count,
    PalViewport* viewports)
{
    if (!s_Graphics.initialized) {
        return PAL_RESULT_GRAPHICS_NOT_INITIALIZED;
    }

    if (!cmdBuffer || !viewports || !count) {
        return PAL_RESULT_NULL_POINTER;
    }

    return cmdBuffer->backend->cmdSetViewport(cmdBuffer, count, viewports);
}

PalResult PAL_CALL palCmdSetScissors(
    PalCommandBuffer* cmdBuffer,
    Uint32 count,
    PalRect2D* scissors)
{
    if (!s_Graphics.initialized) {
        return PAL_RESULT_GRAPHICS_NOT_INITIALIZED;
    }

    if (!cmdBuffer || !scissors || !count) {
        return PAL_RESULT_NULL_POINTER;
    }

    return cmdBuffer->backend->cmdSetScissors(cmdBuffer, count, scissors);
}

PalResult PAL_CALL palCmdBindVertexBuffers(
    PalCommandBuffer* cmdBuffer,
    Uint32 firstSlot,
    Uint32 count,
    PalBuffer** buffers,
    Uint64* offsets)
{
    if (!s_Graphics.initialized) {
        return PAL_RESULT_GRAPHICS_NOT_INITIALIZED;
    }

    if (!cmdBuffer || !buffers || !offsets) {
        return PAL_RESULT_NULL_POINTER;
    }

    return cmdBuffer->backend->cmdBindVertexBuffers(cmdBuffer, firstSlot, count, buffers, offsets);
}

PalResult PAL_CALL palCmdBindIndexBuffer(
    PalCommandBuffer* cmdBuffer,
    PalBuffer* buffer,
    Uint64 offset,
    PalIndexType type)
{
    if (!s_Graphics.initialized) {
        return PAL_RESULT_GRAPHICS_NOT_INITIALIZED;
    }

    if (!cmdBuffer || !buffer) {
        return PAL_RESULT_NULL_POINTER;
    }

    return cmdBuffer->backend->cmdBindIndexBuffer(cmdBuffer, buffer, offset, type);
}

PalResult PAL_CALL palCmdDraw(
    PalCommandBuffer* cmdBuffer,
    Uint32 vertexCount,
    Uint32 instanceCount,
    Uint32 firstVertex,
    Uint32 firstInstance)
{
    if (!s_Graphics.initialized) {
        return PAL_RESULT_GRAPHICS_NOT_INITIALIZED;
    }

    if (!cmdBuffer) {
        return PAL_RESULT_NULL_POINTER;
    }

    return cmdBuffer->backend
        ->cmdDraw(cmdBuffer, vertexCount, instanceCount, firstVertex, firstInstance);
}

PalResult PAL_CALL palCmdDrawIndirect(
    PalCommandBuffer* cmdBuffer,
    PalBuffer* buffer,
    Uint64 offset,
    Uint32 count,
    Uint32 stride)
{
    if (!s_Graphics.initialized) {
        return PAL_RESULT_GRAPHICS_NOT_INITIALIZED;
    }

    if (!cmdBuffer || !buffer) {
        return PAL_RESULT_NULL_POINTER;
    }

    return cmdBuffer->backend->cmdDrawIndirect(cmdBuffer, buffer, offset, count, stride);
}

PalResult PAL_CALL palCmdDrawIndirectCount(
    PalCommandBuffer* cmdBuffer,
    PalBuffer* buffer,
    PalBuffer* countBuffer,
    Uint64 offset,
    Uint64 countBufferOffset,
    Uint32 maxDrawCount,
    Uint32 stride)
{
    if (!s_Graphics.initialized) {
        return PAL_RESULT_GRAPHICS_NOT_INITIALIZED;
    }

    if (!cmdBuffer || !buffer || !countBuffer) {
        return PAL_RESULT_NULL_POINTER;
    }

    return cmdBuffer->backend->cmdDrawIndirectCount(
        cmdBuffer,
        buffer,
        countBuffer,
        offset,
        countBufferOffset,
        maxDrawCount,
        stride);
}

PalResult PAL_CALL palCmdDrawIndexed(
    PalCommandBuffer* cmdBuffer,
    Uint32 indexCount,
    Uint32 instanceCount,
    Uint32 firstIndex,
    Int32 vertexOffset,
    Uint32 firstInstance)
{
    if (!s_Graphics.initialized) {
        return PAL_RESULT_GRAPHICS_NOT_INITIALIZED;
    }

    if (!cmdBuffer) {
        return PAL_RESULT_NULL_POINTER;
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
    Uint64 offset,
    Uint32 count,
    Uint32 stride)
{
    if (!s_Graphics.initialized) {
        return PAL_RESULT_GRAPHICS_NOT_INITIALIZED;
    }

    if (!cmdBuffer || !buffer) {
        return PAL_RESULT_NULL_POINTER;
    }

    return cmdBuffer->backend->cmdDrawIndexedIndirect(cmdBuffer, buffer, offset, count, stride);
}

PalResult PAL_CALL palCmdDrawIndexedIndirectCount(
    PalCommandBuffer* cmdBuffer,
    PalBuffer* buffer,
    PalBuffer* countBuffer,
    Uint64 offset,
    Uint64 countBufferOffset,
    Uint32 maxDrawCount,
    Uint32 stride)
{
    if (!s_Graphics.initialized) {
        return PAL_RESULT_GRAPHICS_NOT_INITIALIZED;
    }

    if (!cmdBuffer || !buffer || !countBuffer) {
        return PAL_RESULT_NULL_POINTER;
    }

    return cmdBuffer->backend->cmdDrawIndexedIndirectCount(
        cmdBuffer,
        buffer,
        countBuffer,
        offset,
        countBufferOffset,
        maxDrawCount,
        stride);
}

PalResult PAL_CALL palCmdMemoryBarrier(
    PalCommandBuffer* cmdBuffer,
    PalUsageStateInfo* oldUsageStateInfo,
    PalUsageStateInfo* newUsageStateInfo)
{
    if (!s_Graphics.initialized) {
        return PAL_RESULT_GRAPHICS_NOT_INITIALIZED;
    }

    if (!cmdBuffer || !oldUsageStateInfo || !newUsageStateInfo) {
        return PAL_RESULT_NULL_POINTER;
    }

    return cmdBuffer->backend->cmdMemoryBarrier(cmdBuffer, oldUsageStateInfo, newUsageStateInfo);
}

PalResult PAL_CALL palCmdImageBarrier(
    PalCommandBuffer* cmdBuffer,
    PalImage* image,
    PalImageSubresourceRange* subresourceRange,
    PalUsageStateInfo* oldUsageStateInfo,
    PalUsageStateInfo* newUsageStateInfo)
{
    if (!s_Graphics.initialized) {
        return PAL_RESULT_GRAPHICS_NOT_INITIALIZED;
    }

    if (!cmdBuffer || !image ||!subresourceRange || !oldUsageStateInfo || !newUsageStateInfo) {
        return PAL_RESULT_NULL_POINTER;
    }

    return cmdBuffer->backend->cmdImageBarrier(
        cmdBuffer, 
        image, 
        subresourceRange, 
        oldUsageStateInfo, 
        newUsageStateInfo);
}

PalResult PAL_CALL palCmdBufferBarrier(
    PalCommandBuffer* cmdBuffer,
    PalBuffer* buffer,
    PalUsageStateInfo* oldUsageStateInfo,
    PalUsageStateInfo* newUsageStateInfo)
{
    if (!s_Graphics.initialized) {
        return PAL_RESULT_GRAPHICS_NOT_INITIALIZED;
    }

    if (!cmdBuffer || !buffer || !oldUsageStateInfo || !newUsageStateInfo) {
        return PAL_RESULT_NULL_POINTER;
    }

    return cmdBuffer->backend
        ->cmdBufferBarrier(cmdBuffer, buffer, oldUsageStateInfo, newUsageStateInfo);
}

PalResult PAL_CALL palCmdDispatch(
    PalCommandBuffer* cmdBuffer,
    Uint32 groupCountX,
    Uint32 groupCountY,
    Uint32 groupCountZ)
{
    if (!s_Graphics.initialized) {
        return PAL_RESULT_GRAPHICS_NOT_INITIALIZED;
    }

    if (!cmdBuffer) {
        return PAL_RESULT_NULL_POINTER;
    }

    return cmdBuffer->backend->cmdDispatch(cmdBuffer, groupCountX, groupCountY, groupCountZ);
}

PalResult PAL_CALL palCmdDispatchBase(
    PalCommandBuffer* cmdBuffer,
    Uint32 baseGroupX,
    Uint32 baseGroupY,
    Uint32 baseGroupZ,
    Uint32 groupCountX,
    Uint32 groupCountY,
    Uint32 groupCountZ)
{
    if (!s_Graphics.initialized) {
        return PAL_RESULT_GRAPHICS_NOT_INITIALIZED;
    }

    if (!cmdBuffer) {
        return PAL_RESULT_NULL_POINTER;
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
    PalBuffer* buffer,
    Uint64 offset)
{
    if (!s_Graphics.initialized) {
        return PAL_RESULT_GRAPHICS_NOT_INITIALIZED;
    }

    if (!cmdBuffer || !buffer) {
        return PAL_RESULT_NULL_POINTER;
    }

    return cmdBuffer->backend->cmdDispatchIndirect(cmdBuffer, buffer, offset);
}

PalResult PAL_CALL palCmdTraceRays(
    PalCommandBuffer* cmdBuffer,
    PalShaderBindingTable* sbt,
    Uint32 raygenIndex,
    Uint32 width,
    Uint32 height,
    Uint32 depth)
{
    if (!s_Graphics.initialized) {
        return PAL_RESULT_GRAPHICS_NOT_INITIALIZED;
    }

    if (!cmdBuffer || !sbt) {
        return PAL_RESULT_NULL_POINTER;
    }

    return cmdBuffer->backend->cmdTraceRays(cmdBuffer, sbt, raygenIndex, width, height, depth);
}

PalResult PAL_CALL palCmdTraceRaysIndirect(
    PalCommandBuffer* cmdBuffer,
    Uint32 raygenIndex,
    PalShaderBindingTable* sbt,
    PalDeviceAddress bufferAddress)
{
    if (!s_Graphics.initialized) {
        return PAL_RESULT_GRAPHICS_NOT_INITIALIZED;
    }

    if (!cmdBuffer || !sbt) {
        return PAL_RESULT_NULL_POINTER;
    }

    return cmdBuffer->backend->cmdTraceRaysIndirect(cmdBuffer, raygenIndex, sbt, bufferAddress);
}

PalResult PAL_CALL palCmdBindDescriptorSet(
    PalCommandBuffer* cmdBuffer,
    PalPipelineBindPoint bindPoint,
    PalPipelineLayout* layout,
    Uint32 setIndex,
    PalDescriptorSet* set)
{
    if (!s_Graphics.initialized) {
        return PAL_RESULT_GRAPHICS_NOT_INITIALIZED;
    }

    if (!cmdBuffer || !layout || !set) {
        return PAL_RESULT_NULL_POINTER;
    }

    return cmdBuffer->backend->cmdBindDescriptorSet(
        cmdBuffer, 
        bindPoint, 
        layout, 
        setIndex, 
        set);
}

PalResult PAL_CALL palCmdPushConstants(
    PalCommandBuffer* cmdBuffer,
    PalPipelineLayout* layout,
    Uint32 shaderStageCount,
    PalShaderStage* shaderStages,
    Uint32 offset,
    Uint32 size,
    const void* value)
{
    if (!s_Graphics.initialized) {
        return PAL_RESULT_GRAPHICS_NOT_INITIALIZED;
    }

    if (!cmdBuffer || !layout || !shaderStages || !value) {
        return PAL_RESULT_NULL_POINTER;
    }

    return cmdBuffer->backend
        ->cmdPushConstants(cmdBuffer, layout, shaderStageCount, shaderStages, offset, size, value);
}

PalResult PAL_CALL palCmdSetCullMode(
    PalCommandBuffer* cmdBuffer,
    PalCullMode cullMode)
{
    if (!s_Graphics.initialized) {
        return PAL_RESULT_GRAPHICS_NOT_INITIALIZED;
    }

    if (!cmdBuffer) {
        return PAL_RESULT_NULL_POINTER;
    }

    return cmdBuffer->backend->cmdSetCullMode(cmdBuffer, cullMode);
}

PalResult PAL_CALL palCmdSetFrontFace(
    PalCommandBuffer* cmdBuffer,
    PalFrontFace frontFace)
{
    if (!s_Graphics.initialized) {
        return PAL_RESULT_GRAPHICS_NOT_INITIALIZED;
    }

    if (!cmdBuffer) {
        return PAL_RESULT_NULL_POINTER;
    }

    return cmdBuffer->backend->cmdSetFrontFace(cmdBuffer, frontFace);
}

PalResult PAL_CALL palCmdSetPrimitiveTopology(
    PalCommandBuffer* cmdBuffer,
    PalPrimitiveTopology topology)
{
    if (!s_Graphics.initialized) {
        return PAL_RESULT_GRAPHICS_NOT_INITIALIZED;
    }

    if (!cmdBuffer) {
        return PAL_RESULT_NULL_POINTER;
    }

    return cmdBuffer->backend->cmdSetPrimitiveTopology(cmdBuffer, topology);
}

PalResult PAL_CALL palCmdSetDepthTestEnable(
    PalCommandBuffer* cmdBuffer,
    bool enable)
{
    if (!s_Graphics.initialized) {
        return PAL_RESULT_GRAPHICS_NOT_INITIALIZED;
    }

    if (!cmdBuffer) {
        return PAL_RESULT_NULL_POINTER;
    }

    return cmdBuffer->backend->cmdSetDepthTestEnable(cmdBuffer, enable);
}

PalResult PAL_CALL palCmdSetDepthWriteEnable(
    PalCommandBuffer* cmdBuffer,
    bool enable)
{
    if (!s_Graphics.initialized) {
        return PAL_RESULT_GRAPHICS_NOT_INITIALIZED;
    }

    if (!cmdBuffer) {
        return PAL_RESULT_NULL_POINTER;
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
        return PAL_RESULT_GRAPHICS_NOT_INITIALIZED;
    }

    if (!cmdBuffer) {
        return PAL_RESULT_NULL_POINTER;
    }

    return cmdBuffer->backend->cmdSetStencilOp(
        cmdBuffer, 
        faceMask, 
        failOp, 
        passOp, 
        depthFailOp, 
        compareOp);
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
        return PAL_RESULT_GRAPHICS_NOT_INITIALIZED;
    }

    if (!device || !info || !outAs) {
        return PAL_RESULT_NULL_POINTER;
    }

    if (!info->buffer) {
        return PAL_RESULT_NULL_POINTER;
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
        return PAL_RESULT_GRAPHICS_NOT_INITIALIZED;
    }

    if (!device || !info || !size) {
        return PAL_RESULT_NULL_POINTER;
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
        return PAL_RESULT_GRAPHICS_NOT_INITIALIZED;
    }

    if (!device || !info) {
        return PAL_RESULT_NULL_POINTER;
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
        return PAL_RESULT_GRAPHICS_NOT_INITIALIZED;
    }

    if (!buffer) {
        return PAL_RESULT_NULL_POINTER;
    }

    return buffer->backend->getBufferMemoryRequirements(buffer, requirements);
}

PalResult PAL_CALL palComputeInstanceBufferRequirements(
    PalDevice* device,
    PalInstanceBufferRequirements* requirements,
    Uint32 instanceCount)
{
    if (!s_Graphics.initialized) {
        return PAL_RESULT_GRAPHICS_NOT_INITIALIZED;
    }

    if (!device) {
        return PAL_RESULT_NULL_POINTER;
    }

    return device->backend->computeInstanceBufferRequirements(device, requirements, instanceCount);
}

PalResult PAL_CALL palWriteInstancesToMappedMemory(
    PalDevice* device,
    void* ptr,
    PalAccelerationStructureInstance* instances,
    Uint32 instanceCount)
{
    if (!s_Graphics.initialized) {
        return PAL_RESULT_GRAPHICS_NOT_INITIALIZED;
    }

    if (!device || !ptr || !instances || instanceCount == 0) {
        return PAL_RESULT_NULL_POINTER;
    }

    // since all blas have backend pointer, we use the first one
    return device->backend->writeInstancesToMappedMemory(device, ptr, instances, instanceCount);
}

PalResult PAL_CALL palBindBufferMemory(
    PalBuffer* buffer,
    PalMemory* memory,
    Uint64 offset)
{
    if (!s_Graphics.initialized) {
        return PAL_RESULT_GRAPHICS_NOT_INITIALIZED;
    }

    if (!buffer || !memory) {
        return PAL_RESULT_NULL_POINTER;
    }

    return buffer->backend->bindBufferMemory(buffer, memory, offset);
}

PalResult PAL_CALL palMapBufferMemory(
    PalBuffer* buffer,
    Uint64 offset,
    Uint64 size,
    void** outPtr)
{
    if (!s_Graphics.initialized) {
        return PAL_RESULT_GRAPHICS_NOT_INITIALIZED;
    }

    if (!buffer) {
        return PAL_RESULT_NULL_POINTER;
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
        return PAL_RESULT_GRAPHICS_NOT_INITIALIZED;
    }

    if (!device || !info || !outLayout) {
        return PAL_RESULT_NULL_POINTER;
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
        return PAL_RESULT_GRAPHICS_NOT_INITIALIZED;
    }

    if (!device || !info || !outPool) {
        return PAL_RESULT_NULL_POINTER;
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
        return PAL_RESULT_GRAPHICS_NOT_INITIALIZED;
    }

    if (!pool) {
        return PAL_RESULT_NULL_POINTER;
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
        return PAL_RESULT_GRAPHICS_NOT_INITIALIZED;
    }

    if (!device || !pool || !layout || !outSet) {
        return PAL_RESULT_NULL_POINTER;
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
    Uint32 count,
    PalDescriptorSetWriteInfo* infos)
{
    if (!s_Graphics.initialized) {
        return PAL_RESULT_GRAPHICS_NOT_INITIALIZED;
    }

    if (!device || !infos) {
        return PAL_RESULT_NULL_POINTER;
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
        return PAL_RESULT_GRAPHICS_NOT_INITIALIZED;
    }

    if (!device || !info || !outLayout) {
        return PAL_RESULT_NULL_POINTER;
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
        return PAL_RESULT_GRAPHICS_NOT_INITIALIZED;
    }

    if (!device || !info || !outPipeline) {
        return PAL_RESULT_NULL_POINTER;
    }

    if (!info->renderingLayout) {
        return PAL_RESULT_NULL_POINTER;
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
        return PAL_RESULT_GRAPHICS_NOT_INITIALIZED;
    }

    if (!device || !info || !outPipeline) {
        return PAL_RESULT_NULL_POINTER;
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
        return PAL_RESULT_GRAPHICS_NOT_INITIALIZED;
    }

    if (!device || !info || !outPipeline) {
        return PAL_RESULT_NULL_POINTER;
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
        return PAL_RESULT_GRAPHICS_NOT_INITIALIZED;
    }

    if (!device || !info || !outSbt) {
        return PAL_RESULT_NULL_POINTER;
    }

    if (!info->rayTracingPipeline) {
        return PAL_RESULT_NULL_POINTER;
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

// ==================================================
// Utils
// ==================================================

bool PAL_CALL palBuildWorkGroupInfo(
    const PalWorkGroupBuildData* data,
    Int32* count,
    PalWorkGroupInfo* infos)
{
    if (!data) {
        return false;
    }

    if (*count == 0 && infos) {
        return false;
    }

    Uint32 workGroupCount[3];
    Uint32 groupInfoCount[3];
    for (int i = 0; i < 3; i++) {
        Uint32 tmp = _ceil(data->workCount[i], data->workGroupSize[i]);
        workGroupCount[i] = tmp;
        groupInfoCount[i] = _ceil(tmp, data->workGroupCount[i]);
    }

    if (!infos) {
        // total number of group build info on all axis
        *count = groupInfoCount[0] * groupInfoCount[1] * groupInfoCount[2];
        ;
        return true;
    }

    for (int i = 0; i < *count; i++) {
        PalWorkGroupInfo* buildInfo = &infos[i];
        // find index
        Uint32 index[3];
        index[0] = i % groupInfoCount[0];
        index[1] = (i / groupInfoCount[0]) % groupInfoCount[1];
        index[2] = i / (groupInfoCount[0] * groupInfoCount[1]);

        // fill group build info
        for (int j = 0; j < 3; j++) {
            buildInfo->workGroupBase[j] = index[j] * data->workGroupCount[j];
            buildInfo->workGroupBase[j] = index[j] * data->workGroupCount[j];
            buildInfo->workGroupBase[j] = index[j] * data->workGroupCount[j];

            Uint32 tmp = workGroupCount[j] - buildInfo->workGroupBase[j];
            buildInfo->workGroupCount[j] = _min(data->workGroupCount[j], tmp);
        }
    }

    return true;
}
