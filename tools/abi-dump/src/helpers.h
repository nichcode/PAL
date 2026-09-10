
#ifndef HELPERS_H
#define HELPERS_H

#include "pal2/pal_core.h"
#include <stddef.h>
#include <stdbool.h>

#ifdef _MSC_VER
#define ALIGNOF(type) __alignof(type)
#else
#define ALIGNOF(type) __alignof__(type)
#endif // _MSC_VER

#define ABI_DUMP_CORE (1u << 0)
#define ABI_DUMP_EVENT (1u << 1)
#define ABI_DUMP_THREAD (1u << 2)
#define ABI_DUMP_OPENGL (1u << 3)
#define ABI_DUMP_GRAPHICS (1u << 4)
#define ABI_DUMP_SYSTEM (1u << 5)
#define ABI_DUMP_VIDEO (1u << 6)
#define ABI_DUMP_VERBOSE (1u << 7)
#define ABI_DUMP_QUICK (1u << 8)
#define ABI_DUMP_ALL 0x7F

extern uint32_t g_DumpFlags;

typedef struct FieldBase
{
    uint32_t offset;
    uint32_t size;
} FieldBase;

typedef struct StructBase
{
    uint32_t align;
    uint32_t size;
    uint32_t padding;
} StructBase;

typedef struct FieldInfo
{
    const char* name;
    FieldBase expected;
    FieldBase actual;
} FieldInfo;

typedef struct StructInfo
{
    const char* name;
    FieldInfo* fields;
    uint32_t fieldCount;
    StructBase expected;
    StructBase actual;
} StructInfo;

#define ARRAY_SIZE(array) (sizeof(array) / sizeof((array)[0]))
#define PADDING(type) (((ALIGNOF(type) - sizeof(type)) % ALIGNOF(type)))
#define FIELD(type, field) ((FieldBase){offsetof(type, field), sizeof(((type*)0)->field)})
#define STRUCT(type) ((StructBase){ALIGNOF(type), sizeof(type), PADDING(type)})

bool dumpStruct(const StructInfo* info);

/**
 * We add a high level function that calls its system dump functions. This is
 * here to avoid bloating the main.c with dumps (eg. allocator dump, etc)
*/
bool coreStructs();
// bool eventStructs();
// bool threadStructs();
// bool systemStructs();
// bool videoStructs();
// bool openglStructs();
// bool graphicsStructs();

#endif // HELPERS_H