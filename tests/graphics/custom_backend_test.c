
#include "pal2/pal_graphics.h"
#include "tests.h"

// we use the same fields for each of our handles so we just typedef a base handle struct
typedef struct {
    void* reserved;
} Adapter;

// we will only expose a single adapter
static Adapter s_Adapter;
static PalAdapterInfo s_Info;
static PalAdapterCapabilities s_Cap;

static void initBackend()
{
    s_Adapter.reserved = nullptr;
    s_Info.apiType = PAL_ADAPTER_API_TYPE_VULKAN;
    strcpy(s_Info.backendName, "Custom");
    s_Info.deviceId = 1666;
    s_Info.driverVersion = 1;
    strcpy(s_Info.name, "GXR 7060");
    s_Info.shaderFormats = PAL_SHADER_FORMAT_SPIRV | PAL_SHADER_FORMAT_GLSL;
    s_Info.sharedMemory = (uint64_t)(1024 * 1024 * 1024) * (uint64_t)2;
    s_Info.type = PAL_ADAPTER_TYPE_DISCRETE;
    s_Info.vendorId = 20037;
    s_Info.vram = (uint64_t)(1024 * 1024 * 1024) * (uint64_t)8;

    s_Cap.computeCaps.maxWorkGroupCount[0] = 65535;
    s_Cap.computeCaps.maxWorkGroupCount[1] = 65535;
    s_Cap.computeCaps.maxWorkGroupCount[2] = 65535;
    s_Cap.computeCaps.maxWorkGroupSize[0] = 1024;
    s_Cap.computeCaps.maxWorkGroupSize[1] = 1024;
    s_Cap.computeCaps.maxWorkGroupSize[2] = 64;
    s_Cap.computeCaps.maxWorkGroupInvocations = 1024;

    s_Cap.imageCaps.maxArrayLayers = 5;
    s_Cap.imageCaps.maxDepth = 128;
    s_Cap.imageCaps.maxHeight = 4800;
    s_Cap.imageCaps.maxMipLevels = 12;
    s_Cap.imageCaps.maxWidth = 4800;

    s_Cap.maxColorAttachments = 8;
    s_Cap.maxComputeQueues = 2;
    s_Cap.maxCopyQueues = 2;
    s_Cap.maxGraphicsQueues = 2;
    s_Cap.maxPushConstantSize = 128;

    s_Cap.maxStorageBufferSize = 65536;
    s_Cap.maxTessellationPatchPoint = 8;
    s_Cap.maxUniformBufferSize = 65536;
    s_Cap.maxVertexAttributes = 14;
    s_Cap.maxVertexLayouts = 4;

    s_Cap.resourceCaps.maxBoundSets = 2;
    s_Cap.resourceCaps.maxPerSetAccelerationStructure = 2;
    s_Cap.resourceCaps.maxPerSetSampledImages = 512;
    s_Cap.resourceCaps.maxPerSetSamplers = 256;
    s_Cap.resourceCaps.maxPerSetStorageBuffers = 720;
    s_Cap.resourceCaps.maxPerSetStorageImages = 720;
    s_Cap.resourceCaps.maxPerSetUniformBuffers = 100;
    s_Cap.resourceCaps.maxPerStageAccelerationStructure = 10;
    s_Cap.resourceCaps.maxPerStageSampledImages = 120;
    s_Cap.resourceCaps.maxPerStageSamplers = 16;
    s_Cap.resourceCaps.maxPerStageStorageBuffers = 50;
    s_Cap.resourceCaps.maxPerStageStorageImages = 50;
    s_Cap.resourceCaps.maxPerStageUniformBuffers = 50;

    s_Cap.viewportCaps.maxBoundsRange = 4800.0f;
    s_Cap.viewportCaps.minBoundsRange = 4800.0f;
    s_Cap.viewportCaps.maxWidth = 4800;
    s_Cap.viewportCaps.maxHeight = 4800;
}

static PalResult PAL_CALL enumerateAdapters(
    uint32_t* count,
    PalAdapter** outAdapters)
{
    if (outAdapters) {
        outAdapters[0] = (PalAdapter*)&s_Adapter;

    } else {
        *count = 1;
    }
    return PAL_RESULT_SUCCESS;
}

static void PAL_CALL getAdapterInfo(
    PalAdapter* adapter,
    PalAdapterInfo* info)
{
    *info = s_Info;
}

static void PAL_CALL getAdapterCapabilities(
    PalAdapter* adapter,
    PalAdapterCapabilities* caps)
{
    *caps = s_Cap;
}

static PalAdapterFeatures PAL_CALL getAdapterFeatures(PalAdapter* adapter)
{
    return PAL_ADAPTER_FEATURE_FENCE_RESET | PAL_ADAPTER_FEATURE_SAMPLER_ANISOTROPY;
}

static uint32_t PAL_CALL getHighestSupportedShaderTarget(
    PalAdapter* adapter,
    PalShaderFormats shaderFormat)
{
    return PAL_MAKE_SHADER_TARGET(1, 0);
}

