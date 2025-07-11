
#include "pal_pch.h"
#include "pal_video_internal.h"
#include "event/pal_event_internal.h"

#define WIN32_CLASS L"PALClass"
#define WIN32_PROP L"PAL"

typedef struct PalWindow
{
    HWND handle;
    const char* title;
    PalVideoInstance* instance;

    Uint32 style;
    Uint32 exStyle;
    Uint32 width;
    Uint32 height;
    Uint32 id;
    int x;
    int y;

    bool hidden;

} PalWindow;

LRESULT CALLBACK windowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

void toWideString(wchar_t* buffer, const char* string)
{
    int len = MultiByteToWideChar(CP_UTF8, 0, string, -1, PAL_NULL, 0);
    MultiByteToWideChar(CP_UTF8, 0, string, -1, buffer, len);
}

bool registerWindowClass(HINSTANCE instance)
{
    static bool init = PAL_FALSE;
    if (!init) {
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

        init = PAL_TRUE;
    }
    return PAL_TRUE;
}

void unRegisterWindowClass()
{
    static bool destroyed = PAL_FALSE;
    if (!destroyed) {
        UnregisterClassW(WIN32_CLASS, GetModuleHandleW(PAL_NULL));
        destroyed = PAL_TRUE;
    }
}

LRESULT CALLBACK windowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    PalWindow* window = (PalWindow*)GetPropW(hwnd, WIN32_PROP);
    if (!window) {
        return DefWindowProcW(hwnd, msg, wParam, lParam);
    }

    PalVideoInstance* video = window->instance;
    switch (msg) {
        case WM_CLOSE: {
            if (video->eventInstance) {
                PalEventInstance* eventInstance = video->eventInstance;
                PalMode mode = eventInstance->modes[PAL_EVENT_QUIT];
                if (mode != PAL_MODE_NONE) {
                    PalEvent event;
                    event.id = window->id;
                    event.type = PAL_EVENT_QUIT;
                    eventInstance->queue->push(eventInstance->eventData, &event);
                }
            }
            return 0;
            break;
        }
    }



    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

PalResult _PCALL palCreateWindow(
    PalVideoInstance* videoInstance, 
    PalWindowDesc* desc, 
    PalWindow** outWindow)
{
    if (!videoInstance || !outWindow || !desc) {
        palSetError(PAL_ERROR_NULL_POINTER);
        return PAL_RESULT_FAIL;
    }

    PalWindow* window = PAL_NULL;
    Uint32 style = WS_CAPTION | WS_SYSMENU | WS_OVERLAPPED;
    Uint32 exStyle = WS_EX_OVERLAPPEDWINDOW;
    HMONITOR monitor = PAL_NULL;
    
    HINSTANCE instance = GetModuleHandleW(PAL_NULL);
    if (!registerWindowClass(instance)) {
        palSetError(PAL_ERROR_VIDEO_DEVICE_NOT_FOUND);
        return PAL_RESULT_FAIL;
    }

    if (desc->flags & PAL_WINDOW_MINIMIZEBOX) {
        style |= WS_MINIMIZEBOX;
    }

    if (desc->flags & PAL_WINDOW_RESIZABLE) {
        style |= WS_THICKFRAME | WS_MAXIMIZEBOX;
    }

    RECT rect = { 0, 0, 0, 0 };
    rect.right = desc->width;
    rect.bottom = desc->height;
    AdjustWindowRectEx(&rect, style, 0, exStyle);

    if (!desc->display) {
        // get primary display
        monitor = MonitorFromPoint((POINT){0, 0}, MONITOR_DEFAULTTOPRIMARY);
    } else {
        monitor = (HMONITOR)desc->display;
    }

    // get display info
    MONITORINFOEXW mi = {};
    mi.cbSize = sizeof(MONITORINFOEXW);
    if (!GetMonitorInfoW(monitor, (MONITORINFO*)&mi)) { 
        palSetError(PAL_ERROR_INVALID_DISPLAY);
        return PAL_RESULT_FAIL; 
    }

    // place on display
    int x = 0;
    int y = 0;
    if (desc->flags & PAL_WINDOW_CENTER) {
        int displayX = mi.rcMonitor.left;
        int displayY = mi.rcMonitor.top;
        int displayW = mi.rcMonitor.right - mi.rcMonitor.left;
        int displayH = mi.rcMonitor.bottom - mi.rcWork.top;

        x = displayX + (displayW - desc->width) / 2;
        y = displayY + (displayH - desc->height) / 2;
    } else {
        // we set 100 for each axix
        x = mi.rcMonitor.left + 100;
        y = mi.rcMonitor.top + 100;
    }

    wchar_t buffer[64] = {};
    toWideString(buffer, desc->title);
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
        palSetError(PAL_ERROR_VIDEO_DEVICE_NOT_FOUND);
        return PAL_RESULT_FAIL;
    }

    window = palAlloc(videoInstance->allocator, sizeof(PalWindow));
    if (!window) {
        palSetError(PAL_ERROR_ALLOCATION_FAILED);
        return PAL_RESULT_FAIL;
    }

    memset(window, 0, sizeof(PalWindow));
    int showFlag = SW_HIDE;
    if (desc->flags & PAL_WINDOW_MAXIMIZED) {
        showFlag = SW_SHOWMAXIMIZED;
    }

    if (desc->flags & PAL_WINDOW_SHOWN) {
        if (showFlag == SW_HIDE) {
            showFlag = SW_SHOW;
        }
    }

    if (showFlag != SW_HIDE) {
        window->hidden = PAL_FALSE;
    } else {
        window->hidden = PAL_TRUE;
    }

    ShowWindow(handle, showFlag);
    SetPropW(handle, WIN32_PROP, window);

    window->instance = videoInstance;
    window->handle = handle;
    window->title = desc->title;
    window->style = style;
    window->exStyle = exStyle;
    window->width = desc->width;
    window->height = desc->height;
    window->x = x;
    window->y = y;
    window->id = videoInstance->nextWindowID;

    videoInstance->nextWindowID++;
    *outWindow = window;
    return PAL_RESULT_OK;
}

void _PCALL palDestroyWindow(PalWindow* window)
{
    if (!window || (window && !window->instance)) {
        palSetError(PAL_ERROR_NULL_POINTER);
        return;
    }

    PalVideoInstance* videoInstance = window->instance;
    DestroyWindow(window->handle);
    palFree(videoInstance->allocator, window);
    unRegisterWindowClass();
}

PalResult _PCALL palGetWindowID(PalWindow* window, Uint32* id)
{
    if (!window || !id || (window && !window->instance)) {
        palSetError(PAL_ERROR_NULL_POINTER);
        return PAL_RESULT_FAIL;
    }

    *id = window->id;
    return PAL_RESULT_OK;
}