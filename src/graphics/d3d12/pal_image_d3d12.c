
/**
    PAL - Prime Abstraction Layer
    Copyright (C) 2025
    Licensed under the Zlib license. See LICENSE file in root.
 */

#if PAL_HAS_D3D12_BACKEND
#include "pal_d3d12.h"

PalResult PAL_CALL createImageD3D12(
    PalDevice* device,
    const PalImageCreateInfo* info,
    PalImage** outImage)
{
    HRESULT result;
    Image* image = nullptr;
    Device* d3d12Device = (Device*)device;

    image = palAllocate(s_D3D12.allocator, sizeof(Image), 0);
    if (!image) {
        return PAL_RESULT_OUT_OF_MEMORY;
    }

    memset(image, 0, sizeof(Image));
    image->desc.Width = (UINT64)info->width;
    image->desc.Height = (UINT64)info->height;
    image->desc.DepthOrArraySize = (UINT16)info->depthOrArraySize;
    image->desc.MipLevels = (UINT16)info->mipLevelCount;
    image->desc.Format = formatToD3D12(info->format);
    image->desc.SampleDesc.Count = samplesToD3D12(info->sampleCount);
    image->desc.SampleDesc.Quality = 0;

    image->desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
    if (info->type == PAL_IMAGE_TYPE_3D) {
        image->desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;

    } else if (info->type == PAL_IMAGE_TYPE_1D) {
        image->desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE1D;
    }

    if (info->usages & PAL_IMAGE_USAGE_COLOR_ATTACHEMENT) {
        image->desc.Flags |= D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;
    }

    if (info->usages & PAL_IMAGE_USAGE_DEPTH_ATTACHEMENT) {
        image->desc.Flags |= D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;
    }

    if (info->usages & PAL_IMAGE_USAGE_STORAGE) {
        image->desc.Flags |= D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
    }

    image->belongsToSwapchain = PAL_FALSE;
    image->info.depthOrArraySize = info->depthOrArraySize;
    image->info.type = info->type;
    image->info.format = info->format;
    image->info.usages = info->usages;
    image->info.height = info->height;
    image->info.mipLevelCount = info->mipLevelCount;
    image->info.sampleCount = info->sampleCount;
    image->info.width = info->width;

    image->device = d3d12Device;
    *outImage = (PalImage*)image;
    return PAL_RESULT_SUCCESS;
}

void PAL_CALL destroyImageD3D12(PalImage* image)
{
    Image* d3dImage = (Image*)image;
    // check if memory has been attached to the image
    if (d3dImage->handle) {
        d3dImage->handle->lpVtbl->Release(d3dImage->handle);
    }
    palFree(s_D3D12.allocator, d3dImage);
}

PalResult PAL_CALL getImageInfoD3D12(
    PalImage* image,
    PalImageInfo* info)
{
    Image* d3dImage = (Image*)image;
    *info = d3dImage->info;
    return PAL_RESULT_SUCCESS;
}

PalResult PAL_CALL getImageMemoryRequirementsD3D12(
    PalImage* image,
    PalMemoryRequirements* requirements)
{
    Image* d3dImage = (Image*)image;
    ID3D12Device5* device = d3dImage->device->handle;
    if (d3dImage->belongsToSwapchain) {
        return PAL_RESULT_INVALID_OPERATION;
    }

    D3D12_RESOURCE_ALLOCATION_INFO allocationInfo = {0};
    D3D12_RESOURCE_ALLOCATION_INFO __ret = {0};
    allocationInfo = *device->lpVtbl->GetResourceAllocationInfo(
        device,
        &__ret,
        0,
        1,
        &d3dImage->desc);

    // d3d12 allows images to be used with only GPU only heap
    requirements->memoryTypes[PAL_MEMORY_TYPE_GPU_ONLY] = PAL_TRUE;
    requirements->memoryTypes[PAL_MEMORY_TYPE_CPU_UPLOAD] = PAL_FALSE;
    requirements->memoryTypes[PAL_MEMORY_TYPE_CPU_READBACK] = PAL_FALSE;

    requirements->alignment = allocationInfo.Alignment;
    requirements->size = allocationInfo.SizeInBytes;
    return PAL_RESULT_SUCCESS;
}

