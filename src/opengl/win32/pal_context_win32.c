
/**
    PAL - Prime Abstraction Layer
    Copyright (C) 2025
    Licensed under the Zlib license. See LICENSE file in root.
 */

#ifdef _WIN32
#include "pal_opengl_win32.h"
#include "pal_shared.h"

PalResult PAL_CALL palCreateGLContext(
    const PalGLContextCreateInfo* info,
    PalGLContext** outContext)
{
    if (!s_Wgl.initialized) {
        return palMakeResult(
            PAL_RESULT_NOT_INITIALIZED, 
            PAL_RESULT_SOURCE_WINDOWS, 
            GetLastError());
    }

    if (!info || !outContext || (info && (!info->window || !info->fbConfig))) {
        return palMakeResult(
            PAL_RESULT_INVALID_ARGUMENT, 
            PAL_RESULT_SOURCE_WINDOWS, 
            GetLastError());
    }

    // check support for requested features
    if (info->profile != PAL_GL_PROFILE_NONE) {
        if (!(s_Wgl.info.extensions & PAL_GL_EXTENSION_CONTEXT_PROFILE)) {
            return palMakeResult(
                PAL_RESULT_NOT_INITIALIZED, 
                PAL_RESULT_SOURCE_WINDOWS, 
                GetLastError());
        }
    }

    if (info->forward) {
        if (!(s_Wgl.info.extensions & PAL_GL_EXTENSION_CREATE_CONTEXT)) {
            return palMakeResult(
                PAL_RESULT_NOT_INITIALIZED, 
                PAL_RESULT_SOURCE_WINDOWS, 
                GetLastError());
        }
    }

    if (info->reset != PAL_GL_CONTEXT_RESET_NONE) {
        if (!(s_Wgl.info.extensions & PAL_GL_EXTENSION_ROBUSTNESS)) {
            return palMakeResult(
                PAL_RESULT_NOT_INITIALIZED, 
                PAL_RESULT_SOURCE_WINDOWS, 
                GetLastError());
        }
    }

    if (info->noError) {
        if (!(s_Wgl.info.extensions & PAL_GL_EXTENSION_NO_ERROR)) {
            return palMakeResult(
                PAL_RESULT_NOT_INITIALIZED, 
                PAL_RESULT_SOURCE_WINDOWS, 
                GetLastError());
        }
    }

    if (info->release != PAL_GL_RELEASE_BEHAVIOR_NONE) {
        if (!(s_Wgl.info.extensions & PAL_GL_EXTENSION_FLUSH_CONTROL)) {
            return palMakeResult(
                PAL_RESULT_NOT_INITIALIZED, 
                PAL_RESULT_SOURCE_WINDOWS, 
                GetLastError());
        }
    }

    // clang-format off
    // check version
    PalBool valid = info->major < s_Wgl.info.major ||
        (info->major == s_Wgl.info.major && info->minor <= s_Wgl.info.minor);
    // clang-format on

    if (!valid) {
        return palMakeResult(
            PAL_RESULT_INVALID_ARGUMENT, 
            PAL_RESULT_SOURCE_WINDOWS, 
            GetLastError());
    }

    HDC hdc = GetDC((HWND)info->window->window);
    if (!hdc) {
        return palMakeResult(
            PAL_RESULT_INVALID_ARGUMENT, 
            PAL_RESULT_SOURCE_WINDOWS, 
            GetLastError());

    }

    // check if the provided pixel format is the same as the window
    if (s_Gdi.getPixelFormat(hdc) != info->fbConfig->index) {
        return palMakeResult(
            PAL_RESULT_INVALID_ARGUMENT, 
            PAL_RESULT_SOURCE_WINDOWS, 
            GetLastError());
    }

    HGLRC share = nullptr;
    if (info->shareContext) {
        share = (HGLRC)info->shareContext;
    }

    HGLRC context = nullptr;
    if (s_Wgl.wglCreateContextAttribsARB) {
        // create context with modern wgl functions
        int32_t attribs[40];
        int32_t index = 0;
        int32_t profile = 0;
        int32_t flags = 0;

        // set context attributes
        // the first element is the key and the second is the value
        // set version
        attribs[index++] = WGL_CONTEXT_MAJOR_VERSION_ARB; // key
        attribs[index++] = info->major;                   // value

        attribs[index++] = WGL_CONTEXT_MINOR_VERSION_ARB;
        attribs[index++] = info->minor;

        // set profile mask
        if (info->profile != PAL_GL_PROFILE_NONE) {
            attribs[index++] = WGL_CONTEXT_PROFILE_MASK_ARB;

            if (info->profile == PAL_GL_PROFILE_COMPATIBILITY) {
                profile = WGL_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB;
            } else if (info->profile == PAL_GL_PROFILE_CORE) {
                profile = WGL_CONTEXT_CORE_PROFILE_BIT_ARB;
            } else {
                profile = WGL_CONTEXT_ES2_PROFILE_BIT_EXT;
            }
            attribs[index++] = info->profile;
        }

        // set forward flag
        if (info->forward) {
            flags |= WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB;
        }

        // set debug flag
        if (info->debug) {
            flags |= WGL_CONTEXT_DEBUG_BIT_ARB;
        }

        // set robustness
        if (info->reset != PAL_GL_CONTEXT_RESET_NONE) {
            flags |= WGL_CONTEXT_ROBUST_ACCESS_BIT_ARB;
            attribs[index++] = WGL_CONTEXT_RESET_NOTIFICATION_STRATEGY_ARB;

            if (info->reset == PAL_GL_CONTEXT_RESET_LOSE_CONTEXT) {
                attribs[index++] = WGL_LOSE_CONTEXT_ON_RESET_ARB;

            } else if (info->reset == PAL_GL_CONTEXT_RESET_NO_NOTIFICATION) {
                attribs[index++] = WGL_NO_RESET_NOTIFICATION_ARB;
            }
        }

        // set no error
        if (info->noError) {
            attribs[index++] = WGL_CONTEXT_OPENGL_NO_ERROR_ARB;
            attribs[index++] = PAL_TRUE;
        }

        // release
        if (info->release != PAL_GL_RELEASE_BEHAVIOR_NONE) {
            attribs[index++] = WGL_CONTEXT_RELEASE_BEHAVIOR_ARB;
            attribs[index++] = WGL_CONTEXT_RELEASE_BEHAVIOR_FLUSH_ARB;
        }

        if (flags) {
            attribs[index++] = WGL_CONTEXT_FLAGS_ARB;
            attribs[index++] = flags;
        }
        attribs[index++] = 0;

        context = s_Wgl.wglCreateContextAttribsARB(hdc, share, attribs);
        if (!context) {
            DWORD error = GetLastError();
            if (error == ERROR_INVALID_PROFILE_ARB) {
                return palMakeResult(
                    PAL_RESULT_INVALID_ARGUMENT, 
                    PAL_RESULT_SOURCE_WINDOWS, 
                    error);

            } else {
                return palMakeResult(
                    PAL_RESULT_PLATFORM_FAILURE, 
                    PAL_RESULT_SOURCE_WINDOWS, 
                    error);
            }
        }

    } else {
        // create context with legacy wgl functions
        context = s_Wgl.wglCreateContext(hdc);
        if (!context) {
            return palMakeResult(
                PAL_RESULT_PLATFORM_FAILURE, 
                PAL_RESULT_SOURCE_WINDOWS, 
                GetLastError());
        }

        // share context
        if (share) {
            if (!s_Wgl.wglShareLists(share, context)) {
                s_Wgl.wglDeleteContext(context);
                ReleaseDC((HWND)info->window->window, hdc);

                return palMakeResult(
                    PAL_RESULT_PLATFORM_FAILURE, 
                    PAL_RESULT_SOURCE_WINDOWS, 
                    GetLastError());
            }
        }
    }

    ReleaseDC((HWND)info->window->window, hdc);
    *outContext = (PalGLContext*)context;
    return PAL_RESULT_SUCCESS;
}

