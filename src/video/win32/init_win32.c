/**
 * Copyright (C) 2025-2026 Nicholas Agbo <agbonicholas04@gmail.com>
 *
 * This software is provided 'as-is', without any express or implied
 * warranty.  In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 *
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 *
 * 3. This notice may not be removed or altered from any source distribution.
 */

#ifdef _WIN32

#include "video/platform.h"
#include "win32_helper.h"
#include "pal2/video/init.h"

#if PAL_HAS_EVENT_MODULE
#include "pal2/core/pack.h"
#include "pal2/event/driver.h"
#endif // PAL_HAS_EVENT_MODULE

#include <windowsx.h>

#define PROCESS_DPI_AWARE 2
#define NULL_BUTTON_SERIAL 0xffffffffU
#define VIDEO_CLASS L"PALVideoClass"
#define VIDEO_PROP L"PALVideoData"

typedef struct PendingEvent {
    PalBool pendingResize;
    PalBool pendingMove;
    PalBool pendingState;
    uint32_t width;
    uint32_t height;
    int32_t x;
    int32_t y;
    PalWindowState state;
    PalWindow* window;
} PendingEvent;

typedef struct Win32Keyboard {
    int32_t pendingHighSurrogate;
    int scancodes[512];
    int keycodes[256];
} Win32Keyboard;

Win32 g_Win32 = {0};
static PalVideoDriver s_Win32Driver = {0};
static PendingEvent s_Event = {0};
static BYTE s_RawBuffer[4096] = {0};
static Win32Keyboard s_Win32Keyboard = {0};

/* =========================================================================
 * Helper
 * ========================================================================= */

static void loadProcs(PalBool loadAll)
{
    // clang-format off

    g_Win32.getDpiForMonitor =(GetDpiForMonitorFn)palGetSymbol(
        g_Win32.shcore, 
        "GetDpiForMonitor");

    g_Win32.setProcessAwareness = (SetProcessAwarenessFn)palGetSymbol(
        g_Win32.shcore, 
        "SetProcessDpiAwareness");

    if (!loadAll) {
        return;
    }

    g_Win32.createDIBSection = (CreateDIBSectionFn)palGetSymbol(
        g_Win32.gdi,
        "CreateDIBSection");

    g_Win32.createBitmap = (CreateBitmapFn)palGetSymbol(
        g_Win32.gdi,
        "CreateBitmap");

    g_Win32.deleteObject = (DeleteObjectFn)palGetSymbol(
        g_Win32.gdi,
        "DeleteObject");

    g_Win32.describePixelFormat = (DescribePixelFormatFn)palGetSymbol(
        g_Win32.gdi,
        "DescribePixelFormat");

    g_Win32.setPixelFormat = (SetPixelFormatFn)palGetSymbol(
        g_Win32.gdi,
        "SetPixelFormat");

    // clang-format on
}

static void checkFeatures(void)
{
    PalVideoFeatures features = 0;

    s_Win32Driver.features |= PAL_VIDEO_FEATURE_MONITOR_SET_ORIENTATION;
    s_Win32Driver.features |= PAL_VIDEO_FEATURE_MONITOR_GET_ORIENTATION;
    s_Win32Driver.features |= PAL_VIDEO_FEATURE_BORDERLESS_WINDOW;
    s_Win32Driver.features |= PAL_VIDEO_FEATURE_TRANSPARENT_WINDOW;
    s_Win32Driver.features |= PAL_VIDEO_FEATURE_TOOL_WINDOW;
    s_Win32Driver.features |= PAL_VIDEO_FEATURE_MONITOR_SET_MODE;
    s_Win32Driver.features |= PAL_VIDEO_FEATURE_MONITOR_GET_MODE;
    s_Win32Driver.features |= PAL_VIDEO_FEATURE_MULTI_MONITORS;
    s_Win32Driver.features |= PAL_VIDEO_FEATURE_WINDOW_SET_SIZE;
    s_Win32Driver.features |= PAL_VIDEO_FEATURE_WINDOW_GET_SIZE;
    s_Win32Driver.features |= PAL_VIDEO_FEATURE_WINDOW_SET_POS;
    s_Win32Driver.features |= PAL_VIDEO_FEATURE_WINDOW_GET_POS;
    s_Win32Driver.features |= PAL_VIDEO_FEATURE_WINDOW_SET_STATE;
    s_Win32Driver.features |= PAL_VIDEO_FEATURE_WINDOW_GET_STATE;
    s_Win32Driver.features |= PAL_VIDEO_FEATURE_WINDOW_SET_VISIBILITY;
    s_Win32Driver.features |= PAL_VIDEO_FEATURE_WINDOW_GET_VISIBILITY;
    s_Win32Driver.features |= PAL_VIDEO_FEATURE_WINDOW_SET_TITLE;
    s_Win32Driver.features |= PAL_VIDEO_FEATURE_WINDOW_GET_TITLE;
    s_Win32Driver.features |= PAL_VIDEO_FEATURE_NO_MAXIMIZEBOX;
    s_Win32Driver.features |= PAL_VIDEO_FEATURE_NO_MINIMIZEBOX;
    s_Win32Driver.features |= PAL_VIDEO_FEATURE_CLIP_CURSOR;
    s_Win32Driver.features |= PAL_VIDEO_FEATURE_WINDOW_FLASH_CAPTION;
    s_Win32Driver.features |= PAL_VIDEO_FEATURE_WINDOW_FLASH_TRAY;
    s_Win32Driver.features |= PAL_VIDEO_FEATURE_WINDOW_FLASH_INTERVAL;
    s_Win32Driver.features |= PAL_VIDEO_FEATURE_WINDOW_SET_INPUT_FOCUS;
    s_Win32Driver.features |= PAL_VIDEO_FEATURE_WINDOW_GET_INPUT_FOCUS;
    s_Win32Driver.features |= PAL_VIDEO_FEATURE_WINDOW_SET_STYLE;
    s_Win32Driver.features |= PAL_VIDEO_FEATURE_WINDOW_GET_STYLE;
    s_Win32Driver.features |= PAL_VIDEO_FEATURE_CURSOR_SET_POS;
    s_Win32Driver.features |= PAL_VIDEO_FEATURE_CURSOR_GET_POS;
    s_Win32Driver.features |= PAL_VIDEO_FEATURE_WINDOW_SET_ICON;

    s_Win32Driver.features |= PAL_VIDEO_FEATURE_TOPMOST_WINDOW;
    s_Win32Driver.features |= PAL_VIDEO_FEATURE_DECORATED_WINDOW;
    s_Win32Driver.features |= PAL_VIDEO_FEATURE_CURSOR_SET_VISIBILITY;
    s_Win32Driver.features |= PAL_VIDEO_FEATURE_WINDOW_GET_MONITOR;
    s_Win32Driver.features |= PAL_VIDEO_FEATURE_MONITOR_GET_PRIMARY;
    s_Win32Driver.features |= PAL_VIDEO_FEATURE_FOREIGN_WINDOWS;
    s_Win32Driver.features |= PAL_VIDEO_FEATURE_MONITOR_VALIDATE_MODE;
    s_Win32Driver.features |= PAL_VIDEO_FEATURE_WINDOW_SET_CURSOR;

    if (g_Win32.getDpiForMonitor && g_Win32.setProcessAwareness) {
        features |= PAL_VIDEO_FEATURE_HIGH_DPI;
        g_Win32.setProcessAwareness(PROCESS_DPI_AWARE);
    }

    s_Win32Driver.features = features;
}

