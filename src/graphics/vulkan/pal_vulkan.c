
/**
    PAL - Prime Abstraction Layer
    Copyright (C) 2025
    Licensed under the Zlib license. See LICENSE file in root.
 */

#if PAL_HAS_VULKAN_BACKEND
#include "pal_vulkan.h"
#include "pal_platform.h"

#ifdef _WIN32
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif // WIN32_LEAN_AND_MEAN

#ifndef NOMINMAX
#define NOMINMAX
#endif // NOMINMAX

// set unicode
#ifndef UNICODE
#define UNICODE
#endif // UNICODE

#include <windows.h>
#define VK_LIB_NAME "vulkan-1.dll"
#define RESULT_SOURCE PAL_RESULT_SOURCE_WIN32
#elif defined(__linux__)
#include <dlfcn.h>
#define VK_LIB_NAME "libvulkan.so"
#define RESULT_SOURCE PAL_RESULT_SOURCE_POSIX
#else
// Android
#define VK_LIB_NAME ""
#endif // _WIN32

#if _PAL_HAS_POSIX
#include <errno.h>
#endif // _PAL_HAS_POSIX

Vulkan s_Vk = {0};

PalResult makeResultVk(VkResult result)
{
    PalResultCode code = PAL_RESULT_CODE_PLATFORM_FAILURE;
    switch (result) {
        case VK_ERROR_FEATURE_NOT_PRESENT:
        case VK_ERROR_EXTENSION_NOT_PRESENT: {
            code = PAL_RESULT_CODE_FEATURE_NOT_SUPPORTED;
            break;
        }

        case VK_ERROR_OUT_OF_HOST_MEMORY:
        case VK_ERROR_TOO_MANY_OBJECTS:
        case VK_ERROR_OUT_OF_DEVICE_MEMORY: {
            code = PAL_RESULT_CODE_OUT_OF_MEMORY;
            break;
        }

        case VK_ERROR_NATIVE_WINDOW_IN_USE_KHR: {
            code = PAL_RESULT_CODE_INVALID_HANDLE;
            break;
        }

        case VK_TIMEOUT: {
            code = PAL_RESULT_CODE_TIMEOUT;
            break;
        }

        case VK_ERROR_MEMORY_MAP_FAILED: {
            code = PAL_RESULT_CODE_INVALID_OPERATION;
            break;
        }

        case VK_ERROR_DEVICE_LOST: {
            code = PAL_RESULT_CODE_DEVICE_LOST;
            break;
        }

        case VK_ERROR_OUT_OF_DATE_KHR: {
            code = PAL_RESULT_CODE_OUT_OF_DATE;
            break;
        }
    }

    return palMakeResult(code, PAL_RESULT_SOURCE_VULKAN, (uint32_t)result);
}

