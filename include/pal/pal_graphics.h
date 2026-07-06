
/**
    PAL - Prime Abstraction Layer
    Copyright (C) 2025
    Licensed under the Zlib license. See LICENSE file in root.
 */

/**
 * @defgroup pal_graphics Graphics
 * @ingroup pal_graphics
 * @{
 */

#ifndef _PAL_GRAPHICS_H
#define _PAL_GRAPHICS_H

#include "pal_core.h"

#define PAL_ADAPTER_NAME_SIZE 128
#define PAL_ADAPTER_BACKEND_NAME_SIZE 32
#define PAL_SHADER_ENTRY_NAME_SIZE 32
#define PAL_UNUSED_SHADER_INDEX UINT32_MAX

#define PAL_BACKEND_KEY ((void*)(uintptr_t)0x50414C48414E4453)
#define PAL_MAX_CUSTOM_BACKENDS 16

#define PAL_MAKE_SHADER_TARGET(major, minor) ((uint32_t)((major) << 8) | (minor))
#define PAL_SHADER_TARGET_MAJOR(target) ((uint32_t)(target) >> 8);
#define PAL_SHADER_TARGET_MINOR(target) ((uint32_t)(target) & 0xFF);

#define PAL_ADAPTER_FEATURE_NONE 0
#define PAL_ADAPTER_FEATURE_SAMPLER_ANISOTROPY (1ULL << 1)
#define PAL_ADAPTER_FEATURE_SAMPLE_RATE_SHADING (1ULL << 2)
#define PAL_ADAPTER_FEATURE_MULTI_VIEWPORT (1ULL << 3)
#define PAL_ADAPTER_FEATURE_TIMELINE_SEMAPHORE (1ULL << 4)
#define PAL_ADAPTER_FEATURE_TESSELLATION_SHADER (1ULL << 5)
#define PAL_ADAPTER_FEATURE_GEOMETRY_SHADER (1ULL << 6)
#define PAL_ADAPTER_FEATURE_SHADER_FLOAT16 (1ULL << 7)
#define PAL_ADAPTER_FEATURE_SHADER_FLOAT64 (1ULL << 8)
#define PAL_ADAPTER_FEATURE_SHADER_INT16 (1ULL << 9)
#define PAL_ADAPTER_FEATURE_SHADER_INT64 (1ULL << 10)
#define PAL_ADAPTER_FEATURE_RAY_TRACING (1ULL << 11)
#define PAL_ADAPTER_FEATURE_INDIRECT_RAY_TRACING (1ULL << 12)
#define PAL_ADAPTER_FEATURE_MESH_SHADER (1ULL << 13)
#define PAL_ADAPTER_FEATURE_FRAGMENT_SHADING_RATE (1ULL << 14)
#define PAL_ADAPTER_FEATURE_DESCRIPTOR_INDEXING (1ULL << 15)
#define PAL_ADAPTER_FEATURE_SWAPCHAIN (1ULL << 16)
#define PAL_ADAPTER_FEATURE_MULTI_VIEW (1ULL << 17)
#define PAL_ADAPTER_FEATURE_IMAGE_VIEW_CUBE_ARRAY (1ULL << 18)
#define PAL_ADAPTER_FEATURE_FENCE_RESET (1ULL << 19)
#define PAL_ADAPTER_FEATURE_POLYGON_MODE_LINE (1ULL << 20)
#define PAL_ADAPTER_FEATURE_DYNAMIC_CULL_MODE (1ULL << 21)
#define PAL_ADAPTER_FEATURE_DYNAMIC_FRONT_FACE (1ULL << 22)
#define PAL_ADAPTER_FEATURE_DYNAMIC_PRIMITIVE_TOPOLOGY (1ULL << 23)
#define PAL_ADAPTER_FEATURE_DYNAMIC_DEPTH_TEST_ENABLE (1ULL << 24)
#define PAL_ADAPTER_FEATURE_DYNAMIC_DEPTH_WRITE_ENABLE (1ULL << 25)
#define PAL_ADAPTER_FEATURE_DYNAMIC_STENCIL_OP (1ULL << 26)
#define PAL_ADAPTER_FEATURE_DEPTH_STENCIL_RESOLVE (1ULL << 27)
#define PAL_ADAPTER_FEATURE_FRAGMENT_SHADING_RATE_ATTACHMENT (1ULL << 28)
#define PAL_ADAPTER_FEATURE_BUFFER_DEVICE_ADDRESS (1ULL << 29)
#define PAL_ADAPTER_FEATURE_INDIRECT_DRAW (1ULL << 30)
#define PAL_ADAPTER_FEATURE_INDIRECT_DISPATCH (1ULL << 31)
#define PAL_ADAPTER_FEATURE_INDIRECT_DRAW_COUNT (1ULL << 32)
#define PAL_ADAPTER_FEATURE_INDIRECT_DRAW_MESH (1ULL << 33)
#define PAL_ADAPTER_FEATURE_INDIRECT_DRAW_MESH_COUNT (1ULL << 34)
#define PAL_ADAPTER_FEATURE_DISPATCH_BASE (1ULL << 35)
#define PAL_ADAPTER_FEATURE_NULL_DESCRIPTORS (1ULL << 36)
#define PAL_ADAPTER_FEATURE_RAY_QUERY (1ULL << 37)

#define PAL_ADAPTER_TYPE_UNKNOWN 0
#define PAL_ADAPTER_TYPE_DISCRETE 1
#define PAL_ADAPTER_TYPE_INTEGRATED 2
#define PAL_ADAPTER_TYPE_VIRTUAL 3
#define PAL_ADAPTER_TYPE_CPU 4
#define PAL_ADAPTER_TYPE_COUNT 5

#define PAL_ADAPTER_API_TYPE_VULKAN 0
#define PAL_ADAPTER_API_TYPE_D3D12 1
#define PAL_ADAPTER_API_TYPE_METAL 2
#define PAL_ADAPTER_API_TYPE_CUSTOM 3
#define PAL_ADAPTER_API_TYPE_COUNT 4

#define PAL_QUEUE_TYPE_GRAPHICS 0
#define PAL_QUEUE_TYPE_COMPUTE 1
#define PAL_QUEUE_TYPE_COPY 2
#define PAL_QUEUE_TYPE_COUNT 3

/** V-Sync.*/
#define PAL_PRESENT_MODE_FIFO 0
#define PAL_PRESENT_MODE_IMMEDIATE 1
#define PAL_PRESENT_MODE_MAILBOX 2
#define PAL_PRESENT_MODE_COUNT 3

#define PAL_COMPOSITE_ALPHA_OPAQUE 0
#define PAL_COMPOSITE_ALPHA_PRE_MULTIPLIED 1
#define PAL_COMPOSITE_ALPHA_POST_MULTIPLIED 2
#define PAL_COMPOSITE_ALPHA_COUNT 3

#define PAL_FORMAT_UNDEFINED 0
#define PAL_FORMAT_R8_UNORM 1
#define PAL_FORMAT_R8_SNORM 2
#define PAL_FORMAT_R8_UINT 3
#define PAL_FORMAT_R8_SINT 4
#define PAL_FORMAT_R8_SRGB 5
#define PAL_FORMAT_R16_UNORM 6
#define PAL_FORMAT_R16_SNORM 7
#define PAL_FORMAT_R16_UINT 8
#define PAL_FORMAT_R16_SINT 9
#define PAL_FORMAT_R16_SFLOAT 10
#define PAL_FORMAT_R32_UINT 11
#define PAL_FORMAT_R32_SINT 12
#define PAL_FORMAT_R32_SFLOAT 13
#define PAL_FORMAT_R64_UINT 14
#define PAL_FORMAT_R64_SINT 15
#define PAL_FORMAT_R64_SFLOAT 16
#define PAL_FORMAT_R8G8_UNORM 17
#define PAL_FORMAT_R8G8_SNORM 18
#define PAL_FORMAT_R8G8_UINT 19
#define PAL_FORMAT_R8G8_SINT 20
#define PAL_FORMAT_R8G8_SRGB 21
#define PAL_FORMAT_R16G16_UNORM 22
#define PAL_FORMAT_R16G16_SNORM 23
#define PAL_FORMAT_R16G16_UINT 24
#define PAL_FORMAT_R16G16_SINT 25
#define PAL_FORMAT_R16G16_SFLOAT 26
#define PAL_FORMAT_R32G32_UINT 27
#define PAL_FORMAT_R32G32_SINT 28
#define PAL_FORMAT_R32G32_SFLOAT 29
#define PAL_FORMAT_R64G64_UINT 30
#define PAL_FORMAT_R64G64_SINT 31
#define PAL_FORMAT_R64G64_SFLOAT 32
#define PAL_FORMAT_R8G8B8_UNORM 33
#define PAL_FORMAT_R8G8B8_SNORM 34
#define PAL_FORMAT_R8G8B8_UINT 35
#define PAL_FORMAT_R8G8B8_SINT 36
#define PAL_FORMAT_R8G8B8_SRGB 37
#define PAL_FORMAT_R16G16B16_UNORM 38
#define PAL_FORMAT_R16G16B16_SNORM 39
#define PAL_FORMAT_R16G16B16_UINT 40
#define PAL_FORMAT_R16G16B16_SINT 41
#define PAL_FORMAT_R16G16B16_SFLOAT 42
#define PAL_FORMAT_R32G32B32_UINT 43
#define PAL_FORMAT_R32G32B32_SINT 44
#define PAL_FORMAT_R32G32B32_SFLOAT 45
#define PAL_FORMAT_R64G64B64_UINT 46
#define PAL_FORMAT_R64G64B64_SINT 47
#define PAL_FORMAT_R64G64B64_SFLOAT 48
#define PAL_FORMAT_B8G8R8_UNORM 49
#define PAL_FORMAT_B8G8R8_SNORM 50
#define PAL_FORMAT_B8G8R8_UINT 51
#define PAL_FORMAT_B8G8R8_SINT 52
#define PAL_FORMAT_B8G8R8_SRGB 53
#define PAL_FORMAT_R8G8B8A8_UNORM 54
#define PAL_FORMAT_R8G8B8A8_SNORM 55
#define PAL_FORMAT_R8G8B8A8_UINT 56
#define PAL_FORMAT_R8G8B8A8_SINT 57
#define PAL_FORMAT_R8G8B8A8_SRGB 58
#define PAL_FORMAT_R16G16B16A16_UNORM 59
#define PAL_FORMAT_R16G16B16A16_SNORM 60
#define PAL_FORMAT_R16G16B16A16_UINT 61
#define PAL_FORMAT_R16G16B16A16_SINT 62
#define PAL_FORMAT_R16G16B16A16_SFLOAT 63
#define PAL_FORMAT_R32G32B32A32_UINT 64
#define PAL_FORMAT_R32G32B32A32_SINT 65
#define PAL_FORMAT_R32G32B32A32_SFLOAT 66
#define PAL_FORMAT_R64G64B64A64_UINT 67
#define PAL_FORMAT_R64G64B64A64_SINT 68
#define PAL_FORMAT_R64G64B64A64_SFLOAT 69
#define PAL_FORMAT_B8G8R8A8_UNORM 70
#define PAL_FORMAT_B8G8R8A8_SNORM 71
#define PAL_FORMAT_B8G8R8A8_UINT 72
#define PAL_FORMAT_B8G8R8A8_SINT 73
#define PAL_FORMAT_B8G8R8A8_SRGB 74
#define PAL_FORMAT_S8_UINT 75
#define PAL_FORMAT_D16_UNORM 76
#define PAL_FORMAT_D32_SFLOAT 77
#define PAL_FORMAT_D16_UNORM_S8_UINT 78
#define PAL_FORMAT_D32_SFLOAT_S8_UINT 79
#define PAL_FORMAT_D24_UNORM_S8_UINT 80
#define PAL_FORMAT_COUNT 81

#define PAL_IMAGE_USAGE_UNDEFINED 0
#define PAL_IMAGE_USAGE_COLOR_ATTACHEMENT (1U << 0)
#define PAL_IMAGE_USAGE_DEPTH_ATTACHEMENT (1U << 1)
#define PAL_IMAGE_USAGE_TRANSFER_SRC (1U << 2)
#define PAL_IMAGE_USAGE_TRANSFER_DST (1U << 3)
#define PAL_IMAGE_USAGE_STORAGE (1U << 4)
#define PAL_IMAGE_USAGE_SAMPLED (1U << 5)

#define PAL_SHADER_FORMAT_SPIRV (1U << 0)
#define PAL_SHADER_FORMAT_DXIL (1U << 1)
#define PAL_SHADER_FORMAT_DXBC (1U << 2)
#define PAL_SHADER_FORMAT_MSL (1U << 3)
#define PAL_SHADER_FORMAT_CUSTOM (1U << 4)

#define PAL_LOAD_OP_LOAD 0
#define PAL_LOAD_OP_CLEAR 1
#define PAL_LOAD_OP_DONT_CARE 2
#define PAL_LOAD_OP_COUNT 3

#define PAL_STORE_OP_STORE 0
#define PAL_STORE_OP_DONT_CARE 1
#define PAL_STORE_OP_COUNT 2

#define PAL_MEMORY_TYPE_GPU_ONLY 0
#define PAL_MEMORY_TYPE_CPU_UPLOAD 1
#define PAL_MEMORY_TYPE_CPU_READBACK 2
#define PAL_MEMORY_TYPE_COUNT 3

#define PAL_IMAGE_TYPE_1D 0
#define PAL_IMAGE_TYPE_2D 1
#define PAL_IMAGE_TYPE_3D 2
#define PAL_IMAGE_TYPE_COUNT 3

#define PAL_IMAGE_ASPECT_COLOR 0
#define PAL_IMAGE_ASPECT_DEPTH 1
#define PAL_IMAGE_ASPECT_STENCIL 2
#define PAL_IMAGE_ASPECT_DEPTH_STENCIL 3
#define PAL_IMAGE_ASPECT_COUNT 4

#define PAL_IMAGE_VIEW_TYPE_1D 0
#define PAL_IMAGE_VIEW_TYPE_1D_ARRAY 1
#define PAL_IMAGE_VIEW_TYPE_2D 2
#define PAL_IMAGE_VIEW_TYPE_2D_ARRAY 3
#define PAL_IMAGE_VIEW_TYPE_3D 4
#define PAL_IMAGE_VIEW_TYPE_CUBE 5
#define PAL_IMAGE_VIEW_TYPE_CUBE_ARRAY 6
#define PAL_IMAGE_VIEW_TYPE_COUNT 7

#define PAL_FILTER_MODE_NEAREST 0
#define PAL_FILTER_MODE_LINEAR 1
#define PAL_FILTER_MODE_COUNT 2

#define PAL_SAMPLER_MIPMAP_MODE_NEAREST 0
#define PAL_SAMPLER_MIPMAP_MODE_LINEAR 1
#define PAL_SAMPLER_MIPMAP_MODE_COUNT 2

#define PAL_SAMPLER_ADDRESS_MODE_REPEAT 0
#define PAL_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT 1
#define PAL_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE 2
#define PAL_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER 3
#define PAL_SAMPLER_ADDRESS_MODE_COUNT 4

#define PAL_BORDER_COLOR_FLOAT_TRANSPARENT_BLACK 0
#define PAL_BORDER_COLOR_INT_TRANSPARENT_BLACK 1
#define PAL_BORDER_COLOR_FLOAT_OPAQUE_BLACK 2
#define PAL_BORDER_COLOR_INT_OPAQUE_BLACK 3
#define PAL_BORDER_COLOR_FLOAT_OPAQUE_WHITE 4
#define PAL_BORDER_COLOR_INT_OPAQUE_WHITE 5
#define PAL_BORDER_COLOR_COUNT 6

#define PAL_SURFACE_FORMAT_BGRA8_UNORM_SRGB_NONLINEAR 0
#define PAL_SURFACE_FORMAT_BGRA8_SRGB_NONLINEAR 1
#define PAL_SURFACE_FORMAT_RGBA8_UNORM_SRGB_NONLINEAR 2
#define PAL_SURFACE_FORMAT_RGBA16_FLOAT_HDR10 3
#define PAL_SURFACE_FORMAT_COUNT 4

#define PAL_WINDOW_INSTANCE_TYPE_WAYLAND 0
#define PAL_WINDOW_INSTANCE_TYPE_X11 1
#define PAL_WINDOW_INSTANCE_TYPE_XCB 2
#define PAL_WINDOW_INSTANCE_TYPE_WIN32 3
#define PAL_WINDOW_INSTANCE_TYPE_COUNT 4

#define PAL_SHADER_STAGE_UNDEFINED 0
#define PAL_SHADER_STAGE_VERTEX 1
#define PAL_SHADER_STAGE_FRAGMENT 2
#define PAL_SHADER_STAGE_COMPUTE 3
#define PAL_SHADER_STAGE_GEOMETRY 4
#define PAL_SHADER_STAGE_MESH 5
#define PAL_SHADER_STAGE_TASK 6
#define PAL_SHADER_STAGE_TESSELLATION_CONTROL 7
#define PAL_SHADER_STAGE_TESSELLATION_EVALUATION 8
#define PAL_SHADER_STAGE_RAYGEN 9
#define PAL_SHADER_STAGE_CLOSEST_HIT 10
#define PAL_SHADER_STAGE_ANY_HIT 11
#define PAL_SHADER_STAGE_MISS 12
#define PAL_SHADER_STAGE_INTERSECTION 13
#define PAL_SHADER_STAGE_CALLABLE 14
#define PAL_SHADER_STAGE_COUNT 15

#define PAL_SAMPLE_COUNT_1 0
#define PAL_SAMPLE_COUNT_2 1
#define PAL_SAMPLE_COUNT_4 2
#define PAL_SAMPLE_COUNT_8 3
#define PAL_SAMPLE_COUNT_16 4
#define PAL_SAMPLE_COUNT_32 5
#define PAL_SAMPLE_COUNT_64 6
#define PAL_SAMPLE_COUNT_COUNT 7 /**< Name redundancy is intentionally consistent.*/

#define PAL_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST 0
#define PAL_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP 1
#define PAL_PRIMITIVE_TOPOLOGY_LINE_LIST 2
#define PAL_PRIMITIVE_TOPOLOGY_LINE_STRIP 3
#define PAL_PRIMITIVE_TOPOLOGY_POINT_LIST 4
#define PAL_PRIMITIVE_TOPOLOGY_PATCH 5
#define PAL_PRIMITIVE_TOPOLOGY_COUNT 6

#define PAL_CULL_MODE_NONE 0
#define PAL_CULL_MODE_FRONT 1
#define PAL_CULL_MODE_BACK 2
#define PAL_CULL_MODE_COUNT 3

#define PAL_FRONT_FACE_CLOCKWISE 0
#define PAL_FRONT_FACE_COUNTER_CLOCKWISE 1
#define PAL_FRONT_FACE_COUNT 2

#define PAL_POLYGON_MODE_FILL 0
#define PAL_POLYGON_MODE_LINE 1
#define PAL_POLYGON_MODE_COUNT 2

#define PAL_STENCIL_FACE_FLAG_FRONT (1U << 0)
#define PAL_STENCIL_FACE_FLAG_BACK (1U << 1)
#define PAL_STENCIL_FACE_FLAG_BOTH (PAL_STENCIL_FACE_FLAG_FRONT | PAL_STENCIL_FACE_FLAG_BACK)

#define PAL_VERTEX_TYPE_UNDEFINED 0
#define PAL_VERTEX_TYPE_INT32 1          
#define PAL_VERTEX_TYPE_INT32_2 2        
#define PAL_VERTEX_TYPE_INT32_3 3        
#define PAL_VERTEX_TYPE_INT32_4 4        
#define PAL_VERTEX_TYPE_UINT32 5         
#define PAL_VERTEX_TYPE_UINT32_2 6       
#define PAL_VERTEX_TYPE_UINT32_3 7       
#define PAL_VERTEX_TYPE_UINT32_4 8       
#define PAL_VERTEX_TYPE_INT8_2 9         
#define PAL_VERTEX_TYPE_INT8_4 10        
#define PAL_VERTEX_TYPE_UINT8_2 11       
#define PAL_VERTEX_TYPE_UINT8_4 12       
#define PAL_VERTEX_TYPE_INT8_2NORM 13    
#define PAL_VERTEX_TYPE_INT8_4NORM 14    
#define PAL_VERTEX_TYPE_UINT8_2NORM 15   
#define PAL_VERTEX_TYPE_UINT8_4NORM 16   
#define PAL_VERTEX_TYPE_INT16_2 17       
#define PAL_VERTEX_TYPE_INT16_4 18       
#define PAL_VERTEX_TYPE_UINT16_2 19      
#define PAL_VERTEX_TYPE_UINT16_4 20      
#define PAL_VERTEX_TYPE_INT16_2NORM 21   
#define PAL_VERTEX_TYPE_INT16_4NORM 22   
#define PAL_VERTEX_TYPE_UINT16_2NORM 23  
#define PAL_VERTEX_TYPE_UINT16_4NORM 24  
#define PAL_VERTEX_TYPE_FLOAT 25         
#define PAL_VERTEX_TYPE_FLOAT2 26        
#define PAL_VERTEX_TYPE_FLOAT3 27        
#define PAL_VERTEX_TYPE_FLOAT4 28        
#define PAL_VERTEX_TYPE_HALF_FLOAT16_2 29
#define PAL_VERTEX_TYPE_HALF_FLOAT16_4 30
#define PAL_VERTEX_TYPE_COUNT 31

#define PAL_VERTEX_SEMANTIC_ID_POSITION 0
#define PAL_VERTEX_SEMANTIC_ID_COLOR 1
#define PAL_VERTEX_SEMANTIC_ID_TEXCOORD 2
#define PAL_VERTEX_SEMANTIC_ID_NORMAL 3
#define PAL_VERTEX_SEMANTIC_ID_TANGENT 4
#define PAL_VERTEX_SEMANTIC_ID_COUNT 5

#define PAL_COMMAND_BUFFER_TYPE_PRIMARY 0
#define PAL_COMMAND_BUFFER_TYPE_SECONDARY 1
#define PAL_COMMAND_BUFFER_TYPE_COUNT 2

#define PAL_VERTEX_LAYOUT_TYPE_PER_VERTEX 0
#define PAL_VERTEX_LAYOUT_TYPE_PER_INSTANCE 1
#define PAL_VERTEX_LAYOUT_TYPE_COUNT 2

#define PAL_COMPARE_OP_NEVER 0
#define PAL_COMPARE_OP_LESS 1
#define PAL_COMPARE_OP_EQUAL 2
#define PAL_COMPARE_OP_LESS_OR_EQUAL 3
#define PAL_COMPARE_OP_GREATER 4
#define PAL_COMPARE_OP_NOT_EQUAL 5
#define PAL_COMPARE_OP_GREATER_OR_EQUAL 6
#define PAL_COMPARE_OP_ALWAYS 7
#define PAL_COMPARE_OP_COUNT 8

#define PAL_STENCIL_OP_KEEP 0
#define PAL_STENCIL_OP_ZERO 1
#define PAL_STENCIL_OP_REPLACE 2
#define PAL_STENCIL_OP_INCREMENT_AND_CLAMP 3
#define PAL_STENCIL_OP_DECREMENT_AND_CLAMP 4
#define PAL_STENCIL_OP_INVERT 5
#define PAL_STENCIL_OP_INCREMENT_AND_WRAP 6
#define PAL_STENCIL_OP_DECREMENT_AND_WRAP 7
#define PAL_STENCIL_OP_COUNT 8

#define PAL_BLEND_OP_ADD 0
#define PAL_BLEND_OP_SUBTRACT 1
#define PAL_BLEND_OP_REVERSE_SUBTRACT 2
#define PAL_BLEND_OP_MIN 3
#define PAL_BLEND_OP_MAX 4
#define PAL_BLEND_OP_COUNT 5

#define PAL_BLEND_FACTOR_ZERO 0
#define PAL_BLEND_FACTOR_ONE 1
#define PAL_BLEND_FACTOR_SRC_COLOR 2
#define PAL_BLEND_FACTOR_ONE_MINUS_SRC_COLOR 3
#define PAL_BLEND_FACTOR_DST_COLOR 4
#define PAL_BLEND_FACTOR_ONE_MINUS_DST_COLOR 5
#define PAL_BLEND_FACTOR_SRC_ALPHA 6
#define PAL_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA 7
#define PAL_BLEND_FACTOR_DST_ALPHA 8
#define PAL_BLEND_FACTOR_ONE_MINUS_DST_ALPHA 9
#define PAL_BLEND_FACTOR_CONSTANT_COLOR 10
#define PAL_BLEND_FACTOR_ONE_MINUS_CONSTANT_COLOR 11
#define PAL_BLEND_FACTOR_CONSTANT_ALPHA 12
#define PAL_BLEND_FACTOR_ONE_MINUS_CONSTANT_ALPHA 13
#define PAL_BLEND_FACTOR_COUNT 14

#define PAL_COLOR_MASK_NONE 0
#define PAL_COLOR_MASK_RED (1U << 0)
#define PAL_COLOR_MASK_GREEN (1U << 1)
#define PAL_COLOR_MASK_BLUE (1U << 2)
#define PAL_COLOR_MASK_ALPHA (1U << 3)

#define PAL_RESOLVE_MODE_NONE 0
#define PAL_RESOLVE_MODE_SAMPLE_ZERO 1
#define PAL_RESOLVE_MODE_AVERAGE 2
#define PAL_RESOLVE_MODE_MIN 3
#define PAL_RESOLVE_MODE_MAX 4
#define PAL_RESOLVE_MODE_COUNT 5

#define PAL_FRAGMENT_SHADING_RATE_1X1 0
#define PAL_FRAGMENT_SHADING_RATE_1X2 1
#define PAL_FRAGMENT_SHADING_RATE_2X1 2
#define PAL_FRAGMENT_SHADING_RATE_2X2 3
#define PAL_FRAGMENT_SHADING_RATE_2X4 4
#define PAL_FRAGMENT_SHADING_RATE_4X2 5
#define PAL_FRAGMENT_SHADING_RATE_4X4 6
#define PAL_FRAGMENT_SHADING_RATE_COUNT 7

#define PAL_FRAGMENT_SHADING_RATE_COMBINER_OP_KEEP 0
#define PAL_FRAGMENT_SHADING_RATE_COMBINER_OP_REPLACE 1
#define PAL_FRAGMENT_SHADING_RATE_COMBINER_OP_MIN 2
#define PAL_FRAGMENT_SHADING_RATE_COMBINER_OP_MAX 3
#define PAL_FRAGMENT_SHADING_RATE_COMBINER_OP_MUL 4
#define PAL_FRAGMENT_SHADING_RATE_COMBINER_OP_COUNT 5

#define PAL_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL 0
#define PAL_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL 1
#define PAL_ACCELERATION_STRUCTURE_TYPE_COUNT 2

#define PAL_ACCELERATION_STRUCTURE_BUILD_MODE_BUILD 0
#define PAL_ACCELERATION_STRUCTURE_BUILD_MODE_UPDATE 1
#define PAL_ACCELERATION_STRUCTURE_BUILD_MODE_COUNT 2

#define PAL_ACCELERATION_STRUCTURE_BUILD_HINT_FAST_BUILD (1U << 0)
#define PAL_ACCELERATION_STRUCTURE_BUILD_HINT_FAST_TRACE (1U << 1)
#define PAL_ACCELERATION_STRUCTURE_BUILD_HINT_LOW_MEMORY (1U << 2)

#define PAL_ACCELERATION_STRUCTURE_INSTANCE_FLAG_FORCE_OPAQUE (1U << 0)
#define PAL_ACCELERATION_STRUCTURE_INSTANCE_FLAG_FORCE_NO_OPAQUE (1U << 1)
#define PAL_ACCELERATION_STRUCTURE_INSTANCE_FLAG_TRIANGLE_FACING_CULL_DISABLE (1U << 2)
#define PAL_ACCELERATION_STRUCTURE_INSTANCE_FLAG_TRIANGLE_FRONT_COUNTERCLOCKWISE (1U << 3)

#define ePAL_ACCELERATION_STRUCTURE_CREATE_FLAG_NONE 0

#define PAL_GEOMETRY_TYPE_TRIANGLE 0
#define PAL_GEOMETRY_TYPE_AABBS 1
#define PAL_GEOMETRY_TYPE_COUNT 2

#define PAL_GEOMETRY_FLAG_OPAQUE (1U << 0)
#define PAL_GEOMETRY_FLAG_NO_DUPLICATE_ANYHIT (1U << 1)

#define PAL_INDEX_TYPE_UINT16 0
#define PAL_INDEX_TYPE_UINT32 1
#define PAL_INDEX_TYPE_COUNT 2

#define PAL_BUFFER_USAGE_VERTEX (1U << 0)
#define PAL_BUFFER_USAGE_INDEX (1U << 1)
#define PAL_BUFFER_USAGE_UNIFORM (1U << 2)
#define PAL_BUFFER_USAGE_STORAGE (1U << 3)
#define PAL_BUFFER_USAGE_TRANSFER_SRC (1U << 4)
#define PAL_BUFFER_USAGE_TRANSFER_DST (1U << 5)
#define PAL_BUFFER_USAGE_ACCELERATION_STRUCTURE (1U << 6)
#define PAL_BUFFER_USAGE_ACCELERATION_STRUCTURE_SCRATCH (1U << 7)
#define PAL_BUFFER_USAGE_ACCELERATION_STRUCTURE_READ_ONLY_INPUT (1U << 8)
#define PAL_BUFFER_USAGE_DEVICE_ADDRESS (1U << 9)
#define PAL_BUFFER_USAGE_INDIRECT (1U << 10)

#define PAL_DEBUG_MESSAGE_SEVERITY_INFO 0
#define PAL_DEBUG_MESSAGE_SEVERITY_WARNING 1
#define PAL_DEBUG_MESSAGE_SEVERITY_ERROR 2
#define PAL_DEBUG_MESSAGE_SEVERITY_COUNT 3

#define PAL_DEBUG_MESSAGE_TYPE_GENERAL 0
#define PAL_DEBUG_MESSAGE_TYPE_VALIDATION 1
#define PAL_DEBUG_MESSAGE_TYPE_PERFORMANCE 2
#define PAL_DEBUG_MESSAGE_TYPE_COUNT 3

#define PAL_USAGE_STATE_UNDEFINED 0
#define PAL_USAGE_STATE_PRESENT 1
#define PAL_USAGE_STATE_COLOR_ATTACHMENT_WRITE 2
#define PAL_USAGE_STATE_DEPTH_ATTACHMENT_READ 3
#define PAL_USAGE_STATE_DEPTH_ATTACHMENT_WRITE 4
#define PAL_USAGE_STATE_STENCIL_ATTACHMENT_READ 5
#define PAL_USAGE_STATE_STENCIL_ATTACHMENT_WRITE 6
#define PAL_USAGE_STATE_FRAGMENT_SHADING_RATE_ATTACHMENT_READ 7
#define PAL_USAGE_STATE_TRANSFER_READ 8
#define PAL_USAGE_STATE_TRANSFER_WRITE 9
#define PAL_USAGE_STATE_VERTEX_READ 10
#define PAL_USAGE_STATE_INDEX_READ 11
#define PAL_USAGE_STATE_INDIRECT_READ 12
#define PAL_USAGE_STATE_UNIFORM_READ 13
#define PAL_USAGE_STATE_SHADER_READ 14
#define PAL_USAGE_STATE_SHADER_WRITE 15
#define PAL_USAGE_STATE_STORAGE_READ 16
#define PAL_USAGE_STATE_STORAGE_WRITE 17
#define PAL_USAGE_STATE_HOST_READ 18
#define PAL_USAGE_STATE_HOST_WRITE 19
#define PAL_USAGE_STATE_ACCELERATION_STRUCTURE_READ 20
#define PAL_USAGE_STATE_ACCELERATION_STRUCTURE_WRITE 21
#define PAL_USAGE_STATE_COUNT 22

#define PAL_DESCRIPTOR_TYPE_STORAGE_BUFFER 0
#define PAL_DESCRIPTOR_TYPE_UNIFORM_BUFFER 1
#define PAL_DESCRIPTOR_TYPE_SAMPLED_IMAGE 2
#define PAL_DESCRIPTOR_TYPE_STORAGE_IMAGE 3
#define PAL_DESCRIPTOR_TYPE_SAMPLER 4
#define PAL_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE 5
#define PAL_DESCRIPTOR_TYPE_COUNT 6

