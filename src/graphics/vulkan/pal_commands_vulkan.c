
/**
    PAL - Prime Abstraction Layer
    Copyright (C) 2025
    Licensed under the Zlib license. See LICENSE file in root.
 */

#if PAL_HAS_VULKAN_BACKEND
#include "pal_vulkan.h"

typedef struct {
    VkAccessFlags2 access;
    VkImageLayout layout;
} BarrierInfo;

static void commitShaderbindingTableUpdate(
    CommandBufferVk* cmdBuffer,
    ShaderBindingTableVk* sbt)
{
    if (!sbt->isDirty) {
        return;
    }

    // begin upload buffer copy to gpu buffer
    VkBufferCopy copyRegion = {0};
    copyRegion.size = sbt->stagingBufferSize;
    s_Vk.cmdCopyBuffer(cmdBuffer->handle, sbt->stagingBuffer, sbt->buffer, 1, &copyRegion);

    // put a memory barrier
    VkBufferMemoryBarrier2KHR barrier = {0};
    barrier.sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER_2_KHR;

    barrier.srcStageMask = VK_PIPELINE_STAGE_2_COPY_BIT_KHR;
    barrier.srcAccessMask = VK_ACCESS_2_TRANSFER_WRITE_BIT_KHR;

    barrier.dstStageMask = VK_PIPELINE_STAGE_2_RAY_TRACING_SHADER_BIT_KHR;
    barrier.dstAccessMask = VK_ACCESS_2_SHADER_READ_BIT_KHR;

    barrier.buffer = sbt->buffer;
    barrier.offset = 0;
    barrier.size = VK_WHOLE_SIZE;

    VkDependencyInfo dependencyInfo = {0};
    dependencyInfo.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO;
    dependencyInfo.bufferMemoryBarrierCount = 1;
    dependencyInfo.pBufferMemoryBarriers = &barrier;

    cmdBuffer->device->cmdPipelineBarrier(cmdBuffer->handle, &dependencyInfo);
    sbt->isDirty = PAL_FALSE;
}

static BarrierInfo barrierToVk(PalUsageState state)
{
    BarrierInfo barrier = {0};
    switch (state) {
        case PAL_USAGE_STATE_PRESENT: {
            barrier.access = 0;
            barrier.layout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
            return barrier;
        }

        case PAL_USAGE_STATE_COLOR_ATTACHMENT_WRITE: {
            barrier.access = VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT_KHR;
            barrier.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
            return barrier;
        }

        case PAL_USAGE_STATE_DEPTH_ATTACHMENT_READ: {
            barrier.access = VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_READ_BIT_KHR;
            barrier.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
            return barrier;
        }

        case PAL_USAGE_STATE_DEPTH_ATTACHMENT_WRITE: {
            barrier.access = VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT_KHR;
            barrier.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
            return barrier;
        }

        case PAL_USAGE_STATE_STENCIL_ATTACHMENT_READ: {
            barrier.access = VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_READ_BIT_KHR;
            barrier.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
            return barrier;
        }

        case PAL_USAGE_STATE_STENCIL_ATTACHMENT_WRITE: {
            barrier.access = VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT_KHR;
            barrier.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
            return barrier;
        }

        case PAL_USAGE_STATE_FRAGMENT_SHADING_RATE_ATTACHMENT_READ: {
            barrier.access = VK_ACCESS_2_FRAGMENT_SHADING_RATE_ATTACHMENT_READ_BIT_KHR;
            barrier.layout = VK_IMAGE_LAYOUT_FRAGMENT_SHADING_RATE_ATTACHMENT_OPTIMAL_KHR;
            return barrier;
        }

        case PAL_USAGE_STATE_TRANSFER_READ: {
            barrier.access = VK_ACCESS_2_TRANSFER_READ_BIT_KHR;
            barrier.layout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
            return barrier;
        }

        case PAL_USAGE_STATE_TRANSFER_WRITE: {
            barrier.access = VK_ACCESS_2_TRANSFER_WRITE_BIT_KHR;
            barrier.layout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
            return barrier;
        }

        case PAL_USAGE_STATE_VERTEX_READ: {
            barrier.access = VK_ACCESS_2_VERTEX_ATTRIBUTE_READ_BIT_KHR;
            barrier.layout = VK_IMAGE_LAYOUT_UNDEFINED;
            return barrier;
        }

        case PAL_USAGE_STATE_INDEX_READ: {
            barrier.access = VK_ACCESS_2_INDEX_READ_BIT_KHR;
            barrier.layout = VK_IMAGE_LAYOUT_UNDEFINED;
            return barrier;
        }

        case PAL_USAGE_STATE_INDIRECT_READ: {
            barrier.access = VK_ACCESS_2_INDIRECT_COMMAND_READ_BIT_KHR;
            barrier.layout = VK_IMAGE_LAYOUT_UNDEFINED;
            return barrier;
        }

        case PAL_USAGE_STATE_UNIFORM_READ: {
            barrier.access = VK_ACCESS_2_UNIFORM_READ_BIT_KHR;
            barrier.layout = VK_IMAGE_LAYOUT_UNDEFINED;
            return barrier;
        }

        case PAL_USAGE_STATE_SHADER_READ: {
            barrier.access = VK_ACCESS_2_SHADER_READ_BIT_KHR;
            barrier.layout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            return barrier;
        }

        case PAL_USAGE_STATE_SHADER_WRITE: {
            barrier.access = VK_ACCESS_2_SHADER_WRITE_BIT_KHR;
            barrier.layout = VK_IMAGE_LAYOUT_GENERAL;
            return barrier;
        }

        case PAL_USAGE_STATE_STORAGE_READ: {
            barrier.access = VK_ACCESS_2_SHADER_READ_BIT_KHR;
            barrier.layout = VK_IMAGE_LAYOUT_GENERAL;
            return barrier;
        }

        case PAL_USAGE_STATE_STORAGE_WRITE: {
            barrier.access = VK_ACCESS_2_SHADER_WRITE_BIT_KHR;
            barrier.layout = VK_IMAGE_LAYOUT_GENERAL;
            return barrier;
        }

        case PAL_USAGE_STATE_HOST_READ: {
            barrier.access = VK_ACCESS_2_HOST_READ_BIT_KHR;
            barrier.layout = VK_IMAGE_LAYOUT_UNDEFINED;
            return barrier;
        }

        case PAL_USAGE_STATE_HOST_WRITE: {
            barrier.access = VK_ACCESS_2_HOST_WRITE_BIT_KHR;
            barrier.layout = VK_IMAGE_LAYOUT_UNDEFINED;
            return barrier;
        }

        case PAL_USAGE_STATE_ACCELERATION_STRUCTURE_READ: {
            barrier.access = VK_ACCESS_2_ACCELERATION_STRUCTURE_READ_BIT_KHR;
            barrier.layout = VK_IMAGE_LAYOUT_UNDEFINED;
            return barrier;
        }

        case PAL_USAGE_STATE_ACCELERATION_STRUCTURE_WRITE: {
            barrier.access = VK_ACCESS_2_ACCELERATION_STRUCTURE_WRITE_BIT_KHR;
            barrier.layout = VK_IMAGE_LAYOUT_UNDEFINED;
            return barrier;
        }
    }

    barrier.access = 0;
    barrier.layout = VK_IMAGE_LAYOUT_UNDEFINED;
    return barrier;
}

static VkRenderingFlags renderingFlagToVk(PalRenderingFlags flags)
{
    VkRenderingFlags renderingFlags = 0;
    if (flags == PAL_RENDERING_FLAG_NONE) {
        renderingFlags = 0;
    }

    if (flags & PAL_RENDERING_FLAG_RESUMING) {
        renderingFlags |= VK_RENDERING_RESUMING_BIT;
    }

    if (flags & PAL_RENDERING_FLAG_SUSPENDING) {
        renderingFlags |= VK_RENDERING_SUSPENDING_BIT;
    }

    return renderingFlags;
}

