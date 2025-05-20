
#pragma once

#include "PAL/PAL_log.h"
#include "PAL/PAL_error.h"
#include "PAL/PAL_window.h"
#include "PAL_api.h"

struct Callbacks
{
    PAL_WindowCloseFun close = nullptr;
    PAL_WindowPosFun pos = nullptr;
    PAL_WindowSizeFun size = nullptr;

    PAL_KeyFun key = nullptr;

    PAL_MouseButtonFun mouseButton = nullptr;
    PAL_MousePosFun mousePos = nullptr;
    PAL_ScrollFun scroll = nullptr;
};

struct PAL_Device
{
    PAL_DynAPI* api;
    PAL_PipeLine pipeline;
    void* handle = nullptr;
    u32 type = 0;
};

struct PAL_Buffer
{
    PAL_Device* device = nullptr;
    void* handle = nullptr;
    u32 type = 0;
    u32 usage = 0;
};

struct PAL_Layout
{
    PAL_Device* device = nullptr;
    PAL_Element elements[PAL_MAX_SLOTS];
    void* handle = nullptr;
    u32 stride = 0;
    u32 count = 0;
    u32 bindingIndex = 0;
};

struct PAL_VertexShader
{
    PAL_Device* device = nullptr;
    void* handle = nullptr;
};

struct PAL_PixelShader
{
    PAL_Device* device = nullptr;
    void* handle = nullptr;
};

struct PAL_Data
{
    Callbacks callbacks;
    PAL_Window* activeWindow = nullptr;
    void* opengl = nullptr;
    void* gdi = nullptr;

    i32 windowCount = 0;
    u32 directXFlags = 0;
    b8 init = false;
};

static PAL_Data s_Data;

void PAL_SetError(u32 error_code, b8 print, const char* error_msg, ...);
void PAL_WriteConsole(u32 log_level, const char* msg);
void PAL_InitInput();

#ifdef PAL_CONFIG_DEBUG
#define PAL_CHECK_INIT() if (!s_Data.init) \
            { PAL_SetError(PAL_PLATFORM_ERROR, true, "PAL is not initialized"); PAL_BREAK; }

#define PAL_ERROR(error_code, ...) PAL_SetError(error_code, true, __VA_ARGS__); PAL_BREAK;
#define PAL_LOG_TRACE(...)   PAL_LogTrace(__VA_ARGS__)
#define PAL_LOG_INFO(...)    PAL_LogInfo(__VA_ARGS__)
#define PAL_LOG_WARN(...)    PAL_LogWarn(__VA_ARGS__)
#define PAL_LOG_ERROR(...)   PAL_LogError(__VA_ARGS__)
#else
#define PAL_CHECK_INIT() if (!s_Data.init) \
            { PAL_SetError(PAL_PLATFORM_ERROR, true, "PAL is not initialized"); }

#define PAL_ERROR(error, ...) PAL_SetError(error_code, false, __VA_ARGS__);
#define PAL_LOG_TRACE(...)   
#define PAL_LOG_INFO(...)    
#define PAL_LOG_WARN(...)    
#define PAL_LOG_ERROR(...) 
#endif // PAL_CONFIG_DEBUG