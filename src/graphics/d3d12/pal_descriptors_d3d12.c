
/**
    PAL - Prime Abstraction Layer
    Copyright (C) 2025
    Licensed under the Zlib license. See LICENSE file in root.
 */

#if PAL_HAS_D3D12_BACKEND
#include "pal_d3d12.h"

PalResult PAL_CALL createDescriptorSetLayoutD3D12(
    PalDevice* device,
    const PalDescriptorSetLayoutCreateInfo* info,
    PalDescriptorSetLayout** outLayout)
{
    HRESULT result;
    DeviceD3D12* d3d12Device = (DeviceD3D12*)device;
    DescriptorSetLayoutD3D12* layout = nullptr;
    DescriptorSetBinding* bindings = nullptr;
    uint32_t count = info->bindingCount;

    PalBool hasDescriptorIndexing = PAL_FALSE;
    if (d3d12Device->features & PAL_ADAPTER_FEATURE_DESCRIPTOR_INDEXING) {
        hasDescriptorIndexing = PAL_TRUE;
    }

    if (info->flags != 0 && !hasDescriptorIndexing) {
        return PAL_RESULT_CODE_FEATURE_NOT_SUPPORTED;
    }

    // partially bound is not supported
    if (info->flags & PAL_DESCRIPTOR_INDEXING_FLAG_PARTIALLY_BOUND) {
        return PAL_RESULT_CODE_FEATURE_NOT_SUPPORTED;
    }

    layout = palAllocate(s_D3D12.allocator, sizeof(DescriptorSetLayoutD3D12), 0);
    bindings = palAllocate(s_D3D12.allocator, sizeof(DescriptorSetBinding) * count, 0);
    if (!layout || !bindings) {
        return PAL_RESULT_CODE_OUT_OF_MEMORY;
    }

    D3D12_DESCRIPTOR_RANGE_FLAGS rangeFlags = D3D12_DESCRIPTOR_RANGE_FLAG_NONE;
    if (info->flags & PAL_DESCRIPTOR_INDEXING_FLAG_UPDATE_AFTER_BIND) {
        rangeFlags = D3D12_DESCRIPTOR_RANGE_FLAG_DESCRIPTORS_VOLATILE;
    }

    uint32_t resourceOffset = 0;
    uint32_t samplerOffset = 0;
    uint32_t SRVRegister = 0;
    uint32_t UAVRegister = 0;
    uint32_t CBVRegister = 0;

    uint32_t sampledImageCount = 0;
    uint32_t storageImageCount = 0;
    uint32_t storageBufferCount = 0;
    uint32_t uniformBufferCount = 0;
    uint32_t tlasCount = 0;
    uint32_t samplerCount = 0;

    for (int i = 0; i < count; i++) {
        DescriptorSetBinding* binding = &bindings[i];
        binding->type = info->bindings[i].descriptorType;

        binding->range.NumDescriptors = info->bindings[i].descriptorCount;
        binding->range.Flags = rangeFlags;
        binding->range.RegisterSpace = 0;

        switch (info->bindings[i].descriptorType) {
            case PAL_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE: {
                binding->range.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
                binding->range.BaseShaderRegister = SRVRegister;

                binding->range.OffsetInDescriptorsFromTableStart = resourceOffset;
                resourceOffset += info->bindings[i].descriptorCount;
                tlasCount += info->bindings[i].descriptorCount;
                SRVRegister += info->bindings[i].descriptorCount;

                break;
            }

            case PAL_DESCRIPTOR_TYPE_SAMPLED_IMAGE: {
                binding->range.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
                binding->range.BaseShaderRegister = SRVRegister;

                binding->range.OffsetInDescriptorsFromTableStart = resourceOffset;
                resourceOffset += info->bindings[i].descriptorCount;
                sampledImageCount += info->bindings[i].descriptorCount;
                SRVRegister += info->bindings[i].descriptorCount;
                break;
            }

            case PAL_DESCRIPTOR_TYPE_SAMPLER: {
                binding->range.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER;
                binding->range.BaseShaderRegister = samplerCount;

                binding->range.OffsetInDescriptorsFromTableStart = samplerOffset;
                samplerOffset += info->bindings[i].descriptorCount;
                samplerCount += info->bindings[i].descriptorCount;
                break;
            }

            case PAL_DESCRIPTOR_TYPE_STORAGE_BUFFER: {
                binding->range.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_UAV;
                binding->range.BaseShaderRegister = UAVRegister;

                binding->range.OffsetInDescriptorsFromTableStart = resourceOffset;
                resourceOffset += info->bindings[i].descriptorCount;
                storageBufferCount += info->bindings[i].descriptorCount;
                UAVRegister += info->bindings[i].descriptorCount;
                break;
            }

            case PAL_DESCRIPTOR_TYPE_STORAGE_IMAGE: {
                binding->range.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_UAV;
                binding->range.BaseShaderRegister = UAVRegister;

                binding->range.OffsetInDescriptorsFromTableStart = resourceOffset;
                resourceOffset += info->bindings[i].descriptorCount;
                storageImageCount += info->bindings[i].descriptorCount;
                UAVRegister += info->bindings[i].descriptorCount;
                break;
            }

            case PAL_DESCRIPTOR_TYPE_UNIFORM_BUFFER: {
                binding->range.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
                binding->range.BaseShaderRegister = CBVRegister;

                binding->range.OffsetInDescriptorsFromTableStart = resourceOffset;
                resourceOffset += info->bindings[i].descriptorCount;
                uniformBufferCount += info->bindings[i].descriptorCount;
                CBVRegister += info->bindings[i].descriptorCount;
                break;
            }
        }
    }

    // check limits
    if (sampledImageCount > d3d12Device->limits.maxDescriptorSampledImages) {
        return PAL_RESULT_CODE_INVALID_ARGUMENT;
    }

    if (storageImageCount > d3d12Device->limits.maxDescriptorStorageImages) {
        return PAL_RESULT_CODE_INVALID_ARGUMENT;
    }

    if (storageBufferCount > d3d12Device->limits.maxDescriptorStorageBuffers) {
        return PAL_RESULT_CODE_INVALID_ARGUMENT;
    }

    if (uniformBufferCount > d3d12Device->limits.maxDescriptorUniformBuffers) {
        return PAL_RESULT_CODE_INVALID_ARGUMENT;
    }

    if (tlasCount > d3d12Device->limits.maxDescriptorAccelerationStructures) {
        return PAL_RESULT_CODE_INVALID_ARGUMENT;
    }

    if (samplerCount > d3d12Device->limits.maxDescriptorSamplers) {
        return PAL_RESULT_CODE_INVALID_ARGUMENT;
    }

    layout->flags = info->flags;
    layout->bindingCount = info->bindingCount;
    layout->samplerCount = samplerCount;
    layout->bindings = bindings;
    layout->reserved = PAL_BACKEND_KEY;
    *outLayout = (PalDescriptorSetLayout*)layout;
    return PAL_RESULT_SUCCESS;
}

