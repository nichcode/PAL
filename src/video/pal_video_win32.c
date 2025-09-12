
/**

Copyright (C) 2025 Nicholas Agbo

This software is provided 'as-is', without any express or implied
warranty.  In no event will the authors be held liable for any damages
arising from the use of this software.

Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it
freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not
   claim that you wrote the original software. If you use this software
   in a product, an acknowledgment in the product documentation would be
   appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be
   misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.

 */

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
#include <windowsx.h>

// ==================================================
// Typedefs, enums and structs
// ==================================================

#define PAL_VIDEO_CLASS L"PALVideoClass"
#define WIN32_DPI 0
#define WIN32_DPI_AWARE 2
#define MAX_MODE_COUNT 128
#define NULL_ORIENTATION 5
#define WINDOW_NAME_SIZE 256

typedef HRESULT (WINAPI* GetDpiForMonitorFn)(
    HMONITOR, 
    Int32, 
    UINT*, 
    UINT*);

typedef HRESULT (WINAPI* SetProcessAwarenessFn)(
    Int32);

typedef HBITMAP (WINAPI* CreateDIBSectionFn)(
    HDC,
    const BITMAPINFO*,
    UINT,
    VOID**,
    HANDLE,
    DWORD);

typedef HBITMAP (WINAPI* CreateBitmapFn) (
    int,
    int,
    UINT,
    UINT,
    CONST VOID*);

typedef BOOL (WINAPI* DeleteObjectFn)(
    HGDIOBJ);

typedef struct {
    bool initialized;
    PalVideoFeatures features;
    const PalAllocator* allocator;
    PalEventDriver* eventDriver;
    HINSTANCE shcore;
    GetDpiForMonitorFn getDpiForMonitor;
    SetProcessAwarenessFn setProcessAwareness;

    HINSTANCE gdi;
    CreateDIBSectionFn createDIBSection;
    CreateBitmapFn createBitmap;
    DeleteObjectFn deleteObject; 

    HINSTANCE instance;
    HWND hiddenWindow;
} VideoWin32;

typedef struct {
    Int32 count;
    Int32 maxCount;
    PalMonitor** monitors;
} MonitorData;

typedef struct {
    bool pendingResize;
    bool pendingMove;
    Uint32 width;
    Uint32 height;
    Int32 x;
    Int32 y;
    PalWindowState state;
    PalWindow* window;
} PendingEvent;

typedef struct {
    bool scancodeState[PAL_SCANCODE_MAX];
    bool keycodeState[PAL_KEYCODE_MAX];
    PalScancode scancodes[512];
    PalKeycode keycodes[256];
} Keyboard;

typedef struct {
    bool push;
    Int32 dx;
    Int32 dy;
    Int32 WheelX;
    Int32 WheelY;
    bool state[PAL_MOUSE_BUTTON_MAX];
} Mouse;

static PendingEvent s_Event;
static VideoWin32 s_Video = {};

static BYTE s_RawBuffer[4096] = {};
static Mouse s_Mouse = {};
static Keyboard s_Keyboard = {};

// ==================================================
// Internal API
// ==================================================

