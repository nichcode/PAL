
#pragma once

#include "PAL/PAL_device.h"

class PAL_DynAPI
{
public:
    virtual void* CreateDevice(void* window_handle, PAL_DeviceDesc* desc) = 0;
    virtual void DestroyDevice(void* handle) = 0;
    virtual void SetClearColor(void* handle, f32 r, f32 g, f32 b, f32 a) = 0;
    virtual void Clear(void* handle) = 0;
    virtual void Present(void* handle) = 0;

    // buffer
    virtual void* CreateBuffer(void* device_handle, PAL_BufferDesc* desc) = 0;
    virtual void DestroyBuffer(void* handle) = 0;
    virtual void SetVertexBuffer(void* handle, u32 binding_slot, u32 stride, u32 offset) = 0;
    virtual void SetIndexBuffer(void* handle) = 0;
};