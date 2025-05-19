
#pragma once

#include "PAL_defines.h"

struct PAL_Window;

enum PAL_WindowFlags
{
    PAL_WINDOW_SHOWN = PAL_BIT(0),
    PAL_WINDOW_CENTER = PAL_BIT(1),
    PAL_WINDOW_RESIZABLE = PAL_BIT(2),
    PAL_WINDOW_MINIMIZEBOX = PAL_BIT(3),
    PAL_WINDOW_MAXIMIZE = PAL_BIT(4),
    PAL_APP_WINDOW = PAL_WINDOW_SHOWN | PAL_WINDOW_RESIZABLE | PAL_WINDOW_MINIMIZEBOX
};

PAL_API PAL_Window* PAL_CreateWindow(const char* title, u32 width, u32 height, u32 flags);
PAL_API void PAL_DestroyWindow(PAL_Window* window);

PAL_API void PAL_PullEvents();
PAL_API void PAL_HideWindow(PAL_Window* window);
PAL_API void PAL_ShowWindow(PAL_Window* window);

PAL_API void PAL_MaximizeWindow(PAL_Window* window);
PAL_API void PAL_MinimizeWindow(PAL_Window* window);
PAL_API void PAL_MakeWindowActive(PAL_Window* window);

PAL_API void PAL_SetWindowTitle(PAL_Window* window, const char* title);
PAL_API void PAL_SetWindowPos(PAL_Window* window, i32 x, i32 y, b8 center);
PAL_API void PAL_SetWindowSize(PAL_Window* window, u32 width, u32 height);

PAL_API const char* PAL_GetWindowTitle(PAL_Window* window);
PAL_API void PAL_GetWindowSize(PAL_Window* window, u32* width, u32* height);
PAL_API void PAL_GetWindowPos(PAL_Window* window, i32* x, i32* y);
PAL_API void* PAL_GetWindowHandle(PAL_Window* window);

PAL_API b8 PAL_WindowShouldClose(PAL_Window* window);
PAL_API b8 PAL_WindowIsHidden(PAL_Window* window);
PAL_API b8 PAL_WindowIsMaximized(PAL_Window* window);
PAL_API b8 PAL_WindowIsMinimized(PAL_Window* window);