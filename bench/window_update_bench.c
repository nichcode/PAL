
#include "bench.h"
#include "pal/pal_video.h"

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

#include <windows.h>

#define PLATFORM_CLASS L"Platform"
static HINSTANCE g_Instance;

#endif // _WIN32

#define MAX_ITERATIONS 1000

static inline void createDestroyUpdate()
{

    PalResult  result;
    PalWindow* window = nullptr;

    PalWindowCreateInfo createInfo = {0};
    createInfo.height              = 480;
    createInfo.width               = 640;
    createInfo.show                = true;
    createInfo.style               = PAL_WINDOW_STYLE_RESIZABLE;
    createInfo.title               = "PAL Window";

    result = palCreateWindow(&createInfo, &window);
    if (result != PAL_RESULT_SUCCESS) {
        palLog(nullptr, "Failed to create window %s", palFormatResult(result));
        return;
    }

    Int32 counter = 10;
    while (counter >= 1) {
        palUpdateVideo();
        counter--;
    }

    palDestroyWindow(window);
}

static inline void createDestroyUpdatePlatform()
{

    HWND window = nullptr;

    Uint32 style   = WS_OVERLAPPEDWINDOW;
    Uint32 exStyle = WS_EX_APPWINDOW;

    window = CreateWindowExW(exStyle, PLATFORM_CLASS, L"Platform Window", style, 100, 100, 640, 480,
                             nullptr, nullptr, g_Instance, nullptr);

    if (!window) {
        palLog(nullptr, "Platform Error");
        return;
    }

    ShowWindow(window, SW_SHOW);
    UpdateWindow(window);
    SetWindowLongPtrW(window, GWLP_USERDATA, (LONG_PTR) nullptr);

    Int32 counter = 10;
    while (counter >= 1) {
        MSG msg;
        while (PeekMessageA(&msg, NULL, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessageA(&msg);
        }

        counter--;
    }

    DestroyWindow(window);
}

void windowUpdateBench()
{

    // initialize PAL video
    PalResult result = palInitVideo(nullptr, nullptr);
    if (result != PAL_RESULT_SUCCESS) {
        palLog(nullptr, "Failed to initialize video %s", palFormatResult(result));
        return;
    }

    // initialize platform video
#ifdef _WIN32
    HINSTANCE g_Instance = GetModuleHandleW(nullptr);

    // register class
    WNDCLASSEXW wc   = {0};
    wc.cbSize        = sizeof(WNDCLASSEXW);
    wc.hCursor       = LoadCursorW(NULL, IDC_ARROW);
    wc.hIcon         = LoadIconW(NULL, IDI_APPLICATION);
    wc.hIconSm       = LoadIconW(NULL, IDI_APPLICATION);
    wc.hInstance     = g_Instance;
    wc.lpfnWndProc   = DefWindowProcW;
    wc.lpszClassName = PLATFORM_CLASS;
    wc.style         = CS_OWNDC;

    if (!RegisterClassExW(&wc)) {
        palLog(nullptr, "Platform Error");
        return;
    }
#endif // _WIN32

    // run benchmarks
    // platform
    runBench(createDestroyUpdatePlatform, "Platform Window Benchmark", MAX_ITERATIONS);

    // PAL
    runBench(createDestroyUpdate, "PAL Window Benchmark", MAX_ITERATIONS);

    // shutdown platform video
#ifdef _WIN32
    UnregisterClassW(PLATFORM_CLASS, g_Instance);
#endif // _WIN32

    // shutdown PAL video
    palShutdownVideo();
}