
/**
    PAL - Prime Abstraction Layer
    Copyright (C) 2025
    Licensed under the Zlib license. See LICENSE file in root.
 */

#ifndef _PAL_VIDEO_EGL_H
#define _PAL_VIDEO_EGL_H

#include <stdint.h>

#define EGL_CAST(type, value) ((type)(value))
#define EGL_OPENGL_API 0x30A2
#define EGL_OPENGL_BIT 0x0008
#define EGL_OPENGL_ES_BIT 0x0001
#define EGL_OPENGL_ES_API 0x30A0
#define EGL_NO_CONTEXT EGL_CAST(EGLContext, 0)
#define EGL_NO_DISPLAY EGL_CAST(EGLDisplay, 0)
#define EGL_NO_SURFACE EGL_CAST(EGLSurface, 0)
#define EGL_NATIVE_VISUAL_ID 0x302E

typedef void* EGLConfig;
typedef void* EGLSurface;
typedef void* EGLContext;
typedef void* EGLDisplay;
typedef void* EGLNativeDisplayType;

typedef int32_t EGLint;
typedef unsigned int EGLBoolean;
typedef unsigned int EGLenum;

typedef void* (*eglGetProcAddressFn)(const char*);

typedef EGLBoolean (*eglInitializeFn)(
    EGLDisplay,
    EGLint*,
    EGLint*);

typedef EGLBoolean (*eglTerminateFn)(EGLDisplay);

typedef EGLDisplay (*eglGetDisplayFn)(void*);

typedef EGLBoolean (*eglChooseConfigFn)(
    EGLDisplay,
    const EGLint*,
    EGLConfig*,
    EGLint,
    EGLint*);

typedef EGLBoolean (*eglGetConfigAttribFn)(
    EGLDisplay,
    EGLConfig,
    EGLint,
    EGLint*);

typedef EGLint (*eglGetErrorFn)(void);

typedef EGLBoolean (*eglBindAPIFn)(EGLenum);

typedef EGLBoolean (*eglGetConfigsFn)(
    EGLDisplay,
    EGLConfig*,
    EGLint,
    EGLint*);

typedef struct {
    void* handle;
    eglInitializeFn initialize;
    eglTerminateFn terminate;
    eglGetDisplayFn getDisplay;
    eglChooseConfigFn chooseConfig;
    eglGetConfigAttribFn getConfigAttrib;
    eglGetErrorFn getError;
    eglBindAPIFn bindAPI;
    eglGetConfigsFn getConfigs;
} VideoEGL;

extern VideoEGL s_VideoEgl;

#endif // _PAL_VIDEO_EGL_H