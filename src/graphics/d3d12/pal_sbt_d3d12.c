
/**
    PAL - Prime Abstraction Layer
    Copyright (C) 2025
    Licensed under the Zlib license. See LICENSE file in root.
 */

#if PAL_HAS_D3D12_BACKEND
#include "pal_d3d12.h"

#define align(v, a) (v + a - 1) & ~(a - 1)

PalResult PAL_CALL createShaderBindingTableD3D12(
    PalDevice* device,
    const PalShaderBindingTableCreateInfo* info,
    PalShaderBindingTable** outSbt)
{
    HRESULT result;
    DeviceD3D12* d3d12Device = (DeviceD3D12*)device;
    ShaderBindingTableD3D12* sbt = nullptr;
    PipelineD3D12* pipeline = (PipelineD3D12*)info->rayTracingPipeline;
    ShaderBindingTableInfo* sbtInfo = &pipeline->sbtInfo;

    if (!(d3d12Device->features & PAL_ADAPTER_FEATURE_RAY_TRACING)) {
        return PAL_RESULT_CODE_FEATURE_NOT_SUPPORTED;
    }

    uint32_t totalGroups = sbtInfo->raygenCount + sbtInfo->hitCount;
    totalGroups += sbtInfo->missCount + sbtInfo->callableCount;
    if (info->recordCount != totalGroups) {
        return PAL_RESULT_CODE_INVALID_ARGUMENT;
    }

    void** raygenHandles = nullptr;
    void** missHandles = nullptr;
    void** hitHandles = nullptr;
    void** callableHandles = nullptr;

    sbt = palAllocate(s_D3D12.allocator, sizeof(ShaderBindingTableD3D12), 0);
    if (!sbt) {
        return PAL_RESULT_CODE_OUT_OF_MEMORY;
    }

    memset(sbt, 0, sizeof(ShaderBindingTableD3D12));
    if (sbtInfo->raygenCount) {
        raygenHandles = palAllocate(s_D3D12.allocator, sizeof(void*) * sbtInfo->raygenCount, 0);
        if (!raygenHandles) {
            return PAL_RESULT_CODE_OUT_OF_MEMORY;
        }
    }

    if (sbtInfo->missCount) {
        missHandles = palAllocate(s_D3D12.allocator, sizeof(void*) * sbtInfo->missCount, 0);
        if (!missHandles) {
            return PAL_RESULT_CODE_OUT_OF_MEMORY;
        }
    }

    if (sbtInfo->hitCount) {
        hitHandles = palAllocate(s_D3D12.allocator, sizeof(void*) * sbtInfo->hitCount, 0);
        if (!hitHandles) {
            return PAL_RESULT_CODE_OUT_OF_MEMORY;
        }
    }

    if (sbtInfo->callableCount) {
        callableHandles = palAllocate(s_D3D12.allocator, sizeof(void*) * sbtInfo->callableCount, 0);
        if (!callableHandles) {
            return PAL_RESULT_CODE_OUT_OF_MEMORY;
        }
    }

    // create SBT buffer
    ID3D12StateObject* handle = pipeline->handle;
    ID3D12StateObjectProperties* props = NULL;
    result = handle->lpVtbl->QueryInterface(handle, &IID_StateObjectProps, (void**)&props);
    if (FAILED(result)) {
        return makeResultD3D12(result);
    }

    uint32_t groupHandleSize = D3D12_SHADER_IDENTIFIER_SIZE_IN_BYTES;
    uint32_t groupHandleAlignment = D3D12_RAYTRACING_SHADER_RECORD_BYTE_ALIGNMENT;
    uint32_t groupBaseAlignment = D3D12_RAYTRACING_SHADER_TABLE_BYTE_ALIGNMENT;

    // get the max local data size
    for (int i = 0; i < info->recordCount; i++) {
        PalShaderBindingTableRecordInfo* record = &info->records[i];
        uint32_t index = record->groupIndex;

        if (index < sbtInfo->raygenCount) {
            // raygen group
            if (record->localDataSize > sbtInfo->raygenDataSize)  {
                return PAL_RESULT_CODE_INVALID_ARGUMENT;
            }

        } else if (index < sbtInfo->raygenCount + sbtInfo->missCount) {
            // miss group
            if (record->localDataSize > sbtInfo->missDataSize)  {
                return PAL_RESULT_CODE_INVALID_ARGUMENT;
            }

        } else if (index < sbtInfo->raygenCount + sbtInfo->missCount + sbtInfo->hitCount) {
            // hit group
            if (record->localDataSize > sbtInfo->hitDataSize) {
                return PAL_RESULT_CODE_INVALID_ARGUMENT;
            }

        } else {
            // callable group
            if (record->localDataSize > sbtInfo->callableDataSize) {
                return PAL_RESULT_CODE_INVALID_ARGUMENT;
            }
        }
    }

    // get strides
    uint32_t raygenStride = 0;
    uint32_t missStride = 0;
    uint32_t hitStride = 0;
    uint32_t callableStride = 0;

    raygenStride = align(groupHandleSize + sbtInfo->raygenDataSize, groupHandleAlignment);
    missStride = align(groupHandleSize + sbtInfo->missDataSize, groupHandleAlignment);
    hitStride = align(groupHandleSize + sbtInfo->hitDataSize, groupHandleAlignment);
    callableStride = align(groupHandleSize + sbtInfo->callableDataSize, groupHandleAlignment);

    // get region size
    uint32_t raygenRegionSize = raygenStride * sbtInfo->raygenCount;
    uint32_t missRegionSize = missStride * sbtInfo->missCount;
    uint32_t hitRegionSize = hitStride * sbtInfo->hitCount;
    uint32_t callableRegionSize = callableStride * sbtInfo->callableCount;

    // get offsets
    uint32_t offset = 0;
    uint32_t raygenOffset = 0;
    uint32_t missOffset = 0;
    uint32_t hitOffset = 0;
    uint32_t callableOffset = 0;

    raygenOffset = align(offset, groupBaseAlignment);
    offset = raygenOffset + raygenRegionSize;

    missOffset = align(offset, groupBaseAlignment);
    offset = missOffset + missRegionSize;

    hitOffset = align(offset, groupBaseAlignment);
    offset = hitOffset + hitRegionSize;

    callableOffset = align(offset, groupBaseAlignment);
    offset = callableOffset + callableRegionSize;
    uint32_t bufferSize = align(offset, groupBaseAlignment);

    // create gpu buffer
    D3D12_HEAP_PROPERTIES heapProps = {0};
    heapProps.Type = D3D12_HEAP_TYPE_DEFAULT;

    D3D12_RESOURCE_DESC bufferDesc = {0};
    bufferDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
    bufferDesc.Width = bufferSize;
    bufferDesc.Height = 1;
    bufferDesc.DepthOrArraySize = 1;
    bufferDesc.MipLevels = 1;
    bufferDesc.SampleDesc.Count = 1;
    bufferDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

    result = d3d12Device->handle->lpVtbl->CreateCommittedResource(
        d3d12Device->handle, 
        &heapProps, 
        0, 
        &bufferDesc,
        D3D12_RESOURCE_STATE_COPY_DEST,
        nullptr,
        &IID_Resource,
        (void**)&sbt->buffer);

    if (FAILED(result)) {
        return makeResultD3D12(result);
    }

    // create staging buffer
    heapProps.Type = D3D12_HEAP_TYPE_UPLOAD;
    result = d3d12Device->handle->lpVtbl->CreateCommittedResource(
        d3d12Device->handle, 
        &heapProps, 
        0, 
        &bufferDesc,
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        &IID_Resource,
        (void**)&sbt->stagingBuffer);

    if (FAILED(result)) {
        return makeResultD3D12(result);
    }

    // get shader group handles
    uint32_t raygenIndex = 0;
    uint32_t missIndex = 0;
    uint32_t hitIndex = 0;
    uint32_t callableIndex = 0;

    for (int i = 0; i < pipeline->shaderExportCount; i++) {
        ShaderExport* tmp = &pipeline->shaderExports[i];
        PalShaderStage stage = tmp->stage;

        // skip any hit, closest hit, intersection shaders without isHitGroup PalBool
        PalBool isHitGroup = PAL_FALSE;
        if (stage == PAL_SHADER_STAGE_ANY_HIT ||
            stage == PAL_SHADER_STAGE_CLOSEST_HIT ||
            stage == PAL_SHADER_STAGE_INTERSECTION) {
            if (tmp->isHitGroup) {
                isHitGroup = PAL_TRUE;

            } else {
                continue;
            }
        }

        void* handle = props->lpVtbl->GetShaderIdentifier(props, tmp->entryName);
        if (stage == PAL_SHADER_STAGE_RAYGEN) {
            raygenHandles[raygenIndex++] = handle;

        } else if (stage == PAL_SHADER_STAGE_MISS) {
            missHandles[missIndex++] = handle;

        } else if (isHitGroup) {
            hitHandles[hitIndex++] = handle;

        } else if (stage == PAL_SHADER_STAGE_CALLABLE) {
            callableHandles[callableIndex++] = handle;
        }
    }

    // copy handles into the buffer
    offset = 0; // reuse variable
    void* ptr = nullptr;
    result = sbt->stagingBuffer->lpVtbl->Map(sbt->stagingBuffer, 0, nullptr, &ptr);
    if (FAILED(result)) {
        return makeResultD3D12(result);
    }

    // raygen
    for (int i = 0; i < sbtInfo->raygenCount; i++) {
        uint8_t* dstPtr = (uint8_t*)ptr + (i * raygenStride);
        PalShaderBindingTableRecordInfo* record = &info->records[offset + i];

        memcpy(dstPtr, raygenHandles[i], groupHandleSize);
        if (record->localDataSize) {
            // this record has local data
            memcpy(dstPtr + groupHandleSize, record->localData, record->localDataSize);
        }
    }
    offset += sbtInfo->raygenCount;

    // miss
    for (int i = 0; i < sbtInfo->missCount; i++) {
        uint8_t* dstPtr = (uint8_t*)ptr + missOffset + (i * missStride);
        PalShaderBindingTableRecordInfo* record = &info->records[offset + i];

        memcpy(dstPtr, missHandles[i], groupHandleSize);
        if (record->localDataSize) {
            // this record has local data
            memcpy(dstPtr + groupHandleSize, record->localData, record->localDataSize);
        }
    }
    offset += sbtInfo->missCount;

    // hit group
    for (int i = 0; i < sbtInfo->hitCount; i++) {
        uint8_t* dstPtr = (uint8_t*)ptr + hitOffset + (i * hitStride);
        PalShaderBindingTableRecordInfo* record = &info->records[offset + i];

        memcpy(dstPtr, hitHandles[i], groupHandleSize);
        if (record->localDataSize) {
            // this record has local data
            memcpy(dstPtr + groupHandleSize, record->localData, record->localDataSize);
        }
    }
    offset += sbtInfo->hitCount;

    // callable
    for (int i = 0; i < sbtInfo->callableCount; i++) {
        uint8_t* dstPtr = (uint8_t*)ptr + callableOffset + (i * callableStride);
        PalShaderBindingTableRecordInfo* record = &info->records[offset + i];

        memcpy(dstPtr, callableHandles[i], groupHandleSize);
        if (record->localDataSize) {
            // this record has local data
            memcpy(dstPtr + groupHandleSize, record->localData, record->localDataSize);
        }
    }

    sbt->stagingBuffer->lpVtbl->Unmap(sbt->stagingBuffer, 0, nullptr);
    sbt->baseAddress = sbt->buffer->lpVtbl->GetGPUVirtualAddress(sbt->buffer);

    // raygen
    if (sbtInfo->raygenCount) {
        sbt->raygen.region.StartAddress = sbt->baseAddress;
        sbt->raygen.offset = 0; // always
        sbt->raygen.startIndex = 0; // always
        sbt->raygen.region.SizeInBytes = raygenRegionSize;
        sbt->raygen.region.StrideInBytes = raygenStride;

        palFree(s_D3D12.allocator, raygenHandles);
    }
    
    // miss
    if (sbtInfo->missCount) {
        sbt->miss.offset = missOffset;
        sbt->miss.startIndex = sbtInfo->raygenCount;
        sbt->miss.region.StartAddress = sbt->baseAddress + missOffset;
        sbt->miss.region.SizeInBytes = missRegionSize;
        sbt->miss.region.StrideInBytes = missStride;

        palFree(s_D3D12.allocator, missHandles);
    }

    // hit
    if (sbtInfo->hitCount) {
        sbt->hit.offset = hitOffset;
        sbt->hit.startIndex = sbtInfo->raygenCount + sbtInfo->missCount;
        sbt->hit.region.StartAddress = sbt->baseAddress + hitOffset;
        sbt->hit.region.SizeInBytes = hitRegionSize;
        sbt->hit.region.StrideInBytes = hitStride;

        palFree(s_D3D12.allocator, hitHandles);
    }

    // callable
    if (sbtInfo->callableCount) {
        sbt->callable.offset = callableOffset;
        sbt->callable.startIndex = sbtInfo->raygenCount + sbtInfo->missCount + sbtInfo->hitCount;
        sbt->callable.region.StartAddress = sbt->baseAddress + callableOffset;
        sbt->callable.region.SizeInBytes = callableRegionSize;
        sbt->callable.region.StrideInBytes = callableStride;

        palFree(s_D3D12.allocator, callableHandles);
    }

    props->lpVtbl->Release(props);
    sbt->handleSize = groupHandleSize;
    sbt->stagingBufferSize = bufferSize;
    sbt->pipeline = pipeline;

    sbt->reserved = PAL_BACKEND_KEY;
    sbt->isDirty = PAL_TRUE; // we need to copy from the staging to the gpu buffer
    *outSbt = (PalShaderBindingTable*)sbt;
    return PAL_RESULT_SUCCESS;
}

