
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