LRESULT CALLBACK videoProc(
    HWND hwnd, 
    UINT msg, 
    WPARAM wParam, 
    LPARAM lParam) {

    // check if the window has been created
    void* data = (void*)(LONG_PTR)GetWindowLongPtrW(hwnd, GWLP_USERDATA);
    if (!data) {
        // window has not been created yet
        return DefWindowProcW(hwnd, msg, wParam, lParam);
    }
    
    switch (msg) {
        case WM_CLOSE: {
            if (s_Video.eventDriver) {
                PalEventDriver* driver = s_Video.eventDriver;
                PalDispatchMode mode = palGetEventDispatchMode(driver, PAL_EVENT_WINDOW_CLOSE);
                if (mode != PAL_DISPATCH_NONE) {
                    PalEvent event = {0};
                    event.type = PAL_EVENT_WINDOW_CLOSE;
                    event.data2 = palPackPointer((PalWindow*)hwnd);
                    palPushEvent(driver, &event);                
                } 
            }
            return 0;
        }

        case WM_SIZE: {
            if (s_Video.eventDriver) {
                PalEventDriver* driver = s_Video.eventDriver;
                PalDispatchMode mode = palGetEventDispatchMode(driver, PAL_EVENT_WINDOW_SIZE);
                Uint32 width = (Uint32)LOWORD(lParam);
                Uint32 height = (Uint32)HIWORD(lParam);

                if (mode == PAL_DISPATCH_CALLBACK) {
                    PalEvent event = {0};
                    event.type = PAL_EVENT_WINDOW_SIZE;
                    event.data = palPackUint32(width, height);
                    event.data2 = palPackPointer((PalWindow*)hwnd);
                    palPushEvent(driver, &event);

                } else {
                    s_Event.pendingResize = true;
                    s_Event.width = width;
                    s_Event.height = height;
                    s_Event.window = (PalWindow*)hwnd;
                }

                // trigger state event
                mode = palGetEventDispatchMode(driver, PAL_EVENT_WINDOW_STATE);
                PalWindowState state;
                if (mode == PAL_DISPATCH_NONE) {
                    return 0;
                }

                switch (wParam) {
                    case SIZE_MINIMIZED: {
                        state = PAL_WINDOW_STATE_MINIMIZED;
                        break;
                    }

                    case SIZE_MAXIMIZED: {
                        state = PAL_WINDOW_STATE_MAXIMIZED;
                        break;
                    }

                    case SIZE_RESTORED: {
                        state = PAL_WINDOW_STATE_RESTORED;
                        break;
                    }
                }

                if (mode == PAL_DISPATCH_CALLBACK) {
                    PalEvent event = {0};
                    event.data = state;
                    event.data2 = palPackPointer((PalWindow*)hwnd);
                    event.type = PAL_EVENT_WINDOW_STATE;
                    palPushEvent(driver, &event);

                } else {
                    s_Event.state = state;
                }
            }
            
            return 0;
        }

        case WM_MOVE: {
            if (s_Video.eventDriver) {
                PalEventDriver* driver = s_Video.eventDriver;
                PalDispatchMode mode = palGetEventDispatchMode(driver, PAL_EVENT_WINDOW_MOVE);
                Int32 x = GET_X_LPARAM(lParam);
                Int32 y = GET_Y_LPARAM(lParam);

                if (mode == PAL_DISPATCH_CALLBACK) {
                    PalEvent event = {0};
                    event.type = PAL_EVENT_WINDOW_MOVE;
                    event.data = palPackInt32(x, y);
                    event.data2 = palPackPointer((PalWindow*)hwnd);
                    palPushEvent(driver, &event);

                } else {
                    s_Event.pendingMove = true;
                    s_Event.x = x;
                    s_Event.y = y;
                    s_Event.window = (PalWindow*)hwnd;
                }
            }

            return 0;
        }

        case WM_SHOWWINDOW: {
            if (s_Video.eventDriver) {
                PalEventDriver* driver = s_Video.eventDriver;
                PalDispatchMode mode = palGetEventDispatchMode(driver, PAL_EVENT_WINDOW_VISIBILITY);
                if (mode != PAL_DISPATCH_NONE) {
                    PalEvent event = {0};
                    event.type = PAL_EVENT_WINDOW_VISIBILITY;
                    event.data = (bool)wParam;
                    event.data2 = palPackPointer((PalWindow*)hwnd);
                    palPushEvent(driver, &event);                
                }
            }
            return 0;
        }

        case WM_SETFOCUS: {
            if (s_Video.eventDriver) {
                PalEventDriver* driver = s_Video.eventDriver;
                PalDispatchMode mode = palGetEventDispatchMode(driver, PAL_EVENT_WINDOW_FOCUS);
                if (mode != PAL_DISPATCH_NONE) {
                    PalEvent event = {0};
                    event.type = PAL_EVENT_WINDOW_FOCUS;
                    event.data = true;
                    event.data2 = palPackPointer((PalWindow*)hwnd);
                    palPushEvent(driver, &event);                
                }
            }
            return 0;
        }

        case WM_KILLFOCUS: {
            if (s_Video.eventDriver) {
                PalEventDriver* driver = s_Video.eventDriver;
                PalDispatchMode mode = palGetEventDispatchMode(driver, PAL_EVENT_WINDOW_FOCUS);
                if (mode != PAL_DISPATCH_NONE) {
                    PalEvent event = {0};
                    event.type = PAL_EVENT_WINDOW_FOCUS;
                    event.data = false;
                    event.data2 = palPackPointer((PalWindow*)hwnd);
                    palPushEvent(driver, &event);                
                }
            }    
            return 0;
        }

        case WM_ENTERSIZEMOVE: {
            s_Mouse.push = false;
            if (s_Video.eventDriver) {
                PalEventDriver* driver = s_Video.eventDriver;
                PalDispatchMode mode = palGetEventDispatchMode(driver, PAL_EVENT_WINDOW_MODAL_BEGIN);
                if (mode != PAL_DISPATCH_NONE) {
                    PalEvent event = {0};
                    event.type = PAL_EVENT_WINDOW_MODAL_BEGIN;
                    event.data2 = palPackPointer((PalWindow*)hwnd);
                    palPushEvent(driver, &event);                
                }
            }
            return 0;
        }

        case WM_EXITSIZEMOVE: {
            s_Mouse.push = true;
            if (s_Video.eventDriver) {
                PalEventDriver* driver = s_Video.eventDriver;
                PalDispatchMode mode = palGetEventDispatchMode(driver, PAL_EVENT_WINDOW_MODAL_END);
                if (mode != PAL_DISPATCH_NONE) {
                    PalEvent event = {0};
                    event.type = PAL_EVENT_WINDOW_MODAL_END;
                    event.data2 = palPackPointer((PalWindow*)hwnd);
                    palPushEvent(driver, &event);                
                }
            }
            return 0;
        }

        case WM_DPICHANGED: {
            if (s_Video.eventDriver) {
                PalEventDriver* driver = s_Video.eventDriver;
                PalDispatchMode mode = palGetEventDispatchMode(driver, PAL_EVENT_MONITOR_DPI_CHANGED);
                if (mode != PAL_DISPATCH_NONE) {
                    PalEvent event = {0};
                    event.type = PAL_EVENT_MONITOR_DPI_CHANGED;
                    event.data = HIWORD(wParam);
                    event.data2 = palPackPointer((PalWindow*)hwnd);
                    palPushEvent(driver, &event);                
                }
            }
            return 0;
        }

        case WM_DEVICECHANGE: {
            // check if the monitors list has been changed
            if (wParam != 0x0007) {
                return 0;
            }

            if (s_Video.eventDriver) {
                PalEventDriver* driver = s_Video.eventDriver;
                PalDispatchMode mode = palGetEventDispatchMode(driver, PAL_EVENT_MONITOR_LIST_CHANGED);
                if (mode != PAL_DISPATCH_NONE) {
                    PalEvent event = {0};
                    event.type = PAL_EVENT_MONITOR_LIST_CHANGED;
                    event.data2 = palPackPointer((PalWindow*)hwnd);
                    palPushEvent(driver, &event);                
                }
            }
            return 0;
        }

        case WM_MOUSEHWHEEL: {
            Int32 delta = GET_WHEEL_DELTA_WPARAM(wParam);
            s_Mouse.WheelX = delta;

            if (s_Video.eventDriver) {
                PalEventDriver* driver = s_Video.eventDriver;
                PalDispatchMode mode = palGetEventDispatchMode(driver, PAL_EVENT_MOUSE_WHEEL);
                if (mode != PAL_DISPATCH_NONE) {
                    PalEvent event = {0};
                    event.type = PAL_EVENT_MOUSE_WHEEL;
                    event.data = palPackInt32(delta, 0);
                    event.data2 = palPackPointer((PalWindow*)hwnd);
                    palPushEvent(driver, &event);                
                }
            }
            return 0;
        }

        case WM_MOUSEWHEEL: {
            Int32 delta = GET_WHEEL_DELTA_WPARAM(wParam);
            s_Mouse.WheelY = delta;

            if (s_Video.eventDriver) {
                PalEventDriver* driver = s_Video.eventDriver;
                PalDispatchMode mode = palGetEventDispatchMode(driver, PAL_EVENT_MOUSE_WHEEL);
                if (mode != PAL_DISPATCH_NONE) {
                    PalEvent event = {0};
                    event.type = PAL_EVENT_MOUSE_WHEEL;
                    event.data = palPackInt32(0, delta);
                    event.data2 = palPackPointer((PalWindow*)hwnd);
                    palPushEvent(driver, &event);                
                }
            }
            return 0;
        }

        case WM_MOUSEMOVE: {
            const Int32 x = GET_X_LPARAM(lParam);
            const Int32 y = GET_Y_LPARAM(lParam);
           
            if (s_Video.eventDriver) {
                PalEventDriver* driver = s_Video.eventDriver;
                PalDispatchMode mode = palGetEventDispatchMode(driver, PAL_EVENT_MOUSE_MOVE);
                if (mode != PAL_DISPATCH_NONE) {
                    PalEvent event = {0};
                    event.type = PAL_EVENT_MOUSE_MOVE;
                    event.data = palPackInt32(x, y);
                    event.data2 = palPackPointer((PalWindow*)hwnd);
                    palPushEvent(driver, &event);                
                }
            }
            return 0;
        }

        case WM_INPUT: {
            UINT size = sizeof(s_RawBuffer);
            UINT result = GetRawInputData(
                (HRAWINPUT)lParam, 
                RID_INPUT, 
                s_RawBuffer, 
                &size, 
                sizeof(RAWINPUTHEADER)
            );

            if (result == (UINT) -1) {
                break;
            }

            RAWINPUT* raw = (RAWINPUT*)s_RawBuffer;
            RAWMOUSE* mouse = &raw->data.mouse;
            // push only if we are not rresizing or moving with the mouse
            if (s_Mouse.push && (mouse->lLastX || mouse->lLastY)) {
                s_Mouse.dx += mouse->lLastX;
                s_Mouse.dy += mouse->lLastY;

                if (s_Video.eventDriver) {
                    PalEventDriver* driver = s_Video.eventDriver;
                    PalDispatchMode mode = palGetEventDispatchMode(driver, PAL_EVENT_MOUSE_DELTA);
                    if (mode != PAL_DISPATCH_NONE) {
                        PalEvent event = {0};
                        event.type = PAL_EVENT_MOUSE_DELTA;
                        event.data = palPackInt32(s_Mouse.dx, s_Mouse.dy);
                        palPushEvent(driver, &event);                
                    }
                }
            }
            break;
        }

        case WM_LBUTTONDOWN:
        case WM_RBUTTONDOWN:
        case WM_MBUTTONDOWN:
        case WM_XBUTTONDOWN:
        case WM_LBUTTONUP:
        case WM_RBUTTONUP:
        case WM_MBUTTONUP:
        case WM_XBUTTONUP: {
            PalMouseButton button;
            bool pressed = false;

            if (msg == WM_LBUTTONDOWN || msg == WM_LBUTTONUP) {
                button = PAL_MOUSE_BUTTON_LEFT;

            } else if (msg == WM_RBUTTONDOWN || msg == WM_RBUTTONUP) {
                button = PAL_MOUSE_BUTTON_RIGHT;

            } else if (msg == WM_MBUTTONDOWN || msg == WM_MBUTTONUP) {
                button = PAL_MOUSE_BUTTON_MIDDLE;

            } else if (msg == WM_XBUTTONDOWN || msg == WM_XBUTTONDOWN) {
                // check which x buttton
                WORD xButton = HIWORD(wParam);
                if (xButton == XBUTTON1) {
                    button = PAL_MOUSE_BUTTON_X1;

                } else if (xButton == XBUTTON2) {
                    button = PAL_MOUSE_BUTTON_X2;
                }
            }

            // check if we pressed or released the button
            if (msg == WM_LBUTTONDOWN || 
                msg == WM_RBUTTONDOWN ||
                msg == WM_MBUTTONDOWN || 
                msg == WM_XBUTTONDOWN) {
                pressed = true;

            } else {
                pressed = false;
            }

            // set mouse capture
            if (msg == WM_LBUTTONDOWN) {
                SetCapture(hwnd);

            } else if (msg == WM_LBUTTONUP) {
                ReleaseCapture();
            }


            s_Mouse.state[button] = pressed;
            if (s_Video.eventDriver) {
                PalEventDriver* driver = s_Video.eventDriver;
                if (pressed) {
                    // mouse button pressed
                    PalDispatchMode mode = palGetEventDispatchMode(driver, PAL_EVENT_MOUSE_BUTTONDOWN);
                    if (mode != PAL_DISPATCH_NONE) {
                        PalEvent event = {0};
                        event.type = PAL_EVENT_MOUSE_BUTTONDOWN;
                        event.data = button;
                        event.data2 = palPackPointer((PalWindow*)hwnd);
                        palPushEvent(driver, &event);
                    }

                } else {
                    // mouse button released
                    PalDispatchMode mode = palGetEventDispatchMode(driver, PAL_EVENT_MOUSE_BUTTONUP);
                    if (mode != PAL_DISPATCH_NONE) {
                        PalEvent event = {0};
                        event.type = PAL_EVENT_MOUSE_BUTTONUP;
                        event.data = button;
                        event.data2 = palPackPointer((PalWindow*)hwnd);
                        palPushEvent(driver, &event);
                    }
                }
            }
            return 0;
        }

        case WM_KEYDOWN:
        case WM_SYSKEYDOWN:
        case WM_KEYUP:
        case WM_SYSKEYUP: {
            PalKeycode keycode = PAL_KEYCODE_UNKNOWN;
            PalScancode scancode = PAL_SCANCODE_UNKNOWN;
            Int32 win32Keycode;
            Int32 win32Scancode;
            bool pressed = false;
            bool extended = false;

            pressed = (HIWORD(lParam) & KF_UP) ? false : true;
            extended = (lParam >> 24) & 1; // we use this for special keys
            win32Scancode = (HIWORD(lParam) & (KF_EXTENDED | 0xff)); // we combine the two since our table is the same
            win32Keycode = (UINT)wParam;

            // spcecial scancode handling
            if (!extended && win32Scancode == 0x045) {
                scancode = PAL_SCANCODE_NUMLOCK;

            } else {
                Uint16 index = win32Scancode | (extended << 8);
                scancode = s_Keyboard.scancodes[index];
            }

            // special keycode handling
            if (extended && win32Keycode == VK_RETURN) {
                keycode = PAL_KEYCODE_KP_ENTER;

            } else if (extended && win32Keycode == VK_OEM_PLUS) {
                keycode = PAL_KEYCODE_KP_EQUAL;

            } else if (extended && win32Keycode == VK_CONTROL) {
                keycode = PAL_KEYCODE_RCTRL;

            } else if (extended && win32Keycode == VK_MENU) {
                keycode = PAL_KEYCODE_RALT;

            } else if (win32Keycode == VK_SHIFT) {
                if (scancode == PAL_SCANCODE_LSHIFT) {
                    keycode = PAL_KEYCODE_LSHIFT;

                } else if (scancode == PAL_SCANCODE_RSHIFT) {
                    keycode = PAL_KEYCODE_RSHIFT;
                }
                
            } else if (!extended && win32Keycode == VK_INSERT) {
                // numpad 0
                keycode = PAL_KEYCODE_KP_0;

            } else if (!extended && win32Keycode == VK_END) {
                // numpad 1
                keycode = PAL_KEYCODE_KP_1;

            } else if (!extended && win32Keycode == VK_DOWN) {
                // numpad 2
                keycode = PAL_KEYCODE_KP_2;
            
            } else if (!extended && win32Keycode == VK_NEXT) {
                // numpad 3
                keycode = PAL_KEYCODE_KP_3;

            } else if (!extended && win32Keycode == VK_LEFT) {
                // numpad 4
                keycode = PAL_KEYCODE_KP_4;

            } else if (!extended && win32Keycode == VK_CLEAR) {
                // numpad 5
                keycode = PAL_KEYCODE_KP_5;
            
            } else if (!extended && win32Keycode == VK_RIGHT) {
                // numpad 6
                keycode = PAL_KEYCODE_KP_6;

            } else if (!extended && win32Keycode == VK_HOME) {
                // numpad 7
                keycode = PAL_KEYCODE_KP_7;

            } else if (!extended && win32Keycode == VK_UP) {
                // numpad 8
                keycode = PAL_KEYCODE_KP_8;

            } else if (!extended && win32Keycode == VK_PRIOR) {
                // numpad 9
                keycode = PAL_KEYCODE_KP_9;
            
            } else if (!extended && win32Keycode == VK_DELETE) {
                // numpad decimal
                keycode = PAL_KEYCODE_KP_DECIMAL;

            } else if (win32Keycode == VK_SNAPSHOT) {
                // printscreen since the platform does not get us a keydown, we do that ourselves
                if (s_Video.eventDriver) {
                    PalEventDriver* driver = s_Video.eventDriver;
                    PalDispatchMode mode = palGetEventDispatchMode(driver, PAL_EVENT_KEYDOWN);
                    if (mode != PAL_DISPATCH_NONE) {
                        PalEvent event = {0};
                        event.type = PAL_EVENT_KEYDOWN;
                        event.data = palPackUint32(PAL_KEYCODE_PRINTSCREEN, scancode);
                        event.data2 = palPackPointer((PalWindow*)hwnd);
                        palPushEvent(driver, &event);
                    }

                    keycode = PAL_KEYCODE_PRINTSCREEN;
                    s_Keyboard.keycodeState[keycode] = true;
                }

            } else {
                keycode = s_Keyboard.keycodes[win32Keycode];
            }

            bool repeat = s_Keyboard.keycodeState[keycode]; // check before updating state
            if (pressed) {
                s_Keyboard.keycodeState[keycode] = true;
                s_Keyboard.scancodeState[scancode] = true;
                
            } else {
                s_Keyboard.keycodeState[keycode] = false;
                s_Keyboard.scancodeState[scancode] = false;  
            }

            if (s_Video.eventDriver) {
                PalEventDriver* driver = s_Video.eventDriver;
                if (pressed) {
                    if (repeat) {
                        // key repeat
                        PalDispatchMode mode = palGetEventDispatchMode(driver, PAL_EVENT_KEYREPEAT);
                        if (mode != PAL_DISPATCH_NONE) {
                            PalEvent event = {0};
                            event.type = PAL_EVENT_KEYREPEAT;
                            event.data = palPackUint32(keycode, scancode);
                            event.data2 = palPackPointer((PalWindow*)hwnd);
                            palPushEvent(driver, &event);
                        }

                    } else {
                        // key press
                        PalDispatchMode mode = palGetEventDispatchMode(driver, PAL_EVENT_KEYDOWN);
                        if (mode != PAL_DISPATCH_NONE) {
                            PalEvent event = {0};
                            event.type = PAL_EVENT_KEYDOWN;
                            event.data = palPackUint32(keycode, scancode);
                            event.data2 = palPackPointer((PalWindow*)hwnd);
                            palPushEvent(driver, &event);
                        }
                    }
                } else {
                    // key release
                    PalDispatchMode mode = palGetEventDispatchMode(driver, PAL_EVENT_KEYUP);
                    if (mode != PAL_DISPATCH_NONE) {
                        PalEvent event = {0};
                        event.type = PAL_EVENT_KEYUP;
                        event.data = palPackUint32(keycode, scancode);
                        event.data2 = palPackPointer((PalWindow*)hwnd);
                        palPushEvent(driver, &event);
                    }
                }
            }

            return 0;
        }

        case WM_ERASEBKGND:
        {
            return true;
        }

        case WM_SETCURSOR: {
            if (LOWORD(lParam) == HTCLIENT) {
                if (data) {
                    SetCursor((HCURSOR)data);
                    return TRUE;
                }
                return TRUE;
            }

            break;
        }

    }

    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

BOOL CALLBACK enumMonitors(
    HMONITOR monitor, 
    HDC hdc, 
    LPRECT lRect, 
    LPARAM lParam) {
    
    MonitorData* data = (MonitorData*)lParam;
    if (data->monitors) {
        if (data->count < data->maxCount) {
            data->monitors[data->count] = (PalMonitor*)monitor;
        }
    }

    data->count++;
    return TRUE;
}

static inline PalOrientation orientationFromWin32(
    DWORD orientation) {
    
    switch (orientation) {
        case DMDO_DEFAULT: 
        return PAL_ORIENTATION_LANDSCAPE;

        case DMDO_90: 
        return PAL_ORIENTATION_PORTRAIT;

        case DMDO_180: 
        return PAL_ORIENTATION_LANDSCAPE_FLIPPED;

        case DMDO_270: 
        return PAL_ORIENTATION_PORTRAIT_FLIPPED;
    }
    return PAL_ORIENTATION_LANDSCAPE;
}

static inline DWORD orientationToin32(
    PalOrientation orientation) {

    switch (orientation) {
        case PAL_ORIENTATION_LANDSCAPE: 
        return DMDO_DEFAULT;

        case PAL_ORIENTATION_PORTRAIT: 
        return DMDO_90;

        case PAL_ORIENTATION_LANDSCAPE_FLIPPED: 
        return DMDO_180;

        case PAL_ORIENTATION_PORTRAIT_FLIPPED:
        return DMDO_270;
    }
    return NULL_ORIENTATION;
}

static inline PalResult setMonitorMode(
    PalMonitor* monitor,
    PalMonitorMode* mode,
    bool test) {
    
    if (!monitor || !mode) {
        return PAL_RESULT_NULL_POINTER;
    } 

    MONITORINFOEXW mi = {};
    mi.cbSize = sizeof(MONITORINFOEXW);
    if (!GetMonitorInfoW((HMONITOR)monitor, (MONITORINFO*)&mi)) { 
        DWORD error = GetLastError();
        if (error == ERROR_INVALID_HANDLE) {
            return PAL_RESULT_INVALID_MONITOR;

        } else {
            return PAL_RESULT_PLATFORM_FAILURE;
        }
    }

    DWORD flags = DM_PELSWIDTH | DM_PELSHEIGHT;
    flags |= DM_DISPLAYFREQUENCY | DM_BITSPERPEL;

    DEVMODE devMode = {};
    devMode.dmSize = sizeof(DEVMODE);
    devMode.dmFields = flags;

    devMode.dmPelsWidth = mode->width;
    devMode.dmPelsHeight = mode->height;
    devMode.dmDisplayFrequency = mode->refreshRate;
    devMode.dmBitsPerPel = mode->bpp;

    DWORD settingsFlag = CDS_FULLSCREEN;
    if (test) {
        settingsFlag = CDS_TEST;
    }

    ULONG result = ChangeDisplaySettingsExW(
        mi.szDevice,
        &devMode,
        NULL,
        settingsFlag,
        NULL
    );

    if (result == DISP_CHANGE_SUCCESSFUL) {
        return PAL_RESULT_SUCCESS;
    } else {
        return PAL_RESULT_INVALID_MONITOR_MODE;
    }
}

static inline bool compareMonitorMode(
    const PalMonitorMode* a, 
    const PalMonitorMode* b) {

    return 
        a->bpp == b->bpp               &&
        a->width == b->width           &&
        a->height == b->height         &&
        a->refreshRate == b->refreshRate;
}

static inline void addMonitorMode(
    PalMonitorMode* modes, 
    const PalMonitorMode* mode, 
    Int32* count) {

    // check if we have a duplicate mode
    for (Int32 i = 0; i < *count; i++) {
        PalMonitorMode* oldMode = &modes[i];
        if (compareMonitorMode(oldMode, mode)) {
            return; // discard it
        }
    }

    // new mode
    modes[*count] = *mode;
    *count += 1;
}

static void createKeycodeTable() {

    // Letters
    s_Keyboard.keycodes['A'] = PAL_KEYCODE_A;
    s_Keyboard.keycodes['B'] = PAL_KEYCODE_B;
    s_Keyboard.keycodes['C'] = PAL_KEYCODE_C;
    s_Keyboard.keycodes['D'] = PAL_KEYCODE_D;
    s_Keyboard.keycodes['E'] = PAL_KEYCODE_E;
    s_Keyboard.keycodes['F'] = PAL_KEYCODE_F;
    s_Keyboard.keycodes['G'] = PAL_KEYCODE_G;
    s_Keyboard.keycodes['H'] = PAL_KEYCODE_H;
    s_Keyboard.keycodes['I'] = PAL_KEYCODE_I;
    s_Keyboard.keycodes['J'] = PAL_KEYCODE_J;
    s_Keyboard.keycodes['K'] = PAL_KEYCODE_K;
    s_Keyboard.keycodes['L'] = PAL_KEYCODE_L;
    s_Keyboard.keycodes['M'] = PAL_KEYCODE_M;
    s_Keyboard.keycodes['N'] = PAL_KEYCODE_N;
    s_Keyboard.keycodes['O'] = PAL_KEYCODE_O;
    s_Keyboard.keycodes['P'] = PAL_KEYCODE_P;
    s_Keyboard.keycodes['Q'] = PAL_KEYCODE_Q;
    s_Keyboard.keycodes['R'] = PAL_KEYCODE_R;
    s_Keyboard.keycodes['S'] = PAL_KEYCODE_S;
    s_Keyboard.keycodes['T'] = PAL_KEYCODE_T;
    s_Keyboard.keycodes['U'] = PAL_KEYCODE_U;
    s_Keyboard.keycodes['V'] = PAL_KEYCODE_V;
    s_Keyboard.keycodes['W'] = PAL_KEYCODE_W;
    s_Keyboard.keycodes['X'] = PAL_KEYCODE_X;
    s_Keyboard.keycodes['Y'] = PAL_KEYCODE_Y;
    s_Keyboard.keycodes['Z'] = PAL_KEYCODE_Z;

    // Numbers (top row)
    s_Keyboard.keycodes['0'] = PAL_KEYCODE_0;
    s_Keyboard.keycodes['1'] = PAL_KEYCODE_1;
    s_Keyboard.keycodes['2'] = PAL_KEYCODE_2;
    s_Keyboard.keycodes['3'] = PAL_KEYCODE_3;
    s_Keyboard.keycodes['4'] = PAL_KEYCODE_4;
    s_Keyboard.keycodes['5'] = PAL_KEYCODE_5;
    s_Keyboard.keycodes['6'] = PAL_KEYCODE_6;
    s_Keyboard.keycodes['7'] = PAL_KEYCODE_7;
    s_Keyboard.keycodes['8'] = PAL_KEYCODE_8;
    s_Keyboard.keycodes['9'] = PAL_KEYCODE_9;

    // Function
    s_Keyboard.keycodes[VK_F1] = PAL_KEYCODE_F1;
    s_Keyboard.keycodes[VK_F2] = PAL_KEYCODE_F2;
    s_Keyboard.keycodes[VK_F3] = PAL_KEYCODE_F3;
    s_Keyboard.keycodes[VK_F4] = PAL_KEYCODE_F4;
    s_Keyboard.keycodes[VK_F5] = PAL_KEYCODE_F5;
    s_Keyboard.keycodes[VK_F6] = PAL_KEYCODE_F6;
    s_Keyboard.keycodes[VK_F7] = PAL_KEYCODE_F7;
    s_Keyboard.keycodes[VK_F8] = PAL_KEYCODE_F8;
    s_Keyboard.keycodes[VK_F9] = PAL_KEYCODE_F9;
    s_Keyboard.keycodes[VK_F10] = PAL_KEYCODE_F10;
    s_Keyboard.keycodes[VK_F11] = PAL_KEYCODE_F11;
    s_Keyboard.keycodes[VK_F12] = PAL_KEYCODE_F12;

    // Control
    s_Keyboard.keycodes[VK_ESCAPE] = PAL_KEYCODE_ESCAPE;
    s_Keyboard.keycodes[VK_RETURN] = PAL_KEYCODE_ENTER;
    s_Keyboard.keycodes[VK_TAB] = PAL_KEYCODE_TAB;
    s_Keyboard.keycodes[VK_BACK] = PAL_KEYCODE_BACKSPACE;
    s_Keyboard.keycodes[VK_SPACE] = PAL_KEYCODE_SPACE;
    s_Keyboard.keycodes[VK_CAPITAL] = PAL_KEYCODE_CAPSLOCK;
    s_Keyboard.keycodes[VK_NUMLOCK] = PAL_KEYCODE_NUMLOCK;
    s_Keyboard.keycodes[VK_SCROLL] = PAL_KEYCODE_SCROLLLOCK;
    s_Keyboard.keycodes[VK_SHIFT] = PAL_KEYCODE_LSHIFT;  // left shift
    s_Keyboard.keycodes[VK_RSHIFT] = PAL_KEYCODE_RSHIFT;
    s_Keyboard.keycodes[VK_CONTROL] = PAL_KEYCODE_LCTRL; // left control
    s_Keyboard.keycodes[VK_RCONTROL] = PAL_KEYCODE_RCTRL;
    s_Keyboard.keycodes[VK_MENU] = PAL_KEYCODE_LALT; // left Alt
    s_Keyboard.keycodes[VK_RMENU] = PAL_KEYCODE_RALT;

    // Arrows
    s_Keyboard.keycodes[VK_LEFT] = PAL_KEYCODE_LEFT;
    s_Keyboard.keycodes[VK_RIGHT] = PAL_KEYCODE_RIGHT;
    s_Keyboard.keycodes[VK_UP] = PAL_KEYCODE_UP;
    s_Keyboard.keycodes[VK_DOWN] = PAL_KEYCODE_DOWN;

    // Navigation
    s_Keyboard.keycodes[VK_INSERT] = PAL_KEYCODE_INSERT;
    s_Keyboard.keycodes[VK_DELETE] = PAL_KEYCODE_DELETE;
    s_Keyboard.keycodes[VK_HOME] = PAL_KEYCODE_HOME;
    s_Keyboard.keycodes[VK_END] = PAL_KEYCODE_END;
    s_Keyboard.keycodes[VK_PRIOR] = PAL_KEYCODE_PAGEUP;
    s_Keyboard.keycodes[VK_NEXT] = PAL_KEYCODE_PAGEDOWN;

    // Keypad
    s_Keyboard.keycodes[VK_NUMPAD0] = PAL_KEYCODE_KP_0;
    s_Keyboard.keycodes[VK_NUMPAD1] = PAL_KEYCODE_KP_1;
    s_Keyboard.keycodes[VK_NUMPAD2] = PAL_KEYCODE_KP_2;
    s_Keyboard.keycodes[VK_NUMPAD3] = PAL_KEYCODE_KP_3;
    s_Keyboard.keycodes[VK_NUMPAD4] = PAL_KEYCODE_KP_4;
    s_Keyboard.keycodes[VK_NUMPAD5] = PAL_KEYCODE_KP_5;
    s_Keyboard.keycodes[VK_NUMPAD6] = PAL_KEYCODE_KP_6;
    s_Keyboard.keycodes[VK_NUMPAD7] = PAL_KEYCODE_KP_7;
    s_Keyboard.keycodes[VK_NUMPAD8] = PAL_KEYCODE_KP_8;
    s_Keyboard.keycodes[VK_NUMPAD9] = PAL_KEYCODE_KP_9;
    
    s_Keyboard.keycodes[VK_ADD] = PAL_KEYCODE_KP_ADD;
    s_Keyboard.keycodes[VK_SUBTRACT] = PAL_KEYCODE_KP_SUBTRACT;
    s_Keyboard.keycodes[VK_MULTIPLY] = PAL_KEYCODE_KP_MULTIPLY;
    s_Keyboard.keycodes[VK_DIVIDE] = PAL_KEYCODE_KP_DIVIDE;
    s_Keyboard.keycodes[VK_DECIMAL] = PAL_KEYCODE_KP_DECIMAL;

    // Misc
    s_Keyboard.keycodes[VK_SNAPSHOT] = PAL_KEYCODE_PRINTSCREEN;
    s_Keyboard.keycodes[VK_PAUSE] = PAL_KEYCODE_PAUSE;    
    s_Keyboard.keycodes[VK_APPS] = PAL_KEYCODE_MENU;
    s_Keyboard.keycodes[VK_OEM_7] = PAL_KEYCODE_APOSTROPHE;
    s_Keyboard.keycodes[VK_OEM_5] = PAL_KEYCODE_BACKSLASH;
    s_Keyboard.keycodes[VK_OEM_COMMA] = PAL_KEYCODE_COMMA;
    s_Keyboard.keycodes[VK_OEM_PLUS] = PAL_KEYCODE_EQUAL;
    s_Keyboard.keycodes[VK_OEM_3] = PAL_KEYCODE_GRAVEACCENT;
    s_Keyboard.keycodes[VK_OEM_MINUS] = PAL_KEYCODE_SUBTRACT;
    s_Keyboard.keycodes[VK_OEM_PERIOD] = PAL_KEYCODE_PERIOD;
    s_Keyboard.keycodes[VK_OEM_1] = PAL_KEYCODE_SEMICOLON;
    s_Keyboard.keycodes[VK_OEM_2] = PAL_KEYCODE_SLASH;
    s_Keyboard.keycodes[VK_OEM_4] = PAL_KEYCODE_LBRACKET;
    s_Keyboard.keycodes[VK_OEM_6] = PAL_KEYCODE_RBRACKET;
    s_Keyboard.keycodes[VK_LWIN] = PAL_KEYCODE_LSUPER;
    s_Keyboard.keycodes[VK_RWIN] = PAL_KEYCODE_RSUPER;
}

static void createScancodeTable() {

    // Scancodes are made from OR'ed (scancode | extended)
    // Letters
    s_Keyboard.scancodes[0x01E] = PAL_SCANCODE_A;
    s_Keyboard.scancodes[0x030] = PAL_SCANCODE_B;
    s_Keyboard.scancodes[0x02E] = PAL_SCANCODE_C;
    s_Keyboard.scancodes[0x020] = PAL_SCANCODE_D;
    s_Keyboard.scancodes[0x012] = PAL_SCANCODE_E;
    s_Keyboard.scancodes[0x021] = PAL_SCANCODE_F;
    s_Keyboard.scancodes[0x022] = PAL_SCANCODE_G;
    s_Keyboard.scancodes[0x023] = PAL_SCANCODE_H;
    s_Keyboard.scancodes[0x017] = PAL_SCANCODE_I;
    s_Keyboard.scancodes[0x024] = PAL_SCANCODE_J;
    s_Keyboard.scancodes[0x025] = PAL_SCANCODE_K;
    s_Keyboard.scancodes[0x026] = PAL_SCANCODE_L;
    s_Keyboard.scancodes[0x032] = PAL_SCANCODE_M;
    s_Keyboard.scancodes[0x031] = PAL_SCANCODE_N;
    s_Keyboard.scancodes[0x018] = PAL_SCANCODE_O;
    s_Keyboard.scancodes[0x019] = PAL_SCANCODE_P;
    s_Keyboard.scancodes[0x010] = PAL_SCANCODE_Q;
    s_Keyboard.scancodes[0x013] = PAL_SCANCODE_R;
    s_Keyboard.scancodes[0x01F] = PAL_SCANCODE_S;
    s_Keyboard.scancodes[0x014] = PAL_SCANCODE_T;
    s_Keyboard.scancodes[0x016] = PAL_SCANCODE_U;
    s_Keyboard.scancodes[0x02F] = PAL_SCANCODE_V;
    s_Keyboard.scancodes[0x011] = PAL_SCANCODE_W;
    s_Keyboard.scancodes[0x02D] = PAL_SCANCODE_X;
    s_Keyboard.scancodes[0x015] = PAL_SCANCODE_Y;
    s_Keyboard.scancodes[0x02C] = PAL_SCANCODE_Z;

    // Numbers (top row)
    s_Keyboard.scancodes[0x00B] = PAL_SCANCODE_0;
    s_Keyboard.scancodes[0x002] = PAL_SCANCODE_1;
    s_Keyboard.scancodes[0x003] = PAL_SCANCODE_2;
    s_Keyboard.scancodes[0x004] = PAL_SCANCODE_3;
    s_Keyboard.scancodes[0x005] = PAL_SCANCODE_4;
    s_Keyboard.scancodes[0x006] = PAL_SCANCODE_5;
    s_Keyboard.scancodes[0x007] = PAL_SCANCODE_6;
    s_Keyboard.scancodes[0x008] = PAL_SCANCODE_7;
    s_Keyboard.scancodes[0x009] = PAL_SCANCODE_8;
    s_Keyboard.scancodes[0x00A] = PAL_SCANCODE_9;

    // Function
    s_Keyboard.scancodes[0x03B] = PAL_SCANCODE_F1;
    s_Keyboard.scancodes[0x03C] = PAL_SCANCODE_F2;
    s_Keyboard.scancodes[0x03D] = PAL_SCANCODE_F3;
    s_Keyboard.scancodes[0x03E] = PAL_SCANCODE_F4;
    s_Keyboard.scancodes[0x03F] = PAL_SCANCODE_F5;
    s_Keyboard.scancodes[0x040] = PAL_SCANCODE_F6;
    s_Keyboard.scancodes[0x041] = PAL_SCANCODE_F7;
    s_Keyboard.scancodes[0x042] = PAL_SCANCODE_F8;
    s_Keyboard.scancodes[0x043] = PAL_SCANCODE_F9;
    s_Keyboard.scancodes[0x044] = PAL_SCANCODE_F10;
    s_Keyboard.scancodes[0x057] = PAL_SCANCODE_F11;
    s_Keyboard.scancodes[0x058] = PAL_SCANCODE_F12;

    // Control
    s_Keyboard.scancodes[0x001] = PAL_SCANCODE_ESCAPE;
    s_Keyboard.scancodes[0x01C] = PAL_SCANCODE_ENTER;
    s_Keyboard.scancodes[0x00F] = PAL_SCANCODE_TAB;
    s_Keyboard.scancodes[0x00E] = PAL_SCANCODE_BACKSPACE;
    s_Keyboard.scancodes[0x039] = PAL_SCANCODE_SPACE;
    s_Keyboard.scancodes[0x03A] = PAL_SCANCODE_CAPSLOCK;
    s_Keyboard.scancodes[0x145] = PAL_SCANCODE_NUMLOCK;
    s_Keyboard.scancodes[0x046] = PAL_SCANCODE_SCROLLLOCK;
    s_Keyboard.scancodes[0x02A] = PAL_SCANCODE_LSHIFT;
    s_Keyboard.scancodes[0x036] = PAL_SCANCODE_RSHIFT;
    s_Keyboard.scancodes[0x01D] = PAL_SCANCODE_LCTRL;
    s_Keyboard.scancodes[0x11D] = PAL_SCANCODE_RCTRL;
    s_Keyboard.scancodes[0x038] = PAL_SCANCODE_LALT;
    s_Keyboard.scancodes[0x138] = PAL_SCANCODE_RALT;

    // Arrows
    s_Keyboard.scancodes[0x14B] = PAL_SCANCODE_LEFT;
    s_Keyboard.scancodes[0x14D] = PAL_SCANCODE_RIGHT;
    s_Keyboard.scancodes[0x148] = PAL_SCANCODE_UP;
    s_Keyboard.scancodes[0x150] = PAL_SCANCODE_DOWN;

    // Navigation
    s_Keyboard.scancodes[0x152] = PAL_SCANCODE_INSERT;
    s_Keyboard.scancodes[0x153] = PAL_SCANCODE_DELETE;
    s_Keyboard.scancodes[0x147] = PAL_SCANCODE_HOME;
    s_Keyboard.scancodes[0x14F] = PAL_SCANCODE_END;
    s_Keyboard.scancodes[0x149] = PAL_SCANCODE_PAGEUP;
    s_Keyboard.scancodes[0x151] = PAL_SCANCODE_PAGEDOWN;

    // Keypad
    s_Keyboard.scancodes[0x052] = PAL_SCANCODE_KP_0;
    s_Keyboard.scancodes[0x04F] = PAL_SCANCODE_KP_1;
    s_Keyboard.scancodes[0x050] = PAL_SCANCODE_KP_2;
    s_Keyboard.scancodes[0x051] = PAL_SCANCODE_KP_3;
    s_Keyboard.scancodes[0x04B] = PAL_SCANCODE_KP_4;
    s_Keyboard.scancodes[0x04C] = PAL_SCANCODE_KP_5;
    s_Keyboard.scancodes[0x04D] = PAL_SCANCODE_KP_6;
    s_Keyboard.scancodes[0x047] = PAL_SCANCODE_KP_7;
    s_Keyboard.scancodes[0x048] = PAL_SCANCODE_KP_8;
    s_Keyboard.scancodes[0x049] = PAL_SCANCODE_KP_9;
    s_Keyboard.scancodes[0x11C] = PAL_SCANCODE_KP_ENTER;
    s_Keyboard.scancodes[0x04E] = PAL_SCANCODE_KP_ADD;
    s_Keyboard.scancodes[0x04A] = PAL_SCANCODE_KP_SUBTRACT;
    s_Keyboard.scancodes[0x037] = PAL_SCANCODE_KP_MULTIPLY;
    s_Keyboard.scancodes[0x135] = PAL_SCANCODE_KP_DIVIDE;
    s_Keyboard.scancodes[0x053] = PAL_SCANCODE_KP_DECIMAL;
    s_Keyboard.scancodes[0x059] = PAL_SCANCODE_KP_EQUAL;

    // Misc
    s_Keyboard.scancodes[0x137] = PAL_SCANCODE_PRINTSCREEN;
    s_Keyboard.scancodes[0x146] = PAL_SCANCODE_PAUSE;    
    s_Keyboard.scancodes[0x045] = PAL_SCANCODE_PAUSE;
    s_Keyboard.scancodes[0x15D] = PAL_SCANCODE_MENU;
    s_Keyboard.scancodes[0x028] = PAL_SCANCODE_APOSTROPHE;
    s_Keyboard.scancodes[0x02B] = PAL_SCANCODE_BACKSLASH;
    s_Keyboard.scancodes[0x033] = PAL_SCANCODE_COMMA;
    s_Keyboard.scancodes[0x00D] = PAL_SCANCODE_EQUAL;
    s_Keyboard.scancodes[0x029] = PAL_SCANCODE_GRAVEACCENT;
    s_Keyboard.scancodes[0x00C] = PAL_SCANCODE_SUBTRACT;
    s_Keyboard.scancodes[0x034] = PAL_SCANCODE_PERIOD;
    s_Keyboard.scancodes[0x027] = PAL_SCANCODE_SEMICOLON;
    s_Keyboard.scancodes[0x035] = PAL_SCANCODE_SLASH;
    s_Keyboard.scancodes[0x01A] = PAL_SCANCODE_LBRACKET;
    s_Keyboard.scancodes[0x01B] = PAL_SCANCODE_RBRACKET;
    s_Keyboard.scancodes[0x15B] = PAL_SCANCODE_LSUPER;
    s_Keyboard.scancodes[0x15C] = PAL_SCANCODE_RSUPER;
}

// ==================================================
// Public API
// ==================================================

PalResult PAL_CALL palInitVideo(
    const PalAllocator *allocator,
    PalEventDriver* eventDriver) {
    
    if (s_Video.initialized) {
        return PAL_RESULT_SUCCESS;
    }
    
    if (allocator && (!allocator->allocate || !allocator->free)) {
        return PAL_RESULT_INVALID_ALLOCATOR;
    }

    // get the instance
    s_Video.instance = GetModuleHandleW(nullptr);

    // register class
    WNDCLASSEXW wc = {0};
    wc.cbSize = sizeof(WNDCLASSEXW);
    wc.hCursor = LoadCursorW(NULL, IDC_ARROW);
    wc.hIcon = LoadIconW(NULL, IDI_APPLICATION);
    wc.hIconSm = LoadIconW(NULL, IDI_APPLICATION);
    wc.hInstance = s_Video.instance;
    wc.lpfnWndProc = videoProc;
    wc.lpszClassName = PAL_VIDEO_CLASS;
    wc.style = CS_OWNDC;

    // since we check every input carefully, the only error we can get is access denied
    if (!RegisterClassExW(&wc)) {
        return PAL_RESULT_ACCESS_DENIED;
    }

    // create hidden window
    s_Video.hiddenWindow = CreateWindowExW(
        0,
        PAL_VIDEO_CLASS,
        L"HiddenWindow",
        WS_OVERLAPPEDWINDOW,
        0,
        0,
        8,
        8,
        nullptr,
        nullptr,
        s_Video.instance,
        nullptr
    );

    if (!s_Video.hiddenWindow) {
        return PAL_RESULT_PLATFORM_FAILURE;
    }

    // set a flag to set if the window has been created
    SetWindowLongPtrW(s_Video.hiddenWindow, GWLP_USERDATA, (LONG_PTR)&s_Event);

    // register raw input for mice to get delta
    RAWINPUTDEVICE rid = {0};
    rid.dwFlags = RIDEV_INPUTSINK;
    rid.hwndTarget = s_Video.hiddenWindow;
    rid.usUsage = 0x02; 
    rid.usUsagePage = 0x01; 
    if (!RegisterRawInputDevices(&rid, 1, sizeof(RAWINPUTDEVICE))) {
        return PAL_RESULT_PLATFORM_FAILURE;
    }

    // create mapping table
    createKeycodeTable();
    createScancodeTable();

    // load shared libraries
    // shcore
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

    // gdi functios
    s_Video.gdi = LoadLibraryA("gdi32.dll");
    if (s_Video.gdi) {
        s_Video.createDIBSection = (CreateDIBSectionFn)GetProcAddress(
            s_Video.gdi,
            "CreateDIBSection"
        );

        s_Video.createBitmap = (CreateBitmapFn)GetProcAddress(
            s_Video.gdi,
            "CreateBitmap"
        );

        s_Video.deleteObject = (DeleteObjectFn)GetProcAddress(
            s_Video.gdi,
            "DeleteObject"
        );
    }

    // set features
    s_Video.features |= PAL_VIDEO_FEATURE_MONITOR_ORIENTATION;
    s_Video.features |= PAL_VIDEO_FEATURE_BORDERLESS_WINDOW;
    s_Video.features |= PAL_VIDEO_FEATURE_TRANSPARENT_WINDOW;
    s_Video.features |= PAL_VIDEO_FEATURE_TOOL_WINDOW;
    s_Video.features |= PAL_VIDEO_FEATURE_MONITOR_MODE_SWITCH;
    s_Video.features |= PAL_VIDEO_FEATURE_MULTI_MONITORS;
    s_Video.features |= PAL_VIDEO_FEATURE_WINDOW_RESIZING;
    s_Video.features |= PAL_VIDEO_FEATURE_WINDOW_POSITIONING;
    s_Video.features |= PAL_VIDEO_FEATURE_WINDOW_MINMAX;
    s_Video.features |= PAL_VIDEO_FEATURE_NO_MAXIMIZEBOX;
    s_Video.features |= PAL_VIDEO_FEATURE_NO_MINIMIZEBOX;
    s_Video.features |= PAL_VIDEO_FEATURE_MONITOR_GAMMA_CONTROL;
    s_Video.features |= PAL_VIDEO_FEATURE_CLIP_CURSOR;
    s_Video.features |= PAL_VIDEO_FEATURE_WINDOW_FLASH_CAPTION;
    s_Video.features |= PAL_VIDEO_FEATURE_WINDOW_FLASH_TRAY;
    s_Video.features |= PAL_VIDEO_FEATURE_WINDOW_FLASH_TRAY;
    s_Video.features |= PAL_VIDEO_FEATURE_WINDOW_FLASH_INTERVAL;

    if (s_Video.getDpiForMonitor && s_Video.setProcessAwareness) {
        s_Video.features |= PAL_VIDEO_FEATURE_HIGH_DPI;
        s_Video.setProcessAwareness(WIN32_DPI_AWARE);
    }

    s_Video.initialized = true;
    s_Video.allocator = allocator;
    s_Video.eventDriver = eventDriver;
    return PAL_RESULT_SUCCESS;
}

void PAL_CALL palShutdownVideo() {

    if (!s_Video.initialized) {
        return;
    }

    if (s_Video.shcore) {
        FreeLibrary(s_Video.shcore);
    }

    FreeLibrary(s_Video.gdi);
    DestroyWindow(s_Video.hiddenWindow);
    UnregisterClassW(PAL_VIDEO_CLASS, s_Video.instance);
    s_Video.initialized = false;
}

void PAL_CALL palUpdateVideo() {

    if (!s_Video.initialized) {
        return;
    }

    s_Mouse.dx = 0;
    s_Mouse.dy = 0;

    MSG msg;
    while (PeekMessageA(&msg, NULL, 0, 0, PM_REMOVE)) {
        TranslateMessage(&msg);
        DispatchMessageA(&msg);
    }

    // push pending move and reszie events
    if (s_Event.pendingResize) {
        PalEvent event = {0};
        event.type = PAL_EVENT_WINDOW_SIZE;
        event.data = palPackUint32(s_Event.width, s_Event.height);
        event.data2 = palPackPointer(s_Event.window);
        palPushEvent(s_Video.eventDriver, &event);

        // push a window state event
        event.type = PAL_EVENT_WINDOW_STATE;
        event.data = s_Event.state;
        palPushEvent(s_Video.eventDriver, &event);

        s_Event.pendingResize = false;

    } else if (s_Event.pendingMove) {
        PalEvent event = {0};
        event.type = PAL_EVENT_WINDOW_MOVE;
        event.data = palPackInt32(s_Event.x, s_Event.y);
        event.data2 = palPackPointer(s_Event.window);
        palPushEvent(s_Video.eventDriver, &event);
        s_Event.pendingMove = false;
    }
}

PalVideoFeatures PAL_CALL palGetVideoFeatures() {

    if (!s_Video.initialized) {
        return 0;
    }

    return s_Video.features;
}

// ==================================================
// Monitor
// ==================================================

PalResult PAL_CALL palEnumerateMonitors(
    Int32 *count,
    PalMonitor **outMonitors) {

    if (!s_Video.initialized) {
        return PAL_RESULT_VIDEO_NOT_INITIALIZED;
    }
    
    if (!count) {
        return PAL_RESULT_NULL_POINTER;
    }

    if (count == 0 && outMonitors) {
        PAL_RESULT_INSUFFICIENT_BUFFER;
    }

    MonitorData data;
    data.count = 0;
    data.monitors = outMonitors;
    data.maxCount = outMonitors ? *count : 0;
    EnumDisplayMonitors(nullptr, nullptr, enumMonitors, (LPARAM)&data);

    if (!outMonitors) {
        *count = data.count;
    }
    return PAL_RESULT_SUCCESS;
}

PalResult PAL_CALL palGetPrimaryMonitor(
    PalMonitor **outMonitor) {

    if (!s_Video.initialized) {
        return PAL_RESULT_VIDEO_NOT_INITIALIZED;
    }
    
    if (!outMonitor) {
        return PAL_RESULT_NULL_POINTER;
    }

    HMONITOR monitor = MonitorFromPoint((POINT){0, 0}, MONITOR_DEFAULTTOPRIMARY);
    if (!monitor) {
        return PAL_RESULT_PLATFORM_FAILURE;
    }

    *outMonitor = (PalMonitor*)monitor;
    return PAL_RESULT_SUCCESS;
}

PalResult PAL_CALL palGetMonitorInfo(
    PalMonitor *monitor,
    PalMonitorInfo *info) {
    
    if (!s_Video.initialized) {
        return PAL_RESULT_VIDEO_NOT_INITIALIZED;
    }

    if (!monitor || !info) {
        return PAL_RESULT_NULL_POINTER;
    }

    MONITORINFOEXW mi = {};
    mi.cbSize = sizeof(MONITORINFOEXW);
    if (!GetMonitorInfoW((HMONITOR)monitor, (MONITORINFO*)&mi)) { 
        DWORD error = GetLastError();
        if (error == ERROR_INVALID_HANDLE) {
            return PAL_RESULT_INVALID_MONITOR;

        } else {
            return PAL_RESULT_PLATFORM_FAILURE;
        }
    }

    info->x = mi.rcMonitor.left;
    info->y = mi.rcMonitor.top;
    info->width = mi.rcMonitor.right - mi.rcMonitor.left;
    info->height = mi.rcMonitor.bottom - mi.rcWork.top;

    // get name
    WideCharToMultiByte(CP_UTF8, 0, mi.szDevice, -1, info->name, 32, NULL, NULL);

    DEVMODE devMode = {};
    devMode.dmSize = sizeof(DEVMODE);
    EnumDisplaySettingsW(mi.szDevice, ENUM_CURRENT_SETTINGS, &devMode);
    info->refreshRate = devMode.dmDisplayFrequency;   
    info->orientation = orientationFromWin32(devMode.dmDisplayOrientation);

    // get dpi scale
    Int32 dpiX, dpiY;
    if (s_Video.getDpiForMonitor) {
        s_Video.getDpiForMonitor((HMONITOR)monitor, WIN32_DPI, &dpiX, &dpiY);
        
    } else {
        dpiX = 96;
    }    
    info->dpi = dpiX;

    // check for primary monitor
    HMONITOR primaryMonitor = MonitorFromPoint(
        (POINT){0, 0}, 
        MONITOR_DEFAULTTOPRIMARY
    );

    if (!primaryMonitor) {
        info->primary = false;
    }

    if (primaryMonitor == (HMONITOR)monitor) {
        info->primary = true;
    }

    return PAL_RESULT_SUCCESS;
}

PalResult PAL_CALL palEnumerateMonitorModes(
    PalMonitor *monitor,
    Int32 *count,
    PalMonitorMode *modes) {
    
    if (!s_Video.initialized) {
        return PAL_RESULT_VIDEO_NOT_INITIALIZED;
    }
    
    if (!monitor || !count) {
        return PAL_RESULT_NULL_POINTER;
    }

    Int32 modeCount = 0;
    Int32 maxModes = 0;
    PalMonitorMode* monitorModes = nullptr;

    MONITORINFOEXW mi = {};
    mi.cbSize = sizeof(MONITORINFOEXW);
    if (!GetMonitorInfoW((HMONITOR)monitor, (MONITORINFO*)&mi)) { 
        DWORD error = GetLastError();
        if (error == ERROR_INVALID_HANDLE) {
            return PAL_RESULT_INVALID_MONITOR;

        } else {
            return PAL_RESULT_PLATFORM_FAILURE;
        }
    }

    if (!modes) {
        // allocate and store tmp monitor modesand check for the interested fields.
        monitorModes = palAllocate(
            s_Video.allocator, 
            sizeof(PalMonitorMode) * MAX_MODE_COUNT,
            0
        );

        if (!monitorModes) {
            return PAL_RESULT_OUT_OF_MEMORY;
        }

        memset(
            monitorModes, 
            0,
            sizeof(PalMonitorMode) * MAX_MODE_COUNT
        );
        maxModes = MAX_MODE_COUNT;

    } else {
        monitorModes = modes;
        maxModes = *count;
    }

    DEVMODEW dm = {};
    dm.dmSize = sizeof(DEVMODE);
    for (Int32 i = 0; EnumDisplaySettingsW(mi.szDevice, i, &dm); i++) {
        // Pal support up to 128 modes
        if (modeCount > maxModes) {
            break;
        }

        PalMonitorMode* mode = &monitorModes[modeCount];
        mode->refreshRate = dm.dmDisplayFrequency;
        mode->width = dm.dmPelsWidth;
        mode->height = dm.dmPelsHeight;
        mode->bpp = dm.dmBitsPerPel;
        addMonitorMode(monitorModes, mode, &modeCount);
    }

    if (!modes) {
        *count = modeCount;
        palFree(s_Video.allocator, monitorModes);
    }

    return PAL_RESULT_SUCCESS;
}

PalResult PAL_CALL palGetCurrentMonitorMode(
    PalMonitor *monitor,
    PalMonitorMode *mode) {
    
    if (!s_Video.initialized) {
        return PAL_RESULT_VIDEO_NOT_INITIALIZED;
    }
    
    if (!monitor || !mode) {
        return PAL_RESULT_NULL_POINTER;
    }

    MONITORINFOEXW mi = {};
    mi.cbSize = sizeof(MONITORINFOEXW);
    if (!GetMonitorInfoW((HMONITOR)monitor, (MONITORINFO*)&mi)) { 
        DWORD error = GetLastError();
        if (error == ERROR_INVALID_HANDLE) {
            return PAL_RESULT_INVALID_MONITOR;

        } else {
            return PAL_RESULT_PLATFORM_FAILURE;
        }
    }

    DEVMODE devMode = {};
    devMode.dmSize = sizeof(DEVMODE);
    EnumDisplaySettingsW(mi.szDevice, ENUM_CURRENT_SETTINGS, &devMode);
    mode->width = devMode.dmPelsWidth;
    mode->height = devMode.dmPelsHeight;
    mode->refreshRate = devMode.dmDisplayFrequency;
    mode->bpp = devMode.dmBitsPerPel;

    return PAL_RESULT_SUCCESS;
}

PalResult PAL_CALL palSetMonitorMode(
    PalMonitor *monitor,
    PalMonitorMode *mode) {
    
    if (!s_Video.initialized) {
        return PAL_RESULT_VIDEO_NOT_INITIALIZED;
    }

    return setMonitorMode(monitor, mode, false);
}

PalResult PAL_CALL palValidateMonitorMode(
    PalMonitor *monitor,
    PalMonitorMode *mode) {

    if (!s_Video.initialized) {
        return PAL_RESULT_VIDEO_NOT_INITIALIZED;
    }
    
    return setMonitorMode(monitor, mode, true);
}

PalResult PAL_CALL palSetMonitorOrientation(
    PalMonitor *monitor,
    PalOrientation orientation) {
    
    if (!s_Video.initialized) {
        return PAL_RESULT_VIDEO_NOT_INITIALIZED;
    }
    
    if (!monitor) {
        return PAL_RESULT_NULL_POINTER;
    }

    DWORD win32Orientation = orientationToin32(orientation);
    if (orientation == NULL_ORIENTATION) {
        return PAL_RESULT_INVALID_ORIENTATION;
    }

    MONITORINFOEXW mi = {};
    mi.cbSize = sizeof(MONITORINFOEXW);
    if (!GetMonitorInfoW((HMONITOR)monitor, (MONITORINFO*)&mi)) {
        DWORD error = GetLastError();
        if (error == ERROR_INVALID_HANDLE) {
            return PAL_RESULT_INVALID_MONITOR;

        } else {
            return PAL_RESULT_PLATFORM_FAILURE;
        }
    }

    DEVMODE devMode = {};
    devMode.dmSize = sizeof(DEVMODE);
    EnumDisplaySettingsW(mi.szDevice, ENUM_CURRENT_SETTINGS, &devMode);
    DWORD monitorOrientation = devMode.dmDisplayOrientation;

    // only swap size if switching between landscape and portrait
    bool isMonitorLandscape = (monitorOrientation == DMDO_DEFAULT || monitorOrientation == DMDO_180);
    bool isLandscape = (win32Orientation == DMDO_DEFAULT || win32Orientation == DMDO_180);
    if (isMonitorLandscape != isLandscape) {
        DWORD tmp = devMode.dmPelsWidth;
        devMode.dmPelsWidth = devMode.dmPelsHeight;
        devMode.dmPelsHeight = tmp;
    }

    devMode.dmFields = DM_PELSWIDTH | DM_PELSHEIGHT | DM_DISPLAYORIENTATION;
    devMode.dmDisplayOrientation = win32Orientation;

    ULONG result = ChangeDisplaySettingsExW(
        mi.szDevice,
        &devMode,
        NULL,
        CDS_RESET,
        NULL
    );

    if (result == DISP_CHANGE_SUCCESSFUL) {
        return PAL_RESULT_SUCCESS;

    } else {
        return PAL_RESULT_INVALID_ORIENTATION;
    }
}

// ==================================================
// Window
// ==================================================

PalResult PAL_CALL palCreateWindow(
    const PalWindowCreateInfo *info,
    PalWindow **outWindow) {
    
    if (!s_Video.initialized) {
        return PAL_RESULT_VIDEO_NOT_INITIALIZED;
    }
    
    if (!info || !outWindow) {
        return PAL_RESULT_NULL_POINTER;
    }

    HWND handle = nullptr;
    PalMonitor* monitor = nullptr;
    PalMonitorInfo monitorInfo;

    Uint32 style = WS_CAPTION | WS_SYSMENU | WS_OVERLAPPED;
    Uint32 exStyle = 0;

    // no minimize box
    if (!(info->style & PAL_WINDOW_STYLE_NO_MINIMIZEBOX)) {
        style |= WS_MINIMIZEBOX;
    }

    // no maximize box
    if (!(info->style & PAL_WINDOW_STYLE_NO_MAXIMIZEBOX)) {
        style |= WS_MAXIMIZEBOX;
    }

    // resizable window
    if (info->style & PAL_WINDOW_STYLE_RESIZABLE) {
        style |= WS_THICKFRAME;

    } else {
        // not resizable. We remove the maximizebox even if user requested
        style &= ~WS_MAXIMIZEBOX;
    }

    // transparent window
    if (info->style & PAL_WINDOW_STYLE_TRANSPARENT) {
        exStyle |= WS_EX_LAYERED;
    }

    // tool window
    if (info->style & PAL_WINDOW_STYLE_TOOL) {
        exStyle |= WS_EX_TOOLWINDOW;
    }

    // topmost window
    if (info->style & PAL_WINDOW_STYLE_TOPMOST) {
        exStyle |= WS_EX_TOPMOST;
    }

    // get monitor
    if (info->monitor) {
        monitor = info->monitor;
    } else {
        // get primary monitor
        monitor = (PalMonitor*)MonitorFromPoint((POINT){0, 0}, MONITOR_DEFAULTTOPRIMARY);
        if (!monitor) {
            return PAL_RESULT_PLATFORM_FAILURE;
        }
    }

    // get monitor info
    PalResult result = palGetMonitorInfo(monitor, &monitorInfo);
    if (result != PAL_RESULT_SUCCESS) {
        return result;
    }

    // compose position. 
    Int32 x, y = 0;
    // the position and size must be scaled with the dpi before this call
    if (info->center) {
        x = monitorInfo.x + (monitorInfo.width - info->width) / 2;
        y = monitorInfo.y + (monitorInfo.height - info->height) / 2;

    } else {
        // we set 100 for each axix
        x = monitorInfo.x + 100;
        y = monitorInfo.y + 100;
    }

    // adjust the window size
    RECT rect = { 0, 0, 0, 0 };
    rect.right = info->width;
    rect.bottom = info->height;
    AdjustWindowRectEx(&rect, style, 0, exStyle);

    wchar_t buffer[WINDOW_NAME_SIZE];  
    MultiByteToWideChar(CP_UTF8, 0, info->title, -1, buffer, 256);

    // create the window
    handle = CreateWindowExW(
        exStyle,
        PAL_VIDEO_CLASS,
        buffer,
        style, 
        x,
        y,
        rect.right - rect.left,
        rect.bottom - rect.top,
        nullptr, 
        nullptr, 
        s_Video.instance,
        nullptr
    );

    if (!handle) {
        return PAL_RESULT_PLATFORM_FAILURE;
    }

    // show, maximize and minimize
    Int32 showFlag = SW_HIDE;
    // maximize
    if (info->showMaximized) {
        showFlag = SW_SHOWMAXIMIZED;
    }

    // minimized
    if (info->showMinimized) {
        showFlag = SW_SHOWMINIMIZED;
    }

    // shown
    if (info->show) {
        if (showFlag == SW_HIDE) {
            // change only if maximize and minimize are not set
            showFlag = SW_SHOW;
        }
    }

    ShowWindow(handle, showFlag);
    UpdateWindow(handle);

    if (info->style & PAL_WINDOW_STYLE_BORDERLESS) {
        // revert changes
        SetWindowLongPtrW(handle, GWL_STYLE, WS_POPUP);
        SetWindowLongPtrW(handle, GWL_EXSTYLE, WS_EX_APPWINDOW);

        // force a frame update
        SetWindowPos(
            handle,
            nullptr,
            0, 
            0,
            0,
            0,
            SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED
        );
    }

    // set a flag to set if the window has been created
    SetWindowLongPtrW(handle, GWLP_USERDATA, (LONG_PTR)&s_Event);

    *outWindow = (PalWindow*)handle;
    return PAL_RESULT_SUCCESS;
}

void PAL_CALL palDestroyWindow(
    PalWindow *window) {

    if (window) {
        DestroyWindow((HWND)window);
    }
}

PalResult PAL_CALL palMinimizeWindow(
    PalWindow* window) {
    
    if (!s_Video.initialized) {
        return PAL_RESULT_VIDEO_NOT_INITIALIZED;
    }
    
    if (!window) {
        return PAL_RESULT_NULL_POINTER;
    }

    if (!ShowWindow((HWND)window, SW_MINIMIZE)) {
        // technically, this will fail if the window handle is invalid
        return PAL_RESULT_INVALID_WINDOW;
    }
    return PAL_RESULT_SUCCESS;
}

PalResult PAL_CALL palMaximizeWindow(
    PalWindow* window) {
    
    if (!s_Video.initialized) {
        return PAL_RESULT_VIDEO_NOT_INITIALIZED;
    }
    
    if (!window) {
        return PAL_RESULT_NULL_POINTER;
    }

    if (!ShowWindow((HWND)window, SW_MAXIMIZE)) {
        // technically, this will fail if the window handle is invalid
        return PAL_RESULT_INVALID_WINDOW;
    }
    return PAL_RESULT_SUCCESS;
}

PalResult PAL_CALL palRestoreWindow(
    PalWindow* window) {
    
    if (!s_Video.initialized) {
        return PAL_RESULT_VIDEO_NOT_INITIALIZED;
    }

    if (!window) {
        return PAL_RESULT_NULL_POINTER;
    }

    if (!ShowWindow((HWND)window, SW_RESTORE)) {
        // technically, this will fail if the window handle is invalid
        return PAL_RESULT_INVALID_WINDOW;
    }
    return PAL_RESULT_SUCCESS;
}

PalResult PAL_CALL palShowWindow(
    PalWindow* window) {
    
    if (!s_Video.initialized) {
        return PAL_RESULT_VIDEO_NOT_INITIALIZED;
    }

    if (!window) {
        return PAL_RESULT_NULL_POINTER;
    }

    if (!ShowWindow((HWND)window, SW_SHOW)) {
        // technically, this will fail if the window handle is invalid
        return PAL_RESULT_INVALID_WINDOW;
    }
    return PAL_RESULT_SUCCESS;
}

PalResult PAL_CALL palHideWindow(
    PalWindow* window) {
    
    if (!s_Video.initialized) {
        return PAL_RESULT_VIDEO_NOT_INITIALIZED;
    }

    if (!window) {
        return PAL_RESULT_NULL_POINTER;
    }

    if (!ShowWindow((HWND)window, SW_HIDE)) {
        // technically, this will fail if the window handle is invalid
        return PAL_RESULT_INVALID_WINDOW;
    }
    return PAL_RESULT_SUCCESS;
}

PalResult PAL_CALL palFlashWindow(
    PalWindow* window,
    const PalFlashInfo* info) {
    
    if (!s_Video.initialized) {
        return PAL_RESULT_VIDEO_NOT_INITIALIZED;
    }

    if (!window || !info) {
        return PAL_RESULT_NULL_POINTER;
    }

    DWORD flags = 0;
    if (info->flags == PAL_FLASH_STOP) {
        flags = FLASHW_STOP;

    } else {
        if (info->flags & PAL_FLASH_CAPTION) {
            flags |= FLASHW_CAPTION;
        } if (info->flags & PAL_FLASH_TRAY) {
            flags |= FLASHW_TRAY;
            flags |= FLASHW_TIMERNOFG;
        }
    }

    FLASHWINFO flashInfo = {};
    flashInfo.cbSize = sizeof(FLASHWINFO);
    flashInfo.dwFlags = flags;
    flashInfo.dwTimeout = info->interval;
    flashInfo.hwnd = (HWND)window;
    flashInfo.uCount = info->count;

    bool success = FlashWindowEx(&flashInfo);
    if (!success) {
        DWORD error = GetLastError();
        if (error == ERROR_INVALID_HANDLE) {
            return PAL_RESULT_INVALID_WINDOW;

        } else {
            return PAL_RESULT_PLATFORM_FAILURE;
        }
    } 

    return PAL_RESULT_SUCCESS;
}

PalResult PAL_CALL palGetWindowStyle(
    PalWindow* window,
    PalWindowStyle* outStyle) {
    
    if (!s_Video.initialized) {
        return PAL_RESULT_VIDEO_NOT_INITIALIZED;
    }
    
    if (!window || !outStyle) {
        return PAL_RESULT_NULL_POINTER;
    }

    PalWindowStyle windowStyle = 0;
    DWORD style = GetWindowLongPtrW((HWND)window, GWL_STYLE);
    DWORD exStyle = GetWindowLongPtrW((HWND)window, GWL_EXSTYLE);

    if (!style) {
        // since there is no window without styles
        // we assume if we dont get any, the window handle is invalid
        return PAL_RESULT_INVALID_WINDOW;
    }
    
    // check if we can resize
    if (style & WS_THICKFRAME) {
        windowStyle |= PAL_WINDOW_STYLE_RESIZABLE;
    }

    // check if we are transparent
    if (exStyle & WS_EX_LAYERED) {
        windowStyle |= PAL_WINDOW_STYLE_TRANSPARENT;
    }

    // check if we are a topmost window
    if (exStyle & WS_EX_TOPMOST) {
        windowStyle |= PAL_WINDOW_STYLE_TOPMOST;
    }
    
    // check if we have a minimize box
    if (!(style & WS_MINIMIZEBOX)) {
        windowStyle |= PAL_WINDOW_STYLE_NO_MINIMIZEBOX;
    }

    // check if we have a maximize box
    if (!(style & WS_MAXIMIZEBOX)) {
        windowStyle |= PAL_WINDOW_STYLE_NO_MAXIMIZEBOX;
    }

    // check if its a tool window
    if (exStyle & WS_EX_TOOLWINDOW) {
        windowStyle |= PAL_WINDOW_STYLE_TOOL;
    }

    // we check borderless last since it will overwrite other styles
    if (style & WS_POPUP) {
        windowStyle |= PAL_WINDOW_STYLE_BORDERLESS;

        // we remove minimize and maximize box if set
        windowStyle &= ~PAL_WINDOW_STYLE_NO_MINIMIZEBOX;
        windowStyle &= ~PAL_WINDOW_STYLE_NO_MAXIMIZEBOX;
    }

    *outStyle = windowStyle;
    return PAL_RESULT_SUCCESS;
}

PalResult PAL_CALL palGetWindowMonitor(
    PalWindow* window, 
    PalMonitor** outMonitor) {
    
    if (!s_Video.initialized) {
        return PAL_RESULT_VIDEO_NOT_INITIALIZED;
    }
    
    if (!window || !outMonitor) {
        return PAL_RESULT_NULL_POINTER;
    }

    HMONITOR monitor = MonitorFromWindow((HWND)window, MONITOR_DEFAULTTONEAREST);
    if (!monitor) {
        DWORD error = GetLastError();
        if (error == ERROR_INVALID_HANDLE) {
            return PAL_RESULT_INVALID_WINDOW;

        } else {
            return PAL_RESULT_PLATFORM_FAILURE;
        }
    }

    *outMonitor = (PalMonitor*)monitor;
    return PAL_RESULT_SUCCESS;
}

PalResult PAL_CALL palGetWindowTitle(
    PalWindow* window,
    char** outTitle) {
    
    if (!s_Video.initialized) {
        return PAL_RESULT_VIDEO_NOT_INITIALIZED;
    }
    
    if (!window || !outTitle) {
        return PAL_RESULT_NULL_POINTER;
    }

    wchar_t buffer[WINDOW_NAME_SIZE];
    if (GetWindowTextW((HWND)window, buffer, WINDOW_NAME_SIZE) == 0) {
        return PAL_RESULT_INVALID_WINDOW;
    }

    // convert to UTF-8 encoding string and allocate a string buffer
    int len = WideCharToMultiByte(CP_UTF8, 0, buffer, -1, nullptr, 0, nullptr, nullptr);
    char* string = palAllocate(s_Video.allocator, len + 1, 0);
    if (!string) {
        return PAL_RESULT_OUT_OF_MEMORY;
    }

    WideCharToMultiByte(CP_UTF8, 0, buffer, -1, string, len, nullptr, nullptr);
    *outTitle = string;
    return PAL_RESULT_SUCCESS;
}

PalResult PAL_CALL palGetWindowPos(
    PalWindow* window, 
    Int32* x, 
    Int32* y) {
    
    if (!s_Video.initialized) {
        return PAL_RESULT_VIDEO_NOT_INITIALIZED;
    }
    
    if (!window) {
        return PAL_RESULT_NULL_POINTER;
    }

    RECT rect;
    if (!GetWindowRect((HWND)window, &rect)) {
        return PAL_RESULT_INVALID_WINDOW;
    }

    if (x) {
        *x = rect.left;
    }

    if (y) {
        *y = rect.top;
    }
    return PAL_RESULT_SUCCESS;
}

PalResult PAL_CALL palGetWindowClientPos(
    PalWindow* window, 
    Int32* x, 
    Int32* y) {
    
    if (!s_Video.initialized) {
        return PAL_RESULT_VIDEO_NOT_INITIALIZED;
    }
    
    if (!window) {
        return PAL_RESULT_NULL_POINTER;
    }

    RECT rect;
    if (!GetClientRect((HWND)window, &rect)) {
        return PAL_RESULT_INVALID_WINDOW;
    }

    if (x) {
        *x = rect.left;
    }

    if (y) {
        *y = rect.top;
    }
    return PAL_RESULT_SUCCESS;
}

PalResult PAL_CALL palGetWindowSize(
    PalWindow* window, 
    Uint32* width, 
    Uint32* height) {
    
    if (!s_Video.initialized) {
        return PAL_RESULT_VIDEO_NOT_INITIALIZED;
    }
    
    if (!window) {
        return PAL_RESULT_NULL_POINTER;
    }

    RECT rect;
    if (!GetWindowRect((HWND)window, &rect)) {
        return PAL_RESULT_INVALID_WINDOW;
    }

    if (width) {
        *width = rect.right - rect.left;
    }

    if (height) {
        *height = rect.bottom - rect.top;
    }
    return PAL_RESULT_SUCCESS;
}

PalResult PAL_CALL palGetWindowClientSize(
    PalWindow* window, 
    Uint32* width, 
    Uint32* height) {
    
    if (!s_Video.initialized) {
        return PAL_RESULT_VIDEO_NOT_INITIALIZED;
    }
    
    if (!window) {
        return PAL_RESULT_NULL_POINTER;
    }

    RECT rect;
    if (!GetClientRect((HWND)window, &rect)) {
        return PAL_RESULT_INVALID_WINDOW;
    }

    if (width) {
        *width = rect.right - rect.left;
    }

    if (height) {
        *height = rect.bottom - rect.top;
    }
    return PAL_RESULT_SUCCESS;
}

PalResult PAL_CALL palGetWindowState(
    PalWindow* window, 
    PalWindowState* state) {

    if (!s_Video.initialized) {
        return PAL_RESULT_VIDEO_NOT_INITIALIZED;
    }
    
    if (!window || !state) {
        return PAL_RESULT_NULL_POINTER;
    }

    WINDOWPLACEMENT wp = {};
    if (!GetWindowPlacement((HWND)window, &wp)) {
        return PAL_RESULT_INVALID_WINDOW;
    }

    if (wp.showCmd == SW_SHOWMINIMIZED || wp.showCmd == SW_MINIMIZE) {
        *state = PAL_WINDOW_STATE_MINIMIZED;

    } else if (wp.showCmd == SW_SHOWMAXIMIZED || wp.showCmd == SW_MAXIMIZE) {
        *state = PAL_WINDOW_STATE_MAXIMIZED;

    } else if (wp.showCmd == SW_RESTORE) {
        *state = PAL_WINDOW_STATE_RESTORED;
    }

    return PAL_RESULT_SUCCESS;
}

const bool* PAL_CALL palGetKeycodeState() {

    if (!s_Video.initialized) {
        return nullptr;
    }
    return s_Keyboard.keycodeState;
}

const bool* PAL_CALL palGetScancodeState() {

    if (!s_Video.initialized) {
        return nullptr;
    }
    return s_Keyboard.scancodeState;
}

const bool* PAL_CALL palGetMouseState() {

    if (!s_Video.initialized) {
        return nullptr;
    }
    return s_Mouse.state;
}

void PAL_CALL palGetMouseDelta(
    Int32* dx, 
    Int32* dy) {
    
    if (!s_Video.initialized) {
        return;
    }

    if (dx) {
        *dx = s_Mouse.dx;
    }

    if (dy) {
        *dy = s_Mouse.dy;
    }
}

void PAL_CALL palGetMouseWheelDelta(
    Int32* dx, 
    Int32* dy) {
    
    if (!s_Video.initialized) {
        return;
    }
    
    if (dx) {
        *dx = s_Mouse.WheelX;
    }

    if (dy) {
        *dy = s_Mouse.WheelY;
    }
}

bool PAL_CALL palIsWindowVisible(
    PalWindow* window) {

    if (!s_Video.initialized) {
        return false;
    }
    
    if (!window) {
        return false;
    }

    return IsWindowVisible((HWND)window);
}

PalWindow* PAL_CALL palGetFocusWindow() {

    if (!s_Video.initialized) {
        return nullptr;
    }
    return (PalWindow*)GetForegroundWindow();
}

PalWindowHandleInfo PAL_CALL palGetWindowHandleInfo(
    PalWindow* window) {
    
    PalWindowHandleInfo handle = {0};
    if (s_Video.initialized && window) {
        handle.nativeDisplay = nullptr;
        handle.nativeWindow = (void*)window;
    }
    return handle;
}

PalResult PAL_CALL palSetWindowOpacity(
    PalWindow* window,
    float opacity) {
    
    if (!s_Video.initialized) {
        return PAL_RESULT_VIDEO_NOT_INITIALIZED;
    }
    
    if (!window) {
        return PAL_RESULT_NULL_POINTER;
    }

    if (!SetLayeredWindowAttributes((HWND)window, 0, (BYTE)(opacity * 255), LWA_ALPHA)) {
        DWORD error = GetLastError();
        if (error == ERROR_INVALID_HANDLE) {
            return PAL_RESULT_INVALID_WINDOW;

        } else if (error == ERROR_INVALID_PARAMETER) {
            return PAL_RESULT_INVALID_PARAMETER;

        } else {
            // FIXME: check for child windows
            return PAL_RESULT_PLATFORM_FAILURE;
        }
    }

    return PAL_RESULT_SUCCESS;
}

PalResult PAL_CALL palSetWindowStyle(
    PalWindow* window,
    PalWindowStyle style) {
    
    if (!s_Video.initialized) {
        return PAL_RESULT_VIDEO_NOT_INITIALIZED;
    }

    if (!window) {
        return PAL_RESULT_NULL_POINTER;
    }

    // convert our style to win32 styles and exStyles
    DWORD win32Style = WS_CAPTION | WS_SYSMENU | WS_OVERLAPPED; // all windows have this styles
    DWORD exStyle = 0;

    // check for resizing
    if (style & PAL_WINDOW_STYLE_RESIZABLE) {
        win32Style |= WS_THICKFRAME;
    }

    // check for transparent window
    if (style & PAL_WINDOW_STYLE_TRANSPARENT) {
        exStyle |= WS_EX_LAYERED;
    }

    // check for topmost window
    if (style & PAL_WINDOW_STYLE_TOPMOST) {
        exStyle |= WS_EX_TOPMOST;
    }

    // check for no minimize box
    if (style & PAL_WINDOW_STYLE_NO_MINIMIZEBOX) {
        win32Style &= ~WS_MINIMIZEBOX;
    }

    // check for maximize box
    if (style & PAL_WINDOW_STYLE_NO_MAXIMIZEBOX) {
        win32Style &= ~WS_MAXIMIZEBOX;
    }

    // check for tool window
    if (style & PAL_WINDOW_STYLE_TOOL) {
        exStyle |= WS_EX_TOOLWINDOW;
    }

    // check for borderless window
    if (style & PAL_WINDOW_STYLE_BORDERLESS) {
        // revert the styles
        win32Style = WS_POPUP;
        exStyle = WS_EX_APPWINDOW;
    }

    HWND hwnd = (HWND)window;
    SetWindowLongPtrW(hwnd, GWL_STYLE, win32Style);
    SetWindowLongPtrW(hwnd, GWL_EXSTYLE, exStyle);

    // force a frame update
    bool success = SetWindowPos(
        hwnd,
        nullptr,
        0, 
        0,
        0,
        0,
        SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED
    );

    if (success) {
        return PAL_RESULT_SUCCESS;

    } else {
        DWORD error = GetLastError();
        if (error == ERROR_INVALID_HANDLE) {
            return PAL_RESULT_INVALID_WINDOW;

        } else {
            return PAL_RESULT_PLATFORM_FAILURE;
        }
    }
}

PalResult PAL_CALL palSetWindowTitle(
    PalWindow* window, 
    const char* title) {
    
    if (!s_Video.initialized) {
        return PAL_RESULT_VIDEO_NOT_INITIALIZED;
    }

    if (!window || !title) {
        return PAL_RESULT_NULL_POINTER;
    }
    
    wchar_t buffer[WINDOW_NAME_SIZE];
    MultiByteToWideChar(CP_UTF8, 0, title, -1, buffer, 256);

    if (!SetWindowTextW((HWND)window, buffer)) {
        return PAL_RESULT_INVALID_WINDOW;
    }

    return PAL_RESULT_SUCCESS;
}

PalResult PAL_CALL palSetWindowPos(
    PalWindow* window, 
    Int32 x, 
    Int32 y) {
    
    if (!s_Video.initialized) {
        return PAL_RESULT_VIDEO_NOT_INITIALIZED;
    }

    if (!window) {
        return PAL_RESULT_NULL_POINTER;
    }
    
    bool success = SetWindowPos(
        (HWND)window, 
        nullptr, 
        x, 
        y, 
        0, 
        0, 
        SWP_NOACTIVATE | SWP_NOZORDER | SWP_NOSIZE
    );

    if (!success) {
        DWORD error = GetLastError();
        if (error == ERROR_INVALID_HANDLE) {
            return PAL_RESULT_INVALID_WINDOW;

        } else {
            return PAL_RESULT_PLATFORM_FAILURE;
        }
    }
    return PAL_RESULT_SUCCESS;
}

PalResult PAL_CALL palSetWindowSize(
    PalWindow* window, 
    Uint32 width, 
    Uint32 height) {
    
    if (!s_Video.initialized) {
        return PAL_RESULT_VIDEO_NOT_INITIALIZED;
    }

    if (!window) {
        return PAL_RESULT_NULL_POINTER;
    }

    bool success = SetWindowPos(
        (HWND)window,
        HWND_TOP, 
        0, 
        0, 
        width, 
        height,
        SWP_NOACTIVATE | SWP_NOOWNERZORDER | SWP_NOMOVE | SWP_NOZORDER
    );

    if (!success) {
        DWORD error = GetLastError();
        if (error == ERROR_INVALID_HANDLE) {
            return PAL_RESULT_INVALID_WINDOW;

        } else if (error == ERROR_INVALID_PARAMETER) {
            return PAL_RESULT_INVALID_PARAMETER;

        } else {
            return PAL_RESULT_PLATFORM_FAILURE;
        }
    }
    return PAL_RESULT_SUCCESS;
}

PalResult PAL_CALL palSetFocusWindow(
    PalWindow* window) {
    
    if (!s_Video.initialized) {
        return PAL_RESULT_VIDEO_NOT_INITIALIZED;
    }

    if (!window) {
        return PAL_RESULT_NULL_POINTER;
    }

    if (!SetForegroundWindow((HWND)window)) {
        DWORD error = GetLastError();
        if (error == ERROR_INVALID_HANDLE) {
            return PAL_RESULT_INVALID_WINDOW;

        } else if (error == ERROR_ACCESS_DENIED) {
            return PAL_RESULT_ACCESS_DENIED;

        } else {
            return PAL_RESULT_PLATFORM_FAILURE;
        }
    }
    return PAL_RESULT_SUCCESS;
}

// ==================================================
// Icon
// ==================================================

PalResult PAL_CALL palCreateIcon(
    const PalIconCreateInfo* info,
    PalIcon** outIcon) {
    
    if (!s_Video.initialized) {
        return PAL_RESULT_VIDEO_NOT_INITIALIZED;
    }

    if (!info || !outIcon) {
        return PAL_RESULT_NULL_POINTER;
    }

    // describe the icon pixels
    BITMAPV5HEADER bitInfo = {};
    bitInfo.bV5Size = sizeof(BITMAPV5HEADER);
    bitInfo.bV5Width = info->width;
    bitInfo.bV5Height = -(Int32)info->height; // this is topdown by default

    // default parameters
    bitInfo.bV5Planes = 1;
    bitInfo.bV5BitCount = 32; // PAL supports 32 bits
    bitInfo.bV5Compression = BI_BITFIELDS;
    bitInfo.bV5RedMask =  0x00FF0000;
    bitInfo.bV5GreenMask = 0x0000FF00;
    bitInfo.bV5BlueMask = 0x000000FF;
    bitInfo.bV5AlphaMask = 0xFF000000;

    HDC hdc = GetDC(nullptr); 
    void* dibPixels = nullptr;

    // create dib section
    HBITMAP bitmap = s_Video.createDIBSection(
        hdc,
        (BITMAPINFO*)&bitInfo,
        DIB_RGB_COLORS,
        &dibPixels,
        nullptr,
        0
    );

    if (!bitmap) {
        ReleaseDC(nullptr, hdc);
        return PAL_RESULT_PLATFORM_FAILURE;
    }
    ReleaseDC(nullptr, hdc);

    // convert RGBA to BGRA
    Uint8* pixels = (Uint8*)dibPixels;
    for (int y = 0; y < info->height; ++y) {
        for (int x = 0; x < info->width; ++x) {
            int i = (y * info->width + x) * 4;
            pixels[i + 0] = info->pixels[i + 2]; // Red
            pixels[i + 1] = info->pixels[i + 1]; // Green
            pixels[i + 2] = info->pixels[i + 0]; // Nlue
            pixels[i + 3] = info->pixels[i + 3]; // Alpha
        }
    }

    // create mask
    HBITMAP mask = s_Video.createBitmap(info->width, info->height, 1, 1, nullptr);
    if (!mask) {
        s_Video.deleteObject(bitmap);
        return PAL_RESULT_PLATFORM_FAILURE;
    }

    ICONINFO iconInfo = {};
    iconInfo.fIcon = TRUE;
    iconInfo.hbmMask = mask;
    iconInfo.hbmColor = bitmap;

    // create the icon with the icon info
    HICON icon = CreateIconIndirect(&iconInfo);
    if (!icon) {
        s_Video.deleteObject(mask);
        s_Video.deleteObject(bitmap);
        return PAL_RESULT_PLATFORM_FAILURE;
    }

    s_Video.deleteObject(mask);
    s_Video.deleteObject(bitmap);
    *outIcon = (PalIcon*)icon;
    return PAL_RESULT_SUCCESS;
}

void PAL_CALL palDestroyIcon(
    PalIcon* icon) {
    
    if (s_Video.initialized && icon) {
        DestroyIcon((HICON)icon);
    }
}

PalResult PAL_CALL palSetWindowIcon(
    PalWindow* window,
    PalIcon* icon) {
    
    if (!s_Video.initialized) {
        return PAL_RESULT_VIDEO_NOT_INITIALIZED;
    }

    if (!window) {
        return PAL_RESULT_NULL_POINTER;
    }

    if (!IsWindow((HWND)window)) {
        return PAL_RESULT_INVALID_WINDOW;
    }

    SendMessageW((HWND)window, WM_SETICON, ICON_BIG, (LPARAM)icon);
    SendMessageW((HWND)window, WM_SETICON, ICON_SMALL, (LPARAM)icon);
    return PAL_RESULT_SUCCESS;
}

// ==================================================
// Cursor
// ==================================================

PalResult PAL_CALL palCreateCursor(
    const PalCursorCreateInfo* info,
    PalCursor** outCursor) {
    
    if (!s_Video.initialized) {
        return PAL_RESULT_VIDEO_NOT_INITIALIZED;
    }

    if (!info || !outCursor) {
        return PAL_RESULT_NULL_POINTER;
    }

    // describe the icon pixels
    BITMAPV5HEADER bitInfo = {};
    bitInfo.bV5Size = sizeof(BITMAPV5HEADER);
    bitInfo.bV5Width = info->width;
    bitInfo.bV5Height = -(Int32)info->height; // this is topdown by default

    // default parameters
    bitInfo.bV5Planes = 1;
    bitInfo.bV5BitCount = 32; // PAL supports 32 bits
    bitInfo.bV5Compression = BI_BITFIELDS;
    bitInfo.bV5RedMask =  0x00FF0000;
    bitInfo.bV5GreenMask = 0x0000FF00;
    bitInfo.bV5BlueMask = 0x000000FF;
    bitInfo.bV5AlphaMask = 0xFF000000;

    HDC hdc = GetDC(nullptr); 
    void* dibPixels = nullptr;

    // create dib section
    HBITMAP bitmap = s_Video.createDIBSection(
        hdc,
        (BITMAPINFO*)&bitInfo,
        DIB_RGB_COLORS,
        &dibPixels,
        nullptr,
        0
    );

    if (!bitmap) {
        ReleaseDC(nullptr, hdc);
        return PAL_RESULT_PLATFORM_FAILURE;
    }
    ReleaseDC(nullptr, hdc);

    // copy pixels and create mask
    memcpy(dibPixels, info->pixels, info->width * info->height * 4);
    HBITMAP mask = s_Video.createBitmap(
        info->width, 
        info->height, 
        1, 
        1, 
        NULL
    );

    ICONINFO iconInfo = {0};
    iconInfo.fIcon = false;
    iconInfo.hbmColor = bitmap;
    iconInfo.hbmMask = mask;
    iconInfo.xHotspot = info->xHotspot;
    iconInfo.xHotspot = info->yHotspot;

    // create the cursor with the iconinfo
    HCURSOR cursor = CreateIconIndirect(&iconInfo);
    if (!cursor) {
        s_Video.deleteObject(mask);
        s_Video.deleteObject(bitmap);
        return PAL_RESULT_PLATFORM_FAILURE;
    }

    s_Video.deleteObject(mask);
    s_Video.deleteObject(bitmap);
    *outCursor = (PalCursor*)cursor;
    return PAL_RESULT_SUCCESS;
}

void PAL_CALL palDestroyCursor(
    PalCursor* cursor) {

    if (s_Video.initialized && cursor) {
        DestroyCursor((HCURSOR)cursor);
    }
}

void PAL_CALL palShowCursor(
    bool show) {
    
    if (s_Video.initialized) {
        ShowCursor(show);
    }
}

void PAL_CALL palClipCursor(
    PalWindow* window,
    bool clip) {
    
    if (!s_Video.initialized || !window) {
        return;
    }

    if (clip) {
        RECT rect;
        if (GetClientRect((HWND)window, &rect)) {
            return;
        }

        POINT tmp = { rect.left, rect.top };
        POINT tmp2 = { rect.right, rect.bottom };

        ClientToScreen((HWND)window, &tmp);
        ClientToScreen((HWND)window, &tmp2);

        RECT clipRect = { tmp.x, tmp.y, tmp2.x, tmp2.y };
        ClipCursor(&clipRect);

    } else {
        ClipCursor(nullptr);
    }
}

void PAL_CALL palGetCursorPos(
    PalWindow* window,
    Int32* x,
    Int32* y) {
    
    if (!s_Video.initialized || !window) {
        return;
    }

    POINT pos;
    if (GetCursorPos(&pos)) {
        if (ScreenToClient((HWND)window, &pos)) {
            if (x) {
                *x = pos.x;
            }

            if (y) {
                *y = pos.y;
            }
        }
    }
}

void PAL_CALL palSetCursorPos(
    PalWindow* window,
    Int32 x,
    Int32 y) {
    
    if (!s_Video.initialized || !window) {
        return;
    }

    POINT pos = { x, y };
    ClientToScreen((HWND)window, &pos);
    SetCursorPos(pos.x, pos.y);
}

void PAL_CALL palSetWindowCursor(
    PalWindow* window,
    PalCursor* cursor) {
    
    if (window || cursor) {
        SetWindowLongPtrW((HWND)window, GWLP_USERDATA, (LONG_PTR)cursor);
    }
}