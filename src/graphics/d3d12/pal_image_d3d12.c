
/**
    PAL - Prime Abstraction Layer
    Copyright (C) 2025
    Licensed under the Zlib license. See LICENSE file in root.
 */

#if PAL_HAS_D3D12_BACKEND
#include "pal_d3d12.h"

static D3D12_FILTER filterToD3D12(
    PalFilterMode minFilter,
    PalFilterMode magFilter,
    PalSamplerMipmapMode mode)
{
    // all the enums start with min so we start with min filter
    switch (minFilter) {
        case PAL_FILTER_MODE_NEAREST: {
            switch (magFilter) {
                case PAL_FILTER_MODE_NEAREST: {
                    // min and mag are nearest. Check sampler mipmap mode
                    if (mode == PAL_SAMPLER_MIPMAP_MODE_NEAREST) {
                        return D3D12_FILTER_MIN_MAG_MIP_POINT; // sampler mode nearest
                    } else {
                        return D3D12_FILTER_MIN_MAG_POINT_MIP_LINEAR; // sampler mode linear
                    }
                }

                case PAL_FILTER_MODE_LINEAR: {
                    // min is nearest, mag is linear . Check sampler mipmap mode
                    if (mode == PAL_SAMPLER_MIPMAP_MODE_NEAREST) {
                        return D3D12_FILTER_MIN_POINT_MAG_LINEAR_MIP_POINT; // sampler mode nearest
                    } else {
                        return D3D12_FILTER_MIN_POINT_MAG_MIP_LINEAR; // sampler mode linear
                    }
                }
            }
        }

        case PAL_FILTER_MODE_LINEAR: {
            switch (magFilter) {
                case PAL_FILTER_MODE_NEAREST: {
                    // min is linear, mag is nearest. Check sampler mipmap mode
                    if (mode == PAL_SAMPLER_MIPMAP_MODE_NEAREST) {
                        return D3D12_FILTER_MIN_LINEAR_MAG_MIP_POINT; // sampler mode nearest
                    } else {
                        return D3D12_FILTER_MIN_LINEAR_MAG_POINT_MIP_LINEAR; // sampler mode linear
                    }
                }

                case PAL_FILTER_MODE_LINEAR: {
                    // min and mag are linear. Check sampler mipmap mode
                    if (mode == PAL_SAMPLER_MIPMAP_MODE_NEAREST) {
                        return D3D12_FILTER_MIN_MAG_LINEAR_MIP_POINT; // sampler mode nearest
                    } else {
                        return D3D12_FILTER_MIN_MAG_MIP_LINEAR; // sampler mode linear
                    }
                }
            }
        }
    }

    return D3D12_FILTER_MIN_MAG_MIP_POINT;
}

static D3D12_TEXTURE_ADDRESS_MODE addressModeToD3D12(PalSamplerAddressMode mode)
{
    switch (mode) {
        case PAL_SAMPLER_ADDRESS_MODE_REPEAT: {
            return D3D12_TEXTURE_ADDRESS_MODE_WRAP;
        }

        case PAL_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT: {
            return D3D12_TEXTURE_ADDRESS_MODE_MIRROR;
        }
        case PAL_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE: {
            return D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
        }
        case PAL_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER: {
            return D3D12_TEXTURE_ADDRESS_MODE_BORDER;
        }
    }
    return D3D12_TEXTURE_ADDRESS_MODE_WRAP;
}

static void borderColorToD3D12(
    PalBorderColor color,
    float outColor[4])
{
    switch (color) {
        case PAL_BORDER_COLOR_FLOAT_TRANSPARENT_BLACK:
        case PAL_BORDER_COLOR_INT_TRANSPARENT_BLACK: {
            outColor[0] = 0.0f;
            outColor[1] = 0.0f;
            outColor[2] = 0.0f;
            outColor[3] = 0.0f;
            break;
        }

        case PAL_BORDER_COLOR_FLOAT_OPAQUE_BLACK:
        case PAL_BORDER_COLOR_INT_OPAQUE_BLACK: {
            outColor[0] = 0.0f;
            outColor[1] = 0.0f;
            outColor[2] = 0.0f;
            outColor[3] = 1.0f;
            break;
        }

        case PAL_BORDER_COLOR_FLOAT_OPAQUE_WHITE:
        case PAL_BORDER_COLOR_INT_OPAQUE_WHITE: {
            outColor[0] = 1.0f;
            outColor[1] = 1.0f;
            outColor[2] = 1.0f;
            outColor[3] = 1.0f;
            break;
        }
    }

    outColor[0] = 0.0f;
    outColor[1] = 0.0f;
    outColor[2] = 0.0f;
    outColor[3] = 0.0f;
}

