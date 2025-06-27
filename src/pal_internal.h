
#ifndef _PAL_INTERNAL_H
 #define _PAL_INTERNAL_H

#include "pal/pal.h"
#include "pal/pal_video.h"
#include "pal/pal_events.h"

#define _PAL_MSG_SIZE 1024
#define _PAL_MAX_DISPLAYS 16
#define _PAL_MAX_EVENTS 256
#define _PAL_MAX_WINDOWS 16

#define PAL_VMAJOR 1
#define PAL_VMINOR 0
#define PAL_VPATCH 0

// hash entry
typedef struct PalHashEntry {
    Uint32 key;
    void* value;
    struct PalHashEntry* next;
} PalHashEntry;

// hash map
typedef struct PalHashMap {
    PalAllocator* allocator;
    PalHashEntry** data;
    Uint64 count;

} PalHashMap;

typedef struct PalLibrary
{
    // read only 
    PalAllocator tmpAllocator;
    PalVersion version;
    PalAllocator* allocator;
    Uint32 flags;
    bool initialized;

} PalLibrary;

typedef struct PalVideoLibrary
{
    PalDisplay displays[_PAL_MAX_DISPLAYS];
    PalHashMap windowHashMap;
    PalAllocator* allocator;
    PalWindowID nextWindowID;
    int displayCount;
    bool initialized;

} PalVideoLibrary;

typedef struct PalEventQueue
{
    // Not Thread safe
    PalEvent data[_PAL_MAX_EVENTS];
    PalEventDispatchType dispatchTypes[_PAL_MAX_EVENTS];
    PalEventCallback callback;
    int head, tail;

} PalEventQueue;

static PalLibrary s_PAL;
static PalVideoLibrary s_Video;
static PalEventQueue s_EventQueue;

void _palFormatArgs(const char* fmt, va_list argsList, char* buffer);
void _palFormat(char* buffer, const char* fmt, ...);

// platform
void _palPlatformWriteConsole(Uint32 level, const char* msg);
void _palPlatformPollEvents();

bool _palPlatformVideoInit();
void _palPlatformVideoShutdown();

// hash map
PalHashMap _palCreateHashMap(PalAllocator* allocator, Uint64 count);
void _palDestroyHashMap(PalHashMap* map);

bool _palHashMapInsert(PalHashMap* map, Uint32 key, void* value);
void* _palHashMapGet(PalHashMap* map, Uint32 key);
bool _palHashMapPop(PalHashMap* map, Uint32 key);

#endif // _PAL_INTERNAL_H