VkFormat formatToVk(PalFormat format)
{
    switch (format) {
        case PAL_FORMAT_R8_UNORM:
            return VK_FORMAT_R8_UNORM;

        case PAL_FORMAT_R8_SNORM:
            return VK_FORMAT_R8_SNORM;

        case PAL_FORMAT_R8_UINT:
            return VK_FORMAT_R8_UINT;

        case PAL_FORMAT_R8_SINT:
            return VK_FORMAT_R8_SINT;

        case PAL_FORMAT_R8_SRGB:
            return VK_FORMAT_R8_SRGB;

        case PAL_FORMAT_R16_UNORM:
            return VK_FORMAT_R16_UNORM;

        case PAL_FORMAT_R16_SNORM:
            return VK_FORMAT_R16_SNORM;

        case PAL_FORMAT_R16_UINT:
            return VK_FORMAT_R16_UINT;

        case PAL_FORMAT_R16_SINT:
            return VK_FORMAT_R16_SINT;

        case PAL_FORMAT_R16_SFLOAT:
            return VK_FORMAT_R16_SFLOAT;

        case PAL_FORMAT_R32_UINT:
            return VK_FORMAT_R32_UINT;

        case PAL_FORMAT_R32_SINT:
            return VK_FORMAT_R32_SINT;

        case PAL_FORMAT_R32_SFLOAT:
            return VK_FORMAT_R32_SFLOAT;

        case PAL_FORMAT_R64_UINT:
            return VK_FORMAT_R64_UINT;

        case PAL_FORMAT_R64_SINT:
            return VK_FORMAT_R64_SINT;

        case PAL_FORMAT_R64_SFLOAT:
            return VK_FORMAT_R64_SFLOAT;

        case PAL_FORMAT_R8G8_UNORM:
            return VK_FORMAT_R8G8_UNORM;

        case PAL_FORMAT_R8G8_SNORM:
            return VK_FORMAT_R8G8_SNORM;

        case PAL_FORMAT_R8G8_UINT:
            return VK_FORMAT_R8G8_UINT;

        case PAL_FORMAT_R8G8_SINT:
            return VK_FORMAT_R8G8_SINT;

        case PAL_FORMAT_R8G8_SRGB:
            return VK_FORMAT_R8G8_SRGB;

        case PAL_FORMAT_R16G16_UNORM:
            return VK_FORMAT_R16G16_UNORM;

        case PAL_FORMAT_R16G16_SNORM:
            return VK_FORMAT_R16G16_SNORM;

        case PAL_FORMAT_R16G16_UINT:
            return VK_FORMAT_R16G16_UINT;

        case PAL_FORMAT_R16G16_SINT:
            return VK_FORMAT_R16G16_SINT;

        case PAL_FORMAT_R16G16_SFLOAT:
            return VK_FORMAT_R16G16_SFLOAT;

        case PAL_FORMAT_R32G32_UINT:
            return VK_FORMAT_R32G32_UINT;

        case PAL_FORMAT_R32G32_SINT:
            return VK_FORMAT_R32G32_SINT;

        case PAL_FORMAT_R32G32_SFLOAT:
            return VK_FORMAT_R32G32_SFLOAT;

        case PAL_FORMAT_R64G64_UINT:
            return VK_FORMAT_R64G64_UINT;

        case PAL_FORMAT_R64G64_SINT:
            return VK_FORMAT_R64G64_SINT;

        case PAL_FORMAT_R64G64_SFLOAT:
            return VK_FORMAT_R64G64_SFLOAT;

        case PAL_FORMAT_R8G8B8_UNORM:
            return VK_FORMAT_R8G8B8_UNORM;

        case PAL_FORMAT_R8G8B8_SNORM:
            return VK_FORMAT_R8G8B8_SNORM;

        case PAL_FORMAT_R8G8B8_UINT:
            return VK_FORMAT_R8G8B8_UINT;

        case PAL_FORMAT_R8G8B8_SINT:
            return VK_FORMAT_R8G8B8_SINT;

        case PAL_FORMAT_R8G8B8_SRGB:
            return VK_FORMAT_R8G8B8_SRGB;

        case PAL_FORMAT_R16G16B16_UNORM:
            return VK_FORMAT_R16G16B16_UNORM;

        case PAL_FORMAT_R16G16B16_SNORM:
            return VK_FORMAT_R16G16B16_SNORM;

        case PAL_FORMAT_R16G16B16_UINT:
            return VK_FORMAT_R16G16B16_UINT;

        case PAL_FORMAT_R16G16B16_SINT:
            return VK_FORMAT_R16G16B16_SINT;

        case PAL_FORMAT_R16G16B16_SFLOAT:
            return VK_FORMAT_R16G16B16_SFLOAT;

        case PAL_FORMAT_R32G32B32_UINT:
            return VK_FORMAT_R32G32B32_UINT;

        case PAL_FORMAT_R32G32B32_SINT:
            return VK_FORMAT_R32G32B32_SINT;

        case PAL_FORMAT_R32G32B32_SFLOAT:
            return VK_FORMAT_R32G32B32_SFLOAT;

        case PAL_FORMAT_R64G64B64_UINT:
            return VK_FORMAT_R64G64B64_UINT;

        case PAL_FORMAT_R64G64B64_SINT:
            return VK_FORMAT_R64G64B64_SINT;

        case PAL_FORMAT_R64G64B64_SFLOAT:
            return VK_FORMAT_R64G64B64_SFLOAT;

        case PAL_FORMAT_B8G8R8_UNORM:
            return VK_FORMAT_B8G8R8_UNORM;

        case PAL_FORMAT_B8G8R8_SNORM:
            return VK_FORMAT_B8G8R8_SNORM;

        case PAL_FORMAT_B8G8R8_UINT:
            return VK_FORMAT_B8G8R8_UINT;

        case PAL_FORMAT_B8G8R8_SINT:
            return VK_FORMAT_B8G8R8_SINT;

        case PAL_FORMAT_B8G8R8_SRGB:
            return VK_FORMAT_B8G8R8_SRGB;

        case PAL_FORMAT_R8G8B8A8_UNORM:
            return VK_FORMAT_R8G8B8A8_UNORM;

        case PAL_FORMAT_R8G8B8A8_SNORM:
            return VK_FORMAT_R8G8B8A8_SNORM;

        case PAL_FORMAT_R8G8B8A8_UINT:
            return VK_FORMAT_R8G8B8A8_UINT;

        case PAL_FORMAT_R8G8B8A8_SINT:
            return VK_FORMAT_R8G8B8A8_SINT;

        case PAL_FORMAT_R8G8B8A8_SRGB:
            return VK_FORMAT_R8G8B8A8_SRGB;

        case PAL_FORMAT_R16G16B16A16_UNORM:
            return VK_FORMAT_R16G16B16A16_UNORM;

        case PAL_FORMAT_R16G16B16A16_SNORM:
            return VK_FORMAT_R16G16B16A16_SNORM;

        case PAL_FORMAT_R16G16B16A16_UINT:
            return VK_FORMAT_R16G16B16A16_UINT;

        case PAL_FORMAT_R16G16B16A16_SINT:
            return VK_FORMAT_R16G16B16A16_SINT;

        case PAL_FORMAT_R16G16B16A16_SFLOAT:
            return VK_FORMAT_R16G16B16A16_SFLOAT;

        case PAL_FORMAT_R32G32B32A32_UINT:
            return VK_FORMAT_R32G32B32A32_UINT;

        case PAL_FORMAT_R32G32B32A32_SINT:
            return VK_FORMAT_R32G32B32A32_SINT;

        case PAL_FORMAT_R32G32B32A32_SFLOAT:
            return VK_FORMAT_R32G32B32A32_SFLOAT;

        case PAL_FORMAT_R64G64B64A64_UINT:
            return VK_FORMAT_R64G64B64A64_UINT;

        case PAL_FORMAT_R64G64B64A64_SINT:
            return VK_FORMAT_R64G64B64A64_SINT;

        case PAL_FORMAT_R64G64B64A64_SFLOAT:
            return VK_FORMAT_R64G64B64A64_SFLOAT;

        case PAL_FORMAT_B8G8R8A8_UNORM:
            return VK_FORMAT_B8G8R8A8_UNORM;

        case PAL_FORMAT_B8G8R8A8_SNORM:
            return VK_FORMAT_B8G8R8A8_SNORM;

        case PAL_FORMAT_B8G8R8A8_UINT:
            return VK_FORMAT_B8G8R8A8_UINT;

        case PAL_FORMAT_B8G8R8A8_SINT:
            return VK_FORMAT_B8G8R8A8_SINT;

        case PAL_FORMAT_B8G8R8A8_SRGB:
            return VK_FORMAT_B8G8R8A8_SRGB;

        case PAL_FORMAT_S8_UINT:
            return VK_FORMAT_S8_UINT;

        case PAL_FORMAT_D16_UNORM:
            return VK_FORMAT_D16_UNORM;

        case PAL_FORMAT_D32_SFLOAT:
            return VK_FORMAT_D32_SFLOAT;

        case PAL_FORMAT_D32_SFLOAT_S8_UINT:
            return VK_FORMAT_D32_SFLOAT_S8_UINT;

        case PAL_FORMAT_D16_UNORM_S8_UINT:
            return VK_FORMAT_D16_UNORM_S8_UINT;

        case PAL_FORMAT_D24_UNORM_S8_UINT:
            return VK_FORMAT_D24_UNORM_S8_UINT;
    }

    return VK_FORMAT_UNDEFINED;
}

VkSampleCountFlags samplesToVk(PalSampleCount count)
{
    switch (count) {
        case PAL_SAMPLE_COUNT_2:
            return VK_SAMPLE_COUNT_2_BIT;

        case PAL_SAMPLE_COUNT_4:
            return VK_SAMPLE_COUNT_4_BIT;

        case PAL_SAMPLE_COUNT_8:
            return VK_SAMPLE_COUNT_8_BIT;

        case PAL_SAMPLE_COUNT_16:
            return VK_SAMPLE_COUNT_16_BIT;

        case PAL_SAMPLE_COUNT_32:
            return VK_SAMPLE_COUNT_32_BIT;

        case PAL_SAMPLE_COUNT_64:
            return VK_SAMPLE_COUNT_64_BIT;
    }

    return VK_SAMPLE_COUNT_1_BIT;
}

VkExtent2D getShadingRateSizeVk(PalFragmentShadingRate rate)
{
    switch (rate) {
        case PAL_FRAGMENT_SHADING_RATE_1X1:
            return (VkExtent2D){1, 1};

        case PAL_FRAGMENT_SHADING_RATE_1X2:
            return (VkExtent2D){1, 2};

        case PAL_FRAGMENT_SHADING_RATE_2X1:
            return (VkExtent2D){2, 1};

        case PAL_FRAGMENT_SHADING_RATE_2X2:
            return (VkExtent2D){2, 2};

        case PAL_FRAGMENT_SHADING_RATE_2X4:
            return (VkExtent2D){2, 4};

        case PAL_FRAGMENT_SHADING_RATE_4X2:
            return (VkExtent2D){4, 2};

        case PAL_FRAGMENT_SHADING_RATE_4X4:
            return (VkExtent2D){4, 4};
    }

    return (VkExtent2D){0, 0};
}

