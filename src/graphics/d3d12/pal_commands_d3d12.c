
/**
    PAL - Prime Abstraction Layer
    Copyright (C) 2025
    Licensed under the Zlib license. See LICENSE file in root.
 */

#if PAL_HAS_D3D12_BACKEND
#include "pal_d3d12.h"

#define align(v, a) (v + a - 1) & ~(a - 1)

static D3D12_RENDER_PASS_FLAGS renderingFlagToD3D12(PalRenderingFlags flags)
{
    D3D12_RENDER_PASS_FLAGS renderingFlags = 0;
    if (flags == PAL_RENDERING_FLAG_NONE) {
        renderingFlags = D3D12_RENDER_PASS_FLAG_NONE;
    }

    if (flags & PAL_RENDERING_FLAG_RESUMING) {
        renderingFlags |= D3D12_RENDER_PASS_FLAG_RESUMING_PASS;
    }

    if (flags & PAL_RENDERING_FLAG_SUSPENDING) {
        renderingFlags |= D3D12_RENDER_PASS_FLAG_SUSPENDING_PASS;
    }

    return renderingFlags;
}

static D3D12_RESOURCE_STATES barrierToD3D12(PalUsageState state)
{
    switch (state) {
        case PAL_USAGE_STATE_UNDEFINED: {
            return D3D12_RESOURCE_STATE_COMMON;
        }

        case PAL_USAGE_STATE_PRESENT: {
            return D3D12_RESOURCE_STATE_PRESENT;
        }

        case PAL_USAGE_STATE_COLOR_ATTACHMENT_WRITE: {
            return D3D12_RESOURCE_STATE_RENDER_TARGET;
        }

        case PAL_USAGE_STATE_DEPTH_ATTACHMENT_READ:
        case PAL_USAGE_STATE_STENCIL_ATTACHMENT_READ: {
            return D3D12_RESOURCE_STATE_DEPTH_READ;
        }

        case PAL_USAGE_STATE_DEPTH_ATTACHMENT_WRITE:
        case PAL_USAGE_STATE_STENCIL_ATTACHMENT_WRITE: {
            return D3D12_RESOURCE_STATE_DEPTH_WRITE;
        }

        case PAL_USAGE_STATE_FRAGMENT_SHADING_RATE_ATTACHMENT_READ: {
            return D3D12_RESOURCE_STATE_SHADING_RATE_SOURCE;
        }

        case PAL_USAGE_STATE_TRANSFER_READ: {
            return D3D12_RESOURCE_STATE_COPY_SOURCE;
        }

        case PAL_USAGE_STATE_TRANSFER_WRITE:
        case PAL_USAGE_STATE_HOST_READ: {
            return D3D12_RESOURCE_STATE_COPY_DEST;
        }

        case PAL_USAGE_STATE_VERTEX_READ: {
            return D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER;
        }

        case PAL_USAGE_STATE_INDEX_READ: {
            return D3D12_RESOURCE_STATE_INDEX_BUFFER;
        }

        case PAL_USAGE_STATE_INDIRECT_READ: {
            return D3D12_RESOURCE_STATE_INDIRECT_ARGUMENT;
        }

        case PAL_USAGE_STATE_UNIFORM_READ: {
            return D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER;
        }

        case PAL_USAGE_STATE_SHADER_READ: {
            return D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE;
        }

        case PAL_USAGE_STATE_STORAGE_READ:
        case PAL_USAGE_STATE_SHADER_WRITE:
        case PAL_USAGE_STATE_STORAGE_WRITE: {
            return D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
        }

        case PAL_USAGE_STATE_HOST_WRITE: {
            return D3D12_RESOURCE_STATE_GENERIC_READ;
        }

        case PAL_USAGE_STATE_ACCELERATION_STRUCTURE_READ:
        case PAL_USAGE_STATE_ACCELERATION_STRUCTURE_WRITE: {
            return D3D12_RESOURCE_STATE_RAYTRACING_ACCELERATION_STRUCTURE;
        }
    }

    return D3D12_RESOURCE_STATE_COMMON;
}

static D3D12_RESOLVE_MODE resolveModeToD3D12(PalResolveMode mode)
{
    switch (mode) {
        case PAL_RESOLVE_MODE_AVERAGE:
            return D3D12_RESOLVE_MODE_AVERAGE;

        case PAL_RESOLVE_MODE_MIN:
            return D3D12_RESOLVE_MODE_MIN;

        case PAL_RESOLVE_MODE_MAX:
            return D3D12_RESOLVE_MODE_MAX;
    }

    return D3D12_RESOLVE_MODE_DECOMPRESS;
}

static void commitShaderbindingTableUpdateD3D12(
    CommandBufferD3D12* cmdBuffer, 
    ShaderBindingTableD3D12* sbt)
{
    if (!sbt->isDirty) {
        return;
    }

    // begin upload buffer copy to gpu buffer
    cmdBuffer->handle->lpVtbl->CopyBufferRegion(
        cmdBuffer->handle,
        sbt->buffer,
        0,
        sbt->stagingBuffer,
        0,
        sbt->stagingBufferSize);

    // put a memory barrier
    D3D12_RESOURCE_BARRIER barrier = {0};
    barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
    barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE;
    barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
    barrier.Transition.pResource = sbt->buffer;

    cmdBuffer->handle->lpVtbl->ResourceBarrier(cmdBuffer->handle, 1, &barrier);
    sbt->isDirty = PAL_FALSE;
}

PalResult PAL_CALL cmdBeginD3D12(
    PalCommandBuffer* cmdBuffer,
    PalRenderingLayoutInfo* info)
{
    HRESULT result;
    CommandBufferD3D12* d3d12CmdBuffer = (CommandBufferD3D12*)cmdBuffer;
    result = d3d12CmdBuffer->allocator->lpVtbl->Reset(d3d12CmdBuffer->allocator);
    if (FAILED(result)) {
        pollMessagesD3D12(d3d12CmdBuffer->device);
        return makeResultD3D12(result);
    }

    result = d3d12CmdBuffer->handle->lpVtbl->Reset(
        d3d12CmdBuffer->handle,
        d3d12CmdBuffer->allocator,
        nullptr);

    if (FAILED(result)) {
        pollMessagesD3D12(d3d12CmdBuffer->device);
        return makeResultD3D12(result);
    }

    d3d12CmdBuffer->linearAllocator.offset = 0;
    return PAL_RESULT_SUCCESS;
}

PalResult PAL_CALL cmdEndD3D12(PalCommandBuffer* cmdBuffer)
{
    CommandBufferD3D12* d3d12CmdBuffer = (CommandBufferD3D12*)cmdBuffer;
    HRESULT result = d3d12CmdBuffer->handle->lpVtbl->Close(d3d12CmdBuffer->handle);
    if (FAILED(result)) {
        pollMessagesD3D12(d3d12CmdBuffer->device);
        return makeResultD3D12(result);
    }

    return PAL_RESULT_SUCCESS;
}

