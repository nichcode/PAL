/**
 * Copyright (C) 2025-2026 Nicholas Agbo <agbonicholas04@gmail.com>
 *
 * This software is provided 'as-is', without any express or implied
 * warranty.  In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 *
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 *
 * 3. This notice may not be removed or altered from any source distribution.
 */

#include "platform.h"
#include "pal2/graphics/init.h"

#define MAX_BACKENDS (PAL_MAX_CUSTOM_BACKENDS + 2)

typedef struct Graphics {
    uint32_t backendCount;
    const PalAllocator* allocator;
    GraphicsBackend backends[MAX_BACKENDS];
} Graphics;

static Graphics s_Graphics = {0};
PalBool g_GraphicsInitialized = PAL_FALSE;

/* =========================================================================
 * Helper
 * ========================================================================= */

static PalBool validateVtable1(const PalGraphicsBackendVtable1* vtable)
{
    // clang-format off

    if (!vtable->enumerateAdapters                             ||
        !vtable->getAdapterInfo                                ||
        !vtable->getAdapterCapabilities                        ||
        !vtable->getAdapterFeatures                            ||
        !vtable->getHighestSupportedShaderTarget               ||
        !vtable->createDevice                                  ||
        !vtable->destroyDevice                                 ||
        !vtable->getDeviceLostReason                           ||
        !vtable->allocateMemory                                ||
        !vtable->freeMemory                                    ||
        !vtable->createQueue                                   ||
        !vtable->destroyQueue                                  ||
        !vtable->waitQueue                                     ||
        !vtable->canQueuePresent                               ||
        !vtable->enumerateFormats                              ||
        !vtable->isFormatSupported                             ||
        !vtable->queryFormatImageUsages                        ||
        !vtable->queryFormatSampleCount                        ||
        !vtable->createImage                                   ||
        !vtable->destroyImage                                  ||
        !vtable->getImageInfo                                  ||
        !vtable->getImageMemoryRequirements                    ||
        !vtable->bindImageMemory                               ||
        !vtable->createImageView                               ||
        !vtable->destroyImageView                              ||
        !vtable->createSampler                                 ||
        !vtable->destroySampler                                ||
        !vtable->createShader                                  ||
        !vtable->destroyShader                                 ||
        !vtable->createFence                                   ||
        !vtable->destroyFence                                  ||
        !vtable->waitFence                                     ||
        !vtable->isFenceSignaled                               ||
        !vtable->createSemaphore                               ||
        !vtable->destroySemaphore                              ||
        !vtable->createCommandPool                             ||
        !vtable->destroyCommandPool                            ||
        !vtable->allocateCommandBuffer                         ||
        !vtable->freeCommandBuffer                             ||
        !vtable->resetCommandBuffer                            ||
        !vtable->submitCommandBuffer                           ||
        !vtable->cmdBegin                                      ||
        !vtable->cmdEnd                                        ||
        !vtable->cmdExecuteCommandBuffer                       ||
        !vtable->cmdBeginRendering                             ||
        !vtable->cmdEndRendering                               ||
        !vtable->cmdCopyBuffer                                 ||
        !vtable->cmdCopyBufferToImage                          ||
        !vtable->cmdCopyImage                                  ||
        !vtable->cmdCopyImageToBuffer                          ||
        !vtable->cmdBindPipeline                               ||
        !vtable->cmdSetViewport                                ||
        !vtable->cmdSetScissors                                ||
        !vtable->cmdBindVertexBuffers                          ||
        !vtable->cmdBindIndexBuffer                            ||
        !vtable->cmdDraw                                       ||
        !vtable->cmdDrawIndexed                                ||
        !vtable->cmdImageBarrier                               ||
        !vtable->cmdBufferBarrier                              ||
        !vtable->cmdDispatch                                   ||
        !vtable->cmdBindDescriptorSet                          ||
        !vtable->cmdPushConstants                              ||
        !vtable->createBuffer                                  ||
        !vtable->destroyBuffer                                 ||
        !vtable->getBufferMemoryRequirements                   ||
        !vtable->computeImageStagingRequirements               ||
        !vtable->writeImageStaging                             ||
        !vtable->bindBufferMemory                              ||
        !vtable->mapBuffer                                     ||
        !vtable->unmapBuffer                                   ||
        !vtable->createDescriptorSetLayout                     ||
        !vtable->destroyDescriptorSetLayout                    ||
        !vtable->createDescriptorPool                          ||
        !vtable->destroyDescriptorPool                         ||
        !vtable->resetDescriptorPool                           ||
        !vtable->allocateDescriptorSet                         ||
        !vtable->updateDescriptorSet                           ||
        !vtable->createPipelineLayout                          ||
        !vtable->destroyPipelineLayout                         ||
        !vtable->createGraphicsPipeline                        ||
        !vtable->createComputePipeline                         ||
        !vtable->destroyPipeline) {
        return PAL_FALSE;
    }

    // clang-format on
    return PAL_TRUE;
}

