
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

#define PAL_CHECK_VIDEO(ret) {                      \
    if (!s_Video.initialized) {                     \
        palSetError(PAL_VIDEO_NOT_INITIALIZED);     \
        return ret;                                 \
    }                                               \
} 

#define PAL_CHECK_WINDOW(window, ret) {             \
    if (!window) {                                  \
        palSetError(PAL_NULL_POINTER);              \
        return ret;                                 \
    }                                               \
} 

#endif // _PAL_VIDEO_C_H