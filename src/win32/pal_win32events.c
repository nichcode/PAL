
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
            PalEventDispatchType dispatchType;
            dispatchType = palGeteventDispatchType(PAL_EVENT_QUIT);
            if (dispatchType == PAL_EVENT_DISPATCH_NONE) { 
                return 0; 
                break; 
            }

            PalEvent event;
            event.type = PAL_EVENT_QUIT;
            event.window = window;
            palPushEvent(event);

            return 0;
            break;
        }

    }

    return DefWindowProcW(hwnd, msg, wParam, lParam);
}