VkFragmentShadingRateCombinerOpKHR combinerOpsToVk(PalFragmentShadingRateCombinerOp op)
{
    switch (op) {
        case PAL_FRAGMENT_SHADING_RATE_COMBINER_OP_KEEP:
            return VK_FRAGMENT_SHADING_RATE_COMBINER_OP_KEEP_KHR;

        case PAL_FRAGMENT_SHADING_RATE_COMBINER_OP_REPLACE:
            return VK_FRAGMENT_SHADING_RATE_COMBINER_OP_REPLACE_KHR;

        case PAL_FRAGMENT_SHADING_RATE_COMBINER_OP_MIN:
            return VK_FRAGMENT_SHADING_RATE_COMBINER_OP_MIN_KHR;

        case PAL_FRAGMENT_SHADING_RATE_COMBINER_OP_MAX:
            return VK_FRAGMENT_SHADING_RATE_COMBINER_OP_MAX_KHR;

        case PAL_FRAGMENT_SHADING_RATE_COMBINER_OP_MUL:
            return VK_FRAGMENT_SHADING_RATE_COMBINER_OP_MUL_KHR;
    }

    return VK_FRAGMENT_SHADING_RATE_COMBINER_OP_KEEP_KHR;
}

VkImageAspectFlags imageAspectToVk(PalImageAspect aspect)
{
    switch (aspect) {
        case PAL_IMAGE_ASPECT_COLOR:
            return VK_IMAGE_ASPECT_COLOR_BIT;

        case PAL_IMAGE_ASPECT_DEPTH:
            return VK_IMAGE_ASPECT_DEPTH_BIT;

        case PAL_IMAGE_ASPECT_STENCIL:
            return VK_IMAGE_ASPECT_STENCIL_BIT;

        case PAL_IMAGE_ASPECT_DEPTH_STENCIL:
            return VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT;
    }

    return VK_IMAGE_ASPECT_COLOR_BIT;
}

VkStencilOp stencilOpToVk(PalStencilOp op)
{
    switch (op) {
        case PAL_STENCIL_OP_KEEP:
            return VK_STENCIL_OP_KEEP;

        case PAL_STENCIL_OP_ZERO:
            return VK_STENCIL_OP_ZERO;

        case PAL_STENCIL_OP_REPLACE:
            return VK_STENCIL_OP_REPLACE;

        case PAL_STENCIL_OP_INCREMENT_AND_CLAMP:
            return VK_STENCIL_OP_INCREMENT_AND_CLAMP;

        case PAL_STENCIL_OP_DECREMENT_AND_CLAMP:
            return VK_STENCIL_OP_DECREMENT_AND_CLAMP;

        case PAL_STENCIL_OP_INVERT:
            return VK_STENCIL_OP_INVERT;

        case PAL_STENCIL_OP_INCREMENT_AND_WRAP:
            return VK_STENCIL_OP_INCREMENT_AND_WRAP;

        case PAL_STENCIL_OP_DECREMENT_AND_WRAP:
            return VK_STENCIL_OP_DECREMENT_AND_WRAP;
    }

    return VK_STENCIL_OP_KEEP;
}

VkCompareOp compareOpToVk(PalCompareOp op)
{
    switch (op) {
        case PAL_COMPARE_OP_NEVER:
            return VK_COMPARE_OP_NEVER;

        case PAL_COMPARE_OP_LESS:
            return VK_COMPARE_OP_LESS;

        case PAL_COMPARE_OP_EQUAL:
            return VK_COMPARE_OP_EQUAL;

        case PAL_COMPARE_OP_LESS_OR_EQUAL:
            return VK_COMPARE_OP_LESS_OR_EQUAL;

        case PAL_COMPARE_OP_GREATER:
            return VK_COMPARE_OP_GREATER;

        case PAL_COMPARE_OP_NOT_EQUAL:
            return VK_COMPARE_OP_NOT_EQUAL;

        case PAL_COMPARE_OP_GREATER_OR_EQUAL:
            return VK_COMPARE_OP_GREATER_OR_EQUAL;

        case PAL_COMPARE_OP_ALWAYS:
            return VK_COMPARE_OP_ALWAYS;
    }

    return VK_COMPARE_OP_NEVER;
}

uint32_t findBestMemoryIndexVk(
    VkPhysicalDevice phyDevice,
    uint32_t memoryMask)
{
    int bestScore = -1;
    uint32_t bestIndex = UINT32_MAX;
    VkPhysicalDeviceMemoryProperties memProps = {0};
    s_Vk.getPhysicalDeviceMemoryProperties(phyDevice, &memProps);

    for (int i = 0; i < memProps.memoryTypeCount; i++) {
        if (!(memoryMask & (1u << i))) {
            continue;
        }

        int score = 0;
        VkMemoryPropertyFlags flags = memProps.memoryTypes[i].propertyFlags;

        // GPU only memory
        if (flags & VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT) {
            score += 100;
        }

        // CPU memory
        if (flags & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT) {
            score += 50;
        }

        if (flags & VK_MEMORY_PROPERTY_HOST_COHERENT_BIT) {
            score += 25;
        }

        if (flags & VK_MEMORY_PROPERTY_HOST_CACHED_BIT) {
            score += 10;
        }

        if (score > bestScore) {
            bestIndex = i;
        }
    }

    return bestIndex;
}

VkFormat vertexTypeToVk(PalVertexType type)
{
    switch (type) {
        case PAL_VERTEX_TYPE_INT32:
            return VK_FORMAT_R32_SINT;

        case PAL_VERTEX_TYPE_INT32_2:
            return VK_FORMAT_R32G32_SINT;

        case PAL_VERTEX_TYPE_INT32_3:
            return VK_FORMAT_R32G32B32_SINT;

        case PAL_VERTEX_TYPE_INT32_4:
            return VK_FORMAT_R32G32B32A32_SINT;

        case PAL_VERTEX_TYPE_UINT32:
            return VK_FORMAT_R32_UINT;

        case PAL_VERTEX_TYPE_UINT32_2:
            return VK_FORMAT_R32G32_UINT;

        case PAL_VERTEX_TYPE_UINT32_3:
            return VK_FORMAT_R32G32B32_UINT;

        case PAL_VERTEX_TYPE_UINT32_4:
            return VK_FORMAT_R32G32B32A32_UINT;

        case PAL_VERTEX_TYPE_INT8_2:
            return VK_FORMAT_R8G8_SINT;

        case PAL_VERTEX_TYPE_INT8_4:
            return VK_FORMAT_R8G8B8A8_SINT;

        case PAL_VERTEX_TYPE_UINT8_2:
            return VK_FORMAT_R8G8_UINT;

        case PAL_VERTEX_TYPE_UINT8_4:
            return VK_FORMAT_R8G8B8A8_UINT;

        case PAL_VERTEX_TYPE_INT8_2NORM:
            return VK_FORMAT_R8G8_SNORM;

        case PAL_VERTEX_TYPE_INT8_4NORM:
            return VK_FORMAT_R8G8B8A8_SNORM;

        case PAL_VERTEX_TYPE_UINT8_2NORM:
            return VK_FORMAT_R8G8_UNORM;

        case PAL_VERTEX_TYPE_UINT8_4NORM:
            return VK_FORMAT_R8G8B8A8_UNORM;

        case PAL_VERTEX_TYPE_INT16_2:
            return VK_FORMAT_R16G16_SINT;

        case PAL_VERTEX_TYPE_INT16_4:
            return VK_FORMAT_R16G16B16A16_SINT;

        case PAL_VERTEX_TYPE_UINT16_2:
            return VK_FORMAT_R16G16_UINT;

        case PAL_VERTEX_TYPE_UINT16_4:
            return VK_FORMAT_R16G16B16A16_UINT;

        case PAL_VERTEX_TYPE_INT16_2NORM:
            return VK_FORMAT_R16G16_SNORM;

        case PAL_VERTEX_TYPE_INT16_4NORM:
            return VK_FORMAT_R16G16B16A16_SNORM;

        case PAL_VERTEX_TYPE_UINT16_2NORM:
            return VK_FORMAT_R16G16_UNORM;

        case PAL_VERTEX_TYPE_UINT16_4NORM:
            return VK_FORMAT_R16G16B16A16_UNORM;

        case PAL_VERTEX_TYPE_FLOAT:
            return VK_FORMAT_R32_SFLOAT;

        case PAL_VERTEX_TYPE_FLOAT2:
            return VK_FORMAT_R32G32_SFLOAT;

        case PAL_VERTEX_TYPE_FLOAT3:
            return VK_FORMAT_R32G32B32_SFLOAT;

        case PAL_VERTEX_TYPE_FLOAT4:
            return VK_FORMAT_R32G32B32A32_SFLOAT;

        case PAL_VERTEX_TYPE_HALF_FLOAT16_2:
            return VK_FORMAT_R16G16_SFLOAT;

        case PAL_VERTEX_TYPE_HALF_FLOAT16_4:
            return VK_FORMAT_R16G16B16A16_SFLOAT;
    }

    return VK_FORMAT_UNDEFINED;
}