void PAL_CALL destroyDescriptorSetLayoutD3D12(PalDescriptorSetLayout* layout)
{
    DescriptorSetLayoutD3D12* d3d12Layout = (DescriptorSetLayoutD3D12*)layout;
    palFree(s_D3D12.allocator, d3d12Layout->bindings);
    palFree(s_D3D12.allocator, d3d12Layout);
}

PalResult PAL_CALL createDescriptorPoolD3D12(
    PalDevice* device,
    const PalDescriptorPoolCreateInfo* info,
    PalDescriptorPool** outPool)
{
    HRESULT result;
    DeviceD3D12* d3d12Device = (DeviceD3D12*)device;
    DescriptorPoolD3D12* pool = nullptr;

    PalBool hasDescriptorIndexing = PAL_FALSE;
    if (d3d12Device->features & PAL_ADAPTER_FEATURE_DESCRIPTOR_INDEXING) {
        hasDescriptorIndexing = PAL_TRUE;
    }

    if (info->flags != 0 && !hasDescriptorIndexing) {
        return PAL_RESULT_CODE_FEATURE_NOT_SUPPORTED;
    }

    // partially bound is not supported
    if (info->flags & PAL_DESCRIPTOR_INDEXING_FLAG_PARTIALLY_BOUND) {
        return PAL_RESULT_CODE_FEATURE_NOT_SUPPORTED;
    }

    pool = palAllocate(s_D3D12.allocator, sizeof(DescriptorPoolD3D12), 0);
    if (!pool) {
        return PAL_RESULT_CODE_OUT_OF_MEMORY;
    }

    memset(pool, 0, sizeof(DescriptorPoolD3D12));
    pool->sets = palAllocate(
        s_D3D12.allocator, 
        sizeof(DescriptorSetD3D12) * info->maxDescriptorSets, 
        0);

    if (!pool->sets) {
        return PAL_RESULT_CODE_OUT_OF_MEMORY;
    }

    uint32_t resourceCount = 0;
    DescriptorHeapLimits* limits = &pool->limits;
    for (int i = 0; i < info->bindingSizeCount; i++) {
        PalDescriptorPoolBindingSize* bindingSize = &info->bindingSizes[i];
        if (bindingSize->descriptorType == PAL_DESCRIPTOR_TYPE_SAMPLER) {
            limits->maxSamplers += bindingSize->bindingCount;

        } else if (bindingSize->descriptorType == PAL_DESCRIPTOR_TYPE_UNIFORM_BUFFER) {
            limits->maxUniformBuffers += bindingSize->bindingCount;
            resourceCount += bindingSize->bindingCount;

        } else if (bindingSize->descriptorType == PAL_DESCRIPTOR_TYPE_STORAGE_BUFFER) {
            limits->maxStorageBuffers += bindingSize->bindingCount;
            resourceCount += bindingSize->bindingCount;

        } else if (bindingSize->descriptorType == PAL_DESCRIPTOR_TYPE_SAMPLED_IMAGE) {
            limits->maxSampledImages += bindingSize->bindingCount;
            resourceCount += bindingSize->bindingCount;

        } else if (bindingSize->descriptorType == PAL_DESCRIPTOR_TYPE_STORAGE_IMAGE) {
            limits->maxStorageImages += bindingSize->bindingCount;
            resourceCount += bindingSize->bindingCount;

        } else if (bindingSize->descriptorType == PAL_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE) {
            limits->maxAs += bindingSize->bindingCount;
            resourceCount += bindingSize->bindingCount;
        }
    }

    // resource heap
    if (resourceCount) {
        DescriptorHeap* heap = &pool->resourceHeap;
        D3D12_CPU_DESCRIPTOR_HANDLE __ret, handle;
        D3D12_GPU_DESCRIPTOR_HANDLE __gpuRet, gpuHandle;

        D3D12_DESCRIPTOR_HEAP_DESC desc = {0};
        desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
        desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
        desc.NumDescriptors = resourceCount;

        result = d3d12Device->handle->lpVtbl->CreateDescriptorHeap(
            d3d12Device->handle,
            &desc,
            &IID_DescriptorHeap,
            (void**)&heap->handle);

        if (FAILED(result)) {
            return makeResultD3D12(result);
        }

        // get increment size
        heap->incrementSize = d3d12Device->handle->lpVtbl->GetDescriptorHandleIncrementSize(
            d3d12Device->handle,
            D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

        // get base CPU and GPU base pointer
        handle = *heap->handle->lpVtbl->GetCPUDescriptorHandleForHeapStart(
            heap->handle,
            &__ret);
        heap->cpuBase = handle.ptr;

        gpuHandle = *heap->handle->lpVtbl->GetGPUDescriptorHandleForHeapStart(
            heap->handle,
            &__gpuRet);
        heap->gpuBase = gpuHandle.ptr;

        pool->hasResourceHeap = PAL_TRUE;
    }

    // sampler heap
    if (limits->maxSamplers) {
        DescriptorHeap* heap = &pool->samplerHeap;
        D3D12_CPU_DESCRIPTOR_HANDLE __ret, handle;
        D3D12_GPU_DESCRIPTOR_HANDLE __gpuRet, gpuHandle;

        D3D12_DESCRIPTOR_HEAP_DESC desc = {0};
        desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
        desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER;
        desc.NumDescriptors = limits->maxSamplers;

        result = d3d12Device->handle->lpVtbl->CreateDescriptorHeap(
            d3d12Device->handle,
            &desc,
            &IID_DescriptorHeap,
            (void**)&heap->handle);

        if (FAILED(result)) {
            return makeResultD3D12(result);
        }

        // get increment size
        heap->incrementSize = d3d12Device->handle->lpVtbl->GetDescriptorHandleIncrementSize(
            d3d12Device->handle,
            D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER);

        // get base CPU and GPU base pointer
        handle = *heap->handle->lpVtbl->GetCPUDescriptorHandleForHeapStart(
            heap->handle,
            &__ret);
        heap->cpuBase = handle.ptr;

        gpuHandle = *heap->handle->lpVtbl->GetGPUDescriptorHandleForHeapStart(
            heap->handle,
            &__gpuRet);
        heap->gpuBase = gpuHandle.ptr;

        pool->hasSamplerHeap = PAL_TRUE;
    }

    pool->flags = info->flags;
    pool->maxSets = info->maxDescriptorSets;
    pool->reserved = PAL_BACKEND_KEY;
    *outPool = (PalDescriptorPool*)pool;
    return PAL_RESULT_SUCCESS;
}

void PAL_CALL destroyDescriptorPoolD3D12(PalDescriptorPool* pool)
{
    DescriptorPoolD3D12* d3d12Pool = (DescriptorPoolD3D12*)pool;
    if (d3d12Pool->hasResourceHeap) {
        d3d12Pool->resourceHeap.handle->lpVtbl->Release(d3d12Pool->resourceHeap.handle);
    }

    if (d3d12Pool->hasSamplerHeap) {
        d3d12Pool->samplerHeap.handle->lpVtbl->Release(d3d12Pool->samplerHeap.handle);
    }

    palFree(s_D3D12.allocator, d3d12Pool->sets);
    palFree(s_D3D12.allocator, d3d12Pool);
}

PalResult PAL_CALL resetDescriptorPoolD3D12(PalDescriptorPool* pool)
{
    DescriptorPoolD3D12* d3d12Pool = (DescriptorPoolD3D12*)pool;
    d3d12Pool->resourceHeap.nextOffset = 0;
    d3d12Pool->samplerHeap.nextOffset = 0;
    d3d12Pool->usedSets = 0;

    d3d12Pool->limits.usedAs = 0;
    d3d12Pool->limits.usedSampledImages = 0;
    d3d12Pool->limits.usedSamplers = 0;
    d3d12Pool->limits.usedStorageBuffers = 0;
    d3d12Pool->limits.usedStorageImages = 0;
    d3d12Pool->limits.usedUniformBuffers = 0;
    return PAL_RESULT_SUCCESS;
}

PalResult PAL_CALL allocateDescriptorSetD3D12(
    PalDevice* device,
    PalDescriptorPool* pool,
    PalDescriptorSetLayout* layout,
    PalDescriptorSet** outSet)
{
    DeviceD3D12* d3d12Device = (DeviceD3D12*)device;
    DescriptorPoolD3D12* d3d12Pool = (DescriptorPoolD3D12*)pool;
    DescriptorSetLayoutD3D12* d3d12Layout = (DescriptorSetLayoutD3D12*)layout;
    DescriptorSetD3D12* set = nullptr;

    uint32_t storageImageCount = 0;
    uint32_t samplerCount = 0;
    uint32_t storageBufferCount = 0;
    uint32_t uniformBufferCount = 0;
    uint32_t sampledImageCount = 0;
    uint32_t tlasCount = 0;

    PalBool isPoolValid = d3d12Pool->flags & PAL_DESCRIPTOR_INDEXING_FLAG_UPDATE_AFTER_BIND;
    PalBool isLayoutValid = d3d12Layout->flags & PAL_DESCRIPTOR_INDEXING_FLAG_UPDATE_AFTER_BIND;
    if (isPoolValid != isLayoutValid) {
        // we check if both are true or false
        return PAL_RESULT_CODE_INVALID_OPERATION;
    }

    // get requirements for the sets using the provided layout
    for (int i = 0; i < d3d12Layout->bindingCount; i++) {
        DescriptorSetBinding* binding = &d3d12Layout->bindings[i];

        if (binding->type == PAL_DESCRIPTOR_TYPE_SAMPLER) {
            samplerCount += binding->range.NumDescriptors;

        } else if (binding->type == PAL_DESCRIPTOR_TYPE_STORAGE_BUFFER) {
            storageBufferCount += binding->range.NumDescriptors;

        } else if (binding->type == PAL_DESCRIPTOR_TYPE_STORAGE_IMAGE) {
            storageImageCount += binding->range.NumDescriptors;

        } else if (binding->type == PAL_DESCRIPTOR_TYPE_UNIFORM_BUFFER) {
            uniformBufferCount += binding->range.NumDescriptors;

        } else if (binding->type == PAL_DESCRIPTOR_TYPE_SAMPLED_IMAGE) {
            sampledImageCount += binding->range.NumDescriptors;

        } else if (binding->type == PAL_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE) {
            tlasCount += binding->range.NumDescriptors;
        }
    }

    // validate descriptor sets limits
    if (d3d12Pool->usedSets + 1 > d3d12Pool->maxSets) {
        // all sets are used
        return PAL_RESULT_CODE_OUT_OF_MEMORY;
    }

    // check descriptor limits
    DescriptorHeapLimits* limits = &d3d12Pool->limits;
    if (limits->usedAs + tlasCount > limits->maxAs) {
        return PAL_RESULT_CODE_OUT_OF_MEMORY;
    }

    if (limits->usedSampledImages + sampledImageCount > limits->maxSampledImages) {
        return PAL_RESULT_CODE_OUT_OF_MEMORY;
    }

    if (limits->usedSamplers + samplerCount > limits->maxSamplers) {
        return PAL_RESULT_CODE_OUT_OF_MEMORY;
    }

    if (limits->usedStorageBuffers + storageBufferCount > limits->maxStorageBuffers) {
        return PAL_RESULT_CODE_OUT_OF_MEMORY;
    }

    if (limits->usedStorageImages + storageImageCount > limits->maxStorageImages) {
        return PAL_RESULT_CODE_OUT_OF_MEMORY;
    }

    if (limits->usedUniformBuffers + uniformBufferCount > limits->maxUniformBuffers) {
        return PAL_RESULT_CODE_OUT_OF_MEMORY;
    }

    // assign offset base to the set so we know where to start and end for each set.
    set = &d3d12Pool->sets[d3d12Pool->usedSets++];
    set->resourceOffset = d3d12Pool->resourceHeap.nextOffset;
    set->samplerOffset = d3d12Pool->samplerHeap.nextOffset;
    set->layout = d3d12Layout;
    set->pool = d3d12Pool;

    uint32_t totalDescriptors = tlasCount + storageBufferCount + uniformBufferCount;
    totalDescriptors += sampledImageCount + storageImageCount;
    d3d12Pool->resourceHeap.nextOffset += totalDescriptors;
    d3d12Pool->samplerHeap.nextOffset += samplerCount;

    limits->usedAs += tlasCount;
    limits->usedSampledImages += sampledImageCount;
    limits->usedSamplers += samplerCount;
    limits->usedStorageBuffers += storageBufferCount;
    limits->usedUniformBuffers += uniformBufferCount;

    set->reserved = PAL_BACKEND_KEY;
    *outSet = (PalDescriptorSet*)set;
    return PAL_RESULT_SUCCESS;
}

PalResult PAL_CALL updateDescriptorSetD3D12(
    PalDevice* device,
    uint32_t count,
    PalDescriptorSetWriteInfo* infos)
{
    DeviceD3D12* d3d12Device = (DeviceD3D12*)device;
    for (int i = 0; i < count; i++) {
        PalDescriptorSetWriteInfo* info = &infos[i];
        DescriptorSetD3D12* set = (DescriptorSetD3D12*)info->descriptorSet;
        DescriptorPoolD3D12* pool = set->pool;
        DescriptorSetLayoutD3D12* layout = set->layout;

        if (info->layoutBindingIndex > layout->bindingCount) {
            return PAL_RESULT_CODE_INVALID_ARGUMENT;
        }

        DescriptorSetBinding* binding = &layout->bindings[info->layoutBindingIndex];
        DescriptorHeap* heap = nullptr;
        uint32_t index = 0;
        uint32_t bindingOffset = binding->range.OffsetInDescriptorsFromTableStart;

        if (binding->type == PAL_DESCRIPTOR_TYPE_SAMPLER) {
            heap = &pool->samplerHeap;
            index = set->samplerOffset + bindingOffset + info->arrayElement;

        } else {
            heap = &pool->resourceHeap;
            index = set->resourceOffset + bindingOffset + info->arrayElement;
        }

        for (int j = 0; j < info->descriptorCount; j++) {
            D3D12_CPU_DESCRIPTOR_HANDLE dst;
            dst.ptr = getDescriptorHandleD3D12(index + j, heap->incrementSize, heap->cpuBase);

            if (info->descriptorType == PAL_DESCRIPTOR_TYPE_SAMPLER) {
                if (info->samplerInfos) {
                    SamplerD3D12* sampler = (SamplerD3D12*)info->samplerInfos[j].sampler;
                    d3d12Device->handle->lpVtbl->CreateSampler(d3d12Device->handle, &sampler->desc, dst);

                } else {
                    if (!(d3d12Device->features & PAL_ADAPTER_FEATURE_NULL_DESCRIPTORS)) {
                        return PAL_RESULT_CODE_FEATURE_NOT_SUPPORTED;
                    }

                    D3D12_SAMPLER_DESC desc = {0};
                    desc.MaxAnisotropy = 1;
                    desc.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
                    desc.BorderColor[3] = 1.0f;

                    desc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
                    desc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
                    desc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
                    desc.Filter = D3D12_FILTER_MIN_MAG_MIP_POINT;
                    d3d12Device->handle->lpVtbl->CreateSampler(d3d12Device->handle, &desc, dst);
                }

            } else if (info->descriptorType == PAL_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE) {
                D3D12_SHADER_RESOURCE_VIEW_DESC desc = {0};
                desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
                desc.ViewDimension = D3D12_SRV_DIMENSION_RAYTRACING_ACCELERATION_STRUCTURE;

                if (info->tlasInfos) {
                    AccelerationStructureD3D12* tlas = nullptr;
                    tlas = (AccelerationStructureD3D12*)info->tlasInfos[j].tlas;
                    desc.RaytracingAccelerationStructure.Location = tlas->address;

                } else {
                    if (!(d3d12Device->features & PAL_ADAPTER_FEATURE_NULL_DESCRIPTORS)) {
                        return PAL_RESULT_CODE_FEATURE_NOT_SUPPORTED;
                    }
                }

                d3d12Device->handle->lpVtbl->CreateShaderResourceView(
                    d3d12Device->handle,
                    nullptr,
                    &desc,
                    dst);

            } else if (info->descriptorType == PAL_DESCRIPTOR_TYPE_SAMPLED_IMAGE) {
                D3D12_SHADER_RESOURCE_VIEW_DESC desc = {0};
                desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
            
                PalImageSubresourceRange range = {0};
                PalImageViewType type;
                ID3D12Resource* handle = nullptr;

                if (info->imageViewInfos) {
                    ImageViewD3D12* imageView = (ImageViewD3D12*)info->imageViewInfos[j].imageView;
                    desc.Format = imageView->format;
                    type = imageView->type;
                    range = imageView->range;
                    handle = imageView->image->handle;

                } else {
                    if (!(d3d12Device->features & PAL_ADAPTER_FEATURE_NULL_DESCRIPTORS)) {
                        return PAL_RESULT_CODE_FEATURE_NOT_SUPPORTED;
                    }

                    desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
                    type = PAL_IMAGE_VIEW_TYPE_2D;
                    range.mipLevelCount = 1;
                    range.layerArrayCount = 1;
                    range.aspect = PAL_IMAGE_ASPECT_COLOR;
                }

                fillSubresourceD3D12(
                    type,
                    &range,
                    nullptr,
                    nullptr,
                    &desc,
                    nullptr);

                d3d12Device->handle->lpVtbl->CreateShaderResourceView(
                    d3d12Device->handle,
                    handle,
                    &desc,
                    dst);

            } else if (info->descriptorType == PAL_DESCRIPTOR_TYPE_STORAGE_IMAGE) {
                D3D12_UNORDERED_ACCESS_VIEW_DESC desc = {0};
                PalImageSubresourceRange range = {0};
                PalImageViewType type;
                ID3D12Resource* handle = nullptr;

                if (info->imageViewInfos) {
                    ImageViewD3D12* imageView = (ImageViewD3D12*)info->imageViewInfos[j].imageView;
                    desc.Format = imageView->format;
                    type = imageView->type;
                    range = imageView->range;
                    handle = imageView->image->handle;

                } else {
                    if (!(d3d12Device->features & PAL_ADAPTER_FEATURE_NULL_DESCRIPTORS)) {
                        return PAL_RESULT_CODE_FEATURE_NOT_SUPPORTED;
                    }

                    desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
                    type = PAL_IMAGE_VIEW_TYPE_2D;
                    range.mipLevelCount = 1;
                    range.layerArrayCount = 1;
                    range.aspect = PAL_IMAGE_ASPECT_COLOR;
                }

                fillSubresourceD3D12(
                    type,
                    &range,
                    nullptr,
                    nullptr,
                    nullptr,
                    &desc);

                d3d12Device->handle->lpVtbl->CreateUnorderedAccessView(
                    d3d12Device->handle,
                    handle,
                    nullptr,
                    &desc,
                    dst);

            } else if (info->descriptorType == PAL_DESCRIPTOR_TYPE_UNIFORM_BUFFER) {
                D3D12_CONSTANT_BUFFER_VIEW_DESC desc = {0};
                D3D12_GPU_VIRTUAL_ADDRESS address = 0;

                if (info->bufferInfos) {
                    PalDescriptorBufferInfo* bufferInfo = &info->bufferInfos[j];
                    BufferD3D12* buffer = (BufferD3D12*)bufferInfo->buffer;
                    desc.SizeInBytes = bufferInfo->size;
                    address = buffer->handle->lpVtbl->GetGPUVirtualAddress(buffer->handle);
                    desc.BufferLocation = address + bufferInfo->offset;

                } else {
                    if (!(d3d12Device->features & PAL_ADAPTER_FEATURE_NULL_DESCRIPTORS)) {
                        return PAL_RESULT_CODE_FEATURE_NOT_SUPPORTED;
                    }
                }

                d3d12Device->handle->lpVtbl->CreateConstantBufferView(
                    d3d12Device->handle,
                    &desc,
                    dst);

            } else {
                // storage buffer
                D3D12_UNORDERED_ACCESS_VIEW_DESC desc = {0};
                desc.ViewDimension = D3D12_UAV_DIMENSION_BUFFER;

                ID3D12Resource* handle = nullptr;
                if (info->bufferInfos) {
                    PalDescriptorBufferInfo* bufferInfo = &info->bufferInfos[j];
                    BufferD3D12* buffer = (BufferD3D12*)bufferInfo->buffer;

                    uint32_t stride = 4;
                    if (bufferInfo->stride) {
                        stride = bufferInfo->stride;
                        desc.Buffer.StructureByteStride = bufferInfo->stride;
                    } else {
                        desc.Format = DXGI_FORMAT_R32_TYPELESS;
                        desc.Buffer.Flags = D3D12_BUFFER_UAV_FLAG_RAW;
                    }

                    handle = buffer->handle;
                    desc.Buffer.FirstElement = bufferInfo->offset / stride;
                    desc.Buffer.NumElements = bufferInfo->size / stride;

                } else {
                    if (!(d3d12Device->features & PAL_ADAPTER_FEATURE_NULL_DESCRIPTORS)) {
                        return PAL_RESULT_CODE_FEATURE_NOT_SUPPORTED;
                    }
                }            

                d3d12Device->handle->lpVtbl->CreateUnorderedAccessView(
                    d3d12Device->handle,
                    handle,
                    nullptr,
                    &desc,
                    dst);
            }
        }
    }
    return PAL_RESULT_SUCCESS;
}

#endif // PAL_HAS_D3D12_BACKEND