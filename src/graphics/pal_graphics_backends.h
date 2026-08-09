
/**
    PAL - Prime Abstraction Layer
    Copyright (C) 2025
    Licensed under the Zlib license. See LICENSE file in root.
 */

#ifndef _PAL_GRAPHICS_BACKENDS_H
#define _PAL_GRAPHICS_BACKENDS_H

#include "pal2/pal_graphics.h"

// clang-format off
typedef struct {
    const PalGraphicsBackendVtable1* vtbl1;
    const PalGraphicsBackendVtable2* vtbl2;
} PalGraphicsVtable;

// ==================================================
// Vulkan
// ==================================================

#if PAL_HAS_VULKAN_BACKEND
PalBool PAL_CALL initGraphicsVk(
    const PalGraphicsDebugger* debugger, 
    const PalAllocator* allocator);

void PAL_CALL shutdownGraphicsVk();

PalResult PAL_CALL enumerateAdaptersVk(
    uint32_t* count,
    PalAdapter** outAdapters);

void PAL_CALL getAdapterInfoVk(
    PalAdapter* adapter,
    PalAdapterInfo* info);

void PAL_CALL getAdapterCapabilitiesVk(
    PalAdapter* adapter,
    PalAdapterCapabilities* caps);

PalAdapterFeatures PAL_CALL getAdapterFeaturesVk(PalAdapter* adapter);

uint32_t PAL_CALL getHighestSupportedShaderTargetVk(
    PalAdapter* adapter,
    PalShaderFormats shaderFormat);

PalResult PAL_CALL createDeviceVk(
    PalAdapter* adapter,
    PalAdapterFeatures features,
    PalDevice** outDevice);

void PAL_CALL destroyDeviceVk(PalDevice* device);

uint32_t PAL_CALL getDeviceLostReasonVk(PalDevice* device);

PalResult PAL_CALL allocateMemoryVk(
    PalDevice* device,
    PalMemoryType type,
    uint64_t memoryMask,
    uint64_t size,
    PalMemory** outMemory);

void PAL_CALL freeMemoryVk(PalMemory* memory);

void PAL_CALL querySamplerAnisotropyCapabilitiesVk(
    PalDevice* device,
    PalSamplerAnisotropyCapabilities* caps);

void PAL_CALL queryMultiViewCapabilitiesVk(
    PalDevice* device,
    PalMultiViewCapabilities* caps);

void PAL_CALL queryMultiViewportCapabilitiesVk(
    PalDevice* device,
    PalMultiViewportCapabilities* caps);

void PAL_CALL queryDepthStencilCapabilitiesVk(
    PalDevice* device,
    PalDepthStencilCapabilities* caps);

void PAL_CALL queryFragmentShadingRateCapabilitiesVk(
    PalDevice* device,
    PalFragmentShadingRateCapabilities* caps);

void PAL_CALL queryMeshShaderCapabilitiesVk(
    PalDevice* device,
    PalMeshShaderCapabilities* caps);

void PAL_CALL queryRayTracingCapabilitiesVk(
    PalDevice* device,
    PalRayTracingCapabilities* caps);

void PAL_CALL queryDescriptorIndexingCapabilitiesVk(
    PalDevice* device,
    PalDescriptorIndexingCapabilities* caps);

PalResult PAL_CALL createQueueVk(
    PalDevice* device,
    PalQueueType type,
    PalQueue** outQueue);

void PAL_CALL destroyQueueVk(PalQueue* queue);

PalBool PAL_CALL canQueuePresentVk(
    PalQueue* queue,
    PalSurface* surface);

PalBool PAL_CALL canQueueShareOwnershipVk(
    PalQueue* a,
    PalQueue* b);
   
PalBool PAL_CALL canQueueUseUsageStateVk(
    PalQueue* queue,
    PalUsageState state);

PalBool PAL_CALL canQueueUsePipelineStagesVk(
    PalQueue* queue,
    PalPipelineStages stages);

PalResult PAL_CALL waitQueueVk(PalQueue* queue);

void PAL_CALL enumerateFormatsVk(
    PalAdapter* adapter,
    uint32_t* count,
    PalFormatInfo* outFormats);

PalBool PAL_CALL isFormatSupportedVk(
    PalAdapter* adapter,
    PalFormat format);

PalImageUsages PAL_CALL queryFormatImageUsagesVk(
    PalAdapter* adapter,
    PalFormat format);

PalSampleCount PAL_CALL queryFormatSampleCountVk(
    PalAdapter* adapter,
    PalFormat format);

PalResult PAL_CALL createImageVk(
    PalDevice* device,
    const PalImageCreateInfo* info,
    PalImage** outImage);

void PAL_CALL destroyImageVk(PalImage* image);

void PAL_CALL getImageInfoVk(
    PalImage* image,
    PalImageInfo* info);

void PAL_CALL getImageMemoryRequirementsVk(
    PalImage* image,
    PalMemoryRequirements* requirements);

PalResult PAL_CALL bindImageMemoryVk(
    PalImage* image,
    PalMemory* memory,
    uint64_t offset);

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

PalResult PAL_CALL createSurfaceVk(
    PalDevice* device,
    void* window,
    void* windowInstance,
    PalWindowInstanceType instanceType,
    PalSurface** outSurface);

void PAL_CALL destroySurfaceVk(PalSurface* surface);

void PAL_CALL getSurfaceCapabilitiesVk(
    PalDevice* device,
    PalSurface* surface,
    PalSurfaceCapabilities* caps);

PalResult PAL_CALL createSwapchainVk(
    PalDevice* device,
    PalQueue* queue,
    PalSurface* surface,
    const PalSwapchainCreateInfo* info,
    PalSwapchain** outSwapchain);

void PAL_CALL destroySwapchainVk(PalSwapchain* swapchain);

PalImage* PAL_CALL getSwapchainImageVk(
    PalSwapchain* swapchain,
    uint32_t index);

PalResult PAL_CALL getNextSwapchainImageVk(
    PalSwapchain* swapchain,
    PalSwapchainNextImageInfo* info,
    uint32_t* outIndex);

PalResult PAL_CALL presentSwapchainVk(
    PalSwapchain* swapchain,
    uint32_t imageIndex,
    PalSemaphore* waitSemaphore);

PalResult PAL_CALL resizeSwapchainVk(
    PalSwapchain* swapchain,
    uint32_t newWidth,
    uint32_t newHeight);

