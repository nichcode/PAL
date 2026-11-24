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
#include "pal2/opengl/init.h"
#include <errno.h>
#include <stdio.h>
#include <string.h>

EGL g_Egl = {0};
static PalBool s_SupportedAPIs[2] = {0};

/* =========================================================================
 * Helper
 * ========================================================================= */

typedef union EGLSymbol {
    void* raw;
    PalLibrarySymbol symbol;
} EGLSymbol;

static PalLibrarySymbol loadEGLProc(const char* name)
{
    if (g_Egl.getProcAddress) {
        EGLSymbol sym;
        sym.raw = g_Egl.getProcAddress(name);
        if (sym.raw) {
            return sym.symbol;
        }
    }

    if (g_Egl.libegl) {
        return palGetSymbol(g_Egl.libegl, name);
    }

    return nullptr;
}

static PalBool loadProcs(void)
{
    // clang-format off
    g_Egl.getProcAddress = (eglGetProcAddressFn)palGetSymbol(g_Egl.libegl, "eglGetProcAddress");
    g_Egl.createPbufferSurface = (eglCreatePbufferSurfaceFn)loadEGLProc("eglCreatePbufferSurface");
    g_Egl.createWindowSurface = (eglCreateWindowSurfaceFn)loadEGLProc("eglCreateWindowSurface");
    g_Egl.createContext = (eglCreateContextFn)loadEGLProc("eglCreateContext");
    g_Egl.destroyContext = (eglDestroyContextFn)loadEGLProc("eglDestroyContext");
    g_Egl.destroySurface = (eglDestroySurfaceFn)loadEGLProc("eglDestroySurface");
    g_Egl.makeCurrent = (eglMakeCurrentFn)loadEGLProc("eglMakeCurrent");
    g_Egl.swapBuffers = (eglSwapBuffersFn)loadEGLProc("eglSwapBuffers");
    g_Egl.swapInterval = (eglSwapIntervalFn)loadEGLProc("eglSwapInterval");
    g_Egl.initialize = (eglInitializeFn)loadEGLProc("eglInitialize");
    g_Egl.terminate = (eglTerminateFn)loadEGLProc("eglTerminate");
    g_Egl.getDisplay = (eglGetDisplayFn)loadEGLProc("eglGetDisplay");
    g_Egl.chooseConfig = (eglChooseConfigFn)loadEGLProc("eglChooseConfig");

    g_Egl.getConfigAttrib = (eglGetConfigAttribFn)loadEGLProc("eglGetConfigAttrib");
    g_Egl.getError = (eglGetErrorFn)loadEGLProc("eglGetError");
    g_Egl.bindAPI = (eglBindAPIFn)loadEGLProc("eglBindAPI");
    g_Egl.queryString = (eglQueryStringFn)loadEGLProc("eglQueryString");
    g_Egl.getConfigs = (eglGetConfigsFn)loadEGLProc("eglGetConfigs");
    g_Egl.glClearColor = (glClearColorFn)loadEGLProc("glClearColor");
    g_Egl.glClear = (glClearFn)loadEGLProc("glClear");

    if (!g_Egl.bindAPI              ||
        !g_Egl.chooseConfig         ||
        !g_Egl.createContext        ||
        !g_Egl.createPbufferSurface ||
        !g_Egl.destroyContext       ||
        !g_Egl.destroySurface       ||
        !g_Egl.getConfigAttrib      ||
        !g_Egl.getDisplay           ||
        !g_Egl.getError             ||
        !g_Egl.getProcAddress       ||
        !g_Egl.initialize           ||
        !g_Egl.makeCurrent          ||
        !g_Egl.swapBuffers          ||
        !g_Egl.swapInterval         ||
        !g_Egl.terminate            ||
        !g_Egl.queryString          ||
        !g_Egl.getConfigs           ||
        !g_Egl.createWindowSurface) {
        return PAL_FALSE;
    }
    // clang-format on

    return PAL_TRUE;
}

/* =========================================================================
 * API
 * ========================================================================= */

void eglShutdownGL();