void PAL_CALL palDestroyGLContext(PalGLContext* context)
{
    if (!s_Wgl.initialized || !context) {
        return;
    }
    s_Wgl.wglDeleteContext((HGLRC)context);
}

PalResult PAL_CALL palMakeContextCurrent(
    PalGLWindow* glWindow,
    PalGLContext* context)
{
    if (!s_Wgl.initialized) {
        return palMakeResult(
            PAL_RESULT_NOT_INITIALIZED, 
            PAL_RESULT_SOURCE_WINDOWS, 
            GetLastError());
    }

    if ((!glWindow && context) || (glWindow && !context)) {
        return palMakeResult(
            PAL_RESULT_INVALID_ARGUMENT, 
            PAL_RESULT_SOURCE_WINDOWS, 
            GetLastError());
    }

    if (context && glWindow) {
        // get hdc
        HDC hdc = GetDC((HWND)glWindow->window);
        if (!hdc) {
            return palMakeResult(
                PAL_RESULT_INVALID_ARGUMENT, 
                PAL_RESULT_SOURCE_WINDOWS, 
                GetLastError());
        }

        if (!s_Wgl.wglMakeCurrent(hdc, (HGLRC)context)) {
            DWORD error = GetLastError();
            if (error == ERROR_INVALID_HANDLE) {
                return palMakeResult(
                    PAL_RESULT_INVALID_ARGUMENT, 
                    PAL_RESULT_SOURCE_WINDOWS, 
                    error);

            } else {
                return palMakeResult(
                    PAL_RESULT_PLATFORM_FAILURE, 
                    PAL_RESULT_SOURCE_WINDOWS, 
                    error);
            }
        }
        ReleaseDC((HWND)glWindow->window, hdc);

    } else if (!context && !glWindow) {
        s_Wgl.wglMakeCurrent(nullptr, nullptr);
    }

    return PAL_RESULT_SUCCESS;
}

PalResult PAL_CALL palSwapBuffers(
    PalGLWindow* glWindow,
    PalGLContext* context)
{
    if (!s_Wgl.initialized) {
        return palMakeResult(
            PAL_RESULT_NOT_INITIALIZED, 
            PAL_RESULT_SOURCE_WINDOWS, 
            GetLastError());
    }

    if (!context || !glWindow) {
        return palMakeResult(
            PAL_RESULT_INVALID_ARGUMENT, 
            PAL_RESULT_SOURCE_WINDOWS, 
            GetLastError());
    }

    // get hdc
    HDC hdc = GetDC((HWND)glWindow->window);
    if (!hdc) {
        return palMakeResult(
            PAL_RESULT_INVALID_ARGUMENT, 
            PAL_RESULT_SOURCE_WINDOWS, 
            GetLastError());
    }

    if (!s_Gdi.swapBuffers(hdc)) {
        DWORD error = GetLastError();
        if (error == ERROR_INVALID_PIXEL_FORMAT) {
            return palMakeResult(
                PAL_RESULT_INVALID_ARGUMENT, 
                PAL_RESULT_SOURCE_WINDOWS, 
                error);

        } else {
            return palMakeResult(
                PAL_RESULT_PLATFORM_FAILURE, 
                PAL_RESULT_SOURCE_WINDOWS, 
                error);
        }
    }

    ReleaseDC((HWND)glWindow->window, hdc);
    return PAL_RESULT_SUCCESS;
}

#endif // _WIN32