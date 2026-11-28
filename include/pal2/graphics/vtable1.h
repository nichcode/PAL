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

#include "acceleration_structure.h"
#include "adapter.h"
#include "buffer.h"
#include "pal2/core/base.h"
#include "pal2/core/result.h"

#include "cmd.h"
#include "cmdbuffer.h"
#include "descriptor.h"
#include "device.h"
#include "format.h"

#include "image.h"
#include "memory.h"
#include "pipeline.h"
#include "queue.h"
#include "sampler.h"

#include "sbt.h"
#include "shader.h"
#include "surface.h"
#include "swapchain.h"
#include "sync.h"

//clang-format off
/** This function has the same semantics and rules as palEnumerateAdapters().
 *
 * Additional rules:
 * - The function must exist before a device is created.
 */
typedef PalResult(PAL_CALL* EnumerateAdaptersFn)(
    uint32_t*,
    PalAdapter**);

/** This function has the same semantics and rules as palGetAdapterInfo().
 *
 * Additional rules:
 * - The function must exist before a device is created.
 */
typedef void(PAL_CALL* GetAdapterInfoFn)(
    PalAdapter*,
    PalAdapterInfo*);

/** This function has the same semantics and rules as palGetAdapterCapabilities().
 *
 * Additional rules:
 * - The function must exist before a device is created.
 */
typedef void(PAL_CALL* GetAdapterCapabilitiesFn)(
    PalAdapter*,
    PalAdapterCapabilities*);

/** This function has the same semantics and rules as palGetAdapterFeatures().
 *
 * Additional rules:
 * - The function must exist before a device is created.
 */
typedef PalAdapterFeatures(PAL_CALL* GetAdapterFeaturesFn)(PalAdapter*);

/** This function has the same semantics and rules as palGetHighestSupportedShaderTarget().
 *
 * Additional rules:
 * - The function must exist before a device is created.
 */
typedef uint32_t(PAL_CALL* GetHighestSupportedShaderTargetFn)(
    PalAdapter*,
    PalShaderFormats);

/** This function has the same semantics and rules as palCreateDevice().
 *
 * Additional rules:
 * - The function must exist before a device is created.
 */
typedef PalResult(PAL_CALL* CreateDeviceFn)(
    PalAdapter*,
    PalAdapterFeatures,
    PalDevice**);

/** This function has the same semantics and rules as palDestroyDevice(). */
typedef void(PAL_CALL* DestroyDeviceFn)(PalDevice*);

/** This function has the same semantics and rules as palGetDeviceLostReason(). */
typedef uint32_t(PAL_CALL* GetDeviceLostReasonFn)(PalDevice*);

/** This function has the same semantics and rules as palAllocateMemory(). */
typedef PalResult(PAL_CALL* AllocateMemoryFn)(
    PalDevice*,
    PalMemoryType,
    uint64_t,
    uint64_t,
    PalMemory**);

/** This function has the same semantics and rules as palFreeMemory(). */
typedef void(PAL_CALL* FreeMemoryFn)(PalMemory*);

/** This function has the same semantics and rules as palQuerySamplerAnisotropyCapabilities(). */
typedef void(PAL_CALL* QuerySamplerAnisotropyCapabilitiesFn)(
    PalDevice*,
    PalSamplerAnisotropyCapabilities*);

/** This function has the same semantics and rules as palQueryMultiViewCapabilities(). */
typedef void(PAL_CALL* QueryMultiViewCapabilitiesFn)(
    PalDevice*,
    PalMultiViewCapabilities*);

/** This function has the same semantics and rules as palQueryMultiViewportCapabilities(). */
typedef void(PAL_CALL* QueryMultiViewportCapabilitiesFn)(
    PalDevice*,
    PalMultiViewportCapabilities*);

/** This function has the same semantics and rules as palQueryDepthStencilCapabilities(). */
typedef void(PAL_CALL* QueryDepthStencilCapabilitiesFn)(
    PalDevice*,
    PalDepthStencilCapabilities*);

/** This function has the same semantics and rules as palQueryFragmentShadingRateCapabilities(). */
typedef void(PAL_CALL* QueryFragmentShadingRateCapabilitiesFn)(
    PalDevice*,
    PalFragmentShadingRateCapabilities*);

/** This function has the same semantics and rules as palQueryMeshShaderCapabilities(). */
typedef void(PAL_CALL* QueryMeshShaderCapabilitiesFn)(
    PalDevice*,
    PalMeshShaderCapabilities*);

/** This function has the same semantics and rules as palQueryRayTracingCapabilities(). */
typedef void(PAL_CALL* QueryRayTracingCapabilitiesFn)(
    PalDevice*,
    PalRayTracingCapabilities*);

/** This function has the same semantics and rules as palQueryDescriptorIndexingCapabilities(). */
typedef void(PAL_CALL* QueryDescriptorIndexingCapabilitiesFn)(
    PalDevice*,
    PalDescriptorIndexingCapabilities*);

