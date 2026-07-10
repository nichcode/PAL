
/**
    PAL - Prime Abstraction Layer
    Copyright (C) 2025
    Licensed under the Zlib license. See LICENSE file in root.
 */

#ifdef _WIN32
#include "pal_wgl.h"
#include "opengl/pal_opengl_shared.h"
#include <stdio.h>
#include <stdlib.h>

Gdi s_Gdi = {0};
Wgl s_Wgl = {0};
static PalBool s_SupportedAPIs[2] = {0};

PalResult wglInitGL(
    PalGLAPI api,
    void* instance,
    const PalAllocator* allocator)
{
    if (api != PAL_GL_API_OPENGL) {
        return PAL_RESULT_CODE_FEATURE_NOT_SUPPORTED;
    }

    // register class
    s_Wgl.instance = instance;
    WNDCLASSEXW wc = {0};
    wc.style = CS_OWNDC;
    wc.lpfnWndProc = DefWindowProcW;
    wc.lpszClassName = PAL_GL_CLASS;
    wc.cbSize = sizeof(WNDCLASSEXW);

    // since we check every input carefully, the only error we can get is access
    // denied
    if (!RegisterClassExW(&wc)) {
        return palMakeResult(
            PAL_RESULT_CODE_PLATFORM_FAILURE, 
            PAL_RESULT_SOURCE_WIN32, 
            GetLastError());
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
        return palMakeResult(
            PAL_RESULT_CODE_PLATFORM_FAILURE, 
            PAL_RESULT_SOURCE_WIN32, 
            GetLastError());
    }

    s_Gdi.handle = LoadLibraryA("gdi32.dll");
    s_Wgl.opengl = LoadLibraryA("opengl32.dll");
    if (!s_Gdi.handle || !s_Wgl.opengl) {
        return palMakeResult(
            PAL_RESULT_CODE_PLATFORM_FAILURE, 
            PAL_RESULT_SOURCE_WIN32, 
            GetLastError());
    }

    // clang-format off
    // load gdi function pointers
    s_Gdi.choosePixelFormat = (ChoosePixelFormatFn)GetProcAddress(
        s_Gdi.handle,
        "ChoosePixelFormat");

    s_Gdi.setPixelFormat = (SetPixelFormatFn)GetProcAddress(
        s_Gdi.handle,
        "SetPixelFormat");

    s_Gdi.getPixelFormat = (GetPixelFormatFn)GetProcAddress(
        s_Gdi.handle,
        "GetPixelFormat");

    s_Gdi.describePixelFormat = (DescribePixelFormatFn)GetProcAddress(
        s_Gdi.handle,
        "DescribePixelFormat");

    s_Gdi.swapBuffers = (SwapBuffersFn)GetProcAddress(
        s_Gdi.handle,
        "SwapBuffers");

    // load wgl function pointers
    s_Wgl.getProcAddress = (wglGetProcAddressFn)GetProcAddress(
        s_Wgl.opengl,
        "wglGetProcAddress");

    s_Wgl.createContext = (wglCreateContextFn)GetProcAddress(
        s_Wgl.opengl,
        "wglCreateContext");

    s_Wgl.deleteContext = (wglDeleteContextFn)GetProcAddress(
        s_Wgl.opengl,
        "wglDeleteContext");

    s_Wgl.makeCurrent = (wglMakeCurrentFn)GetProcAddress(
        s_Wgl.opengl,
        "wglMakeCurrent");

    s_Wgl.shareLists = (wglShareListsFn)GetProcAddress(
        s_Wgl.opengl,
        "wglShareLists");

    if (!s_Gdi.choosePixelFormat     ||
        !s_Gdi.describePixelFormat   ||
        !s_Gdi.swapBuffers           ||
        !s_Gdi.setPixelFormat) {
        return PAL_RESULT_CODE_PLATFORM_FAILURE;
    }

    if (!s_Wgl.getProcAddress    ||
        !s_Wgl.createContext     ||
        !s_Wgl.deleteContext     ||
        !s_Wgl.makeCurrent) {
        return PAL_RESULT_CODE_PLATFORM_FAILURE;
    }
    // clang-format on

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

    int32_t pixelFormat = s_Gdi.choosePixelFormat(s_Wgl.hdc, &pfd);
    s_Gdi.setPixelFormat(s_Wgl.hdc, pixelFormat, &pfd);
    s_Wgl.context = s_Wgl.createContext(s_Wgl.hdc);

    if (!s_Wgl.makeCurrent(s_Wgl.hdc, s_Wgl.context)) {
        return palMakeResult(
            PAL_RESULT_CODE_PLATFORM_FAILURE, 
            PAL_RESULT_SOURCE_WIN32, 
            GetLastError());
    }

    // clang-format off
    // load wgl extension function pointers
    s_Wgl.choosePixelFormatARB = (wglChoosePixelFormatARBFn)s_Wgl.getProcAddress(
        "wglChoosePixelFormatARB");

    s_Wgl.getPixelFormatAttribivARB = (wglGetPixelFormatAttribivARBFn)s_Wgl.getProcAddress(
        "wglGetPixelFormatAttribivARB");

    s_Wgl.createContextAttribsARB = (wglCreateContextAttribsARBFn)s_Wgl.getProcAddress(
        "wglCreateContextAttribsARB");

    s_Wgl.swapIntervalEXT = (wglSwapIntervalEXTFn)s_Wgl.getProcAddress(
        "wglSwapIntervalEXT");

    s_Wgl.getExtensionsStringARB = (wglGetExtensionsStringARBFn)s_Wgl.getProcAddress(
        "wglGetExtensionsStringARB");

    s_Wgl.getExtensionsStringEXT = (wglGetExtensionsStringEXTFn)s_Wgl.getProcAddress(
        "wglGetExtensionsStringEXT");

    // load gl functions
    s_Wgl.glGetString = (glGetStringFn)GetProcAddress(
        s_Wgl.opengl,
        "glGetString");
    // clang-format on

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
    if (s_Wgl.getExtensionsStringARB) {
        extensions = s_Wgl.getExtensionsStringARB(s_Wgl.hdc);

    } else if (s_Wgl.getExtensionsStringEXT) {
        extensions = s_Wgl.getExtensionsStringEXT();
    }

    if (extensions) {
        // multisample
        if (checkString("WGL_ARB_multisample", extensions)) {
            s_Wgl.info.extensions |= PAL_GL_EXTENSION_MULTISAMPLE;
        }
        // multisample

        // color space
        if (checkString("WGL_ARB_framebuffer_sRGB", extensions)) {
            s_Wgl.info.extensions |= PAL_GL_EXTENSION_COLORSPACE_SRGB;
        }

        if (checkString("WGL_EXT_framebuffer_sRGB", extensions)) {
            s_Wgl.info.extensions |= PAL_GL_EXTENSION_COLORSPACE_SRGB;
        }

        if (checkString("WGL_EXT_colorspace", extensions)) {
            s_Wgl.info.extensions |= PAL_GL_EXTENSION_COLORSPACE_SRGB;
        }
        // color space

        // create context
        if (checkString("WGL_ARB_create_context", extensions)) {
            s_Wgl.info.extensions |= PAL_GL_EXTENSION_CREATE_CONTEXT;
        }
        // create context

        // create profile
        if (checkString("WGL_ARB_create_context_profile", extensions)) {
            s_Wgl.info.extensions |= PAL_GL_EXTENSION_CONTEXT_PROFILE;
        }
        // create profile

        // create profile es2
        if (checkString("WGL_EXT_create_context_es2_profile", extensions)) {
            s_Wgl.info.extensions |= PAL_GL_EXTENSION_CONTEXT_PROFILE_ES2;
        }
        // create profile es2

        // robustness
        if (checkString("WGL_ARB_create_context_robustness", extensions)) {
            s_Wgl.info.extensions |= PAL_GL_EXTENSION_ROBUSTNESS;
        }
        // robustness

        // no error
        if (checkString("WGL_ARB_create_context_no_error", extensions)) {
            s_Wgl.info.extensions |= PAL_GL_EXTENSION_NO_ERROR;
        }
        // no error

        // swap control
        if (checkString("WGL_EXT_swap_control", extensions)) {
            s_Wgl.info.extensions |= PAL_GL_EXTENSION_SWAP_CONTROL;
        }
        // swap control

        // flush control
        if (checkString("WGL_ARB_context_flush_control", extensions)) {
            s_Wgl.info.extensions |= PAL_GL_EXTENSION_FLUSH_CONTROL;
        }
        // flush control

        // pixel format
        if (checkString("WGL_ARB_pixel_format", extensions)) {
            s_Wgl.info.extensions |= PAL_GL_EXTENSION_PIXEL_FORMAT;
        }
        // pixel format
    }

    s_Wgl.info.api = PAL_GL_API_OPENGL;
    s_Wgl.info.backend = PAL_GL_BACKEND_WGL;
    return PAL_RESULT_SUCCESS;
}

