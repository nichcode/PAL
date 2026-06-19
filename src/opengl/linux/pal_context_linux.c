
/**
    PAL - Prime Abstraction Layer
    Copyright (C) 2025
    Licensed under the Zlib license. See LICENSE file in root.
 */

#ifdef __linux__
#define _GNU_SOURCE
#define _POSIX_C_SOURCE 200112L

#include "pal_opengl_linux.h"
#include "pal_shared.h"

PalResult PAL_CALL palCreateGLContext(
    const PalGLContextCreateInfo* info,
    PalGLContext** outContext)
{
    if (!s_GL.initialized) {
        palMakeResult(
            PAL_RESULT_NOT_INITIALIZED, 
            PAL_RESULT_SOURCE_LINUX, 
            errno);
    }

    if (!info || !outContext) {
        palMakeResult(
            PAL_RESULT_INVALID_ARGUMENT, 
            PAL_RESULT_SOURCE_LINUX, 
            errno);
    }

    if (!info->window || !info->fbConfig) {
        palMakeResult(
            PAL_RESULT_INVALID_ARGUMENT, 
            PAL_RESULT_SOURCE_LINUX, 
            errno);
    }

    // check if the window was created with the same display
    // the opengl system is using
    if (info->window->display != s_GL.instance) {
        palMakeResult(
            PAL_RESULT_INVALID_HANDLE, 
            PAL_RESULT_SOURCE_LINUX, 
            errno);
    }

    // check support for requested features
    if (info->profile != PAL_GL_PROFILE_NONE) {
        if (!(s_GL.info.extensions & PAL_GL_EXTENSION_CONTEXT_PROFILE)) {
            palMakeResult(
                PAL_RESULT_FEATURE_NOT_SUPPORTED, 
                PAL_RESULT_SOURCE_LINUX, 
                errno);
        }
    }

    if (info->forward) {
        if (!(s_GL.info.extensions & PAL_GL_EXTENSION_CREATE_CONTEXT)) {
            palMakeResult(
                PAL_RESULT_FEATURE_NOT_SUPPORTED, 
                PAL_RESULT_SOURCE_LINUX, 
                errno);
        }
    }

    if (info->reset != PAL_GL_CONTEXT_RESET_NONE) {
        if (!(s_GL.info.extensions & PAL_GL_EXTENSION_ROBUSTNESS)) {
            palMakeResult(
                PAL_RESULT_FEATURE_NOT_SUPPORTED, 
                PAL_RESULT_SOURCE_LINUX, 
                errno);
        }
    }

    if (info->noError) {
        if (!(s_GL.info.extensions & PAL_GL_EXTENSION_NO_ERROR)) {
            palMakeResult(
                PAL_RESULT_FEATURE_NOT_SUPPORTED, 
                PAL_RESULT_SOURCE_LINUX, 
                errno);
        }
    }

    if (info->release != PAL_GL_RELEASE_BEHAVIOR_NONE) {
        if (!(s_GL.info.extensions & PAL_GL_EXTENSION_FLUSH_CONTROL)) {
            palMakeResult(
                PAL_RESULT_FEATURE_NOT_SUPPORTED, 
                PAL_RESULT_SOURCE_LINUX, 
                errno);
        }
    }

    // check version
    PalBool valid = info->major < s_GL.info.major ||
                 (info->major == s_GL.info.major && info->minor <= s_GL.info.minor);

    if (!valid) {
        palMakeResult(
            PAL_RESULT_INVALID_ARGUMENT, 
            PAL_RESULT_SOURCE_LINUX, 
            errno);
    }

    ContextData* data = getFreeContextData();
    if (!data) {
        palMakeResult(
            PAL_RESULT_OUT_OF_MEMORY, 
            PAL_RESULT_SOURCE_LINUX, 
            errno);
    }

    // we need to get the EGL config from the user supplied index
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
    if (s_GL.apiType == EGL_OPENGL_API) {
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
    EGLContext context = s_GL.eglCreateContext(s_GL.display, config, share, attribs);
    if (context == EGL_NO_CONTEXT) {
        EGLint error = s_GL.eglGetError();
        if (error == EGL_BAD_CONFIG) {
            palMakeResult(
                PAL_RESULT_INVALID_ARGUMENT, 
                PAL_RESULT_SOURCE_LINUX, 
                errno);

        } else if (error == EGL_BAD_ATTRIBUTE) {
            // we just return invalid version
            // it could be invalid profile
            palMakeResult(
                PAL_RESULT_INVALID_ARGUMENT, 
                PAL_RESULT_SOURCE_LINUX, 
                errno);

        } else {
            palMakeResult(
                PAL_RESULT_PLATFORM_FAILURE, 
                PAL_RESULT_SOURCE_LINUX, 
                errno);
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
            palMakeResult(
                PAL_RESULT_INVALID_ARGUMENT, 
                PAL_RESULT_SOURCE_LINUX, 
                errno);

        } else {
            palMakeResult(
                PAL_RESULT_PLATFORM_FAILURE, 
                PAL_RESULT_SOURCE_LINUX, 
                errno);
        }
    }

    // make the context current and swap for the first time
    // this will make the window visible
    if (s_GL.eglMakeCurrent(s_GL.display, surface, surface, context)) {
        s_GL.glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        s_GL.glClear(0x00004000);
        s_GL.eglSwapBuffers(s_GL.display, surface);

        // revert
        s_GL.eglMakeCurrent(s_GL.display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
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
            // make it not current if it was current
            s_GL.eglMakeCurrent(s_GL.display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
            s_GL.eglDestroyContext(s_GL.display, (EGLContext)context);
            s_GL.eglDestroySurface(s_GL.display, data->surface);
            data->used = PAL_FALSE;
        }
    }
}

PalResult PAL_CALL palMakeContextCurrent(
    PalGLWindow* glWindow,
    PalGLContext* context)
{
    if (!s_GL.initialized) {
        palMakeResult(
            PAL_RESULT_NOT_INITIALIZED, 
            PAL_RESULT_SOURCE_LINUX, 
            errno);
    }

    if ((!glWindow && context) || (glWindow && !context)) {
        palMakeResult(
            PAL_RESULT_INVALID_ARGUMENT, 
            PAL_RESULT_SOURCE_LINUX, 
            errno);
    }

    if (context && glWindow) {
        ContextData* data = findContextData(context);
        if (!data) {
            palMakeResult(
                PAL_RESULT_INVALID_HANDLE, 
                PAL_RESULT_SOURCE_LINUX, 
                errno);
        }

        EGLint ret;
        ret = s_GL.eglMakeCurrent(s_GL.display, data->surface, data->surface, (EGLConfig)context);
        if (!ret) {
            EGLint error = s_GL.eglGetError();
            if (error == EGL_BAD_CONTEXT) {
                palMakeResult(
                    PAL_RESULT_INVALID_HANDLE, 
                    PAL_RESULT_SOURCE_LINUX, 
                    errno);

            } else if (error == EGL_BAD_SURFACE) {
                // since we always create a window surface
                palMakeResult(
                    PAL_RESULT_INVALID_HANDLE, 
                    PAL_RESULT_SOURCE_LINUX, 
                    errno);

            } else {
                palMakeResult(
                    PAL_RESULT_PLATFORM_FAILURE, 
                    PAL_RESULT_SOURCE_LINUX, 
                    errno);
            }
        }

    } else if (!context && !glWindow) {
        s_GL.eglMakeCurrent(s_GL.display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
    }

    return PAL_RESULT_SUCCESS;
}

#endif // __linux__