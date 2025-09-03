
// ==================================================
// Includes
// ==================================================

#include "pal/pal_video.h"

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

// ==================================================
// Typedefs, enums and structs
// ==================================================

#define PAL_VIDEO_CLASS L"PALVideoClass"
#define PAL_VIDEO_PROP L"PALVideo"

typedef HRESULT (WINAPI* GetDpiForMonitorFn)(
    HMONITOR, 
    Int32, 
    UINT*, 
    UINT*);

typedef HRESULT (WINAPI* SetProcessAwarenessFn)(
    Int32);

typedef struct {
    bool initialized;
    PalVideoFeatures features;
    const PalAllocator* allocator;
    GetDpiForMonitorFn getDpiForMonitor;
    SetProcessAwarenessFn setProcessAwareness;
    HINSTANCE shcore;
    HINSTANCE instance;
} VideoWin32;

static VideoWin32 s_Video = {};

// ==================================================
// Internal API
// ==================================================

LRESULT CALLBACK videoProc(
    HWND hwnd, 
    UINT msg, 
    WPARAM wParam, 
    LPARAM lParam) {

    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

// ==================================================
// Public API
// ==================================================

PalResult PAL_CALL palInitVideo(
    const PalAllocator *allocator) {
    
    if (s_Video.initialized) {
        return PAL_RESULT_SUCCESS;
    }
    
    if (allocator && (!allocator->allocate || !allocator->free)) {
        return PAL_RESULT_INVALID_ALLOCATOR;
    }

    // get the instance
    s_Video.instance = GetModuleHandleW(nullptr);

    // register class
    WNDCLASSEXW wc = {};
    wc.cbSize = sizeof(WNDCLASSEXW);
    wc.hCursor = LoadCursorW(s_Video.instance, IDC_ARROW);
    wc.hIcon = LoadIconW(s_Video.instance, IDI_APPLICATION);
    wc.hIconSm = LoadIconW(s_Video.instance, IDI_APPLICATION);
    wc.hInstance = s_Video.instance;
    wc.lpfnWndProc = videoProc;
    wc.lpszClassName = PAL_VIDEO_CLASS;
    wc.style = CS_DBLCLKS | CS_OWNDC | CS_HREDRAW | CS_VREDRAW;

    // since we check every input carefully, the only error we can get is access denied
    if (!RegisterClassExW(&wc)) {
        return PAL_RESULT_ACCESS_DENIED;
    }

    // load shared libraries
    s_Video.shcore = LoadLibraryA("shcore.dll");
    if (s_Video.shcore) {
        s_Video.getDpiForMonitor = (GetDpiForMonitorFn)GetProcAddress(
            s_Video.shcore,
            "GetDpiForMonitor"
        );

        s_Video.setProcessAwareness = (SetProcessAwarenessFn)GetProcAddress(
            s_Video.shcore,
            "SetProcessDpiAwareness"
        );
    }

    // set features
    s_Video.features |= PAL_VIDEO_FEATURE_DISPLAY_ORIENTATION;
    s_Video.features |= PAL_VIDEO_FEATURE_BORDERLESS_WINDOW;
    s_Video.features |= PAL_VIDEO_FEATURE_DISPLAY_MODE_SWITCH;
    s_Video.features |= PAL_VIDEO_FEATURE_MULTI_DISPLAYS;
    s_Video.features |= PAL_VIDEO_FEATURE_WINDOW_RESIZING;
    s_Video.features |= PAL_VIDEO_FEATURE_WINDOW_POSITIONING;
    s_Video.features |= PAL_VIDEO_FEATURE_WINDOW_MINMAX;
    s_Video.features |= PAL_VIDEO_FEATURE_DISPLAY_GAMMA_CONTROL;
    s_Video.features |= PAL_VIDEO_FEATURE_CLIP_CURSOR;
    s_Video.features |= PAL_VIDEO_FEATURE_WINDOW_FLASH_CAPTION;
    s_Video.features |= PAL_VIDEO_FEATURE_WINDOW_FLASH_TRAY;

    if (s_Video.getDpiForMonitor && s_Video.setProcessAwareness) {
        s_Video.features |= PAL_VIDEO_FEATURE_HIGH_DPI;
    }

    s_Video.initialized = true;
    s_Video.allocator = allocator;
    return PAL_RESULT_SUCCESS;
}

void PAL_CALL palShutdownVideo() {

    if (!s_Video.initialized) {
        return;
    }

    UnregisterClassW(PAL_VIDEO_CLASS, s_Video.instance);
    s_Video.initialized = false;
}

PalVideoFeatures PAL_CALL palGetVideoFeatures() {

    if (!s_Video.initialized) {
        return 0;
    }

    return s_Video.features;
}