/** This function has the same semantics and rules as palCreateQueue(). */
typedef PalResult(PAL_CALL* CreateQueueFn)(
    PalDevice*,
    PalQueueType,
    PalQueue**);

/** This function has the same semantics and rules as palDestroyQueue(). */
typedef void(PAL_CALL* DestroyQueueFn)(PalQueue*);

/** This function has the same semantics and rules as palCanQueuePresent(). */
typedef PalBool(PAL_CALL* CanQueuePresentFn)(
    PalQueue*,
    PalSurface*);

/** This function has the same semantics and rules as palWaitQueue(). */
typedef PalResult(PAL_CALL* WaitQueueFn)(PalQueue*);

/** This function has the same semantics and rules as palEnumerateFormats().
 *
 * Additional rules:
 * - The function must exist before a device is created.
 */
typedef void(PAL_CALL* EnumerateFormatsFn)(
    PalAdapter*,
    uint32_t*,
    PalFormatInfo*);

/** This function has the same semantics and rules as palIsFormatSupported().
 *
 * Additional rules:
 * - The function must exist before a device is created.
 */
typedef PalBool(PAL_CALL* IsFormatSupportedFn)(
    PalAdapter*,
    PalFormat);

/** This function has the same semantics and rules as palQueryFormatImageUsages().
 *
 * Additional rules:
 * - The function must exist before a device is created.
 */
typedef PalImageUsages(PAL_CALL* QueryFormatImageUsagesFn)(
    PalAdapter*,
    PalFormat);

/** This function has the same semantics and rules as palQueryFormatSampleCount().
 *
 * Additional rules:
 * - The function must exist before a device is created.
 */
typedef PalSampleCount(PAL_CALL* QueryFormatSampleCountFn)(
    PalAdapter*,
    PalFormat);

/** This function has the same semantics and rules as palCreateImage(). */
typedef PalResult(PAL_CALL* CreateImageFn)(
    PalDevice*,
    const PalImageCreateInfo*,
    PalImage**);

/** This function has the same semantics and rules as palDestroyImage(). */
typedef void(PAL_CALL* DestroyImageFn)(PalImage*);

/** This function has the same semantics and rules as palGetImageInfo(). */
typedef void(PAL_CALL* GetImageInfoFn)(
    PalImage*,
    PalImageInfo*);

/** This function has the same semantics and rules as palGetImageMemoryRequirements(). */
typedef void(PAL_CALL* GetImageMemoryRequirementsFn)(
    PalImage*,
    PalMemoryRequirements*);

/** This function has the same semantics and rules as palBindImageMemory(). */
typedef PalResult(PAL_CALL* BindImageMemoryFn)(
    PalImage*,
    PalMemory*,
    uint64_t);

/** This function has the same semantics and rules as palCreateImageView(). */
typedef PalResult(PAL_CALL* CreateImageViewFn)(
    PalDevice*,
    PalImage*,
    const PalImageViewCreateInfo*,
    PalImageView**);

/** This function has the same semantics and rules as palDestroyImageView(). */
typedef void(PAL_CALL* DestroyImageViewFn)(PalImageView*);

/** This function has the same semantics and rules as palCreateSampler(). */
typedef PalResult(PAL_CALL* CreateSamplerFn)(
    PalDevice*,
    const PalSamplerCreateInfo*,
    PalSampler**);

/** This function has the same semantics and rules as palDestroySampler(). */
typedef void(PAL_CALL* DestroySamplerFn)(PalSampler*);

/** This function has the same semantics and rules as palCreateSurface(). */
typedef PalResult(PAL_CALL* CreateSurfaceFn)(
    PalDevice*,
    void*,
    void*,
    PalWindowInstanceType,
    PalSurface**);

/** This function has the same semantics and rules as palDestroySurface(). */
typedef void(PAL_CALL* DestroySurfaceFn)(PalSurface*);

/** This function has the same semantics and rules as palGetSurfaceCapabilities(). */
typedef void(PAL_CALL* GetSurfaceCapabilitiesFn)(
    PalDevice*,
    PalSurface*,
    PalSurfaceCapabilities*);

/** This function has the same semantics and rules as palCreateSwapchain(). */
typedef PalResult(PAL_CALL* CreateSwapchainFn)(
    PalDevice*,
    PalQueue*,
    PalSurface*,
    const PalSwapchainCreateInfo*,
    PalSwapchain**);

/** This function has the same semantics and rules as palDestroySwapchain(). */
typedef void(PAL_CALL* DestroySwapchainFn)(PalSwapchain*);

/** This function has the same semantics and rules as palGetSwapchainImage(). */
typedef PalImage*(PAL_CALL* GetSwapchainImageFn)(
    PalSwapchain*,
    uint32_t);