void PAL_CALL cmdExecuteCommandBufferD3D12(
    PalCommandBuffer* primaryCmdBuffer,
    PalCommandBuffer* secondaryCmdBuffer)
{
    CommandBufferD3D12* d3dPrimaryCmdBuffer = (CommandBufferD3D12*)primaryCmdBuffer;
    CommandBufferD3D12* d3dSecondaryCmdBuffer = (CommandBufferD3D12*)secondaryCmdBuffer;

    d3dPrimaryCmdBuffer->handle->lpVtbl->ExecuteBundle(
        d3dPrimaryCmdBuffer->handle,
        (ID3D12GraphicsCommandList*)d3dSecondaryCmdBuffer->handle);
}

void PAL_CALL cmdSetFragmentShadingRateD3D12(
    PalCommandBuffer* cmdBuffer,
    PalFragmentShadingRateState* state)
{
    CommandBufferD3D12* d3d12CmdBuffer = (CommandBufferD3D12*)cmdBuffer;    
    D3D12_SHADING_RATE shadingRate = shadingRateToD3D12(state->rate);
    D3D12_SHADING_RATE_COMBINER combinerOps[2];
    for (int i = 0; i < 2; i++) {
        combinerOps[i] = combinerOpsToD3D12(state->combinerOps[i]);
    }

    d3d12CmdBuffer->handle->lpVtbl->RSSetShadingRate(
        d3d12CmdBuffer->handle,
        shadingRate,
        combinerOps);
}

void PAL_CALL cmdDrawMeshTasksD3D12(
    PalCommandBuffer* cmdBuffer,
    uint32_t groupCountX,
    uint32_t groupCountY,
    uint32_t groupCountZ)
{
    CommandBufferD3D12* d3d12CmdBuffer = (CommandBufferD3D12*)cmdBuffer;
    d3d12CmdBuffer->handle->lpVtbl->DispatchMesh(
        d3d12CmdBuffer->handle,
        groupCountX,
        groupCountY,
        groupCountZ);
}

void PAL_CALL cmdDrawMeshTasksIndirectD3D12(
    PalCommandBuffer* cmdBuffer,
    PalBuffer* buffer,
    uint32_t drawCount)
{
    CommandBufferD3D12* d3d12CmdBuffer = (CommandBufferD3D12*)cmdBuffer;
    DeviceD3D12* device = d3d12CmdBuffer->device;
    BufferD3D12* d3d12Buffer = (BufferD3D12*)buffer;

    d3d12CmdBuffer->handle->lpVtbl->ExecuteIndirect(
        d3d12CmdBuffer->handle,
        device->meshSignature,
        drawCount,
        d3d12Buffer->handle,
        0,
        nullptr,
        0);
}

void PAL_CALL cmdDrawMeshTasksIndirectCountD3D12(
    PalCommandBuffer* cmdBuffer,
    PalBuffer* buffer,
    PalBuffer* countBuffer,
    uint32_t maxDrawCount)
{
    CommandBufferD3D12* d3d12CmdBuffer = (CommandBufferD3D12*)cmdBuffer;
    DeviceD3D12* device = d3d12CmdBuffer->device;
    BufferD3D12* d3d12Buffer = (BufferD3D12*)buffer;
    BufferD3D12* d3d12CountBuffer = (BufferD3D12*)countBuffer;

    d3d12CmdBuffer->handle->lpVtbl->ExecuteIndirect(
        d3d12CmdBuffer->handle,
        device->meshSignature,
        maxDrawCount,
        d3d12Buffer->handle,
        0,
        d3d12CountBuffer->handle,
        0);
}

void PAL_CALL cmdBuildAccelerationStructureD3D12(
    PalCommandBuffer* cmdBuffer,
    PalAccelerationStructureBuildInfo* info)
{
    CommandBufferD3D12* d3d12CmdBuffer = (CommandBufferD3D12*)cmdBuffer;
    D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_DESC buildInfo = {0};
    if (info->type == PAL_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL) {
        D3D12_RAYTRACING_GEOMETRY_DESC* geometries = nullptr;
        geometries = palLinearAlloc(
            &d3d12CmdBuffer->linearAllocator, 
            sizeof(D3D12_RAYTRACING_GEOMETRY_DESC) * info->count, 
            0);

        memset(geometries, 0, sizeof(D3D12_RAYTRACING_GEOMETRY_DESC) * info->count);
        fillBuildInfoD3D12(PAL_FALSE, info, geometries, &buildInfo);
        d3d12CmdBuffer->handle->lpVtbl->BuildRaytracingAccelerationStructure(
            d3d12CmdBuffer->handle,
            &buildInfo,
            0,
            nullptr);

    } else {
        fillBuildInfoD3D12(PAL_FALSE, info, nullptr, &buildInfo);
        d3d12CmdBuffer->handle->lpVtbl->BuildRaytracingAccelerationStructure(
            d3d12CmdBuffer->handle,
            &buildInfo,
            0,
            nullptr);
    }
}

