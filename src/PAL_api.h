
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

    virtual void Draw(void* handle, u32 count) = 0;
    virtual void DrawInstanced(void* handle, u32 count, u32 instance_count) = 0;
    virtual void DrawIndexed(void* handle, u32 count) = 0;
    virtual void DrawIndexedInstance(void* handle, u32 count, u32 instance_count) = 0;

    // buffer
    virtual void* CreateBuffer(void* device_handle, PAL_BufferDesc* desc) = 0;
    virtual void DestroyBuffer(void* handle) = 0;
    virtual void SetVertexBuffer(void* handle, u32 binding_slot, u32 stride, u32 offset) = 0;
    virtual void SetIndexBuffer(void* handle) = 0;

    // layout
    virtual void* CreateLayout(void* device_handle, PAL_Element* elements, u32 count, u32 binding_index) = 0;
    virtual void DestroyLayout(void* handle) = 0;
    virtual void SetLayout(void* handle) = 0;

    // shaders
    virtual void* CreateVertexShader(void* device_handle, void* layout_handle, const char* source, b8 load) = 0;
    virtual void DestroyVertexShader(void* handle) = 0;
    virtual void* CreatePixelShader(void* device_handle, const char* source, b8 load) = 0;
    virtual void DestroyPixelShader(void* handle) = 0;
    virtual void SetShaders(void* device, void* vertex_shader, void* pixel_shader) = 0;
};