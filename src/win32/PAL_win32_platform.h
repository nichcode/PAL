
#pragma once

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif // WIN32_LEAN_AND_MEAN

#include "PAL_internal.h"
#include "PAL/PAL_input.h"

#include <windows.h>

static HINSTANCE s_Instance;
static const wchar_t* s_ClassName = L"WindowClass";
static const wchar_t* s_PropName = L"WindowProp";

LRESULT CALLBACK PAL_Win32Proc(HWND hwnd, u32 msg, WPARAM w_param, LPARAM l_param);

void PAL_Win32Init();
void PAL_Win32Terminate();

struct PAL_Window
{
    HWND handle;
    const char* title;
    u32 width, height, flags;
    i32 x, y;
    b8 shouldClose, focused, hidden;
    b8 maximized, minimized;

    u32 keycodes[512];
    i32 scancodes[PAL_KEY_MAX + 1];

    u32 keys[PAL_KEY_MAX + 1];
    u32 mouseButtons[PAL_MOUSE_BUTTON_MAX + 1];
    i32 mousePos[2];
};