static void createKeycodeTable(void)
{
    // Letters
    s_Win32Keyboard.keycodes['A'] = PAL_KEYCODE_A;
    s_Win32Keyboard.keycodes['B'] = PAL_KEYCODE_B;
    s_Win32Keyboard.keycodes['C'] = PAL_KEYCODE_C;
    s_Win32Keyboard.keycodes['D'] = PAL_KEYCODE_D;
    s_Win32Keyboard.keycodes['E'] = PAL_KEYCODE_E;
    s_Win32Keyboard.keycodes['F'] = PAL_KEYCODE_F;
    s_Win32Keyboard.keycodes['G'] = PAL_KEYCODE_G;
    s_Win32Keyboard.keycodes['H'] = PAL_KEYCODE_H;
    s_Win32Keyboard.keycodes['I'] = PAL_KEYCODE_I;
    s_Win32Keyboard.keycodes['J'] = PAL_KEYCODE_J;
    s_Win32Keyboard.keycodes['K'] = PAL_KEYCODE_K;
    s_Win32Keyboard.keycodes['L'] = PAL_KEYCODE_L;
    s_Win32Keyboard.keycodes['M'] = PAL_KEYCODE_M;
    s_Win32Keyboard.keycodes['N'] = PAL_KEYCODE_N;
    s_Win32Keyboard.keycodes['O'] = PAL_KEYCODE_O;
    s_Win32Keyboard.keycodes['P'] = PAL_KEYCODE_P;
    s_Win32Keyboard.keycodes['Q'] = PAL_KEYCODE_Q;
    s_Win32Keyboard.keycodes['R'] = PAL_KEYCODE_R;
    s_Win32Keyboard.keycodes['S'] = PAL_KEYCODE_S;
    s_Win32Keyboard.keycodes['T'] = PAL_KEYCODE_T;
    s_Win32Keyboard.keycodes['U'] = PAL_KEYCODE_U;
    s_Win32Keyboard.keycodes['V'] = PAL_KEYCODE_V;
    s_Win32Keyboard.keycodes['W'] = PAL_KEYCODE_W;
    s_Win32Keyboard.keycodes['X'] = PAL_KEYCODE_X;
    s_Win32Keyboard.keycodes['Y'] = PAL_KEYCODE_Y;
    s_Win32Keyboard.keycodes['Z'] = PAL_KEYCODE_Z;

    // Control
    s_Win32Keyboard.keycodes[VK_SPACE] = PAL_KEYCODE_SPACE;

    // Misc
    s_Win32Keyboard.keycodes[VK_OEM_7] = PAL_KEYCODE_APOSTROPHE;
    s_Win32Keyboard.keycodes[VK_OEM_5] = PAL_KEYCODE_BACKSLASH;
    s_Win32Keyboard.keycodes[VK_OEM_COMMA] = PAL_KEYCODE_COMMA;
    s_Win32Keyboard.keycodes[VK_OEM_PLUS] = PAL_KEYCODE_EQUAL;
    s_Win32Keyboard.keycodes[VK_OEM_3] = PAL_KEYCODE_GRAVEACCENT;
    s_Win32Keyboard.keycodes[VK_OEM_MINUS] = PAL_KEYCODE_SUBTRACT;
    s_Win32Keyboard.keycodes[VK_OEM_PERIOD] = PAL_KEYCODE_PERIOD;
    s_Win32Keyboard.keycodes[VK_OEM_1] = PAL_KEYCODE_SEMICOLON;
    s_Win32Keyboard.keycodes[VK_OEM_2] = PAL_KEYCODE_SLASH;
    s_Win32Keyboard.keycodes[VK_OEM_4] = PAL_KEYCODE_LBRACKET;
    s_Win32Keyboard.keycodes[VK_OEM_6] = PAL_KEYCODE_RBRACKET;
}

