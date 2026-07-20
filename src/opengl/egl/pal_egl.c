
/**
    PAL - Prime Abstraction Layer
    Copyright (C) 2025
    Licensed under the Zlib license. See LICENSE file in root.
 */

#include "pal_platform.h"

#if _PAL_HAS_EGL
#include "opengl/pal_opengl_shared.h"
#include "pal_egl.h"
#include <dlfcn.h>
#include <errno.h>
#include <stdio.h>

EGL s_Egl = {0};
static PalBool s_SupportedAPIs[2] = {0};

PalResult PAL_CALL eglInitGL(
    PalGLAPI api,
    void* instance,
    const PalAllocator* allocator)
{
    s_Egl.maxContextData = 16; // initial size
    s_Egl.contextData = palAllocate(s_Egl.allocator, sizeof(ContextData) * s_Egl.maxContextData, 0);

    if (!s_Egl.maxContextData) {
        return PAL_RESULT_CODE_OUT_OF_MEMORY;
    }

    s_Egl.handle = dlopen("libEGL.so", RTLD_LAZY);
    if (!s_Egl.handle) {
        return palMakeResult(PAL_RESULT_CODE_PLATFORM_FAILURE, PAL_RESULT_SOURCE_POSIX, errno);
    }

    // clang-format off
    s_Egl.getProcAddress = (eglGetProcAddressFn)dlsym(
        s_Egl.handle,
        "eglGetProcAddress");

    s_Egl.createPbufferSurface = (eglCreatePbufferSurfaceFn)s_Egl.getProcAddress(
        "eglCreatePbufferSurface");

    s_Egl.createWindowSurface = (eglCreateWindowSurfaceFn)s_Egl.getProcAddress(
            "eglCreateWindowSurface");

    s_Egl.createContext = (eglCreateContextFn)s_Egl.getProcAddress("eglCreateContext");
    s_Egl.destroyContext = (eglDestroyContextFn)s_Egl.getProcAddress("eglDestroyContext");
    s_Egl.destroySurface = (eglDestroySurfaceFn)s_Egl.getProcAddress("eglDestroySurface");
    s_Egl.makeCurrent = (eglMakeCurrentFn)s_Egl.getProcAddress("eglMakeCurrent");
    s_Egl.swapBuffers = (eglSwapBuffersFn)s_Egl.getProcAddress("eglSwapBuffers");
    s_Egl.swapInterval = (eglSwapIntervalFn)s_Egl.getProcAddress("eglSwapInterval");
    s_Egl.initialize = (eglInitializeFn)s_Egl.getProcAddress("eglInitialize");
    s_Egl.terminate = (eglTerminateFn)s_Egl.getProcAddress("eglTerminate");
    s_Egl.getDisplay = (eglGetDisplayFn)s_Egl.getProcAddress("eglGetDisplay");
    s_Egl.chooseConfig = (eglChooseConfigFn)s_Egl.getProcAddress("eglChooseConfig");

    s_Egl.getConfigAttrib = (eglGetConfigAttribFn)s_Egl.getProcAddress("eglGetConfigAttrib");
    s_Egl.getError = (eglGetErrorFn)s_Egl.getProcAddress("eglGetError");
    s_Egl.bindAPI = (eglBindAPIFn)s_Egl.getProcAddress("eglBindAPI");
    s_Egl.queryString = (eglQueryStringFn)s_Egl.getProcAddress("eglQueryString");
    s_Egl.getConfigs = (eglGetConfigsFn)s_Egl.getProcAddress("eglGetConfigs");
    s_Egl.glClearColor = (glClearColorFn)s_Egl.getProcAddress("glClearColor");
    s_Egl.glClear = (glClearFn)s_Egl.getProcAddress("glClear");

    if (!s_Egl.bindAPI              ||
        !s_Egl.chooseConfig         ||
        !s_Egl.createContext        ||
        !s_Egl.createPbufferSurface ||
        !s_Egl.destroyContext       ||
        !s_Egl.destroySurface       ||
        !s_Egl.getConfigAttrib      ||
        !s_Egl.getDisplay           ||
        !s_Egl.getError             ||
        !s_Egl.getProcAddress       ||
        !s_Egl.initialize           ||
        !s_Egl.makeCurrent          ||
        !s_Egl.swapBuffers          ||
        !s_Egl.swapInterval         ||
        !s_Egl.terminate            ||
        !s_Egl.queryString          ||
        !s_Egl.getConfigs           ||
        !s_Egl.createWindowSurface) {
        return PAL_RESULT_CODE_PLATFORM_FAILURE;
    }
    // clang-format on

    // get api type
    if (api == PAL_GL_API_OPENGL) {
        s_Egl.apiType = EGL_OPENGL_API;
        s_Egl.apiTypeBit = EGL_OPENGL_BIT;

    } else {
        s_Egl.apiType = EGL_OPENGL_ES_API;
        s_Egl.apiTypeBit = EGL_OPENGL_ES2_BIT; // default
    }

    EGLint error = 0;
    if (!s_Egl.bindAPI(s_Egl.apiType)) {
        error = s_Egl.getError();
        return palMakeResult(PAL_RESULT_CODE_PLATFORM_FAILURE, PAL_RESULT_SOURCE_EGL, error);
    }

    EGLDisplay display = s_Egl.getDisplay(instance);
    EGLDisplay* tmpDisplay = EGL_NO_DISPLAY;
    if (display == EGL_NO_DISPLAY) {
        error = s_Egl.getError();
        return palMakeResult(PAL_RESULT_CODE_PLATFORM_FAILURE, PAL_RESULT_SOURCE_EGL, error);
    }

    if (!s_Egl.initialize(display, nullptr, nullptr)) {
        error = s_Egl.getError();
        return palMakeResult(PAL_RESULT_CODE_PLATFORM_FAILURE, PAL_RESULT_SOURCE_EGL, error);
    }

    // use a simple FBConfig
    EGLConfig config;
    int numConfigs;
    EGLint type;

    // clang-format off
    EGLint attribs[] = {
        EGL_RENDERABLE_TYPE, 
        s_Egl.apiTypeBit, 
        EGL_SURFACE_TYPE, 
        EGL_PBUFFER_BIT, 
        EGL_NONE
    };
    // clang-format on

    s_Egl.chooseConfig(display, attribs, &config, 1, &numConfigs);
    if (!config || numConfigs == 0) {
        // API bind type might not support puffer
        // create a default display and use that
        tmpDisplay = s_Egl.getDisplay(EGL_DEFAULT_DISPLAY);
        if (display == EGL_NO_DISPLAY) {
            error = s_Egl.getError();
            return palMakeResult(PAL_RESULT_CODE_PLATFORM_FAILURE, PAL_RESULT_SOURCE_EGL, error);
        }

        if (!s_Egl.initialize(tmpDisplay, nullptr, nullptr)) {
            error = s_Egl.getError();
            return palMakeResult(PAL_RESULT_CODE_PLATFORM_FAILURE, PAL_RESULT_SOURCE_EGL, error);
        }
    } else {
        // set the tmp display to our display
        tmpDisplay = display;
    }

    s_Egl.chooseConfig(tmpDisplay, attribs, &config, 1, &numConfigs);
    if (!config) {
        error = s_Egl.getError();
        return palMakeResult(PAL_RESULT_CODE_PLATFORM_FAILURE, PAL_RESULT_SOURCE_EGL, error);
    }

    s_Egl.getConfigAttrib(tmpDisplay, config, EGL_RENDERABLE_TYPE, &type);
    if (!(type & s_Egl.apiTypeBit)) {
        // we must support the required API
        error = s_Egl.getError();
        return palMakeResult(PAL_RESULT_CODE_PLATFORM_FAILURE, PAL_RESULT_SOURCE_EGL, error);
    }

    EGLSurface surface = EGL_NO_SURFACE;
    EGLint pBufferAttribs[] = {EGL_WIDTH, 1, EGL_HEIGHT, 1, EGL_NONE};
    surface = s_Egl.createPbufferSurface(tmpDisplay, config, pBufferAttribs);
    if (surface == EGL_NO_SURFACE) {
        error = s_Egl.getError();
        return palMakeResult(PAL_RESULT_CODE_PLATFORM_FAILURE, PAL_RESULT_SOURCE_EGL, error);
    }

    // create a dummy context
    EGLContext context = EGL_NO_CONTEXT;
    if (s_Egl.apiType == EGL_OPENGL_API) {
        // clang-format off
        EGLint contextAttrib[] = {
            EGL_CONTEXT_MAJOR_VERSION, 
            2, 
            EGL_CONTEXT_MINOR_VERSION, 
            1, 
            EGL_NONE
        };
        // clang-format on
        context = s_Egl.createContext(tmpDisplay, config, EGL_NO_CONTEXT, contextAttrib);

    } else {
        EGLint contextAttrib[] = {EGL_CONTEXT_CLIENT_VERSION, 2, EGL_NONE};
        context = s_Egl.createContext(tmpDisplay, config, EGL_NO_CONTEXT, contextAttrib);
    }

    if (context == EGL_NO_CONTEXT) {
        error = s_Egl.getError();
        return palMakeResult(PAL_RESULT_CODE_PLATFORM_FAILURE, PAL_RESULT_SOURCE_EGL, error);
    }

    s_Egl.makeCurrent(tmpDisplay, surface, surface, context);
    s_Egl.glGetString = (glGetStringFn)s_Egl.getProcAddress("glGetString");
    const char* version = (const char*)s_Egl.glGetString(GL_VERSION);
    if (version) {
        if (s_Egl.apiType == EGL_OPENGL_API) {
            sscanf(version, "%d.%d", &s_Egl.info.major, &s_Egl.info.minor);
        } else {
            sscanf(version + 10, "%d.%d", &s_Egl.info.major, &s_Egl.info.minor);
        }
    }

    const char* renderer = (const char*)s_Egl.glGetString(GL_RENDERER);
    const char* vendor = (const char*)s_Egl.glGetString(GL_VENDOR);
    strcpy(s_Egl.info.vendor, vendor);
    strcpy(s_Egl.info.version, version);
    strcpy(s_Egl.info.graphicsCard, renderer);

    // EGL extensions can be queried without a bound context
    // we just do that over here after making the context current
    const char* extensions = s_Egl.queryString(tmpDisplay, EGL_EXTENSIONS);
    if (extensions) {
        // color space
        if (checkString("EGL_KHR_gl_colorspace", extensions)) {
            s_Egl.info.extensions |= PAL_GL_EXTENSION_COLORSPACE_SRGB;
        }

        // create context
        if (checkString("EGL_KHR_create_context", extensions)) {
            s_Egl.info.extensions |= PAL_GL_EXTENSION_CREATE_CONTEXT;
            s_Egl.info.extensions |= PAL_GL_EXTENSION_CONTEXT_PROFILE;
        }

        // robustness
        if (checkString("EGL_EXT_create_context_robustness", extensions)) {
            s_Egl.info.extensions |= PAL_GL_EXTENSION_ROBUSTNESS;
        }

        // no error
        if (checkString("EGL_KHR_create_context_no_error", extensions)) {
            s_Egl.info.extensions |= PAL_GL_EXTENSION_NO_ERROR;
        }

        // flush control
        if (checkString("EGL_KHR_context_flush_control", extensions)) {
            s_Egl.info.extensions |= PAL_GL_EXTENSION_FLUSH_CONTROL;
        }

        // swap control
        if (checkString("EGL_EXT_swap_control", extensions)) {
            s_Egl.info.extensions |= PAL_GL_EXTENSION_FLUSH_CONTROL;
        }

        if (checkString("EGL_EXT_swap_control_tear", extensions)) {
            s_Egl.info.extensions |= PAL_GL_EXTENSION_FLUSH_CONTROL;
        }
    }

    // part of the core API
    s_Egl.info.extensions |= PAL_GL_EXTENSION_MULTISAMPLE;
    if (type & EGL_OPENGL_ES_BIT || type & EGL_OPENGL_ES2_BIT) {
        s_Egl.info.extensions |= PAL_GL_EXTENSION_CONTEXT_PROFILE_ES2;
    }

    // check if we support the core swap interval
    if (s_Egl.swapInterval) {
        s_Egl.info.extensions |= PAL_GL_EXTENSION_SWAP_CONTROL;
    }

    if (s_Egl.apiType != EGL_OPENGL_API) {
        if (s_Egl.info.extensions & PAL_GL_EXTENSION_CONTEXT_PROFILE) {
            s_Egl.info.extensions &= ~PAL_GL_EXTENSION_CONTEXT_PROFILE;
            s_Egl.info.extensions &= ~PAL_GL_EXTENSION_CONTEXT_PROFILE_ES2;
        }

        if (type & EGL_OPENGL_ES3_BIT) {
            s_Egl.apiTypeBit = EGL_OPENGL_ES3_BIT;
        }
    }

    s_Egl.makeCurrent(tmpDisplay, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
    s_Egl.destroyContext(tmpDisplay, context);
    s_Egl.destroySurface(tmpDisplay, surface);

    s_Egl.info.api = api;
    s_Egl.info.backend = PAL_GL_BACKEND_EGL;
    s_Egl.allocator = allocator;

    if (tmpDisplay != display) {
        // tmpDisplay is a seperate display
        // terminate it
        s_Egl.terminate(tmpDisplay);
    }

    s_Egl.display = display;
    s_Egl.instance = instance;
    return PAL_RESULT_SUCCESS;
}

void PAL_CALL eglShutdownGL()
{
    palFree(s_Egl.allocator, s_Egl.contextData);
    if (s_Egl.display) {
        s_Egl.terminate(s_Egl.display);
    }
    dlclose(s_Egl.handle);
}

const PalGLInfo* PAL_CALL eglGetGLInfo()
{
    return &s_Egl.info;
}

PalResult PAL_CALL eglEnumerateGLFBConfigs(
    uint32_t* count,
    PalGLFBConfig* configs)
{
    int32_t configCount = 0;
    int32_t maxConfigCount = 0;
    if (configs) {
        maxConfigCount = *count;
    }

    // get the number of configs and filter the ones for opengl desktop
    EGLint numConfigs = 0;
    EGLint error = 0;
    if (!s_Egl.getConfigs(s_Egl.display, nullptr, 0, &numConfigs)) {
        error = s_Egl.getError();
        return palMakeResult(PAL_RESULT_CODE_PLATFORM_FAILURE, PAL_RESULT_SOURCE_EGL, error);
    }

    EGLint configSize = sizeof(EGLConfig) * numConfigs;
    EGLConfig* eglConfigs = palAllocate(s_Egl.allocator, configSize, 0);
    if (!eglConfigs) {
        return PAL_RESULT_CODE_OUT_OF_MEMORY;
    }

    s_Egl.getConfigs(s_Egl.display, eglConfigs, numConfigs, &numConfigs);
    for (int i = 0; i < numConfigs; i++) {
        // attributes we care about
        EGLint surfaceType = 0, renderable = 0;
        EGLint colorType = 0;

        EGLConfig config = eglConfigs[i];
        s_Egl.getConfigAttrib(s_Egl.display, config, EGL_SURFACE_TYPE, &surfaceType);
        s_Egl.getConfigAttrib(s_Egl.display, config, EGL_RENDERABLE_TYPE, &renderable);
        s_Egl.getConfigAttrib(s_Egl.display, config, EGL_COLOR_BUFFER_TYPE, &colorType);

        // we need only opengl API configs
        if (colorType != EGL_RGB_BUFFER) {
            continue;
        }

        if (s_Egl.apiType == EGL_OPENGL_ES3_BIT) {
            // EGL_OPENGL_ES2_BIT
            if (!(renderable & EGL_OPENGL_ES2_BIT) && !(renderable & EGL_OPENGL_ES3_BIT)) {
                continue;
            }

        } else if (s_Egl.apiType == EGL_OPENGL_ES2_BIT) {
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

            s_Egl.getConfigAttrib(s_Egl.display, config, EGL_RED_SIZE, &redBits);
            s_Egl.getConfigAttrib(s_Egl.display, config, EGL_GREEN_SIZE, &greenBits);
            s_Egl.getConfigAttrib(s_Egl.display, config, EGL_BLUE_SIZE, &blueBits);
            s_Egl.getConfigAttrib(s_Egl.display, config, EGL_ALPHA_SIZE, &alphaBits);
            s_Egl.getConfigAttrib(s_Egl.display, config, EGL_DEPTH_SIZE, &depthBits);
            s_Egl.getConfigAttrib(s_Egl.display, config, EGL_STENCIL_SIZE, &stencilBits);
            s_Egl.getConfigAttrib(s_Egl.display, config, EGL_SAMPLES, &samples);
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
            if (s_Egl.info.extensions & PAL_GL_EXTENSION_COLORSPACE_SRGB) {
                // since EGL does not have a bit to check SRGB support
                // we check if all the color bits are greater than or equal to 8
                if (fbConfig->redBits >= 8 && fbConfig->greenBits >= 8 && fbConfig->blueBits >= 8) {
                    fbConfig->sRGB = PAL_TRUE;
                }
            }
        }
        configCount++;
    }

    palFree(s_Egl.allocator, eglConfigs);
    if (!configs) {
        *count = configCount;
    }

    return PAL_RESULT_SUCCESS;
}

void* PAL_CALL eglGetGLProcAddress(const char* name)
{
    return s_Egl.getProcAddress(name);
}

void PAL_CALL eglSetSwapInterval(int32_t interval)
{
    s_Egl.swapInterval(s_Egl.display, interval);
}

const PalBool* PAL_CALL eglGetSupportedGLAPIs(void* instance)
{
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

#endif // _PAL_HAS_EGL