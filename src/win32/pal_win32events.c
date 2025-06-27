
#include "pal_pch.h"
#include "pal_win32platform.h"
#include "pal/pal_events.h"

void _palPlatformPollEvents()
{
    MSG msg;
    while (PeekMessageA(&msg, PAL_NULL, 0, 0, PM_REMOVE)) {
        TranslateMessage(&msg);
        DispatchMessageA(&msg);
    }
}

LRESULT CALLBACK palProcWin32(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    PalWindow* window = (PalWindow*)GetPropW(hwnd, WIN32_PROP);
    if (!window) {
        return DefWindowProcW(hwnd, msg, wParam, lParam);
    }

    switch (msg) {
        case WM_CLOSE: {
            // get dispatch type and do nothing if is PAL_EVENT_DISPATCH_NONE
            PalDispatch dispatch;
            dispatch = palGetDispatch(PAL_EVENT_QUIT);
            if (dispatch == PAL_DISPATCH_NONE) { 
                return 0; 
                break; 
            }

            PalEvent event;
            event.type = PAL_EVENT_QUIT;
            event.windowID = window->id;

            if (dispatch == PAL_DISPATCH_CALLBACK) {
                palTriggerEvent(&event);
            } else {
                palPushEvent(event);
            }

            return 0;
            break;
        }

        case WM_SIZE: {
            const Uint32 width = (Uint32)LOWORD(lParam);
            const Uint32 height = (Uint32)HIWORD(lParam);

            if (width != window->width || height != window->height) {
                window->width = width;
                window->height = height;

                if (wParam == SIZE_MAXIMIZED) {
                    window->maximized = PAL_TRUE;
                    window->minimized = PAL_FALSE;
                    window->fullscreen = PAL_FALSE;

                } else if (wParam == SIZE_MINIMIZED) {
                    window->minimized = PAL_TRUE;
                    window->maximized = PAL_FALSE;
                    window->fullscreen = PAL_FALSE;
                }

                // not the same size
                PalDispatch dispatch;
                dispatch = palGetDispatch(PAL_EVENT_WINDOW_RESIZE);
                if (dispatch == PAL_DISPATCH_NONE) { 
                    return 0; 
                    break; 
                }

                PalEvent event;
                event.type = PAL_EVENT_WINDOW_RESIZE;
                event.windowID = window->id;
                event.size.width = width;
                event.size.height = height;

                if (dispatch == PAL_DISPATCH_CALLBACK) {
                    palTriggerEvent(&event);
                } else {
                    palPushEvent(event);
                }
            }

            return 0;
            break;
        }

    }

    return DefWindowProcW(hwnd, msg, wParam, lParam);
}