
#include "pal_pch.h"
#include "pal/pal_core.h"

static PalTLSID s_ErrorTLSID = 0;

typedef struct ErrorTLSData
{
    PalError error;

} ErrorTLSData;

void _PCALL palSetError(PalError error)
{
    ErrorTLSData* data = palGetTLS(s_ErrorTLSID);
    if (!data) {
        data = palAlloc(sizeof(ErrorTLSData));
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

const char* _PCALL palFormatError(PalError error)
{
    switch (error) {
        case PAL_ERROR_NONE:
        return "The operation was successful";

        case PAL_NULL_POINTER:
        return "The pointer is invalid";
        
        case PAL_INVALID_ALLOCATOR:
        return "Allocator function pointers are not set";
        
        case PAL_VIDEO_NOT_INITIALIZED:
        return "Video system is not intialized";
        
        case PAL_OUT_OF_MEMORY:
        return "Out of memory";
        
        case PAL_DISPLAY_ERROR:
        return "The display index is invalid";
    }

    return PAL_NULL;
}