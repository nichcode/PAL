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

#ifndef GRAPHICS_PLATFORM_H
#define GRAPHICS_PLATFORM_H

#include "pal2/core/memory.h"
#include "pal2/graphics/init.h"
#include "pal2/graphics/vtable2.h"
#include "shared.h"

/** Initializes the graphics backend. */
typedef PalResult (*InitGraphicsFn)(
    const PalGraphicsDebugger* debugger,
    const PalAllocator* allocator);

/** Shuts down the graphics backend. */
typedef void (*ShutdownGraphicsFn)(void);

typedef struct GraphicsBackend {
    PalBool isCore;
    InitGraphicsFn init;
    ShutdownGraphicsFn shutdown;

    const PalGraphicsBackendVtable1* vtbl1;
    const PalGraphicsBackendVtable2* vtbl2;
} GraphicsBackend;

#define GRAPHICS_HANDLE(name)                                                                                          \
    struct name {                                                                                                      \
        GraphicsBackend* backend;                                                                                      \
    };

extern PalBool g_GraphicsInitialized;

/** Gets all supported internal backends per platform. */
const GraphicsBackend* const* platformGetGraphicsBackends(uint32_t* count);

/** Gets all active backends registered during initialization. */
const GraphicsBackend* graphicsGetBackends(uint32_t* count);

#endif // GRAPHICS_PLATFORM_H