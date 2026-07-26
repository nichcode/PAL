
/**
    PAL - Prime Abstraction Layer
    Copyright (C) 2025
    Licensed under the Zlib license. See LICENSE file in root.
 */

#include "pal_platform.h"

#if _PAL_HAS_EGL
#include "pal_egl.h"

static ContextData* getFreeContextData()
{
    for (int i = 0; i < s_Egl.maxContextData; ++i) {
        if (!s_Egl.contextData[i].used) {
            s_Egl.contextData[i].used = PAL_TRUE;
            return &s_Egl.contextData[i];
        }
    }

    // resize the data array
    // this will almost not reach here since most setups are 1-4 monitors
    ContextData* data = nullptr;
    int count = s_Egl.maxContextData * 2; // double the size
    int freeIndex = s_Egl.maxContextData + 1;
    data = palAllocate(s_Egl.allocator, sizeof(ContextData) * count, 0);
    if (data) {
        memcpy(data, s_Egl.contextData, s_Egl.maxContextData * sizeof(ContextData));

        palFree(s_Egl.allocator, s_Egl.contextData);
        s_Egl.contextData = data;
        s_Egl.maxContextData = count;

        s_Egl.contextData[freeIndex].used = PAL_TRUE;
        return &s_Egl.contextData[freeIndex];
    }
    return nullptr;
}

static ContextData* findContextData(PalGLContext* context)
{
    for (int i = 0; i < s_Egl.maxContextData; ++i) {
        if (s_Egl.contextData[i].used && s_Egl.contextData[i].context == context) {
            return &s_Egl.contextData[i];
        }
    }
    return nullptr;
}

static void freeContextData(PalGLContext* context)
{
    for (int i = 0; i < s_Egl.maxContextData; ++i) {
        if (s_Egl.contextData[i].used && s_Egl.contextData[i].context == context) {
            s_Egl.contextData[i].used = PAL_FALSE;
        }
    }
}

