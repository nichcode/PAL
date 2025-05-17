
#pragma once

#include "PAL/PAL_log.h"
#include "PAL/PAL_error.h"

#include <stdarg.h>

void PAL_SetError(u32 error_code);
char* PAL_FormatArgs(const char* fmt, va_list args_list);

i32 PAL_MultibyteToWchar(const char* str, u32 str_len, wchar_t* wstr);
i32 PAL_WcharToMultibyte(const wchar_t* wstr, u32 wstr_len, char* str);

void PAL_WriteConsole(u32 log_level, const char* msg);

#ifdef PAL_CONFIG_DEBUG
#define PAL_ERROR(error) PAL_SetError(error); PAL_BREAK;
#define PAL_LOG_TRACE(...)   PAL_LogTrace(__VA_ARGS__)
#define PAL_LOG_INFO(...)    PAL_LogInfo(__VA_ARGS__)
#define PAL_LOG_WARN(...)    PAL_LogWarn(__VA_ARGS__)
#define PAL_LOG_ERROR(...)   PAL_LogError(__VA_ARGS__)
#else
#define PAL_ERROR(error) PAL_SetError(error);
#define PAL_LOG_TRACE(...)   
#define PAL_LOG_INFO(...)    
#define PAL_LOG_WARN(...)    
#define PAL_LOG_ERROR(...) 
#endif // PAL_CONFIG_DEBUG