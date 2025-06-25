
#include "pal_pch.h"
#include "pal_win32platform.h"

LRESULT CALLBACK palWin32Proc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    return DefWindowProcW(hwnd, msg, wParam, lParam);
}