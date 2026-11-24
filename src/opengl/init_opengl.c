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
#include <string.h>

const GLBackend* g_GLBackend = nullptr;
PalGLInfo g_GLInfo = {0};
PalBool g_GLInitialized = PAL_FALSE;

typedef union GLSymbol
{
    void* raw;
    PalLibrarySymbol symbol;
} GLSymbol;

const PalBool* eglGetSupportedGLAPIs(const void*);
const PalBool* wglGetSupportedGLAPIs(const void*);

PalResult PAL_CALL palInitGL(
    PalGLAPI api,
    const void* instance,
    const PalAllocator* allocator)
{
    if (g_GLInitialized) {
        return PAL_RESULT_SUCCESS;
    }

    ASSERT(instance != nullptr, "instance parameter is nullptr");

    uint32_t backendCount = 0;
    const GLBackend* const* backends = platformGetGLBackends(&backendCount);
    if (backendCount == 0) {
        return palMakeResult(
            PAL_RESULT_CODE_FEATURE_NOT_SUPPORTED,
            PAL_RESULT_SOURCE_NONE,
            0);
    }

    uint32_t selectedIndex = 0; // first supported backend
    const GLBackend* selectedBackend = backends[selectedIndex];
    PalResult result = selectedBackend->init(api, instance, allocator);

    if (result != PAL_RESULT_SUCCESS) {
        for (uint32_t i = 0; i < backendCount; i++) {
            if (i == selectedIndex) {
                continue;
            }

            const GLBackend* candidate = backends[i];
            result = candidate->init(api, instance, allocator);

            if (result == PAL_RESULT_SUCCESS) {
                selectedBackend = candidate;
                selectedIndex = i;
                break;
            }
        }
    }

    if (result != PAL_RESULT_SUCCESS) {
        return result;
    }

    g_GLBackend = selectedBackend;
    g_GLInitialized = PAL_TRUE;

    return PAL_RESULT_SUCCESS;
}

void PAL_CALL palShutdownGL(void)
{
    ASSERT(g_GLInitialized != PAL_FALSE, "GL system not initialized");

    g_GLBackend->shutdown();
    g_GLBackend = nullptr;
    g_GLInitialized = PAL_FALSE;
    memset(&g_GLInfo, 0, sizeof(PalGLInfo));
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
#if PLATFORM_EGL
    return eglGetSupportedGLAPIs(instance);
#elif defined(_WIN32)
    return wglGetSupportedGLAPIs(instance);
#endif // PLATFORM_EGL
}