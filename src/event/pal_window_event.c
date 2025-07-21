
#include "pal_pch.h"
#include "pal_window_event.h"
#include "pal_event_internal.h"

void palWindowQuitEvent(
    PalEventDriver driver,
    Uint32 windowID) {

    if (driver->modes[PAL_EVENT_QUIT] != PAL_EVENT_MODE_DISABLED) {
        PalEvent event;
        event.sourceID = windowID;
        event.type = PAL_EVENT_QUIT;
        palPushEvent(driver, &event);
    }
}

void palWindowDpiChangeEvent(
    PalEventDriver driver, 
    Uint32 dpi,
    Uint32 windowID) {

    if (driver->modes[PAL_EVENT_DPI_CHANGED] != PAL_EVENT_MODE_DISABLED) {
        PalEvent event;
        event.sourceID = windowID;
        event.type = PAL_EVENT_DPI_CHANGED;
        event.data = dpi;
        palPushEvent(driver, &event);
    }
}

void palWindowResizeEvent(
    PalEventDriver driver, 
    Uint32 width, 
    Uint32 height,
    Uint32 windowID) {
 
    if (driver->modes[PAL_EVENT_WINDOW_RESIZE] != PAL_EVENT_MODE_DISABLED) {
        PalEvent event;
        event.sourceID = windowID;
        event.type = PAL_EVENT_WINDOW_RESIZE;
        event.data = palPackUint32(width, height);
        palPushEvent(driver, &event);
    }
}

void palWindowMoveEvent(
    PalEventDriver driver, 
    int x, 
    int y,
    Uint32 windowID) {

    if (driver->modes[PAL_EVENT_WINDOW_MOVE] != PAL_EVENT_MODE_DISABLED) {
        PalEvent event;
        event.sourceID = windowID;
        event.type = PAL_EVENT_WINDOW_MOVE;
        event.data = palPackInt32(x, y);
        palPushEvent(driver, &event);
    }
}

