
/**
    PAL - Prime Abstraction Layer
    Copyright (C) 2025
    Licensed under the Zlib license. See LICENSE file in root.
 */

#ifndef _PAL_GRAPHICS_BACKENDS_H
#define _PAL_GRAPHICS_BACKENDS_H

#include "pal/pal_graphics.h"

// clang-format off
typedef struct {
    PalResult (PAL_CALL *enumerateAdapters)(int32_t*, PalAdapter**);
    PalResult (PAL_CALL *getAdapterInfo)(PalAdapter*, PalAdapterInfo*);
    PalResult (PAL_CALL *getAdapterCapabilities)(PalAdapter*, PalAdapterCapabilities*);
    PalAdapterFeatures (PAL_CALL *getAdapterFeatures)(PalAdapter*);
    uint32_t (PAL_CALL *getHighestSupportedShaderTarget)(PalAdapter*, PalShaderFormats);
    PalResult (PAL_CALL *createDevice)(PalAdapter*, PalAdapterFeatures, PalDevice**);
    void (PAL_CALL *destroyDevice)(PalDevice*);
    PalResult (PAL_CALL *allocateMemory)(PalDevice*, PalMemoryType, uint64_t, uint64_t, PalMemory**);
    void (PAL_CALL *freeMemory)(PalDevice*, PalMemory*);
    PalResult (PAL_CALL *querySamplerAnisotropyCapabilities)(PalDevice*, PalSamplerAnisotropyCapabilities*);
    PalResult (PAL_CALL *queryMultiViewCapabilities)(PalDevice*, PalMultiViewCapabilities*);
    PalResult (PAL_CALL *queryMultiViewportCapabilities)(PalDevice*, PalMultiViewportCapabilities*);
    PalResult (PAL_CALL *queryDepthStencilCapabilities)(PalDevice*, PalDepthStencilCapabilities*);
    PalResult (PAL_CALL *queryFragmentShadingRateCapabilities)(PalDevice*, PalFragmentShadingRateCapabilities*);
    PalResult (PAL_CALL *queryMeshShaderCapabilities)(PalDevice*, PalMeshShaderCapabilities*);
    PalResult (PAL_CALL *queryRayTracingCapabilities)(PalDevice*, PalRayTracingCapabilities*);
    PalResult (PAL_CALL *queryDescriptorIndexingCapabilities)(PalDevice*, PalDescriptorIndexingCapabilities*);

    PalResult (PAL_CALL *createQueue)(PalDevice*, PalQueueType, PalQueue**);
    void (PAL_CALL *destroyQueue)(PalQueue*);
    PalBool (PAL_CALL *canQueuePresent)(PalQueue*, PalSurface*);
    PalResult (PAL_CALL *waitQueue)(PalQueue*);
    PalResult (PAL_CALL *enumerateFormats)(PalAdapter*, int32_t*, PalFormatInfo*);
    PalBool (PAL_CALL *isFormatSupported)(PalAdapter*, PalFormat);
    PalImageUsages (PAL_CALL *queryFormatImageUsages)(PalAdapter*, PalFormat);
    PalSampleCount (PAL_CALL *queryFormatSampleCount)(PalAdapter*, PalFormat);
    PalResult (PAL_CALL *createImage)(PalDevice*, const PalImageCreateInfo*, PalImage**);
    void (PAL_CALL *destroyImage)(PalImage*);
    PalResult (PAL_CALL *getImageInfo)(PalImage*, PalImageInfo*);
    PalResult (PAL_CALL *getImageMemoryRequirements)(PalImage*, PalMemoryRequirements*);
    PalResult (PAL_CALL *bindImageMemory)(PalImage*, PalMemory*, uint64_t);
    PalResult (PAL_CALL *createImageView)(PalDevice*, PalImage*, const PalImageViewCreateInfo*, PalImageView**);
    void (PAL_CALL *destroyImageView)(PalImageView*);

    PalResult (PAL_CALL *createSampler)(PalDevice*, const PalSamplerCreateInfo*, PalSampler**);
    void (PAL_CALL *destroySampler)(PalSampler*);
    PalResult (PAL_CALL *createSurface)(PalDevice*, void*, void*, PalWindowInstanceType, PalSurface**);
    void (PAL_CALL *destroySurface)(PalSurface*);
    PalResult (PAL_CALL *getSurfaceCapabilities)(PalDevice*, PalSurface*, PalSurfaceCapabilities*);
    PalResult (PAL_CALL *createSwapchain)(PalDevice*, PalQueue*, PalSurface*, const PalSwapchainCreateInfo*, PalSwapchain**);
    void (PAL_CALL *destroySwapchain)(PalSwapchain*);
    PalImage* (PAL_CALL *getSwapchainImage)(PalSwapchain*, uint32_t);
    PalResult (PAL_CALL *getNextSwapchainImage)(PalSwapchain*, PalSwapchainNextImageInfo*, uint32_t*);
    PalResult (PAL_CALL *presentSwapchain)(PalSwapchain*, uint32_t, PalSemaphore*);
    PalResult (PAL_CALL *resizeSwapchain)(PalSwapchain*, uint32_t, uint32_t);
    PalResult (PAL_CALL *createShader)(PalDevice*, const PalShaderCreateInfo*, PalShader**);
    void (PAL_CALL *destroyShader)(PalShader*);
    PalResult (PAL_CALL *createFence)(PalDevice*, PalBool, PalFence**);
    void (PAL_CALL *destroyFence)(PalFence*);
    PalResult (PAL_CALL *waitFence)(PalFence*, uint64_t);
    PalResult (PAL_CALL *resetFence)(PalFence*);
    PalBool (PAL_CALL *isFenceSignaled)(PalFence*);

    PalResult (PAL_CALL *createSemaphore)(PalDevice*, PalBool, PalSemaphore**);
    void (PAL_CALL *destroySemaphore)(PalSemaphore*);
    PalResult (PAL_CALL *waitSemaphore)(PalSemaphore*, uint64_t, uint64_t);
    PalResult (PAL_CALL *signalSemaphore)(PalSemaphore*, PalQueue*, uint64_t);
    PalResult (PAL_CALL *getSemaphoreValue)(PalSemaphore*, uint64_t*);
    PalResult (PAL_CALL *createCommandPool)(PalDevice*, PalQueue*, PalCommandPool**);
    void (PAL_CALL *destroyCommandPool)(PalCommandPool*);
    PalResult (PAL_CALL *resetCommandPool)(PalCommandPool*);
    PalResult (PAL_CALL *allocateCommandBuffer)(PalDevice*, PalCommandPool*, PalCommandBufferType, PalCommandBuffer**);
    void (PAL_CALL *freeCommandBuffer)(PalCommandBuffer*);
    PalResult (PAL_CALL *resetCommandBuffer)(PalCommandBuffer*);
    PalResult (PAL_CALL *submitCommandBuffer)(PalQueue*, PalCommandBufferSubmitInfo*);

    PalResult (PAL_CALL *cmdBegin)(PalCommandBuffer*, PalRenderingLayoutInfo*);
    PalResult (PAL_CALL *cmdEnd)(PalCommandBuffer*);
    PalResult (PAL_CALL *cmdExecuteCommandBuffer)(PalCommandBuffer*, PalCommandBuffer*);
    PalResult (PAL_CALL *cmdSetFragmentShadingRate)(PalCommandBuffer*, PalFragmentShadingRateState*);
    PalResult (PAL_CALL *cmdDrawMeshTasks)(PalCommandBuffer*, uint32_t, uint32_t, uint32_t);
    PalResult (PAL_CALL *cmdDrawMeshTasksIndirect)(PalCommandBuffer*, PalBuffer*, uint32_t);
    PalResult (PAL_CALL *cmdDrawMeshTasksIndirectCount)(PalCommandBuffer*, PalBuffer*, PalBuffer*, uint32_t);
    PalResult (PAL_CALL *cmdBuildAccelerationStructure)(PalCommandBuffer*, PalAccelerationStructureBuildInfo*);
    PalResult (PAL_CALL *cmdBeginRendering)(PalCommandBuffer*, PalRenderingInfo*);
    PalResult (PAL_CALL *cmdEndRendering)(PalCommandBuffer*);
    PalResult (PAL_CALL *cmdCopyBuffer)(PalCommandBuffer*, PalBuffer*, PalBuffer*, PalBufferCopyInfo*);
    PalResult (PAL_CALL *cmdCopyBufferToImage)(PalCommandBuffer*, PalImage*, PalBuffer*, PalBufferImageCopyInfo*);
    PalResult (PAL_CALL *cmdCopyImage)(PalCommandBuffer*, PalImage*,PalImage*, PalImageCopyInfo*);
    PalResult (PAL_CALL *cmdCopyImageToBuffer)(PalCommandBuffer*, PalBuffer*, PalImage*, PalBufferImageCopyInfo*);
    PalResult (PAL_CALL *cmdBindPipeline)(PalCommandBuffer*, PalPipeline*);
    PalResult (PAL_CALL *cmdSetViewport)(PalCommandBuffer*, uint32_t, PalViewport*);
    PalResult (PAL_CALL *cmdSetScissors)(PalCommandBuffer*, uint32_t, PalRect2D*);
    PalResult (PAL_CALL *cmdBindVertexBuffers)(PalCommandBuffer*, uint32_t, uint32_t, PalBuffer**, uint64_t*);
    PalResult (PAL_CALL *cmdBindIndexBuffer)(PalCommandBuffer*, PalBuffer*, uint64_t, PalIndexType);
    PalResult (PAL_CALL *cmdDraw)(PalCommandBuffer*, uint32_t, uint32_t, uint32_t, uint32_t);
    PalResult (PAL_CALL *cmdDrawIndirect)(PalCommandBuffer*, PalBuffer*, uint32_t);
    PalResult (PAL_CALL *cmdDrawIndirectCount)(PalCommandBuffer*, PalBuffer*, PalBuffer*, uint32_t);
    PalResult (PAL_CALL *cmdDrawIndexed)(PalCommandBuffer*, uint32_t, uint32_t, uint32_t, int32_t, uint32_t);
    PalResult (PAL_CALL *cmdDrawIndexedIndirect)(PalCommandBuffer*, PalBuffer*, uint32_t);
    PalResult (PAL_CALL *cmdDrawIndexedIndirectCount)(PalCommandBuffer*, PalBuffer*, PalBuffer*, uint32_t);
    PalResult (PAL_CALL *cmdAccelerationStructureBarrier)(PalCommandBuffer*, PalAccelerationStructure*, PalUsageState, PalUsageState);
    PalResult (PAL_CALL *cmdImageBarrier)(PalCommandBuffer*, PalImage*, PalImageSubresourceRange*, PalUsageState, PalUsageState);
    PalResult (PAL_CALL *cmdBufferBarrier)(PalCommandBuffer*, PalBuffer*, PalUsageState, PalUsageState);
    PalResult (PAL_CALL *cmdDispatch)(PalCommandBuffer*, uint32_t, uint32_t, uint32_t);
    PalResult (PAL_CALL *cmdDispatchBase)(PalCommandBuffer*, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t);
    PalResult (PAL_CALL *cmdDispatchIndirect)(PalCommandBuffer*, PalBuffer*);
    PalResult (PAL_CALL *cmdTraceRays)(PalCommandBuffer*, PalShaderBindingTable*, uint32_t, uint32_t, uint32_t, uint32_t);
    PalResult (PAL_CALL *cmdTraceRaysIndirect)(PalCommandBuffer*, uint32_t, PalShaderBindingTable*, PalBuffer*);
    PalResult (PAL_CALL *cmdBindDescriptorSet)(PalCommandBuffer*, uint32_t, PalDescriptorSet*);
    PalResult (PAL_CALL *cmdPushConstants)(PalCommandBuffer*, uint32_t, uint32_t, const void*);
    PalResult (PAL_CALL *cmdSetCullMode)(PalCommandBuffer*, PalCullMode);
    PalResult (PAL_CALL *cmdSetFrontFace)(PalCommandBuffer*, PalFrontFace);
    PalResult (PAL_CALL *cmdSetPrimitiveTopology)(PalCommandBuffer*, PalPrimitiveTopology);
    PalResult (PAL_CALL *cmdSetDepthTestEnable)(PalCommandBuffer*, PalBool);
    PalResult (PAL_CALL *cmdSetDepthWriteEnable)(PalCommandBuffer*, PalBool);
    PalResult (PAL_CALL *cmdSetStencilOp)(PalCommandBuffer*, PalStencilFaceFlags, PalStencilOp, PalStencilOp, PalStencilOp, PalCompareOp);

    PalResult (PAL_CALL *createAccelerationstructure)(PalDevice*, const PalAccelerationStructureCreateInfo*, PalAccelerationStructure**);
    void (PAL_CALL *destroyAccelerationstructure)(PalAccelerationStructure*);
    PalResult (PAL_CALL *getAccelerationStructureBuildSize)(PalDevice*, PalAccelerationStructureBuildInfo*, PalAccelerationStructureBuildSize*);
    PalResult (PAL_CALL *createBuffer)(PalDevice*, const PalBufferCreateInfo*, PalBuffer**);
    void (PAL_CALL *destroyBuffer)(PalBuffer*);
    PalResult (PAL_CALL *getBufferMemoryRequirements)(PalBuffer*, PalMemoryRequirements*);
    PalResult (PAL_CALL *computeInstanceBufferRequirements)(PalDevice*, uint32_t, uint64_t*);
    PalResult (PAL_CALL *computeImageCopyStagingBufferRequirements)(PalDevice*, PalFormat, PalBufferImageCopyInfo*, uint32_t*, uint32_t*, uint64_t*);
    PalResult (PAL_CALL *writeToInstanceBuffer)(PalDevice*, void*, PalAccelerationStructureInstance*, uint32_t);
    PalResult (PAL_CALL *writeToImageCopyStagingBuffer)(PalDevice*, void*, void*, PalFormat, PalBufferImageCopyInfo*);
    PalResult (PAL_CALL *bindBufferMemory)(PalBuffer*, PalMemory*, uint64_t);
    PalResult (PAL_CALL *mapBuffer)(PalBuffer*, uint64_t, uint64_t, void**);
    void (PAL_CALL *unmapBuffer)(PalBuffer*);
    PalDeviceAddress (PAL_CALL *getBufferDeviceAddress)(PalBuffer*);

    PalResult (PAL_CALL *createDescriptorSetLayout)(PalDevice*, const PalDescriptorSetLayoutCreateInfo*, PalDescriptorSetLayout**);
    void (PAL_CALL *destroyDescriptorSetLayout)(PalDescriptorSetLayout*);
    PalResult (PAL_CALL *createDescriptorPool)(PalDevice*, const PalDescriptorPoolCreateInfo*, PalDescriptorPool**);
    void (PAL_CALL *destroyDescriptorPool)(PalDescriptorPool*);
    PalResult (PAL_CALL *resetDescriptorPool)(PalDescriptorPool*);
    PalResult (PAL_CALL *allocateDescriptorSet)(PalDevice*, PalDescriptorPool*, PalDescriptorSetLayout*, PalDescriptorSet**);
    PalResult (PAL_CALL *updateDescriptorSet)(PalDevice*, uint32_t, PalDescriptorSetWriteInfo*);

    PalResult (PAL_CALL *createPipelineLayout)(PalDevice*, const PalPipelineLayoutCreateInfo*, PalPipelineLayout**);
    void (PAL_CALL *destroyPipelineLayout)(PalPipelineLayout*);
    PalResult (PAL_CALL *createGraphicsPipeline)(PalDevice*, const PalGraphicsPipelineCreateInfo*, PalPipeline**);
    PalResult (PAL_CALL *createComputePipeline)(PalDevice*, const PalComputePipelineCreateInfo*, PalPipeline**);
    PalResult (PAL_CALL *createRayTracingPipeline)(PalDevice*, const PalRayTracingPipelineCreateInfo*, PalPipeline**);
    void (PAL_CALL *destroyPipeline)(PalPipeline*);
    PalResult (PAL_CALL *createShaderBindingTable)(PalDevice*, const PalShaderBindingTableCreateInfo*, PalShaderBindingTable**);
    void (PAL_CALL *destroyShaderBindingTable)(PalShaderBindingTable*);
    PalResult (PAL_CALL *updateShaderBindingTable)(PalShaderBindingTable*, uint32_t, PalShaderBindingTableRecordInfo*);
} PalGraphicsVtable;

