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

#include "wgl.h"
#include "opengl/platform.h"
#include "pal2/opengl/init.h"

#define GL_CLASS L"PALGLClass"

GDI g_Gdi = {0};
WGL g_Wgl = {0};
static PalBool s_SupportedAPIs[2] = {0};

/* =========================================================================
 * Helper
 * ========================================================================= */

typedef union WGLSymbol
{
    void* raw;
    PalLibrarySymbol symbol;
} WGLSymbol;

static PalLibrarySymbol loadWGLProc(const char* name)
{
    WGLSymbol sym;
    sym.raw = g_Wgl.getProcAddress(name);
    if (sym.raw) {
        return sym.symbol;
    }
    
    return nullptr;
}

static void loadProcs()
{
    // clang-format off

    g_Wgl.getProcAddress = (wglGetProcAddressFn)palGetSymbol(
        g_Wgl.opengl,
        "wglGetProcAddress");

    g_Gdi.choosePixelFormat = (ChoosePixelFormatFn)palGetSymbol(
        g_Gdi.gdi,
        "ChoosePixelFormat");

    g_Gdi.setPixelFormat = (SetPixelFormatFn)palGetSymbol(
        g_Gdi.gdi,
        "SetPixelFormat");

    g_Gdi.getPixelFormat = (GetPixelFormatFn)palGetSymbol(
        g_Gdi.gdi,
        "GetPixelFormat");

    g_Gdi.describePixelFormat = (DescribePixelFormatFn)palGetSymbol(
        g_Gdi.gdi,
        "DescribePixelFormat");

    g_Gdi.swapBuffers = (SwapBuffersFn)palGetSymbol(
        g_Gdi.gdi,
        "SwapBuffers");

    g_Wgl.createContext = (wglCreateContextFn)palGetSymbol(
        g_Wgl.opengl,
        "wglCreateContext");

    g_Wgl.deleteContext = (wglDeleteContextFn)palGetSymbol(
        g_Wgl.opengl,
        "wglDeleteContext");

    g_Wgl.makeCurrent = (wglMakeCurrentFn)palGetSymbol(
        g_Wgl.opengl,
        "wglMakeCurrent");

    g_Wgl.shareLists = (wglShareListsFn)palGetSymbol(
        g_Wgl.opengl,
        "wglShareLists");

    g_Wgl.choosePixelFormatARB = (wglChoosePixelFormatARBFn)loadWGLProc(
        "wglChoosePixelFormatARB");

    g_Wgl.getPixelFormatAttribivARB = (wglGetPixelFormatAttribivARBFn)loadWGLProc(
        "wglGetPixelFormatAttribivARB");

    g_Wgl.createContextAttribsARB = (wglCreateContextAttribsARBFn)loadWGLProc(
        "wglCreateContextAttribsARB");

    g_Wgl.swapIntervalEXT = (wglSwapIntervalEXTFn)loadWGLProc(
        "wglSwapIntervalEXT");

    g_Wgl.getExtensionsStringARB = (wglGetExtensionsStringARBFn)loadWGLProc(
        "wglGetExtensionsStringARB");

    g_Wgl.getExtensionsStringEXT = (wglGetExtensionsStringEXTFn)loadWGLProc(
        "wglGetExtensionsStringEXT");

    g_Wgl.glGetString = (glGetStringFn)palGetSymbol(
        g_Wgl.opengl,
        "glGetString");

    // clang-format on
}


/* =========================================================================
 * API
 * ========================================================================= */

void wglShutdownGL();