void fillBuildInfoVk(
    uint32_t count,
    PalAccelerationStructureBuildInfo* info,
    uint32_t* maxPrimities,
    VkAccelerationStructureGeometryKHR* geometries,
    VkAccelerationStructureKHR srcAs,
    VkAccelerationStructureKHR dstAs,
    VkAccelerationStructureBuildRangeInfoKHR* rangeInfos,
    VkAccelerationStructureBuildGeometryInfoKHR* buildInfo)
{
    for (int i = 0; i < count; i++) {
        // fill vulkan geometry struct
        VkAccelerationStructureGeometryKHR* tmp = &geometries[i];
        tmp->sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_KHR;
        tmp->flags = 0;

        if (info->type == PAL_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL) {
            tmp->geometryType = VK_GEOMETRY_TYPE_INSTANCES_KHR;
            VkAccelerationStructureGeometryInstancesDataKHR* data = nullptr;
            data = &tmp->geometry.instances;
            data->sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_INSTANCES_DATA_KHR;
            data->arrayOfPointers = PAL_FALSE;

            VkDeviceOrHostAddressConstKHR address = {0};
            address.deviceAddress = info->instanceBufferAddress;
            data->data = address;

            if (maxPrimities) {
                maxPrimities[i] = info->count;
            }

            // range info
            if (rangeInfos) {
                VkAccelerationStructureBuildRangeInfoKHR* rangeInfo = &rangeInfos[i];
                rangeInfo->primitiveCount = info->count;
                rangeInfo->firstVertex = 0;     // PAL does not allow setting this
                rangeInfo->primitiveOffset = 0; // PAL does not allow setting this
                rangeInfo->transformOffset = 0; // PAL does not allow setting this
            }
            break;

        } else {
            if (info->geometries[i].flags & PAL_GEOMETRY_FLAG_OPAQUE) {
                tmp->flags |= VK_GEOMETRY_OPAQUE_BIT_KHR;
            }

            if (info->geometries[i].flags & PAL_GEOMETRY_FLAG_NO_DUPLICATE_ANYHIT) {
                tmp->flags |= VK_GEOMETRY_OPAQUE_BIT_KHR;
            }

            if (maxPrimities) {
                maxPrimities[i] = info->geometries[i].primitiveCount;
            }

            // range info
            if (rangeInfos) {
                VkAccelerationStructureBuildRangeInfoKHR* rangeInfo = &rangeInfos[i];
                rangeInfo->primitiveCount = info->geometries[i].primitiveCount;
                rangeInfo->firstVertex = 0;     // PAL does not allow setting this
                rangeInfo->primitiveOffset = 0; // PAL does not allow setting this
                rangeInfo->transformOffset = 0; // PAL does not allow setting this
            }
        }

        if (info->geometries[i].type == PAL_GEOMETRY_TYPE_TRIANGLE) {
            tmp->geometryType = VK_GEOMETRY_TYPE_TRIANGLES_KHR;
            VkAccelerationStructureGeometryTrianglesDataKHR* data = &tmp->geometry.triangles;
            data->sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_TRIANGLES_DATA_KHR;

            VkDeviceOrHostAddressConstKHR vertexAddress = {0};
            VkDeviceOrHostAddressConstKHR indexAddress = {0};
            const PalGeometryDataTriangle* tmpData = info->geometries[i].data;

            vertexAddress.deviceAddress = tmpData->vertexBufferAddress;
            data->vertexData = vertexAddress;
            data->maxVertex = tmpData->vertexCount - 1;
            data->vertexFormat = vertexTypeToVk(tmpData->vertexType);
            data->vertexStride = tmpData->vertexStride;

            indexAddress.deviceAddress = tmpData->indexBufferAddress;
            data->indexData = indexAddress;
            if (tmpData->indexType == PAL_INDEX_TYPE_UINT32) {
                data->indexType = VK_INDEX_TYPE_UINT32;
            } else {
                data->indexType = VK_INDEX_TYPE_UINT16;
            }

            // set to none if there is no index buffer address
            if (!tmpData->indexBufferAddress) {
                data->indexType = VK_INDEX_TYPE_NONE_KHR;
            }

        } else if (info->geometries[i].type == PAL_GEOMETRY_TYPE_AABBS) {
            tmp->geometryType = VK_GEOMETRY_TYPE_AABBS_KHR;
            VkAccelerationStructureGeometryAabbsDataKHR* data = &tmp->geometry.aabbs;
            data->sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_AABBS_DATA_KHR;

            VkDeviceOrHostAddressConstKHR address = {0};
            const PalGeometryDataAABBS* tmpData = info->geometries[i].data;
            address.deviceAddress = tmpData->bufferAddress;
            data->data = address;
            data->stride = tmpData->stride;
        }
    }

    buildInfo->sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_GEOMETRY_INFO_KHR;
    if (info->type == PAL_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL) {
        buildInfo->type = VK_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL_KHR;
    } else {
        buildInfo->type = VK_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL_KHR;
    }

    // build mode
    if (info->buildMode == PAL_ACCELERATION_STRUCTURE_BUILD_MODE_BUILD) {
        buildInfo->mode = VK_BUILD_ACCELERATION_STRUCTURE_MODE_BUILD_KHR;
    } else {
        buildInfo->mode = VK_BUILD_ACCELERATION_STRUCTURE_MODE_UPDATE_KHR;
    }

    // build hints
    buildInfo->flags = 0;
    if (info->buildHints & PAL_ACCELERATION_STRUCTURE_BUILD_HINT_FAST_BUILD) {
        buildInfo->flags |= VK_BUILD_ACCELERATION_STRUCTURE_PREFER_FAST_BUILD_BIT_KHR;
    }

    if (info->buildHints & PAL_ACCELERATION_STRUCTURE_BUILD_HINT_FAST_TRACE) {
        buildInfo->flags |= VK_BUILD_ACCELERATION_STRUCTURE_PREFER_FAST_TRACE_BIT_KHR;
    }

    if (info->buildHints & PAL_ACCELERATION_STRUCTURE_BUILD_HINT_LOW_MEMORY) {
        buildInfo->flags |= VK_BUILD_ACCELERATION_STRUCTURE_LOW_MEMORY_BIT_KHR;
    }

    buildInfo->geometryCount = count;
    buildInfo->srcAccelerationStructure = srcAs;
    buildInfo->dstAccelerationStructure = dstAs;
    buildInfo->pGeometries = geometries;

    VkDeviceOrHostAddressKHR scratchData = {0};
    scratchData.deviceAddress = info->scratchBufferAddress;
    buildInfo->scratchData = scratchData;
}