// ==================================================
// Vulkan
// ==================================================

#if PAL_HAS_VULKAN_BACKEND
PalResult PAL_CALL initGraphicsVk(const PalGraphicsDebugger*, const PalAllocator*);
void PAL_CALL shutdownGraphicsVk();
PalResult PAL_CALL enumerateAdaptersVk(int32_t*, PalAdapter**);
PalResult PAL_CALL getAdapterInfoVk(PalAdapter*, PalAdapterInfo*);
PalResult PAL_CALL getAdapterCapabilitiesVk(PalAdapter*, PalAdapterCapabilities*);
PalAdapterFeatures PAL_CALL getAdapterFeaturesVk(PalAdapter*);
uint32_t PAL_CALL getHighestSupportedShaderTargetVk(PalAdapter*, PalShaderFormats);
PalResult PAL_CALL createDeviceVk(PalAdapter*, PalAdapterFeatures, PalDevice**);
void PAL_CALL destroyDeviceVk(PalDevice*);
PalResult PAL_CALL waitDeviceVk(PalDevice*);
PalResult PAL_CALL allocateMemoryVk(PalDevice*, PalMemoryType, uint64_t, uint64_t, PalMemory**);
void PAL_CALL freeMemoryVk(PalDevice*, PalMemory*);
PalResult PAL_CALL querySamplerAnisotropyCapabilitiesVk(PalDevice*, PalSamplerAnisotropyCapabilities*);
PalResult PAL_CALL queryMultiViewCapabilitiesVk(PalDevice*, PalMultiViewCapabilities*);
PalResult PAL_CALL queryMultiViewportCapabilitiesVk(PalDevice*, PalMultiViewportCapabilities*);
PalResult PAL_CALL queryDepthStencilCapabilitiesVk(PalDevice*, PalDepthStencilCapabilities*);
PalResult PAL_CALL queryFragmentShadingRateCapabilitiesVk(PalDevice*, PalFragmentShadingRateCapabilities*);
PalResult PAL_CALL queryMeshShaderCapabilitiesVk(PalDevice*, PalMeshShaderCapabilities*);
PalResult PAL_CALL queryRayTracingCapabilitiesVk(PalDevice*, PalRayTracingCapabilities*);
PalResult PAL_CALL queryDescriptorIndexingCapabilitiesVk(PalDevice*, PalDescriptorIndexingCapabilities*);

