
#include "pal_pch.h"
#include "pal_internal.h"

Uint64 hash(Uint32 key, Uint64 count);

void _palFormatArgs(const char* fmt, va_list argsList, char* buffer)
{
    va_list listCopy;
#ifdef _MSC_VER
        listCopy = argsList;
#elif defined(P_PLATFORM_APPLE)
        listCopy = argsList;
#else
        __builtin_va_copy(listCopy, argsList);
#endif // _MSC_VER

    int len = vsnprintf(0, 0, fmt, listCopy);
    vsnprintf(buffer, len + 1, fmt, listCopy);
    buffer[len] = 0;
}

void _palFormat(char* buffer, const char* fmt, ...)
{
    va_list argPtr;
    va_start(argPtr, fmt);
    _palFormatArgs(fmt, argPtr, buffer);
    va_end(argPtr);
}

PalHashMap _palCreateHashMap(PalAllocator* allocator, Uint64 count)
{
    PalHashMap map;
    map.allocator = allocator;
    map.count = count;
    map.data = (PalHashEntry**)map.allocator->alloc(
        count * sizeof(PalHashEntry*)
    );

    palZeroMemory(map.data, count * sizeof(PalHashEntry*));
    return map;
}

void _palDestroyHashMap(PalHashMap* map)
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
    map->allocator = PAL_NULL;
}

bool _palHashMapInsert(PalHashMap* map, Uint32 key, void* value)
{
    Uint64 index = hash(key, map->count);
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

void* _palHashMapGet(PalHashMap* map, Uint32 key)
{
    Uint64 index = hash(key, map->count);
    for (PalHashEntry* e = map->data[index]; e; e = e->next) {
        if (e->key == key) {
            return e->value;
        }
    }
    return PAL_NULL;
}

bool _palHashMapPop(PalHashMap* map, Uint32 key)
{
    Uint64 index = hash(key, map->count);
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

Uint64 hash(Uint32 key, Uint64 count) 
{
    // Knuth's multiplicative hash
    return (key * 2654435761u) % count;  
}