static VkResolveModeFlags resolveModeToVk(PalResolveMode mode)
{
    switch (mode) {
        case PAL_RESOLVE_MODE_SAMPLE_ZERO:
            return VK_RESOLVE_MODE_SAMPLE_ZERO_BIT_KHR;

        case PAL_RESOLVE_MODE_AVERAGE:
            return VK_RESOLVE_MODE_AVERAGE_BIT_KHR;

        case PAL_RESOLVE_MODE_MIN:
            return VK_RESOLVE_MODE_MIN_BIT_KHR;

        case PAL_RESOLVE_MODE_MAX:
            return VK_RESOLVE_MODE_MAX_BIT_KHR;
    }

    return VK_RESOLVE_MODE_NONE_KHR;
}

PalResult PAL_CALL cmdBeginVk(
    PalCommandBuffer* cmdBuffer,
    PalRenderingLayoutInfo* info)
{
    CommandBufferVk* cmdBufferImpl = (CommandBufferVk*)cmdBuffer;
    VkCommandBufferBeginInfo beginInfo = {0};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

    VkCommandBufferInheritanceInfo inheritanceInfo = {0};
    inheritanceInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_INHERITANCE_INFO;
    VkCommandBufferInheritanceRenderingInfoKHR layout = {0};
    layout.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_INHERITANCE_RENDERING_INFO_KHR;

    cmdBufferImpl->allocator.offset = 0; // reset
    VkFormat format = VK_FORMAT_UNDEFINED;
    VkFormat* colorAttachments = nullptr;

    if (!cmdBufferImpl->primary) {
        // secondary command buffer
        colorAttachments = palLinearAlloc(
            &cmdBufferImpl->allocator,
            sizeof(VkFormat) * info->colorAttachentCount,
            0);

        if (!colorAttachments) {
            return PAL_RESULT_CODE_OUT_OF_MEMORY;
        }

        for (int i = 0; i < info->colorAttachentCount; i++) {
            format = formatToVk(info->colorAttachmentsFormat[i]);
            colorAttachments[i] = format;
        }
        layout.colorAttachmentCount = info->colorAttachentCount;
        layout.pColorAttachmentFormats = colorAttachments;

        // depth stencil attachment
        format = formatToVk(info->depthStencilAttachmentFormat);
        layout.depthAttachmentFormat = format;
        layout.stencilAttachmentFormat = format;

        layout.rasterizationSamples = samplesToVk(info->sampleCount);
        layout.flags = renderingFlagToVk(info->flags);
        if (info->viewCount == 1) {
            layout.viewMask = 0;
        } else {
            layout.viewMask = (1 << info->viewCount) - 1;
        }

        inheritanceInfo.pNext = &layout;
        beginInfo.pNext = &inheritanceInfo;
    }

    VkResult result = s_Vk.cmdBegin(cmdBufferImpl->handle, &beginInfo);
    if (result != VK_SUCCESS) {
        return makeResultVk(result);
    }
    return PAL_RESULT_SUCCESS;
}

PalResult PAL_CALL cmdEndVk(PalCommandBuffer* cmdBuffer)
{
    CommandBufferVk* cmdBufferImpl = (CommandBufferVk*)cmdBuffer;
    VkResult result = s_Vk.cmdEnd(cmdBufferImpl->handle);
    if (result != VK_SUCCESS) {
        return makeResultVk(result);
    }

    return PAL_RESULT_SUCCESS;
}

PalResult PAL_CALL cmdExecuteCommandBufferVk(
    PalCommandBuffer* primaryCmdBuffer,
    PalCommandBuffer* secondaryCmdBuffer)
{
    CommandBufferVk* primaryCmdBufferImpl = (CommandBufferVk*)primaryCmdBuffer;
    CommandBufferVk* secondaryCmdBufferImpl = (CommandBufferVk*)secondaryCmdBuffer;
    s_Vk.cmdExecuteCommandBuffer(primaryCmdBufferImpl->handle, 1, &secondaryCmdBufferImpl->handle);
    return PAL_RESULT_SUCCESS;
}

void PAL_CALL cmdSetFragmentShadingRateVk(
    PalCommandBuffer* cmdBuffer,
    PalFragmentShadingRateState* state)
{
    CommandBufferVk* cmdBufferImpl = (CommandBufferVk*)cmdBuffer;
    DeviceVk* device = cmdBufferImpl->device;
    VkExtent2D size = getShadingRateSizeVk(state->rate);
    VkFragmentShadingRateCombinerOpKHR combinerOps[2];
    for (int i = 0; i < 2; i++) {
        combinerOps[i] = combinerOpsToVk(state->combinerOps[i]);
    }

    device->cmdSetFragmentShadingRate(cmdBufferImpl->handle, &size, combinerOps);
}

void PAL_CALL cmdDrawMeshTasksVk(
    PalCommandBuffer* cmdBuffer,
    uint32_t groupCountX,
    uint32_t groupCountY,
    uint32_t groupCountZ)
{
    CommandBufferVk* cmdBufferImpl = (CommandBufferVk*)cmdBuffer;
    DeviceVk* device = cmdBufferImpl->device;
    device->cmdDrawMeshTask(cmdBufferImpl->handle, groupCountX, groupCountY, groupCountZ);
}

void PAL_CALL cmdDrawMeshTasksIndirectVk(
    PalCommandBuffer* cmdBuffer,
    PalBuffer* buffer,
    uint32_t drawCount)
{
    CommandBufferVk* cmdBufferImpl = (CommandBufferVk*)cmdBuffer;
    DeviceVk* device = cmdBufferImpl->device;
    BufferVk* bufferImpl = (BufferVk*)buffer;

    uint32_t stride = sizeof(VkDrawMeshTasksIndirectCommandEXT);
    device
        ->cmdDrawMeshTaskIndirect(cmdBufferImpl->handle, bufferImpl->handle, 0, drawCount, stride);
}

void PAL_CALL cmdDrawMeshTasksIndirectCountVk(
    PalCommandBuffer* cmdBuffer,
    PalBuffer* buffer,
    PalBuffer* countBuffer,
    uint32_t maxDrawCount)
{
    CommandBufferVk* cmdBufferImpl = (CommandBufferVk*)cmdBuffer;
    DeviceVk* device = cmdBufferImpl->device;
    BufferVk* bufferImpl = (BufferVk*)buffer;
    BufferVk* countBufferImpl = (BufferVk*)countBuffer;

    uint32_t stride = sizeof(VkDrawMeshTasksIndirectCommandEXT);
    device->cmdDrawMeshTaskIndirectCount(
        cmdBufferImpl->handle,
        bufferImpl->handle,
        0,
        countBufferImpl->handle,
        0,
        maxDrawCount,
        stride);
}

