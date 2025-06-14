
#ifndef PAL_H
#define PAL_H

#include "PAL_allocator.h"
#include "PAL_error.h"
#include "PAL_events.h"
#include "PAL_log.h"
#include "PAL_window.h"

typedef enum PAL_InitFlags
{
    PAL_DEBUG = PAL_BIT(1),
    PAL_CUSTOM_ALLOCATOR = PAL_BIT(2),
    PAL_EVENT_POLLING = PAL_BIT(3), // not used yet

} PAL_InitFlags;

PAL_API PAL_BOOL PAL_Init(Uint32 flags);
PAL_API void PAL_Terminate();

#endif // PAL_H