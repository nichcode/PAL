
/**

Copyright (C) 2025 Nicholas Agbo
  
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

#include "pal/pal_opengl.h"

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif // WIN32_LEAN_AND_MEAN

#ifndef NOMINMAX
#define NOMINMAX
#endif // NOMINMAX

// set unicode
#ifndef UNICODE
#define UNICODE
#endif // UNICODE

#include <windows.h>
#include <stdio.h>

#define PAL_WIN32_GL_CLASS L"PALGLClass"

#ifndef GL_VENDOR
#define GL_VENDOR                         0x1F00
#define GL_RENDERER                       0x1F01
#define GL_VERSION                        0x1F02
#endif // GL_VENDOR

typedef unsigned int GLenum;
typedef unsigned char GLubyte;

// gdi functions
typedef int (WINAPI *ChoosePixelFormatFn)(
    HDC hdc,
    CONST PIXELFORMATDESCRIPTOR *ppfd);

typedef WINBOOL (WINAPI *SetPixelFormatFn)(
    HDC hdc,
    int format,
    CONST PIXELFORMATDESCRIPTOR *ppfd);

typedef int (WINAPI *DescribePixelFormatFn)(
    HDC hdc,
    int iPixelFormat,
    UINT nBytes,
    LPPIXELFORMATDESCRIPTOR *ppfd);

// wgl functions
typedef PROC (WINAPI *wglGetProcAddressFn)(LPCSTR lpszProc);
typedef HGLRC (WINAPI *wglCreateContextFn)(HDC hDc);
typedef BOOL (WINAPI *wglDeleteContextFn)(HGLRC oldContext);

typedef BOOL (WINAPI *wglMakeCurrentFn)(
    HDC hDc, 
    HGLRC newContext);

// gl functions
typedef const GLubyte *(WINAPI *glGetStringFn)(GLenum name);

// extensions
typedef BOOL (WINAPI *wglChoosePixelFormatARBFn)(
    HDC hdc, 
    const int *piAttribIList, 
    const FLOAT *pfAttribFList, 
    UINT nMaxFormats, 
    int *piFormats, 
    UINT *nNumFormats);

typedef HGLRC (WINAPI *wglCreateContextAttribsARBFn)(
    HDC hDC, 
    HGLRC hShareContext, 
    const int *attribList);

typedef BOOL (WINAPI *wglSwapIntervalEXTFn)(int interval);

typedef const char* (WINAPI *wglGetExtensionsStringEXTFn)();
typedef const char* (WINAPI *wglGetExtensionsStringARBFn)(HDC hDC);

typedef struct GLWin32 {
    HINSTANCE opengl;
    HINSTANCE gdi;
    HINSTANCE instance;

    // gdi pointers
    SetPixelFormatFn setPixelFormat;
    DescribePixelFormatFn describePixelFormat;
    ChoosePixelFormatFn choosePixelFormat;

    // wgl pointers
    wglGetProcAddressFn wglGetProcAddress;
    wglCreateContextFn wglCreateContext;
    wglDeleteContextFn wglDeleteContext;
    wglMakeCurrentFn wglMakeCurrent;

    // gl functions
    glGetStringFn glGetString; 
    
    // wgl extensions
    wglCreateContextAttribsARBFn wglCreateContextAttribsARB;
    wglChoosePixelFormatARBFn wglChoosePixelFormatARB;
    wglSwapIntervalEXTFn wglSwapIntervalEXT;
    wglGetExtensionsStringEXTFn wglGetExtensionsStringEXT;
    wglGetExtensionsStringARBFn wglGetExtensionsStringARB;

    char graphicsCard[64];
    char version[64];
    char vendor[32];
    PalGLExtensions extensions;
    Int32 versionMajor, versionMinor;

    bool initialized, supportContextEx;
} GLWin32;

static GLWin32 s_GL = {};

bool checkExtension(
    const char* extension, 
    const char* extensions);

// ==================================================
// Public API
// ==================================================

PalResult _PCALL palLoadGLICD() {

    if (!s_GL.initialized) {
        // register class
        s_GL.instance = GetModuleHandleW(nullptr);

        WNDCLASSEXW wc = {};
        wc.style = CS_OWNDC;
        wc.lpfnWndProc = DefWindowProcW;
        wc.lpszClassName = PAL_WIN32_GL_CLASS;
        wc.cbSize = sizeof(WNDCLASSEXW);

        if (!RegisterClassExW(&wc)) {
            return PAL_RESULT_ACCESS_DENIED;
        }

        HWND dummyWindow = CreateWindowExW(
            0,
            PAL_WIN32_GL_CLASS,
            L"Dummy Window",
            0,
            CW_USEDEFAULT,
            CW_USEDEFAULT,
            CW_USEDEFAULT,
            CW_USEDEFAULT,
            0,
            0,
            s_GL.instance,
            0
        );

        if (!dummyWindow) {
            return PAL_RESULT_PLATFORM_FAILURE;
        }

        // load libraries
        s_GL.gdi = LoadLibraryA("gdi32.dll");
        s_GL.opengl = LoadLibraryA("opengl32.dll");
        if (!s_GL.gdi || !s_GL.opengl) {
            return PAL_RESULT_PLATFORM_FAILURE;
        }

        // load gdi function pointers
        s_GL.choosePixelFormat = (ChoosePixelFormatFn)GetProcAddress(
            s_GL.gdi,
            "ChoosePixelFormat"
        );

        s_GL.setPixelFormat = (SetPixelFormatFn)GetProcAddress(
            s_GL.gdi,
            "SetPixelFormat"
        );

        s_GL.describePixelFormat = (DescribePixelFormatFn)GetProcAddress(
            s_GL.gdi,
            "DescribePixelFormat"
        );

        // load wgl function pointers
        s_GL.wglGetProcAddress = (wglGetProcAddressFn)GetProcAddress(
            s_GL.opengl,
            "wglGetProcAddress"
        );

        s_GL.wglCreateContext = (wglCreateContextFn)GetProcAddress(
            s_GL.opengl,
            "wglCreateContext"
        );

        s_GL.wglDeleteContext = (wglDeleteContextFn)GetProcAddress(
            s_GL.opengl,
            "wglDeleteContext"
        );

        s_GL.wglMakeCurrent = (wglMakeCurrentFn)GetProcAddress(
            s_GL.opengl,
            "wglMakeCurrent"
        );

        if (!s_GL.choosePixelFormat   || 
            !s_GL.describePixelFormat || 
            !s_GL.setPixelFormat) {
            return PAL_RESULT_PLATFORM_FAILURE;
        }

        if (!s_GL.wglGetProcAddress   || 
            !s_GL.wglCreateContext    || 
            !s_GL.wglDeleteContext    || 
            !s_GL.wglMakeCurrent) {
            return PAL_RESULT_PLATFORM_FAILURE;
        }

        HDC dummyDC = GetDC(dummyWindow);
        PIXELFORMATDESCRIPTOR pfd = {};
        pfd.nSize = sizeof(pfd);
        pfd.nVersion = 1;
        pfd.iPixelType = PFD_TYPE_RGBA;
        pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
        pfd.cColorBits = 32;
        pfd.cAlphaBits = 8;
        pfd.iLayerType = PFD_MAIN_PLANE;
        pfd.cDepthBits = 24;
        pfd.cStencilBits = 8;

        int pixelFormat = s_GL.choosePixelFormat(dummyDC, &pfd);
        s_GL.setPixelFormat(dummyDC, pixelFormat, &pfd);

        HGLRC dummyContext = s_GL.wglCreateContext(dummyDC);
        
        if (!s_GL.wglMakeCurrent(dummyDC, dummyContext)) {
            s_GL.wglDeleteContext(dummyContext);
            ReleaseDC(dummyWindow, dummyDC);
            DestroyWindow(dummyWindow);
            return PAL_RESULT_PLATFORM_FAILURE;
        }

        // load wgl extension function pointers
        s_GL.wglChoosePixelFormatARB = (wglChoosePixelFormatARBFn)s_GL.wglGetProcAddress(
            "wglChoosePixelFormatARB"
        );

        s_GL.wglCreateContextAttribsARB = (wglCreateContextAttribsARBFn)s_GL.wglGetProcAddress(
            "wglCreateContextAttribsARB"
        );

        s_GL.wglSwapIntervalEXT = (wglSwapIntervalEXTFn)s_GL.wglGetProcAddress(
            "wglSwapIntervalEXT"
        );

        s_GL.wglGetExtensionsStringARB = (wglGetExtensionsStringARBFn)s_GL.wglGetProcAddress(
            "wglGetExtensionsStringARB"
        );

        s_GL.wglGetExtensionsStringEXT = (wglGetExtensionsStringEXTFn)s_GL.wglGetProcAddress(
            "wglGetExtensionsStringEXT"
        );

        if (s_GL.wglChoosePixelFormatARB       || 
            s_GL.wglCreateContextAttribsARB    ||
            s_GL.wglSwapIntervalEXT) {
            s_GL.supportContextEx = true;
        }

        // load gl functions
        s_GL.glGetString = (glGetStringFn)GetProcAddress(
            s_GL.opengl,
            "glGetString"
        );

        const char* version = (const char*)s_GL.glGetString(GL_VERSION);
        if (version) {
#ifdef _MSC_VER
        sscanf_s(version, "%d.%d", &major, &minor);
#else
        sscanf(version, "%d.%d", &s_GL.versionMajor, &s_GL.versionMinor);
#endif    
        }

        const char* renderer = (const char*)s_GL.glGetString(GL_RENDERER);
        const char* vendor = (const char*)s_GL.glGetString(GL_VENDOR);
        strcpy(s_GL.vendor, vendor);
        strcpy(s_GL.version, version);
        strcpy(s_GL.graphicsCard, renderer);

        // check available extensions
        const char* extensions = nullptr;
        if (s_GL.wglGetExtensionsStringARB) {
            extensions = s_GL.wglGetExtensionsStringARB(dummyDC);

        } else if (s_GL.wglGetExtensionsStringEXT) {
            extensions = s_GL.wglGetExtensionsStringEXT();
        }

        if (extensions) {
            // multisample
            if (checkExtension("WGL_ARB_multisample", extensions)) {
                s_GL.extensions |= PAL_GL_EXTENSION_MULTISAMPLE;
            }
            // multisample

            // color space
            if (checkExtension("WGL_ARB_framebuffer_sRGB", extensions)) {
                s_GL.extensions |= PAL_GL_EXTENSION_COLORSPACE_SRGB;
            }

            if (checkExtension("WGL_EXT_framebuffer_sRGB", extensions)) {
                s_GL.extensions |= PAL_GL_EXTENSION_COLORSPACE_SRGB;
            }

            if (checkExtension("WGL_EXT_colorspace", extensions)) {
                s_GL.extensions |= PAL_GL_EXTENSION_COLORSPACE_SRGB;
            }
            // color space

            // create context
            if (checkExtension("WGL_ARB_create_context", extensions)) {
                s_GL.extensions |= PAL_GL_EXTENSION_CREATE_CONTEXT;
            }
            // create context

            // create profile
            if (checkExtension("WGL_ARB_create_context_profile", extensions)) {
                s_GL.extensions |= PAL_GL_EXTENSION_CONTEXT_PROFILE;
            }
            // create profile

            // create profile es2
            if (checkExtension("WGL_EXT_create_context_es2_profile", extensions)) {
                s_GL.extensions |= PAL_GL_EXTENSION_CONTEXT_PROFILE_ES2;
            }
            // create profile es2

            // robustness
            if (checkExtension("WGL_ARB_create_context_robustness", extensions)) {
                s_GL.extensions |= PAL_GL_EXTENSION_ROBUSTNESS;
            }
            // robustness

            // no error
            if (checkExtension("WGL_ARB_create_context_no_error", extensions)) {
                s_GL.extensions |= PAL_GL_EXTENSION_NO_ERROR;
            }
            // no error

            // swap control
            if (checkExtension("WGL_EXT_swap_control", extensions)) {
                s_GL.extensions |= PAL_GL_EXTENSION_SWAP_CONTROL;
            }

            if (checkExtension("WGL_ARB_context_flush_control", extensions)) {
                s_GL.extensions |= PAL_GL_EXTENSION_SWAP_CONTROL;
            }
            // swap control

            // pixel format
            if (checkExtension("WGL_ARB_pixel_format", extensions)) {
                s_GL.extensions |= PAL_GL_EXTENSION_PIXEL_FORMAT;
            }
            // pixel format
        }

        s_GL.wglMakeCurrent(dummyDC, 0);
        ReleaseDC(dummyWindow, dummyDC);
        DestroyWindow(dummyWindow);

        FreeLibrary(s_GL.gdi);
        FreeLibrary(s_GL.opengl);
        s_GL.initialized = true;
    }

    return PAL_RESULT_SUCCESS;
}

PalGLInfo _PCALL palGetGLInfo() {

    PalGLInfo info;
    info.versionMajor = s_GL.versionMajor;
    info.versionMinor = s_GL.versionMinor;
    info.extensions = s_GL.extensions;
    strcpy(info.vendor, s_GL.vendor);
    strcpy(info.version, s_GL.version);
    strcpy(info.graphicsCard, s_GL.graphicsCard);

    return info;
}

// ==================================================
// Internal API
// ==================================================

bool checkExtension(
    const char* extension, 
    const char* extensions) {
    
    const char* start = extensions;
    size_t extensionLen = strlen(extension);

    for (;;) {
        const char* where = nullptr;
        const char* terminator = nullptr;

        where = strstr(start, extension);
        if (!where) {
            return false;
        }

        // the extension was found, we find the terminator by adding the sizeof the extension
        terminator = where + extensionLen;
        if (where == start || *(where - 1) == ' ') {
            if (*terminator == ' ' || *terminator == '\0') {
                return true;
            }
        }

        start = terminator;
    }
}