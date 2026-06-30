
/**
    PAL - Prime Abstraction Layer
    Copyright (C) 2025
    Licensed under the Zlib license. See LICENSE file in root.
 */

#ifndef _PAL_OPENGL_BACKENDS_H
#define _PAL_OPENGL_BACKENDS_H

#include "pal_platform.h"
#include "pal/pal_opengl.h"

// clang-format off
typedef struct {
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
} OpenglBackend;

// ==================================================
// WGL
// ==================================================

#ifdef _WIN32
PalResult wglInitGL(PalGLAPI, void*, const PalAllocator*);
void wglShutdownGL();
const PalGLInfo* wglGetGLInfo();
PalResult wglEnumerateGLFBConfigs(int32_t*, PalGLFBConfig*);
PalResult wglCreateGLContext(const PalGLContextCreateInfo*, PalGLContext**);
void wglDestroyGLContext(PalGLContext*);
PalResult wglMakeContextCurrent(PalGLWindow*, PalGLContext*);
void* wglGetGLProcAddress(const char*);
PalResult wglSwapBuffers(PalGLWindow*, PalGLContext*);
PalResult wglSetSwapInterval(int32_t);
const PalBool* wglGetSupportedGLAPIs(void*);

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
PalResult eglInitGL(PalGLAPI, void*, const PalAllocator*);
void eglShutdownGL();
const PalGLInfo* eglGetGLInfo();
PalResult eglEnumerateGLFBConfigs(int32_t*, PalGLFBConfig*);
PalResult eglCreateGLContext(const PalGLContextCreateInfo*, PalGLContext**);
void eglDestroyGLContext(PalGLContext*);
PalResult eglMakeContextCurrent(PalGLWindow*, PalGLContext*);
void* eglGetGLProcAddress(const char*);
PalResult eglSwapBuffers(PalGLWindow*, PalGLContext*);
PalResult eglSetSwapInterval(int32_t);
const PalBool* eglGetSupportedGLAPIs(void*);

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

// clang-format on

#endif // _PAL_HAS_EGL

#endif // _PAL_OPENGL_BACKENDS_H