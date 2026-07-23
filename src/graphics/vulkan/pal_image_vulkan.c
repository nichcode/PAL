
/**
    PAL - Prime Abstraction Layer
    Copyright (C) 2025
    Licensed under the Zlib license. See LICENSE file in root.
 */

#if PAL_HAS_VULKAN_BACKEND
#include "pal_vulkan.h"

static VkImageUsageFlags imageUsageToVk(PalImageUsages usages)
{
    VkImageUsageFlags flags = 0;
    if (usages & PAL_IMAGE_USAGE_COLOR_ATTACHEMENT) {
        flags |= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    }

    if (usages & PAL_IMAGE_USAGE_DEPTH_ATTACHEMENT) {
        flags |= VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
    }

    if (usages & PAL_IMAGE_USAGE_TRANSFER_SRC) {
        flags |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
    }

    if (usages & PAL_IMAGE_USAGE_TRANSFER_DST) {
        flags |= VK_IMAGE_USAGE_TRANSFER_DST_BIT;
    }

    if (usages & PAL_IMAGE_USAGE_STORAGE) {
        flags |= VK_IMAGE_USAGE_STORAGE_BIT;
    }

    if (usages & PAL_IMAGE_USAGE_SAMPLED) {
        flags |= VK_IMAGE_USAGE_SAMPLED_BIT;
    }

    return flags;
}

static VkImageViewType imageViewTypeToVk(PalImageViewType type)
{
    switch (type) {
        case PAL_IMAGE_VIEW_TYPE_1D:
            return VK_IMAGE_VIEW_TYPE_1D;

        case PAL_IMAGE_VIEW_TYPE_1D_ARRAY:
            return VK_IMAGE_VIEW_TYPE_1D_ARRAY;

        case PAL_IMAGE_VIEW_TYPE_2D:
            return VK_IMAGE_VIEW_TYPE_2D;

        case PAL_IMAGE_VIEW_TYPE_2D_ARRAY:
            return VK_IMAGE_VIEW_TYPE_2D_ARRAY;

        case PAL_IMAGE_VIEW_TYPE_3D:
            return VK_IMAGE_VIEW_TYPE_3D;

        case PAL_IMAGE_VIEW_TYPE_CUBE:
            return VK_IMAGE_VIEW_TYPE_CUBE;

        case PAL_IMAGE_VIEW_TYPE_CUBE_ARRAY:
            return VK_IMAGE_VIEW_TYPE_CUBE_ARRAY;
    }

    return VK_IMAGE_VIEW_TYPE_2D;
}

static VkFilter filterToVk(PalFilterMode mode)
{
    switch (mode) {
        case PAL_FILTER_MODE_NEAREST: {
            return VK_FILTER_NEAREST;
        }

        case PAL_FILTER_MODE_LINEAR: {
            return VK_FILTER_LINEAR;
        }
    }
    return VK_FILTER_NEAREST;
}

static VkSamplerMipmapMode mipmapModeToVk(PalSamplerMipmapMode mode)
{
    switch (mode) {
        case PAL_SAMPLER_MIPMAP_MODE_NEAREST: {
            return VK_SAMPLER_MIPMAP_MODE_NEAREST;
        }

        case PAL_SAMPLER_MIPMAP_MODE_LINEAR: {
            return VK_SAMPLER_MIPMAP_MODE_LINEAR;
        }
    }
    return VK_SAMPLER_MIPMAP_MODE_NEAREST;
}

static VkSamplerAddressMode addressModeToVk(PalSamplerAddressMode mode)
{
    switch (mode) {
        case PAL_SAMPLER_ADDRESS_MODE_REPEAT: {
            return VK_SAMPLER_ADDRESS_MODE_REPEAT;
        }

        case PAL_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT: {
            return VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT;
        }
        case PAL_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE: {
            return VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
        }
        case PAL_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER: {
            return VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
        }
    }
    return VK_SAMPLER_ADDRESS_MODE_REPEAT;
}

