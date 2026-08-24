
#ifndef _GRAPHICS_HELPER_H
#define _GRAPHICS_HELPER_H

#include "tests.h"
#include "pal2/pal_graphics.h"
#include "pal2/pal_video.h"

#define MAX_FRAMES_IN_FLIGHT 2

PalWindow* gfxHelperCreateWindow(
    const char* title,
    PalEventDriver* eventDriver);

void gfxHelperDestroyWindow(
    PalWindow* window, 
    PalEventDriver* eventDriver);

PalSurface* gfxHelperCreateSurface(
    PalDevice* device, 
    PalWindow* window);

#endif // _GRAPHICS_HELPER_H