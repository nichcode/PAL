
#ifndef _PAL_VIDEO_C_H
 #define _PAL_VIDEO_C_H

#include "pal_shared.h"
#include "pal/pal_video.h"

typedef struct PalVideoSystem
{
    PalDisplay displays[PAL_MAX_DISPLAYS];
    PalHashMap map;
    PalAllocator allocator;
    PalWindowID nextWindowID;
    int displayCount;
    bool initialized;

} PalVideoSystem;

static PalVideoSystem s_Video;

bool palLoadLibraries();
void palUnloadLibraries();

bool palCompareDisplayMode(const PalDisplayMode* a, const PalDisplayMode* b);
void palAddDisplayMode(const PalDisplayMode* mode);
void palGetColorBits(PalDisplayMode* mode, int bpp);
void palGetDisplayDPI(PalDisplay* display);


#endif // _PAL_VIDEO_C_H