PalResult PAL_CALL createShaderVk(
    PalDevice* device,
    const PalShaderCreateInfo* info,
    PalShader** outShader);

void PAL_CALL destroyShaderVk(PalShader* shader);

PalResult PAL_CALL createFenceVk(
    PalDevice* device,
    PalBool signaled,
    PalFence** outFence);

void PAL_CALL destroyFenceVk(PalFence* fence);

PalResult PAL_CALL waitFenceVk(
    PalFence* fence,
    uint64_t timeout);

PalResult PAL_CALL resetFenceVk(PalFence* fence);

PalBool PAL_CALL isFenceSignaledVk(PalFence* fence);

PalResult PAL_CALL createSemaphoreVk(
    PalDevice* device,
    PalBool enableTimeline,
    PalSemaphore** outSemaphore);

void PAL_CALL destroySemaphoreVk(PalSemaphore* semaphore);

PalResult PAL_CALL waitSemaphoreVk(
    PalSemaphore* semaphore,
    uint64_t value,
    uint64_t timeout);

PalResult PAL_CALL signalSemaphoreVk(
    PalSemaphore* semaphore,
    PalQueue* queue,
    uint64_t value);

PalResult PAL_CALL getSemaphoreValueVk(
    PalSemaphore* semaphore, 
    uint64_t* value);

PalResult PAL_CALL createCommandPoolVk(
    PalDevice* device,
    PalQueue* queue,
    PalCommandPool** outPool);

void PAL_CALL destroyCommandPoolVk(PalCommandPool* pool);

PalResult PAL_CALL allocateCommandBufferVk(
    PalDevice* device,
    PalCommandPool* pool,
    PalCommandBufferType type,
    PalCommandBuffer** outCmdBuffer);

void PAL_CALL freeCommandBufferVk(PalCommandBuffer* cmdBuffer);

PalResult PAL_CALL resetCommandBufferVk(PalCommandBuffer* cmdBuffer);

PalResult PAL_CALL submitCommandBufferVk(
    PalQueue* queue,
    PalCommandBufferSubmitInfo* info);

PalResult PAL_CALL cmdBeginVk(
    PalCommandBuffer* cmdBuffer,
    PalRenderingLayoutInfo* info);

PalResult PAL_CALL cmdEndVk(PalCommandBuffer* cmdBuffer);

void PAL_CALL cmdExecuteCommandBufferVk(
    PalCommandBuffer* primaryCmdBuffer,
    PalCommandBuffer* secondaryCmdBuffer);

void PAL_CALL cmdSetFragmentShadingRateVk(
    PalCommandBuffer* cmdBuffer,
    PalFragmentShadingRateState* state);

void PAL_CALL cmdDrawMeshTasksVk(
    PalCommandBuffer* cmdBuffer,
    uint32_t groupCountX,
    uint32_t groupCountY,
    uint32_t groupCountZ);

void PAL_CALL cmdDrawMeshTasksIndirectVk(
    PalCommandBuffer* cmdBuffer,
    PalBuffer* buffer,
    uint32_t drawCount);

void PAL_CALL cmdDrawMeshTasksIndirectCountVk(
    PalCommandBuffer* cmdBuffer,
    PalBuffer* buffer,
    PalBuffer* countBuffer,
    uint32_t maxDrawCount);

void PAL_CALL cmdBuildAccelerationStructureVk(
    PalCommandBuffer* cmdBuffer,
    PalAccelerationStructureBuildInfo* info);

void PAL_CALL cmdBeginRenderingVk(
    PalCommandBuffer* cmdBuffer,
    PalRenderingInfo* info);

void PAL_CALL cmdEndRenderingVk(PalCommandBuffer* cmdBuffer);

void PAL_CALL cmdCopyBufferVk(
    PalCommandBuffer* cmdBuffer,
    PalBuffer* dst,
    PalBuffer* src,
    PalBufferCopyInfo* copyInfo);

void PAL_CALL cmdCopyBufferToImageVk(
    PalCommandBuffer* cmdBuffer,
    PalImage* dstImage,
    PalBuffer* srcBuffer,
    PalBufferImageCopyInfo* copyInfo);

void PAL_CALL cmdCopyImageVk(
    PalCommandBuffer* cmdBuffer,
    PalImage* dst,
    PalImage* src,
    PalImageCopyInfo* copyInfo);

void PAL_CALL cmdCopyImageToBufferVk(
    PalCommandBuffer* cmdBuffer,
    PalBuffer* dstBuffer,
    PalImage* srcImage,
    PalBufferImageCopyInfo* copyInfo);

void PAL_CALL cmdBindPipelineVk(
    PalCommandBuffer* cmdBuffer,
    PalPipeline* pipeline);

void PAL_CALL cmdSetViewportVk(
    PalCommandBuffer* cmdBuffer,
    uint32_t count,
    PalViewport* viewports);

void PAL_CALL cmdSetScissorsVk(
    PalCommandBuffer* cmdBuffer,
    uint32_t count,
    PalRect2D* scissors);

void PAL_CALL cmdBindVertexBuffersVk(
    PalCommandBuffer* cmdBuffer,
    uint32_t firstSlot,
    uint32_t count,
    PalBuffer** buffers,
    uint64_t* offsets);

void PAL_CALL cmdBindIndexBufferVk(
    PalCommandBuffer* cmdBuffer,
    PalBuffer* buffer,
    uint64_t offset,
    PalIndexType type);

void PAL_CALL cmdDrawVk(
    PalCommandBuffer* cmdBuffer,
    uint32_t vertexCount,
    uint32_t instanceCount,
    uint32_t firstVertex,
    uint32_t firstInstance);

void PAL_CALL cmdDrawIndirectVk(
    PalCommandBuffer* cmdBuffer,
    PalBuffer* buffer,
    uint32_t count);

void PAL_CALL cmdDrawIndirectCountVk(
    PalCommandBuffer* cmdBuffer,
    PalBuffer* buffer,
    PalBuffer* countBuffer,
    uint32_t count);

void PAL_CALL cmdDrawIndexedVk(
    PalCommandBuffer* cmdBuffer,
    uint32_t indexCount,
    uint32_t instanceCount,
    uint32_t firstIndex,
    int32_t vertexOffset,
    uint32_t firstInstance);

void PAL_CALL cmdDrawIndexedIndirectVk(
    PalCommandBuffer* cmdBuffer,
    PalBuffer* buffer,
    uint32_t count);