void PAL_CALL cmdBeginRenderingD3D12(
    PalCommandBuffer* cmdBuffer,
    PalRenderingInfo* info)
{
    CommandBufferD3D12* d3d12CmdBuffer = (CommandBufferD3D12*)cmdBuffer;
    D3D12_RENDER_PASS_RENDER_TARGET_DESC colorAttachments[MAX_ATTACHMENTS];
    D3D12_RENDER_PASS_DEPTH_STENCIL_DESC depthStencilAttachment = {0};
    D3D12_CPU_DESCRIPTOR_HANDLE colorCpuHandles[MAX_ATTACHMENTS];
    D3D12_CPU_DESCRIPTOR_HANDLE depthStencilCpuHandle;

    PalAttachmentDesc* desc = nullptr;
    ImageViewD3D12* imageView = nullptr;
    ImageViewD3D12* resolveImageView = nullptr;
    memset(colorAttachments, 0, sizeof(D3D12_RENDER_PASS_RENDER_TARGET_DESC) * MAX_ATTACHMENTS);

    for (int i = 0; i < info->colorAttachentCount; i++) {
        D3D12_RENDER_PASS_RENDER_TARGET_DESC* attachment = &colorAttachments[i];
        desc = &info->colorAttachments[i];
        imageView = (ImageViewD3D12*)desc->imageView;
        resolveImageView = (ImageViewD3D12*)desc->resolveImageView;

        RTVHeapAllocator* allocator = &imageView->device->rtvAllocator;
        uint32_t size = allocator->incrementSize;
        uint64_t base = allocator->baseOffset;
        colorCpuHandles[i].ptr = getDescriptorHandleD3D12(imageView->heapIndex, size, base);

        attachment->BeginningAccess.Clear.ClearValue.Color[0] = desc->clearValue.color[0];
        attachment->BeginningAccess.Clear.ClearValue.Color[1] = desc->clearValue.color[1];
        attachment->BeginningAccess.Clear.ClearValue.Color[2] = desc->clearValue.color[2];
        attachment->BeginningAccess.Clear.ClearValue.Color[3] = desc->clearValue.color[3];

        // load op
        if (desc->loadOp == PAL_LOAD_OP_CLEAR) {
            attachment->BeginningAccess.Type = D3D12_RENDER_PASS_BEGINNING_ACCESS_TYPE_CLEAR;

        } else if (desc->loadOp == PAL_LOAD_OP_LOAD) {
            attachment->BeginningAccess.Type = D3D12_RENDER_PASS_BEGINNING_ACCESS_TYPE_PRESERVE;

        } else if (desc->loadOp == PAL_LOAD_OP_DONT_CARE) {
            attachment->BeginningAccess.Type = D3D12_RENDER_PASS_BEGINNING_ACCESS_TYPE_DISCARD;
        }

        // store op
        if (desc->storeOp == PAL_STORE_OP_STORE) {
            attachment->EndingAccess.Type = D3D12_RENDER_PASS_ENDING_ACCESS_TYPE_PRESERVE;

        } else if (desc->storeOp == PAL_STORE_OP_DONT_CARE) {
            attachment->EndingAccess.Type = D3D12_RENDER_PASS_ENDING_ACCESS_TYPE_DISCARD;
        }

        attachment->cpuDescriptor = colorCpuHandles[i];
        if (resolveImageView) {
            attachment->EndingAccess.Resolve.pDstResource = resolveImageView->image->handle;
            attachment->EndingAccess.Resolve.Format = formatToD3D12(resolveImageView->format);
            attachment->EndingAccess.Resolve.ResolveMode = resolveModeToD3D12(desc->resolveMode);
            attachment->EndingAccess.Type = D3D12_RENDER_PASS_ENDING_ACCESS_TYPE_RESOLVE;
        }
    }

    // depth attachment
    if (info->depthStencilAttachment) {
        desc = info->depthStencilAttachment;
        imageView = (ImageViewD3D12*)desc->imageView;
        resolveImageView = (ImageViewD3D12*)desc->resolveImageView;

        DSVHeapAllocator* allocator = &imageView->device->dsvAllocator;
        uint32_t size = allocator->incrementSize;
        uint64_t base = allocator->baseOffset;
        depthStencilCpuHandle.ptr = getDescriptorHandleD3D12(imageView->heapIndex, size, base);

        D3D12_RENDER_PASS_BEGINNING_ACCESS* depthBegin = nullptr;
        D3D12_RENDER_PASS_BEGINNING_ACCESS* stencilBegin = nullptr;
        D3D12_RENDER_PASS_ENDING_ACCESS* depthEnd = nullptr;
        D3D12_RENDER_PASS_ENDING_ACCESS* stencilEnd = nullptr;

        depthBegin = &depthStencilAttachment.DepthBeginningAccess;
        stencilBegin = &depthStencilAttachment.StencilBeginningAccess;
        depthEnd = &depthStencilAttachment.DepthEndingAccess;
        stencilEnd = &depthStencilAttachment.StencilEndingAccess;

        depthBegin->Clear.ClearValue.DepthStencil.Depth = desc->clearValue.depth;
        depthBegin->Clear.ClearValue.DepthStencil.Stencil = desc->clearValue.stencil;
        stencilBegin->Clear.ClearValue.DepthStencil.Depth = desc->clearValue.depth;
        stencilBegin->Clear.ClearValue.DepthStencil.Stencil = desc->clearValue.stencil;

        // load op
        if (desc->loadOp == PAL_LOAD_OP_CLEAR) {
            depthBegin->Type = D3D12_RENDER_PASS_BEGINNING_ACCESS_TYPE_CLEAR;

        } else if (desc->loadOp == PAL_LOAD_OP_LOAD) {
            depthBegin->Type = D3D12_RENDER_PASS_BEGINNING_ACCESS_TYPE_PRESERVE;

        } else if (desc->loadOp == PAL_LOAD_OP_DONT_CARE) {
            depthBegin->Type = D3D12_RENDER_PASS_BEGINNING_ACCESS_TYPE_DISCARD;
        }

        // store op
        if (desc->storeOp == PAL_STORE_OP_STORE) {
            depthEnd->Type = D3D12_RENDER_PASS_ENDING_ACCESS_TYPE_PRESERVE;

        } else if (desc->storeOp == PAL_STORE_OP_DONT_CARE) {
            depthEnd->Type = D3D12_RENDER_PASS_ENDING_ACCESS_TYPE_DISCARD;
        }

        // stencil load op
        if (desc->loadOp == PAL_LOAD_OP_CLEAR) {
            stencilBegin->Type = D3D12_RENDER_PASS_BEGINNING_ACCESS_TYPE_CLEAR;

        } else if (desc->loadOp == PAL_LOAD_OP_LOAD) {
            stencilBegin->Type = D3D12_RENDER_PASS_BEGINNING_ACCESS_TYPE_PRESERVE;

        } else if (desc->loadOp == PAL_LOAD_OP_DONT_CARE) {
            stencilBegin->Type = D3D12_RENDER_PASS_BEGINNING_ACCESS_TYPE_DISCARD;
        }

        // stencil store op
        if (desc->stencilStoreOp == PAL_STORE_OP_STORE) {
            stencilEnd->Type = D3D12_RENDER_PASS_ENDING_ACCESS_TYPE_PRESERVE;

        } else if (desc->stencilStoreOp == PAL_STORE_OP_DONT_CARE) {
            stencilEnd->Type = D3D12_RENDER_PASS_ENDING_ACCESS_TYPE_DISCARD;
        }

        depthStencilAttachment.cpuDescriptor = depthStencilCpuHandle;
        if (resolveImageView) {
            depthEnd->Resolve.pDstResource = resolveImageView->image->handle;
            depthEnd->Resolve.Format = formatToD3D12(resolveImageView->format);
            depthEnd->Resolve.ResolveMode = resolveModeToD3D12(desc->resolveMode);
            depthEnd->Type = D3D12_RENDER_PASS_ENDING_ACCESS_TYPE_RESOLVE;

            stencilEnd->Resolve.pDstResource = depthEnd->Resolve.pDstResource;
            stencilEnd->Resolve.Format = depthEnd->Resolve.Format;
            stencilEnd->Resolve.ResolveMode = depthEnd->Resolve.ResolveMode;
            stencilEnd->Type = depthEnd->Type;
        }
    }

    D3D12_CPU_DESCRIPTOR_HANDLE* tmpCpuHandle = nullptr;
    D3D12_RENDER_PASS_DEPTH_STENCIL_DESC* tmpDesc = nullptr;
    if (info->depthStencilAttachment) {
        tmpCpuHandle = &depthStencilCpuHandle;
        tmpDesc = &depthStencilAttachment;
    }

    d3d12CmdBuffer->handle->lpVtbl->OMSetRenderTargets(
        d3d12CmdBuffer->handle,
        info->colorAttachentCount,
        colorCpuHandles,
        PAL_FALSE,
        tmpCpuHandle);

    D3D12_RENDER_PASS_FLAGS flags = renderingFlagToD3D12(info->flags);
    d3d12CmdBuffer->handle->lpVtbl->BeginRenderPass(
        d3d12CmdBuffer->handle,
        info->colorAttachentCount,
        colorAttachments,
        tmpDesc,
        flags);
}

void PAL_CALL cmdEndRenderingD3D12(PalCommandBuffer* cmdBuffer)
{
    CommandBufferD3D12* d3d12CmdBuffer = (CommandBufferD3D12*)cmdBuffer;
    d3d12CmdBuffer->handle->lpVtbl->EndRenderPass(d3d12CmdBuffer->handle);
}