PalResult PAL_CALL bindImageMemoryD3D12(
    PalImage* image,
    PalMemory* memory,
    uint64_t offset)
{
    HRESULT result;
    Image* d3dImage = (Image*)image;
    ID3D12Device5* device = d3dImage->device->handle;
    if (d3dImage->belongsToSwapchain) {
        return PAL_RESULT_INVALID_OPERATION;
    }

    ID3D12Heap* mem = (ID3D12Heap*)memory;
    result = device->lpVtbl->CreatePlacedResource(
        device,
        mem,
        offset,
        &d3dImage->desc,
        0,
        nullptr,
        &IID_Resource,
        (void**)&d3dImage->handle);

    if (FAILED(result)) {
        pollMessagesD3D12(d3dImage->device);
        if (result == E_OUTOFMEMORY) {
            return PAL_RESULT_OUT_OF_MEMORY;
        } else if (result == E_INVALIDARG) {
            return PAL_RESULT_INVALID_ARGUMENT;
        }
        return PAL_RESULT_PLATFORM_FAILURE;
    }

    return PAL_RESULT_SUCCESS;
}

// ==================================================
// Image View
// ==================================================

PalResult PAL_CALL createImageViewD3D12(
    PalDevice* device,
    PalImage* image,
    const PalImageViewCreateInfo* info,
    PalImageView** outImageView)
{
    HRESULT result;
    ImageView* imageView = nullptr;
    Device* d3d12Device = (Device*)device;
    Image* d3dImage = (Image*)image;

    if (info->type == PAL_IMAGE_VIEW_TYPE_CUBE_ARRAY) {
        if (!(d3d12Device->features & PAL_ADAPTER_FEATURE_IMAGE_VIEW_CUBE_ARRAY)) {
            return PAL_RESULT_ADAPTER_FEATURE_NOT_SUPPORTED;
        }
    }

    imageView = palAllocate(s_D3D12.allocator, sizeof(ImageView), 0);
    if (!imageView) {
        return PAL_RESULT_OUT_OF_MEMORY;
    }

    imageView->heapIndex = UINT32_MAX;
    PalBool hasRTV = (d3dImage->desc.Flags & D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET);
    PalBool hasDSV = (d3dImage->desc.Flags & D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL);

    imageView->format = formatToD3D12(info->format);
    if (info->subresourceRange.aspect == PAL_IMAGE_ASPECT_COLOR && hasRTV) {
        RTVHeapAllocator* allocator = &d3d12Device->rtvAllocator;
        uint32_t index = allocator->freeTop;
        allocator->freeTop = allocator->freeList[index];

        D3D12_CPU_DESCRIPTOR_HANDLE dst;
        dst.ptr = getDescriptorHandleD3D12(index, allocator->incrementSize, allocator->baseOffset);

        D3D12_RENDER_TARGET_VIEW_DESC desc = {0};
        desc.Format = imageView->format;
        fillSubresourceD3D12(
            info->type, 
            &info->subresourceRange, 
            &desc, 
            nullptr, 
            nullptr, 
            nullptr);

        imageView->heapIndex = index;
        d3d12Device->handle->lpVtbl->CreateRenderTargetView(
            d3d12Device->handle,
            d3dImage->handle,
            &desc,
            dst);

    } else if (info->subresourceRange.aspect != PAL_IMAGE_ASPECT_COLOR && hasDSV) {
        DSVHeapAllocator* allocator = &d3d12Device->dsvAllocator;
        uint32_t index = allocator->freeTop;
        allocator->freeTop = allocator->freeList[index];

        D3D12_CPU_DESCRIPTOR_HANDLE dst;
        dst.ptr = getDescriptorHandleD3D12(index, allocator->incrementSize, allocator->baseOffset);

        D3D12_DEPTH_STENCIL_VIEW_DESC desc = {0};
        desc.Format = imageView->format;
        fillSubresourceD3D12(
            info->type, 
            &info->subresourceRange, 
            nullptr, 
            &desc, 
            nullptr, 
            nullptr);

        imageView->heapIndex = index;
        d3d12Device->handle->lpVtbl->CreateDepthStencilView(
            d3d12Device->handle,
            d3dImage->handle,
            &desc,
            dst);
    }

    imageView->range = info->subresourceRange;
    imageView->type = info->type;
    imageView->image = d3dImage;

    imageView->device = d3d12Device;
    *outImageView = (PalImageView*)imageView;
    return PAL_RESULT_SUCCESS;
}

