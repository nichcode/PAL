
#ifndef PAL_H
#define PAL_H

#include "PAL_allocator.h"
#include "PAL_error.h"
#include "PAL_log.h"

typedef enum PAL_InitFlags
{
    PAL_DEBUG = PAL_BIT(0),
    PAL_CUSTOM_ALLOCATOR = PAL_BIT(1)

} PAL_InitFlags;

PAL_API PAL_BOOL PAL_Init(Uint32 flags);
PAL_API void PAL_Terminate();

#endif // PAL_H