
#ifndef PAL_ERROR_H
#define PAL_ERROR_H

#include "PAL_defines.h"

typedef void (*PAL_ErrorFun)(Uint32, const char*);

typedef enum PAL_Error
{
    PAL_NO_ERROR,
    PAL_ALLOCATION_ERROR,
    PAL_PLATFORM_ERROR,
    PAL_INVALID_ERROR

} PAL_Error;

PAL_API void PAL_SetErrorCallback(PAL_ErrorFun callback);
PAL_API PAL_BOOL PAL_SetError(Uint32 code, const char* fmt, ...);

PAL_API PAL_ErrorFun PAL_GetErrorCallback();
PAL_API Uint32 PAL_GetError();
PAL_API const char* PAL_GetErrorMessage();

#endif // PAL_ERROR_H