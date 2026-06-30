
/**
    PAL - Prime Abstraction Layer
    Copyright (C) 2025
    Licensed under the Zlib license. See LICENSE file in root.
 */

#ifndef _PAL_GRAPHICS_BACKENDS_H
#define _PAL_GRAPHICS_BACKENDS_H

#include "pal/pal_graphics.h"

typedef struct {
    // clang-format off
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
    PalResult (PAL_CALL *mapImageMemory)(PalImage*, uint64_t, uint64_t, void**);
    void (PAL_CALL *unmapImageMemory)(PalImage*);
    PalResult (PAL_CALL *createImageView)(PalDevice*, PalImage*, const PalImageViewCreateInfo*, PalImageView**);
    void (PAL_CALL *destroyImageView)(PalImageView*);

    PalResult (PAL_CALL *createSampler)(PalDevice*, const PalSamplerCreateInfo*, PalSampler**);
    void (PAL_CALL *destroySampler)(PalSampler*);
    PalResult (PAL_CALL *createSurface)(PalDevice*, void*, void*, PalWindowInstanceType, PalSurface**);
    void (PAL_CALL *destroySurface)(PalSurface*);
    PalResult (PAL_CALL *getSurfaceCapabilities)(PalDevice*, PalSurface*, PalSurfaceCapabilities*);
    PalResult (PAL_CALL *createSwapchain)(PalDevice*, PalQueue*, PalSurface*, const PalSwapchainCreateInfo*, PalSwapchain**);
    void (PAL_CALL *destroySwapchain)(PalSwapchain*);
    PalImage* (PAL_CALL *getSwapchainImage)(PalSwapchain*, int32_t);
    PalResult (PAL_CALL *getNextSwapchainImage)(PalSwapchain*, PalSwapchainNextImageInfo*, uint32_t*);
    PalResult (PAL_CALL *presentSwapchain)(PalSwapchain*, PalSwapchainPresentInfo*);
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
    PalResult (PAL_CALL *mapBufferMemory)(PalBuffer*, uint64_t, uint64_t, void**);
    void (PAL_CALL *unmapBufferMemory)(PalBuffer*);
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

#endif // _PAL_GRAPHICS_BACKENDS_H