#define PAL_DESCRIPTOR_INDEXING_FLAG_NONE 0
#define PAL_DESCRIPTOR_INDEXING_FLAG_UPDATE_AFTER_BIND (1U << 0)
#define PAL_DESCRIPTOR_INDEXING_FLAG_PARTIALLY_BOUND (1U << 1)
#define PAL_DESCRIPTOR_INDEXING_FLAG_NON_UNIFORM_INDEXING (1U << 2)

#define PAL_ACCELERATION_STRUCTURE_BUILD_HINT_FAST_TRACE (1U << 1)
#define PAL_ACCELERATION_STRUCTURE_BUILD_HINT_LOW_MEMORY (1U << 2)

#define PAL_RAY_TRACING_SHADER_GROUP_TYPE_GENERAL 0
#define PAL_RAY_TRACING_SHADER_GROUP_TYPE_PROCEDURAL_HIT 1
#define PAL_RAY_TRACING_SHADER_GROUP_TYPE_TRIANGLES_HIT 2
#define PAL_RAY_TRACING_SHADER_GROUP_TYPE_COUNT 3

#define PAL_BUFFER_MEMORY_USAGE_MANUAL 0
#define PAL_BUFFER_MEMORY_USAGE_AUTO_GPU_ONLY 1
#define PAL_BUFFER_MEMORY_USAGE_AUTO_CPU_UPLOAD 2
#define PAL_BUFFER_MEMORY_USAGE_AUTO_CPU_READBACK 3
#define PAL_BUFFER_MEMORY_USAGE_COUNT 4

#define PAL_IMAGE_MEMORY_USAGE_MANUAL 0
#define PAL_IMAGE_MEMORY_USAGE_AUTO_GPU_ONLY 1
#define PAL_IMAGE_MEMORY_USAGE_COUNT 2

#define PAL_RENDERING_FLAG_NONE 0
#define PAL_RENDERING_FLAG_SUSPENDING (1U << 0)
#define PAL_RENDERING_FLAG_RESUMING (1U << 1)

#define PAL_GRAPHICS_BACKEND_VTABLE_VERSION_1 0

/**
 * @struct PalAdapter
 * @brief Opaque handle to an adapter (GPU).
 *
 * @since 2.0
 */
typedef struct PalAdapter PalAdapter;

/**
 * @struct PalDevice
 * @brief Opaque handle to a device. Devices are created from an adapter (GPU).
 *
 * @since 2.0
 */
typedef struct PalDevice PalDevice;

/**
 * @struct PalMemory
 * @brief Opaque handle to a device memory. This is not `CPU` memory.
 *
 * @since 2.0
 */
typedef struct PalMemory PalMemory;

/**
 * @struct PalQueue
 * @brief Opaque handle to a queue.
 *
 * @since 2.0
 */
typedef struct PalQueue PalQueue;

/**
 * @struct PalSurface
 * @brief Opaque handle to a surface.
 *
 * @since 2.0
 */
typedef struct PalSurface PalSurface;

/**
 * @struct PalSwapchain
 * @brief Opaque handle to a swapchain.
 *
 * @since 2.0
 */
typedef struct PalSwapchain PalSwapchain;

/**
 * @struct PalImage
 * @brief Opaque handle to an image.
 *
 * @since 2.0
 */
typedef struct PalImage PalImage;

/**
 * @struct PalImageView
 * @brief Opaque handle to an image view.
 *
 * @since 2.0
 */
typedef struct PalImageView PalImageView;

/**
 * @struct PalShader
 * @brief Opaque handle to a shader.
 *
 * @since 2.0
 */
typedef struct PalShader PalShader;

/**
 * @struct PalBuffer
 * @brief Opaque handle to a buffer.
 *
 * @since 2.0
 */
typedef struct PalBuffer PalBuffer;

/**
 * @struct PalFence
 * @brief Opaque handle to a fence.
 *
 * @since 2.0
 */
typedef struct PalFence PalFence;

/**
 * @struct PalSemaphore
 * @brief Opaque handle to a semaphore.
 *
 * @since 2.0
 */
typedef struct PalSemaphore PalSemaphore;

/**
 * @struct PalCommandPool
 * @brief Opaque handle to a command pool.
 *
 * @since 2.0
 */
typedef struct PalCommandPool PalCommandPool;

/**
 * @struct PalCommandBuffer
 * @brief Opaque handle to a command buffer.
 *
 * @since 2.0
 */
typedef struct PalCommandBuffer PalCommandBuffer;

/**
 * @struct PalDescriptorSetLayout
 * @brief Opaque handle to a descriptor set layout.
 *
 * This defines the layout, ordering and the number of descriptors a descriptor set uses.
 *
 * The layouts should reflect the exact layout of the shaders. Eg.
 * descriptorBindings[2] = { sampler, sampled image } is different from
 * descriptorBindings[2] = { sampled image, sampler }. The ordering must be correct.
 *
 * @since 2.0
 */
typedef struct PalDescriptorSetLayout PalDescriptorSetLayout;

/**
 * @struct PalDescriptorPool
 * @brief Opaque handle to a descriptor pool.
 *
 * @since 2.0
 */
typedef struct PalDescriptorPool PalDescriptorPool;

/**
 * @struct PalDescriptorSet
 * @brief Opaque handle to a descriptor set.
 *
 * @since 2.0
 */
typedef struct PalDescriptorSet PalDescriptorSet;

/**
 * @struct PalSampler
 * @brief Opaque handle to a sampler.
 *
 * @since 2.0
 */
typedef struct PalSampler PalSampler;

/**
 * @struct PalPipelineLayout
 * @brief Opaque handle to a pipeline layout.
 *
 * @since 2.0
 */
typedef struct PalPipelineLayout PalPipelineLayout;

/**
 * @struct PalPipeline
 * @brief Opaque handle to a pipeline. This is the same handle used for all pipeline types
 * (Graphics, Compute and Ray tracing).
 *
 * @since 2.0
 */
typedef struct PalPipeline PalPipeline;

/**
 * @struct PalShaderBindingTable
 * @brief Opaque handle to a shader binding table.
 *
 * @since 2.0
 */
typedef struct PalShaderBindingTable PalShaderBindingTable;

/**
 * @struct PalAccelerationStructure
 * @brief Opaque handle to an acceleration structure.
 *
 * @since 2.0
 */
typedef struct PalAccelerationStructure PalAccelerationStructure;

/**
 * @typedef PalDebugMessageSeverity
 * @brief Debugger messages severity types used to filter incoming messages.
 *
 * All message severities follow the format `PAL_DEBUG_MESSAGE_SEVERITY_**` for
 * consistency and API use.
 *
 * @since 2.0
 */
typedef uint32_t PalDebugMessageSeverity;

/**
 * @typedef PalDebugMessageType
 * @brief Debugger messages types used to filter incoming messages.
 *
 * All message types follow the format `PAL_DEBUG_MESSAGE_TYPE_**` for
 * consistency and API use.
 *
 * @since 2.0
 */
typedef uint32_t PalDebugMessageType;

/**
 * @typedef PalDeviceAddress
 * @brief Adapter address. Used to get adapter (GPU) address of mostly buffers.
 *
 * @since 2.0
 */
typedef uint64_t PalDeviceAddress;

/**
 * @typedef PalAdapterFeatures
 * @brief Adapter features. This is a bitmask.
 *
 * All adapter features follow the format `PAL_ADAPTER_FEATURE_**` for
 * consistency and API use.
 *
 * @since 2.0
 */
typedef uint64_t PalAdapterFeatures;

/**
 * @typedef PalAdapterType
 * @brief Adapter (GPU) types.
 *
 * All adapter types follow the format `PAL_ADAPTER_TYPE_**` for
 * consistency and API use.
 *
 * @since 2.0
 */
typedef uint32_t PalAdapterType;

/**
 * @typedef PalAdapterApiType
 * @brief Adapter API types.
 *
 * All adapter api types follow the format `PAL_ADAPTER_API_TYPE_**` for
 * consistency and API use.
 *
 * Customs backends that dont fit the already declared api types should use
 * `PAL_ADAPTER_API_TYPE_CUSTOM`.
 *
 * @since 2.0
 */
typedef uint32_t PalAdapterApiType;

/**
 * @typedef PalQueueType
 * @brief Queue types.
 *
 * All queue types follow the format `PAL_QUEUE_TYPE_**` for
 * consistency and API use.
 *
 * @since 2.0
 */
typedef uint32_t PalQueueType;

/**
 * @typedef PalPresentMode
 * @brief Present modes
 *
 * All present modes follow the format `PAL_PRESENT_MODE_**` for
 * consistency and API use.
 *
 * @since 2.0
 */
typedef uint32_t PalPresentMode;

/**
 * @typedef PalCompositeAplha
 * @brief Composite alphas
 *
 * All composite alphas follow the format `PAL_COMPOSITE_ALPHA_**` for
 * consistency and API use.
 *
 * @since 2.0
 */
typedef uint32_t PalCompositeAplha;

/**
 * @typedef PalFormat
 * @brief Format types.
 *
 * All format types follow the format `PAL_FORMAT_**` for
 * consistency and API use.
 *
 * @since 2.0
 */
typedef uint32_t PalFormat;

/**
 * @typedef PalImageUsages
 * @brief Image usages. Multiple image usages can be OR'ed together using bitwise
 * OR operator (`|`).
 *
 * All image usages follow the format `PAL_IMAGE_USAGE_**` for
 * consistency and API use.
 *
 * @since 2.0
 */
typedef uint32_t PalImageUsages;

/**
 * @typedef PalShaderFormats
 * @brief Shader formats. This is a bitmask.
 *
 * All shader formats follow the format `PAL_SHADER_FORMAT_**` for
 * consistency and API use.
 *
 * @since 2.0
 */
typedef uint32_t PalShaderFormats;

/**
 * @typedef PalLoadOp
 * @brief Load operation type.
 *
 * All load operation type follow the format `PAL_LOAD_OP_**` for
 * consistency and API use.
 *
 * @since 2.0
 */
typedef uint32_t PalLoadOp;

/**
 * @typedef PalStoreOp
 * @brief Store operation type.
 *
 * All store operation type follow the format `PAL_STORE_OP_**` for
 * consistency and API use.
 *
 * @since 2.0
 */
typedef uint32_t PalStoreOp;

/**
 * @typedef PalMemoryType
 * @brief Memory types.
 *
 * All memory types follow the format `PAL_MEMORY_TYPE_**` for
 * consistency and API use.
 *
 * @since 2.0
 */
typedef uint32_t PalMemoryType;

/**
 * @typedef PalImageType
 * @brief Image types.
 *
 * All image types follow the format `PAL_IMAGE_TYPE_**` for
 * consistency and API use.
 *
 * @since 2.0
 */
typedef uint32_t PalImageType;

/**
 * @typedef PalImageAspect
 * @brief Image aspects.
 *
 * All image aspect follow the format `PAL_IMAGE_ASPECT_**` for
 * consistency and API use.
 *
 * @since 2.0
 */
typedef uint32_t PalImageAspect;

/**
 * @typedef PalImageViewType
 * @brief Image view types.
 *
 * All image view types follow the format `PAL_IMAGE_VIEW_TYPE_**` for
 * consistency and API use.
 *
 * @since 2.0
 */
typedef uint32_t PalImageViewType;

/**
 * @typedef PalFilterMode
 * @brief Filter modes.
 *
 * All filter modes follow the format `PAL_FILTER_MODE_**` for
 * consistency and API use.
 *
 * @since 2.0
 */
typedef uint32_t PalFilterMode;

/**
 * @typedef PalSamplerMipmapMode
 * @brief Sampler mipmap modes.
 *
 * All sampler mipmap modes follow the format `PAL_SAMPLER_MIPMAP_MODE_**` for
 * consistency and API use.
 *
 * @since 2.0
 */
typedef uint32_t PalSamplerMipmapMode;

/**
 * @typedef PalSamplerAddressMode
 * @brief Sampler address modes.
 *
 * All sampler address modes follow the format `PAL_SAMPLER_ADDRESS_MODE_**` for
 * consistency and API use.
 *
 * @since 2.0
 */
typedef uint32_t PalSamplerAddressMode;

/**
 * @typedef PalBorderColor
 * @brief Border color.
 *
 * All border colors follow the format `PAL_BORDER_COLOR_**` for
 * consistency and API use.
 *
 * @since 2.0
 */
typedef uint32_t PalBorderColor;

/**
 * @typedef PalSurfaceFormat
 * @brief Surface format types.
 *
 * All surface format types follow the format `PAL_SURFACE_FORMAT_**` for
 * consistency and API use.
 *
 * @since 2.0
 */
typedef uint32_t PalSurfaceFormat;

/**
 * @typedef WindowInstanceType
 * @brief Display types for a window.
 *
 * All window display types follow the format `PAL_WINDOW_INSTANCE_TYPE_**` for
 * consistency and API use.
 *
 * @since 2.0
 */
typedef uint32_t PalWindowInstanceType;

/**
 * @typedef PalShaderStage
 * @brief shader stage types.
 *
 * All shader stage types follow the format `PAL_SHADER_STAGE_**` for
 * consistency and API use.
 *
 * @since 2.0
 */
typedef uint32_t PalShaderStage;

/**
 * @typedef PalSampleCount
 * @brief sample count.
 *
 * All sample count follow the format `PAL_SAMPLE_COUNT_**` for
 * consistency and API use.
 *
 * @since 2.0
 */
typedef uint32_t PalSampleCount;

/**
 * @typedef PalPrimitiveTopology
 * @brief Primitve topology types.
 *
 * All primitve topology types follow the format `PAL_PRIMITIVE_TOPOLOGY_**` for
 * consistency and API use.
 *
 * @since 2.0
 */
typedef uint32_t PalPrimitiveTopology;

/**
 * @typedef PalCullMode
 * @brief Cull modes.
 *
 * All cull modes follow the format `PAL_CULL_MODE_**` for
 * consistency and API use.
 *
 * @since 2.0
 */
typedef uint32_t PalCullMode;

/**
 * @typedef PalFrontFace
 * @brief Front face modes.
 *
 * All front face modes follow the format `PAL_FRONT_FACE_**` for
 * consistency and API use.
 *
 * @since 2.0
 */
typedef uint32_t PalFrontFace;

/**
 * @typedef PalPolygonMode
 * @brief Polygon modes.
 *
 * All polygon modes follow the format `PAL_POLYGON_MODE_**` for
 * consistency and API use.
 *
 * @since 2.0
 */
typedef uint32_t PalPolygonMode;

/**
 * @typedef PalStencilFaceFlags
 * @brief Stencil face flags. Multiple stencil face flags can be OR'ed together using bitwise
 * OR operator (`|`).
 *
 * All tencil face flags follow the format `PAL_STENCIL_FACE_FLAG_**` for
 * consistency and API use.
 *
 * @since 2.0
 */
typedef uint32_t PalStencilFaceFlags;

/**
 * @typedef PalVertexType
 * @brief Vertex attribute types.
 *
 * All vertex attribute types follow the format `PAL_VERTEX_TYPE_**` for
 * consistency and API use.
 *
 * @since 2.0
 */
typedef uint32_t PalVertexType;

/**
 * @typedef PalVertexSemanticID
 * @brief Vertex semantic id types.
 *
 * All vertex semantic id types follow the format `PAL_VERTEX_SEMANTIC_ID_**` for
 * consistency and API use.
 *
 * @since 2.0
 */
typedef uint32_t PalVertexSemanticID;

/**
 * @typedef PalCommandBufferType
 * @brief Command buffer types.
 *
 * All command buffer types follow the format `PAL_COMMAND_BUFFER_TYPE_**` for
 * consistency and API use.
 *
 * @since 2.0
 */
typedef uint32_t PalCommandBufferType;

/**
 * @typedef PalVertexLayoutType
 * @brief Vertex layout types.
 *
 * All vertex layout types follow the format `PAL_VERTEX_LAYOUT_TYPE_**` for
 * consistency and API use.
 *
 * @since 2.0
 */
typedef uint32_t PalVertexLayoutType;

/**
 * @typedef PalCompareOp
 * @brief Compare operation modes.
 *
 * All compare operation modes follow the format `PAL_COMPARE_OP_**` for
 * consistency and API use.
 *
 * @since 2.0
 */
typedef uint32_t PalCompareOp;

/**
 * @typedef PalStencilOp
 * @brief Stencil operation modes.
 *
 * All stencil operation modes follow the format `PAL_STENCIL_OP_**` for
 * consistency and API use.
 *
 * @since 2.0
 */
typedef uint32_t PalStencilOp;

/**
 * @typedef PalBlendOp
 * @brief Blend operation modes.
 *
 * All blend operation modes follow the format `PAL_BLEND_OP_**` for
 * consistency and API use.
 *
 * @since 2.0
 */
typedef uint32_t PalBlendOp;

/**
 * @typedef PalBlendFactor
 * @brief Blend factor modes.
 *
 * All blend factor modes follow the format `PAL_BLEND_FACTOR_**` for
 * consistency and API use.
 *
 * @since 2.0
 */
typedef uint32_t PalBlendFactor;

/**
 * @typedef PalColorMask
 * @brief Color mask flags. Multiple color mask flags can be OR'ed together using bitwise
 * OR operator (`|`).
 *
 * `PAL_COLOR_MASK_NONE` is not a bit and must not be combined with other bits.
 *
 * All color mask flags follow the format `PAL_COLOR_MASK_**` for
 * consistency and API use.
 *
 * @since 2.0
 */
typedef uint32_t PalColorMask;

/**
 * @typedef PalResolveMode
 * @brief Resolve modes.
 *
 * All resolve modes follow the format `PAL_RESOLVE_MODE_**` for
 * consistency and API use.
 *
 * @since 2.0
 */
typedef uint32_t PalResolveMode;

/**
 * @typedef PalFragmentShadingRate
 * @brief Fragment shading rates.
 *
 * All fragment shading rates follow the format `PAL_FRAGMENT_SHADING_RATE_**` for
 * consistency and API use.
 *
 * @since 2.0
 */
typedef uint32_t PalFragmentShadingRate;

/**
 * @typedef PalFragmentShadingRateCombinerOp
 * @brief Fragment shading rate combiner operaton modes.
 *
 * All fragment shading rate combiner operation modes follow the format
 * `PAL_FRAGMENT_SHADING_RATE_COMBINER_OP_**` for consistency and API use.
 *
 * @since 2.0
 */
typedef uint32_t PalFragmentShadingRateCombinerOp;

/**
 * @typedef PalAccelerationStructureType
 * @brief Acceleration structure types.
 *
 * All acceleration structure types follow the format `PAL_ACCELERATION_STRUCTURE_TYPE_**`
 * for consistency and API use.
 *
 * @since 2.0
 */
typedef uint32_t PalAccelerationStructureType;

/**
 * @typedef PalAccelerationStructureCreateFlags
 * @brief Acceleration structure create flags. Multiple hints can be OR'ed together using
 * bitwise OR operator (`|`).
 *
 * All acceleration structure create flags follow the format
 * `PAL_ACCELERATION_STRUCTURE_CREATE_FLAG_**` for consistency and API use.
 *
 * @since 2.0
 */
typedef uint32_t PalAccelerationStructureCreateFlags;

/**
 * @typedef PalAccelerationStructureBuildMode
 * @brief Acceleration structure build modes.
 *
 * All acceleration structure build modes follow the format
 * `PAL_ACCELERATION_STRUCTURE_BUILD_MODE_**` for consistency and API use.
 *
 * @since 2.0
 */
typedef uint32_t PalAccelerationStructureBuildMode;

/**
 * @typedef PalAccelerationStructureBuildHints
 * @brief Acceleration structure build hints. Multiple hints can be OR'ed together using
 * bitwise OR operator (`|`). Hints can be ignored by the driver.
 *
 * All acceleration structure build hints follow the format
 * `PAL_ACCELERATION_STRUCTURE_BUILD_HINT_**` for consistency and API use.
 *
 * @since 2.0
 */
typedef uint32_t PalAccelerationStructureBuildHints;

/**
 * @typedef PalAccelerationStructureInstanceFlags
 * @brief Acceleration structure instance flags. Multiple flags can be OR'ed together using
 * bitwise OR operator (`|`).
 *
 * All acceleration structure instance flags follow the format
 * `PAL_ACCELERATION_STRUCTURE_INSTANCE_FLAG_**` for consistency and API use.
 *
 * @since 2.0
 */
typedef uint32_t PalAccelerationStructureInstanceFlags;

/**
 * @typedef PalGeometryType
 * @brief Geometry types.
 *
 * All geometry types follow the format `PAL_GEOMETRY_TYPE_**` for consistency and API use.
 *
 * @since 2.0
 */
typedef uint32_t PalGeometryType;

/**
 * @typedef PalGeometryFlags
 * @brief Geometry flags. Multiple flags can be OR'ed together using
 * bitwise OR operator (`|`). Not all combinations are valid.
 *
 * All geometry flags follow the format `PAL_GEOMETRY_FLAG_**` for consistency and API use.
 *
 * @since 2.0
 */
typedef uint32_t PalGeometryFlags;

/**
 * @typedef PalIndexType
 * @brief Index types.
 *
 * All index types follow the format `PAL_INDEX_TYPE_**` for consistency and API use.
 *
 * @since 2.0
 */
typedef uint32_t PalIndexType;

/**
 * @typedef PalBufferUsages
 * @brief Buffer usages. Multiple buffer usages can be OR'ed together using bitwise
 * OR operator (`|`).
 *
 * All buffer usages follow the format `PAL_BUFFER_USAGE_**` for consistency and API use.
 *
 * @since 2.0
 */
typedef uint32_t PalBufferUsages;

/**
 * @typedef PalUsageState
 * @brief Usage states.
 *
 * All usage states follow the format `PAL_USAGE_STATE_**` for consistency and API use.
 *
 * @since 2.0
 */
typedef uint32_t PalUsageState;

/**
 * @typedef PalDescriptorType
 * @brief Descriptor types.
 *
 * All descriptor types follow the format `PAL_DESCRIPTOR_TYPE_**` for consistency and API use.
 *
 * @since 2.0
 */
typedef uint32_t PalDescriptorType;

/**
 * @typedef PalRayTracingShaderGroupType
 * @brief Ray tracing shader group types.
 *
 * All ray tracing shader group types follow the format `PAL_RAY_TRACING_SHADER_GROUP_TYPE_**`
 * for consistency and API use.
 *
 * @since 2.0
 */
typedef uint32_t PalRayTracingShaderGroupType;

/**
 * @typedef PalDescriptorIndexingFlags
 * @brief Descriptor indexing subfeature flags.
 * 
 * These flags show the capabilities of the descriptor indexing feature. Each flag determines 
 * the operations that are allowed.
 * 
 * `PAL_DESCRIPTOR_INDEXING_FLAG_UPDATE_AFTER_BIND`: Descriptors in a descriptor set can be updated 
 * after the descriptor set been bound in a command buffer.
 * 
 * `PAL_DESCRIPTOR_INDEXING_FLAG_PARTIALLY_BOUND`: Unused descriptors can be left uninitialized if
 * a shader never accesses them.
 * 
 * `PAL_DESCRIPTOR_INDEXING_FLAG_NON_UNIFORM_INDEXING`: Different threads can access different 
 * descriptors.
 *
 * All descriptor indexing flags follow the format `PAL_DESCRIPTOR_INDEXING_FLAG_**`
 * for consistency and API use.
 *
 * @since 2.0
 */
typedef uint32_t PalDescriptorIndexingFlags;

/**
 * @typedef PalBufferMemoryUsage
 * @brief Buffer memory usages.
 * 
 * `PAL_BUFFER_MEMORY_USAGE_MANUAL`: PAL does not allocate memory for the buffer. Users are required
 * to get the required size and allocate memory for the buffer after the buffer has been created. 
 * The lifetime of the memory is the responsibility of the user.
 * 
 * `PAL_BUFFER_MEMORY_USAGE_AUTO_GPU_ONLY`: PAL allocates gpu only memory and manages the memory
 * for the user. This is ideal if a custom allocator will not be used by the user.
 * 
 * `PAL_BUFFER_MEMORY_USAGE_AUTO_CPU_UPLOAD`: PAL allocates cpu upload memory and manages the 
 * memory for the user. This is ideal if a custom allocator will not be used by the user.
 * 
 * `PAL_BUFFER_MEMORY_USAGE_AUTO_CPU_READBACK`: PAL allocates cpu readback memory and manages the 
 * memory for the user. This is ideal if a custom allocator will not be used by the user.
 *
 * All buffer memory usages follow the format `PAL_BUFFER_MEMORY_USAGE_**`
 * for consistency and API use.
 *
 * @since 2.0
 */
typedef uint32_t PalBufferMemoryUsage;

/**
 * @typedef PalImageMemoryUsage
 * @brief Image memory usages.
 * 
 * `PAL_IMAGE_MEMORY_USAGE_MANUAL`: PAL does not allocate memory for the image. Users are required
 * to get the required size and allocate memory for the image after the image has been created. 
 * The lifetime of the memory is the responsibility of the user.
 * 
 * `PAL_IMAGE_MEMORY_USAGE_AUTO_GPU_ONLY`: PAL allocates gpu only memory and manages the memory
 * for the user. This is ideal if a custom allocator will not be used by the user.
 *
 * All image memory usages follow the format `PAL_IMAGE_MEMORY_USAGE_**`
 * for consistency and API use.
 *
 * @since 2.0
 */
typedef uint32_t PalImageMemoryUsage;

/**
 * @typedef PalRenderingFlags
 * @brief Rendering flags.
 * 
 * All rendering flags follow the format `PAL_RENDERING_FLAG_**`
 * for consistency and API use.
 *
 * @since 2.0
 */
typedef uint32_t PalRenderingFlags;

/**
 * @typedef PalGraphicsBackendVtableVersion
 * @brief Graphics backend vtable versions.
 *
 * All graphics backend vtable versions follow the format `PAL_GRAPHICS_BACKEND_VTABLE_VERSION_**`
 * for consistency and API use.
 *
 * @since 2.0
 */
typedef uint64_t PalGraphicsBackendVtableVersion;

/**
 * @typedef PalDebugCallback
 * @brief Function pointer type used for debug callbacks.
 *
 * @param userData Optional pointer to user data passed from ::PalGraphicsDebugger. Can be `nullptr`.
 * @param severity Severity of the message. (`PAL_DEBUG_MESSAGE_SEVERITY_INFO`,
 * `PAL_DEBUG_MESSAGE_SEVERITY_WARNING` and `PAL_DEBUG_MESSAGE_SEVERITY_ERROR`).
 * @param type Type of the message. (`PAL_DEBUG_MESSAGE_TYPE_GENERAL`,
 * `PAL_DEBUG_MESSAGE_TYPE_VALIDATION` and `PAL_DEBUG_MESSAGE_TYPE_PERFORMANCE`).
 * @param msg Null-terminated UTF-8 debug message.
 *
 * @since 2.0
 * @sa palInitGraphics
 */
typedef void(PAL_CALL* PalDebugCallback)(
    void* userData,
    PalDebugMessageSeverity severity,
    PalDebugMessageType type,
    const char* msg);

/**
 * @struct PalAdapterInfo
 * @brief Information about an adapter (GPU).
 *
 * @since 2.0
 */
typedef struct {
    uint64_t vram;                                   /**< Total video memory in bytes*/
    uint64_t sharedMemory;                           /**< Total shared memory in bytes*/
    uint32_t vendorId;                               /**< Adapter vendor id.*/
    uint32_t deviceId;                               /**< Adapter device id.*/
    uint32_t driverVersion;                          /**< Adapter version.*/
    PalShaderFormats shaderFormats;                  /**< (eg. `PAL_SHADER_FORMAT_SPIRV`)*/
    PalAdapterType type;                             /**< (eg. `PAL_ADAPTER_TYPE_DISCRETE`).*/
    PalAdapterApiType apiType;                       /**< (eg. `PAL_ADAPTER_API_TYPE_VULKAN`).*/
    char name[PAL_ADAPTER_NAME_SIZE];                /**< Adapter name.*/
    char backendName[PAL_ADAPTER_BACKEND_NAME_SIZE]; /**< Adapter backend name.*/
} PalAdapterInfo;

/**
 * @struct PalImageCapabilities
 * @brief Image capabilities of an adapter (GPU).
 *
 * @since 2.0
 */
typedef struct {
    uint32_t maxWidth;       /**< Max width in pixels.*/
    uint32_t maxHeight;      /**< Max height in pixels.*/
    uint32_t maxDepth;       /**< Max depth in pixels.*/
    uint32_t maxArrayLayers; /**< Max array layers.*/
    uint32_t maxMipLevels;   /**< Max mipmap levels.*/
} PalImageCapabilities;

/**
 * @struct PalResourceCapabilities
 * @brief Resource capabilities of an adapter (GPU).
 *
 * @since 2.0
 */
typedef struct {
    uint32_t maxPerStageSampledImages;         /**< Max sampled images per shader stage .*/
    uint32_t maxPerSetSampledImages;           /**< Max sampled images per descriptor set.*/
    uint32_t maxPerStageStorageImages;         /**< Max storage images per shader stage.*/
    uint32_t maxPerSetStorageImages;           /**< Max storage images per descriptor set.*/
    uint32_t maxPerStageSamplers;              /**< Max samplers per shader stage.*/
    uint32_t maxPerSetSamplers;                /**< Max samplers per descriptor set.*/
    uint32_t maxPerStageStorageBuffers;        /**< Max storage buffers per shader stage.*/
    uint32_t maxPerSetStorageBuffers;          /**< Max storage buffers per descriptor set.*/
    uint32_t maxPerStageUniformBuffers;        /**< Max uniform buffers per shader stage.*/
    uint32_t maxPerSetUniformBuffers;          /**< Max uniform buffers per descriptor set.*/
    uint32_t maxPerStageAccelerationStructure; /**< Max acceleration structures per shader stage.*/
    uint32_t maxPerSetAccelerationStructure; /**< Max acceleration structures per descriptor set.*/
    uint32_t maxBoundSets;                   /**< Max bound descriptor sets.*/
} PalResourceCapabilities;

/**
 * @struct PalComputeCapabilities
 * @brief Compute capabilities of an adapter (GPU).
 *
 * @since 2.0
 */
typedef struct {
    uint32_t maxWorkGroupInvocations; /**< Max invocations across all workgroups.*/
    uint32_t maxWorkGroupCount[3];    /**< Max workgroups per dimension.*/
    uint32_t maxWorkGroupSize[3];     /**< Max workgroup size per dimension.*/
} PalComputeCapabilities;

/**
 * @struct PalViewportCapabilities
 * @brief Viewport capabilities of an adapter (GPU).
 *
 * @since 2.0
 */
typedef struct {
    uint32_t maxWidth;    /**< Max width in pixels.*/
    uint32_t maxHeight;   /**< Max height in pixels.*/
    float minBoundsRange; /**< Min coordinate range.*/
    float maxBoundsRange; /**< Max coordinate range.*/
} PalViewportCapabilities;

/**
 * @struct PalAdapterCapabilities
 * @brief Capabilities of an adapter (GPU).
 *
 * @since 2.0
 */
typedef struct {
    uint32_t maxComputeQueues;            /**< Max compute queues that can b created.*/
    uint32_t maxGraphicsQueues;           /**< Max graphics queues that can b created.*/
    uint32_t maxCopyQueues;               /**< Max copy queues that can b created.*/
    uint32_t maxColorAttachments;         /**< Max number of simultaneous color attachments.*/
    uint32_t maxUniformBufferSize;        /**< Max uniform buffer size in bytes.*/
    uint32_t maxStorageBufferSize;        /**< Max storage buffer size in bytes.*/
    uint32_t maxPushConstantSize;         /**< Max push constants size in bytes.*/
    uint32_t maxVertexLayouts;            /**< Max vertex layouts.*/
    uint32_t maxVertexAttributes;         /**< Max vertex attributes across all vertex layouts.*/
    uint32_t maxTessellationPatchPoint;   /**< Max tessellation patch point.*/
    PalViewportCapabilities viewportCaps; /**< Viewport capabilities.*/
    PalImageCapabilities imageCaps;       /**< Image capabilities.*/
    PalResourceCapabilities resourceCaps; /**< resource (descriptors) capabilities.*/
    PalComputeCapabilities computeCaps;   /**< Compute capabilities.*/
} PalAdapterCapabilities;