PalResult PAL_CALL createQueueVk(PalDevice*, PalQueueType, PalQueue**);
void PAL_CALL destroyQueueVk(PalQueue*);
PalResult PAL_CALL waitQueueVk(PalQueue*);
PalBool PAL_CALL canQueuePresentVk(PalQueue*, PalSurface*);
PalResult PAL_CALL enumerateFormatsVk(PalAdapter*, int32_t*, PalFormatInfo*);
PalBool PAL_CALL isFormatSupportedVk(PalAdapter*, PalFormat);
PalImageUsages PAL_CALL queryFormatImageUsagesVk(PalAdapter*, PalFormat);
PalSampleCount PAL_CALL queryFormatSampleCountVk(PalAdapter*, PalFormat);
PalResult PAL_CALL createImageVk(PalDevice*, const PalImageCreateInfo*, PalImage**);
void PAL_CALL destroyImageVk(PalImage*);
PalResult PAL_CALL getImageInfoVk(PalImage*, PalImageInfo*);
PalResult PAL_CALL getImageMemoryRequirementsVk(PalImage*, PalMemoryRequirements*);
PalResult PAL_CALL bindImageMemoryVk(PalImage*, PalMemory*, uint64_t);
PalResult PAL_CALL createImageViewVk(PalDevice*, PalImage*, const PalImageViewCreateInfo*, PalImageView**);
void PAL_CALL destroyImageViewVk(PalImageView*);