static VkBorderColor borderColorToVk(PalBorderColor color)
{
    switch (color) {
        case PAL_BORDER_COLOR_FLOAT_TRANSPARENT_BLACK: {
            return VK_BORDER_COLOR_FLOAT_TRANSPARENT_BLACK;
        }

        case PAL_BORDER_COLOR_INT_TRANSPARENT_BLACK: {
            return VK_BORDER_COLOR_INT_TRANSPARENT_BLACK;
        }

        case PAL_BORDER_COLOR_FLOAT_OPAQUE_BLACK: {
            return VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK;
        }

        case PAL_BORDER_COLOR_INT_OPAQUE_BLACK: {
            return VK_BORDER_COLOR_INT_OPAQUE_BLACK;
        }

        case PAL_BORDER_COLOR_FLOAT_OPAQUE_WHITE: {
            return VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;
        }

        case PAL_BORDER_COLOR_INT_OPAQUE_WHITE: {
            return VK_BORDER_COLOR_INT_OPAQUE_WHITE;
        }
    }
    return VK_BORDER_COLOR_FLOAT_TRANSPARENT_BLACK;
}

PalResult PAL_CALL createImageVk(
    PalDevice* device,
    const PalImageCreateInfo* info,
    PalImage** outImage)
{
    VkResult result;
    ImageVk* image = nullptr;
    DeviceVk* deviceImpl = (DeviceVk*)device;
    MemoryVk* memory = nullptr;

    image = palAllocate(s_Vk.allocator, sizeof(ImageVk), 0);
    if (!image) {
        return PAL_RESULT_CODE_OUT_OF_MEMORY;
    }

    if (info->memoryUsage != PAL_IMAGE_MEMORY_USAGE_MANUAL) {
        memory = palAllocate(s_Vk.allocator, sizeof(MemoryVk), 0);
        if (!memory) {
            return PAL_RESULT_CODE_OUT_OF_MEMORY;
        }
    }

    VkImageCreateInfo createInfo = {0};
    createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    createInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    createInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
    createInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    createInfo.extent.width = info->width;
    createInfo.extent.height = info->height;
    createInfo.extent.depth = info->depth;

    createInfo.arrayLayers = info->arrayLayerCount;
    createInfo.mipLevels = info->mipLevelCount;
    createInfo.format = formatToVk(info->format);
    createInfo.samples = samplesToVk(info->sampleCount);
    createInfo.usage = imageUsageToVk(info->usages);

    createInfo.imageType = VK_IMAGE_TYPE_2D;
    if (info->type == PAL_IMAGE_TYPE_3D) {
        createInfo.imageType = VK_IMAGE_TYPE_3D;

    } else if (info->type == PAL_IMAGE_TYPE_1D) {
        createInfo.imageType = VK_IMAGE_TYPE_1D;
    }

    result = s_Vk.createImage(deviceImpl->handle, &createInfo, &s_Vk.allocatorImpl, &image->handle);
    if (result != VK_SUCCESS) {
        palFree(s_Vk.allocator, image);
        return makeResultVk(result);
    }

    image->isMemoryManaged = PAL_FALSE;
    if (info->memoryUsage != PAL_IMAGE_MEMORY_USAGE_MANUAL) {
        PalMemoryType memoryType = PAL_MEMORY_TYPE_GPU_ONLY;

        // allocate and manage memory
        VkMemoryRequirements memReq = {0};
        s_Vk.getImageMemoryRequirements(deviceImpl->handle, image->handle, &memReq);

        VkMemoryAllocateInfo allocateInfo = {0};
        allocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocateInfo.allocationSize = (VkDeviceSize)memReq.size;

        uint32_t memoryMask = deviceImpl->memoryClassMask[memoryType] & memReq.memoryTypeBits;
        uint32_t memoryIndex = findBestMemoryIndexVk(deviceImpl->phyDevice, memoryMask);
        if (!(memoryMask & (1u << memoryIndex))) {
            return PAL_RESULT_CODE_PLATFORM_FAILURE;
        }

        allocateInfo.memoryTypeIndex = memoryIndex;
        result = s_Vk.allocateMemory(
            deviceImpl->handle,
            &allocateInfo,
            &s_Vk.allocatorImpl,
            &memory->handle);

        if (result != VK_SUCCESS) {
            return makeResultVk(result);
        }

        result = s_Vk.bindImageMemory(deviceImpl->handle, image->handle, memory->handle, 0);
        if (result != VK_SUCCESS) {
            return makeResultVk(result);
        }

        memory->type = memoryType;
        image->isMemoryManaged = PAL_TRUE;
    }

    image->device = deviceImpl;
    image->info.type = info->type;
    image->info.format = info->format;
    image->info.usages = info->usages;
    image->info.height = info->height;
    image->info.mipLevelCount = info->mipLevelCount;
    image->info.sampleCount = info->sampleCount;
    image->info.width = info->width;
    image->info.depth = info->depth;
    image->info.arrayLayerCount = info->arrayLayerCount;
    image->info.belongsToSwapchain = PAL_FALSE;

    image->memory = memory;
    *outImage = (PalImage*)image;
    return PAL_RESULT_SUCCESS;
}

