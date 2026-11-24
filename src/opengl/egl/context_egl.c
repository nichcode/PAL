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

#include "shared.h"
#if PLATFORM_EGL

#include "egl.h"
#include "opengl/platform.h"
#include "pal2/opengl/context.h"
#include "pal2/opengl/init.h"
#include <string.h>

/* =========================================================================
 * Helper
 * ========================================================================= */

ContextData* getFreeContextData(void)
{
    for (uint32_t i = 0; i < g_Egl.maxContextData; ++i) {
        if (!g_Egl.contextData[i].used) {
            memset(&g_Egl.contextData[i], 0, sizeof(ContextData));
            g_Egl.contextData[i].used = PAL_TRUE;
            return &g_Egl.contextData[i];
        }
    }

    uint32_t oldCount = g_Egl.maxContextData;
    uint32_t count = (oldCount > 0) ? (oldCount * 2) : 4;
    ContextData* data = palAllocate(g_Egl.allocator, sizeof(ContextData) * count, 0);
    if (data) {
        memset(data, 0, sizeof(ContextData) * count);
        if (g_Egl.contextData && oldCount > 0) {
            memcpy(data, g_Egl.contextData, oldCount * sizeof(ContextData));
            palFree(g_Egl.allocator, g_Egl.contextData);
        }

        g_Egl.contextData = data;
        g_Egl.maxContextData = count;

        g_Egl.contextData[oldCount].used = PAL_TRUE;
        return &g_Egl.contextData[oldCount];
    }
    return nullptr;
}

ContextData* findContextData(PalGLContext* context)
{
    for (uint32_t i = 0; i < g_Egl.maxContextData; ++i) {
        if (g_Egl.contextData[i].used && g_Egl.contextData[i].context == context) {
            return &g_Egl.contextData[i];
        }
    }
    return nullptr;
}


/* =========================================================================
 * API
 * ========================================================================= */

