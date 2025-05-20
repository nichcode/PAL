
#pragma once

#include "PAL_defines.h"

struct PAL_Window;
struct PAL_Device;
struct PAL_Buffer;
struct PAL_Layout;
struct PAL_VertexShader;
struct PAL_PixelShader;

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

enum PAL_ElementType
{
    PAL_PER_VERTEX = 1,
    PAL_PER_INSTANCE
};

enum PAL_ElementFormat
{
    PAL_FORMAT_INT = 1,
    PAL_FORMAT_INT2,
    PAL_FORMAT_INT3,
    PAL_FORMAT_INT4,
    PAL_FORMAT_FLOAT,
    PAL_FORMAT_FLOAT2,
    PAL_FORMAT_FLOAT3,
    PAL_FORMAT_FLOAT4
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

struct PAL_Element
{
    const char* name = nullptr;
    u32 format = 0;
    u32 index = 0;
    u32 offset = 0; // internal use only
    u32 type = 0;
    u32 instanceStepRate = 0;
};

struct PAL_PipeLine
{
    PAL_Layout* layout = nullptr;
    PAL_Buffer* vertexBuffer = nullptr;
    PAL_Buffer* indexBuffer = nullptr;
    PAL_VertexShader* vertexShader = nullptr;
    PAL_PixelShader* pixelShader = nullptr;

    u32 vertexBufferSlot = 0;
    u32 vertexBufferStride = 0;
    u32 vertexBufferOffset = 0;
    u32 indexCount = 0;
};

PAL_API PAL_Device* PAL_CreateDevice(PAL_Window* window, PAL_DeviceDesc* device_desc);
PAL_API void PAL_DestroyDevice(PAL_Device* device);

PAL_API void PAL_SetClearColor(PAL_Device* device, f32 r, f32 g, f32 b, f32 a);

PAL_API void PAL_Clear(PAL_Device* device); 
PAL_API void PAL_Present(PAL_Device* device); 

PAL_API PAL_PipeLine* PAL_GetPipeLine(PAL_Device* device);
PAL_API void PAL_Flush(PAL_Device* device);

PAL_API PAL_Buffer* PAL_CreateBuffer(PAL_Device* device, PAL_BufferDesc* buffer_desc);
PAL_API void PAL_DestroyBuffer(PAL_Buffer* buffer);

PAL_API PAL_Layout* PAL_CreateLayout(PAL_Device* device, PAL_Element* elements, u32 count, u32 binding_index);
PAL_API void PAL_DestroyLayout(PAL_Layout* layout);
PAL_API u32 PAL_GetLayoutStride(PAL_Layout* layout);

PAL_API PAL_VertexShader* PAL_CreateVertexShader(PAL_Device* device, PAL_Layout* layout, const char* source, b8 load);
PAL_API void PAL_DestroyVertexShader(PAL_VertexShader* vertex_shader);

PAL_API PAL_PixelShader* PAL_CreatePixelShader(PAL_Device* device, const char* source, b8 load);
PAL_API void PAL_DestroyPixelShader(PAL_PixelShader* pixel_shader);