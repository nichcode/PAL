
#ifndef _PAL_HASHMAP_H
 #define _PAL_HASHMAP_H

#include "pal/pal_core.h"

typedef struct PalHashEntry {
    Uint32 key;
    void* value;
    struct PalHashEntry* next;
} PalHashEntry;

typedef struct PalHashMap {
    PalHashEntry** data;
    PalAllocator* allocator;
    Uint64 count;

} PalHashMap;

PalHashMap palCreateHashMap(PalAllocator* allocator, Uint64 count);
void palDestroyHashMap(PalHashMap* map);
Uint64 palHashUint32(Uint32 key, Uint64 count);

bool palHashMapInsert(PalHashMap* map, Uint32 key, void* value);
void* palHashMapGet(PalHashMap* map, Uint32 key);
bool palHashMapPop(PalHashMap* map, Uint32 key);

#endif // _PAL_HASHMAP_H