PalResult eglInitGL(
    PalGLAPI api,
    const void* instance,
    const PalAllocator* allocator)
{
    PalResult result = PAL_RESULT_CODE_PLATFORM_FAILURE;
    g_GLInfo.api = api;
    g_GLInfo.backend = PAL_GL_BACKEND_EGL;
    g_Egl.allocator = allocator;

    g_Egl.libegl = palLoadLibrary("libEGL.so");
    if (!g_Egl.libegl) {
        result = palMakeResult(PAL_RESULT_CODE_PLATFORM_FAILURE, PAL_RESULT_SOURCE_POSIX, (uint32_t)errno);
        goto cleanup;
    }

    g_Egl.maxContextData = 16;
    g_Egl.contextData = palAllocate(g_Egl.allocator, sizeof(ContextData) * (size_t)g_Egl.maxContextData, 0);
    if (!g_Egl.contextData) {
        result = palMakeResult(PAL_RESULT_CODE_OUT_OF_MEMORY, PAL_RESULT_SOURCE_NONE, 0);
        goto cleanup;
    }
    memset(g_Egl.contextData, 0, sizeof(ContextData) * (size_t)g_Egl.maxContextData);

    if (api == PAL_GL_API_OPENGL) {
        g_Egl.apiType = EGL_OPENGL_API;
        g_Egl.apiTypeBit = EGL_OPENGL_BIT;
    } else {
        g_Egl.apiType = EGL_OPENGL_ES_API;
        g_Egl.apiTypeBit = EGL_OPENGL_ES2_BIT;
    }

    if (!loadProcs()) {
        result = palMakeResult(PAL_RESULT_CODE_PLATFORM_FAILURE, PAL_RESULT_SOURCE_NONE, 0);
        goto cleanup;
    }

    if (!g_Egl.bindAPI(g_Egl.apiType)) {
        result = palMakeResult(PAL_RESULT_CODE_PLATFORM_FAILURE, PAL_RESULT_SOURCE_EGL, (uint32_t)g_Egl.getError());
        goto cleanup;
    }

    EGLDisplay display = g_Egl.getDisplay((EGLNativeDisplayType)instance);
    if (display == EGL_NO_DISPLAY) {
        result = palMakeResult(PAL_RESULT_CODE_PLATFORM_FAILURE, PAL_RESULT_SOURCE_EGL, (uint32_t)g_Egl.getError());
        goto cleanup;
    }

    if (!g_Egl.initialize(display, nullptr, nullptr)) {
        result = palMakeResult(PAL_RESULT_CODE_PLATFORM_FAILURE, PAL_RESULT_SOURCE_EGL, (uint32_t)g_Egl.getError());
        goto cleanup;
    }
    g_Egl.display = display;

    EGLDisplay tmpDisplay = EGL_NO_DISPLAY;
    EGLConfig config = nullptr;
    EGLint numConfigs = 0;
    EGLint type = 0;

    EGLint attribs[] = {
        EGL_RENDERABLE_TYPE,
        g_Egl.apiTypeBit,
        EGL_SURFACE_TYPE,
        EGL_PBUFFER_BIT,
        EGL_NONE,
    };

    g_Egl.chooseConfig(display, attribs, &config, 1, &numConfigs);
    if (!config || numConfigs == 0) {
        tmpDisplay = g_Egl.getDisplay(EGL_DEFAULT_DISPLAY);
        if (tmpDisplay == EGL_NO_DISPLAY) {
            result = palMakeResult(PAL_RESULT_CODE_PLATFORM_FAILURE, PAL_RESULT_SOURCE_EGL, (uint32_t)g_Egl.getError());
            goto cleanup;
        }

        if (!g_Egl.initialize(tmpDisplay, nullptr, nullptr)) {
            result = palMakeResult(PAL_RESULT_CODE_PLATFORM_FAILURE, PAL_RESULT_SOURCE_EGL, (uint32_t)g_Egl.getError());
            goto cleanup;
        }

        g_Egl.chooseConfig(tmpDisplay, attribs, &config, 1, &numConfigs);
        if (!config || numConfigs == 0) {
            g_Egl.terminate(tmpDisplay);
            result = palMakeResult(PAL_RESULT_CODE_PLATFORM_FAILURE, PAL_RESULT_SOURCE_EGL, (uint32_t)g_Egl.getError());
            goto cleanup;
        }
    } else {
        tmpDisplay = display;
    }

    g_Egl.getConfigAttrib(tmpDisplay, config, EGL_RENDERABLE_TYPE, &type);
    if (!(type & g_Egl.apiTypeBit)) {
        if (tmpDisplay != display) {
            g_Egl.terminate(tmpDisplay);
        }
        result = palMakeResult(PAL_RESULT_CODE_PLATFORM_FAILURE, PAL_RESULT_SOURCE_EGL, (uint32_t)g_Egl.getError());
        goto cleanup;
    }

    EGLint pBufferAttribs[] = {EGL_WIDTH, 1, EGL_HEIGHT, 1, EGL_NONE};
    EGLSurface surface = g_Egl.createPbufferSurface(tmpDisplay, config, pBufferAttribs);
    if (surface == EGL_NO_SURFACE) {
        if (tmpDisplay != display) {
            g_Egl.terminate(tmpDisplay);
        }
        result = palMakeResult(PAL_RESULT_CODE_PLATFORM_FAILURE, PAL_RESULT_SOURCE_EGL, (uint32_t)g_Egl.getError());
        goto cleanup;
    }

    // create a dummy context
    EGLContext context = EGL_NO_CONTEXT;
    if (g_Egl.apiType == EGL_OPENGL_API) {
        EGLint contextAttrib[] = {
            EGL_CONTEXT_MAJOR_VERSION,
            2,
            EGL_CONTEXT_MINOR_VERSION,
            1,
            EGL_NONE,
        };
        context = g_Egl.createContext(tmpDisplay, config, EGL_NO_CONTEXT, contextAttrib);
    } else {
        EGLint contextAttrib[] = {EGL_CONTEXT_CLIENT_VERSION, 2, EGL_NONE};
        context = g_Egl.createContext(tmpDisplay, config, EGL_NO_CONTEXT, contextAttrib);
    }

    if (context == EGL_NO_CONTEXT) {
        g_Egl.destroySurface(tmpDisplay, surface);
        if (tmpDisplay != display) {
            g_Egl.terminate(tmpDisplay);
        }
        result = palMakeResult(PAL_RESULT_CODE_PLATFORM_FAILURE, PAL_RESULT_SOURCE_EGL, (uint32_t)g_Egl.getError());
        goto cleanup;
    }

    g_Egl.makeCurrent(tmpDisplay, surface, surface, context);
    g_Egl.glGetString = (glGetStringFn)loadEGLProc("glGetString");

    if (g_Egl.glGetString) {
        const char* version = (const char*)g_Egl.glGetString(GL_VERSION);
        if (version) {
            if (g_Egl.apiType == EGL_OPENGL_API) {
                sscanf(version, "%u.%u", &g_GLInfo.major, &g_GLInfo.minor);
            } else if (strncmp(version, "OpenGL ES ", 10) == 0) {
                sscanf(version + 10, "%u.%u", &g_GLInfo.major, &g_GLInfo.minor);
            }
            snprintf(g_GLInfo.version, sizeof(g_GLInfo.version), "%s", version);
        }

        const char* renderer = (const char*)g_Egl.glGetString(GL_RENDERER);
        if (renderer) {
            snprintf(g_GLInfo.graphicsCard, sizeof(g_GLInfo.graphicsCard), "%s", renderer);
        }

        const char* vendor = (const char*)g_Egl.glGetString(GL_VENDOR);
        if (vendor) {
            snprintf(g_GLInfo.vendor, sizeof(g_GLInfo.vendor), "%s", vendor);
        }
    }

    const char* extensions = g_Egl.queryString(tmpDisplay, EGL_EXTENSIONS);
    if (extensions) {
        if (checkString("EGL_KHR_gl_colorspace", extensions)) {
            g_GLInfo.extensions |= PAL_GL_EXTENSION_COLORSPACE_SRGB;
        }

        if (checkString("EGL_KHR_create_context", extensions)) {
            g_GLInfo.extensions |= PAL_GL_EXTENSION_CREATE_CONTEXT;
            g_GLInfo.extensions |= PAL_GL_EXTENSION_CONTEXT_PROFILE;
        }

        if (checkString("EGL_EXT_create_context_robustness", extensions)) {
            g_GLInfo.extensions |= PAL_GL_EXTENSION_ROBUSTNESS;
        }

        if (checkString("EGL_KHR_create_context_no_error", extensions)) {
            g_GLInfo.extensions |= PAL_GL_EXTENSION_NO_ERROR;
        }

        if (checkString("EGL_KHR_context_flush_control", extensions)) {
            g_GLInfo.extensions |= PAL_GL_EXTENSION_FLUSH_CONTROL;
        }

        if (checkString("EGL_EXT_swap_control", extensions) || checkString("EGL_EXT_swap_control_tear", extensions)) {
            g_GLInfo.extensions |= PAL_GL_EXTENSION_SWAP_CONTROL;
        }
    }

    g_GLInfo.extensions |= PAL_GL_EXTENSION_MULTISAMPLE;
    if (type & EGL_OPENGL_ES_BIT || type & EGL_OPENGL_ES2_BIT) {
        g_GLInfo.extensions |= PAL_GL_EXTENSION_CONTEXT_PROFILE_ES2;
    }

    if (g_Egl.swapInterval) {
        g_GLInfo.extensions |= PAL_GL_EXTENSION_SWAP_CONTROL;
    }

    if (g_Egl.apiType != EGL_OPENGL_API) {
        if (g_GLInfo.extensions & PAL_GL_EXTENSION_CONTEXT_PROFILE) {
            g_GLInfo.extensions &= ~PAL_GL_EXTENSION_CONTEXT_PROFILE;
            g_GLInfo.extensions &= ~PAL_GL_EXTENSION_CONTEXT_PROFILE_ES2;
        }

        if (type & EGL_OPENGL_ES3_BIT) {
            g_Egl.apiTypeBit = EGL_OPENGL_ES3_BIT;
        }
    }

    g_Egl.makeCurrent(tmpDisplay, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
    g_Egl.destroyContext(tmpDisplay, context);
    g_Egl.destroySurface(tmpDisplay, surface);
    if (tmpDisplay != display) {
        g_Egl.terminate(tmpDisplay);
    }

    g_Egl.instance = instance;
    return PAL_RESULT_SUCCESS;

cleanup:
    eglShutdownGL();
    return result;
}

void eglShutdownGL()
{
    if (g_Egl.contextData) {
        palFree(g_Egl.allocator, g_Egl.contextData);
    }

    if (g_Egl.display && g_Egl.terminate) {
        g_Egl.terminate(g_Egl.display);
    }

    if (g_Egl.libegl) {
        palFreeLibrary(g_Egl.libegl);
    }

    memset(&g_Egl, 0, sizeof(EGL));
}

void* eglGetGLProcAddress(const char* name)
{
    void* proc = g_Egl.getProcAddress(name);
    if (!proc && g_Egl.libegl) {
        EGLSymbol sym;
        sym.symbol = palGetSymbol(g_Egl.libegl, name);
        proc = sym.raw;
    }

    return proc;
}

const PalBool* eglGetSupportedGLAPIs(const void* instance)
{
    PalLibrary* libegl = palLoadLibrary("libEGL.so");
    if (!libegl) {
        return nullptr;
    }

    // clang-format off
    eglInitializeFn initialize = (eglInitializeFn)palGetSymbol(libegl, "eglInitialize");
    eglTerminateFn terminate = (eglTerminateFn)palGetSymbol(libegl, "eglTerminate");
    eglGetDisplayFn getDisplay = (eglGetDisplayFn)palGetSymbol(libegl, "eglGetDisplay");
    eglQueryStringFn queryString = (eglQueryStringFn)palGetSymbol(libegl, "eglQueryString");

    if (!initialize  ||
        !terminate   ||
        !getDisplay  ||
        !queryString) {
        palFreeLibrary(libegl);
        return nullptr;
    }
    // clang-format on

    EGLDisplay display = getDisplay((EGLNativeDisplayType)instance);
    if (display == EGL_NO_DISPLAY) {
        palFreeLibrary(libegl);
        return nullptr;
    }

    if (!initialize(display, nullptr, nullptr)) {
        palFreeLibrary(libegl);
        return nullptr;
    }

    const char* apis = queryString(display, EGL_CLIENT_APIS);
    if (apis) {
        s_SupportedAPIs[PAL_GL_API_OPENGL] = checkString("OpenGL", apis);
        s_SupportedAPIs[PAL_GL_API_OPENGL_ES] = checkString("OpenGL_ES", apis);
    } else {
        s_SupportedAPIs[PAL_GL_API_OPENGL] = PAL_FALSE;
        s_SupportedAPIs[PAL_GL_API_OPENGL_ES] = PAL_FALSE;
    }

    terminate(display);
    palFreeLibrary(libegl);

    return s_SupportedAPIs;
}

#endif // PAL_HAS_WAYLAND_BACKEND