/** This function has the same semantics and rules as palGetNextSwapchainImage(). */
typedef PalResult(PAL_CALL* GetNextSwapchainImageFn)(
    PalSwapchain*,
    PalSwapchainNextImageInfo*,
    uint32_t*);

/** This function has the same semantics and rules as palPresentSwapchain(). */
typedef PalResult(PAL_CALL* PresentSwapchainFn)(
    PalSwapchain*,
    uint32_t,
    PalSemaphore*);

/** This function has the same semantics and rules as palResizeSwapchain(). */
typedef PalResult(PAL_CALL* ResizeSwapchainFn)(
    PalSwapchain*,
    uint32_t,
    uint32_t);

/** This function has the same semantics and rules as palCreateShader(). */
typedef PalResult(PAL_CALL* CreateShaderFn)(
    PalDevice*,
    const PalShaderCreateInfo*,
    PalShader**);

/** This function has the same semantics and rules as palDestroyShader(). */
typedef void(PAL_CALL* DestroyShaderFn)(PalShader*);

/** This function has the same semantics and rules as palCreateFence(). */
typedef PalResult(PAL_CALL* CreateFenceFn)(
    PalDevice*,
    PalBool,
    PalFence**);

/** This function has the same semantics and rules as palDestroyFence(). */
typedef void(PAL_CALL* DestroyFenceFn)(PalFence*);

/** This function has the same semantics and rules as palWaitFence(). */
typedef PalResult(PAL_CALL* WaitFenceFn)(
    PalFence*,
    uint64_t);

/** This function has the same semantics and rules as palResetFence(). */
typedef PalResult(PAL_CALL* ResetFenceFn)(PalFence*);

/** This function has the same semantics and rules as palIsFenceSignaled(). */
typedef PalBool(PAL_CALL* IsFenceSignaledFn)(PalFence*);

/** This function has the same semantics and rules as palCreateSemaphore(). */
typedef PalResult(PAL_CALL* CreateSemaphoreFn)(
    PalDevice*,
    PalBool,
    PalSemaphore**);

/** This function has the same semantics and rules as palDestroySemaphore(). */
typedef void(PAL_CALL* DestroySemaphoreFn)(PalSemaphore*);

/** This function has the same semantics and rules as palWaitSemaphore(). */
typedef PalResult(PAL_CALL* WaitSemaphoreFn)(
    PalSemaphore*,
    uint64_t,
    uint64_t);

/** This function has the same semantics and rules as palSignalSemaphore(). */
typedef PalResult(PAL_CALL* SignalSemaphoreFn)(
    PalSemaphore*,
    PalQueue*,
    uint64_t);

/** This function has the same semantics and rules as palGetSemaphoreValue(). */
typedef PalResult(PAL_CALL* GetSemaphoreValueFn)(
    PalSemaphore*,
    uint64_t*);

/** This function has the same semantics and rules as palCreateCommandPool(). */
typedef PalResult(PAL_CALL* CreateCommandPoolFn)(
    PalDevice*,
    PalQueue*,
    PalCommandPool**);

/** This function has the same semantics and rules as palDestroyCommandPool(). */
typedef void(PAL_CALL* DestroyCommandPoolFn)(PalCommandPool*);

/** This function has the same semantics and rules as palAllocateCommandBuffer(). */
typedef PalResult(PAL_CALL* AllocateCommandBufferFn)(
    PalDevice*,
    PalCommandPool*,
    PalCommandBufferType,
    PalCommandBuffer**);

/** This function has the same semantics and rules as palFreeCommandBuffer(). */
typedef void(PAL_CALL* FreeCommandBufferFn)(PalCommandBuffer*);

/** This function has the same semantics and rules as palResetCommandBuffer(). */
typedef PalResult(PAL_CALL* ResetCommandBufferFn)(PalCommandBuffer*);

/** This function has the same semantics and rules as palSubmitCommandBuffer(). */
typedef PalResult(PAL_CALL* SubmitCommandBufferFn)(
    PalQueue*,
    PalCommandBufferSubmitInfo*);

/** This function has the same semantics and rules as palCmdBegin(). */
typedef PalResult(PAL_CALL* CmdBeginFn)(
    PalCommandBuffer*,
    PalRenderingLayoutInfo*);

/** This function has the same semantics and rules as palCmdEnd(). */
typedef PalResult(PAL_CALL* CmdEndFn)(PalCommandBuffer*);

/** This function has the same semantics and rules as palCmdExecuteCommandBuffer(). */
typedef void(PAL_CALL* CmdExecuteCommandBufferFn)(
    PalCommandBuffer*,
    PalCommandBuffer*);

/** This function has the same semantics and rules as palCmdSetFragmentShadingRate(). */
typedef void(PAL_CALL* CmdSetFragmentShadingRateFn)(
    PalCommandBuffer*,
    PalFragmentShadingRateState*);