/**
 * @struct PalSamplerAnisotropyCapabilities
 * @brief Sampler anisotropy capabilities of an adapter (GPU).
 *
 * @since 2.0
 */
typedef struct {
    uint32_t maxAnisotropy; /**< Max texture filtering level.*/
} PalSamplerAnisotropyCapabilities;

/**
 * @struct PalMultiViewCapabilities
 * @brief Multi view capabilities of an adapter (GPU).
 *
 * @since 2.0
 */
typedef struct {
    uint32_t maxViewCount; /**< Max number views of an image.*/
} PalMultiViewCapabilities;

/**
 * @struct PalMultiViewportCapabilities
 * @brief Multi viewport capabilities of an adapter (GPU).
 *
 * @since 2.0
 */
typedef struct {
    uint32_t maxCount; /**< Max number of simultaneous viewports.*/
} PalMultiViewportCapabilities;

/**
 * @struct PalDepthStencilCapabilities
 * @brief Depth stencil capabilities of an adapter (GPU).
 *
 * @since 2.0
 */
typedef struct {
    uint32_t supportedDepthResolveModes;   /**< Masks of supported depth resolve modes.*/
    uint32_t supportedStencilResolveModes; /**< Masks of supported stencil resolve modes.*/

    /** If `PAL_TRUE`, depth and stencil can have seperate resolve modes.*/
    PalBool supportsIndependentResolve;

    /**If `PAL_TRUE`, depth/stencil can be `PAL_RESOLVE_MODE_NONE` while the other is resolved.*/
    PalBool supportsIndependentResolveNone;
} PalDepthStencilCapabilities;

/**
 * @struct PalFragmentShadingRateCapabilities
 * @brief Fragment shading rate capabilities of an adapter (GPU).
 *
 * @since 2.0
 */
typedef struct {
    uint32_t supportedShadingRates; /**< Masks of supported shading rates.*/
    uint32_t supportedCombinerOps;  /**< Masks of supported combiner operations.*/
    uint32_t minTexelWidth;         /**< Min texel width in pixels*/
    uint32_t minTexelHeight;        /**< Min texel height in pixels*/
    uint32_t maxTexelWidth;         /**< Max texel width in pixels*/
    uint32_t maxTexelHeight;        /**< Max texel height in pixels*/
} PalFragmentShadingRateCapabilities;

/**
 * @struct PalMeshShaderCapabilities
 * @brief Mesh shader capabilities of an adapter (GPU).
 *
 * @since 2.0
 */
typedef struct {
    uint32_t maxOutputPrimitives;         /**< Max number of primitives per mesh workgroup.*/
    uint32_t maxOutputVertices;           /**< Max number of vertices per mesh workgroup.*/
    uint32_t maxWorkGroupInvocations;     /**< Max mesh invocations across all workgroups.*/
    uint32_t maxTaskWorkGroupInvocations; /**< Max task invocations across all workgroups.*/
    uint32_t maxWorkGroupCount[3];        /**< Max mesh workgroups per dimension.*/
    uint32_t maxTaskWorkGroupCount[3];    /**< Max task workgroups per dimension.*/
} PalMeshShaderCapabilities;

/**
 * @struct PalRayTracingCapabilities
 * @brief Ray tracing capabilities of an adapter (GPU).
 *
 * @since 2.0
 */
typedef struct {
    uint32_t maxRecursionDepth;      /**< Max number of ray recursion.*/
    uint32_t maxHitAttributeSize;    /**< Max attributes size in bytes.*/
    uint32_t maxInstanceCount;       /**< Max number of instances.*/
    uint32_t maxPrimitiveCount;      /**< Max number of primitives.*/
    uint32_t maxGeometryCount;       /**< Max number of geometries.*/
    uint32_t maxPayloadSize;         /**< Max payload size in bytes.*/
    uint32_t maxDispatchInvocations; /**< Max number of dispatch threads.*/
} PalRayTracingCapabilities;

/**
 * @struct PalDescriptorIndexingCapabilities
 * @brief Descriptor indexing capabilities of an adapter (GPU).
 *
 * @since 2.0
 */
typedef struct {
    PalDescriptorIndexingFlags flags;   /**< Capabilities flags. see `PalDescriptorIndexingFlags`*/
    uint32_t maxPerStageSampledImages;  /**< Max sampled images per shader stage .*/
    uint32_t maxPerSetSampledImages;    /**< Max sampled images per descriptor set.*/
    uint32_t maxPerStageStorageImages;  /**< Max storage images per shader stage.*/
    uint32_t maxPerSetStorageImages;    /**< Max storage images per descriptor set.*/
    uint32_t maxPerStageSamplers;       /**< Max samplers per shader stage.*/
    uint32_t maxPerSetSamplers;         /**< Max samplers per descriptor set.*/
    uint32_t maxPerStageStorageBuffers; /**< Max storage buffers per shader stage.*/
    uint32_t maxPerSetStorageBuffers;   /**< Max storage buffers per descriptor set.*/
    uint32_t maxPerStageUniformBuffers; /**< Max uniform buffers per shader stage.*/
    uint32_t maxPerSetUniformBuffers;   /**< Max uniform buffers per descriptor set.*/
    uint32_t maxPerStageAccelerationStructure; /**< Max acceleration structures per shader stage.*/
    uint32_t maxPerSetAccelerationStructure; /**< Max acceleration structures per descriptor set.*/
} PalDescriptorIndexingCapabilities;

/**
 * @struct PalSurfaceCapabilities
 * @brief surface capabilities of an adapter (GPU).
 *
 * @since 2.0
 */
typedef struct {
    uint32_t supportedPresentModes;    /**< Masks of supported present modes.*/
    uint32_t supportedCompositeAlphas; /**< Masks of supported composite alphas.*/
    uint32_t supportedFormats;         /**< Masks of supported surface formats.*/
    uint32_t minImageCount;            /**< Min image or back buffer count.*/
    uint32_t maxImageCount;            /**< Max image or back buffer count.*/
    uint32_t minImageWidth;            /**< Min image width in pixels.*/
    uint32_t minImageHeight;           /**< Min image height in pixels.*/
    uint32_t maxImageWidth;            /**< Max image width in pixels.*/
    uint32_t maxImageHeight;           /**< Max image height in pixels.*/
    uint32_t maxImageArrayLayers;      /**< Max number of image layers.*/
} PalSurfaceCapabilities;

/**
 * @struct PalFormatInfo
 * @brief Information about a format. This includes the supported image usages and maximum sample
 * count from the provided format.
 *
 * @since 2.0
 */
typedef struct {
    PalImageUsages usages;      /**< (eg. `PAL_IMAGE_USAGE_COLOR`).*/
    PalFormat format;           /**< (eg. `PAL_FORMAT_R8G8B8A8_UNORM`).*/
    PalSampleCount sampleCount; /**< (eg. `PAL_SAMPLE_COUNT_8`).*/
} PalFormatInfo;

/**
 * @struct PalImageInfo
 * @brief Information about an image.
 *
 * @since 2.0
 */
typedef struct {
    PalImageUsages usages;       /**< (eg. `PAL_IMAGE_USAGE_COLOR`).*/
    uint32_t width;              /**< Width of the image in pixels.*/
    uint32_t height;             /**< Height of the image in pixels.*/
    uint32_t depth;              /**< Depth of the image in pixels.*/
    uint32_t arrayLayerCount;    /**< Number of array layers.*/
    uint32_t mipLevelCount;      /**< Number of mipmap levels.*/
    PalSampleCount sampleCount;  /**< (eg. `PAL_SAMPLE_COUNT_8`).*/
    PalImageType type;           /**< (eg. `PAL_IMAGE_TYPE_2D`).*/
    PalFormat format;            /**< (eg. `PAL_FORMAT_R8G8B8A8_UNORM`).*/
    PalBool belongsToSwapchain; /**< If `PAL_TRUE`, the image belongs to a swapchain.*/
} PalImageInfo;

/**
 * @struct PalClearValue
 * @brief Clear values used with rendering.
 *
 * If used with a color attachment, the color values will be used and depth and stencil
 * will be used with depth stencil attachments.
 *
 * @since 2.0
 */
typedef struct {
    float color[4];   /**< Color clear value.*/
    float depth;      /**< Depth clear value.*/
    uint32_t stencil; /**< Stencil clear value.*/
} PalClearValue;

/**
 * @struct PalAttachmentDesc
 * @brief An attachment description.
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * @since 2.0
 */
typedef struct {
    PalImageView* imageView;           /**< Image view. Must not be `nullptr`.*/
    PalImageView* resolveImageView;    /**< Resolve image view. Can be `nullptr`.*/
    PalLoadOp loadOp;                  /**< (eg. `PAL_LOAD_OP_CLEAR`).*/
    PalStoreOp storeOp;                /**< (eg. `PAL_STORE_OP_STORE`).*/
    PalLoadOp stencilLoadOp;           /**< (eg. `PAL_LOAD_OP_DONT_CARE`).*/
    PalStoreOp stencilStoreOp;         /**< (eg. `PAL_STORE_OP_DONT_CARE`).*/
    PalResolveMode resolveMode;        /**< Used if resolveImageView is set.*/
    PalResolveMode stencilResolveMode; /**< Used if resolveImageView is set.*/
    uint32_t texelWidth;               /**< Texel width for fragment shading rate attachment.*/
    uint32_t texelHeight;              /**< Texel height for fragment shading rate attachment.*/
    PalClearValue clearValue;          /**< Clear value for color and depth/stencil attachments.*/
} PalAttachmentDesc;

/**
 * @struct PalViewport
 * @brief A viewport in pixels (float).
 *
 * @since 2.0
 */
typedef struct {
    float x;        /**< X position in pixels.*/
    float y;        /**< Y position in pixels.*/
    float width;    /**< Width in pixels.*/
    float height;   /**< Height in pixels.*/
    float minDepth; /**< Min depth value.*/
    float maxDepth; /**< Max depth value.*/
} PalViewport;

/**
 * @struct PalRect2D
 * @brief A 2D rectangle in pixels.
 *
 * @since 2.0
 */
typedef struct {
    int32_t x;       /**< X position in pixels.*/
    int32_t y;       /**< Y position in pixels.*/
    uint32_t width;  /**< Width in pixels.*/
    uint32_t height; /**< Height in pixels.*/
} PalRect2D;

/**
 * @struct PalMemoryRequirements
 * @brief Memory requirements for a resource (image, buffer etc).
 *
 * @since 2.0
 */
typedef struct {
    uint64_t size;                 /**< Required size in bytes.*/
    uint64_t alignment;            /**< Required alignment in bytes.*/
    uint32_t supportedMemoryTypes; /**< Masks of supported memory types.*/
    uint32_t memoryMask;           /**< Memory masks used in allocations. Must not be changed.*/
} PalMemoryRequirements;

/**
 * @struct PalCommandBufferSubmitInfo
 * @brief Submit information of a command buffer.
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * @since 2.0
 */
typedef struct {
    uint64_t waitValue;            /**< Timeline semaphore value to wait on.*/
    uint64_t signalValue;          /**< Timeline semaphore value to signal.*/
    PalCommandBuffer* cmdBuffer;   /**< Command buffer to submit.*/
    PalSemaphore* waitSemaphore;   /**< Wait semaphore.*/
    PalSemaphore* signalSemaphore; /**< Signal semaphore.*/
    PalFence* fence;               /**< Fence to signal.*/
} PalCommandBufferSubmitInfo;

/**
 * @struct PalSwapchainNextImageInfo
 * @brief Next image information of a swapchain.
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * @since 2.0
 */
typedef struct {
    uint64_t timeout;              /**< Timeout in milliseconds.*/
    PalSemaphore* signalSemaphore; /**< Timeline semaphore value to signal.*/
    PalFence* fence;               /**< Fence to signal.*/
} PalSwapchainNextImageInfo;

/**
 * @struct PalRenderingInfo
 * @brief Information about how rendering should be done in graphics pipeline.
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * @since 2.0
 */
typedef struct {
    PalAttachmentDesc* colorAttachments;              /**< Color attachments.*/
    PalAttachmentDesc* depthStencilAttachment;        /**< Depth/Stencil attachment.*/
    PalAttachmentDesc* fragmentShadingRateAttachment; /**< Fragment shading rate attachment.*/
    uint32_t viewCount;                               /**< View count. Set to 1 for default.*/
    uint32_t colorAttachentCount;                     /**< Number of color attachments.*/
} PalRenderingInfo;

/**
 * @struct PalRenderingLayoutInfo
 * @brief Information about a pre-existing PalRenderingInfo.
 * This is used to reference an already existing PalRenderingInfo.
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * @since 2.0
 */
typedef struct {
    PalFormat* colorAttachmentsFormat;             /**< Color attachments formats.*/
    uint32_t colorAttachentCount;                  /**< Number of color attachment formats.*/
    uint32_t viewCount;                            /**< View count. Set to 1 for default.*/
    PalSampleCount sampleCount;                    /**< (eg. `PAL_SAMPLE_COUNT_4`).*/
    PalRenderingFlags flags;                       /**< (eg. `PAL_RENDERING_FLAG_NONE`).*/
    PalFormat depthStencilAttachmentFormat;        /**< Depth/Stencil attachment format.*/
    PalFormat fragmentShadingRateAttachmentFormat; /**< Fragment shading rate attachment format.*/
} PalRenderingLayoutInfo;

/**
 * @struct PalWorkGroupBuildData
 * @brief Compute or Mesh(or Task) workgroup input data build helper.
 *
 * Uninitialized fields may result in undefined behavior. `workCount` can be specified in pixels,
 * vertices etc.Eg. an image of 800 x 600 will be [0] = 800, [1] = 600 and [2] = 1.
 *
 * @since 2.0
 */
typedef struct {
    uint32_t workCount[3];      /**< Workload per dimension. (eg. Image (200, 200, 1)).*/
    uint32_t workGroupSize[3];  /**< Threads per workgroup per axis of the adapter (GPU).*/
    uint32_t workGroupCount[3]; /**< Workgroups per axis of the adapter (GPU).*/
} PalWorkGroupBuildData;

/**
 * @struct PalWorkGroupInfo
 * @brief Information about compute or mesh(or task) dispatch data or a dispatch tile.
 *
 * @since 2.0
 */
typedef struct {
    uint32_t workGroupBase[3];  /**< Offset per dimension of a dispatch tile.*/
    uint32_t workGroupCount[3]; /**< Workgroup count per dimension of a dispatch tile.*/
} PalWorkGroupInfo;

/**
 * @struct PalDrawIndirectData
 * @brief Draw indirect data of a single draw call.
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * @since 2.0
 */
typedef struct {
    uint32_t vertexCount;   /**< Vertex count.*/
    uint32_t instanceCount; /**< Instance count.*/
    uint32_t firstVertex;   /**< First vertex.*/
    uint32_t firstInstance; /**< First instance.*/
} PalDrawIndirectData;

/**
 * @struct PalDrawIndexedIndirectData
 * @brief Draw indexed indirect data of a single draw call.
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * @since 2.0
 */
typedef struct {
    uint32_t indexCount;    /**< Index count.*/
    uint32_t instanceCount; /**< Instance count.*/
    uint32_t firstIndex;    /**< First index.*/
    int32_t vertexOffset;   /**< Vertex offset.*/
    uint32_t firstInstance; /**< First instance.*/
} PalDrawIndexedIndirectData;

/**
 * @struct PalDispatchIndirectData
 * @brief Draw or dispatch indirect data of a single dispatch call.
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * @since 2.0
 */
typedef struct {
    uint32_t groupCountXOrWidth;  /**< Number of groups on the x dimension or dispatch width.*/
    uint32_t groupCountXOrHeight; /**< Number of groups on the y dimension or dispatch height.*/
    uint32_t groupCountXOrDepth;  /**< Number of groups on the z dimension or dispatch depth.*/
} PalDispatchIndirectData;

/**
 * @struct PalVertexAttribute
 * @brief Vertex attribute.
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * @since 2.0
 */
typedef struct {
    PalVertexSemanticID semanticID; /**< (eg. `PAL_VERTEX_SEMANTIC_ID_POSITION`).*/
    PalVertexType type;             /**< (eg. `PAL_VERTEX_TYPE_FLOAT3`).*/
} PalVertexAttribute;

/**
 * @struct PalVertexLayout
 * @brief Vertex layout.
 *
 * This defines the layout, ordering and the number of vertex attributes the layout uses.
 *
 * The layouts should reflect the exact layout of the shaders. Eg.
 * attributes[2] = { position, color } is different from
 * attributes[2] = { color, position }. The ordering must be correct.
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * @since 2.0
 */
typedef struct {
    PalVertexAttribute* attributes; /**< Vertex attributes.*/
    uint64_t attributeCount;        /**< Number of vertex attributes.*/
    PalVertexLayoutType type;       /**< (eg. `PAL_VERTEX_LAYOUT_TYPE_PER_VERTEX`).*/
    uint32_t binding;               /**< Vertex buffer binding slot.*/
} PalVertexLayout;

/**
 * @struct PalGraphicsDebugger
 * @brief Graphics debugger.
 *
 * The debugger will not be initialized if PalGraphicsDebugger::callback is set and valid.
 *
 * @since 2.0
 */
typedef struct {
    void* userData;              /**< Optional user provided data. Can be `nullptr`.*/
    PalDebugCallback callback;   /**< Debug callback function.*/
    PalBool denyGeneral;         /**< Do not recieve general messages.*/
    PalBool denyValidation;      /**< Do not recieve validation messages.*/
    PalBool denyPerformance;     /**< Do not recieve performance messages.*/
    PalBool denyInfoSeverity;    /**< Do not recieve info severity messages.*/
    PalBool denyWarningSeverity; /**< Do not recieve warning severity messages.*/
    PalBool denyErrorSeverity;   /**< Do not recieve error severity messages.*/
} PalGraphicsDebugger;

/**
 * @struct PalRasterizerState
 * @brief Rasterizer state. This is used with a graphics pipeline.
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * @since 2.0
 */
typedef struct {
    PalBool enableDepthClamp;   /**< `PAL_TRUE` to enable depth clamp.*/
    PalBool enableDepthBias;    /**< `PAL_TRUE` to enable depth bias.*/
    float depthBiasConstant;    /**< Depth bias constant.*/
    float depthBiasSlope;       /**< Depth bias slope.*/
    float depthBiasClamp;       /**< Depth bias clamp.*/
    PalPolygonMode polygonMode; /**< (eg. `PAL_POLYGON_MODE_FILL`).*/
    PalCullMode cullMode;       /**< (eg. `PAL_CULL_MODE_NONE`).*/
    PalFrontFace frontFace;     /**< (eg. `PAL_FRONT_FACE_CLOCKWISE`).*/
} PalRasterizerState;

/**
 * @struct PalMultisampleState
 * @brief Multisample state. This is used with a graphics pipeline.
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * @since 2.0
 */
typedef struct {
    uint64_t sampleMask;           /**< Set to `nullptr` to use default.*/
    PalBool enableSampleShading;   /**< `PAL_TRUE` to enable sample shading.*/
    PalBool enableAlphaToCoverage; /**< `PAL_TRUE` to enable alpha to coverage.*/
    PalSampleCount sampleCount;    /**< (eg. `PAL_SAMPLE_COUNT_4`).*/
    float minSampleShading;        /**< Minimum sample shading.*/
} PalMultisampleState;

/**
 * @struct PalStencilOpState
 * @brief Stencil operation state. This is used with a graphics pipeline.
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * @since 2.0
 */
typedef struct {
    PalStencilOp failOp;      /**< Stencil fail operation.*/
    PalStencilOp passOp;      /**< Pass operation.*/
    PalStencilOp depthFailOp; /**< Depth fail operation.*/
    PalCompareOp compareOp;   /**< Compare operation.*/
} PalStencilOpState;

/**
 * @struct PalDepthStencilState
 * @brief Depth stencil state. This is used with a graphics pipeline.
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * @since 2.0
 */
typedef struct {
    PalBool enableDepthTest;               /**< `PAL_TRUE` to enable depth test.*/
    PalBool enableDepthWrite;              /**< `PAL_TRUE` to enable depth write.*/
    PalBool enableStencilTest;             /**< `PAL_TRUE` to enable stencil test.*/
    PalCompareOp compareOp;                /**< Compare operation.*/
    PalStencilOpState frontStencilOpState; /**< Front stencil operation state.*/
    PalStencilOpState backStencilOpState;  /**< Back stencil operation state.*/
} PalDepthStencilState;

/**
 * @struct PalColorBlendAttachment
 * @brief Color blend attachmeent. This is used with a graphics pipeline.
 *
 * Every rendering attachment (color, etc) must have a color blend attachment to
 * describe how blending is applied to the attachment.
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * @since 2.0
 */
typedef struct {
    PalColorMask colorWriteMask;        /**< (eg. `PAL_COLOR_MASK_RED` | `PAL_COLOR_MASK_RED`).*/
    PalBool enableBlend;                /**< `PAL_TRUE` to enable blending.*/
    PalBlendFactor srcColorBlendFactor; /**< (eg. `PAL_BLEND_FACTOR_SRC_ALPHA`).*/
    PalBlendFactor dstColorBlendFactor; /**< (eg. `PAL_BLEND_FACTOR_ONE_MINUS_DST_ALPHA`).*/
    PalBlendOp colorBlendOp;            /**< (eg. `PAL_BLEND_OP_ADD`).*/
    PalBlendFactor srcAlphaBlendFactor; /**< (eg. `PAL_BLEND_FACTOR_SRC_COLOR`).*/
    PalBlendFactor dstAlphaBlendFactor; /**< (eg. `PAL_BLEND_FACTOR_ONE_MINUS_DST_COLOR`).*/
    PalBlendOp alphaBlendOp;            /**< (eg. `PAL_BLEND_OP_SUBTRACT`).*/
} PalColorBlendAttachment;

/**
 * @struct PalFragmentShadingRateState
 * @brief Fragment shading rate state. This is used with a graphics pipeline.
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * @since 2.0
 */
typedef struct {
    PalFragmentShadingRate rate;                     /**< (eg. `PAL_FRAGMENT_SHADING_RATE_2X2`).*/
    PalFragmentShadingRateCombinerOp combinerOps[2]; /**< Combiner operations.*/
} PalFragmentShadingRateState;

/**
 * @struct PalAccelerationStructureInstance
 * @brief Acceleration structure instance base data.
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * @since 2.0
 */
typedef struct {
    PalAccelerationStructure* blas;              /**< Bottom level acceleration structure.*/
    PalAccelerationStructureInstanceFlags flags; /**< Instance flags.*/
    uint32_t mask;           /**< Only the lower 8-bits are used (0x00 - 0xFF).*/
    uint32_t instanceId;     /**< User defined identifier.*/
    uint32_t hitGroupOffset; /**< Offset added to hitgroup index in the Shader Binding Table.*/
    float transform[12];     /**< Transform (row major 3x4).*/
} PalAccelerationStructureInstance;

/**
 * @struct PalAccelerationStructureBuildSize
 * @brief Acceleration structure build size.
 *
 * @since 2.0
 */
typedef struct {
    uint64_t accelerationStructureSize; /**< Required acceleration structure size in bytes.*/
    uint64_t scratchBufferSize;         /**< Required scratch buffer size in bytes.*/
    uint64_t updateScratchBufferSize;   /**< Required scratch buffer update size in bytes.*/
} PalAccelerationStructureBuildSize;

/**
 * @struct PalGeometryDataTriangle
 * @brief Acceleration structure triangle geometry data.
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * @since 2.0
 */
typedef struct {
    PalDeviceAddress vertexBufferAddress;    /**< Address of the vertex buffer.*/
    PalDeviceAddress indexBufferAddress;     /**< Address of the index buffer.*/
    PalDeviceAddress transformBufferAddress; /**< Address of the transform buffer.*/
    PalVertexType vertexType;                /**< (eg. `PAL_VERTEX_TYPE_FLOAT3`)*/
    PalIndexType indexType;                  /**< (eg. `PAL_INDEX_TYPE_UINT32`).*/
    uint32_t vertexCount;                    /**< Number of vertices.*/
    uint32_t vertexStride;                   /**< Size of each vertex in bytes.*/
} PalGeometryDataTriangle;

/**
 * @struct PalGeometryDataAABBS
 * @brief Acceleration structure AABBS geometry data.
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * @since 2.0
 */
typedef struct {
    PalDeviceAddress bufferAddress; /**< Address of the AABBS buffer.*/
    uint64_t stride;                /**< Size of each AABBS in bytes.*/
} PalGeometryDataAABBS;

/**
 * @struct PalGeometry
 * @brief Acceleration structure geometry.
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * @since 2.0
 */
typedef struct {
    const void* data;        /**< This will be casted based on `type`.*/
    uint64_t primitiveCount; /**< Number of primitives in `data`.*/
    PalGeometryFlags flags;  /**< (eg. `PAL_GEOMETRY_FLAG_OPAQUE`).*/
    PalGeometryType type;    /**< (eg. `PAL_GEOMETRY_TYPE_TRIANGLE`).*/
} PalGeometry;

/**
 * @struct PalAccelerationStructureBuildInfo
 * @brief Build information of an acceleration structure.
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * @since 2.0
 */
typedef struct {
    PalAccelerationStructure* dst;          /**< Destination aceleration structure.*/
    PalAccelerationStructure* src;          /**< Source aceleration structure. Used for updates.*/
    PalGeometry* geometries;                /**< BLAS geometries. `nullptr` for TLAS*/
    PalDeviceAddress scratchBufferAddress;  /**< Address of scratch buffer.*/
    PalDeviceAddress instanceBufferAddress; /**< Address of instance buffer. `nullptr` for BLAS.*/
    PalAccelerationStructureBuildHints buildHints; /**< Might be ignored by driver.*/
    PalAccelerationStructureType type; /**< (eg. `PAL_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL`).*/
    PalAccelerationStructureBuildMode buildMode; /**< Build or update.*/
    uint32_t count; /**< Number of elements in `geometries` or `instanceBufferAddress`.*/
} PalAccelerationStructureBuildInfo;

/**
 * @struct PalDescriptorSetLayoutBinding
 * @brief Single descriptor set layout binding.
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * @since 2.0
 */
typedef struct {
    uint32_t descriptorCount;         /**< Number of descriptors of `descriptorType`.*/
    PalDescriptorType descriptorType; /**< (eg. `PAL_DESCRIPTOR_TYPE_SAMPLED_IMAGE`).*/
} PalDescriptorSetLayoutBinding;

/**
 * @struct PalDescriptorPoolBindingSize
 * @brief Descriptor pool binding size.
 * Describes the sizes of each descriptor type in the descriptor pool.
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * @since 2.0
 */
typedef struct {
    uint32_t bindingCount;            /**< Number of bindings of `descriptorType`.*/
    PalDescriptorType descriptorType; /**< (eg. `PAL_DESCRIPTOR_TYPE_SAMPLED_IMAGE`).*/
} PalDescriptorPoolBindingSize;

/**
 * @struct PalDescriptorBufferInfo
 * @brief Information about a buffer descriptor.
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * @since 2.0
 */
typedef struct {
    PalBuffer* buffer; /**< Buffer associated with the descriptor.*/
    uint64_t offset;   /**< Offset in bytes. If structured, this will be divided by `stride`.*/
    uint64_t size;     /**< Size of the buffer in bytes.*/
    uint64_t stride;   /**< For structured buffers. This will be ignored if not supported.*/
} PalDescriptorBufferInfo;

/**
 * @struct PalDescriptorImageViewInfo
 * @brief Information about an image view descriptor.
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * @since 2.0
 */
typedef struct {
    PalImageView* imageView; /**< Image view associated with the descriptor.*/
} PalDescriptorImageViewInfo;

/**
 * @struct PalDescriptorSamplerInfo
 * @brief Information about a sampler descriptor.
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * @since 2.0
 */
typedef struct {
    PalSampler* sampler; /**< Sampler associated with the descriptor.*/
} PalDescriptorSamplerInfo;

/**
 * @struct PalDescriptorTLASInfo
 * @brief Information about a TLAS descriptor.
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * @since 2.0
 */
typedef struct {
    PalAccelerationStructure* tlas; /**< TLAS associated with the descriptor.*/
} PalDescriptorTLASInfo;

/**
 * @struct PalDescriptorSetWriteInfo
 * @brief Write information of a descriptor set.
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * @since 2.0
 */
typedef struct {
    PalDescriptorSet* descriptorSet;            /**< Descriptor set to write into.*/
    PalDescriptorBufferInfo* bufferInfos;       /**< Used with buffer descriptors.*/
    PalDescriptorImageViewInfo* imageViewInfos; /**< Used with image descriptors.*/
    PalDescriptorSamplerInfo* samplerInfos;     /**< Used with sampler descriptors.*/
    PalDescriptorTLASInfo* tlasInfos;           /**< Used with TLAS descriptors.*/
    PalDescriptorType descriptorType;           /**< (eg. `PAL_DESCRIPTOR_TYPE_SAMPLED_IMAGE`).*/
    uint32_t layoutBindingIndex; /**< Index into the descriptor set layout bindings.*/
    uint32_t arrayElement;       /**< First index within the descriptor set layout bindings.*/
    uint32_t descriptorCount;    /**< Number of descriptors to write.*/
} PalDescriptorSetWriteInfo;

/**
 * @struct PalPushConstantInfo
 * @brief Push constant range.
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * @since 2.0
 */
typedef struct {
    uint32_t offset; /**< Offset in bytes.*/
    uint32_t size;   /**< Size in bytes.*/
} PalPushConstantInfo;

/**
 * @struct PalImageSubresourceRange
 * @brief Subresource range for images and image views.
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * @since 2.0
 */
typedef struct {
    PalImageAspect aspect;    /**< (eg. `PAL_IMAGE_ASPECT_COLOR`).*/
    uint32_t startMipLevel;   /**< Start mipmap level. 0 for default.*/
    uint32_t mipLevelCount;   /**< Number of mipmap levels.*/
    uint32_t startArrayLayer; /**< Start array layer. 0 for default.*/
    uint32_t layerArrayCount; /**< Number of array layers.*/
} PalImageSubresourceRange;

/**
 * @struct PalBufferCopyInfo
 * @brief Information for buffer to buffer copies.
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * @since 2.0
 */
typedef struct {
    uint64_t size;      /**< Size in bytes to copy from source buffer.*/
    uint64_t dstOffset; /**< Offset in bytes in destination buffer.*/
    uint64_t srcOffset; /**< Offset in bytes in source buffer.*/
} PalBufferCopyInfo;

/**
 * @struct PalBufferImageCopyInfo
 * @brief Information for image to buffer and vice versa copies.
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * @since 2.0
 */
typedef struct {
    uint64_t bufferOffset;         /**< Offset in bytes into the buffer.*/
    PalImageAspect imageAspect;    /**< (eg. `PAL_IMAGE_ASPECT_COLOR`).*/
    uint32_t bufferRowLength;      /**< Buffer row length in texels.*/
    uint32_t bufferImageHeight;    /**< Buffer image height in texels.*/
    uint32_t ImageMipLevel;        /**< Mipmap level of the image.*/
    uint32_t ImageStartArrayLayer; /**< Starting array layer of the image.*/
    uint32_t ImageArrayLayerCount; /**< Number of array layers of the image.*/
    int32_t imageOffsetX;          /**< X image offset in bytes.*/
    int32_t imageOffsetY;          /**< Y image offset in bytes.*/
    int32_t imageOffsetZ;          /**< Z image offset in bytes.*/
    uint32_t imageWidth;           /**< Image width in bytes.*/
    uint32_t imageHeight;          /**< Image height in bytes.*/
    uint32_t imageDepth;           /**< Image depth in bytes.*/
} PalBufferImageCopyInfo;

/**
 * @struct PalImageCopyInfo
 * @brief Information for image to image copies.
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * @since 2.0
 */
