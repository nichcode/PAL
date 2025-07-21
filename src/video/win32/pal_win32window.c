
#include "pal_pch.h"
#include "video/pal_video_internal.h"
#include "platform/pal_platform.h"
#include "event/pal_window_event.h"

#define WIN32_CLASS L"PALClass"
#define WIN32_PROP L"PAL"

struct PalWindow_T {
    PalVideo video;
    HWND handle;
    HINSTANCE instance;
    const char* title;
    Uint32 width;
    Uint32 height;
    Uint32 id;
    Uint32 style;
    Uint32 exStyle;
    int x;
    int y;

    bool hidden;
};

LRESULT CALLBACK windowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
bool registerWindowClass(HINSTANCE instance);
void unRegisterWindowClass(HINSTANCE instance);

PalResult _PCALL palCreateWindow(
    PalVideo video, 
    PalWindowConfig* config,
    PalWindow* outWindow) {

    PalWindow window = PAL_NULL;
    PalDisplay display = PAL_NULL;
    PalDisplayInfo displayInfo;
    HINSTANCE instance;
    bool hidden;

    PalVideoFeatureFlags features = video->featureFlags;
    instance = GetModuleHandleW(PAL_NULL);
    if (video->windowCount == 0) {
        if (!registerWindowClass(instance)) {
            return PAL_ERROR_DEVICE_NOT_FOUND;
        }
    }

    Uint32 style = WS_CAPTION | WS_SYSMENU | WS_OVERLAPPED;
    Uint32 exStyle = WS_EX_OVERLAPPEDWINDOW;

    if (config->flags & PAL_WINDOW_MINIMIZEBOX) {
        style |= WS_MINIMIZEBOX;
    }

    if (config->flags & PAL_WINDOW_RESIZABLE) {
        style |= WS_THICKFRAME | WS_MAXIMIZEBOX;
    }

    if (config->display) {
        display = config->display;
    } else {
        palGetPrimaryDisplay(video, &display);
    }

    // get display info
    PalResult result = palGetDisplayInfo(display, &displayInfo);
    if (result != PAL_SUCCESS) {
        return PAL_ERROR_INVALID_DISPLAY;
    }

    Uint32 width = 0;
    Uint32 height = 0;
    int x = 0;
    int y = 0;
    
    if (config->flags & PAL_WINDOW_CENTER) {
        x = displayInfo.x + (displayInfo.width - config->width) / 2;
        y = displayInfo.y + (displayInfo.height - config->height) / 2;

    } else {
        // we set 100 for each axix
        x = displayInfo.x + 100;
        y = displayInfo.y + 100;
    }

    float scale;
    // set window size
    if (config->flags & PAL_WINDOW_HIGH_DPI) {
        // check for support
        if (features & PAL_VIDEO_HIGH_DPI) {
            scale = (float)displayInfo.dpi / 96.0f;
            width = (Uint32)((float)config->width * scale);
            height = (Uint32)((float)config->height * scale);
        } else {
            return PAL_ERROR_FEATURE_UNSUPPORTED;
        }

    } else {
        width = config->width;
        height = config->height;
    }

    RECT rect = { 0, 0, 0, 0 };
    rect.right = width;
    rect.bottom = height;
    AdjustWindowRectEx(&rect, style, 0, exStyle);

    Uint64 len = strlen(config->title);
    wchar_t buffer[len + 1];
    palZeroMemory(buffer, len);
    palStringToWideString(buffer, config->title);

    HWND handle = CreateWindowExW(
        exStyle,
        WIN32_CLASS,
        buffer,
        style, 
        x,
        y,
        rect.right - rect.left,
        rect.bottom - rect.top,
        PAL_NULL, 
        PAL_NULL, 
        instance,
        PAL_NULL
    );

    if (!handle) {
        return PAL_ERROR_DEVICE_NOT_FOUND;
    }

    window = palAllocate(video->allocator, sizeof(struct PalWindow_T));
    if (!window) {
        return PAL_ERROR_OUT_OF_MEMORY;
    }
    palZeroMemory(window, sizeof(struct PalWindow_T));

    int showFlag = SW_HIDE;
    if (config->flags & PAL_WINDOW_MAXIMIZED) {
        showFlag = SW_SHOWMAXIMIZED;
    }

    if (config->flags & PAL_WINDOW_SHOWN) {
        if (showFlag == SW_HIDE) {
            showFlag = SW_SHOW;
        }
    }

    if (showFlag != SW_HIDE) {
        hidden = PAL_FALSE;
    } else {
        hidden = PAL_TRUE;
    }

    ShowWindow(handle, showFlag);
    SetPropW(handle, WIN32_PROP, window);

    window->video = video;
    window->handle = handle;
    window->title = config->title;
    window->style = style;
    window->exStyle = exStyle;
    window->width = config->width;
    window->height = config->height;
    window->x = x;
    window->y = y;
    window->hidden = hidden;

    window->id = ++video->nextWindowID;
    video->windowCount++;
    *outWindow = window;
    return PAL_SUCCESS;
}

