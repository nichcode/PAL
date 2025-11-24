
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
    PAL_GPU_COMMAND_GRAPHICS = PAL_BIT64(0),
    PAL_GPU_COMMAND_COMPUTE = PAL_BIT64(1),
    PAL_GPU_COMMAND_TRANSFER = PAL_BIT64(2)
} PalGPUCommands;

typedef enum {
    PAL_GPU_FEATURE_RAY_TRACING = PAL_BIT64(0),
    PAL_GPU_FEATURE_MESH_SHADER = PAL_BIT64(1),
    PAL_GPU_FEATURE_VARIABLE_RATE_SHADING = PAL_BIT64(2),
    PAL_GPU_FEATURE_DESCRIPTOR_INDEXING = PAL_BIT64(3)
} PalGPUFeatures;

typedef struct {
    bool debugLayerSupported;
    PalGPUType type;
    PalGPUApiType apiType;
    Uint32 version;
    Uint64 totalMemory; // in bytes
    PalGPUCommands commands;
    PalGPUFeatures features;
    char versionString[PAL_GPU_VERSION_SIZE];
    char name[PAL_GPU_NAME_SIZE];
} PalGPUAdapterInfo;

typedef struct {
    PalResult PAL_CALL (*enumerateGPUAdapters)(
        Int32* count,
        PalGPUAdapter** outAdapters);

    PalResult PAL_CALL (*getGPUAdapterInfo)(
        PalGPUAdapter* adapter,
        PalGPUAdapterInfo* info);
} PalGPUBackend;

PAL_API PalResult PAL_CALL palInitGraphics(const PalAllocator* allocator);
PAL_API void PAL_CALL palShutdownGraphics();

PAL_API PalResult PAL_CALL palEnumerateGPUAdapters(
    Int32* count,
    PalGPUAdapter** outAdapters);

PAL_API PalResult PAL_CALL palGetGPUAdapterInfo(
    PalGPUAdapter* adapter,
    PalGPUAdapterInfo* info);

PAL_API PalResult PAL_CALL palAddGPUBackend(const PalGPUBackend* backend);

PAL_API PalResult PAL_CALL palCreateGPUDevice(
    bool debug,
    PalGPUAdapter* adapter,
    PalGPUDevice** outDevice);

PAL_API void PAL_CALL palDestroyGPUDevice(PalGPUDevice* device);

/** @} */ // end of pal_graphics group

#endif // _PAL_GRAPHICS_H