void PAL_CALL destroyImageViewD3D12(PalImageView* imageView)
{
    ImageView* d3dImageView = (ImageView*)imageView;
    Device* device = d3dImageView->device;
    RTVHeapAllocator* allocator = nullptr;

    if (d3dImageView->heapIndex != UINT32_MAX) {
        if (d3dImageView->range.aspect == PAL_IMAGE_ASPECT_COLOR) {
            RTVHeapAllocator* allocator = &device->rtvAllocator;
            allocator->freeList[d3dImageView->heapIndex] = allocator->freeTop;
            allocator->freeTop = d3dImageView->heapIndex;

        } else {
            DSVHeapAllocator* allocator = &device->dsvAllocator;
            allocator->freeList[d3dImageView->heapIndex] = allocator->freeTop;
            allocator->freeTop = d3dImageView->heapIndex;
        }
    }
    palFree(s_D3D12.allocator, d3dImageView);
}

PalResult PAL_CALL createSamplerD3D12(
    PalDevice* device,
    const PalSamplerCreateInfo* info,
    PalSampler** outSampler)
{
    Device* d3d12Device = (Device*)device;
    if (info->maxAnisotropy > d3d12Device->limits.maxAnisotropy) {
        return PAL_RESULT_INVALID_ARGUMENT;
    }

    Sampler* sampler = nullptr;
    sampler = palAllocate(s_D3D12.allocator, sizeof(Sampler), 0);
    if (!sampler) {
        return PAL_RESULT_OUT_OF_MEMORY;
    }

    memset(sampler, 0, sizeof(Sampler));
    sampler->desc.MaxLOD = info->maxLod;
    sampler->desc.MinLOD = info->minLod;
    sampler->desc.MipLODBias = info->mipLodBias;

    sampler->desc.MaxAnisotropy = 1;
    if (info->enableAnisotropy) {
        sampler->desc.MaxAnisotropy = (UINT)info->maxAnisotropy;
    }

    sampler->desc.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
    if (info->enableCompare) {
        sampler->desc.ComparisonFunc = compareOpToD3D12(info->compareOp);
    }

    borderColorToD3D12(info->borderColor, sampler->desc.BorderColor);
    sampler->desc.AddressU = addressModeToD3D12(info->addressModeU);
    sampler->desc.AddressV = addressModeToD3D12(info->addressModeV);
    sampler->desc.AddressW = addressModeToD3D12(info->addressModeW);

    sampler->desc.Filter = filterToD3D12(
        info->minFilterMode,
        info->magFilterMode,
        info->mipmapMode);

    *outSampler = (PalSampler*)sampler;
    return PAL_RESULT_SUCCESS;
}

void PAL_CALL destroySamplerD3D12(PalSampler* sampler)
{
    Sampler* d3dSampler = (Sampler*)sampler;
    palFree(s_D3D12.allocator, d3dSampler);
}

#endif // PAL_HAS_D3D12_BACKEND