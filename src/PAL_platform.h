
#ifndef PAL_PLATFORM_H
#define PAL_PLATFORM_H

#include <stdarg.h>

#include "PAL/PAL.h"

#define PAL_MESSAGE_SIZE 1024

struct PAL_PlatformData
{
    char errorString[PAL_MESSAGE_SIZE] = {};
    PAL_Allocator defaultAllocator;
    PAL_Allocator* allocator = nullptr;
    PAL_ErrorFun errorCallback = nullptr;
    Uint32 errorCode = 0;
    bool debug = false;
};

static PAL_PlatformData s_PAL;

//**********************************************
//************Platform Specific API*************
//**********************************************

PAL_Allocator PAL_PlatformGetAllocator();

//**********************************************
//************Platform Agnostic API*************
//**********************************************

void PAL_FormatArgs(const char* fmt, va_list argsList, char* buffer);
void PAL_Format(char* buffer, const char* fmt, ...);

#endif // PAL_PLATFORM_H