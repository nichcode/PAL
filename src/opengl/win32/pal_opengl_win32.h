
/**
    PAL - Prime Abstraction Layer
    Copyright (C) 2025
    Licensed under the Zlib license. See LICENSE file in root.
 */

#ifndef _PAL_OPENGL_WIN32_H
#define _PAL_OPENGL_WIN32_H
#ifdef _WIN32

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

#include "pal/pal_opengl.h"
#include <windows.h>

#define PAL_GL_CLASS L"PALGLClass"

// check to see if this is not defined yet
#ifndef GL_VENDOR
#define GL_VENDOR 0x1F00
#define GL_RENDERER 0x1F01
#define GL_VERSION 0x1F02
#define GL_EXTENSIONS 0x1F03
#endif // GL_VENDOR

#ifndef WGL_NUMBER_PIXEL_FORMATS_ARB

#define WGL_NUMBER_PIXEL_FORMATS_ARB 0x2000
#define WGL_ACCELERATION_ARB 0x2003
#define WGL_RED_BITS_ARB 0x2015
#define WGL_GREEN_BITS_ARB 0x2017
#define WGL_BLUE_BITS_ARB 0x2019
#define WGL_ALPHA_BITS_ARB 0x201b

#define WGL_SUPPORT_OPENGL_ARB 0x2010
#define WGL_DRAW_TO_WINDOW_ARB 0x2001
#define WGL_PIXEL_TYPE_ARB 0x2013
#define WGL_DEPTH_BITS_ARB 0x2022
#define WGL_STENCIL_BITS_ARB 0x2023
#define WGL_STEREO_ARB 0x2012
#define WGL_DOUBLE_BUFFER_ARB 0x2011
#define WGL_SAMPLES_ARB 0x2042
#define WGL_FRAMEBUFFER_SRGB_CAPABLE_ARB 0x20a9
#define WGL_TYPE_RGBA_ARB 0x202b
#define WGL_NO_ACCELERATION_ARB 0x2025

#define WGL_CONTEXT_MAJOR_VERSION_ARB 0x2091
#define WGL_CONTEXT_MINOR_VERSION_ARB 0x2092
#define WGL_CONTEXT_PROFILE_MASK_ARB 0x9126
#define WGL_CONTEXT_CORE_PROFILE_BIT_ARB 0x00000001
#define WGL_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB 0x00000002
#define WGL_CONTEXT_DEBUG_BIT_ARB 0x00000001
#define WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB 0x00000002
#define WGL_CONTEXT_ES2_PROFILE_BIT_EXT 0x00000004
#define WGL_CONTEXT_ROBUST_ACCESS_BIT_ARB 0x00000004
#define WGL_LOSE_CONTEXT_ON_RESET_ARB 0x8252
#define WGL_CONTEXT_RESET_NOTIFICATION_STRATEGY_ARB 0x8256
#define WGL_NO_RESET_NOTIFICATION_ARB 0x8261
#define WGL_CONTEXT_OPENGL_NO_ERROR_ARB 0x31b3
#define WGL_CONTEXT_RELEASE_BEHAVIOR_ARB 0x2097
#define WGL_CONTEXT_RELEASE_BEHAVIOR_FLUSH_ARB 0x2098
#define WGL_CONTEXT_FLAGS_ARB 0x2094

#define ERROR_INVALID_PROFILE_ARB 0x2096

#endif // WGL_NUMBER_PIXEL_FORMATS_ARB

typedef unsigned int GLenum;
typedef unsigned char GLubyte;

// gdi functions
typedef int(WINAPI* ChoosePixelFormatFn)(
    HDC,
    CONST PIXELFORMATDESCRIPTOR*);

typedef BOOL(WINAPI* SetPixelFormatFn)(
    HDC,
    int,
    CONST PIXELFORMATDESCRIPTOR*);

typedef int(WINAPI* DescribePixelFormatFn)(
    HDC,
    int,
    UINT,
    LPPIXELFORMATDESCRIPTOR);

typedef int(WINAPI* GetPixelFormatFn)(HDC);

typedef BOOL(WINAPI* SwapBuffersFn)(HDC);

// wgl functions
typedef PROC(WINAPI* wglGetProcAddressFn)(LPCSTR);

typedef HGLRC(WINAPI* wglCreateContextFn)(HDC);

typedef BOOL(WINAPI* wglDeleteContextFn)(HGLRC);

typedef BOOL(WINAPI* wglShareListsFn)(
    HGLRC,
    HGLRC);

typedef BOOL(WINAPI* wglMakeCurrentFn)(
    HDC,
    HGLRC);

// gl functions
typedef const GLubyte*(WINAPI* glGetStringFn)(GLenum);

// extensions
typedef BOOL(WINAPI* wglChoosePixelFormatARBFn)(
    HDC,
    const int*,
    const FLOAT*,
    UINT,
    int*,
    UINT*);

typedef BOOL(WINAPI* wglGetPixelFormatAttribivARBFn)(
    HDC,
    int,
    int,
    UINT,
    const int*,
    int*);

typedef HGLRC(WINAPI* wglCreateContextAttribsARBFn)(
    HDC,
    HGLRC,
    const int*);

typedef BOOL(WINAPI* wglSwapIntervalEXTFn)(int);

typedef const char*(WINAPI* wglGetExtensionsStringEXTFn)();

typedef const char*(WINAPI* wglGetExtensionsStringARBFn)(HDC);

typedef struct {
    SetPixelFormatFn setPixelFormat;
    DescribePixelFormatFn describePixelFormat;
    ChoosePixelFormatFn choosePixelFormat;
    GetPixelFormatFn getPixelFormat;
    SwapBuffersFn swapBuffers;
    HINSTANCE handle;
} Gdi;

typedef struct {
    PalBool initialized;
    wglGetProcAddressFn wglGetProcAddress;
    wglCreateContextFn wglCreateContext;
    wglDeleteContextFn wglDeleteContext;
    wglMakeCurrentFn wglMakeCurrent;
    wglShareListsFn wglShareLists;
    glGetStringFn glGetString;

    wglCreateContextAttribsARBFn wglCreateContextAttribsARB;
    wglChoosePixelFormatARBFn wglChoosePixelFormatARB;
    wglSwapIntervalEXTFn wglSwapIntervalEXT;
    wglGetExtensionsStringEXTFn wglGetExtensionsStringEXT;
    wglGetExtensionsStringARBFn wglGetExtensionsStringARB;
    wglGetPixelFormatAttribivARBFn wglGetPixelFormatAttribivARB;

    const PalAllocator* allocator;
    HINSTANCE opengl;
    HINSTANCE instance;
    HWND window;
    HDC hdc;
    HGLRC context;
    PalGLInfo info;
} Wgl;

extern Gdi s_Gdi;
extern Wgl s_Wgl;

#endif // _WIN32
#endif // _PAL_OPENGL_WIN32_H