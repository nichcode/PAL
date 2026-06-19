
/**
    PAL - Prime Abstraction Layer
    Copyright (C) 2025
    Licensed under the Zlib license. See LICENSE file in root.
 */

#ifdef __linux__
#define _GNU_SOURCE
#define _POSIX_C_SOURCE 200112L
#include "pal_opengl_linux.h"
#include "opengl/pal_opengl_shared.h"
#include "pal_shared.h"

#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <EGL/egl.h>

GLLinux s_GL = {0};
static PalBool s_SupportedAPIs[2] = {0};

ContextData* getFreeContextData()
{
    for (int i = 0; i < s_GL.maxContextData; ++i) {
        if (!s_GL.contextData[i].used) {
            s_GL.contextData[i].used = PAL_TRUE;
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
        memcpy(data, s_GL.contextData, s_GL.maxContextData * sizeof(ContextData));

        palFree(s_GL.allocator, s_GL.contextData);
        s_GL.contextData = data;
        s_GL.maxContextData = count;

        s_GL.contextData[freeIndex].used = PAL_TRUE;
        return &s_GL.contextData[freeIndex];
    }
    return nullptr;
}

ContextData* findContextData(PalGLContext* context)
{
    for (int i = 0; i < s_GL.maxContextData; ++i) {
        if (s_GL.contextData[i].used && s_GL.contextData[i].context == context) {
            return &s_GL.contextData[i];
        }
    }
}

void freeContextData(PalGLContext* context)
{
    for (int i = 0; i < s_GL.maxContextData; ++i) {
        if (s_GL.contextData[i].used && s_GL.contextData[i].context == context) {
            s_GL.contextData[i].used = PAL_FALSE;
        }
    }
}

PalResult PAL_CALL palInitGL(
    PalGLAPI api,
    void* instance,
    const PalAllocator* allocator)
{
    if (s_GL.initialized) {
        return PAL_RESULT_SUCCESS;
    }

    if (!instance) {
        palMakeResult(
            PAL_RESULT_INVALID_ARGUMENT, 
            PAL_RESULT_SOURCE_LINUX, 
            errno);
    }

    if (allocator && (!allocator->allocate || !allocator->free)) {
        palMakeResult(
            PAL_RESULT_INVALID_ARGUMENT, 
            PAL_RESULT_SOURCE_LINUX, 
            errno);
    }

    s_GL.maxContextData = 16; // initial size
    s_GL.contextData = palAllocate(s_GL.allocator, sizeof(ContextData) * s_GL.maxContextData, 0);
    if (!s_GL.maxContextData) {
        palMakeResult(
            PAL_RESULT_OUT_OF_MEMORY, 
            PAL_RESULT_SOURCE_LINUX, 
            errno);
    }

    s_GL.handle = dlopen("libEGL.so", RTLD_LAZY);
    if (!s_GL.handle) {
        palMakeResult(
            PAL_RESULT_PLATFORM_FAILURE, 
            PAL_RESULT_SOURCE_LINUX, 
            errno);
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

    s_GL.glClearColor = (glClearColorFn)s_GL.eglGetProcAddress("glClearColor");
    s_GL.glClear = (glClearFn)s_GL.eglGetProcAddress("glClear");

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

        palMakeResult(
            PAL_RESULT_PLATFORM_FAILURE, 
            PAL_RESULT_SOURCE_LINUX, 
            errno);
    }
    // clang-format on

    // get api type
    if (api == PAL_GL_API_OPENGL) {
        s_GL.apiType = EGL_OPENGL_API;
        s_GL.apiTypeBit = EGL_OPENGL_BIT;

    } else {
        s_GL.apiType = EGL_OPENGL_ES_API;
        s_GL.apiTypeBit = EGL_OPENGL_ES2_BIT; // default
    }

    if (!s_GL.eglBindAPI(s_GL.apiType)) {
        palMakeResult(
            PAL_RESULT_PLATFORM_FAILURE, 
            PAL_RESULT_SOURCE_LINUX, 
            errno);
    }

    EGLDisplay display = s_GL.eglGetDisplay(instance);
    EGLDisplay* tmpDisplay = EGL_NO_DISPLAY;
    if (display == EGL_NO_DISPLAY) {
        palMakeResult(
            PAL_RESULT_PLATFORM_FAILURE, 
            PAL_RESULT_SOURCE_LINUX, 
            errno);
    }

    if (!s_GL.eglInitialize(display, nullptr, nullptr)) {
        palMakeResult(
            PAL_RESULT_PLATFORM_FAILURE, 
            PAL_RESULT_SOURCE_LINUX, 
            errno);
    }

    // use a simple FBConfig
    EGLConfig config;
    int numConfigs;
    EGLint type;

    // clang-format off
    EGLint attribs[] = {
        EGL_RENDERABLE_TYPE, 
        s_GL.apiTypeBit, 
        EGL_SURFACE_TYPE, 
        EGL_PBUFFER_BIT, 
        EGL_NONE
    };
    // clang-format on

    s_GL.eglChooseConfig(display, attribs, &config, 1, &numConfigs);
    if (!config || numConfigs == 0) {
        // API bind type might not support puffer
        // create a default display and use that
        tmpDisplay = s_GL.eglGetDisplay(EGL_DEFAULT_DISPLAY);
        if (display == EGL_NO_DISPLAY) {
            palMakeResult(
                PAL_RESULT_PLATFORM_FAILURE, 
                PAL_RESULT_SOURCE_LINUX, 
                errno);
        }

        if (!s_GL.eglInitialize(tmpDisplay, nullptr, nullptr)) {
            palMakeResult(
                PAL_RESULT_PLATFORM_FAILURE, 
                PAL_RESULT_SOURCE_LINUX, 
                errno);
        }
    } else {
        // set the tmp display to our display
        tmpDisplay = display;
    }

    s_GL.eglChooseConfig(tmpDisplay, attribs, &config, 1, &numConfigs);
    if (!config) {
        palMakeResult(
            PAL_RESULT_PLATFORM_FAILURE, 
            PAL_RESULT_SOURCE_LINUX, 
            errno);
    }

    s_GL.eglGetConfigAttrib(tmpDisplay, config, EGL_RENDERABLE_TYPE, &type);
    if (!(type & s_GL.apiTypeBit)) {
        // we must support the required API
        palMakeResult(
            PAL_RESULT_PLATFORM_FAILURE, 
            PAL_RESULT_SOURCE_LINUX, 
            errno);
    }

    EGLSurface surface = EGL_NO_SURFACE;
    EGLint pBufferAttribs[] = { EGL_WIDTH, 1, EGL_HEIGHT, 1, EGL_NONE };
    surface = s_GL.eglCreatePbufferSurface(tmpDisplay, config, pBufferAttribs);
    if (surface == EGL_NO_SURFACE) {
        palMakeResult(
            PAL_RESULT_PLATFORM_FAILURE, 
            PAL_RESULT_SOURCE_LINUX, 
            errno);
    }

    // create a dummy context
    EGLContext context = EGL_NO_CONTEXT;
    if (s_GL.apiType == EGL_OPENGL_API) {
        // clang-format off
        EGLint contextAttrib[] = {
            EGL_CONTEXT_MAJOR_VERSION, 
            2, 
            EGL_CONTEXT_MINOR_VERSION, 
            1, 
            EGL_NONE
        };
        // clang-format on

        context = s_GL.eglCreateContext(tmpDisplay, config, EGL_NO_CONTEXT, contextAttrib);

    } else {
        EGLint contextAttrib[] = { EGL_CONTEXT_CLIENT_VERSION, 2, EGL_NONE };
        context = s_GL.eglCreateContext(tmpDisplay, config, EGL_NO_CONTEXT, contextAttrib);
    }

    if (context == EGL_NO_CONTEXT) {
        palMakeResult(
            PAL_RESULT_PLATFORM_FAILURE, 
            PAL_RESULT_SOURCE_LINUX, 
            errno);
    }

    s_GL.eglMakeCurrent(tmpDisplay, surface, surface, context);
    s_GL.glGetString = (glGetStringFn)s_GL.eglGetProcAddress("glGetString");

    const char* version = (const char*)s_GL.glGetString(GL_VERSION);
    if (version) {
        if (s_GL.apiType == EGL_OPENGL_API) {
            sscanf(version, "%d.%d", &s_GL.info.major, &s_GL.info.minor);
        } else {
            sscanf(version + 10, "%d.%d", &s_GL.info.major, &s_GL.info.minor);
        }
    }

    const char* renderer = (const char*)s_GL.glGetString(GL_RENDERER);
    const char* vendor = (const char*)s_GL.glGetString(GL_VENDOR);
    strcpy(s_GL.info.vendor, vendor);
    strcpy(s_GL.info.version, version);
    strcpy(s_GL.info.graphicsCard, renderer);

    // EGL extensions can be queried without a bound context
    // we just do that over here after making the context current
    const char* extensions = s_GL.eglQueryString(tmpDisplay, EGL_EXTENSIONS);
    if (extensions) {
        // color space
        if (checkString("EGL_KHR_gl_colorspace", extensions)) {
            s_GL.info.extensions |= PAL_GL_EXTENSION_COLORSPACE_SRGB;
        }

        // create context
        if (checkString("EGL_KHR_create_context", extensions)) {
            s_GL.info.extensions |= PAL_GL_EXTENSION_CREATE_CONTEXT;
            s_GL.info.extensions |= PAL_GL_EXTENSION_CONTEXT_PROFILE;
        }

        // robustness
        if (checkString("EGL_EXT_create_context_robustness", extensions)) {
            s_GL.info.extensions |= PAL_GL_EXTENSION_ROBUSTNESS;
        }

        // no error
        if (checkString("EGL_KHR_create_context_no_error", extensions)) {
            s_GL.info.extensions |= PAL_GL_EXTENSION_NO_ERROR;
        }

        // flush control
        if (checkString("EGL_KHR_context_flush_control", extensions)) {
            s_GL.info.extensions |= PAL_GL_EXTENSION_FLUSH_CONTROL;
        }

        // swap control
        if (checkString("EGL_EXT_swap_control", extensions)) {
            s_GL.info.extensions |= PAL_GL_EXTENSION_FLUSH_CONTROL;
        }

        if (checkString("EGL_EXT_swap_control_tear", extensions)) {
            s_GL.info.extensions |= PAL_GL_EXTENSION_FLUSH_CONTROL;
        }
    }

    // part of the core API
    s_GL.info.extensions |= PAL_GL_EXTENSION_MULTISAMPLE;
    if (type & EGL_OPENGL_ES_BIT || type & EGL_OPENGL_ES2_BIT) {
        s_GL.info.extensions |= PAL_GL_EXTENSION_CONTEXT_PROFILE_ES2;
    }

    // check if we support the core swap interval
    if (s_GL.eglSwapInterval) {
        s_GL.info.extensions |= PAL_GL_EXTENSION_SWAP_CONTROL;
    }

    if (s_GL.apiType != EGL_OPENGL_API) {
        if (s_GL.info.extensions & PAL_GL_EXTENSION_CONTEXT_PROFILE) {
            s_GL.info.extensions &= ~PAL_GL_EXTENSION_CONTEXT_PROFILE;
            s_GL.info.extensions &= ~PAL_GL_EXTENSION_CONTEXT_PROFILE_ES2;
        }

        if (type & EGL_OPENGL_ES3_BIT) {
            s_GL.apiTypeBit = EGL_OPENGL_ES3_BIT;
        }
    }

    s_GL.eglMakeCurrent(tmpDisplay, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
    s_GL.eglDestroyContext(tmpDisplay, context);
    s_GL.eglDestroySurface(tmpDisplay, surface);

    s_GL.info.api = api;
    s_GL.info.backend = PAL_GL_BACKEND_EGL;
    s_GL.allocator = allocator;
    s_GL.initialized = PAL_TRUE;

    if (tmpDisplay != display) {
        // tmpDisplay is a seperate display
        // terminate it
        s_GL.eglTerminate(tmpDisplay);
    }

    s_GL.display = display;
    return PAL_RESULT_SUCCESS;
}

void PAL_CALL palShutdownGL()
{
    if (!s_GL.initialized) {
        return;
    }

    palFree(s_GL.allocator, s_GL.contextData);
    if (s_GL.display) {
        s_GL.eglTerminate(s_GL.display);
    }

    dlclose(s_GL.handle);
    s_GL.initialized = PAL_FALSE;
}

const PalGLInfo* PAL_CALL palGetGLInfo()
{
    if (!s_GL.initialized) {
        return nullptr;
    }
    return &s_GL.info;
}

PalResult PAL_CALL palEnumerateGLFBConfigs(
    int32_t* count,
    PalGLFBConfig* configs)
{
    if (!s_GL.initialized) {
        palMakeResult(
            PAL_RESULT_NOT_INITIALIZED, 
            PAL_RESULT_SOURCE_LINUX, 
            errno);
    }

    if (!count || count == 0 && configs) {
        palMakeResult(
            PAL_RESULT_INVALID_ARGUMENT, 
            PAL_RESULT_SOURCE_LINUX, 
            errno);
    }

    int32_t configCount = 0;
    int32_t maxConfigCount = 0;
    if (configs) {
        maxConfigCount = *count;
    }

    // get the number of configs and filter the ones for opengl desktop
    EGLint numConfigs = 0;
    if (!s_GL.eglGetConfigs(s_GL.display, nullptr, 0, &numConfigs)) {
        palMakeResult(
            PAL_RESULT_PLATFORM_FAILURE, 
            PAL_RESULT_SOURCE_LINUX, 
            errno);
    }

    EGLint configSize = sizeof(EGLConfig) * numConfigs;
    EGLConfig* eglConfigs = palAllocate(s_GL.allocator, configSize, 0);
    if (!eglConfigs) {
        palMakeResult(
            PAL_RESULT_OUT_OF_MEMORY, 
            PAL_RESULT_SOURCE_LINUX, 
            errno);
    }

    s_GL.eglGetConfigs(s_GL.display, eglConfigs, numConfigs, &numConfigs);
    for (int i = 0; i < numConfigs; i++) {
        // attributes we care about
        EGLint surfaceType = 0, renderable = 0;
        EGLint colorType = 0;

        EGLConfig config = eglConfigs[i];
        s_GL.eglGetConfigAttrib(s_GL.display, config, EGL_SURFACE_TYPE, &surfaceType);
        s_GL.eglGetConfigAttrib(s_GL.display, config, EGL_RENDERABLE_TYPE, &renderable);
        s_GL.eglGetConfigAttrib(s_GL.display, config, EGL_COLOR_BUFFER_TYPE, &colorType);

        // we need only opengl API configs
        if (colorType != EGL_RGB_BUFFER) {
            continue;
        }

        if (s_GL.apiType == EGL_OPENGL_ES3_BIT) {
            // EGL_OPENGL_ES2_BIT
            if (!(renderable & EGL_OPENGL_ES2_BIT) && !(renderable & EGL_OPENGL_ES3_BIT)) {
                continue;
            }

        } else if (s_GL.apiType == EGL_OPENGL_ES2_BIT) {
            if (!(renderable & EGL_OPENGL_ES2_BIT)) {
                continue;
            }

        } else {
            if (!(renderable & EGL_OPENGL_BIT)) {
                continue;
            }
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

            s_GL.eglGetConfigAttrib(s_GL.display, config, EGL_RED_SIZE, &redBits);
            s_GL.eglGetConfigAttrib(s_GL.display, config, EGL_GREEN_SIZE, &greenBits);
            s_GL.eglGetConfigAttrib(s_GL.display, config, EGL_BLUE_SIZE, &blueBits);
            s_GL.eglGetConfigAttrib(s_GL.display, config, EGL_ALPHA_SIZE, &alphaBits);
            s_GL.eglGetConfigAttrib(s_GL.display, config, EGL_DEPTH_SIZE, &depthBits);
            s_GL.eglGetConfigAttrib(s_GL.display, config, EGL_STENCIL_SIZE, &stencilBits);
            s_GL.eglGetConfigAttrib(s_GL.display, config, EGL_SAMPLES, &samples);
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
            fbConfig->doubleBuffer = PAL_TRUE;
            fbConfig->stereo = PAL_FALSE;

            fbConfig->sRGB = PAL_FALSE;
            if (s_GL.info.extensions & PAL_GL_EXTENSION_COLORSPACE_SRGB) {
                // since EGL does not have a bit to check SRGB support
                // we check if all the color bits are greater than or equal to 8
                if (fbConfig->redBits >= 8 && fbConfig->greenBits >= 8 && fbConfig->blueBits >= 8) {
                    fbConfig->sRGB = PAL_TRUE;
                }
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

void* PAL_CALL palGetGLProcAddress(const char* name)
{
    if (!s_GL.initialized) {
        return nullptr;
    }

    return s_GL.eglGetProcAddress(name);
}

PalResult PAL_CALL palSetSwapInterval(int32_t interval)
{
    if (!s_GL.initialized) {
        palMakeResult(
            PAL_RESULT_NOT_INITIALIZED, 
            PAL_RESULT_SOURCE_LINUX, 
            errno);
    }

    if (!s_GL.eglSwapInterval) {
        palMakeResult(
            PAL_RESULT_FEATURE_NOT_SUPPORTED, 
            PAL_RESULT_SOURCE_LINUX, 
            errno);
    }

    s_GL.eglSwapInterval(s_GL.display, interval);
    return PAL_RESULT_SUCCESS;
}

const PalBool* PAL_CALL palGetSupportedGLAPIs(void* instance)
{
    if (!instance) {
        return nullptr;
    }

    void* handle = dlopen("libEGL.so", RTLD_LAZY);
    if (!handle) {
        return nullptr;
    }

    // clang-format off
    eglGetProcAddressFn getProcAddress = (eglGetProcAddressFn)dlsym(handle, "eglGetProcAddress");
    eglInitializeFn initialize = (eglInitializeFn)getProcAddress("eglInitialize");
    eglTerminateFn terminate = (eglTerminateFn)getProcAddress("eglTerminate");
    eglGetDisplayFn getDisplay = (eglGetDisplayFn)getProcAddress("eglGetDisplay");
    eglQueryStringFn queryString = (eglQueryStringFn)getProcAddress("eglQueryString");
    if (!getProcAddress || !initialize || !terminate || !getDisplay || !queryString) {
        return nullptr;
    }
    // clang-format on

    EGLDisplay display = getDisplay(instance);
    if (display == EGL_NO_DISPLAY) {
        return nullptr;
    }

    if (!initialize(display, nullptr, nullptr)) {
        return nullptr;
    }

    const char* apis = queryString(display, EGL_CLIENT_APIS);
    if (apis) {
        if (checkString("OpenGL", apis)) {
            s_SupportedAPIs[PAL_GL_API_OPENGL] = PAL_TRUE;
        } else {
            s_SupportedAPIs[PAL_GL_API_OPENGL] = PAL_FALSE;
        }

        if (checkString("OpenGL_ES", apis)) {
            s_SupportedAPIs[PAL_GL_API_OPENGL_ES] = PAL_TRUE;
        } else {
            s_SupportedAPIs[PAL_GL_API_OPENGL_ES] = PAL_FALSE;
        }
    }

    terminate(display);
    dlclose(handle);
    return s_SupportedAPIs;
}

#endif // __linux__