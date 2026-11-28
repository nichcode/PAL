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

#include "pal2/graphics/adapter.h"
#include "platform.h"

GRAPHICS_HANDLE(PalAdapter)

PalResult PAL_CALL palEnumerateAdapters(
    uint32_t* count,
    PalAdapter** adapters)
{
    if (!g_GraphicsInitialized) {
        return palMakeResult(PAL_RESULT_CODE_INVALID_OPERATION, PAL_RESULT_SOURCE_NONE, 0);
    }

    if (!count || (!adapters && *count > 0)) {
        return palMakeResult(PAL_RESULT_CODE_INVALID_ARGUMENT, PAL_RESULT_SOURCE_NONE, 0);
    }

    uint32_t backendCount = 0;
    const GraphicsBackend* backends = graphicsGetBackends(&backendCount);
    if (backendCount == 0) {
        *count = 0;
        return PAL_RESULT_SUCCESS;
    }

    if (!adapters) {
        uint32_t adapterCount = 0;
        for (uint32_t i = 0; i < backendCount; i++) {
            const GraphicsBackend* backend = &backends[i];
            uint32_t backendAdapterCount = 0;
            PalResult result = backend->vtbl1->enumerateAdapters(&backendAdapterCount, nullptr);
            if (result == PAL_RESULT_SUCCESS) {
                adapterCount += backendAdapterCount;
            }
        }

        *count = adapterCount;
        return PAL_RESULT_SUCCESS;
    }

    uint32_t maxCount = *count;
    uint32_t offset = 0;
    for (uint32_t i = 0; i < backendCount && offset < maxCount; i++) {
        const GraphicsBackend* backend = &backends[i];
        uint32_t backendAdapterCount = 0;
        PalResult result = backend->vtbl1->enumerateAdapters(&backendAdapterCount, nullptr);
        if (result != PAL_RESULT_SUCCESS || backendAdapterCount == 0) {
            continue;
        }

        uint32_t remaining = maxCount - offset;
        if (backendAdapterCount > remaining) {
            backendAdapterCount = remaining;
        }

        PalAdapter** adapters = &adapters[offset];
        result = backend->vtbl1->enumerateAdapters(&backendAdapterCount, adapters);
        if (result == PAL_RESULT_SUCCESS) {
            for (uint32_t j = 0; j < backendAdapterCount; j++) {
                adapters[j]->backend = backend;
            }
            offset += backendAdapterCount;
        }
    }

    *count = offset;
    return PAL_RESULT_SUCCESS;
}

void PAL_CALL palGetAdapterInfo(
    PalAdapter* adapter,
    PalAdapterInfo* info)
{
    ASSERT(adapter != nullptr, "adapter parameter is nullptr");
    ASSERT(info != nullptr, "info parameter is nullptr");

    adapter->backend->vtbl1->getAdapterInfo(adapter, info);
}

void PAL_CALL palGetAdapterCapabilities(
    PalAdapter* adapter,
    PalAdapterCapabilities* caps)
{
    ASSERT(adapter != nullptr, "adapter parameter is nullptr");
    ASSERT(caps != nullptr, "caps parameter is nullptr");

    adapter->backend->vtbl1->getAdapterCapabilities(adapter, caps);
}

PalAdapterFeatures PAL_CALL palGetAdapterFeatures(PalAdapter* adapter)
{
    ASSERT(adapter != nullptr, "adapter parameter is nullptr");

    return adapter->backend->vtbl1->getAdapterFeatures(adapter);
}

uint32_t PAL_CALL palGetHighestSupportedShaderTarget(
    PalAdapter* adapter,
    PalShaderFormats shaderFormat)
{
    ASSERT(adapter != nullptr, "adapter parameter is nullptr");

    return adapter->backend->vtbl1->getHighestSupportedShaderTarget(adapter, shaderFormat);
}