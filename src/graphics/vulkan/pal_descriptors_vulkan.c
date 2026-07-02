
/**
    PAL - Prime Abstraction Layer
    Copyright (C) 2025
    Licensed under the Zlib license. See LICENSE file in root.
 */

#if PAL_HAS_VULKAN_BACKEND
#include "pal_vulkan.h"

PalResult PAL_CALL createDescriptorSetLayoutVk(
    PalDevice* device,
    const PalDescriptorSetLayoutCreateInfo* info,
    PalDescriptorSetLayout** outLayout)
{
    VkResult result;
    DeviceVk* vkDevice = (DeviceVk*)device;
    VkDescriptorSetLayoutBinding* bindings = nullptr;
    VkDescriptorBindingFlags* bindingFlags = nullptr;
    DescriptorSetLayoutVk* layout = nullptr;
    uint32_t count = info->bindingCount;
    VkDescriptorSetLayoutBindingFlagsCreateInfoEXT flagsCreateInfo = {0};
    flagsCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_BINDING_FLAGS_CREATE_INFO_EXT;

    PalBool hasDescriptorIndexing = vkDevice->features & PAL_ADAPTER_FEATURE_DESCRIPTOR_INDEXING;
    if (info->descriptorIndexingFlags != 0 && hasDescriptorIndexing) {
        return PAL_RESULT_CODE_FEATURE_NOT_SUPPORTED;
    }

    layout = palAllocate(s_Vk.allocator, sizeof(DescriptorSetLayoutVk), 0);
    bindings = palAllocate(s_Vk.allocator, sizeof(VkDescriptorSetLayoutBinding) * count, 0);
    if (!layout || !bindings) {
        return PAL_RESULT_CODE_OUT_OF_MEMORY;
    }

    if (info->descriptorIndexingFlags != 0) {
        bindingFlags = palAllocate(s_Vk.allocator, sizeof(VkDescriptorBindingFlags) * count, 0);
        if (!bindingFlags) {
            return PAL_RESULT_CODE_OUT_OF_MEMORY;
        }
    }

    VkDescriptorSetLayoutCreateInfo createInfo = {0};
    createInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    createInfo.bindingCount = count;
    createInfo.pBindings = bindings;

    uint32_t bindingIndex = 0;
    for (int i = 0; i < count; i++) {
        VkDescriptorSetLayoutBinding* binding = &bindings[i];
        binding->binding = bindingIndex++;
        binding->descriptorCount = info->bindings[i].descriptorCount;
        binding->descriptorType = descriptortypeToVk(info->bindings[i].descriptorType);

        binding->pImmutableSamplers = nullptr;
        binding->stageFlags = vkDevice->shaderStages;

        // set descriptor indexing flags
        if (info->descriptorIndexingFlags & PAL_DESCRIPTOR_INDEXING_FLAG_UPDATE_AFTER_BIND) {
            bindingFlags[i] |= VK_DESCRIPTOR_BINDING_UPDATE_AFTER_BIND_BIT_EXT;
            bindingFlags[i] |= VK_DESCRIPTOR_BINDING_UPDATE_UNUSED_WHILE_PENDING_BIT_EXT;
        }

        if (info->descriptorIndexingFlags & PAL_DESCRIPTOR_INDEXING_FLAG_PARTIALLY_BOUND) {
            bindingFlags[i] = VK_DESCRIPTOR_BINDING_PARTIALLY_BOUND_BIT_EXT;
        }
    }

    if (info->descriptorIndexingFlags != 0) {
        flagsCreateInfo.bindingCount = count;
        flagsCreateInfo.pBindingFlags = bindingFlags;
        createInfo.pNext = &flagsCreateInfo;
        createInfo.flags = VK_DESCRIPTOR_SET_LAYOUT_CREATE_UPDATE_AFTER_BIND_POOL_BIT_EXT;
    }

    result = s_Vk.createDescriptorSetLayout(
        vkDevice->handle,
        &createInfo,
        &s_Vk.vkAllocator,
        &layout->handle);

    if (result != VK_SUCCESS) {
        palFree(s_Vk.allocator, layout);
        palFree(s_Vk.allocator, bindings);
        return makeResultVk(result);
    }

    palFree(s_Vk.allocator, bindings);

    layout->device = vkDevice;
    layout->flags = info->descriptorIndexingFlags;
    layout->reserved = PAL_BACKEND_KEY;
    *outLayout = (PalDescriptorSetLayout*)layout;
    return PAL_RESULT_SUCCESS;
}

void PAL_CALL destroyDescriptorSetLayoutVk(PalDescriptorSetLayout* layout)
{
    DescriptorSetLayoutVk* vkLayout = (DescriptorSetLayoutVk*)layout;
    s_Vk.destroyDescriptorSetLayout(
        vkLayout->device->handle,
        vkLayout->handle,
        &s_Vk.vkAllocator);

    palFree(s_Vk.allocator, layout);
}

