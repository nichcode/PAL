
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
    CommandBufferVk* vkCmdBuffer = (CommandBufferVk*)cmdBuffer;
    VkCommandBufferBeginInfo beginInfo = {0};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

    VkCommandBufferInheritanceInfo inheritanceInfo = {0};
    inheritanceInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_INHERITANCE_INFO;
    VkCommandBufferInheritanceRenderingInfoKHR layout = {0};
    layout.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_INHERITANCE_RENDERING_INFO_KHR;

    vkCmdBuffer->allocator.offset = 0; // reset
    VkFormat format = VK_FORMAT_UNDEFINED;
    VkFormat* colorAttachments = nullptr;

    if (!vkCmdBuffer->primary) {
        // secondary command buffer
        colorAttachments = palLinearAlloc(
            &vkCmdBuffer->allocator, 
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

    VkResult result = s_Vk.cmdBegin(vkCmdBuffer->handle, &beginInfo);
    if (result != VK_SUCCESS) {
        return makeResultVk(result);
    }
    return PAL_RESULT_SUCCESS;
}

PalResult PAL_CALL cmdEndVk(PalCommandBuffer* cmdBuffer)
{
    CommandBufferVk* vkCmdBuffer = (CommandBufferVk*)cmdBuffer;
    VkResult result = s_Vk.cmdEnd(vkCmdBuffer->handle);
    if (result != VK_SUCCESS) {
        return makeResultVk(result);
    }

    return PAL_RESULT_SUCCESS;
}

PalResult PAL_CALL cmdExecuteCommandBufferVk(
    PalCommandBuffer* primaryCmdBuffer,
    PalCommandBuffer* secondaryCmdBuffer)
{
    CommandBufferVk* vkCmdBuffer = (CommandBufferVk*)primaryCmdBuffer;
    CommandBufferVk* vkCmdBuffer2 = (CommandBufferVk*)secondaryCmdBuffer;
    s_Vk.cmdExecuteCommandBuffer(vkCmdBuffer->handle, 1, &vkCmdBuffer2->handle);
    return PAL_RESULT_SUCCESS;
}

void PAL_CALL cmdSetFragmentShadingRateVk(
    PalCommandBuffer* cmdBuffer,
    PalFragmentShadingRateState* state)
{
    CommandBufferVk* vkCmdBuffer = (CommandBufferVk*)cmdBuffer;
    DeviceVk* device = vkCmdBuffer->device;
    VkExtent2D size = getShadingRateSizeVk(state->rate);
    VkFragmentShadingRateCombinerOpKHR combinerOps[2];
    for (int i = 0; i < 2; i++) {
        combinerOps[i] = combinerOpsToVk(state->combinerOps[i]);
    }

    device->cmdSetFragmentShadingRate(vkCmdBuffer->handle, &size, combinerOps);
}

void PAL_CALL cmdDrawMeshTasksVk(
    PalCommandBuffer* cmdBuffer,
    uint32_t groupCountX,
    uint32_t groupCountY,
    uint32_t groupCountZ)
{
    CommandBufferVk* vkCmdBuffer = (CommandBufferVk*)cmdBuffer;
    DeviceVk* device = vkCmdBuffer->device;
    device->cmdDrawMeshTask(vkCmdBuffer->handle, groupCountX, groupCountY, groupCountZ);
}

void PAL_CALL cmdDrawMeshTasksIndirectVk(
    PalCommandBuffer* cmdBuffer,
    PalBuffer* buffer,
    uint32_t drawCount)
{
    CommandBufferVk* vkCmdBuffer = (CommandBufferVk*)cmdBuffer;
    DeviceVk* device = vkCmdBuffer->device;
    BufferVk* vkBuffer = (BufferVk*)buffer;

    uint32_t stride = sizeof(VkDrawMeshTasksIndirectCommandEXT);
    device->cmdDrawMeshTaskIndirect(
        vkCmdBuffer->handle, 
        vkBuffer->handle,
        0, 
        drawCount,
        stride);
}

void PAL_CALL cmdDrawMeshTasksIndirectCountVk(
    PalCommandBuffer* cmdBuffer,
    PalBuffer* buffer,
    PalBuffer* countBuffer,
    uint32_t maxDrawCount)
{
    CommandBufferVk* vkCmdBuffer = (CommandBufferVk*)cmdBuffer;
    DeviceVk* device = vkCmdBuffer->device;
    BufferVk* vkBuffer = (BufferVk*)buffer;
    BufferVk* vkCountBuffer = (BufferVk*)countBuffer;

    uint32_t stride = sizeof(VkDrawMeshTasksIndirectCommandEXT);
    device->cmdDrawMeshTaskIndirectCount(
        vkCmdBuffer->handle,
        vkBuffer->handle,
        0,
        vkCountBuffer->handle,
        0,
        maxDrawCount,
        stride);
}

void PAL_CALL cmdBuildAccelerationStructureVk(
    PalCommandBuffer* cmdBuffer,
    PalAccelerationStructureBuildInfo* info)
{
    CommandBufferVk* vkCmdBuffer = (CommandBufferVk*)cmdBuffer;
    DeviceVk* device = vkCmdBuffer->device;
    
    VkAccelerationStructureGeometryKHR* geometries = nullptr;
    VkAccelerationStructureBuildRangeInfoKHR* rangeInfos = nullptr;
    const VkAccelerationStructureBuildRangeInfoKHR** tmpRangeInfos = nullptr;
    VkAccelerationStructureBuildGeometryInfoKHR buildInfo = {0};

    tmpRangeInfos = palLinearAlloc(
        &vkCmdBuffer->allocator, 
        sizeof(void*) * info->count, 
        0);

    geometries = palLinearAlloc(
        &vkCmdBuffer->allocator, 
        sizeof(VkAccelerationStructureGeometryKHR) * info->count, 
        0);

    rangeInfos = palLinearAlloc(
        &vkCmdBuffer->allocator, 
        sizeof(VkAccelerationStructureBuildRangeInfoKHR) * info->count, 
        0);

    if (!tmpRangeInfos || !rangeInfos || !geometries) {
        return;
    }

    memset(geometries, 0, sizeof(VkAccelerationStructureGeometryKHR) * info->count);
    memset(rangeInfos, 0, sizeof(VkAccelerationStructureBuildRangeInfoKHR) * info->count);
    fillBuildInfoVk(PAL_FALSE, info, geometries, &buildInfo, rangeInfos);

    for (int i = 0; i < info->count; i++) {
        tmpRangeInfos[i] = &rangeInfos[i];
    }

    vkCmdBuffer->device->cmdBuildAccelerationStructures(
        vkCmdBuffer->handle,
        1, 
        &buildInfo, 
        tmpRangeInfos);
}

void PAL_CALL cmdBeginRenderingVk(
    PalCommandBuffer* cmdBuffer,
    PalRenderingInfo* info)
{
    CommandBufferVk* vkCmdBuffer = (CommandBufferVk*)cmdBuffer;
    VkRenderingInfoKHR rendering = {0};
    rendering.sType = VK_STRUCTURE_TYPE_RENDERING_INFO_KHR;

    VkRenderingAttachmentInfoKHR depthAttachment = {0};
    VkRenderingAttachmentInfoKHR stencilAttachment = {0};
    
    VkRenderingAttachmentInfoKHR* colorAttachments = nullptr;
    colorAttachments = palLinearAlloc(
        &vkCmdBuffer->allocator, 
        sizeof(VkRenderingAttachmentInfoKHR) * info->colorAttachentCount, 
        0);

    if (!colorAttachments) {
        return;
    }

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
    vkCmdBuffer->device->cmdBeginRendering(vkCmdBuffer->handle, &rendering);
}

void PAL_CALL cmdEndRenderingVk(PalCommandBuffer* cmdBuffer)
{
    CommandBufferVk* vkCmdBuffer = (CommandBufferVk*)cmdBuffer;
    vkCmdBuffer->device->cmdEndRendering(vkCmdBuffer->handle);
}

void PAL_CALL cmdCopyBufferVk(
    PalCommandBuffer* cmdBuffer,
    PalBuffer* dst,
    PalBuffer* src,
    PalBufferCopyInfo* copyInfo)
{
    CommandBufferVk* vkCmdBuffer = (CommandBufferVk*)cmdBuffer;
    BufferVk* dstBuffer = (BufferVk*)dst;
    BufferVk* srcBuffer = (BufferVk*)src;

    VkBufferCopy copyRegion = {0};
    copyRegion.size = copyInfo->size;
    copyRegion.dstOffset = copyInfo->dstOffset;
    copyRegion.srcOffset = copyInfo->srcOffset;
    s_Vk.cmdCopyBuffer(vkCmdBuffer->handle, srcBuffer->handle, dstBuffer->handle, 1, &copyRegion);
}

void PAL_CALL cmdCopyBufferToImageVk(
    PalCommandBuffer* cmdBuffer,
    PalImage* dstImage,
    PalBuffer* srcBuffer,
    PalBufferImageCopyInfo* copyInfo)
{
    CommandBufferVk* vkCmdBuffer = (CommandBufferVk*)cmdBuffer;
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
        vkCmdBuffer->handle, 
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
    CommandBufferVk* vkCmdBuffer = (CommandBufferVk*)cmdBuffer;
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
        vkCmdBuffer->handle, 
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
    CommandBufferVk* vkCmdBuffer = (CommandBufferVk*)cmdBuffer;
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
        vkCmdBuffer->handle, 
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
    CommandBufferVk* vkCmdBuffer = (CommandBufferVk*)cmdBuffer;
    PipelineVk* vkPipeline = (PipelineVk*)pipeline;
    s_Vk.cmdBindPipeline(vkCmdBuffer->handle, vkPipeline->bindPoint, vkPipeline->handle);
    vkCmdBuffer->pipeline = vkPipeline;
}

void PAL_CALL cmdSetViewportVk(
    PalCommandBuffer* cmdBuffer,
    uint32_t count,
    PalViewport* viewports)
{
    CommandBufferVk* vkCmdBuffer = (CommandBufferVk*)cmdBuffer;
    VkViewport* vkViewports = nullptr;
    vkViewports = palLinearAlloc(&vkCmdBuffer->allocator, sizeof(VkViewport) * count, 0);
    for (int i = 0; i < count; i++) {
        VkViewport* tmp = &vkViewports[i];
        tmp->x = viewports[i].x;
        tmp->y = viewports[i].y;
        tmp->width = viewports[i].width;
        tmp->height = viewports[i].height;
        tmp->minDepth = viewports[i].minDepth;
        tmp->maxDepth = viewports[i].maxDepth;
    }

    s_Vk.cmdSetViewports(vkCmdBuffer->handle, 0, count, vkViewports);
}

void PAL_CALL cmdSetScissorsVk(
    PalCommandBuffer* cmdBuffer,
    uint32_t count,
    PalRect2D* scissors)
{
    CommandBufferVk* vkCmdBuffer = (CommandBufferVk*)cmdBuffer;
    VkRect2D* vkScissors = nullptr;
    vkScissors = palLinearAlloc(&vkCmdBuffer->allocator, sizeof(VkRect2D) * count, 0);
    for (int i = 0; i < count; i++) {
        VkRect2D* tmp = &vkScissors[i];
        tmp->offset.x = scissors[i].x;
        tmp->offset.y = scissors[i].y;
        tmp->extent.width = scissors[i].width;
        tmp->extent.height = scissors[i].height;
    }

    s_Vk.cmdSetScissors(vkCmdBuffer->handle, 0, count, vkScissors);
}

void PAL_CALL cmdBindVertexBuffersVk(
    PalCommandBuffer* cmdBuffer,
    uint32_t firstSlot,
    uint32_t count,
    PalBuffer** buffers,
    uint64_t* offsets)
{
    CommandBufferVk* vkCmdBuffer = (CommandBufferVk*)cmdBuffer;
    VkBuffer* vkBuffers = nullptr;
    vkBuffers = palLinearAlloc(&vkCmdBuffer->allocator, sizeof(VkBuffer) * count, 0);
    for (int i = 0; i < count; i++) {
        BufferVk* tmp = (BufferVk*)buffers[i];
        vkBuffers[i] = tmp->handle;
    }

    s_Vk.cmdBindVertexBuffers(vkCmdBuffer->handle, firstSlot, count, vkBuffers, offsets);
}

void PAL_CALL cmdBindIndexBufferVk(
    PalCommandBuffer* cmdBuffer,
    PalBuffer* buffer,
    uint64_t offset,
    PalIndexType type)
{
    CommandBufferVk* vkCmdBuffer = (CommandBufferVk*)cmdBuffer;
    BufferVk* vkBuffer = (BufferVk*)buffer;
    VkIndexType bufferType = VK_INDEX_TYPE_UINT32;
    if (type == PAL_INDEX_TYPE_UINT16) {
        bufferType = VK_INDEX_TYPE_UINT16;
    }

    s_Vk.cmdBindIndexBuffer(vkCmdBuffer->handle, vkBuffer->handle, offset, bufferType);
}

void PAL_CALL cmdDrawVk(
    PalCommandBuffer* cmdBuffer,
    uint32_t vertexCount,
    uint32_t instanceCount,
    uint32_t firstVertex,
    uint32_t firstInstance)
{
    CommandBufferVk* vkCmdBuffer = (CommandBufferVk*)cmdBuffer;
    s_Vk.cmdDraw(vkCmdBuffer->handle, vertexCount, instanceCount, firstVertex, firstInstance);
}

void PAL_CALL cmdDrawIndirectVk(
    PalCommandBuffer* cmdBuffer,
    PalBuffer* buffer,
    uint32_t count)
{
    CommandBufferVk* vkCmdBuffer = (CommandBufferVk*)cmdBuffer;
    BufferVk* vkBuffer = (BufferVk*)buffer;
    uint32_t stride = sizeof(VkDrawIndirectCommand);
    s_Vk.cmdDrawIndirect(vkCmdBuffer->handle, vkBuffer->handle, 0, count, stride);
}

void PAL_CALL cmdDrawIndirectCountVk(
    PalCommandBuffer* cmdBuffer,
    PalBuffer* buffer,
    PalBuffer* countBuffer,
    uint32_t maxDrawCount)
{
    CommandBufferVk* vkCmdBuffer = (CommandBufferVk*)cmdBuffer;
    BufferVk* vkBuffer = (BufferVk*)buffer;
    BufferVk* vkCountBuffer = (BufferVk*)countBuffer;
    DeviceVk* device = vkCmdBuffer->device;

    uint32_t stride = sizeof(VkDrawIndirectCommand);
    device->cmdDrawIndirectCount(
        vkCmdBuffer->handle,
        vkBuffer->handle,
        0,
        vkCountBuffer->handle,
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
    CommandBufferVk* vkCmdBuffer = (CommandBufferVk*)cmdBuffer;
    s_Vk.cmdDrawIndexed(
        vkCmdBuffer->handle,
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
    CommandBufferVk* vkCmdBuffer = (CommandBufferVk*)cmdBuffer;
    BufferVk* vkBuffer = (BufferVk*)buffer;
    uint32_t stride = sizeof(VkDrawIndexedIndirectCommand);
    s_Vk.cmdDrawIndexedIndirect(vkCmdBuffer->handle, vkBuffer->handle, 0, count, stride);
}

void PAL_CALL cmdDrawIndexedIndirectCountVk(
    PalCommandBuffer* cmdBuffer,
    PalBuffer* buffer,
    PalBuffer* countBuffer,
    uint32_t maxDrawCount)
{
    CommandBufferVk* vkCmdBuffer = (CommandBufferVk*)cmdBuffer;
    BufferVk* vkBuffer = (BufferVk*)buffer;
    BufferVk* vkCountBuffer = (BufferVk*)countBuffer;
    DeviceVk* device = vkCmdBuffer->device;

    uint32_t stride = sizeof(VkDrawIndexedIndirectCommand);
    device->cmdDrawIndexedIndirectCount(
        vkCmdBuffer->handle,
        vkBuffer->handle,
        0,
        vkCountBuffer->handle,
        0,
        maxDrawCount,
        stride);
}

void PAL_CALL cmdAccelerationStructureBarrierVk(
    PalCommandBuffer* cmdBuffer,
    PalAccelerationStructure* as,
    PalBarrierInfo* info)
{
    CommandBufferVk* vkCmdBuffer = (CommandBufferVk*)cmdBuffer;
    PipelineVk* pipeline = vkCmdBuffer->pipeline;
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
    vkCmdBuffer->device->cmdPipelineBarrier(vkCmdBuffer->handle, &dependencyInfo);
}

void PAL_CALL cmdImageBarrierVk(
    PalCommandBuffer* cmdBuffer,
    PalImage* image,
    PalImageSubresourceRange* subresourceRange,
    PalBarrierInfo* info)
{
    CommandBufferVk* vkCmdBuffer = (CommandBufferVk*)cmdBuffer;
    PipelineVk* pipeline = vkCmdBuffer->pipeline;
    ImageVk* vkImage = (ImageVk*)image;
    VkImageMemoryBarrier2KHR barrier = {0};
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2_KHR;

    BarrierInfo old = barrierToVk(info->oldState);
    BarrierInfo new = barrierToVk(info->newState);

    barrier.srcStageMask = pipelineStagesToVk(info->srcStages);
    barrier.srcAccessMask = old.access;
    barrier.oldLayout = old.layout;
    barrier.dstStageMask = pipelineStagesToVk(info->dstStages);
    barrier.dstAccessMask = new.access;
    barrier.newLayout = new.layout;

    barrier.image = vkImage->handle;
    barrier.subresourceRange.aspectMask = imageAspectToVk(subresourceRange->aspect);
    barrier.subresourceRange.baseArrayLayer = subresourceRange->startArrayLayer;
    barrier.subresourceRange.baseMipLevel = subresourceRange->startMipLevel;
    barrier.subresourceRange.layerCount = subresourceRange->layerArrayCount;
    barrier.subresourceRange.levelCount = subresourceRange->mipLevelCount;

    VkDependencyInfo dependencyInfo = {0};
    dependencyInfo.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO;
    dependencyInfo.imageMemoryBarrierCount = 1;
    dependencyInfo.pImageMemoryBarriers = &barrier;
    vkCmdBuffer->device->cmdPipelineBarrier(vkCmdBuffer->handle, &dependencyInfo);
}

void PAL_CALL cmdBufferBarrierVk(
    PalCommandBuffer* cmdBuffer,
    PalBuffer* buffer,
    PalBarrierInfo* info)
{
    CommandBufferVk* vkCmdBuffer = (CommandBufferVk*)cmdBuffer;
    PipelineVk* pipeline = vkCmdBuffer->pipeline;
    BufferVk* vkBuffer = (BufferVk*)buffer;
    VkBufferMemoryBarrier2KHR barrier = {0};
    barrier.sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER_2_KHR;

    BarrierInfo old = barrierToVk(info->oldState);
    BarrierInfo new = barrierToVk(info->newState);

    barrier.srcStageMask = pipelineStagesToVk(info->srcStages);
    barrier.srcAccessMask = old.access;
    barrier.dstStageMask = pipelineStagesToVk(info->dstStages);
    barrier.dstAccessMask = new.access;

    barrier.buffer = vkBuffer->handle;
    barrier.offset = 0;
    barrier.size = VK_WHOLE_SIZE;

    VkDependencyInfo dependencyInfo = {0};
    dependencyInfo.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO;
    dependencyInfo.bufferMemoryBarrierCount = 1;
    dependencyInfo.pBufferMemoryBarriers = &barrier;
    vkCmdBuffer->device->cmdPipelineBarrier(vkCmdBuffer->handle, &dependencyInfo);
}

void PAL_CALL cmdDispatchVk(
    PalCommandBuffer* cmdBuffer,
    uint32_t groupCountX,
    uint32_t groupCountY,
    uint32_t groupCountZ)
{
    CommandBufferVk* vkCmdBuffer = (CommandBufferVk*)cmdBuffer;
    s_Vk.cmdDispatch(vkCmdBuffer->handle, groupCountX, groupCountY, groupCountZ);
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
    CommandBufferVk* vkCmdBuffer = (CommandBufferVk*)cmdBuffer;
    DeviceVk* device = vkCmdBuffer->device;
   
    device->cmdDispatchBase(
        vkCmdBuffer->handle,
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
    CommandBufferVk* vkCmdBuffer = (CommandBufferVk*)cmdBuffer;
    BufferVk* vkBuffer = (BufferVk*)buffer;
    s_Vk.cmdDispatchIndirect(vkCmdBuffer->handle, vkBuffer->handle, 0);
}

void PAL_CALL cmdTraceRaysVk(
    PalCommandBuffer* cmdBuffer,
    PalShaderBindingTable* sbt,
    uint32_t raygenIndex,
    uint32_t width,
    uint32_t height,
    uint32_t depth)
{
    CommandBufferVk* vkCmdBuffer = (CommandBufferVk*)cmdBuffer;
    DeviceVk* device = vkCmdBuffer->device;
    ShaderBindingTableVk* vkSbt = (ShaderBindingTableVk*)sbt;

    VkStridedDeviceAddressRegionKHR raygenAddress = {0};
    raygenAddress.size = vkSbt->raygen.region.size;
    raygenAddress.stride = vkSbt->raygen.region.stride;
    raygenAddress.deviceAddress = vkSbt->baseAddress + raygenIndex * vkSbt->raygen.region.stride;

    // we need to make sure the SBT is up to date
    commitShaderbindingTableUpdate(vkCmdBuffer, vkSbt);

    vkCmdBuffer->device->cmdTraceRays(
        vkCmdBuffer->handle,
        &raygenAddress,
        &vkSbt->miss.region,
        &vkSbt->hit.region,
        &vkSbt->callable.region,
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
    CommandBufferVk* vkCmdBuffer = (CommandBufferVk*)cmdBuffer;
    ShaderBindingTableVk* vkSbt = (ShaderBindingTableVk*)sbt;
    BufferVk* vkBuffer = (BufferVk*)buffer;

    PalDeviceAddress address = vkSbt->baseAddress + raygenIndex * vkSbt->raygen.region.stride;
    vkSbt->raygen.region.deviceAddress = address;

    // we need to make sure the SBT is up to date
    commitShaderbindingTableUpdate(vkCmdBuffer, vkSbt);

    // copy user buffer data into a tmp gpu buffer abd execute with it
    VkBufferCopy copyRegion = {0};
    copyRegion.size = sizeof(VkTraceRaysIndirectCommandKHR);
    s_Vk.cmdCopyBuffer(vkCmdBuffer->handle, vkBuffer->handle, vkCmdBuffer->buffer, 1, &copyRegion);

    // put a memory barrier
    VkBufferMemoryBarrier2KHR barrier = {0};
    barrier.sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER_2_KHR;
    barrier.srcStageMask = VK_PIPELINE_STAGE_2_COPY_BIT_KHR;
    barrier.srcAccessMask = VK_ACCESS_2_TRANSFER_WRITE_BIT_KHR;
    barrier.dstStageMask = VK_PIPELINE_STAGE_2_RAY_TRACING_SHADER_BIT_KHR;
    barrier.dstAccessMask = VK_ACCESS_2_SHADER_READ_BIT_KHR;

    barrier.buffer = vkCmdBuffer->buffer;
    barrier.offset = 0;
    barrier.size = VK_WHOLE_SIZE;
    
    VkDependencyInfo dependencyInfo = {0};
    dependencyInfo.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO;
    dependencyInfo.bufferMemoryBarrierCount = 1;
    dependencyInfo.pBufferMemoryBarriers = &barrier;
    vkCmdBuffer->device->cmdPipelineBarrier(vkCmdBuffer->handle, &dependencyInfo);

    VkDeviceAddress bufAddress = 0;
    VkBufferDeviceAddressInfoKHR bufferInfo = {0};
    bufferInfo.buffer = vkCmdBuffer->buffer;
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO_KHR;
    bufAddress = vkCmdBuffer->device->getBufferrAddress(vkCmdBuffer->device->handle, &bufferInfo);

    vkCmdBuffer->device->cmdTraceRaysIndirect(
        vkCmdBuffer->handle,
        &vkSbt->raygen.region,
        &vkSbt->miss.region,
        &vkSbt->hit.region,
        &vkSbt->callable.region,
        bufAddress);
}

void PAL_CALL cmdBindDescriptorSetVk(
    PalCommandBuffer* cmdBuffer,
    uint32_t setIndex,
    PalDescriptorSet* set)
{
    CommandBufferVk* vkCmdBuffer = (CommandBufferVk*)cmdBuffer;
    PipelineVk* pipeline = vkCmdBuffer->pipeline;
    DescriptorSetVk* vkSet = (DescriptorSetVk*)set;

    s_Vk.cmdBindDescriptorSets(
        vkCmdBuffer->handle,
        pipeline->bindPoint,
        pipeline->layout,
        setIndex,
        1,
        &vkSet->handle,
        0,
        nullptr);
}

void PAL_CALL cmdPushConstantsVk(
    PalCommandBuffer* cmdBuffer,
    uint64_t offset,
    uint64_t size,
    const void* value)
{
    CommandBufferVk* vkCmdBuffer = (CommandBufferVk*)cmdBuffer;
    PipelineVk* pipeline = vkCmdBuffer->pipeline;
    
    s_Vk.cmdPushConstants(
        vkCmdBuffer->handle, 
        pipeline->layout, 
        vkCmdBuffer->device->shaderStages,
        offset, 
        size, 
        value);
}

void PAL_CALL cmdSetCullModeVk(
    PalCommandBuffer* cmdBuffer,
    PalCullMode cullMode)
{
    CommandBufferVk* vkCmdBuffer = (CommandBufferVk*)cmdBuffer;
    DeviceVk* device = vkCmdBuffer->device;

    VkCullModeFlags vkCullMode = 0;
    switch (cullMode) {
        case PAL_CULL_MODE_BACK:
            vkCullMode = VK_CULL_MODE_BACK_BIT;

        case PAL_CULL_MODE_FRONT:
            vkCullMode = VK_CULL_MODE_FRONT_BIT;

        case PAL_CULL_MODE_NONE:
            vkCullMode = VK_CULL_MODE_NONE;
    }
    
    device->cmdSetCullMode(vkCmdBuffer->handle, vkCullMode);
}

void PAL_CALL cmdSetFrontFaceVk(
    PalCommandBuffer* cmdBuffer,
    PalFrontFace frontFace)
{
    CommandBufferVk* vkCmdBuffer = (CommandBufferVk*)cmdBuffer;
    DeviceVk* device = vkCmdBuffer->device;

    VkFrontFace vkFrontFace = 0;
    switch (frontFace) {
        case PAL_FRONT_FACE_CLOCKWISE:
            vkFrontFace = VK_FRONT_FACE_CLOCKWISE;

        case PAL_FRONT_FACE_COUNTER_CLOCKWISE:
            vkFrontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
    }

    device->cmdSetFrontFace(vkCmdBuffer->handle, vkFrontFace);
}

void PAL_CALL cmdSetPrimitiveTopologyVk(
    PalCommandBuffer* cmdBuffer,
    PalPrimitiveTopology topology)
{
    CommandBufferVk* vkCmdBuffer = (CommandBufferVk*)cmdBuffer;
    DeviceVk* device = vkCmdBuffer->device;
  
    VkPrimitiveTopology vkTopology = 0;
    switch (topology) {
        case PAL_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST: {
            vkTopology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
            break;
        }

        case PAL_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP: {
            vkTopology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP;
            break;
        }

        case PAL_PRIMITIVE_TOPOLOGY_LINE_LIST: {
            vkTopology = VK_PRIMITIVE_TOPOLOGY_LINE_LIST;
            break;
        }

        case PAL_PRIMITIVE_TOPOLOGY_LINE_STRIP: {
            vkTopology = VK_PRIMITIVE_TOPOLOGY_LINE_STRIP;
            break;
        }

        case PAL_PRIMITIVE_TOPOLOGY_POINT_LIST: {
            vkTopology = VK_PRIMITIVE_TOPOLOGY_POINT_LIST;
            break;
        }
    }

    device->cmdSetPrimitiveTopology(vkCmdBuffer->handle, vkTopology);
}

void PAL_CALL cmdSetDepthTestEnableVk(
    PalCommandBuffer* cmdBuffer,
    PalBool enable)
{
    CommandBufferVk* vkCmdBuffer = (CommandBufferVk*)cmdBuffer;
    DeviceVk* device = vkCmdBuffer->device;
    device->cmdSetDepthTestEnable(vkCmdBuffer->handle, enable);
}

void PAL_CALL cmdSetDepthWriteEnableVk(
    PalCommandBuffer* cmdBuffer,
    PalBool enable)
{
    CommandBufferVk* vkCmdBuffer = (CommandBufferVk*)cmdBuffer;
    DeviceVk* device = vkCmdBuffer->device;
    device->cmdSetDepthWriteEnable(vkCmdBuffer->handle, enable);
}

void PAL_CALL cmdSetStencilOpVk(
    PalCommandBuffer* cmdBuffer,
    PalStencilFaceFlags faceMask,
    PalStencilOp failOp,
    PalStencilOp passOp,
    PalStencilOp depthFailOp,
    PalCompareOp compareOp)
{
    CommandBufferVk* vkCmdBuffer = (CommandBufferVk*)cmdBuffer;
    DeviceVk* device = vkCmdBuffer->device;
    VkStencilFaceFlags faceFlags = 0;
    if (faceMask & PAL_STENCIL_FACE_FLAG_BACK) {
        faceFlags |= VK_STENCIL_FACE_BACK_BIT;
    }

    if (faceMask & PAL_STENCIL_FACE_FLAG_FRONT) {
        faceFlags |= VK_STENCIL_FACE_FRONT_BIT;
    }

    VkStencilOp vkFailOp = stencilOpToVk(failOp);
    VkStencilOp vkPassOp = stencilOpToVk(passOp);
    VkStencilOp vkDepthFailOp = stencilOpToVk(depthFailOp);
    VkCompareOp vkCompareOp = compareOpToVk(compareOp);
    
    device->cmdSetStencilOp(
        vkCmdBuffer->handle, 
        faceFlags, 
        failOp, 
        passOp, 
        depthFailOp, 
        compareOp);
}

#endif // PAL_HAS_VULKAN_BACKEND