void PAL_CALL cmdBuildAccelerationStructureVk(
    PalCommandBuffer* cmdBuffer,
    PalAccelerationStructureBuildInfo* info)
{
    CommandBufferVk* cmdBufferImpl = (CommandBufferVk*)cmdBuffer;
    DeviceVk* device = cmdBufferImpl->device;

    VkAccelerationStructureGeometryKHR* geometries = nullptr;
    VkAccelerationStructureBuildRangeInfoKHR* rangeInfos = nullptr;
    const VkAccelerationStructureBuildRangeInfoKHR** tmpRangeInfos = nullptr;
    VkAccelerationStructureBuildGeometryInfoKHR buildInfo = {0};

    tmpRangeInfos = palLinearAlloc(&cmdBufferImpl->allocator, sizeof(void*) * info->count, 0);

    geometries = palLinearAlloc(
        &cmdBufferImpl->allocator,
        sizeof(VkAccelerationStructureGeometryKHR) * info->count,
        0);

    rangeInfos = palLinearAlloc(
        &cmdBufferImpl->allocator,
        sizeof(VkAccelerationStructureBuildRangeInfoKHR) * info->count,
        0);

    memset(geometries, 0, sizeof(VkAccelerationStructureGeometryKHR) * info->count);
    memset(rangeInfos, 0, sizeof(VkAccelerationStructureBuildRangeInfoKHR) * info->count);
    fillBuildInfoVk(PAL_FALSE, info, geometries, &buildInfo, rangeInfos);

    for (int i = 0; i < info->count; i++) {
        tmpRangeInfos[i] = &rangeInfos[i];
    }

    cmdBufferImpl->device
        ->cmdBuildAccelerationStructures(cmdBufferImpl->handle, 1, &buildInfo, tmpRangeInfos);
}

void PAL_CALL cmdBeginRenderingVk(
    PalCommandBuffer* cmdBuffer,
    PalRenderingInfo* info)
{
    CommandBufferVk* cmdBufferImpl = (CommandBufferVk*)cmdBuffer;
    VkRenderingInfoKHR rendering = {0};
    rendering.sType = VK_STRUCTURE_TYPE_RENDERING_INFO_KHR;

    VkRenderingAttachmentInfoKHR depthAttachment = {0};
    VkRenderingAttachmentInfoKHR stencilAttachment = {0};

    VkRenderingAttachmentInfoKHR* colorAttachments = nullptr;
    colorAttachments = palLinearAlloc(
        &cmdBufferImpl->allocator,
        sizeof(VkRenderingAttachmentInfoKHR) * info->colorAttachentCount,
        0);

    VkRenderingAttachmentInfoKHR* attachment = nullptr;
    PalAttachmentDesc* desc = nullptr;
    ImageViewVk* imageView = nullptr;
    ImageViewVk* resolveImageView = nullptr;
    VkImageLayout layout = VK_IMAGE_LAYOUT_GENERAL;

    VkRenderingFragmentShadingRateAttachmentInfoKHR fsrInfo = {0};
    fsrInfo.sType = VK_STRUCTURE_TYPE_RENDERING_FRAGMENT_SHADING_RATE_ATTACHMENT_INFO_KHR;

    for (int i = 0; i < info->colorAttachentCount; i++) {
        attachment = &colorAttachments[i];
        desc = &info->colorAttachments[i];
        imageView = (ImageViewVk*)desc->imageView;
        resolveImageView = (ImageViewVk*)desc->resolveImageView;

        attachment->sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO_KHR;
        attachment->pNext = nullptr;
        layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        attachment->resolveImageView = nullptr;
        attachment->resolveImageLayout = VK_IMAGE_LAYOUT_UNDEFINED;

        attachment->clearValue.color.float32[0] = desc->clearValue.color[0];
        attachment->clearValue.color.float32[1] = desc->clearValue.color[1];
        attachment->clearValue.color.float32[2] = desc->clearValue.color[2];
        attachment->clearValue.color.float32[3] = desc->clearValue.color[3];

        attachment->imageView = imageView->handle;
        if (resolveImageView) {
            attachment->resolveImageView = resolveImageView->handle;
            attachment->resolveImageLayout = layout;
        }

        // load op
        if (desc->loadOp == PAL_LOAD_OP_CLEAR) {
            attachment->loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;

        } else if (desc->loadOp == PAL_LOAD_OP_LOAD) {
            attachment->loadOp = VK_ATTACHMENT_LOAD_OP_LOAD;

        } else if (desc->loadOp == PAL_LOAD_OP_DONT_CARE) {
            attachment->loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        }

        // store op
        if (desc->storeOp == PAL_STORE_OP_STORE) {
            attachment->storeOp = VK_ATTACHMENT_STORE_OP_STORE;

        } else if (desc->storeOp == PAL_STORE_OP_DONT_CARE) {
            attachment->storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        }

        attachment->resolveMode = resolveModeToVk(desc->resolveMode);
        attachment->imageLayout = layout;
    }

    rendering.colorAttachmentCount = info->colorAttachentCount;
    rendering.pColorAttachments = colorAttachments;

    // depth attachment
    if (info->depthStencilAttachment) {
        attachment = &depthAttachment;
        desc = info->depthStencilAttachment;
        imageView = (ImageViewVk*)desc->imageView;
        resolveImageView = (ImageViewVk*)desc->resolveImageView;

        attachment->sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO_KHR;
        attachment->pNext = nullptr;
        attachment->resolveImageView = nullptr;
        attachment->resolveImageLayout = VK_IMAGE_LAYOUT_UNDEFINED;

        stencilAttachment.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO_KHR;
        stencilAttachment.pNext = nullptr;
        stencilAttachment.resolveImageView = nullptr;
        stencilAttachment.resolveImageLayout = VK_IMAGE_LAYOUT_UNDEFINED;

        attachment->clearValue.depthStencil.depth = desc->clearValue.depth;
        stencilAttachment.clearValue.depthStencil.stencil = desc->clearValue.stencil;

        attachment->imageView = imageView->handle;
        stencilAttachment.imageView = imageView->handle;
        if (resolveImageView) {
            attachment->resolveImageView = resolveImageView->handle;
            attachment->resolveImageLayout = layout;

            stencilAttachment.resolveImageView = resolveImageView->handle;
            stencilAttachment.resolveImageLayout = layout;
        }

        // load op
        if (desc->loadOp == PAL_LOAD_OP_CLEAR) {
            attachment->loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;

        } else if (desc->loadOp == PAL_LOAD_OP_LOAD) {
            attachment->loadOp = VK_ATTACHMENT_LOAD_OP_LOAD;

        } else if (desc->loadOp == PAL_LOAD_OP_DONT_CARE) {
            attachment->loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        }

        // store op
        if (desc->storeOp == PAL_STORE_OP_STORE) {
            attachment->storeOp = VK_ATTACHMENT_STORE_OP_STORE;

        } else if (desc->storeOp == PAL_STORE_OP_DONT_CARE) {
            attachment->storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        }

        // stencil load op
        if (desc->stencilLoadOp == PAL_LOAD_OP_CLEAR) {
            stencilAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;

        } else if (desc->stencilLoadOp == PAL_LOAD_OP_LOAD) {
            stencilAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_LOAD;

        } else if (desc->stencilLoadOp == PAL_LOAD_OP_DONT_CARE) {
            stencilAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        }

        // stencil store op
        if (desc->stencilStoreOp == PAL_STORE_OP_STORE) {
            stencilAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;

        } else if (desc->stencilStoreOp == PAL_STORE_OP_DONT_CARE) {
            stencilAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        }

        attachment->resolveMode = resolveModeToVk(desc->resolveMode);
        attachment->imageLayout = VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL;
        stencilAttachment.resolveMode = resolveModeToVk(desc->resolveMode);
        stencilAttachment.imageLayout = VK_IMAGE_LAYOUT_STENCIL_ATTACHMENT_OPTIMAL;

        rendering.pDepthAttachment = &depthAttachment;
        rendering.pStencilAttachment = &stencilAttachment;
    }

    // fragment shading rate
    if (info->fragmentShadingRateImageView) {
        imageView = (ImageViewVk*)info->fragmentShadingRateImageView;
        fsrInfo.imageView = imageView->handle;
        fsrInfo.shadingRateAttachmentTexelSize.width = info->fragmentShadingRateTexelWidth;
        fsrInfo.shadingRateAttachmentTexelSize.height = info->fragmentShadingRateTexelHeight;
        fsrInfo.imageLayout = VK_IMAGE_LAYOUT_FRAGMENT_SHADING_RATE_ATTACHMENT_OPTIMAL_KHR;
        rendering.pNext = &fsrInfo;
    }

    rendering.layerCount = info->arrayLayerCount;
    rendering.renderArea.offset.x = info->renderArea.x;
    rendering.renderArea.offset.y = info->renderArea.y;
    rendering.renderArea.extent.width = info->renderArea.width;
    rendering.renderArea.extent.height = info->renderArea.height;

    if (info->viewCount == 1) {
        rendering.viewMask = 0;
    } else {
        rendering.viewMask = (1 << info->viewCount) - 1;
    }

    rendering.flags = renderingFlagToVk(info->flags);
    cmdBufferImpl->device->cmdBeginRendering(cmdBufferImpl->handle, &rendering);
}

