
/**
    PAL - Prime Abstraction Layer
    Copyright (C) 2025
    Licensed under the Zlib license. See LICENSE file in root.
 */

#include "pal/pal_opengl.h"
#include "pal_opengl_backends.h"
#include <stdlib.h>

static OpenglBackend* s_Backend = nullptr;

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
    if (!instance) {
        return PAL_RESULT_CODE_INVALID_ARGUMENT;
    }

    // initialize the backends
#ifdef _WIN32
    PalResult result = wglInitGL(api, instance, allocator);
    if (result != PAL_RESULT_SUCCESS) {
        return result;
    }
    s_Backend = &s_WglBackend;
#endif // _WIN32

#if _PAL_HAS_EGL
    PalResult result = eglInitGL(api, instance, allocator);
    if (result != PAL_RESULT_SUCCESS) {
        return result;
    }
    s_Backend = &s_EglBackend;
#endif // _PAL_HAS_EGL

    // check if we found a backend
    if (!s_Backend) {
        return PAL_RESULT_CODE_PLATFORM_FAILURE;
    }
    return PAL_RESULT_SUCCESS;
}

void PAL_CALL palShutdownGL()
{
    s_Backend->shutdownGL();
}

const PalGLInfo* PAL_CALL palGetGLInfo()
{
    return s_Backend->getGLInfo();
}

PalResult PAL_CALL palEnumerateGLFBConfigs(
    uint32_t* count,
    PalGLFBConfig* configs)
{
    if (!count) {
        return PAL_RESULT_CODE_INVALID_ARGUMENT;
    }

    return s_Backend->enumerateGLFBConfigs(count, configs);
}

const PalGLFBConfig* PAL_CALL palGetClosestGLFBConfig(
    PalGLFBConfig* configs,
    uint32_t count,
    const PalGLFBConfig* desired)
{
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
    if (!info || !outContext) {
        return PAL_RESULT_CODE_INVALID_ARGUMENT;
    }

    return s_Backend->createGLContext(info, outContext);
}

void PAL_CALL palDestroyGLContext(PalGLContext* context)
{
    s_Backend->destroyGLContext(context);
}

PalResult PAL_CALL palMakeContextCurrent(
    PalGLWindow* glWindow,
    PalGLContext* context)
{
    return s_Backend->makeContextCurrent(glWindow, context);
}

void* PAL_CALL palGetGLProcAddress(const char* name)
{
    return s_Backend->getGLProcAddress(name);
}

PalResult PAL_CALL palSwapBuffers(
    PalGLWindow* glWindow,
    PalGLContext* context)
{
    return s_Backend->swapBuffers(glWindow, context);
}

void PAL_CALL palSetSwapInterval(int32_t interval)
{
    s_Backend->setSwapInterval(interval);
}

const PalBool* PAL_CALL palGetSupportedGLAPIs(void* instance)
{
#if _PAL_HAS_EGL
        return eglGetSupportedGLAPIs(instance);
#elif defined(_WIN32)
        return wglGetSupportedGLAPIs(instance);
#endif // _PAL_HAS_EGL
}