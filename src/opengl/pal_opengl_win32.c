
// ==================================================
// Includes
// ==================================================

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

#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

// ==================================================
// Typedefs, enums and structs
// ==================================================

#define PAL_GL_CLASS L"PALGLClass"

// check to see if this is not defined yet
#ifndef GL_VENDOR
#define GL_VENDOR 0x1F00
#define GL_RENDERER 0x1F01
#define GL_VERSION 0x1F02
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
    bool initialized;
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

static Gdi s_Gdi;
static Wgl s_Wgl;

// ==================================================
// Internal API
// ==================================================

static inline bool checkExtension(
    const char* extension,
    const char* extensions)
{
    const char* start = extensions;
    size_t extensionLen = strlen(extension);

    for (;;) {
        const char* where = nullptr;
        const char* terminator = nullptr;

        where = strstr(start, extension);
        if (!where) {
            return false;
        }

        // the extension was found, we find the terminator by adding the sizeof
        // the extension
        terminator = where + extensionLen;
        if (where == start || *(where - 1) == ' ') {
            if (*terminator == ' ' || *terminator == '\0') {
                return true;
            }
        }

        start = terminator;
    }
}

// ==================================================
// Public API
// ==================================================

PalResult PAL_CALL palInitGL(const PalAllocator* allocator)
{
    if (s_Wgl.initialized) {
        return PAL_RESULT_SUCCESS;
    }

    if (allocator && (!allocator->allocate || !allocator->free)) {
        return PAL_RESULT_INVALID_ALLOCATOR;
    }

    // get the instance
    s_Wgl.instance = GetModuleHandleW(nullptr);

    // register class
    WNDCLASSEXW wc = {0};
    wc.style = CS_OWNDC;
    wc.lpfnWndProc = DefWindowProcW;
    wc.lpszClassName = PAL_GL_CLASS;
    wc.cbSize = sizeof(WNDCLASSEXW);

    // since we check every input carefully, the only error we can get is access
    // denied
    if (!RegisterClassExW(&wc)) {
        return PAL_RESULT_ACCESS_DENIED;
    }

    // create hidden window
    s_Wgl.window = CreateWindowExW(
        0,
        PAL_GL_CLASS,
        L"Dummy Window",
        0,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        0,
        0,
        s_Wgl.instance,
        0);

    if (!s_Wgl.window) {
        return PAL_RESULT_PLATFORM_FAILURE;
    }

    s_Gdi.handle = LoadLibraryA("gdi32.dll");
    s_Wgl.opengl = LoadLibraryA("opengl32.dll");
    if (!s_Gdi.handle || !s_Wgl.opengl) {
        return PAL_RESULT_PLATFORM_FAILURE;
    }

    // load gdi function pointers
    s_Gdi.choosePixelFormat =
        (ChoosePixelFormatFn)GetProcAddress(s_Gdi.handle, "ChoosePixelFormat");

    s_Gdi.setPixelFormat =
        (SetPixelFormatFn)GetProcAddress(s_Gdi.handle, "SetPixelFormat");

    s_Gdi.getPixelFormat =
        (GetPixelFormatFn)GetProcAddress(s_Gdi.handle, "GetPixelFormat");

    s_Gdi.describePixelFormat = (DescribePixelFormatFn)GetProcAddress(
        s_Gdi.handle,
        "DescribePixelFormat");

    s_Gdi.swapBuffers =
        (SwapBuffersFn)GetProcAddress(s_Gdi.handle, "SwapBuffers");

    // load wgl function pointers
    s_Wgl.wglGetProcAddress =
        (wglGetProcAddressFn)GetProcAddress(s_Wgl.opengl, "wglGetProcAddress");

    s_Wgl.wglCreateContext =
        (wglCreateContextFn)GetProcAddress(s_Wgl.opengl, "wglCreateContext");

    s_Wgl.wglDeleteContext =
        (wglDeleteContextFn)GetProcAddress(s_Wgl.opengl, "wglDeleteContext");

    s_Wgl.wglMakeCurrent =
        (wglMakeCurrentFn)GetProcAddress(s_Wgl.opengl, "wglMakeCurrent");
        
    s_Wgl.wglShareLists =
        (wglShareListsFn)GetProcAddress(s_Wgl.opengl, "wglShareLists");

    if (!s_Gdi.choosePixelFormat || !s_Gdi.describePixelFormat ||
        !s_Gdi.swapBuffers || !s_Gdi.setPixelFormat) {
        return PAL_RESULT_PLATFORM_FAILURE;
    }

    if (!s_Wgl.wglGetProcAddress || !s_Wgl.wglCreateContext ||
        !s_Wgl.wglDeleteContext || !s_Wgl.wglMakeCurrent) {
        return PAL_RESULT_PLATFORM_FAILURE;
    }

    s_Wgl.hdc = GetDC(s_Wgl.window);
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

    Int32 pixelFormat = s_Gdi.choosePixelFormat(s_Wgl.hdc, &pfd);
    s_Gdi.setPixelFormat(s_Wgl.hdc, pixelFormat, &pfd);

    s_Wgl.context = s_Wgl.wglCreateContext(s_Wgl.hdc);
    if (!s_Wgl.wglMakeCurrent(s_Wgl.hdc, s_Wgl.context)) {
        return PAL_RESULT_PLATFORM_FAILURE;
    }

    // load wgl extension function pointers
    s_Wgl.wglChoosePixelFormatARB =
        (wglChoosePixelFormatARBFn)s_Wgl.wglGetProcAddress(
            "wglChoosePixelFormatARB");

    s_Wgl.wglGetPixelFormatAttribivARB =
        (wglGetPixelFormatAttribivARBFn)s_Wgl.wglGetProcAddress(
            "wglGetPixelFormatAttribivARB");

    s_Wgl.wglCreateContextAttribsARB =
        (wglCreateContextAttribsARBFn)s_Wgl.wglGetProcAddress(
            "wglCreateContextAttribsARB");

    s_Wgl.wglSwapIntervalEXT =
        (wglSwapIntervalEXTFn)s_Wgl.wglGetProcAddress("wglSwapIntervalEXT");

    s_Wgl.wglGetExtensionsStringARB =
        (wglGetExtensionsStringARBFn)s_Wgl.wglGetProcAddress(
            "wglGetExtensionsStringARB");

    s_Wgl.wglGetExtensionsStringEXT =
        (wglGetExtensionsStringEXTFn)s_Wgl.wglGetProcAddress(
            "wglGetExtensionsStringEXT");

    // load gl functions
    s_Wgl.glGetString =
        (glGetStringFn)GetProcAddress(s_Wgl.opengl, "glGetString");

    const char* version = (const char*)s_Wgl.glGetString(GL_VERSION);
    if (version) {
#ifdef _MSC_VER
        sscanf_s(version, "%d.%d", &s_Wgl.info.major, &s_Wgl.info.minor);
#else
        sscanf(version, "%d.%d", &s_Wgl.info.major, &s_Wgl.info.minor);
#endif
    }

    const char* renderer = (const char*)s_Wgl.glGetString(GL_RENDERER);
    const char* vendor = (const char*)s_Wgl.glGetString(GL_VENDOR);
    strcpy(s_Wgl.info.vendor, vendor);
    strcpy(s_Wgl.info.version, version);
    strcpy(s_Wgl.info.graphicsCard, renderer);

    // check available extensions
    const char* extensions = nullptr;
    if (s_Wgl.wglGetExtensionsStringARB) {
        extensions = s_Wgl.wglGetExtensionsStringARB(s_Wgl.hdc);

    } else if (s_Wgl.wglGetExtensionsStringEXT) {
        extensions = s_Wgl.wglGetExtensionsStringEXT();
    }

    if (extensions) {
        // multisample
        if (checkExtension("WGL_ARB_multisample", extensions)) {
            s_Wgl.info.extensions |= PAL_GL_EXTENSION_MULTISAMPLE;
        }
        // multisample

        // color space
        if (checkExtension("WGL_ARB_framebuffer_sRGB", extensions)) {
            s_Wgl.info.extensions |= PAL_GL_EXTENSION_COLORSPACE_SRGB;
        }

        if (checkExtension("WGL_EXT_framebuffer_sRGB", extensions)) {
            s_Wgl.info.extensions |= PAL_GL_EXTENSION_COLORSPACE_SRGB;
        }

        if (checkExtension("WGL_EXT_colorspace", extensions)) {
            s_Wgl.info.extensions |= PAL_GL_EXTENSION_COLORSPACE_SRGB;
        }
        // color space

        // create context
        if (checkExtension("WGL_ARB_create_context", extensions)) {
            s_Wgl.info.extensions |= PAL_GL_EXTENSION_CREATE_CONTEXT;
        }
        // create context

        // create profile
        if (checkExtension("WGL_ARB_create_context_profile", extensions)) {
            s_Wgl.info.extensions |= PAL_GL_EXTENSION_CONTEXT_PROFILE;
        }
        // create profile

        // create profile es2
        if (checkExtension("WGL_EXT_create_context_es2_profile", extensions)) {
            s_Wgl.info.extensions |= PAL_GL_EXTENSION_CONTEXT_PROFILE_ES2;
        }
        // create profile es2

        // robustness
        if (checkExtension("WGL_ARB_create_context_robustness", extensions)) {
            s_Wgl.info.extensions |= PAL_GL_EXTENSION_ROBUSTNESS;
        }
        // robustness

        // no error
        if (checkExtension("WGL_ARB_create_context_no_error", extensions)) {
            s_Wgl.info.extensions |= PAL_GL_EXTENSION_NO_ERROR;
        }
        // no error

        // swap control
        if (checkExtension("WGL_EXT_swap_control", extensions)) {
            s_Wgl.info.extensions |= PAL_GL_EXTENSION_SWAP_CONTROL;
        }
        // swap control

        // flush control
        if (checkExtension("WGL_ARB_context_flush_control", extensions)) {
            s_Wgl.info.extensions |= PAL_GL_EXTENSION_FLUSH_CONTROL;
        }
        // flush control

        // pixel format
        if (checkExtension("WGL_ARB_pixel_format", extensions)) {
            s_Wgl.info.extensions |= PAL_GL_EXTENSION_PIXEL_FORMAT;
        }
        // pixel format
    }

    s_Wgl.initialized = true;
    return PAL_RESULT_SUCCESS;
}