typedef struct {
    PalImageAspect aspect;       /**< (eg. `PAL_IMAGE_ASPECT_COLOR`).*/
    uint32_t dstMipLevel;        /**< Mipmap level of destination image.*/
    uint32_t srcMipLevel;        /**< Mipmap level of source image.*/
    uint32_t dstStartArrayLayer; /**< Starting array layer of destination image.*/
    uint32_t srcStartArrayLayer; /**< Starting array layer of source image.*/
    uint32_t arrayLayerCount;    /**< Number of array layers of destination and source images.*/
    int32_t dstOffsetX;          /**< X destination image offset in bytes.*/
    int32_t srcOffsetX;          /**< X source image offset in bytes.*/
    int32_t dstOffsetY;          /**< Y destination image offset in bytes.*/
    int32_t srcOffsetY;          /**< Y source image offset in bytes.*/
    int32_t dstOffsetZ;          /**< Z destination image offset in bytes.*/
    int32_t srcOffsetZ;          /**< Z source image offset in bytes.*/
    uint32_t width;              /**< Width of the region to copy from source image.*/
    uint32_t height;             /**< Height of the region to copy from source image.*/
    uint32_t depth;              /**< Depth of the region to copy from source image.*/
} PalImageCopyInfo;

/**
 * @struct PalShaderBindingTableRecordInfo
 * @brief Information for image to image copies.
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * The records array must be in this order [raygen][miss][hitgroup][callable].
 *
 * @since 2.0
 */
typedef struct {
    void* localData;        /**< Must not be `nullptr` if `localDataSize` is not 0.*/
    uint32_t groupIndex;    /**< Index into the shader groups used to create the pipeline.*/
    uint32_t localDataSize; /**< Must not be greater than the data size of the group.*/
} PalShaderBindingTableRecordInfo;

/**
 * @struct PalShaderEntryInfo
 * @brief Entry information of a shader.
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * @since 2.0
 */
typedef struct {
    const char* entryName;       /**< Must not be `nullptr`.*/
    PalShaderStage stage;        /**< (eg. `PAL_SHADER_STAGE_VERTEX`).*/
    uint32_t patchControlPoints; /**< For tessellation shaders. Will be ignored by other stages.*/
} PalShaderEntryInfo;

/**
 * @struct PalImageCreateInfo
 * @brief Creation parameters for an image.
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * @since 2.0
 */
typedef struct {
    PalImageUsages usages;    /**< (eg. `PAL_IMAGE_USAGE_COLOR` | `PAL_IMAGE_USAGE_TRANSFER_DST`).*/
    uint32_t width;           /**< Width in pixels.*/
    uint32_t height;          /**< Height in pixels.*/
    uint32_t depth;           /**< Depth in pixels.*/
    uint32_t arrayLayerCount; /**< Number of array layers.*/
    uint32_t mipLevelCount;   /**< Number of mipmap levels.*/
    PalSampleCount sampleCount;      /**< (eg. `PAL_SAMPLE_COUNT_1`).*/
    PalImageType type;               /**< (eg. `PAL_IMAGE_TYPE_2D`).*/
    PalFormat format;                /**< (eg. `PAL_FORMAT_B8G8R8A8_UNORM`).*/
    PalImageMemoryUsage memoryUsage; /**< See `PalImageMemoryUsage`.*/
} PalImageCreateInfo;

/**
 * @struct PalImageCreateInfo
 * @brief Creation parameters for an image view.
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * @since 2.0
 */
typedef struct {
    PalFormat format;                          /**< (eg. `PAL_FORMAT_B8G8R8A8_UNORM`).*/
    PalImageViewType type;                     /**< (eg. `PAL_IMAGE_VIEW_TYPE_2D`).*/
    PalImageSubresourceRange subresourceRange; /**< Range of the image to create the view with.*/
} PalImageViewCreateInfo;

/**
 * @struct PalSamplerCreateInfo
 * @brief Creation parameters for a sampler.
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * @since 2.0
 */
typedef struct {
    PalBool enableCompare;              /**< `PAL_TRUE` to enable compare operations.*/
    PalBool enableAnisotropy;           /**< `PAL_TRUE` to enable texture filtering.*/
    float mipLodBias;                   /**< Mipmap level bias.*/
    float minLod;                       /**< Minimum Mipmap level allowed.*/
    float maxLod;                       /**< Maximum Mipmap level allowed.*/
    float maxAnisotropy;                /**< Texture filtering level.*/
    PalFilterMode minFilterMode;        /**< (eg. `PAL_FILTER_MODE_LINEAR`).*/
    PalFilterMode magFilterMode;        /**< (eg. `PAL_FILTER_MODE_LINEAR`).*/
    PalSamplerMipmapMode mipmapMode;    /**< (eg. `PAL_SAMPLER_MIPMAP_MODE_LINEAR`).*/
    PalSamplerAddressMode addressModeU; /**< (eg. `PAL_SAMPLER_ADDRESS_MODE_REPEAT`).*/
    PalSamplerAddressMode addressModeV; /**< (eg. `PAL_SAMPLER_ADDRESS_MODE_REPEAT`).*/
    PalSamplerAddressMode addressModeW; /**< (eg. `PAL_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE`).*/
    PalCompareOp compareOp;             /**< (eg. `PAL_COMPARE_OP_GREATER`).*/
    PalBorderColor borderColor;         /**< (eg. `PAL_BORDER_COLOR_FLOAT_OPAQUE_BLACK`).*/
} PalSamplerCreateInfo;

/**
 * @struct PalSwapchainCreateInfo
 * @brief Creation parameters for a swapchain.
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * @since 2.0
 */
typedef struct {
    PalBool clipped;                  /**< `PAL_TRUE` to discard pixels that are not visible.*/
    uint32_t width;                   /**< Width in pixels.*/
    uint32_t height;                  /**< Height in pixels.*/
    uint32_t imageCount;              /**< Number of images or back buffers.*/
    uint32_t imageArrayLayerCount;    /**< Number of array layers.*/
    PalPresentMode presentMode;       /**< (eg. `PAL_PRESENT_MODE_FIFO`).*/
    PalCompositeAplha compositeAlpha; /**< (eg. `PAL_COMPOSITE_ALPHA_OPAQUE`).*/
    PalSurfaceFormat format;          /**< (eg. `PAL_SURFACE_FORMAT_BGRA8_UNORM_SRGB_NONLINEAR`).*/
} PalSwapchainCreateInfo;

/**
 * @struct PalShaderCreateInfo
 * @brief Creation parameters for a shader.
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * @since 2.0
 */
typedef struct {
    void* bytecode;              /**< Pointer to the shader bytecode.*/
    PalShaderEntryInfo* entries; /**< Shader entries.*/
    uint32_t bytecodeSize;       /**< Size of `bytecode` in bytes.*/
    uint32_t entryCount;         /**< Number of shader entries.*/
} PalShaderCreateInfo;

/**
 * @struct PalBufferCreateInfo
 * @brief Creation parameters for a buffer.
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * @since 2.0
 */
typedef struct {
    uint64_t size;                    /**< Size in bytes.*/
    PalBufferUsages usages;           /**< (eg. `PAL_BUFFER_USAGE_VERTEX`).*/
    PalBufferMemoryUsage memoryUsage; /**< See `PalBufferMemoryUsage`.*/
} PalBufferCreateInfo;

/**
 * @struct PalAccelerationStructureCreateInfo
 * @brief Creation parameters for an acceleration structure.
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * @since 2.0
 */
typedef struct {
    PalBuffer* buffer;                 /**< Acceleration structure buffer.*/
    uint64_t offset;                   /**< Size in bytes.*/
    uint64_t size;                     /**< Offset in bytes.*/
    PalAccelerationStructureType type; /**< (eg. `PAL_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL`).*/
    PalAccelerationStructureCreateFlags createFlags; /**< Set to 0 for default.*/
} PalAccelerationStructureCreateInfo;

/**
 * @struct PalDescriptorSetLayoutCreateInfo
 * @brief Creation parameters for a descriptor set layout.
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * @since 2.0
 */
typedef struct {
    PalDescriptorSetLayoutBinding* bindings;            /**< Bindings.*/
    PalDescriptorIndexingFlags flags; /**< See `PalDescriptorIndexingFlags`.*/
    uint32_t bindingCount;                              /**< Number of bindings.*/
} PalDescriptorSetLayoutCreateInfo;

/**
 * @struct PalDescriptorPoolCreateInfo
 * @brief Creation parameters for a descriptor pool.
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * @since 2.0
 */
typedef struct {
    PalDescriptorPoolBindingSize* bindingSizes; /**< Binding sizes.*/
    uint64_t bindingSizeCount;                  /**< Number of bindings sizes.*/
    uint32_t maxDescriptorSets; /**< Maximum number of descriptor sets that can be allocated.*/
    PalDescriptorIndexingFlags flags; /**< See `PalDescriptorIndexingFlags`.*/
} PalDescriptorPoolCreateInfo;

/**
 * @struct PalPipelineLayoutCreateInfo
 * @brief Creation parameters for a pipeline layout.
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * @since 2.0
 */
typedef struct {
    PalDescriptorSetLayout** descriptorSetLayouts; /**< Descriptor set layouts.*/
    PalPushConstantInfo pushConstantInfo;          /**< Push constant info.*/
    uint32_t descriptorSetLayoutCount;             /**< Number of descriptor set layouts.*/
    PalBool usePushConstant;                       /**< `PAL_TRUE` to use push constant.*/
} PalPipelineLayoutCreateInfo;

/**
 * @struct PalGraphicsPipelineCreateInfo
 * @brief Creation parameters for a graphics pipeline.
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * @since 2.0
 */
typedef struct {
    PalPipelineLayout* pipelineLayout;                     /**< Pipeline layout.*/
    PalShader** shaders;                                   /**< Shaders.*/
    PalVertexLayout* vertexLayouts;                        /**< Vertex layouts.*/
    PalColorBlendAttachment* colorBlendAttachments;        /**< Color blend attachments.*/
    PalRasterizerState* rasterizerState;                   /**< Rasterizer state.*/
    PalMultisampleState* multisampleState;                 /**< Multisample state.*/
    PalDepthStencilState* depthStencilState;               /**< Depth stencil state.*/
    PalFragmentShadingRateState* fragmentShadingRateState; /**< Fragment shading rate state.*/
    PalRenderingLayoutInfo* renderingLayout;               /**< Rendering layout.*/
    PalBool primitiveRestartEnable; /**< `PAL_TRUE` to enable primitive restart for indexed draw.*/
    uint32_t vertexLayoutCount;     /**< Number of vertex layouts.*/
    uint32_t colorBlendAttachmentCount; /**< Number of color attachments.*/
    uint32_t shaderCount;               /**< Number of shaders.*/
    PalIndexType indexType; /**< Will be used if `primitiveRestartEnable` is `PAL_TRUE`.*/
    PalPrimitiveTopology topology;
} PalGraphicsPipelineCreateInfo;

/**
 * @struct PalComputePipelineCreateInfo
 * @brief Creation parameters for a compute pipeline.
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * @since 2.0
 */
typedef struct {
    PalPipelineLayout* pipelineLayout; /**< Pipeline layout.*/
    PalShader* computeShader;          /**< Compute shader.*/
} PalComputePipelineCreateInfo;

/**
 * @struct PalRayTracingShaderGroupCreateInfo
 * @brief Creation parameters for a ray tracing pipeline shader group.
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * The shader group array must be in this order [raygen][miss][hitgroup][callable].
 *
 * @since 2.0
 */
typedef struct {
    PalRayTracingShaderGroupType type;     /**< (eg. `PAL_RAY_TRACING_SHADER_GROUP_TYPE_GENERAL`).*/
    uint32_t anyHitShaderIndex;            /**< Index of the anyhit shader.*/
    uint32_t anyHitShaderEntryIndex;       /**< Index of the anyhit shader entry.*/
    uint32_t closestHitShaderIndex;        /**< Index of the closest hit shader.*/
    uint32_t closestHitShaderEntryIndex;   /**< Index of the closest hit shader entry.*/
    uint32_t generalShaderIndex;           /**< Index of the general shader.*/
    uint32_t generalShaderEntryIndex;      /**< Index of the general shader entry.*/
    uint32_t intersectionShaderIndex;      /**< Index of the intersection shader.*/
    uint32_t intersectionShaderEntryIndex; /**< Index of the intersection shader entry.*/
    uint32_t maxDataSize; /**< Size of extra data associated with the shader group.*/
} PalRayTracingShaderGroupCreateInfo;

/**
 * @struct PalRayTracingPipelineCreateInfo
 * @brief Creation parameters for a ray tracing pipeline.
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * @since 2.0
 */
typedef struct {
    PalPipelineLayout* pipelineLayout;                /**< Pipeline layout.*/
    PalRayTracingShaderGroupCreateInfo* shaderGroups; /**< Shader groups.*/
    PalShader** shaders;                              /**< Shaders.*/
    uint64_t shaderGroupCount;                        /**< Number of shader groups.*/
    uint32_t shaderCount;                             /**< Number of shaders.*/
    uint32_t maxRecursionDepth;                       /**< Max number of ray recursion.*/
    uint32_t maxAttributeSize;                        /**< Max attributes size in bytes.*/
    uint32_t maxPayloadSize;                          /**< Max payload size in bytes.*/
} PalRayTracingPipelineCreateInfo;

/**
 * @struct PalShaderBindingTableCreateInfo
 * @brief Creation parameters for a shader binding table.
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * @since 2.0
 */
typedef struct {
    PalShaderBindingTableRecordInfo* records; /**< Shader binding table records.*/
    PalPipeline* rayTracingPipeline;          /**< Ray tracing pipeline.*/
    uint64_t recordCount;                     /**< Number of shader binding table records.*/
} PalShaderBindingTableCreateInfo;

/**
 * @struct PalGraphicsBackendRegistrationInfo
 * @brief Registration info of a graphics backend.
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * @since 2.0
 */
typedef struct {
    const void* vtable;                      /**< Pointer to the backend vtable.*/
    PalGraphicsBackendVtableVersion version; /**< (eg. `PAL_GRAPHICS_BACKEND_VTABLE_VERSION_1`).*/
} PalGraphicsBackendInfo;

/**
 * @struct PalGraphicsBackendVtable1
 * @brief Version 1 dispatch table for PAL graphics system backends.
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * All backend handle implementation (eg. struct CustomBuffer) must reserve its first field as
 * a `void*` and set it to `PAL_BACKEND_KEY` at the handles creation function pointer
 * (eg. createBufferCustom). This will be validated at the handle creation function
 * (eg. palCreateBuffer).
 *
 * Pal trust thee backend author to not overwrite or use the reserve space. It is used by the
 * graphics layer.
 *
 * @since 2.0
 */
