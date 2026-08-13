
#include "pal2/pal_graphics.h"
#include "tests.h"

#define TEXTURE_WIDTH 32
#define TEXTURE_HEIGHT 32

static void PAL_CALL onGraphicsDebug(
    void* userData,
    PalDebugMessageSeverity severity,
    PalDebugMessageType type,
    const char* msg)
{
    palLog(nullptr, msg);
}

PalBool queueOwnershipTest()
{
    PalAdapter* adapter = nullptr;
    PalDevice* device = nullptr;
    PalQueue* gfxQueue = nullptr;
    PalQueue* cpyQueue = nullptr;

    PalCommandPool* gfxCmdPool = nullptr;
    PalCommandPool* cpyCmdPool = nullptr;
    PalCommandBuffer* gfxCmdBuffer;
    PalCommandBuffer* cpyCmdBuffer;

    PalFence* fence = nullptr;
    PalSemaphore* semaphore = nullptr;
    PalImage* texture = nullptr;
    PalBuffer* stagingBuffer = nullptr;

    PalGraphicsDebugger debugger = {0};
    debugger.callback = onGraphicsDebug;
    debugger.userData = nullptr;
    debugger.enableGPUValidation = PAL_TRUE;

    PalResult result = palInitGraphics(nullptr, nullptr, 0, nullptr);
    if (result != PAL_RESULT_SUCCESS) {
        logResult(result, "Failed to initialize graphics");
        return PAL_FALSE;
    }

    // enumerate all available adapters
    uint32_t adapterCount = 0;
    result = palEnumerateAdapters(&adapterCount, nullptr);
    if (result != PAL_RESULT_SUCCESS) {
        logResult(result, "Failed to get adapters");
        return PAL_FALSE;
    }

    if (adapterCount == 0) {
        palLog(nullptr, "No adapters found");
        return PAL_FALSE;
    }
    palLog(nullptr, "Adapter count: %d", adapterCount);

    PalAdapter** adapters = nullptr;
    adapters = palAllocate(nullptr, sizeof(PalAdapter*) * adapterCount, 0);
    if (!adapters) {
        palLog(nullptr, "Failed to allocate memory");
        return PAL_FALSE;
    }

    result = palEnumerateAdapters(&adapterCount, adapters);
    if (result != PAL_RESULT_SUCCESS) {
        logResult(result, "Failed to get adapters");
        return PAL_FALSE;
    }

    PalAdapterCapabilities caps = {0};
    PalAdapterFeatures adapterFeatures = 0;
    PalAdapterInfo adapterInfo = {0};
    for (int32_t i = 0; i < adapterCount; i++) {
        adapter = adapters[i];
        palGetAdapterCapabilities(adapter, &caps);
        if (caps.maxComputeQueues == 0) {
            adapter = nullptr;
            continue;
        }

        palGetAdapterInfo(adapter, &adapterInfo);
        // Skip CPU adapters
        if (adapterInfo.type == PAL_ADAPTER_TYPE_CPU) {
            adapter = nullptr;
            continue;
        }

        // we want an adapter that supports vtable version 2+
        if (adapterInfo.vtableVersion < PAL_GRAPHICS_BACKEND_VTABLE_VERSION_2) {
            adapter = nullptr;
            continue;
        }

        // In general, we need an adapter which multiple graphics and copy queues
        if (caps.maxGraphicsQueues < 1 || caps.maxCopyQueues < 1) {
            adapter = nullptr;
            continue;
        }

        break;
    }

    palFree(nullptr, adapters);
    if (!adapter) {
        palLog(nullptr, "Failed to find a required adapter");
        return PAL_FALSE;
    }

    // create a device
    result = palCreateDevice(adapter, 0, &device);
    if (result != PAL_RESULT_SUCCESS) {
        logResult(result, "Failed to create device");
        return PAL_FALSE;
    }

    // create a graphics queue and create a command pool from it
    result = palCreateQueue(device, PAL_QUEUE_TYPE_GRAPHICS, &gfxQueue);
    if (result != PAL_RESULT_SUCCESS) {
        logResult(result, "Failed to create queue");
        return PAL_FALSE;
    }

    result = palCreateCommandPool(device, gfxQueue, &gfxCmdPool);
    if (result != PAL_RESULT_SUCCESS) {
        logResult(result, "Failed to create command pool");
        return PAL_FALSE;
    }

    // allocate a command buffer from the graphics command pool
    result = palAllocateCommandBuffer(
        device, 
        gfxCmdPool, 
        PAL_COMMAND_BUFFER_TYPE_PRIMARY, 
        &gfxCmdBuffer);
        
    if (result != PAL_RESULT_SUCCESS) {
        logResult(result, "Failed to allocate command buffer");
        return PAL_FALSE;
    }

    // We have a graphics queue created. We check if there is a copy queue cannot share resources
    // with the graphics queue.
    for (uint32_t i = 0; i < caps.maxCopyQueues; i++) {
        // create a copy queue
        result = palCreateQueue(device, PAL_QUEUE_TYPE_COPY, &cpyQueue);
        if (result != PAL_RESULT_SUCCESS) {
            logResult(result, "Failed to create queue");
            return PAL_FALSE;
        }

        PalBool canShareResource = palCanQueueShareOwnership(gfxQueue, cpyQueue);
        if (canShareResource) {
            palDestroyQueue(cpyQueue);
            cpyQueue = nullptr;
            continue;

        } else {
            break;
        }   
    }

    const char* msg = nullptr;
    const char* msg2 = nullptr;
    if (!cpyQueue) {
        // we didnt get a copy queue that cannot share resource ownership with thr graphics queue
        // TODO: fix queue limit recycling
        result = palCreateQueue(device, PAL_QUEUE_TYPE_COPY, &cpyQueue);
        if (result != PAL_RESULT_SUCCESS) {
            logResult(result, "Failed to create queue");
            return PAL_FALSE;
        }

        msg = "Warning: Copy queue shares resource ownership with the graphics queue.";
        msg2 = "  Concurrent execution is possible";

    } else {
        msg = "Warning: Copy queue does not share resource ownership with the graphics queue";
        msg2 = "  Ownership transfer is required";
    }

    palLog(nullptr, msg);
    palLog(nullptr, msg2);

    result = palCreateCommandPool(device, cpyQueue, &cpyCmdPool);
    if (result != PAL_RESULT_SUCCESS) {
        logResult(result, "Failed to create command pool");
        return PAL_FALSE;
    }

    // allocate a command buffer from the graphics command pool
    result = palAllocateCommandBuffer(
        device, 
        cpyCmdPool, 
        PAL_COMMAND_BUFFER_TYPE_PRIMARY, 
        &cpyCmdBuffer);
        
    if (result != PAL_RESULT_SUCCESS) {
        logResult(result, "Failed to allocate command buffer");
        return PAL_FALSE;
    }

    result = palCreateSemaphore(device, PAL_FALSE, &semaphore);
    if (result != PAL_RESULT_SUCCESS) {
        logResult(result, "Failed to create semaphore");
        return PAL_FALSE;
    }

    result = palCreateFence(device, PAL_FALSE, &fence);
    if (result != PAL_RESULT_SUCCESS) {
        logResult(result, "Failed to create fence");
        return PAL_FALSE;
    }

    PalImageCreateInfo imageCreateInfo = {0};
    imageCreateInfo.arrayLayerCount = 1;
    imageCreateInfo.depth = 1;
    imageCreateInfo.format = PAL_FORMAT_R8G8B8A8_UNORM;
    imageCreateInfo.mipLevelCount = 1;
    imageCreateInfo.sampleCount = PAL_SAMPLE_COUNT_1;
    imageCreateInfo.type = PAL_IMAGE_TYPE_2D;
    imageCreateInfo.usages = PAL_IMAGE_USAGE_TRANSFER_DST | PAL_IMAGE_USAGE_SAMPLED;
    imageCreateInfo.width = TEXTURE_WIDTH;
    imageCreateInfo.height = TEXTURE_HEIGHT;
    imageCreateInfo.memoryUsage = PAL_IMAGE_MEMORY_USAGE_AUTO_GPU_ONLY;

    result = palCreateImage(device, &imageCreateInfo, &texture);
    if (result != PAL_RESULT_SUCCESS) {
        logResult(result, "Failed to create image");
        return PAL_FALSE;
    }

    PalImageStagingRequirements stagingReq = {0};
    PalBufferImageCopyInfo copyInfo = {0};
    copyInfo.ImageArrayLayerCount = 1;
    copyInfo.imageWidth = TEXTURE_WIDTH;
    copyInfo.imageHeight = TEXTURE_HEIGHT;
    copyInfo.imageDepth = 1;

    palComputeImageStagingRequirements(
        device,
        imageCreateInfo.format,
        &copyInfo,
        &stagingReq);

    copyInfo.bufferRowLength = stagingReq.bufferRowLength;
    copyInfo.bufferImageHeight = stagingReq.bufferImageHeight;

    PalBufferCreateInfo bufferCreateInfo = {0};
    bufferCreateInfo.size = stagingReq.bufferSize;
    bufferCreateInfo.usages = PAL_BUFFER_USAGE_TRANSFER_SRC;
    bufferCreateInfo.memoryUsage = PAL_BUFFER_MEMORY_USAGE_AUTO_CPU_UPLOAD;

    result = palCreateBuffer(device, &bufferCreateInfo, &stagingBuffer);
    if (result != PAL_RESULT_SUCCESS) {
        logResult(result, "Failed to create buffer");
        return PAL_FALSE;
    }

    // copy dummy data
    void* ptr = nullptr;
    uint32_t textureData[TEXTURE_WIDTH * TEXTURE_HEIGHT];
    memset(textureData, 1, TEXTURE_WIDTH * TEXTURE_HEIGHT);

    result = palMapBuffer(stagingBuffer, 0, bufferCreateInfo.size, &ptr);
    if (result != PAL_RESULT_SUCCESS) {
        logResult(result, "Failed to map buffer");
        return PAL_FALSE;
    }

    palWriteImageStaging(device, imageCreateInfo.format, &copyInfo, texture, textureData);
    palUnmapBuffer(stagingBuffer);

    // We use the copy queue to copy the buffer to the texture thus taking ownership
    result = palCmdBegin(cpyCmdBuffer, nullptr);
    if (result != PAL_RESULT_SUCCESS) {
        logResult(result, "Failed to begin command buffer");
        return PAL_FALSE;
    }

    PalImageSubresourceRange range = {0};
    range.startMipLevel = 0;
    range.startArrayLayer = 0;
    range.mipLevelCount = 1;
    range.layerArrayCount = 1;

    PalBarrierInfo barrierInfo = {0};
    barrierInfo.oldState = PAL_USAGE_STATE_UNDEFINED;
    barrierInfo.srcStages = PAL_PIPELINE_STAGE_NONE;
    barrierInfo.newState = PAL_USAGE_STATE_TRANSFER_WRITE;
    barrierInfo.dstStages = PAL_PIPELINE_STAGE_TRANSFER;

    palCmdImageBarrier(cpyCmdBuffer, texture, &range, &barrierInfo);
    palCmdCopyBufferToImage(cpyCmdBuffer, texture, stagingBuffer, &copyInfo);

    // The copy queue is done with the texture, we transfer ownership to the graphics queue
    PalBarrierInfo2 barrierInfo2 = {0};
    barrierInfo2.oldState = PAL_USAGE_STATE_TRANSFER_WRITE;
    barrierInfo2.srcStages = PAL_PIPELINE_STAGE_TRANSFER;
    barrierInfo2.newState = PAL_USAGE_STATE_SHADER_READ;
    barrierInfo2.dstStages = PAL_PIPELINE_STAGE_FRAGMENT_SHADER;

    // The graphics command buffer has to be in recording state.
    result = palCmdBegin(gfxCmdBuffer, nullptr);
    if (result != PAL_RESULT_SUCCESS) {
        logResult(result, "Failed to begin command buffer");
        return PAL_FALSE;
    }

    // If the two queues can share resource ownership, the function below just
    // sets the barrier but does no ownership transfer
    barrierInfo2.dstCmdBuffer = gfxCmdBuffer; // The graphics queue command buffer
    palCmdImageBarrier2(cpyCmdBuffer, texture, &range, &barrierInfo2);

    // End the copy command buffer
    result = palCmdEnd(cpyCmdBuffer);
    if (result != PAL_RESULT_SUCCESS) {
        logResult(result, "Failed to end command buffer");
        return PAL_FALSE;
    }

    // Submit the copy command buffer
    PalCommandBufferSubmitInfo submitInfo = {0};
    submitInfo.cmdBuffer = cpyCmdBuffer;
    submitInfo.fence = nullptr; // we are not using fence
    submitInfo.signalSemaphore = semaphore;

    result = palSubmitCommandBuffer(cpyQueue, &submitInfo);
    if (result != PAL_RESULT_SUCCESS) {
        logResult(result, "Failed to submit command buffer");
        return PAL_FALSE;
    }

    // We transition the image to a different usage to check if the graphics queue has ownership
    barrierInfo.oldState = PAL_USAGE_STATE_SHADER_READ;
    barrierInfo.srcStages = PAL_PIPELINE_STAGE_FRAGMENT_SHADER;
    barrierInfo.newState = PAL_USAGE_STATE_COLOR_ATTACHMENT_WRITE;
    barrierInfo.dstStages = PAL_PIPELINE_STAGE_COLOR_ATTACHMENT;
    palCmdImageBarrier(gfxCmdBuffer, texture, &range, &barrierInfo);

    // End the graphics command buffer
    result = palCmdEnd(gfxCmdBuffer);
    if (result != PAL_RESULT_SUCCESS) {
        logResult(result, "Failed to end command buffer");
        return PAL_FALSE;
    }

    // Submit the graphics command buffer
    submitInfo.cmdBuffer = gfxCmdBuffer;
    submitInfo.waitStages = PAL_PIPELINE_STAGE_TRANSFER;
    submitInfo.waitSemaphore = semaphore; // wait for the copy queue

    result = palSubmitCommandBuffer(gfxQueue, &submitInfo);
    if (result != PAL_RESULT_SUCCESS) {
        logResult(result, "Failed to submit command buffer");
        return PAL_FALSE;
    }

    // wait for the fence
    result = palWaitFence(fence, PAL_INFINITE);
    if (result != PAL_RESULT_SUCCESS) {
        logResult(result, "Failed to wait fence");
        return PAL_FALSE;
    }

    palDestroyFence(fence);
    palDestroySemaphore(semaphore);
    palDestroyBuffer(stagingBuffer);
    palDestroyImage(texture);

    palFreeCommandBuffer(cpyCmdBuffer);
    palFreeCommandBuffer(gfxCmdBuffer);
    palDestroyCommandPool(cpyCmdPool);
    palDestroyCommandPool(gfxCmdPool);

    palDestroyQueue(cpyQueue);
    palDestroyQueue(gfxQueue);
    palDestroyDevice(device);
    palShutdownGraphics();
}