static PalResult PAL_CALL createDevice(
    PalAdapter* adapter,
    PalAdapterFeatures features,
    PalDevice** outDevice)
{
    return PAL_RESULT_SUCCESS;
}

static void PAL_CALL destroyDevice(PalDevice* device)
{
}

static uint32_t PAL_CALL getDeviceLostReason(PalDevice* device)
{
    return 1;
}

static PalResult PAL_CALL allocateMemory(
    PalDevice* device,
    PalMemoryType type,
    uint64_t memoryMask,
    uint64_t size,
    PalMemory** outMemory)
{
    return PAL_RESULT_SUCCESS;
}

static void PAL_CALL freeMemory(PalMemory* memory)
{
}

static void PAL_CALL querySamplerAnisotropyCapabilities(
    PalDevice* device,
    PalSamplerAnisotropyCapabilities* caps)
{
}

static PalResult PAL_CALL createQueue(
    PalDevice* device,
    PalQueueType type,
    PalQueue** outQueue)
{
    return PAL_RESULT_SUCCESS;
}

static void PAL_CALL destroyQueue(PalQueue* queue)
{
}

static PalBool PAL_CALL canQueuePresent(
    PalQueue* queue,
    PalSurface* surface)
{
    return PAL_FALSE;
}

static PalResult PAL_CALL waitQueue(PalQueue* queue)
{
    return PAL_RESULT_SUCCESS;
}

static void PAL_CALL enumerateFormats(
    PalAdapter* adapter,
    uint32_t* count,
    PalFormatInfo* outFormats)
{
}

static PalBool PAL_CALL isFormatSupported(
    PalAdapter* adapter,
    PalFormat format)
{
    return PAL_FALSE;
}

static PalImageUsages PAL_CALL queryFormatImageUsages(
    PalAdapter* adapter,
    PalFormat format)
{
    return 0;
}

static PalSampleCount PAL_CALL queryFormatSampleCount(
    PalAdapter* adapter,
    PalFormat format)
{
    return 0;
}

static PalResult PAL_CALL createImage(
    PalDevice* device,
    const PalImageCreateInfo* info,
    PalImage** outImage)
{
    return PAL_RESULT_SUCCESS;
}

static void PAL_CALL destroyImage(PalImage* image)
{
}

static void PAL_CALL getImageInfo(
    PalImage* image,
    PalImageInfo* info)
{
}

static void PAL_CALL getImageMemoryRequirements(
    PalImage* image,
    PalMemoryRequirements* requirements)
{
}

static PalResult PAL_CALL bindImageMemory(
    PalImage* image,
    PalMemory* memory,
    uint64_t offset)
{
    return PAL_RESULT_SUCCESS;
}

static PalResult PAL_CALL createImageView(
    PalDevice* device,
    PalImage* image,
    const PalImageViewCreateInfo* info,
    PalImageView** outImageView)
{
    return PAL_RESULT_SUCCESS;
}

static void PAL_CALL destroyImageView(PalImageView* imageView)
{
}

static PalResult PAL_CALL createSampler(
    PalDevice* device,
    const PalSamplerCreateInfo* info,
    PalSampler** outSampler)
{
    return PAL_RESULT_SUCCESS;
}

static void PAL_CALL destroySampler(PalSampler* sampler)
{
}

static PalResult PAL_CALL createShader(
    PalDevice* device,
    const PalShaderCreateInfo* info,
    PalShader** outShader)
{
    return PAL_RESULT_SUCCESS;
}

static void PAL_CALL destroyShader(PalShader* shader)
{
}

static PalResult PAL_CALL createFence(
    PalDevice* device,
    PalBool signaled,
    PalFence** outFence)
{
    return PAL_RESULT_SUCCESS;
}

static void PAL_CALL destroyFence(PalFence* fence)
{
}

static PalResult PAL_CALL waitFence(
    PalFence* fence,
    uint64_t timeout)
{
    return PAL_RESULT_SUCCESS;
}

static PalResult PAL_CALL resetFence(PalFence* fence)
{
    return PAL_RESULT_SUCCESS;
}

static PalBool PAL_CALL isFenceSignaled(PalFence* fence)
{
    return PAL_FALSE;
}

static PalResult PAL_CALL createSemaphore(
    PalDevice* device,
    PalBool enableTimeline,
    PalSemaphore** outSemaphore)
{
    return PAL_RESULT_SUCCESS;
}

static void PAL_CALL destroySemaphore(PalSemaphore* semaphore)
{
}

static PalResult PAL_CALL createCommandPool(
    PalDevice* device,
    PalQueue* queue,
    PalCommandPool** outPool)
{
    return PAL_RESULT_SUCCESS;
}

static void PAL_CALL destroyCommandPool(PalCommandPool* pool)
{
}

