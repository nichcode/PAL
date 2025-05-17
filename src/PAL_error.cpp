
#include "PAL_pch.h"
#include "PAL_internal.h"

static u32 s_Error = 0;

void PAL_SetError(u32 error_code)
{
    s_Error = error_code;
}

u32 PAL_GetError()
{
    return s_Error;
}

const char* PAL_GetErrorMessage(u32 error)
{
    const char* error_msg = nullptr;
    if (error == PAL_ERROR_NONE) { return nullptr; }

    switch (error) {
        case PAL_PLATFORM_ERROR: {
            error_msg = "This error occurs when a function or operation fails on the operating system.";
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