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

#ifdef _WIN32

#include "opengl/platform.h"
#include "pal2/opengl/context.h"
#include "pal2/opengl/init.h"
#include "wgl.h"

PalResult wglCreateGLContext(
    const PalGLContextCreateInfo* info,
    PalGLContext** context)
{
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

    HWND hwnd = (HWND)info->window->window;
    HDC hdc = GetDC(hwnd);
    if (!hdc) {
        return palMakeResult(PAL_RESULT_CODE_INVALID_ARGUMENT, PAL_RESULT_SOURCE_WIN32, GetLastError());
    }

    if (g_Gdi.getPixelFormat(hdc) != info->fbConfig->index) {
        DWORD error = GetLastError();
        ReleaseDC(hwnd, hdc);
        return palMakeResult(PAL_RESULT_CODE_INVALID_ARGUMENT, PAL_RESULT_SOURCE_WIN32, error);
    }

    HGLRC share = nullptr;
    if (info->shareContext) {
        share = (HGLRC)info->shareContext;
    }

    HGLRC wglContext = nullptr;
    if (g_Wgl.createContextAttribsARB) {
        int32_t attribs[40];
        int32_t index = 0;
        int32_t profile = 0;
        int32_t flags = 0;

        attribs[index++] = WGL_CONTEXT_MAJOR_VERSION_ARB;
        attribs[index++] = info->major;

        attribs[index++] = WGL_CONTEXT_MINOR_VERSION_ARB;
        attribs[index++] = info->minor;

        if (info->profile != PAL_GL_PROFILE_NONE) {
            attribs[index++] = WGL_CONTEXT_PROFILE_MASK_ARB;

            if (info->profile == PAL_GL_PROFILE_COMPATIBILITY) {
                profile = WGL_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB;
            } else if (info->profile == PAL_GL_PROFILE_CORE) {
                profile = WGL_CONTEXT_CORE_PROFILE_BIT_ARB;
            } else {
                profile = WGL_CONTEXT_ES2_PROFILE_BIT_EXT;
            }
            attribs[index++] = profile;
        }

        if (info->forward) {
            flags |= WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB;
        }

        if (info->debug) {
            flags |= WGL_CONTEXT_DEBUG_BIT_ARB;
        }

        if (info->reset != PAL_GL_CONTEXT_RESET_NONE) {
            flags |= WGL_CONTEXT_ROBUST_ACCESS_BIT_ARB;
            attribs[index++] = WGL_CONTEXT_RESET_NOTIFICATION_STRATEGY_ARB;

            if (info->reset == PAL_GL_CONTEXT_RESET_LOSE_CONTEXT) {
                attribs[index++] = WGL_LOSE_CONTEXT_ON_RESET_ARB;
            } else if (info->reset == PAL_GL_CONTEXT_RESET_NO_NOTIFICATION) {
                attribs[index++] = WGL_NO_RESET_NOTIFICATION_ARB;
            }
        }

        if (info->noError) {
            attribs[index++] = WGL_CONTEXT_OPENGL_NO_ERROR_ARB;
            attribs[index++] = PAL_TRUE;
        }

        if (info->release != PAL_GL_RELEASE_BEHAVIOR_NONE) {
            attribs[index++] = WGL_CONTEXT_RELEASE_BEHAVIOR_ARB;
            attribs[index++] = WGL_CONTEXT_RELEASE_BEHAVIOR_FLUSH_ARB;
        }

        if (flags) {
            attribs[index++] = WGL_CONTEXT_FLAGS_ARB;
            attribs[index++] = flags;
        }
        attribs[index++] = 0;

        wglContext = g_Wgl.createContextAttribsARB(hdc, share, attribs);
        if (!wglContext) {
            DWORD error = GetLastError();
            ReleaseDC(hwnd, hdc);
            if (error == ERROR_INVALID_PROFILE_ARB) {
                return palMakeResult(PAL_RESULT_CODE_INVALID_ARGUMENT, PAL_RESULT_SOURCE_WIN32, error);
            } else {
                return palMakeResult(PAL_RESULT_CODE_PLATFORM_FAILURE, PAL_RESULT_SOURCE_WIN32, error);
            }
        }

    } else {
        wglContext = g_Wgl.createContext(hdc);
        if (!wglContext) {
            DWORD error = GetLastError();
            ReleaseDC(hwnd, hdc);
            return palMakeResult(PAL_RESULT_CODE_PLATFORM_FAILURE, PAL_RESULT_SOURCE_WIN32, error);
        }

        if (share) {
            if (!g_Wgl.shareLists(share, wglContext)) {
                DWORD error = GetLastError();
                g_Wgl.deleteContext(wglContext);
                ReleaseDC(hwnd, hdc);
                return palMakeResult(PAL_RESULT_CODE_PLATFORM_FAILURE, PAL_RESULT_SOURCE_WIN32, error);
            }
        }
    }

    ReleaseDC(hwnd, hdc);
    *context = (PalGLContext*)wglContext;

    return PAL_RESULT_SUCCESS;
}

void wglDestroyGLContext(PalGLContext* context)
{
    g_Wgl.deleteContext((HGLRC)context);
}

PalResult wglMakeContextCurrent(
    PalGLWindow* glWindow,
    PalGLContext* context)
{
    if (context && glWindow) {
        HWND hwnd = (HWND)glWindow->window;
        HDC hdc = GetDC(hwnd);
        if (!hdc) {
            return palMakeResult(
                PAL_RESULT_CODE_INVALID_ARGUMENT,
                PAL_RESULT_SOURCE_WIN32,
                GetLastError());
        }

        if (!g_Wgl.makeCurrent(hdc, (HGLRC)context)) {
            DWORD error = GetLastError();
            ReleaseDC(hwnd, hdc);
            if (error == ERROR_INVALID_HANDLE) {
                return palMakeResult(
                    PAL_RESULT_CODE_INVALID_ARGUMENT,
                    PAL_RESULT_SOURCE_WIN32,
                    error);
            } else {
                return palMakeResult(
                    PAL_RESULT_CODE_PLATFORM_FAILURE,
                    PAL_RESULT_SOURCE_WIN32,
                    error);
            }
        }
        ReleaseDC(hwnd, hdc);

    } else if (!context && !glWindow) {
        g_Wgl.makeCurrent(nullptr, nullptr);
    }

    return PAL_RESULT_SUCCESS;
}

PalResult wglSwapBuffers(
    PalGLWindow* glWindow,
    PalGLContext* context)
{
    (void)context;

    HWND hwnd = (HWND)glWindow->window;
    HDC hdc = GetDC(hwnd);
    if (!hdc) {
        return palMakeResult(PAL_RESULT_CODE_INVALID_ARGUMENT, PAL_RESULT_SOURCE_WIN32, GetLastError());
    }

    if (!g_Gdi.swapBuffers(hdc)) {
        DWORD error = GetLastError();
        ReleaseDC(hwnd, hdc);
        if (error == ERROR_INVALID_PIXEL_FORMAT) {
            return palMakeResult(PAL_RESULT_CODE_INVALID_ARGUMENT, PAL_RESULT_SOURCE_WIN32, error);
        } else {
            return palMakeResult(PAL_RESULT_CODE_PLATFORM_FAILURE, PAL_RESULT_SOURCE_WIN32, error);
        }
    }

    ReleaseDC(hwnd, hdc);
    return PAL_RESULT_SUCCESS;
}

void wglSetSwapInterval(int32_t interval)
{
    g_Wgl.swapIntervalEXT(interval);
}

#endif // _WIN32