void PAL_CALL palShutdownGL()
{
    if (!s_Wgl.initialized) {
        return;
    }

    s_Wgl.wglMakeCurrent(s_Wgl.hdc, nullptr);
    s_Wgl.wglDeleteContext(s_Wgl.context);
    ReleaseDC(s_Wgl.window, s_Wgl.hdc);
    DestroyWindow(s_Wgl.window);
    UnregisterClassW(PAL_GL_CLASS, s_Wgl.instance);

    FreeLibrary(s_Wgl.opengl);
    FreeLibrary(s_Gdi.handle);
    s_Wgl.initialized = false;
}

const PalGLInfo* PAL_CALL palGetGLInfo()
{

    if (!s_Wgl.initialized) {
        return nullptr;
    }
    return &s_Wgl.info;
}

PalResult PAL_CALL palEnumerateGLFBConfigs(
    PalGLWindow* glWindow,
    Int32* count,
    PalGLFBConfig* configs)
{

    if (!s_Wgl.initialized) {
        return PAL_RESULT_GL_NOT_INITIALIZED;
    }

    if (!count || !glWindow) {
        return PAL_RESULT_NULL_POINTER;
    }

    if (count == 0 && configs) {
        return PAL_RESULT_INSUFFICIENT_BUFFER;
    }

    HDC windowDC = GetDC((HWND)glWindow->window);
    if (!windowDC) {
        return PAL_RESULT_INVALID_GL_WINDOW;
    }

    Int32 configCount = 0;
    Int32 maxConfigCount = 0;
    Int32 nativeCount = 0;
    const Int32 configAttrib = WGL_NUMBER_PIXEL_FORMATS_ARB;

    if (configs) {
        maxConfigCount = *count;
    }

    // check if we support modern extention
    if (s_Wgl.wglGetPixelFormatAttribivARB) {
        // get framebuffer config with extensions
        if (!s_Wgl.wglGetPixelFormatAttribivARB(
                windowDC,
                0,
                0,
                1,
                &configAttrib,
                &nativeCount)) {
            return PAL_RESULT_PLATFORM_FAILURE;
        }

        // attributes we care about
        Int32 attributes[] = {
            WGL_SUPPORT_OPENGL_ARB,
            WGL_DRAW_TO_WINDOW_ARB,
            WGL_PIXEL_TYPE_ARB,
            WGL_ACCELERATION_ARB,
            WGL_RED_BITS_ARB,
            WGL_GREEN_BITS_ARB,
            WGL_BLUE_BITS_ARB,
            WGL_ALPHA_BITS_ARB,
            WGL_DEPTH_BITS_ARB,
            WGL_STENCIL_BITS_ARB,
            WGL_SAMPLES_ARB,
            WGL_STEREO_ARB,
            WGL_DOUBLE_BUFFER_ARB,
            WGL_FRAMEBUFFER_SRGB_CAPABLE_ARB};

        Int32 values[sizeof(attributes) / sizeof(attributes[0])];
        for (Int32 i = 1; i <= nativeCount; i++) {
            if (!s_Wgl.wglGetPixelFormatAttribivARB(
                    windowDC,
                    i,
                    0,
                    sizeof(attributes) / sizeof(attributes[0]),
                    attributes,
                    values)) {
                continue;
            }

            // we index the values list in the same way as the arributes list
            // so index 0 is WGL_SUPPORT_OPENGL_ARB and 1 is
            // WGL_DRAW_TO_WINDOW_ARB
            if (!values[0] || !values[1]) {
                // WGL_SUPPORT_OPENGL_ARB and WGL_DRAW_TO_WINDOW_ARB support
                continue;
            }

            if (values[2] != WGL_TYPE_RGBA_ARB) {
                // WGL_PIXEL_TYPE_ARB support
                continue;
            }

            if (values[3] == WGL_NO_ACCELERATION_ARB) {
                continue;
            }

            if (configs && configCount < maxConfigCount) {
                PalGLFBConfig* config = &configs[configCount];
                config->index = i;

                config->redBits = values[4];     // WGL_RED_BITS_ARB
                config->greenBits = values[5];   // WGL_GREEN_BITS_ARB
                config->blueBits = values[6];    // WGL_BLUE_BITS_ARB
                config->alphaBits = values[7];   // WGL_ALPHA_BITS_ARB
                config->depthBits = values[8];   // WGL_DEPTH_BITS_ARB
                config->stencilBits = values[9]; // WGL_STENCIL_BITS_ARB
                config->samples = values[10];    // WGL_SAMPLES_ARB

                if (config->samples == 0) {
                    config->samples = 1;
                }

                // WGL_STEREO_ARB
                config->stereo = values[11];

                // WGL_DOUBLE_BUFFER_ARB
                config->doubleBuffer = values[12];

                // WGL_FRAMEBUFFER_SRGB_CAPABLE_ARB
                config->sRGB = values[13];
            }
            configCount++;
        }

    } else {
        // get pixel format with legacy pixel descriptor
        nativeCount = s_Gdi.describePixelFormat(windowDC, 1, 0, nullptr);

        for (Int32 i = 1; i <= nativeCount; i++) {
            PIXELFORMATDESCRIPTOR pfd;
            if (!s_Gdi.describePixelFormat(
                    windowDC,
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
                continue;
            }

            if (configs && configCount < maxConfigCount) {
                PalGLFBConfig* config = &configs[configCount];
                config->index = i;

                config->redBits = pfd.cRedBits;
                config->greenBits = pfd.cGreenBits;
                config->blueBits = pfd.cBlueBits;
                config->alphaBits = pfd.cAlphaBits;
                config->depthBits = pfd.cDepthBits;
                config->stencilBits = pfd.cStencilBits;
                config->samples = 1;

                config->doubleBuffer =
                    (pfd.dwFlags & PFD_DOUBLEBUFFER) ? true : false;
                config->stereo = (pfd.dwFlags & PFD_STEREO) ? true : false;
                config->sRGB = false;
            }
            configCount++;
        }
    }

    ReleaseDC((HWND)glWindow->window, windowDC);
    if (!configs) {
        *count = configCount;
    }
    return PAL_RESULT_SUCCESS;
}

const PalGLFBConfig* PAL_CALL palGetClosestGLFBConfig(
    PalGLFBConfig* configs,
    Int32 count,
    const PalGLFBConfig* desired)
{
    if (!s_Wgl.initialized) {
        return nullptr;
    }

    if (!configs || !desired) {
        return nullptr;
    }

    if (count == 0) {
        return nullptr;
    }

    Int32 score = 0;
    Int32 bestScore = 0x7FFFFFFF;
    PalGLFBConfig* best = nullptr;
    for (Int32 i = 0; i < count; i++) {
        PalGLFBConfig* tmp = &configs[i];

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
            best = &configs[i];
        }
    }

    return best;
}

