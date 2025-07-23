
#ifndef _PAL_WIN32_VIDEO_H
#define _PAL_WIN32_VIDEO_H

#define WIN32_VIDEO_CLASS L"PALVideoClass"
#define WIN32_VIDEO_PROP L"PALVideo"

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif // WIN32_LEAN_AND_MEAN

#ifndef NOMINMAX
#define NOMINMAX
#endif // NOMINMAX

#include <windows.h>

typedef struct PalVideoDataWin32 {
    HINSTANCE instance;
    HINSTANCE shcore;
} PalVideoDataWin32;

LRESULT CALLBACK palVideoProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
bool palRegisterVideoClass(HINSTANCE instance);
void palUnregisterVideoClass(HINSTANCE instance);

#endif // _PAL_WIN32_VIDEO_H