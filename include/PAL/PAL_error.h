
#pragma once

#include "PAL_defines.h"

enum PAL_ErrorCodes
{
    PAL_ERROR_NONE = 0,
    PAL_PLATFORM_ERROR,
    PAL_OUT_OF_MEMORY,
    PAL_INVALID_PARAMETER,
    PAL_INVALID_POINTER,
    PAL_INVALID_VALUE,
};

PAL_API u32 PAL_GetError();
PAL_API const char* PAL_GetErrorMessage(u32 error);