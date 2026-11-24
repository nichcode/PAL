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
#include "pal2/opengl/init.h"

const GLBackend* g_GLBackend = nullptr;
PalGLInfo g_GLInfo = {0};
PalBool g_GLInitialized = PAL_FALSE;

typedef union GLSymbol
{
    void* raw;
    PalLibrarySymbol symbol;
} GLSymbol;

PalResult PAL_CALL palInitGL(
    PalGLAPI api,
    const void* instance,
    const PalAllocator* allocator)
{
    ASSERT(instance != nullptr, "instance parameter is nullptr");

    return g_GLBackend->init(api, instance, allocator);
}

void PAL_CALL palShutdownGL(void)
{
    ASSERT(g_GLInitialized != PAL_FALSE, "GL system not initialized");

    g_GLBackend->shutdown();
}

const PalGLInfo* PAL_CALL palGetGLInfo(void)
{
    ASSERT(g_GLInitialized != PAL_FALSE, "GL system not initialized");

    if (g_GLInitialized) {
        return &g_GLInfo;
    }
    return nullptr;
}

PalLibrarySymbol PAL_CALL palGetGLProcAddress(const char* name)
{
    if (g_GLInitialized) {
        GLSymbol sym;
        sym.raw = g_GLBackend->getProcAddress(name);
        if (sym.raw) {
            return sym.symbol;
        }

        return nullptr;
    }

    return nullptr;
}

const PalBool* PAL_CALL palGetSupportedGLAPIs(const void* instance)
{
    return g_GLBackend->getSupportedAPIs(instance);
}