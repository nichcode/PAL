
/**

Copyright (C) 2025 Nicholas Agbo <agbonicholas04@gmail.com>

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

#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// ==================================================
// Typedefs, enums and structs
// ==================================================

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
// EGL header is not included
/* C++ / C typecast macros for special EGL handle values */
#if defined(__cplusplus)
#define EGL_CAST(type, value) (static_cast<type>(value))
#else
#define EGL_CAST(type, value) ((type)(value))
#endif

#define EGL_OPENGL_API 0x30A2
#define EGL_OPENGL_BIT 0x0008
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

typedef struct {
    bool used;
    EGLSurface surface;
    PalGLContext* context;
} ContextData;

typedef struct {
    bool initialized;
    Int32 maxContextData;
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

    void* handle;
    ContextData* contextData;
    EGLDisplay display;
    PalGLInfo info;
} GLLinux;

static GLLinux s_GL = {0};

// ==================================================
// Internal API
// ==================================================

static inline bool checkExtension(
    const char* extension,
    const char* extensions)
{
    const char* start = extensions;
    size_t extensionLen = strlen(extension);

    for (;;) {
        const char* where = nullptr;
        const char* terminator = nullptr;

        where = strstr(start, extension);
        if (!where) {
            return false;
        }

        // the extension was found, we find the terminator by adding the sizeof
        // the extension
        terminator = where + extensionLen;
        if (where == start || *(where - 1) == ' ') {
            if (*terminator == ' ' || *terminator == '\0') {
                return true;
            }
        }

        start = terminator;
    }
}

static ContextData* getFreeContextData()
{
    for (int i = 0; i < s_GL.maxContextData; ++i) {
        if (!s_GL.contextData[i].used) {
            s_GL.contextData[i].used = true;
            return &s_GL.contextData[i];
        }
    }

    // resize the data array
    // this will almost not reach here since most setups are 1-4 monitors
    ContextData* data = nullptr;
    int count = s_GL.maxContextData * 2; // double the size
    int freeIndex = s_GL.maxContextData + 1;
    data = palAllocate(s_GL.allocator, sizeof(ContextData) * count, 0);
    if (data) {
        memcpy(
            data,
            s_GL.contextData,
            s_GL.maxContextData * sizeof(ContextData));

        palFree(s_GL.allocator, s_GL.contextData);
        s_GL.contextData = data;
        s_GL.maxContextData = count;

        s_GL.contextData[freeIndex].used = true;
        return &s_GL.contextData[freeIndex];
    }
    return nullptr;
}

static ContextData* findContextData(PalGLContext* context)
{
    for (int i = 0; i < s_GL.maxContextData; ++i) {
        if (s_GL.contextData[i].used &&
            s_GL.contextData[i].context == context) {
            return &s_GL.contextData[i];
        }
    }
}

