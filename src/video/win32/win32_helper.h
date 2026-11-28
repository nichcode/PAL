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

#ifndef WIN32_HELPER_H
#define WIN32_HELPER_H
#ifdef _WIN32

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif // WIN32_LEAN_AND_MEAN

#ifndef NOMINMAX
#define NOMINMAX
#endif // NOMINMAX

#ifndef UNICODE
#define UNICODE
#define _UNICODE
#endif // UNICODE

#include "pal2/core/library.h"
#include "pal2/video/init.h"
#include "pal2/video/window.h"
#include "pal2/video/monitor.h"

#include <windows.h>

#define VIDEO_PROP L"PALVideoData"
#define VIDEO_CLASS L"PALVideoClass"

typedef HRESULT(WINAPI* GetDpiForMonitorFn)(
    HMONITOR,
    int32_t,
    UINT*,
    UINT*);

typedef HRESULT(WINAPI* SetProcessAwarenessFn)(int32_t);

typedef HBITMAP(WINAPI* CreateDIBSectionFn)(
    HDC,
    const BITMAPINFO*,
    UINT,
    VOID**,
    HANDLE,
    DWORD);

typedef HBITMAP(WINAPI* CreateBitmapFn)(
    int,
    int,
    UINT,
    UINT,
    CONST VOID*);

typedef BOOL(WINAPI* DeleteObjectFn)(HGDIOBJ);

typedef int(WINAPI* DescribePixelFormatFn)(
    HDC,
    int,
    UINT,
    LPPIXELFORMATDESCRIPTOR);

typedef BOOL(WINAPI* SetPixelFormatFn)(
    HDC,
    int,
    CONST PIXELFORMATDESCRIPTOR*);

typedef struct WindowData {
    PalBool used;
    PalBool isAttached;
    PalWindowState state;
    HCURSOR cursor;
    LONG_PTR wndProc;
} WindowData;

typedef struct Win32 {
    PalLibrary* gdi;
    PalLibrary* shcore;
    HINSTANCE instance;
    HWND hiddenWindow;
    HCURSOR defaultCursor;

    uint32_t maxWindowData;
    PalBool pushMouseDelta;
    WindowData* windowData;
    const PalAllocator* allocator;
    struct PalEventDriver* eventDriver;
    
    GetDpiForMonitorFn getDpiForMonitor;
    SetProcessAwarenessFn setProcessAwareness;
    CreateDIBSectionFn createDIBSection;
    CreateBitmapFn createBitmap;
    DeleteObjectFn deleteObject;
    DescribePixelFormatFn describePixelFormat;
    SetPixelFormatFn setPixelFormat;
} Win32;

extern Win32 g_Win32;

PalResult win32CreateBitmap(
    uint32_t width, 
    uint32_t height, 
    const uint8_t* pixels,
    HBITMAP* bitmap);

void win32DestroyBitmap(HBITMAP map);

#endif // _WIN32
#endif // WIN32_HELPER_H