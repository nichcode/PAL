
#include "PAL_pch.h"
#include "PAL_wgl_context.h"
#include "opengl/PAL_gl_funcs.h"

#define LOAD_WGL_FUNC(name) wglGetProcAddress(name)

PFNWGLCHOOSEPIXELFORMATARBPROC wglChoosePixelFormatARB = nullptr;
PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB = nullptr;

void _WGLCreateDummyContext()
{
    WNDCLASSEXW window_class = {};
    window_class.style = CS_OWNDC;
    window_class.lpfnWndProc = DefWindowProcW;
    window_class.lpszClassName = L"PrimeDummyWindow";
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

    CHECK_ERR(dummy_window, "Win32 dummy window creation failed", return);
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

    int pixel_format = ChoosePixelFormat(dummy_dc, &pfd);
    SetPixelFormat(dummy_dc, pixel_format, &pfd);

    HGLRC dummy_context = wglCreateContext(dummy_dc);
    bool res = wglMakeCurrent(dummy_dc, dummy_context);

    // wgl
    wglChoosePixelFormatARB = (PFNWGLCHOOSEPIXELFORMATARBPROC)LOAD_WGL_FUNC("wglChoosePixelFormatARB");
    CHECK_ERR(wglChoosePixelFormatARB, "failed to load wglChoosePixelFormatARB", return);
    
    wglCreateContextAttribsARB = (PFNWGLCREATECONTEXTATTRIBSARBPROC)LOAD_WGL_FUNC("wglCreateContextAttribsARB");
    CHECK_ERR(wglCreateContextAttribsARB, "failed to load wglCreateContextAttribs ARB", return);
    
    wglSwapIntervalEXT = (PFNWGLSWAPINTERVALEXTPROC)LOAD_WGL_FUNC("wglSwapIntervalEXT");
    CHECK_ERR(wglSwapIntervalEXT, "failed to load wglSwapIntervalEXT", return);

    _LoadGL();

    wglMakeCurrent(dummy_dc, 0);
    ReleaseDC(dummy_window, dummy_dc);
    DestroyWindow(dummy_window);
}

HGLRC _WGLCreateContext(HWND window, i32 major, i32 minor)
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
    wglChoosePixelFormatARB(hdc, pixel_format_attrib, nullptr, 1, &pixel_format, &num_format);
    CHECK_ERR(num_format, "failed to find pixel format", return nullptr);

    PIXELFORMATDESCRIPTOR pixel_format_desc = {};
    DescribePixelFormat(hdc, pixel_format, sizeof(PIXELFORMATDESCRIPTOR), &pixel_format_desc);
    SetPixelFormat(hdc, pixel_format, &pixel_format_desc);

    b8 valid = major < glVersion.major || (major == glVersion.major && minor <= glVersion.minor);
    if (!valid) { 
        _SetError("your device doesent support opengl version %i.%i", major, minor);
        return nullptr;
    }

    int opengl_attrib[] = {
        WGL_CONTEXT_MAJOR_VERSION_ARB, major,
        WGL_CONTEXT_MINOR_VERSION_ARB, minor,
        WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
        WGL_CONTEXT_FLAGS_ARB, WGL_CONTEXT_DEBUG_BIT_ARB,
        0
    };

    HGLRC context = wglCreateContextAttribsARB(hdc, 0, opengl_attrib);
    CHECK_ERR(context, "wgl context creation failed", return nullptr)
    ReleaseDC(window, hdc);
    return context;
}