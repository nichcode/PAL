
/**
    PAL - Prime Abstraction Layer
    Copyright (C) 2025
    Licensed under the Zlib license. See LICENSE file in root.
 */

#ifndef _PAL_BACKENDS_OPENGL_H
#define _PAL_BACKENDS_OPENGL_H

#include "pal_opengl_helper.h"

// ==================================================
// WGL
// ==================================================

PalResult PAL_CALL wglInitGL();
void PAL_CALL wglShutdownGL();
const PalGLInfo* PAL_CALL wglGetGLInfo();
PalResult PAL_CALL wglEnumerateGLFBConfigs(int32_t*, PalGLFBConfig*);
PalResult PAL_CALL wglCreateGLContext(const PalGLContextCreateInfo*, PalGLContext**);
void PAL_CALL wglDestroyGLContext(PalGLContext*);
PalResult PAL_CALL wglMakeContextCurrent(PalGLWindow*, PalGLContext*);
void* PAL_CALL wglGetGLProcAddress(const char*);
PalResult PAL_CALL wglSwapBuffers(PalGLWindow*, PalGLContext*);
PalResult PAL_CALL wglSetSwapInterval(int32_t);

static Backend s_WglBackend = {
    .shutdownGL = wglShutdownGL,
    .getGLInfo = wglGetGLInfo,
    .enumerateGLFBConfigs = wglEnumerateGLFBConfigs,
    .createGLContext = wglCreateGLContext,
    .destroyGLContext = wglDestroyGLContext,
    .makeContextCurrent = wglMakeContextCurrent,
    .getGLProcAddress = wglGetGLProcAddress,
    .swapBuffers = wglSwapBuffers,
    .setSwapInterval = wglSetSwapInterval,
};

// ==================================================
// GLX
// ==================================================

PalResult PAL_CALL glxInitGL();
void PAL_CALL glxShutdownGL();
const PalGLInfo* PAL_CALL glxGetGLInfo();
PalResult PAL_CALL glxEnumerateGLFBConfigs(int32_t*, PalGLFBConfig*);
PalResult PAL_CALL glxCreateGLContext(const PalGLContextCreateInfo*, PalGLContext**);
void PAL_CALL glxDestroyGLContext(PalGLContext*);
PalResult PAL_CALL glxMakeContextCurrent(PalGLWindow*, PalGLContext*);
void* PAL_CALL glxGetGLProcAddress(const char*);
PalResult PAL_CALL glxSwapBuffers(PalGLWindow*, PalGLContext*);
PalResult PAL_CALL glxSetSwapInterval(int32_t);

static Backend s_GlxBackend = {
    .shutdownGL = glxShutdownGL,
    .getGLInfo = glxGetGLInfo,
    .enumerateGLFBConfigs = glxEnumerateGLFBConfigs,
    .createGLContext = glxCreateGLContext,
    .destroyGLContext = glxDestroyGLContext,
    .makeContextCurrent = glxMakeContextCurrent,
    .getGLProcAddress = glxGetGLProcAddress,
    .swapBuffers = glxSwapBuffers,
    .setSwapInterval = glxSetSwapInterval,
};

// ==================================================
// EGL
// ==================================================

PalResult PAL_CALL eglInitGL();
void PAL_CALL eglShutdownGL();
const PalGLInfo* PAL_CALL eglGetGLInfo();
PalResult PAL_CALL eglEnumerateGLFBConfigs(int32_t*, PalGLFBConfig*);
PalResult PAL_CALL eglCreateGLContext(const PalGLContextCreateInfo*, PalGLContext**);
void PAL_CALL eglDestroyGLContext(PalGLContext*);
PalResult PAL_CALL eglMakeContextCurrent(PalGLWindow*, PalGLContext*);
void* PAL_CALL eglGetGLProcAddress(const char*);
PalResult PAL_CALL eglSwapBuffers(PalGLWindow*, PalGLContext*);
PalResult PAL_CALL eglSetSwapInterval(int32_t);

static Backend s_EglBackend = {
    .shutdownGL = eglShutdownGL,
    .getGLInfo = eglGetGLInfo,
    .enumerateGLFBConfigs = eglEnumerateGLFBConfigs,
    .createGLContext = eglCreateGLContext,
    .destroyGLContext = eglDestroyGLContext,
    .makeContextCurrent = eglMakeContextCurrent,
    .getGLProcAddress = eglGetGLProcAddress,
    .swapBuffers = eglSwapBuffers,
    .setSwapInterval = eglSetSwapInterval,
};

#endif // _PAL_BACKENDS_OPENGL_H
