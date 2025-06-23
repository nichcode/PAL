
#ifndef _PAL_INTERNAL_H
 #define _PAL_INTERNAL_H

#include "pal/pal_core.h"

#define MAX_LOG_SIZE 1024

// a helper function
void _PCALL palLogInternal(PalLogLevel level, const char* fmt, ...);

#define PAL_TRACE(...)    palLogInternal(PAL_LOG_TRACE, __VA_ARGS__);
#define PAL_INFO(...)     palLogInternal(PAL_LOG_INFO, __VA_ARGS__);
#define PAL_WARN(...)     palLogInternal(PAL_LOG_WARN, __VA_ARGS__);
#define PAL_ERROR(...)    palLogInternal(PAL_LOG_ERROR, __VA_ARGS__);

#endif // _PAL_INTERNAL_H