static PalResult PAL_CALL allocateCommandBuffer(
    PalDevice* device,
    PalCommandPool* pool,
    PalCommandBufferType type,
    PalCommandBuffer** outCmdBuffer)
{
    return PAL_RESULT_SUCCESS;
}

static void PAL_CALL freeCommandBuffer(PalCommandBuffer* cmdBuffer)
{
}

static PalResult PAL_CALL resetCommandBuffer(PalCommandBuffer* cmdBuffer)
{
    return PAL_RESULT_SUCCESS;
}

static PalResult PAL_CALL submitCommandBuffer(
    PalQueue* queue,
    PalCommandBufferSubmitInfo* info)
{
    return PAL_RESULT_SUCCESS;
}

static PalResult PAL_CALL cmdBegin(
    PalCommandBuffer* cmdBuffer,
    PalRenderingLayoutInfo* info)
{
    return PAL_RESULT_SUCCESS;
}

static PalResult PAL_CALL cmdEnd(PalCommandBuffer* cmdBuffer)
{
    return PAL_RESULT_SUCCESS;
}

static void PAL_CALL cmdExecuteCommandBuffer(
    PalCommandBuffer* primaryCmdBuffer,
    PalCommandBuffer* secondaryCmdBuffer)
{
}

static void PAL_CALL cmdSetFragmentShadingRate(
    PalCommandBuffer* cmdBuffer,
    PalFragmentShadingRateState* state)
{
}

static void PAL_CALL cmdDrawMeshTasks(
    PalCommandBuffer* cmdBuffer,
    uint32_t groupCountX,
    uint32_t groupCountY,
    uint32_t groupCountZ)
{
}

static void PAL_CALL cmdDrawMeshTasksIndirect(
    PalCommandBuffer* cmdBuffer,
    PalBuffer* buffer,
    uint32_t drawCount)
{
}

static void PAL_CALL cmdDrawMeshTasksIndirectCount(
    PalCommandBuffer* cmdBuffer,
    PalBuffer* buffer,
    PalBuffer* countBuffer,
    uint32_t maxDrawCount)
{
}

static void PAL_CALL cmdBuildAccelerationStructure(
    PalCommandBuffer* cmdBuffer,
    PalAccelerationStructureBuildInfo* info)
{
}

static void PAL_CALL cmdBeginRendering(
    PalCommandBuffer* cmdBuffer,
    PalRenderingInfo* info)
{
}

static void PAL_CALL cmdEndRendering(PalCommandBuffer* cmdBuffer)
{
}

static void PAL_CALL cmdCopyBuffer(
    PalCommandBuffer* cmdBuffer,
    PalBuffer* dst,
    PalBuffer* src,
    PalBufferCopyInfo* copyInfo)
{
}

static void PAL_CALL cmdCopyBufferToImage(
    PalCommandBuffer* cmdBuffer,
    PalImage* dstImage,
    PalBuffer* srcBuffer,
    PalBufferImageCopyInfo* copyInfo)
{
}

static void PAL_CALL cmdCopyImage(
    PalCommandBuffer* cmdBuffer,
    PalImage* dst,
    PalImage* src,
    PalImageCopyInfo* copyInfo)
{
}

static void PAL_CALL cmdCopyImageToBuffer(
    PalCommandBuffer* cmdBuffer,
    PalBuffer* dstBuffer,
    PalImage* srcImage,
    PalBufferImageCopyInfo* copyInfo)
{
}

static void PAL_CALL cmdBindPipeline(
    PalCommandBuffer* cmdBuffer,
    PalPipeline* pipeline)
{
}

static void PAL_CALL cmdSetViewport(
    PalCommandBuffer* cmdBuffer,
    uint32_t count,
    PalViewport* viewports)
{
}

static void PAL_CALL cmdSetScissors(
    PalCommandBuffer* cmdBuffer,
    uint32_t count,
    PalRect2D* scissors)
{
}

static void PAL_CALL cmdBindVertexBuffers(
    PalCommandBuffer* cmdBuffer,
    uint32_t firstSlot,
    uint32_t count,
    PalBuffer** buffers,
    uint64_t* offsets)
{
}

static void PAL_CALL cmdBindIndexBuffer(
    PalCommandBuffer* cmdBuffer,
    PalBuffer* buffer,
    uint64_t offset,
    PalIndexType type)
{
}

static void PAL_CALL cmdDraw(
    PalCommandBuffer* cmdBuffer,
    uint32_t vertexCount,
    uint32_t instanceCount,
    uint32_t firstVertex,
    uint32_t firstInstance)
{
}

static void PAL_CALL cmdDrawIndexed(
    PalCommandBuffer* cmdBuffer,
    uint32_t indexCount,
    uint32_t instanceCount,
    uint32_t firstIndex,
    int32_t vertexOffset,
    uint32_t firstInstance)
{
}

static void PAL_CALL cmdImageBarrier(
    PalCommandBuffer* cmdBuffer,
    PalImage* image,
    PalImageSubresourceRange* subresourceRange,
    PalBarrierInfo* info)
{
}

