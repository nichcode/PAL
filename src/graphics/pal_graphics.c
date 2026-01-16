
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

#define MAX_BACKENDS 32
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

PalResult PAL_CALL enumerateVkAdapters(
    Int32* count,
    PalAdapter** outAdapters);

PalResult PAL_CALL getVkAdapterInfo(
    PalAdapter* adapter,
    PalAdapterInfo* info);

PalResult PAL_CALL getVkAdapterCapabilities(
    PalAdapter* adapter,
    PalAdapterCapabilities* caps);

PalAdapterFeatures PAL_CALL getVkAdapterFeatures(PalAdapter* adapter);

PalResult PAL_CALL createVkDevice(
    PalAdapter* adapter,
    PalAdapterFeatures features,
    PalDevice** outDevice);

void PAL_CALL destroyVkDevice(PalDevice* device);

PalResult PAL_CALL waitVkDevice(PalDevice* device);

PalResult PAL_CALL allocateVkMemory(
    PalDevice* device,
    PalMemoryType type,
    Uint64 size,
    PalMemory** outMemory);

void PAL_CALL freeVkMemory(
    PalDevice* device,
    PalMemory* memory);

PalResult PAL_CALL queryVkDepthStencilCapabilities(
    PalDevice* device,
    PalDepthStencilCapabilities* caps);

PalResult PAL_CALL queryVkFragmentShadingRateCapabilities(
    PalDevice* device,
    PalFragmentShadingRateCapabilities* caps);

PalResult PAL_CALL queryVkMeshShaderCapabilities(
    PalDevice* device,
    PalMeshShaderCapabilities* caps);

PalResult PAL_CALL queryVkRayTracingCapabilities(
    PalDevice* device,
    PalRayTracingCapabilities* caps);

PalResult PAL_CALL queryVkDescriptorIndexingCapabilities(
    PalDevice* device,
    PalDescriptorIndexingCapabilities* caps);

PalResult PAL_CALL createVkQueue(
    PalDevice* device,
    PalQueueType type,
    PalQueue** outQueue);

void PAL_CALL destroyVkQueue(PalQueue* queue);

PalResult PAL_CALL waitVkQueue(PalQueue* queue);

bool PAL_CALL canVkQueuePresent(
    PalQueue* queue,
    PalGraphicsWindow* window);

PalResult PAL_CALL enumerateVkFormats(
    PalAdapter* adapter,
    Int32* count,
    PalFormatInfo* outFormats);

bool PAL_CALL isVkFormatSupported(
    PalAdapter* adapter,
    PalFormat format);

PalImageUsages PAL_CALL queryVkFormatImageUsages(
    PalAdapter* adapter,
    PalFormat format);

PalImageViewUsages PAL_CALL queryVkFormatImageViewUsages(
    PalAdapter* adapter,
    PalFormat format);

PalResult PAL_CALL createVkImage(
    PalDevice* device,
    const PalImageCreateInfo* info,
    PalImage** outImage);

void PAL_CALL destroyVkImage(PalImage* image);

PalResult PAL_CALL getVkImageInfo(
    PalImage* image,
    PalImageInfo* info);

PalResult PAL_CALL getVkImageMemoryRequirements(
    PalImage* image,
    PalMemoryRequirements* requirements);

PalResult PAL_CALL bindVkImageMemory(
    PalImage* image,
    PalMemory* memory,
    Uint64 offset);

PalResult PAL_CALL createVkImageView(
    PalDevice* device,
    PalImage* image,
    const PalImageViewCreateInfo* info,
    PalImageView** outImageView);

void PAL_CALL destroyVkImageView(PalImageView* imageView);

PalResult PAL_CALL queryVkSwapchainCapabilities(
    PalDevice* device,
    PalGraphicsWindow* window,
    PalSwapchainCapabilities* caps);

PalResult PAL_CALL createVkSwapchain(
    PalDevice* device,
    PalQueue* queue,
    PalGraphicsWindow* window,
    const PalSwapchainCreateInfo* info,
    PalSwapchain** outSwapchain);

void PAL_CALL destroyVkSwapchain(PalSwapchain* swapchain);

PalImage* PAL_CALL getVkSwapchainImage(
    PalSwapchain* swapchain,
    Int32 index);

PalResult PAL_CALL getVkNextSwapchainImage(
    PalSwapchain* swapchain,
    PalSwapchainNextImageInfo* info,
    Uint32* outIndex);

PalResult PAL_CALL presentVkSwapchain(
    PalSwapchain* swapchain,
    PalSwapchainPresentInfo* info);

PalResult PAL_CALL createVkShader(
    PalDevice* device,
    const PalShaderCreateInfo* info,
    PalShader** outShader);

void PAL_CALL destroyVkShader(PalShader* shader);

PalResult PAL_CALL createVkFence(
    PalDevice* device,
    bool signaled,
    PalFence** outFence);