PalResult PAL_CALL createDescriptorPoolVk(
    PalDevice* device,
    const PalDescriptorPoolCreateInfo* info,
    PalDescriptorPool** outPool)
{
    VkResult result;
    DeviceVk* vkDevice = (DeviceVk*)device;
    DescriptorPoolVk* pool = nullptr;
    VkDescriptorPoolSize* poolSizes = nullptr;
    uint32_t bindingSizeCount = info->bindingSizeCount;
    VkDescriptorPoolCreateInfo createInfo = {0};
    createInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;

    PalBool hasDescriptorIndexing = vkDevice->features & PAL_ADAPTER_FEATURE_DESCRIPTOR_INDEXING;
    if (info->descriptorIndexingFlags != 0 && hasDescriptorIndexing) {
        return PAL_RESULT_CODE_FEATURE_NOT_SUPPORTED;
    }

    if (info->descriptorIndexingFlags & PAL_DESCRIPTOR_INDEXING_FLAG_PARTIALLY_BOUND) {
        createInfo.flags = VK_DESCRIPTOR_POOL_CREATE_UPDATE_AFTER_BIND_BIT_EXT;
    }

    pool = palAllocate(s_Vk.allocator, sizeof(DescriptorPoolVk), 0);
    poolSizes = palAllocate(s_Vk.allocator, sizeof(VkDescriptorPoolSize) * bindingSizeCount, 0);
    if (!pool || !poolSizes) {
        return PAL_RESULT_CODE_OUT_OF_MEMORY;
    }

    for (int i = 0; i < bindingSizeCount; i++) {
        VkDescriptorPoolSize* poolSize = &poolSizes[i];
        poolSize->descriptorCount = info->bindingSizes[i].bindingCount;
        poolSize->type = descriptortypeToVk(info->bindingSizes[i].descriptorType);
    }

    createInfo.maxSets = info->maxDescriptorSets;
    createInfo.poolSizeCount = bindingSizeCount;
    createInfo.pPoolSizes = poolSizes;

    result = s_Vk.createDescriptorPool(
        vkDevice->handle,
        &createInfo,
        &s_Vk.vkAllocator,
        &pool->handle);

    if (result != VK_SUCCESS) {
        palFree(s_Vk.allocator, pool);
        palFree(s_Vk.allocator, poolSizes);
        return makeResultVk(result);
    }

    palFree(s_Vk.allocator, poolSizes);
    pool->device = vkDevice;
    pool->flags = info->descriptorIndexingFlags;
    pool->reserved = PAL_BACKEND_KEY;
    *outPool = (PalDescriptorPool*)pool;
    return PAL_RESULT_SUCCESS;
}

void PAL_CALL destroyDescriptorPoolVk(PalDescriptorPool* pool)
{
    DescriptorPoolVk* vkPool = (DescriptorPoolVk*)pool;
    s_Vk.destroyDescriptorPool(vkPool->device->handle, vkPool->handle, &s_Vk.vkAllocator);
    palFree(s_Vk.allocator, pool);
}

PalResult PAL_CALL resetDescriptorPoolVk(PalDescriptorPool* pool)
{
    DescriptorPoolVk* vkPool = (DescriptorPoolVk*)pool;
    s_Vk.resetDescriptorPool(vkPool->device->handle, vkPool->handle, 0);
    return PAL_RESULT_SUCCESS;
}

PalResult PAL_CALL allocateDescriptorSetVk(
    PalDevice* device,
    PalDescriptorPool* pool,
    PalDescriptorSetLayout* layout,
    PalDescriptorSet** outSet)
{
    VkResult result;
    DeviceVk* vkDevice = (DeviceVk*)device;
    DescriptorPoolVk* vkPool = (DescriptorPoolVk*)pool;
    DescriptorSetLayoutVk* vkLayout = (DescriptorSetLayoutVk*)layout;
    DescriptorSetVk* set = nullptr;

    PalBool isPoolValid = vkPool->flags & PAL_DESCRIPTOR_INDEXING_FLAG_UPDATE_AFTER_BIND;
    PalBool isLayoutValid = vkLayout->flags & PAL_DESCRIPTOR_INDEXING_FLAG_UPDATE_AFTER_BIND;
    if (isPoolValid != isLayoutValid) {
        // we check if both are true or false
        return PAL_RESULT_CODE_INVALID_OPERATION;
    }

    set = palAllocate(s_Vk.allocator, sizeof(DescriptorSetVk), 0);
    if (!set) {
        return PAL_RESULT_CODE_OUT_OF_MEMORY;
    }

    VkDescriptorSetAllocateInfo allocateInfo = {0};
    allocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocateInfo.descriptorPool = vkPool->handle;
    allocateInfo.descriptorSetCount = 1;
    allocateInfo.pSetLayouts = &vkLayout->handle;

    result = s_Vk.allocateDescriptorSet(vkDevice->handle, &allocateInfo, &set->handle);
    if (result != VK_SUCCESS) {
        palFree(s_Vk.allocator, set);
        return makeResultVk(result);
    }

    set->pool = vkPool;
    set->device = vkDevice;
    set->reserved = PAL_BACKEND_KEY;
    *outSet = (PalDescriptorSet*)set;
    return PAL_RESULT_SUCCESS;
}

