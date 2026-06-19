
/**
    PAL - Prime Abstraction Layer
    Copyright (C) 2025
    Licensed under the Zlib license. See LICENSE file in root.
 */

#ifndef _PAL_OPENGL_LINUX_H
#define _PAL_OPENGL_LINUX_H
#ifdef __linux__

#include "pal/pal_opengl.h"
#include <errno.h>

#ifndef GL_VENDOR
#define GL_VENDOR 0x1F00
#define GL_RENDERER 0x1F01
#define GL_VERSION 0x1F02
#define GL_EXTENSIONS 0x1F03
#endif // GL_VENDOR

typedef unsigned int GLenum;
typedef unsigned char GLubyte;
typedef int32_t EGLint;
typedef unsigned int EGLBoolean;
typedef unsigned int EGLenum;

typedef unsigned long int khronos_uintptr_t;
typedef khronos_uintptr_t EGLNativeWindowType;

typedef void* EGLConfig;
typedef void* EGLSurface;
typedef void* EGLContext;
typedef void* EGLDisplay;
typedef void* EGLNativeDisplayType;

#ifndef EGL_OPENGL_API
#define EGL_CAST(type, value) ((type)(value))
#define EGL_OPENGL_API 0x30A2
#define EGL_OPENGL_BIT 0x0008
#define EGL_OPENGL_ES_BIT 0x0001
#define EGL_OPENGL_ES_API 0x30A0
#define EGL_CLIENT_APIS   0x308D
#define EGL_NO_CONTEXT EGL_CAST(EGLContext, 0)
#define EGL_NO_DISPLAY EGL_CAST(EGLDisplay, 0)
#define EGL_NO_SURFACE EGL_CAST(EGLSurface, 0)
#define EGL_DEFAULT_DISPLAY EGL_CAST(EGLNativeDisplayType, 0)
#define EGL_GL_COLORSPACE 0x309D
#define EGL_COLORSPACE_sRGB 0x3089
#define EGL_COLORSPACE_LINEAR 0x308A
#define EGL_COLOR_BUFFER_TYPE 0x303F
#define EGL_OPENGL_ES_BIT 0x0001
#define EGL_OPENGL_ES_API 0x30A0
#define EGL_RENDERABLE_TYPE 0x3040
#define EGL_RGB_BUFFER 0x308E
#define EGL_ALPHA_SIZE 0x3021
#define EGL_BAD_ATTRIBUTE 0x3004
#define EGL_BAD_CONFIG 0x3005
#define EGL_BAD_CONTEXT 0x3006
#define EGL_BAD_SURFACE 0x300D
#define EGL_EXTENSIONS 0x3055
#define EGL_GREEN_SIZE 0x3023
#define EGL_MAX_PBUFFER_WIDTH 0x302C
#define EGL_NATIVE_VISUAL_ID 0x302E
#define EGL_NONE 0x3038
#define EGL_HEIGHT 0x3056
#define EGL_WIDTH 0x3057
#define EGL_OPENGL_ES_BIT 0x0001
#define EGL_OPENGL_ES2_BIT 0x0004
#define EGL_OPENGL_ES3_BIT 0x00000040

#define EGL_PBUFFER_BIT 0x0001
#define EGL_RED_SIZE 0x3024
#define EGL_BLUE_SIZE 0x3022
#define EGL_DEPTH_SIZE 0x3025
#define EGL_SAMPLES 0x3031
#define EGL_STENCIL_SIZE 0x3026
#define EGL_SURFACE_TYPE 0x3033
#define EGL_WINDOW_BIT 0x0004
#define EGL_CONTEXT_MAJOR_VERSION 0x3098
#define EGL_CONTEXT_MINOR_VERSION 0x30FB
#define EGL_CONTEXT_CLIENT_VERSION 0x3098