static void freeContextData(PalGLContext* context)
{
    for (int i = 0; i < s_GL.maxContextData; ++i) {
        if (s_GL.contextData[i].used &&
            s_GL.contextData[i].context == context) {
            s_GL.contextData[i].used = false;
        }
    }
}

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

    s_GL.maxContextData = 16; // initial size
    s_GL.contextData = palAllocate(
        s_GL.allocator,
        sizeof(ContextData) * s_GL.maxContextData,
        0);

    if (!s_GL.maxContextData) {
        return PAL_RESULT_OUT_OF_MEMORY;
    }

    s_GL.handle = dlopen("libEGL.so", RTLD_LAZY);
    if (!s_GL.handle) {
        return PAL_RESULT_PLATFORM_FAILURE;
    }

    // clang-format off

    s_GL.eglGetProcAddress = (eglGetProcAddressFn)dlsym(
        s_GL.handle, 
        "eglGetProcAddress");

    s_GL.eglCreateContext = (eglCreateContextFn)s_GL.eglGetProcAddress(
        "eglCreateContext");

    s_GL.eglDestroyContext = (eglDestroyContextFn)s_GL.eglGetProcAddress(
        "eglDestroyContext");

    s_GL.eglDestroySurface = (eglDestroySurfaceFn)s_GL.eglGetProcAddress(
        "eglDestroySurface");

    s_GL.eglMakeCurrent = (eglMakeCurrentFn)s_GL.eglGetProcAddress(
        "eglMakeCurrent");

    s_GL.eglSwapBuffers = (eglSwapBuffersFn)s_GL.eglGetProcAddress(
        "eglSwapBuffers");

    s_GL.eglSwapInterval = (eglSwapIntervalFn)s_GL.eglGetProcAddress(
        "eglSwapInterval");

    s_GL.eglInitialize = (eglInitializeFn)s_GL.eglGetProcAddress(
        "eglInitialize");

    s_GL.eglTerminate = (eglTerminateFn)s_GL.eglGetProcAddress("eglTerminate");

    s_GL.eglGetDisplay = (eglGetDisplayFn)s_GL.eglGetProcAddress(
        "eglGetDisplay");

    s_GL.eglCreatePbufferSurface = (eglCreatePbufferSurfaceFn)s_GL.eglGetProcAddress(
        "eglCreatePbufferSurface");

    s_GL.eglChooseConfig = (eglChooseConfigFn)s_GL.eglGetProcAddress(
        "eglChooseConfig");

    s_GL.eglGetConfigAttrib = (eglGetConfigAttribFn)s_GL.eglGetProcAddress(
        "eglGetConfigAttrib");

    s_GL.eglGetError = (eglGetErrorFn)s_GL.eglGetProcAddress("eglGetError");
    s_GL.eglBindAPI = (eglBindAPIFn)s_GL.eglGetProcAddress("eglBindAPI");

    s_GL.eglQueryString = (eglQueryStringFn)s_GL.eglGetProcAddress(
        "eglQueryString");

    s_GL.eglGetConfigs = (eglGetConfigsFn)s_GL.eglGetProcAddress(
        "eglGetConfigs");

    s_GL.eglCreateWindowSurface = (eglCreateWindowSurfaceFn)s_GL.eglGetProcAddress(
            "eglCreateWindowSurface");

    if (!s_GL.eglBindAPI              || 
        !s_GL.eglChooseConfig         || 
        !s_GL.eglCreateContext        ||
        !s_GL.eglCreatePbufferSurface || 
        !s_GL.eglDestroyContext       ||
        !s_GL.eglDestroySurface       || 
        !s_GL.eglGetConfigAttrib      ||
        !s_GL.eglGetDisplay           || 
        !s_GL.eglGetError             || 
        !s_GL.eglGetProcAddress       ||
        !s_GL.eglInitialize           || 
        !s_GL.eglMakeCurrent          || 
        !s_GL.eglSwapBuffers          ||
        !s_GL.eglSwapInterval         || 
        !s_GL.eglTerminate            || 
        !s_GL.eglQueryString          ||
        !s_GL.eglGetConfigs           || 
        !s_GL.eglCreateWindowSurface) {
        return PAL_RESULT_PLATFORM_FAILURE;
    }

    // create a dummy context
    if (!s_GL.eglBindAPI(EGL_OPENGL_API)) {
        return PAL_RESULT_PLATFORM_FAILURE;
    }

    EGLDisplay display = s_GL.eglGetDisplay(EGL_DEFAULT_DISPLAY);
    if (display == EGL_NO_DISPLAY) {
        return PAL_RESULT_PLATFORM_FAILURE;
    }

    if (!s_GL.eglInitialize(display, nullptr, nullptr)) {
        return PAL_RESULT_PLATFORM_FAILURE;
    }

    // use a simple FBConfig
    EGLConfig config;
    int numConfigs;
    EGLint attribs[] = {
        EGL_RENDERABLE_TYPE,
        EGL_OPENGL_BIT,
        EGL_SURFACE_TYPE,
        EGL_PBUFFER_BIT,
        EGL_NONE};

    EGLint type;
    s_GL.eglChooseConfig(display, attribs, &config, 1, &numConfigs);
    s_GL.eglGetConfigAttrib(display, config, EGL_RENDERABLE_TYPE, &type);
    if (!(type & EGL_OPENGL_BIT)) {
        // we must support opengl API (desktop)
        return PAL_RESULT_PLATFORM_FAILURE;
    }

    // Since we don't want to create dummy window to get the driver info
    // we use EGL_OPENGL_ES2_BIT to create without a window
    if (!(type & EGL_OPENGL_ES2_BIT)) {
        // FIXME: create a dummy window if EGL_OPENGL_ES2_BIT
        return PAL_RESULT_PLATFORM_FAILURE;
    }

    EGLSurface surface = EGL_NO_SURFACE;
    EGLint pBufferAttribs[] = {
        EGL_WIDTH, 1,
        EGL_HEIGHT, 1, 
        EGL_NONE};

    surface = s_GL.eglCreatePbufferSurface(
        display, 
        config, 
        pBufferAttribs);

    if (surface == EGL_NO_SURFACE) {
        return PAL_RESULT_PLATFORM_FAILURE;
    }

    EGLint contextAttrib[] = {
        EGL_CONTEXT_MAJOR_VERSION, 2, 
        EGL_CONTEXT_MINOR_VERSION, 1, 
        EGL_NONE};

    // create a dummy context
    EGLContext context = EGL_NO_CONTEXT;
    context = s_GL.eglCreateContext(
        display, 
        config, 
        EGL_NO_CONTEXT, 
        contextAttrib);

    if (!s_GL.eglMakeCurrent(display, surface, surface, context)) {
        return PAL_RESULT_PLATFORM_FAILURE;
    }

    s_GL.glGetString = (glGetStringFn)s_GL.eglGetProcAddress("glGetString");
    const char* version = (const char*)s_GL.glGetString(GL_VERSION);
    if (version) {
        sscanf(version, "%d.%d", &s_GL.info.major, &s_GL.info.minor);
    }

    const char* renderer = (const char*)s_GL.glGetString(GL_RENDERER);
    const char* vendor = (const char*)s_GL.glGetString(GL_VENDOR);
    strcpy(s_GL.info.vendor, vendor);
    strcpy(s_GL.info.version, version);
    strcpy(s_GL.info.graphicsCard, renderer);

    // EGL extensions can be queried without a bound context
    // we just do that over here after making the context current
    const char* extensions = s_GL.eglQueryString(display, EGL_EXTENSIONS);
    if (extensions) {
        // color space
        if (checkExtension("EGL_KHR_gl_colorspace", extensions)) {
            s_GL.info.extensions |= PAL_GL_EXTENSION_COLORSPACE_SRGB;
        }

        // create context
        if (checkExtension("EGL_KHR_create_context", extensions)) {
            s_GL.info.extensions |= PAL_GL_EXTENSION_CREATE_CONTEXT;
            s_GL.info.extensions |= PAL_GL_EXTENSION_CONTEXT_PROFILE;
        }

        // robustness
        if (checkExtension("EGL_EXT_create_context_robustness", extensions)) {
            s_GL.info.extensions |= PAL_GL_EXTENSION_ROBUSTNESS;
        }

        // no error
        if (checkExtension("EGL_KHR_create_context_no_error", extensions)) {
            s_GL.info.extensions |= PAL_GL_EXTENSION_NO_ERROR;
        }

        // flush control
        if (checkExtension("EGL_KHR_context_flush_control", extensions)) {
            s_GL.info.extensions |= PAL_GL_EXTENSION_FLUSH_CONTROL;
        }

        // swap control
        if (checkExtension("EGL_EXT_swap_control", extensions)) {
            s_GL.info.extensions |= PAL_GL_EXTENSION_FLUSH_CONTROL;
        }

        if (checkExtension("EGL_EXT_swap_control_tear", extensions)) {
            s_GL.info.extensions |= PAL_GL_EXTENSION_FLUSH_CONTROL;
        }
    }

    // part of the core API
    s_GL.info.extensions |= PAL_GL_EXTENSION_MULTISAMPLE;

    if (type & EGL_OPENGL_ES_BIT || 
        type & EGL_OPENGL_ES2_BIT ||
        type & EGL_OPENGL_ES3_BIT) {
        s_GL.info.extensions |= PAL_GL_EXTENSION_CONTEXT_PROFILE_ES2;
    }

    // check if we support the core swap interval
    if (s_GL.eglSwapInterval) {
        s_GL.info.extensions |= PAL_GL_EXTENSION_SWAP_CONTROL;
    }

    s_GL.eglMakeCurrent(
        display,
        EGL_NO_SURFACE,
        EGL_NO_SURFACE,
        EGL_NO_CONTEXT);

    // clang-format on

    s_GL.eglDestroyContext(display, context);
    s_GL.eglDestroySurface(display, surface);

    s_GL.allocator = allocator;
    s_GL.initialized = true;
    s_GL.display = display;
    return PAL_RESULT_SUCCESS;
}