/** This function has the same semantics and rules as palCmdDrawMeshTasks(). */
typedef void(PAL_CALL* CmdDrawMeshTasksFn)(
    PalCommandBuffer*,
    uint32_t,
    uint32_t,
    uint32_t);

/** This function has the same semantics and rules as palCmdDrawMeshTasksIndirect(). */
typedef void(PAL_CALL* CmdDrawMeshTasksIndirectFn)(
    PalCommandBuffer*,
    PalBuffer*,
    uint32_t);

/** This function has the same semantics and rules as palCmdDrawMeshTasksIndirectCount(). */
typedef void(PAL_CALL* CmdDrawMeshTasksIndirectCountFn)(
    PalCommandBuffer*,
    PalBuffer*,
    PalBuffer*,
    uint32_t);

/** This function has the same semantics and rules as palCmdBuildAccelerationStructure(). */
typedef void(PAL_CALL* CmdBuildAccelerationStructureFn)(
    PalCommandBuffer*,
    PalAccelerationStructureBuildInfo*);

/** This function has the same semantics and rules as palCmdBeginRendering(). */
typedef void(PAL_CALL* CmdBeginRenderingFn)(
    PalCommandBuffer*,
    PalRenderingInfo*);

/** This function has the same semantics and rules as palCmdEndRendering(). */
typedef void(PAL_CALL* CmdEndRenderingFn)(PalCommandBuffer*);

/** This function has the same semantics and rules as palCmdCopyBuffer(). */
typedef void(PAL_CALL* CmdCopyBufferFn)(
    PalCommandBuffer*,
    PalBuffer*,
    PalBuffer*,
    PalBufferCopyInfo*);

/** This function has the same semantics and rules as palCmdCopyBufferToImage(). */
typedef void(PAL_CALL* CmdCopyBufferToImageFn)(
    PalCommandBuffer*,
    PalImage*,
    PalBuffer*,
    PalBufferImageCopyInfo*);

/** This function has the same semantics and rules as palCmdCopyImage(). */
typedef void(PAL_CALL* CmdCopyImageFn)(
    PalCommandBuffer*,
    PalImage*,
    PalImage*,
    PalImageCopyInfo*);

/** This function has the same semantics and rules as palCmdCopyImageToBuffer(). */
typedef void(PAL_CALL* CmdCopyImageToBufferFn)(
    PalCommandBuffer*,
    PalBuffer*,
    PalImage*,
    PalBufferImageCopyInfo*);

/** This function has the same semantics and rules as palCmdBindPipeline(). */
typedef void(PAL_CALL* CmdBindPipelineFn)(
    PalCommandBuffer*,
    PalPipeline*);

/** This function has the same semantics and rules as palCmdSetViewport(). */
typedef void(PAL_CALL* CmdSetViewportFn)(
    PalCommandBuffer*,
    uint32_t,
    PalViewport*);

/** This function has the same semantics and rules as palCmdSetScissors(). */
typedef void(PAL_CALL* CmdSetScissorsFn)(
    PalCommandBuffer*,
    uint32_t,
    PalRect2D*);

/** This function has the same semantics and rules as palCmdBindVertexBuffers(). */
typedef void(PAL_CALL* CmdBindVertexBuffersFn)(
    PalCommandBuffer*,
    uint32_t,
    uint32_t,
    PalBuffer**,
    uint64_t*);

/** This function has the same semantics and rules as palCmdBindIndexBuffer(). */
typedef void(PAL_CALL* CmdBindIndexBufferFn)(
    PalCommandBuffer*,
    PalBuffer*,
    uint64_t,
    PalIndexType);

/** This function has the same semantics and rules as palCmdDraw(). */
typedef void(PAL_CALL* CmdDrawFn)(
    PalCommandBuffer*,
    uint32_t,
    uint32_t,
    uint32_t,
    uint32_t);

/** This function has the same semantics and rules as palCmdDrawIndirect(). */
typedef void(PAL_CALL* CmdDrawIndirectFn)(
    PalCommandBuffer*,
    PalBuffer*,
    uint32_t);

/** This function has the same semantics and rules as palCmdDrawIndirectCount(). */
typedef void(PAL_CALL* CmdDrawIndirectCountFn)(
    PalCommandBuffer*,
    PalBuffer*,
    PalBuffer*,
    uint32_t);

/** This function has the same semantics and rules as palCmdDrawIndexed(). */
typedef void(PAL_CALL* CmdDrawIndexedFn)(
    PalCommandBuffer*,
    uint32_t,
    uint32_t,
    uint32_t,
    int32_t,
    uint32_t);

/** This function has the same semantics and rules as palCmdDrawIndexedIndirect(). */
typedef void(PAL_CALL* CmdDrawIndexedIndirectFn)(
    PalCommandBuffer*,
    PalBuffer*,
    uint32_t);