void PAL_CALL cmdEndRenderingVk(PalCommandBuffer* cmdBuffer)
{
    CommandBufferVk* cmdBufferImpl = (CommandBufferVk*)cmdBuffer;
    cmdBufferImpl->device->cmdEndRendering(cmdBufferImpl->handle);
}

void PAL_CALL cmdCopyBufferVk(
    PalCommandBuffer* cmdBuffer,
    PalBuffer* dst,
    PalBuffer* src,
    PalBufferCopyInfo* copyInfo)
{
    CommandBufferVk* cmdBufferImpl = (CommandBufferVk*)cmdBuffer;
    BufferVk* dstBuffer = (BufferVk*)dst;
    BufferVk* srcBuffer = (BufferVk*)src;

    VkBufferCopy copyRegion = {0};
    copyRegion.size = copyInfo->size;
    copyRegion.dstOffset = copyInfo->dstOffset;
    copyRegion.srcOffset = copyInfo->srcOffset;
    s_Vk.cmdCopyBuffer(cmdBufferImpl->handle, srcBuffer->handle, dstBuffer->handle, 1, &copyRegion);
}

void PAL_CALL cmdCopyBufferToImageVk(
    PalCommandBuffer* cmdBuffer,
    PalImage* dstImage,
    PalBuffer* srcBuffer,
    PalBufferImageCopyInfo* copyInfo)
{
    CommandBufferVk* cmdBufferImpl = (CommandBufferVk*)cmdBuffer;
    ImageVk* dst = (ImageVk*)dstImage;
    BufferVk* src = (BufferVk*)srcBuffer;

    VkBufferImageCopy copyRegion = {0};
    copyRegion.bufferImageHeight = copyInfo->bufferImageHeight;
    copyRegion.bufferOffset = copyInfo->bufferOffset;
    copyRegion.bufferRowLength = copyInfo->bufferRowLength;

    copyRegion.imageOffset.x = copyInfo->imageOffsetX;
    copyRegion.imageOffset.y = copyInfo->imageOffsetY;
    copyRegion.imageOffset.z = copyInfo->imageOffsetZ;

    copyRegion.imageExtent.width = copyInfo->imageWidth;
    copyRegion.imageExtent.height = copyInfo->imageHeight;
    copyRegion.imageExtent.depth = copyInfo->imageDepth;

    copyRegion.imageSubresource.aspectMask = imageAspectToVk(copyInfo->imageAspect);
    copyRegion.imageSubresource.baseArrayLayer = copyInfo->ImageStartArrayLayer;
    copyRegion.imageSubresource.layerCount = copyInfo->ImageArrayLayerCount;
    copyRegion.imageSubresource.mipLevel = copyInfo->ImageMipLevel;

    s_Vk.cmdCopyBufferToImage(
        cmdBufferImpl->handle,
        src->handle,
        dst->handle,
        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
        1,
        &copyRegion);
}

void PAL_CALL cmdCopyImageVk(
    PalCommandBuffer* cmdBuffer,
    PalImage* dst,
    PalImage* src,
    PalImageCopyInfo* copyInfo)
{
    CommandBufferVk* cmdBufferImpl = (CommandBufferVk*)cmdBuffer;
    ImageVk* dstImage = (ImageVk*)dst;
    ImageVk* srcImage = (ImageVk*)src;

    VkImageCopy copyRegion = {0};
    copyRegion.dstOffset.x = copyInfo->dstOffsetX;
    copyRegion.dstOffset.y = copyInfo->dstOffsetY;
    copyRegion.dstOffset.z = copyInfo->dstOffsetZ;

    copyRegion.srcOffset.x = copyInfo->srcOffsetX;
    copyRegion.srcOffset.y = copyInfo->srcOffsetY;
    copyRegion.srcOffset.z = copyInfo->srcOffsetZ;

    copyRegion.extent.width = copyInfo->width;
    copyRegion.extent.height = copyInfo->height;
    copyRegion.extent.depth = copyInfo->depth;

    copyRegion.dstSubresource.aspectMask = imageAspectToVk(copyInfo->aspect);
    copyRegion.dstSubresource.baseArrayLayer = copyInfo->dstStartArrayLayer;
    copyRegion.dstSubresource.layerCount = copyInfo->arrayLayerCount;
    copyRegion.dstSubresource.mipLevel = copyInfo->dstMipLevel;

    copyRegion.srcSubresource.aspectMask = imageAspectToVk(copyInfo->aspect);
    copyRegion.srcSubresource.baseArrayLayer = copyInfo->srcStartArrayLayer;
    copyRegion.srcSubresource.layerCount = copyInfo->arrayLayerCount;
    copyRegion.srcSubresource.mipLevel = copyInfo->srcMipLevel;

    s_Vk.cmdCopyImage(
        cmdBufferImpl->handle,
        srcImage->handle,
        VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
        dstImage->handle,
        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
        1,
        &copyRegion);
}

void PAL_CALL cmdCopyImageToBufferVk(
    PalCommandBuffer* cmdBuffer,
    PalBuffer* dstBuffer,
    PalImage* srcImage,
    PalBufferImageCopyInfo* copyInfo)
{
    CommandBufferVk* cmdBufferImpl = (CommandBufferVk*)cmdBuffer;
    BufferVk* dst = (BufferVk*)dstBuffer;
    ImageVk* src = (ImageVk*)srcImage;

    VkBufferImageCopy copyRegion = {0};
    copyRegion.bufferImageHeight = copyInfo->bufferImageHeight;
    copyRegion.bufferOffset = copyInfo->bufferOffset;
    copyRegion.bufferRowLength = copyInfo->bufferRowLength;

    copyRegion.imageOffset.x = copyInfo->imageOffsetX;
    copyRegion.imageOffset.y = copyInfo->imageOffsetY;
    copyRegion.imageOffset.z = copyInfo->imageOffsetZ;

    copyRegion.imageExtent.width = copyInfo->imageWidth;
    copyRegion.imageExtent.height = copyInfo->imageHeight;
    copyRegion.imageExtent.depth = copyInfo->imageDepth;

    copyRegion.imageSubresource.aspectMask = imageAspectToVk(copyInfo->imageAspect);
    copyRegion.imageSubresource.baseArrayLayer = copyInfo->ImageStartArrayLayer;
    copyRegion.imageSubresource.layerCount = copyInfo->ImageArrayLayerCount;
    copyRegion.imageSubresource.mipLevel = copyInfo->ImageMipLevel;

    s_Vk.cmdCopyImageToBuffer(
        cmdBufferImpl->handle,
        src->handle,
        VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
        dst->handle,
        1,
        &copyRegion);
}

