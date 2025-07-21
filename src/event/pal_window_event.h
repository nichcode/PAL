
#ifndef _PAL_WINDOW_EVENT
#define _PAL_WINDOW_EVENT

#include "pal/pal_event.h"

void palWindowQuitEvent(
    PalEventDriver driver,
    Uint32 windowID);

void palWindowDpiChangeEvent(
    PalEventDriver driver, 
    Uint32 dpi,
    Uint32 windowID);

void palWindowResizeEvent(
    PalEventDriver driver, 
    Uint32 width, 
    Uint32 height,
    Uint32 windowID);

void palWindowMoveEvent(
    PalEventDriver driver, 
    int x, 
    int y,
    Uint32 windowID);

#endif // _PAL_WINDOW_EVENT