/** This function has the same semantics and rules as palCmdDrawIndexedIndirectCount(). */
typedef void(PAL_CALL* CmdDrawIndexedIndirectCountFn)(
    PalCommandBuffer*,
    PalBuffer*,
    PalBuffer*,
    uint32_t);

/** This function has the same semantics and rules as palCmdAccelerationStructureBarrier(). */
typedef void(PAL_CALL* CmdAccelerationStructureBarrierFn)(
    PalCommandBuffer*,
    PalAccelerationStructure*,
    PalBarrierInfo*);

/** This function has the same semantics and rules as palCmdImageBarrier(). */
typedef void(PAL_CALL* CmdImageBarrierFn)(
    PalCommandBuffer*,
    PalImage*,
    PalImageSubresourceRange*,
    PalBarrierInfo*);

/** This function has the same semantics and rules as palCmdBufferBarrier(). */
typedef void(PAL_CALL* CmdBufferBarrierFn)(
    PalCommandBuffer*,
    PalBuffer*,
    PalBarrierInfo*);

/** This function has the same semantics and rules as palCmdDispatch(). */
typedef void(PAL_CALL* CmdDispatchFn)(
    PalCommandBuffer*,
    uint32_t,
    uint32_t,
    uint32_t);

/** This function has the same semantics and rules as palCmdDispatchBase(). */
typedef void(PAL_CALL* CmdDispatchBaseFn)(
    PalCommandBuffer*,
    uint32_t,
    uint32_t,
    uint32_t,
    uint32_t,
    uint32_t,
    uint32_t);

/** This function has the same semantics and rules as palCmdDispatchIndirect(). */
typedef void(PAL_CALL* CmdDispatchIndirectFn)(
    PalCommandBuffer*,
    PalBuffer*);

/** This function has the same semantics and rules as palCmdTraceRays(). */
typedef void(PAL_CALL* CmdTraceRaysFn)(
    PalCommandBuffer*,
    PalShaderBindingTable*,
    uint32_t,
    uint32_t,
    uint32_t,
    uint32_t);

/** This function has the same semantics and rules as palCmdTraceRaysIndirect(). */
typedef void(PAL_CALL* CmdTraceRaysIndirectFn)(
    PalCommandBuffer*,
    uint32_t,
    PalShaderBindingTable*,
    PalBuffer*);

/** This function has the same semantics and rules as palCmdBindDescriptorSet(). */
typedef void(PAL_CALL* CmdBindDescriptorSetFn)(
    PalCommandBuffer*,
    uint32_t,
    PalDescriptorSet*);

/** This function has the same semantics and rules as palCmdPushConstants(). */
typedef void(PAL_CALL* CmdPushConstantsFn)(
    PalCommandBuffer*,
    uint32_t,
    uint32_t,
    const void*);

/** This function has the same semantics and rules as palCmdSetCullMode(). */
typedef void(PAL_CALL* CmdSetCullModeFn)(
    PalCommandBuffer*,
    PalCullMode);

/** This function has the same semantics and rules as palCmdSetFrontFace(). */
typedef void(PAL_CALL* CmdSetFrontFaceFn)(
    PalCommandBuffer*,
    PalFrontFace);

/** This function has the same semantics and rules as palCmdSetPrimitiveTopology(). */
typedef void(PAL_CALL* CmdSetPrimitiveTopologyFn)(
    PalCommandBuffer*,
    PalPrimitiveTopology);

/** This function has the same semantics and rules as palCmdSetDepthTestEnable(). */
typedef void(PAL_CALL* CmdSetDepthTestEnableFn)(
    PalCommandBuffer*,
    PalBool);

/** This function has the same semantics and rules as palCmdSetDepthWriteEnable(). */
typedef void(PAL_CALL* CmdSetDepthWriteEnableFn)(
    PalCommandBuffer*,
    PalBool);

/** This function has the same semantics and rules as palCmdSetStencilOp(). */
typedef void(PAL_CALL* CmdSetStencilOpFn)(
    PalCommandBuffer*,
    PalStencilFaceFlags,
    PalStencilOp,
    PalStencilOp,
    PalStencilOp,
    PalCompareOp);

/** This function has the same semantics and rules as palCreateAccelerationstructure(). */
typedef PalResult(PAL_CALL* CreateAccelerationstructureFn)(
    PalDevice*,
    const PalAccelerationStructureCreateInfo*,
    PalAccelerationStructure**);

/** This function has the same semantics and rules as palDestroyAccelerationstructure(). */
typedef void(PAL_CALL* DestroyAccelerationstructureFn)(PalAccelerationStructure*);

/** This function has the same semantics and rules as palGetAccelerationStructureBuildSize(). */
typedef void(PAL_CALL* GetAccelerationStructureBuildSizeFn)(
    PalDevice*,
    PalAccelerationStructureBuildInfo*,
    PalAccelerationStructureBuildSize*);