typedef struct {
    /**
     * Backend implementation of ::palEnumerateAdapters.
     *
     * Must obey the rules and semantics documented in palEnumerateAdapters().
     */
    PalResult(PAL_CALL* enumerateAdapters)(
        int32_t* count,
        PalAdapter** outAdapters);

    /**
     * Backend implementation of ::palGetAdapterInfo.
     *
     * Must obey the rules and semantics documented in palGetAdapterInfo().
     */
    PalResult(PAL_CALL* getAdapterInfo)(
        PalAdapter* adapter,
        PalAdapterInfo* info);

    /**
     * Backend implementation of ::palGetAdapterCapabilities.
     *
     * Must obey the rules and semantics documented in palGetAdapterCapabilities().
     */
    PalResult(PAL_CALL* getAdapterCapabilities)(
        PalAdapter* adapter,
        PalAdapterCapabilities* caps);

    /**
     * Backend implementation of ::palGetAdapterFeatures.
     *
     * Must obey the rules and semantics documented in palGetAdapterFeatures().
     */
    PalAdapterFeatures(PAL_CALL* getAdapterFeatures)(PalAdapter* adapter);

    /**
     * Backend implementation of ::palGetHighestSupportedShaderTarget.
     *
     * Must obey the rules and semantics documented in palGetHighestSupportedShaderTarget().
     */
    uint32_t(PAL_CALL* getHighestSupportedShaderTarget)(
        PalAdapter* adapter,
        PalShaderFormats shaderFormat);

    /**
     * Backend implementation of ::palCreateDevice.
     *
     * Must obey the rules and semantics documented in palCreateDevice().
     */
    PalResult(PAL_CALL* createDevice)(
        PalAdapter* adapter,
        PalAdapterFeatures features,
        PalDevice** outDevice);

    /**
     * Backend implementation of ::palDestroyDevice.
     *
     * Must obey the rules and semantics documented in palDestroyDevice().
     */
    void(PAL_CALL* destroyDevice)(PalDevice* device);

    /**
     * Backend implementation of ::palAllocateMemory.
     *
     * Must obey the rules and semantics documented in palAllocateMemory().
     */
    PalResult(PAL_CALL* allocateMemory)(
        PalDevice* device,
        PalMemoryType type,
        uint64_t memoryMask,
        uint64_t size,
        PalMemory** outMemory);

    /**
     * Backend implementation of ::palFreeMemory.
     *
     * Must obey the rules and semantics documented in palFreeMemory().
     */
    void(PAL_CALL* freeMemory)(
        PalDevice* device,
        PalMemory* memory);

    /**
     * Backend implementation of ::palQuerySamplerAnisotropyCapabilities.
     *
     * Must obey the rules and semantics documented in
     * palQuerySamplerAnisotropyCapabilities().
     */
    PalResult(PAL_CALL* querySamplerAnisotropyCapabilities)(
        PalDevice* device,
        PalSamplerAnisotropyCapabilities* caps);

    /**
     * Backend implementation of ::palQueryMultiViewCapabilities.
     *
     * Must obey the rules and semantics documented in
     * palQueryMultiViewCapabilities().
     */
    PalResult(PAL_CALL* queryMultiViewCapabilities)(
        PalDevice* device,
        PalMultiViewCapabilities* caps);

    /**
     * Backend implementation of ::palQueryMultiViewportCapabilities.
     *
     * Must obey the rules and semantics documented in
     * palQueryMultiViewportCapabilities().
     */
    PalResult(PAL_CALL* queryMultiViewportCapabilities)(
        PalDevice* device,
        PalMultiViewportCapabilities* caps);

    /**
     * Backend implementation of ::palQueryDepthStencilCapabilities.
     *
     * Must obey the rules and semantics documented in
     * palQueryDepthStencilCapabilities().
     */
    PalResult(PAL_CALL* queryDepthStencilCapabilities)(
        PalDevice* device,
        PalDepthStencilCapabilities* caps);

    /**
     * Backend implementation of ::palQueryFragmentShadingRateCapabilities.
     *
     * Must obey the rules and semantics documented in
     * palQueryFragmentShadingRateCapabilities().
     */
    PalResult(PAL_CALL* queryFragmentShadingRateCapabilities)(
        PalDevice* device,
        PalFragmentShadingRateCapabilities* caps);

    /**
     * Backend implementation of ::palQueryMeshShaderCapabilities.
     *
     * Must obey the rules and semantics documented in
     * palQueryMeshShaderCapabilities().
     */
    PalResult(PAL_CALL* queryMeshShaderCapabilities)(
        PalDevice* device,
        PalMeshShaderCapabilities* caps);

    /**
     * Backend implementation of ::palQueryRayTracingCapabilities.
     *
     * Must obey the rules and semantics documented in
     * palQueryRayTracingCapabilities().
     */
    PalResult(PAL_CALL* queryRayTracingCapabilities)(
        PalDevice* device,
        PalRayTracingCapabilities* caps);

    /**
     * Backend implementation of ::palQueryDescriptorIndexingCapabilities.
     *
     * Must obey the rules and semantics documented in
     * palQueryDescriptorIndexingCapabilities().
     */
    PalResult(PAL_CALL* queryDescriptorIndexingCapabilities)(
        PalDevice* device,
        PalDescriptorIndexingCapabilities* caps);

    /**
     * Backend implementation of ::palCreateQueue.
     *
     * Must obey the rules and semantics documented in palCreateQueue().
     */
    PalResult(PAL_CALL* createQueue)(
        PalDevice* device,
        PalQueueType type,
        PalQueue** outQueue);

    /**
     * Backend implementation of ::palDestroyQueue.
     *
     * Must obey the rules and semantics documented in palDestroyQueue().
     */
    void(PAL_CALL* destroyQueue)(PalQueue* queue);

    /**
     * Backend implementation of ::palCanQueuePresent.
     *
     * Must obey the rules and semantics documented in palCanQueuePresent().
     */
    PalBool(PAL_CALL* canQueuePresent)(
        PalQueue* queue,
        PalSurface* surface);

    /**
     * Backend implementation of ::palWaitQueue.
     *
     * Must obey the rules and semantics documented in palWaitQueue().
     */
    PalResult(PAL_CALL* waitQueue)(PalQueue* queue);

    /**
     * Backend implementation of ::palEnumerateFormats.
     *
     * Must obey the rules and semantics documented in palEnumerateFormats().
     */
    PalResult(PAL_CALL* enumerateFormats)(
        PalAdapter* adapter,
        int32_t* count,
        PalFormatInfo* outFormats);

    /**
     * Backend implementation of ::palIsFormatSupported.
     *
     * Must obey the rules and semantics documented in palIsFormatSupported().
     */
    PalBool(PAL_CALL* isFormatSupported)(
        PalAdapter* adapter,
        PalFormat format);

    /**
     * Backend implementation of ::palQueryFormatImageUsages.
     *
     * Must obey the rules and semantics documented in palQueryFormatImageUsages().
     */
    PalImageUsages(PAL_CALL* queryFormatImageUsages)(
        PalAdapter* adapter,
        PalFormat format);

    /**
     * Backend implementation of ::palQueryFormatSampleCount.
     *
     * Must obey the rules and semantics documented in palQueryFormatSampleCount().
     */
    PalSampleCount(PAL_CALL* queryFormatSampleCount)(
        PalAdapter* adapter,
        PalFormat format);

    /**
     * Backend implementation of ::palCreateImage.
     *
     * Must obey the rules and semantics documented in palCreateImage().
     */
    PalResult(PAL_CALL* createImage)(
        PalDevice* device,
        const PalImageCreateInfo* info,
        PalImage** outImage);

    /**
     * Backend implementation of ::palDestroyImage.
     *
     * Must obey the rules and semantics documented in palDestroyImage().
     */
    void(PAL_CALL* destroyImage)(PalImage* image);

    /**
     * Backend implementation of ::palGetImageInfo.
     *
     * Must obey the rules and semantics documented in palGetImageInfo().
     */
    PalResult(PAL_CALL* getImageInfo)(
        PalImage* image,
        PalImageInfo* info);

    /**
     * Backend implementation of ::palGetImageMemoryRequirements.
     *
     * Must obey the rules and semantics documented in palGetImageMemoryRequirements().
     */
    PalResult(PAL_CALL* getImageMemoryRequirements)(
        PalImage* image,
        PalMemoryRequirements* requirements);

    /**
     * Backend implementation of ::palBindImageMemory.
     *
     * Must obey the rules and semantics documented in palBindImageMemory().
     */
    PalResult(PAL_CALL* bindImageMemory)(
        PalImage* image,
        PalMemory* memory,
        uint64_t offset);

    /**
     * Backend implementation of ::palCreateImageView.
     *
     * Must obey the rules and semantics documented in palCreateImageView().
     */
    PalResult(PAL_CALL* createImageView)(
        PalDevice* device,
        PalImage* image,
        const PalImageViewCreateInfo* info,
        PalImageView** outImageView);

    /**
     * Backend implementation of ::palDestroyImageView.
     *
     * Must obey the rules and semantics documented in palDestroyImageView().
     */
    void(PAL_CALL* destroyImageView)(PalImageView* imageView);

    /**
     * Backend implementation of ::palCreateSampler.
     *
     * Must obey the rules and semantics documented in palCreateSampler().
     */
    PalResult(PAL_CALL* createSampler)(
        PalDevice* device,
        const PalSamplerCreateInfo* info,
        PalSampler** outSampler);

    /**
     * Backend implementation of ::palDestroySampler.
     *
     * Must obey the rules and semantics documented in palDestroySampler().
     */
    void(PAL_CALL* destroySampler)(PalSampler* sampler);

    /**
     * Backend implementation of ::palCreateSurface.
     *
     * Must obey the rules and semantics documented in palCreateSurface().
     */
    PalResult(PAL_CALL* createSurface)(
        PalDevice* device,
        void* window,
        void* windowInstance,
        PalWindowInstanceType instanceType,
        PalSurface** outSurface);

    /**
     * Backend implementation of ::palDestroySurface.
     *
     * Must obey the rules and semantics documented in palDestroySurface().
     */
    void(PAL_CALL* destroySurface)(PalSurface* surface);

    /**
     * Backend implementation of ::palGetSurfaceCapabilities.
     *
     * Must obey the rules and semantics documented in palGetSurfaceCapabilities().
     */
    PalResult(PAL_CALL* getSurfaceCapabilities)(
        PalDevice* device,
        PalSurface* surface,
        PalSurfaceCapabilities* caps);

    /**
     * Backend implementation of ::palCreateSwapchain.
     *
     * Must obey the rules and semantics documented in palCreateSwapchain().
     */
    PalResult(PAL_CALL* createSwapchain)(
        PalDevice* device,
        PalQueue* queue,
        PalSurface* surface,
        const PalSwapchainCreateInfo* info,
        PalSwapchain** outSwapchain);

    /**
     * Backend implementation of ::palDestroySwapchain.
     *
     * Must obey the rules and semantics documented in palDestroySwapchain().
     */
    void(PAL_CALL* destroySwapchain)(PalSwapchain* swapchain);

    /**
     * Backend implementation of ::palGetSwapchainImage.
     *
     * Must obey the rules and semantics documented in palGetSwapchainImage().
     */
    PalImage*(PAL_CALL* getSwapchainImage)(
        PalSwapchain* swapchain,
        uint32_t index);

    /**
     * Backend implementation of ::palGetNextSwapchainImage.
     *
     * Must obey the rules and semantics documented in palGetNextSwapchainImage().
     */
    PalResult(PAL_CALL* getNextSwapchainImage)(
        PalSwapchain* swapchain,
        PalSwapchainNextImageInfo* info,
        uint32_t* outIndex);

    /**
     * Backend implementation of ::palPresentSwapchain.
     *
     * Must obey the rules and semantics documented in palPresentSwapchain().
     */
    PalResult(PAL_CALL* presentSwapchain)(
        PalSwapchain* swapchain,
        uint32_t imageIndex,
        PalSemaphore* waitSemaphore);

    /**
     * Backend implementation of ::palResizeSwapchain.
     *
     * Must obey the rules and semantics documented in palResizeSwapchain().
     */
    PalResult(PAL_CALL* resizeSwapchain)(
        PalSwapchain* swapchain,
        uint32_t newWidth,
        uint32_t newHeight);

    /**
     * Backend implementation of ::palCreateShader.
     *
     * Must obey the rules and semantics documented in palCreateShader().
     */
    PalResult(PAL_CALL* createShader)(
        PalDevice* device,
        const PalShaderCreateInfo* info,
        PalShader** outShader);

    /**
     * Backend implementation of ::palDestroyShader.
     *
     * Must obey the rules and semantics documented in palDestroyShader().
     */
    void(PAL_CALL* destroyShader)(PalShader* shader);

    /**
     * Backend implementation of ::palCreateFence.
     *
     * Must obey the rules and semantics documented in palCreateFence().
     */
    PalResult(PAL_CALL* createFence)(
        PalDevice* device,
        PalBool signaled,
        PalFence** outFence);

    /**
     * Backend implementation of ::palDestroyFence.
     *
     * Must obey the rules and semantics documented in palDestroyFence().
     */
    void(PAL_CALL* destroyFence)(PalFence* fence);

    /**
     * Backend implementation of ::palWaitFence.
     *
     * Must obey the rules and semantics documented in palWaitFence().
     */
    PalResult(PAL_CALL* waitFence)(
        PalFence* fence,
        uint64_t timeout);

    /**
     * Backend implementation of ::palResetFence.
     *
     * Must obey the rules and semantics documented in palResetFence().
     */
    PalResult(PAL_CALL* resetFence)(PalFence* fence);

    /**
     * Backend implementation of ::palIsFenceSignaled.
     *
     * Must obey the rules and semantics documented in palIsFenceSignaled().
     */
    PalBool(PAL_CALL* isFenceSignaled)(PalFence* fence);

    /**
     * Backend implementation of ::palCreateSemaphore.
     *
     * Must obey the rules and semantics documented in palCreateSemaphore().
     */
    PalResult(PAL_CALL* createSemaphore)(
        PalDevice* device,
        PalBool enableTimeline,
        PalSemaphore** outSemaphore);

    /**
     * Backend implementation of ::palDestroySemaphore.
     *
     * Must obey the rules and semantics documented in palDestroySemaphore().
     */
    void(PAL_CALL* destroySemaphore)(PalSemaphore* semaphore);

    /**
     * Backend implementation of ::palWaitSemaphore.
     *
     * Must obey the rules and semantics documented in palWaitSemaphore().
     */
    PalResult(PAL_CALL* waitSemaphore)(
        PalSemaphore* semaphore,
        uint64_t value,
        uint64_t timeout);

    /**
     * Backend implementation of ::palSignalSemaphore.
     *
     * Must obey the rules and semantics documented in palSignalSemaphore().
     */
    PalResult(PAL_CALL* signalSemaphore)(
        PalSemaphore* semaphore,
        PalQueue* queue,
        uint64_t value);

    /**
     * Backend implementation of ::palGetSemaphoreValue.
     *
     * Must obey the rules and semantics documented in palGetSemaphoreValue().
     */
    PalResult(PAL_CALL* getSemaphoreValue)(
        PalSemaphore* semaphore,
        uint64_t* outValue);

    /**
     * Backend implementation of ::palCreateCommandPool.
     *
     * Must obey the rules and semantics documented in palCreateCommandPool().
     */
    PalResult(PAL_CALL* createCommandPool)(
        PalDevice* device,
        PalQueue* queue,
        PalCommandPool** outPool);

    /**
     * Backend implementation of ::palDestroyCommandPool.
     *
     * Must obey the rules and semantics documented in palDestroyCommandPool().
     */
    void(PAL_CALL* destroyCommandPool)(PalCommandPool* pool);

    /**
     * Backend implementation of ::palResetCommandPool.
     *
     * Must obey the rules and semantics documented in palResetCommandPool().
     */
    PalResult(PAL_CALL* resetCommandPool)(PalCommandPool* pool);

    /**
     * Backend implementation of ::palAllocateCommandBuffer.
     *
     * Must obey the rules and semantics documented in palAllocateCommandBuffer().
     */
    PalResult(PAL_CALL* allocateCommandBuffer)(
        PalDevice* device,
        PalCommandPool* pool,
        PalCommandBufferType type,
        PalCommandBuffer** outCmdBuffer);

    /**
     * Backend implementation of ::palFreeCommandBuffer.
     *
     * Must obey the rules and semantics documented in palFreeCommandBuffer().
     */
    void(PAL_CALL* freeCommandBuffer)(PalCommandBuffer* cmdBuffer);

    /**
     * Backend implementation of ::palResetCommandBuffer.
     *
     * Must obey the rules and semantics documented in palResetCommandBuffer().
     */
    PalResult(PAL_CALL* resetCommandBuffer)(PalCommandBuffer* cmdBuffer);

    /**
     * Backend implementation of ::palSubmitCommandBuffer.
     *
     * Must obey the rules and semantics documented in palSubmitCommandBuffer().
     */
    PalResult(PAL_CALL* submitCommandBuffer)(
        PalQueue* queue,
        PalCommandBufferSubmitInfo* info);

    /**
     * Backend implementation of ::palCmdBegin.
     *
     * Must obey the rules and semantics documented in palCmdBegin().
     */
    PalResult(PAL_CALL* cmdBegin)(
        PalCommandBuffer* cmdBuffer,
        PalRenderingLayoutInfo* info);

    /**
     * Backend implementation of ::palCmdEnd.
     *
     * Must obey the rules and semantics documented in palCmdEnd().
     */
    PalResult(PAL_CALL* cmdEnd)(PalCommandBuffer* cmdBuffer);

    /**
     * Backend implementation of ::palCmdExecuteCommandBuffer.
     *
     * Must obey the rules and semantics documented in palCmdExecuteCommandBuffer().
     */
    PalResult(PAL_CALL* cmdExecuteCommandBuffer)(
        PalCommandBuffer* primaryCmdBuffer,
        PalCommandBuffer* secondaryCmdBuffer);

    /**
     * Backend implementation of ::palCmdSetFragmentShadingRate.
     *
     * Must obey the rules and semantics documented in palCmdSetFragmentShadingRate().
     */
    PalResult(PAL_CALL* cmdSetFragmentShadingRate)(
        PalCommandBuffer* cmdBuffer,
        PalFragmentShadingRateState* state);

    /**
     * Backend implementation of ::palCmdDrawMeshTasks.
     *
     * Must obey the rules and semantics documented in palCmdDrawMeshTasks().
     */
    PalResult(PAL_CALL* cmdDrawMeshTasks)(
        PalCommandBuffer* cmdBuffer,
        uint32_t groupCountX,
        uint32_t groupCountY,
        uint32_t groupCountZ);

    /**
     * Backend implementation of ::palCmdDrawMeshTasksIndirect.
     *
     * Must obey the rules and semantics documented in palCmdDrawMeshTasksIndirect().
     */
    PalResult(PAL_CALL* cmdDrawMeshTasksIndirect)(
        PalCommandBuffer* cmdBuffer,
        PalBuffer* buffer,
        uint32_t drawCount);

    /**
     * Backend implementation of ::palCmdDrawMeshTasksIndirectCount.
     *
     * Must obey the rules and semantics documented in palCmdDrawMeshTasksIndirectCount().
     */
    PalResult(PAL_CALL* cmdDrawMeshTasksIndirectCount)(
        PalCommandBuffer* cmdBuffer,
        PalBuffer* buffer,
        PalBuffer* countBuffer,
        uint32_t maxDrawCount);

    /**
     * Backend implementation of ::palCmdBuildAccelerationStructure.
     *
     * Must obey the rules and semantics documented in palCmdBuildAccelerationStructure().
     */
    PalResult(PAL_CALL* cmdBuildAccelerationStructure)(
        PalCommandBuffer* cmdBuffer,
        PalAccelerationStructureBuildInfo* info);

    /**
     * Backend implementation of ::palCmdBeginRendering.
     *
     * Must obey the rules and semantics documented in palCmdBeginRendering().
     */
    PalResult(PAL_CALL* cmdBeginRendering)(
        PalCommandBuffer* cmdBuffer,
        PalRenderingInfo* info);

    /**
     * Backend implementation of ::palCmdEndRendering.
     *
     * Must obey the rules and semantics documented in palCmdEndRendering().
     */
    PalResult(PAL_CALL* cmdEndRendering)(PalCommandBuffer* cmdBuffer);

    /**
     * Backend implementation of ::palCmdCopyBuffer.
     *
     * Must obey the rules and semantics documented in palCmdCopyBuffer().
     */
    PalResult(PAL_CALL* cmdCopyBuffer)(
        PalCommandBuffer* cmdBuffer,
        PalBuffer* dst,
        PalBuffer* src,
        PalBufferCopyInfo* copyInfo);

    /**
     * Backend implementation of ::palCmdCopyBufferToImage.
     *
     * Must obey the rules and semantics documented in palCmdCopyBufferToImage().
     */
    PalResult(PAL_CALL* cmdCopyBufferToImage)(
        PalCommandBuffer* cmdBuffer,
        PalImage* dstImage,
        PalBuffer* srcBuffer,
        PalBufferImageCopyInfo* copyInfo);

    /**
     * Backend implementation of ::cmdCopyImage.
     *
     * Must obey the rules and semantics documented in cmdCopyImage().
     */
    PalResult(PAL_CALL* cmdCopyImage)(
        PalCommandBuffer* cmdBuffer,
        PalImage* dst,
        PalImage* src,
        PalImageCopyInfo* copyInfo);

    /**
     * Backend implementation of ::palCmdCopyImageToBuffer.
     *
     * Must obey the rules and semantics documented in palCmdCopyImageToBuffer().
     */
    PalResult(PAL_CALL* cmdCopyImageToBuffer)(
        PalCommandBuffer* cmdBuffer,
        PalBuffer* dstBuffer,
        PalImage* srcImage,
        PalBufferImageCopyInfo* copyInfo);

    /**
     * Backend implementation of ::palCmdBindPipeline.
     *
     * Must obey the rules and semantics documented in palCmdBindPipeline().
     */
    PalResult(PAL_CALL* cmdBindPipeline)(
        PalCommandBuffer* cmdBuffer,
        PalPipeline* pipeline);

    /**
     * Backend implementation of ::palCmdSetViewport.
     *
     * Must obey the rules and semantics documented in palCmdSetViewport().
     */
    PalResult(PAL_CALL* cmdSetViewport)(
        PalCommandBuffer* cmdBuffer,
        uint32_t count,
        PalViewport* viewports);

    /**
     * Backend implementation of ::palCmdSetScissors.
     *
     * Must obey the rules and semantics documented in palCmdSetScissors().
     */
    PalResult(PAL_CALL* cmdSetScissors)(
        PalCommandBuffer* cmdBuffer,
        uint32_t count,
        PalRect2D* scissors);

    /**
     * Backend implementation of ::palCmdBindVertexBuffers.
     *
     * Must obey the rules and semantics documented in palCmdBindVertexBuffers().
     */
    PalResult(PAL_CALL* cmdBindVertexBuffers)(
        PalCommandBuffer* cmdBuffer,
        uint32_t firstSlot,
        uint32_t count,
        PalBuffer** buffers,
        uint64_t* offsets);

    /**
     * Backend implementation of ::palCmdBindIndexBuffer.
     *
     * Must obey the rules and semantics documented in palCmdBindIndexBuffer().
     */
    PalResult(PAL_CALL* cmdBindIndexBuffer)(
        PalCommandBuffer* cmdBuffer,
        PalBuffer* buffer,
        uint64_t offset,
        PalIndexType type);

    /**
     * Backend implementation of ::palCmdDraw.
     *
     * Must obey the rules and semantics documented in palCmdDraw().
     */
    PalResult(PAL_CALL* cmdDraw)(
        PalCommandBuffer* cmdBuffer,
        uint32_t vertexCount,
        uint32_t instanceCount,
        uint32_t firstVertex,
        uint32_t firstInstance);

    /**
     * Backend implementation of ::palCmdDrawIndirect.
     *
     * Must obey the rules and semantics documented in palCmdDrawIndirect().
     */
    PalResult(PAL_CALL* cmdDrawIndirect)(
        PalCommandBuffer* cmdBuffer,
        PalBuffer* buffer,
        uint32_t count);

    /**
     * Backend implementation of ::palCmdDrawIndirectCount.
     *
     * Must obey the rules and semantics documented in palCmdDrawIndirectCount().
     */
    PalResult(PAL_CALL* cmdDrawIndirectCount)(
        PalCommandBuffer* cmdBuffer,
        PalBuffer* buffer,
        PalBuffer* countBuffer,
        uint32_t count);

    /**
     * Backend implementation of ::palCmdDrawIndexed.
     *
     * Must obey the rules and semantics documented in palCmdDrawIndexed().
     */
    PalResult(PAL_CALL* cmdDrawIndexed)(
        PalCommandBuffer* cmdBuffer,
        uint32_t indexCount,
        uint32_t instanceCount,
        uint32_t firstIndex,
        int32_t vertexOffset,
        uint32_t firstInstance);

    /**
     * Backend implementation of ::palCmdDrawIndexedIndirect.
     *
     * Must obey the rules and semantics documented in palCmdDrawIndexedIndirect().
     */
    PalResult(PAL_CALL* cmdDrawIndexedIndirect)(
        PalCommandBuffer* cmdBuffer,
        PalBuffer* buffer,
        uint32_t count);

    /**
     * Backend implementation of ::palCmdDrawIndexedIndirectCount.
     *
     * Must obey the rules and semantics documented in palCmdDrawIndexedIndirectCount().
     */
    PalResult(PAL_CALL* cmdDrawIndexedIndirectCount)(
        PalCommandBuffer* cmdBuffer,
        PalBuffer* buffer,
        PalBuffer* countBuffer,
        uint32_t count);

    /**
     * Backend implementation of ::palCmdAccelerationStructureBarrier.
     *
     * Must obey the rules and semantics documented in palCmdAccelerationStructureBarrier().
     */
    PalResult(PAL_CALL* cmdAccelerationStructureBarrier)(
        PalCommandBuffer* cmdBuffer,
        PalAccelerationStructure* as,
        PalUsageState oldUsageState,
        PalUsageState newUsageState);

    /**
     * Backend implementation of ::palCmdImageBarrier.
     *
     * Must obey the rules and semantics documented in palCmdImageBarrier().
     */
    PalResult(PAL_CALL* cmdImageBarrier)(
        PalCommandBuffer* cmdBuffer,
        PalImage* image,
        PalImageSubresourceRange* subresourceRange,
        PalUsageState oldUsageState,
        PalUsageState newUsageState);

    /**
     * Backend implementation of ::palCmdBufferBarrier.
     *
     * Must obey the rules and semantics documented in palCmdBufferBarrier().
     */
    PalResult(PAL_CALL* cmdBufferBarrier)(
        PalCommandBuffer* cmdBuffer,
        PalBuffer* buffer,
        PalUsageState oldUsageState,
        PalUsageState newUsageState);

    /**
     * Backend implementation of ::palCmdDispatch.
     *
     * Must obey the rules and semantics documented in palCmdDispatch().
     */
    PalResult(PAL_CALL* cmdDispatch)(
        PalCommandBuffer* cmdBuffer,
        uint32_t groupCountX,
        uint32_t groupCountY,
        uint32_t groupCountZ);

    /**
     * Backend implementation of ::palCmdDispatchBase.
     *
     * Must obey the rules and semantics documented in palCmdDispatchBase().
     */
    PalResult(PAL_CALL* cmdDispatchBase)(
        PalCommandBuffer* cmdBuffer,
        uint32_t baseGroupX,
        uint32_t baseGroupY,
        uint32_t baseGroupZ,
        uint32_t groupCountX,
        uint32_t groupCountY,
        uint32_t groupCountZ);

    /**
     * Backend implementation of ::palCmdDispatchIndirect.
     *
     * Must obey the rules and semantics documented in palCmdDispatchIndirect().
     */
    PalResult(PAL_CALL* cmdDispatchIndirect)(
        PalCommandBuffer* cmdBuffer,
        PalBuffer* buffer);

    /**
     * Backend implementation of ::palCmdTraceRays.
     *
     * Must obey the rules and semantics documented in palCmdTraceRays().
     */
    PalResult(PAL_CALL* cmdTraceRays)(
        PalCommandBuffer* cmdBuffer,
        PalShaderBindingTable* sbt,
        uint32_t raygenIndex,
        uint32_t width,
        uint32_t height,
        uint32_t depth);

    /**
     * Backend implementation of ::palCmdTraceRaysIndirect.
     *
     * Must obey the rules and semantics documented in palCmdTraceRaysIndirect().
     */
    PalResult(PAL_CALL* cmdTraceRaysIndirect)(
        PalCommandBuffer* cmdBuffer,
        uint32_t raygenIndex,
        PalShaderBindingTable* sbt,
        PalBuffer* buffer);

    /**
     * Backend implementation of ::palCmdBindDescriptorSet.
     *
     * Must obey the rules and semantics documented in palCmdBindDescriptorSet().
     */
    PalResult(PAL_CALL* cmdBindDescriptorSet)(
        PalCommandBuffer* cmdBuffer,
        uint32_t setIndex,
        PalDescriptorSet* set);

    /**
     * Backend implementation of ::palCmdPushConstants.
     *
     * Must obey the rules and semantics documented in palCmdPushConstants().
     */
    PalResult(PAL_CALL* cmdPushConstants)(
        PalCommandBuffer* cmdBuffer,
        uint32_t offset,
        uint32_t size,
        const void* value);

    /**
     * Backend implementation of ::palCmdSetCullMode.
     *
     * Must obey the rules and semantics documented in palCmdSetCullMode().
     */
    PalResult(PAL_CALL* cmdSetCullMode)(
        PalCommandBuffer* cmdBuffer,
        PalCullMode cullMode);

    /**
     * Backend implementation of ::palCmdSetFrontFace.
     *
     * Must obey the rules and semantics documented in palCmdSetFrontFace().
     */
    PalResult(PAL_CALL* cmdSetFrontFace)(
        PalCommandBuffer* cmdBuffer,
        PalFrontFace frontFace);

    /**
     * Backend implementation of ::palCmdSetPrimitiveTopology.
     *
     * Must obey the rules and semantics documented in palCmdSetPrimitiveTopology().
     */
    PalResult(PAL_CALL* cmdSetPrimitiveTopology)(
        PalCommandBuffer* cmdBuffer,
        PalPrimitiveTopology topology);

    /**
     * Backend implementation of ::palCmdSetDepthTestEnable.
     *
     * Must obey the rules and semantics documented in palCmdSetDepthTestEnable().
     */
    PalResult(PAL_CALL* cmdSetDepthTestEnable)(
        PalCommandBuffer* cmdBuffer,
        PalBool enable);

    /**
     * Backend implementation of ::palCmdSetDepthWriteEnable.
     *
     * Must obey the rules and semantics documented in palCmdSetDepthWriteEnable().
     */
    PalResult(PAL_CALL* cmdSetDepthWriteEnable)(
        PalCommandBuffer* cmdBuffer,
        PalBool enable);

    /**
     * Backend implementation of ::palCmdSetStencilOp.
     *
     * Must obey the rules and semantics documented in palCmdSetStencilOp().
     */
    PalResult(PAL_CALL* cmdSetStencilOp)(
        PalCommandBuffer* cmdBuffer,
        PalStencilFaceFlags faceMask,
        PalStencilOp failOp,
        PalStencilOp passOp,
        PalStencilOp depthFailOp,
        PalCompareOp compareOp);

    /**
     * Backend implementation of ::palCreateAccelerationstructure.
     *
     * Must obey the rules and semantics documented in palCreateAccelerationstructure().
     */
    PalResult(PAL_CALL* createAccelerationstructure)(
        PalDevice* device,
        const PalAccelerationStructureCreateInfo* info,
        PalAccelerationStructure** outAs);

    /**
     * Backend implementation of ::palDestroyAccelerationstructure.
     *
     * Must obey the rules and semantics documented in palDestroyAccelerationstructure().
     */
    void(PAL_CALL* destroyAccelerationstructure)(PalAccelerationStructure* as);

    /**
     * Backend implementation of ::palGetAccelerationStructureBuildSize.
     *
     * Must obey the rules and semantics documented in palGetAccelerationStructureBuildSize().
     */
    PalResult(PAL_CALL* getAccelerationStructureBuildSize)(
        PalDevice* device,
        PalAccelerationStructureBuildInfo* info,
        PalAccelerationStructureBuildSize* size);

    /**
     * Backend implementation of ::palCreateBuffer.
     *
     * Must obey the rules and semantics documented in palCreateBuffer().
     */
    PalResult(PAL_CALL* createBuffer)(
        PalDevice* device,
        const PalBufferCreateInfo* info,
        PalBuffer** outBuffer);

    /**
     * Backend implementation of ::palDestroyBuffer.
     *
     * Must obey the rules and semantics documented in palDestroyBuffer().
     */
    void(PAL_CALL* destroyBuffer)(PalBuffer* buffer);

    /**
     * Backend implementation of ::palGetBufferMemoryRequirements.
     *
     * Must obey the rules and semantics documented in palGetBufferMemoryRequirements().
     */
    PalResult(PAL_CALL* getBufferMemoryRequirements)(
        PalBuffer* buffer,
        PalMemoryRequirements* requirements);

    /**
     * Backend implementation of ::palComputeInstanceBufferRequirements.
     *
     * Must obey the rules and semantics documented in palComputeInstanceBufferRequirements().
     */
    PalResult(PAL_CALL* computeInstanceBufferRequirements)(
        PalDevice* device,
        uint32_t instanceCount,
        uint64_t* outSize);

    /**
     * Backend implementation of ::palComputeImageCopyStagingBufferRequirements.
     *
     * Must obey the rules and semantics documented in
     * palComputeImageCopyStagingBufferRequirements().
     */
    PalResult(PAL_CALL* computeImageCopyStagingBufferRequirements)(
        PalDevice* device,
        PalFormat imageFormat,
        PalBufferImageCopyInfo* copyInfo,
        uint32_t* outBufferRowLength,
        uint32_t* outBufferImageHeight,
        uint64_t* outSize);

    /**
     * Backend implementation of ::palWriteToInstanceBuffer.
     *
     * Must obey the rules and semantics documented in palWriteToInstanceBuffer().
     */
    PalResult(PAL_CALL* writeToInstanceBuffer)(
        PalDevice* device,
        void* ptr,
        PalAccelerationStructureInstance* instances,
        uint32_t instanceCount);

    /**
     * Backend implementation of ::palWriteToImageCopyStagingBuffer.
     *
     * Must obey the rules and semantics documented in palWriteToImageCopyStagingBuffer().
     */
    PalResult(PAL_CALL* writeToImageCopyStagingBuffer)(
        PalDevice* device,
        void* ptr,
        void* srcData,
        PalFormat imageFormat,
        PalBufferImageCopyInfo* copyInfo);

    /**
     * Backend implementation of ::palBindBufferMemory.
     *
     * Must obey the rules and semantics documented in palBindBufferMemory().
     */
    PalResult(PAL_CALL* bindBufferMemory)(
        PalBuffer* buffer,
        PalMemory* memory,
        uint64_t offset);

    /**
     * Backend implementation of ::palMapBuffer.
     *
     * Must obey the rules and semantics documented in palMapBuffer().
     */
    PalResult(PAL_CALL* mapBuffer)(
        PalBuffer* buffer,
        uint64_t offset,
        uint64_t size,
        void** outPtr);

    /**
     * Backend implementation of ::palUnmapBuffer.
     *
     * Must obey the rules and semantics documented in palUnmapBuffer().
     */
    void(PAL_CALL* unmapBuffer)(PalBuffer* buffer);

    /**
     * Backend implementation of ::palGetBufferDeviceAddress.
     *
     * Must obey the rules and semantics documented in palGetBufferDeviceAddress().
     */
    PalDeviceAddress(PAL_CALL* getBufferDeviceAddress)(PalBuffer* buffer);

    /**
     * Backend implementation of ::palCreateDescriptorSetLayout.
     *
     * Must obey the rules and semantics documented in palCreateDescriptorSetLayout().
     */
    PalResult(PAL_CALL* createDescriptorSetLayout)(
        PalDevice* device,
        const PalDescriptorSetLayoutCreateInfo* info,
        PalDescriptorSetLayout** outLayout);

    /**
     * Backend implementation of ::palDestroyDescriptorSetLayout.
     *
     * Must obey the rules and semantics documented in palDestroyDescriptorSetLayout().
     */
    void(PAL_CALL* destroyDescriptorSetLayout)(PalDescriptorSetLayout* layout);

    /**
     * Backend implementation of ::palCreateDescriptorPool.
     *
     * Must obey the rules and semantics documented in palCreateDescriptorPool().
     */
    PalResult(PAL_CALL* createDescriptorPool)(
        PalDevice* device,
        const PalDescriptorPoolCreateInfo* info,
        PalDescriptorPool** outPool);

    /**
     * Backend implementation of ::palDestroyDescriptorPool.
     *
     * Must obey the rules and semantics documented in palDestroyDescriptorPool().
     */
    void(PAL_CALL* destroyDescriptorPool)(PalDescriptorPool* pool);

    /**
     * Backend implementation of ::palResetDescriptorPool.
     *
     * Must obey the rules and semantics documented in palResetDescriptorPool().
     */
    PalResult(PAL_CALL* resetDescriptorPool)(PalDescriptorPool* pool);

    /**
     * Backend implementation of ::palAllocateDescriptorSet.
     *
     * Must obey the rules and semantics documented in palAllocateDescriptorSet().
     */
    PalResult(PAL_CALL* allocateDescriptorSet)(
        PalDevice* device,
        PalDescriptorPool* pool,
        PalDescriptorSetLayout* layout,
        PalDescriptorSet** outSet);

    /**
     * Backend implementation of ::palUpdateDescriptorSet.
     *
     * Must obey the rules and semantics documented in palUpdateDescriptorSet().
     */
    PalResult(PAL_CALL* updateDescriptorSet)(
        PalDevice* device,
        uint32_t count,
        PalDescriptorSetWriteInfo* infos);

    /**
     * Backend implementation of ::palCreatePipelineLayout.
     *
     * Must obey the rules and semantics documented in palCreatePipelineLayout().
     */
    PalResult(PAL_CALL* createPipelineLayout)(
        PalDevice* device,
        const PalPipelineLayoutCreateInfo* info,
        PalPipelineLayout** outLayout);

    /**
     * Backend implementation of ::palDestroyPipelineLayout.
     *
     * Must obey the rules and semantics documented in palDestroyPipelineLayout().
     */
    void(PAL_CALL* destroyPipelineLayout)(PalPipelineLayout* layout);

    /**
     * Backend implementation of ::palCreateGraphicsPipeline.
     *
     * Must obey the rules and semantics documented in palCreateGraphicsPipeline().
     */
    PalResult(PAL_CALL* createGraphicsPipeline)(
        PalDevice* device,
        const PalGraphicsPipelineCreateInfo* info,
        PalPipeline** outPipeline);

    /**
     * Backend implementation of ::palCreateComputePipeline.
     *
     * Must obey the rules and semantics documented in palCreateComputePipeline().
     */
    PalResult(PAL_CALL* createComputePipeline)(
        PalDevice* device,
        const PalComputePipelineCreateInfo* info,
        PalPipeline** outPipeline);

    /**
     * Backend implementation of ::palCreateRayTracingPipeline.
     *
     * Must obey the rules and semantics documented in palCreateRayTracingPipeline().
     */
    PalResult(PAL_CALL* createRayTracingPipeline)(
        PalDevice* device,
        const PalRayTracingPipelineCreateInfo* info,
        PalPipeline** outPipeline);

    /**
     * Backend implementation of ::palDestroyPipeline.
     *
     * Must obey the rules and semantics documented in palDestroyPipeline().
     */
    void(PAL_CALL* destroyPipeline)(PalPipeline* pipeline);

    /**
     * Backend implementation of ::palCreateShaderBindingTable.
     *
     * Must obey the rules and semantics documented in palCreateShaderBindingTable().
     */
    PalResult(PAL_CALL* createShaderBindingTable)(
        PalDevice* device,
        const PalShaderBindingTableCreateInfo* info,
        PalShaderBindingTable** outSbt);

    /**
     * Backend implementation of ::palDestroyShaderBindingTable.
     *
     * Must obey the rules and semantics documented in palDestroyShaderBindingTable().
     */
    void(PAL_CALL* destroyShaderBindingTable)(PalShaderBindingTable* sbt);

    /**
     * Backend implementation of ::palUpdateShaderBindingTable.
     *
     * Must obey the rules and semantics documented in palUpdateShaderBindingTable().
     */
    PalResult(PAL_CALL* updateShaderBindingTable)(
        PalShaderBindingTable* sbt,
        uint32_t count,
        PalShaderBindingTableRecordInfo* infos);
} PalGraphicsBackendVtable1;

/**
 * @brief Initialize the graphics system.
 *
 * The debugger, allocator and custom backends will not not copied, therefore the pointers must
 * remain valid until the graphics system is shutdown. Set the debugger or
 * PalGraphicsDebugger::callback to `nullptr` to disable debugging and validation layers.
 *
 * If `debugger` is not `nullptr` and there is no debug layers, this function will not fail but
 * debugging will be disabled.
 *
 * All backends must have their vtable functions fully set according to the version requirements.
 * If a feature is not supported by a backend, `PAL_RESULT_ADAPTER_FEATURE_NOT_SUPPORTED` must be
 * returned by the appropriate function. This is validated at initialization and will fail and
 * return `PAL_RESULT_INVALID_ARGUMENT`.
 *
 * @param[in] debugger Optional debugger. Set to `nullptr` to disable debugging and validation
 * layers.
 * @param[in] allocator Optional user-provided allocator. Set to `nullptr` to use default.
 * @param[in] customBackendCount The number of custom backends in `customBackends`.
 * @param[in] customBackends Pointer to an array of custom backends.
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Must only be called from the main thread.
 *
 * @since 2.0
 * @sa palShutdownGraphics
 */
PAL_API PalResult PAL_CALL palInitGraphics(
    const PalGraphicsDebugger* debugger,
    const PalAllocator* allocator,
    uint32_t customBackendCount,
    const PalGraphicsBackendInfo* customBackends);

/**
 * @brief Shutdown the graphics system.
 *
 * If the graphics system has not been initialized, the function returns silently.
 * All created devices, queues, images, swapchains etc must be destroyed before this call.
 *
 * Thread safety: Must only be called from the main thread.
 *
 * @since 2.0
 * @sa palInitGraphics
 */
PAL_API void PAL_CALL palShutdownGraphics();

/**
 * @brief Returns a list of all adapters (GPU) from custom and internal backends.
 *
 * The graphics system must be initialized before this call.
 *
 * If a custom backend which implements an adapter with its API type of Vulkan, and there is an
 * adapter from the internal backends with the same specifications, this function will return both
 * of them in the list. Use PalAdapterInfo::backendName to differentiate between custom and
 * internal backend. the backend name for internal backend is `PAL`.
 *
 * Call this function first with PalAdapter array set to `nullptr` to get the number of adapters.
 * Allocate memory for the PalAdapter array and passed in the count and the allocated array. If
 * the count of the array is less than the number of adapters, PAL will write upto that limit.
 *
 * The adapter handles must not be freed by the user, they are managed by the
 * graphics system. Users are required to cache this, and call this function again
 * if adapters are added or removed which is rare except for virtual ones.
 *
 * @param[in, out] count Capacity of the PalAdapter array.
 * @param[out] outAdapters User allocated array of PalAdapter.
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Must only be called from the main thread.
 *
 * @since 2.0
 */
PAL_API PalResult PAL_CALL palEnumerateAdapters(
    int32_t* count,
    PalAdapter** outAdapters);

/**
 * @brief Get information about an adapter (GPU).
 *
 * The graphics system must be initialized before this call.
 *
 * @param[in] adapter Adapter to query information on.
 * @param[out] info Pointer to a PalAdapterInfo to fill.
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Thread safe if `info` is per thread.
 *
 * @since 2.0
 * @sa palEnumerateAdapters
 */
PAL_API PalResult PAL_CALL palGetAdapterInfo(
    PalAdapter* adapter,
    PalAdapterInfo* info);

/**
 * @brief Get capabilites or limits about an adapter (GPU).
 *
 * The graphics system must be initialized before this call.
 *
 * @param[in] adapter Adapter to query capabilities on.
 * @param[out] caps Pointer to a PalAdapterCapabilities to fill.
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Thread safe if `caps` is per thread.
 *
 * @since 2.0
 * @sa palEnumerateAdapters
 */
PAL_API PalResult PAL_CALL palGetAdapterCapabilities(
    PalAdapter* adapter,
    PalAdapterCapabilities* caps);

/**
 * @brief Get the supported features of an adapter (GPU).
 *
 * The graphics system must be initialized before this call.
 *
 * @param[in] adapter Adapter to query features on.
 *
 * @return adapter features on success or `0` on failure.
 *
 * Thread safety: Thread safe.
 *
 * @since 2.0
 * @sa palEnumerateAdapters
 */
PAL_API PalAdapterFeatures PAL_CALL palGetAdapterFeatures(PalAdapter* adapter);

/**
 * @brief Get the highest supported shader target of an adapter (GPU).
 *
 * The graphics system must be initialized before this call.
 *
 * @param[in] adapter Adapter to query.
 * @param[in] shaderFormat The shader format. Must have only a single bit set.
 *
 * @return The highest supported shader target encoded with `PAL_MAKE_SHADER_TARGET` macro
 * on success otherwise `0` on failure.
 *
 * Thread safety: Thread safe.
 *
 * @since 2.0
 * @sa palEnumerateAdapters
 */
PAL_API uint32_t PAL_CALL palGetHighestSupportedShaderTarget(
    PalAdapter* adapter,
    PalShaderFormats shaderFormat);

/**
 * @brief Create a device from an adapter (GPU).
 *
 * The graphics system must be initialized before this call. PAL does not enable any features
 * implicitly not even common ones like `PAL_ADAPTER_FEATURE_SWAPCHAIN`.
 *
 * Every requested feature must be supported by the adapter. Use palGetAdapterFeatures to check
 * the supported features of the adapter that can be enabled. Using a feature which is not
 * supported will fail and return `PAL_RESULT_ADAPTER_FEATURE_NOT_SUPPORTED`.
 *
 * @param[in] adapter Adapter that creates the device.
 * @param[in] features Adapter features to enable. Must be supported.
 * @param[out] outDevice Pointer to a PalDevice to recieve the created device. Must not be `nullptr`.
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Thread safe if `adapter` is externally synchronized.
 *
 * @since 2.0
 * @sa palDestroyDevice
 */
PAL_API PalResult PAL_CALL palCreateDevice(
    PalAdapter* adapter,
    PalAdapterFeatures features,
    PalDevice** outDevice);

/**
 * @brief Destroy a device.
 *
 * The graphics system must be initialized before this call.
 * If the provided device is invalid or `nullptr`, this function returns
 * silently.
 *
 * @param[in] device Pointer to the device to destroy.
 *
 * Thread safety: Thread safe if the adapter used to create the device is
 * externally synchronized.
 *
 * @since 2.0
 * @sa palCreateDevice
 */
PAL_API void PAL_CALL palDestroyDevice(PalDevice* device);

/**
 * @brief Allocates GPU memory for the specified device.
 *
 * The graphics system must be initialized before this call. On CPU adapters, there is usually no
 * `PAL_MEMORY_TYPE_GPU_ONLY` memory type available. So it uses shared memory as the vram and set
 * the shared memory to the `PAL_MEMORY_TYPE_GPU_ONLY` for correctness.
 *
 * @param[in] device Pointer to device to allocate memory on.
 * @param[in] type Memory type to allocate. Must be supported by the adapter associated with the
 * device.
 * @param[in] memoryMask Memory mask. Must match memory type.
 * @param[in] size Number of bytes to allocate. Must not be 0.
 * @param[out] outMemory Pointer to a PalMemory to recieved the allocated GPU memory.
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Thread safe if `device` is externally synchronized and
 * `outMemory` is per thread.
 *
 * @since 2.0
 * @sa palFreeMemory
 */
PAL_API PalResult PAL_CALL palAllocateMemory(
    PalDevice* device,
    PalMemoryType type,
    uint64_t memoryMask,
    uint64_t size,
    PalMemory** outMemory);

/**
 * @brief Free GPU memory allocated by palAllocateMemory.
 *
 * The graphics system must be initialized before this call.
 * If `memory` is `nullptr`, this function will return silently.
 *
 * @param[in] device Pointer to device to free memory on.
 * @param[in] memory Pointer to memory to free.
 *
 * Thread safety: Thread safe if `device` is externally synchronized and
 * `outMemory` is per thread.
 *
 * @since 2.0
 * @sa palAllocateMemory
 */
PAL_API void PAL_CALL palFreeMemory(
    PalDevice* device,
    PalMemory* memory);

/**
 * @brief Get sampler anisotropy feature capabilites or limits about a device.
 *
 * The graphics system must be initialized before this call.
 *
 * `PAL_ADAPTER_FEATURE_SAMPLER_ANISOTROPY` must be supported and enabled when creating the
 * device. If not, this function fails and returns `PAL_RESULT_ADAPTER_FEATURE_NOT_SUPPORTED`.
 *
 * @param[in] device Device to query sampler anisotropy feature capabilities on.
 * @param[out] caps Pointer to a PalSamplerAnisotropyCapabilities to fill.
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Thread safe if `caps` is per thread.
 *
 * @since 2.0
 */
PAL_API PalResult PAL_CALL palQuerySamplerAnisotropyCapabilities(
    PalDevice* device,
    PalSamplerAnisotropyCapabilities* caps);

/**
 * @brief Get multi view feature capabilites or limits about a device.
 *
 * The graphics system must be initialized before this call.
 *
 * `PAL_ADAPTER_FEATURE_MULTI_VIEW` must be supported and enabled when creating the
 * device. If not, this function fails and returns `PAL_RESULT_ADAPTER_FEATURE_NOT_SUPPORTED`.
 *
 * @param[in] device Device to query multi view feature capabilities on.
 * @param[out] caps Pointer to a PalMultiViewCapabilities to fill.
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Thread safe if `caps` is per thread.
 *
 * @since 2.0
 */
PAL_API PalResult PAL_CALL palQueryMultiViewCapabilities(
    PalDevice* device,
    PalMultiViewCapabilities* caps);

