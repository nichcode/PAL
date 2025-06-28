
#ifndef _PAL_SHARED_H
 #define _PAL_SHARED_H

#define PAL_MSG_SIZE 1024
#define PAL_MAX_DISPLAYS 16
#define PAL_MAX_DISPLAY_MODES 64
#define PAL_DISPLAY_NAME_SIZE 32
#define PAL_MAX_EVENTS 256
#define PAL_MAX_WINDOWS 16

#define PAL_VERSION_MAJOR 1
#define PAL_VERSION_MINOR 0
#define PAL_VERSION_PATCH 0

#include "pal/pal_core.h"

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

typedef struct PalWindow
{
    void* handle;
    const char* title;
    Uint32 id;

    Uint32 flags;
    Uint32 style;
    Uint32 exStyle;
    Uint32 width;
    Uint32 height;
    int x;
    int y;

    bool maximized;
    bool minimized;
    bool fullscreen;
    bool hidden;

} PalWindow;

void palFormatArgs(const char* fmt, va_list argsList, char* buffer);
void palFormat(char* buffer, const char* fmt, ...);

PalHashMap palCreateHashMap(PalAllocator* allocator, Uint64 count);
void palDestroyHashMap(PalHashMap* map);
Uint64 palHashUint32(Uint32 key, Uint64 count);

bool palHashMapInsert(PalHashMap* map, Uint32 key, void* value);
void* palHashMapGet(PalHashMap* map, Uint32 key);
bool palHashMapPop(PalHashMap* map, Uint32 key);

void palWriteConsole(Uint32 level, const char* msg);
bool palRegisterWindowClass();
void palUnregisterWindowClass();
bool palSetAllocator(PalAllocator* dest, const PalAllocator* src);

#endif // _PAL_SHARED_H