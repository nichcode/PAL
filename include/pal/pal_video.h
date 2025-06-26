
#ifndef _PAL_VIDEO_H
 #define _PAL_VIDEO_H

#include "pal_memory.h"

#define PAL_MAX_MODES 64

typedef struct PalWindow PalWindow;

typedef enum PalWindowFlags
{
    PAL_WINDOW_SHOWN = PAL_BIT(0),
    PAL_WINDOW_MAXIMIZED = PAL_BIT(1),
    PAL_WINDOW_RESIZABLE = PAL_BIT(2),
    PAL_WINDOW_CENTER = PAL_BIT(3),
    PAL_WINDOW_MINIMIZEBOX = PAL_BIT(4),
    PAL_WINDOW_DEAFULT = PAL_WINDOW_RESIZABLE | PAL_WINDOW_MINIMIZEBOX | PAL_WINDOW_SHOWN

} PalWindowFlags;

typedef struct PalDisplayMode
{
    Uint32 width;
    Uint32 height;
    Uint16 refreshRate;

    Uint16 redBits;
    Uint16 greenBits;
    Uint16 blueBits;
    Uint16 alphaBits;

} PalDisplayMode;

typedef struct PalDisplay
{
    PalDisplayMode modes[PAL_MAX_MODES];
    char name[32];
    void* handle;
    int index;
    int modeCount;

    int x;
    int y;
    Uint32 width;
    Uint32 height;

    float dpiScaleX;
    float dpiScaleY;

    Uint16 refreshRate;

} PalDisplay;

typedef struct PalWindowDesc
{
    const char* title;
    Uint16 displayIndex;
    Uint32 width;
    Uint32 height;
    PalWindowFlags flags;

} PalWindowDesc;

_PAPI bool _PCALL palInitVideoSystem(const PalAllocator* allocator);
_PAPI void _PCALL palShutdownVideoSystem();
_PAPI bool _PCALL palIsVideoSystemInitialized();

_PAPI int _PCALL palGetDisplayCount();
_PAPI const PalDisplay* _PCALL palGetPrimaryDisplay();
_PAPI const PalDisplay* _PCALL palGetDisplay(int index);

_PAPI PalWindow* _PCALL palCreateWindow(PalWindowDesc* desc);
_PAPI void _PCALL palDestroyWindow(PalWindow* window);

_PAPI void _PCALL palShowWindow(PalWindow* window);
_PAPI void _PCALL palHideWindow(PalWindow* window);

_PAPI void _PCALL palMaximizeWindow(PalWindow* window);
_PAPI void _PCALL palMinimizeWindow(PalWindow* window);
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
_PAPI bool _PCALL palIsWindowHidden(PalWindow* window);
_PAPI bool _PCALL palIsWindowFullScreen(PalWindow* window);

#endif // _PAL_VIDEO_H