void PAL_CALL cmdCopyBufferD3D12(
    PalCommandBuffer* cmdBuffer,
    PalBuffer* dst,
    PalBuffer* src,
    PalBufferCopyInfo* copyInfo)
{
    CommandBufferD3D12* d3d12CmdBuffer = (CommandBufferD3D12*)cmdBuffer;
    BufferD3D12* dstbuffer = (BufferD3D12*)dst;
    BufferD3D12* srcBuffer = (BufferD3D12*)src;
    d3d12CmdBuffer->handle->lpVtbl->CopyBufferRegion(
        d3d12CmdBuffer->handle,
        dstbuffer->handle,
        copyInfo->dstOffset,
        srcBuffer->handle,
        copyInfo->srcOffset,
        copyInfo->size);
}

void PAL_CALL cmdCopyBufferToImageD3D12(
    PalCommandBuffer* cmdBuffer,
    PalImage* dstImage,
    PalBuffer* srcBuffer,
    PalBufferImageCopyInfo* copyInfo)
{
    CommandBufferD3D12* d3d12CmdBuffer = (CommandBufferD3D12*)cmdBuffer;
    ImageD3D12* dst = (ImageD3D12*)dstImage;
    BufferD3D12* src = (BufferD3D12*)srcBuffer;

    D3D12_TEXTURE_COPY_LOCATION dstLocation = {0};
    dstLocation.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
    dstLocation.pResource = dst->handle;

    D3D12_TEXTURE_COPY_LOCATION srcLocation = {0};
    srcLocation.pResource = src->handle;
    srcLocation.Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;
    D3D12_PLACED_SUBRESOURCE_FOOTPRINT* footPrint = &srcLocation.PlacedFootprint;

    footPrint->Offset = copyInfo->bufferOffset;
    footPrint->Footprint.Width = copyInfo->imageWidth;
    footPrint->Footprint.Height = copyInfo->imageHeight;
    footPrint->Footprint.Depth = copyInfo->imageDepth;
    footPrint->Footprint.Format = formatToD3D12(dst->info.format);

    uint32_t imageFormatSize = getFormatSizeD3D12(dst->info.format);
    uint64_t rowPitch = align((uint64_t)copyInfo->imageWidth * imageFormatSize, TEXTURE_PITCH);
    footPrint->Footprint.RowPitch = (UINT)rowPitch;

    uint32_t planeCount = 1;
    if (copyInfo->imageAspect == PAL_IMAGE_ASPECT_DEPTH_STENCIL) {
        planeCount = 2;
    }

    D3D12_BOX box = {0};
    box.right = copyInfo->imageWidth;
    box.bottom = copyInfo->imageHeight;
    box.back = copyInfo->imageDepth;

    uint32_t level = copyInfo->ImageMipLevel;
    uint32_t startLayer = copyInfo->ImageStartArrayLayer;
    uint32_t layerCount = copyInfo->ImageArrayLayerCount;
    uint32_t maxLayers = dst->info.arrayLayerCount;
    uint32_t maxLevels = dst->info.mipLevelCount;

    for (uint32_t plane = 0; plane < planeCount; plane++) {
        for (uint32_t layer = startLayer; layer < startLayer + layerCount; layer++) {
            uint32_t index = level + (layer * maxLevels) + (plane * maxLevels * maxLayers);

            dstLocation.SubresourceIndex = index;
            d3d12CmdBuffer->handle->lpVtbl->CopyTextureRegion(
                d3d12CmdBuffer->handle,
                &dstLocation,
                copyInfo->imageOffsetX,
                copyInfo->imageOffsetY,
                copyInfo->imageOffsetZ,
                &srcLocation,
                &box);
        }
    }
}

void PAL_CALL cmdCopyImageD3D12(
    PalCommandBuffer* cmdBuffer,
    PalImage* dst,
    PalImage* src,
    PalImageCopyInfo* copyInfo)
{
    CommandBufferD3D12* d3d12CmdBuffer = (CommandBufferD3D12*)cmdBuffer;
    ImageD3D12* dstImage = (ImageD3D12*)dst;
    ImageD3D12* srcImage = (ImageD3D12*)src;

    D3D12_TEXTURE_COPY_LOCATION dstLocation = {0};
    dstLocation.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
    dstLocation.pResource = dstImage->handle;

    D3D12_TEXTURE_COPY_LOCATION srcLocation = {0};
    srcLocation.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
    srcLocation.pResource = srcImage->handle;

    D3D12_BOX box = {0};
    box.left = copyInfo->srcOffsetX;
    box.top = copyInfo->srcOffsetY;
    box.front = copyInfo->srcOffsetZ;
    box.right = copyInfo->srcOffsetX + copyInfo->width;
    box.bottom = copyInfo->srcOffsetY + copyInfo->height;
    box.back = copyInfo->srcOffsetZ + copyInfo->depth;

    uint32_t planeCount = 1;
    uint32_t layerCount = copyInfo->arrayLayerCount;
    if (copyInfo->aspect == PAL_IMAGE_ASPECT_DEPTH_STENCIL) {
        planeCount = 2;
    }

    uint32_t dstLevel = copyInfo->dstMipLevel;
    uint32_t dstStartLayer = copyInfo->dstStartArrayLayer;
    uint32_t dstMaxLayers = dstImage->info.arrayLayerCount;
    uint32_t dstMaxLevels = dstImage->info.mipLevelCount;

    uint32_t srcLevel = copyInfo->srcMipLevel;
    uint32_t srcStartLayer = copyInfo->srcStartArrayLayer;
    uint32_t srcMaxLayers = srcImage->info.arrayLayerCount;
    uint32_t srcMaxLevels = srcImage->info.mipLevelCount;

    for (uint32_t plane = 0; plane < planeCount; plane++) {
        for (uint32_t layer = 0; layer + layerCount; layer++) {
            // clang-format off
            uint32_t dstIndex = dstLevel + (dstStartLayer + layer * dstMaxLevels) + (plane * dstMaxLevels * dstMaxLayers);
            uint32_t srcIndex = srcLevel + (srcStartLayer + layer * srcMaxLevels) + (plane * srcMaxLevels * srcMaxLayers);
            // clang-format on

            dstLocation.SubresourceIndex = dstIndex;
            srcLocation.SubresourceIndex = srcIndex;

            d3d12CmdBuffer->handle->lpVtbl->CopyTextureRegion(
                d3d12CmdBuffer->handle,
                &dstLocation,
                copyInfo->dstOffsetX,
                copyInfo->dstOffsetY,
                copyInfo->dstOffsetZ,
                &srcLocation,
                &box);
        }
    }
}