static void* alignedRealloc(
    void* memory, 
    uint64_t size, 
    uint64_t alignment)
{
#if defined(_MSC_VER) || defined(__MINGW32__)
    return _aligned_realloc(memory, size, alignment);
#else
    return realloc(memory, size);
#endif // _MSC_VER
}

static void alignedFree(void* ptr)
{
#if defined(_MSC_VER) || defined(__MINGW32__)
    _aligned_free(ptr);
#else
    free(ptr);
#endif // _MSC_VER
}

static void* VKAPI_CALL allocateVk(
    void* pUserData,
    size_t size,
    size_t alignment,
    VkSystemAllocationScope allocationScope)
{
    return palAllocate(s_Vk.allocator, size, alignment);
}

static void VKAPI_CALL freeVk(
    void* pUserData,
    void* ptr)
{
    palFree(s_Vk.allocator, ptr);
}

static void* VKAPI_CALL reallocVk(
    void* pUserData,
    void* pOriginal,
    size_t size,
    size_t alignment,
    VkSystemAllocationScope allocationScope)
{
    // Note: This is a hack which could cost performance but
    // realloc is not really called that much so it should be fine
    // this is because we dont know the old size
    void* block = alignedRealloc(pOriginal, size, alignment);
    if (block) {
        void* memory = palAllocate(s_Vk.allocator, size, alignment);
        if (!memory) {
            alignedFree(block);
            return nullptr;
        }

        memcpy(memory, block, size);
        alignedFree(block);
        return memory;
    }
    return nullptr;
}

static void* loadLibrary(const char* name)
{
#ifdef _WIN32
    return LoadLibraryA(name);
#elif defined (__linux__)
    return dlopen(name, RTLD_LAZY);
#endif
}

static void freeLibrary(void* lib)
{
#ifdef _WIN32
    FreeLibrary(lib);
#elif defined (__linux__)
    dlclose(lib);
#endif
}

static void* loadProc(void* lib, const char* name)
{
#ifdef _WIN32
    return GetProcAddress(lib, name);
#elif defined (__linux__)
    return dlsym(lib, name);
#endif
}

static uint32_t getNativeCode()
{
#ifdef _WIN32
    return GetLastError();
#elif defined (__linux__)
    return errno;
#endif
}

VkBool32 VKAPI_CALL debugCallbackVk(
    VkDebugUtilsMessageSeverityFlagBitsEXT severity,
    VkDebugUtilsMessageTypeFlagBitsEXT type,
    const VkDebugUtilsMessengerCallbackDataEXT* data,
    void* userData)
{
    if (!s_Vk.callback) {
        return VK_FALSE;
    }

    PalDebugMessageSeverity debugSeverity = 0;
    PalDebugMessageType debugType = 0;
    if (type & VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT) {
        debugType = PAL_DEBUG_MESSAGE_TYPE_GENERAL;
    }

    if (type & VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT) {
        debugType = PAL_DEBUG_MESSAGE_TYPE_PERFORMANCE;
    }

    if (type & VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT) {
        debugType = PAL_DEBUG_MESSAGE_TYPE_VALIDATION;
    }

    if (type & VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT) {
        debugSeverity = PAL_DEBUG_MESSAGE_SEVERITY_INFO;
    }

    if (type & VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) {
        debugSeverity = PAL_DEBUG_MESSAGE_SEVERITY_WARNING;
    }

    if (type & VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT) {
        debugSeverity = PAL_DEBUG_MESSAGE_SEVERITY_ERROR;
    }

    s_Vk.callback(userData, debugSeverity, debugType, data->pMessage);
    return VK_FALSE;
}