void wglShutdownGL()
{
    s_Wgl.makeCurrent(s_Wgl.hdc, nullptr);
    s_Wgl.deleteContext(s_Wgl.context);
    ReleaseDC(s_Wgl.window, s_Wgl.hdc);
    DestroyWindow(s_Wgl.window);
    UnregisterClassW(PAL_GL_CLASS, s_Wgl.instance);

    FreeLibrary(s_Wgl.opengl);
    FreeLibrary(s_Gdi.handle);

    memset(&s_Wgl, 0, sizeof(Wgl));
}

const PalGLInfo* wglGetGLInfo()
{
    return &s_Wgl.info;
}

PalResult wglEnumerateGLFBConfigs(
    uint32_t* count,
    PalGLFBConfig* configs)
{
    int32_t configCount = 0;
    int32_t maxConfigCount = 0;
    int32_t nativeCount = 0;
    const int32_t configAttrib = WGL_NUMBER_PIXEL_FORMATS_ARB;

    if (configs) {
        maxConfigCount = *count;
    }

    // check if we support modern extention
    if (s_Wgl.getPixelFormatAttribivARB) {
        // get framebuffer config with extensions
        if (!s_Wgl.getPixelFormatAttribivARB(s_Wgl.hdc, 0, 0, 1, &configAttrib, &nativeCount)) {
            return palMakeResult(
                PAL_RESULT_CODE_PLATFORM_FAILURE, 
                PAL_RESULT_SOURCE_WIN32, 
                GetLastError());
        }

        // attributes we care about
        int32_t attributes[] = {
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

        int32_t values[sizeof(attributes) / sizeof(attributes[0])];
        for (int32_t i = 1; i <= nativeCount; i++) {
            if (!s_Wgl.getPixelFormatAttribivARB(
                    s_Wgl.hdc,
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
        nativeCount = s_Gdi.describePixelFormat(s_Wgl.hdc, 1, 0, nullptr);

        for (int32_t i = 1; i <= nativeCount; i++) {
            PIXELFORMATDESCRIPTOR pfd;
            if (!s_Gdi.describePixelFormat(s_Wgl.hdc, i, sizeof(PIXELFORMATDESCRIPTOR), &pfd)) {
                continue;
            }

            // filter for opengl pixel formats
            if (!(pfd.dwFlags & PFD_SUPPORT_OPENGL) || !(pfd.dwFlags & PFD_DRAW_TO_WINDOW)) {
                continue;
            }

            if (pfd.iPixelType != PFD_TYPE_RGBA) {
                continue;
            }

            if (!(pfd.dwFlags & PFD_GENERIC_ACCELERATED) && (pfd.dwFlags & PFD_GENERIC_FORMAT)) {
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

                config->stereo = (pfd.dwFlags & PFD_STEREO) ? PAL_TRUE : PAL_FALSE;
                config->sRGB = PAL_FALSE;
                config->doubleBuffer = (pfd.dwFlags & PFD_DOUBLEBUFFER) ? PAL_TRUE : PAL_FALSE;
            }
            configCount++;
        }
    }

    if (!configs) {
        *count = configCount;
    }
    return PAL_RESULT_SUCCESS;
}

void* wglGetGLProcAddress(const char* name)
{
    void* proc = s_Wgl.getProcAddress(name);
    if (!proc) {
        proc = (void*)GetProcAddress(s_Wgl.opengl, name);
    }
    return proc;
}

void wglSetSwapInterval(int32_t interval)
{
    s_Wgl.swapIntervalEXT(interval);
}

const PalBool* wglGetSupportedGLAPIs(void* instance)
{
    s_SupportedAPIs[PAL_GL_API_OPENGL] = PAL_TRUE;
    s_SupportedAPIs[PAL_GL_API_OPENGL_ES] = PAL_FALSE;
    return s_SupportedAPIs;
}

#endif // _WIN32