void PAL_CALL cmdCopyImageToBufferD3D12(
    PalCommandBuffer* cmdBuffer,
    PalBuffer* dstBuffer,
    PalImage* srcImage,
    PalBufferImageCopyInfo* copyInfo)
{
    CommandBufferD3D12* d3d12CmdBuffer = (CommandBufferD3D12*)cmdBuffer;
    BufferD3D12* dst = (BufferD3D12*)dstBuffer;
    ImageD3D12* src = (ImageD3D12*)srcImage;

    D3D12_TEXTURE_COPY_LOCATION dstLocation = {0};
    dstLocation.pResource = dst->handle;
    dstLocation.Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;
    D3D12_PLACED_SUBRESOURCE_FOOTPRINT* footPrint = &dstLocation.PlacedFootprint;
    footPrint->Offset = copyInfo->bufferOffset;
    footPrint->Footprint.Width = copyInfo->imageWidth;
    footPrint->Footprint.Height = copyInfo->imageHeight;
    footPrint->Footprint.Depth = copyInfo->imageDepth;
    footPrint->Footprint.Format = formatToD3D12(src->info.format);

    D3D12_TEXTURE_COPY_LOCATION srcLocation = {0};
    srcLocation.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
    srcLocation.pResource = src->handle;

    uint32_t imageFormatSize = getFormatSizeD3D12(src->info.format);
    uint64_t rowPitch = align((uint64_t)copyInfo->imageWidth * imageFormatSize, TEXTURE_PITCH);
    footPrint->Footprint.RowPitch = (UINT)rowPitch;

    D3D12_BOX box = {0};
    box.left = copyInfo->imageOffsetX;
    box.top = copyInfo->imageOffsetY;
    box.front = copyInfo->imageOffsetZ;
    box.right = copyInfo->imageOffsetX + copyInfo->imageWidth;
    box.bottom = copyInfo->imageOffsetY + copyInfo->imageHeight;
    box.back = copyInfo->imageOffsetX + copyInfo->imageDepth;

    uint32_t planeCount = 1;
    if (copyInfo->imageAspect == PAL_IMAGE_ASPECT_DEPTH_STENCIL) {
        planeCount = 2;
    }

    uint32_t level = copyInfo->ImageMipLevel;
    uint32_t startLayer = copyInfo->ImageStartArrayLayer;
    uint32_t layerCount = copyInfo->ImageArrayLayerCount;
    uint32_t maxLayers = src->info.arrayLayerCount;
    uint32_t maxLevels = src->info.mipLevelCount;

    for (uint32_t plane = 0; plane < planeCount; plane++) {
        for (uint32_t layer = startLayer; layer < startLayer + layerCount; layer++) {
            uint32_t index = level + (layer * maxLevels) + (plane * maxLevels * maxLayers);

            srcLocation.SubresourceIndex = index;
            d3d12CmdBuffer->handle->lpVtbl->CopyTextureRegion(
                d3d12CmdBuffer->handle,
                &dstLocation,
                0,
                0,
                0,
                &srcLocation,
                &box);
        }
    }
}

void PAL_CALL cmdBindPipelineD3D12(
    PalCommandBuffer* cmdBuffer,
    PalPipeline* pipeline)
{
    CommandBufferD3D12* d3d12CmdBuffer = (CommandBufferD3D12*)cmdBuffer;
    PipelineD3D12* d3dPipeline = (PipelineD3D12*)pipeline;
    if (d3dPipeline->type == RAY_TRACING_PIPELINE) {
        d3d12CmdBuffer->handle->lpVtbl->SetPipelineState1(
            d3d12CmdBuffer->handle,
            d3dPipeline->handle);

        d3d12CmdBuffer->handle->lpVtbl->SetComputeRootSignature(
            d3d12CmdBuffer->handle, 
            d3dPipeline->layout->handle);

    } else {
        d3d12CmdBuffer->handle->lpVtbl->SetPipelineState(
            d3d12CmdBuffer->handle,
            d3dPipeline->handle);

        d3d12CmdBuffer->handle->lpVtbl->SetComputeRootSignature(
            d3d12CmdBuffer->handle, 
            d3dPipeline->layout->handle);

        if (d3dPipeline->type == GRAPHICS_PIPELINE) {
            d3d12CmdBuffer->handle->lpVtbl->IASetPrimitiveTopology(
                d3d12CmdBuffer->handle,
                d3dPipeline->topology);

            d3d12CmdBuffer->handle->lpVtbl->SetGraphicsRootSignature(
                d3d12CmdBuffer->handle, 
                d3dPipeline->layout->handle);

            if (d3dPipeline->hasFsr) {
                d3d12CmdBuffer->handle->lpVtbl->RSSetShadingRate(
                    d3d12CmdBuffer->handle,
                    d3dPipeline->shadingRate,
                    d3dPipeline->combinerOps);
            }
        }
    }

    d3d12CmdBuffer->pipeline = d3dPipeline;
}

void PAL_CALL cmdSetViewportD3D12(
    PalCommandBuffer* cmdBuffer,
    uint32_t count,
    PalViewport* viewports)
{
    CommandBufferD3D12* d3d12CmdBuffer = (CommandBufferD3D12*)cmdBuffer;
    D3D12_VIEWPORT d3dViewports[D3D12_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE];
    for (int i = 0; i < count; i++) {
        D3D12_VIEWPORT* tmp = &d3dViewports[i];
        tmp->TopLeftX = viewports[i].x;
        tmp->TopLeftY = viewports[i].y;
        tmp->Width = viewports[i].width;
        tmp->Height = viewports[i].height;
        tmp->MinDepth = viewports[i].minDepth;
        tmp->MaxDepth = viewports[i].maxDepth;
    }

    d3d12CmdBuffer->handle->lpVtbl->RSSetViewports(d3d12CmdBuffer->handle, count, d3dViewports);
}

void PAL_CALL cmdSetScissorsD3D12(
    PalCommandBuffer* cmdBuffer,
    uint32_t count,
    PalRect2D* scissors)
{
    CommandBufferD3D12* d3d12CmdBuffer = (CommandBufferD3D12*)cmdBuffer;
    D3D12_RECT d3dScissors[D3D12_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE];
    for (int i = 0; i < count; i++) {
        D3D12_RECT* tmp = &d3dScissors[i];
        tmp->left = scissors[i].x;
        tmp->top = scissors[i].y;
        tmp->right = scissors[i].width;
        tmp->bottom = scissors[i].height;
    }

    d3d12CmdBuffer->handle->lpVtbl->RSSetScissorRects(d3d12CmdBuffer->handle, count, d3dScissors);
}

void PAL_CALL cmdBindVertexBuffersD3D12(
    PalCommandBuffer* cmdBuffer,
    uint32_t firstSlot,
    uint32_t count,
    PalBuffer** buffers,
    uint64_t* offsets)
{
    CommandBufferD3D12* d3d12CmdBuffer = (CommandBufferD3D12*)cmdBuffer;
    PipelineD3D12* pipeline = d3d12CmdBuffer->pipeline;
    D3D12_VERTEX_BUFFER_VIEW* views = nullptr;
    views = palLinearAlloc(
        &d3d12CmdBuffer->linearAllocator, 
        sizeof(D3D12_VERTEX_BUFFER_VIEW) * count, 
        0);

    for (int i = 0; i < count; i++) {
        BufferD3D12* tmp = (BufferD3D12*)buffers[i];
        views[i].BufferLocation = tmp->handle->lpVtbl->GetGPUVirtualAddress(tmp->handle);
        views[i].BufferLocation = views[i].BufferLocation + offsets[i];
        views[i].SizeInBytes = (UINT)tmp->size;
        views[i].StrideInBytes = pipeline->strides[i];
    }

    d3d12CmdBuffer->handle->lpVtbl->IASetVertexBuffers(
        d3d12CmdBuffer->handle,
        firstSlot,
        count,
        views);
}