PalResult eglCreateGLContext(
    const PalGLContextCreateInfo* info,
    PalGLContext** context)
{
    if (info->window->instance != g_Egl.instance) {
        return PAL_RESULT_CODE_INVALID_HANDLE;
    }

    if (info->profile != PAL_GL_PROFILE_NONE) {
        if (!(g_GLInfo.extensions & PAL_GL_EXTENSION_CONTEXT_PROFILE)) {
            return PAL_RESULT_CODE_FEATURE_NOT_SUPPORTED;
        }
    }

    if (info->forward) {
        if (!(g_GLInfo.extensions & PAL_GL_EXTENSION_CREATE_CONTEXT)) {
            return PAL_RESULT_CODE_FEATURE_NOT_SUPPORTED;
        }
    }

    if (info->reset != PAL_GL_CONTEXT_RESET_NONE) {
        if (!(g_GLInfo.extensions & PAL_GL_EXTENSION_ROBUSTNESS)) {
            return PAL_RESULT_CODE_FEATURE_NOT_SUPPORTED;
        }
    }

    if (info->noError) {
        if (!(g_GLInfo.extensions & PAL_GL_EXTENSION_NO_ERROR)) {
            return PAL_RESULT_CODE_FEATURE_NOT_SUPPORTED;
        }
    }

    if (info->release != PAL_GL_RELEASE_BEHAVIOR_NONE) {
        if (!(g_GLInfo.extensions & PAL_GL_EXTENSION_FLUSH_CONTROL)) {
            return PAL_RESULT_CODE_FEATURE_NOT_SUPPORTED;
        }
    }

    // clang-format off
    PalBool valid = info->major < g_GLInfo.major ||
                    (info->major == g_GLInfo.major && info->minor <= g_GLInfo.minor);
    // clang-format on

    if (!valid) {
        return PAL_RESULT_CODE_INVALID_ARGUMENT;
    }

    EGLint numConfigs = 0;
    if (!g_Egl.getConfigs(g_Egl.display, nullptr, 0, &numConfigs)) {
        return palMakeResult(PAL_RESULT_CODE_PLATFORM_FAILURE, PAL_RESULT_SOURCE_EGL, (uint32_t)g_Egl.getError());
    }

    size_t configSize = sizeof(EGLConfig) * (size_t)numConfigs;
    EGLConfig* eglConfigs = palAllocate(g_Egl.allocator, configSize, 0);
    if (!eglConfigs) {
        return PAL_RESULT_CODE_OUT_OF_MEMORY;
    }

    g_Egl.getConfigs(g_Egl.display, eglConfigs, numConfigs, &numConfigs);
    EGLConfig config = eglConfigs[info->fbConfig->index];
    palFree(g_Egl.allocator, eglConfigs);

    ContextData* data = getFreeContextData();
    if (!data) {
        return PAL_RESULT_CODE_OUT_OF_MEMORY;
    }

    EGLContext share = EGL_NO_CONTEXT;
    if (info->shareContext) {
        share = (EGLContext)info->shareContext;
    }

    int32_t attribs[40];
    int32_t index = 0;
    int32_t profile = 0;
    int32_t flags = 0;

    if (g_Egl.apiType == EGL_OPENGL_API) {
        attribs[index++] = EGL_CONTEXT_MAJOR_VERSION_KHR;
        attribs[index++] = (int32_t)info->major;

        attribs[index++] = EGL_CONTEXT_MINOR_VERSION_KHR;
        attribs[index++] = (int32_t)info->minor;

        if (info->profile != PAL_GL_PROFILE_NONE) {
            attribs[index++] = EGL_CONTEXT_OPENGL_PROFILE_MASK_KHR;

            if (info->profile == PAL_GL_PROFILE_COMPATIBILITY) {
                profile = EGL_CONTEXT_OPENGL_COMPATIBILITY_PROFILE_BIT_KHR;
            } else if (info->profile == PAL_GL_PROFILE_CORE) {
                profile = EGL_CONTEXT_OPENGL_CORE_PROFILE_BIT_KHR;
            }

            attribs[index++] = profile;
        }

        if (info->forward) {
            flags |= EGL_CONTEXT_OPENGL_FORWARD_COMPATIBLE_BIT_KHR;
        }

    } else {
        attribs[index++] = EGL_CONTEXT_CLIENT_VERSION;
        attribs[index++] = (int32_t)info->major;
    }

    if (info->debug) {
        flags |= EGL_CONTEXT_OPENGL_DEBUG_BIT_KHR;
    }

    if (info->reset != PAL_GL_CONTEXT_RESET_NONE) {
        flags |= EGL_CONTEXT_OPENGL_ROBUST_ACCESS_BIT_KHR;
        attribs[index++] = EGL_CONTEXT_OPENGL_RESET_NOTIFICATION_STRATEGY_KHR;

        if (info->reset == PAL_GL_CONTEXT_RESET_LOSE_CONTEXT) {
            attribs[index++] = EGL_LOSE_CONTEXT_ON_RESET_KHR;
        } else if (info->reset == PAL_GL_CONTEXT_RESET_NO_NOTIFICATION) {
            attribs[index++] = EGL_NO_RESET_NOTIFICATION_KHR;
        }
    }

    if (info->noError) {
        attribs[index++] = EGL_CONTEXT_OPENGL_NO_ERROR_KHR;
        attribs[index++] = PAL_TRUE;
    }

    if (info->release != PAL_GL_RELEASE_BEHAVIOR_NONE) {
        attribs[index++] = EGL_CONTEXT_RELEASE_BEHAVIOR_KHR;
        attribs[index++] = EGL_CONTEXT_RELEASE_BEHAVIOR_FLUSH_KHR;
    }

    if (flags) {
        attribs[index++] = EGL_CONTEXT_FLAGS_KHR;
        attribs[index++] = flags;
    }
    attribs[index++] = EGL_NONE;

    EGLContext eglContext = g_Egl.createContext(g_Egl.display, config, share, attribs);
    if (eglContext == EGL_NO_CONTEXT) {
        data->used = PAL_FALSE;
        EGLint error = g_Egl.getError();

        if (error == EGL_BAD_CONFIG || error == EGL_BAD_ATTRIBUTE) {
            return palMakeResult(PAL_RESULT_CODE_INVALID_ARGUMENT, PAL_RESULT_SOURCE_EGL, (uint32_t)error);
        } else {
            return palMakeResult(PAL_RESULT_CODE_PLATFORM_FAILURE, PAL_RESULT_SOURCE_EGL, (uint32_t)error);
        }
    }

    EGLint surfaceAttribs[3];
    surfaceAttribs[0] = EGL_GL_COLORSPACE;
    if (info->fbConfig->sRGB) {
        surfaceAttribs[1] = EGL_GL_COLORSPACE_SRGB_KHR;
    } else {
        surfaceAttribs[1] = EGL_GL_COLORSPACE_LINEAR_KHR;
    }
    surfaceAttribs[2] = EGL_NONE;

    EGLSurface surface = g_Egl.createWindowSurface(
        g_Egl.display,
        config,
        (EGLNativeWindowType)info->window->window,
        surfaceAttribs);

    if (surface == EGL_NO_SURFACE) {
        g_Egl.destroyContext(g_Egl.display, eglContext);
        data->used = PAL_FALSE;
        EGLint error = g_Egl.getError();

        if (error == EGL_BAD_CONFIG) {
            return palMakeResult(PAL_RESULT_CODE_INVALID_ARGUMENT, PAL_RESULT_SOURCE_EGL, (uint32_t)error);
        } else {
            return palMakeResult(PAL_RESULT_CODE_PLATFORM_FAILURE, PAL_RESULT_SOURCE_EGL, (uint32_t)error);
        }
    }

    // make the context current and swap for the first time
    // this will make the window visible
    if (g_Egl.makeCurrent(g_Egl.display, surface, surface, eglContext)) {
        if (g_Egl.glClearColor && g_Egl.glClear) {
            g_Egl.glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
            g_Egl.glClear(0x00004000);
        }

        g_Egl.swapBuffers(g_Egl.display, surface);
        g_Egl.makeCurrent(g_Egl.display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
    }

    data->context = (PalGLContext*)eglContext;
    data->surface = surface;

    *context = (PalGLContext*)eglContext;
    return PAL_RESULT_SUCCESS;
}

void eglDestroyGLContext(PalGLContext* context)
{
    ContextData* data = findContextData(context);
    g_Egl.makeCurrent(g_Egl.display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
    g_Egl.destroyContext(g_Egl.display, (EGLContext)context);

    if (data) {
        g_Egl.destroySurface(g_Egl.display, data->surface);
        data->used = PAL_FALSE;
    }
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

        EGLBoolean ret = g_Egl.makeCurrent(g_Egl.display, data->surface, data->surface, (EGLContext)context);
        if (!ret) {
            EGLint error = g_Egl.getError();
            if (error == EGL_BAD_CONTEXT || error == EGL_BAD_SURFACE) {
                return palMakeResult(PAL_RESULT_CODE_INVALID_HANDLE, PAL_RESULT_SOURCE_EGL, (uint32_t)error);
            } else {
                return palMakeResult(PAL_RESULT_CODE_PLATFORM_FAILURE, PAL_RESULT_SOURCE_EGL, (uint32_t)error);
            }
        }

    } else if (!context && !glWindow) {
        g_Egl.makeCurrent(g_Egl.display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
    }

    return PAL_RESULT_SUCCESS;
}

PalResult eglSwapBuffers(
    PalGLWindow* glWindow,
    PalGLContext* context)
{
    (void)glWindow;

    ContextData* data = findContextData(context);
    if (!data) {
        return PAL_RESULT_CODE_INVALID_HANDLE;
    }

    if (!g_Egl.swapBuffers(g_Egl.display, data->surface)) {
        EGLint error = g_Egl.getError();
        if (error == EGL_BAD_CONTEXT || error == EGL_BAD_SURFACE) {
            return palMakeResult(PAL_RESULT_CODE_INVALID_HANDLE, PAL_RESULT_SOURCE_EGL, (uint32_t)error);
        } else {
            return palMakeResult(PAL_RESULT_CODE_PLATFORM_FAILURE, PAL_RESULT_SOURCE_EGL, (uint32_t)error);
        }
    }

    return PAL_RESULT_SUCCESS;
}

void eglSetSwapInterval(int32_t interval)
{
    g_Egl.swapInterval(g_Egl.display, interval);
}

#endif // PAL_HAS_WAYLAND_BACKEND