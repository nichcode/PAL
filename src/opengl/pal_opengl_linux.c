
/**

Copyright (C) 2025 Nicholas Agbo

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

// ==================================================
// Includes
// ==================================================

#include "pal/pal_opengl.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h> 

// X11 headers
#include <X11/Xlib.h>

// ==================================================
// Typedefs, enums and structs
// ==================================================

typedef struct {
    void (*shutdownGL)();
    PalResult (*enumerateGLFBConfigs)(PalGLWindow*, Int32*, PalGLFBConfig*);
    PalResult (*createGLContext)(const PalGLContextCreateInfo*, PalGLContext**);
    void (*destroyGLContext)(PalGLContext*);
    PalResult (*makeContextCurrent)(PalGLWindow*, PalGLContext*);
    void* (*getProcAddress)(const char*);
    PalResult (*swapBuffers)(PalGLWindow*, PalGLContext*);
    PalResult (*setSwapInterval)(Int32);
} backend;

typedef struct {
    bool initialized;
    const PalAllocator* allocator;
    const backend* backend;
    PalGLInfo info;
} GLLinux;

static GLLinux s_GL = {0};

// ==================================================
// X11 Typedefs, enums and structs
// ==================================================

typedef struct {
    int screen;
    Display* display;
    Window window;
    // dummy context
    PalGLInfo info;
} Glx;

// ==================================================
// Internal API
// ==================================================

// ==================================================
// X11 API
// ==================================================

PalResult xInitGL()
{

}

void xShutdownGL()
{

}

PalResult xEnumerateGLFBConfigs(
    PalGLWindow* glWindow,
    Int32* count,
    PalGLFBConfig* configs)
{

}

PalResult xCreateGLContext(
    const PalGLContextCreateInfo* info,
    PalGLContext** outContext)
{

}

void xDestroyGLContext(PalGLContext* context)
{

}

PalResult xMakeContextCurrent(
    PalGLWindow* glWindow,
    PalGLContext* context)
{

}

void* xGLGetProcAddress(const char* name)
{

}

PalResult xSwapBuffers(
    PalGLWindow* glWindow,
    PalGLContext* context)
{

}

PalResult xSetSwapInterval(Int32 interval)
{

}

static backend s_Xbackend = {
    .shutdownGL = xShutdownGL,
    .enumerateGLFBConfigs = xEnumerateGLFBConfigs,
    .createGLContext = xCreateGLContext,
    .destroyGLContext = xDestroyGLContext,
    .makeContextCurrent = xMakeContextCurrent,
    .getProcAddress = xGLGetProcAddress,
    .swapBuffers = xSwapBuffers,
    .setSwapInterval = xSetSwapInterval
};

// ==================================================
// Public API
// ==================================================

PalResult PAL_CALL palInitGL(const PalAllocator* allocator)
{
    if (s_GL.initialized) {
        return PAL_RESULT_SUCCESS;
    }

    if (allocator && (!allocator->allocate || !allocator->free)) {
        return PAL_RESULT_INVALID_ALLOCATOR;
    }

    // get backend type
    bool x11 = true;
    const char* session = getenv("XDG_SESSION_TYPE");
    if (session) {
        if (strcmp(session, "wayland") == 0) {
            x11 = false;
        }
    }

    if (x11) {
        PalResult ret = xInitGL();
        if (ret != PAL_RESULT_SUCCESS) {
            return ret;
        }
        s_GL.backend = &s_Xbackend;
    }

    s_GL.allocator = allocator;
    s_GL.initialized = true;
    return PAL_RESULT_SUCCESS;
}

void PAL_CALL palShutdownGL()
{
    if (s_GL.initialized) {
        s_GL.backend->shutdownGL();
        s_GL.initialized = false;
    }
}

const PalGLInfo* PAL_CALL palGetGLInfo()
{
    if (!s_GL.initialized) {
        return nullptr;
    }
    return &s_GL.info;
}

PalResult PAL_CALL palEnumerateGLFBConfigs(
    PalGLWindow* glWindow,
    Int32* count,
    PalGLFBConfig* configs)
{
    if (!s_GL.initialized) {
        return PAL_RESULT_GL_NOT_INITIALIZED;
    }

    if (!count) {
        return PAL_RESULT_NULL_POINTER;
    }

    if (count == 0 && configs) {
        return PAL_RESULT_INSUFFICIENT_BUFFER;
    }

    return s_GL.backend->enumerateGLFBConfigs(glWindow, count, configs);
}

const PalGLFBConfig* PAL_CALL palGetClosestGLFBConfig(
    PalGLFBConfig* configs,
    Int32 count,
    const PalGLFBConfig* desired)
{
    if (!s_GL.initialized) {
        return nullptr;
    }

    if (!configs || !desired) {
        return nullptr;
    }

    if (count == 0) {
        return nullptr;
    }

    Int32 score = 0;
    Int32 bestScore = 0x7FFFFFFF;
    PalGLFBConfig* best = nullptr;
    for (Int32 i = 0; i < count; i++) {
        PalGLFBConfig* tmp = &configs[i];

        // filter out hard constraints
        if (desired->doubleBuffer && !tmp->doubleBuffer) {
            continue;
        }

        if (desired->stereo && !tmp->stereo) {
            continue;
        }

        score = 0;

        // score color bits
        score += abs(tmp->redBits - desired->redBits);
        score += abs(tmp->greenBits - desired->greenBits);
        score += abs(tmp->blueBits - desired->blueBits);
        score += abs(tmp->alphaBits - desired->alphaBits);
        score += abs(tmp->depthBits - desired->depthBits);
        score += abs(tmp->stencilBits - desired->stencilBits);

        // score soft constraints
        if (desired->samples != tmp->samples) {
            score += 1000;
        }

        if (desired->sRGB != tmp->sRGB) {
            score += 500;
        }

        if (score < bestScore) {
            bestScore = score;
            best = &configs[i];
        }
    }

    return best;
}

// ==================================================
// Context
// ==================================================

PalResult PAL_CALL palCreateGLContext(
    const PalGLContextCreateInfo* info,
    PalGLContext** outContext)
{
    if (!s_GL.initialized) {
        return PAL_RESULT_GL_NOT_INITIALIZED;
    }

    if (!info || !outContext || (info && (!info->window || !info->fbConfig))) {
        return PAL_RESULT_NULL_POINTER;
    }

    // check support for requested features
    if (info->profile != PAL_GL_PROFILE_NONE) {
        if (!(s_GL.info.extensions & PAL_GL_EXTENSION_CONTEXT_PROFILE)) {
            return PAL_RESULT_GL_EXTENSION_NOT_SUPPORTED;
        }
    }

    if (info->forward) {
        if (!(s_GL.info.extensions & PAL_GL_EXTENSION_CREATE_CONTEXT)) {
            return PAL_RESULT_GL_EXTENSION_NOT_SUPPORTED;
        }
    }

    if (info->reset != PAL_GL_CONTEXT_RESET_NONE) {
        if (!(s_GL.info.extensions & PAL_GL_EXTENSION_ROBUSTNESS)) {
            return PAL_RESULT_GL_EXTENSION_NOT_SUPPORTED;
        }
    }

    if (info->noError) {
        if (!(s_GL.info.extensions & PAL_GL_EXTENSION_NO_ERROR)) {
            return PAL_RESULT_GL_EXTENSION_NOT_SUPPORTED;
        }
    }

    if (info->release != PAL_GL_RELEASE_BEHAVIOR_NONE) {
        if (!(s_GL.info.extensions & PAL_GL_EXTENSION_FLUSH_CONTROL)) {
            return PAL_RESULT_GL_EXTENSION_NOT_SUPPORTED;
        }
    }

    // clang-format off

    // check version
    bool valid = info->major < s_GL.info.major ||
        (info->major == s_GL.info.major && info->minor <= s_GL.info.minor);
    // clang-format on

    if (!valid) {
        return PAL_RESULT_INVALID_GL_VERSION;
    }

    return s_GL.backend->createGLContext(info, outContext);
}

void PAL_CALL palDestroyGLContext(PalGLContext* context)
{
    if (s_GL.initialized && context) {
        s_GL.backend->destroyGLContext(context);
    }
}

PalResult PAL_CALL palMakeContextCurrent(
    PalGLWindow* glWindow,
    PalGLContext* context)
{
    if (!s_GL.initialized) {
        return PAL_RESULT_GL_NOT_INITIALIZED;
    }

    if ((!glWindow && context) || (glWindow && !context)) {
        return PAL_RESULT_NULL_POINTER;
    }

    return s_GL.backend->makeContextCurrent(glWindow, context);
}

void* PAL_CALL palGLGetProcAddress(const char* name)
{
    if (!s_GL.initialized) {
        return nullptr;
    }

    return s_GL.backend->getProcAddress(name);
}

PalResult PAL_CALL palSwapBuffers(
    PalGLWindow* glWindow,
    PalGLContext* context)
{
    if (!s_GL.initialized) {
        return PAL_RESULT_GL_NOT_INITIALIZED;
    }

    if (!context || !glWindow) {
        return PAL_RESULT_NULL_POINTER;
    }

    return s_GL.backend->swapBuffers(glWindow, context);
}

PalResult PAL_CALL palSetSwapInterval(Int32 interval)
{
    if (!s_GL.initialized) {
        return PAL_RESULT_GL_NOT_INITIALIZED;
    }

    return s_GL.backend->setSwapInterval(interval);
}