void PAL_CALL cmdBindIndexBufferD3D12(
    PalCommandBuffer* cmdBuffer,
    PalBuffer* buffer,
    uint64_t offset,
    PalIndexType type)
{
    CommandBufferD3D12* d3d12CmdBuffer = (CommandBufferD3D12*)cmdBuffer;
    D3D12_INDEX_BUFFER_VIEW view = {0};
    BufferD3D12* indexBuffer = (BufferD3D12*)buffer;

    view.BufferLocation = indexBuffer->handle->lpVtbl->GetGPUVirtualAddress(indexBuffer->handle);
    view.BufferLocation = view.BufferLocation + offset;
    view.SizeInBytes = (UINT)indexBuffer->size;
    if (type == PAL_INDEX_TYPE_UINT16) {
        view.Format = DXGI_FORMAT_R16_UINT;
    } else {
        view.Format = DXGI_FORMAT_R32_UINT;
    }

    d3d12CmdBuffer->handle->lpVtbl->IASetIndexBuffer(d3d12CmdBuffer->handle, &view);
}

void PAL_CALL cmdDrawD3D12(
    PalCommandBuffer* cmdBuffer,
    uint32_t vertexCount,
    uint32_t instanceCount,
    uint32_t firstVertex,
    uint32_t firstInstance)
{
    CommandBufferD3D12* d3d12CmdBuffer = (CommandBufferD3D12*)cmdBuffer;
    d3d12CmdBuffer->handle->lpVtbl->DrawInstanced(
        d3d12CmdBuffer->handle,
        vertexCount,
        instanceCount,
        firstVertex,
        firstInstance);
}

void PAL_CALL cmdDrawIndirectD3D12(
    PalCommandBuffer* cmdBuffer,
    PalBuffer* buffer,
    uint32_t count)
{
    CommandBufferD3D12* d3d12CmdBuffer = (CommandBufferD3D12*)cmdBuffer;
    DeviceD3D12* device = d3d12CmdBuffer->device;
    BufferD3D12* d3d12Buffer = (BufferD3D12*)buffer;

    d3d12CmdBuffer->handle->lpVtbl->ExecuteIndirect(
        d3d12CmdBuffer->handle,
        device->drawSignature,
        count,
        d3d12Buffer->handle,
        0,
        nullptr,
        0);
}

void PAL_CALL cmdDrawIndirectCountD3D12(
    PalCommandBuffer* cmdBuffer,
    PalBuffer* buffer,
    PalBuffer* countBuffer,
    uint32_t maxDrawCount)
{
    CommandBufferD3D12* d3d12CmdBuffer = (CommandBufferD3D12*)cmdBuffer;
    DeviceD3D12* device = d3d12CmdBuffer->device;
    BufferD3D12* d3d12Buffer = (BufferD3D12*)buffer;
    BufferD3D12* d3d12CountBuffer = (BufferD3D12*)countBuffer;

    d3d12CmdBuffer->handle->lpVtbl->ExecuteIndirect(
        d3d12CmdBuffer->handle,
        device->drawSignature,
        maxDrawCount,
        d3d12Buffer->handle,
        0,
        d3d12CountBuffer->handle,
        0);
}

void PAL_CALL cmdDrawIndexedD3D12(
    PalCommandBuffer* cmdBuffer,
    uint32_t indexCount,
    uint32_t instanceCount,
    uint32_t firstIndex,
    int32_t vertexOffset,
    uint32_t firstInstance)
{
    CommandBufferD3D12* d3d12CmdBuffer = (CommandBufferD3D12*)cmdBuffer;
    d3d12CmdBuffer->handle->lpVtbl->DrawIndexedInstanced(
        d3d12CmdBuffer->handle,
        indexCount,
        instanceCount,
        firstIndex,
        vertexOffset,
        firstInstance);
}

void PAL_CALL cmdDrawIndexedIndirectD3D12(
    PalCommandBuffer* cmdBuffer,
    PalBuffer* buffer,
    uint32_t count)
{
    CommandBufferD3D12* d3d12CmdBuffer = (CommandBufferD3D12*)cmdBuffer;
    DeviceD3D12* device = d3d12CmdBuffer->device;
    BufferD3D12* d3d12Buffer = (BufferD3D12*)buffer;

    d3d12CmdBuffer->handle->lpVtbl->ExecuteIndirect(
        d3d12CmdBuffer->handle,
        device->drawIndexedSignature,
        count,
        d3d12Buffer->handle,
        0,
        nullptr,
        0);
}

void PAL_CALL cmdDrawIndexedIndirectCountD3D12(
    PalCommandBuffer* cmdBuffer,
    PalBuffer* buffer,
    PalBuffer* countBuffer,
    uint32_t maxDrawCount)
{
    CommandBufferD3D12* d3d12CmdBuffer = (CommandBufferD3D12*)cmdBuffer;
    DeviceD3D12* device = d3d12CmdBuffer->device;
    BufferD3D12* d3d12Buffer = (BufferD3D12*)buffer;
    BufferD3D12* d3d12CountBuffer = (BufferD3D12*)countBuffer;

    d3d12CmdBuffer->handle->lpVtbl->ExecuteIndirect(
        d3d12CmdBuffer->handle,
        device->drawIndexedSignature,
        maxDrawCount,
        d3d12Buffer->handle,
        0,
        d3d12CountBuffer->handle,
        0);
}

void PAL_CALL cmdAccelerationStructureBarrierD3D12(
    PalCommandBuffer* cmdBuffer,
    PalAccelerationStructure* as,
    PalBarrierInfo* info)
{
    CommandBufferD3D12* d3d12CmdBuffer = (CommandBufferD3D12*)cmdBuffer;
    AccelerationStructureD3D12* d3dAs = (AccelerationStructureD3D12*)as;
    D3D12_RESOURCE_BARRIER barrier = {0};
    barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_UAV;
    barrier.UAV.pResource = d3dAs->handle;
    d3d12CmdBuffer->handle->lpVtbl->ResourceBarrier(d3d12CmdBuffer->handle, 1, &barrier);
}

