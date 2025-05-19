
#pragma once

#include "PAL_defines.h"

struct PAL_Window;

typedef void (*PAL_WindowCloseFun)(PAL_Window* window);
typedef void (*PAL_WindowPosFun)(PAL_Window* window, i32 x, i32 y);
typedef void (*PAL_WindowSizeFun)(PAL_Window* window, u32 width, u32 height);

typedef void (*PAL_KeyFun)(PAL_Window* window, u32 key, i32 scancode, u32 action);

typedef void (*PAL_MouseButtonFun)(PAL_Window* window, u32 button, u32 action);
typedef void (*PAL_MousePosFun)(PAL_Window* window,  i32 x, i32 y);
typedef void (*PAL_ScrollFun)(PAL_Window* window, f32 offset_x, f32 offset_y);

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

PAL_API void PAL_SetWindowCloseCallback(PAL_WindowCloseFun callback);
PAL_API void PAL_SetWindowPosCallback(PAL_WindowPosFun callback);
PAL_API void PAL_SetWindowSizeCallback(PAL_WindowSizeFun callback);

PAL_API void PAL_SetKeyCallback(PAL_KeyFun callback);

PAL_API void PAL_SetMouseButtonCallback(PAL_MouseButtonFun callback);
PAL_API void PAL_SetMousePosCallback(PAL_MousePosFun callback);
PAL_API void PAL_SetScrollCallback(PAL_ScrollFun callback);

PAL_API const char* PAL_GetWindowTitle(PAL_Window* window);
PAL_API void PAL_GetWindowSize(PAL_Window* window, u32* width, u32* height);
PAL_API void PAL_GetWindowPos(PAL_Window* window, i32* x, i32* y);
PAL_API void* PAL_GetWindowHandle(PAL_Window* window);

PAL_API b8 PAL_WindowShouldClose(PAL_Window* window);
PAL_API b8 PAL_WindowIsHidden(PAL_Window* window);
PAL_API b8 PAL_WindowIsMaximized(PAL_Window* window);
PAL_API b8 PAL_WindowIsMinimized(PAL_Window* window);