static PalBool validateVtable2(const PalGraphicsBackendVtable2* vtable)
{
    if (!validateVtable1(vtable->vtable1)) {
        return PAL_FALSE;
    }

    // clang-format off

    if (!vtable->canQueueShareOwnership      || 
        !vtable->canQueueUseUsageState       ||
        !vtable->canQueueUsePipelineStages   ||
        !vtable->cmdImageOwnershipTransfer   ||
        !vtable->cmdBufferOwnershipTransfer) {
        return PAL_FALSE;
    }

    // clang-format on
    return PAL_TRUE;
}

static PalBool addBackend(const PalGraphicsBackendInfo* info)
{
    GraphicsBackend* backend = &s_Graphics.backends[s_Graphics.backendCount++];
    memset(backend, 0, sizeof(GraphicsBackend));

    if (info->version == PAL_GRAPHICS_BACKEND_VTABLE_VERSION_1) {
        const PalGraphicsBackendVtable1* vtable = (PalGraphicsBackendVtable1*)info->vtable;
        if (!validateVtable1(vtable)) {
            return PAL_FALSE;
        }
        backend->vtbl1 = vtable;

    } else if (info->version == PAL_GRAPHICS_BACKEND_VTABLE_VERSION_2) {
        const PalGraphicsBackendVtable2* vtable = (PalGraphicsBackendVtable2*)info->vtable;
        if (!validateVtable2(vtable)) {
            return PAL_FALSE;
        }
        backend->vtbl1 = vtable->vtable1;
        backend->vtbl2 = vtable;
    }

    return PAL_TRUE;
}

/* =========================================================================
 * API
 * ========================================================================= */

PalResult PAL_CALL palInitGraphics(
    const PalGraphicsDebugger* debugger,
    const PalAllocator* allocator,
    uint32_t backendCount,
    const PalGraphicsBackendInfo* backends)
{
    if (g_GraphicsInitialized) {
        return PAL_RESULT_SUCCESS;
    }

    uint32_t coreBackendCount = 0;
    const GraphicsBackend* const* coreBackends = platformGetGraphicsBackends(&coreBackendCount);
    for (uint32_t i = 0; i < coreBackendCount; i++) {
        const GraphicsBackend* backend = coreBackends[i];
        PalResult result = backend->init(debugger, allocator);

        if (result != PAL_RESULT_SUCCESS) {
            continue;
        }

        PalGraphicsBackendInfo backendInfo = {0};
        backendInfo.version = PAL_GRAPHICS_BACKEND_VTABLE_VERSION_2;
        backendInfo.vtable = backend->vtbl2;
        addBackend(&backendInfo);
    }

    for (uint32_t i = 0; i < backendCount; i++) {
        if (!addBackend(&backends[i])) {
            return PAL_RESULT_CODE_INVALID_ARGUMENT;
        }
    }
    
    g_GraphicsInitialized = PAL_TRUE;
    return PAL_RESULT_SUCCESS;
}

void PAL_CALL palShutdownGraphics()
{
    ASSERT(g_GraphicsInitialized != PAL_FALSE, "Graphics system not initialized");

    uint32_t backendCount = 0;
    const GraphicsBackend* const* backends = platformGetGraphicsBackends(&backendCount);
    for (uint32_t i = 0; i < backendCount; i++) {
        const GraphicsBackend* backend = backends[i];
        if (backend->isCore) {
            backend->shutdown();
        }
    }

    memset(&s_Graphics, 0, sizeof(s_Graphics));
    g_GraphicsInitialized = PAL_FALSE;
}