void PAL_CALL cmdImageBarrierD3D12(
    PalCommandBuffer* cmdBuffer,
    PalImage* image,
    PalImageSubresourceRange* subresourceRange,
    PalBarrierInfo* info)
{
    CommandBufferD3D12* d3d12CmdBuffer = (CommandBufferD3D12*)cmdBuffer;
    ImageD3D12* d3d12Image = (ImageD3D12*)image;
    D3D12_RESOURCE_STATES old, new;
    D3D12_RESOURCE_BARRIER barrier = {0};

    old = barrierToD3D12(info->oldState);
    new = barrierToD3D12(info->newState);

    // read/write barrier without transition
    if (old == new && old == D3D12_RESOURCE_STATE_UNORDERED_ACCESS) {
        barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_UAV;
        barrier.UAV.pResource = d3d12Image->handle;
        d3d12CmdBuffer->handle->lpVtbl->ResourceBarrier(d3d12CmdBuffer->handle, 1, &barrier);
        return;
    }

    uint32_t planeCount = 1; // for color or depth
    if (subresourceRange->aspect == PAL_IMAGE_ASPECT_DEPTH_STENCIL) {
        planeCount = 2;
    }

    D3D12_RESOURCE_BARRIER* barriers = nullptr;
    uint32_t levelCount = subresourceRange->mipLevelCount;
    uint32_t layerCount = subresourceRange->layerArrayCount;

    uint32_t startLevel = subresourceRange->startMipLevel;
    uint32_t startLayer = subresourceRange->startArrayLayer;
    uint32_t maxLevels = d3d12Image->info.mipLevelCount;
    uint32_t maxLayers = d3d12Image->info.arrayLayerCount;
    uint32_t barrierCount = layerCount * levelCount * planeCount;

    if (startLevel == 0 && levelCount == maxLevels && startLayer == 0 && layerCount == maxLayers) {
        // full resource
        barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
        barrier.Transition.pResource = d3d12Image->handle;
        barrier.Transition.StateBefore = old;
        barrier.Transition.StateAfter = new;
        barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
        d3d12CmdBuffer->handle->lpVtbl->ResourceBarrier(d3d12CmdBuffer->handle, 1, &barrier);
        return;
    }

    if (layerCount == 1 && layerCount == 1 && planeCount == 1) {
        // single plane
        barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
        barrier.Transition.pResource = d3d12Image->handle;
        barrier.Transition.StateBefore = old;
        barrier.Transition.StateAfter = new;
        barrier.Transition.Subresource = startLevel + startLayer * maxLevels;
        d3d12CmdBuffer->handle->lpVtbl->ResourceBarrier(d3d12CmdBuffer->handle, 1, &barrier);
        return;
    }

    barriers = palLinearAlloc(
        &d3d12CmdBuffer->linearAllocator, 
        sizeof(D3D12_RESOURCE_BARRIER) * barrierCount, 
        0);

    uint32_t count = 0;
    for (uint32_t plane = 0; plane < planeCount; plane++) {
        for (uint32_t layer = startLayer; layer < startLayer + layerCount; layer++) {
            for (uint32_t level = startLevel; level < startLevel + levelCount; level++) {
                uint32_t index = level + (layer * maxLevels) + (plane * maxLevels * maxLayers);

                D3D12_RESOURCE_BARRIER* tmp = &barriers[count++];
                tmp->Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
                tmp->Transition.pResource = d3d12Image->handle;
                tmp->Transition.StateBefore = old;
                tmp->Transition.StateAfter = new;
                tmp->Transition.Subresource = index;
            }
        }
    }

    d3d12CmdBuffer->handle->lpVtbl->ResourceBarrier(d3d12CmdBuffer->handle, barrierCount, barriers);
}

void PAL_CALL cmdBufferBarrierD3D12(
    PalCommandBuffer* cmdBuffer,
    PalBuffer* buffer,
    PalBarrierInfo* info)
{
    CommandBufferD3D12* d3d12CmdBuffer = (CommandBufferD3D12*)cmdBuffer;
    BufferD3D12* d3d12Buffer = (BufferD3D12*)buffer;
    D3D12_RESOURCE_STATES old, new;
    if (!d3d12Buffer->canStateChange) {
        return;
    }

    old = barrierToD3D12(info->oldState);
    new = barrierToD3D12(info->newState);

    D3D12_RESOURCE_BARRIER barrier = {0};
    if (old == new && D3D12_RESOURCE_STATE_UNORDERED_ACCESS) {
        barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_UAV;
        barrier.UAV.pResource = d3d12Buffer->handle;
    } else {
        barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
        barrier.Transition.pResource = d3d12Buffer->handle;
        barrier.Transition.StateBefore = old;
        barrier.Transition.StateAfter = new;
    }

    d3d12CmdBuffer->handle->lpVtbl->ResourceBarrier(d3d12CmdBuffer->handle, 1, &barrier);
}

void PAL_CALL cmdDispatchD3D12(
    PalCommandBuffer* cmdBuffer,
    uint32_t groupCountX,
    uint32_t groupCountY,
    uint32_t groupCountZ)
{
    CommandBufferD3D12* d3d12CmdBuffer = (CommandBufferD3D12*)cmdBuffer;
    d3d12CmdBuffer->handle->lpVtbl->Dispatch(
        d3d12CmdBuffer->handle,
        groupCountX,
        groupCountY,
        groupCountZ);
}

void PAL_CALL cmdDispatchIndirectD3D12(
    PalCommandBuffer* cmdBuffer,
    PalBuffer* buffer)
{
    CommandBufferD3D12* d3d12CmdBuffer = (CommandBufferD3D12*)cmdBuffer;
    BufferD3D12* d3d12Buffer = (BufferD3D12*)buffer;
    DeviceD3D12* device = d3d12CmdBuffer->device;

    d3d12CmdBuffer->handle->lpVtbl->ExecuteIndirect(
        d3d12CmdBuffer->handle,
        device->dispatchSignature,
        1, // one dispatch
        d3d12Buffer->handle,
        0,
        nullptr,
        0);
}

void PAL_CALL cmdTraceRaysD3D12(
    PalCommandBuffer* cmdBuffer,
    PalShaderBindingTable* sbt,
    uint32_t raygenIndex,
    uint32_t width,
    uint32_t height,
    uint32_t depth)
{
    CommandBufferD3D12* d3d12CmdBuffer = (CommandBufferD3D12*)cmdBuffer;
    ShaderBindingTableD3D12* d3d12Sbt = (ShaderBindingTableD3D12*)sbt;
    uint64_t stride = d3d12Sbt->raygen.region.StrideInBytes;
    D3D12_GPU_VIRTUAL_ADDRESS_RANGE raygenAddress = {0};
    raygenAddress.SizeInBytes = d3d12Sbt->raygen.region.SizeInBytes;
    raygenAddress.StartAddress = d3d12Sbt->baseAddress + raygenIndex * stride;

    // we need to make sure the SBT is up to date
    commitShaderbindingTableUpdateD3D12(d3d12CmdBuffer, d3d12Sbt);

    D3D12_DISPATCH_RAYS_DESC desc = {0};
    desc.Width = width;
    desc.Height = height;
    desc.Depth = depth;
    desc.RayGenerationShaderRecord = raygenAddress;
    desc.HitGroupTable = d3d12Sbt->hit.region;
    desc.MissShaderTable = d3d12Sbt->miss.region;
    desc.CallableShaderTable = d3d12Sbt->callable.region;
    d3d12CmdBuffer->handle->lpVtbl->DispatchRays(d3d12CmdBuffer->handle, &desc);
}

