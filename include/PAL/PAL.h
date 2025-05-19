
#pragma once

#include "PAL_error.h"
#include "PAL_log.h"
#include "PAL_input.h"
#include "PAL_window.h"

PAL_API void PAL_Init();
PAL_API void PAL_Terminate();

PAL_API char* PAL_ToString(const wchar_t* wide_string);
PAL_API wchar_t* PAL_ToWideString(const char* string);

PAL_API char* PAL_Format(const char* fmt, ...);