PalResult wglInitGL(
    PalGLAPI api,
    void* instance,
    const PalAllocator* allocator)
{
    PalResult result = PAL_RESULT_CODE_PLATFORM_FAILURE;
    g_GLInfo.api = PAL_GL_API_OPENGL;
    g_GLInfo.backend = PAL_GL_BACKEND_WGL;
    HGLRC context = nullptr;

    if (api != PAL_GL_API_OPENGL) {
        return PAL_RESULT_CODE_FEATURE_NOT_SUPPORTED;
    }

    g_Gdi.gdi = LoadLibraryA("gdi32.dll");
    g_Wgl.opengl = LoadLibraryA("opengl32.dll");
    if (!g_Gdi.gdi || !g_Wgl.opengl) {
        result = palMakeResult(PAL_RESULT_CODE_PLATFORM_FAILURE, PAL_RESULT_SOURCE_WIN32, GetLastError());
        goto cleanup;
    }

    g_Wgl.instance = instance;
    WNDCLASSEXW wc = {0};
    wc.style = CS_OWNDC;
    wc.lpfnWndProc = DefWindowProcW;
    wc.lpszClassName = GL_CLASS;
    wc.cbSize = sizeof(WNDCLASSEXW);

    if (!RegisterClassExW(&wc)) {
        result = palMakeResult(PAL_RESULT_CODE_PLATFORM_FAILURE, PAL_RESULT_SOURCE_WIN32, GetLastError());
        goto cleanup;
    }

    g_Wgl.window = CreateWindowExW(
        0,
        GL_CLASS,
        L"Dummy Window",
        0,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        0,
        0,
        g_Wgl.instance,
        0);

    if (!g_Wgl.window) {
        result = palMakeResult(PAL_RESULT_CODE_PLATFORM_FAILURE, PAL_RESULT_SOURCE_WIN32, GetLastError());
        goto cleanup;
    }

    loadProcs();

    g_Wgl.hdc = GetDC(g_Wgl.window);

    PIXELFORMATDESCRIPTOR pfd = {0};
    pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
    pfd.nVersion = 1;
    pfd.iPixelType = PFD_TYPE_RGBA;
    pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
    pfd.cColorBits = 32;
    pfd.cAlphaBits = 8;
    pfd.iLayerType = PFD_MAIN_PLANE;
    pfd.cDepthBits = 24;
    pfd.cStencilBits = 8;

    int32_t pixelFormat = g_Gdi.choosePixelFormat(g_Wgl.hdc, &pfd);
    g_Gdi.setPixelFormat(g_Wgl.hdc, pixelFormat, &pfd);
    context = g_Wgl.createContext(g_Wgl.hdc);

    if (!g_Wgl.makeCurrent(g_Wgl.hdc, context)) {
        result = palMakeResult(PAL_RESULT_CODE_PLATFORM_FAILURE, PAL_RESULT_SOURCE_WIN32, GetLastError());
        goto cleanup;
    }

    const char* version = (const char*)g_Wgl.glGetString(GL_VERSION);
    if (version) {
#ifdef _MSC_VER
        sscanf_s(version, "%d.%d", &g_GLInfo.major, &g_GLInfo.minor);
#else
        sscanf(version, "%d.%d", &g_GLInfo.major, &g_GLInfo.minor);
#endif
        snprintf(g_GLInfo.version, sizeof(g_GLInfo.version), "%s", version);
    }

    const char* renderer = (const char*)g_Wgl.glGetString(GL_RENDERER);
    if (renderer) {
        snprintf(g_GLInfo.graphicsCard, sizeof(g_GLInfo.graphicsCard), "%s", renderer);
    }

    const char* vendor = (const char*)g_Wgl.glGetString(GL_VENDOR);
    if (vendor) {
        snprintf(g_GLInfo.vendor, sizeof(g_GLInfo.vendor), "%s", vendor);
    }

    const char* extensions = nullptr;
    if (g_Wgl.getExtensionsStringARB) {
        extensions = g_Wgl.getExtensionsStringARB(g_Wgl.hdc);
    } else if (g_Wgl.getExtensionsStringEXT) {
        extensions = g_Wgl.getExtensionsStringEXT();
    }

    if (extensions) {
        if (checkString("WGL_ARB_multisample", extensions)) {
            g_GLInfo.extensions |= PAL_GL_EXTENSION_MULTISAMPLE;
        }

        if (checkString("WGL_ARB_framebuffer_sRGB", extensions) ||
            checkString("WGL_EXT_framebuffer_sRGB", extensions) ||
            checkString("WGL_EXT_colorspace", extensions)) {
            g_GLInfo.extensions |= PAL_GL_EXTENSION_COLORSPACE_SRGB;
        }

        if (checkString("WGL_ARB_create_context", extensions)) {
            g_GLInfo.extensions |= PAL_GL_EXTENSION_CREATE_CONTEXT;
        }

        if (checkString("WGL_ARB_create_context_profile", extensions)) {
            g_GLInfo.extensions |= PAL_GL_EXTENSION_CONTEXT_PROFILE;
        }

        if (checkString("WGL_EXT_create_context_es2_profile", extensions)) {
            g_GLInfo.extensions |= PAL_GL_EXTENSION_CONTEXT_PROFILE_ES2;
        }

        if (checkString("WGL_ARB_create_context_robustness", extensions)) {
            g_GLInfo.extensions |= PAL_GL_EXTENSION_ROBUSTNESS;
        }

        if (checkString("WGL_ARB_create_context_no_error", extensions)) {
            g_GLInfo.extensions |= PAL_GL_EXTENSION_NO_ERROR;
        }

        if (checkString("WGL_EXT_swap_control", extensions)) {
            g_GLInfo.extensions |= PAL_GL_EXTENSION_SWAP_CONTROL;
        }

        if (checkString("WGL_ARB_context_flush_control", extensions)) {
            g_GLInfo.extensions |= PAL_GL_EXTENSION_FLUSH_CONTROL;
        }

        if (checkString("WGL_ARB_pixel_format", extensions)) {
            g_GLInfo.extensions |= PAL_GL_EXTENSION_PIXEL_FORMAT;
        }
    }

    g_Wgl.makeCurrent(nullptr, nullptr);
    g_Wgl.deleteContext(context);

    return PAL_RESULT_SUCCESS;

cleanup:
    wglShutdownGL();
    
    return result;
}

void wglShutdownGL()
{
    g_Wgl.makeCurrent(g_Wgl.hdc, nullptr);

    if (g_Wgl.window) {
        ReleaseDC(g_Wgl.window, g_Wgl.hdc);
        DestroyWindow(g_Wgl.window);
        UnregisterClassW(GL_CLASS, g_Wgl.instance);
    }

    FreeLibrary(g_Wgl.opengl);
    FreeLibrary(g_Gdi.gdi);

    memset(&g_Wgl, 0, sizeof(WGL));
}

void* wglGetGLProcAddress(const char* name)
{
    void* proc = g_Wgl.getProcAddress(name);
    if (!proc) {
        proc = (void*)GetProcAddress(g_Wgl.opengl, name);
    }
    return proc;
}

const PalBool* wglGetSupportedGLAPIs(void* instance)
{
    if (instance) {
        s_SupportedAPIs[PAL_GL_API_OPENGL] = PAL_TRUE;
        s_SupportedAPIs[PAL_GL_API_OPENGL_ES] = PAL_FALSE;
        return s_SupportedAPIs;
    }

    s_SupportedAPIs[PAL_GL_API_OPENGL] = PAL_FALSE;
    s_SupportedAPIs[PAL_GL_API_OPENGL_ES] = PAL_FALSE;
    return s_SupportedAPIs;
}