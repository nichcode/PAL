
/**

Copyright (C) 2025 Nicholas Agbo <agbonicholas04@gmail.com>

This software is provided 'as-is', without any express or implied
warranty.  In no event will the authors be held liable for any damages
arising from the use of this software.

Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it
freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not
   claim that you wrote the original software. If you use this software
   in a product, an acknowledgment in the product documentation would be
   appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be
   misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.

 */

/**
 * @defgroup pal_graphics Graphics
 * Graphics PAL functionality such as GPUs, GPUDevices, swapchains and more.
 *
 * @{
 */

#ifndef _PAL_GRAPHICS_H
#define _PAL_GRAPHICS_H

#include "pal_core.h"

#define PAL_GPU_NAME_SIZE 128
#define PAL_GPU_VERSION_SIZE 16

#define PAL_VERSION_DEFAULT 0x0667
#define PAL_MAKE_VERSION(major, minor) (((major) << 16) | ((minor) & 0xFFFF))
#define PAL_VERSION_MAJOR(version) ((version) >> 16)
#define PAL_VERSION_MINOR(version) ((version) & 0xFFFF)

typedef struct PalGPUAdapter PalGPUAdapter;
typedef struct PalGPUDevice PalGPUDevice;

typedef enum {
    PAL_GPU_TYPE_UNKNOWN,
    PAL_GPU_TYPE_DISCRETE,
    PAL_GPU_TYPE_INTEGRATED,
    PAL_GPU_TYPE_VIRTUAL,
    PAL_GPU_TYPE_CPU
} PalGPUType;

typedef enum {
    PAL_GPU_API_VULKAN,
    PAL_GPU_API_D3D12,
    PAL_GPU_API_METAL,

    // for custom backends
    PAL_GPU_API_OPENGL,
    PAL_GPU_API_GLES,
    PAL_GPU_API_D3D11,
    PAL_GPU_API_D3D9,
    PAL_GPU_API_PPM,
} PalGPUApiType;

typedef enum {
    PAL_GPU_COMMAND_QUEUE_GRAPHICS = PAL_BIT64(0),
    PAL_GPU_COMMAND_QUEUE_COMPUTE = PAL_BIT64(1),
    PAL_GPU_COMMAND_QUEUE_TRANSFER = PAL_BIT64(2)
} PalGPUCommandQueues;

typedef enum {
    PAL_GPU_SHADER_FORMAT_SPIRV = PAL_BIT(0),
    PAL_GPU_SHADER_FORMAT_DXIL = PAL_BIT(1),
    PAL_GPU_SHADER_FORMAT_DXBC = PAL_BIT(2),
    PAL_GPU_SHADER_FORMAT_GLSL = PAL_BIT(3),
    PAL_GPU_SHADER_FORMAT_MSL = PAL_BIT(4),
    PAL_GPU_SHADER_FORMAT_PPM = PAL_BIT(5)
} PalGPUShaderFormats;

typedef enum {
    PAL_GPU_FEATURE_SAMPLER_ANISOTROPY = PAL_BIT64(0),
    PAL_GPU_FEATURE_SAMPLE_RATE_SHADING = PAL_BIT64(1),
    PAL_GPU_FEATURE_MULTI_VIEWPORT = PAL_BIT64(2),
    PAL_GPU_FEATURE_TIMELINE_SEMAPHORE = PAL_BIT64(3),
    PAL_GPU_FEATURE_TESSELLATION_SHADER = PAL_BIT64(4),
    PAL_GPU_FEATURE_GEOMETRY_SHADER = PAL_BIT64(5),
    PAL_GPU_FEATURE_SHADER_FLOAT16 = PAL_BIT64(6),
    PAL_GPU_FEATURE_SHADER_FLOAT64 = PAL_BIT64(7),
    PAL_GPU_FEATURE_SHADER_INT16 = PAL_BIT64(8),
    PAL_GPU_FEATURE_SHADER_INT64 = PAL_BIT64(9),
    PAL_GPU_FEATURE_DYNAMIC_RENDERING = PAL_BIT64(10),
    PAL_GPU_FEATURE_RAY_TRACING = PAL_BIT64(11),
    PAL_GPU_FEATURE_MESH_SHADER = PAL_BIT64(12),
    PAL_GPU_FEATURE_VARIABLE_RATE_SHADING = PAL_BIT64(13),
    PAL_GPU_FEATURE_DESCRIPTOR_INDEXING = PAL_BIT64(14),
    PAL_GPU_FEATURE_SWAPCHAIN = PAL_BIT64(15)
} PalGPUFeatures;

typedef struct {
    bool debugLayerSupported;
    PalGPUType type;
    PalGPUApiType apiType;
    PalGPUShaderFormats shaderFormats;
    Uint64 totalMemory; // in bytes
    PalGPUCommandQueues commandQueues;
    PalGPUFeatures features;
    char versionString[PAL_GPU_VERSION_SIZE];
    char name[PAL_GPU_NAME_SIZE];
} PalGPUAdapterInfo;

typedef struct {
    bool debug;
    PalGPUCommandQueues commandQueues;
} PalGPUDeviceCreateInfo;

typedef struct {
    PalResult PAL_CALL (*enumerateGPUAdapters)(
        Int32* count,
        PalGPUAdapter** outAdapters);

    PalResult PAL_CALL (*getGPUAdapterInfo)(
        PalGPUAdapter* adapter,
        PalGPUAdapterInfo* info);

    PalResult PAL_CALL (*createGPUDevice)(
        PalGPUAdapter* adapter,
        const PalGPUDeviceCreateInfo* info,
        PalGPUDevice** outDevice);

    void PAL_CALL (*destroyGPUDevice)(PalGPUDevice* device);
} PalGPUBackend;

PAL_API PalResult PAL_CALL palInitGraphics(
    bool enableDebug,
    Int32 versionHint,
    const PalAllocator* allocator);

PAL_API void PAL_CALL palShutdownGraphics();

PAL_API PalResult PAL_CALL palEnumerateGPUAdapters(
    Int32* count,
    PalGPUAdapter** outAdapters);

PAL_API PalResult PAL_CALL palGetGPUAdapterInfo(
    PalGPUAdapter* adapter,
    PalGPUAdapterInfo* info);

PAL_API PalResult PAL_CALL palAddGPUBackend(const PalGPUBackend* backend);

PAL_API PalResult PAL_CALL palCreateGPUDevice(
    PalGPUAdapter* adapter,
    const PalGPUDeviceCreateInfo* info,
    PalGPUDevice** outDevice);

PAL_API void PAL_CALL palDestroyGPUDevice(PalGPUDevice* device);

/** @} */ // end of pal_graphics group

#endif // _PAL_GRAPHICS_H