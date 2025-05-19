
#pragma once

#include "PAL_api.h"

class PAL_GLContext
{
public:
    virtual void Destroy() = 0;
    virtual void Present() = 0;
};

struct PAL_GLDevice
{
    PAL_GLContext* context = nullptr;
};

class PAL_GLDynAPI : public PAL_DynAPI
{
public:
    void* CreateDeviceHandle(void* window_handle, PAL_DeviceDesc* device_desc) override;
    void DestroyDeviceHandle(void* handle) override;

    void SetClearColor(void* handle, f32 r, f32 g, f32 b, f32 a) override;

    void Clear(void* handle) override;
    void Present(void* device_handle) override;
};

PAL_GLContext* CreateGLContext(void* window_handle, b8 debug, u32 major, u32 minor);