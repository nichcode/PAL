
#include "pal/pal_graphics.h"
#include "tests.h"

static PalResult PAL_CALL enumerateAdapters(
    uint32_t*, 
    PalAdapter**)
{

}

static void PAL_CALL getAdapterInfo(
    PalAdapter*, 
    PalAdapterInfo*)
{

}

static void PAL_CALL getAdapterCapabilities(
    PalAdapter*, 
    PalAdapterCapabilities*)
{

}

static PalAdapterFeatures PAL_CALL getAdapterFeatures(PalAdapter*)
{

}

static uint32_t PAL_CALL getHighestSupportedShaderTarget(
    PalAdapter*, 
    PalShaderFormats)
{

}

static PalResult PAL_CALL createDevice(
    PalAdapter*, 
    PalAdapterFeatures, 
    PalDevice**)
{

}

static void PAL_CALL destroyDevice(PalDevice*)
{

}

static PalResult PAL_CALL waitDevice(PalDevice*)
{

}

static PalResult PAL_CALL allocateMemory(
    PalDevice*, 
    PalMemoryType, 
    uint64_t, 
    uint64_t, 
    PalMemory**)
{

}

static void PAL_CALL freeMemory(PalMemory*)
{

}

static void PAL_CALL querySamplerAnisotropyCapabilities(
    PalDevice*, 
    PalSamplerAnisotropyCapabilities*)
{

}

static PalResult PAL_CALL createQueue(
    PalDevice*, 
    PalQueueType, 
    PalQueue**)
{

}

static void PAL_CALL destroyQueue(PalQueue*)
{

}

static PalResult PAL_CALL waitQueue(PalQueue*)
{

}

static PalBool PAL_CALL canQueuePresent(
    PalQueue*, 
    PalSurface*)
{

}

static void PAL_CALL enumerateFormats(
    PalAdapter*, 
    uint32_t*, 
    PalFormatInfo*)
{

}

static PalBool PAL_CALL isFormatSupported(
    PalAdapter*, 
    PalFormat)
{

}

static PalImageUsages PAL_CALL queryFormatImageUsages(
    PalAdapter*, 
    PalFormat)
{

}

static PalSampleCount PAL_CALL queryFormatSampleCount(
    PalAdapter*, 
    PalFormat)
{

}

static PalResult PAL_CALL createImage(
    PalDevice*, 
    const PalImageCreateInfo*, 
    PalImage**)
{

}

static void PAL_CALL destroyImage(PalImage*)
{

}

static void PAL_CALL getImageInfo(
    PalImage*, 
    PalImageInfo*)
{

}

static void PAL_CALL getImageMemoryRequirements(
    PalImage*, 
    PalMemoryRequirements*)
{

}

static PalResult PAL_CALL bindImageMemory(
    PalImage*, 
    PalMemory*, 
    uint64_t)
{

}

static PalResult PAL_CALL createImageView(
    PalDevice*, 
    PalImage*, 
    const PalImageViewCreateInfo*, 
    PalImageView**)
{

}

static void PAL_CALL destroyImageView(PalImageView*)
{

}

static PalResult PAL_CALL createSampler(
    PalDevice*, 
    const PalSamplerCreateInfo*, 
    PalSampler**)
{

}

static void PAL_CALL destroySampler(PalSampler*)
{

}

static PalResult PAL_CALL createShader(
    PalDevice*, 
    const PalShaderCreateInfo*, 
    PalShader**)
{

}

static void PAL_CALL destroyShader(PalShader*)
{

}

static PalResult PAL_CALL createFence(
    PalDevice*, 
    PalBool, 
    PalFence**)
{

}

static void PAL_CALL destroyFence(PalFence*)
{

}

static PalResult PAL_CALL waitFence(
    PalFence*, 
    uint64_t)
{

}

static PalResult PAL_CALL resetFence(PalFence*)
{

}

static PalBool PAL_CALL isFenceSignaled(PalFence*)
{

}

static PalResult PAL_CALL createSemaphore(
    PalDevice*, 
    PalBool, 
    PalSemaphore**)
{

}

static void PAL_CALL destroySemaphore(PalSemaphore*)
{

}

static PalResult PAL_CALL createCommandPool(
    PalDevice*, 
    PalQueue*, 
    PalCommandPool**)
{

}

