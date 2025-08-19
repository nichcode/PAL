
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
    LPPIXELFORMATDESCRIPTOR ppfd);

typedef WINBOOL (WINAPI *SwapBuffersFn)(HDC hdc);

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

    HWND window;
    HDC dc;
    HGLRC context;

    // gdi pointers
    SetPixelFormatFn setPixelFormat;
    DescribePixelFormatFn describePixelFormat;
    ChoosePixelFormatFn choosePixelFormat;
    SwapBuffersFn swapBuffers;

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

typedef struct PalGLContext {
    const PalAllocator* allocator;
    HGLRC handle;
    HWND window;
    HDC dc;
} PalGLContext;

static GLWin32 s_GL = {};

bool checkExtension(
    const char* extension, 
    const char* extensions);

static void shutdownGL();

static void getGLPixelFormatLegacy(
    HDC dc,
    Int32 *count,
    PalGLPixelFormat *formats);

static PalResult createContextLegacy(
    const PalGLContextCreateInfo* info,
    PalGLContext* context);

static void getGLPixelFormatExt(
    HDC dc,
    Int32 *count,
    PalGLPixelFormat *formats);

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

        s_GL.window = CreateWindowExW(
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

        if (!s_GL.window) {
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

        s_GL.swapBuffers = (SwapBuffersFn)GetProcAddress(
            s_GL.gdi,
            "SwapBuffers"
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
            !s_GL.swapBuffers         || 
            !s_GL.setPixelFormat) {
            return PAL_RESULT_PLATFORM_FAILURE;
        }

        if (!s_GL.wglGetProcAddress   || 
            !s_GL.wglCreateContext    || 
            !s_GL.wglDeleteContext    || 
            !s_GL.wglMakeCurrent) {
            return PAL_RESULT_PLATFORM_FAILURE;
        }

        s_GL.dc = GetDC(s_GL.window);
        PIXELFORMATDESCRIPTOR pfd = {};
        pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
        pfd.nVersion = 1;
        pfd.iPixelType = PFD_TYPE_RGBA;
        pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
        pfd.cColorBits = 32;
        pfd.cAlphaBits = 8;
        pfd.iLayerType = PFD_MAIN_PLANE;
        pfd.cDepthBits = 24;
        pfd.cStencilBits = 8;

        Int32 pixelFormat = s_GL.choosePixelFormat(s_GL.dc, &pfd);
        s_GL.setPixelFormat(s_GL.dc, pixelFormat, &pfd);

        s_GL.context = s_GL.wglCreateContext(s_GL.dc);
        
        if (!s_GL.wglMakeCurrent(s_GL.dc, s_GL.context)) {
            shutdownGL();
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
            extensions = s_GL.wglGetExtensionsStringARB(s_GL.dc);

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

        shutdownGL();
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

PalResult _PCALL palEnumerateGLPixelFormats(
    void* nativeWindow,
    Int32 *count,
    PalGLPixelFormat *formats) {
    
    if (!count || !nativeWindow) {
        return PAL_RESULT_NULL_POINTER;
    }

    if (count == 0 && formats) {
        return PAL_RESULT_INSUFFICIENT_BUFFER;
    }

    HDC windowDC = GetDC((HWND)nativeWindow);
    if (!windowDC) {
        return PAL_RESULT_INVALID_WINDOW_HANDLE;
    }

    if (s_GL.wglCreateContextAttribsARB) {

        // TODO: Remove
        getGLPixelFormatLegacy(windowDC, count, formats);
        //getGLPixelFormatExt();
         
    } else {
        getGLPixelFormatLegacy(windowDC, count, formats);
    }

    return PAL_RESULT_SUCCESS;
}

PalGLPixelFormat* _PCALL palGetClosestGLPixelFormat(
    PalGLPixelFormat *formats,
    Int32 count,
    const PalGLPixelFormat* desired) {

    if (!formats || !desired) {
        return nullptr;
    }

    if (count == 0) {
        return nullptr;
    }

    Int32 score = 0;
    Int32 bestScore = 0x7FFFFFFF;
    PalGLPixelFormat* best = nullptr;

    for (Int32 i = 0; i < count; i++) {
        PalGLPixelFormat* tmp = &formats[i];
        
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
            best = &formats[i];
        }
    }

    return best;
}

PalResult _PCALL palCreateGLContext(
    const PalGLContextCreateInfo* info,
    PalGLContext** outContext) {

    if (!info || !outContext || (info && !info->windowHandle)) {
        return PAL_RESULT_NULL_POINTER;
    }

    if (info->allocator && (info->allocator->allocate || info->allocator->free)) {
        return PAL_RESULT_INVALID_ALLOCATOR;
    }

    PalGLContext* context = palAllocate(
        info->allocator, 
        sizeof(PalGLContext),
        0
    );

    if (!context) {
        return PAL_RESULT_OUT_OF_MEMORY;
    }

    memset(context, 0, sizeof(PalGLContext));
    context->dc = GetDC((HWND)info->windowHandle);
    if (!context->dc) {
        palFree(info->allocator, context);
        return PAL_RESULT_INVALID_WINDOW_HANDLE;
    }

    PalResult result;
    context->window = (HWND)info->windowHandle;
    if (s_GL.wglCreateContextAttribsARB) {

        // TODO: Remove
        result = createContextLegacy(info, context);
        if (result != PAL_RESULT_SUCCESS) {
            ReleaseDC(context->window, context->dc);
            palFree(info->allocator, context);
            return result;
        }
         
    } else {
        //getGLPixelFormatLegacy(windowDC, count, formats);
    }

    *outContext = context;
    return PAL_RESULT_SUCCESS;
}

void _PCALL palDestroyGLContext(PalGLContext* context) {

    if (!context) {
        return;
    }

    ReleaseDC(context->window, context->dc);
    s_GL.wglDeleteContext(context->handle);
    palFree(context->allocator, context);
}

PalResult _PCALL palMakeCurrent(PalGLContext* context) {

    if (!context) {
        return PAL_RESULT_NULL_POINTER;
    }

    bool success = s_GL.wglMakeCurrent(context->dc, context->handle);
    if (!success) {
        return PAL_RESULT_PLATFORM_FAILURE;
    }

    return PAL_RESULT_SUCCESS;
}

PalResult _PCALL palSwapBuffers(PalGLContext* context) {

    if (!context) {
        return PAL_RESULT_NULL_POINTER;
    }

    bool success = s_GL.swapBuffers(context->dc);
    if (!success) {
        return PAL_RESULT_PLATFORM_FAILURE;
    }
    return PAL_RESULT_SUCCESS;
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

static void shutdownGL() {

    s_GL.wglMakeCurrent(s_GL.dc, nullptr);
    s_GL.wglDeleteContext(s_GL.context);
    ReleaseDC(s_GL.window, s_GL.dc);
    DestroyWindow(s_GL.window);
    UnregisterClassW(PAL_WIN32_GL_CLASS, s_GL.instance);

    FreeLibrary(s_GL.opengl);
    FreeLibrary(s_GL.gdi);
}

static void getGLPixelFormatLegacy(
    HDC dc,
    Int32 *count,
    PalGLPixelFormat *formats) {

    Int32 formatCount = 0;
    Int32 maxFormatCount = 0;
    Int32 nativeCount = 0;

    nativeCount = s_GL.describePixelFormat(
        dc,
        1,
        0, 
        nullptr
    );

    if (formats) {
        maxFormatCount = *count;
    }

    for (Int32 i = 1; i <= nativeCount; i++) {
        PIXELFORMATDESCRIPTOR pfd;
        if (!s_GL.describePixelFormat(
            dc,
            i,
            sizeof(PIXELFORMATDESCRIPTOR),
            &pfd)) {
            continue;
        }

        // filter for opengl pixel formats
        if (!(pfd.dwFlags & PFD_SUPPORT_OPENGL) ||
            !(pfd.dwFlags & PFD_DRAW_TO_WINDOW)) {
            continue;
        }

        if (pfd.iPixelType != PFD_TYPE_RGBA) {
            continue;
        }

        if (!(pfd.dwFlags & PFD_GENERIC_ACCELERATED) &&
            (pfd.dwFlags & PFD_GENERIC_FORMAT)) {
            // software pixel format
            continue;
        }

        if (formats && formatCount < maxFormatCount) {
            PalGLPixelFormat* format = &formats[formatCount];
            format->index = i;

            format->redBits = pfd.cRedBits;
            format->greenBits = pfd.cGreenBits;
            format->blueBits = pfd.cBlueBits;
            format->alphaBits = pfd.cAlphaBits;
            format->depthBits = pfd.cDepthBits;
            format->stencilBits = pfd.cStencilBits;

            format->samples = 1;
            format->doubleBuffer = (pfd.dwFlags & PFD_DOUBLEBUFFER) ? true : false;
            format->stereo = (pfd.dwFlags & PFD_STEREO) ? true : false;
            format->sRGB = false;
        }

        formatCount++;
    }

    if (!formats) {
        *count = formatCount;
    }
}

static PalResult createContextLegacy(
    const PalGLContextCreateInfo* info,
    PalGLContext* context) {

    const PalGLPixelFormat* format = info->format;
    DWORD flags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL;
    if (format->doubleBuffer) {
        flags |= PFD_DOUBLEBUFFER;
    }

    if (format->stereo) {
        flags |= PFD_STEREO;
    }
    
    PIXELFORMATDESCRIPTOR pfd = {};
    pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
    pfd.nVersion = 1;
    pfd.iPixelType = PFD_TYPE_RGBA;
    pfd.dwFlags = flags;

    pfd.cRedBits = format->redBits;
    pfd.cGreenBits = format->greenBits;
    pfd.cBlueBits = format->blueBits;
    pfd.cAlphaBits = format->alphaBits;
    pfd.cDepthBits = format->depthBits;
    pfd.cStencilBits = format->stencilBits;
    pfd.iLayerType = PFD_MAIN_PLANE;

    bool success = s_GL.setPixelFormat(context->dc, format->index, &pfd);
    if (!success) {
        return PAL_RESULT_INVALID_GL_PIXEL_FORMAT;
    }

    context->handle = s_GL.wglCreateContext(context->dc);
    if (!context->handle) {
        return PAL_RESULT_PLATFORM_FAILURE;
    }

    return PAL_RESULT_SUCCESS;
}

static void getGLPixelFormatExt(
    HDC dc,
    Int32 *count,
    PalGLPixelFormat *formats) {

}