static void createScancodeTable(void)
{
    // Scancodes are made from OR'ed (scancode | extended)
    // Letters
    s_Win32Keyboard.scancodes[0x01E] = PAL_SCANCODE_A;
    s_Win32Keyboard.scancodes[0x030] = PAL_SCANCODE_B;
    s_Win32Keyboard.scancodes[0x02E] = PAL_SCANCODE_C;
    s_Win32Keyboard.scancodes[0x020] = PAL_SCANCODE_D;
    s_Win32Keyboard.scancodes[0x012] = PAL_SCANCODE_E;
    s_Win32Keyboard.scancodes[0x021] = PAL_SCANCODE_F;
    s_Win32Keyboard.scancodes[0x022] = PAL_SCANCODE_G;
    s_Win32Keyboard.scancodes[0x023] = PAL_SCANCODE_H;
    s_Win32Keyboard.scancodes[0x017] = PAL_SCANCODE_I;
    s_Win32Keyboard.scancodes[0x024] = PAL_SCANCODE_J;
    s_Win32Keyboard.scancodes[0x025] = PAL_SCANCODE_K;
    s_Win32Keyboard.scancodes[0x026] = PAL_SCANCODE_L;
    s_Win32Keyboard.scancodes[0x032] = PAL_SCANCODE_M;
    s_Win32Keyboard.scancodes[0x031] = PAL_SCANCODE_N;
    s_Win32Keyboard.scancodes[0x018] = PAL_SCANCODE_O;
    s_Win32Keyboard.scancodes[0x019] = PAL_SCANCODE_P;
    s_Win32Keyboard.scancodes[0x010] = PAL_SCANCODE_Q;
    s_Win32Keyboard.scancodes[0x013] = PAL_SCANCODE_R;
    s_Win32Keyboard.scancodes[0x01F] = PAL_SCANCODE_S;
    s_Win32Keyboard.scancodes[0x014] = PAL_SCANCODE_T;
    s_Win32Keyboard.scancodes[0x016] = PAL_SCANCODE_U;
    s_Win32Keyboard.scancodes[0x02F] = PAL_SCANCODE_V;
    s_Win32Keyboard.scancodes[0x011] = PAL_SCANCODE_W;
    s_Win32Keyboard.scancodes[0x02D] = PAL_SCANCODE_X;
    s_Win32Keyboard.scancodes[0x015] = PAL_SCANCODE_Y;
    s_Win32Keyboard.scancodes[0x02C] = PAL_SCANCODE_Z;

    // Numbers (top row)
    s_Win32Keyboard.scancodes[0x00B] = PAL_SCANCODE_0;
    s_Win32Keyboard.scancodes[0x002] = PAL_SCANCODE_1;
    s_Win32Keyboard.scancodes[0x003] = PAL_SCANCODE_2;
    s_Win32Keyboard.scancodes[0x004] = PAL_SCANCODE_3;
    s_Win32Keyboard.scancodes[0x005] = PAL_SCANCODE_4;
    s_Win32Keyboard.scancodes[0x006] = PAL_SCANCODE_5;
    s_Win32Keyboard.scancodes[0x007] = PAL_SCANCODE_6;
    s_Win32Keyboard.scancodes[0x008] = PAL_SCANCODE_7;
    s_Win32Keyboard.scancodes[0x009] = PAL_SCANCODE_8;
    s_Win32Keyboard.scancodes[0x00A] = PAL_SCANCODE_9;

    // Function
    s_Win32Keyboard.scancodes[0x03B] = PAL_SCANCODE_F1;
    s_Win32Keyboard.scancodes[0x03C] = PAL_SCANCODE_F2;
    s_Win32Keyboard.scancodes[0x03D] = PAL_SCANCODE_F3;
    s_Win32Keyboard.scancodes[0x03E] = PAL_SCANCODE_F4;
    s_Win32Keyboard.scancodes[0x03F] = PAL_SCANCODE_F5;
    s_Win32Keyboard.scancodes[0x040] = PAL_SCANCODE_F6;
    s_Win32Keyboard.scancodes[0x041] = PAL_SCANCODE_F7;
    s_Win32Keyboard.scancodes[0x042] = PAL_SCANCODE_F8;
    s_Win32Keyboard.scancodes[0x043] = PAL_SCANCODE_F9;
    s_Win32Keyboard.scancodes[0x044] = PAL_SCANCODE_F10;
    s_Win32Keyboard.scancodes[0x057] = PAL_SCANCODE_F11;
    s_Win32Keyboard.scancodes[0x058] = PAL_SCANCODE_F12;

    // Control
    s_Win32Keyboard.scancodes[0x001] = PAL_SCANCODE_ESCAPE;
    s_Win32Keyboard.scancodes[0x01C] = PAL_SCANCODE_ENTER;
    s_Win32Keyboard.scancodes[0x00F] = PAL_SCANCODE_TAB;
    s_Win32Keyboard.scancodes[0x00E] = PAL_SCANCODE_BACKSPACE;
    s_Win32Keyboard.scancodes[0x039] = PAL_SCANCODE_SPACE;
    s_Win32Keyboard.scancodes[0x03A] = PAL_SCANCODE_CAPSLOCK;
    s_Win32Keyboard.scancodes[0x145] = PAL_SCANCODE_NUMLOCK;
    s_Win32Keyboard.scancodes[0x046] = PAL_SCANCODE_SCROLLLOCK;
    s_Win32Keyboard.scancodes[0x02A] = PAL_SCANCODE_LSHIFT;
    s_Win32Keyboard.scancodes[0x036] = PAL_SCANCODE_RSHIFT;
    s_Win32Keyboard.scancodes[0x01D] = PAL_SCANCODE_LCTRL;
    s_Win32Keyboard.scancodes[0x11D] = PAL_SCANCODE_RCTRL;
    s_Win32Keyboard.scancodes[0x038] = PAL_SCANCODE_LALT;
    s_Win32Keyboard.scancodes[0x138] = PAL_SCANCODE_RALT;

    // Arrows
    s_Win32Keyboard.scancodes[0x14B] = PAL_SCANCODE_LEFT;
    s_Win32Keyboard.scancodes[0x14D] = PAL_SCANCODE_RIGHT;
    s_Win32Keyboard.scancodes[0x148] = PAL_SCANCODE_UP;
    s_Win32Keyboard.scancodes[0x150] = PAL_SCANCODE_DOWN;

    // Navigation
    s_Win32Keyboard.scancodes[0x152] = PAL_SCANCODE_INSERT;
    s_Win32Keyboard.scancodes[0x153] = PAL_SCANCODE_DELETE;
    s_Win32Keyboard.scancodes[0x147] = PAL_SCANCODE_HOME;
    s_Win32Keyboard.scancodes[0x14F] = PAL_SCANCODE_END;
    s_Win32Keyboard.scancodes[0x149] = PAL_SCANCODE_PAGEUP;
    s_Win32Keyboard.scancodes[0x151] = PAL_SCANCODE_PAGEDOWN;

    // Keypad
    s_Win32Keyboard.scancodes[0x052] = PAL_SCANCODE_KP_0;
    s_Win32Keyboard.scancodes[0x04F] = PAL_SCANCODE_KP_1;
    s_Win32Keyboard.scancodes[0x050] = PAL_SCANCODE_KP_2;
    s_Win32Keyboard.scancodes[0x051] = PAL_SCANCODE_KP_3;
    s_Win32Keyboard.scancodes[0x04B] = PAL_SCANCODE_KP_4;
    s_Win32Keyboard.scancodes[0x04C] = PAL_SCANCODE_KP_5;
    s_Win32Keyboard.scancodes[0x04D] = PAL_SCANCODE_KP_6;
    s_Win32Keyboard.scancodes[0x047] = PAL_SCANCODE_KP_7;
    s_Win32Keyboard.scancodes[0x048] = PAL_SCANCODE_KP_8;
    s_Win32Keyboard.scancodes[0x049] = PAL_SCANCODE_KP_9;
    s_Win32Keyboard.scancodes[0x11C] = PAL_SCANCODE_KP_ENTER;
    s_Win32Keyboard.scancodes[0x04E] = PAL_SCANCODE_KP_ADD;
    s_Win32Keyboard.scancodes[0x04A] = PAL_SCANCODE_KP_SUBTRACT;
    s_Win32Keyboard.scancodes[0x037] = PAL_SCANCODE_KP_MULTIPLY;
    s_Win32Keyboard.scancodes[0x135] = PAL_SCANCODE_KP_DIVIDE;
    s_Win32Keyboard.scancodes[0x053] = PAL_SCANCODE_KP_DECIMAL;
    s_Win32Keyboard.scancodes[0x059] = PAL_SCANCODE_KP_EQUAL;

    // Misc
    s_Win32Keyboard.scancodes[0x137] = PAL_SCANCODE_PRINTSCREEN;
    s_Win32Keyboard.scancodes[0x146] = PAL_SCANCODE_PAUSE;
    s_Win32Keyboard.scancodes[0x045] = PAL_SCANCODE_PAUSE;
    s_Win32Keyboard.scancodes[0x15D] = PAL_SCANCODE_MENU;
    s_Win32Keyboard.scancodes[0x028] = PAL_SCANCODE_APOSTROPHE;
    s_Win32Keyboard.scancodes[0x02B] = PAL_SCANCODE_BACKSLASH;
    s_Win32Keyboard.scancodes[0x033] = PAL_SCANCODE_COMMA;
    s_Win32Keyboard.scancodes[0x00D] = PAL_SCANCODE_EQUAL;
    s_Win32Keyboard.scancodes[0x029] = PAL_SCANCODE_GRAVEACCENT;
    s_Win32Keyboard.scancodes[0x00C] = PAL_SCANCODE_SUBTRACT;
    s_Win32Keyboard.scancodes[0x034] = PAL_SCANCODE_PERIOD;
    s_Win32Keyboard.scancodes[0x027] = PAL_SCANCODE_SEMICOLON;
    s_Win32Keyboard.scancodes[0x035] = PAL_SCANCODE_SLASH;
    s_Win32Keyboard.scancodes[0x01A] = PAL_SCANCODE_LBRACKET;
    s_Win32Keyboard.scancodes[0x01B] = PAL_SCANCODE_RBRACKET;
    s_Win32Keyboard.scancodes[0x15B] = PAL_SCANCODE_LSUPER;
    s_Win32Keyboard.scancodes[0x15C] = PAL_SCANCODE_RSUPER;
}