void PAL_CALL cmdBindPipelineVk(
    PalCommandBuffer* cmdBuffer,
    PalPipeline* pipeline)
{
    CommandBufferVk* cmdBufferImpl = (CommandBufferVk*)cmdBuffer;
    PipelineVk* pipelineImpl = (PipelineVk*)pipeline;
    s_Vk.cmdBindPipeline(cmdBufferImpl->handle, pipelineImpl->bindPoint, pipelineImpl->handle);
    cmdBufferImpl->pipeline = pipelineImpl;
}

void PAL_CALL cmdSetViewportVk(
    PalCommandBuffer* cmdBuffer,
    uint32_t count,
    PalViewport* viewports)
{
    CommandBufferVk* cmdBufferImpl = (CommandBufferVk*)cmdBuffer;
    VkViewport* viewportsImpl = nullptr;
    viewportsImpl = palLinearAlloc(&cmdBufferImpl->allocator, sizeof(VkViewport) * count, 0);
    for (int i = 0; i < count; i++) {
        VkViewport* tmp = &viewportsImpl[i];
        tmp->x = viewports[i].x;
        tmp->y = viewports[i].y;
        tmp->width = viewports[i].width;
        tmp->height = viewports[i].height;
        tmp->minDepth = viewports[i].minDepth;
        tmp->maxDepth = viewports[i].maxDepth;
    }

    s_Vk.cmdSetViewports(cmdBufferImpl->handle, 0, count, viewportsImpl);
}

void PAL_CALL cmdSetScissorsVk(
    PalCommandBuffer* cmdBuffer,
    uint32_t count,
    PalRect2D* scissors)
{
    CommandBufferVk* cmdBufferImpl = (CommandBufferVk*)cmdBuffer;
    VkRect2D* scissorsImpl = nullptr;
    scissorsImpl = palLinearAlloc(&cmdBufferImpl->allocator, sizeof(VkRect2D) * count, 0);
    for (int i = 0; i < count; i++) {
        VkRect2D* tmp = &scissorsImpl[i];
        tmp->offset.x = scissors[i].x;
        tmp->offset.y = scissors[i].y;
        tmp->extent.width = scissors[i].width;
        tmp->extent.height = scissors[i].height;
    }

    s_Vk.cmdSetScissors(cmdBufferImpl->handle, 0, count, scissorsImpl);
}

void PAL_CALL cmdBindVertexBuffersVk(
    PalCommandBuffer* cmdBuffer,
    uint32_t firstSlot,
    uint32_t count,
    PalBuffer** buffers,
    uint64_t* offsets)
{
    CommandBufferVk* cmdBufferImpl = (CommandBufferVk*)cmdBuffer;
    VkBuffer* buffersImpl = nullptr;
    buffersImpl = palLinearAlloc(&cmdBufferImpl->allocator, sizeof(VkBuffer) * count, 0);
    for (int i = 0; i < count; i++) {
        BufferVk* tmp = (BufferVk*)buffers[i];
        buffersImpl[i] = tmp->handle;
    }

    s_Vk.cmdBindVertexBuffers(cmdBufferImpl->handle, firstSlot, count, buffersImpl, offsets);
}

void PAL_CALL cmdBindIndexBufferVk(
    PalCommandBuffer* cmdBuffer,
    PalBuffer* buffer,
    uint64_t offset,
    PalIndexType type)
{
    CommandBufferVk* cmdBufferImpl = (CommandBufferVk*)cmdBuffer;
    BufferVk* bufferImpl = (BufferVk*)buffer;
    VkIndexType bufferType = VK_INDEX_TYPE_UINT32;
    if (type == PAL_INDEX_TYPE_UINT16) {
        bufferType = VK_INDEX_TYPE_UINT16;
    }

    s_Vk.cmdBindIndexBuffer(cmdBufferImpl->handle, bufferImpl->handle, offset, bufferType);
}

void PAL_CALL cmdDrawVk(
    PalCommandBuffer* cmdBuffer,
    uint32_t vertexCount,
    uint32_t instanceCount,
    uint32_t firstVertex,
    uint32_t firstInstance)
{
    CommandBufferVk* cmdBufferImpl = (CommandBufferVk*)cmdBuffer;
    s_Vk.cmdDraw(cmdBufferImpl->handle, vertexCount, instanceCount, firstVertex, firstInstance);
}

void PAL_CALL cmdDrawIndirectVk(
    PalCommandBuffer* cmdBuffer,
    PalBuffer* buffer,
    uint32_t count)
{
    CommandBufferVk* cmdBufferImpl = (CommandBufferVk*)cmdBuffer;
    BufferVk* bufferImpl = (BufferVk*)buffer;
    uint32_t stride = sizeof(VkDrawIndirectCommand);
    s_Vk.cmdDrawIndirect(cmdBufferImpl->handle, bufferImpl->handle, 0, count, stride);
}

void PAL_CALL cmdDrawIndirectCountVk(
    PalCommandBuffer* cmdBuffer,
    PalBuffer* buffer,
    PalBuffer* countBuffer,
    uint32_t maxDrawCount)
{
    CommandBufferVk* cmdBufferImpl = (CommandBufferVk*)cmdBuffer;
    BufferVk* bufferImpl = (BufferVk*)buffer;
    BufferVk* countBufferImpl = (BufferVk*)countBuffer;
    DeviceVk* device = cmdBufferImpl->device;

    uint32_t stride = sizeof(VkDrawIndirectCommand);
    device->cmdDrawIndirectCount(
        cmdBufferImpl->handle,
        bufferImpl->handle,
        0,
        countBufferImpl->handle,
        0,
        maxDrawCount,
        stride);
}

void PAL_CALL cmdDrawIndexedVk(
    PalCommandBuffer* cmdBuffer,
    uint32_t indexCount,
    uint32_t instanceCount,
    uint32_t firstIndex,
    int32_t vertexOffset,
    uint32_t firstInstance)
{
    CommandBufferVk* cmdBufferImpl = (CommandBufferVk*)cmdBuffer;
    s_Vk.cmdDrawIndexed(
        cmdBufferImpl->handle,
        indexCount,
        instanceCount,
        firstIndex,
        vertexOffset,
        firstInstance);
}

void PAL_CALL cmdDrawIndexedIndirectVk(
    PalCommandBuffer* cmdBuffer,
    PalBuffer* buffer,
    uint32_t count)
{
    CommandBufferVk* cmdBufferImpl = (CommandBufferVk*)cmdBuffer;
    BufferVk* bufferImpl = (BufferVk*)buffer;
    uint32_t stride = sizeof(VkDrawIndexedIndirectCommand);
    s_Vk.cmdDrawIndexedIndirect(cmdBufferImpl->handle, bufferImpl->handle, 0, count, stride);
}

void PAL_CALL cmdDrawIndexedIndirectCountVk(
    PalCommandBuffer* cmdBuffer,
    PalBuffer* buffer,
    PalBuffer* countBuffer,
    uint32_t maxDrawCount)
{
    CommandBufferVk* cmdBufferImpl = (CommandBufferVk*)cmdBuffer;
    BufferVk* bufferImpl = (BufferVk*)buffer;
    BufferVk* countBufferImpl = (BufferVk*)countBuffer;
    DeviceVk* device = cmdBufferImpl->device;

    uint32_t stride = sizeof(VkDrawIndexedIndirectCommand);
    device->cmdDrawIndexedIndirectCount(
        cmdBufferImpl->handle,
        bufferImpl->handle,
        0,
        countBufferImpl->handle,
        0,
        maxDrawCount,
        stride);
}