PalResult PAL_CALL createSamplerVk(PalDevice*, const PalSamplerCreateInfo*, PalSampler**);
void PAL_CALL destroySamplerVk(PalSampler*);
PalResult PAL_CALL createSurfaceVk(PalDevice*, void*, void*, PalWindowInstanceType, PalSurface**);
void PAL_CALL destroySurfaceVk(PalSurface*);
PalResult PAL_CALL getSurfaceCapabilitiesVk(PalDevice*, PalSurface*, PalSurfaceCapabilities*);
PalResult PAL_CALL createSwapchainVk(PalDevice*, PalQueue*, PalSurface*, const PalSwapchainCreateInfo*, PalSwapchain**);
void PAL_CALL destroySwapchainVk(PalSwapchain*);
PalImage* PAL_CALL getSwapchainImageVk(PalSwapchain*, uint32_t);
PalResult PAL_CALL getNextSwapchainImageVk(PalSwapchain*, PalSwapchainNextImageInfo*, uint32_t*);
PalResult PAL_CALL presentSwapchainVk(PalSwapchain*, uint32_t, PalSemaphore*);
PalResult PAL_CALL resizeSwapchainVk(PalSwapchain*, uint32_t, uint32_t);
PalResult PAL_CALL createShaderVk(PalDevice*, const PalShaderCreateInfo*, PalShader**);
void PAL_CALL destroyShaderVk(PalShader*);

PalResult PAL_CALL createFenceVk(PalDevice*, PalBool, PalFence**);
void PAL_CALL destroyFenceVk(PalFence*);
PalResult PAL_CALL waitFenceVk(PalFence*, uint64_t);
PalResult PAL_CALL resetFenceVk(PalFence*);
PalBool PAL_CALL isFenceSignaledVk(PalFence*);
PalResult PAL_CALL createSemaphoreVk(PalDevice*, PalBool, PalSemaphore**);
void PAL_CALL destroySemaphoreVk(PalSemaphore*);
PalResult PAL_CALL waitSemaphoreVk(PalSemaphore*, uint64_t, uint64_t);
PalResult PAL_CALL signalSemaphoreVk(PalSemaphore*, PalQueue*, uint64_t);
PalResult PAL_CALL getSemaphoreValueVk(PalSemaphore*, uint64_t*);
PalResult PAL_CALL createCommandPoolVk(PalDevice*, PalQueue*, PalCommandPool**);
void PAL_CALL destroyCommandPoolVk(PalCommandPool*);
PalResult PAL_CALL resetCommandPoolVk(PalCommandPool*);
PalResult PAL_CALL allocateCommandBufferVk(PalDevice*, PalCommandPool*, PalCommandBufferType, PalCommandBuffer**);
void PAL_CALL freeCommandBufferVk(PalCommandBuffer*);
PalResult PAL_CALL resetCommandBufferVk(PalCommandBuffer*);
PalResult PAL_CALL submitCommandBufferVk(PalQueue*, PalCommandBufferSubmitInfo*);

