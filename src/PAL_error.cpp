
#include "PAL_pch.h"
#include "PAL_internal.h"
#include "PAL/PAL.h"

struct PAL_Error
{
    u32 code = PAL_ERROR_NONE;
    std::string msg;
};

static PAL_Error s_Error;

const char* PAL_GetCodeString(u32 error_code) 
{
    switch (error_code) {
        case PAL_PLATFORM_ERROR: { return "PAL_PLATFORM_ERROR"; break; }
        case PAL_OUT_OF_MEMORY: { return "PAL_OUT_OF_MEMORY"; break; }
        case PAL_INVALID_PARAMETER: { return "PAL_INVALID_PARAMETER"; break; }
        case PAL_INVALID_POINTER: { return "PAL_INVALID_POINTER"; break; }
    }
    return "";
}

void PAL_SetError(u32 error_code, b8 print, const char* error_msg, ...)
{
    s_Error.code = error_code;
    va_list arg_ptr;
    va_start(arg_ptr, error_msg);
    char* formatted = PAL_FormatArgs(error_msg, arg_ptr);
    va_end(arg_ptr);

    const char* code_str = PAL_GetCodeString(error_code);
    char* out_msg = PAL_Format("%s: %s\n", code_str, formatted);

    s_Error.msg = std::string(out_msg);
    delete[] formatted;
    delete[] out_msg;

    if (print) { PAL_WriteConsole(PAL_LOG_LEVEL_ERROR, s_Error.msg.c_str()); }
}

u32 PAL_GetError()
{
    return s_Error.code;
}

const char* PAL_GetErrorMessage()
{
    return s_Error.msg.c_str();
}

const char* PAL_GetErrorDescription(u32 error_code)
{
    const char* error_msg = nullptr;
    if (error_code == PAL_ERROR_NONE) { return nullptr; }

    switch (error_code) {
        case PAL_PLATFORM_ERROR: {
            error_msg = "This error occurs when a function or operation fails on the operating system level.";
            break;
        }

        case PAL_OUT_OF_MEMORY: {
            error_msg = "This error occurs when the operating system  \
                        does not have enough memory during allocation.";
            break;
        }

        case PAL_INVALID_PARAMETER: {
            error_msg = "This error occurs when the user pass in a invalid paramter to a function.";
            break;
        }

        case PAL_INVALID_POINTER: {
            error_msg = "This error occurs when the user pass in a invalid pointer to a function.";
            break;
        }
    }
    return error_msg;
}