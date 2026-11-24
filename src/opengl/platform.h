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

#ifndef OPENGL_PLATFORM_H
#define OPENGL_PLATFORM_H

#include "shared.h"
#include "pal2/opengl/init.h"
#include "pal2/opengl/fbconfig.h"
#include "pal2/opengl/context.h"

/**
 * Platform contract for the opengl module.
 *
 * Every platform must implement every function declared here. A function
 * that says it has the same semantics and rules as a public `pal*` function
 * has only the platform requirements. A function with no public counterpart
 * is documented in full. These rules apply to all of them:
 *
 * - Arguments are validated by the caller. A platform function can assume
 *   every pointer it receives is valid and must not validate it again.
 *   Any exception will be documented by the function.
 */

/* =========================================================================
 * Init
 * ========================================================================= */

/** This function has the same semantics and rules as palInitGL(). */
typedef PalResult (*InitGLFn)(
    PalGLAPI api,
    const void* instance,
    const PalAllocator* allocator);

/** This function has the same semantics and rules as palShutdownGL(). */
typedef void (*ShutdownGLFn)(void);

/** This function has the same semantics and rules as palGetSupportedGLAPIs(). */
typedef const PalBool* (*GetSupportedGLAPIsFn)(const void* instance);

/** This function has the same semantics and rules as palGetGLProcAddress(). */
typedef void* (*GetGLProcAddressFn)(const char* name);


/* =========================================================================
 * Framebuffer Configuration
 * ========================================================================= */

/** This function has the same semantics and rules as palEnumerateGLFBConfigs(). */
typedef PalResult (*EnumerateGLFBConfigsFn)(
    uint32_t* count, 
    PalGLFBConfig* configs);


/* =========================================================================
 * Context
 * ========================================================================= */

/** This function has the same semantics and rules as palCreateGLContext(). */
typedef PalResult (*CreateGLContextFn)(
    const PalGLContextCreateInfo* info, 
    PalGLContext** outContext);

/** This function has the same semantics and rules as palDestroyGLContext(). */
typedef void (*DestroyGLContextFn)(PalGLContext* context);

/** This function has the same semantics and rules as palMakeContextCurrent(). */
typedef PalResult (*MakeContextCurrentFn)(
    PalGLWindow* window, 
    PalGLContext* context);

/** This function has the same semantics and rules as palSwapBuffers(). */
typedef PalResult (*SwapBuffersFn)(
    PalGLWindow* window, 
    PalGLContext* context);

/** This function has the same semantics and rules as palSetSwapInterval(). */
typedef void (*SetSwapIntervalFn)(int32_t interval);


/** A opengl backend. */
typedef struct GLBackend {
    InitGLFn init;
    ShutdownGLFn shutdown;
    GetSupportedGLAPIsFn getSupportedAPIs;
    EnumerateGLFBConfigsFn enumerateFBConfigs;
    GetGLProcAddressFn getProcAddress;

    CreateGLContextFn createContext;
    DestroyGLContextFn destroyContext;
    MakeContextCurrentFn makeContextCurrent;
    SwapBuffersFn swapBuffers;
    SetSwapIntervalFn setSwapInterval;
} GLBackend;

extern const GLBackend* g_GLBackend; /**< Active opengl backend. */
extern PalGLInfo g_GLInfo;
extern PalBool g_GLInitialized;

/** Gets all supported backends per platform. */
const GLBackend* const* platformGetGLBackends(uint32_t* count);

PalBool checkString(
    const char* string,
    const char* strings);

#endif // OPENGL_PLATFORM_H