/** This function has the same semantics and rules as palCreateBuffer(). */
typedef PalResult(PAL_CALL* CreateBufferFn)(
    PalDevice*,
    const PalBufferCreateInfo*,
    PalBuffer**);

/** This function has the same semantics and rules as palDestroyBuffer(). */
typedef void(PAL_CALL* DestroyBufferFn)(PalBuffer*);

/** This function has the same semantics and rules as palGetBufferMemoryRequirements(). */
typedef void(PAL_CALL* GetBufferMemoryRequirementsFn)(
    PalBuffer*,
    PalMemoryRequirements*);

/** This function has the same semantics and rules as palComputeInstanceStagingSize(). */
typedef void(PAL_CALL* ComputeInstanceStagingSizeFn)(
    PalDevice*,
    uint32_t,
    uint64_t*);

/** This function has the same semantics and rules as palComputeImageStagingRequirements(). */
typedef void(PAL_CALL* ComputeImageStagingRequirementsFn)(
    PalDevice*,
    PalFormat,
    const PalBufferImageCopyInfo*,
    PalImageStagingRequirements*);

/** This function has the same semantics and rules as palWriteInstanceStaging(). */
typedef void(PAL_CALL* WriteInstanceStagingFn)(
    PalDevice*,
    uint32_t,
    PalAccelerationStructureInstance*,
    void*);

/** This function has the same semantics and rules as palWriteImageStaging(). */
typedef void(PAL_CALL* WriteImageStagingFn)(
    PalDevice*,
    PalFormat,
    PalBufferImageCopyInfo*,
    void*,
    void*);

/** This function has the same semantics and rules as palBindBufferMemory(). */
typedef PalResult(PAL_CALL* BindBufferMemoryFn)(
    PalBuffer*,
    PalMemory*,
    uint64_t);

/** This function has the same semantics and rules as palMapBuffer(). */
typedef PalResult(PAL_CALL* MapBufferFn)(
    PalBuffer*,
    uint64_t,
    uint64_t,
    void**);

/** This function has the same semantics and rules as palUnmapBuffer(). */
typedef void(PAL_CALL* UnmapBufferFn)(PalBuffer*);

/** This function has the same semantics and rules as palGetBufferDeviceAddress(). */
typedef PalDeviceAddress(PAL_CALL* GetBufferDeviceAddressFn)(PalBuffer*);

/** This function has the same semantics and rules as palCreateDescriptorSetLayout(). */
typedef PalResult(PAL_CALL* CreateDescriptorSetLayoutFn)(
    PalDevice*,
    const PalDescriptorSetLayoutCreateInfo*,
    PalDescriptorSetLayout**);

/** This function has the same semantics and rules as palDestroyDescriptorSetLayout(). */
typedef void(PAL_CALL* DestroyDescriptorSetLayoutFn)(PalDescriptorSetLayout*);

/** This function has the same semantics and rules as palCreateDescriptorPool(). */
typedef PalResult(PAL_CALL* CreateDescriptorPoolFn)(
    PalDevice*,
    const PalDescriptorPoolCreateInfo*,
    PalDescriptorPool**);

/** This function has the same semantics and rules as palDestroyDescriptorPool(). */
typedef void(PAL_CALL* DestroyDescriptorPoolFn)(PalDescriptorPool*);

/** This function has the same semantics and rules as palResetDescriptorPool(). */
typedef PalResult(PAL_CALL* ResetDescriptorPoolFn)(PalDescriptorPool*);

/** This function has the same semantics and rules as palAllocateDescriptorSet(). */
typedef PalResult(PAL_CALL* AllocateDescriptorSetFn)(
    PalDevice*,
    PalDescriptorPool*,
    PalDescriptorSetLayout*,
    PalDescriptorSet**);

/** This function has the same semantics and rules as palUpdateDescriptorSet(). */
typedef PalResult(PAL_CALL* UpdateDescriptorSetFn)(
    PalDevice*,
    uint32_t,
    PalDescriptorSetWriteInfo*);

/** This function has the same semantics and rules as palCreatePipelineLayout(). */
typedef PalResult(PAL_CALL* CreatePipelineLayoutFn)(
    PalDevice*,
    const PalPipelineLayoutCreateInfo*,
    PalPipelineLayout**);

/** This function has the same semantics and rules as palDestroyPipelineLayout(). */
typedef void(PAL_CALL* DestroyPipelineLayoutFn)(PalPipelineLayout*);

/** This function has the same semantics and rules as palCreateGraphicsPipeline(). */
typedef PalResult(PAL_CALL* CreateGraphicsPipelineFn)(
    PalDevice*,
    const PalGraphicsPipelineCreateInfo*,
    PalPipeline**);

