
#pragma once

#include "PAL_defines.h"

struct PAL_Window;
struct PAL_Device;

enum PAL_DeviceType
{
    PAL_OPENGL_DEVICE = 1,
};

struct PAL_DeviceDesc
{
    u32 type = 0;
    u32 width = 0;
    u32 height = 0;
    b8 debug = false;

    u32 glVersionMajor = 0;
    u32 glVersionMinor = 0;
};

PAL_API PAL_Device* PAL_CreateDevice(PAL_Window* window, PAL_DeviceDesc* device_desc);
PAL_API void PAL_DestroyDevice(PAL_Device* device);

PAL_API void PAL_SetClearColor(PAL_Device* device, f32 r, f32 g, f32 b, f32 a);

PAL_API void PAL_Clear(PAL_Device* device); 
PAL_API void PAL_Present(PAL_Device* device); 