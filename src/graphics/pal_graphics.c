
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

PalResult PAL_CALL initGraphicsVk(
    const PalGraphicsDebugger* debugger,
    const PalAllocator* allocator);

PalResult PAL_CALL shutdownGraphicsVk();

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

PalResult PAL_CALL createDeviceVk(
    PalAdapter* adapter,
    PalAdapterFeatures features,
    PalDevice** outDevice);

void PAL_CALL destroyDeviceVk(PalDevice* device);

PalResult PAL_CALL waitDeviceVk(PalDevice* device);

PalResult PAL_CALL allocateMemoryVk(
    PalDevice* device,
    PalMemoryType type,
    Uint64 memoryMask,
    Uint64 size,
    PalMemory** outMemory);

void PAL_CALL freeMemoryVk(
    PalDevice* device,
    PalMemory* memory);

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

PalResult PAL_CALL createQueueVk(
    PalDevice* device,
    PalQueueType type,
    PalQueue** outQueue);

void PAL_CALL destroyQueueVk(PalQueue* queue);

PalResult PAL_CALL waitQueueVk(PalQueue* queue);

bool PAL_CALL canQueuePresentVk(
    PalQueue* queue,
    PalGraphicsWindow* window);

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

PalResult PAL_CALL createImageViewVk(
    PalDevice* device,
    PalImage* image,
    const PalImageViewCreateInfo* info,
    PalImageView** outImageView);

void PAL_CALL destroyImageViewVk(PalImageView* imageView);

PalResult PAL_CALL createSamplerVk(
    PalDevice* device,
    const PalSamplerCreateInfo* info,
    PalSampler** outSampler);

void PAL_CALL destroySamplerVk(PalSampler* sampler);

PalResult PAL_CALL querySwapchainCapabilitiesVk(
    PalDevice* device,
    PalGraphicsWindow* window,
    PalSwapchainCapabilities* caps);

PalResult PAL_CALL createSwapchainVk(
    PalDevice* device,
    PalQueue* queue,
    PalGraphicsWindow* window,
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

PalResult PAL_CALL createShaderVk(
    PalDevice* device,
    const PalShaderCreateInfo* info,
    PalShader** outShader);

void PAL_CALL destroyShaderVk(PalShader* shader);

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
    Uint64* value);

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
    Uint64 dstOffset,
    Uint64 srcOffset,
    Uint32 size);