static void PAL_CALL destroyCommandPool(PalCommandPool*)
{

}

static PalResult PAL_CALL allocateCommandBuffer(
    PalDevice*, 
    PalCommandPool*, 
    PalCommandBufferType, 
    PalCommandBuffer**)
{

}

static void PAL_CALL freeCommandBuffer(PalCommandBuffer*)
{

}

static PalResult PAL_CALL resetCommandBuffer(PalCommandBuffer*)
{

}

static PalResult PAL_CALL submitCommandBuffer(
    PalQueue*, 
    PalCommandBufferSubmitInfo*)
{

}

static PalResult PAL_CALL cmdBegin(
    PalCommandBuffer*, 
    PalRenderingLayoutInfo*)
{

}

static PalResult PAL_CALL cmdEnd(PalCommandBuffer*)
{

}

static void PAL_CALL cmdExecuteCommandBuffer(
    PalCommandBuffer*, 
    PalCommandBuffer*)
{

}

static void PAL_CALL cmdBeginRendering(
    PalCommandBuffer*, 
    PalRenderingInfo*)
{

}

static void PAL_CALL cmdEndRendering(PalCommandBuffer*)
{

}

static void PAL_CALL cmdCopyBuffer(
    PalCommandBuffer*, 
    PalBuffer*, 
    PalBuffer*, 
    PalBufferCopyInfo*)
{

}

static void PAL_CALL cmdCopyBufferToImage(
    PalCommandBuffer*, 
    PalImage*, 
    PalBuffer*, 
    PalBufferImageCopyInfo*)
{

}

static void PAL_CALL cmdCopyImage(
    PalCommandBuffer*, 
    PalImage*, 
    PalImage*, 
    PalImageCopyInfo*)
{

}

static void PAL_CALL cmdCopyImageToBuffer(
    PalCommandBuffer*, 
    PalBuffer*, 
    PalImage*, 
    PalBufferImageCopyInfo*)
{

}

static void PAL_CALL cmdBindPipeline(
    PalCommandBuffer*, 
    PalPipeline*)
{

}

static void PAL_CALL cmdSetViewport(
    PalCommandBuffer*, 
    uint32_t, 
    PalViewport*)
{

}

static void PAL_CALL cmdSetScissors(
    PalCommandBuffer*, 
    uint32_t, 
    PalRect2D*)
{

}

static void PAL_CALL cmdBindVertexBuffers(
    PalCommandBuffer*, 
    uint32_t, 
    uint32_t, 
    PalBuffer**, 
    uint64_t*)
{

}

static void PAL_CALL cmdBindIndexBuffer(
    PalCommandBuffer*, 
    PalBuffer*, 
    uint64_t, 
    PalIndexType)
{

}

static void PAL_CALL cmdDraw(
    PalCommandBuffer*, 
    uint32_t, 
    uint32_t, 
    uint32_t, 
    uint32_t)
{

}

static void PAL_CALL cmdDrawIndexed(
    PalCommandBuffer*, 
    uint32_t, 
    uint32_t, 
    uint32_t, 
    int32_t, 
    uint32_t)
{

}

static void PAL_CALL cmdImageBarrier(
    PalCommandBuffer*, 
    PalImage*, 
    PalImageSubresourceRange*, 
    PalBarrierInfo*)
{

}

static void PAL_CALL cmdBufferBarrier(
    PalCommandBuffer*, 
    PalBuffer*, 
    PalBarrierInfo*)
{

}

static void PAL_CALL cmdDispatch(
    PalCommandBuffer*, 
    uint32_t, 
    uint32_t, 
    uint32_t)
{

}

static void PAL_CALL cmdBindDescriptorSet(
    PalCommandBuffer*, 
    uint32_t, 
    PalDescriptorSet*)
{

}

static void PAL_CALL cmdPushConstants(
    PalCommandBuffer*, 
    uint32_t, 
    uint32_t, 
    const void*)
{

}

static PalResult PAL_CALL createBuffer(
    PalDevice*, 
    const PalBufferCreateInfo*, 
    PalBuffer**)
{

}

static void PAL_CALL destroyBuffer(PalBuffer*)
{

}

