
/**
    PAL - Prime Abstraction Layer
    Copyright (C) 2025
    Licensed under the Zlib license. See LICENSE file in root.
 */

#if PAL_HAS_D3D12_BACKEND
#include "pal_d3d12.h"

PalResult PAL_CALL createAccelerationstructureD3D12(
    PalDevice* device,
    const PalAccelerationStructureCreateInfo* info,
    PalAccelerationStructure** outAs)
{
    HRESULT result;
    AccelerationStructureD3D12* as = nullptr;
    DeviceD3D12* deviceImpl = (DeviceD3D12*)device;
    BufferD3D12* bufferImpl = (BufferD3D12*)info->buffer;

    as = palAllocate(s_D3D12.allocator, sizeof(AccelerationStructureD3D12), 0);
    if (!as) {
        return PAL_RESULT_CODE_OUT_OF_MEMORY;
    }

    as->type = info->type;
    as->handle = bufferImpl->handle;
    as->address = as->handle->lpVtbl->GetGPUVirtualAddress(as->handle);
    as->address += info->offset;
    *outAs = (PalAccelerationStructure*)as;
    return PAL_RESULT_SUCCESS;
}

void PAL_CALL destroyAccelerationstructureD3D12(PalAccelerationStructure* as)
{
    AccelerationStructureD3D12* asImpl = (AccelerationStructureD3D12*)as;
    palFree(s_D3D12.allocator, asImpl);
}

void PAL_CALL getAccelerationStructureBuildSizeD3D12(
    PalDevice* device,
    PalAccelerationStructureBuildInfo* info,
    PalAccelerationStructureBuildSize* size)
{
    DeviceD3D12* deviceImpl = (DeviceD3D12*)device;
    D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_DESC buildInfo = {0};
    D3D12_RAYTRACING_ACCELERATION_STRUCTURE_PREBUILD_INFO sizeInfo = {0};

    if (info->type == PAL_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL) {
        D3D12_RAYTRACING_GEOMETRY_DESC* geometries = nullptr;
        // clang-format off
        geometries = palAllocate(
            s_D3D12.allocator, 
            sizeof(D3D12_RAYTRACING_GEOMETRY_DESC) * info->count, 
            0);
        // clang-format on

        if (!geometries) {
            return;
        }

        memset(geometries, 0, sizeof(D3D12_RAYTRACING_GEOMETRY_DESC) * info->count);
        fillBuildInfoD3D12(PAL_TRUE, info, geometries, &buildInfo);

        deviceImpl->handle->lpVtbl->GetRaytracingAccelerationStructurePrebuildInfo(
            deviceImpl->handle,
            &buildInfo.Inputs,
            &sizeInfo);

        palFree(s_D3D12.allocator, geometries);

    } else {
        fillBuildInfoD3D12(PAL_TRUE, info, nullptr, &buildInfo);

        deviceImpl->handle->lpVtbl->GetRaytracingAccelerationStructurePrebuildInfo(
            deviceImpl->handle,
            &buildInfo.Inputs,
            &sizeInfo);
    }

    size->accelerationStructureSize = sizeInfo.ResultDataMaxSizeInBytes;
    size->scratchBufferSize = sizeInfo.ScratchDataSizeInBytes;
    size->updateScratchBufferSize = sizeInfo.UpdateScratchDataSizeInBytes;
}

#endif // PAL_HAS_D3D12_BACKEND