LRESULT CALLBACK videoProc(
    HWND hwnd,
    UINT msg,
    WPARAM wParam,
    LPARAM lParam)
{
    WindowData* data = (WindowData*)GetPropW(hwnd, VIDEO_PROP);
    if (!data) {
        return DefWindowProcW(hwnd, msg, wParam, lParam);
    }

    PalDispatchMode mode = PAL_DISPATCH_MODE_NONE;
    switch (msg) {
        case WM_CLOSE: {
#if PAL_HAS_EVENT_MODULE
            if (g_Win32.eventDriver) {
                PalEventDriver* driver = g_Win32.eventDriver;
                mode = palGetEventDispatchMode(driver, PAL_EVENT_TYPE_WINDOW_CLOSE);
                if (mode != PAL_DISPATCH_MODE_NONE) {
                    PalEvent event = {0};
                    event.type = PAL_EVENT_TYPE_WINDOW_CLOSE;
                    event.data2 = palPackPointer((PalWindow*)hwnd);
                    palPushEvent(driver, &event);
                }
            }
#endif // PAL_HAS_EVENT_MODULE

            return 0;
        }

        case WM_SIZE: {
#if PAL_HAS_EVENT_MODULE
            if (g_Win32.eventDriver) {
                PalEventDriver* driver = g_Win32.eventDriver;
                mode = palGetEventDispatchMode(driver, PAL_EVENT_TYPE_WINDOW_SIZE);
                uint32_t width = (uint32_t)LOWORD(lParam);
                uint32_t height = (uint32_t)HIWORD(lParam);

                if (mode == PAL_DISPATCH_MODE_CALLBACK) {
                    PalEvent event = {0};
                    event.type = PAL_EVENT_TYPE_WINDOW_SIZE;
                    event.data = palPackUint32(width, height);
                    event.data2 = palPackPointer((PalWindow*)hwnd);
                    palPushEvent(driver, &event);

                } else if (mode == PAL_DISPATCH_MODE_POLL) {
                    s_Event.pendingResize = PAL_TRUE;
                    s_Event.width = width;
                    s_Event.height = height;
                    s_Event.window = (PalWindow*)hwnd;
                }

                // trigger state event
                mode = palGetEventDispatchMode(driver, PAL_EVENT_TYPE_WINDOW_STATE);
                PalWindowState state = PAL_WINDOW_STATE_RESTORED;
                if (mode == PAL_DISPATCH_MODE_NONE) {
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
                }

                // if state has not changed, we discard the event
                if (data->state == state) {
                    return 0;
                }

                if (mode == PAL_DISPATCH_MODE_CALLBACK) {
                    PalEvent event = {0};
                    event.type = PAL_EVENT_TYPE_WINDOW_STATE;
                    event.data = state;
                    event.data2 = palPackPointer((PalWindow*)hwnd);
                    palPushEvent(driver, &event);

                } else if (mode == PAL_DISPATCH_MODE_POLL) {
                    s_Event.pendingState = PAL_TRUE;
                    s_Event.state = state;
                }
                data->state = state;
            }
#endif // PAL_HAS_EVENT_MODULE

            return 0;
        }

        case WM_MOVE: {
#if PAL_HAS_EVENT_MODULE
            if (g_Win32.eventDriver) {
                PalEventDriver* driver = g_Win32.eventDriver;
                mode = palGetEventDispatchMode(driver, PAL_EVENT_TYPE_WINDOW_MOVE);
                int32_t x = GET_X_LPARAM(lParam);
                int32_t y = GET_Y_LPARAM(lParam);

                if (mode == PAL_DISPATCH_MODE_CALLBACK) {
                    PalEvent event = {0};
                    event.type = PAL_EVENT_TYPE_WINDOW_MOVE;
                    event.data = palPackInt32(x, y);
                    event.data2 = palPackPointer((PalWindow*)hwnd);
                    palPushEvent(driver, &event);

                } else {
                    s_Event.pendingMove = PAL_TRUE;
                    s_Event.x = x;
                    s_Event.y = y;
                    s_Event.window = (PalWindow*)hwnd;
                }
            }
#endif // PAL_HAS_EVENT_MODULE

            return 0;
        }

        case WM_SHOWWINDOW: {
#if PAL_HAS_EVENT_MODULE
            if (g_Win32.eventDriver) {
                PalEventDriver* driver = g_Win32.eventDriver;
                PalEventType type = PAL_EVENT_TYPE_WINDOW_VISIBILITY;
                mode = palGetEventDispatchMode(driver, type);
                if (mode != PAL_DISPATCH_MODE_NONE) {
                    PalEvent event = {0};
                    event.type = type;
                    event.data = (PalBool)wParam;
                    event.data2 = palPackPointer((PalWindow*)hwnd);
                    palPushEvent(driver, &event);
                }
            }
#endif // PAL_HAS_EVENT_MODULE

            return 0;
        }

        case WM_SETFOCUS: {
#if PAL_HAS_EVENT_MODULE
            if (g_Win32.eventDriver) {
                PalEventDriver* driver = g_Win32.eventDriver;
                mode = palGetEventDispatchMode(driver, PAL_EVENT_TYPE_WINDOW_FOCUS);
                if (mode != PAL_DISPATCH_MODE_NONE) {
                    PalEvent event = {0};
                    event.type = PAL_EVENT_TYPE_WINDOW_FOCUS;
                    event.data = PAL_TRUE;
                    event.data2 = palPackPointer((PalWindow*)hwnd);
                    palPushEvent(driver, &event);
                }
            }
#endif // PAL_HAS_EVENT_MODULE

            return 0;
        }

        case WM_KILLFOCUS: {
#if PAL_HAS_EVENT_MODULE
            if (g_Win32.eventDriver) {
                PalEventDriver* driver = g_Win32.eventDriver;
                mode = palGetEventDispatchMode(driver, PAL_EVENT_TYPE_WINDOW_FOCUS);
                if (mode != PAL_DISPATCH_MODE_NONE) {
                    PalEvent event = {0};
                    event.type = PAL_EVENT_TYPE_WINDOW_FOCUS;
                    event.data = PAL_FALSE;
                    event.data2 = palPackPointer((PalWindow*)hwnd);
                    palPushEvent(driver, &event);
                }
            }
#endif // PAL_HAS_EVENT_MODULE

            return 0;
        }

        case WM_ENTERSIZEMOVE: {
            g_Win32.pushMouseDelta = PAL_FALSE;

#if PAL_HAS_EVENT_MODULE
            if (g_Win32.eventDriver) {
                PalEventDriver* driver = g_Win32.eventDriver;
                PalEventType type = PAL_EVENT_TYPE_WINDOW_MODAL_BEGIN;
                mode = palGetEventDispatchMode(driver, type);
                if (mode != PAL_DISPATCH_MODE_NONE) {
                    PalEvent event = {0};
                    event.type = type;
                    event.data2 = palPackPointer((PalWindow*)hwnd);
                    palPushEvent(driver, &event);
                }
            }
#endif // PAL_HAS_EVENT_MODULE

            return 0;
        }

        case WM_EXITSIZEMOVE: {
            g_Win32.pushMouseDelta = PAL_TRUE;

#if PAL_HAS_EVENT_MODULE
            if (g_Win32.eventDriver) {
                PalEventDriver* driver = g_Win32.eventDriver;
                PalEventType type = PAL_EVENT_TYPE_WINDOW_MODAL_END;
                mode = palGetEventDispatchMode(driver, type);
                if (mode != PAL_DISPATCH_MODE_NONE) {
                    PalEvent event = {0};
                    event.type = type;
                    event.data2 = palPackPointer((PalWindow*)hwnd);
                    palPushEvent(driver, &event);
                }
            }
#endif // PAL_HAS_EVENT_MODULE

            return 0;
        }

        case WM_DPICHANGED: {
#if PAL_HAS_EVENT_MODULE
            if (g_Win32.eventDriver) {
                PalEventDriver* driver = g_Win32.eventDriver;
                PalEventType type = PAL_EVENT_TYPE_MONITOR_DPI_CHANGED;
                mode = palGetEventDispatchMode(driver, type);
                if (mode != PAL_DISPATCH_MODE_NONE) {
                    PalEvent event = {0};
                    event.type = type;
                    event.data = HIWORD(wParam);
                    event.data2 = palPackPointer((PalWindow*)hwnd);
                    palPushEvent(driver, &event);
                }
            }
#endif PAL_HAS_EVENT_MODULE

            return 0;
        }

        case WM_DEVICECHANGE: {
#if PAL_HAS_EVENT_MODULE
            if (wParam != 0x0007) {
                return 0;
            }

            if (g_Win32.eventDriver) {
                PalEventDriver* driver = g_Win32.eventDriver;
                PalEventType type = PAL_EVENT_TYPE_MONITOR_LIST_CHANGED;
                mode = palGetEventDispatchMode(driver, type);
                if (mode != PAL_DISPATCH_MODE_NONE) {
                    PalEvent event = {0};
                    event.type = type;
                    event.data2 = palPackPointer((PalWindow*)hwnd);
                    palPushEvent(driver, &event);
                }
            }
#endif // PAL_HAS_EVENT_MODULE

            return 0;
        }

        case WM_MOUSEHWHEEL: {
            int32_t delta = GET_WHEEL_DELTA_WPARAM(wParam);
            g_Mouse.WheelX = delta / WHEEL_DELTA;

#if PAL_HAS_EVENT_MODULE
            if (g_Win32.eventDriver) {
                PalEventDriver* driver = g_Win32.eventDriver;
                mode = palGetEventDispatchMode(driver, PAL_EVENT_TYPE_MOUSE_WHEEL);
                if (mode != PAL_DISPATCH_MODE_NONE) {
                    PalEvent event = {0};
                    event.type = PAL_EVENT_TYPE_MOUSE_WHEEL;
                    event.data = palPackFloat((float)g_Mouse.WheelX, 0);
                    event.data2 = palPackPointer((PalWindow*)hwnd);
                    palPushEvent(driver, &event);
                }
            }
#endif // PAL_HAS_EVENT_MODULE

            return 0;
        }

        case WM_MOUSEWHEEL: {
            int32_t delta = GET_WHEEL_DELTA_WPARAM(wParam);
            g_Mouse.WheelY = delta / WHEEL_DELTA;

#if PAL_HAS_EVENT_MODULE
            if (g_Win32.eventDriver) {
                PalEventDriver* driver = g_Win32.eventDriver;
                mode = palGetEventDispatchMode(driver, PAL_EVENT_TYPE_MOUSE_WHEEL);
                if (mode != PAL_DISPATCH_MODE_NONE) {
                    PalEvent event = {0};
                    event.type = PAL_EVENT_TYPE_MOUSE_WHEEL;
                    event.data = palPackFloat(0, (float)g_Mouse.WheelY);
                    event.data2 = palPackPointer((PalWindow*)hwnd);
                    palPushEvent(driver, &event);
                }
            }
#endif // PAL_HAS_EVENT_MODULE

            return 0;
        }

        case WM_MOUSEMOVE: {
            const int32_t x = GET_X_LPARAM(lParam);
            const int32_t y = GET_Y_LPARAM(lParam);

#if PAL_HAS_EVENT_MODULE
            if (g_Win32.eventDriver) {
                PalEventDriver* driver = g_Win32.eventDriver;
                mode = palGetEventDispatchMode(driver, PAL_EVENT_TYPE_MOUSE_MOVE);
                if (mode != PAL_DISPATCH_MODE_NONE) {
                    PalEvent event = {0};
                    event.type = PAL_EVENT_TYPE_MOUSE_MOVE;
                    event.data = palPackInt32(x, y);
                    event.data2 = palPackPointer((PalWindow*)hwnd);
                    palPushEvent(driver, &event);
                }
            }
#endif // PAL_HAS_EVENT_MODULE

            return 0;
        }

        case WM_INPUT: {
            UINT size = sizeof(s_RawBuffer);
            UINT result = GetRawInputData(
                (HRAWINPUT)lParam,
                RID_INPUT,
                s_RawBuffer,
                &size,
                sizeof(RAWINPUTHEADER));

            if (result == (UINT)-1) {
                break;
            }

            RAWINPUT* raw = (RAWINPUT*)s_RawBuffer;
            RAWMOUSE* mouse = &raw->data.mouse;

            if (g_Win32.pushMouseDelta && (mouse->lLastX || mouse->lLastY)) {
                g_Mouse.dx += mouse->lLastX;
                g_Mouse.dy += mouse->lLastY;

                float dx = (float)g_Mouse.dx;
                float dy = (float)g_Mouse.dy;

#if PAL_HAS_EVENT_MODULE
                if (g_Win32.eventDriver) {
                    PalEventDriver* driver = g_Win32.eventDriver;
                    PalEventType type = PAL_EVENT_TYPE_MOUSE_DELTA;
                    mode = palGetEventDispatchMode(driver, type);
                    if (mode != PAL_DISPATCH_MODE_NONE) {
                        PalEvent event = {0};
                        event.type = type;
                        event.data = palPackFloat(dx, dy);
                        palPushEvent(driver, &event);
                    }
                }
#endif // PAL_HAS_EVENT_MODULE

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
            PalMouseButton button = PAL_MOUSE_BUTTON_UNKNOWN;
            PalEventType type;
            PalBool pressed = PAL_FALSE;

            if (msg == WM_LBUTTONDOWN || msg == WM_LBUTTONUP) {
                button = PAL_MOUSE_BUTTON_LEFT;

            } else if (msg == WM_RBUTTONDOWN || msg == WM_RBUTTONUP) {
                button = PAL_MOUSE_BUTTON_RIGHT;

            } else if (msg == WM_MBUTTONDOWN || msg == WM_MBUTTONUP) {
                button = PAL_MOUSE_BUTTON_MIDDLE;

            } else if (msg == WM_XBUTTONDOWN || msg == WM_XBUTTONUP) {
                WORD xButton = HIWORD(wParam);
                if (xButton == XBUTTON1) {
                    button = PAL_MOUSE_BUTTON_X1;
                } else if (xButton == XBUTTON2) {
                    button = PAL_MOUSE_BUTTON_X2;
                }
            }

            // clang-format off
            // check if we pressed or released the button
            if (msg == WM_LBUTTONDOWN ||
                msg == WM_RBUTTONDOWN ||
                msg == WM_MBUTTONDOWN ||
                msg == WM_XBUTTONDOWN) {
                pressed = PAL_TRUE;
                type = PAL_EVENT_TYPE_MOUSE_BUTTONDOWN;

            } else {
                pressed = PAL_FALSE;
                type = PAL_EVENT_TYPE_MOUSE_BUTTONUP;
            }
            // clang-format on

            if (msg == WM_LBUTTONDOWN) {
                SetCapture(hwnd);

            } else if (msg == WM_LBUTTONUP) {
                ReleaseCapture();
            }

            g_Mouse.state[button] = pressed;

#if PAL_HAS_EVENT_MODULE
            if (g_Win32.eventDriver) {
                PalEventDriver* driver = g_Win32.eventDriver;
                mode = palGetEventDispatchMode(driver, type);
                if (mode != PAL_DISPATCH_MODE_NONE) {
                    PalEvent event = {0};
                    event.type = type;
                    event.data = palPackUint32(button, NULL_BUTTON_SERIAL);
                    event.data2 = palPackPointer((PalWindow*)hwnd);
                    palPushEvent(driver, &event);
                }
            }
#endif // PAL_HAS_EVENT_MODULE

            return 0;
        }

        case WM_KEYDOWN:
        case WM_SYSKEYDOWN:
        case WM_KEYUP:
        case WM_SYSKEYUP: {
            PalKeycode keycode = PAL_KEYCODE_UNKNOWN;
            PalScancode scancode = PAL_SCANCODE_UNKNOWN;
            PalEventType type;
            int32_t win32Keycode;
            int32_t win32Scancode;
            PalBool pressed = PAL_FALSE;
            PalBool extended = PAL_FALSE;

            pressed = (HIWORD(lParam) & KF_UP) ? PAL_FALSE : PAL_TRUE;
            extended = (lParam >> 24) & 1; // we use this for special keys
            win32Scancode = (HIWORD(lParam) & (KF_EXTENDED | 0xff));
            win32Keycode = (UINT)wParam;

            // spcecial scancode handling
            if (!extended && win32Scancode == 0x045) {
                scancode = PAL_SCANCODE_NUMLOCK;

            } else {
                uint16_t index = win32Scancode | (extended << 8);
                scancode = s_Win32Keyboard.scancodes[index];
            }

            keycode = s_Win32Keyboard.keycodes[win32Keycode];
            if (keycode == PAL_KEYCODE_UNKNOWN) {
                keycode = (PalKeycode)(uint32_t)scancode;
            }

            if (win32Keycode == VK_SNAPSHOT) {
                // printscreen since the platform does not get us a keydown, we do that ourselves.
                g_Keyboard.keycodeState[keycode] = PAL_TRUE;

#if PAL_HAS_EVENT_MODULE
                if (g_Win32.eventDriver) {
                    PalEventDriver* driver = g_Win32.eventDriver;
                    mode = palGetEventDispatchMode(driver, PAL_EVENT_TYPE_KEYDOWN);
                    keycode = PAL_KEYCODE_PRINTSCREEN;
                    if (mode != PAL_DISPATCH_MODE_NONE) {
                        PalEvent event = {0};
                        event.type = PAL_EVENT_TYPE_KEYDOWN;
                        event.data = palPackUint32(keycode, scancode);
                        event.data2 = palPackPointer((PalWindow*)hwnd);
                        palPushEvent(driver, &event);
                    }
                }
#endif // PAL_HAS_EVENT_MODULE
            }

            PalBool repeat = g_Keyboard.keycodeState[keycode];
            if (pressed) {
                g_Keyboard.keycodeState[keycode] = PAL_TRUE;
                g_Keyboard.scancodeState[scancode] = PAL_TRUE;

                type = PAL_EVENT_TYPE_KEYDOWN;
                if (repeat) {
                    type = PAL_EVENT_TYPE_KEYREPEAT;
                }

            } else {
                g_Keyboard.keycodeState[keycode] = PAL_FALSE;
                g_Keyboard.scancodeState[scancode] = PAL_FALSE;
                type = PAL_EVENT_TYPE_KEYUP;
            }

#if PAL_HAS_EVENT_MODULE
            if (g_Win32.eventDriver) {
                PalEventDriver* driver = g_Win32.eventDriver;
                mode = palGetEventDispatchMode(driver, type);
                if (mode != PAL_DISPATCH_MODE_NONE) {
                    PalEvent event = {0};
                    event.type = type;
                    event.data = palPackUint32(keycode, scancode);
                    event.data2 = palPackPointer((PalWindow*)hwnd);
                    palPushEvent(driver, &event);
                }
            }
#endif // PAL_HAS_EVENT_MODULE

            return 0;
        }

        case WM_ERASEBKGND: {
            return PAL_TRUE;
        }

        case WM_SETCURSOR: {
            if (LOWORD(lParam) == HTCLIENT) {
                if (data && data->cursor) {
                    SetCursor(data->cursor);
                } else {
                    SetCursor(g_Win32.defaultCursor);
                }
                return PAL_TRUE;
            }
            break;
        }

        case WM_CHAR: {
#if PAL_HAS_EVENT_MODULE
            PalEventType type = PAL_EVENT_TYPE_KEYCHAR;
            uint32_t codepoint = 0;

            if (g_Win32.eventDriver) {
                PalEventDriver* driver = g_Win32.eventDriver;
                mode = palGetEventDispatchMode(driver, type);
                if (mode == PAL_DISPATCH_MODE_NONE) {
                    break;
                }
            }

            // Most characters comes as two WM_CHAR messags or event we store the first one
            // and combine with the second if we got any.
            uint16_t character = (uint16_t)wParam;
            if (character >= 0xD800 && character <= 0xDBFF) {
                // high surrogate
                s_Win32Keyboard.pendingHighSurrogate = character;

            } else if (character >= 0xDC00 && character <= 0xDFFF) {
                if (s_Win32Keyboard.pendingHighSurrogate) {
                    // low surrogate we combine both
                    uint32_t high = s_Win32Keyboard.pendingHighSurrogate - 0xD800;
                    uint32_t low = character - 0xDC00;
                    codepoint = 0x10000 + ((high << 10) | low);
                    s_Win32Keyboard.pendingHighSurrogate = 0;
                }

            } else {
                // normal character (A-Z)
                codepoint = character;
            }

            // push an event
            PalEvent event = {0};
            event.type = type;
            event.data = codepoint;
            event.data2 = palPackPointer((PalWindow*)hwnd);
            palPushEvent(g_Win32.eventDriver, &event);
#endif // PAL_HAS_EVENT_MODULE
        }

    }

    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

/* =========================================================================
 * API
 * ========================================================================= */

void win32ShutdownVideo(void);

PalVideoDriver* win32GetVideoDriver(void)
{
    if (s_Win32Driver.features != 0) {
        return &s_Win32Driver;
    }

    if (!g_Win32.shcore) {
        g_Win32.shcore = palLoadLibrary("shcore.dll");
        if (g_Win32.shcore) {
            loadProcs(PAL_FALSE);
            checkFeatures();

            palFreeLibrary(g_Win32.shcore);
            memset(&g_Win32, 0, sizeof(Win32));
        }
    }

    s_Win32Driver.type = PAL_VIDEO_DRIVER_TYPE_WIN32;
    return &s_Win32Driver;
}

PalResult win32InitVideo(
    const PalAllocator* allocator,
    struct PalEventDriver* eventDriver,
    const void* preferredInstance,
    void** selectedInstance)
{
    PalResult result = PAL_RESULT_CODE_PLATFORM_FAILURE;
    g_Win32.allocator = allocator;
    g_Win32.eventDriver = eventDriver;

    g_Win32.shcore = palLoadLibrary("shcore.dll");
    g_Win32.gdi = palLoadLibrary("gdi32.dll");
    if (!g_Win32.gdi) {
        result = palMakeResult(PAL_RESULT_CODE_PLATFORM_FAILURE, PAL_RESULT_SOURCE_WIN32, GetLastError());
        goto cleanup;
    }

    loadProcs(PAL_TRUE);
    createScancodeTable();
    createKeycodeTable();
    checkFeatures();

    g_Win32.maxWindowData = 32;
    g_Win32.windowData = palAllocate(allocator, sizeof(WindowData) * g_Win32.maxWindowData, 0);
    if (!g_Win32.windowData) {
        result = PAL_RESULT_CODE_OUT_OF_MEMORY;
        goto cleanup;
    }
    memset(g_Win32.windowData, 0, sizeof(WindowData) * g_Win32.maxWindowData);

    if (preferredInstance) {
        g_Win32.instance = (HINSTANCE)preferredInstance;
        *selectedInstance = (void*)preferredInstance;

    } else {
        g_Win32.instance = GetModuleHandleW(nullptr);
        if (!g_Win32.instance) {
            result = PAL_RESULT_CODE_PLATFORM_FAILURE;
            goto cleanup;
        }
        *selectedInstance = (void*)g_Win32.instance;
    }

    g_Win32.defaultCursor = LoadCursorW(NULL, IDC_ARROW);

    WNDCLASSEXW wc = {0};
    wc.cbSize = sizeof(WNDCLASSEXW);
    wc.hCursor = g_Win32.defaultCursor;
    wc.hIcon = LoadIconW(NULL, IDI_APPLICATION);
    wc.hIconSm = LoadIconW(NULL, IDI_APPLICATION);
    wc.hInstance = g_Win32.instance;
    wc.lpfnWndProc = videoProc;
    wc.lpszClassName = VIDEO_CLASS;
    wc.style = CS_OWNDC;

    if (!RegisterClassExW(&wc)) {
        result = palMakeResult(PAL_RESULT_CODE_PLATFORM_FAILURE, PAL_RESULT_SOURCE_WIN32, GetLastError());
        goto cleanup;
    }

    g_Win32.hiddenWindow = CreateWindowExW(
        0,
        VIDEO_CLASS,
        L"HiddenWindow",
        WS_OVERLAPPEDWINDOW,
        0,
        0,
        8,
        8,
        nullptr,
        nullptr,
        g_Win32.instance,
        nullptr);

    if (!g_Win32.hiddenWindow) {
        result = palMakeResult(PAL_RESULT_CODE_PLATFORM_FAILURE, PAL_RESULT_SOURCE_WIN32, GetLastError());
        goto cleanup;
    }

    SetPropW(g_Win32.hiddenWindow, VIDEO_PROP, &s_Event);

    RAWINPUTDEVICE rid = {0};
    rid.dwFlags = RIDEV_INPUTSINK;
    rid.hwndTarget = g_Win32.hiddenWindow;
    rid.usUsage = 0x02;
    rid.usUsagePage = 0x01;
    if (!RegisterRawInputDevices(&rid, 1, sizeof(RAWINPUTDEVICE))) {
        result = palMakeResult(PAL_RESULT_CODE_PLATFORM_FAILURE, PAL_RESULT_SOURCE_WIN32, GetLastError());
        goto cleanup;
    }

    g_Win32.pushMouseDelta = PAL_TRUE;
    return PAL_RESULT_SUCCESS;

cleanup:
    win32ShutdownVideo();
    return result;
}

void win32ShutdownVideo(void)
{
    if (g_Win32.shcore) {
        palFreeLibrary(g_Win32.shcore);
    }

    if (g_Win32.gdi) {
        palFreeLibrary(g_Win32.gdi);
    }

    if (g_Win32.hiddenWindow) {
        DestroyWindow(g_Win32.hiddenWindow);
    }

    if (g_Win32.windowData) {
        palFree(g_Win32.allocator, g_Win32.windowData);
    }

    if (g_Win32.instance) {
        UnregisterClassW(VIDEO_CLASS, g_Win32.instance);
    }

    memset(&g_Win32, 0, sizeof(Win32));
    memset(&s_Win32Keyboard, 0, sizeof(Win32Keyboard));
}

void win32UpdateVideo(void)
{
    g_Mouse.dx = 0;
    g_Mouse.dy = 0;

    MSG msg;
    while (PeekMessageA(&msg, NULL, 0, 0, PM_REMOVE)) {
        TranslateMessage(&msg);
        DispatchMessageA(&msg);
    }

#if PAL_HAS_EVENT_MODULE
    if (s_Event.pendingResize) {
        PalEvent event = {0};
        event.type = PAL_EVENT_TYPE_WINDOW_SIZE;
        event.data = palPackUint32(s_Event.width, s_Event.height);
        event.data2 = palPackPointer(s_Event.window);
        palPushEvent(g_Win32.eventDriver, &event);

        if (s_Event.pendingState) {
            PalEvent event = {0};
            event.data = s_Event.state;
            event.data2 = palPackPointer(s_Event.window);
            event.type = PAL_EVENT_TYPE_WINDOW_STATE;
            palPushEvent(g_Win32.eventDriver, &event);
            s_Event.pendingState = PAL_FALSE;
        }

        s_Event.pendingResize = PAL_FALSE;

    } else if (s_Event.pendingMove) {
        PalEvent event = {0};
        event.type = PAL_EVENT_TYPE_WINDOW_MOVE;
        event.data = palPackInt32(s_Event.x, s_Event.y);
        event.data2 = palPackPointer(s_Event.window);
        palPushEvent(g_Win32.eventDriver, &event);
        s_Event.pendingMove = PAL_FALSE;
    }
#endif // PAL_HAS_EVENT_MODULE
}

#endif // _WIN32