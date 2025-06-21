
#ifndef PAL_LOG_H
    #define PAL_LOG_H

#include "pal_defines.h"

typedef enum PAL_LogLevel
{
    PAL_LOG_TRACE,
    PAL_LOG_INFO,
    PAL_LOG_WARN,
    PAL_LOG_ERROR

} PAL_LogLevel;

PAL_API void PAL_CALL palLog(PAL_LogLevel level, const char* fmt, ...);

PAL_API void PAL_CALL palLogTrace(const char* fmt, ...);
PAL_API void PAL_CALL palLogInfo(const char* fmt, ...);
PAL_API void PAL_CALL palLogWarn(const char* fmt, ...);
PAL_API void PAL_CALL palLogError(const char* fmt, ...);

#endif // PAL_LOG_H