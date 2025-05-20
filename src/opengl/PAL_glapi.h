
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

struct PAL_GLBuffer
{
    u32 id = 0;
    u32 size = 0;
    u32 usage = 0;    
    void* data =  nullptr;
    b8 dataSent = false;
};

class PAL_GLDynAPI : public PAL_DynAPI
{
public:
    void* CreateDevice(void* window_handle, PAL_DeviceDesc* device_desc) override;
    void DestroyDevice(void* handle) override;

    void SetClearColor(void* handle, f32 r, f32 g, f32 b, f32 a) override;

    void Clear(void* handle) override;
    void Present(void* device_handle) override;

    void* CreateBuffer(void* device_handle, PAL_BufferDesc* desc) override;
    void DestroyBuffer(void* handle) override;
    void SetVertexBuffer(void* handle, u32 binding_slot, u32 stride, u32 offset) override;
    void SetIndexBuffer(void* handle) override;
};

PAL_GLContext* CreateGLContext(void* window_handle, b8 debug, u32 major, u32 minor);