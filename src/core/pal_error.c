
#include "pal_pch.h"
#include "pal/pal_core.h"

static PalTLSID s_ErrorTLSID = 0;
typedef struct ErrorTLSData
{
    PalError error;

} ErrorTLSData;

void _PCALL palSetError(
    PalError error)
{
    ErrorTLSData* data = palGetTLS(s_ErrorTLSID);
    if (!data) {
        data = palAlloc(PAL_NULL, sizeof(ErrorTLSData));
    }
    data->error = error;
    palSetTLS(s_ErrorTLSID, data);
}

PalError _PCALL palGetError()
{
    ErrorTLSData* data = palGetTLS(s_ErrorTLSID);
    if (!data) {
        return PAL_ERROR_NONE;
    }
    return data->error;
}

const char* _PCALL palErrorToString(
    PalError error)
{
    switch (error)
    {
    case PAL_ERROR_NONE:
    return "No Error";

    case PAL_ERROR_NULL_POINTER:
    return "Null pointer";

    case PAL_ERROR_INVALID_ARGUMENT:
    return "Invalid argument";

    case PAL_ERROR_INVALID_ALLOCATOR:
    return "Invalid Allocator";

    case PAL_ERROR_ALLOCATION_FAILED:
    return "Allocation failed";

    case PAL_ERROR_INVALID_DISPLAY:
    return "Invalid display";

    case PAL_ERROR_VIDEO_DEVICE_NOT_FOUND:
    return "Video device not found";

    }
}