PalResult PAL_CALL initGraphicsVk(
    const PalGraphicsDebugger* debugger,
    const PalAllocator* allocator)
{
    // load vulkan
    s_Vk.handle = loadLibrary(VK_LIB_NAME);
    if (!s_Vk.handle) {
        return palMakeResult(PAL_RESULT_CODE_PLATFORM_FAILURE, RESULT_SOURCE, getNativeCode());
    }

    // clang-format off
    s_Vk.enumerateInstanceVersion = (PFN_vkEnumerateInstanceVersion)loadProc(
        s_Vk.handle,
        "vkEnumerateInstanceVersion");

    s_Vk.enumerateInstanceExtensionProperties = (PFN_vkEnumerateInstanceExtensionProperties)loadProc(
        s_Vk.handle,
        "vkEnumerateInstanceExtensionProperties");

    s_Vk.createInstance = (PFN_vkCreateInstance)loadProc(
        s_Vk.handle,
        "vkCreateInstance");

    s_Vk.destroyInstance = (PFN_vkDestroyInstance)loadProc(
        s_Vk.handle,
        "vkDestroyInstance");

    s_Vk.enumeratePhysicalDevices = (PFN_vkEnumeratePhysicalDevices)loadProc(
        s_Vk.handle,
        "vkEnumeratePhysicalDevices");

    s_Vk.getPhysicalDeviceProperties = (PFN_vkGetPhysicalDeviceProperties)loadProc(
        s_Vk.handle,
        "vkGetPhysicalDeviceProperties");

    s_Vk.getPhysicalDeviceMemoryProperties = (PFN_vkGetPhysicalDeviceMemoryProperties)loadProc(
        s_Vk.handle,
        "vkGetPhysicalDeviceMemoryProperties");

    s_Vk.enumerateInstanceLayerProperties = (PFN_vkEnumerateInstanceLayerProperties)loadProc(
        s_Vk.handle,
        "vkEnumerateInstanceLayerProperties");

    s_Vk.getPhysicalDeviceQueueFamilyProperties = (PFN_vkGetPhysicalDeviceQueueFamilyProperties)loadProc(
        s_Vk.handle,
        "vkGetPhysicalDeviceQueueFamilyProperties");

    s_Vk.enumerateDeviceExtensionProperties = (PFN_vkEnumerateDeviceExtensionProperties)loadProc(
        s_Vk.handle,
        "vkEnumerateDeviceExtensionProperties");

    s_Vk.getPhysicalDeviceFeatures = (PFN_vkGetPhysicalDeviceFeatures)loadProc(
        s_Vk.handle,
        "vkGetPhysicalDeviceFeatures");

    s_Vk.getPhysicalDeviceFeatures2 = (PFN_vkGetPhysicalDeviceFeatures2)loadProc(
        s_Vk.handle,
        "vkGetPhysicalDeviceFeatures2");

    s_Vk.getInstanceProcAddr = (PFN_vkGetInstanceProcAddr)loadProc(
        s_Vk.handle,
        "vkGetInstanceProcAddr");

    s_Vk.createImage = (PFN_vkCreateImage)loadProc(
        s_Vk.handle,
        "vkCreateImage");

    s_Vk.destroyImage = (PFN_vkDestroyImage)loadProc(
        s_Vk.handle,
        "vkDestroyImage");

    s_Vk.createImageView = (PFN_vkCreateImageView)loadProc(
        s_Vk.handle,
        "vkCreateImageView");

    s_Vk.destroyImageView = (PFN_vkDestroyImageView)loadProc(
        s_Vk.handle,
        "vkDestroyImageView");

    s_Vk.createShader = (PFN_vkCreateShaderModule)loadProc(
        s_Vk.handle,
        "vkCreateShaderModule");

    s_Vk.destroyShader = (PFN_vkDestroyShaderModule)loadProc(
        s_Vk.handle,
        "vkDestroyShaderModule");

    s_Vk.createSampler = (PFN_vkCreateSampler)loadProc(
        s_Vk.handle,
        "vkCreateSampler");

    s_Vk.destroySampler = (PFN_vkDestroySampler)loadProc(
        s_Vk.handle,
        "vkDestroySampler");

    s_Vk.getPhysicalDeviceProperties2 = (PFN_vkGetPhysicalDeviceProperties2)loadProc(
        s_Vk.handle,
        "vkGetPhysicalDeviceProperties2");

    s_Vk.getPhysicalDeviceFormatProperties = (PFN_vkGetPhysicalDeviceFormatProperties)loadProc(
        s_Vk.handle,
        "vkGetPhysicalDeviceFormatProperties");

    s_Vk.getPhysicalDeviceImageFormatProperties = (PFN_vkGetPhysicalDeviceImageFormatProperties)loadProc(
        s_Vk.handle,
        "vkGetPhysicalDeviceImageFormatProperties");

    s_Vk.createDevice = (PFN_vkCreateDevice)loadProc(
        s_Vk.handle,
        "vkCreateDevice");

    s_Vk.destroyDevice = (PFN_vkDestroyDevice)loadProc(
        s_Vk.handle,
        "vkDestroyDevice");

    s_Vk.getDeviceQueue = (PFN_vkGetDeviceQueue)loadProc(
        s_Vk.handle,
        "vkGetDeviceQueue");

    s_Vk.queueSubmit = (PFN_vkQueueSubmit)loadProc(
        s_Vk.handle,
        "vkQueueSubmit");

    s_Vk.getDeviceProcAddr = (PFN_vkGetDeviceProcAddr)loadProc(
        s_Vk.handle,
        "vkGetDeviceProcAddr");

    s_Vk.getImageMemoryRequirements = (PFN_vkGetImageMemoryRequirements)loadProc(
        s_Vk.handle,
        "vkGetImageMemoryRequirements");

    s_Vk.allocateMemory = (PFN_vkAllocateMemory)loadProc(
        s_Vk.handle,
        "vkAllocateMemory");

    s_Vk.freeMemory = (PFN_vkFreeMemory)loadProc(
        s_Vk.handle,
        "vkFreeMemory");

    s_Vk.bindImageMemory = (PFN_vkBindImageMemory)loadProc(
        s_Vk.handle,
        "vkBindImageMemory");

    s_Vk.createCommandPool = (PFN_vkCreateCommandPool)loadProc(
        s_Vk.handle,
        "vkCreateCommandPool");

    s_Vk.destroyCommandPool = (PFN_vkDestroyCommandPool)loadProc(
        s_Vk.handle,
        "vkDestroyCommandPool");

    s_Vk.allocateCommandBuffer = (PFN_vkAllocateCommandBuffers)loadProc(
        s_Vk.handle,
        "vkAllocateCommandBuffers");

    s_Vk.freeCommandBuffer = (PFN_vkFreeCommandBuffers)loadProc(
        s_Vk.handle,
        "vkFreeCommandBuffers");

    s_Vk.createFence = (PFN_vkCreateFence)loadProc(
        s_Vk.handle,
        "vkCreateFence");

    s_Vk.destroyFence = (PFN_vkDestroyFence)loadProc(
        s_Vk.handle,
        "vkDestroyFence");

    s_Vk.resetFence = (PFN_vkResetFences)loadProc(
        s_Vk.handle,
        "vkResetFences");

    s_Vk.waitFence = (PFN_vkWaitForFences)loadProc(
        s_Vk.handle,
        "vkWaitForFences");

    s_Vk.isFenceSignaled = (PFN_vkGetFenceStatus)loadProc(
        s_Vk.handle,
        "vkGetFenceStatus");

    s_Vk.createSemaphore = (PFN_vkCreateSemaphore)loadProc(
        s_Vk.handle,
        "vkCreateSemaphore");

    s_Vk.destroySemaphore = (PFN_vkDestroySemaphore)loadProc(
        s_Vk.handle,
        "vkDestroySemaphore");

    s_Vk.cmdBegin = (PFN_vkBeginCommandBuffer)loadProc(
        s_Vk.handle,
        "vkBeginCommandBuffer");

    s_Vk.cmdEnd = (PFN_vkEndCommandBuffer)loadProc(
        s_Vk.handle,
        "vkEndCommandBuffer");

    s_Vk.resetCommandPool = (PFN_vkResetCommandPool)loadProc(
        s_Vk.handle,
        "vkResetCommandPool");

    s_Vk.resetCommandBuffer = (PFN_vkResetCommandBuffer)loadProc(
        s_Vk.handle,
        "vkResetCommandBuffer");

    s_Vk.cmdExecuteCommandBuffer = (PFN_vkCmdExecuteCommands)loadProc(
        s_Vk.handle,
        "vkCmdExecuteCommands");

    s_Vk.cmdCopyBuffer = (PFN_vkCmdCopyBuffer)loadProc(
        s_Vk.handle,
        "vkCmdCopyBuffer");

    s_Vk.cmdCopyBufferToImage = (PFN_vkCmdCopyBufferToImage)loadProc(
        s_Vk.handle,
        "vkCmdCopyBufferToImage");

    s_Vk.cmdCopyImage = (PFN_vkCmdCopyImage)loadProc(
        s_Vk.handle,
        "vkCmdCopyImage");

    s_Vk.cmdCopyImageToBuffer = (PFN_vkCmdCopyImageToBuffer)loadProc(
        s_Vk.handle,
        "vkCmdCopyImageToBuffer");

    s_Vk.cmdBindPipeline = (PFN_vkCmdBindPipeline)loadProc(
        s_Vk.handle,
        "vkCmdBindPipeline");

    s_Vk.cmdSetViewports = (PFN_vkCmdSetViewport)loadProc(
        s_Vk.handle,
        "vkCmdSetViewport");

    s_Vk.cmdSetScissors = (PFN_vkCmdSetScissor)loadProc(
        s_Vk.handle,
        "vkCmdSetScissor");

    s_Vk.cmdBindVertexBuffers = (PFN_vkCmdBindVertexBuffers)loadProc(
        s_Vk.handle,
        "vkCmdBindVertexBuffers");

    s_Vk.cmdBindIndexBuffer = (PFN_vkCmdBindIndexBuffer)loadProc(
        s_Vk.handle,
        "vkCmdBindIndexBuffer");

    s_Vk.cmdDraw = (PFN_vkCmdDraw)loadProc(
        s_Vk.handle,
        "vkCmdDraw");

    s_Vk.cmdDrawIndirect = (PFN_vkCmdDrawIndirect)loadProc(
        s_Vk.handle,
        "vkCmdDrawIndirect");

    s_Vk.cmdDrawIndexed = (PFN_vkCmdDrawIndexed)loadProc(
        s_Vk.handle,
        "vkCmdDrawIndexed");

    s_Vk.cmdDrawIndexedIndirect = (PFN_vkCmdDrawIndexedIndirect)loadProc(
        s_Vk.handle,
        "vkCmdDrawIndexedIndirect");

    s_Vk.cmdDispatch = (PFN_vkCmdDispatch)loadProc(
        s_Vk.handle,
        "vkCmdDispatch");

    s_Vk.cmdDispatchIndirect = (PFN_vkCmdDispatchIndirect)loadProc(
        s_Vk.handle,
        "vkCmdDispatchIndirect");

    s_Vk.cmdBindDescriptorSets = (PFN_vkCmdBindDescriptorSets)loadProc(
        s_Vk.handle,
        "vkCmdBindDescriptorSets");

    s_Vk.cmdPushConstants = (PFN_vkCmdPushConstants)loadProc(
        s_Vk.handle,
        "vkCmdPushConstants");

    s_Vk.createBuffer = (PFN_vkCreateBuffer)loadProc(
        s_Vk.handle,
        "vkCreateBuffer");

    s_Vk.destroyBuffer = (PFN_vkDestroyBuffer)loadProc(
        s_Vk.handle,
        "vkDestroyBuffer");

    s_Vk.mapMemory = (PFN_vkMapMemory)loadProc(
        s_Vk.handle,
        "vkMapMemory");

    s_Vk.unmapMemory = (PFN_vkUnmapMemory)loadProc(
        s_Vk.handle,
        "vkUnmapMemory");

    s_Vk.getBufferDeviceAddress = (PFN_vkGetBufferDeviceAddress)loadProc(
        s_Vk.handle,
        "vkGetBufferDeviceAddress");

    s_Vk.getBufferMemoryRequirements = (PFN_vkGetBufferMemoryRequirements)loadProc(
        s_Vk.handle,
        "vkGetBufferMemoryRequirements");

    s_Vk.bindBufferMemory = (PFN_vkBindBufferMemory)loadProc(
        s_Vk.handle,
        "vkBindBufferMemory");

    s_Vk.createDescriptorSetLayout = (PFN_vkCreateDescriptorSetLayout)loadProc(
        s_Vk.handle,
        "vkCreateDescriptorSetLayout");

    s_Vk.destroyDescriptorSetLayout = (PFN_vkDestroyDescriptorSetLayout)loadProc(
        s_Vk.handle,
        "vkDestroyDescriptorSetLayout");

    s_Vk.createDescriptorPool = (PFN_vkCreateDescriptorPool)loadProc(
        s_Vk.handle,
        "vkCreateDescriptorPool");

    s_Vk.destroyDescriptorPool = (PFN_vkDestroyDescriptorPool)loadProc(
        s_Vk.handle,
        "vkDestroyDescriptorPool");

    s_Vk.resetDescriptorPool = (PFN_vkResetDescriptorPool)loadProc(
        s_Vk.handle,
        "vkResetDescriptorPool");

    s_Vk.allocateDescriptorSet = (PFN_vkAllocateDescriptorSets)loadProc(
        s_Vk.handle,
        "vkAllocateDescriptorSets");

    s_Vk.updateDescriptorSet = (PFN_vkUpdateDescriptorSets)loadProc(
        s_Vk.handle,
        "vkUpdateDescriptorSets");

    s_Vk.createPipelineLayout = (PFN_vkCreatePipelineLayout)loadProc(
        s_Vk.handle,
        "vkCreatePipelineLayout");

    s_Vk.destroyPipelineLayout = (PFN_vkDestroyPipelineLayout)loadProc(
        s_Vk.handle,
        "vkDestroyPipelineLayout");

    s_Vk.createGraphicsPipeline = (PFN_vkCreateGraphicsPipelines)loadProc(
        s_Vk.handle,
        "vkCreateGraphicsPipelines");

    s_Vk.createComputePipeline = (PFN_vkCreateComputePipelines)loadProc(
        s_Vk.handle,
        "vkCreateComputePipelines");

    s_Vk.destroyPipeline = (PFN_vkDestroyPipeline)loadProc(
        s_Vk.handle,
        "vkDestroyPipeline");

    s_Vk.waitQueue = (PFN_vkQueueWaitIdle)loadProc(
        s_Vk.handle,
        "vkQueueWaitIdle");
    // clang-format on

    // get version
    PalBool versionFallback = PAL_FALSE;
    uint32_t version = 0;
    if (s_Vk.enumerateInstanceVersion) {
        s_Vk.enumerateInstanceVersion(&version);
        if (version <= VK_API_VERSION_1_0) {
            versionFallback = PAL_TRUE;
        }
    }

    VkResult result;
    uint32_t layerCount = 0;
    PalBool hasValidationLayer = PAL_FALSE;
    s_Vk.messenger = nullptr;
    s_Vk.allocator = allocator;
    VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo = {0};
    debugCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;

    if (debugger && debugger->callback) {
        // layers
        result = s_Vk.enumerateInstanceLayerProperties(&layerCount, nullptr);
        if (result == VK_SUCCESS) {
            VkLayerProperties* props = nullptr;
            props = palAllocate(s_Vk.allocator, sizeof(VkLayerProperties) * layerCount, 0);
            if (!props) {
                return PAL_RESULT_CODE_OUT_OF_MEMORY;
            }

            s_Vk.enumerateInstanceLayerProperties(&layerCount, props);
            for (int i = 0; i < layerCount; i++) {
                const char* name = props[i].layerName;
                if (strcmp(name, "VK_LAYER_KHRONOS_validation") == 0) {
                    hasValidationLayer = PAL_TRUE;
                    break;
                }
            }
            palFree(s_Vk.allocator, props);

            // message types
            if (!debugger->denyGeneral) {
                debugCreateInfo.messageType |= VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT;
            }

            if (!debugger->denyPerformance) {
                debugCreateInfo.messageType |= VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
            }

            if (!debugger->denyValidation) {
                debugCreateInfo.messageType |= VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT;
            }

            // message severities
            if (!debugger->denyInfoSeverity) {
                debugCreateInfo.messageSeverity |= VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT;
            }

            if (!debugger->denyWarningSeverity) {
                debugCreateInfo.messageSeverity |= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT;
            }

            if (!debugger->denyErrorSeverity) {
                debugCreateInfo.messageSeverity |= VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
            }

            debugCreateInfo.pUserData = debugger->userData;
            debugCreateInfo.pfnUserCallback = debugCallbackVk;
            s_Vk.callback = debugger->callback;
        }
    }

    // extensions
    uint32_t extCount = 0;
    const char* extensions[8];
    result = s_Vk.enumerateInstanceExtensionProperties(nullptr, &extCount, nullptr);
    if (result != VK_SUCCESS) {
        return makeResultVk(result);
    }

    VkExtensionProperties* extensionProps = nullptr;
    extensionProps = palAllocate(s_Vk.allocator, sizeof(VkExtensionProperties) * extCount, 0);
    if (!extensionProps) {
        return PAL_RESULT_SUCCESS;
    }

    PalBool hasXlib = PAL_FALSE;
    PalBool hasXcb = PAL_FALSE;
    PalBool hasWayland = PAL_FALSE;
    PalBool hasWin32 = PAL_FALSE;
    PalBool hasSurface = PAL_FALSE;
    PalBool hasExtDebug = PAL_FALSE;
    s_Vk.enumerateInstanceExtensionProperties(nullptr, &extCount, extensionProps);

    for (int i = 0; i < extCount; i++) {
        VkExtensionProperties* prop = &extensionProps[i];
        if (strcmp(prop->extensionName, "VK_KHR_xlib_surface") == 0) {
            hasXlib = PAL_TRUE;

        } else if (strcmp(prop->extensionName, "VK_KHR_xcb_surface") == 0) {
            hasXcb = PAL_TRUE;

        } else if (strcmp(prop->extensionName, "VK_KHR_wayland_surface") == 0) {
            hasWayland = PAL_TRUE;

        } else if (strcmp(prop->extensionName, "VK_KHR_win32_surface") == 0) {
            hasWin32 = PAL_TRUE;

        } else if (strcmp(prop->extensionName, "VK_KHR_surface") == 0) {
            hasSurface = PAL_TRUE;

        } else if (strcmp(prop->extensionName, "VK_EXT_debug_utils") == 0) {
            hasExtDebug = PAL_TRUE;
        }
    }
    palFree(s_Vk.allocator, extensionProps);

    int extensionCount = 0;
    if (hasSurface) {
        extensions[extensionCount++] = "VK_KHR_surface";
        if (hasWayland) {
            extensions[extensionCount++] = "VK_KHR_wayland_surface";
        }

        if (hasXlib) {
            extensions[extensionCount++] = "VK_KHR_xlib_surface";
        }

        if (hasXcb) {
            extensions[extensionCount++] = "VK_KHR_xcb_surface";
        }

        if (hasWin32) {
            extensions[extensionCount++] = "VK_KHR_win32_surface";
        }
    }

    const char* layers[2];
    layerCount = 0;
    if (hasValidationLayer && hasExtDebug) {
        extensions[extensionCount++] = "VK_EXT_debug_utils";
        layers[layerCount++] = "VK_LAYER_KHRONOS_validation";
    }

    if (versionFallback) {
        const char* name = "VK_KHR_get_physical_device_properties2";
        extensions[extensionCount++] = name;
    }

    VkApplicationInfo appInfo = {0};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.apiVersion = version;
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "Engine";
    appInfo.pApplicationName = "App";
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);

    VkInstanceCreateInfo instanceCreateInfo = {0};
    instanceCreateInfo.pApplicationInfo = &appInfo;
    instanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    instanceCreateInfo.enabledExtensionCount = extensionCount;
    instanceCreateInfo.enabledLayerCount = layerCount;
    instanceCreateInfo.ppEnabledExtensionNames = extensions;
    instanceCreateInfo.ppEnabledLayerNames = layers;

    if (debugger && debugger->callback) {
        instanceCreateInfo.pNext = &debugCreateInfo;
    }

    // vk allocator
    s_Vk.vkAllocator.pfnAllocation = allocateVk;
    s_Vk.vkAllocator.pfnFree = freeVk;
    s_Vk.vkAllocator.pfnReallocation = reallocVk;

    VkInstance instance = nullptr;
    result = s_Vk.createInstance(&instanceCreateInfo, &s_Vk.vkAllocator, &instance);
    if (result != VK_SUCCESS) {
        return makeResultVk(result);
    }

    // clang-format off
    if (versionFallback) {
        // load get physical device properties2 proc if we are on version 1.0
        s_Vk.getPhysicalDeviceFeatures2 =
            (PFN_vkGetPhysicalDeviceFeatures2KHR)s_Vk.getInstanceProcAddr(
                s_Vk.handle,
                "vkGetPhysicalDeviceFeatures2KHR");
    }

    // load surface creation function pointers
    s_Vk.createWaylandSurface = nullptr;
    s_Vk.createXlibSurface = nullptr;
    s_Vk.createXcbSurface = nullptr;
    s_Vk.createWin32Surface = nullptr;

    if (hasWayland) {
        s_Vk.createWaylandSurface = (PFN_vkCreateWaylandSurfaceKHR)s_Vk.getInstanceProcAddr(
            instance,
            "vkCreateWaylandSurfaceKHR");
    }

    if (hasXlib) {
        s_Vk.createXlibSurface = (PFN_vkCreateXlibSurfaceKHR)s_Vk.getInstanceProcAddr(
            instance,
            "vkCreateXlibSurfaceKHR");
    }

    if (hasXcb) {
        s_Vk.createXcbSurface = (PFN_vkCreateXcbSurfaceKHR)s_Vk.getInstanceProcAddr(
            instance,
            "vkCreateXcbSurfaceKHR");
    }

    if (hasWin32) {
        s_Vk.createWin32Surface = (PFN_vkCreateWin32SurfaceKHR)s_Vk.getInstanceProcAddr(
            instance,
            "vkCreateWin32SurfaceKHR");
    }

    // remaining function procs
    s_Vk.destroySurface = (PFN_vkDestroySurfaceKHR)s_Vk.getInstanceProcAddr(
        instance,
        "vkDestroySurfaceKHR");

    s_Vk.getSurfaceCapabilities =
        (PFN_vkGetPhysicalDeviceSurfaceCapabilitiesKHR)s_Vk.getInstanceProcAddr(
            instance,
            "vkGetPhysicalDeviceSurfaceCapabilitiesKHR");

    s_Vk.getSurfacePresentModes =
        (PFN_vkGetPhysicalDeviceSurfacePresentModesKHR)s_Vk.getInstanceProcAddr(
            instance,
           "vkGetPhysicalDeviceSurfacePresentModesKHR");

    s_Vk.getSurfaceFormats = (PFN_vkGetPhysicalDeviceSurfaceFormatsKHR)s_Vk.getInstanceProcAddr(
        instance,
        "vkGetPhysicalDeviceSurfaceFormatsKHR");

    s_Vk.checkSurfaceSupport = (PFN_vkGetPhysicalDeviceSurfaceSupportKHR)s_Vk.getInstanceProcAddr(
        instance,
        "vkGetPhysicalDeviceSurfaceSupportKHR");

    if (debugger) {
        s_Vk.createMessenger =
            (PFN_vkCreateDebugUtilsMessengerEXT)s_Vk.getInstanceProcAddr(
                instance,
                "vkCreateDebugUtilsMessengerEXT");

        s_Vk.destroyMessenger =
            (PFN_vkDestroyDebugUtilsMessengerEXT)s_Vk.getInstanceProcAddr(
                instance,
                "vkDestroyDebugUtilsMessengerEXT");

        s_Vk.createMessenger(instance, &debugCreateInfo, &s_Vk.vkAllocator, &s_Vk.messenger);
    }
    // clang-format on

    s_Vk.adapters = nullptr;
    s_Vk.instance = instance;
    return PAL_RESULT_SUCCESS;
}

void PAL_CALL shutdownGraphicsVk()
{
    if (s_Vk.messenger) {
        s_Vk.destroyMessenger(s_Vk.instance, s_Vk.messenger, &s_Vk.vkAllocator);
    }

    s_Vk.destroyInstance(s_Vk.instance, &s_Vk.vkAllocator);
    freeLibrary(s_Vk.handle);
    if (s_Vk.adapters) {
        palFree(s_Vk.allocator, s_Vk.adapters);
    }
    memset(&s_Vk, 0, sizeof(s_Vk));
}

#endif // PAL_HAS_VULKAN_BACKEND