void PAL_CALL destroyVkFence(PalFence* fence);

PalResult PAL_CALL waitVkFence(
    PalFence* fence,
    Uint64 timeout);

PalResult PAL_CALL resetVkFence(PalFence* fence);

bool PAL_CALL isVkFenceSignaled(PalFence* fence);

PalResult PAL_CALL createVkSemaphore(
    PalDevice* device,
    PalSemaphore** outSemaphore);

void PAL_CALL destroyVkSemaphore(PalSemaphore* semaphore);

PalResult PAL_CALL waitVkSemaphore(
    PalSemaphore* semaphore,
    PalQueue* queue,
    Uint64 value,
    Uint64 timeout);

PalResult PAL_CALL signalVkSemaphore(
    PalSemaphore* semaphore,
    PalQueue* queue,
    Uint64 value);

PalResult PAL_CALL getVkSemaphoreValue(
    PalSemaphore* semaphore,
    Uint64* value);

PalResult PAL_CALL createVkCommandPool(
    PalDevice* device,
    PalQueue* queue,
    PalCommandPool** outPool);

void PAL_CALL destroyVkCommandPool(PalCommandPool* pool);

PalResult PAL_CALL resetVkCommandPool(PalCommandPool* pool);

PalResult PAL_CALL allocateVkCommandBuffer(
    PalDevice* device,
    PalCommandPool* pool,
    PalCommandBufferType type,
    PalCommandBuffer** outBuffer);

void PAL_CALL freeVkCommandBuffer(PalCommandBuffer* buffer);

PalResult PAL_CALL beginVkCommandBuffer(
    PalCommandBuffer* cmdBuffer,
    PalRenderingLayoutInfo* info);

PalResult PAL_CALL endVkCommandBuffer(PalCommandBuffer* cmdBuffer);

PalResult PAL_CALL resetVkCommandBuffer(PalCommandBuffer* cmdBuffer);

PalResult PAL_CALL executeCommandBufferVk(
    PalCommandBuffer* primaryCmdBuffer,
    PalCommandBuffer* secondaryCmdBuffer);

PalResult PAL_CALL setVkFragmentShadingRate(
    PalCommandBuffer* cmdBuffer,
    PalFragmentShadingRateState* state);

PalResult PAL_CALL drawVkMeshTasks(
    PalCommandBuffer* cmdBuffer,
    Uint32 groupCountX,
    Uint32 groupCountY,
    Uint32 groupCountZ);

PalResult PAL_CALL drawVkMeshTasksIndirect(
    PalCommandBuffer* cmdBuffer,
    PalBuffer* buffer,
    Uint64 offset,
    Uint32 drawCount,
    Uint32 stride);

PalResult PAL_CALL drawVkMeshTasksIndirectCount(
    PalCommandBuffer* cmdBuffer,
    PalBuffer* buffer,
    PalBuffer* countBuffer,
    Uint64 offset,
    Uint64 countBufferOffset,
    Uint32 maxDrawCount,
    Uint32 stride);

PalResult PAL_CALL buildVkAccelerationStructure(
    PalCommandBuffer* cmdBuffer,
    PalAccelerationStructureBuildInfo* info);

PalResult PAL_CALL beginRenderingVk(
    PalCommandBuffer* cmdBuffer,
    PalRenderingInfo* info);

PalResult PAL_CALL endRenderingVk(PalCommandBuffer* cmdBuffer);

PalResult PAL_CALL copyVkBuffer(
    PalCommandBuffer* cmdBuffer,
    PalBuffer* dst,
    PalBuffer* src,
    Uint64 dstOffset,
    Uint64 srcOffset,
    Uint32 size);

PalResult PAL_CALL bindVkPipeline(
    PalCommandBuffer* cmdBuffer,
    PalPipeline* pipeline);

PalResult PAL_CALL setVkViewport(
    PalCommandBuffer* cmdBuffer,
    Uint32 count,
    PalViewport* viewports);

PalResult PAL_CALL setVkScissors(
    PalCommandBuffer* cmdBuffer,
    Uint32 count,
    PalRect2D* scissors);

PalResult PAL_CALL bindVkVertexBuffers(
    PalCommandBuffer* cmdBuffer,
    Uint32 firstSlot,
    Uint32 count,
    PalBuffer** buffers,
    Uint64* offsets);

PalResult PAL_CALL bindVkIndexBuffer(
    PalCommandBuffer* cmdBuffer,
    PalBuffer* buffer,
    Uint64 offset,
    PalIndexType type);

PalResult PAL_CALL drawVk(
    PalCommandBuffer* cmdBuffer,
    PalDrawData* data);

PalResult PAL_CALL drawIndirectVk(
    PalCommandBuffer* cmdBuffer,
    PalBuffer* buffer,
    Uint64 offset,
    Uint32 count);