PalResult PAL_CALL cmdBeginVk(PalCommandBuffer*, PalRenderingLayoutInfo*);
PalResult PAL_CALL cmdEndVk(PalCommandBuffer*);
PalResult PAL_CALL cmdExecuteCommandBufferVk(PalCommandBuffer*, PalCommandBuffer*);
PalResult PAL_CALL cmdSetFragmentShadingRateVk(PalCommandBuffer*, PalFragmentShadingRateState*);
PalResult PAL_CALL cmdDrawMeshTasksVk(PalCommandBuffer*, uint32_t, uint32_t, uint32_t);
PalResult PAL_CALL cmdDrawMeshTasksIndirectVk(PalCommandBuffer*, PalBuffer*, uint32_t);
PalResult PAL_CALL cmdDrawMeshTasksIndirectCountVk(PalCommandBuffer*, PalBuffer*, PalBuffer*, uint32_t);
PalResult PAL_CALL cmdBuildAccelerationStructureVk(PalCommandBuffer*, PalAccelerationStructureBuildInfo*);
PalResult PAL_CALL cmdBeginRenderingVk(PalCommandBuffer*, PalRenderingInfo*);
PalResult PAL_CALL cmdEndRenderingVk(PalCommandBuffer*);
PalResult PAL_CALL cmdCopyBufferVk(PalCommandBuffer*, PalBuffer*, PalBuffer*, PalBufferCopyInfo*);
PalResult PAL_CALL cmdCopyBufferToImageVk(PalCommandBuffer*, PalImage*, PalBuffer*, PalBufferImageCopyInfo*);
PalResult PAL_CALL cmdCopyImageVk(PalCommandBuffer*, PalImage*, PalImage*, PalImageCopyInfo*);
PalResult PAL_CALL cmdCopyImageToBufferVk(PalCommandBuffer*, PalBuffer*, PalImage*, PalBufferImageCopyInfo*);
PalResult PAL_CALL cmdBindPipelineVk(PalCommandBuffer*, PalPipeline*);
PalResult PAL_CALL cmdSetViewportVk(PalCommandBuffer*, uint32_t, PalViewport*);
PalResult PAL_CALL cmdSetScissorsVk(PalCommandBuffer*, uint32_t, PalRect2D*);
PalResult PAL_CALL cmdBindVertexBuffersVk(PalCommandBuffer*, uint32_t, uint32_t, PalBuffer**, uint64_t*);
PalResult PAL_CALL cmdBindIndexBufferVk(PalCommandBuffer*, PalBuffer*, uint64_t, PalIndexType);
PalResult PAL_CALL cmdDrawVk(PalCommandBuffer*, uint32_t, uint32_t, uint32_t, uint32_t);
PalResult PAL_CALL cmdDrawIndirectVk(PalCommandBuffer*, PalBuffer*, uint32_t);
PalResult PAL_CALL cmdDrawIndirectCountVk(PalCommandBuffer*, PalBuffer*, PalBuffer*, uint32_t);
PalResult PAL_CALL cmdDrawIndexedVk(PalCommandBuffer*, uint32_t, uint32_t, uint32_t, int32_t, uint32_t);
PalResult PAL_CALL cmdDrawIndexedIndirectVk(PalCommandBuffer*, PalBuffer*, uint32_t);
PalResult PAL_CALL cmdDrawIndexedIndirectCountVk(PalCommandBuffer*, PalBuffer*, PalBuffer*, uint32_t);
PalResult PAL_CALL cmdAccelerationStructureBarrierVk(PalCommandBuffer*, PalAccelerationStructure*, PalUsageState, PalUsageState);
PalResult PAL_CALL cmdImageBarrierVk(PalCommandBuffer*, PalImage*, PalImageSubresourceRange*, PalUsageState, PalUsageState);
PalResult PAL_CALL cmdBufferBarrierVk(PalCommandBuffer*, PalBuffer*, PalUsageState, PalUsageState);
PalResult PAL_CALL cmdDispatchVk(PalCommandBuffer*, uint32_t, uint32_t, uint32_t);
PalResult PAL_CALL cmdDispatchBaseVk(PalCommandBuffer*, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t);
PalResult PAL_CALL cmdDispatchIndirectVk(PalCommandBuffer*, PalBuffer*);
PalResult PAL_CALL cmdTraceRaysVk(PalCommandBuffer*, PalShaderBindingTable*, uint32_t, uint32_t, uint32_t, uint32_t);
PalResult PAL_CALL cmdTraceRaysIndirectVk(PalCommandBuffer*, uint32_t, PalShaderBindingTable*, PalBuffer*);
PalResult PAL_CALL cmdBindDescriptorSetVk(PalCommandBuffer*, uint32_t, PalDescriptorSet*);
PalResult PAL_CALL cmdPushConstantsVk(PalCommandBuffer*, uint32_t, uint32_t, const void*);
PalResult PAL_CALL cmdSetCullModeVk(PalCommandBuffer*, PalCullMode);
PalResult PAL_CALL cmdSetFrontFaceVk(PalCommandBuffer*, PalFrontFace);
PalResult PAL_CALL cmdSetPrimitiveTopologyVk(PalCommandBuffer*, PalPrimitiveTopology);
PalResult PAL_CALL cmdSetDepthTestEnableVk(PalCommandBuffer*, PalBool);
PalResult PAL_CALL cmdSetDepthWriteEnableVk(PalCommandBuffer*, PalBool);
PalResult PAL_CALL cmdSetStencilOpVk(PalCommandBuffer*, PalStencilFaceFlags, PalStencilOp, PalStencilOp, PalStencilOp, PalCompareOp);

PalResult PAL_CALL createAccelerationstructureVk(PalDevice*, const PalAccelerationStructureCreateInfo*, PalAccelerationStructure**);
void PAL_CALL destroyAccelerationstructureVk(PalAccelerationStructure*);
PalResult PAL_CALL getAccelerationStructureBuildSizeVk(PalDevice*, PalAccelerationStructureBuildInfo*, PalAccelerationStructureBuildSize*);
PalResult PAL_CALL createBufferVk(PalDevice*, const PalBufferCreateInfo*, PalBuffer**);
void PAL_CALL destroyBufferVk(PalBuffer*);
PalResult PAL_CALL getBufferMemoryRequirementsVk(PalBuffer*, PalMemoryRequirements*);
PalResult PAL_CALL computeInstanceBufferRequirementsVk(PalDevice*, uint32_t, uint64_t*);
PalResult PAL_CALL computeImageCopyStagingBufferRequirementsVk(PalDevice*, PalFormat, PalBufferImageCopyInfo*, uint32_t*, uint32_t*, uint64_t*);
PalResult PAL_CALL writeToInstanceBufferVk(PalDevice*, void*, PalAccelerationStructureInstance*, uint32_t);
PalResult PAL_CALL writeToImageCopyStagingBufferVk(PalDevice*, void*, void*, PalFormat, PalBufferImageCopyInfo*);
PalResult PAL_CALL bindBufferMemoryVk(PalBuffer*, PalMemory*, uint64_t);
PalResult PAL_CALL mapBufferVk(PalBuffer*, uint64_t, uint64_t, void**);
void PAL_CALL unmapBufferVk(PalBuffer*);
PalDeviceAddress PAL_CALL getBufferDeviceAddressVk(PalBuffer*);
PalResult PAL_CALL createDescriptorSetLayoutVk(PalDevice*, const PalDescriptorSetLayoutCreateInfo*, PalDescriptorSetLayout**);
void PAL_CALL destroyDescriptorSetLayoutVk(PalDescriptorSetLayout*);
PalResult PAL_CALL createDescriptorPoolVk(PalDevice*, const PalDescriptorPoolCreateInfo*, PalDescriptorPool**);
void PAL_CALL destroyDescriptorPoolVk(PalDescriptorPool*);
PalResult PAL_CALL resetDescriptorPoolVk(PalDescriptorPool*);
PalResult PAL_CALL allocateDescriptorSetVk(PalDevice*, PalDescriptorPool*, PalDescriptorSetLayout*, PalDescriptorSet**);
PalResult PAL_CALL updateDescriptorSetVk(PalDevice*, uint32_t, PalDescriptorSetWriteInfo*);

PalResult PAL_CALL createPipelineLayoutVk(PalDevice*, const PalPipelineLayoutCreateInfo*, PalPipelineLayout**);
void PAL_CALL destroyPipelineLayoutVk(PalPipelineLayout*);
PalResult PAL_CALL createGraphicsPipelineVk(PalDevice*, const PalGraphicsPipelineCreateInfo*, PalPipeline**);
PalResult PAL_CALL createComputePipelineVk(PalDevice*, const PalComputePipelineCreateInfo*, PalPipeline**);
PalResult PAL_CALL createRayTracingPipelineVk(PalDevice*, const PalRayTracingPipelineCreateInfo*, PalPipeline**);
void PAL_CALL destroyPipelineVk(PalPipeline*);
PalResult PAL_CALL createShaderBindingTableVk(PalDevice*, const PalShaderBindingTableCreateInfo*, PalShaderBindingTable**);
void PAL_CALL destroyShaderBindingTableVk(PalShaderBindingTable*);
PalResult PAL_CALL updateShaderBindingTableVk(PalShaderBindingTable*, uint32_t, PalShaderBindingTableRecordInfo*);

