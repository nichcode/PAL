
#ifndef _PAL_LOG_H
 #define _PAL_LOG_H

#include "pal_defines.h"

typedef enum PalLogLevel
{
    PAL_LOG_LEVEL_TRACE,
    PAL_LOG_LEVEL_INFO,
    PAL_LOG_LEVEL_WARN,
    PAL_LOG_LEVEL_ERROR

} PalLogLevel;

_PAPI void _PCALL palLog(PalLogLevel level, const char* fmt, ...);

#define palLogTrace(...)    palLog(PAL_LOG_LEVEL_TRACE, __VA_ARGS__);
#define palLogInfo(...)     palLog(PAL_LOG_LEVEL_INFO, __VA_ARGS__);
#define palLogWarn(...)     palLog(PAL_LOG_LEVEL_WARN, __VA_ARGS__);
#define palLogError(...)    palLog(PAL_LOG_LEVEL_ERROR, __VA_ARGS__);

#endif // _PAL_LOG_H
