
#include "pal_pch.h"
#include "pal_hashmap.h"

PalHashMap palCreateHashMap(PalAllocator* allocator, Uint64 count)
{
    PalHashMap map;
    map.count = count;
    map.data = (PalHashEntry**)allocator->alloc(
        count * sizeof(PalHashEntry*)
    );

    palZeroMemory(map.data, count * sizeof(PalHashEntry*));
    map.allocator = allocator;
    return map;
}

void palDestroyHashMap(PalHashMap* map)
{
    for (Uint64 i = 0; i < map->count; ++i) {
        PalHashEntry* entry = map->data[i];
        while (entry) {
            PalHashEntry* next = entry->next;
            map->allocator->free(entry);
            entry = next;
        }
    }
    map->allocator->free(map->data);
    map->data = PAL_NULL;
    map->count = 0;
}

Uint64 palHashUint32(Uint32 key, Uint64 count)
{
    return (key * 2654435761u) % count;
}

bool palHashMapInsert(PalHashMap* map, Uint32 key, void* value)
{
    Uint64 index = palHashUint32(key, map->count);
    PalHashEntry* head = map->data[index];

    for (PalHashEntry* e = head; e; e = e->next) {
        if (e->key == key) {
            e->value = value;
            return PAL_TRUE;
        }
    }

    PalHashEntry* newEntry = (PalHashEntry*)map->allocator->alloc(sizeof(PalHashEntry));
    if (!newEntry) {
        return PAL_FALSE;
    }

    newEntry->key = key;
    newEntry->value = value;
    newEntry->next = head;
    map->data[index] = newEntry;
    return PAL_TRUE;
}

void* palHashMapGet(PalHashMap* map, Uint32 key)
{
    Uint64 index = palHashUint32(key, map->count);
    for (PalHashEntry* e = map->data[index]; e; e = e->next) {
        if (e->key == key) {
            return e->value;
        }
    }
    return PAL_NULL;
}

bool palHashMapPop(PalHashMap* map, Uint32 key)
{
    Uint64 index = palHashUint32(key, map->count);
    PalHashEntry** ptr = &map->data[index];

    while (*ptr) {
        if ((*ptr)->key == key) {
            PalHashEntry* toDelete = *ptr;
            *ptr = toDelete->next;
            map->allocator->free(toDelete);
            return PAL_TRUE;
        }
        ptr = &(*ptr)->next;
    }
    return PAL_FALSE;
}