PalResult eglCreateGLContext(
    const PalGLContextCreateInfo* info,
    PalGLContext** outContext)
{
    if (!info || !outContext) {
        return PAL_RESULT_CODE_INVALID_ARGUMENT;
    }

    if (!info->window || !info->fbConfig) {
        return PAL_RESULT_CODE_INVALID_ARGUMENT;
    }

    // check if the window was created with the same instance
    // the opengl system is using
    if (info->window->instance != s_Egl.instance) {
        return PAL_RESULT_CODE_INVALID_HANDLE;
    }

    // check support for requested features
    if (info->profile != PAL_GL_PROFILE_NONE) {
        if (!(s_Egl.info.extensions & PAL_GL_EXTENSION_CONTEXT_PROFILE)) {
            return PAL_RESULT_CODE_FEATURE_NOT_SUPPORTED;
        }
    }

    if (info->forward) {
        if (!(s_Egl.info.extensions & PAL_GL_EXTENSION_CREATE_CONTEXT)) {
            return PAL_RESULT_CODE_FEATURE_NOT_SUPPORTED;
        }
    }

    if (info->reset != PAL_GL_CONTEXT_RESET_NONE) {
        if (!(s_Egl.info.extensions & PAL_GL_EXTENSION_ROBUSTNESS)) {
            return PAL_RESULT_CODE_FEATURE_NOT_SUPPORTED;
        }
    }

    if (info->noError) {
        if (!(s_Egl.info.extensions & PAL_GL_EXTENSION_NO_ERROR)) {
            return PAL_RESULT_CODE_FEATURE_NOT_SUPPORTED;
        }
    }

    if (info->release != PAL_GL_RELEASE_BEHAVIOR_NONE) {
        if (!(s_Egl.info.extensions & PAL_GL_EXTENSION_FLUSH_CONTROL)) {
            return PAL_RESULT_CODE_FEATURE_NOT_SUPPORTED;
        }
    }

    // check version
    // clang-format off
    PalBool valid = info->major < s_Egl.info.major ||
                    (info->major == s_Egl.info.major && info->minor <= s_Egl.info.minor);
    // clang-format on

    if (!valid) {
        return PAL_RESULT_CODE_INVALID_ARGUMENT;
    }

    ContextData* data = getFreeContextData();
    if (!data) {
        return PAL_RESULT_CODE_OUT_OF_MEMORY;
    }

    // we need to get the EGL config from the user supplied index
    EGLint numConfigs = 0;
    if (!s_Egl.getConfigs(s_Egl.display, nullptr, 0, &numConfigs)) {
        return palMakeResult(
            PAL_RESULT_CODE_PLATFORM_FAILURE,
            PAL_RESULT_SOURCE_EGL,
            s_Egl.getError());
    }

    EGLint configSize = sizeof(EGLConfig) * numConfigs;
    EGLConfig* eglConfigs = palAllocate(s_Egl.allocator, configSize, 0);
    if (!eglConfigs) {
        return PAL_RESULT_CODE_OUT_OF_MEMORY;
    }

    s_Egl.getConfigs(s_Egl.display, eglConfigs, numConfigs, &numConfigs);
    EGLConfig config = eglConfigs[info->fbConfig->index];
    EGLContext* share = nullptr;
    if (info->shareContext) {
        share = (EGLContext)info->shareContext;
    } else {
        share = EGL_NO_CONTEXT;
    }

    int32_t attribs[40];
    int32_t index = 0;
    int32_t profile = 0;
    int32_t flags = 0;

    // set context attributes
    // the first element is the key and the second is the value
    if (s_Egl.apiType == EGL_OPENGL_API) {
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

    } else {
        attribs[index++] = EGL_CONTEXT_CLIENT_VERSION;
        // our configs support EGL_OPENGL_ES2_BIT and EGL_OPENGL_ES3_BIT
        attribs[index++] = info->major;
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
        attribs[index++] = PAL_TRUE;
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

    // create context
    EGLContext context = s_Egl.createContext(s_Egl.display, config, share, attribs);
    if (context == EGL_NO_CONTEXT) {
        EGLint error = s_Egl.getError();
        if (error == EGL_BAD_CONFIG || error == EGL_BAD_ATTRIBUTE) {
            return palMakeResult(PAL_RESULT_CODE_INVALID_ARGUMENT, PAL_RESULT_SOURCE_EGL, error);

        } else {
            return palMakeResult(PAL_RESULT_CODE_PLATFORM_FAILURE, PAL_RESULT_SOURCE_EGL, error);
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

    EGLSurface surface = s_Egl.createWindowSurface(
        s_Egl.display,
        config,
        (EGLNativeWindowType)info->window->window,
        surfaceAttribs);

    if (surface == EGL_NO_SURFACE) {
        EGLint error = s_Egl.getError();
        if (error == EGL_BAD_CONFIG) {
            return palMakeResult(PAL_RESULT_CODE_INVALID_ARGUMENT, PAL_RESULT_SOURCE_EGL, error);

        } else {
            return palMakeResult(PAL_RESULT_CODE_PLATFORM_FAILURE, PAL_RESULT_SOURCE_EGL, error);
        }
    }

    // make the context current and swap for the first time
    // this will make the window visible
    if (s_Egl.makeCurrent(s_Egl.display, surface, surface, context)) {
        s_Egl.glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        s_Egl.glClear(0x00004000);
        s_Egl.swapBuffers(s_Egl.display, surface);

        // revert
        s_Egl.makeCurrent(s_Egl.display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
    }

    palFree(s_Egl.allocator, eglConfigs);
    data->context = (PalGLContext*)context;
    data->surface = surface;

    *outContext = (PalGLContext*)context;
    return PAL_RESULT_SUCCESS;
}

void eglDestroyGLContext(PalGLContext* context)
{
    ContextData* data = findContextData(context);
    // make it not current if it was current
    s_Egl.makeCurrent(s_Egl.display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
    s_Egl.destroyContext(s_Egl.display, (EGLContext)context);
    s_Egl.destroySurface(s_Egl.display, data->surface);
    data->used = PAL_FALSE;
}

PalResult eglMakeContextCurrent(
    PalGLWindow* glWindow,
    PalGLContext* context)
{
    if (context && glWindow) {
        ContextData* data = findContextData(context);
        if (!data) {
            return PAL_RESULT_CODE_INVALID_HANDLE;
        }

        EGLint ret;
        ret = s_Egl.makeCurrent(s_Egl.display, data->surface, data->surface, (EGLConfig)context);
        if (!ret) {
            EGLint error = s_Egl.getError();
            if (error == EGL_BAD_CONTEXT || error == EGL_BAD_SURFACE) {
                return palMakeResult(PAL_RESULT_CODE_INVALID_HANDLE, PAL_RESULT_SOURCE_EGL, error);

            } else {
                return palMakeResult(
                    PAL_RESULT_CODE_PLATFORM_FAILURE,
                    PAL_RESULT_SOURCE_EGL,
                    error);
            }
        }

    } else if (!context && !glWindow) {
        s_Egl.makeCurrent(s_Egl.display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
    }

    return PAL_RESULT_SUCCESS;
}

PalResult eglSwapBuffers(
    PalGLWindow* glWindow,
    PalGLContext* context)
{
    ContextData* data = findContextData(context);
    if (!data) {
        return PAL_RESULT_CODE_INVALID_HANDLE;
    }

    if (!s_Egl.swapBuffers(s_Egl.display, data->surface)) {
        EGLint error = s_Egl.getError();
        if (error == EGL_BAD_CONTEXT || error == EGL_BAD_SURFACE) {
            return palMakeResult(PAL_RESULT_CODE_INVALID_HANDLE, PAL_RESULT_SOURCE_EGL, error);

        } else {
            return palMakeResult(PAL_RESULT_CODE_PLATFORM_FAILURE, PAL_RESULT_SOURCE_EGL, error);
        }
    }

    return PAL_RESULT_SUCCESS;
}

#endif // _PAL_HAS_EGL