PalResult PAL_CALL createImageD3D12(
    PalDevice* device,
    const PalImageCreateInfo* info,
    PalImage** outImage)
{
    HRESULT result;
    ImageD3D12* image = nullptr;
    DeviceD3D12* d3d12Device = (DeviceD3D12*)device;

    image = palAllocate(s_D3D12.allocator, sizeof(ImageD3D12), 0);
    if (!image) {
        return PAL_RESULT_CODE_OUT_OF_MEMORY;
    }

    memset(image, 0, sizeof(ImageD3D12));
    image->desc.Width = (UINT64)info->width;
    image->desc.Height = (UINT64)info->height;
    image->desc.DepthOrArraySize = (UINT16)info->arrayLayerCount;
    image->desc.MipLevels = (UINT16)info->mipLevelCount;
    image->desc.Format = formatToD3D12(info->format);
    image->desc.SampleDesc.Count = samplesToD3D12(info->sampleCount);
    image->desc.SampleDesc.Quality = 0;

    image->desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
    if (info->type == PAL_IMAGE_TYPE_3D) {
        image->desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
        image->desc.DepthOrArraySize = (UINT16)info->depth;

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

    image->isMemoryManaged = PAL_FALSE;
    if (info->memoryUsage != PAL_IMAGE_MEMORY_USAGE_MANUAL) {
        D3D12_HEAP_PROPERTIES heapProps = {0};
        heapProps.Type = D3D12_HEAP_TYPE_DEFAULT;

        result = d3d12Device->handle->lpVtbl->CreateCommittedResource(
            d3d12Device->handle,
            &heapProps,
            0,
            &image->desc,
            D3D12_RESOURCE_STATE_COMMON,
            nullptr,
            &IID_Resource,
            (void**)&image->handle);

        if (FAILED(result)) {
            pollMessagesD3D12(d3d12Device);
            return makeResultD3D12(result);
        }

        image->isMemoryManaged = PAL_TRUE;
    }

    image->info.depth = info->depth;
    image->info.arrayLayerCount = info->arrayLayerCount;
    image->info.belongsToSwapchain = PAL_FALSE;
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
    ImageD3D12* d3d12Image = (ImageD3D12*)image;
    if (d3d12Image->isMemoryManaged) {
        d3d12Image->handle->lpVtbl->Release(d3d12Image->handle);
    }
    palFree(s_D3D12.allocator, d3d12Image);
}

void PAL_CALL getImageInfoD3D12(
    PalImage* image,
    PalImageInfo* info)
{
    ImageD3D12* d3d12Image = (ImageD3D12*)image;
    *info = d3d12Image->info;
}

void PAL_CALL getImageMemoryRequirementsD3D12(
    PalImage* image,
    PalMemoryRequirements* requirements)
{
    ImageD3D12* d3d12Image = (ImageD3D12*)image;
    ID3D12Device5* device = d3d12Image->device->handle;

    D3D12_RESOURCE_ALLOCATION_INFO allocationInfo = {0};
    D3D12_RESOURCE_ALLOCATION_INFO __ret = {0};
    allocationInfo =
        *device->lpVtbl->GetResourceAllocationInfo(device, &__ret, 0, 1, &d3d12Image->desc);

    requirements->supportedMemoryTypes = (1u << PAL_MEMORY_TYPE_GPU_ONLY);
    requirements->alignment = allocationInfo.Alignment;
    requirements->size = allocationInfo.SizeInBytes;
}

PalResult PAL_CALL bindImageMemoryD3D12(
    PalImage* image,
    PalMemory* memory,
    uint64_t offset)
{
    HRESULT result;
    ImageD3D12* d3d12Image = (ImageD3D12*)image;
    ID3D12Device5* device = d3d12Image->device->handle;
    if (d3d12Image->info.belongsToSwapchain) {
        return PAL_RESULT_CODE_INVALID_OPERATION;
    }

    if (d3d12Image->isMemoryManaged) {
        return PAL_RESULT_CODE_INVALID_OPERATION;
    }

    ID3D12Heap* mem = (ID3D12Heap*)memory;
    result = device->lpVtbl->CreatePlacedResource(
        device,
        mem,
        offset,
        &d3d12Image->desc,
        0,
        nullptr,
        &IID_Resource,
        (void**)&d3d12Image->handle);

    if (FAILED(result)) {
        pollMessagesD3D12(d3d12Image->device);
        return makeResultD3D12(result);
    }

    return PAL_RESULT_SUCCESS;
}

PalResult PAL_CALL createImageViewD3D12(
    PalDevice* device,
    PalImage* image,
    const PalImageViewCreateInfo* info,
    PalImageView** outImageView)
{
    HRESULT result;
    ImageViewD3D12* imageView = nullptr;
    DeviceD3D12* d3d12Device = (DeviceD3D12*)device;
    ImageD3D12* d3d12Image = (ImageD3D12*)image;

    imageView = palAllocate(s_D3D12.allocator, sizeof(ImageViewD3D12), 0);
    if (!imageView) {
        return PAL_RESULT_CODE_OUT_OF_MEMORY;
    }

    imageView->heapIndex = UINT32_MAX;
    PalBool hasRTV = (d3d12Image->desc.Flags & D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET);
    PalBool hasDSV = (d3d12Image->desc.Flags & D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL);

    imageView->format = formatToD3D12(info->format);
    if (info->subresourceRange.aspect == PAL_IMAGE_ASPECT_COLOR && hasRTV) {
        RTVHeapAllocator* allocator = &d3d12Device->rtvAllocator;
        uint32_t index = allocator->freeTop;
        allocator->freeTop = allocator->freeList[index];

        D3D12_CPU_DESCRIPTOR_HANDLE dst;
        dst.ptr = getDescriptorHandleD3D12(index, allocator->incrementSize, allocator->baseOffset);

        D3D12_RENDER_TARGET_VIEW_DESC desc = {0};
        desc.Format = imageView->format;
        fillSubresourceD3D12(DESC_TYPE_RTV, info->type, &info->subresourceRange, &desc);
        imageView->heapIndex = index;
        d3d12Device->handle->lpVtbl
            ->CreateRenderTargetView(d3d12Device->handle, d3d12Image->handle, &desc, dst);

    } else if (info->subresourceRange.aspect != PAL_IMAGE_ASPECT_COLOR && hasDSV) {
        DSVHeapAllocator* allocator = &d3d12Device->dsvAllocator;
        uint32_t index = allocator->freeTop;
        allocator->freeTop = allocator->freeList[index];

        D3D12_CPU_DESCRIPTOR_HANDLE dst;
        dst.ptr = getDescriptorHandleD3D12(index, allocator->incrementSize, allocator->baseOffset);

        D3D12_DEPTH_STENCIL_VIEW_DESC desc = {0};
        desc.Format = imageView->format;
        fillSubresourceD3D12(DESC_TYPE_DSV, info->type, &info->subresourceRange, &desc);
        imageView->heapIndex = index;
        d3d12Device->handle->lpVtbl
            ->CreateDepthStencilView(d3d12Device->handle, d3d12Image->handle, &desc, dst);
    }

    imageView->range = info->subresourceRange;
    imageView->type = info->type;
    imageView->image = d3d12Image;

    imageView->device = d3d12Device;
    *outImageView = (PalImageView*)imageView;
    return PAL_RESULT_SUCCESS;
}

void PAL_CALL destroyImageViewD3D12(PalImageView* imageView)
{
    ImageViewD3D12* d3dImageView = (ImageViewD3D12*)imageView;
    DeviceD3D12* device = d3dImageView->device;
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
    DeviceD3D12* d3d12Device = (DeviceD3D12*)device;
    if (info->maxAnisotropy > d3d12Device->limits.maxAnisotropy) {
        return PAL_RESULT_CODE_INVALID_ARGUMENT;
    }

    SamplerD3D12* sampler = nullptr;
    sampler = palAllocate(s_D3D12.allocator, sizeof(SamplerD3D12), 0);
    if (!sampler) {
        return PAL_RESULT_CODE_OUT_OF_MEMORY;
    }

    memset(sampler, 0, sizeof(SamplerD3D12));
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

    sampler->desc.Filter =
        filterToD3D12(info->minFilterMode, info->magFilterMode, info->mipmapMode);

    *outSampler = (PalSampler*)sampler;
    return PAL_RESULT_SUCCESS;
}

void PAL_CALL destroySamplerD3D12(PalSampler* sampler)
{
    SamplerD3D12* d3dSampler = (SamplerD3D12*)sampler;
    palFree(s_D3D12.allocator, d3dSampler);
}

#endif // PAL_HAS_D3D12_BACKEND