PalResult PAL_CALL updateDescriptorSetVk(
    PalDevice* device,
    uint32_t count,
    PalDescriptorSetWriteInfo* infos)
{
    VkResult result;
    DeviceVk* vkDevice = (DeviceVk*)device;
    VkWriteDescriptorSet* writes = nullptr;
    VkDescriptorBufferInfo* bufferInfos = nullptr;
    VkDescriptorImageInfo* imageInfos = nullptr;
    VkAccelerationStructureKHR* tlas = nullptr;
    VkWriteDescriptorSetAccelerationStructureKHR* tlasInfos = nullptr;

    uint32_t bufferCount = 0;
    uint32_t imageCount = 0;
    uint32_t tlasCount = 0;
    uint32_t tlasInfoCount = 0;

    for (int i = 0; i < count; i++) {
        if (infos[i].descriptorType == PAL_DESCRIPTOR_TYPE_STORAGE_BUFFER ||
            infos[i].descriptorType == PAL_DESCRIPTOR_TYPE_UNIFORM_BUFFER) {
            bufferCount += infos[i].descriptorCount;

        } else if (infos[i].descriptorType == PAL_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE) {
            tlasCount += infos[i].descriptorCount;
            tlasInfoCount++;

        } else {
            imageCount += infos[i].descriptorCount;
        }
    }

    writes = palAllocate(s_Vk.allocator, sizeof(VkWriteDescriptorSet) * count, 0);
    if (!writes) {
        return PAL_RESULT_CODE_OUT_OF_MEMORY;
    }
    memset(writes, 0, sizeof(VkWriteDescriptorSet) * count);

    if (bufferCount) {
        bufferInfos = palAllocate(s_Vk.allocator, sizeof(VkDescriptorBufferInfo) * bufferCount, 0);
        if (!bufferInfos) {
            return PAL_RESULT_CODE_OUT_OF_MEMORY;
        }

        memset(bufferInfos, 0, sizeof(VkDescriptorBufferInfo) * bufferCount);
    }

    
    if (imageCount) {
        imageInfos = palAllocate(s_Vk.allocator, sizeof(VkDescriptorImageInfo) * imageCount, 0);
        if (!imageInfos) {
            return PAL_RESULT_CODE_OUT_OF_MEMORY;
        }

        memset(imageInfos, 0, sizeof(VkDescriptorImageInfo) * imageCount);
    }

    if (tlasCount) {
        uint32_t tlasInfoSize = sizeof(VkWriteDescriptorSetAccelerationStructureKHR) * tlasInfoCount;
        tlasInfos = palAllocate(s_Vk.allocator, tlasInfoSize, 0);
        tlas = palAllocate(s_Vk.allocator, sizeof(VkAccelerationStructureKHR) * count, 0);
        if (!tlasInfos || !tlas) {
            return PAL_RESULT_CODE_OUT_OF_MEMORY;
        }

        memset(tlasInfos, 0, tlasInfoSize);
        memset(tlas, 0, sizeof(VkAccelerationStructureKHR) * tlasCount);
    }

    // reset and reuse
    bufferCount = 0;
    imageCount = 0;
    tlasCount = 0;
    tlasInfoCount = 0;

    for (int i = 0; i < count; i++) {
        VkWriteDescriptorSet* write = &writes[i];
        PalDescriptorSetWriteInfo* info = &infos[i];

        write->sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        write->dstArrayElement = info->arrayElement;
        write->dstBinding = info->layoutBindingIndex;
        write->descriptorCount = info->descriptorCount;
        write->descriptorType = descriptortypeToVk(info->descriptorType);

        DescriptorSetVk* set = (DescriptorSetVk*)info->descriptorSet;
        write->dstSet = set->handle;

        for (int j = 0; j < write->descriptorCount; j++) {
            if (info->descriptorType == PAL_DESCRIPTOR_TYPE_STORAGE_BUFFER ||
                info->descriptorType == PAL_DESCRIPTOR_TYPE_UNIFORM_BUFFER) {
                VkDescriptorBufferInfo* bufferInfo = &bufferInfos[bufferCount + j];

                if (info->bufferInfos) {
                    PalDescriptorBufferInfo* tmp = &info->bufferInfos[j];
                    BufferVk* vkBuffer = (BufferVk*)tmp->buffer;

                    bufferInfo->buffer = vkBuffer->handle;
                    bufferInfo->offset = tmp->offset;
                    bufferInfo->range = tmp->size;

                } else  {
                    if (!(vkDevice->features & PAL_ADAPTER_FEATURE_NULL_DESCRIPTORS)) {
                        return PAL_RESULT_CODE_FEATURE_NOT_SUPPORTED;
                    }
                }

            } else if (info->descriptorType == PAL_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE) {
                if (info->tlasInfos) {
                    PalDescriptorTLASInfo* tmp = &info->tlasInfos[j];
                    AccelerationStructureVk* as = (AccelerationStructureVk*)tmp->tlas;
                    tlas[tlasCount + j] = as->handle;

                } else {
                    if (!(vkDevice->features & PAL_ADAPTER_FEATURE_NULL_DESCRIPTORS)) {
                        return PAL_RESULT_CODE_FEATURE_NOT_SUPPORTED;
                    }
                }

            } else {
                VkDescriptorImageInfo* imageInfo = &imageInfos[imageCount + j];

                if (info->descriptorType == PAL_DESCRIPTOR_TYPE_SAMPLER) {
                    if (info->samplerInfos) {
                        PalDescriptorSamplerInfo* tmp = &info->samplerInfos[j];
                        SamplerVk* vkSampler = (SamplerVk*)tmp->sampler;
                        imageInfo->sampler = vkSampler->handle;
                        imageInfo->imageLayout = VK_IMAGE_LAYOUT_UNDEFINED;

                    }  else {
                        if (!(vkDevice->features & PAL_ADAPTER_FEATURE_NULL_DESCRIPTORS)) {
                            return PAL_RESULT_CODE_FEATURE_NOT_SUPPORTED;
                        }
                    }    

                } else if (info->descriptorType == PAL_DESCRIPTOR_TYPE_STORAGE_IMAGE) {
                    if (info->imageViewInfos) {
                        PalDescriptorImageViewInfo* tmp = &info->imageViewInfos[j];
                        ImageViewVk* vkImageView = (ImageViewVk*)tmp->imageView;

                        imageInfo->sampler = nullptr;
                        imageInfo->imageLayout = VK_IMAGE_LAYOUT_GENERAL;
                        imageInfo->imageView = vkImageView->handle;

                    } else {
                        if (!(vkDevice->features & PAL_ADAPTER_FEATURE_NULL_DESCRIPTORS)) {
                            return PAL_RESULT_CODE_FEATURE_NOT_SUPPORTED;
                        }
                    }

                } else if (info->descriptorType == PAL_DESCRIPTOR_TYPE_SAMPLED_IMAGE) {
                    if (info->imageViewInfos) {
                        PalDescriptorImageViewInfo* tmp = &info->imageViewInfos[j];
                        ImageViewVk* vkImageView = (ImageViewVk*)tmp->imageView;

                        imageInfo->sampler = nullptr;
                        imageInfo->imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
                        imageInfo->imageView = vkImageView->handle;

                    } else {
                        if (!(vkDevice->features & PAL_ADAPTER_FEATURE_NULL_DESCRIPTORS)) {
                            return PAL_RESULT_CODE_FEATURE_NOT_SUPPORTED;
                        }
                    }
                }
            }
        }

        if (info->descriptorType == PAL_DESCRIPTOR_TYPE_STORAGE_BUFFER ||
            info->descriptorType == PAL_DESCRIPTOR_TYPE_UNIFORM_BUFFER) {
            write->pBufferInfo = &bufferInfos[bufferCount];
            bufferCount += write->descriptorCount;

        } else if (info->descriptorType == PAL_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE) {
            VkWriteDescriptorSetAccelerationStructureKHR* tlasInfo = &tlasInfos[tlasInfoCount];
            tlasInfo->sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET_ACCELERATION_STRUCTURE_KHR;
            tlasInfo->accelerationStructureCount = info->descriptorCount;
            tlasInfo->pAccelerationStructures = &tlas[tlasCount];
            
            write->pNext = &tlasInfos[tlasInfoCount++];
            tlasCount += write->descriptorCount;

        } else {
            write->pImageInfo = &imageInfos[imageCount];
            imageCount += write->descriptorCount;
        }
    }

    s_Vk.updateDescriptorSet(vkDevice->handle, count, writes, 0, nullptr);
    palFree(s_Vk.allocator, writes);
    palFree(s_Vk.allocator, bufferInfos);
    palFree(s_Vk.allocator, imageInfos);
    palFree(s_Vk.allocator, tlasInfos);
    palFree(s_Vk.allocator, tlas);

    return PAL_RESULT_SUCCESS;
}

#endif // PAL_HAS_VULKAN_BACKEND