void PAL_CALL palShutdownGL()
{
    if (!s_GL.initialized) {
        return;
    }

    palFree(s_GL.allocator, s_GL.contextData);
    s_GL.eglTerminate(s_GL.display);
    dlclose(s_GL.handle);
    s_GL.initialized = false;
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

    Int32 configCount = 0;
    Int32 maxConfigCount = 0;

    if (configs) {
        maxConfigCount = *count;
    }

    // get the number of configs and filter the ones for opengl desktop
    EGLint numConfigs = 0;
    if (!s_GL.eglGetConfigs(s_GL.display, nullptr, 0, &numConfigs)) {
        return PAL_RESULT_PLATFORM_FAILURE;
    }

    EGLint configSize = sizeof(EGLConfig) * numConfigs;
    EGLConfig* eglConfigs = palAllocate(s_GL.allocator, configSize, 0);
    if (!eglConfigs) {
        return PAL_RESULT_OUT_OF_MEMORY;
    }

    s_GL.eglGetConfigs(s_GL.display, eglConfigs, numConfigs, &numConfigs);
    for (int i = 0; i < numConfigs; i++) {
        // attributes we care about
        EGLint surfaceType = 0, renderable = 0;
        EGLint colorType = 0;

        EGLConfig config = eglConfigs[i];
        s_GL.eglGetConfigAttrib(
            s_GL.display,
            config,
            EGL_SURFACE_TYPE,
            &surfaceType);

        s_GL.eglGetConfigAttrib(
            s_GL.display,
            config,
            EGL_RENDERABLE_TYPE,
            &renderable);

        s_GL.eglGetConfigAttrib(
            s_GL.display,
            config,
            EGL_COLOR_BUFFER_TYPE,
            &colorType);

        // we need only opengl API configs
        if (colorType != EGL_RGB_BUFFER) {
            continue;
        }

        if (!(renderable & EGL_OPENGL_BIT)) {
            continue;
        }

        if (!(surfaceType & EGL_WINDOW_BIT)) {
            continue;
        }

        if (configs && configCount < maxConfigCount) {
            // get this only if user supplied an output buffer (PalGLFBConfig*)
            PalGLFBConfig* fbConfig = &configs[configCount];
            fbConfig->index = i;

            EGLint redBits, greenBits, blueBits, alphaBits;
            EGLint depthBits, stencilBits, samples;

            s_GL.eglGetConfigAttrib(
                s_GL.display,
                config,
                EGL_RED_SIZE,
                &redBits);

            s_GL.eglGetConfigAttrib(
                s_GL.display,
                config,
                EGL_GREEN_SIZE,
                &greenBits);

            s_GL.eglGetConfigAttrib(
                s_GL.display,
                config,
                EGL_BLUE_SIZE,
                &blueBits);

            s_GL.eglGetConfigAttrib(
                s_GL.display,
                config,
                EGL_ALPHA_SIZE,
                &alphaBits);

            s_GL.eglGetConfigAttrib(
                s_GL.display,
                config,
                EGL_DEPTH_SIZE,
                &depthBits);

            s_GL.eglGetConfigAttrib(
                s_GL.display,
                config,
                EGL_STENCIL_SIZE,
                &stencilBits);

            s_GL.eglGetConfigAttrib(
                s_GL.display,
                config,
                EGL_SAMPLES,
                &samples);

            if (samples == 0) {
                samples = 1;
            }

            fbConfig->redBits = redBits;
            fbConfig->greenBits = greenBits;
            fbConfig->blueBits = blueBits;
            fbConfig->alphaBits = alphaBits;
            fbConfig->depthBits = depthBits;
            fbConfig->stencilBits = stencilBits;
            fbConfig->samples = samples;

            // True for EGL_WINDOW_BIT
            fbConfig->doubleBuffer = true;
            fbConfig->stereo = false;

            if (s_GL.info.extensions & PAL_GL_EXTENSION_COLORSPACE_SRGB) {
                // since EGL does not have a bit to check SRGB support
                // we check if all the color bits are greater than or equal to 8
                if (fbConfig->redBits >= 8 && fbConfig->greenBits >= 8 &&
                    fbConfig->blueBits >= 8) {
                    fbConfig->sRGB = true;
                }
            } else {
                fbConfig->sRGB = false;
            }
        }
        configCount++;
    }

    palFree(s_GL.allocator, eglConfigs);
    if (!configs) {
        *count = configCount;
    }

    return PAL_RESULT_SUCCESS;
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

    if (!info || !outContext) {
        return PAL_RESULT_NULL_POINTER;
    }

    if (!info->window || !info->fbConfig) {
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

    ContextData* data = getFreeContextData();
    if (!data) {
        return PAL_RESULT_OUT_OF_MEMORY;
    }

    // we need to get the EGL config from the user supplied index
    EGLint numConfigs = 0;
    if (!s_GL.eglGetConfigs(s_GL.display, nullptr, 0, &numConfigs)) {
        return PAL_RESULT_PLATFORM_FAILURE;
    }

    EGLint configSize = sizeof(EGLConfig) * numConfigs;
    EGLConfig* eglConfigs = palAllocate(s_GL.allocator, configSize, 0);
    if (!eglConfigs) {
        return PAL_RESULT_OUT_OF_MEMORY;
    }

    s_GL.eglGetConfigs(s_GL.display, eglConfigs, numConfigs, &numConfigs);
    EGLConfig config = eglConfigs[info->fbConfig->index];

    EGLContext* share = nullptr;
    if (info->shareContext) {
        share = (EGLContext)info->shareContext;
    }

    Int32 attribs[40];
    Int32 index = 0;
    Int32 profile = 0;
    Int32 flags = 0;

    // set context attributes
    // the first element is the key and the second is the value
    // set version
    attribs[index++] = EGL_CONTEXT_MAJOR_VERSION_KHR; // key
    attribs[index++] = info->major;                   // value

    attribs[index++] = EGL_CONTEXT_MINOR_VERSION_KHR;
    attribs[index++] = info->minor;

    // set profile mask
    if (info->profile != PAL_GL_PROFILE_NONE) {
        attribs[index++] = EGL_CONTEXT_OPENGL_PROFILE_MASK_KHR;

        if (info->profile == PAL_GL_PROFILE_COMPATIBILITY) {
            profile = EGL_CONTEXT_OPENGL_COMPATIBILITY_PROFILE_BIT_KHR;
        } else if (info->profile == PAL_GL_PROFILE_CORE) {
            profile = EGL_CONTEXT_OPENGL_CORE_PROFILE_BIT_KHR;
        }

        attribs[index++] = info->profile;
    }

    // set forward flag
    if (info->forward) {
        flags |= EGL_CONTEXT_OPENGL_FORWARD_COMPATIBLE_BIT_KHR;
    }

    // set debug flag
    if (info->debug) {
        flags |= EGL_CONTEXT_OPENGL_DEBUG_BIT_KHR;
    }

    // set robustness
    if (info->reset != PAL_GL_CONTEXT_RESET_NONE) {
        flags |= EGL_CONTEXT_OPENGL_ROBUST_ACCESS_BIT_KHR;
        attribs[index++] = EGL_CONTEXT_OPENGL_RESET_NOTIFICATION_STRATEGY_KHR;

        if (info->reset == PAL_GL_CONTEXT_RESET_LOSE_CONTEXT) {
            attribs[index++] = EGL_LOSE_CONTEXT_ON_RESET_KHR;

        } else if (info->reset == PAL_GL_CONTEXT_RESET_NO_NOTIFICATION) {
            attribs[index++] = EGL_NO_RESET_NOTIFICATION_KHR;
        }
    }

    // set no error
    if (info->noError) {
        attribs[index++] = EGL_CONTEXT_OPENGL_NO_ERROR_KHR;
        attribs[index++] = true;
    }

    // release
    if (info->release != PAL_GL_RELEASE_BEHAVIOR_NONE) {
        attribs[index++] = EGL_CONTEXT_RELEASE_BEHAVIOR_KHR;
        attribs[index++] = EGL_CONTEXT_RELEASE_BEHAVIOR_FLUSH_KHR;
    }

    if (flags) {
        attribs[index++] = EGL_CONTEXT_FLAGS_KHR;
        attribs[index++] = flags;
    }
    attribs[index++] = EGL_NONE;

    // clang-format off
    // create context
    EGLContext context = s_GL.eglCreateContext(
        s_GL.display, 
        config, 
        EGL_NO_CONTEXT, 
        attribs);
    // clang-format on

    if (context == EGL_NO_CONTEXT) {
        EGLint error = s_GL.eglGetError();
        if (error == EGL_BAD_CONFIG) {
            return PAL_RESULT_INVALID_GL_FBCONFIG;

        } else if (error == EGL_BAD_ATTRIBUTE) {
            // we just return invalid version
            // it could be invalid profile
            return PAL_RESULT_INVALID_GL_VERSION;

        } else {
            return PAL_RESULT_PLATFORM_FAILURE;
        }
    }

    // create a window surface
    // The only attrib we want is colorspace
    EGLint surfaceAttribs[3];
    surfaceAttribs[0] = EGL_GL_COLORSPACE;
    if (info->fbConfig->sRGB) {
        surfaceAttribs[1] = EGL_GL_COLORSPACE_SRGB_KHR;
    } else {
        surfaceAttribs[1] = EGL_GL_COLORSPACE_LINEAR_KHR;
    }
    surfaceAttribs[2] = EGL_NONE;

    EGLSurface surface = s_GL.eglCreateWindowSurface(
        s_GL.display,
        config,
        (EGLNativeWindowType)info->window->window,
        surfaceAttribs);

    if (surface == EGL_NO_SURFACE) {
        EGLint error = s_GL.eglGetError();
        if (error == EGL_BAD_CONFIG) {
            return PAL_RESULT_INVALID_GL_FBCONFIG;

        } else {
            return PAL_RESULT_PLATFORM_FAILURE;
        }
    }

    palFree(s_GL.allocator, eglConfigs);
    data->context = (PalGLContext*)context;
    data->surface = surface;

    *outContext = (PalGLContext*)context;
    return PAL_RESULT_SUCCESS;
}

void PAL_CALL palDestroyGLContext(PalGLContext* context)
{
    if (s_GL.initialized && context) {
        ContextData* data = findContextData(context);
        if (data) {
            s_GL.eglDestroyContext(s_GL.display, (EGLContext)context);
            s_GL.eglDestroySurface(s_GL.display, data->surface);
            data->used = false;
        }
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

    if (context && glWindow) {
        ContextData* data = findContextData(context);
        if (!data) {
            return PAL_RESULT_INVALID_GL_CONTEXT;
        }

        EGLint ret = s_GL.eglMakeCurrent(
            s_GL.display,
            data->surface,
            data->surface,
            (EGLConfig)context);

        if (!ret) {
            EGLint error = s_GL.eglGetError();
            if (error == EGL_BAD_CONTEXT) {
                return PAL_RESULT_INVALID_GL_CONTEXT;

            } else if (error == EGL_BAD_SURFACE) {
                // since we always create a window surface
                return PAL_RESULT_INVALID_GL_WINDOW;

            } else {
                return PAL_RESULT_PLATFORM_FAILURE;
            }
        }

    } else if (!context && !glWindow) {
        s_GL.eglMakeCurrent(
            s_GL.display,
            EGL_NO_SURFACE,
            EGL_NO_SURFACE,
            EGL_NO_CONTEXT);
    }

    return PAL_RESULT_SUCCESS;
}

void* PAL_CALL palGLGetProcAddress(const char* name)
{
    if (!s_GL.initialized) {
        return nullptr;
    }

    return s_GL.eglGetProcAddress(name);
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

    ContextData* data = findContextData(context);
    if (!data) {
        return PAL_RESULT_INVALID_GL_CONTEXT;
    }

    if (!s_GL.eglSwapBuffers(s_GL.display, data->surface)) {
        EGLint error = s_GL.eglGetError();
        if (error == EGL_BAD_CONTEXT) {
            return PAL_RESULT_INVALID_GL_CONTEXT;

        } else if (error == EGL_BAD_SURFACE) {
            // since we always create a window surface
            return PAL_RESULT_INVALID_GL_WINDOW;

        } else {
            return PAL_RESULT_PLATFORM_FAILURE;
        }
    }

    return PAL_RESULT_SUCCESS;
}

PalResult PAL_CALL palSetSwapInterval(Int32 interval)
{
    if (!s_GL.initialized) {
        return PAL_RESULT_GL_NOT_INITIALIZED;
    }

    if (!s_GL.eglSwapInterval) {
        return PAL_RESULT_GL_EXTENSION_NOT_SUPPORTED;
    }

    s_GL.eglSwapInterval(s_GL.display, interval);
    return PAL_RESULT_SUCCESS;
}