static void PAL_CALL cmdBufferBarrier(
    PalCommandBuffer* cmdBuffer,
    PalBuffer* buffer,
    PalBarrierInfo* info)
{
}

static void PAL_CALL cmdDispatch(
    PalCommandBuffer* cmdBuffer,
    uint32_t groupCountX,
    uint32_t groupCountY,
    uint32_t groupCountZ)
{
}

static void PAL_CALL cmdBindDescriptorSet(
    PalCommandBuffer* cmdBuffer,
    uint32_t setIndex,
    PalDescriptorSet* set)
{
}

static void PAL_CALL cmdPushConstants(
    PalCommandBuffer* cmdBuffer,
    uint32_t offset,
    uint32_t size,
    const void* value)
{
}

static PalResult PAL_CALL createBuffer(
    PalDevice* device,
    const PalBufferCreateInfo* info,
    PalBuffer** outBuffer)
{
    return PAL_RESULT_SUCCESS;
}

static void PAL_CALL destroyBuffer(PalBuffer* buffer)
{
}

static void PAL_CALL getBufferMemoryRequirements(
    PalBuffer* buffer,
    PalMemoryRequirements* requirements)
{
}

static void PAL_CALL computeImageStagingRequirements(
    PalDevice* device,
    PalFormat imageFormat,
    const PalBufferImageCopyInfo* copyInfo,
    PalImageStagingRequirements* requirements)
{
}

static void PAL_CALL writeImageStaging(
    PalDevice* device,
    PalFormat imageFormat,
    PalBufferImageCopyInfo* copyInfo,
    void* srcData,
    void* ptr)
{
}

static PalResult PAL_CALL bindBufferMemory(
    PalBuffer* buffer,
    PalMemory* memory,
    uint64_t offset)
{
    return PAL_RESULT_SUCCESS;
}

static PalResult PAL_CALL mapBuffer(
    PalBuffer* buffer,
    uint64_t offset,
    uint64_t size,
    void** outPtr)
{
    return PAL_RESULT_SUCCESS;
}

static void PAL_CALL unmapBuffer(PalBuffer* buffer)
{
}

static PalResult PAL_CALL createDescriptorSetLayout(
    PalDevice* device,
    const PalDescriptorSetLayoutCreateInfo* info,
    PalDescriptorSetLayout** outLayout)
{
    return PAL_RESULT_SUCCESS;
}

static void PAL_CALL destroyDescriptorSetLayout(PalDescriptorSetLayout* layout)
{
}

static PalResult PAL_CALL createDescriptorPool(
    PalDevice* device,
    const PalDescriptorPoolCreateInfo* info,
    PalDescriptorPool** outPool)
{
    return PAL_RESULT_SUCCESS;
}

static void PAL_CALL destroyDescriptorPool(PalDescriptorPool* pool)
{
}

static PalResult PAL_CALL resetDescriptorPool(PalDescriptorPool* pool)
{
    return PAL_RESULT_SUCCESS;
}

static PalResult PAL_CALL allocateDescriptorSet(
    PalDevice* device,
    PalDescriptorPool* pool,
    PalDescriptorSetLayout* layout,
    PalDescriptorSet** outSet)
{
    return PAL_RESULT_SUCCESS;
}

static PalResult PAL_CALL updateDescriptorSet(
    PalDevice* device,
    uint32_t count,
    PalDescriptorSetWriteInfo* infos)
{
    return PAL_RESULT_SUCCESS;
}

static PalResult PAL_CALL createPipelineLayout(
    PalDevice* device,
    const PalPipelineLayoutCreateInfo* info,
    PalPipelineLayout** outLayout)
{
    return PAL_RESULT_SUCCESS;
}

static void PAL_CALL destroyPipelineLayout(PalPipelineLayout* layout)
{
}

static PalResult PAL_CALL createGraphicsPipeline(
    PalDevice* device,
    const PalGraphicsPipelineCreateInfo* info,
    PalPipeline** outPipeline)
{
    return PAL_RESULT_SUCCESS;
}

static PalResult PAL_CALL createComputePipeline(
    PalDevice* device,
    const PalComputePipelineCreateInfo* info,
    PalPipeline** outPipeline)
{
    return PAL_RESULT_SUCCESS;
}

static void PAL_CALL destroyPipeline(PalPipeline* pipeline)
{
}