/** This function has the same semantics and rules as palCreateComputePipeline(). */
typedef PalResult(PAL_CALL* CreateComputePipelineFn)(
    PalDevice*,
    const PalComputePipelineCreateInfo*,
    PalPipeline**);

/** This function has the same semantics and rules as palCreateRayTracingPipeline(). */
typedef PalResult(PAL_CALL* CreateRayTracingPipelineFn)(
    PalDevice*,
    const PalRayTracingPipelineCreateInfo*,
    PalPipeline**);

/** This function has the same semantics and rules as palDestroyPipeline(). */
typedef void(PAL_CALL* DestroyPipelineFn)(PalPipeline*);

/** This function has the same semantics and rules as palCreateShaderBindingTable(). */
typedef PalResult(PAL_CALL* CreateShaderBindingTableFn)(
    PalDevice*,
    const PalShaderBindingTableCreateInfo*,
    PalShaderBindingTable**);

/** This function has the same semantics and rules as palDestroyShaderBindingTable(). */
typedef void(PAL_CALL* DestroyShaderBindingTableFn)(PalShaderBindingTable*);

/** This function has the same semantics and rules as palUpdateShaderBindingTable(). */
typedef void(PAL_CALL* UpdateShaderBindingTableFn)(
    PalShaderBindingTable*,
    uint32_t,
    PalShaderBindingTableRecordInfo*);
