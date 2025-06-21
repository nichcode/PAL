
#ifndef PAL_INTERNAL_H
    #define PAL_INTERNAL_H

#include "pal/pal.h"

#define PAL_MSG_SIZE 1024

typedef struct PalLibrary
{
    PalAllocator* allocator;
    PalDebugMessageFn debugCallback;
    Uint32 flags;
    bool initialized;

} PalLibrary;

static PalLibrary s_PAL = {};

void palFormatArgs(const char* fmt, va_list argsList, char* buffer);
void palFormat(char* buffer, const char* fmt, ...);
void palSetError(PAL_Result code, const char* msg, ...);

const char* palGetResultString(PAL_Result code);


#define PAL_SET_ERROR(code, ...) {                       \
    if (s_PAL.flags & PAL_DEBUG_BIT) {                   \
        palSetError(code, __VA_ARGS__);                  \
    }                                                    \
} 

#define PAL_LOG(...) {                                   \
    if (s_PAL.flags & PAL_DEBUG_BIT) {                   \
        palLogInfo(__VA_ARGS__);                         \
    }                                                    \
} 

#endif // PAL_INTERNAL_H