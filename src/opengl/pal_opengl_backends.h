
/**
    PAL - Prime Abstraction Layer
    Copyright (C) 2025
    Licensed under the Zlib license. See LICENSE file in root.
 */

#ifndef _PAL_OPENGL_BACKENDS_H
#define _PAL_OPENGL_BACKENDS_H

#include "pal_platform.h"
#include "pal/pal_opengl.h"

typedef struct {
    // clang-format off
    void (*shutdownGL)();
    const PalGLInfo* (*getGLInfo)();
    PalResult (*enumerateGLFBConfigs)(int32_t*, PalGLFBConfig*);
    PalResult (*createGLContext)(const PalGLContextCreateInfo*, PalGLContext**);
    void (*destroyGLContext)(PalGLContext* context);
    PalResult (*makeContextCurrent)(PalGLWindow*, PalGLContext*);
    void* (*getGLProcAddress)(const char*);
    PalResult (*swapBuffers)(PalGLWindow*, PalGLContext*);
    PalResult (*setSwapInterval)(int32_t);
    const PalBool* (*GetSupportedGLAPIs)(void*);
    // clang-format on
} OpenglBackend;

// ==================================================
// WGL
// ==================================================

#ifdef _WIN32
PalResult wglInitGL(PalGLAPI api, void* instance, const PalAllocator* allocator);
void wglShutdownGL();
const PalGLInfo* wglGetGLInfo();
PalResult wglEnumerateGLFBConfigs(int32_t* count, PalGLFBConfig* configs);
PalResult wglCreateGLContext(const PalGLContextCreateInfo* info, PalGLContext** outContext);
void wglDestroyGLContext(PalGLContext* context);
PalResult wglMakeContextCurrent(PalGLWindow* glWindow, PalGLContext* context);
void* wglGetGLProcAddress(const char* name);
PalResult wglSwapBuffers(PalGLWindow* glWindow, PalGLContext* context);
PalResult wglSetSwapInterval(int32_t interval);
const PalBool* wglGetSupportedGLAPIs(void* instance);

static OpenglBackend s_WglBackend = {
    .shutdownGL = wglShutdownGL,
    .getGLInfo = wglGetGLInfo,
    .enumerateGLFBConfigs = wglEnumerateGLFBConfigs,
    .createGLContext = wglCreateGLContext,
    .destroyGLContext = wglDestroyGLContext,
    .makeContextCurrent = wglMakeContextCurrent,
    .getGLProcAddress = wglGetGLProcAddress,
    .swapBuffers = wglSwapBuffers,
    .setSwapInterval = wglSetSwapInterval,
    .GetSupportedGLAPIs = wglGetSupportedGLAPIs};

#endif // _WIN32

// ==================================================
// EGL
// ==================================================

#if _PAL_HAS_EGL
PalResult eglInitGL(PalGLAPI api, void* instance, const PalAllocator* allocator);
void eglShutdownGL();
const PalGLInfo* eglGetGLInfo();
PalResult eglEnumerateGLFBConfigs(int32_t* count, PalGLFBConfig* configs);
PalResult eglCreateGLContext(const PalGLContextCreateInfo* info, PalGLContext** outContext);
void eglDestroyGLContext(PalGLContext* context);
PalResult eglMakeContextCurrent(PalGLWindow* glWindow, PalGLContext* context);
void* eglGetGLProcAddress(const char* name);
PalResult eglSwapBuffers(PalGLWindow* glWindow, PalGLContext* context);
PalResult eglSetSwapInterval(int32_t interval);
const PalBool* eglGetSupportedGLAPIs(void* instance);

static OpenglBackend s_EglBackend = {
    .shutdownGL = eglShutdownGL,
    .getGLInfo = eglGetGLInfo,
    .enumerateGLFBConfigs = eglEnumerateGLFBConfigs,
    .createGLContext = eglCreateGLContext,
    .destroyGLContext = eglDestroyGLContext,
    .makeContextCurrent = eglMakeContextCurrent,
    .getGLProcAddress = eglGetGLProcAddress,
    .swapBuffers = eglSwapBuffers,
    .setSwapInterval = eglSetSwapInterval,
    .GetSupportedGLAPIs = eglGetSupportedGLAPIs};

#endif // _PAL_HAS_EGL

#endif // _PAL_OPENGL_BACKENDS_H