void PAL_CALL cmdDrawIndexedIndirectCountVk(
    PalCommandBuffer* cmdBuffer,
    PalBuffer* buffer,
    PalBuffer* countBuffer,
    uint32_t count);

void PAL_CALL cmdAccelerationStructureBarrierVk(
    PalCommandBuffer* cmdBuffer,
    PalAccelerationStructure* as,
    PalBarrierInfo* info);

void PAL_CALL cmdImageBarrierVk(
    PalCommandBuffer* cmdBuffer,
    PalImage* image,
    PalImageSubresourceRange* subresourceRange,
    PalBarrierInfo* info);

void PAL_CALL cmdImageBarrier2Vk(
    PalCommandBuffer* cmdBuffer,
    PalImage* image,
    PalImageSubresourceRange* subresourceRange,
    PalBarrierInfo2* info);

void PAL_CALL cmdBufferBarrierVk(
    PalCommandBuffer* cmdBuffer,
    PalBuffer* buffer,
    PalBarrierInfo* info);

void PAL_CALL cmdBufferBarrier2Vk(
    PalCommandBuffer* cmdBuffer,
    PalBuffer* buffer,
    PalBarrierInfo2* info);

void PAL_CALL cmdDispatchVk(
    PalCommandBuffer* cmdBuffer,
    uint32_t groupCountX,
    uint32_t groupCountY,
    uint32_t groupCountZ);

void PAL_CALL cmdDispatchBaseVk(
    PalCommandBuffer* cmdBuffer,
    uint32_t baseGroupX,
    uint32_t baseGroupY,
    uint32_t baseGroupZ,
    uint32_t groupCountX,
    uint32_t groupCountY,
    uint32_t groupCountZ);

void PAL_CALL cmdDispatchIndirectVk(
    PalCommandBuffer* cmdBuffer,
    PalBuffer* buffer);

void PAL_CALL cmdTraceRaysVk(
    PalCommandBuffer* cmdBuffer,
    PalShaderBindingTable* sbt,
    uint32_t raygenIndex,
    uint32_t width,
    uint32_t height,
    uint32_t depth);

void PAL_CALL cmdTraceRaysIndirectVk(
    PalCommandBuffer* cmdBuffer,
    uint32_t raygenIndex,
    PalShaderBindingTable* sbt,
    PalBuffer* buffer);

void PAL_CALL cmdBindDescriptorSetVk(
    PalCommandBuffer* cmdBuffer,
    uint32_t setIndex,
    PalDescriptorSet* set);

void PAL_CALL cmdPushConstantsVk(
    PalCommandBuffer* cmdBuffer,
    uint32_t offset,
    uint32_t size,
    const void* value);

void PAL_CALL cmdSetCullModeVk(
    PalCommandBuffer* cmdBuffer,
    PalCullMode cullMode);

void PAL_CALL cmdSetFrontFaceVk(
    PalCommandBuffer* cmdBuffer,
    PalFrontFace frontFace);

void PAL_CALL cmdSetPrimitiveTopologyVk(
    PalCommandBuffer* cmdBuffer,
    PalPrimitiveTopology topology);

void PAL_CALL cmdSetDepthTestEnableVk(
    PalCommandBuffer* cmdBuffer,
    PalBool enable);

void PAL_CALL cmdSetDepthWriteEnableVk(
    PalCommandBuffer* cmdBuffer,
    PalBool enable);