void _PCALL palDestroyWindow(PalWindow window) {

    if (!window || (window && !window->video)) {
        return;
    }

    DestroyWindow(window->handle);
    window->video->windowCount--;
    palFree(window->video->allocator, window);
}

bool registerWindowClass(HINSTANCE instance) {

    WNDCLASSEXW wc = {};
    wc.cbClsExtra = 0;
    wc.cbSize = sizeof(WNDCLASSEXW);
    wc.cbWndExtra = 0;
    wc.hbrBackground = NULL;
    wc.hCursor = LoadCursorW(instance, IDC_ARROW);
    wc.hIcon = LoadIconW(instance, IDI_APPLICATION);
    wc.hIconSm = LoadIconW(instance, IDI_APPLICATION);
    wc.hInstance = instance;
    wc.lpfnWndProc = windowProc;
    wc.lpszClassName = WIN32_CLASS;
    wc.lpszMenuName = NULL;
    wc.style = CS_DBLCLKS | CS_OWNDC | CS_HREDRAW | CS_VREDRAW;

    if (!RegisterClassExW(&wc)) {
        return PAL_FALSE;
    }
    return PAL_TRUE;
}

void unRegisterWindowClass(HINSTANCE instance) {

    UnregisterClassW(WIN32_CLASS, instance);
}

LRESULT CALLBACK windowProc(
    HWND hwnd, 
    UINT msg, 
    WPARAM wParam, 
    LPARAM lParam) {

    PalWindow window = (PalWindow)GetPropW(hwnd, WIN32_PROP);
    if (!window) {
        return DefWindowProcW(hwnd, msg, wParam, lParam);
    }

    switch (msg) {
        case WM_DPICHANGED: {
            UINT dpi = HIWORD(wParam);
            if (window->video->featureFlags & PAL_VIDEO_HIGH_DPI) {
                RECT* rect = (RECT*)lParam;

                int x = rect->left;
                int y = rect->top;
                int w = rect->right - rect->left;
                int h = rect->bottom - rect->top;

                SetWindowPos(hwnd, PAL_NULL, x, y, w, h,
                    SWP_NOZORDER | SWP_NOACTIVATE
                );

                window->x = x;
                window->y = y;
                window->width = w;
                window->height = h;
            }

            
#if PAL_HAS_EVENT
            if (window->video && window->video->eventDriver) {
            PalEventDriver driver = window->video->eventDriver;
            palWindowDpiChangeEvent(driver, dpi, window->id);
            }
#endif // PAL_HAS_EVENT
            break;
        }

        case WM_CLOSE: {
#if PAL_HAS_EVENT
            if (window->video && window->video->eventDriver) {
                PalEventDriver driver = window->video->eventDriver;
                palWindowQuitEvent(driver, window->id);
            }
#endif // PAL_HAS_EVENT
            break;
        }

        case WM_MOVE: {
            int x = GET_X_LPARAM(lParam);
            int y = GET_Y_LPARAM(lParam);
            window->x = x;
            window->y = y;

#if PAL_HAS_EVENT
            if (window->video && window->video->eventDriver) {
                PalEventDriver driver = window->video->eventDriver;
                palWindowMoveEvent(driver, x, y, window->id);
            }
#endif // PAL_HAS_EVENT
            return 0;
            break;
        }

        case WM_SIZE: {
            const Uint32 width = (Uint32)LOWORD(lParam);
            const Uint32 height = (Uint32)HIWORD(lParam);

            if (width != window->width || height != window->height) {
                window->width = width;
                window->height = height;

#if PAL_HAS_EVENT
                if (window->video && window->video->eventDriver) {
                    PalEventDriver driver = window->video->eventDriver;
                    palWindowResizeEvent(driver, width, height, window->id);
                }
#endif // PAL_HAS_EVENT
            }
            return 0;
            break;
        }
    }

    return DefWindowProcW(hwnd, msg, wParam, lParam);
}