/**
 * @brief Get multi viewport feature capabilites or limits about a device.
 *
 * The graphics system must be initialized before this call.
 *
 * `PAL_ADAPTER_FEATURE_MULTI_VIEWPORT` must be supported and enabled when creating the
 * device. If not, this function fails and returns `PAL_RESULT_ADAPTER_FEATURE_NOT_SUPPORTED`.
 *
 * @param[in] device Device to query multi viewport feature capabilities on.
 * @param[out] caps Pointer to a PalMultiViewportCapabilities to fill.
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Thread safe if `caps` is per thread.
 *
 * @since 2.0
 */
PAL_API PalResult PAL_CALL palQueryMultiViewportCapabilities(
    PalDevice* device,
    PalMultiViewportCapabilities* caps);

/**
 * @brief Get depth stencil feature capabilites or limits about a device.
 *
 * The graphics system must be initialized before this call.
 *
 * `PAL_ADAPTER_FEATURE_DEPTH_STENCIL_RESOLVE` must be supported and enabled when creating the
 * device. If not, this function fails and returns `PAL_RESULT_ADAPTER_FEATURE_NOT_SUPPORTED`.
 *
 * @param[in] device Device to query depth stencil feature capabilities on.
 * @param[out] caps Pointer to a PalDepthStencilCapabilities to fill.
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Thread safe if `caps` is per thread.
 *
 * @since 2.0
 */
PAL_API PalResult PAL_CALL palQueryDepthStencilCapabilities(
    PalDevice* device,
    PalDepthStencilCapabilities* caps);

/**
 * @brief Get fragment shading rate feature capabilites or limits about a device.
 *
 * The graphics system must be initialized before this call.
 *
 * `PAL_ADAPTER_FEATURE_FRAGMENT_SHADING_RATE` must be supported and enabled when creating the
 * device. If not, this function fails and returns `PAL_RESULT_ADAPTER_FEATURE_NOT_SUPPORTED`.
 *
 * @param[in] device Device to query fragment shading rate feature capabilities on.
 * @param[out] caps Pointer to a PalFragmentShadingRateCapabilities to fill.
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Thread safe if `caps` is per thread.
 *
 * @since 2.0
 */
PAL_API PalResult PAL_CALL palQueryFragmentShadingRateCapabilities(
    PalDevice* device,
    PalFragmentShadingRateCapabilities* caps);

/**
 * @brief Get mesh shader feature capabilites or limits about a device.
 *
 * The graphics system must be initialized before this call.
 *
 * `PAL_ADAPTER_FEATURE_MESH_SHADER` must be supported and enabled when creating the
 * device. If not, this function fails and returns `PAL_RESULT_ADAPTER_FEATURE_NOT_SUPPORTED`.
 *
 * @param[in] device Device to query mesh shader feature capabilities on.
 * @param[out] caps Pointer to a PalMeshShaderCapabilities to fill.
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Thread safe if `caps` is per thread.
 *
 * @since 2.0
 */
PAL_API PalResult PAL_CALL palQueryMeshShaderCapabilities(
    PalDevice* device,
    PalMeshShaderCapabilities* caps);

/**
 * @brief Get ray tracing feature capabilites or limits about a device.
 *
 * The graphics system must be initialized before this call.
 *
 * `PAL_ADAPTER_FEATURE_RAY_TRACING` must be supported and enabled when creating the
 * device. If not, this function fails and returns `PAL_RESULT_ADAPTER_FEATURE_NOT_SUPPORTED`.
 *
 * @param[in] device Device to query ray tracing feature capabilities on.
 * @param[out] caps Pointer to a PalRayTracingCapabilities to fill.
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Thread safe if `caps` is per thread.
 *
 * @since 2.0
 */
PAL_API PalResult PAL_CALL palQueryRayTracingCapabilities(
    PalDevice* device,
    PalRayTracingCapabilities* caps);

/**
 * @brief Get descriptor indexing feature capabilites or limits about a device.
 *
 * The graphics system must be initialized before this call.
 *
 * `PAL_ADAPTER_FEATURE_DESCRIPTOR_INDEXING` must be supported and enabled when creating the
 * device. If not, this function fails and returns `PAL_RESULT_ADAPTER_FEATURE_NOT_SUPPORTED`.
 *
 * @param[in] device Device to query descriptor indexing feature capabilities on.
 * @param[out] caps Pointer to a PalDescriptorIndexingCapabilities to fill.
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Thread safe if `caps` is per thread.
 *
 * @since 2.0
 */
PAL_API PalResult PAL_CALL palQueryDescriptorIndexingCapabilities(
    PalDevice* device,
    PalDescriptorIndexingCapabilities* caps);

/**
 * @brief Create a queue from a device.
 *
 * The graphics system must be initialized before this call.
 *
 * The number of queues of each type which can be created is limited per adapter. check with
 * PalAdapterCapabilities::maxComputeQueues, PalAdapterCapabilities::maxGraphicsQueues and
 * PalAdapterCapabilities::maxCopyQueues respectively for the limit for each queue type.
 * Creating more queues than the supported will fail and return `PAL_RESULT_OUT_OF_QUEUE`.
 *
 * Not all graphics queues support presentation. Create a graphics queue and then check if
 * its support presentation for the provided surface. see `palCanQueuePresent()`. Any graphics
 * queue supports offscreen rendering.
 *
 * @param[in] device Device that creates the queue.
 * @param[in] type Queue type. (eg. PAL_QUEUE_TYPE_GRAPHICS).
 * @param[out] outQueue Pointer to a PalQueue to recieve the created queue.
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Thread safe if `device` is externally synchronized.
 *
 * @since 2.0
 * @sa palDestroyQueue
 */
PAL_API PalResult PAL_CALL palCreateQueue(
    PalDevice* device,
    PalQueueType type,
    PalQueue** outQueue);

/**
 * @brief Destroy a queue.
 *
 * The graphics system must be initialized before this call.
 * If the provided queue is invalid or `nullptr`, this function returns
 * silently.
 *
 * @param[in] queue Queue to destroy.
 *
 * Thread safety: Thread safe if the device used to create the queue is
 * externally synchronized.
 *
 * @since 2.0
 * @sa palCreateQueue
 */
PAL_API void PAL_CALL palDestroyQueue(PalQueue* queue);

/**
 * @brief Check if a queue is presentable to the provided window.
 *
 * The graphics system must be initialized before this call.
 *
 * @param[in] queue Queue to query.
 * @param[in] surface Surface to check presentation support for.
 *
 * @return True if queue can present otherwise `PAL_FALSE` if queue can not present.
 *
 * Thread safety: Must only be called from the main thread.
 *
 * @since 2.0
 * @sa palCreateQueue
 */
PAL_API PalBool PAL_CALL palCanQueuePresent(
    PalQueue* queue,
    PalSurface* surface);

/**
 * @brief Blocks indefinitely until the queue becomes idle.
 *
 * The graphics system must be initialized before this call.
 *
 * This function blocks indefinitely until all submitted work on the queue has been completetd.
 * Returns `PAL_RESULT_SUCCESS` to indicate all pending operations has been completetd.
 *
 * @param[in] queue Pointer to queue to wait.
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Thread safe if `queue` is externally synchronized.
 *
 * @since 2.0
 */
PAL_API PalResult PAL_CALL palWaitQueue(PalQueue* queue);

/**
 * @brief Returns a list of all supported formats of an adapter (GPU).
 *
 * The graphics system must be initialized before this call.
 *
 * This function returns the supported format with the supported image usages
 * associated with the format. This is a handy way of selecting a format based on the image
 * usages. Use palIsFormatSupported() to check for a specific format.
 *
 * Call this function first with PalFormatInfo array set to `nullptr` to get the number of formats.
 * Allocate memory for the PalFormatInfo array and passed in the count and the allocated array. If
 * the count of the array is less than the number of formats, PAL will write upto that limit.
 *
 * @param[in] adapter Adapter to query formats on.
 * @param[in, out] count Capacity of the PalFormatInfo array.
 * @param[out] outFormats User allocated array of PalFormatInfo.
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Thread safe if `outFormats` is per thread.
 *
 * @since 2.0
 * @sa palIsFormatSupported
 */
PAL_API PalResult PAL_CALL palEnumerateFormats(
    PalAdapter* adapter,
    int32_t* count,
    PalFormatInfo* outFormats);

/**
 * @brief Check support for a format on an adapter (GPU).
 *
 * The graphics system must be initialized before this call.
 *
 * This is much faster than enumerating all the formats to pick one. You directly check support
 * for the format you want to use. Call palQueryFormatImageUsages() to check for supported image
 * usages if format is supported.
 *
 * @param[in] adapter Adapter to query format on.
 * @param[in] format Format to query support for.
 *
 * @return True if format is supported otherwise `PAL_FALSE` if not supported.
 *
 * Thread safety: Thread safe.
 *
 * @since 2.0
 * @sa palQueryFormatImageUsages
 * @sa palQueryFormatImageViewUsages
 */
PAL_API PalBool PAL_CALL palIsFormatSupported(
    PalAdapter* adapter,
    PalFormat format);

/**
 * @brief Checks supported image usages associated with a format.
 *
 * The graphics system must be initialized before this call.
 *
 * @param[in] adapter Adapter to query format on.
 * @param[in] format Format to query image usages for.
 *
 * @return Supported image usages on success otherwise `0` on failure.
 *
 * Thread safety: Thread safe.
 *
 * @since 2.0
 */
PAL_API PalImageUsages PAL_CALL palQueryFormatImageUsages(
    PalAdapter* adapter,
    PalFormat format);

/**
 * @brief Checks supported sample count associated with a format.
 *
 * The graphics system must be initialized before this call.
 *
 * @param[in] adapter Adapter to query format on.
 * @param[in] format Format to query sample count for.
 *
 * @return Supported sample count on success otherwise `0` on failure.
 *
 * Thread safety: Thread safe.
 *
 * @since 2.0
 */
PAL_API PalSampleCount PAL_CALL palQueryFormatSampleCount(
    PalAdapter* adapter,
    PalFormat format);

/**
 * @brief Create an image.
 *
 * The graphics system must be initialized before this call.
 *
 * PalImageCreateInfo::width, PalImageCreateInfo::height and PalImageCreateInfo::sampleCount
 * must not be greater than the limits of the adapter used to create the device. Check
 * adapter capabilities for the limits.
 *
 * @param[in] device Device that creates the image.
 * @param[in] info Pointer to a PalImageCreateInfo struct that specifies parameters.
 * Must not be `nullptr`.
 * @param[out] outImage Pointer to a PalImage to recieve the created image.
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Thread safe if `device` is externally synchronized.
 *
 * @since 2.0
 * @sa palDestroyImage
 */
PAL_API PalResult PAL_CALL palCreateImage(
    PalDevice* device,
    const PalImageCreateInfo* info,
    PalImage** outImage);

/**
 * @brief Destroy an image.
 *
 * The graphics system must be initialized before this call.
 * If the provided image is invalid or `nullptr`, this function returns
 * silently.
 *
 * @param[in] image Image to destroy.
 *
 * Thread safety: Thread safe if the device used to create the image is
 * externally synchronized.
 *
 * @since 2.0
 * @sa palCreateImage
 */
PAL_API void PAL_CALL palDestroyImage(PalImage* image);

/**
 * @brief Get information about an image.
 *
 * The graphics system must be initialized before this call.
 * This function also supports swapchain images.
 *
 * @param[in] image Image to query information on.
 * @param[out] info Pointer to a PalImageInfo to fill.
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Thread safe if `info` is per thread.
 *
 * @since 2.0
 * @sa palCreateImage
 */
PAL_API PalResult PAL_CALL palGetImageInfo(
    PalImage* image,
    PalImageInfo* info);

/**
 * @brief Get memory requirements for the provided image.
 *
 * The graphics system must be initialized before this call.
 *
 * @param[in] image Image to query memory requirements on.
 * @param[out] requirements Pointer to a PalMemoryRequirements to fill.
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Thread safe if `requirements` is per thread.
 *
 * @since 2.0
 */
PAL_API PalResult PAL_CALL palGetImageMemoryRequirements(
    PalImage* image,
    PalMemoryRequirements* requirements);

/**
 * @brief Bind an allocated memory to an image.
 *
 * The graphics system must be initialized before this call.
 * The memory size and alignment should match the requirements of the image.
 * Get the requirements with palGetImageMemoryRequirements().
 *
 * @param[in] image Image to bind memory to.
 * @param[in] memory Memory to bind. Must not be `nullptr`.
 * @param[in] offset Starting point within the memory.
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Thread safe if `requirements` is per thread.
 *
 * @since 2.0
 * @sa palGetImageMemoryRequirements
 */
PAL_API PalResult PAL_CALL palBindImageMemory(
    PalImage* image,
    PalMemory* memory,
    uint64_t offset);

/**
 * @brief Create an image view.
 *
 * The graphics system must be initialized before this call.
 *
 * PalImageViewCreateInfo::type must be compatible by the type of the base image. Eg. A 2D base
 * image must be have an image view of either `PAL_IMAGE_VIEW_TYPE_2D` or
 * `PAL_IMAGE_VIEW_TYPE_2D_ARRAY`.
 *
 * `PAL_ADAPTER_FEATURE_IMAGE_VIEW_CUBE_ARRAY` must be supported and enabled by the device
 * used to create the image view if `PAL_IMAGE_VIEW_TYPE_CUBE_ARRAY` will be used.
 *
 * @param[in] device Device that creates the image view.
 * @param[in] image Image to create the image view with.
 * @param[in] info Pointer to a PalImageViewCreateInfo struct that specifies parameters.
 * Must not be `nullptr`.
 * @param[out] outImageView Pointer to a PalImageView to recieve the created image view.
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Thread safe if `device` is externally synchronized.
 *
 * @since 2.0
 * @sa palDestroyImageView
 */
PAL_API PalResult PAL_CALL palCreateImageView(
    PalDevice* device,
    PalImage* image,
    const PalImageViewCreateInfo* info,
    PalImageView** outImageView);

/**
 * @brief Destroy an image view.
 *
 * The graphics system must be initialized before this call.
 * If the provided image view is invalid or `nullptr`, this function returns
 * silently.
 *
 * @param[in] imageView Image view to destroy.
 *
 * Thread safety: Thread safe if the device used to create the image view is
 * externally synchronized.
 *
 * @since 2.0
 * @sa palCreateImageView
 */
PAL_API void PAL_CALL palDestroyImageView(PalImageView* imageView);

/**
 * @brief Create a sampler.
 *
 * The graphics system must be initialized before this call.
 * Samplers are immutable so any parameter used to create it cannot will be fixed after
 * creation.
 *
 * @param[in] device Device that creates the sampler.
 * @param[in] info Pointer to a PalSamplerCreateInfo struct that specifies parameters.
 * Must not be `nullptr`.
 * @param[out] outSampler Pointer to a PalSampler to recieve the created sampler.
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Thread safe if `device` is externally synchronized.
 *
 * @since 2.0
 * @sa palDestroySampler
 */
PAL_API PalResult PAL_CALL palCreateSampler(
    PalDevice* device,
    const PalSamplerCreateInfo* info,
    PalSampler** outSampler);

/**
 * @brief Destroy a sampler.
 *
 * The graphics system must be initialized before this call.
 * If the provided sampler is invalid or `nullptr`, this function returns
 * silently.
 *
 * @param[in] sampler Sampler to destroy.
 *
 * Thread safety: Thread safe if the device used to create the sampler is
 * externally synchronized.
 *
 * @since 2.0
 * @sa palCreateSampler
 */
PAL_API void PAL_CALL palDestroySampler(PalSampler* sampler);

/**
 * @brief Create a surface for a window.
 *
 * The graphics system must be initialized before this call.
 *
 * `PAL_ADAPTER_FEATURE_SWAPCHAIN` must be supported and enabled by the device if not, this
 * function will fail and return `PAL_RESULT_ADAPTER_FEATURE_NOT_SUPPORTED`.
 *
 * @param[in] device Device that creates the surface.
 * @param[in] window Window to create the surface for.
 * @param[in] windowInstance The instance of the window.
 * @param[in] instanceType The instance type of the window.
 * @param[out] outSurface Pointer to a PalSurface to recieve the created surface.
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Thread safe if `device` is externally synchronized.
 *
 * @since 2.0
 * @sa palDestroySurface
 */
PAL_API PalResult PAL_CALL palCreateSurface(
    PalDevice* device,
    void* window,
    void* windowInstance,
    PalWindowInstanceType instanceType,
    PalSurface** outSurface);

/**
 * @brief Destroy a surface.
 *
 * The graphics system must be initialized before this call.
 * If the provided surface is invalid or `nullptr`, this function returns
 * silently.
 *
 * @param[in] surface Surface to destroy.
 *
 * Thread safety: Thread safe if the device used to create the surface is
 * externally synchronized.
 *
 * @since 2.0
 * @sa palCreateSurface
 */
PAL_API void PAL_CALL palDestroySurface(PalSurface* surface);

/**
 * @brief Get surface capabilites about a device.
 *
 * The graphics system must be initialized before this call.
 *
 * `PAL_ADAPTER_FEATURE_SWAPCHAIN` must be supported and enabled when creating the
 * device. If not, this function fails and returns `PAL_RESULT_ADAPTER_FEATURE_NOT_SUPPORTED`.
 *
 * @param[in] device Device to query surface feature capabilities on.
 * @param[in] surface Surface to query capabilities.
 * @param[out] caps Pointer to a PalSurfaceCapabilities to fill.
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Thread safe if `caps` is per thread.
 *
 * @since 2.0
 */
PAL_API PalResult PAL_CALL palGetSurfaceCapabilities(
    PalDevice* device,
    PalSurface* surface,
    PalSurfaceCapabilities* caps);

/**
 * @brief Create a swaphain.
 *
 * The graphics system must be initialized before this call.
 *
 * `PAL_ADAPTER_FEATURE_SWAPCHAIN` must be supported and enabled by the device if not, this
 * function will fail and return `PAL_RESULT_ADAPTER_FEATURE_NOT_SUPPORTED`.
 *
 * @param[in] device Device that creates the swapchain.
 * @param[in] queue Queue to create swapchain with. This must be a graphics queue.
 * @param[in] surface Surface to create swapchain with.
 * @param[in] info Pointer to a PalSwapchainCreateInfo struct that specifies parameters.
 * Must not be `nullptr`.
 * @param[out] outSwapchain Pointer to a PalSwapchain to recieve the created swapchain.
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Thread safe if `device` is externally synchronized.
 *
 * @since 2.0
 * @sa palDestroySwapchain
 */
PAL_API PalResult PAL_CALL palCreateSwapchain(
    PalDevice* device,
    PalQueue* queue,
    PalSurface* surface,
    const PalSwapchainCreateInfo* info,
    PalSwapchain** outSwapchain);

/**
 * @brief Destroy a swapchain.
 *
 * The graphics system must be initialized before this call.
 * If the provided swapchain is invalid or `nullptr`, this function returns
 * silently.
 *
 * @param[in] swapchain Swapchain to destroy.
 *
 * Thread safety: Thread safe if the device used to create the swapchain is
 * externally synchronized.
 *
 * @since 2.0
 * @sa palCreateSwapchain
 */
PAL_API void PAL_CALL palDestroySwapchain(PalSwapchain* swapchain);

/**
 * @brief Get a swapchain image from the list of images with an index.
 *
 * The graphics system must be initialized before this call.
 *
 * @param[in] swapchain Swapchain to get image from.
 * @param[in] index Index of image in the list. Must not be greater than the image count.
 *
 * @return A pointer to the image on success otherwise `nullptr` on failure.
 *
 * Thread safety: Thread safe.
 *
 * @since 2.0
 * @sa palGetNextSwapchainImage
 */
PAL_API PalImage* PAL_CALL palGetSwapchainImage(
    PalSwapchain* swapchain,
    uint32_t index);

/**
 * @brief Get the next available image from the swapchain image list.
 *
 * The graphics system must be initialized before this call.
 *
 * @param[in] swapchain Swapchain to get image index from.
 * @param[in] info Pointer to a PalSwapchainNextImageInfo struct that specifies parameters.
 * Must not be `nullptr`.
 * @param[out] outIndex Pointer to a uint32_t to recieve the next image index.
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Thread safe if `swapchain` externally synchronized.
 *
 * @since 2.0
 * @sa palGetSwapchainImage
 */
PAL_API PalResult PAL_CALL palGetNextSwapchainImage(
    PalSwapchain* swapchain,
    PalSwapchainNextImageInfo* info,
    uint32_t* outIndex);

/**
 * @brief Present the swapchain.
 *
 * The graphics system must be initialized before this call.
 *
 * @param[in] swapchain Swapchain to present.
 * @param[in] imageIndex Swapchain image index to present.
 * @param[in] waitSemaphore The semaphore to wait for.
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Must only be called from the main thread.
 *
 * @since 2.0
 */
PAL_API PalResult PAL_CALL palPresentSwapchain(
    PalSwapchain* swapchain,
    uint32_t imageIndex,
    PalSemaphore* waitSemaphore);

/**
 * @brief Resize the provided swapchain.
 *
 * The graphics system must be initialized before this call.
 *
 * The swapchain images must not be in use before this call. All resources (image views) that
 * reference the swapchain images must be destroyed and recreated.
 *
 * @param[in] swapchain Swapchain to resize.
 * @param[in] newWidth The new width of the swapchain.
 * @param[in] newHeight The new height of the swapchain.
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Thread safe if `swapchain` externally synchronized.
 *
 * @since 2.0
 */
PAL_API PalResult PAL_CALL palResizeSwapchain(
    PalSwapchain* swapchain,
    uint32_t newWidth,
    uint32_t newHeight);

/**
 * @brief Create a shader.
 *
 * The graphics system must be initialized before this call.
 *
 * `PAL_ADAPTER_FEATURE_GEOMETRY_SHADER` must be supported and enabled by the device if
 * `PAL_SHADER_STAGE_GEOMETRY` will be used.
 *
 * `PAL_ADAPTER_FEATURE_MESH_SHADER` must be supported and enabled by the device if
 * `PAL_SHADER_STAGE_MESH` or `PAL_SHADER_STAGE_TASK` will be used.
 *
 * `PAL_ADAPTER_FEATURE_TESSELLATION_SHADER` must be supported and enabled by the device if
 * `PAL_SHADER_STAGE_TESSELLATION_CONTROL` or `PAL_SHADER_STAGE_TESSELLATION_EVALUATION` will
 * be used.
 *
 * `PAL_ADAPTER_FEATURE_RAY_TRACING` must be supported and enabled by the device if
 * `PAL_SHADER_STAGE_RAYGEN` or `PAL_SHADER_STAGE_CLOSEST_HIT` or `PAL_SHADER_STAGE_ANY_HIT` or
 * `PAL_SHADER_STAGE_MISS` or `PAL_SHADER_STAGE_INTERSECTION` or `PAL_SHADER_STAGE_CALLABLE` will
 * be used.
 *
 * @param[in] device Device that creates the shader.
 * @param[in] info Pointer to a PalShaderCreateInfo struct that specifies parameters.
 * Must not be `nullptr`.
 * @param[out] outShader Pointer to a PalShader to recieve the created shader.
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Thread safe if `device` is externally synchronized.
 *
 * @note The shader entry name must not be greater than `PAL_SHADER_ENTRY_NAME_SIZE (32)`.
 *
 * @since 2.0
 * @sa palDestroyShader
 */
PAL_API PalResult PAL_CALL palCreateShader(
    PalDevice* device,
    const PalShaderCreateInfo* info,
    PalShader** outShader);

/**
 * @brief Destroy a shader.
 *
 * The graphics system must be initialized before this call.
 * If the provided shader is invalid or `nullptr`, this function returns
 * silently.
 *
 * @param[in] shader Shader to destroy.
 *
 * Thread safety: Thread safe if the device used to create the shader is
 * externally synchronized.
 *
 * @since 2.0
 * @sa palCreateShader
 */
PAL_API void PAL_CALL palDestroyShader(PalShader* shader);

/**
 * @brief Create a fence.
 *
 * The graphics system must be initialized before this call.
 *
 * @param[in] device Device that creates the fence.
 * @param[in] signaled True if fence should be created signaled. If true, the fence must be reset
 * before waiting for it to prevent waiting forever.
 * @param[out] outFence Pointer to a PalFence to recieve the created fence.
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Thread safe if `device` is externally synchronized.
 *
 * @since 2.0
 * @sa palDestroyFence
 */
PAL_API PalResult PAL_CALL palCreateFence(
    PalDevice* device,
    PalBool signaled,
    PalFence** outFence);

/**
 * @brief Destroy a fence.
 *
 * The graphics system must be initialized before this call.
 * If the provided fence is invalid or `nullptr`, this function returns
 * silently.
 *
 * @param[in] fence Fence to destroy.
 *
 * Thread safety: Thread safe if the device used to create the fence is
 * externally synchronized.
 *
 * @since 2.0
 * @sa palCreateFence
 */
PAL_API void PAL_CALL palDestroyFence(PalFence* fence);

/**
 * @brief Wait for a fence.
 *
 * The graphics system must be initialized before this call.
 *
 * This function blocks for `timeout` until the fence is signaled or there is a timeout.
 * Returns `PAL_RESULT_SUCCESS` or `PAL_RESULT_TIMEOUT` respectively.
 *
 * @param[in] fence Fence to wait for.
 * @param[in] timeout Time to wait for in milliseconds. Set to `PAL_INFINITE` for indefintely.
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Thread safe if `fence` is externally synchronized.
 *
 * @since 2.0
 * @sa palIsFenceSignaled
 */
PAL_API PalResult PAL_CALL palWaitFence(
    PalFence* fence,
    uint64_t timeout);

/**
 * @brief Reset a fence to an unsignaled state.
 *
 * The graphics system must be initialized before this call.
 *
 * `PAL_ADAPTER_FEATURE_FENCE_RESET` must be supported and enabled when creating the
 * device. If not, this function fails and returns `PAL_RESULT_ADAPTER_FEATURE_NOT_SUPPORTED`.
 *
 * @param[in] fence Fence to reset.
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Thread safe if `fence` is externally synchronized.
 *
 * @since 2.0
 * @sa palIsFenceSignaled
 */
PAL_API PalResult PAL_CALL palResetFence(PalFence* fence);

/**
 * @brief Checks if the provided fence is in a signaled state.
 *
 * The graphics system must be initialized before this call.
 *
 * @param[in] fence Fence to check.
 *
 * @return True if signaled otherwise `PAL_FALSE`.
 *
 * Thread safety: Thread safe.
 *
 * @since 2.0
 * @sa palResetFence
 * @sa palWaitFence
 */
PAL_API PalBool PAL_CALL palIsFenceSignaled(PalFence* fence);

/**
 * @brief Create a semaphore.
 *
 * The graphics system must be initialized before this call.
 *
 * @param[in] device Device that creates the semaphore.
 * @param[in] enableTimeline If true, `PAL_ADAPTER_FEATURE_TIMELINE_SEMAPHORE` must be supported
 * and enabled by the device creating the semaphore.
 * @param[out] outSemaphore Pointer to a PalSemaphore to recieve the created semaphore.
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Thread safe if `device` is externally synchronized.
 *
 * @since 2.0
 * @sa palDestroySemaphore
 */
PAL_API PalResult PAL_CALL palCreateSemaphore(
    PalDevice* device,
    PalBool enableTimeline,
    PalSemaphore** outSemaphore);

/**
 * @brief Destroy a semaphore.
 *
 * The graphics system must be initialized before this call.
 * If the provided semaphore is invalid or `nullptr`, this function returns
 * silently.
 *
 * @param[in] semaphore Semaphore to destroy.
 *
 * Thread safety: Thread safe if the device used to create the semaphore is
 * externally synchronized.
 *
 * @since 2.0
 * @sa palCreateSemaphore
 */
PAL_API void PAL_CALL palDestroySemaphore(PalSemaphore* semaphore);

/**
 * @brief Waits for a semaphore to reach the provided value.
 *
 * The graphics system must be initialized before this call.
 *
 * The provided semaphore must be a timeline semaphore If not, this function fails and returns
 * `PAL_RESULT_ADAPTER_FEATURE_NOT_SUPPORTED`.
 *
 * @param[in] semaphore Semaphore to wait on.
 * @param[in] value Value to wait for.
 * @param[in] timeout Time to wait for in milliseconds. Set to `PAL_INFINITE` for indefintely.
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Thread safe if `semaphore` is externally synchronized.
 *
 * @since 2.0
 * @sa palSignalSemaphore
 * @sa palGetSemaphoreValue
 */
PAL_API PalResult PAL_CALL palWaitSemaphore(
    PalSemaphore* semaphore,
    uint64_t value,
    uint64_t timeout);

/**
 * @brief Signals a semaphore from the provided value.
 *
 * The graphics system must be initialized before this call.
 *
 * The provided semaphore must be a timeline semaphore If not, this function fails and returns
 * `PAL_RESULT_ADAPTER_FEATURE_NOT_SUPPORTED`.
 *
 * @param[in] semaphore Semaphore to signal.
 * @param[in] queue Queue used to signal the semaphore.
 * @param[in] value Value to signal.
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Thread safe if `queue` is externally synchronized.
 *
 * @since 2.0
 * @sa palWaitSemaphore
 * @sa palGetSemaphoreValue
 */
PAL_API PalResult PAL_CALL palSignalSemaphore(
    PalSemaphore* semaphore,
    PalQueue* queue,
    uint64_t value);

/**
 * @brief Get the value of a semaphore.
 *
 * The graphics system must be initialized before this call.
 *
 * The provided semaphore must be a timeline semaphore If not, this function fails and returns
 * `PAL_RESULT_ADAPTER_FEATURE_NOT_SUPPORTED`.
 *
 * @param[in] semaphore Semaphore to get its value.
 * @param[out] outValue Pointer to a uint64_t to receive the semaphore value.
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Thread safe if `semaphore` is externally synchronized.
 *
 * @since 2.0
 * @sa palWaitSemaphore
 * @sa palSignalSemaphore
 */
PAL_API PalResult PAL_CALL palGetSemaphoreValue(
    PalSemaphore* semaphore,
    uint64_t* outValue);

/**
 * @brief Create a command pool from a device.
 *
 * The graphics system must be initialized before this call.
 *
 * @param[in] device Device that creates the command pool.
 * @param[in] queue Queue the command pool buffers will be submitted to.
 * @param[out] outPool Pointer to a PalCommandPool to recieve the created command pool.
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Thread safe if `device` is externally synchronized.
 *
 * @since 2.0
 * @sa palDestroyCommandPool
 */
PAL_API PalResult PAL_CALL palCreateCommandPool(
    PalDevice* device,
    PalQueue* queue,
    PalCommandPool** outPool);

/**
 * @brief Destroy a command pool.
 *
 * The graphics system must be initialized before this call.
 * If the provided command pool is invalid or `nullptr`, this function returns
 * silently.
 *
 * Destroying a command pool frees all command buffers automatically.
 *
 * @param[in] pool Command pool to destroy.
 *
 * Thread safety: Thread safe if the device used to create the command pool is
 * externally synchronized.
 *
 * @since 2.0
 * @sa palCreateCommandPool
 */
PAL_API void PAL_CALL palDestroyCommandPool(PalCommandPool* pool);

/**
 * @brief Reset all command buffers allocated from the provided command pool.
 *
 * The graphics system must be initialized before this call.
 *
 * @param[in] pool Command pool to reset its command buffers.
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Thread safe if `pool` is externally synchronized.
 *
 * @since 2.0
 */
PAL_API PalResult PAL_CALL palResetCommandPool(PalCommandPool* pool);

/**
 * @brief Allocate a command buffer from the provided command pool.
 *
 * The graphics system must be initialized before this call.
 *
 * @param[in] device Device to allocate command buffer on.
 * @param[in] pool Command pool to allocate command buffer from.
 * @param[in] type Type of the command buffer (eg. PAL_COMMAND_BUFFER_TYPE_PRIMARY).
 * @param[out] outCmdBuffer Pointer to a PalCommandBuffer to recieve the created command buffer.
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Thread safe if `device` and `pool` are externally synchronized.
 *
 * @since 2.0
 * @sa palFreeCommandBuffer
 */
PAL_API PalResult PAL_CALL palAllocateCommandBuffer(
    PalDevice* device,
    PalCommandPool* pool,
    PalCommandBufferType type,
    PalCommandBuffer** outCmdbuffer);

