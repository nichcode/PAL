
/**
    PAL - Prime Abstraction Layer
    Copyright (C) 2025
    Licensed under the Zlib license. See LICENSE file in root.
 */

#ifndef _PAL_OPENGL_BACKENDS_H
#define _PAL_OPENGL_BACKENDS_H

#include "pal2/pal_opengl.h"
#include "pal_platform.h"

// clang-format off
typedef struct {
    void (*shutdownGL)();

    const PalGLInfo* (*getGLInfo)();

    PalResult (*enumerateGLFBConfigs)(
        uint32_t* count, 
        PalGLFBConfig* configs);

    PalResult (*createGLContext)(
        const PalGLContextCreateInfo* info, 
        PalGLContext** outContext);

    void (*destroyGLContext)(PalGLContext* context);

    PalResult (*makeContextCurrent)(
        PalGLWindow* window, 
        PalGLContext* context);

    void* (*getGLProcAddress)(const char* name);

    PalResult (*swapBuffers)(
        PalGLWindow* window, 
        PalGLContext* context);

    void (*setSwapInterval)(int32_t interval);
    
    const PalBool* (*GetSupportedGLAPIs)(void* instance);
} OpenglBackend;

// ==================================================
// WGL
// ==================================================

#ifdef _WIN32
PalResult wglInitGL(
    PalGLAPI api, 
    void* instance, 
    const PalAllocator* allocator);

void wglShutdownGL();

const PalGLInfo* wglGetGLInfo();

PalResult wglEnumerateGLFBConfigs(
    uint32_t* count, 
    PalGLFBConfig* configs);

PalResult wglCreateGLContext(
    const PalGLContextCreateInfo* info, 
    PalGLContext** outContext);

void wglDestroyGLContext(PalGLContext* context);

PalResult wglMakeContextCurrent(
    PalGLWindow* window, 
    PalGLContext* context);

void* wglGetGLProcAddress(const char* name);

PalResult wglSwapBuffers(
    PalGLWindow* window, 
    PalGLContext* context);

void wglSetSwapInterval(int32_t interval);

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
PalResult eglInitGL(
    PalGLAPI api, 
    void* instance, 
    const PalAllocator* allocator);

void eglShutdownGL();

const PalGLInfo* eglGetGLInfo();

PalResult eglEnumerateGLFBConfigs(
    uint32_t* count, 
    PalGLFBConfig* configs);

PalResult eglCreateGLContext(
    const PalGLContextCreateInfo* info, 
    PalGLContext** outContext);

void eglDestroyGLContext(PalGLContext* context);

PalResult eglMakeContextCurrent(
    PalGLWindow* window, 
    PalGLContext* context);

void* eglGetGLProcAddress(const char* name);

PalResult eglSwapBuffers(
    PalGLWindow* window, 
    PalGLContext* context);

void eglSetSwapInterval(int32_t interval);

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

// clang-format on

#endif // _PAL_HAS_EGL

#endif // _PAL_OPENGL_BACKENDS_H