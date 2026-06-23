
/**
    PAL - Prime Abstraction Layer
    Copyright (C) 2025
    Licensed under the Zlib license. See LICENSE file in root.
 */

#ifndef _PAL_GRAPHICS_VTABLE_H
#define _PAL_GRAPHICS_VTABLE_H

#include "pal/pal_graphics.h"

typedef struct {
    PalResult (PAL_CALL *enumerateAdapters)(
        int32_t* count,
        PalAdapter** outAdapters);

    PalResult (PAL_CALL *getAdapterInfo)(
        PalAdapter* adapter,
        PalAdapterInfo* info);

    PalResult (PAL_CALL *getAdapterCapabilities)(
        PalAdapter* adapter,
        PalAdapterCapabilities* caps);

    PalAdapterFeatures (PAL_CALL *getAdapterFeatures)(PalAdapter* adapter);

    uint32_t (PAL_CALL *getHighestSupportedShaderTarget)(
        PalAdapter* adapter, 
        PalShaderFormats shaderFormat);

    PalResult (PAL_CALL *createDevice)(
        PalAdapter* adapter,
        PalAdapterFeatures features,
        PalDevice** outDevice);

    void (PAL_CALL *destroyDevice)(PalDevice* device);

    PalResult (PAL_CALL *allocateMemory)(
        PalDevice* device,
        PalMemoryType type,
        uint64_t memoryMask,
        uint64_t size,
        PalMemory** outMemory);

    void (PAL_CALL *freeMemory)(
        PalDevice* device,
        PalMemory* memory);

    PalResult (PAL_CALL *querySamplerAnisotropyCapabilities)(
        PalDevice* device,
        PalSamplerAnisotropyCapabilities* caps);

    PalResult (PAL_CALL *queryMultiViewCapabilities)(
        PalDevice* device,
        PalMultiViewCapabilities* caps);

    PalResult (PAL_CALL *queryMultiViewportCapabilities)(
        PalDevice* device,
        PalMultiViewportCapabilities* caps);

    PalResult (PAL_CALL *queryDepthStencilCapabilities)(
        PalDevice* device,
        PalDepthStencilCapabilities* caps);
    PalResult (PAL_CALL *queryFragmentShadingRateCapabilities)(
        PalDevice* device,
        PalFragmentShadingRateCapabilities* caps);

    PalResult (PAL_CALL *queryMeshShaderCapabilities)(
        PalDevice* device,
        PalMeshShaderCapabilities* caps);

    PalResult (PAL_CALL *queryRayTracingCapabilities)(
        PalDevice* device,
        PalRayTracingCapabilities* caps);

    PalResult (PAL_CALL *queryDescriptorIndexingCapabilities)(
        PalDevice* device,
        PalDescriptorIndexingCapabilities* caps);

    PalResult (PAL_CALL *createQueue)(
        PalDevice* device,
        PalQueueType type,
        PalQueue** outQueue);

    void (PAL_CALL *destroyQueue)(PalQueue* queue);

    PalBool (PAL_CALL *canQueuePresent)(
        PalQueue* queue,
        PalSurface* surface);

    PalResult (PAL_CALL *waitQueue)(PalQueue* queue);

    PalResult (PAL_CALL *enumerateFormats)(
        PalAdapter* adapter,
        int32_t* count,
        PalFormatInfo* outFormats);

    PalBool (PAL_CALL *isFormatSupported)(
        PalAdapter* adapter,
        PalFormat format);

    PalImageUsages (PAL_CALL *queryFormatImageUsages)(
        PalAdapter* adapter,
        PalFormat format);

    PalSampleCount (PAL_CALL *queryFormatSampleCount)(
        PalAdapter* adapter,
        PalFormat format);

    PalResult (PAL_CALL *createImage)(
        PalDevice* device,
        const PalImageCreateInfo* info,
        PalImage** outImage);

    void (PAL_CALL *destroyImage)(PalImage* image);

    PalResult (PAL_CALL *getImageInfo)(
        PalImage* image,
        PalImageInfo* info);

    PalResult (PAL_CALL *getImageMemoryRequirements)(
        PalImage* image,
        PalMemoryRequirements* requirements);

    PalResult (PAL_CALL *bindImageMemory)(
        PalImage* image,
        PalMemory* memory,
        uint64_t offset);

    PalResult (PAL_CALL *mapImageMemory)(
        PalImage* image,
        uint64_t offset,
        uint64_t size,
        void** outPtr);

    void (PAL_CALL *unmapImageMemory)(PalImage* image);

    PalResult (PAL_CALL *createImageView)(
        PalDevice* device,
        PalImage* image,
        const PalImageViewCreateInfo* info,
        PalImageView** outImageView);

    void (PAL_CALL *destroyImageView)(PalImageView* imageView);

    PalResult (PAL_CALL *createSampler)(
        PalDevice* device,
        const PalSamplerCreateInfo* info,
        PalSampler** outSampler);

    void (PAL_CALL *destroySampler)(PalSampler* sampler);

    PalResult (PAL_CALL *createSurface)(
        PalDevice* device,
        void* window,
        void* windowInstance,
        PalWindowInstanceType instanceType,
        PalSurface** outSurface);

    void (PAL_CALL *destroySurface)(PalSurface* surface);

    PalResult (PAL_CALL *getSurfaceCapabilities)(
        PalDevice* device,
        PalSurface* surface,
        PalSurfaceCapabilities* caps);

    PalResult (PAL_CALL *createSwapchain)(
        PalDevice* device,
        PalQueue* queue,
        PalSurface* surface,
        const PalSwapchainCreateInfo* info,
        PalSwapchain** outSwapchain);

    void (PAL_CALL *destroySwapchain)(PalSwapchain* swapchain);

    PalImage* (PAL_CALL *getSwapchainImage)(
        PalSwapchain* swapchain,
        int32_t index);

    PalResult (PAL_CALL *getNextSwapchainImage)(
        PalSwapchain* swapchain,
        PalSwapchainNextImageInfo* info,
        uint32_t* outIndex);

    PalResult (PAL_CALL *presentSwapchain)(
        PalSwapchain* swapchain,
        PalSwapchainPresentInfo* info);

    PalResult (PAL_CALL *resizeSwapchain)(
        PalSwapchain* swapchain,
        uint32_t newWidth,
        uint32_t newHeight);

    PalResult (PAL_CALL *createShader)(
        PalDevice* device,
        const PalShaderCreateInfo* info,
        PalShader** outShader);

    void (PAL_CALL *destroyShader)(PalShader* shader);

    PalResult (PAL_CALL *createFence)(
        PalDevice* device,
        PalBool signaled,
        PalFence** outFence);

    void (PAL_CALL *destroyFence)(PalFence* fence);

    PalResult (PAL_CALL *waitFence)(
        PalFence* fence,
        uint64_t timeout);

    PalResult (PAL_CALL *resetFence)(PalFence* fence);

    PalBool (PAL_CALL *isFenceSignaled)(PalFence* fence);

    PalResult (PAL_CALL *createSemaphore)(
        PalDevice* device,
        PalBool enableTimeline,
        PalSemaphore** outSemaphore);

    void (PAL_CALL *destroySemaphore)(PalSemaphore* semaphore);

    PalResult (PAL_CALL *waitSemaphore)(
        PalSemaphore* semaphore,
        uint64_t value,
        uint64_t timeout);

    PalResult (PAL_CALL *signalSemaphore)(
        PalSemaphore* semaphore,
        PalQueue* queue,
        uint64_t value);

    PalResult (PAL_CALL *getSemaphoreValue)(
        PalSemaphore* semaphore,
        uint64_t* outValue);

    PalResult (PAL_CALL *createCommandPool)(
        PalDevice* device,
        PalQueue* queue,
        PalCommandPool** outPool);

    void (PAL_CALL *destroyCommandPool)(PalCommandPool* pool);

    PalResult (PAL_CALL *resetCommandPool)(PalCommandPool* pool);

    PalResult (PAL_CALL *allocateCommandBuffer)(
        PalDevice* device,
        PalCommandPool* pool,
        PalCommandBufferType type,
        PalCommandBuffer** outCmdBuffer);

    void (PAL_CALL *freeCommandBuffer)(PalCommandBuffer* cmdBuffer);

    PalResult (PAL_CALL *resetCommandBuffer)(PalCommandBuffer* cmdBuffer);

    PalResult (PAL_CALL *submitCommandBuffer)(
        PalQueue* queue,
        PalCommandBufferSubmitInfo* info);

    PalResult (PAL_CALL *cmdBegin)(
        PalCommandBuffer* cmdBuffer,
        PalRenderingLayoutInfo* info);

    PalResult (PAL_CALL *cmdEnd)(PalCommandBuffer* cmdBuffer);

    PalResult (PAL_CALL *cmdExecuteCommandBuffer)(
        PalCommandBuffer* primaryCmdBuffer,
        PalCommandBuffer* secondaryCmdBuffer);

    PalResult (PAL_CALL *cmdSetFragmentShadingRate)(
        PalCommandBuffer* cmdBuffer,
        PalFragmentShadingRateState* state);

    PalResult (PAL_CALL *cmdDrawMeshTasks)(
        PalCommandBuffer* cmdBuffer,
        uint32_t groupCountX,
        uint32_t groupCountY,
        uint32_t groupCountZ);

    PalResult (PAL_CALL *cmdDrawMeshTasksIndirect)(
        PalCommandBuffer* cmdBuffer,
        PalBuffer* buffer,
        uint32_t drawCount);

    PalResult (PAL_CALL *cmdDrawMeshTasksIndirectCount)(
        PalCommandBuffer* cmdBuffer,
        PalBuffer* buffer,
        PalBuffer* countBuffer,
        uint32_t maxDrawCount);

    PalResult (PAL_CALL *cmdBuildAccelerationStructure)(
        PalCommandBuffer* cmdBuffer,
        PalAccelerationStructureBuildInfo* info);

    PalResult (PAL_CALL *cmdBeginRendering)(
        PalCommandBuffer* cmdBuffer,
        PalRenderingInfo* info);

    PalResult (PAL_CALL *cmdEndRendering)(PalCommandBuffer* cmdBuffer);

    PalResult (PAL_CALL *cmdCopyBuffer)(
        PalCommandBuffer* cmdBuffer,
        PalBuffer* dst,
        PalBuffer* src,
        PalBufferCopyInfo* copyInfo);

    PalResult (PAL_CALL *cmdCopyBufferToImage)(
        PalCommandBuffer* cmdBuffer,
        PalImage* dstImage,
        PalBuffer* srcBuffer,
        PalBufferImageCopyInfo* copyInfo);

    PalResult (PAL_CALL *cmdCopyImage)(
        PalCommandBuffer* cmdBuffer,
        PalImage* dst,
        PalImage* src,
        PalImageCopyInfo* copyInfo);

    PalResult (PAL_CALL *cmdCopyImageToBuffer)(
        PalCommandBuffer* cmdBuffer,
        PalBuffer* dstBuffer,
        PalImage* srcImage,
        PalBufferImageCopyInfo* copyInfo);

    PalResult (PAL_CALL *cmdBindPipeline)(
        PalCommandBuffer* cmdBuffer,
        PalPipeline* pipeline);

    PalResult (PAL_CALL *cmdSetViewport)(
        PalCommandBuffer* cmdBuffer,
        uint32_t count,
        PalViewport* viewports);

    PalResult (PAL_CALL *cmdSetScissors)(
        PalCommandBuffer* cmdBuffer,
        uint32_t count,
        PalRect2D* scissors);

    PalResult (PAL_CALL *cmdBindVertexBuffers)(
        PalCommandBuffer* cmdBuffer,
        uint32_t firstSlot,
        uint32_t count,
        PalBuffer** buffers,
        uint64_t* offsets);

    PalResult (PAL_CALL *cmdBindIndexBuffer)(
        PalCommandBuffer* cmdBuffer,
        PalBuffer* buffer,
        uint64_t offset,
        PalIndexType type);

    PalResult (PAL_CALL *cmdDraw)(
        PalCommandBuffer* cmdBuffer,
        uint32_t vertexCount,
        uint32_t instanceCount,
        uint32_t firstVertex,
        uint32_t firstInstance);

    PalResult (PAL_CALL *cmdDrawIndirect)(
        PalCommandBuffer* cmdBuffer,
        PalBuffer* buffer,
        uint32_t count);

    PalResult (PAL_CALL *cmdDrawIndirectCount)(
        PalCommandBuffer* cmdBuffer,
        PalBuffer* buffer,
        PalBuffer* countBuffer,
        uint32_t count);

    PalResult (PAL_CALL *cmdDrawIndexed)(
        PalCommandBuffer* cmdBuffer,
        uint32_t indexCount,
        uint32_t instanceCount,
        uint32_t firstIndex,
        int32_t vertexOffset,
        uint32_t firstInstance);

    PalResult (PAL_CALL *cmdDrawIndexedIndirect)(
        PalCommandBuffer* cmdBuffer,
        PalBuffer* buffer,
        uint32_t count);

    PalResult (PAL_CALL *cmdDrawIndexedIndirectCount)(
        PalCommandBuffer* cmdBuffer,
        PalBuffer* buffer,
        PalBuffer* countBuffer,
        uint32_t count);

    PalResult (PAL_CALL *cmdAccelerationStructureBarrier)(
        PalCommandBuffer* cmdBuffer,
        PalAccelerationStructure* as,
        PalUsageState oldUsageState,
        PalUsageState newUsageState);

    PalResult (PAL_CALL *cmdImageBarrier)(
        PalCommandBuffer* cmdBuffer,
        PalImage* image,
        PalImageSubresourceRange* subresourceRange,
        PalUsageState oldUsageState,
        PalUsageState newUsageState);

    PalResult (PAL_CALL *cmdBufferBarrier)(
        PalCommandBuffer* cmdBuffer,
        PalBuffer* buffer,
        PalUsageState oldUsageState,
        PalUsageState newUsageState);

    PalResult (PAL_CALL *cmdDispatch)(
        PalCommandBuffer* cmdBuffer,
        uint32_t groupCountX,
        uint32_t groupCountY,
        uint32_t groupCountZ);

    PalResult (PAL_CALL *cmdDispatchBase)(
        PalCommandBuffer* cmdBuffer,
        uint32_t baseGroupX,
        uint32_t baseGroupY,
        uint32_t baseGroupZ,
        uint32_t groupCountX,
        uint32_t groupCountY,
        uint32_t groupCountZ);

    PalResult (PAL_CALL *cmdDispatchIndirect)(
        PalCommandBuffer* cmdBuffer,
        PalBuffer* buffer);

    PalResult (PAL_CALL *cmdTraceRays)(
        PalCommandBuffer* cmdBuffer,
        PalShaderBindingTable* sbt,
        uint32_t raygenIndex,
        uint32_t width,
        uint32_t height,
        uint32_t depth);

    PalResult (PAL_CALL *cmdTraceRaysIndirect)(
        PalCommandBuffer* cmdBuffer,
        uint32_t raygenIndex,
        PalShaderBindingTable* sbt,
        PalBuffer* buffer);

    PalResult (PAL_CALL *cmdBindDescriptorSet)(
        PalCommandBuffer* cmdBuffer,
        uint32_t setIndex,
        PalDescriptorSet* set);

    PalResult (PAL_CALL *cmdPushConstants)(
        PalCommandBuffer* cmdBuffer,
        uint32_t shaderStageCount,
        PalShaderStage* shaderStages,
        uint32_t offset,
        uint32_t size,
        const void* value);

    PalResult (PAL_CALL *cmdSetCullMode)(
        PalCommandBuffer* cmdBuffer,
        PalCullMode cullMode);

    PalResult (PAL_CALL *cmdSetFrontFace)(
        PalCommandBuffer* cmdBuffer,
        PalFrontFace frontFace);

    PalResult (PAL_CALL *cmdSetPrimitiveTopology)(
        PalCommandBuffer* cmdBuffer,
        PalPrimitiveTopology topology);

    PalResult (PAL_CALL *cmdSetDepthTestEnable)(
        PalCommandBuffer* cmdBuffer,
        PalBool enable);

    PalResult (PAL_CALL *cmdSetDepthWriteEnable)(
        PalCommandBuffer* cmdBuffer,
        PalBool enable);

    PalResult (PAL_CALL *cmdSetStencilOp)(
        PalCommandBuffer* cmdBuffer,
        PalStencilFaceFlags faceMask,
        PalStencilOp failOp,
        PalStencilOp passOp,
        PalStencilOp depthFailOp,
        PalCompareOp compareOp);

    PalResult (PAL_CALL *createAccelerationstructure)(
        PalDevice* device,
        const PalAccelerationStructureCreateInfo* info,
        PalAccelerationStructure** outAs);

    void (PAL_CALL *destroyAccelerationstructure)(PalAccelerationStructure* as);

    PalResult (PAL_CALL *getAccelerationStructureBuildSize)(
        PalDevice* device,
        PalAccelerationStructureBuildInfo* info,
        PalAccelerationStructureBuildSize* size);

    PalResult (PAL_CALL *createBuffer)(
        PalDevice* device,
        const PalBufferCreateInfo* info,
        PalBuffer** outBuffer);

    void (PAL_CALL *destroyBuffer)(PalBuffer* buffer);

    PalResult (PAL_CALL *getBufferMemoryRequirements)(
        PalBuffer* buffer,
        PalMemoryRequirements* requirements);

    PalResult (PAL_CALL *computeInstanceBufferRequirements)(
        PalDevice* device,
        uint32_t instanceCount,
        uint64_t* outSize);

    PalResult (PAL_CALL *computeImageCopyStagingBufferRequirements)(
        PalDevice* device,
        PalFormat imageFormat,
        PalBufferImageCopyInfo* copyInfo,
        uint32_t* outBufferRowLength,
        uint32_t* outBufferImageHeight,
        uint64_t* outSize);

    PalResult (PAL_CALL *writeToInstanceBuffer)(
        PalDevice* device,
        void* ptr,
        PalAccelerationStructureInstance* instances,
        uint32_t instanceCount);

    PalResult (PAL_CALL *writeToImageCopyStagingBuffer)(
        PalDevice* device,
        void* ptr,
        void* srcData,
        PalFormat imageFormat,
        PalBufferImageCopyInfo* copyInfo);

    PalResult (PAL_CALL *bindBufferMemory)(
        PalBuffer* buffer,
        PalMemory* memory,
        uint64_t offset);

    PalResult (PAL_CALL *mapBufferMemory)(
        PalBuffer* buffer,
        uint64_t offset,
        uint64_t size,
        void** outPtr);

    void (PAL_CALL *unmapBufferMemory)(PalBuffer* buffer);

    PalDeviceAddress (PAL_CALL *getBufferDeviceAddress)(PalBuffer* buffer);

    PalResult (PAL_CALL *createDescriptorSetLayout)(
        PalDevice* device,
        const PalDescriptorSetLayoutCreateInfo* info,
        PalDescriptorSetLayout** outLayout);

    void (PAL_CALL *destroyDescriptorSetLayout)(PalDescriptorSetLayout* layout);

    PalResult (PAL_CALL *createDescriptorPool)(
        PalDevice* device,
        const PalDescriptorPoolCreateInfo* info,
        PalDescriptorPool** outPool);

    void (PAL_CALL *destroyDescriptorPool)(PalDescriptorPool* pool);

    PalResult (PAL_CALL *resetDescriptorPool)(PalDescriptorPool* pool);

    PalResult (PAL_CALL *allocateDescriptorSet)(
        PalDevice* device,
        PalDescriptorPool* pool,
        PalDescriptorSetLayout* layout,
        PalDescriptorSet** outSet);

    PalResult (PAL_CALL *updateDescriptorSet)(
        PalDevice* device,
        uint32_t count,
        PalDescriptorSetWriteInfo* infos);

    PalResult (PAL_CALL *createPipelineLayout)(
        PalDevice* device,
        const PalPipelineLayoutCreateInfo* info,
        PalPipelineLayout** outLayout);

    void (PAL_CALL *destroyPipelineLayout)(PalPipelineLayout* layout);

    PalResult (PAL_CALL *createGraphicsPipeline)(
        PalDevice* device,
        const PalGraphicsPipelineCreateInfo* info,
        PalPipeline** outPipeline);

    PalResult (PAL_CALL *createComputePipeline)(
        PalDevice* device,
        const PalComputePipelineCreateInfo* info,
        PalPipeline** outPipeline);

    PalResult (PAL_CALL *createRayTracingPipeline)(
        PalDevice* device,
        const PalRayTracingPipelineCreateInfo* info,
        PalPipeline** outPipeline);

    void (PAL_CALL *destroyPipeline)(PalPipeline* pipeline);

    PalResult (PAL_CALL *createShaderBindingTable)(
        PalDevice* device,
        const PalShaderBindingTableCreateInfo* info,
        PalShaderBindingTable** outSbt);

    void (PAL_CALL *destroyShaderBindingTable)(PalShaderBindingTable* sbt);

    PalResult (PAL_CALL *updateShaderBindingTable)(
        PalShaderBindingTable* sbt, 
        uint32_t count,
        PalShaderBindingTableRecordInfo* infos);
} PalGraphicsVtable;

#endif // _PAL_GRAPHICS_VTABLE_H