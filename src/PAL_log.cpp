
#pragma once

#include "PAL_pch.h"
#include "PAL_internal.h"
#include "PAL/PAL_platform.h"

static const char* levels[4] = { "[TRACE]: ", "[INFO]: ", "[WARN]: ", "[ERROR]: " };

void PAL_Log(u32 log_level, const char* msg, ...)
{
    va_list arg_ptr;
    va_start(arg_ptr, msg);
    char* string = PAL_FormatArgs(msg, arg_ptr);
    va_end(arg_ptr);

    char* out_string = PAL_Format("%s%s\n", levels[log_level], string);
    PAL_WriteConsole(log_level, out_string);

    delete[] string;
    delete[] out_string;
}

void PAL_LogTrace(const char* msg, ...)
{
    va_list arg_ptr;
    va_start(arg_ptr, msg);
    char* string = PAL_FormatArgs(msg, arg_ptr);
    va_end(arg_ptr);

    char* out_string = PAL_Format("%s%s\n", levels[PAL_LOG_LEVEL_TRACE], string);
    PAL_WriteConsole(PAL_LOG_LEVEL_TRACE, out_string);

    delete[] string;
    delete[] out_string;
}

void PAL_LogInfo(const char* msg, ...)
{
    va_list arg_ptr;
    va_start(arg_ptr, msg);
    char* string = PAL_FormatArgs(msg, arg_ptr);
    va_end(arg_ptr);

    char* out_string = PAL_Format("%s%s\n", levels[PAL_LOG_LEVEL_INFO], string);
    PAL_WriteConsole(PAL_LOG_LEVEL_INFO, out_string);

    delete[] string;
    delete[] out_string;
}

void PAL_LogWarn(const char* msg, ...)
{
    va_list arg_ptr;
    va_start(arg_ptr, msg);
    char* string = PAL_FormatArgs(msg, arg_ptr);
    va_end(arg_ptr);

    char* out_string = PAL_Format("%s%s\n", levels[PAL_LOG_LEVEL_WARN], string);
    PAL_WriteConsole(PAL_LOG_LEVEL_WARN, out_string);

    delete[] string;
    delete[] out_string;
}

void PAL_LogError(const char* msg, ...)
{
    va_list arg_ptr;
    va_start(arg_ptr, msg);
    char* string = PAL_FormatArgs(msg, arg_ptr);
    va_end(arg_ptr);

    char* out_string = PAL_Format("%s%s\n", levels[PAL_LOG_LEVEL_ERROR], string);
    PAL_WriteConsole(PAL_LOG_LEVEL_ERROR, out_string);

    delete[] string;
    delete[] out_string;
}