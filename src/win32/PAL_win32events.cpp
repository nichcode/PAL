
#include "PAL_pch.h"
#include "PAL_win32platform.h"
#include "PAL_eventqueue.h"

void PAL_PlatformPollEvents()
{
    MSG msg;
    while (PeekMessageA(&msg, PAL_NULL, 0, 0, PM_REMOVE)) {
        TranslateMessage(&msg);
        DispatchMessageA(&msg);
    }
}

void PAL_PlatformWaitEvents()
{
    MSG msg;
    while (GetMessageA(&msg, PAL_NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessageA(&msg);
    }
}

LRESULT CALLBACK PAL_Win32Proc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    PAL_Window* window = (PAL_Window*)GetPropW(hwnd, s_PALWin32.propName);
    if (!window) {
        return DefWindowProcW(hwnd, msg, wParam, lParam);
    }

    switch (msg) {
        case WM_CLOSE: {
            if (s_PAL.eventPolling) {
                PAL_Event event = {};
                event.type = PAL_QUIT_EVENT;
                event.window = window;
                s_EventQueue.push(event);

            } else {
                if (s_Callbacks.close) {
                    s_Callbacks.close(window);
                }
            }

            return 0;
            break;
        } // WM_CLOSE

    } //switch

    return DefWindowProcW(hwnd, msg, wParam, lParam);
}