void PAL_CALL cmdAccelerationStructureBarrierVk(
    PalCommandBuffer* cmdBuffer,
    PalAccelerationStructure* as,
    PalBarrierInfo* info)
{
    CommandBufferVk* cmdBufferImpl = (CommandBufferVk*)cmdBuffer;
    VkMemoryBarrier2KHR barrier = {0};
    barrier.sType = VK_STRUCTURE_TYPE_MEMORY_BARRIER_2_KHR;

    BarrierInfo old = barrierToVk(info->oldState);
    BarrierInfo new = barrierToVk(info->newState);

    barrier.srcStageMask = pipelineStagesToVk(info->srcStages);
    barrier.srcAccessMask = old.access;
    barrier.dstStageMask = pipelineStagesToVk(info->dstStages);
    barrier.dstAccessMask = new.access;

    VkDependencyInfo dependencyInfo = {0};
    dependencyInfo.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO;
    dependencyInfo.memoryBarrierCount = 1;
    dependencyInfo.pMemoryBarriers = &barrier;
    cmdBufferImpl->device->cmdPipelineBarrier(cmdBufferImpl->handle, &dependencyInfo);
}

void PAL_CALL cmdImageBarrierVk(
    PalCommandBuffer* cmdBuffer,
    PalImage* image,
    PalImageSubresourceRange* subresourceRange,
    PalBarrierInfo* info)
{
    CommandBufferVk* cmdBufferImpl = (CommandBufferVk*)cmdBuffer;
    ImageVk* imageImpl = (ImageVk*)image;
    VkImageMemoryBarrier2KHR barrier = {0};
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2_KHR;

    BarrierInfo old = barrierToVk(info->oldState);
    BarrierInfo new = barrierToVk(info->newState);
    if (old.layout != VK_IMAGE_LAYOUT_UNDEFINED) {
        // make sure new.layout is not undefined
        if (new.layout == VK_IMAGE_LAYOUT_UNDEFINED) {
            new.layout = old.layout;
        }
    }

    barrier.srcStageMask = pipelineStagesToVk(info->srcStages);
    barrier.srcAccessMask = old.access;
    barrier.oldLayout = old.layout;
    barrier.dstStageMask = pipelineStagesToVk(info->dstStages);
    barrier.dstAccessMask = new.access;
    barrier.newLayout = new.layout;

    barrier.image = imageImpl->handle;
    barrier.subresourceRange.aspectMask = imageAspectToVk(subresourceRange->aspect);
    barrier.subresourceRange.baseArrayLayer = subresourceRange->startArrayLayer;
    barrier.subresourceRange.baseMipLevel = subresourceRange->startMipLevel;
    barrier.subresourceRange.layerCount = subresourceRange->layerArrayCount;
    barrier.subresourceRange.levelCount = subresourceRange->mipLevelCount;

    VkDependencyInfo dependencyInfo = {0};
    dependencyInfo.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO;
    dependencyInfo.imageMemoryBarrierCount = 1;
    dependencyInfo.pImageMemoryBarriers = &barrier;
    cmdBufferImpl->device->cmdPipelineBarrier(cmdBufferImpl->handle, &dependencyInfo);
}

void PAL_CALL cmdImageOwnershipTransferVk(
    PalCommandBuffer* srcCmdBuffer,
    PalCommandBuffer* dstCmdBuffer,
    PalImage* image,
    PalImageSubresourceRange* subresourceRange,
    PalUsageState srcUsageState,
    PalPipelineStages srcPipelineStages)
{
    ImageVk* imageImpl = (ImageVk*)image;
    CommandBufferVk* srcCmdBufferImpl = (CommandBufferVk*)srcCmdBuffer;
    CommandBufferVk* dstCmdBufferImpl = (CommandBufferVk*)dstCmdBuffer;
    uint32_t srcQueueFamily = srcCmdBufferImpl->pool->queue->phyQueue->familyIndex;
    uint32_t dstQueueFamily = dstCmdBufferImpl->pool->queue->phyQueue->familyIndex;

    if (srcQueueFamily == dstQueueFamily) {
        PalBarrierInfo info = {0};
        info.oldState = srcUsageState;
        info.srcStages = srcPipelineStages;
        cmdImageBarrierVk(srcCmdBuffer, image, subresourceRange, &info);
        return;
    }

    VkImageSubresourceRange range = {0};
    range.aspectMask = imageAspectToVk(subresourceRange->aspect);
    range.baseArrayLayer = subresourceRange->startArrayLayer;
    range.baseMipLevel = subresourceRange->startMipLevel;
    range.layerCount = subresourceRange->layerArrayCount;
    range.levelCount = subresourceRange->mipLevelCount;

    BarrierInfo old = barrierToVk(srcUsageState);
    VkImageMemoryBarrier2KHR releaseBarrier = {0};
    releaseBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2_KHR;
    releaseBarrier.subresourceRange = range;
    releaseBarrier.image = imageImpl->handle;

    releaseBarrier.srcStageMask = pipelineStagesToVk(srcPipelineStages);
    releaseBarrier.srcAccessMask = old.access;
    releaseBarrier.oldLayout = old.layout;
    releaseBarrier.srcQueueFamilyIndex = srcQueueFamily;

    releaseBarrier.dstStageMask = VK_PIPELINE_STAGE_2_NONE;
    releaseBarrier.dstAccessMask = VK_ACCESS_2_NONE;
    releaseBarrier.newLayout = releaseBarrier.oldLayout;
    releaseBarrier.dstQueueFamilyIndex = dstQueueFamily;

    VkImageMemoryBarrier2KHR acquireBarrier = {0};
    acquireBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2_KHR;
    acquireBarrier.subresourceRange = range;
    acquireBarrier.image = imageImpl->handle;

    acquireBarrier.srcStageMask = VK_PIPELINE_STAGE_2_NONE;
    acquireBarrier.srcAccessMask = VK_ACCESS_2_NONE;
    acquireBarrier.oldLayout = old.layout;
    acquireBarrier.srcQueueFamilyIndex = srcQueueFamily;

    acquireBarrier.dstStageMask = VK_PIPELINE_STAGE_2_NONE;
    acquireBarrier.dstAccessMask = VK_ACCESS_2_NONE;
    acquireBarrier.newLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    acquireBarrier.dstQueueFamilyIndex = dstQueueFamily;

    VkDependencyInfo dependencyInfo = {0};
    dependencyInfo.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO;
    dependencyInfo.imageMemoryBarrierCount = 1;
    dependencyInfo.pImageMemoryBarriers = &releaseBarrier;
    srcCmdBufferImpl->device->cmdPipelineBarrier(srcCmdBufferImpl->handle, &dependencyInfo);

    dependencyInfo.pImageMemoryBarriers = &acquireBarrier;
    dstCmdBufferImpl->device->cmdPipelineBarrier(dstCmdBufferImpl->handle, &dependencyInfo);
}

void PAL_CALL cmdBufferBarrierVk(
    PalCommandBuffer* cmdBuffer,
    PalBuffer* buffer,
    PalBarrierInfo* info)
{
    CommandBufferVk* cmdBufferImpl = (CommandBufferVk*)cmdBuffer;
    BufferVk* bufferImpl = (BufferVk*)buffer;
    VkBufferMemoryBarrier2KHR barrier = {0};
    barrier.sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER_2_KHR;

    BarrierInfo old = barrierToVk(info->oldState);
    BarrierInfo new = barrierToVk(info->newState);

    barrier.srcStageMask = pipelineStagesToVk(info->srcStages);
    barrier.srcAccessMask = old.access;
    barrier.dstStageMask = pipelineStagesToVk(info->dstStages);
    barrier.dstAccessMask = new.access;

    barrier.buffer = bufferImpl->handle;
    barrier.offset = 0;
    barrier.size = VK_WHOLE_SIZE;

    VkDependencyInfo dependencyInfo = {0};
    dependencyInfo.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO;
    dependencyInfo.bufferMemoryBarrierCount = 1;
    dependencyInfo.pBufferMemoryBarriers = &barrier;
    cmdBufferImpl->device->cmdPipelineBarrier(cmdBufferImpl->handle, &dependencyInfo);
}

