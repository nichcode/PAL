
/**
    PAL - Prime Abstraction Layer
    Copyright (C) 2025
    Licensed under the Zlib license. See LICENSE file in root.
 */

#ifdef _WIN32
#ifndef _PAL_VIDEO_COMMON_WIN32_H
#define _PAL_VIDEO_COMMON_WIN32_H

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

#include "pal/pal_video.h"
#include <windows.h>

#define PAL_VIDEO_CLASS L"PALVideoClass"
#define PAL_VIDEO_PROP L"PalVideoData"
#define WINDOW_NAME_SIZE 256

LRESULT CALLBACK videoProc(
    HWND hwnd,
    UINT msg,
    WPARAM wParam,
    LPARAM lParam);

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

typedef struct {
    PalBool used;
    PalBool isAttached;
    PalWindowState state;
    HCURSOR cursor;
    LONG_PTR wndProc;
} WindowData;

typedef struct {
    PalBool initialized;
    int32_t pixelFormat;
    int32_t maxWindowData;
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
    DescribePixelFormatFn describePixelFormat;
    SetPixelFormatFn setPixelFormat;

    HINSTANCE instance;
    HWND hiddenWindow;
    HCURSOR defaultCursor;
    WindowData* windowData;
} VideoWin32;

static VideoWin32 s_Video = {0};

#endif // _PAL_VIDEO_COMMON_WIN32_H
#endif // _WIN32