
/**
 * @file vtable1.h
 * 
 * Copyright (C) 2025-2026 Nicholas Agbo <agbonicholas04@gmail.com>
 *
 * This software is provided 'as-is', without any express or implied
 * warranty.  In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 *
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 *
 * 3. This notice may not be removed or altered from any source distribution.
 */

#ifndef PAL_GRAPHICS_VTABLE1_H
#define PAL_GRAPHICS_VTABLE1_H

#include "types.h"

/**
 * @struct PalGraphicsBackendVtable1
 * @brief Graphics backend dispatch table version 1.
 *
 * This struct must remain valid until @ref palShutdownGraphics() is called.
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * @since Added in version 2.0
 * @ingroup pal_graphics
 */
typedef struct PalGraphicsBackendVtable1 {
    //clang-format off
    /** Implements @ref palEnumerateAdapters(). */
    PalResult(PAL_CALL* enumerateAdapters)(uint32_t*, PalAdapter**);
    /** Implements @ref palGetAdapterInfo(). */
    void(PAL_CALL* getAdapterInfo)(PalAdapter*, PalAdapterInfo*);
    /** Implements @ref palGetAdapterCapabilities(). */
    void(PAL_CALL* getAdapterCapabilities)(PalAdapter*, PalAdapterCapabilities*);
    /** Implements @ref palGetAdapterFeatures(). */
    PalAdapterFeatures(PAL_CALL* getAdapterFeatures)(PalAdapter*);
    /** Implements @ref palGetHighestSupportedShaderTarget(). */
    uint32_t(PAL_CALL* getHighestSupportedShaderTarget)(PalAdapter*, PalShaderFormats);
    /** Implements @ref palCreateDevice(). */
    PalResult(PAL_CALL* createDevice)(PalAdapter*, PalAdapterFeatures, PalDevice**);
    /** Implements @ref palDestroyDevice(). */
    void(PAL_CALL* destroyDevice)(PalDevice*);
    /** Implements @ref palGetDeviceLostReason(). */
    uint32_t(PAL_CALL* getDeviceLostReason)(PalDevice*);
    /** Implements @ref palAllocateMemory(). */
    PalResult(PAL_CALL* allocateMemory)(PalDevice*, PalMemoryType, uint64_t, uint64_t, PalMemory**);
    /** Implements @ref palFreeMemory(). */
    void(PAL_CALL* freeMemory)(PalMemory*);
    /** Implements @ref palQuerySamplerAnisotropyCapabilities(). */
    void(PAL_CALL* querySamplerAnisotropyCapabilities)(PalDevice*, PalSamplerAnisotropyCapabilities*);
    /** Implements @ref palQueryMultiViewCapabilities(). */
    void(PAL_CALL* queryMultiViewCapabilities)(PalDevice*, PalMultiViewCapabilities*);
    /** Implements @ref palQueryMultiViewportCapabilities(). */
    void(PAL_CALL* queryMultiViewportCapabilities)( PalDevice*, PalMultiViewportCapabilities*);
    /** Implements @ref palQueryDepthStencilCapabilities(). */
    void(PAL_CALL* queryDepthStencilCapabilities)(PalDevice*, PalDepthStencilCapabilities*);
    /** Implements @ref palQueryFragmentShadingRateCapabilities(). */
    void(PAL_CALL* queryFragmentShadingRateCapabilities)(PalDevice*, PalFragmentShadingRateCapabilities*);
    /** Implements @ref palQueryMeshShaderCapabilities(). */
    void(PAL_CALL* queryMeshShaderCapabilities)(PalDevice*, PalMeshShaderCapabilities*);
    /** Implements @ref palQueryRayTracingCapabilities(). */
    void(PAL_CALL* queryRayTracingCapabilities)(PalDevice*, PalRayTracingCapabilities*);
    /** Implements @ref palQueryDescriptorIndexingCapabilities(). */
    void(PAL_CALL* queryDescriptorIndexingCapabilities)(PalDevice*, PalDescriptorIndexingCapabilities*);
    /** Implements @ref palCreateQueue(). */
    PalResult(PAL_CALL* createQueue)(PalDevice*, PalQueueType, PalQueue**);
    /** Implements @ref palDestroyQueue(). */
    void(PAL_CALL* destroyQueue)(PalQueue*);
    /** Implements @ref palCanQueuePresent(). */
    PalBool(PAL_CALL* canQueuePresent)(PalQueue*, PalSurface*);
    /** Implements @ref palWaitQueue(). */
    PalResult(PAL_CALL* waitQueue)(PalQueue*);
    /** Implements @ref palEnumerateFormats(). */
    void(PAL_CALL* enumerateFormats)(PalAdapter*, uint32_t*, PalFormatInfo*);
    /** Implements @ref palIsFormatSupported(). */
    PalBool(PAL_CALL* isFormatSupported)(PalAdapter*, PalFormat);
    /** Implements @ref palQueryFormatImageUsages(). */
    PalImageUsages(PAL_CALL* queryFormatImageUsages)(PalAdapter*, PalFormat);
    /** Implements @ref palQueryFormatSampleCount(). */
    PalSampleCount(PAL_CALL* queryFormatSampleCount)(PalAdapter*, PalFormat);
    /** Implements @ref palCreateImage(). */
    PalResult(PAL_CALL* createImage)(PalDevice*, const PalImageCreateInfo*, PalImage**);
    /** Implements @ref palDestroyImage(). */
    void(PAL_CALL* destroyImage)(PalImage*);
    /** Implements @ref palGetImageInfo(). */
    void(PAL_CALL* getImageInfo)(PalImage*, PalImageInfo*);
    /** Implements @ref palGetImageMemoryRequirements(). */
    void(PAL_CALL* getImageMemoryRequirements)(PalImage*, PalMemoryRequirements*);
    /** Implements @ref palBindImageMemory(). */
    PalResult(PAL_CALL* bindImageMemory)(PalImage*, PalMemory*, uint64_t);
    /** Implements @ref palCreateImageView(). */
    PalResult(PAL_CALL* createImageView)(PalDevice*, PalImage*, const PalImageViewCreateInfo*, PalImageView**);
    /** Implements @ref palDestroyImageView(). */
    void(PAL_CALL* destroyImageView)(PalImageView*);
    /** Implements @ref palCreateSampler(). */
    PalResult(PAL_CALL* createSampler)(PalDevice*, const PalSamplerCreateInfo*, PalSampler**);
    /** Implements @ref palDestroySampler(). */
    void(PAL_CALL* destroySampler)(PalSampler*);
    /** Implements @ref palCreateSurface(). */
    PalResult(PAL_CALL* createSurface)(PalDevice*, void*, void*, PalWindowInstanceType, PalSurface**);
    /** Implements @ref palDestroySurface(). */
    void(PAL_CALL* destroySurface)(PalSurface*);
    /** Implements @ref palGetSurfaceCapabilities(). */
    void(PAL_CALL* getSurfaceCapabilities)(PalDevice*, PalSurface*, PalSurfaceCapabilities*);
    /** Implements @ref palCreateSwapchain(). */
    PalResult(PAL_CALL* createSwapchain)(PalDevice*, PalQueue*, PalSurface*, const PalSwapchainCreateInfo*, PalSwapchain**);
    /** Implements @ref palDestroySwapchain(). */
    void(PAL_CALL* destroySwapchain)(PalSwapchain*);
    /** Implements @ref palGetSwapchainImage(). */
    PalImage*(PAL_CALL* getSwapchainImage)(PalSwapchain*, uint32_t);
    /** Implements @ref palGetNextSwapchainImage(). */
    PalResult(PAL_CALL* getNextSwapchainImage)(PalSwapchain*, PalSwapchainNextImageInfo*, uint32_t*);
    /** Implements @ref palPresentSwapchain(). */
    PalResult(PAL_CALL* presentSwapchain)(PalSwapchain*, uint32_t, PalSemaphore*);
    /** Implements @ref palResizeSwapchain(). */
    PalResult(PAL_CALL* resizeSwapchain)(PalSwapchain*, uint32_t, uint32_t);
    /** Implements @ref palCreateShader(). */
    PalResult(PAL_CALL* createShader)(PalDevice*, const PalShaderCreateInfo*, PalShader**);
    /** Implements @ref palDestroyShader(). */
    void(PAL_CALL* destroyShader)(PalShader*);
    /** Implements @ref palCreateFence(). */
    PalResult(PAL_CALL* createFence)(PalDevice*, PalBool, PalFence**);
    /** Implements @ref palDestroyFence(). */
    void(PAL_CALL* destroyFence)(PalFence*);
    /** Implements @ref palWaitFence(). */
    PalResult(PAL_CALL* waitFence)(PalFence*, uint64_t);
    /** Implements @ref palResetFence(). */
    PalResult(PAL_CALL* resetFence)(PalFence*);
    /** Implements @ref palIsFenceSignaled(). */
    PalBool(PAL_CALL* isFenceSignaled)(PalFence*);
    /** Implements @ref palCreateSemaphore(). */
    PalResult(PAL_CALL* createSemaphore)(PalDevice*, PalBool, PalSemaphore**);
    /** Implements @ref palDestroySemaphore(). */
    void(PAL_CALL* destroySemaphore)(PalSemaphore*);
    /** Implements @ref palWaitSemaphore(). */
    PalResult(PAL_CALL* waitSemaphore)(PalSemaphore*, uint64_t, uint64_t);
    /** Implements @ref palSignalSemaphore(). */
    PalResult(PAL_CALL* signalSemaphore)(PalSemaphore*, PalQueue*, uint64_t);
    /** Implements @ref palGetSemaphoreValue(). */
    PalResult(PAL_CALL* getSemaphoreValue)(PalSemaphore*, uint64_t*);
    /** Implements @ref palCreateCommandPool(). */
    PalResult(PAL_CALL* createCommandPool)(PalDevice*, PalQueue*, PalCommandPool**);
    /** Implements @ref palDestroyCommandPool(). */
    void(PAL_CALL* destroyCommandPool)(PalCommandPool*);
    /** Implements @ref palAllocateCommandBuffer(). */
    PalResult(PAL_CALL* allocateCommandBuffer)(PalDevice*, PalCommandPool*, PalCommandBufferType, PalCommandBuffer**);
    /** Implements @ref palFreeCommandBuffer(). */
    void(PAL_CALL* freeCommandBuffer)(PalCommandBuffer*);
    /** Implements @ref palResetCommandBuffer(). */
    PalResult(PAL_CALL* resetCommandBuffer)(PalCommandBuffer*);
    /** Implements @ref palSubmitCommandBuffer(). */
    PalResult(PAL_CALL* submitCommandBuffer)(PalQueue*, PalCommandBufferSubmitInfo*);
    /** Implements @ref palCmdBegin(). */
    PalResult(PAL_CALL* cmdBegin)(PalCommandBuffer*, PalRenderingLayoutInfo*);
    /** Implements @ref palCmdEnd(). */
    PalResult(PAL_CALL* cmdEnd)(PalCommandBuffer*);
    /** Implements @ref palCmdExecuteCommandBuffer(). */
    void(PAL_CALL* cmdExecuteCommandBuffer)(PalCommandBuffer*, PalCommandBuffer*);
    /** Implements @ref palCmdSetFragmentShadingRate(). */
    void(PAL_CALL* cmdSetFragmentShadingRate)(PalCommandBuffer*, PalFragmentShadingRateState*);
    /** Implements @ref palCmdDrawMeshTasks(). */
    void(PAL_CALL* cmdDrawMeshTasks)(PalCommandBuffer*, uint32_t, uint32_t, uint32_t);
    /** Implements @ref palCmdDrawMeshTasksIndirect(). */
    void(PAL_CALL* cmdDrawMeshTasksIndirect)(PalCommandBuffer*, PalBuffer*, uint32_t);
    /** Implements @ref palCmdDrawMeshTasksIndirectCount(). */
    void(PAL_CALL* cmdDrawMeshTasksIndirectCount)(PalCommandBuffer*, PalBuffer*, PalBuffer*, uint32_t);
    /** Implements @ref palCmdBuildAccelerationStructure(). */
    void(PAL_CALL* cmdBuildAccelerationStructure)(PalCommandBuffer*, PalAccelerationStructureBuildInfo*);
    /** Implements @ref palCmdBeginRendering(). */
    void(PAL_CALL* cmdBeginRendering)(PalCommandBuffer*, PalRenderingInfo*);
    /** Implements @ref palCmdEndRendering(). */
    void(PAL_CALL* cmdEndRendering)(PalCommandBuffer*);
    /** Implements @ref palCmdCopyBuffer(). */
    void(PAL_CALL* cmdCopyBuffer)(PalCommandBuffer*, PalBuffer*, PalBuffer*, PalBufferCopyInfo*);
    /** Implements @ref palCmdCopyBufferToImage(). */
    void(PAL_CALL* cmdCopyBufferToImage)(PalCommandBuffer*, PalImage*, PalBuffer*, PalBufferImageCopyInfo*);
    /** Implements @ref palCmdCopyImage(). */
    void(PAL_CALL* cmdCopyImage)(PalCommandBuffer*, PalImage*, PalImage*, PalImageCopyInfo*);
    /** Implements @ref palCmdCopyImageToBuffer(). */
    void(PAL_CALL* cmdCopyImageToBuffer)(PalCommandBuffer*, PalBuffer*, PalImage*, PalBufferImageCopyInfo*);
    /** Implements @ref palCmdBindPipeline(). */
    void(PAL_CALL* cmdBindPipeline)(PalCommandBuffer*, PalPipeline*);
    /** Implements @ref palCmdSetViewport(). */
    void(PAL_CALL* cmdSetViewport)(PalCommandBuffer*, uint32_t, PalViewport*);
    /** Implements @ref palCmdSetScissors(). */
    void(PAL_CALL* cmdSetScissors)(PalCommandBuffer*, uint32_t, PalRect2D*);
    /** Implements @ref palCmdBindVertexBuffers(). */
    void(PAL_CALL* cmdBindVertexBuffers)(PalCommandBuffer*, uint32_t, uint32_t, PalBuffer**, uint64_t*);
    /** Implements @ref palCmdBindIndexBuffer(). */
    void(PAL_CALL* cmdBindIndexBuffer)(PalCommandBuffer*, PalBuffer*, uint64_t, PalIndexType);
    /** Implements @ref palCmdDraw(). */
    void(PAL_CALL* cmdDraw)(PalCommandBuffer*, uint32_t, uint32_t, uint32_t, uint32_t);
    /** Implements @ref palCmdDrawIndirect(). */
    void(PAL_CALL* cmdDrawIndirect)(PalCommandBuffer*, PalBuffer*, uint32_t);
    /** Implements @ref palCmdDrawIndirectCount(). */
    void(PAL_CALL* cmdDrawIndirectCount)(PalCommandBuffer*, PalBuffer*, PalBuffer*, uint32_t);
    /** Implements @ref palCmdDrawIndexed(). */
    void(PAL_CALL* cmdDrawIndexed)(PalCommandBuffer*, uint32_t, uint32_t, uint32_t, int32_t, uint32_t);
    /** Implements @ref palCmdDrawIndexedIndirect(). */
    void(PAL_CALL* cmdDrawIndexedIndirect)(PalCommandBuffer*, PalBuffer*, uint32_t);
    /** Implements @ref palCmdDrawIndexedIndirectCount(). */
    void(PAL_CALL* cmdDrawIndexedIndirectCount)(PalCommandBuffer*, PalBuffer*, PalBuffer*, uint32_t);
    /** Implements @ref palCmdAccelerationStructureBarrier(). */
    void(PAL_CALL* cmdAccelerationStructureBarrier)(PalCommandBuffer*, PalAccelerationStructure*, PalBarrierInfo*);
    /** Implements @ref palCmdImageBarrier(). */
    void(PAL_CALL* cmdImageBarrier)(PalCommandBuffer*, PalImage*, PalImageSubresourceRange*, PalBarrierInfo*);
    /** Implements @ref palCmdBufferBarrier(). */
    void(PAL_CALL* cmdBufferBarrier)(PalCommandBuffer*, PalBuffer*, PalBarrierInfo*);
    /** Implements @ref palCmdDispatch(). */
    void(PAL_CALL* cmdDispatch)(PalCommandBuffer*, uint32_t, uint32_t, uint32_t);
    /** Implements @ref palCmdDispatchBase(). */
    void(PAL_CALL* cmdDispatchBase)(PalCommandBuffer*, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t);
    /** Implements @ref palCmdDispatchIndirect(). */
    void(PAL_CALL* cmdDispatchIndirect)(PalCommandBuffer*, PalBuffer*);
    /** Implements @ref palCmdTraceRays(). */
    void(PAL_CALL* cmdTraceRays)(PalCommandBuffer*, PalShaderBindingTable*, uint32_t, uint32_t, uint32_t, uint32_t);
    /** Implements @ref palCmdTraceRaysIndirect(). */
    void(PAL_CALL* cmdTraceRaysIndirect)(PalCommandBuffer*, uint32_t, PalShaderBindingTable*, PalBuffer*);
    /** Implements @ref palCmdBindDescriptorSet(). */
    void(PAL_CALL* cmdBindDescriptorSet)(PalCommandBuffer*, uint32_t, PalDescriptorSet*);
    /** Implements @ref palCmdPushConstants(). */
    void(PAL_CALL* cmdPushConstants)(PalCommandBuffer*, uint32_t, uint32_t, const void*);
    /** Implements @ref palCmdSetCullMode(). */
    void(PAL_CALL* cmdSetCullMode)(PalCommandBuffer*, PalCullMode);
    /** Implements @ref palCmdSetFrontFace(). */
    void(PAL_CALL* cmdSetFrontFace)(PalCommandBuffer*, PalFrontFace);
    /** Implements @ref palCmdSetPrimitiveTopology(). */
    void(PAL_CALL* cmdSetPrimitiveTopology)(PalCommandBuffer*, PalPrimitiveTopology);
    /** Implements @ref palCmdSetDepthTestEnable(). */
    void(PAL_CALL* cmdSetDepthTestEnable)(PalCommandBuffer*, PalBool);
    /** Implements @ref palCmdSetDepthWriteEnable(). */
    void(PAL_CALL* cmdSetDepthWriteEnable)(PalCommandBuffer*, PalBool);
    /** Implements @ref palCmdSetStencilOp(). */
    void(PAL_CALL* cmdSetStencilOp)(PalCommandBuffer*, PalStencilFaceFlags, PalStencilOp, PalStencilOp, PalStencilOp, PalCompareOp);
    /** Implements @ref palCreateAccelerationstructure(). */
    PalResult(PAL_CALL* createAccelerationstructure)(PalDevice*, const PalAccelerationStructureCreateInfo*, PalAccelerationStructure**);
    /** Implements @ref palDestroyAccelerationStructure(). */
    void(PAL_CALL* destroyAccelerationstructure)(PalAccelerationStructure*);
    /** Implements @ref palGetAccelerationStructureBuildSize(). */
    void(PAL_CALL* getAccelerationStructureBuildSize)(PalDevice*, PalAccelerationStructureBuildInfo*, PalAccelerationStructureBuildSize*);
    /** Implements @ref palCreateBuffer(). */
    PalResult(PAL_CALL* createBuffer)(PalDevice*, const PalBufferCreateInfo*, PalBuffer**);
    /** Implements @ref palDestroyBuffer(). */
    void(PAL_CALL* destroyBuffer)(PalBuffer*);
    /** Implements @ref palGetBufferMemoryRequirements(). */
    void(PAL_CALL* getBufferMemoryRequirements)(PalBuffer*, PalMemoryRequirements*);
    /** Implements @ref palComputeInstanceStagingSize(). */
    void(PAL_CALL* computeInstanceStagingSize)(PalDevice*, uint32_t, uint64_t*);
    /** Implements @ref palComputeImageStagingRequirements(). */
    void(PAL_CALL* computeImageStagingRequirements)(PalDevice*, PalFormat, const PalBufferImageCopyInfo*, PalImageStagingRequirements*);
    /** Implements @ref palWriteInstanceStaging(). */
    void(PAL_CALL* writeInstanceStaging)(PalDevice*, uint32_t, PalAccelerationStructureInstance*, void*);
    /** Implements @ref palWriteImageStaging(). */
    void(PAL_CALL* writeImageStaging)(PalDevice*, PalFormat, PalBufferImageCopyInfo*, void*, void*);
    /** Implements @ref palBindBufferMemory(). */
    PalResult(PAL_CALL* bindBufferMemory)(PalBuffer*, PalMemory*, uint64_t);
    /** Implements @ref palMapBuffer(). */
    PalResult(PAL_CALL* mapBuffer)(PalBuffer*, uint64_t, uint64_t, void**);
    /** Implements @ref palUnmapBuffer(). */
    void(PAL_CALL* unmapBuffer)(PalBuffer*);
    /** Implements @ref palGetBufferDeviceAddress(). */
    PalDeviceAddress(PAL_CALL* getBufferDeviceAddress)(PalBuffer*);
    /** Implements @ref palCreateDescriptorSetLayout(). */
    PalResult(PAL_CALL* createDescriptorSetLayout)(PalDevice*, const PalDescriptorSetLayoutCreateInfo*, PalDescriptorSetLayout**);
    /** Implements @ref palDestroyDescriptorSetLayout(). */
    void(PAL_CALL* destroyDescriptorSetLayout)(PalDescriptorSetLayout*);
    /** Implements @ref palCreateDescriptorPool(). */
    PalResult(PAL_CALL* createDescriptorPool)(PalDevice*, const PalDescriptorPoolCreateInfo*, PalDescriptorPool**);
    /** Implements @ref palDestroyDescriptorPool(). */
    void(PAL_CALL* destroyDescriptorPool)(PalDescriptorPool*);
    /** Implements @ref palResetDescriptorPool(). */
    PalResult(PAL_CALL* resetDescriptorPool)(PalDescriptorPool*);
    /** Implements @ref palAllocateDescriptorSet(). */
    PalResult(PAL_CALL* allocateDescriptorSet)(PalDevice*, PalDescriptorPool*, PalDescriptorSetLayout*, PalDescriptorSet**);
    /** Implements @ref palUpdateDescriptorSet(). */
    PalResult(PAL_CALL* updateDescriptorSet)(PalDevice*, uint32_t, PalDescriptorSetWriteInfo*);
    /** Implements @ref palCreatePipelineLayout(). */
    PalResult(PAL_CALL* createPipelineLayout)(PalDevice*, const PalPipelineLayoutCreateInfo*, PalPipelineLayout**);
    /** Implements @ref palDestroyPipelineLayout(). */
    void(PAL_CALL* destroyPipelineLayout)(PalPipelineLayout*);
    /** Implements @ref palCreateGraphicsPipeline(). */
    PalResult(PAL_CALL* createGraphicsPipeline)(PalDevice*, const PalGraphicsPipelineCreateInfo*, PalPipeline**);
    /** Implements @ref palCreateComputePipeline(). */
    PalResult(PAL_CALL* createComputePipeline)(PalDevice*, const PalComputePipelineCreateInfo*, PalPipeline**);
    /** Implements @ref palCreateRayTracingPipeline(). */
    PalResult(PAL_CALL* createRayTracingPipeline)(PalDevice*, const PalRayTracingPipelineCreateInfo*, PalPipeline**);
    /** Implements @ref palDestroyPipeline(). */
    void(PAL_CALL* destroyPipeline)(PalPipeline*);
    /** Implements @ref palCreateShaderBindingTable(). */
    PalResult(PAL_CALL* createShaderBindingTable)(PalDevice*, const PalShaderBindingTableCreateInfo*, PalShaderBindingTable**);
    /** Implements @ref palDestroyShaderBindingTable(). */
    void(PAL_CALL* destroyShaderBindingTable)(PalShaderBindingTable*);
    /** Implements @ref palUpdateShaderBindingTable(). */
    void(PAL_CALL* updateShaderBindingTable)(PalShaderBindingTable*, uint32_t, PalShaderBindingTableRecordInfo*);
    //clang-format on
} PalGraphicsBackendVtable1;

#endif // PAL_GRAPHICS_VTABLE1_H