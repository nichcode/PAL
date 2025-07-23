
#include "pal_pch.h"
#include "pal/pal_video.h"
#include "platform/pal_platform.h"
#include "pal_win32video.h"

PalResult _PCALL palUpdateVideo(
    PalVideo video) {

    if (!video) {
        return PAL_ERROR_NULL_POINTER;
    }
    
    MSG msg;
    while (PeekMessageA(&msg, PAL_NULL, 0, 0, PM_REMOVE)) {
        TranslateMessage(&msg);
        DispatchMessageA(&msg);
    }

    return PAL_SUCCESS;
}

Uint32 palGetSupportedFeatures() {

    PalVideoFeatureFlags flags = 0;
    flags |= PAL_VIDEO_DISPLAY_ORIENTATION;
    flags |= PAL_VIDEO_BORDERLESS_WINDOW;
    flags |= PAL_VIDEO_FULLSCREEN;
    flags |= PAL_VIDEO_RESIZING;
    flags |= PAL_VIDEO_MULTI_DISPLAYS;
    flags |= PAL_VIDEO_WINDOW_MINMAX;
    flags |= PAL_VIDEO_WINDOW_POSITIONING;
    flags |= PAL_VIDEO_GAMMA_CONTROL;
    flags |= PAL_VIDEO_CLIP_CURSOR;

    // get os version
    PalVersion osVersion;
    if (!palGetOsVersionWin32(&osVersion)) {
        return PAL_ERROR_MODULE_NOT_FOUND;
    }

    if (palIsOsVersionWin32(&osVersion, &g_VersionsWin32.window8_1)) {
        flags |= PAL_VIDEO_PER_DISPLAY_DPI;
        flags |= PAL_VIDEO_HIGH_DPI;
    }
    return flags;   
}

PalResult palCreateVideoData(PalAllocator* allocator, void** outData) {

    PalVideoDataWin32* data = palAllocate(allocator, sizeof(PalVideoDataWin32));
    if (!data) {
        return PAL_ERROR_OUT_OF_MEMORY;
    }

    data->instance = GetModuleHandleW(PAL_NULL);
    palRegisterVideoClass(data->instance);

    data->shcore = LoadLibraryA("shcore.dll");
    *outData = data;
    return PAL_SUCCESS;
}

void palDestroyVideoData(PalAllocator* allocator, void* data) {

    PalVideoDataWin32* videoData = data;
    palUnregisterVideoClass(videoData->instance);
    if (videoData->shcore) {
        FreeLibrary(videoData->shcore);
    }
    palFree(allocator, videoData);
}

bool palRegisterVideoClass(HINSTANCE instance) {

    WNDCLASSEXW wcCheck = {};
    wcCheck.cbSize = sizeof(WNDCLASSEXW);
    if (GetClassInfoExW(instance, WIN32_VIDEO_CLASS, &wcCheck)) {
        return PAL_TRUE;
    }

    WNDCLASSEXW wc = {};
    wc.cbClsExtra = 0;
    wc.cbSize = sizeof(WNDCLASSEXW);
    wc.cbWndExtra = 0;
    wc.hbrBackground = NULL;
    wc.hCursor = LoadCursorW(instance, IDC_ARROW);
    wc.hIcon = LoadIconW(instance, IDI_APPLICATION);
    wc.hIconSm = LoadIconW(instance, IDI_APPLICATION);
    wc.hInstance = instance;
    wc.lpfnWndProc = palVideoProc;
    wc.lpszClassName = WIN32_VIDEO_CLASS;
    wc.lpszMenuName = NULL;
    wc.style = CS_DBLCLKS | CS_OWNDC | CS_HREDRAW | CS_VREDRAW;

    if (!RegisterClassExW(&wc)) {
        return PAL_FALSE;
    }
    return PAL_TRUE;
}

void palUnregisterVideoClass(HINSTANCE instance) {

    WNDCLASSEXW wcCheck = {};
    wcCheck.cbSize = sizeof(WNDCLASSEXW);
    if (GetClassInfoExW(instance, WIN32_VIDEO_CLASS, &wcCheck)) {
        UnregisterClassW(WIN32_VIDEO_CLASS, instance);
    }
}
