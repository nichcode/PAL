
#pragma once

#include "PAL/PAL_device.h"

class PAL_DynAPI
{
public:
    virtual void* CreateDeviceHandle(void* window_handle, PAL_DeviceDesc* desc) = 0;
    virtual void DestroyDeviceHandle(void* handle) = 0;

    virtual void SetClearColor(void* handle, f32 r, f32 g, f32 b, f32 a) = 0;

    virtual void Clear(void* handle) = 0;
    virtual void Present(void* handle) = 0;
};