#define EGL_CONTEXT_MAJOR_VERSION_KHR 0x3098
#define EGL_CONTEXT_MINOR_VERSION_KHR 0x30FB
#define EGL_CONTEXT_FLAGS_KHR 0x30FC
#define EGL_CONTEXT_OPENGL_PROFILE_MASK_KHR 0x30FD
#define EGL_CONTEXT_OPENGL_RESET_NOTIFICATION_STRATEGY_KHR 0x31BD
#define EGL_NO_RESET_NOTIFICATION_KHR 0x31BE
#define EGL_LOSE_CONTEXT_ON_RESET_KHR 0x31BF
#define EGL_CONTEXT_OPENGL_DEBUG_BIT_KHR 0x00000001
#define EGL_CONTEXT_OPENGL_FORWARD_COMPATIBLE_BIT_KHR 0x00000002
#define EGL_CONTEXT_OPENGL_ROBUST_ACCESS_BIT_KHR 0x00000004
#define EGL_CONTEXT_OPENGL_CORE_PROFILE_BIT_KHR 0x00000001
#define EGL_CONTEXT_OPENGL_COMPATIBILITY_PROFILE_BIT_KHR 0x00000002
#define EGL_OPENGL_ES3_BIT_KHR 0x00000040
#define EGL_CONTEXT_OPENGL_NO_ERROR_KHR 0x31B3
#define EGL_CONTEXT_RELEASE_BEHAVIOR_KHR 0x2097
#define EGL_CONTEXT_RELEASE_BEHAVIOR_FLUSH_KHR 0x2098
#define EGL_GL_COLORSPACE_KHR 0x309D
#define EGL_GL_COLORSPACE_SRGB_KHR 0x3089
#define EGL_GL_COLORSPACE_LINEAR_KHR 0x308A
#endif // EGL header

typedef void* (*eglGetProcAddressFn)(const char*);

typedef EGLContext (*eglCreateContextFn)(
    EGLDisplay,
    EGLConfig,
    EGLContext,
    const EGLint*);

typedef EGLBoolean (*eglDestroyContextFn)(
    EGLDisplay,
    EGLContext);

typedef EGLBoolean (*eglMakeCurrentFn)(
    EGLDisplay,
    EGLSurface,
    EGLSurface,
    EGLContext);

typedef EGLBoolean (*eglSwapBuffersFn)(
    EGLDisplay,
    EGLSurface);

typedef EGLBoolean (*eglSwapIntervalFn)(
    EGLDisplay,
    EGLint);

typedef EGLBoolean (*eglInitializeFn)(
    EGLDisplay,
    EGLint*,
    EGLint*);

typedef EGLBoolean (*eglTerminateFn)(EGLDisplay);

typedef EGLDisplay (*eglGetDisplayFn)(EGLNativeDisplayType);

typedef EGLBoolean (*eglDestroySurfaceFn)(
    EGLDisplay,
    EGLSurface);

typedef EGLSurface (*eglCreatePbufferSurfaceFn)(
    EGLDisplay,
    EGLConfig,
    const EGLint*);

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

typedef const char* (*eglQueryStringFn)(
    EGLDisplay,
    EGLint);

typedef EGLBoolean (*eglGetConfigsFn)(
    EGLDisplay,
    EGLConfig*,
    EGLint,
    EGLint*);

typedef EGLSurface (*eglCreateWindowSurfaceFn)(
    EGLDisplay,
    EGLConfig,
    EGLNativeWindowType,
    const EGLint*);

typedef const GLubyte* (*glGetStringFn)(GLenum);
typedef void(PAL_GL_APIENTRY* glClearFn)(uint32_t);

typedef void(PAL_GL_APIENTRY* glClearColorFn)(
    float,
    float,
    float,
    float);

typedef struct {
    PalBool used;
    EGLSurface surface;
    PalGLContext* context;
} ContextData;

typedef struct {
    PalBool initialized;
    int32_t maxContextData;
    EGLenum apiType;
    int apiTypeBit;
    const PalAllocator* allocator;

    eglGetProcAddressFn eglGetProcAddress;
    eglCreateContextFn eglCreateContext;
    eglDestroyContextFn eglDestroyContext;
    eglMakeCurrentFn eglMakeCurrent;
    eglSwapBuffersFn eglSwapBuffers;
    eglSwapIntervalFn eglSwapInterval;
    eglInitializeFn eglInitialize;
    eglTerminateFn eglTerminate;
    eglGetDisplayFn eglGetDisplay;
    eglDestroySurfaceFn eglDestroySurface;
    eglCreatePbufferSurfaceFn eglCreatePbufferSurface;
    eglChooseConfigFn eglChooseConfig;
    eglGetConfigAttribFn eglGetConfigAttrib;
    eglGetErrorFn eglGetError;
    eglBindAPIFn eglBindAPI;
    eglQueryStringFn eglQueryString;
    eglGetConfigsFn eglGetConfigs;
    eglCreateWindowSurfaceFn eglCreateWindowSurface;

    glGetStringFn glGetString;
    glClearColorFn glClearColor;
    glClearFn glClear;

    void* handle;
    void* instance;
    ContextData* contextData;
    EGLDisplay display;
    PalGLInfo info;
} GLLinux;

extern GLLinux s_GL;

ContextData* getFreeContextData();
ContextData* findContextData(PalGLContext* context);
void freeContextData(PalGLContext* context);

#endif // __linux__
#endif // _PAL_OPENGL_LINUX_H