// ==================================================
// Context
// ==================================================

PalResult PAL_CALL palCreateGLContext(
    const PalGLContextCreateInfo* info,
    PalGLContext** outContext)
{
    if (!s_Wgl.initialized) {
        return PAL_RESULT_GL_NOT_INITIALIZED;
    }

    if (!info || !outContext || (info && (!info->window || !info->fbConfig))) {
        return PAL_RESULT_NULL_POINTER;
    }

    // check support for requested features
    if (info->profile != PAL_GL_PROFILE_NONE) {
        if (!(s_Wgl.info.extensions & PAL_GL_EXTENSION_CONTEXT_PROFILE)) {
            return PAL_RESULT_GL_EXTENSION_NOT_SUPPORTED;
        }
    }

    if (info->forward) {
        if (!(s_Wgl.info.extensions & PAL_GL_EXTENSION_CREATE_CONTEXT)) {
            return PAL_RESULT_GL_EXTENSION_NOT_SUPPORTED;
        }
    }

    if (info->reset != PAL_GL_CONTEXT_RESET_NONE) {
        if (!(s_Wgl.info.extensions & PAL_GL_EXTENSION_ROBUSTNESS)) {
            return PAL_RESULT_GL_EXTENSION_NOT_SUPPORTED;
        }
    }

    if (info->noError) {
        if (!(s_Wgl.info.extensions & PAL_GL_EXTENSION_NO_ERROR)) {
            return PAL_RESULT_GL_EXTENSION_NOT_SUPPORTED;
        }
    }

    if (info->release != PAL_GL_RELEASE_BEHAVIOR_NONE) {
        if (!(s_Wgl.info.extensions & PAL_GL_EXTENSION_FLUSH_CONTROL)) {
            return PAL_RESULT_GL_EXTENSION_NOT_SUPPORTED;
        }
    }

    // check version
    bool valid =
        info->major < s_Wgl.info.major ||
        (info->major == s_Wgl.info.major && info->minor <= s_Wgl.info.minor);

    if (!valid) {
        return PAL_RESULT_INVALID_GL_VERSION;
    }

    HDC hdc = GetDC((HWND)info->window->window);
    if (!hdc) {
        return PAL_RESULT_INVALID_GL_WINDOW;
    }

    // check if the window's pixel format has already been set
    if (s_Gdi.getPixelFormat(hdc) == 0) {
        // set the pixel format
        Int32 pixelFormat = info->fbConfig->index;
        // since we have the pixel format already
        // we ask the OS (platform) to fill the pfd struct for us from that
        // index
        PIXELFORMATDESCRIPTOR pfd;
        if (!s_Gdi.describePixelFormat(
                hdc,
                pixelFormat,
                sizeof(PIXELFORMATDESCRIPTOR),
                &pfd)) {
            return PAL_RESULT_INVALID_GL_FBCONFIG;
        }

        // we then set the pixel format for the hdc
        if (!s_Gdi.setPixelFormat(hdc, pixelFormat, &pfd)) {
            return PAL_RESULT_INVALID_GL_FBCONFIG;
        }
    }

    HGLRC context = nullptr;
    if (s_Wgl.wglCreateContextAttribsARB) {
        // create context with modern wgl functions
        Int32 attribs[40];
        Int32 index = 0;
        Int32 profile = 0;
        Int32 flags = 0;

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
            attribs[index++] = true;
        }

        // release
        if (s_Wgl.info.extensions & PAL_GL_EXTENSION_FLUSH_CONTROL) {
            if (info->release != PAL_GL_RELEASE_BEHAVIOR_NONE) {
                attribs[index++] = WGL_CONTEXT_RELEASE_BEHAVIOR_ARB;
                attribs[index++] = WGL_CONTEXT_RELEASE_BEHAVIOR_FLUSH_ARB;
            }
        }

        if (flags) {
            attribs[index++] = WGL_CONTEXT_FLAGS_ARB;
            attribs[index++] = flags;
        }
        attribs[index++] = 0;

        context = s_Wgl.wglCreateContextAttribsARB(hdc, nullptr, attribs);
        if (!context) {
            DWORD error = GetLastError();
            if (error == ERROR_INVALID_PROFILE_ARB) {
                return PAL_RESULT_INVALID_GL_PROFILE;
            } else {
                return PAL_RESULT_PLATFORM_FAILURE;
            }
        }

    } else {
        // create context with legacy wgl functions
        context = s_Wgl.wglCreateContext(hdc);
        if (!context) {
            return PAL_RESULT_PLATFORM_FAILURE;
        }
    }

    // share context
    if (info->shareContext) {
        if (!s_Wgl.wglShareLists((HGLRC)info->shareContext, context)) {
            s_Wgl.wglDeleteContext(context);
            ReleaseDC((HWND)info->window->window, hdc);
            return PAL_RESULT_PLATFORM_FAILURE;
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
        return PAL_RESULT_GL_NOT_INITIALIZED;
    }

    if ((!glWindow && context) || (glWindow && !context)) {
        return PAL_RESULT_NULL_POINTER;
    }

    if (context && glWindow) {
        // get hdc
        HDC hdc = GetDC((HWND)glWindow->window);
        if (!hdc) {
            return PAL_RESULT_INVALID_GL_WINDOW;
        }

        if (!s_Wgl.wglMakeCurrent(hdc, (HGLRC)context)) {
            DWORD error = GetLastError();
            if (error == ERROR_INVALID_HANDLE) {
                return PAL_RESULT_INVALID_GL_CONTEXT;

            } else {
                return PAL_RESULT_PLATFORM_FAILURE;
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
        return PAL_RESULT_GL_NOT_INITIALIZED;
    }

    if (!context || !glWindow) {
        return PAL_RESULT_NULL_POINTER;
    }

    // get hdc
    HDC hdc = GetDC((HWND)glWindow->window);
    if (!hdc) {
        return PAL_RESULT_INVALID_GL_WINDOW;
    }

    if (!s_Gdi.swapBuffers(hdc)) {
        DWORD error = GetLastError();
        if (error == ERROR_INVALID_PIXEL_FORMAT) {
            return PAL_RESULT_INVALID_GL_FBCONFIG;

        } else {
            return PAL_RESULT_PLATFORM_FAILURE;
        }
    }

    ReleaseDC((HWND)glWindow->window, hdc);
    return PAL_RESULT_SUCCESS;
}

void* PAL_CALL palGLGetProcAddress(const char* name)
{
    if (!s_Wgl.initialized) {
        return nullptr;
    }

    void* proc = s_Wgl.wglGetProcAddress(name);
    if (!proc) {
        proc = (void*)GetProcAddress(s_Wgl.opengl, name);
    }
    return proc;
}

PalResult PAL_CALL palSetSwapInterval(Int32 interval)
{
    if (!s_Wgl.initialized) {
        return PAL_RESULT_GL_NOT_INITIALIZED;
    }

    if (!s_Wgl.wglSwapIntervalEXT) {
        return PAL_RESULT_GL_EXTENSION_NOT_SUPPORTED;
    }

    s_Wgl.wglSwapIntervalEXT(interval);
    return PAL_RESULT_SUCCESS;
}