void PAL_CALL cmdBufferOwnershipTransferVk(
    PalCommandBuffer* srcCmdBuffer,
    PalCommandBuffer* dstCmdBuffer,
    PalBuffer* buffer,
    PalUsageState srcUsageState,
    PalPipelineStages srcPipelineStages)
{
    BufferVk* bufferImpl = (BufferVk*)buffer;
    CommandBufferVk* srcCmdBufferImpl = (CommandBufferVk*)srcCmdBuffer;
    CommandBufferVk* dstCmdBufferImpl = (CommandBufferVk*)dstCmdBuffer;
    uint32_t srcQueueFamily = srcCmdBufferImpl->pool->queue->phyQueue->familyIndex;
    uint32_t dstQueueFamily = dstCmdBufferImpl->pool->queue->phyQueue->familyIndex;

    if (srcQueueFamily == dstQueueFamily) {
        PalBarrierInfo info = {0};
        info.oldState = srcUsageState;
        info.srcStages = srcPipelineStages;
        cmdBufferBarrierVk(srcCmdBuffer, buffer, &info);
        return;
    }

    BarrierInfo old = barrierToVk(srcUsageState);
    VkBufferMemoryBarrier2KHR releaseBarrier = {0};
    releaseBarrier.sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER_2_KHR;
    releaseBarrier.srcStageMask = pipelineStagesToVk(srcPipelineStages);
    releaseBarrier.srcAccessMask = old.access;
    releaseBarrier.srcQueueFamilyIndex = srcQueueFamily;

    releaseBarrier.dstStageMask = VK_PIPELINE_STAGE_2_NONE;
    releaseBarrier.dstAccessMask = VK_ACCESS_2_NONE;
    releaseBarrier.dstQueueFamilyIndex = dstQueueFamily;

    VkBufferMemoryBarrier2KHR acquireBarrier = {0};
    acquireBarrier.sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER_2_KHR;
    acquireBarrier.srcStageMask = VK_PIPELINE_STAGE_2_NONE;
    acquireBarrier.srcAccessMask = VK_ACCESS_2_NONE;
    acquireBarrier.srcQueueFamilyIndex = srcQueueFamily;

    acquireBarrier.dstStageMask = VK_PIPELINE_STAGE_2_NONE;
    acquireBarrier.dstAccessMask = VK_ACCESS_2_NONE;
    acquireBarrier.dstQueueFamilyIndex = dstQueueFamily;

    VkDependencyInfo dependencyInfo = {0};
    dependencyInfo.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO;
    dependencyInfo.bufferMemoryBarrierCount = 1;
    dependencyInfo.pBufferMemoryBarriers = &releaseBarrier;
    srcCmdBufferImpl->device->cmdPipelineBarrier(srcCmdBufferImpl->handle, &dependencyInfo);

    dependencyInfo.pBufferMemoryBarriers = &acquireBarrier;
    dstCmdBufferImpl->device->cmdPipelineBarrier(dstCmdBufferImpl->handle, &dependencyInfo);
}

void PAL_CALL cmdDispatchVk(
    PalCommandBuffer* cmdBuffer,
    uint32_t groupCountX,
    uint32_t groupCountY,
    uint32_t groupCountZ)
{
    CommandBufferVk* cmdBufferImpl = (CommandBufferVk*)cmdBuffer;
    s_Vk.cmdDispatch(cmdBufferImpl->handle, groupCountX, groupCountY, groupCountZ);
}

void PAL_CALL cmdDispatchBaseVk(
    PalCommandBuffer* cmdBuffer,
    uint32_t baseGroupX,
    uint32_t baseGroupY,
    uint32_t baseGroupZ,
    uint32_t groupCountX,
    uint32_t groupCountY,
    uint32_t groupCountZ)
{
    CommandBufferVk* cmdBufferImpl = (CommandBufferVk*)cmdBuffer;
    DeviceVk* device = cmdBufferImpl->device;

    device->cmdDispatchBase(
        cmdBufferImpl->handle,
        baseGroupX,
        baseGroupY,
        baseGroupZ,
        groupCountX,
        groupCountY,
        groupCountZ);
}

void PAL_CALL cmdDispatchIndirectVk(
    PalCommandBuffer* cmdBuffer,
    PalBuffer* buffer)
{
    CommandBufferVk* cmdBufferImpl = (CommandBufferVk*)cmdBuffer;
    BufferVk* bufferImpl = (BufferVk*)buffer;
    s_Vk.cmdDispatchIndirect(cmdBufferImpl->handle, bufferImpl->handle, 0);
}

void PAL_CALL cmdTraceRaysVk(
    PalCommandBuffer* cmdBuffer,
    PalShaderBindingTable* sbt,
    uint32_t raygenIndex,
    uint32_t width,
    uint32_t height,
    uint32_t depth)
{
    CommandBufferVk* cmdBufferImpl = (CommandBufferVk*)cmdBuffer;
    DeviceVk* device = cmdBufferImpl->device;
    ShaderBindingTableVk* sbtImpl = (ShaderBindingTableVk*)sbt;

    VkStridedDeviceAddressRegionKHR raygenAddress = {0};
    raygenAddress.size = sbtImpl->raygen.region.size;
    raygenAddress.stride = sbtImpl->raygen.region.stride;
    raygenAddress.deviceAddress =
        sbtImpl->baseAddress + raygenIndex * sbtImpl->raygen.region.stride;

    // we need to make sure the SBT is up to date
    commitShaderbindingTableUpdate(cmdBufferImpl, sbtImpl);

    cmdBufferImpl->device->cmdTraceRays(
        cmdBufferImpl->handle,
        &raygenAddress,
        &sbtImpl->miss.region,
        &sbtImpl->hit.region,
        &sbtImpl->callable.region,
        width,
        height,
        depth);
}

void PAL_CALL cmdTraceRaysIndirectVk(
    PalCommandBuffer* cmdBuffer,
    uint32_t raygenIndex,
    PalShaderBindingTable* sbt,
    PalBuffer* buffer)
{
    CommandBufferVk* cmdBufferImpl = (CommandBufferVk*)cmdBuffer;
    ShaderBindingTableVk* sbtImpl = (ShaderBindingTableVk*)sbt;
    BufferVk* bufferImpl = (BufferVk*)buffer;

    PalDeviceAddress address = sbtImpl->baseAddress + raygenIndex * sbtImpl->raygen.region.stride;
    sbtImpl->raygen.region.deviceAddress = address;

    // we need to make sure the SBT is up to date
    commitShaderbindingTableUpdate(cmdBufferImpl, sbtImpl);

    // copy user buffer data into a tmp gpu buffer abd execute with it
    VkBufferCopy copyRegion = {0};
    copyRegion.size = sizeof(VkTraceRaysIndirectCommandKHR);
    s_Vk.cmdCopyBuffer(
        cmdBufferImpl->handle,
        bufferImpl->handle,
        cmdBufferImpl->buffer,
        1,
        &copyRegion);

    // put a memory barrier
    VkBufferMemoryBarrier2KHR barrier = {0};
    barrier.sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER_2_KHR;
    barrier.srcStageMask = VK_PIPELINE_STAGE_2_COPY_BIT_KHR;
    barrier.srcAccessMask = VK_ACCESS_2_TRANSFER_WRITE_BIT_KHR;
    barrier.dstStageMask = VK_PIPELINE_STAGE_2_RAY_TRACING_SHADER_BIT_KHR;
    barrier.dstAccessMask = VK_ACCESS_2_SHADER_READ_BIT_KHR;

    barrier.buffer = cmdBufferImpl->buffer;
    barrier.offset = 0;
    barrier.size = VK_WHOLE_SIZE;

    VkDependencyInfo dependencyInfo = {0};
    dependencyInfo.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO;
    dependencyInfo.bufferMemoryBarrierCount = 1;
    dependencyInfo.pBufferMemoryBarriers = &barrier;
    cmdBufferImpl->device->cmdPipelineBarrier(cmdBufferImpl->handle, &dependencyInfo);

    VkDeviceAddress bufAddress = 0;
    VkBufferDeviceAddressInfoKHR bufferInfo = {0};
    bufferInfo.buffer = cmdBufferImpl->buffer;
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO_KHR;
    bufAddress =
        cmdBufferImpl->device->getBufferrAddress(cmdBufferImpl->device->handle, &bufferInfo);

    cmdBufferImpl->device->cmdTraceRaysIndirect(
        cmdBufferImpl->handle,
        &sbtImpl->raygen.region,
        &sbtImpl->miss.region,
        &sbtImpl->hit.region,
        &sbtImpl->callable.region,
        bufAddress);
}

