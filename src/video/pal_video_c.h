
#include "pal/pal_video.h"
#include "containers/pal_hashmap.h"

#define PAL_MAX_DISPLAYS 16
#define PAL_MAX_WINDOWS 16

typedef struct PalVideo
{
    PalDisplay displays[PAL_MAX_DISPLAYS];
    PalHashMap map;
    PalAllocator allocator;
    PalWindowID nextWindowID;
    int displayCount;
    bool initialized;

} PalVideo;

extern PalVideo g_Video;

bool palInitWindow();
void palShutdownWindow();

bool palInitDisplay();
void palShutdownDisplay();

#define PAL_CHECK_VIDEO(ret) {                        \
    if (!g_Video.initialized) {                       \
        palSetError(PAL_VIDEO_NOT_INITIALIZED);       \
        return ret;                                   \
    }                                                 \
}

// This removes the newline warning