
#ifndef PAL_LOG_H
#define PAL_LOG_H

#include "PAL_defines.h"

typedef enum PAL_LogLevels
{
    PAL_LOG_TRACE,
    PAL_LOG_INFO,
    PAL_LOG_WARN,
    PAL_LOG_ERROR

} PAL_LogLevels;

PAL_API void PAL_Log(Uint32 level, const char* fmt, ...);

PAL_API void PAL_LogTrace(const char* fmt, ...);
PAL_API void PAL_LogInfo(const char* fmt, ...);
PAL_API void PAL_LogWarn(const char* fmt, ...);
PAL_API void PAL_LogError(const char* fmt, ...);

#endif // PAL_LOG_H