void PAL_CALL cmdBindDescriptorSetVk(
    PalCommandBuffer* cmdBuffer,
    uint32_t setIndex,
    PalDescriptorSet* set)
{
    CommandBufferVk* cmdBufferImpl = (CommandBufferVk*)cmdBuffer;
    PipelineVk* pipeline = cmdBufferImpl->pipeline;
    DescriptorSetVk* setImpl = (DescriptorSetVk*)set;

    s_Vk.cmdBindDescriptorSets(
        cmdBufferImpl->handle,
        pipeline->bindPoint,
        pipeline->layout,
        setIndex,
        1,
        &setImpl->handle,
        0,
        nullptr);
}

void PAL_CALL cmdPushConstantsVk(
    PalCommandBuffer* cmdBuffer,
    uint64_t offset,
    uint64_t size,
    const void* value)
{
    CommandBufferVk* cmdBufferImpl = (CommandBufferVk*)cmdBuffer;
    PipelineVk* pipeline = cmdBufferImpl->pipeline;

    s_Vk.cmdPushConstants(
        cmdBufferImpl->handle,
        pipeline->layout,
        cmdBufferImpl->device->shaderStages,
        offset,
        size,
        value);
}

void PAL_CALL cmdSetCullModeVk(
    PalCommandBuffer* cmdBuffer,
    PalCullMode cullMode)
{
    CommandBufferVk* cmdBufferImpl = (CommandBufferVk*)cmdBuffer;
    DeviceVk* device = cmdBufferImpl->device;

    VkCullModeFlags cullModeImpl = 0;
    switch (cullMode) {
        case PAL_CULL_MODE_BACK:
            cullModeImpl = VK_CULL_MODE_BACK_BIT;

        case PAL_CULL_MODE_FRONT:
            cullModeImpl = VK_CULL_MODE_FRONT_BIT;

        case PAL_CULL_MODE_NONE:
            cullModeImpl = VK_CULL_MODE_NONE;
    }

    device->cmdSetCullMode(cmdBufferImpl->handle, cullModeImpl);
}

void PAL_CALL cmdSetFrontFaceVk(
    PalCommandBuffer* cmdBuffer,
    PalFrontFace frontFace)
{
    CommandBufferVk* cmdBufferImpl = (CommandBufferVk*)cmdBuffer;
    DeviceVk* device = cmdBufferImpl->device;

    VkFrontFace frontFaceImpl = 0;
    switch (frontFace) {
        case PAL_FRONT_FACE_CLOCKWISE:
            frontFaceImpl = VK_FRONT_FACE_CLOCKWISE;

        case PAL_FRONT_FACE_COUNTER_CLOCKWISE:
            frontFaceImpl = VK_FRONT_FACE_COUNTER_CLOCKWISE;
    }

    device->cmdSetFrontFace(cmdBufferImpl->handle, frontFaceImpl);
}

void PAL_CALL cmdSetPrimitiveTopologyVk(
    PalCommandBuffer* cmdBuffer,
    PalPrimitiveTopology topology)
{
    CommandBufferVk* cmdBufferImpl = (CommandBufferVk*)cmdBuffer;
    DeviceVk* device = cmdBufferImpl->device;

    VkPrimitiveTopology topologyImpl = 0;
    switch (topology) {
        case PAL_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST: {
            topologyImpl = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
            break;
        }

        case PAL_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP: {
            topologyImpl = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP;
            break;
        }

        case PAL_PRIMITIVE_TOPOLOGY_LINE_LIST: {
            topologyImpl = VK_PRIMITIVE_TOPOLOGY_LINE_LIST;
            break;
        }

        case PAL_PRIMITIVE_TOPOLOGY_LINE_STRIP: {
            topologyImpl = VK_PRIMITIVE_TOPOLOGY_LINE_STRIP;
            break;
        }

        case PAL_PRIMITIVE_TOPOLOGY_POINT_LIST: {
            topologyImpl = VK_PRIMITIVE_TOPOLOGY_POINT_LIST;
            break;
        }
    }

    device->cmdSetPrimitiveTopology(cmdBufferImpl->handle, topologyImpl);
}

void PAL_CALL cmdSetDepthTestEnableVk(
    PalCommandBuffer* cmdBuffer,
    PalBool enable)
{
    CommandBufferVk* cmdBufferImpl = (CommandBufferVk*)cmdBuffer;
    DeviceVk* device = cmdBufferImpl->device;
    device->cmdSetDepthTestEnable(cmdBufferImpl->handle, enable);
}

void PAL_CALL cmdSetDepthWriteEnableVk(
    PalCommandBuffer* cmdBuffer,
    PalBool enable)
{
    CommandBufferVk* cmdBufferImpl = (CommandBufferVk*)cmdBuffer;
    DeviceVk* device = cmdBufferImpl->device;
    device->cmdSetDepthWriteEnable(cmdBufferImpl->handle, enable);
}

void PAL_CALL cmdSetStencilOpVk(
    PalCommandBuffer* cmdBuffer,
    PalStencilFaceFlags faceMask,
    PalStencilOp failOp,
    PalStencilOp passOp,
    PalStencilOp depthFailOp,
    PalCompareOp compareOp)
{
    CommandBufferVk* cmdBufferImpl = (CommandBufferVk*)cmdBuffer;
    DeviceVk* device = cmdBufferImpl->device;
    VkStencilFaceFlags faceFlags = 0;
    if (faceMask & PAL_STENCIL_FACE_FLAG_BACK) {
        faceFlags |= VK_STENCIL_FACE_BACK_BIT;
    }

    if (faceMask & PAL_STENCIL_FACE_FLAG_FRONT) {
        faceFlags |= VK_STENCIL_FACE_FRONT_BIT;
    }

    VkStencilOp failOpImpl = stencilOpToVk(failOp);
    VkStencilOp passOpImpl = stencilOpToVk(passOp);
    VkStencilOp depthFailOpImpl = stencilOpToVk(depthFailOp);
    VkCompareOp compareOpImpl = compareOpToVk(compareOp);

    device->cmdSetStencilOp(
        cmdBufferImpl->handle,
        faceFlags,
        failOpImpl,
        passOpImpl,
        depthFailOpImpl,
        compareOpImpl);
}

#endif // PAL_HAS_VULKAN_BACKEND