void PAL_CALL destroyShaderBindingTableD3D12(PalShaderBindingTable* sbt)
{
    ShaderBindingTableD3D12* d3d12Sbt = (ShaderBindingTableD3D12*)sbt;
    d3d12Sbt->buffer->lpVtbl->Release(d3d12Sbt->buffer);
    d3d12Sbt->stagingBuffer->lpVtbl->Release(d3d12Sbt->stagingBuffer);
    palFree(s_D3D12.allocator, d3d12Sbt);
}

PalResult PAL_CALL updateShaderBindingTableD3D12(
    PalShaderBindingTable* sbt, 
    uint32_t count,
    PalShaderBindingTableRecordInfo* infos)
{
    HRESULT result;
    ShaderBindingTableD3D12* d3d12Sbt = (ShaderBindingTableD3D12*)sbt;
    PipelineD3D12* pipeline = d3d12Sbt->pipeline;
    ShaderBindingTableInfo* sbtInfo = &pipeline->sbtInfo;

    void* data = nullptr;
    d3d12Sbt->stagingBuffer->lpVtbl->Map(d3d12Sbt->stagingBuffer, 0, nullptr, &data);
    if (FAILED(result)) {
        return makeResultD3D12(result);
    }

    uint64_t stride = 0;
    uint64_t offset = 0;
    uint32_t startIndex = 0;

    for (int i = 0; i < count; i++) {
        PalShaderBindingTableRecordInfo* info = &infos[i];
        if (!info->localDataSize) {
            return PAL_RESULT_CODE_INVALID_ARGUMENT;
        }

        // find the group the record belongs to
        uint32_t index = info->groupIndex;
        if (index < sbtInfo->raygenCount) {
            // raygen group
            offset = 0;
            stride = d3d12Sbt->raygen.region.StrideInBytes;
            startIndex = d3d12Sbt->raygen.startIndex;

        } else if (index < sbtInfo->raygenCount + sbtInfo->missCount) {
            // miss group
            offset = d3d12Sbt->miss.offset;
            stride = d3d12Sbt->miss.region.StrideInBytes;
            startIndex = d3d12Sbt->miss.startIndex;

        } else if (index < sbtInfo->raygenCount + sbtInfo->missCount + sbtInfo->hitCount) {
            // hit group
            offset = d3d12Sbt->hit.offset;
            stride = d3d12Sbt->hit.region.StrideInBytes;
            startIndex = d3d12Sbt->hit.startIndex;

        } else {
            // callable group
            offset = d3d12Sbt->callable.offset;
            stride = d3d12Sbt->callable.region.StrideInBytes;
            startIndex = d3d12Sbt->callable.startIndex;
        }

        // write payload
        uint32_t localIndex = index - startIndex;
        uint8_t* dst = (uint8_t*)data + offset + (localIndex * stride);
        memcpy(dst + d3d12Sbt->handleSize, info->localData, info->localDataSize);
    }

    d3d12Sbt->stagingBuffer->lpVtbl->Unmap(d3d12Sbt->stagingBuffer, 0, nullptr);
    d3d12Sbt->isDirty = PAL_TRUE;
    return PAL_RESULT_SUCCESS;
}

#endif // PAL_HAS_D3D12_BACKEND