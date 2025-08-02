
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

#include "pal_pch.h"
#include "pal_win32video.h"

// ==================================================
// Public API
// ==================================================

PalResult _PCALL palCreateVideoSystem(
    const PalVideoSystemCreateInfo* info,
    PalVideoSystem** outVideo) {

    PalVideoSystem* video;
    if (!info || !outVideo) {
        return PAL_RESULT_NULL_POINTER;
    }

    if (info->allocator && 
    (!info->allocator->allocate || !info->allocator->free)) {
        return PAL_RESULT_INVALID_ALLOCATOR;
    }

    video = palAllocate(info->allocator, sizeof(PalVideoSystem), 0);
    if (!video) {
        return PAL_RESULT_OUT_OF_MEMORY;
    }

    memset(video, 0, sizeof(PalVideoSystem));
    if (info->allocator) {
        video->allocator = info->allocator;
    }

    // register class
    video->instance = GetModuleHandleW(nullptr);
    WNDCLASSEXW wc = {0};
    wc.cbSize = sizeof(WNDCLASSEXW);
    if (!GetClassInfoExW(video->instance, WIN32_VIDEO_CLASS, &wc)) {
        wc.cbClsExtra = 0;
        wc.cbWndExtra = 0;
        wc.hbrBackground = NULL;
        wc.hCursor = LoadCursorW(video->instance, (LPCWSTR)IDC_ARROW);
        wc.hIcon = LoadIconW(video->instance, (LPCWSTR)IDI_APPLICATION);
        wc.hIconSm = LoadIconW(video->instance, (LPCWSTR)IDI_APPLICATION);
        wc.hInstance = video->instance;
        wc.lpfnWndProc = palVideoProc;
        wc.lpszClassName = WIN32_VIDEO_CLASS;
        wc.lpszMenuName = NULL;
        wc.style = CS_DBLCLKS | CS_OWNDC | CS_HREDRAW | CS_VREDRAW;

        if (!RegisterClassExW(&wc)) {
            return PAL_RESULT_ACCESS_DENIED;
        }
    }

    // load shared libraries
    video->shcore = LoadLibraryA("shcore.dll");
    if (video->shcore) {
        video->hasShcore = true;
    }

    *outVideo = video;
    return PAL_RESULT_SUCCESS;
}

void _PCALL palDestroyVideoSystem(
    PalVideoSystem* video) {

    if (!video || (video && !video->instance)) {
        return;
    }

    UnregisterClassW(WIN32_VIDEO_CLASS, video->instance);
    palFree(video->allocator, video);
}

// TODO: remove
LRESULT CALLBACK palVideoProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {

    return DefWindowProcW(hwnd, msg, wParam, lParam);
}