void PAL_CALL cmdTraceRaysIndirectD3D12(
    PalCommandBuffer* cmdBuffer,
    uint32_t raygenIndex,
    PalShaderBindingTable* sbt,
    PalBuffer* buffer)
{
    HRESULT result;
    CommandBufferD3D12* d3d12CmdBuffer = (CommandBufferD3D12*)cmdBuffer;
    DeviceD3D12* device = d3d12CmdBuffer->device;
    ShaderBindingTableD3D12* d3d12Sbt = (ShaderBindingTableD3D12*)sbt;
    BufferD3D12* d3d12Buffer = (BufferD3D12*)buffer;
    D3D12_DISPATCH_RAYS_DESC desc = {0};

    // we need to make sure the SBT is up to date
    commitShaderbindingTableUpdateD3D12(d3d12CmdBuffer, d3d12Sbt);

    // BufferD3D12 is mappable
    void* ptr = nullptr;
    result = d3d12Buffer->handle->lpVtbl->Map(d3d12Buffer->handle, 0, nullptr, &ptr);
    if (FAILED(result)) {
        return;
    }

    // copy indirect parameters from the buffer
    PalDispatchIndirectData data = {0};
    memcpy(&data, ptr, sizeof(PalDispatchIndirectData));
    d3d12Buffer->handle->lpVtbl->Unmap(d3d12Buffer->handle, 0, nullptr);

    desc.Width = data.groupCountXOrWidth;
    desc.Height = data.groupCountXOrHeight;
    desc.Depth = data.groupCountXOrDepth;

    uint64_t stride = d3d12Sbt->raygen.region.StrideInBytes;
    D3D12_GPU_VIRTUAL_ADDRESS_RANGE raygenAddress = {0};
    raygenAddress.SizeInBytes = d3d12Sbt->raygen.region.SizeInBytes;
    raygenAddress.StartAddress = d3d12Sbt->baseAddress + raygenIndex * stride;

    desc.RayGenerationShaderRecord = raygenAddress;
    desc.HitGroupTable = d3d12Sbt->hit.region;
    desc.MissShaderTable = d3d12Sbt->miss.region;
    desc.CallableShaderTable = d3d12Sbt->callable.region;

    // fill the data into the tmp upload buffer of the command buffer
    ptr = nullptr;
    d3d12CmdBuffer->stagingBuffer->lpVtbl->Map(d3d12CmdBuffer->stagingBuffer, 0, nullptr, &ptr);
    memcpy(ptr, &desc, sizeof(D3D12_DISPATCH_RAYS_DESC));
    d3d12CmdBuffer->stagingBuffer->lpVtbl->Unmap(d3d12CmdBuffer->stagingBuffer, 0, nullptr);

    // copy to the gpu tmp buffer of the command buffer and execute with that
    d3d12CmdBuffer->handle->lpVtbl->CopyBufferRegion(
        d3d12CmdBuffer->handle,
        d3d12CmdBuffer->buffer,
        0,
        d3d12CmdBuffer->stagingBuffer,
        0,
        sizeof(D3D12_DISPATCH_RAYS_DESC));

    // put a memory barrier
    D3D12_RESOURCE_BARRIER barrier = {0};
    barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
    barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE;
    barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
    barrier.Transition.pResource = d3d12CmdBuffer->buffer;
    d3d12CmdBuffer->handle->lpVtbl->ResourceBarrier(d3d12CmdBuffer->handle, 1, &barrier);

    d3d12CmdBuffer->handle->lpVtbl->ExecuteIndirect(
        d3d12CmdBuffer->handle,
        device->raySignature,
        1,
        d3d12CmdBuffer->buffer,
        0,
        nullptr,
        0);
}

void PAL_CALL cmdBindDescriptorSetD3D12(
    PalCommandBuffer* cmdBuffer,
    uint32_t setIndex,
    PalDescriptorSet* set)
{
    CommandBufferD3D12* d3d12CmdBuffer = (CommandBufferD3D12*)cmdBuffer;
    PipelineD3D12* pipeline = d3d12CmdBuffer->pipeline;
    DescriptorSetD3D12* d3dSet = (DescriptorSetD3D12*)set;
    DescriptorPoolD3D12* pool = d3dSet->pool;

    // bind heaps
    uint32_t heapCount = 0;
    ID3D12DescriptorHeap* heaps[2];
    if (pool->hasResourceHeap) {
        heaps[heapCount++] = pool->resourceHeap.handle;
    }

    if (pool->hasSamplerHeap) {
        heaps[heapCount++] = pool->samplerHeap.handle;
    }
    d3d12CmdBuffer->handle->lpVtbl->SetDescriptorHeaps(d3d12CmdBuffer->handle, heapCount, heaps);

    // bind resource descriptor table
    uint32_t resourceCount = d3dSet->layout->bindingCount - d3dSet->layout->samplerCount;
    uint32_t baseIndex = setIndex;
    if (pipeline->layout->constantIndex != UINT32_MAX) {
        // If push constant was used to create the pipeline layout
        // slot 0 will be reserve for it
        baseIndex++;
    }

    if (resourceCount) {
        D3D12_GPU_DESCRIPTOR_HANDLE base;
        base.ptr = getDescriptorHandleD3D12(
            d3dSet->resourceOffset,
            pool->resourceHeap.incrementSize,
            pool->resourceHeap.gpuBase);

        if (pipeline->type == GRAPHICS_PIPELINE) {
            d3d12CmdBuffer->handle->lpVtbl->SetGraphicsRootDescriptorTable(
                d3d12CmdBuffer->handle,
                baseIndex, // base set index is resource first before sampler
                base);

        } else {
            // ray tracing uses the compute path
            d3d12CmdBuffer->handle->lpVtbl->SetComputeRootDescriptorTable(
                d3d12CmdBuffer->handle,
                baseIndex, // base set index is resource first before sampler
                base);
        }

        baseIndex++;
    }

    // bind sampler descriptor table
    if (d3dSet->layout->samplerCount) {
        D3D12_GPU_DESCRIPTOR_HANDLE base;
        base.ptr = getDescriptorHandleD3D12(
            d3dSet->samplerOffset,
            pool->samplerHeap.incrementSize,
            pool->samplerHeap.gpuBase);

        if (pipeline->type == GRAPHICS_PIPELINE) {
            d3d12CmdBuffer->handle->lpVtbl->SetGraphicsRootDescriptorTable(
                d3d12CmdBuffer->handle,
                baseIndex,
                base);

        } else {
            // ray tracing uses the compute path
            d3d12CmdBuffer->handle->lpVtbl->SetComputeRootDescriptorTable(
                d3d12CmdBuffer->handle,
                baseIndex,
                base);
        }
    }
}

void PAL_CALL cmdPushConstantsD3D12(
    PalCommandBuffer* cmdBuffer,
    uint32_t offset,
    uint32_t size,
    const void* value)
{
    CommandBufferD3D12* d3d12CmdBuffer = (CommandBufferD3D12*)cmdBuffer;
    PipelineD3D12* pipeline = d3d12CmdBuffer->pipeline;
    if (pipeline->layout->constantIndex != UINT32_MAX) {
        if (pipeline->type == GRAPHICS_PIPELINE) {
            d3d12CmdBuffer->handle->lpVtbl->SetGraphicsRoot32BitConstants(
                d3d12CmdBuffer->handle,
                pipeline->layout->constantIndex,
                size / 4,
                value,
                offset / 4);

        } else {
            // ray tracing uses the compute path
            d3d12CmdBuffer->handle->lpVtbl->SetComputeRoot32BitConstants(
                d3d12CmdBuffer->handle,
                pipeline->layout->constantIndex,
                size / 4,
                value,
                offset / 4);
        }
    }
}

#endif // PAL_HAS_D3D12_BACKEND