PalBool customBackendTest()
{
    // build the vtable
    PalGraphicsBackendVtable1 vtable = {0};
    vtable.enumerateAdapters = enumerateAdapters;
    vtable.getAdapterInfo = getAdapterInfo;
    vtable.getAdapterCapabilities = getAdapterCapabilities;
    vtable.getAdapterFeatures = getAdapterFeatures;
    vtable.getHighestSupportedShaderTarget = getHighestSupportedShaderTarget;
    vtable.createDevice = createDevice;
    vtable.destroyDevice = destroyDevice;
    vtable.getDeviceLostReason = getDeviceLostReason;
    vtable.allocateMemory = allocateMemory;
    vtable.freeMemory = freeMemory;
    vtable.querySamplerAnisotropyCapabilities = querySamplerAnisotropyCapabilities;
    vtable.createQueue = createQueue;
    vtable.destroyQueue = destroyQueue;
    vtable.waitQueue = waitQueue;
    vtable.canQueuePresent = canQueuePresent;
    vtable.enumerateFormats = enumerateFormats;
    vtable.isFormatSupported = isFormatSupported;
    vtable.queryFormatImageUsages = queryFormatImageUsages;
    vtable.queryFormatSampleCount = queryFormatSampleCount;
    vtable.createImage = createImage;
    vtable.destroyImage = destroyImage;
    vtable.getImageInfo = getImageInfo;
    vtable.getImageMemoryRequirements = getImageMemoryRequirements;
    vtable.bindImageMemory = bindImageMemory;
    vtable.createImageView = createImageView;
    vtable.destroyImageView = destroyImageView;
    vtable.createSampler = createSampler;
    vtable.destroySampler = destroySampler;
    vtable.createShader = createShader;
    vtable.destroyShader = destroyShader;
    vtable.createFence = createFence;
    vtable.destroyFence = destroyFence;
    vtable.waitFence = waitFence;
    vtable.resetFence = resetFence;
    vtable.isFenceSignaled = isFenceSignaled;
    vtable.createSemaphore = createSemaphore;
    vtable.destroySemaphore = destroySemaphore;
    vtable.createCommandPool = createCommandPool;
    vtable.destroyCommandPool = destroyCommandPool;
    vtable.allocateCommandBuffer = allocateCommandBuffer;
    vtable.freeCommandBuffer = freeCommandBuffer;
    vtable.resetCommandBuffer = resetCommandBuffer;
    vtable.submitCommandBuffer = submitCommandBuffer;
    vtable.cmdBegin = cmdBegin;
    vtable.cmdEnd = cmdEnd;
    vtable.cmdExecuteCommandBuffer = cmdExecuteCommandBuffer;
    vtable.cmdBeginRendering = cmdBeginRendering;
    vtable.cmdEndRendering = cmdEndRendering;
    vtable.cmdCopyBuffer = cmdCopyBuffer;
    vtable.cmdCopyBufferToImage = cmdCopyBufferToImage;
    vtable.cmdCopyImage = cmdCopyImage;
    vtable.cmdCopyImageToBuffer = cmdCopyImageToBuffer;
    vtable.cmdBindPipeline = cmdBindPipeline;
    vtable.cmdSetViewport = cmdSetViewport;
    vtable.cmdSetScissors = cmdSetScissors;
    vtable.cmdBindVertexBuffers = cmdBindVertexBuffers;
    vtable.cmdBindIndexBuffer = cmdBindIndexBuffer;
    vtable.cmdDraw = cmdDraw;
    vtable.cmdDrawIndexed = cmdDrawIndexed;
    vtable.cmdImageBarrier = cmdImageBarrier;
    vtable.cmdBufferBarrier = cmdBufferBarrier;
    vtable.cmdDispatch = cmdDispatch;
    vtable.cmdBindDescriptorSet = cmdBindDescriptorSet;
    vtable.cmdPushConstants = cmdPushConstants;
    vtable.createBuffer = createBuffer;
    vtable.destroyBuffer = destroyBuffer;
    vtable.getBufferMemoryRequirements = getBufferMemoryRequirements;
    vtable.computeImageStagingRequirements = computeImageStagingRequirements;
    vtable.writeImageStaging = writeImageStaging;
    vtable.bindBufferMemory = bindBufferMemory;
    vtable.mapBuffer = mapBuffer;
    vtable.unmapBuffer = unmapBuffer;
    vtable.createDescriptorSetLayout = createDescriptorSetLayout;
    vtable.destroyDescriptorSetLayout = destroyDescriptorSetLayout;
    vtable.createDescriptorPool = createDescriptorPool;
    vtable.destroyDescriptorPool = destroyDescriptorPool;
    vtable.resetDescriptorPool = resetDescriptorPool;
    vtable.allocateDescriptorSet = allocateDescriptorSet;
    vtable.updateDescriptorSet = updateDescriptorSet;
    vtable.createPipelineLayout = createPipelineLayout;
    vtable.destroyPipelineLayout = destroyPipelineLayout;
    vtable.createGraphicsPipeline = createGraphicsPipeline;
    vtable.createComputePipeline = createComputePipeline;
    vtable.destroyPipeline = destroyPipeline;

    // We are still keeping the custom backend at version 1 so we dont have to implement
    // the required V2 functions
    PalGraphicsBackendInfo backendInfo = {0};
    backendInfo.version = PAL_GRAPHICS_BACKEND_VTABLE_VERSION_1;
    backendInfo.vtable = &vtable;

    // do any backend initialization before graphics init
    initBackend();

    PalResult result = palInitGraphics(nullptr, nullptr, 1, &backendInfo);
    if (result != PAL_RESULT_SUCCESS) {
        logResult(result, "Failed to initialize graphics");
        return PAL_FALSE;
    }

    // enumerate all available adapters
    uint32_t count = 0;
    result = palEnumerateAdapters(&count, nullptr);
    if (result != PAL_RESULT_SUCCESS) {
        logResult(result, "Failed to get query adapters");
        return PAL_FALSE;
    }

    if (count == 0) {
        palLog(nullptr, "No adapters found");
        return PAL_FALSE;
    }
    palLog(nullptr, "Adapter count: %u", count);

    // allocate an array of adapters or use a fixed array
    // Example: PalAdapter* adapters[12];
    PalAdapter** adapters = nullptr;
    adapters = palAllocate(nullptr, sizeof(PalAdapter*) * count, 0);
    if (!adapters) {
        palLog(nullptr, "Failed to allocate memory");
        return PAL_FALSE;
    }

    result = palEnumerateAdapters(&count, adapters);
    if (result != PAL_RESULT_SUCCESS) {
        logResult(result, "Failed to get query adapters");
        return PAL_FALSE;
    }

    for (int32_t i = 0; i < count; i++) {
        PalAdapter* adapter = adapters[i];

        PalAdapterInfo info;
        palGetAdapterInfo(adapter, &info);

        PalAdapterCapabilities caps;
        palGetAdapterCapabilities(adapter, &caps);
        PalAdapterFeatures features = palGetAdapterFeatures(adapter);

        uint64_t vramMib = (uint64_t)info.vram / (1024 * 1024);
        uint64_t sharedMemMib = (uint64_t)info.sharedMemory / (1024 * 1024);

        palLog(nullptr, "GPU Name: %s", info.name);
        palLog(nullptr, " Backend Name: %s", info.backendName);
        palLog(nullptr, " Driver Version: %llu", info.driverVersion);
        palLog(nullptr, " Vram %llu MIB", vramMib);
        palLog(nullptr, " Shared Memory %llu MIB", sharedMemMib);
        palLog(nullptr, " Device Id: %u", info.deviceId);
        palLog(nullptr, " Vendor Id: %u", info.vendorId);

        const char* typeString;
        switch (info.type) {
            case PAL_ADAPTER_TYPE_INTEGRATED: {
                typeString = "Integrated";
                break;
            }

            case PAL_ADAPTER_TYPE_VIRTUAL: {
                typeString = "Virtual";
                break;
            }

            case PAL_ADAPTER_TYPE_DISCRETE: {
                typeString = "Discrete";
                break;
            }

            case PAL_ADAPTER_TYPE_CPU: {
                typeString = "CPU";
                break;
            }
        }
        palLog(nullptr, " Type: %s", typeString);

        const char* apiTypeString;
        switch (info.apiType) {
            case PAL_ADAPTER_API_TYPE_D3D12: {
                apiTypeString = "D3D12";
                break;
            }

            case PAL_ADAPTER_API_TYPE_VULKAN: {
                apiTypeString = "Vulkan";
                break;
            }

            case PAL_ADAPTER_API_TYPE_METAL: {
                apiTypeString = "Metal";
                break;
            }
        }
        palLog(nullptr, " API Type: %s", apiTypeString);

        palLog(nullptr, "");
        palLog(nullptr, " Capabilities:");
        palLog(nullptr, "  Max compute queue: %u", caps.maxComputeQueues);
        palLog(nullptr, "  Max graphics queue: %u", caps.maxGraphicsQueues);
        palLog(nullptr, "  Max copy queue: %u", caps.maxCopyQueues);
        palLog(nullptr, "  Max color attachments: %u", caps.maxColorAttachments);
        palLog(nullptr, "  Max uniform buffer size: %u Bytes", caps.maxUniformBufferSize);

        palLog(nullptr, "  Max storage buffer size: %u Bytes", caps.maxStorageBufferSize);
        palLog(nullptr, "  Max push constant size: %u Bytes", caps.maxPushConstantSize);
        palLog(nullptr, "  Max vertex layouts: %u", caps.maxVertexLayouts);
        palLog(nullptr, "  Max vertex attributes: %u", caps.maxVertexAttributes);
        palLog(nullptr, "  Max tessellation patch point: %u", caps.maxTessellationPatchPoint);

        palLog(nullptr, "");
        palLog(nullptr, "  Viewport Capabilities:");
        palLog(nullptr, "   Max width: %u", caps.viewportCaps.maxWidth);
        palLog(nullptr, "   Max height: %u", caps.viewportCaps.maxHeight);
        palLog(nullptr, "   Min bounds range: %.1f", caps.viewportCaps.minBoundsRange);
        palLog(nullptr, "   Max bounds range: %.1f", caps.viewportCaps.maxBoundsRange);

        palLog(nullptr, "");
        palLog(nullptr, "  Image Capabilities:");
        palLog(nullptr, "   Max width: %u", caps.imageCaps.maxWidth);
        palLog(nullptr, "   Max height: %u", caps.imageCaps.maxHeight);
        palLog(nullptr, "   Max depth: %u", caps.imageCaps.maxDepth);
        palLog(nullptr, "   Max array layers: %u", caps.imageCaps.maxArrayLayers);
        palLog(nullptr, "   Max mip levels: %u", caps.imageCaps.maxMipLevels);

        palLog(nullptr, "");
        palLog(nullptr, "  Resource Capabilities:");
        PalResourceCapabilities* resourceCaps = &caps.resourceCaps;

        // clang-format off
        palLog(nullptr, "   Max per stage sampled images: %u", resourceCaps->maxPerStageSampledImages);
        palLog(nullptr, "   Max per set sampled images: %u", resourceCaps->maxPerSetSampledImages);
        palLog(nullptr, "   Max per stage storage images: %u", resourceCaps->maxPerStageStorageImages);
        palLog(nullptr, "   Max per set storage images: %u", resourceCaps->maxPerSetStorageImages);
        
        palLog(nullptr, "   Max per stage samplers: %u", resourceCaps->maxPerStageSamplers);
        palLog(nullptr, "   Max per set samplers: %u", resourceCaps->maxPerSetSamplers);
        palLog(nullptr, "   Max per stage storage buffers: %u", resourceCaps->maxPerStageStorageBuffers);
        palLog(nullptr, "   Max per set storage buffers: %u", resourceCaps->maxPerSetStorageBuffers);

        palLog(nullptr, "   Max per stage uniform buffers: %u", resourceCaps->maxPerStageUniformBuffers);
        palLog(nullptr, "   Max per set uniform buffers: %u", resourceCaps->maxPerSetUniformBuffers);
        palLog(nullptr, "   Max per stage acceleration structures: %u", resourceCaps->maxPerStageAccelerationStructure);
        palLog(nullptr, "   Max per set acceleration structures: %u", resourceCaps->maxPerSetAccelerationStructure);
        palLog(nullptr, "   Max bound sets: %u", resourceCaps->maxBoundSets);
        // clang-format on

        palLog(nullptr, "");
        palLog(nullptr, "  Compute Capabilities:");
        palLog(nullptr, "   Max invocations: %u", caps.computeCaps.maxWorkGroupInvocations);
        palLog(nullptr, "   Max work group count[0]: %u", caps.computeCaps.maxWorkGroupCount[0]);
        palLog(nullptr, "   Max work group count[1]: %u", caps.computeCaps.maxWorkGroupCount[1]);
        palLog(nullptr, "   Max work group count[2]: %u", caps.computeCaps.maxWorkGroupCount[2]);
        palLog(nullptr, "   Max work group size[0]: %u", caps.computeCaps.maxWorkGroupSize[0]);
        palLog(nullptr, "   Max work group size[1]: %u", caps.computeCaps.maxWorkGroupSize[1]);
        palLog(nullptr, "   Max work group size[2]: %u", caps.computeCaps.maxWorkGroupSize[2]);

        // shader formats
        uint32_t target;
        uint32_t targetMajor = 0;
        uint32_t targetMinor = 0;

        palLog(nullptr, "");
        palLog(nullptr, " Supported Shader Formats:");
        if (info.shaderFormats & PAL_SHADER_FORMAT_SPIRV) {
            palLog(nullptr, "  SPIRV");
        }

        if (info.shaderFormats & PAL_SHADER_FORMAT_DXBC) {
            palLog(nullptr, "  DXBC");
        }

        if (info.shaderFormats & PAL_SHADER_FORMAT_DXIL) {
            palLog(nullptr, "  DXIL");
        }

        if (info.shaderFormats & PAL_SHADER_FORMAT_GLSL) {
            palLog(nullptr, "  GLSL");
        }

        // features
        palLog(nullptr, "");
        palLog(nullptr, " Supported Features:");
        if (features & PAL_ADAPTER_FEATURE_SAMPLER_ANISOTROPY) {
            palLog(nullptr, "  Sampler Anisotropy");
        }

        if (features & PAL_ADAPTER_FEATURE_MULTI_VIEWPORT) {
            palLog(nullptr, "  Multi viewport");
        }

        if (features & PAL_ADAPTER_FEATURE_RAY_TRACING) {
            palLog(nullptr, "  Ray tracing");
        }

        if (features & PAL_ADAPTER_FEATURE_MESH_SHADER) {
            palLog(nullptr, "  Mesh and task shader");
        }

        if (features & PAL_ADAPTER_FEATURE_FRAGMENT_SHADING_RATE) {
            palLog(nullptr, "  Fragment shading rate");
        }

        if (features & PAL_ADAPTER_FEATURE_DESCRIPTOR_INDEXING) {
            palLog(nullptr, "  Descriptor indexing");
        }

        if (features & PAL_ADAPTER_FEATURE_MULTI_VIEW) {
            palLog(nullptr, "  Multiview");
        }

        if (features & PAL_ADAPTER_FEATURE_DEPTH_STENCIL_RESOLVE) {
            palLog(nullptr, "  Depth stencil resolve");
        }

        if (features & PAL_ADAPTER_FEATURE_SWAPCHAIN) {
            palLog(nullptr, "  Swapchain");
        }

        if (features & PAL_ADAPTER_FEATURE_SAMPLE_RATE_SHADING) {
            palLog(nullptr, "  Sample rate shading");
        }

        if (features & PAL_ADAPTER_FEATURE_TIMELINE_SEMAPHORE) {
            palLog(nullptr, "  Timeline Semaphore");
        }

        if (features & PAL_ADAPTER_FEATURE_TESSELLATION_SHADER) {
            palLog(nullptr, "  Tesselation Shader");
        }

        if (features & PAL_ADAPTER_FEATURE_GEOMETRY_SHADER) {
            palLog(nullptr, "  Geometry shader");
        }

        if (features & PAL_ADAPTER_FEATURE_SHADER_FLOAT16) {
            palLog(nullptr, "  Shader float16");
        }

        if (features & PAL_ADAPTER_FEATURE_SHADER_FLOAT64) {
            palLog(nullptr, "  Shader float64");
        }

        if (features & PAL_ADAPTER_FEATURE_SHADER_INT16) {
            palLog(nullptr, "  Shader int16");
        }

        if (features & PAL_ADAPTER_FEATURE_SHADER_INT64) {
            palLog(nullptr, "  Shader int64");
        }

        if (features & PAL_ADAPTER_FEATURE_IMAGE_VIEW_CUBE_ARRAY) {
            palLog(nullptr, "  Image view type Cube array");
        }

        if (features & PAL_ADAPTER_FEATURE_FENCE_RESET) {
            palLog(nullptr, "  Resetting fence");
        }

        if (features & PAL_ADAPTER_FEATURE_POLYGON_MODE_LINE) {
            palLog(nullptr, "  Polygon mode line");
        }

        if (features & PAL_ADAPTER_FEATURE_DYNAMIC_CULL_MODE) {
            palLog(nullptr, "  Dynamic cull mode");
        }

        if (features & PAL_ADAPTER_FEATURE_DYNAMIC_FRONT_FACE) {
            palLog(nullptr, "  Dynamic front face");
        }

        if (features & PAL_ADAPTER_FEATURE_DYNAMIC_PRIMITIVE_TOPOLOGY) {
            palLog(nullptr, "  Dynamic primitive topology");
        }

        if (features & PAL_ADAPTER_FEATURE_DYNAMIC_DEPTH_TEST_ENABLE) {
            palLog(nullptr, "  Dynamic depth test enable");
        }

        if (features & PAL_ADAPTER_FEATURE_DYNAMIC_DEPTH_WRITE_ENABLE) {
            palLog(nullptr, "  Dynamic depth write enable");
        }

        if (features & PAL_ADAPTER_FEATURE_DYNAMIC_STENCIL_OP) {
            palLog(nullptr, "  Dynamic stencil op");
        }

        if (features & PAL_ADAPTER_FEATURE_FRAGMENT_SHADING_RATE_ATTACHMENT) {
            palLog(nullptr, "  Fragment shading rate attachment");
        }

        if (features & PAL_ADAPTER_FEATURE_BUFFER_DEVICE_ADDRESS) {
            palLog(nullptr, "  Buffer device address");
        }

        if (features & PAL_ADAPTER_FEATURE_INDIRECT_DRAW) {
            palLog(nullptr, "  Indirect draw");
        }

        if (features & PAL_ADAPTER_FEATURE_INDIRECT_DRAW_COUNT) {
            palLog(nullptr, "  Indirect draw count");
        }

        if (features & PAL_ADAPTER_FEATURE_INDIRECT_DRAW_MESH) {
            palLog(nullptr, "  Indirect mesh draw");
        }

        if (features & PAL_ADAPTER_FEATURE_INDIRECT_DRAW_MESH_COUNT) {
            palLog(nullptr, "  Indirect mesh draw count");
        }

        if (features & PAL_ADAPTER_FEATURE_DISPATCH_BASE) {
            palLog(nullptr, "  Dispatch base");
        }

        if (features & PAL_ADAPTER_FEATURE_NULL_DESCRIPTORS) {
            palLog(nullptr, "  Null descriptors");
        }

        if (features & PAL_ADAPTER_FEATURE_INDIRECT_RAY_TRACING) {
            palLog(nullptr, "  Indirect ray tracing");
        }

        if (features & PAL_ADAPTER_FEATURE_RAY_QUERY) {
            palLog(nullptr, "  Ray query");
        }

        palLog(nullptr, "");
    }

    // shutdown the graphics system
    palShutdownGraphics();

    // do any shutdown of the backend after graphics shutdown

    palFree(nullptr, adapters);

    return PAL_TRUE;
}