
#ifndef PAL_PLATFORM_H
#define PAL_PLATFORM_H

#include <stdarg.h>

#include "PAL/PAL.h"

#define PAL_MESSAGE_SIZE 1024

struct PAL_Callbacks
{
    PAL_CloseFun close;
};

struct PAL_PlatformData
{
    char errorString[PAL_MESSAGE_SIZE] = {};
    PAL_Allocator defaultAllocator;
    PAL_Allocator* allocator = nullptr;
    PAL_ErrorFun errorCallback = nullptr;
    void* eventsData = nullptr;
    Uint32 errorCode = 0;
    bool debug, initialized, customAllocator  = false;
    bool eventPolling = false;
};

static PAL_PlatformData s_PAL;
static PAL_Callbacks s_Callbacks;

//**********************************************
//************Platform Specific API*************
//**********************************************

bool PAL_PlatformInit();
void PAL_PlatformTerminate();

void PAL_WriteConsole(Uint32 level, const char* msg);
void PAL_PlatformPollEvents();
void PAL_PlatformWaitEvents();
PAL_Allocator PAL_PlatformGetAllocator();

//**********************************************
//************Platform Agnostic API*************
//**********************************************

void PAL_FormatArgs(const char* fmt, va_list argsList, char* buffer);
void PAL_Format(char* buffer, const char* fmt, ...);
const char* PAL_GetErrorString(Uint32 code);

#define PAL_CHECK_INIT(ret) {                                       \
    if (!s_PAL.initialized) {                                       \
        PAL_SetError(PAL_PLATFORM_ERROR, "PAL is not initialized"); \
        return ret;                                                 \
    }                                                               \
} 

#endif // PAL_PLATFORM_H