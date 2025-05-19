
#pragma once

#include "PAL_win32.h"

#define WGL_DRAW_TO_WINDOW_ARB            0x2001
#define WGL_SUPPORT_OPENGL_ARB            0x2010
#define WGL_DOUBLE_BUFFER_ARB             0x2011
#define WGL_ACCELERATION_ARB              0x2003
#define WGL_SWAP_METHOD_ARB               0x2007
#define WGL_PIXEL_TYPE_ARB                0x2013
#define WGL_COLOR_BITS_ARB                0x2014
#define WGL_DEPTH_BITS_ARB                0x2022
#define WGL_STENCIL_BITS_ARB              0x2023
#define WGL_FULL_ACCELERATION_ARB         0x2027
#define WGL_SWAP_COPY_ARB                 0x2029
#define WGL_TYPE_RGBA_ARB                 0x202B
#define GL_TRUE 1
#define GL_FALSE 0

#define WGL_CONTEXT_DEBUG_BIT_ARB         0x00000001
#define WGL_CONTEXT_MAJOR_VERSION_ARB     0x2091
#define WGL_CONTEXT_MINOR_VERSION_ARB     0x2092
#define WGL_CONTEXT_LAYER_PLANE_ARB       0x2093
#define WGL_CONTEXT_PROFILE_MASK_ARB      0x9126
#define WGL_CONTEXT_CORE_PROFILE_BIT_ARB  0x00000001
#define WGL_CONTEXT_FLAGS_ARB             0x2094

// gdi32 function procs
typedef int WINAPI (*ChoosePixelFormatFun)(HDC hdc,CONST PIXELFORMATDESCRIPTOR *ppfd);
typedef int WINAPI (*SetPixelFormatFun)(HDC hdc,int format,CONST PIXELFORMATDESCRIPTOR *ppfd);
typedef int WINAPI (*DescribePixelFormatFun)(HDC hdc,int iPixelFormat,UINT nBytes,LPPIXELFORMATDESCRIPTOR ppfd);
typedef int WINAPI (*SwapBuffersFun)(HDC hdc);

// wgl function procs
typedef PROC (WINAPI * PFNWGLGETPROCADDRESSPROC) (LPCSTR lpszProc);
typedef HGLRC (WINAPI * PFNWGLCREATECONTEXTPROC) (HDC hDc);
typedef BOOL (WINAPI * PFNWGLDELETECONTEXTPROC) (HGLRC oldContext);
typedef BOOL (WINAPI * PFNWGLMAKECURRENTPROC) (HDC hDc, HGLRC newContext);

typedef BOOL (WINAPI * PFNWGLSWAPINTERVALEXTPROC) (int interval);

typedef BOOL (WINAPI * PFNWGLCHOOSEPIXELFORMATARBPROC) (HDC hdc, const int *piAttribIList, 
            const FLOAT *pfAttribFList, UINT nMaxFormats, int *piFormats, UINT *nNumFormats);

typedef HGLRC (WINAPI * PFNWGLCREATECONTEXTATTRIBSARBPROC) (HDC hDC, HGLRC hShareContext, 
                                                    const int *attribList);

ChoosePixelFormatFun s_ChoosePixelFormat = nullptr;
SetPixelFormatFun s_SetPixelFormat = nullptr;
DescribePixelFormatFun s_DescribePixelFormat = nullptr;
SwapBuffersFun s_SwapBuffers = nullptr;

PFNWGLGETPROCADDRESSPROC s_wglGetProcAddress = nullptr;
PFNWGLCREATECONTEXTPROC s_wglCreateContext = nullptr;
PFNWGLDELETECONTEXTPROC s_wglDeleteContext = nullptr;
PFNWGLMAKECURRENTPROC s_wglMakeCurrent = nullptr;

PFNWGLSWAPINTERVALEXTPROC s_wglSwapIntervalEXT = nullptr;
PFNWGLCHOOSEPIXELFORMATARBPROC s_wglChoosePixelFormatARB = nullptr;
PFNWGLCREATECONTEXTATTRIBSARBPROC s_wglCreateContextAttribsARB = nullptr;

void* PAL_LoadGdi(const char* func_name);
void* PAL_LoadWGL(const char* func_name);

void PAL_WGLCreateDummyContext();
HGLRC PAL_WGLCreateContext(HWND window, HDC* dc, b8 debug, i32 major, i32 minor);