PalResult PAL_CALL cmdBindPipelineVk(
    PalCommandBuffer* cmdBuffer,
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

PalResult PAL_CALL cmdImageViewBarrierVk(
    PalCommandBuffer* cmdBuffer,
    PalImageView* imageView,
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
    Uint32 width,
    Uint32 height,
    Uint32 depth);

PalResult PAL_CALL cmdTraceRaysIndirectVk(
    PalCommandBuffer* cmdBuffer,
    PalDeviceAddress bufferAddress);

PalResult PAL_CALL cmdBindDescriptorSetVk(
    PalCommandBuffer* cmdBuffer,
    PalPipeline* pipeline,
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

PalResult PAL_CALL createAccelerationstructureVk(
    PalDevice* device,
    const PalAccelerationStructureCreateInfo* info,
    PalAccelerationStructure** outAs);

void PAL_CALL destroyAccelerationstructureVk(PalAccelerationStructure* as);

PalResult PAL_CALL getAccelerationStructureBuildSizeVk(
    PalDevice* device,
    PalAccelerationStructureBuildInfo* info,
    PalAccelerationStructureBuildSize* size);

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

PalDeviceAddress PAL_CALL getBufferDeviceAddressVk(PalBuffer* buffer);

PalResult PAL_CALL mapMemoryVk(
    PalDevice* device,
    PalMemory* memory,
    Uint64 offset,
    Uint64 size,
    void** outPtr);

void PAL_CALL unmapMemoryVk(
    PalDevice* device,
    PalMemory* memory);

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

PalResult PAL_CALL createPipelineLayoutVk(
    PalDevice* device,
    const PalPipelineLayoutCreateInfo* info,
    PalPipelineLayout** outLayout);

void PAL_CALL destroyPipelineLayoutVk(PalPipelineLayout* layout);

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

static PalGraphicsBackend s_VkBackend = {
    // adapter
    .enumerateAdapters = enumerateAdaptersVk,
    .getAdapterInfo = getAdapterInfoVk,
    .getAdapterCapabilities = getAdapterCapabilitiesVk,
    .getAdapterFeatures = getAdapterFeaturesVk,

    // device
    .createDevice = createDeviceVk,
    .destroyDevice = destroyDeviceVk,
    .waitDevice = waitDeviceVk,

    // memory
    .allocateMemory = allocateMemoryVk,
    .freeMemory = freeMemoryVk,
    .mapMemory = mapMemoryVk,
    .unmapMemory = unmapMemoryVk,

    // extended adapter features
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

    // image
    .createImage = createImageVk,
    .destroyImage = destroyImageVk,
    .getImageInfo = getImageInfoVk,
    .getImageMemoryRequirements = getImageMemoryRequirementsVk,
    .bindImageMemory = bindImageMemoryVk,

    // image view
    .createImageView = createImageViewVk,
    .destroyImageView = destroyImageViewVk,

    // sampler
    .createSampler = createSamplerVk,
    .destroySampler = destroySamplerVk,

    // swapchain
    .querySwapchainCapabilities = querySwapchainCapabilitiesVk,
    .createSwapchain = createSwapchainVk,
    .destroySwapchain = destroySwapchainVk,
    .getSwapchainImage = getSwapchainImageVk,
    .getNextSwapchainImage = getNextSwapchainImageVk,
    .presentSwapchain = presentSwapchainVk,

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
    .cmdBindPipeline = cmdBindPipelineVk,
    .cmdSetViewport = cmdSetViewportVk,
    .cmdSetScissors = cmdSetScissorsVk,
    .cmdBindVertexBuffers = cmdBindVertexBuffersVk,
    .cmdBindIndexBuffer = cmdBindIndexBufferVk,
    .cmdDraw = cmdDrawVk,
    .cmdDrawIndirect = cmdDrawIndirectVk,
    .cmdDrawIndexedIndirectCount = cmdDrawIndirectCountVk,
    .cmdDrawIndexed = cmdDrawIndexedVk,
    .cmdDrawIndexedIndirect = cmdDrawIndexedIndirectVk,
    .cmdDrawIndexedIndirectCount = cmdDrawIndexedIndirectCountVk,
    .cmdMemoryBarrier = cmdMemoryBarrierVk,
    .cmdImageViewBarrier = cmdImageViewBarrierVk,
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
    .destroyPipeline = destroyPipelineVk};

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
    // adapter
    if (!backend->enumerateAdapters                     ||
        !backend->getAdapterInfo                        ||
        !backend->getAdapterCapabilities                ||
        !backend->getAdapterFeatures                    ||

        // device
        !backend->createDevice                          ||
        !backend->destroyDevice                         ||
        !backend->waitDevice                            ||

        // memory
        !backend->allocateMemory                        ||
        !backend->freeMemory                            ||
        !backend->mapMemory                             ||
        !backend->unmapMemory                           ||

        // extended adapter features
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

        // image
        !backend->createImage                           ||
        !backend->destroyImage                          ||
        !backend->getImageInfo                          ||
        !backend->getImageMemoryRequirements            ||
        !backend->bindImageMemory                       ||

        // image view
        !backend->createImageView                       ||
        !backend->destroyImageView                      ||

        // sampler
        !backend->createSampler                         ||
        !backend->destroySampler                        ||

        // swapchain
        !backend->querySwapchainCapabilities            ||
        !backend->createSwapchain                       ||
        !backend->destroySwapchain                      ||
        !backend->getSwapchainImage                     ||
        !backend->getNextSwapchainImage                 ||
        !backend->presentSwapchain                      ||

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
        !backend->cmdImageViewBarrier                   ||
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
        !backend->destroyPipeline) {
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

#ifdef _WIN32
    // vulkan and d3d12
#elif defined(__linux__)
    // vulkan
#if PAL_HAS_VULKAN
    initGraphicsVk(debugger, allocator);
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

            for (int j = 0; j < _count; j++) {
                adapters[j]->backend = backend->base;
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

PalResult PAL_CALL palWaitDevice(PalDevice* device)
{
    if (!s_Graphics.initialized) {
        return PAL_RESULT_GRAPHICS_NOT_INITIALIZED;
    }

    if (!device) {
        return PAL_RESULT_NULL_POINTER;
    }

    return device->backend->waitDevice(device);
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
    PalGraphicsWindow* window)
{
    if (s_Graphics.initialized && queue) {
        return queue->backend->canQueuePresent(queue, window);
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

    return device->backend->querySwapchainCapabilities(device, window, caps);
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

    PalResult result;
    PalSwapchain* swapchain = nullptr;
    result = device->backend->createSwapchain(device, queue, window, info, &swapchain);
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
    Uint64* value)
{
    if (!s_Graphics.initialized) {
        return PAL_RESULT_GRAPHICS_NOT_INITIALIZED;
    }

    if (!semaphore || !value) {
        return PAL_RESULT_NULL_POINTER;
    }

    return semaphore->backend->getSemaphoreValue(semaphore, value);
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
    Uint64 dstOffset,
    Uint64 srcOffset,
    Uint32 size)
{
    if (!s_Graphics.initialized) {
        return PAL_RESULT_GRAPHICS_NOT_INITIALIZED;
    }

    if (!cmdBuffer || !dst || !src) {
        return PAL_RESULT_NULL_POINTER;
    }

    return cmdBuffer->backend->cmdCopyBuffer(cmdBuffer, dst, src, dstOffset, srcOffset, size);
}

PalResult PAL_CALL palCmdBindPipeline(
    PalCommandBuffer* cmdBuffer,
    PalPipeline* pipeline)
{
    if (!s_Graphics.initialized) {
        return PAL_RESULT_GRAPHICS_NOT_INITIALIZED;
    }

    if (!cmdBuffer || !pipeline) {
        return PAL_RESULT_NULL_POINTER;
    }

    return cmdBuffer->backend->cmdBindPipeline(cmdBuffer, pipeline);
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

PalResult PAL_CALL palCmdImageViewBarrier(
    PalCommandBuffer* cmdBuffer,
    PalImageView* imageView,
    PalUsageStateInfo* oldUsageStateInfo,
    PalUsageStateInfo* newUsageStateInfo)
{
    if (!s_Graphics.initialized) {
        return PAL_RESULT_GRAPHICS_NOT_INITIALIZED;
    }

    if (!cmdBuffer || !imageView || !oldUsageStateInfo || !newUsageStateInfo) {
        return PAL_RESULT_NULL_POINTER;
    }

    return cmdBuffer->backend
        ->cmdImageViewBarrier(cmdBuffer, imageView, oldUsageStateInfo, newUsageStateInfo);
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
    Uint32 width,
    Uint32 height,
    Uint32 depth)
{
    if (!s_Graphics.initialized) {
        return PAL_RESULT_GRAPHICS_NOT_INITIALIZED;
    }

    if (!cmdBuffer) {
        return PAL_RESULT_NULL_POINTER;
    }

    return cmdBuffer->backend->cmdTraceRays(cmdBuffer, width, height, depth);
}

PalResult PAL_CALL palCmdTraceRaysIndirect(
    PalCommandBuffer* cmdBuffer,
    PalDeviceAddress bufferAddress)
{
    if (!s_Graphics.initialized) {
        return PAL_RESULT_GRAPHICS_NOT_INITIALIZED;
    }

    if (!cmdBuffer) {
        return PAL_RESULT_NULL_POINTER;
    }

    return cmdBuffer->backend->cmdTraceRaysIndirect(cmdBuffer, bufferAddress);
}

PalResult PAL_CALL palCmdBindDescriptorSet(
    PalCommandBuffer* cmdBuffer,
    PalPipeline* pipeline,
    PalPipelineLayout* layout,
    Uint32 setIndex,
    PalDescriptorSet* set)
{
    if (!s_Graphics.initialized) {
        return PAL_RESULT_GRAPHICS_NOT_INITIALIZED;
    }

    if (!cmdBuffer || !pipeline || !layout || !set) {
        return PAL_RESULT_NULL_POINTER;
    }

    return cmdBuffer->backend->cmdBindDescriptorSet(cmdBuffer, pipeline, layout, setIndex, set);
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

PalResult PAL_CALL palMapMemory(
    PalDevice* device,
    PalMemory* memory,
    Uint64 offset,
    Uint64 size,
    void** outPtr)
{
    if (!s_Graphics.initialized) {
        return PAL_RESULT_GRAPHICS_NOT_INITIALIZED;
    }

    if (!device || !memory || !outPtr) {
        return PAL_RESULT_NULL_POINTER;
    }

    return device->backend->mapMemory(device, memory, offset, size, outPtr);
}

void PAL_CALL palUnmapMemory(
    PalDevice* device,
    PalMemory* memory)
{
    if (!s_Graphics.initialized || !device || !memory) {
        return;
    }
    device->backend->unmapMemory(device, memory);
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
