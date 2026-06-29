
/**
    PAL - Prime Abstraction Layer
    Copyright (C) 2025
    Licensed under the Zlib license. See LICENSE file in root.
 */

#include "pal/pal_opengl.h"
#include "pal_opengl_backends.h"
#include <stdlib.h>

typedef struct {
    PalBool initialized;
    const OpenglBackend* backend;
} Opengl;

static Opengl s_Gl = {0};

PalBool checkString(
    const char* string,
    const char* strings)
{
    const char* start = strings;
    size_t stringLen = strlen(string);

    for (;;) {
        const char* where = nullptr;
        const char* terminator = nullptr;

        where = strstr(start, string);
        if (!where) {
            return PAL_FALSE;
        }

        // the string was found, we find the terminator by adding the sizeof the strings
        terminator = where + stringLen;
        if (where == start || *(where - 1) == ' ') {
            if (*terminator == ' ' || *terminator == '\0') {
                return PAL_TRUE;
            }
        }

        start = terminator;
    }
}

PalResult PAL_CALL palInitGL(
    PalGLAPI api,
    void* instance,
    const PalAllocator* allocator)
{
    if (s_Gl.initialized) {
        return PAL_RESULT_SUCCESS;
    }

    if (!instance) {
        return PAL_RESULT_CODE_INVALID_ARGUMENT;
    }

    if (allocator && (!allocator->allocate || !allocator->free)) {
        return PAL_RESULT_CODE_INVALID_ARGUMENT;
    }

    // initialize the backends
#ifdef _WIN32
    PalResult result = wglInitGL(api, instance, allocator);
    if (result != PAL_RESULT_SUCCESS) {
        return result;
    }
    s_Gl.backend = &s_WglBackend;
#endif // _WIN32

#if _PAL_HAS_EGL
    PalResult result = eglInitGL(api, instance, allocator);
    if (result != PAL_RESULT_SUCCESS) {
        return result;
    }
    s_Gl.backend = &s_EglBackend;
#endif // _PAL_HAS_EGL

    // check if we found a backend
    if (!s_Gl.backend) {
        return PAL_RESULT_CODE_PLATFORM_FAILURE;
    }

    s_Gl.initialized = PAL_TRUE;
    return PAL_RESULT_SUCCESS;
}

void PAL_CALL palShutdownGL()
{
    if (s_Gl.initialized) {
        s_Gl.backend->shutdownGL();
        s_Gl.initialized = PAL_FALSE;
    }
}

const PalGLInfo* PAL_CALL palGetGLInfo()
{
    if (s_Gl.initialized) {
        return s_Gl.backend->getGLInfo();
    }
    return nullptr;
}

PalResult PAL_CALL palEnumerateGLFBConfigs(
    int32_t* count,
    PalGLFBConfig* configs)
{
    if (!s_Gl.initialized) {
        return PAL_RESULT_CODE_NOT_INITIALIZED;
    }

    if (!count || *count == 0 && configs) {
        return PAL_RESULT_CODE_INVALID_ARGUMENT;
    }

    return s_Gl.backend->enumerateGLFBConfigs(count, configs);
}

const PalGLFBConfig* PAL_CALL palGetClosestGLFBConfig(
    PalGLFBConfig* configs,
    int32_t count,
    const PalGLFBConfig* desired)
{
    if (!configs || !desired) {
        return nullptr;
    }

    if (count == 0) {
        return nullptr;
    }

    int32_t score = 0;
    int32_t bestScore = 0x7FFFFFFF;
    PalGLFBConfig* best = nullptr;
    for (int32_t i = 0; i < count; i++) {
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

PalResult PAL_CALL palCreateGLContext(
    const PalGLContextCreateInfo* info,
    PalGLContext** outContext)
{
    if (!s_Gl.initialized) {
        return PAL_RESULT_CODE_NOT_INITIALIZED;
    }

    if (!info || !outContext) {
        return PAL_RESULT_CODE_INVALID_ARGUMENT;
    }

    return s_Gl.backend->createGLContext(info, outContext);
}

void PAL_CALL palDestroyGLContext(PalGLContext* context)
{
    if (s_Gl.initialized && context) {
        s_Gl.backend->destroyGLContext(context);
    }
}

PalResult PAL_CALL palMakeContextCurrent(
    PalGLWindow* glWindow,
    PalGLContext* context)
{
    if (!s_Gl.initialized) {
        return PAL_RESULT_CODE_NOT_INITIALIZED;
    }

    if (!glWindow || !context) {
        return PAL_RESULT_CODE_INVALID_ARGUMENT;
    }

    return s_Gl.backend->makeContextCurrent(glWindow, context);
}

void* PAL_CALL palGetGLProcAddress(const char* name)
{
    if (s_Gl.initialized && name) {
        return s_Gl.backend->getGLProcAddress(name);
    }
    return nullptr;
}

PalResult PAL_CALL palSwapBuffers(
    PalGLWindow* glWindow,
    PalGLContext* context)
{
    if (!s_Gl.initialized) {
        return PAL_RESULT_CODE_NOT_INITIALIZED;
    }

    if (!glWindow || !context) {
        return PAL_RESULT_CODE_INVALID_ARGUMENT;
    }

    return s_Gl.backend->swapBuffers(glWindow, context);
}

PalResult PAL_CALL palSetSwapInterval(int32_t interval)
{
    if (!s_Gl.initialized) {
        return PAL_RESULT_CODE_NOT_INITIALIZED;
    }
    return s_Gl.backend->setSwapInterval(interval);
}

const PalBool* PAL_CALL palGetSupportedGLAPIs(void* instance)
{
    if (instance) {
#if _PAL_HAS_EGL
        return eglGetSupportedGLAPIs(instance);
#elif defined(_WIN32)
        return wglGetSupportedGLAPIs(instance);
#endif // _PAL_HAS_EGL
    }
    return nullptr;
}