/**
 * @brief Free an allocated command buffer.
 *
 * The graphics system must be initialized before this call.
 * If the provided command buffer is invalid or `nullptr`, this function returns
 * silently.
 *
 * @param[in] cmdBuffer Command buffer to free.
 *
 * Thread safety: Thread safe if the command pool used to create the command buffer is
 * externally synchronized.
 *
 * @since 2.0
 * @sa palAllocateCommandBuffer
 */
PAL_API void PAL_CALL palFreeCommandBuffer(PalCommandBuffer* cmdBuffer);

/**
 * @brief Reset the provided command buffer.
 *
 * The graphics system must be initialized before this call.
 *
 * @param[in] cmdBuffer Command buffer to reset.
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Thread safe if `cmdBuffer` is externally synchronized.
 *
 * @since 2.0
 */
PAL_API PalResult PAL_CALL palResetCommandBuffer(PalCommandBuffer* cmdBuffer);

/**
 * @brief Submit a command buffer to the provided queue for execution.
 *
 * The graphics system must be initialized before this call. The command buffer must not
 * be in a recording state.
 *
 * @param[in] queue Queue to execute the command buffer.
 * @param[in] info Pointer to a PalCommandBufferSubmitInfo struct that specifies parameters.
 * Must not be `nullptr`.
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Thread safe if `queue` is externally synchronized.
 *
 * @since 2.0
 */
PAL_API PalResult PAL_CALL palSubmitCommandBuffer(
    PalQueue* queue,
    PalCommandBufferSubmitInfo* info);

/**
 * @brief Begin recording commands to the provided command buffer.
 *
 * The graphics system must be initialized before this call. This function must be called
 * before any other `palCmd**` function is used.
 *
 * @param[in] cmdBuffer Command buffer to begin recording.
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Thread safe if `cmdBuffer` is externally synchronized.
 *
 * @since 2.0
 * @sa palCmdEnd
 */
PAL_API PalResult PAL_CALL palCmdBegin(
    PalCommandBuffer* cmdBuffer,
    PalRenderingLayoutInfo* info);

/**
 * @brief End recording commands to the provided command buffer.
 *
 * The graphics system must be initialized before this call.
 *
 * @param[in] cmdBuffer Command buffer to begin recording.
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Thread safe if `cmdBuffer` is externally synchronized.
 *
 * @since 2.0
 * @sa palCmdBegin
 */
PAL_API PalResult PAL_CALL palCmdEnd(PalCommandBuffer* cmdBuffer);

/**
 * @brief Execute a secondary command buffer within a primary command buffer.
 *
 * The graphics system must be initialized before this call. The `secondaryCmdBuffer` must
 * be created with the type `PAL_COMMAND_BUFFER_TYPE_SECONDARY`.
 *
 * @param[in] primaryCmdBuffer Primary command buffer. Must be in recording state.
 * @param[in] secondaryCmdBuffer Secondary command buffer.
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Thread safe if `primaryCmdBuffer` is externally synchronized.
 *
 * @since 2.0
 */
PAL_API PalResult PAL_CALL palCmdExecuteCommandBuffer(
    PalCommandBuffer* primaryCmdBuffer,
    PalCommandBuffer* secondaryCmdBuffer);

/**
 * @brief Set the fragment shading rate used for draw calls.
 *
 * The graphics system must be initialized before this call.
 *
 * `PAL_ADAPTER_FEATURE_FRAGMENT_SHADING_RATE` must be supported and enabled by the device if not,
 * this function will fail and return `PAL_RESULT_ADAPTER_FEATURE_NOT_SUPPORTED`.
 *
 * @param[in] cmdBuffer Command buffer being recorded.
 * @param[in] state Pointer to a PalFragmentShadingRateState struct that specifies parameters.
 * Must not be `nullptr`.
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Thread safe if `cmdBuffer` is externally synchronized.
 *
 * @since 2.0
 */
PAL_API PalResult PAL_CALL palCmdSetFragmentShadingRate(
    PalCommandBuffer* cmdBuffer,
    PalFragmentShadingRateState* state);

/**
 * @brief Dispatch mesh shader workgroups.
 *
 * The graphics system must be initialized before this call.
 *
 * `PAL_ADAPTER_FEATURE_MESH_SHADER` must be supported and enabled by the device if not, this
 * function will fail and return `PAL_RESULT_ADAPTER_FEATURE_NOT_SUPPORTED`.
 *
 * @param[in] cmdBuffer Command buffer being recorded.
 * @param[in] groupCountX Number of mesh shader groups to dispatch on the x axis.
 * @param[in] groupCountY Number of mesh shader groups to dispatch on the y axis.
 * @param[in] groupCountZ Number of mesh shader groups to dispatch on the z axis.
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Thread safe if `cmdBuffer` is externally synchronized.
 *
 * @note A pipeline must be bound before this call.
 *
 * @since 2.0
 * @sa palBuildWorkGroupInfo
 */
PAL_API PalResult PAL_CALL palCmdDrawMeshTasks(
    PalCommandBuffer* cmdBuffer,
    uint32_t groupCountX,
    uint32_t groupCountY,
    uint32_t groupCountZ);

/**
 * @brief Dispatch mesh shader workgroups using parameters from a buffer.
 *
 * The graphics system must be initialized before this call.
 *
 * `PAL_ADAPTER_FEATURE_MESH_SHADER` and `PAL_ADAPTER_FEATURE_INDIRECT_DRAW_MESH` must be supported
 * and enabled by the device if not, this function will fail and return
 * `PAL_RESULT_ADAPTER_FEATURE_NOT_SUPPORTED`.
 *
 * @param[in] cmdBuffer Command buffer being recorded.
 * @param[in] buffer Buffer containing an array of PalDispatchIndirectData structs.
 * Can be a single struct.
 * @param[in] drawCount Number of draws to perform.
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Thread safe if `cmdBuffer` is externally synchronized.
 *
 * @note A pipeline must be bound before this call.
 * @note The buffer must be created with `PAL_BUFFER_USAGE_INDIRECT` usage.
 *
 * @since 2.0
 * @sa palBuildWorkGroupInfo
 */
PAL_API PalResult PAL_CALL palCmdDrawMeshTasksIndirect(
    PalCommandBuffer* cmdBuffer,
    PalBuffer* buffer,
    uint32_t drawCount);

/**
 * @brief Dispatch mesh shader workgroups using parameters from buffers.
 *
 * The graphics system must be initialized before this call.
 *
 * `PAL_ADAPTER_FEATURE_MESH_SHADER` and `PAL_ADAPTER_FEATURE_INDIRECT_DRAW_MESH_COUNT` must be
 * supported and enabled by the device if not, this function will fail and return
 * `PAL_RESULT_ADAPTER_FEATURE_NOT_SUPPORTED`.
 *
 * @param[in] cmdBuffer Command buffer being recorded.
 * @param[in] buffer Buffer containing an array of PalDispatchIndirectData structs.
 * Can be a single struct.
 * @param[in] countBuffer Buffer containing a single `uint32_t` specifying the number of draws.
 * @param[in] maxDrawCount Maximum Number of draws to perform.
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Thread safe if `cmdBuffer` is externally synchronized.
 *
 * @note A pipeline must be bound before this call.
 * @note The buffer must be created with `PAL_BUFFER_USAGE_INDIRECT` usage.
 *
 * @since 2.0
 * @sa palBuildWorkGroupInfo
 */
PAL_API PalResult PAL_CALL palCmdDrawMeshTasksIndirectCount(
    PalCommandBuffer* cmdBuffer,
    PalBuffer* buffer,
    PalBuffer* countBuffer,
    uint32_t maxDrawCount);

/**
 * @brief Build or update an acceleration structure.
 *
 * The graphics system must be initialized before this call.
 *
 * `PAL_ADAPTER_FEATURE_RAY_TRACING` must be supported and enabled by the device if not, this
 * function will fail and return `PAL_RESULT_ADAPTER_FEATURE_NOT_SUPPORTED`.
 *
 * @param[in] cmdBuffer Command buffer being recorded.
 * @param[in] info Pointer to a PalAccelerationStructureBuildInfo struct that specifies parameters.
 * Must not be `nullptr`.
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Thread safe if `cmdBuffer` is externally synchronized.
 *
 * @since 2.0
 */
PAL_API PalResult PAL_CALL palCmdBuildAccelerationStructure(
    PalCommandBuffer* cmdBuffer,
    PalAccelerationStructureBuildInfo* info);

/**
 * @brief Begin a rendering pass.
 *
 * The graphics system must be initialized before this call.
 *
 * @param[in] cmdBuffer Command buffer being recorded.
 * @param[in] info Pointer to a PalRenderingInfo struct that specifies parameters.
 * Must not be `nullptr`.
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Thread safe if `cmdBuffer` is externally synchronized.
 *
 * @since 2.0
 */
PAL_API PalResult PAL_CALL palCmdBeginRendering(
    PalCommandBuffer* cmdBuffer,
    PalRenderingInfo* info);

/**
 * @brief End a rendering pass.
 *
 * The graphics system must be initialized before this call.
 *
 * @param[in] cmdBuffer Command buffer being recorded.
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Thread safe if `cmdBuffer` is externally synchronized.
 *
 * @since 2.0
 */
PAL_API PalResult PAL_CALL palCmdEndRendering(PalCommandBuffer* cmdBuffer);

/**
 * @brief Copy data from one buffer to the other.
 *
 * The graphics system must be initialized before this call.
 *
 * @param[in] cmdBuffer Command buffer being recorded.
 * @param[in] dst Destination buffer.
 * @param[in] src Source buffer.
 * @param[in] copyInfo Pointer to a PalBufferCopyInfo struct that specifies parameters.
 * Must not be `nullptr`.
 *
 * Pointer to a PalImageCreateInfo struct that specifies parameters.
 * Must not be `nullptr`.
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Thread safe if `cmdBuffer` is externally synchronized.
 *
 * @since 2.0
 */
PAL_API PalResult PAL_CALL palCmdCopyBuffer(
    PalCommandBuffer* cmdBuffer,
    PalBuffer* dst,
    PalBuffer* src,
    PalBufferCopyInfo* copyInfo);

/**
 * @brief Copy data from a buffer to an image.
 *
 * The graphics system must be initialized before this call.
 *
 * @param[in] cmdBuffer Command buffer being recorded.
 * @param[in] dstImage Destination image.
 * @param[in] srcBuffer Source buffer.
 * @param[in] copyInfo Pointer to a PalBufferImageCopyInfo struct that specifies parameters.
 * Must not be `nullptr`.
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Thread safe if `cmdBuffer` is externally synchronized.
 *
 * @since 2.0
 */
PAL_API PalResult PAL_CALL palCmdCopyBufferToImage(
    PalCommandBuffer* cmdBuffer,
    PalImage* dstImage,
    PalBuffer* srcBuffer,
    PalBufferImageCopyInfo* copyInfo);

/**
 * @brief Copy data from one image to the other.
 *
 * The graphics system must be initialized before this call.
 *
 * @param[in] cmdBuffer Command buffer being recorded.
 * @param[in] dst Destination image.
 * @param[in] src Source image.
 * @param[in] copyInfo Pointer to a PalImageCopyInfo struct that specifies parameters.
 * Must not be `nullptr`.
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Thread safe if `cmdBuffer` is externally synchronized.
 *
 * @since 2.0
 */
PAL_API PalResult PAL_CALL palCmdCopyImage(
    PalCommandBuffer* cmdBuffer,
    PalImage* dst,
    PalImage* src,
    PalImageCopyInfo* copyInfo);

/**
 * @brief Copy data from an image to a buffer.
 *
 * The graphics system must be initialized before this call.
 *
 * @param[in] cmdBuffer Command buffer being recorded.
 * @param[in] dstBuffer Destination buffer.
 * @param[in] srcImage Source image.
 * @param[in] copyInfo Pointer to a PalBufferImageCopyInfo struct that specifies parameters.
 * Must not be `nullptr`.
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Thread safe if `cmdBuffer` is externally synchronized.
 *
 * @since 2.0
 */
PAL_API PalResult PAL_CALL palCmdCopyImageToBuffer(
    PalCommandBuffer* cmdBuffer,
    PalBuffer* dstBuffer,
    PalImage* srcImage,
    PalBufferImageCopyInfo* copyInfo);

/**
 * @brief Bind a pipeline.
 *
 * The graphics system must be initialized before this call. Every pipeline knows it types which is
 * set at the respective creation functions. (`palCreate**Graphics/Compute/RayTracing**Pipeline`).
 *
 * @param[in] cmdBuffer Command buffer being recorded.
 * @param[in] pipeline Pipeline to bind.
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Thread safe if `cmdBuffer` is externally synchronized.
 *
 * @since 2.0
 */
PAL_API PalResult PAL_CALL palCmdBindPipeline(
    PalCommandBuffer* cmdBuffer,
    PalPipeline* pipeline);

/**
 * @brief Set the viewport(s) used in draw commands.
 *
 * The graphics system must be initialized before this call. This always overwrites any previous
 * viewports that were set since the first viewport index is always 0.
 *
 * @param[in] cmdBuffer Command buffer being recorded.
 * @param[in] count Capacity of the PalViewport array.
 * @param[in] viewports Pointer to an array of viewports.
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Thread safe if `cmdBuffer` is externally synchronized.
 *
 * @since 2.0
 */
PAL_API PalResult PAL_CALL palCmdSetViewport(
    PalCommandBuffer* cmdBuffer,
    uint32_t count,
    PalViewport* viewports);

/**
 * @brief Set the scissor(s) used in draw commands.
 *
 * The graphics system must be initialized before this call. This always overwrites any previous
 * scissors that were set since the first scissor index is always 0.
 *
 * @param[in] cmdBuffer Command buffer being recorded.
 * @param[in] count Capacity of the PalRect2D array.
 * @param[in] scissors Pointer to an array of scissors.
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Thread safe if `cmdBuffer` is externally synchronized.
 *
 * @since 2.0
 */
PAL_API PalResult PAL_CALL palCmdSetScissors(
    PalCommandBuffer* cmdBuffer,
    uint32_t count,
    PalRect2D* scissors);

/**
 * @brief Bind vertex buffer(s) used in draw commands.
 *
 * The graphics system must be initialized before this call.
 *
 * @param[in] cmdBuffer Command buffer being recorded.
 * @param[in] firstSlot Index of the first vertex buffer binding slot.
 * @param[in] count Number of vertex buffers to bind.
 * @param[in] buffers Pointer to an array of vertex buffers.
 * @param[in] offsets Pointer to an array of offsets in bytes into each vertex buffer.
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Thread safe if `cmdBuffer` is externally synchronized.
 *
 * @note A pipeline must be bound before this call.
 *
 * @since 2.0
 */
PAL_API PalResult PAL_CALL palCmdBindVertexBuffers(
    PalCommandBuffer* cmdBuffer,
    uint32_t firstSlot,
    uint32_t count,
    PalBuffer** buffers,
    uint64_t* offsets);

/**
 * @brief Bind index buffer used in draw commands.
 *
 * The graphics system must be initialized before this call.
 *
 * @param[in] cmdBuffer Command buffer being recorded.
 * @param[in] buffer Index buffer to bind.
 * @param[in] offset Offset in bytes into the index buffer.
 * @param[in] type Type of indices stored in the index buffer. (eg. `PAL_INDEX_TYPE_UINT32`).
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Thread safe if `cmdBuffer` is externally synchronized.
 *
 * @note A pipeline must be bound before this call.
 *
 * @since 2.0
 */
PAL_API PalResult PAL_CALL palCmdBindIndexBuffer(
    PalCommandBuffer* cmdBuffer,
    PalBuffer* buffer,
    uint64_t offset,
    PalIndexType type);

/**
 * @brief Issue a non-indexed draw command.
 *
 * The graphics system must be initialized before this call.
 *
 * @param[in] cmdBuffer Command buffer being recorded.
 * @param[in] vertexCount Number of vertices to draw.
 * @param[in] instanceCount Number of instances to draw.
 * @param[in] firstVertex Index of the first vertex to draw.
 * @param[in] firstInstance Index of the first instance to draw.
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Thread safe if `cmdBuffer` is externally synchronized.
 *
 * @note A pipeline must be bound before this call.
 *
 * @since 2.0
 * @sa palDrawIndexed
 */
PAL_API PalResult PAL_CALL palCmdDraw(
    PalCommandBuffer* cmdBuffer,
    uint32_t vertexCount,
    uint32_t instanceCount,
    uint32_t firstVertex,
    uint32_t firstInstance);

/**
 * @brief Issue a non-indexed draw command using buffers.
 *
 * The graphics system must be initialized before this call.
 *
 * `PAL_ADAPTER_FEATURE_INDIRECT_DRAW` must be supported and enabled by the device if not, this
 * function will fail and return `PAL_RESULT_ADAPTER_FEATURE_NOT_SUPPORTED`.
 *
 * @param[in] cmdBuffer Command buffer being recorded.
 * @param[in] buffer Buffer containing an array of PalDrawIndirectData structs.
 * Can be a single struct.
 * @param[in] count Number of draws to perform.
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Thread safe if `cmdBuffer` is externally synchronized.
 *
 * @note A pipeline must be bound before this call.
 * @note The buffer must be created with `PAL_BUFFER_USAGE_INDIRECT` usage.
 *
 * @since 2.0
 * @sa palDrawIndexedIndirect
 */
PAL_API PalResult PAL_CALL palCmdDrawIndirect(
    PalCommandBuffer* cmdBuffer,
    PalBuffer* buffer,
    uint32_t count);

/**
 * @brief Issue a non-indexed draw command using buffers.
 *
 * The graphics system must be initialized before this call.
 *
 * `PAL_ADAPTER_FEATURE_INDIRECT_DRAW_COUNT` must be supported and enabled by the device if not,
 * this function will fail and return `PAL_RESULT_ADAPTER_FEATURE_NOT_SUPPORTED`.
 *
 * @param[in] cmdBuffer Command buffer being recorded.
 * @param[in] buffer Buffer containing an array of PalDrawIndirectData structs.
 * Can be a single struct.
 * @param[in] countBuffer Buffer containing a single `uint32_t` specifying the number of draws.
 * @param[in] maxDrawCount Maximum Number of draws to perform.
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Thread safe if `cmdBuffer` is externally synchronized.
 *
 * @note A pipeline must be bound before this call.
 * @note The buffer must be created with `PAL_BUFFER_USAGE_INDIRECT` usage.
 *
 * @since 2.0
 * @sa palCmdDrawIndexedIndirectCount
 */
PAL_API PalResult PAL_CALL palCmdDrawIndirectCount(
    PalCommandBuffer* cmdBuffer,
    PalBuffer* buffer,
    PalBuffer* countBuffer,
    uint32_t maxDrawCount);

/**
 * @brief Issue an indexed draw command.
 *
 * The graphics system must be initialized before this call.
 *
 * @param[in] cmdBuffer Command buffer being recorded.
 * @param[in] indexCount Number of indices to draw.
 * @param[in] instanceCount Number of instances to draw.
 * @param[in] firstIndex Index of the first index to draw.
 * @param[in] vertexOffset Added offset to vertex indices.
 * @param[in] firstInstance Index of the first instance to draw.
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Thread safe if `cmdBuffer` is externally synchronized.
 *
 * @note A pipeline must be bound before this call.
 *
 * @since 2.0
 * @sa palDraw
 */
PAL_API PalResult PAL_CALL palCmdDrawIndexed(
    PalCommandBuffer* cmdBuffer,
    uint32_t indexCount,
    uint32_t instanceCount,
    uint32_t firstIndex,
    int32_t vertexOffset,
    uint32_t firstInstance);

/**
 * @brief Issue an indexed draw command using buffers.
 *
 * The graphics system must be initialized before this call.
 *
 * `PAL_ADAPTER_FEATURE_INDIRECT_DRAW` must be supported and enabled by the device if not, this
 * function will fail and return `PAL_RESULT_ADAPTER_FEATURE_NOT_SUPPORTED`.
 *
 * @param[in] cmdBuffer Command buffer being recorded.
 * @param[in] buffer Buffer containing an array of PalDrawIndexedIndirectData structs.
 * Can be a single struct.
 * @param[in] count Number of draws to perform.
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Thread safe if `cmdBuffer` is externally synchronized.
 *
 * @note A pipeline must be bound before this call.
 * @note The buffer must be created with `PAL_BUFFER_USAGE_INDIRECT` usage.
 *
 * @since 2.0
 * @sa palDrawIndirect
 */
PAL_API PalResult PAL_CALL palCmdDrawIndexedIndirect(
    PalCommandBuffer* cmdBuffer,
    PalBuffer* buffer,
    uint32_t count);

/**
 * @brief Issue an indexed draw command using buffers.
 *
 * The graphics system must be initialized before this call.
 *
 * `PAL_ADAPTER_FEATURE_INDIRECT_DRAW_COUNT` must be supported and enabled by the device if not,
 * this function will fail and return `PAL_RESULT_ADAPTER_FEATURE_NOT_SUPPORTED`.
 *
 * @param[in] cmdBuffer Command buffer being recorded.
 * @param[in] buffer Buffer containing an array of PalDrawIndexedIndirectData structs.
 * Can be a single struct.
 * @param[in] countBuffer Buffer containing a single `uint32_t` specifying the number of draws.
 * @param[in] maxDrawCount Maximum Number of draws to perform.
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Thread safe if `cmdBuffer` is externally synchronized.
 *
 * @note A pipeline must be bound before this call.
 * @note The buffer must be created with `PAL_BUFFER_USAGE_INDIRECT` usage.
 *
 * @since 2.0
 * @sa palCmdDrawIndirectCount
 */
PAL_API PalResult PAL_CALL palCmdDrawIndexedIndirectCount(
    PalCommandBuffer* cmdBuffer,
    PalBuffer* buffer,
    PalBuffer* countBuffer,
    uint32_t maxDrawCount);

/**
 * @brief Transition an acceleration structure from one usage state to another.
 *
 * `PAL_ADAPTER_FEATURE_RAY_TRACING` must be supported and enabled by the device if not,
 * this function will fail and return `PAL_RESULT_ADAPTER_FEATURE_NOT_SUPPORTED`.
 *
 * The graphics system must be initialized before this call. This function defines a
 * dependency between `oldUsageState` and `newUsageState`. It ensures that all
 * operations performed under `oldUsageState` are completed and visible before the acceleration
 * structure is accessed under `newUsageState`.
 *
 * This function does not modify the acceleration structure, it only exforces execution ordering
 * and acceleration structure memory visibility.
 *
 * A barrier is not needed between BLAS and TLAS if there dont shared any resource. If both
 * builds use a different scratch buffer, no barrier is needed.
 *
 * Example:
 *
 * To make sure BLAS builds before TLAS access it and TLAS does not use scratch buffer
 * whilst BLAS is buidling,
 * we put a barrier to transition the BLAS to ensure it has finished building and the scratch
 * buffer is not being used. This is expressed with `oldUsageState` being
 * `PAL_USAGE_STATE_ACCELERATION_STRUCTURE_WRITE`.
 *
 * `newUsageState` should be the new usage state we want after the BLAS
 * has finished building which is `PAL_USAGE_STATE_ACCELERATION_STRUCTURE_READ`.
 *
 * @param[in] cmdBuffer Command buffer being recorded.
 * @param[in] as Acceleration structure to set barrier on.
 * @param[in] oldUsageState The old usage state.
 * @param[in] newUsageState The new usage state.
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Thread safe if `cmdBuffer` is externally synchronized.
 *
 * @note A pipeline must be bound before this call.
 *
 * @since 2.0
 * @sa palCmdImageBarrier
 * @sa palCmdBufferBarrier
 */
PAL_API PalResult PAL_CALL palCmdAccelerationStructureBarrier(
    PalCommandBuffer* cmdBuffer,
    PalAccelerationStructure* as,
    PalUsageState oldUsageState,
    PalUsageState newUsageState);

/**
 * @brief Transition an image from one usage state to another.
 *
 * The graphics system must be initialized before this call. This function defines a
 * dependency between `oldUsageState` and `newUsageState`. It ensures that all
 * operations performed under `oldUsageState` are completed and visible before the image
 * is accessed under `newUsageState`.
 *
 * This function does not modify the image, it only exforces execution ordering and image memory
 * visibility.
 *
 * Example:
 *
 * To make sure the an image is ready for presenting after a render pass,
 * we put a barrier to transition the image to ensure the render pass has finished writing
 * to the image. This is expressed with `oldUsageState` being
 * `PAL_USAGE_STATE_COLOR_ATTACHMENT`.
 *
 * `newUsageState` should be the new usage state we want after the render pass
 * has finished which is `PAL_USAGE_STATE_PRESENT`.
 *
 * @param[in] cmdBuffer Command buffer being recorded.
 * @param[in] image Image to set barrier on.
 * @param[in] subresourceRange Subresource range of the image.
 * @param[in] oldUsageStateInfo The old usage state.
 * @param[in] newUsageStateInfo The new usage state.
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Thread safe if `cmdBuffer` is externally synchronized.
 *
 * @since 2.0
 * @sa palCmdAccelerationStructureBarrier
 * @sa palCmdBufferBarrier
 */
PAL_API PalResult PAL_CALL palCmdImageBarrier(
    PalCommandBuffer* cmdBuffer,
    PalImage* image,
    PalImageSubresourceRange* subresourceRange,
    PalUsageState oldUsageState,
    PalUsageState newUsageState);

/**
 * @brief Transition a buffer from one usage state to another.
 *
 * The graphics system must be initialized before this call. This function defines a
 * dependency between `oldUsageState` and `newUsageState`. It ensures that all
 * operations performed under `oldUsageState` are completed and visible before the buffer
 * is accessed under `newUsageState`.
 *
 * This function does not modify the buffer, it only exforces execution ordering and buffer memory
 * visibility.
 *
 * Example:
 *
 * To read back data from a buffer that will be written to by a shader,
 * we put a barrier to transition the buffer to ensurethe shader has finished writing to the
 * buffer. This is expressed with `oldUsageState` being `PAL_USAGE_STATE_SHADER_WRITE`.
 *
 * `newUsageState` should be the new usage state we want after the write
 * has finished which is`PAL_USAGE_STATE_TRANSFER_READ`.
 *
 * @param[in] cmdBuffer Command buffer being recorded.
 * @param[in] buffer Buffer to set barrier on.
 * @param[in] oldUsageStateInfo The old usage state.
 * @param[in] newUsageStateInfo The new usage state.
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Thread safe if `cmdBuffer` is externally synchronized.
 *
 * @since 2.0
 * @sa palCmdAccelerationStructureBarrier
 * @sa palCmdImageBarrier
 */
PAL_API PalResult PAL_CALL palCmdBufferBarrier(
    PalCommandBuffer* cmdBuffer,
    PalBuffer* buffer,
    PalUsageState oldUsageState,
    PalUsageState newUsageState);

/**
 * @brief Dispatch compute shader workgroups.
 *
 * The graphics system must be initialized before this call.
 *
 * @param[in] cmdBuffer Command buffer being recorded.
 * @param[in] groupCountX Number of compute shader groups to dispatch on the x axis.
 * @param[in] groupCountY Number of compute shader groups to dispatch on the y axis.
 * @param[in] groupCountZ Number of compute shader groups to dispatch on the z axis.
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Thread safe if `cmdBuffer` is externally synchronized.
 *
 * @note A pipeline must be bound before this call.
 *
 * @since 2.0
 * @sa palBuildWorkGroupInfo
 */
PAL_API PalResult PAL_CALL palCmdDispatch(
    PalCommandBuffer* cmdBuffer,
    uint32_t groupCountX,
    uint32_t groupCountY,
    uint32_t groupCountZ);

/**
 * @brief Dispatch compute shader workgroups with base offset.
 *
 * The graphics system must be initialized before this call.
 *
 * `PAL_ADAPTER_FEATURE_DISPATCH_BASE` must be supported and enabled by the device if not, this
 * function will fail and return `PAL_RESULT_ADAPTER_FEATURE_NOT_SUPPORTED`.
 *
 * @param[in] cmdBuffer Command buffer being recorded.
 * @param[in] baseGroupX Base group offset on the x axis.
 * @param[in] baseGroupY Base group offset on the y axis.
 * @param[in] baseGroupZ Base group offset on the z axis.
 * @param[in] groupCountX Number of compute shader groups to dispatch on the x axis.
 * @param[in] groupCountY Number of compute shader groups to dispatch on the y axis.
 * @param[in] groupCountZ Number of compute shader groups to dispatch on the z axis.
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Thread safe if `cmdBuffer` is externally synchronized.
 *
 * @note A pipeline must be bound before this call.
 *
 * @since 2.0
 * @sa palBuildWorkGroupInfo
 */
PAL_API PalResult PAL_CALL palCmdDispatchBase(
    PalCommandBuffer* cmdBuffer,
    uint32_t baseGroupX,
    uint32_t baseGroupY,
    uint32_t baseGroupZ,
    uint32_t groupCountX,
    uint32_t groupCountY,
    uint32_t groupCountZ);

/**
 * @brief Dispatch compute shader workgroups using parameters from a buffer.
 *
 * The graphics system must be initialized before this call.
 *
 * `PAL_ADAPTER_FEATURE_INDIRECT_DISPATCH` must be supported and enabled by the device if not, this
 * function will fail and return `PAL_RESULT_ADAPTER_FEATURE_NOT_SUPPORTED`.
 *
 * @param[in] cmdBuffer Command buffer being recorded.
 * @param[in] buffer Buffer containing the PalDispatchIndirectData struct. Must not be `nullptr`.
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Thread safe if `cmdBuffer` is externally synchronized.
 *
 * @note A pipeline must be bound before this call.
 * @note The buffer must be created with `PAL_BUFFER_USAGE_INDIRECT` usage.
 *
 * @since 2.0
 * @sa palBuildWorkGroupInfo
 */
PAL_API PalResult PAL_CALL palCmdDispatchIndirect(
    PalCommandBuffer* cmdBuffer,
    PalBuffer* buffer);

/**
 * @brief Dispatch rays.
 *
 * The graphics system must be initialized before this call.
 *
 * `PAL_ADAPTER_FEATURE_RAY_TRACING` must be supported and enabled by the device if not,
 * this function will fail and return `PAL_RESULT_ADAPTER_FEATURE_NOT_SUPPORTED`.
 *
 * @param[in] cmdBuffer Command buffer being recorded.
 * @param[in] sbt The shader binding table to use.
 * @param[in] raygenIndex Index of the raygen shader to execute.
 * @param[in] width Number of rays to trace on the x axis.
 * @param[in] height Number of rays to trace on the y axis.
 * @param[in] depth Number of rays to trace on the z axis.
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Thread safe if `cmdBuffer` is externally synchronized.
 *
 * @note A pipeline must be bound before this call.
 *
 * @since 2.0
 */
PAL_API PalResult PAL_CALL palCmdTraceRays(
    PalCommandBuffer* cmdBuffer,
    PalShaderBindingTable* sbt,
    uint32_t raygenIndex,
    uint32_t width,
    uint32_t height,
    uint32_t depth);

/**
 * @brief Dispatch rays using parameters from a buffer.
 *
 * The graphics system must be initialized before this call.
 *
 * `PAL_ADAPTER_FEATURE_INDIRECT_RAY_TRACING` must be supported and enabled by the device if not,
 * this function will fail and return `PAL_RESULT_ADAPTER_FEATURE_NOT_SUPPORTED`.
 *
 * @param[in] cmdBuffer Command buffer being recorded.
 * @param[in] raygenIndex Index of the raygen shader to execute.
 * @param[in] sbt The shader binding table to use.
 * @param[in] buffer Buffer containing the PalDispatchIndirectData struct. Must not be `nullptr`.
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Thread safe if `cmdBuffer` is externally synchronized.
 *
 * @note The argument buffer memory must not be `PAL_MEMORY_TYPE_GPU_ONLY`. The implementation
 * internally copies the data into a GPU buffer for execution.
 *
 * @note A pipeline must be bound before this call.
 * @note The buffer must be created with `PAL_BUFFER_USAGE_INDIRECT` usage.
 *
 * @since 2.0
 */
PAL_API PalResult PAL_CALL palCmdTraceRaysIndirect(
    PalCommandBuffer* cmdBuffer,
    uint32_t raygenIndex,
    PalShaderBindingTable* sbt,
    PalBuffer* buffer);

/**
 * @brief Bind a descriptor set to the provided command buffer.
 *
 * The graphics system must be initialized before this call.
 *
 * @param[in] cmdBuffer Command buffer being recorded.
 * @param[in] setIndex Index of the descriptor set to bind.
 * @param[in] set Descriptor set to bind. Must be compatible with `layout`.
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Thread safe if `cmdBuffer` is externally synchronized.
 *
 * @note A pipeline must be bound before this call.
 *
 * @since 2.0
 */