//clang-format on

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
    EnumerateAdaptersFn enumerateAdapters;
    GetAdapterInfoFn getAdapterInfo;
    GetAdapterCapabilitiesFn getAdapterCapabilities;
    GetAdapterFeaturesFn getAdapterFeatures;
    GetHighestSupportedShaderTargetFn getHighestSupportedShaderTarget;
    CreateDeviceFn createDevice;
    DestroyDeviceFn destroyDevice;
    GetDeviceLostReasonFn getDeviceLostReason;
    AllocateMemoryFn allocateMemory;
    FreeMemoryFn freeMemory;
    QuerySamplerAnisotropyCapabilitiesFn querySamplerAnisotropyCapabilities;
    QueryMultiViewCapabilitiesFn queryMultiViewCapabilities;
    QueryMultiViewportCapabilitiesFn queryMultiViewportCapabilities;
    QueryDepthStencilCapabilitiesFn queryDepthStencilCapabilities;
    QueryFragmentShadingRateCapabilitiesFn queryFragmentShadingRateCapabilities;
    QueryMeshShaderCapabilitiesFn queryMeshShaderCapabilities;
    QueryRayTracingCapabilitiesFn queryRayTracingCapabilities;
    QueryDescriptorIndexingCapabilitiesFn queryDescriptorIndexingCapabilities;
    CreateQueueFn createQueue;
    DestroyQueueFn destroyQueue;
    CanQueuePresentFn canQueuePresent;
    WaitQueueFn waitQueue;
    EnumerateFormatsFn enumerateFormats;
    IsFormatSupportedFn isFormatSupported;
    QueryFormatImageUsagesFn queryFormatImageUsages;
    QueryFormatSampleCountFn queryFormatSampleCount;
    CreateImageFn createImage;
    DestroyImageFn destroyImage;
    GetImageInfoFn getImageInfo;
    GetImageMemoryRequirementsFn getImageMemoryRequirements;
    BindImageMemoryFn bindImageMemory;
    CreateImageViewFn createImageView;
    DestroyImageViewFn destroyImageView;
    CreateSamplerFn createSampler;
    DestroySamplerFn destroySampler;
    CreateSurfaceFn createSurface;
    DestroySurfaceFn destroySurface;
    GetSurfaceCapabilitiesFn getSurfaceCapabilities;
    CreateSwapchainFn createSwapchain;
    DestroySwapchainFn destroySwapchain;
    GetSwapchainImageFn getSwapchainImage;
    GetNextSwapchainImageFn getNextSwapchainImage;
    PresentSwapchainFn presentSwapchain;
    ResizeSwapchainFn resizeSwapchain;
    CreateShaderFn createShader;
    DestroyShaderFn destroyShader;
    CreateFenceFn createFence;
    DestroyFenceFn destroyFence;
    WaitFenceFn waitFence;
    ResetFenceFn resetFence;
    IsFenceSignaledFn isFenceSignaled;
    CreateSemaphoreFn createSemaphore;
    DestroySemaphoreFn destroySemaphore;
    WaitSemaphoreFn waitSemaphore;
    SignalSemaphoreFn signalSemaphore;
    GetSemaphoreValueFn getSemaphoreValue;
    CreateCommandPoolFn createCommandPool;
    DestroyCommandPoolFn destroyCommandPool;
    AllocateCommandBufferFn allocateCommandBuffer;
    FreeCommandBufferFn freeCommandBuffer;
    ResetCommandBufferFn resetCommandBuffer;
    SubmitCommandBufferFn submitCommandBuffer;
    CmdBeginFn cmdBegin;
    CmdEndFn cmdEnd;
    CmdExecuteCommandBufferFn cmdExecuteCommandBuffer;
    CmdSetFragmentShadingRateFn cmdSetFragmentShadingRate;
    CmdDrawMeshTasksFn cmdDrawMeshTasks;
    CmdDrawMeshTasksIndirectFn cmdDrawMeshTasksIndirect;
    CmdDrawMeshTasksIndirectCountFn cmdDrawMeshTasksIndirectCount;
    CmdBuildAccelerationStructureFn cmdBuildAccelerationStructure;
    CmdBeginRenderingFn cmdBeginRendering;
    CmdEndRenderingFn cmdEndRendering;
    CmdCopyBufferFn cmdCopyBuffer;
    CmdCopyBufferToImageFn cmdCopyBufferToImage;
    CmdCopyImageFn cmdCopyImage;
    CmdCopyImageToBufferFn cmdCopyImageToBuffer;
    CmdBindPipelineFn cmdBindPipeline;
    CmdSetViewportFn cmdSetViewport;
    CmdSetScissorsFn cmdSetScissors;
    CmdBindVertexBuffersFn cmdBindVertexBuffers;
    CmdBindIndexBufferFn cmdBindIndexBuffer;
    CmdDrawFn cmdDraw;
    CmdDrawIndirectFn cmdDrawIndirect;
    CmdDrawIndirectCountFn cmdDrawIndirectCount;
    CmdDrawIndexedFn cmdDrawIndexed;
    CmdDrawIndexedIndirectFn cmdDrawIndexedIndirect;
    CmdDrawIndexedIndirectCountFn cmdDrawIndexedIndirectCount;
    CmdAccelerationStructureBarrierFn cmdAccelerationStructureBarrier;
    CmdImageBarrierFn cmdImageBarrier;
    CmdBufferBarrierFn cmdBufferBarrier;
    CmdDispatchFn cmdDispatch;
    CmdDispatchBaseFn cmdDispatchBase;
    CmdDispatchIndirectFn cmdDispatchIndirect;
    CmdTraceRaysFn cmdTraceRays;
    CmdTraceRaysIndirectFn cmdTraceRaysIndirect;
    CmdBindDescriptorSetFn cmdBindDescriptorSet;
    CmdPushConstantsFn cmdPushConstants;
    CmdSetCullModeFn cmdSetCullMode;
    CmdSetFrontFaceFn cmdSetFrontFace;
    CmdSetPrimitiveTopologyFn cmdSetPrimitiveTopology;
    CmdSetDepthTestEnableFn cmdSetDepthTestEnable;
    CmdSetDepthWriteEnableFn cmdSetDepthWriteEnable;
    CmdSetStencilOpFn cmdSetStencilOp;
    CreateAccelerationstructureFn createAccelerationstructure;
    DestroyAccelerationstructureFn destroyAccelerationstructure;
    GetAccelerationStructureBuildSizeFn getAccelerationStructureBuildSize;
    CreateBufferFn createBuffer;
    DestroyBufferFn destroyBuffer;
    GetBufferMemoryRequirementsFn getBufferMemoryRequirements;
    ComputeInstanceStagingSizeFn computeInstanceStagingSize;
    ComputeImageStagingRequirementsFn computeImageStagingRequirements;
    WriteInstanceStagingFn writeInstanceStaging;
    WriteImageStagingFn writeImageStaging;
    BindBufferMemoryFn bindBufferMemory;
    MapBufferFn mapBuffer;
    UnmapBufferFn unmapBuffer;
    GetBufferDeviceAddressFn getBufferDeviceAddress;
    CreateDescriptorSetLayoutFn createDescriptorSetLayout;
    DestroyDescriptorSetLayoutFn destroyDescriptorSetLayout;
    CreateDescriptorPoolFn createDescriptorPool;
    DestroyDescriptorPoolFn destroyDescriptorPool;
    ResetDescriptorPoolFn resetDescriptorPool;
    AllocateDescriptorSetFn allocateDescriptorSet;
    UpdateDescriptorSetFn updateDescriptorSet;
    CreatePipelineLayoutFn createPipelineLayout;
    DestroyPipelineLayoutFn destroyPipelineLayout;
    CreateGraphicsPipelineFn createGraphicsPipeline;
    CreateComputePipelineFn createComputePipeline;
    CreateRayTracingPipelineFn createRayTracingPipeline;
    DestroyPipelineFn destroyPipeline;
    CreateShaderBindingTableFn createShaderBindingTable;
    DestroyShaderBindingTableFn destroyShaderBindingTable;
    UpdateShaderBindingTableFn updateShaderBindingTable;
} PalGraphicsBackendVtable1;

#endif // PAL_GRAPHICS_VTABLE1_H