void PAL_CALL destroyImageVk(PalImage* image)
{
    ImageVk* imageImpl = (ImageVk*)image;
    if (imageImpl->info.belongsToSwapchain) {
        return;
    }

    s_Vk.destroyImage(imageImpl->device->handle, imageImpl->handle, &s_Vk.allocatorImpl);
    if (imageImpl->isMemoryManaged) {
        s_Vk.freeMemory(imageImpl->device->handle, imageImpl->memory->handle, &s_Vk.allocatorImpl);
        palFree(s_Vk.allocator, imageImpl->memory);
    }

    palFree(s_Vk.allocator, imageImpl);
}

void PAL_CALL getImageInfoVk(
    PalImage* image,
    PalImageInfo* info)
{
    ImageVk* imageImpl = (ImageVk*)image;
    *info = imageImpl->info;
}

void PAL_CALL getImageMemoryRequirementsVk(
    PalImage* image,
    PalMemoryRequirements* requirements)
{
    ImageVk* imageImpl = (ImageVk*)image;
    if (imageImpl->info.belongsToSwapchain) {
        return;
    }

    DeviceVk* device = imageImpl->device;
    VkMemoryRequirements memReq = {0};
    s_Vk.getImageMemoryRequirements(device->handle, imageImpl->handle, &memReq);
    requirements->alignment = (uint64_t)memReq.alignment;
    requirements->size = (uint64_t)memReq.size;
    requirements->memoryMask = palPackUint32(memReq.memoryTypeBits, 0);
    requirements->supportedMemoryTypes = 0;

    if ((memReq.memoryTypeBits & device->memoryClassMask[PAL_MEMORY_TYPE_GPU_ONLY]) != 0) {
        requirements->supportedMemoryTypes |= (1u << PAL_MEMORY_TYPE_GPU_ONLY);
    }
}

PalResult PAL_CALL bindImageMemoryVk(
    PalImage* image,
    PalMemory* memory,
    uint64_t offset)
{
    ImageVk* imageImpl = (ImageVk*)image;
    if (imageImpl->info.belongsToSwapchain) {
        return PAL_RESULT_CODE_INVALID_OPERATION;
    }

    if (imageImpl->memory) {
        return PAL_RESULT_CODE_INVALID_OPERATION;
    }

    MemoryVk* memoryImpl = (MemoryVk*)memory;
    s_Vk.bindImageMemory(imageImpl->device->handle, imageImpl->handle, memoryImpl->handle, offset);
    imageImpl->memory = memoryImpl;
    return PAL_RESULT_SUCCESS;
}