PAL_API PalResult PAL_CALL palCmdBindDescriptorSet(
    PalCommandBuffer* cmdBuffer,
    uint32_t setIndex,
    PalDescriptorSet* set);

/**
 * @brief Update push constant data for the provided command buffer.
 *
 * The graphics system must be initialized before this call.
 *
 * @param[in] cmdBuffer Command buffer being recorded.
 * @param[in] offset Offset in bytes into the push constant range.
 * @param[in] size Size of `value` in bytes.
 * @param[in] value Pointer to the push constant range data to write.
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Thread safe if `cmdBuffer` is externally synchronized.
 *
 * @note A pipeline must be bound before this call.
 *
 * @since 2.0
 */
PAL_API PalResult PAL_CALL palCmdPushConstants(
    PalCommandBuffer* cmdBuffer,
    uint32_t offset,
    uint32_t size,
    const void* value);

/**
 * @brief Set the cull mode for the provided command buffer.
 *
 * The graphics system must be initialized before this call.
 *
 * `PAL_ADAPTER_FEATURE_DYNAMIC_CULL_MODE` must be supported and enabled by the device if not,
 * this function will fail and return `PAL_RESULT_ADAPTER_FEATURE_NOT_SUPPORTED`.
 *
 * @param[in] cmdBuffer Command buffer being recorded.
 * @param[in] cullMode Cull mode to set.
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Thread safe if `cmdBuffer` is externally synchronized.
 *
 * @since 2.0
 */
PAL_API PalResult PAL_CALL palCmdSetCullMode(
    PalCommandBuffer* cmdBuffer,
    PalCullMode cullMode);

/**
 * @brief Set the front face for the provided command buffer.
 *
 * The graphics system must be initialized before this call.
 *
 * `PAL_ADAPTER_FEATURE_DYNAMIC_FRONT_FACE` must be supported and enabled by the device if not,
 * this function will fail and return `PAL_RESULT_ADAPTER_FEATURE_NOT_SUPPORTED`.
 *
 * @param[in] cmdBuffer Command buffer being recorded.
 * @param[in] frontFace Front face to set.
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Thread safe if `cmdBuffer` is externally synchronized.
 *
 * @since 2.0
 */
PAL_API PalResult PAL_CALL palCmdSetFrontFace(
    PalCommandBuffer* cmdBuffer,
    PalFrontFace frontFace);

/**
 * @brief Set the primitive topology for the provided command buffer.
 *
 * The graphics system must be initialized before this call.
 *
 * `PAL_ADAPTER_FEATURE_DYNAMIC_PRIMITIVE_TOPOLOGY` must be supported and enabled by the device
 * if not, this function will fail and return `PAL_RESULT_ADAPTER_FEATURE_NOT_SUPPORTED`.
 *
 * @param[in] cmdBuffer Command buffer being recorded.
 * @param[in] topology Topology to set.
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Thread safe if `cmdBuffer` is externally synchronized.
 *
 * @since 2.0
 */
PAL_API PalResult PAL_CALL palCmdSetPrimitiveTopology(
    PalCommandBuffer* cmdBuffer,
    PalPrimitiveTopology topology);

/**
 * @brief Set depth test enable for the provided command buffer.
 *
 * The graphics system must be initialized before this call.
 *
 * `PAL_ADAPTER_FEATURE_DYNAMIC_DEPTH_TEST_ENABLE` must be supported and enabled by the device
 * if not, this function will fail and return `PAL_RESULT_ADAPTER_FEATURE_NOT_SUPPORTED`.
 *
 * @param[in] cmdBuffer Command buffer being recorded.
 * @param[in] enable True to enable.
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Thread safe if `cmdBuffer` is externally synchronized.
 *
 * @since 2.0
 */
PAL_API PalResult PAL_CALL palCmdSetDepthTestEnable(
    PalCommandBuffer* cmdBuffer,
    PalBool enable);

/**
 * @brief Set depth write enable for the provided command buffer.
 *
 * The graphics system must be initialized before this call.
 *
 * `PAL_ADAPTER_FEATURE_DYNAMIC_DEPTH_WRITE_ENABLE` must be supported and enabled by the device
 * if not, this function will fail and return `PAL_RESULT_ADAPTER_FEATURE_NOT_SUPPORTED`.
 *
 * @param[in] cmdBuffer Command buffer being recorded.
 * @param[in] enable True to enable.
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Thread safe if `cmdBuffer` is externally synchronized.
 *
 * @since 2.0
 */
PAL_API PalResult PAL_CALL palCmdSetDepthWriteEnable(
    PalCommandBuffer* cmdBuffer,
    PalBool enable);

/**
 * @brief Set depth stencil operation for the provided command buffer.
 *
 * The graphics system must be initialized before this call.
 *
 * `PAL_ADAPTER_FEATURE_DYNAMIC_STENCIL_OP` must be supported and enabled by the device
 * if not, this function will fail and return `PAL_RESULT_ADAPTER_FEATURE_NOT_SUPPORTED`.
 *
 * @param[in] cmdBuffer Command buffer being recorded.
 * @param[in] faceMask Bitmask specifying faces to apply the stencil to.
 * @param[in] failOp Stencil operation to perform when stencil fails.
 * @param[in] passOp Stencil operation to perform when stencil and depth passes.
 * @param[in] depthFailOp Stencil operation to perform when stencil passes but depth fails.
 * @param[in] compareOp Compare operation for stencil tests.
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Thread safe if `cmdBuffer` is externally synchronized.
 *
 * @since 2.0
 */
PAL_API PalResult PAL_CALL palCmdSetStencilOp(
    PalCommandBuffer* cmdBuffer,
    PalStencilFaceFlags faceMask,
    PalStencilOp failOp,
    PalStencilOp passOp,
    PalStencilOp depthFailOp,
    PalCompareOp compareOp);

/**
 * @brief Create an acceleration structure.
 *
 * The graphics system must be initialized before this call.
 *
 * `PAL_ADAPTER_FEATURE_RAY_TRACING` must be supported and enabled by the device if not, this
 * function will fail and return `PAL_RESULT_ADAPTER_FEATURE_NOT_SUPPORTED`.
 *
 * @param[in] device Device that creates the acceleration structure.
 * @param[in] info Pointer to a PalAccelerationStructureCreateInfo struct that specifies parameters.
 * Must not be `nullptr`.
 * @param[out] outAs Pointer to a PalAccelerationStructure to recieve the created acceleration
 * structure.
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Thread safe if `device` is externally synchronized.
 *
 * @note The memory associated with PalAccelerationStructureCreateInfo::buffer must be
 * `PAL_MEMORY_TYPE_GPU_ONLY`.
 *
 * @since 2.0
 * @sa palDestroyAccelerationstructure
 */
PAL_API PalResult PAL_CALL palCreateAccelerationstructure(
    PalDevice* device,
    const PalAccelerationStructureCreateInfo* info,
    PalAccelerationStructure** outAs);

/**
 * @brief Destroy an acceleration structure.
 *
 * The graphics system must be initialized before this call.
 * If the provided acceleration structure is invalid or `nullptr`, this function returns
 * silently.
 *
 * @param[in] as Acceleration structure to destroy.
 *
 * Thread safety: Thread safe if the device used to create the acceleration structure is
 * externally synchronized.
 *
 * @since 2.0
 * @sa palCreateAccelerationstructure
 */
PAL_API void PAL_CALL palDestroyAccelerationstructure(PalAccelerationStructure* as);

/**
 * @brief Get the build size of an acceleration structure.
 *
 * The graphics system must be initialized before this call.
 * PalAccelerationStructureBuildInfo::dst, PalAccelerationStructureBuildInfo::scratchBufferAddress
 * and PalAccelerationStructureBuildInfo::src must be set to `nullptr`.
 *
 * `PAL_ADAPTER_FEATURE_RAY_TRACING` must be supported and enabled by the device if not, this
 * function will fail and return `PAL_RESULT_ADAPTER_FEATURE_NOT_SUPPORTED`.
 *
 * @param[in] device Device to query.
 * @param[in] info Pointer to a PalAccelerationStructureBuildInfo struct that specifies parameters.
 * Must not be `nullptr`.
 * @param[out] size Pointer to a PalAccelerationStructureBuildSize to recieve the build size.
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Thread safe if `cmdBuffer` is externally synchronized.
 *
 * @since 2.0
 */
PAL_API PalResult PAL_CALL palGetAccelerationStructureBuildSize(
    PalDevice* device,
    PalAccelerationStructureBuildInfo* info,
    PalAccelerationStructureBuildSize* size);

/**
 * @brief Create a buffer.
 *
 * The graphics system must be initialized before this call.
 *
 * `PAL_ADAPTER_FEATURE_BUFFER_DEVICE_ADDRESS` must be supported and enabled by the device if
 * `PAL_BUFFER_USAGE_DEVICE_ADDRESS` will be used.
 *
 * `PAL_ADAPTER_FEATURE_RAY_TRACING` must be supported and enabled by the device if
 * `PAL_BUFFER_USAGE_ACCELERATION_STRUCTURE` or `PAL_BUFFER_USAGE_ACCELERATION_STRUCTURE_SCRATCH`
 * or `PAL_BUFFER_USAGE_ACCELERATION_STRUCTURE_READ_ONLY_INPUT` will be used.
 *
 * `PAL_BUFFER_USAGE_INDIRECT` must be supported and enabled by the device if the buffer will be
 * used as an indirect buffer.
 *
 * @param[in] device Device that creates the buffer.
 * @param[in] info Pointer to a PalBufferCreateInfo struct that specifies parameters.
 * Must not be `nullptr`.
 * @param[out] outBuffer Pointer to a PalBuffer to recieve the created buffer.
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Thread safe if `device` is externally synchronized.
 *
 * @since 2.0
 * @sa palDestroyBuffer
 */
PAL_API PalResult PAL_CALL palCreateBuffer(
    PalDevice* device,
    const PalBufferCreateInfo* info,
    PalBuffer** outBuffer);

/**
 * @brief Destroy a buffer.
 *
 * The graphics system must be initialized before this call.
 * If the provided buffer is invalid or `nullptr`, this function returns
 * silently.
 *
 * @param[in] buffer buffer to destroy.
 *
 * Thread safety: Thread safe if the device used to create the buffer is
 * externally synchronized.
 *
 * @since 2.0
 * @sa palCreateBuffer
 */
PAL_API void PAL_CALL palDestroyBuffer(PalBuffer* buffer);

/**
 * @brief Get memory requirements for the provided buffer.
 *
 * The graphics system must be initialized before this call.
 *
 * @param[in] buffer Buffer to query memory requirements on.
 * @param[out] requirements Pointer to a PalMemoryRequirements to fill.
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Thread safe if `requirements` is per thread.
 *
 * @since 2.0
 */
PAL_API PalResult PAL_CALL palGetBufferMemoryRequirements(
    PalBuffer* buffer,
    PalMemoryRequirements* requirements);

/**
 * @brief Compute size and alignment requirements for an instance buffer.
 *
 * The graphics system must be initialized before this call. This does not allocate memory
 * for the buffer.
 *
 * `outSize` must be the size that is used to create the instance buffer. It will be
 * computed with regards to the provided `instanceCount`. This function must be used and required
 * for all instance buffers. This is used with acceleration
 * structure (`TLAS`).
 *
 * @param[in] device Device to compute instance buffer requirements with.
 * @param[in] instanceCount Number of instances the instance buffer will hold.
 * @param[out] outSize Pointer to a uint64_t to recieve the required size.
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Thread safe if `device` is externally synchronized.
 *
 * @since 2.0
 */
PAL_API PalResult PAL_CALL palComputeInstanceBufferRequirements(
    PalDevice* device,
    uint32_t instanceCount,
    uint64_t* outSize);

/**
 * @brief Compute size and alignment requirements for an image copy staging buffer.
 *
 * The graphics system must be initialized before this call. This does not allocate memory
 * for the buffer.
 *
 * `outSize` must be the size that is used to create the image copy staging buffer. It will be
 * computed with regards to the provided `imageFormat` and `copyInfo`. This function must be
 * used and required for all image copy staging buffers. This is used with image copy commands.
 *
 * PalBufferImageCopyInfo::bufferRowLength and PalBufferImageCopyInfo::bufferImageHeight are hints.
 * The driver might used it defaults if the requested is not supported. Check `outBufferRowLength`
 * and `outBufferImageHeight` to see the values the driver used. Set the new values to
 * `copyInfo` before writing to the buffer with `palWriteToImageCopyStagingBuffer()`.
 *
 * @param[in] device Device to compute image copy staging buffer requirements with.
 * @param[in] imageFormat Destination image format.
 * @param[in] copyInfo Pointer to a PalBufferImageCopyInfo struct that specifies parameters.
 * Must not be `nullptr`.
 * @param[out] outBufferRowLength Pointer to a uint32_t to recieve the required buffer row length.
 * @param[out] outBufferImageHeight Pointer to a uint32_t to recieve the required buffer imag
 * height.
 * @param[out] outSize Pointer to a uint64_t to recieve the required size.
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Thread safe if `device` is externally synchronized.
 *
 * @since 2.0
 */
PAL_API PalResult PAL_CALL palComputeImageCopyStagingBufferRequirements(
    PalDevice* device,
    PalFormat imageFormat,
    PalBufferImageCopyInfo* copyInfo,
    uint32_t* outBufferRowLength,
    uint32_t* outBufferImageHeight,
    uint64_t* outSize);

/**
 * @brief Update or write data to an instance buffer.
 *
 * The graphics system must be initialized before this call.
 *
 * @param[in] device The device. Must match the one used to create the instance buffer.
 * @param[out] ptr Pointer to the CPU visible memory. Must be mapped.
 * @param[in] instances Array of PalAccelerationStructureInstance struct to write.
 * Can be a single struct.
 * @param[in] instanceCount Number of instances in `instances`.
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Thread safe if `device` is externally synchronized.
 *
 * @since 2.0
 */
PAL_API PalResult PAL_CALL palWriteToInstanceBuffer(
    PalDevice* device,
    void* ptr,
    PalAccelerationStructureInstance* instances,
    uint32_t instanceCount);

/**
 * @brief Update or write data to an image copy staging buffer.
 *
 * The graphics system must be initialized before this call.
 *
 * @param[in] device The device. Must match the one used to create the image copy staging buffer.
 * @param[out] ptr Pointer to the CPU visible memory. Must be mapped.
 * @param[out] srcData Pointer to the CPU visible memory with the data.
 * @param[in] imageFormat Destination image format.
 * @param[in] copyInfo Pointer to a PalBufferImageCopyInfo struct that specifies parameters.
 * Must not be `nullptr`.
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Thread safe if `device` is externally synchronized.
 *
 * @since 2.0
 */
PAL_API PalResult PAL_CALL palWriteToImageCopyStagingBuffer(
    PalDevice* device,
    void* ptr,
    void* srcData,
    PalFormat imageFormat,
    PalBufferImageCopyInfo* copyInfo);

/**
 * @brief Bind an allocated memory to a buffer.
 *
 * The graphics system must be initialized before this call.
 * The memory size and alignment should match the requirements of the buffer.
 * Get the requirements with palGetBufferMemoryRequirements().
 *
 * @param[in] buffer Buffer to bind memory to.
 * @param[in] memory Memory to bind. Must not be `nullptr`.
 * @param[in] offset Starting point within the memory.
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Thread safe if `requirements` is per thread.
 *
 * @since 2.0
 * @sa palGetBufferMemoryRequirements
 */
PAL_API PalResult PAL_CALL palBindBufferMemory(
    PalBuffer* buffer,
    PalMemory* memory,
    uint64_t offset);

/**
 * @brief Maps buffer to CPU visible address space.
 *
 * The graphics system must be initialized before this call. The buffer must have a valid
 * memory bound to it before this call.
 *
 * Only `PAL_MEMORY_TYPE_CPU_UPLOAD` and `PAL_MEMORY_TYPE_CPU_READBACK` can be mapped to
 * CPU visible space. Mapping `PAL_MEMORY_TYPE_GPU_ONLY` will fail and return
 * `PAL_RESULT_MEMORY_MAP_FAILED`.
 *
 * @param[in] buffer Pointer to buffer to map. Memory must be bound.
 * @param[in] offset Starting point within the buffer.
 * @param[in] size Number of bytes to map from the offset. `offset + size` must not be
 * greater than buffer size.
 * @param[out] outPtr Pointer to a void* to recieved the mapped memory.
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Thread safe if `buffer` is externally synchronized.
 * Mapping with different offsets into the same buffer is thread safe as long as `buffer`
 * is externally synchronized.
 *
 * @since 2.0
 * @sa palUnmapBuffer
 */
PAL_API PalResult PAL_CALL palMapBuffer(
    PalBuffer* buffer,
    uint64_t offset,
    uint64_t size,
    void** outPtr);

/**
 * @brief Unmap buffer from CPU visible address space.
 *
 * The graphics system must be initialized before this call. The buffer must be mapped
 * before this call. After this call, the CPU pointer must not be used anymore.
 *
 * @param[in] buffer Pointer to buffer to unmap.
 *
 * Thread safety: Thread safe if `buffer` is externally synchronized.
 *
 * @since 2.0
 * @sa palMapBuffer
 */
PAL_API void PAL_CALL palUnmapBuffer(PalBuffer* buffer);

/**
 * @brief Get the device address of the provided buffer.
 *
 * The graphics system must be initialized before this call. Buffer must have
 * `PAL_BUFFER_USAGE_DEVICE_ADDRESS` usage flag.
 *
 * @param[in] buffer Buffer to get its device address.
 *
 * @return Buffer device address on success or `0` on failure.
 *
 * Thread safety: Thread safe if `buffer` is per thread.
 *
 * @since 2.0
 */
PAL_API PalDeviceAddress PAL_CALL palGetBufferDeviceAddress(PalBuffer* buffer);

/**
 * @brief Create a descriptor set layout that defines the bindings used by descriptor sets.
 *
 * The graphics system must be initialized before this call.
 *
 * This defines the layout, ordering and the number of descriptors a descriptor set uses.
 *
 * The layouts should reflect the exact layout of the shaders. Eg.
 * descriptorBindings[2] = { sampler, sampled image } is different from
 * descriptorBindings[2] = { sampled image, sampler }. The ordering must be correct.
 *
 * @param[in] device Device that creates the descriptor set layout.
 * @param[in] info Pointer to a PalDescriptorSetLayoutCreateInfo struct that specifies parameters.
 * Must not be `nullptr`.
 * @param[out] outLayout Pointer to a PalDescriptorSetLayout to recieve the created descriptor
 * set layout.
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Thread safe if `device` is externally synchronized.
 *
 * @since 2.0
 * @sa palDestroyDescriptorSetLayout
 */
PAL_API PalResult PAL_CALL palCreateDescriptorSetLayout(
    PalDevice* device,
    const PalDescriptorSetLayoutCreateInfo* info,
    PalDescriptorSetLayout** outLayout);

/**
 * @brief Destroy a descriptor set layout.
 *
 * The graphics system must be initialized before this call.
 * If the provided descriptor set layout is invalid or `nullptr`, this function returns
 * silently.
 *
 * @param[in] layout Descriptor set layout to destroy.
 *
 * Thread safety: Thread safe if the device used to create the descriptor set layout is
 * externally synchronized.
 *
 * @since 2.0
 * @sa palCreateDescriptorSetLayout
 */
PAL_API void PAL_CALL palDestroyDescriptorSetLayout(PalDescriptorSetLayout* layout);

/**
 * @brief Create a descriptor pool to allocate descriptor sets.
 *
 * The graphics system must be initialized before this call.
 *
 * @param[in] device Device that creates the descriptor pool.
 * @param[in] info Pointer to a PalDescriptorPoolCreateInfo struct that specifies parameters.
 * Must not be `nullptr`.
 * @param[out] outPool Pointer to a PalDescriptorPool to recieve the created descriptor pool.
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Thread safe if `device` is externally synchronized.
 *
 * @since 2.0
 * @sa palDestroyDescriptorPool
 */
PAL_API PalResult PAL_CALL palCreateDescriptorPool(
    PalDevice* device,
    const PalDescriptorPoolCreateInfo* info,
    PalDescriptorPool** outPool);

/**
 * @brief Destroy a descriptor pool.
 *
 * The graphics system must be initialized before this call.
 * If the provided descriptor pool is invalid or `nullptr`, this function returns
 * silently.
 *
 * @param[in] pool Descriptor pool to destroy.
 *
 * Thread safety: Thread safe if the device used to create the descriptor pool is
 * externally synchronized.
 *
 * @since 2.0
 * @sa palCreateDescriptorPool
 */
PAL_API void PAL_CALL palDestroyDescriptorPool(PalDescriptorPool* pool);

/**
 * @brief Reset the provided descriptor pool. This resets all allocated descriptor sets.
 *
 * The graphics system must be initialized before this call.
 *
 * @param[in] pool Descriptor pool to reset.
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Thread safe if `pool` is externally synchronized.
 *
 * @since 2.0
 */
PAL_API PalResult PAL_CALL palResetDescriptorPool(PalDescriptorPool* pool);

/**
 * @brief Allocate a descriptor set from the provided descriptor pool.
 *
 * The graphics system must be initialized before this call. The descriptor set will be
 * allocated uninitialized therefore update it before use except the case where descriptor
 * indexing is enabled.
 *
 * `pool` and `layout` must either be created with descriptor indexing enabled or not. Any other
 * pair will fail and return `PAL_RESULT_INVALID_OPERATION`.
 *
 * @param[in] device Device to allocate descriptor set on.
 * @param[in] pool Descriptor pool to allocate descriptor set from.
 * @param[in] layout Descriptor set layout that defines the bindings.
 * @param[out] outSet Pointer to a PalDescriptorSet to recieve the created descriptor set.
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Thread safe if `device` and `pool` are externally synchronized.
 *
 * @since 2.0
 */
PAL_API PalResult PAL_CALL palAllocateDescriptorSet(
    PalDevice* device,
    PalDescriptorPool* pool,
    PalDescriptorSetLayout* layout,
    PalDescriptorSet** outSet);

/**
 * @brief Update a descriptor set with descriptors (resources).
 *
 * The graphics system must be initialized before this call.
 *
 * If the write info has no valid resource handle, then `PAL_ADAPTER_FEATURE_NULL_DESCRIPTORS`
 * must be supported and enabled when creating the device if not, this function will fail and
 * return `PAL_RESULT_ADAPTER_FEATURE_NOT_SUPPORTED`.
 *
 * @param[in] device The Device. Must match the one used to allocate descriptor set.
 * @param[in] count Capacity of the PalDescriptorSetWriteInfo array.
 * @param[in] infos Array of PalDescriptorSetWriteInfo to write.
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Thread safe if `device` is externally synchronized.
 *
 * @since 2.0
 */
PAL_API PalResult PAL_CALL palUpdateDescriptorSet(
    PalDevice* device,
    uint32_t count,
    PalDescriptorSetWriteInfo* infos);

/**
 * @brief Create a pipeline layout. This defines the descriptor set interfaces and push
 * constant ranges.
 *
 * The graphics system must be initialized before this call.
 *
 * @param[in] device Device that creates the pipeline layout.
 * @param[in] info Pointer to a PalPipelineLayoutCreateInfo struct that specifies parameters.
 * Must not be `nullptr`.
 * @param[out] outLayout Pointer to a PalPipelineLayout to recieve the created pipeline layout.
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Thread safe if `device` is externally synchronized.
 *
 * @since 2.0
 * @sa palDestroyPipelineLayout
 */
PAL_API PalResult PAL_CALL palCreatePipelineLayout(
    PalDevice* device,
    const PalPipelineLayoutCreateInfo* info,
    PalPipelineLayout** outLayout);

/**
 * @brief Destroy a pipeline layout.
 *
 * The graphics system must be initialized before this call.
 * If the provided pipeline layout is invalid or `nullptr`, this function returns
 * silently.
 *
 * @param[in] layout Pipeline layout to destroy.
 *
 * Thread safety: Thread safe if the device used to create the pipeline layout is
 * externally synchronized.
 *
 * @since 2.0
 * @sa palCreatePipelineLayout
 */
PAL_API void PAL_CALL palDestroyPipelineLayout(PalPipelineLayout* layout);

/**
 * @brief Create a graphics pipeline.
 *
 * The graphics system must be initialized before this call.
 *
 * @param[in] device Device that creates the graphics pipeline.
 * @param[in] info Pointer to a PalGraphicsPipelineCreateInfo struct that specifies parameters.
 * Must not be `nullptr`.
 * @param[out] outPipeline Pointer to a PalPipeline to recieve the created buffer.
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Thread safe if `device` is externally synchronized.
 *
 * @since 2.0
 * @sa palDestroyPipeline
 */
PAL_API PalResult PAL_CALL palCreateGraphicsPipeline(
    PalDevice* device,
    const PalGraphicsPipelineCreateInfo* info,
    PalPipeline** outPipeline);

/**
 * @brief Create a compute pipeline.
 *
 * The graphics system must be initialized before this call.
 *
 * @param[in] device Device that creates the compute pipeline.
 * @param[in] info Pointer to a PalComputePipelineCreateInfo struct that specifies parameters.
 * Must not be `nullptr`.
 * @param[out] outPipeline Pointer to a PalPipeline to recieve the created buffer.
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Thread safe if `device` is externally synchronized.
 *
 * @note The first entry of the compute shader will be used.
 *
 * @since 2.0
 * @sa palDestroyPipeline
 */
PAL_API PalResult PAL_CALL palCreateComputePipeline(
    PalDevice* device,
    const PalComputePipelineCreateInfo* info,
    PalPipeline** outPipeline);

/**
 * @brief Create a ray tracing pipeline.
 *
 * The graphics system must be initialized before this call.
 *
 * `PAL_ADAPTER_FEATURE_RAY_TRACING` must be supported and enabled by the device if not, this
 * function will fail and return `PAL_RESULT_ADAPTER_FEATURE_NOT_SUPPORTED`.
 *
 * @param[in] device Device that creates the ray tracing pipeline.
 * @param[in] info Pointer to a PalRayTracingPipelineCreateInfo struct that specifies parameters.
 * Must not be `nullptr`.
 * @param[out] outPipeline Pointer to a PalPipeline to recieve the created buffer.
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Thread safe if `device` is externally synchronized.
 *
 * @note The shader group array must be in this order [raygen][miss][hitgroup][callable].
 *
 * @since 2.0
 * @sa palDestroyPipeline
 */
PAL_API PalResult PAL_CALL palCreateRayTracingPipeline(
    PalDevice* device,
    const PalRayTracingPipelineCreateInfo* info,
    PalPipeline** outPipeline);

/**
 * @brief Destroy a pipeline.
 *
 * The graphics system must be initialized before this call.
 * If the provided pipeline is invalid or `nullptr`, this function returns
 * silently.
 *
 * @param[in] pipeline Pipeline to destroy.
 *
 * Thread safety: Thread safe if the device used to create the pipeline is
 * externally synchronized.
 *
 * @since 2.0
 * @sa palCreateGraphicsPipeline
 * @sa palCreateComputePipeline
 * @sa palCreateRayTracingPipeline
 */
PAL_API void PAL_CALL palDestroyPipeline(PalPipeline* pipeline);

/**
 * @brief Create a shader binding table.
 *
 * The graphics system must be initialized before this call.
 *
 * `PAL_ADAPTER_FEATURE_RAY_TRACING` must be supported and enabled by the device if not, this
 * function will fail and return `PAL_RESULT_ADAPTER_FEATURE_NOT_SUPPORTED`.
 *
 * PalShaderBindingTableCreateInfo::recordCount must match the shader group count of
 * PalShaderBindingTableCreateInfo::rayTracingPipeline.
 *
 * @param[in] device Device that creates the shader binding table.
 * @param[in] info Pointer to a PalShaderBindingTableCreateInfo struct that specifies parameters.
 * Must not be `nullptr`.
 * @param[out] outSbt Pointer to a PalShaderBindingTable to recieve the created shader binding
 * table.
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Thread safe if `device` is externally synchronized.
 *
 * @note The records array must be in this order [raygen][miss][hitgroup][callable].
 *
 * @since 2.0
 * @sa palDestroyShaderBindingTable
 */
PAL_API PalResult PAL_CALL palCreateShaderBindingTable(
    PalDevice* device,
    const PalShaderBindingTableCreateInfo* info,
    PalShaderBindingTable** outSbt);

/**
 * @brief Destroy a shader binding table.
 *
 * The graphics system must be initialized before this call.
 * If the provided shader binding table is invalid or `nullptr`, this function returns
 * silently.
 *
 * @param[in] sbt Shader binding table to destroy.
 *
 * Thread safety: Thread safe if the device used to create the shader binding table is
 * externally synchronized.
 *
 * @since 2.0
 * @sa palCreateShaderBindingTable
 */
PAL_API void PAL_CALL palDestroyShaderBindingTable(PalShaderBindingTable* sbt);

/**
 * @brief Update a shader binding table record payloads.
 *
 * The graphics system must be initialized before this call.
 *
 * This call does not update shader handles. It only updates the payload associated
 * with the record. PalShaderBindingTableRecordInfo::groupIndex is the index into
 * the shader groups used to create the ray tracing pipeline.
 *
 * @param[in] sbt The shader binding table to update.
 * @param[in] count Capacity of the PalShaderBindingTableRecordInfo array.
 * @param[in] infos Array of PalShaderBindingTableRecordInfo to update.
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Thread safe if `sbt` is externally synchronized.
 *
 * @since 2.0
 */
PAL_API PalResult PAL_CALL palUpdateShaderBindingTable(
    PalShaderBindingTable* sbt,
    uint32_t count,
    PalShaderBindingTableRecordInfo* infos);

/**
 * @brief Build work group info(s) from work inputs specified in pixels, vertices etc.
 *
 * Call this function first with PalWorkGroupInfo array set to `nullptr` to get the number of work
 * group infos. Allocate memory for the PalWorkGroupInfo array and passed in the count and the
 * allocated array. If the count of the array is less than the number of work group infos, PAL will
 * write upto that limit.
 *
 * If the count is 0 and the PalWorkGroupInfo array is `nullptr`, the function fails
 * and returns `PAL_FALSE`.
 *
 * This function works the maths for how many work groups to dispatch in each axis and how many
 * times it needs to be dispatch in order for the work to be done. It works well with
 * palCmdDispatchBase() since its also gives the base for each work group.
 *
 * @param[in] data Pointer to a PalWorkGroupBuildData with parameters.
 * @param[in, out] count Capacity of the PalWorkGroupInfo array.
 * @param[out] infos Pointer to an Array of PalWorkGroupInfo.
 *
 * @return `PAL_TRUE` on success otherwise `PAL_FALSE`.
 *
 * Thread safety: Must only be called from the main thread.
 *
 * @since 2.0
 * @sa palCmdDrawMeshTasks
 * @sa palCmdDrawMeshTasksIndirect
 * @sa palCmdDrawMeshTasksIndirectCount
 * @sa palCmdDispatch
 * @sa palCmdDispatchBase
 */
PAL_API PalBool PAL_CALL palBuildWorkGroupInfo(
    const PalWorkGroupBuildData* data,
    int32_t* count,
    PalWorkGroupInfo* info);

/**
 * @brief Check if a constant is supported in a mask.
 * 
 * This function is used to check all masks in `supported_**` format in most of the capabilities
 * query structs. 
 * 
 * Example:
 * 
 * To check if `PAL_PRESENT_MODE_IMMEDIATE` is supported after querying `PalSurfaceCapabilities` 
 * capabilities of a surface, PalSurfaceCapabilities::supportedPresentModes should be the `mask` 
 * parameter and `PAL_PRESENT_MODE_IMMEDIATE` as the value parameter.
 *
 * @param[in] mask The supported mask.
 * @param[in, out] value The value to check in the supported mask.
 *
 * @return `PAL_TRUE` on success otherwise `PAL_FALSE`.
 *
 * Thread safety: Thread safe.
 *
 * @since 2.0
 */
static inline PalBool PAL_CALL palIsSupported(
    uint32_t mask,
    uint32_t value)
{
    return (mask & (1U << value)) != 0;
}

/** @} */

#endif // _PAL_GRAPHICS_H
