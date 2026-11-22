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

#ifndef VIDEO_EGL_H
#define VIDEO_EGL_H

#ifndef _WIN32

#include "pal2/core/library.h"

#define EGL_OPENGL_API 0x30A2
#define EGL_OPENGL_BIT 0x0008
#define EGL_OPENGL_ES_BIT 0x0001
#define EGL_OPENGL_ES_API 0x30A0
#define EGL_NO_CONTEXT ((EGLContext)nullptr)
#define EGL_NO_DISPLAY ((EGLDisplay)nullptr)
#define EGL_NO_SURFACE ((EGLSurface)nullptr)
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
typedef EGLBoolean (*eglInitializeFn)(EGLDisplay, EGLint*, EGLint*);
typedef EGLBoolean (*eglTerminateFn)(EGLDisplay);
typedef EGLDisplay (*eglGetDisplayFn)(void*);

typedef EGLBoolean (*eglChooseConfigFn)(EGLDisplay, const EGLint*, EGLConfig*, EGLint, EGLint*);
typedef EGLBoolean (*eglGetConfigAttribFn)(EGLDisplay, EGLConfig, EGLint, EGLint*);
typedef EGLint (*eglGetErrorFn)(void);
typedef EGLBoolean (*eglBindAPIFn)(EGLenum);
typedef EGLBoolean (*eglGetConfigsFn)(EGLDisplay, EGLConfig*, EGLint, EGLint*);

typedef struct VideoEGL {
    PalLibrary* libegl;
    eglInitializeFn initialize;
    eglTerminateFn terminate;
    eglGetDisplayFn getDisplay;
    eglChooseConfigFn chooseConfig;
    eglGetConfigAttribFn getConfigAttrib;
    eglGetErrorFn getError;
    eglBindAPIFn bindAPI;
    eglGetConfigsFn getConfigs;
} VideoEGL;

extern VideoEGL g_VideoEgl;

void loadVideoEGL(void);
void unloadVideoEGL(void);

#endif // _WIN32
#endif // VIDEO_EGL_H