static PalGraphicsVtable s_VkBackend = {
    .enumerateAdapters = enumerateAdaptersVk,
    .getAdapterInfo = getAdapterInfoVk,
    .getAdapterCapabilities = getAdapterCapabilitiesVk,
    .getAdapterFeatures = getAdapterFeaturesVk,
    .getHighestSupportedShaderTarget = getHighestSupportedShaderTargetVk,
    .createDevice = createDeviceVk,
    .destroyDevice = destroyDeviceVk,
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
    .resetCommandPool = resetCommandPoolVk,
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
    .computeInstanceBufferRequirements = computeInstanceBufferRequirementsVk,
    .computeImageCopyStagingBufferRequirements = computeImageCopyStagingBufferRequirementsVk,
    .writeToInstanceBuffer = writeToInstanceBufferVk,
    .writeToImageCopyStagingBuffer = writeToImageCopyStagingBufferVk,
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

#endif // PAL_HAS_VULKAN_BACKEND

// ==================================================
// D3D12
// ==================================================

#if PAL_HAS_D3D12_BACKEND
PalResult PAL_CALL initGraphicsD3D12(const PalGraphicsDebugger*, const PalAllocator*);
void PAL_CALL shutdownGraphicsD3D12();
PalResult PAL_CALL enumerateAdaptersD3D12(int32_t*, PalAdapter**);
PalResult PAL_CALL getAdapterInfoD3D12(PalAdapter*, PalAdapterInfo*);
PalResult PAL_CALL getAdapterCapabilitiesD3D12(PalAdapter*, PalAdapterCapabilities*);
PalAdapterFeatures PAL_CALL getAdapterFeaturesD3D12(PalAdapter*);
uint32_t PAL_CALL getHighestSupportedShaderTargetD3D12(PalAdapter*, PalShaderFormats);
PalResult PAL_CALL createDeviceD3D12(PalAdapter*, PalAdapterFeatures, PalDevice**);
void PAL_CALL destroyDeviceD3D12(PalDevice*);
PalResult PAL_CALL waitDeviceD3D12(PalDevice*);
PalResult PAL_CALL allocateMemoryD3D12(PalDevice*, PalMemoryType, uint64_t, uint64_t, PalMemory**);
void PAL_CALL freeMemoryD3D12(PalDevice*, PalMemory*);
PalResult PAL_CALL querySamplerAnisotropyCapabilitiesD3D12(PalDevice*, PalSamplerAnisotropyCapabilities*);
PalResult PAL_CALL queryMultiViewCapabilitiesD3D12(PalDevice*, PalMultiViewCapabilities*);
PalResult PAL_CALL queryMultiViewportCapabilitiesD3D12(PalDevice*, PalMultiViewportCapabilities*);
PalResult PAL_CALL queryDepthStencilCapabilitiesD3D12(PalDevice*, PalDepthStencilCapabilities*);
PalResult PAL_CALL queryFragmentShadingRateCapabilitiesD3D12(PalDevice*, PalFragmentShadingRateCapabilities*);
PalResult PAL_CALL queryMeshShaderCapabilitiesD3D12(PalDevice*, PalMeshShaderCapabilities*);
PalResult PAL_CALL queryRayTracingCapabilitiesD3D12(PalDevice*, PalRayTracingCapabilities*);
PalResult PAL_CALL queryDescriptorIndexingCapabilitiesD3D12(PalDevice*, PalDescriptorIndexingCapabilities*);

PalResult PAL_CALL createQueueD3D12(PalDevice*, PalQueueType, PalQueue**);
void PAL_CALL destroyQueueD3D12(PalQueue*);
PalResult PAL_CALL waitQueueD3D12(PalQueue*);
PalBool PAL_CALL canQueuePresentD3D12(PalQueue*, PalSurface*);
PalResult PAL_CALL enumerateFormatsD3D12(PalAdapter*, int32_t*, PalFormatInfo*);
PalBool PAL_CALL isFormatSupportedD3D12(PalAdapter*, PalFormat);
PalImageUsages PAL_CALL queryFormatImageUsagesD3D12(PalAdapter*, PalFormat);
PalSampleCount PAL_CALL queryFormatSampleCountD3D12(PalAdapter*, PalFormat);
PalResult PAL_CALL createImageD3D12(PalDevice*, const PalImageCreateInfo*, PalImage**);
void PAL_CALL destroyImageD3D12(PalImage*);
PalResult PAL_CALL getImageInfoD3D12(PalImage*, PalImageInfo*);
PalResult PAL_CALL getImageMemoryRequirementsD3D12(PalImage*, PalMemoryRequirements*);
PalResult PAL_CALL bindImageMemoryD3D12(PalImage*, PalMemory*, uint64_t);
PalResult PAL_CALL createImageViewD3D12(PalDevice*, PalImage*, const PalImageViewCreateInfo*, PalImageView**);
void PAL_CALL destroyImageViewD3D12(PalImageView*);

PalResult PAL_CALL createSamplerD3D12(PalDevice*, const PalSamplerCreateInfo*, PalSampler**);
void PAL_CALL destroySamplerD3D12(PalSampler*);
PalResult PAL_CALL createSurfaceD3D12(PalDevice*, void*, void*, PalWindowInstanceType, PalSurface**);
void PAL_CALL destroySurfaceD3D12(PalSurface*);
PalResult PAL_CALL getSurfaceCapabilitiesD3D12(PalDevice*, PalSurface*, PalSurfaceCapabilities*);
PalResult PAL_CALL createSwapchainD3D12(PalDevice*, PalQueue*, PalSurface*, const PalSwapchainCreateInfo*, PalSwapchain**);
void PAL_CALL destroySwapchainD3D12(PalSwapchain*);
PalImage* PAL_CALL getSwapchainImageD3D12(PalSwapchain*, uint32_t);
PalResult PAL_CALL getNextSwapchainImageD3D12(PalSwapchain*, PalSwapchainNextImageInfo*, uint32_t*);
PalResult PAL_CALL presentSwapchainD3D12(PalSwapchain*, uint32_t, PalSemaphore*);
PalResult PAL_CALL resizeSwapchainD3D12(PalSwapchain*, uint32_t, uint32_t);
PalResult PAL_CALL createShaderD3D12(PalDevice*, const PalShaderCreateInfo*, PalShader**);
void PAL_CALL destroyShaderD3D12(PalShader*);

PalResult PAL_CALL createFenceD3D12(PalDevice*, PalBool, PalFence**);
void PAL_CALL destroyFenceD3D12(PalFence*);
PalResult PAL_CALL waitFenceD3D12(PalFence*, uint64_t);
PalResult PAL_CALL resetFenceD3D12(PalFence*);
PalBool PAL_CALL isFenceSignaledD3D12(PalFence*);
PalResult PAL_CALL createSemaphoreD3D12(PalDevice*, PalBool, PalSemaphore**);
void PAL_CALL destroySemaphoreD3D12(PalSemaphore*);
PalResult PAL_CALL waitSemaphoreD3D12(PalSemaphore*, uint64_t, uint64_t);
PalResult PAL_CALL signalSemaphoreD3D12(PalSemaphore*, PalQueue*, uint64_t);
PalResult PAL_CALL getSemaphoreValueD3D12(PalSemaphore*, uint64_t*);
PalResult PAL_CALL createCommandPoolD3D12(PalDevice*, PalQueue*, PalCommandPool**);
void PAL_CALL destroyCommandPoolD3D12(PalCommandPool*);
PalResult PAL_CALL resetCommandPoolD3D12(PalCommandPool*);
PalResult PAL_CALL allocateCommandBufferD3D12(PalDevice*, PalCommandPool*, PalCommandBufferType, PalCommandBuffer**);
void PAL_CALL freeCommandBufferD3D12(PalCommandBuffer*);
PalResult PAL_CALL resetCommandBufferD3D12(PalCommandBuffer*);
PalResult PAL_CALL submitCommandBufferD3D12(PalQueue*, PalCommandBufferSubmitInfo*);

PalResult PAL_CALL cmdBeginD3D12(PalCommandBuffer*, PalRenderingLayoutInfo*);
PalResult PAL_CALL cmdEndD3D12(PalCommandBuffer*);
PalResult PAL_CALL cmdExecuteCommandBufferD3D12(PalCommandBuffer*, PalCommandBuffer*);
PalResult PAL_CALL cmdSetFragmentShadingRateD3D12(PalCommandBuffer*, PalFragmentShadingRateState*);
PalResult PAL_CALL cmdDrawMeshTasksD3D12(PalCommandBuffer*, uint32_t, uint32_t, uint32_t);
PalResult PAL_CALL cmdDrawMeshTasksIndirectD3D12(PalCommandBuffer*, PalBuffer*, uint32_t);
PalResult PAL_CALL cmdDrawMeshTasksIndirectCountD3D12(PalCommandBuffer*, PalBuffer*, PalBuffer*, uint32_t);
PalResult PAL_CALL cmdBuildAccelerationStructureD3D12(PalCommandBuffer*, PalAccelerationStructureBuildInfo*);
PalResult PAL_CALL cmdBeginRenderingD3D12(PalCommandBuffer*, PalRenderingInfo*);
PalResult PAL_CALL cmdEndRenderingD3D12(PalCommandBuffer*);
PalResult PAL_CALL cmdCopyBufferD3D12(PalCommandBuffer*, PalBuffer*, PalBuffer*, PalBufferCopyInfo*);
PalResult PAL_CALL cmdCopyBufferToImageD3D12(PalCommandBuffer*, PalImage*, PalBuffer*, PalBufferImageCopyInfo*);
PalResult PAL_CALL cmdCopyImageD3D12(PalCommandBuffer*, PalImage*, PalImage*, PalImageCopyInfo*);
PalResult PAL_CALL cmdCopyImageToBufferD3D12(PalCommandBuffer*, PalBuffer*, PalImage*, PalBufferImageCopyInfo*);
PalResult PAL_CALL cmdBindPipelineD3D12(PalCommandBuffer*, PalPipeline*);
PalResult PAL_CALL cmdSetViewportD3D12(PalCommandBuffer*, uint32_t, PalViewport*);
PalResult PAL_CALL cmdSetScissorsD3D12(PalCommandBuffer*, uint32_t, PalRect2D*);
PalResult PAL_CALL cmdBindVertexBuffersD3D12(PalCommandBuffer*, uint32_t, uint32_t, PalBuffer**, uint64_t*);
PalResult PAL_CALL cmdBindIndexBufferD3D12(PalCommandBuffer*, PalBuffer*, uint64_t, PalIndexType);
PalResult PAL_CALL cmdDrawD3D12(PalCommandBuffer*, uint32_t, uint32_t, uint32_t, uint32_t);
PalResult PAL_CALL cmdDrawIndirectD3D12(PalCommandBuffer*, PalBuffer*, uint32_t);
PalResult PAL_CALL cmdDrawIndirectCountD3D12(PalCommandBuffer*, PalBuffer*, PalBuffer*, uint32_t);
PalResult PAL_CALL cmdDrawIndexedD3D12(PalCommandBuffer*, uint32_t, uint32_t, uint32_t, int32_t, uint32_t);
PalResult PAL_CALL cmdDrawIndexedIndirectD3D12(PalCommandBuffer*, PalBuffer*, uint32_t);
PalResult PAL_CALL cmdDrawIndexedIndirectCountD3D12(PalCommandBuffer*, PalBuffer*, PalBuffer*, uint32_t);
PalResult PAL_CALL cmdAccelerationStructureBarrierD3D12(PalCommandBuffer*, PalAccelerationStructure*, PalUsageState, PalUsageState);
PalResult PAL_CALL cmdImageBarrierD3D12(PalCommandBuffer*, PalImage*, PalImageSubresourceRange*, PalUsageState, PalUsageState);
PalResult PAL_CALL cmdBufferBarrierD3D12(PalCommandBuffer*, PalBuffer*, PalUsageState, PalUsageState);
PalResult PAL_CALL cmdDispatchD3D12(PalCommandBuffer*, uint32_t, uint32_t, uint32_t);
PalResult PAL_CALL cmdDispatchBaseD3D12(PalCommandBuffer*, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t);
PalResult PAL_CALL cmdDispatchIndirectD3D12(PalCommandBuffer*, PalBuffer*);
PalResult PAL_CALL cmdTraceRaysD3D12(PalCommandBuffer*, PalShaderBindingTable*, uint32_t, uint32_t, uint32_t, uint32_t);
PalResult PAL_CALL cmdTraceRaysIndirectD3D12(PalCommandBuffer*, uint32_t, PalShaderBindingTable*, PalBuffer*);
PalResult PAL_CALL cmdBindDescriptorSetD3D12(PalCommandBuffer*, uint32_t, PalDescriptorSet*);
PalResult PAL_CALL cmdPushConstantsD3D12(PalCommandBuffer*, uint32_t, uint32_t, const void*);
PalResult PAL_CALL cmdSetCullModeD3D12(PalCommandBuffer*, PalCullMode);
PalResult PAL_CALL cmdSetFrontFaceD3D12(PalCommandBuffer*, PalFrontFace);
PalResult PAL_CALL cmdSetPrimitiveTopologyD3D12(PalCommandBuffer*, PalPrimitiveTopology);
PalResult PAL_CALL cmdSetDepthTestEnableD3D12(PalCommandBuffer*, PalBool);
PalResult PAL_CALL cmdSetDepthWriteEnableD3D12(PalCommandBuffer*, PalBool);
PalResult PAL_CALL cmdSetStencilOpD3D12(PalCommandBuffer*, PalStencilFaceFlags, PalStencilOp, PalStencilOp, PalStencilOp, PalCompareOp);

PalResult PAL_CALL createAccelerationstructureD3D12(PalDevice*, const PalAccelerationStructureCreateInfo*, PalAccelerationStructure**);
void PAL_CALL destroyAccelerationstructureD3D12(PalAccelerationStructure*);
PalResult PAL_CALL getAccelerationStructureBuildSizeD3D12(PalDevice*, PalAccelerationStructureBuildInfo*, PalAccelerationStructureBuildSize*);
PalResult PAL_CALL createBufferD3D12(PalDevice*, const PalBufferCreateInfo*, PalBuffer**);
void PAL_CALL destroyBufferD3D12(PalBuffer*);
PalResult PAL_CALL getBufferMemoryRequirementsD3D12(PalBuffer*, PalMemoryRequirements*);
PalResult PAL_CALL computeInstanceBufferRequirementsD3D12(PalDevice*, uint32_t, uint64_t*);
PalResult PAL_CALL computeImageCopyStagingBufferRequirementsD3D12(PalDevice*, PalFormat, PalBufferImageCopyInfo*, uint32_t*, uint32_t*, uint64_t*);
PalResult PAL_CALL writeToInstanceBufferD3D12(PalDevice*, void*, PalAccelerationStructureInstance*, uint32_t);
PalResult PAL_CALL writeToImageCopyStagingBufferD3D12(PalDevice*, void*, void*, PalFormat, PalBufferImageCopyInfo*);
PalResult PAL_CALL bindBufferMemoryD3D12(PalBuffer*, PalMemory*, uint64_t);
PalResult PAL_CALL mapBufferD3D12(PalBuffer*, uint64_t, uint64_t, void**);
void PAL_CALL unmapBufferD3D12(PalBuffer*);
PalDeviceAddress PAL_CALL getBufferDeviceAddressD3D12(PalBuffer*);
PalResult PAL_CALL createDescriptorSetLayoutD3D12(PalDevice*, const PalDescriptorSetLayoutCreateInfo*, PalDescriptorSetLayout**);
void PAL_CALL destroyDescriptorSetLayoutD3D12(PalDescriptorSetLayout*);
PalResult PAL_CALL createDescriptorPoolD3D12(PalDevice*, const PalDescriptorPoolCreateInfo*, PalDescriptorPool**);
void PAL_CALL destroyDescriptorPoolD3D12(PalDescriptorPool*);
PalResult PAL_CALL resetDescriptorPoolD3D12(PalDescriptorPool*);
PalResult PAL_CALL allocateDescriptorSetD3D12(PalDevice*, PalDescriptorPool*, PalDescriptorSetLayout*, PalDescriptorSet**);
PalResult PAL_CALL updateDescriptorSetD3D12(PalDevice*, uint32_t, PalDescriptorSetWriteInfo*);

PalResult PAL_CALL createPipelineLayoutD3D12(PalDevice*, const PalPipelineLayoutCreateInfo*, PalPipelineLayout**);
void PAL_CALL destroyPipelineLayoutD3D12(PalPipelineLayout*);
PalResult PAL_CALL createGraphicsPipelineD3D12(PalDevice*, const PalGraphicsPipelineCreateInfo*, PalPipeline**);
PalResult PAL_CALL createComputePipelineD3D12(PalDevice*, const PalComputePipelineCreateInfo*, PalPipeline**);
PalResult PAL_CALL createRayTracingPipelineD3D12(PalDevice*, const PalRayTracingPipelineCreateInfo*, PalPipeline**);
void PAL_CALL destroyPipelineD3D12(PalPipeline*);
PalResult PAL_CALL createShaderBindingTableD3D12(PalDevice*, const PalShaderBindingTableCreateInfo*, PalShaderBindingTable**);
void PAL_CALL destroyShaderBindingTableD3D12(PalShaderBindingTable*);
PalResult PAL_CALL updateShaderBindingTableD3D12(PalShaderBindingTable*, uint32_t, PalShaderBindingTableRecordInfo*);

static PalGraphicsVtable s_D3D12Backend = {
    .enumerateAdapters = enumerateAdaptersD3D12,
    .getAdapterInfo = getAdapterInfoD3D12,
    .getAdapterCapabilities = getAdapterCapabilitiesD3D12,
    .getAdapterFeatures = getAdapterFeaturesD3D12,
    .getHighestSupportedShaderTarget = getHighestSupportedShaderTargetD3D12,
    .createDevice = createDeviceD3D12,
    .destroyDevice = destroyDeviceD3D12,
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
    .resetCommandPool = resetCommandPoolD3D12,
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
    .createAccelerationstructure = createAccelerationstructureD3D12,
    .destroyAccelerationstructure = destroyAccelerationstructureD3D12,
    .getAccelerationStructureBuildSize = getAccelerationStructureBuildSizeD3D12,
    .createBuffer = createBufferD3D12,
    .destroyBuffer = destroyBufferD3D12,
    .getBufferMemoryRequirements = getBufferMemoryRequirementsD3D12,
    .computeInstanceBufferRequirements = computeInstanceBufferRequirementsD3D12,
    .computeImageCopyStagingBufferRequirements = computeImageCopyStagingBufferRequirementsD3D12,
    .writeToInstanceBuffer = writeToInstanceBufferD3D12,
    .writeToImageCopyStagingBuffer = writeToImageCopyStagingBufferD3D12,
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

#endif // PAL_HAS_D3D12_BACKEND

// clang-format on

#endif // _PAL_GRAPHICS_BACKENDS_H