static void PAL_CALL getBufferMemoryRequirements(
    PalBuffer*, 
    PalMemoryRequirements*)
{

}

static void PAL_CALL computeImageStagingRequirements(
    PalDevice*, 
    PalFormat, 
    const PalBufferImageCopyInfo*, 
    PalImageStagingRequirements*)
{

}

static void PAL_CALL writeImageStaging(
    PalDevice*, 
    PalFormat, 
    PalBufferImageCopyInfo*, 
    void*, 
    void*)
{

}

static PalResult PAL_CALL bindBufferMemory(
    PalBuffer*, 
    PalMemory*, 
    uint64_t)
{

}

static PalResult PAL_CALL mapBuffer(
    PalBuffer*, 
    uint64_t, 
    uint64_t, 
    void**)
{

}

static void PAL_CALL unmapBuffer(PalBuffer*)
{

}

static PalResult PAL_CALL createDescriptorSetLayout(
    PalDevice*, 
    const PalDescriptorSetLayoutCreateInfo*,
    PalDescriptorSetLayout**)
{

}

static void PAL_CALL destroyDescriptorSetLayout(PalDescriptorSetLayout*)
{

}

static PalResult PAL_CALL createDescriptorPool(
    PalDevice*, 
    const PalDescriptorPoolCreateInfo*, 
    PalDescriptorPool**)
{

}

static void PAL_CALL destroyDescriptorPool(PalDescriptorPool*)
{

}

static PalResult PAL_CALL resetDescriptorPool(PalDescriptorPool*)
{

}

static PalResult PAL_CALL allocateDescriptorSet(
    PalDevice*, 
    PalDescriptorPool*, 
    PalDescriptorSetLayout*, 
    PalDescriptorSet**)
{

}

static PalResult PAL_CALL updateDescriptorSet(
    PalDevice*, 
    uint32_t, 
    PalDescriptorSetWriteInfo*)
{

}

static PalResult PAL_CALL createPipelineLayout(
    PalDevice*, 
    const PalPipelineLayoutCreateInfo*, 
    PalPipelineLayout**)
{

}

static void PAL_CALL destroyPipelineLayout(PalPipelineLayout*)
{

}

static PalResult PAL_CALL createGraphicsPipeline(
    PalDevice*, 
    const PalGraphicsPipelineCreateInfo*, 
    PalPipeline**)
{

}

static PalResult PAL_CALL createComputePipeline(
    PalDevice*, 
    const PalComputePipelineCreateInfo*, 
    PalPipeline**)
{

}

static void PAL_CALL destroyPipeline(PalPipeline*)
{

}

static void PAL_CALL onGraphicsDebug(
    void* userData,
    PalDebugMessageSeverity severity,
    PalDebugMessageType type,
    const char* msg)
{
    palLog(nullptr, msg);
}

