
#pragma once

#include "PAL_defines.h"

struct PAL_Window;
struct PAL_Device;
struct PAL_Buffer;

enum PAL_DeviceType
{
    PAL_OPENGL_DEVICE = 1,
};

enum PAL_BufferType
{
    PAL_VERTEX_BUFFER = 1,
    PAL_INDEX_BUFFER
};

enum PAL_BufferUsage
{
    PAL_USAGE_STATIC = 1,
    PAL_USAGE_DYNAMIC
};

enum PAL_IndexFormat
{
    PAL_INDEX_U32 = 1,
    PAL_INDEX_U16
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

struct PAL_BufferDesc
{
    u32 type = 0;
    u32 usage = 0;
    u32 size = 0;

    u32 indexFormat = PAL_INDEX_U32; // for index buffer only
    u32 indexCount = 0; // for index buffer only
    void* data = nullptr;
};

PAL_API PAL_Device* PAL_CreateDevice(PAL_Window* window, PAL_DeviceDesc* device_desc);
PAL_API void PAL_DestroyDevice(PAL_Device* device);

PAL_API void PAL_SetClearColor(PAL_Device* device, f32 r, f32 g, f32 b, f32 a);

PAL_API void PAL_Clear(PAL_Device* device); 
PAL_API void PAL_Present(PAL_Device* device); 

//****************************************************//
PAL_API PAL_Buffer* PAL_CreateBuffer(PAL_Device* device, PAL_BufferDesc* buffer_desc);
PAL_API void PAL_DestroyBuffer(PAL_Buffer* buffer);

PAL_API void PAL_SetVertexBuffer(PAL_Buffer* vertex_buffer, u32 binding_slot, u32 stride, u32 offset);
PAL_API void PAL_SetIndexBuffer(PAL_Buffer* index_buffer);