PalResult PAL_CALL drawIndirectCountVk(
    PalCommandBuffer* cmdBuffer,
    PalBuffer* buffer,
    PalBuffer* countBuffer,
    Uint64 offset,
    Uint64 countBufferOffset,
    Uint32 count);

PalResult PAL_CALL drawIndexedVk(
    PalCommandBuffer* cmdBuffer,
    PalDrawIndexedData* data);

PalResult PAL_CALL drawIndexedIndirectVk(
    PalCommandBuffer* cmdBuffer,
    PalBuffer* buffer,
    Uint64 offset,
    Uint32 count);

PalResult PAL_CALL drawIndexedIndirectCountVk(
    PalCommandBuffer* cmdBuffer,
    PalBuffer* buffer,
    PalBuffer* countBuffer,
    Uint64 offset,
    Uint64 countBufferOffset,
    Uint32 count);

PalResult PAL_CALL imageViewBarrierVk(
    PalCommandBuffer* cmdBuffer,
    PalImageView* imageView,
    PalUsageStateInfo* oldUsageStateInfo,
    PalUsageStateInfo* newUsageStateInfo);

PalResult PAL_CALL bufferBarrierVk(
    PalCommandBuffer* cmdBuffer,
    PalBuffer* buffer,
    PalUsageStateInfo* oldUsageStateInfo,
    PalUsageStateInfo* newUsageStateInfo);

PalResult PAL_CALL dispatchVk(
    PalCommandBuffer* cmdBuffer,
    Uint32 groupCountX,
    Uint32 groupCountY,
    Uint32 groupCountZ);

PalResult PAL_CALL dispatchBaseVk(
    PalCommandBuffer* cmdBuffer,
    Uint32 baseGroupX,
    Uint32 baseGroupY,
    Uint32 baseGroupZ,
    Uint32 groupCountX,
    Uint32 groupCountY,
    Uint32 groupCountZ);

PalResult PAL_CALL dispatchIndirectVk(
    PalCommandBuffer* cmdBuffer,
    PalBuffer* buffer,
    Uint64 offset);

PalResult PAL_CALL bindVkDescriptorSet(
    PalCommandBuffer* cmdBuffer,
    PalPipeline* pipeline,
    PalPipelineLayout* layout,
    Uint32 setIndex,
    PalDescriptorSet* set);

PalResult PAL_CALL pushConstantsVk(
    PalCommandBuffer* cmdBuffer,
    PalPipelineLayout* layout,
    Uint32 shaderStageCount,
    PalShaderStage* shaderStages,
    Uint32 offset,
    Uint32 size,
    const void* value);

PalResult PAL_CALL submitVkCommandBuffer(
    PalQueue* queue,
    PalCommandBufferSubmitInfo* info);

PalResult PAL_CALL createVkAccelerationstructure(
    PalDevice* device,
    const PalAccelerationStructureCreateInfo* info,
    PalAccelerationStructure** outAs);

void PAL_CALL destroyVkAccelerationstructure(PalAccelerationStructure* as);

PalResult PAL_CALL getVkAccelerationStructureBuildSize(
    PalDevice* device,
    PalAccelerationStructureBuildInfo* info,
    PalAccelerationStructureBuildSize* size);

PalResult PAL_CALL createVkBuffer(
    PalDevice* device,
    const PalBufferCreateInfo* info,
    PalBuffer** outBuffer);

void PAL_CALL destroyVkBuffer(PalBuffer* buffer);

PalResult PAL_CALL getVkBufferMemoryRequirements(
    PalBuffer* buffer,
    PalMemoryRequirements* requirements);

PalResult PAL_CALL bindVkBufferMemory(
    PalBuffer* buffer,
    PalMemory* memory,
    Uint64 offset);

PalResult PAL_CALL mapVkMemory(
    PalDevice* device,
    PalMemory* memory,
    Uint64 offset,
    Uint64 size,
    void** outPtr);

void PAL_CALL unmapVkMemory(
    PalDevice* device,
    PalMemory* memory);

PalResult PAL_CALL createVkDescriptorSetLayout(
    PalDevice* device,
    const PalDescriptorSetLayoutCreateInfo* info,
    PalDescriptorSetLayout** outLayout);

void PAL_CALL destroyVkDescriptorSetLayout(PalDescriptorSetLayout* layout);

PalResult PAL_CALL createVkDescriptorPool(
    PalDevice* device,
    const PalDescriptorPoolCreateInfo* info,
    PalDescriptorPool** outPool);

void PAL_CALL destroyVkDescriptorPool(PalDescriptorPool* pool);

PalResult PAL_CALL resetVkDescriptorPool(PalDescriptorPool* pool);

PalResult PAL_CALL allocateVkDescriptorSet(
    PalDevice* device,
    PalDescriptorPool* pool,
    PalDescriptorSetLayout* layout,
    PalDescriptorSet** outSet);

void PAL_CALL freeVkDescriptorSet(PalDescriptorSet* set);