PalResult PAL_CALL createImageViewVk(
    PalDevice* device,
    PalImage* image,
    const PalImageViewCreateInfo* info,
    PalImageView** outImageView)
{
    VkResult result = VK_SUCCESS;
    ImageViewVk* imageView = nullptr;
    DeviceVk* deviceImpl = (DeviceVk*)device;
    ImageVk* imageImpl = (ImageVk*)image;

    imageView = palAllocate(s_Vk.allocator, sizeof(ImageViewVk), 0);
    if (!imageView) {
        return PAL_RESULT_CODE_OUT_OF_MEMORY;
    }

    VkImageViewCreateInfo createInfo = {0};
    createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    createInfo.format = formatToVk(info->format);
    createInfo.image = imageImpl->handle;
    createInfo.viewType = imageViewTypeToVk(info->type);

    createInfo.subresourceRange.aspectMask = imageAspectToVk(info->subresourceRange.aspect);
    createInfo.subresourceRange.baseArrayLayer = info->subresourceRange.startArrayLayer;
    createInfo.subresourceRange.baseMipLevel = info->subresourceRange.startMipLevel;
    createInfo.subresourceRange.levelCount = info->subresourceRange.mipLevelCount;
    createInfo.subresourceRange.layerCount = info->subresourceRange.layerArrayCount;

    result = s_Vk.createImageView(
        deviceImpl->handle,
        &createInfo,
        &s_Vk.allocatorImpl,
        &imageView->handle);

    if (result != VK_SUCCESS) {
        palFree(s_Vk.allocator, imageView);
        return makeResultVk(result);
    }

    imageView->device = deviceImpl;
    imageView->image = imageImpl;
    imageView->layerCount = createInfo.subresourceRange.layerCount;
    *outImageView = (PalImageView*)imageView;
    return PAL_RESULT_SUCCESS;
}

void PAL_CALL destroyImageViewVk(PalImageView* imageView)
{
    ImageViewVk* imageViewImpl = (ImageViewVk*)imageView;
    s_Vk.destroyImageView(
        imageViewImpl->device->handle,
        imageViewImpl->handle,
        &s_Vk.allocatorImpl);
    palFree(s_Vk.allocator, imageViewImpl);
}

PalResult PAL_CALL createSamplerVk(
    PalDevice* device,
    const PalSamplerCreateInfo* info,
    PalSampler** outSampler)
{
    VkResult result = VK_SUCCESS;
    SamplerVk* sampler = nullptr;
    DeviceVk* deviceImpl = (DeviceVk*)device;

    sampler = palAllocate(s_Vk.allocator, sizeof(SamplerVk), 0);
    if (!sampler) {
        return PAL_RESULT_CODE_OUT_OF_MEMORY;
    }

    VkSamplerCreateInfo createInfo = {0};
    createInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    createInfo.anisotropyEnable = info->enableAnisotropy;
    createInfo.compareEnable = info->enableCompare;

    createInfo.mipLodBias = info->mipLodBias;
    createInfo.minLod = info->minLod;
    createInfo.maxLod = info->maxLod;
    createInfo.maxAnisotropy = info->maxAnisotropy;
    createInfo.compareOp = compareOpToVk(info->compareOp);

    createInfo.minFilter = filterToVk(info->minFilterMode);
    createInfo.magFilter = filterToVk(info->magFilterMode);
    createInfo.mipmapMode = mipmapModeToVk(info->mipmapMode);

    createInfo.addressModeU = addressModeToVk(info->addressModeU);
    createInfo.addressModeV = addressModeToVk(info->addressModeV);
    createInfo.addressModeW = addressModeToVk(info->addressModeW);
    createInfo.borderColor = borderColorToVk(info->borderColor);

    result =
        s_Vk.createSampler(deviceImpl->handle, &createInfo, &s_Vk.allocatorImpl, &sampler->handle);

    if (result != VK_SUCCESS) {
        palFree(s_Vk.allocator, sampler);
        return makeResultVk(result);
    }

    sampler->device = deviceImpl;
    *outSampler = (PalSampler*)sampler;
    return PAL_RESULT_SUCCESS;
}

void PAL_CALL destroySamplerVk(PalSampler* sampler)
{
    SamplerVk* samplerImpl = (SamplerVk*)sampler;
    s_Vk.destroySampler(samplerImpl->device->handle, samplerImpl->handle, &s_Vk.allocatorImpl);
    palFree(s_Vk.allocator, samplerImpl);
}

#endif // PAL_HAS_VULKAN_BACKEND