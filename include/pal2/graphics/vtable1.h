/**
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

#ifndef PAL_GRAPHICS_VTABLE_1_H
#define PAL_GRAPHICS_VTABLE_1_H

#include "device.h"
#include "cmdbuffer.h"
#include "shader.h"
#include "acceleration_structure.h"
#include "pipeline.h"
#include "swapchain.h"
#include "sampler.h"

/**
 * @struct PalGraphicsBackendVtable1
 * @brief Version 1 dispatch table for PAL graphics system backends.
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * @since Added in version 2.0
 */
typedef struct PalGraphicsBackendVtable1 {
    /**
     * Backend implementation of ::palEnumerateAdapters.
     *
     * Must obey the rules and semantics documented in palEnumerateAdapters().
     */
    PalResult(PAL_CALL* enumerateAdapters)(
        uint32_t* count,
        PalAdapter** outAdapters);

    /**
     * Backend implementation of ::palGetAdapterInfo.
     *
     * Must obey the rules and semantics documented in palGetAdapterInfo().
     */
    void(PAL_CALL* getAdapterInfo)(
        PalAdapter* adapter,
        PalAdapterInfo* info);

    /**
     * Backend implementation of ::palGetAdapterCapabilities.
     *
     * Must obey the rules and semantics documented in palGetAdapterCapabilities().
     */
    void(PAL_CALL* getAdapterCapabilities)(
        PalAdapter* adapter,
        PalAdapterCapabilities* caps);

    /**
     * Backend implementation of ::palGetAdapterFeatures.
     *
     * Must obey the rules and semantics documented in palGetAdapterFeatures().
     */
    PalAdapterFeatures(PAL_CALL* getAdapterFeatures)(PalAdapter* adapter);

    /**
     * Backend implementation of ::palGetHighestSupportedShaderTarget.
     *
     * Must obey the rules and semantics documented in palGetHighestSupportedShaderTarget().
     */
    uint32_t(PAL_CALL* getHighestSupportedShaderTarget)(
        PalAdapter* adapter,
        PalShaderFormats shaderFormat);

    /**
     * Backend implementation of ::palCreateDevice.
     *
     * Must obey the rules and semantics documented in palCreateDevice().
     */
    PalResult(PAL_CALL* createDevice)(
        PalAdapter* adapter,
        PalAdapterFeatures features,
        PalDevice** outDevice);

    /**
     * Backend implementation of ::palDestroyDevice.
     *
     * Must obey the rules and semantics documented in palDestroyDevice().
     */
    void(PAL_CALL* destroyDevice)(PalDevice* device);

    /**
     * Backend implementation of ::palGetDeviceLostReason.
     *
     * Must obey the rules and semantics documented in palGetDeviceLostReason().
     */
    uint32_t(PAL_CALL* getDeviceLostReason)(PalDevice* device);

    /**
     * Backend implementation of ::palAllocateMemory.
     *
     * Must obey the rules and semantics documented in palAllocateMemory().
     */
    PalResult(PAL_CALL* allocateMemory)(
        PalDevice* device,
        PalMemoryType type,
        uint64_t memoryMask,
        uint64_t size,
        PalMemory** outMemory);

    /**
     * Backend implementation of ::palFreeMemory.
     *
     * Must obey the rules and semantics documented in palFreeMemory().
     */
    void(PAL_CALL* freeMemory)(PalMemory* memory);

    /**
     * Backend implementation of ::palQuerySamplerAnisotropyCapabilities.
     *
     * Must obey the rules and semantics documented in
     * palQuerySamplerAnisotropyCapabilities().
     */
    void(PAL_CALL* querySamplerAnisotropyCapabilities)(
        PalDevice* device,
        PalSamplerAnisotropyCapabilities* caps);

    /**
     * Backend implementation of ::palQueryMultiViewCapabilities.
     *
     * Must obey the rules and semantics documented in
     * palQueryMultiViewCapabilities().
     */
    void(PAL_CALL* queryMultiViewCapabilities)(
        PalDevice* device,
        PalMultiViewCapabilities* caps);

    /**
     * Backend implementation of ::palQueryMultiViewportCapabilities.
     *
     * Must obey the rules and semantics documented in
     * palQueryMultiViewportCapabilities().
     */
    void(PAL_CALL* queryMultiViewportCapabilities)(
        PalDevice* device,
        PalMultiViewportCapabilities* caps);

    /**
     * Backend implementation of ::palQueryDepthStencilCapabilities.
     *
     * Must obey the rules and semantics documented in
     * palQueryDepthStencilCapabilities().
     */
    void(PAL_CALL* queryDepthStencilCapabilities)(
        PalDevice* device,
        PalDepthStencilCapabilities* caps);

    /**
     * Backend implementation of ::palQueryFragmentShadingRateCapabilities.
     *
     * Must obey the rules and semantics documented in
     * palQueryFragmentShadingRateCapabilities().
     */
    void(PAL_CALL* queryFragmentShadingRateCapabilities)(
        PalDevice* device,
        PalFragmentShadingRateCapabilities* caps);

    /**
     * Backend implementation of ::palQueryMeshShaderCapabilities.
     *
     * Must obey the rules and semantics documented in
     * palQueryMeshShaderCapabilities().
     */
    void(PAL_CALL* queryMeshShaderCapabilities)(
        PalDevice* device,
        PalMeshShaderCapabilities* caps);

    /**
     * Backend implementation of ::palQueryRayTracingCapabilities.
     *
     * Must obey the rules and semantics documented in
     * palQueryRayTracingCapabilities().
     */
    void(PAL_CALL* queryRayTracingCapabilities)(
        PalDevice* device,
        PalRayTracingCapabilities* caps);

    /**
     * Backend implementation of ::palQueryDescriptorIndexingCapabilities.
     *
     * Must obey the rules and semantics documented in
     * palQueryDescriptorIndexingCapabilities().
     */
    void(PAL_CALL* queryDescriptorIndexingCapabilities)(
        PalDevice* device,
        PalDescriptorIndexingCapabilities* caps);

    /**
     * Backend implementation of ::palCreateQueue.
     *
     * Must obey the rules and semantics documented in palCreateQueue().
     */
    PalResult(PAL_CALL* createQueue)(
        PalDevice* device,
        PalQueueType type,
        PalQueue** outQueue);

    /**
     * Backend implementation of ::palDestroyQueue.
     *
     * Must obey the rules and semantics documented in palDestroyQueue().
     */
    void(PAL_CALL* destroyQueue)(PalQueue* queue);

    /**
     * Backend implementation of ::palCanQueuePresent.
     *
     * Must obey the rules and semantics documented in palCanQueuePresent().
     */
    PalBool(PAL_CALL* canQueuePresent)(
        PalQueue* queue,
        PalSurface* surface);

    /**
     * Backend implementation of ::palWaitQueue.
     *
     * Must obey the rules and semantics documented in palWaitQueue().
     */
    PalResult(PAL_CALL* waitQueue)(PalQueue* queue);

    /**
     * Backend implementation of ::palEnumerateFormats.
     *
     * Must obey the rules and semantics documented in palEnumerateFormats().
     */
    void(PAL_CALL* enumerateFormats)(
        PalAdapter* adapter,
        uint32_t* count,
        PalFormatInfo* outFormats);

    /**
     * Backend implementation of ::palIsFormatSupported.
     *
     * Must obey the rules and semantics documented in palIsFormatSupported().
     */
    PalBool(PAL_CALL* isFormatSupported)(
        PalAdapter* adapter,
        PalFormat format);

    /**
     * Backend implementation of ::palQueryFormatImageUsages.
     *
     * Must obey the rules and semantics documented in palQueryFormatImageUsages().
     */
    PalImageUsages(PAL_CALL* queryFormatImageUsages)(
        PalAdapter* adapter,
        PalFormat format);

    /**
     * Backend implementation of ::palQueryFormatSampleCount.
     *
     * Must obey the rules and semantics documented in palQueryFormatSampleCount().
     */
    PalSampleCount(PAL_CALL* queryFormatSampleCount)(
        PalAdapter* adapter,
        PalFormat format);

    /**
     * Backend implementation of ::palCreateImage.
     *
     * Must obey the rules and semantics documented in palCreateImage().
     */
    PalResult(PAL_CALL* createImage)(
        PalDevice* device,
        const PalImageCreateInfo* info,
        PalImage** outImage);

    /**
     * Backend implementation of ::palDestroyImage.
     *
     * Must obey the rules and semantics documented in palDestroyImage().
     */
    void(PAL_CALL* destroyImage)(PalImage* image);

    /**
     * Backend implementation of ::palGetImageInfo.
     *
     * Must obey the rules and semantics documented in palGetImageInfo().
     */
    void(PAL_CALL* getImageInfo)(
        PalImage* image,
        PalImageInfo* info);

    /**
     * Backend implementation of ::palGetImageMemoryRequirements.
     *
     * Must obey the rules and semantics documented in palGetImageMemoryRequirements().
     */
    void(PAL_CALL* getImageMemoryRequirements)(
        PalImage* image,
        PalMemoryRequirements* requirements);

    /**
     * Backend implementation of ::palBindImageMemory.
     *
     * Must obey the rules and semantics documented in palBindImageMemory().
     */
    PalResult(PAL_CALL* bindImageMemory)(
        PalImage* image,
        PalMemory* memory,
        uint64_t offset);

    /**
     * Backend implementation of ::palCreateImageView.
     *
     * Must obey the rules and semantics documented in palCreateImageView().
     */
    PalResult(PAL_CALL* createImageView)(
        PalDevice* device,
        PalImage* image,
        const PalImageViewCreateInfo* info,
        PalImageView** outImageView);

    /**
     * Backend implementation of ::palDestroyImageView.
     *
     * Must obey the rules and semantics documented in palDestroyImageView().
     */
    void(PAL_CALL* destroyImageView)(PalImageView* imageView);

    /**
     * Backend implementation of ::palCreateSampler.
     *
     * Must obey the rules and semantics documented in palCreateSampler().
     */
    PalResult(PAL_CALL* createSampler)(
        PalDevice* device,
        const PalSamplerCreateInfo* info,
        PalSampler** outSampler);

    /**
     * Backend implementation of ::palDestroySampler.
     *
     * Must obey the rules and semantics documented in palDestroySampler().
     */
    void(PAL_CALL* destroySampler)(PalSampler* sampler);

    /**
     * Backend implementation of ::palCreateSurface.
     *
     * Must obey the rules and semantics documented in palCreateSurface().
     */
    PalResult(PAL_CALL* createSurface)(
        PalDevice* device,
        void* window,
        void* windowInstance,
        PalWindowInstanceType instanceType,
        PalSurface** outSurface);

    /**
     * Backend implementation of ::palDestroySurface.
     *
     * Must obey the rules and semantics documented in palDestroySurface().
     */
    void(PAL_CALL* destroySurface)(PalSurface* surface);

    /**
     * Backend implementation of ::palGetSurfaceCapabilities.
     *
     * Must obey the rules and semantics documented in palGetSurfaceCapabilities().
     */
    void(PAL_CALL* getSurfaceCapabilities)(
        PalDevice* device,
        PalSurface* surface,
        PalSurfaceCapabilities* caps);

    /**
     * Backend implementation of ::palCreateSwapchain.
     *
     * Must obey the rules and semantics documented in palCreateSwapchain().
     */
    PalResult(PAL_CALL* createSwapchain)(
        PalDevice* device,
        PalQueue* queue,
        PalSurface* surface,
        const PalSwapchainCreateInfo* info,
        PalSwapchain** outSwapchain);

    /**
     * Backend implementation of ::palDestroySwapchain.
     *
     * Must obey the rules and semantics documented in palDestroySwapchain().
     */
    void(PAL_CALL* destroySwapchain)(PalSwapchain* swapchain);

    /**
     * Backend implementation of ::palGetSwapchainImage.
     *
     * Must obey the rules and semantics documented in palGetSwapchainImage().
     */
    PalImage*(PAL_CALL* getSwapchainImage)(
        PalSwapchain* swapchain,
        uint32_t index);

    /**
     * Backend implementation of ::palGetNextSwapchainImage.
     *
     * Must obey the rules and semantics documented in palGetNextSwapchainImage().
     */
    PalResult(PAL_CALL* getNextSwapchainImage)(
        PalSwapchain* swapchain,
        PalSwapchainNextImageInfo* info,
        uint32_t* outIndex);

    /**
     * Backend implementation of ::palPresentSwapchain.
     *
     * Must obey the rules and semantics documented in palPresentSwapchain().
     */
    PalResult(PAL_CALL* presentSwapchain)(
        PalSwapchain* swapchain,
        uint32_t imageIndex,
        PalSemaphore* waitSemaphore);

    /**
     * Backend implementation of ::palResizeSwapchain.
     *
     * Must obey the rules and semantics documented in palResizeSwapchain().
     */
    PalResult(PAL_CALL* resizeSwapchain)(
        PalSwapchain* swapchain,
        uint32_t newWidth,
        uint32_t newHeight);

    /**
     * Backend implementation of ::palCreateShader.
     *
     * Must obey the rules and semantics documented in palCreateShader().
     */
    PalResult(PAL_CALL* createShader)(
        PalDevice* device,
        const PalShaderCreateInfo* info,
        PalShader** outShader);

    /**
     * Backend implementation of ::palDestroyShader.
     *
     * Must obey the rules and semantics documented in palDestroyShader().
     */
    void(PAL_CALL* destroyShader)(PalShader* shader);

    /**
     * Backend implementation of ::palCreateFence.
     *
     * Must obey the rules and semantics documented in palCreateFence().
     */
    PalResult(PAL_CALL* createFence)(
        PalDevice* device,
        PalBool signaled,
        PalFence** outFence);

    /**
     * Backend implementation of ::palDestroyFence.
     *
     * Must obey the rules and semantics documented in palDestroyFence().
     */
    void(PAL_CALL* destroyFence)(PalFence* fence);

    /**
     * Backend implementation of ::palWaitFence.
     *
     * Must obey the rules and semantics documented in palWaitFence().
     */
    PalResult(PAL_CALL* waitFence)(
        PalFence* fence,
        uint64_t timeout);

    /**
     * Backend implementation of ::palResetFence.
     *
     * Must obey the rules and semantics documented in palResetFence().
     */
    PalResult(PAL_CALL* resetFence)(PalFence* fence);

    /**
     * Backend implementation of ::palIsFenceSignaled.
     *
     * Must obey the rules and semantics documented in palIsFenceSignaled().
     */
    PalBool(PAL_CALL* isFenceSignaled)(PalFence* fence);

    /**
     * Backend implementation of ::palCreateSemaphore.
     *
     * Must obey the rules and semantics documented in palCreateSemaphore().
     */
    PalResult(PAL_CALL* createSemaphore)(
        PalDevice* device,
        PalBool enableTimeline,
        PalSemaphore** outSemaphore);

    /**
     * Backend implementation of ::palDestroySemaphore.
     *
     * Must obey the rules and semantics documented in palDestroySemaphore().
     */
    void(PAL_CALL* destroySemaphore)(PalSemaphore* semaphore);

    /**
     * Backend implementation of ::palWaitSemaphore.
     *
     * Must obey the rules and semantics documented in palWaitSemaphore().
     */
    PalResult(PAL_CALL* waitSemaphore)(
        PalSemaphore* semaphore,
        uint64_t value,
        uint64_t timeout);

    /**
     * Backend implementation of ::palSignalSemaphore.
     *
     * Must obey the rules and semantics documented in palSignalSemaphore().
     */
    PalResult(PAL_CALL* signalSemaphore)(
        PalSemaphore* semaphore,
        PalQueue* queue,
        uint64_t value);

    /**
     * Backend implementation of ::palGetSemaphoreValue.
     *
     * Must obey the rules and semantics documented in palGetSemaphoreValue().
     */
    PalResult(PAL_CALL* getSemaphoreValue)(
        PalSemaphore* semaphore,
        uint64_t* value);

    /**
     * Backend implementation of ::palCreateCommandPool.
     *
     * Must obey the rules and semantics documented in palCreateCommandPool().
     */
    PalResult(PAL_CALL* createCommandPool)(
        PalDevice* device,
        PalQueue* queue,
        PalCommandPool** outPool);

    /**
     * Backend implementation of ::palDestroyCommandPool.
     *
     * Must obey the rules and semantics documented in palDestroyCommandPool().
     */
    void(PAL_CALL* destroyCommandPool)(PalCommandPool* pool);

    /**
     * Backend implementation of ::palAllocateCommandBuffer.
     *
     * Must obey the rules and semantics documented in palAllocateCommandBuffer().
     */
    PalResult(PAL_CALL* allocateCommandBuffer)(
        PalDevice* device,
        PalCommandPool* pool,
        PalCommandBufferType type,
        PalCommandBuffer** outCmdBuffer);

    /**
     * Backend implementation of ::palFreeCommandBuffer.
     *
     * Must obey the rules and semantics documented in palFreeCommandBuffer().
     */
    void(PAL_CALL* freeCommandBuffer)(PalCommandBuffer* cmdBuffer);

    /**
     * Backend implementation of ::palResetCommandBuffer.
     *
     * Must obey the rules and semantics documented in palResetCommandBuffer().
     */
    PalResult(PAL_CALL* resetCommandBuffer)(PalCommandBuffer* cmdBuffer);

    /**
     * Backend implementation of ::palSubmitCommandBuffer.
     *
     * Must obey the rules and semantics documented in palSubmitCommandBuffer().
     */
    PalResult(PAL_CALL* submitCommandBuffer)(
        PalQueue* queue,
        PalCommandBufferSubmitInfo* info);

    /**
     * Backend implementation of ::palCmdBegin.
     *
     * Must obey the rules and semantics documented in palCmdBegin().
     */
    PalResult(PAL_CALL* cmdBegin)(
        PalCommandBuffer* cmdBuffer,
        PalRenderingLayoutInfo* info);

    /**
     * Backend implementation of ::palCmdEnd.
     *
     * Must obey the rules and semantics documented in palCmdEnd().
     */
    PalResult(PAL_CALL* cmdEnd)(PalCommandBuffer* cmdBuffer);

    /**
     * Backend implementation of ::palCmdExecuteCommandBuffer.
     *
     * Must obey the rules and semantics documented in palCmdExecuteCommandBuffer().
     */
    void(PAL_CALL* cmdExecuteCommandBuffer)(
        PalCommandBuffer* primaryCmdBuffer,
        PalCommandBuffer* secondaryCmdBuffer);

    /**
     * Backend implementation of ::palCmdSetFragmentShadingRate.
     *
     * Must obey the rules and semantics documented in palCmdSetFragmentShadingRate().
     */
    void(PAL_CALL* cmdSetFragmentShadingRate)(
        PalCommandBuffer* cmdBuffer,
        PalFragmentShadingRateState* state);

    /**
     * Backend implementation of ::palCmdDrawMeshTasks.
     *
     * Must obey the rules and semantics documented in palCmdDrawMeshTasks().
     */
    void(PAL_CALL* cmdDrawMeshTasks)(
        PalCommandBuffer* cmdBuffer,
        uint32_t groupCountX,
        uint32_t groupCountY,
        uint32_t groupCountZ);

    /**
     * Backend implementation of ::palCmdDrawMeshTasksIndirect.
     *
     * Must obey the rules and semantics documented in palCmdDrawMeshTasksIndirect().
     */
    void(PAL_CALL* cmdDrawMeshTasksIndirect)(
        PalCommandBuffer* cmdBuffer,
        PalBuffer* buffer,
        uint32_t drawCount);

    /**
     * Backend implementation of ::palCmdDrawMeshTasksIndirectCount.
     *
     * Must obey the rules and semantics documented in palCmdDrawMeshTasksIndirectCount().
     */
    void(PAL_CALL* cmdDrawMeshTasksIndirectCount)(
        PalCommandBuffer* cmdBuffer,
        PalBuffer* buffer,
        PalBuffer* countBuffer,
        uint32_t maxDrawCount);

    /**
     * Backend implementation of ::palCmdBuildAccelerationStructure.
     *
     * Must obey the rules and semantics documented in palCmdBuildAccelerationStructure().
     */
    void(PAL_CALL* cmdBuildAccelerationStructure)(
        PalCommandBuffer* cmdBuffer,
        PalAccelerationStructureBuildInfo* info);

    /**
     * Backend implementation of ::palCmdBeginRendering.
     *
     * Must obey the rules and semantics documented in palCmdBeginRendering().
     */
    void(PAL_CALL* cmdBeginRendering)(
        PalCommandBuffer* cmdBuffer,
        PalRenderingInfo* info);

    /**
     * Backend implementation of ::palCmdEndRendering.
     *
     * Must obey the rules and semantics documented in palCmdEndRendering().
     */
    void(PAL_CALL* cmdEndRendering)(PalCommandBuffer* cmdBuffer);

    /**
     * Backend implementation of ::palCmdCopyBuffer.
     *
     * Must obey the rules and semantics documented in palCmdCopyBuffer().
     */
    void(PAL_CALL* cmdCopyBuffer)(
        PalCommandBuffer* cmdBuffer,
        PalBuffer* dst,
        PalBuffer* src,
        PalBufferCopyInfo* copyInfo);

    /**
     * Backend implementation of ::palCmdCopyBufferToImage.
     *
     * Must obey the rules and semantics documented in palCmdCopyBufferToImage().
     */
    void(PAL_CALL* cmdCopyBufferToImage)(
        PalCommandBuffer* cmdBuffer,
        PalImage* dstImage,
        PalBuffer* srcBuffer,
        PalBufferImageCopyInfo* copyInfo);

    /**
     * Backend implementation of ::cmdCopyImage.
     *
     * Must obey the rules and semantics documented in cmdCopyImage().
     */
    void(PAL_CALL* cmdCopyImage)(
        PalCommandBuffer* cmdBuffer,
        PalImage* dst,
        PalImage* src,
        PalImageCopyInfo* copyInfo);

    /**
     * Backend implementation of ::palCmdCopyImageToBuffer.
     *
     * Must obey the rules and semantics documented in palCmdCopyImageToBuffer().
     */
    void(PAL_CALL* cmdCopyImageToBuffer)(
        PalCommandBuffer* cmdBuffer,
        PalBuffer* dstBuffer,
        PalImage* srcImage,
        PalBufferImageCopyInfo* copyInfo);

    /**
     * Backend implementation of ::palCmdBindPipeline.
     *
     * Must obey the rules and semantics documented in palCmdBindPipeline().
     */
    void(PAL_CALL* cmdBindPipeline)(
        PalCommandBuffer* cmdBuffer,
        PalPipeline* pipeline);

    /**
     * Backend implementation of ::palCmdSetViewport.
     *
     * Must obey the rules and semantics documented in palCmdSetViewport().
     */
    void(PAL_CALL* cmdSetViewport)(
        PalCommandBuffer* cmdBuffer,
        uint32_t count,
        PalViewport* viewports);

    /**
     * Backend implementation of ::palCmdSetScissors.
     *
     * Must obey the rules and semantics documented in palCmdSetScissors().
     */
    void(PAL_CALL* cmdSetScissors)(
        PalCommandBuffer* cmdBuffer,
        uint32_t count,
        PalRect2D* scissors);

    /**
     * Backend implementation of ::palCmdBindVertexBuffers.
     *
     * Must obey the rules and semantics documented in palCmdBindVertexBuffers().
     */
    void(PAL_CALL* cmdBindVertexBuffers)(
        PalCommandBuffer* cmdBuffer,
        uint32_t firstSlot,
        uint32_t count,
        PalBuffer** buffers,
        uint64_t* offsets);

    /**
     * Backend implementation of ::palCmdBindIndexBuffer.
     *
     * Must obey the rules and semantics documented in palCmdBindIndexBuffer().
     */
    void(PAL_CALL* cmdBindIndexBuffer)(
        PalCommandBuffer* cmdBuffer,
        PalBuffer* buffer,
        uint64_t offset,
        PalIndexType type);

    /**
     * Backend implementation of ::palCmdDraw.
     *
     * Must obey the rules and semantics documented in palCmdDraw().
     */
    void(PAL_CALL* cmdDraw)(
        PalCommandBuffer* cmdBuffer,
        uint32_t vertexCount,
        uint32_t instanceCount,
        uint32_t firstVertex,
        uint32_t firstInstance);

    /**
     * Backend implementation of ::palCmdDrawIndirect.
     *
     * Must obey the rules and semantics documented in palCmdDrawIndirect().
     */
    void(PAL_CALL* cmdDrawIndirect)(
        PalCommandBuffer* cmdBuffer,
        PalBuffer* buffer,
        uint32_t count);

    /**
     * Backend implementation of ::palCmdDrawIndirectCount.
     *
     * Must obey the rules and semantics documented in palCmdDrawIndirectCount().
     */
    void(PAL_CALL* cmdDrawIndirectCount)(
        PalCommandBuffer* cmdBuffer,
        PalBuffer* buffer,
        PalBuffer* countBuffer,
        uint32_t count);

    /**
     * Backend implementation of ::palCmdDrawIndexed.
     *
     * Must obey the rules and semantics documented in palCmdDrawIndexed().
     */
    void(PAL_CALL* cmdDrawIndexed)(
        PalCommandBuffer* cmdBuffer,
        uint32_t indexCount,
        uint32_t instanceCount,
        uint32_t firstIndex,
        int32_t vertexOffset,
        uint32_t firstInstance);

    /**
     * Backend implementation of ::palCmdDrawIndexedIndirect.
     *
     * Must obey the rules and semantics documented in palCmdDrawIndexedIndirect().
     */
    void(PAL_CALL* cmdDrawIndexedIndirect)(
        PalCommandBuffer* cmdBuffer,
        PalBuffer* buffer,
        uint32_t count);

    /**
     * Backend implementation of ::palCmdDrawIndexedIndirectCount.
     *
     * Must obey the rules and semantics documented in palCmdDrawIndexedIndirectCount().
     */
    void(PAL_CALL* cmdDrawIndexedIndirectCount)(
        PalCommandBuffer* cmdBuffer,
        PalBuffer* buffer,
        PalBuffer* countBuffer,
        uint32_t count);

    /**
     * Backend implementation of ::palCmdAccelerationStructureBarrier.
     *
     * Must obey the rules and semantics documented in palCmdAccelerationStructureBarrier().
     */
    void(PAL_CALL* cmdAccelerationStructureBarrier)(
        PalCommandBuffer* cmdBuffer,
        PalAccelerationStructure* as,
        PalBarrierInfo* info);

    /**
     * Backend implementation of ::palCmdImageBarrier.
     *
     * Must obey the rules and semantics documented in palCmdImageBarrier().
     */
    void(PAL_CALL* cmdImageBarrier)(
        PalCommandBuffer* cmdBuffer,
        PalImage* image,
        PalImageSubresourceRange* subresourceRange,
        PalBarrierInfo* info);

    /**
     * Backend implementation of ::palCmdBufferBarrier.
     *
     * Must obey the rules and semantics documented in palCmdBufferBarrier().
     */
    void(PAL_CALL* cmdBufferBarrier)(
        PalCommandBuffer* cmdBuffer,
        PalBuffer* buffer,
        PalBarrierInfo* info);

    /**
     * Backend implementation of ::palCmdDispatch.
     *
     * Must obey the rules and semantics documented in palCmdDispatch().
     */
    void(PAL_CALL* cmdDispatch)(
        PalCommandBuffer* cmdBuffer,
        uint32_t groupCountX,
        uint32_t groupCountY,
        uint32_t groupCountZ);

    /**
     * Backend implementation of ::palCmdDispatchBase.
     *
     * Must obey the rules and semantics documented in palCmdDispatchBase().
     */
    void(PAL_CALL* cmdDispatchBase)(
        PalCommandBuffer* cmdBuffer,
        uint32_t baseGroupX,
        uint32_t baseGroupY,
        uint32_t baseGroupZ,
        uint32_t groupCountX,
        uint32_t groupCountY,
        uint32_t groupCountZ);

    /**
     * Backend implementation of ::palCmdDispatchIndirect.
     *
     * Must obey the rules and semantics documented in palCmdDispatchIndirect().
     */
    void(PAL_CALL* cmdDispatchIndirect)(
        PalCommandBuffer* cmdBuffer,
        PalBuffer* buffer);

    /**
     * Backend implementation of ::palCmdTraceRays.
     *
     * Must obey the rules and semantics documented in palCmdTraceRays().
     */
    void(PAL_CALL* cmdTraceRays)(
        PalCommandBuffer* cmdBuffer,
        PalShaderBindingTable* sbt,
        uint32_t raygenIndex,
        uint32_t width,
        uint32_t height,
        uint32_t depth);

    /**
     * Backend implementation of ::palCmdTraceRaysIndirect.
     *
     * Must obey the rules and semantics documented in palCmdTraceRaysIndirect().
     */
    void(PAL_CALL* cmdTraceRaysIndirect)(
        PalCommandBuffer* cmdBuffer,
        uint32_t raygenIndex,
        PalShaderBindingTable* sbt,
        PalBuffer* buffer);

    /**
     * Backend implementation of ::palCmdBindDescriptorSet.
     *
     * Must obey the rules and semantics documented in palCmdBindDescriptorSet().
     */
    void(PAL_CALL* cmdBindDescriptorSet)(
        PalCommandBuffer* cmdBuffer,
        uint32_t setIndex,
        PalDescriptorSet* set);

    /**
     * Backend implementation of ::palCmdPushConstants.
     *
     * Must obey the rules and semantics documented in palCmdPushConstants().
     */
    void(PAL_CALL* cmdPushConstants)(
        PalCommandBuffer* cmdBuffer,
        uint32_t offset,
        uint32_t size,
        const void* value);

    /**
     * Backend implementation of ::palCmdSetCullMode.
     *
     * Must obey the rules and semantics documented in palCmdSetCullMode().
     */
    void(PAL_CALL* cmdSetCullMode)(
        PalCommandBuffer* cmdBuffer,
        PalCullMode cullMode);

    /**
     * Backend implementation of ::palCmdSetFrontFace.
     *
     * Must obey the rules and semantics documented in palCmdSetFrontFace().
     */
    void(PAL_CALL* cmdSetFrontFace)(
        PalCommandBuffer* cmdBuffer,
        PalFrontFace frontFace);

    /**
     * Backend implementation of ::palCmdSetPrimitiveTopology.
     *
     * Must obey the rules and semantics documented in palCmdSetPrimitiveTopology().
     */
    void(PAL_CALL* cmdSetPrimitiveTopology)(
        PalCommandBuffer* cmdBuffer,
        PalPrimitiveTopology topology);

    /**
     * Backend implementation of ::palCmdSetDepthTestEnable.
     *
     * Must obey the rules and semantics documented in palCmdSetDepthTestEnable().
     */
    void(PAL_CALL* cmdSetDepthTestEnable)(
        PalCommandBuffer* cmdBuffer,
        PalBool enable);

    /**
     * Backend implementation of ::palCmdSetDepthWriteEnable.
     *
     * Must obey the rules and semantics documented in palCmdSetDepthWriteEnable().
     */
    void(PAL_CALL* cmdSetDepthWriteEnable)(
        PalCommandBuffer* cmdBuffer,
        PalBool enable);

    /**
     * Backend implementation of ::palCmdSetStencilOp.
     *
     * Must obey the rules and semantics documented in palCmdSetStencilOp().
     */
    void(PAL_CALL* cmdSetStencilOp)(
        PalCommandBuffer* cmdBuffer,
        PalStencilFaceFlags faceMask,
        PalStencilOp failOp,
        PalStencilOp passOp,
        PalStencilOp depthFailOp,
        PalCompareOp compareOp);

    /**
     * Backend implementation of ::palCreateAccelerationstructure.
     *
     * Must obey the rules and semantics documented in palCreateAccelerationstructure().
     */
    PalResult(PAL_CALL* createAccelerationstructure)(
        PalDevice* device,
        const PalAccelerationStructureCreateInfo* info,
        PalAccelerationStructure** outAs);

    /**
     * Backend implementation of ::palDestroyAccelerationStructure.
     *
     * Must obey the rules and semantics documented in palDestroyAccelerationStructure().
     */
    void(PAL_CALL* destroyAccelerationstructure)(PalAccelerationStructure* as);

    /**
     * Backend implementation of ::palGetAccelerationStructureBuildSize.
     *
     * Must obey the rules and semantics documented in palGetAccelerationStructureBuildSize().
     */
    void(PAL_CALL* getAccelerationStructureBuildSize)(
        PalDevice* device,
        PalAccelerationStructureBuildInfo* info,
        PalAccelerationStructureBuildSize* size);

    /**
     * Backend implementation of ::palCreateBuffer.
     *
     * Must obey the rules and semantics documented in palCreateBuffer().
     */
    PalResult(PAL_CALL* createBuffer)(
        PalDevice* device,
        const PalBufferCreateInfo* info,
        PalBuffer** outBuffer);

    /**
     * Backend implementation of ::palDestroyBuffer.
     *
     * Must obey the rules and semantics documented in palDestroyBuffer().
     */
    void(PAL_CALL* destroyBuffer)(PalBuffer* buffer);

    /**
     * Backend implementation of ::palGetBufferMemoryRequirements.
     *
     * Must obey the rules and semantics documented in palGetBufferMemoryRequirements().
     */
    void(PAL_CALL* getBufferMemoryRequirements)(
        PalBuffer* buffer,
        PalMemoryRequirements* requirements);

    /**
     * Backend implementation of ::palComputeInstanceStagingSize.
     *
     * Must obey the rules and semantics documented in palComputeInstanceStagingSize().
     */
    void(PAL_CALL* computeInstanceStagingSize)(
        PalDevice* device,
        uint32_t instanceCount,
        uint64_t* outSize);

    /**
     * Backend implementation of ::palComputeImageStagingRequirements.
     *
     * Must obey the rules and semantics documented in palComputeImageStagingRequirements().
     */
    void(PAL_CALL* computeImageStagingRequirements)(
        PalDevice* device,
        PalFormat imageFormat,
        const PalBufferImageCopyInfo* copyInfo,
        PalImageStagingRequirements* requirements);

    /**
     * Backend implementation of ::palWriteInstanceStaging.
     *
     * Must obey the rules and semantics documented in palWriteInstanceStaging().
     */
    void(PAL_CALL* writeInstanceStaging)(
        PalDevice* device,
        uint32_t instanceCount,
        PalAccelerationStructureInstance* instances,
        void* ptr);

    /**
     * Backend implementation of ::palWriteImageStaging.
     *
     * Must obey the rules and semantics documented in palWriteImageStaging().
     */
    void(PAL_CALL* writeImageStaging)(
        PalDevice* device,
        PalFormat imageFormat,
        PalBufferImageCopyInfo* copyInfo,
        void* srcData,
        void* ptr);

    /**
     * Backend implementation of ::palBindBufferMemory.
     *
     * Must obey the rules and semantics documented in palBindBufferMemory().
     */
    PalResult(PAL_CALL* bindBufferMemory)(
        PalBuffer* buffer,
        PalMemory* memory,
        uint64_t offset);

    /**
     * Backend implementation of ::palMapBuffer.
     *
     * Must obey the rules and semantics documented in palMapBuffer().
     */
    PalResult(PAL_CALL* mapBuffer)(
        PalBuffer* buffer,
        uint64_t offset,
        uint64_t size,
        void** outPtr);

    /**
     * Backend implementation of ::palUnmapBuffer.
     *
     * Must obey the rules and semantics documented in palUnmapBuffer().
     */
    void(PAL_CALL* unmapBuffer)(PalBuffer* buffer);

    /**
     * Backend implementation of ::palGetBufferDeviceAddress.
     *
     * Must obey the rules and semantics documented in palGetBufferDeviceAddress().
     */
    PalDeviceAddress(PAL_CALL* getBufferDeviceAddress)(PalBuffer* buffer);

    /**
     * Backend implementation of ::palCreateDescriptorSetLayout.
     *
     * Must obey the rules and semantics documented in palCreateDescriptorSetLayout().
     */
    PalResult(PAL_CALL* createDescriptorSetLayout)(
        PalDevice* device,
        const PalDescriptorSetLayoutCreateInfo* info,
        PalDescriptorSetLayout** outLayout);

    /**
     * Backend implementation of ::palDestroyDescriptorSetLayout.
     *
     * Must obey the rules and semantics documented in palDestroyDescriptorSetLayout().
     */
    void(PAL_CALL* destroyDescriptorSetLayout)(PalDescriptorSetLayout* layout);

    /**
     * Backend implementation of ::palCreateDescriptorPool.
     *
     * Must obey the rules and semantics documented in palCreateDescriptorPool().
     */
    PalResult(PAL_CALL* createDescriptorPool)(
        PalDevice* device,
        const PalDescriptorPoolCreateInfo* info,
        PalDescriptorPool** outPool);

    /**
     * Backend implementation of ::palDestroyDescriptorPool.
     *
     * Must obey the rules and semantics documented in palDestroyDescriptorPool().
     */
    void(PAL_CALL* destroyDescriptorPool)(PalDescriptorPool* pool);

    /**
     * Backend implementation of ::palResetDescriptorPool.
     *
     * Must obey the rules and semantics documented in palResetDescriptorPool().
     */
    PalResult(PAL_CALL* resetDescriptorPool)(PalDescriptorPool* pool);

    /**
     * Backend implementation of ::palAllocateDescriptorSet.
     *
     * Must obey the rules and semantics documented in palAllocateDescriptorSet().
     */
    PalResult(PAL_CALL* allocateDescriptorSet)(
        PalDevice* device,
        PalDescriptorPool* pool,
        PalDescriptorSetLayout* layout,
        PalDescriptorSet** outSet);

    /**
     * Backend implementation of ::palUpdateDescriptorSet.
     *
     * Must obey the rules and semantics documented in palUpdateDescriptorSet().
     */
    PalResult(PAL_CALL* updateDescriptorSet)(
        PalDevice* device,
        uint32_t count,
        PalDescriptorSetWriteInfo* infos);

    /**
     * Backend implementation of ::palCreatePipelineLayout.
     *
     * Must obey the rules and semantics documented in palCreatePipelineLayout().
     */
    PalResult(PAL_CALL* createPipelineLayout)(
        PalDevice* device,
        const PalPipelineLayoutCreateInfo* info,
        PalPipelineLayout** outLayout);

    /**
     * Backend implementation of ::palDestroyPipelineLayout.
     *
     * Must obey the rules and semantics documented in palDestroyPipelineLayout().
     */
    void(PAL_CALL* destroyPipelineLayout)(PalPipelineLayout* layout);

    /**
     * Backend implementation of ::palCreateGraphicsPipeline.
     *
     * Must obey the rules and semantics documented in palCreateGraphicsPipeline().
     */
    PalResult(PAL_CALL* createGraphicsPipeline)(
        PalDevice* device,
        const PalGraphicsPipelineCreateInfo* info,
        PalPipeline** outPipeline);

    /**
     * Backend implementation of ::palCreateComputePipeline.
     *
     * Must obey the rules and semantics documented in palCreateComputePipeline().
     */
    PalResult(PAL_CALL* createComputePipeline)(
        PalDevice* device,
        const PalComputePipelineCreateInfo* info,
        PalPipeline** outPipeline);

    /**
     * Backend implementation of ::palCreateRayTracingPipeline.
     *
     * Must obey the rules and semantics documented in palCreateRayTracingPipeline().
     */
    PalResult(PAL_CALL* createRayTracingPipeline)(
        PalDevice* device,
        const PalRayTracingPipelineCreateInfo* info,
        PalPipeline** outPipeline);

    /**
     * Backend implementation of ::palDestroyPipeline.
     *
     * Must obey the rules and semantics documented in palDestroyPipeline().
     */
    void(PAL_CALL* destroyPipeline)(PalPipeline* pipeline);

    /**
     * Backend implementation of ::palCreateShaderBindingTable.
     *
     * Must obey the rules and semantics documented in palCreateShaderBindingTable().
     */
    PalResult(PAL_CALL* createShaderBindingTable)(
        PalDevice* device,
        const PalShaderBindingTableCreateInfo* info,
        PalShaderBindingTable** outSbt);

    /**
     * Backend implementation of ::palDestroyShaderBindingTable.
     *
     * Must obey the rules and semantics documented in palDestroyShaderBindingTable().
     */
    void(PAL_CALL* destroyShaderBindingTable)(PalShaderBindingTable* sbt);

    /**
     * Backend implementation of ::palUpdateShaderBindingTable.
     *
     * Must obey the rules and semantics documented in palUpdateShaderBindingTable().
     */
    void(PAL_CALL* updateShaderBindingTable)(
        PalShaderBindingTable* sbt,
        uint32_t count,
        PalShaderBindingTableRecordInfo* infos);
} PalGraphicsBackendVtable1;

#endif // PAL_GRAPHICS_VTABLE_1_H