void PAL_CALL cmdSetStencilOpVk(
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

void PAL_CALL getAccelerationStructureBuildSizeVk(
    PalDevice* device,
    PalAccelerationStructureBuildInfo* info,
    PalAccelerationStructureBuildSize* size);

PalResult PAL_CALL createBufferVk(
    PalDevice* device,
    const PalBufferCreateInfo* info,
    PalBuffer** outBuffer);

void PAL_CALL destroyBufferVk(PalBuffer* buffer);

void PAL_CALL getBufferMemoryRequirementsVk(
    PalBuffer* buffer,
    PalMemoryRequirements* requirements);

void PAL_CALL computeInstanceStagingSizeVk(
    PalDevice* device,
    uint32_t instanceCount, 
    uint64_t* outSize);

void PAL_CALL computeImageStagingRequirementsVk(
    PalDevice* device,
    PalFormat imageFormat,
    const PalBufferImageCopyInfo* copyInfo,
    PalImageStagingRequirements* requirements);

void PAL_CALL writeInstanceStagingVk(
    PalDevice* device,
    uint32_t instanceCount,
    PalAccelerationStructureInstance* instances,
    void* ptr);

void PAL_CALL writeImageStagingVk(
    PalDevice* device,
    PalFormat imageFormat,
    PalBufferImageCopyInfo* copyInfo,
    void* srcData,
    void* ptr);

PalResult PAL_CALL bindBufferMemoryVk(
    PalBuffer* buffer,
    PalMemory* memory,
    uint64_t offset);

PalResult PAL_CALL mapBufferVk(
    PalBuffer* buffer,
    uint64_t offset,
    uint64_t size,
    void** outPtr);

void PAL_CALL unmapBufferVk(PalBuffer* buffer);

PalDeviceAddress PAL_CALL getBufferDeviceAddressVk(PalBuffer* buffer);

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

PalResult PAL_CALL updateDescriptorSetVk(
    PalDevice* device,
    uint32_t count,
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

PalResult PAL_CALL createShaderBindingTableVk(
    PalDevice* device,
    const PalShaderBindingTableCreateInfo* info,
    PalShaderBindingTable** outSbt);

void PAL_CALL destroyShaderBindingTableVk(PalShaderBindingTable* sbt);

void PAL_CALL updateShaderBindingTableVk(
    PalShaderBindingTable* sbt,
    uint32_t count,
    PalShaderBindingTableRecordInfo* infos);

static const PalGraphicsBackendVtable1 s_VkBackend1 = {
    .enumerateAdapters = enumerateAdaptersVk,
    .getAdapterInfo = getAdapterInfoVk,
    .getAdapterCapabilities = getAdapterCapabilitiesVk,
    .getAdapterFeatures = getAdapterFeaturesVk,
    .getHighestSupportedShaderTarget = getHighestSupportedShaderTargetVk,
    .createDevice = createDeviceVk,
    .destroyDevice = destroyDeviceVk,
    .getDeviceLostReason = getDeviceLostReasonVk,
    .allocateMemory = allocateMemoryVk,
    .freeMemory = freeMemoryVk,
    .querySamplerAnisotropyCapabilities = querySamplerAnisotropyCapabilitiesVk,
    .queryMultiViewCapabilities = queryMultiViewCapabilitiesVk,
    .queryMultiViewportCapabilities = queryMultiViewportCapabilitiesVk,
    .queryDepthStencilCapabilities = queryDepthStencilCapabilitiesVk,
    .queryFragmentShadingRateCapabilities = queryFragmentShadingRateCapabilitiesVk,
    .queryMeshShaderCapabilities = queryMeshShaderCapabilitiesVk,
    .queryRayTracingCapabilities = queryRayTracingCapabilitiesVk,
    .queryDescriptorIndexingCapabilities = queryDescriptorIndexingCapabilitiesVk,
    .createQueue = createQueueVk,
    .destroyQueue = destroyQueueVk,
    .waitQueue = waitQueueVk,
    .canQueuePresent = canQueuePresentVk,
    .enumerateFormats = enumerateFormatsVk,
    .isFormatSupported = isFormatSupportedVk,
    .queryFormatImageUsages = queryFormatImageUsagesVk,
    .queryFormatSampleCount = queryFormatSampleCountVk,
    .createImage = createImageVk,
    .destroyImage = destroyImageVk,
    .getImageInfo = getImageInfoVk,
    .getImageMemoryRequirements = getImageMemoryRequirementsVk,
    .bindImageMemory = bindImageMemoryVk,
    .createImageView = createImageViewVk,
    .destroyImageView = destroyImageViewVk,
    .createSampler = createSamplerVk,
    .destroySampler = destroySamplerVk,
    .createSurface = createSurfaceVk,
    .destroySurface = destroySurfaceVk,
    .getSurfaceCapabilities = getSurfaceCapabilitiesVk,
    .createSwapchain = createSwapchainVk,
    .destroySwapchain = destroySwapchainVk,
    .getSwapchainImage = getSwapchainImageVk,
    .getNextSwapchainImage = getNextSwapchainImageVk,
    .presentSwapchain = presentSwapchainVk,
    .resizeSwapchain = resizeSwapchainVk,
    .createShader = createShaderVk,
    .destroyShader = destroyShaderVk,
    .createFence = createFenceVk,
    .destroyFence = destroyFenceVk,
    .waitFence = waitFenceVk,
    .resetFence = resetFenceVk,
    .isFenceSignaled = isFenceSignaledVk,
    .createSemaphore = createSemaphoreVk,
    .destroySemaphore = destroySemaphoreVk,
    .waitSemaphore = waitSemaphoreVk,
    .signalSemaphore = signalSemaphoreVk,
    .getSemaphoreValue = getSemaphoreValueVk,
    .createCommandPool = createCommandPoolVk,
    .destroyCommandPool = destroyCommandPoolVk,
    .allocateCommandBuffer = allocateCommandBufferVk,
    .freeCommandBuffer = freeCommandBufferVk,
    .resetCommandBuffer = resetCommandBufferVk,
    .submitCommandBuffer = submitCommandBufferVk,
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
    .cmdAccelerationStructureBarrier = cmdAccelerationStructureBarrierVk,
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
    .createAccelerationstructure = createAccelerationstructureVk,
    .destroyAccelerationstructure = destroyAccelerationstructureVk,
    .getAccelerationStructureBuildSize = getAccelerationStructureBuildSizeVk,
    .createBuffer = createBufferVk,
    .destroyBuffer = destroyBufferVk,
    .getBufferMemoryRequirements = getBufferMemoryRequirementsVk,
    .computeInstanceStagingSize = computeInstanceStagingSizeVk,
    .computeImageStagingRequirements = computeImageStagingRequirementsVk,
    .writeInstanceStaging = writeInstanceStagingVk,
    .writeImageStaging = writeImageStagingVk,
    .bindBufferMemory = bindBufferMemoryVk,
    .getBufferDeviceAddress = getBufferDeviceAddressVk,
    .mapBuffer = mapBufferVk,
    .unmapBuffer = unmapBufferVk,
    .createDescriptorSetLayout = createDescriptorSetLayoutVk,
    .destroyDescriptorSetLayout = destroyDescriptorSetLayoutVk,
    .createDescriptorPool = createDescriptorPoolVk,
    .destroyDescriptorPool = destroyDescriptorPoolVk,
    .resetDescriptorPool = resetDescriptorPoolVk,
    .allocateDescriptorSet = allocateDescriptorSetVk,
    .updateDescriptorSet = updateDescriptorSetVk,
    .createPipelineLayout = createPipelineLayoutVk,
    .destroyPipelineLayout = destroyPipelineLayoutVk,
    .createGraphicsPipeline = createGraphicsPipelineVk,
    .createComputePipeline = createComputePipelineVk,
    .createRayTracingPipeline = createRayTracingPipelineVk,
    .destroyPipeline = destroyPipelineVk,
    .createShaderBindingTable = createShaderBindingTableVk,
    .destroyShaderBindingTable = destroyShaderBindingTableVk,
    .updateShaderBindingTable = updateShaderBindingTableVk};

static const PalGraphicsBackendVtable2 s_VkBackend2 = {
    .vtable1 = s_VkBackend1,
    .canQueueShareOwnership = canQueueShareOwnershipVk,
    .canQueueUseUsageState = canQueueUseUsageStateVk,
    .canQueueUsePipelineStages = canQueueUsePipelineStagesVk,
    .cmdImageBarrier2 = cmdImageBarrier2Vk,
    .cmdBufferBarrier2 = cmdBufferBarrier2Vk};

#endif // PAL_HAS_VULKAN_BACKEND

// ==================================================
// D3D12
// ==================================================

#if PAL_HAS_D3D12_BACKEND
PalBool PAL_CALL initGraphicsD3D12(
    const PalGraphicsDebugger* debugger, 
    const PalAllocator* allocator);

void PAL_CALL shutdownGraphicsD3D12();

PalResult PAL_CALL enumerateAdaptersD3D12(
    uint32_t* count,
    PalAdapter** outAdapters);

void PAL_CALL getAdapterInfoD3D12(
    PalAdapter* adapter,
    PalAdapterInfo* info);

void PAL_CALL getAdapterCapabilitiesD3D12(
    PalAdapter* adapter,
    PalAdapterCapabilities* caps);

PalAdapterFeatures PAL_CALL getAdapterFeaturesD3D12(PalAdapter* adapter);

uint32_t PAL_CALL getHighestSupportedShaderTargetD3D12(
    PalAdapter* adapter,
    PalShaderFormats shaderFormat);

PalResult PAL_CALL createDeviceD3D12(
    PalAdapter* adapter,
    PalAdapterFeatures features,
    PalDevice** outDevice);

void PAL_CALL destroyDeviceD3D12(PalDevice* device);

uint32_t PAL_CALL getDeviceLostReasonD3D12(PalDevice* device);

PalResult PAL_CALL allocateMemoryD3D12(
    PalDevice* device,
    PalMemoryType type,
    uint64_t memoryMask,
    uint64_t size,
    PalMemory** outMemory);

void PAL_CALL freeMemoryD3D12(PalMemory* memory);

void PAL_CALL querySamplerAnisotropyCapabilitiesD3D12(
    PalDevice* device,
    PalSamplerAnisotropyCapabilities* caps);

void PAL_CALL queryMultiViewCapabilitiesD3D12(
    PalDevice* device,
    PalMultiViewCapabilities* caps);

void PAL_CALL queryMultiViewportCapabilitiesD3D12(
    PalDevice* device,
    PalMultiViewportCapabilities* caps);

void PAL_CALL queryDepthStencilCapabilitiesD3D12(
    PalDevice* device,
    PalDepthStencilCapabilities* caps);

void PAL_CALL queryFragmentShadingRateCapabilitiesD3D12(
    PalDevice* device,
    PalFragmentShadingRateCapabilities* caps);

void PAL_CALL queryMeshShaderCapabilitiesD3D12(
    PalDevice* device,
    PalMeshShaderCapabilities* caps);

void PAL_CALL queryRayTracingCapabilitiesD3D12(
    PalDevice* device,
    PalRayTracingCapabilities* caps);

void PAL_CALL queryDescriptorIndexingCapabilitiesD3D12(
    PalDevice* device,
    PalDescriptorIndexingCapabilities* caps);

PalResult PAL_CALL createQueueD3D12(
    PalDevice* device,
    PalQueueType type,
    PalQueue** outQueue);

void PAL_CALL destroyQueueD3D12(PalQueue* queue);

PalBool PAL_CALL canQueuePresentD3D12(
    PalQueue* queue,
    PalSurface* surface);

PalBool PAL_CALL canQueueShareOwnershipD3D12(
    PalQueue* a,
    PalQueue* b);
   
PalBool PAL_CALL canQueueUseUsageStateD3D12(
    PalQueue* queue,
    PalUsageState state);

PalBool PAL_CALL canQueueUsePipelineStagesD3D12(
    PalQueue* queue,
    PalPipelineStages stages);

PalResult PAL_CALL waitQueueD3D12(PalQueue* queue);

void PAL_CALL enumerateFormatsD3D12(
    PalAdapter* adapter,
    uint32_t* count,
    PalFormatInfo* outFormats);

PalBool PAL_CALL isFormatSupportedD3D12(
    PalAdapter* adapter,
    PalFormat format);

PalImageUsages PAL_CALL queryFormatImageUsagesD3D12(
    PalAdapter* adapter,
    PalFormat format);

PalSampleCount PAL_CALL queryFormatSampleCountD3D12(
    PalAdapter* adapter,
    PalFormat format);

PalResult PAL_CALL createImageD3D12(
    PalDevice* device,
    const PalImageCreateInfo* info,
    PalImage** outImage);

void PAL_CALL destroyImageD3D12(PalImage* image);

void PAL_CALL getImageInfoD3D12(
    PalImage* image,
    PalImageInfo* info);

void PAL_CALL getImageMemoryRequirementsD3D12(
    PalImage* image,
    PalMemoryRequirements* requirements);

PalResult PAL_CALL bindImageMemoryD3D12(
    PalImage* image,
    PalMemory* memory,
    uint64_t offset);

PalResult PAL_CALL createImageViewD3D12(
    PalDevice* device,
    PalImage* image,
    const PalImageViewCreateInfo* info,
    PalImageView** outImageView);

void PAL_CALL destroyImageViewD3D12(PalImageView* imageView);

PalResult PAL_CALL createSamplerD3D12(
    PalDevice* device,
    const PalSamplerCreateInfo* info,
    PalSampler** outSampler);

void PAL_CALL destroySamplerD3D12(PalSampler* sampler);

PalResult PAL_CALL createSurfaceD3D12(
    PalDevice* device,
    void* window,
    void* windowInstance,
    PalWindowInstanceType instanceType,
    PalSurface** outSurface);

void PAL_CALL destroySurfaceD3D12(PalSurface* surface);

void PAL_CALL getSurfaceCapabilitiesD3D12(
    PalDevice* device,
    PalSurface* surface,
    PalSurfaceCapabilities* caps);

PalResult PAL_CALL createSwapchainD3D12(
    PalDevice* device,
    PalQueue* queue,
    PalSurface* surface,
    const PalSwapchainCreateInfo* info,
    PalSwapchain** outSwapchain);

void PAL_CALL destroySwapchainD3D12(PalSwapchain* swapchain);

PalImage* PAL_CALL getSwapchainImageD3D12(
    PalSwapchain* swapchain,
    uint32_t index);

PalResult PAL_CALL getNextSwapchainImageD3D12(
    PalSwapchain* swapchain,
    PalSwapchainNextImageInfo* info,
    uint32_t* outIndex);

PalResult PAL_CALL presentSwapchainD3D12(
    PalSwapchain* swapchain,
    uint32_t imageIndex,
    PalSemaphore* waitSemaphore);

PalResult PAL_CALL resizeSwapchainD3D12(
    PalSwapchain* swapchain,
    uint32_t newWidth,
    uint32_t newHeight);

PalResult PAL_CALL createShaderD3D12(
    PalDevice* device,
    const PalShaderCreateInfo* info,
    PalShader** outShader);

void PAL_CALL destroyShaderD3D12(PalShader* shader);

PalResult PAL_CALL createFenceD3D12(
    PalDevice* device,
    PalBool signaled,
    PalFence** outFence);

void PAL_CALL destroyFenceD3D12(PalFence* fence);

PalResult PAL_CALL waitFenceD3D12(
    PalFence* fence,
    uint64_t timeout);

PalResult PAL_CALL resetFenceD3D12(PalFence* fence);

PalBool PAL_CALL isFenceSignaledD3D12(PalFence* fence);

PalResult PAL_CALL createSemaphoreD3D12(
    PalDevice* device,
    PalBool enableTimeline,
    PalSemaphore** outSemaphore);

void PAL_CALL destroySemaphoreD3D12(PalSemaphore* semaphore);

PalResult PAL_CALL waitSemaphoreD3D12(
    PalSemaphore* semaphore,
    uint64_t value,
    uint64_t timeout);

PalResult PAL_CALL signalSemaphoreD3D12(
    PalSemaphore* semaphore,
    PalQueue* queue,
    uint64_t value);

PalResult PAL_CALL getSemaphoreValueD3D12(
    PalSemaphore* semaphore, 
    uint64_t* value);

PalResult PAL_CALL createCommandPoolD3D12(
    PalDevice* device,
    PalQueue* queue,
    PalCommandPool** outPool);

void PAL_CALL destroyCommandPoolD3D12(PalCommandPool* pool);

PalResult PAL_CALL allocateCommandBufferD3D12(
    PalDevice* device,
    PalCommandPool* pool,
    PalCommandBufferType type,
    PalCommandBuffer** outCmdBuffer);

void PAL_CALL freeCommandBufferD3D12(PalCommandBuffer* cmdBuffer);

PalResult PAL_CALL resetCommandBufferD3D12(PalCommandBuffer* cmdBuffer);

PalResult PAL_CALL submitCommandBufferD3D12(
    PalQueue* queue,
    PalCommandBufferSubmitInfo* info);

PalResult PAL_CALL cmdBeginD3D12(
    PalCommandBuffer* cmdBuffer,
    PalRenderingLayoutInfo* info);

PalResult PAL_CALL cmdEndD3D12(PalCommandBuffer* cmdBuffer);

void PAL_CALL cmdExecuteCommandBufferD3D12(
    PalCommandBuffer* primaryCmdBuffer,
    PalCommandBuffer* secondaryCmdBuffer);

void PAL_CALL cmdSetFragmentShadingRateD3D12(
    PalCommandBuffer* cmdBuffer,
    PalFragmentShadingRateState* state);

void PAL_CALL cmdDrawMeshTasksD3D12(
    PalCommandBuffer* cmdBuffer,
    uint32_t groupCountX,
    uint32_t groupCountY,
    uint32_t groupCountZ);

void PAL_CALL cmdDrawMeshTasksIndirectD3D12(
    PalCommandBuffer* cmdBuffer,
    PalBuffer* buffer,
    uint32_t drawCount);

void PAL_CALL cmdDrawMeshTasksIndirectCountD3D12(
    PalCommandBuffer* cmdBuffer,
    PalBuffer* buffer,
    PalBuffer* countBuffer,
    uint32_t maxDrawCount);

void PAL_CALL cmdBuildAccelerationStructureD3D12(
    PalCommandBuffer* cmdBuffer,
    PalAccelerationStructureBuildInfo* info);

void PAL_CALL cmdBeginRenderingD3D12(
    PalCommandBuffer* cmdBuffer,
    PalRenderingInfo* info);

void PAL_CALL cmdEndRenderingD3D12(PalCommandBuffer* cmdBuffer);

void PAL_CALL cmdCopyBufferD3D12(
    PalCommandBuffer* cmdBuffer,
    PalBuffer* dst,
    PalBuffer* src,
    PalBufferCopyInfo* copyInfo);

void PAL_CALL cmdCopyBufferToImageD3D12(
    PalCommandBuffer* cmdBuffer,
    PalImage* dstImage,
    PalBuffer* srcBuffer,
    PalBufferImageCopyInfo* copyInfo);

void PAL_CALL cmdCopyImageD3D12(
    PalCommandBuffer* cmdBuffer,
    PalImage* dst,
    PalImage* src,
    PalImageCopyInfo* copyInfo);

void PAL_CALL cmdCopyImageToBufferD3D12(
    PalCommandBuffer* cmdBuffer,
    PalBuffer* dstBuffer,
    PalImage* srcImage,
    PalBufferImageCopyInfo* copyInfo);

void PAL_CALL cmdBindPipelineD3D12(
    PalCommandBuffer* cmdBuffer,
    PalPipeline* pipeline);

void PAL_CALL cmdSetViewportD3D12(
    PalCommandBuffer* cmdBuffer,
    uint32_t count,
    PalViewport* viewports);

void PAL_CALL cmdSetScissorsD3D12(
    PalCommandBuffer* cmdBuffer,
    uint32_t count,
    PalRect2D* scissors);

void PAL_CALL cmdBindVertexBuffersD3D12(
    PalCommandBuffer* cmdBuffer,
    uint32_t firstSlot,
    uint32_t count,
    PalBuffer** buffers,
    uint64_t* offsets);

void PAL_CALL cmdBindIndexBufferD3D12(
    PalCommandBuffer* cmdBuffer,
    PalBuffer* buffer,
    uint64_t offset,
    PalIndexType type);

void PAL_CALL cmdDrawD3D12(
    PalCommandBuffer* cmdBuffer,
    uint32_t vertexCount,
    uint32_t instanceCount,
    uint32_t firstVertex,
    uint32_t firstInstance);

void PAL_CALL cmdDrawIndirectD3D12(
    PalCommandBuffer* cmdBuffer,
    PalBuffer* buffer,
    uint32_t count);

void PAL_CALL cmdDrawIndirectCountD3D12(
    PalCommandBuffer* cmdBuffer,
    PalBuffer* buffer,
    PalBuffer* countBuffer,
    uint32_t count);

void PAL_CALL cmdDrawIndexedD3D12(
    PalCommandBuffer* cmdBuffer,
    uint32_t indexCount,
    uint32_t instanceCount,
    uint32_t firstIndex,
    int32_t vertexOffset,
    uint32_t firstInstance);

void PAL_CALL cmdDrawIndexedIndirectD3D12(
    PalCommandBuffer* cmdBuffer,
    PalBuffer* buffer,
    uint32_t count);

void PAL_CALL cmdDrawIndexedIndirectCountD3D12(
    PalCommandBuffer* cmdBuffer,
    PalBuffer* buffer,
    PalBuffer* countBuffer,
    uint32_t count);

void PAL_CALL cmdAccelerationStructureBarrierD3D12(
    PalCommandBuffer* cmdBuffer,
    PalAccelerationStructure* as,
    PalBarrierInfo* info);

void PAL_CALL cmdImageBarrierD3D12(
    PalCommandBuffer* cmdBuffer,
    PalImage* image,
    PalImageSubresourceRange* subresourceRange,
    PalBarrierInfo* info);

void PAL_CALL cmdImageBarrier2D3D12(
    PalCommandBuffer* cmdBuffer,
    PalImage* image,
    PalImageSubresourceRange* subresourceRange,
    PalBarrierInfo2* info);

void PAL_CALL cmdBufferBarrierD3D12(
    PalCommandBuffer* cmdBuffer,
    PalBuffer* buffer,
    PalBarrierInfo* info);

void PAL_CALL cmdBufferBarrier2D3D12(
    PalCommandBuffer* cmdBuffer,
    PalBuffer* buffer,
    PalBarrierInfo2* info);

void PAL_CALL cmdDispatchD3D12(
    PalCommandBuffer* cmdBuffer,
    uint32_t groupCountX,
    uint32_t groupCountY,
    uint32_t groupCountZ);

void PAL_CALL cmdDispatchIndirectD3D12(
    PalCommandBuffer* cmdBuffer,
    PalBuffer* buffer);

void PAL_CALL cmdTraceRaysD3D12(
    PalCommandBuffer* cmdBuffer,
    PalShaderBindingTable* sbt,
    uint32_t raygenIndex,
    uint32_t width,
    uint32_t height,
    uint32_t depth);

void PAL_CALL cmdTraceRaysIndirectD3D12(
    PalCommandBuffer* cmdBuffer,
    uint32_t raygenIndex,
    PalShaderBindingTable* sbt,
    PalBuffer* buffer);

void PAL_CALL cmdBindDescriptorSetD3D12(
    PalCommandBuffer* cmdBuffer,
    uint32_t setIndex,
    PalDescriptorSet* set);

void PAL_CALL cmdPushConstantsD3D12(
    PalCommandBuffer* cmdBuffer,
    uint32_t offset,
    uint32_t size,
    const void* value);

PalResult PAL_CALL createAccelerationstructureD3D12(
    PalDevice* device,
    const PalAccelerationStructureCreateInfo* info,
    PalAccelerationStructure** outAs);

void PAL_CALL destroyAccelerationstructureD3D12(PalAccelerationStructure* as);

void PAL_CALL getAccelerationStructureBuildSizeD3D12(
    PalDevice* device,
    PalAccelerationStructureBuildInfo* info,
    PalAccelerationStructureBuildSize* size);

PalResult PAL_CALL createBufferD3D12(
    PalDevice* device,
    const PalBufferCreateInfo* info,
    PalBuffer** outBuffer);

void PAL_CALL destroyBufferD3D12(PalBuffer* buffer);

void PAL_CALL getBufferMemoryRequirementsD3D12(
    PalBuffer* buffer,
    PalMemoryRequirements* requirements);

void PAL_CALL computeInstanceStagingSizeD3D12(
    PalDevice* device,
    uint32_t instanceCount, 
    uint64_t* outSize);

void PAL_CALL computeImageStagingRequirementsD3D12(
    PalDevice* device,
    PalFormat imageFormat,
    const PalBufferImageCopyInfo* copyInfo,
    PalImageStagingRequirements* requirements);

void PAL_CALL writeInstanceStagingD3D12(
    PalDevice* device,
    uint32_t instanceCount,
    PalAccelerationStructureInstance* instances,
    void* ptr);

void PAL_CALL writeImageStagingD3D12(
    PalDevice* device,
    PalFormat imageFormat,
    PalBufferImageCopyInfo* copyInfo,
    void* srcData,
    void* ptr);

PalResult PAL_CALL bindBufferMemoryD3D12(
    PalBuffer* buffer,
    PalMemory* memory,
    uint64_t offset);

PalResult PAL_CALL mapBufferD3D12(
    PalBuffer* buffer,
    uint64_t offset,
    uint64_t size,
    void** outPtr);

void PAL_CALL unmapBufferD3D12(PalBuffer* buffer);

PalDeviceAddress PAL_CALL getBufferDeviceAddressD3D12(PalBuffer* buffer);

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

PalResult PAL_CALL updateDescriptorSetD3D12(
    PalDevice* device,
    uint32_t count,
    PalDescriptorSetWriteInfo* infos);

PalResult PAL_CALL createPipelineLayoutD3D12(
    PalDevice* device,
    const PalPipelineLayoutCreateInfo* info,
    PalPipelineLayout** outLayout);

void PAL_CALL destroyPipelineLayoutD3D12(PalPipelineLayout* layout);

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

PalResult PAL_CALL createShaderBindingTableD3D12(
    PalDevice* device,
    const PalShaderBindingTableCreateInfo* info,
    PalShaderBindingTable** outSbt);

void PAL_CALL destroyShaderBindingTableD3D12(PalShaderBindingTable* sbt);

void PAL_CALL updateShaderBindingTableD3D12(
    PalShaderBindingTable* sbt,
    uint32_t count,
    PalShaderBindingTableRecordInfo* infos);

static const PalGraphicsBackendVtable1 s_D3D12Backend1 = {
    .enumerateAdapters = enumerateAdaptersD3D12,
    .getAdapterInfo = getAdapterInfoD3D12,
    .getAdapterCapabilities = getAdapterCapabilitiesD3D12,
    .getAdapterFeatures = getAdapterFeaturesD3D12,
    .getHighestSupportedShaderTarget = getHighestSupportedShaderTargetD3D12,
    .createDevice = createDeviceD3D12,
    .destroyDevice = destroyDeviceD3D12,
    .getDeviceLostReason = getDeviceLostReasonD3D12,
    .allocateMemory = allocateMemoryD3D12,
    .freeMemory = freeMemoryD3D12,
    .querySamplerAnisotropyCapabilities = querySamplerAnisotropyCapabilitiesD3D12,
    .queryMultiViewCapabilities = queryMultiViewCapabilitiesD3D12,
    .queryMultiViewportCapabilities = queryMultiViewportCapabilitiesD3D12,
    .queryDepthStencilCapabilities = queryDepthStencilCapabilitiesD3D12,
    .queryFragmentShadingRateCapabilities = queryFragmentShadingRateCapabilitiesD3D12,
    .queryMeshShaderCapabilities = queryMeshShaderCapabilitiesD3D12,
    .queryRayTracingCapabilities = queryRayTracingCapabilitiesD3D12,
    .queryDescriptorIndexingCapabilities = queryDescriptorIndexingCapabilitiesD3D12,
    .createQueue = createQueueD3D12,
    .destroyQueue = destroyQueueD3D12,
    .waitQueue = waitQueueD3D12,
    .canQueuePresent = canQueuePresentD3D12,
    .enumerateFormats = enumerateFormatsD3D12,
    .isFormatSupported = isFormatSupportedD3D12,
    .queryFormatImageUsages = queryFormatImageUsagesD3D12,
    .queryFormatSampleCount = queryFormatSampleCountD3D12,
    .createImage = createImageD3D12,
    .destroyImage = destroyImageD3D12,
    .getImageInfo = getImageInfoD3D12,
    .getImageMemoryRequirements = getImageMemoryRequirementsD3D12,
    .bindImageMemory = bindImageMemoryD3D12,
    .createImageView = createImageViewD3D12,
    .destroyImageView = destroyImageViewD3D12,
    .createSampler = createSamplerD3D12,
    .destroySampler = destroySamplerD3D12,
    .createSurface = createSurfaceD3D12,
    .destroySurface = destroySurfaceD3D12,
    .getSurfaceCapabilities = getSurfaceCapabilitiesD3D12,
    .createSwapchain = createSwapchainD3D12,
    .destroySwapchain = destroySwapchainD3D12,
    .getSwapchainImage = getSwapchainImageD3D12,
    .getNextSwapchainImage = getNextSwapchainImageD3D12,
    .presentSwapchain = presentSwapchainD3D12,
    .resizeSwapchain = resizeSwapchainD3D12,
    .createShader = createShaderD3D12,
    .destroyShader = destroyShaderD3D12,
    .createFence = createFenceD3D12,
    .destroyFence = destroyFenceD3D12,
    .waitFence = waitFenceD3D12,
    .resetFence = resetFenceD3D12,
    .isFenceSignaled = isFenceSignaledD3D12,
    .createSemaphore = createSemaphoreD3D12,
    .destroySemaphore = destroySemaphoreD3D12,
    .waitSemaphore = waitSemaphoreD3D12,
    .signalSemaphore = signalSemaphoreD3D12,
    .getSemaphoreValue = getSemaphoreValueD3D12,
    .createCommandPool = createCommandPoolD3D12,
    .destroyCommandPool = destroyCommandPoolD3D12,
    .allocateCommandBuffer = allocateCommandBufferD3D12,
    .freeCommandBuffer = freeCommandBufferD3D12,
    .resetCommandBuffer = resetCommandBufferD3D12,
    .submitCommandBuffer = submitCommandBufferD3D12,
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
    .cmdAccelerationStructureBarrier = cmdAccelerationStructureBarrierD3D12,
    .cmdImageBarrier = cmdImageBarrierD3D12,
    .cmdBufferBarrier = cmdBufferBarrierD3D12,
    .cmdDispatch = cmdDispatchD3D12,
    .cmdDispatchBase = nullptr,
    .cmdDispatchIndirect = cmdDispatchIndirectD3D12,
    .cmdTraceRays = cmdTraceRaysD3D12,
    .cmdTraceRaysIndirect = cmdTraceRaysIndirectD3D12,
    .cmdBindDescriptorSet = cmdBindDescriptorSetD3D12,
    .cmdPushConstants = cmdPushConstantsD3D12,
    .cmdSetCullMode = nullptr,
    .cmdSetFrontFace = nullptr,
    .cmdSetPrimitiveTopology = nullptr,
    .cmdSetDepthTestEnable = nullptr,
    .cmdSetDepthWriteEnable = nullptr,
    .cmdSetStencilOp = nullptr,
    .createAccelerationstructure = createAccelerationstructureD3D12,
    .destroyAccelerationstructure = destroyAccelerationstructureD3D12,
    .getAccelerationStructureBuildSize = getAccelerationStructureBuildSizeD3D12,
    .createBuffer = createBufferD3D12,
    .destroyBuffer = destroyBufferD3D12,
    .getBufferMemoryRequirements = getBufferMemoryRequirementsD3D12,
    .computeInstanceStagingSize = computeInstanceStagingSizeD3D12,
    .computeImageStagingRequirements = computeImageStagingRequirementsD3D12,
    .writeInstanceStaging = writeInstanceStagingD3D12,
    .writeImageStaging = writeImageStagingD3D12,
    .bindBufferMemory = bindBufferMemoryD3D12,
    .getBufferDeviceAddress = getBufferDeviceAddressD3D12,
    .mapBuffer = mapBufferD3D12,
    .unmapBuffer = unmapBufferD3D12,
    .createDescriptorSetLayout = createDescriptorSetLayoutD3D12,
    .destroyDescriptorSetLayout = destroyDescriptorSetLayoutD3D12,
    .createDescriptorPool = createDescriptorPoolD3D12,
    .destroyDescriptorPool = destroyDescriptorPoolD3D12,
    .resetDescriptorPool = resetDescriptorPoolD3D12,
    .allocateDescriptorSet = allocateDescriptorSetD3D12,
    .updateDescriptorSet = updateDescriptorSetD3D12,
    .createPipelineLayout = createPipelineLayoutD3D12,
    .destroyPipelineLayout = destroyPipelineLayoutD3D12,
    .createGraphicsPipeline = createGraphicsPipelineD3D12,
    .createComputePipeline = createComputePipelineD3D12,
    .createRayTracingPipeline = createRayTracingPipelineD3D12,
    .destroyPipeline = destroyPipelineD3D12,
    .createShaderBindingTable = createShaderBindingTableD3D12,
    .destroyShaderBindingTable = destroyShaderBindingTableD3D12,
    .updateShaderBindingTable = updateShaderBindingTableD3D12};

static const PalGraphicsBackendVtable2 s_D3D12Backend2 = {
    .vtable1 = s_D3D12Backend1,
    .canQueueShareOwnership = canQueueShareOwnershipD3D12,
    .canQueueUseUsageState = canQueueUseUsageStateD3D12,
    .canQueueUsePipelineStages = canQueueUsePipelineStagesD3D12,
    .cmdImageBarrier2 = cmdImageBarrier2D3D12,
    .cmdBufferBarrier2 = cmdBufferBarrier2D3D12};

#endif // PAL_HAS_D3D12_BACKEND

// clang-format on
#endif // _PAL_GRAPHICS_BACKENDS_H