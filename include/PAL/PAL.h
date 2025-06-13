
#ifndef PAL_H
#define PAL_H

#include "PAL_allocator.h"

typedef enum PAL_InitFlags
{
    PAL_INIT_FLAGS_NONE,
    PAL_INIT_FLAGS_DEBUG = PAL_BIT(0),
    PAL_INIT_FLAGS_CUSTOM_ALLOCATOR = PAL_BIT(1)

} PAL_InitFlags;

typedef struct PAL_InitDesc
{
    Uint32 flags;
    PAL_Allocator* allocator;

} PAL_InitDesc;

PAL_API PAL_BOOL PAL_Init(PAL_InitDesc* desc);
PAL_API void PAL_Terminate();

#endif // PAL_H