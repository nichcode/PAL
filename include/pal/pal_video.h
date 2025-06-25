
#ifndef _PAL_VIDEO_H
 #define _PAL_VIDEO_H

#include "pal_memory.h"

typedef struct PalWindow PalWindow;

typedef enum PalWindowFlags
{
    PAL_WINDOW_SHOWN = PAL_BIT(0),
    PAL_WINDOW_MAXIMIZED = PAL_BIT(1),
    PAL_WINDOW_RESIZABLE = PAL_BIT(2),
    PAL_WINDOW_CENTER = PAL_BIT(3),
    PAL_WINDOW_MINIMIZEDBOX = PAL_BIT(4),
    PAL_WINDOW_DEAFULT = PAL_WINDOW_RESIZABLE | PAL_WINDOW_MINIMIZEDBOX | PAL_WINDOW_SHOWN

} PalWindowFlags;

typedef struct PalWindowDesc
{
    const char* title;
    Uint32 width;
    Uint32 height;
    PalWindowFlags flags;

} PalWindowDesc;

_PAPI bool _PCALL palInitVideoSystem(const PalAllocator* allocator);
_PAPI void _PCALL palShutdownVideoSystem();
_PAPI bool _PCALL palIsVideoSystemInitialized();

_PAPI PalWindow* _PCALL palCreateWindow(PalWindowDesc* desc);
_PAPI void _PCALL palDestroyWindow(PalWindow* window);

_PAPI void _PCALL palMaximizeWindow(PalWindow* window);
_PAPI void _PCALL palMinimizeWindow(PalWindow* window);
_PAPI void _PCALL palRestoreWindow(PalWindow* window);
_PAPI void _PCALL palSetWindowFullScreen(PalWindow* window, bool enable);

_PAPI const char* _PCALL palGetWindowTitle(PalWindow* window);
_PAPI void _PCALL palGetWindowPos(PalWindow* window, int* x, int* y);
_PAPI void _PCALL palGetWindowSize(PalWindow* window, Uint32* width, Uint32* height);
_PAPI PalWindowFlags _PCALL palGetWindowFlags(PalWindow* window);

_PAPI void _PCALL palSetWindowTitle(PalWindow* window, const char* title);
_PAPI void _PCALL palSetWindowPos(PalWindow* window, int x, int y);
_PAPI void _PCALL palSetWindowSize(PalWindow* window, Uint32 width, Uint32 height);

_PAPI bool _PCALL palIsWindowMaximized(PalWindow* window);
_PAPI bool _PCALL palIsWindowMinimized(PalWindow* window);
_PAPI bool _PCALL palIsWindowRestored(PalWindow* window);
_PAPI bool _PCALL palIsWindowFullScreen(PalWindow* window);

#endif // _PAL_VIDEO_H