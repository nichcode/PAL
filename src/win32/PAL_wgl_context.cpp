
#include "PAL_pch.h"
#include "opengl/PAL_glfuncs.h"
#include "PAL_wgl_context.h"

void* PAL_LoadGdi(const char* func_name)
{
    return (void*)GetProcAddress((HMODULE)s_Data.gdi, func_name);
}

void* PAL_LoadWGL(const char* func_name)
{
    if (!s_wglGetProcAddress) {
        s_wglGetProcAddress = (PFNWGLGETPROCADDRESSPROC)GetProcAddress(
            (HMODULE)s_Data.opengl, "wglGetProcAddress");
    }
    
    void* proc = (void*)s_wglGetProcAddress(func_name);
    if (!proc) { proc = (void*)GetProcAddress((HMODULE)s_Data.opengl, func_name); }
    return proc;
}

void PAL_WGLCreateDummyContext()
{
    s_ChoosePixelFormat = (ChoosePixelFormatFun)PAL_LoadGdi("ChoosePixelFormat");
    s_SetPixelFormat = (SetPixelFormatFun)PAL_LoadGdi("SetPixelFormat");
    s_DescribePixelFormat = (DescribePixelFormatFun)PAL_LoadGdi("DescribePixelFormat");
    s_SwapBuffers = (SwapBuffersFun)PAL_LoadGdi("SwapBuffers");

    // load wgl procs
    s_wglCreateContext = (PFNWGLCREATECONTEXTPROC)PAL_LoadWGL("wglCreateContext");
    s_wglDeleteContext = (PFNWGLDELETECONTEXTPROC)PAL_LoadWGL("wglDeleteContext");
    s_wglMakeCurrent = (PFNWGLMAKECURRENTPROC)PAL_LoadWGL("wglMakeCurrent");

    WNDCLASSEXW window_class = {};
    window_class.style = CS_OWNDC;
    window_class.lpfnWndProc = DefWindowProcW;
    window_class.lpszClassName = L"PALDummyWindow";
    window_class.cbSize = sizeof(WNDCLASSEXW);

    auto class_id = RegisterClassExW(&window_class);

    HWND dummy_window = CreateWindowExW(
        0,
        MAKEINTATOM(class_id),
        L"PrimeDummyWindow",
        0,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        0,
        0,
        window_class.hInstance,
        0);

    HDC dummy_dc = GetDC(dummy_window);
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

    int pixel_format = s_ChoosePixelFormat(dummy_dc, &pfd);
    s_SetPixelFormat(dummy_dc, pixel_format, &pfd);

    HGLRC dummy_context = s_wglCreateContext(dummy_dc);
    bool res = s_wglMakeCurrent(dummy_dc, dummy_context);

    s_wglChoosePixelFormatARB = (PFNWGLCHOOSEPIXELFORMATARBPROC)PAL_LoadWGL("wglChoosePixelFormatARB");
    s_wglCreateContextAttribsARB = (PFNWGLCREATECONTEXTATTRIBSARBPROC)PAL_LoadWGL("wglCreateContextAttribsARB");
    s_wglSwapIntervalEXT = (PFNWGLSWAPINTERVALEXTPROC)PAL_LoadWGL("wglSwapIntervalEXT");

    LoadGL();
    ReleaseDC(dummy_window, dummy_dc);
    DestroyWindow(dummy_window);
}

HGLRC PAL_WGLCreateContext(HWND window, HDC* dc, b8 debug, i32 major, i32 minor)
{
    auto hdc = GetDC(window);

    int pixel_format_attrib[] = {
        WGL_DRAW_TO_WINDOW_ARB, GL_TRUE,
        WGL_SUPPORT_OPENGL_ARB, GL_TRUE,
        WGL_DOUBLE_BUFFER_ARB, GL_TRUE,
        WGL_SWAP_METHOD_ARB,  WGL_SWAP_COPY_ARB,
        WGL_ACCELERATION_ARB, WGL_FULL_ACCELERATION_ARB,
        WGL_PIXEL_TYPE_ARB, WGL_TYPE_RGBA_ARB,
        WGL_COLOR_BITS_ARB, 32,
        WGL_DEPTH_BITS_ARB, 24,
        WGL_STENCIL_BITS_ARB, 8,
        0
    };

    int pixel_format = 0;
    UINT num_format = 0;
    s_wglChoosePixelFormatARB(hdc, pixel_format_attrib, nullptr, 1, &pixel_format, &num_format);

    PIXELFORMATDESCRIPTOR pixel_format_desc = {};
    s_DescribePixelFormat(hdc, pixel_format, sizeof(PIXELFORMATDESCRIPTOR), &pixel_format_desc);
    s_SetPixelFormat(hdc, pixel_format, &pixel_format_desc);
    
    int* gl_flags = 0;
    if (debug) {
        int opengl_attrib[] = {
            WGL_CONTEXT_MAJOR_VERSION_ARB, major,
            WGL_CONTEXT_MINOR_VERSION_ARB, minor,
            WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
            WGL_CONTEXT_FLAGS_ARB, WGL_CONTEXT_DEBUG_BIT_ARB,
            0
        };

        gl_flags = opengl_attrib;
    }
    else {
        int opengl_attrib[] = {
            WGL_CONTEXT_MAJOR_VERSION_ARB, major,
            WGL_CONTEXT_MINOR_VERSION_ARB, minor,
            WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
            0
        };

        gl_flags = opengl_attrib;
    }

    HGLRC context = s_wglCreateContextAttribsARB(hdc, 0, gl_flags);
    s_wglMakeCurrent(hdc, context);
    *dc = hdc;

    return context;
}