PalResult PAL_CALL updateVkDescriptorSet(
    PalDevice* device,
    Uint32 count,
    PalDescriptorSetWriteInfo* infos);

PalResult PAL_CALL createVkPipelineLayout(
    PalDevice* device,
    const PalPipelineLayoutCreateInfo* info,
    PalPipelineLayout** outLayout);

void PAL_CALL destroyVkPipelineLayout(PalPipelineLayout* layout);

PalResult PAL_CALL createVkGraphicsPipeline(
    PalDevice* device,
    const PalGraphicsPipelineCreateInfo* info,
    PalPipeline** outPipeline);

PalResult PAL_CALL createVkComputePipeline(
    PalDevice* device,
    const PalComputePipelineCreateInfo* info,
    PalPipeline** outPipeline);

void PAL_CALL destroyVkPipeline(PalPipeline* pipeline);

static PalGraphicsBackend s_VkBackend = {
    .enumerateAdapters = enumerateVkAdapters,
    .getAdapterInfo = getVkAdapterInfo,
    .getAdapterCapabilities = getVkAdapterCapabilities,
    .getAdapterFeatures = getVkAdapterFeatures,
    .createDevice = createVkDevice,
    .destroyDevice = destroyVkDevice,
    .waitDevice = waitVkDevice,
    .allocateMemory = allocateVkMemory,
    .freeMemory = freeVkMemory,
    .mapMemory = mapVkMemory,
    .unmapMemory = unmapVkMemory,
    .queryDepthStencilCapabilities = queryVkDepthStencilCapabilities,
    .queryFragmentShadingRateCapabilities = queryVkFragmentShadingRateCapabilities,
    .queryMeshShaderCapabilities = queryVkMeshShaderCapabilities,
    .queryRayTracingCapabilities = queryVkRayTracingCapabilities,
    .queryDescriptorIndexingCapabilities = queryVkDescriptorIndexingCapabilities,
    .createQueue = createVkQueue,
    .destroyQueue = destroyVkQueue,
    .waitQueue = waitVkQueue,
    .canQueuePresent = canVkQueuePresent,
    .enumerateFormats = enumerateVkFormats,
    .isFormatSupported = isVkFormatSupported,
    .queryFormatImageUsages = queryVkFormatImageUsages,
    .queryFormatImageViewUsages = queryVkFormatImageViewUsages,
    .createImage = createVkImage,
    .destroyImage = destroyVkImage,
    .getImageInfo = getVkImageInfo,
    .getImageMemoryRequirements = getVkImageMemoryRequirements,
    .bindImageMemory = bindVkImageMemory,
    .createImageView = createVkImageView,
    .destroyImageView = destroyVkImageView,
    .querySwapchainCapabilities = queryVkSwapchainCapabilities,
    .createSwapchain = createVkSwapchain,
    .destroySwapchain = destroyVkSwapchain,
    .getSwapchainImage = getVkSwapchainImage,
    .getNextSwapchainImage = getVkNextSwapchainImage,
    .presentSwapchain = presentVkSwapchain,
    .createShader = createVkShader,
    .destroyShader = destroyVkShader,
    .createFence = createVkFence,
    .destroyFence = destroyVkFence,
    .waitFenceTimeout = waitVkFence,
    .resetFence = resetVkFence,
    .isFenceSignaled = isVkFenceSignaled,
    .createSemaphore = createVkSemaphore,
    .destroySemaphore = destroyVkSemaphore,
    .waitSemaphore = waitVkSemaphore,
    .signalSemaphore = signalVkSemaphore,
    .getSemaphoreValue = getVkSemaphoreValue,
    .createCommandPool = createVkCommandPool,
    .destroyCommandPool = destroyVkCommandPool,
    .resetCommandPool = resetVkCommandPool,
    .allocateCommandBuffer = allocateVkCommandBuffer,
    .freeCommandBuffer = freeVkCommandBuffer,
    .beginCommandBuffer = beginVkCommandBuffer,
    .endCommandBuffer = endVkCommandBuffer,
    .resetCommandBuffer = resetVkCommandBuffer,
    .executeCommandBuffer = executeCommandBufferVk,
    .setFragmentShadingRate = setVkFragmentShadingRate,
    .drawMeshTasks = drawVkMeshTasks,
    .drawMeshTasksIndirect = drawVkMeshTasksIndirect,
    .drawMeshTasksIndirectCount = drawVkMeshTasksIndirectCount,
    .buildAccelerationStructure = buildVkAccelerationStructure,
    .beginRendering = beginRenderingVk,
    .endRendering = endRenderingVk,
    .copyBuffer = copyVkBuffer,
    .bindPipeline = bindVkPipeline,
    .setViewport = setVkViewport,
    .setScissors = setVkScissors,
    .bindVertexBuffers = bindVkVertexBuffers,
    .bindIndexBuffer = bindVkIndexBuffer,
    .draw = drawVk,
    .drawIndirect = drawIndirectVk,
    .drawIndexedIndirectCount = drawIndirectCountVk,
    .drawIndexed = drawIndexedVk,
    .drawIndexedIndirect = drawIndexedIndirectVk,
    .drawIndexedIndirectCount = drawIndexedIndirectCountVk,
    .imageViewBarrier = imageViewBarrierVk,
    .bufferBarrier = bufferBarrierVk,
    .dispatch = dispatchVk,
    .dispatchBase = dispatchBaseVk,
    .dispatchIndirect = dispatchIndirectVk,
    .bindDescriptorSet = bindVkDescriptorSet,
    .pushConstants = pushConstantsVk,
    .submitCommandBuffer = submitVkCommandBuffer,
    .createAccelerationstructure = createVkAccelerationstructure,
    .destroyAccelerationstructure = destroyVkAccelerationstructure,
    .getAccelerationStructureBuildSize = getVkAccelerationStructureBuildSize,
    .createBuffer = createVkBuffer,
    .destroyBuffer = destroyVkBuffer,
    .getBufferMemoryRequirements = getVkBufferMemoryRequirements,
    .bindBufferMemory = bindVkBufferMemory,
    .createDescriptorSetLayout = createVkDescriptorSetLayout,
    .destroyDescriptorSetLayout = destroyVkDescriptorSetLayout,
    .createDescriptorPool = createVkDescriptorPool,
    .destroyDescriptorPool = destroyVkDescriptorPool,
    .resetDescriptorPool = resetVkDescriptorPool,
    .allocateDescriptorSet = allocateVkDescriptorSet,
    .updateDescriptorSet = updateVkDescriptorSet,
    .createPipelineLayout = createVkPipelineLayout,
    .destroyPipelineLayout = destroyVkPipelineLayout,
    .createGraphicsPipeline = createVkGraphicsPipeline,
    .createComputePipeline = createVkComputePipeline,
    .destroyPipeline = destroyVkPipeline};

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
    if (!backend->enumerateAdapters                     ||
        !backend->getAdapterInfo                        ||
        !backend->getAdapterCapabilities                ||
        !backend->getAdapterFeatures                    ||
        !backend->createDevice                          ||
        !backend->destroyDevice                         ||
        !backend->waitDevice                            ||
        !backend->allocateMemory                        ||
        !backend->freeMemory                            ||
        !backend->queryDepthStencilCapabilities         ||
        !backend->queryFragmentShadingRateCapabilities  ||
        !backend->queryMeshShaderCapabilities           ||
        !backend->queryRayTracingCapabilities           ||
        !backend->queryDescriptorIndexingCapabilities   ||
        !backend->createQueue                           ||
        !backend->destroyQueue                          ||
        !backend->waitQueue                             ||
        !backend->canQueuePresent                       ||
        !backend->enumerateFormats                      ||
        !backend->isFormatSupported                     ||
        !backend->queryFormatImageUsages                ||
        !backend->queryFormatImageViewUsages            ||
        !backend->createImage                           ||
        !backend->destroyImage                          ||
        !backend->getImageInfo                          ||
        !backend->getImageMemoryRequirements            ||
        !backend->bindImageMemory                       ||
        !backend->createImageView                       ||
        !backend->destroyImageView                      ||
        !backend->querySwapchainCapabilities            ||
        !backend->createSwapchain                       ||
        !backend->destroySwapchain                      ||
        !backend->getSwapchainImage                     ||
        !backend->getNextSwapchainImage                 ||
        !backend->presentSwapchain                      ||
        !backend->createShader                          ||
        !backend->destroyShader                         ||
        !backend->createFence                           ||
        !backend->destroyFence                          ||
        !backend->waitFenceTimeout                      ||
        !backend->resetFence                            ||
        !backend->isFenceSignaled                       ||
        !backend->createSemaphore                       ||
        !backend->destroySemaphore                      ||
        !backend->waitSemaphore                         ||
        !backend->signalSemaphore                       ||
        !backend->getSemaphoreValue                     ||
        !backend->createCommandPool                     ||
        !backend->destroyCommandPool                    ||
        !backend->resetCommandPool                      ||
        !backend->allocateCommandBuffer                 ||
        !backend->freeCommandBuffer                     ||
        !backend->beginCommandBuffer                    ||
        !backend->endCommandBuffer                      ||
        !backend->resetCommandBuffer                    ||
        !backend->executeCommandBuffer                  ||
        !backend->setFragmentShadingRate                ||
        !backend->drawMeshTasks                         ||
        !backend->drawMeshTasksIndirect                 ||
        !backend->drawMeshTasksIndirectCount            ||
        !backend->buildAccelerationStructure            ||
        !backend->beginRendering                        ||
        !backend->endRendering                          ||
        !backend->copyBuffer                            ||
        !backend->bindPipeline                          ||
        !backend->setViewport                           ||
        !backend->setScissors                           ||
        !backend->bindVertexBuffers                     ||
        !backend->bindIndexBuffer                       ||
        !backend->draw                                  ||
        !backend->drawIndirect                          ||
        !backend->drawIndirectCount                     ||
        !backend->drawIndexed                           ||
        !backend->drawIndexedIndirect                   ||
        !backend->drawIndexedIndirectCount              ||
        !backend->imageViewBarrier                      ||
        !backend->bufferBarrier                         ||
        !backend->dispatch                              ||
        !backend->dispatchBase                          ||
        !backend->dispatchIndirect                      ||
        !backend->bindDescriptorSet                     ||
        !backend->pushConstants                         ||
        !backend->submitCommandBuffer                   ||
        !backend->createAccelerationstructure           ||
        !backend->destroyAccelerationstructure          ||
        !backend->getAccelerationStructureBuildSize     ||
        !backend->createBuffer                          ||
        !backend->destroyBuffer                         ||
        !backend->getBufferMemoryRequirements           ||
        !backend->bindBufferMemory                      ||
        !backend->mapMemory                             ||
        !backend->unmapMemory                           ||
        !backend->createDescriptorSetLayout             ||
        !backend->destroyDescriptorSetLayout            ||
        !backend->createDescriptorPool                  ||
        !backend->destroyDescriptorPool                 ||
        !backend->resetDescriptorPool                   ||
        !backend->allocateDescriptorSet                 ||
        !backend->updateDescriptorSet                   ||
        !backend->createPipelineLayout                  ||
        !backend->destroyPipelineLayout                 ||
        !backend->createGraphicsPipeline                ||
        !backend->createComputePipeline                 ||
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

            for (int i = 0; i < _count; i++) {
                adapters[i]->backend = backend->base;
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
    if (!s_Graphics.initialized) {
        return 0;
    }

    if (!adapter) {
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
    Uint64 size,
    PalMemory** outMemory)
{
    if (!s_Graphics.initialized) {
        return PAL_RESULT_GRAPHICS_NOT_INITIALIZED;
    }

    if (!outMemory || !device) {
        return PAL_RESULT_NULL_POINTER;
    }

    return device->backend->allocateMemory(device, type, size, outMemory);
}

void PAL_CALL palFreeMemory(
    PalDevice* device,
    PalMemory* memory)
{
    if (s_Graphics.initialized && device && memory) {
        device->backend->freeMemory(device, memory);
    }
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

    return fence->backend->waitFenceTimeout(fence, timeout);
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
    PalQueue* queue,
    Uint64 value,
    Uint64 timeout)
{
    if (!s_Graphics.initialized) {
        return PAL_RESULT_GRAPHICS_NOT_INITIALIZED;
    }

    if (!semaphore || !queue) {
        return PAL_RESULT_NULL_POINTER;
    }

    return semaphore->backend->waitSemaphore(semaphore, queue, value, timeout);
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

PalResult PAL_CALL palBeginCommandBuffer(
    PalCommandBuffer* cmdBuffer,
    PalRenderingLayoutInfo* info)
{
    if (!s_Graphics.initialized) {
        return PAL_RESULT_GRAPHICS_NOT_INITIALIZED;
    }

    if (!cmdBuffer) {
        return PAL_RESULT_NULL_POINTER;
    }

    return cmdBuffer->backend->beginCommandBuffer(cmdBuffer, info);
}

PalResult PAL_CALL palEndCommandBuffer(PalCommandBuffer* cmdBuffer)
{
    if (!s_Graphics.initialized) {
        return PAL_RESULT_GRAPHICS_NOT_INITIALIZED;
    }

    if (!cmdBuffer) {
        return PAL_RESULT_NULL_POINTER;
    }

    return cmdBuffer->backend->endCommandBuffer(cmdBuffer);
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

PalResult PAL_CALL palExecuteCommandBuffer(
    PalCommandBuffer* primaryCmdBuffer,
    PalCommandBuffer* secondaryCmdBuffer)
{
    if (!s_Graphics.initialized) {
        return PAL_RESULT_GRAPHICS_NOT_INITIALIZED;
    }

    if (!primaryCmdBuffer || !secondaryCmdBuffer) {
        return PAL_RESULT_NULL_POINTER;
    }

    return primaryCmdBuffer->backend->executeCommandBuffer(primaryCmdBuffer, secondaryCmdBuffer);
}

PalResult PAL_CALL palSetFragmentShadingRate(
    PalCommandBuffer* cmdBuffer,
    PalFragmentShadingRateState* state)
{
    if (!s_Graphics.initialized) {
        return PAL_RESULT_GRAPHICS_NOT_INITIALIZED;
    }

    if (!cmdBuffer || !state) {
        return PAL_RESULT_NULL_POINTER;
    }

    return cmdBuffer->backend->setFragmentShadingRate(cmdBuffer, state);
}

PalResult PAL_CALL palDrawMeshTasks(
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

    return cmdBuffer->backend->drawMeshTasks(cmdBuffer, groupCountX, groupCountY, groupCountZ);
}

PalResult PAL_CALL palDrawMeshTasksIndirect(
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

    return cmdBuffer->backend->drawMeshTasksIndirect(cmdBuffer, buffer, offset, drawCount, stride);
}

PalResult PAL_CALL palDrawMeshTasksIndirectCount(
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

    return cmdBuffer->backend->drawMeshTasksIndirectCount(
        cmdBuffer,
        buffer,
        countBuffer,
        offset,
        countBufferOffset,
        maxDrawCount,
        stride);
}

PalResult PAL_CALL palBuildAccelerationStructures(
    PalCommandBuffer* cmdBuffer,
    PalAccelerationStructureBuildInfo* info)
{
    if (!s_Graphics.initialized) {
        return PAL_RESULT_GRAPHICS_NOT_INITIALIZED;
    }

    if (!cmdBuffer) {
        return PAL_RESULT_NULL_POINTER;
    }

    return cmdBuffer->backend->buildAccelerationStructure(cmdBuffer, info);
}

PalResult PAL_CALL palBeginRendering(
    PalCommandBuffer* cmdBuffer,
    PalRenderingInfo* info)
{
    if (!s_Graphics.initialized) {
        return PAL_RESULT_GRAPHICS_NOT_INITIALIZED;
    }

    if (!cmdBuffer || !info) {
        return PAL_RESULT_NULL_POINTER;
    }

    return cmdBuffer->backend->beginRendering(cmdBuffer, info);
}

PalResult PAL_CALL palEndRendering(PalCommandBuffer* cmdBuffer)
{
    if (!s_Graphics.initialized) {
        return PAL_RESULT_GRAPHICS_NOT_INITIALIZED;
    }

    if (!cmdBuffer) {
        return PAL_RESULT_NULL_POINTER;
    }

    return cmdBuffer->backend->endRendering(cmdBuffer);
}

PalResult PAL_CALL palCopyBuffer(
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

    return cmdBuffer->backend->copyBuffer(cmdBuffer, dst, src, dstOffset, srcOffset, size);
}

PalResult PAL_CALL palBindPipeline(
    PalCommandBuffer* cmdBuffer,
    PalPipeline* pipeline)
{
    if (!s_Graphics.initialized) {
        return PAL_RESULT_GRAPHICS_NOT_INITIALIZED;
    }

    if (!cmdBuffer || !pipeline) {
        return PAL_RESULT_NULL_POINTER;
    }

    return cmdBuffer->backend->bindPipeline(cmdBuffer, pipeline);
}

PalResult PAL_CALL palSetViewport(
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

    return cmdBuffer->backend->setViewport(cmdBuffer, count, viewports);
}

PalResult PAL_CALL palSetScissors(
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

    return cmdBuffer->backend->setScissors(cmdBuffer, count, scissors);
}

PalResult PAL_CALL palBindVertexBuffers(
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

    return cmdBuffer->backend->bindVertexBuffers(cmdBuffer, firstSlot, count, buffers, offsets);
}

PalResult PAL_CALL palBindIndexBuffer(
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

    return cmdBuffer->backend->bindIndexBuffer(cmdBuffer, buffer, offset, type);
}

PalResult PAL_CALL palDraw(
    PalCommandBuffer* cmdBuffer,
    PalDrawData* data)
{
    if (!s_Graphics.initialized) {
        return PAL_RESULT_GRAPHICS_NOT_INITIALIZED;
    }

    if (!cmdBuffer || !data) {
        return PAL_RESULT_NULL_POINTER;
    }

    return cmdBuffer->backend->draw(cmdBuffer, data);
}

PalResult PAL_CALL palDrawIndirect(
    PalCommandBuffer* cmdBuffer,
    PalBuffer* buffer,
    Uint64 offset,
    Uint32 count)
{
    if (!s_Graphics.initialized) {
        return PAL_RESULT_GRAPHICS_NOT_INITIALIZED;
    }

    if (!cmdBuffer || !buffer) {
        return PAL_RESULT_NULL_POINTER;
    }

    return cmdBuffer->backend->drawIndirect(cmdBuffer, buffer, offset, count);
}

PalResult PAL_CALL palDrawIndirectCount(
    PalCommandBuffer* cmdBuffer,
    PalBuffer* buffer,
    PalBuffer* countBuffer,
    Uint64 offset,
    Uint64 countBufferOffset,
    Uint32 count)
{
    if (!s_Graphics.initialized) {
        return PAL_RESULT_GRAPHICS_NOT_INITIALIZED;
    }

    if (!cmdBuffer || !buffer || !countBuffer) {
        return PAL_RESULT_NULL_POINTER;
    }

    return cmdBuffer->backend
        ->drawIndirectCount(cmdBuffer, buffer, countBuffer, offset, countBufferOffset, count);
}

PalResult PAL_CALL palDrawIndexed(
    PalCommandBuffer* cmdBuffer,
    PalDrawIndexedData* data)
{
    if (!s_Graphics.initialized) {
        return PAL_RESULT_GRAPHICS_NOT_INITIALIZED;
    }

    if (!cmdBuffer || !data) {
        return PAL_RESULT_NULL_POINTER;
    }

    return cmdBuffer->backend->drawIndexed(cmdBuffer, data);
}

PalResult PAL_CALL palDrawIndexedIndirect(
    PalCommandBuffer* cmdBuffer,
    PalBuffer* buffer,
    Uint64 offset,
    Uint32 count)
{
    if (!s_Graphics.initialized) {
        return PAL_RESULT_GRAPHICS_NOT_INITIALIZED;
    }

    if (!cmdBuffer || !buffer) {
        return PAL_RESULT_NULL_POINTER;
    }

    return cmdBuffer->backend->drawIndexedIndirect(cmdBuffer, buffer, offset, count);
}

PalResult PAL_CALL palDrawIndexedIndirectCount(
    PalCommandBuffer* cmdBuffer,
    PalBuffer* buffer,
    PalBuffer* countBuffer,
    Uint64 offset,
    Uint64 countBufferOffset,
    Uint32 count)
{
    if (!s_Graphics.initialized) {
        return PAL_RESULT_GRAPHICS_NOT_INITIALIZED;
    }

    if (!cmdBuffer || !buffer || !countBuffer) {
        return PAL_RESULT_NULL_POINTER;
    }

    return cmdBuffer->backend->drawIndexedIndirectCount(
        cmdBuffer,
        buffer,
        countBuffer,
        offset,
        countBufferOffset,
        count);
}

PalResult PAL_CALL palImageViewBarrier(
    PalCommandBuffer* cmdBuffer,
    PalImageView* imageView,
    PalUsageStateInfo* oldUsageStateInfo,
    PalUsageStateInfo* newUsageStateInfo)
{
    if (!s_Graphics.initialized) {
        return PAL_RESULT_GRAPHICS_NOT_INITIALIZED;
    }

    if (!cmdBuffer || !imageView) {
        return PAL_RESULT_NULL_POINTER;
    }

    return cmdBuffer->backend->imageViewBarrier(
        cmdBuffer,
        imageView,
        oldUsageStateInfo,
        newUsageStateInfo);
}

PalResult PAL_CALL palBufferBarrier(
    PalCommandBuffer* cmdBuffer,
    PalBuffer* buffer,
    PalUsageStateInfo* oldUsageStateInfo,
    PalUsageStateInfo* newUsageStateInfo)
{
    if (!s_Graphics.initialized) {
        return PAL_RESULT_GRAPHICS_NOT_INITIALIZED;
    }

    if (!cmdBuffer || !buffer) {
        return PAL_RESULT_NULL_POINTER;
    }

    return cmdBuffer->backend->bufferBarrier(
        cmdBuffer,
        buffer,
        oldUsageStateInfo,
        newUsageStateInfo);
}

PalResult PAL_CALL palDispatch(
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

    return cmdBuffer->backend->dispatch(cmdBuffer, groupCountX, groupCountY, groupCountZ);
}

PalResult PAL_CALL palDispatchBase(
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

    return cmdBuffer->backend->dispatchBase(
        cmdBuffer,
        baseGroupX,
        baseGroupY,
        baseGroupZ,
        groupCountX,
        groupCountY,
        groupCountZ);
}

PalResult PAL_CALL palDispatchIndirect(
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

    return cmdBuffer->backend->dispatchIndirect(cmdBuffer, buffer, offset);
}

PalResult PAL_CALL palBindDescriptorSet(
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

    return cmdBuffer->backend->bindDescriptorSet(cmdBuffer, pipeline, layout, setIndex, set);
}

PalResult PAL_CALL palPushConstants(
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

    return cmdBuffer->backend->pushConstants(
        cmdBuffer,
        layout,
        shaderStageCount,
        shaderStages,
        offset,
        size,
        value);
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
// Ray Tracing Pipeline
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

    if (!info->scratchBuffer || !info->dst) {
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
// Pipeline
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
    PalWorkGroupInfo* info)
{
    if (!data) {
        return false;
    }

    Uint32 workGroupCount[3];
    Uint32 groupInfoCount[3];
    for (int i = 0; i < 3; i++) {
        Uint32 tmp = _ceil(data->workCount[i], data->workGroupSize[i]);
        workGroupCount[i] = tmp;
        groupInfoCount[i] = _ceil(tmp, data->workGroupCount[i]);
    }

    if (!info) {
        // total number of group build info on all axis
        *count = groupInfoCount[0] * groupInfoCount[1] * groupInfoCount[2];
        ;
        return true;
    }

    for (int i = 0; i < *count; i++) {
        PalWorkGroupInfo* buildInfo = &info[i];
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