PalBool customBackendTest()
{
    // build the vtable
    PalGraphicsBackendVtable1 vtable = {0};
    vtable.enumerateAdapters = enumerateAdapters,
    vtable.getAdapterInfo = getAdapterInfo,
    vtable.getAdapterCapabilities = getAdapterCapabilities,
    vtable.getAdapterFeatures = getAdapterFeatures,
    vtable.getHighestSupportedShaderTarget = getHighestSupportedShaderTarget,
    vtable.createDevice = createDevice,
    vtable.destroyDevice = destroyDevice,
    vtable.allocateMemory = allocateMemory,
    vtable.freeMemory = freeMemory,
    vtable.querySamplerAnisotropyCapabilities = querySamplerAnisotropyCapabilities,
    vtable.createQueue = createQueue,
    vtable.destroyQueue = destroyQueue,
    vtable.waitQueue = waitQueue,
    vtable.canQueuePresent = canQueuePresent,
    vtable.enumerateFormats = enumerateFormats,
    vtable.isFormatSupported = isFormatSupported,
    vtable.queryFormatImageUsages = queryFormatImageUsages,
    vtable.queryFormatSampleCount = queryFormatSampleCount,
    vtable.createImage = createImage,
    vtable.destroyImage = destroyImage,
    vtable.getImageInfo = getImageInfo,
    vtable.getImageMemoryRequirements = getImageMemoryRequirements,
    vtable.bindImageMemory = bindImageMemory,
    vtable.createImageView = createImageView,
    vtable.destroyImageView = destroyImageView,
    vtable.createSampler = createSampler,
    vtable.destroySampler = destroySampler,
    vtable.createShader = createShader,
    vtable.destroyShader = destroyShader,
    vtable.createFence = createFence,
    vtable.destroyFence = destroyFence,
    vtable.waitFence = waitFence,
    vtable.resetFence = resetFence,
    vtable.isFenceSignaled = isFenceSignaled,
    vtable.createSemaphore = createSemaphore,
    vtable.destroySemaphore = destroySemaphore,
    vtable.createCommandPool = createCommandPool,
    vtable.destroyCommandPool = destroyCommandPool,
    vtable.allocateCommandBuffer = allocateCommandBuffer,
    vtable.freeCommandBuffer = freeCommandBuffer,
    vtable.resetCommandBuffer = resetCommandBuffer,
    vtable.submitCommandBuffer = submitCommandBuffer,
    vtable.cmdBegin = cmdBegin,
    vtable.cmdEnd = cmdEnd,
    vtable.cmdExecuteCommandBuffer = cmdExecuteCommandBuffer,
    vtable.cmdBeginRendering = cmdBeginRendering,
    vtable.cmdEndRendering = cmdEndRendering,
    vtable.cmdCopyBuffer = cmdCopyBuffer,
    vtable.cmdCopyBufferToImage = cmdCopyBufferToImage,
    vtable.cmdCopyImage = cmdCopyImage,
    vtable.cmdCopyImageToBuffer = cmdCopyImageToBuffer,
    vtable.cmdBindPipeline = cmdBindPipeline,
    vtable.cmdSetViewport = cmdSetViewport,
    vtable.cmdSetScissors = cmdSetScissors,
    vtable.cmdBindVertexBuffers = cmdBindVertexBuffers,
    vtable.cmdBindIndexBuffer = cmdBindIndexBuffer,
    vtable.cmdDraw = cmdDraw,
    vtable.cmdDrawIndexed = cmdDrawIndexed,
    vtable.cmdImageBarrier = cmdImageBarrier,
    vtable.cmdBufferBarrier = cmdBufferBarrier,
    vtable.cmdDispatch = cmdDispatch,
    vtable.cmdBindDescriptorSet = cmdBindDescriptorSet,
    vtable.cmdPushConstants = cmdPushConstants,
    vtable.createBuffer = createBuffer,
    vtable.destroyBuffer = destroyBuffer,
    vtable.getBufferMemoryRequirements = getBufferMemoryRequirements,
    vtable.computeImageStagingRequirements = computeImageStagingRequirements,
    vtable.writeImageStaging = writeImageStaging,
    vtable.bindBufferMemory = bindBufferMemory,
    vtable.mapBuffer = mapBuffer,
    vtable.unmapBuffer = unmapBuffer,
    vtable.createDescriptorSetLayout = createDescriptorSetLayout,
    vtable.destroyDescriptorSetLayout = destroyDescriptorSetLayout,
    vtable.createDescriptorPool = createDescriptorPool,
    vtable.destroyDescriptorPool = destroyDescriptorPool,
    vtable.resetDescriptorPool = resetDescriptorPool,
    vtable.allocateDescriptorSet = allocateDescriptorSet,
    vtable.updateDescriptorSet = updateDescriptorSet,
    vtable.createPipelineLayout = createPipelineLayout,
    vtable.destroyPipelineLayout = destroyPipelineLayout,
    vtable.createGraphicsPipeline = createGraphicsPipeline,
    vtable.createComputePipeline = createComputePipeline,
    vtable.destroyPipeline = destroyPipeline;

    PalGraphicsBackendInfo backendInfo = {0};
    backendInfo.version = PAL_GRAPHICS_BACKEND_VTABLE_VERSION_1;
    backendInfo.vtable = &vtable;

    PalResult result = palInitGraphics(nullptr, nullptr, 1, &backendInfo);
    if (result != PAL_RESULT_SUCCESS) {
        logResult(result, "Failed to initialize graphics");
        return PAL_FALSE;
    }

    // TODO: enumerate and select our adapter and do some work
    palShutdownGraphics();

    return PAL_TRUE;
}