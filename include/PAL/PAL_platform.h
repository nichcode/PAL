
#pragma once

#include "PAL_defines.h"
#include <stdarg.h>

PAL_API void PAL_Init();
PAL_API void PAL_Terminate();

PAL_API char* PAL_ToString(const wchar_t* wide_string);
PAL_API wchar_t* PAL_ToWideString(const char* string);

PAL_API char* PAL_FormatArgs(const char* fmt, va_list args_list);
PAL_API char* PAL_Format(const char* fmt, ...);

PAL_API void* PAL_LoadLibrary(const char* dll);
PAL_API void* PAL_GetProcAddress(void* dll, const char* func_name);
PAL_API void PAL_FreeLibrary(void* dll);