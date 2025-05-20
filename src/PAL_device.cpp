
#pragma once

#include "PAL_pch.h"
#include "PAL_internal.h"
#include "PAL/PAL_device.h"

#include "opengl/PAL_glapi.h"

#define DEVICE_ERROR(device, ret) if (!device) \
            { PAL_ERROR(PAL_INVALID_POINTER, "Device is null "); return ret; }

inline u32 PAL_GetFormatSize(u32 format)
{
    switch (format)
    {
        case PAL_FORMAT_INT:
        case PAL_FORMAT_FLOAT: {
            return 4;
        }

        case PAL_FORMAT_INT2:
        case PAL_FORMAT_FLOAT2: {
            return 8;
        }

        case PAL_FORMAT_INT3:
        case PAL_FORMAT_FLOAT3: {
            return 12;
        }

        case PAL_FORMAT_INT4:
        case PAL_FORMAT_FLOAT4: {
            return 16;
        }
    }

    PAL_ERROR(PAL_INVALID_PARAMETER, "Invalid element format");
    return 0;
}

PAL_Device* PAL_CreateDevice(PAL_Window* window, PAL_DeviceDesc* device_desc)
{
    PAL_CHECK_INIT()

    if (!window) {
        PAL_ERROR(PAL_INVALID_POINTER, "Window is null");
        return nullptr;
    }

    if (!device_desc) {
        PAL_ERROR(PAL_INVALID_POINTER, "Device desc is null");
        return nullptr;
    }

    PAL_Device* device = new PAL_Device();
    if (!device) {
        PAL_ERROR(PAL_PLATFORM_ERROR, "Failed to allocate memory for the device");
        return nullptr;
    }

    device->type = device_desc->type;
    switch (device->type) {
        case PAL_OPENGL_DEVICE: {
            device->api = new PAL_GLDynAPI();
            device->handle = device->api->CreateDevice(
                PAL_GetWindowHandle(window),
                device_desc
            );
        }
    }

    return device;
}

void PAL_DestroyDevice(PAL_Device* device)
{
    DEVICE_ERROR(device,)
    device->api->DestroyDevice(device->handle);
    delete device->api;
    delete device;
    device = nullptr;
}

void PAL_SetClearColor(PAL_Device* device, f32 r, f32 g, f32 b, f32 a)
{
    DEVICE_ERROR(device,)
    device->api->SetClearColor(device->handle, r, g, b, a);
}

void PAL_Clear(PAL_Device* device)
{
    DEVICE_ERROR(device,)
    device->api->Clear(device->handle);
}

void PAL_Present(PAL_Device* device)
{
    DEVICE_ERROR(device,)
    device->api->Present(device->handle);
}

PAL_PipeLine* PAL_GetPipeLine(PAL_Device* device)
{
    PAL_CHECK_INIT()
    DEVICE_ERROR(device, nullptr)
    return &device->pipeline;
}

void PAL_Flush(PAL_Device* device)
{
    PAL_CHECK_INIT()
    DEVICE_ERROR(device,)

    PAL_Layout* layout = device->pipeline.layout;
    PAL_Buffer* vertexBuffer = device->pipeline.vertexBuffer;
    PAL_Buffer* indexBuffer = device->pipeline.indexBuffer;
    
    // TODO: primitive type
    // TODO: draw type
    // shaders

    if (layout) {
        device->api->SetLayout(layout->handle);
    }

    if (vertexBuffer) {
        if (vertexBuffer->type == PAL_VERTEX_BUFFER) {
            device->api->SetVertexBuffer(
                vertexBuffer->handle,
                device->pipeline.vertexBufferSlot,
                device->pipeline.vertexBufferStride,
                device->pipeline.vertexBufferOffset
            ); 
        }
    }

    // if (layout) {
    //     device->api->SetLayout(layout->handle);
    // }

    if (indexBuffer) {
        if (indexBuffer->type == PAL_INDEX_BUFFER) {
            device->api->SetIndexBuffer(indexBuffer->handle);
        }
    }

    device->api->DrawIndexed(device->handle, device->pipeline.indexCount);
}

PAL_Buffer* PAL_CreateBuffer(PAL_Device* device, PAL_BufferDesc* buffer_desc)
{
    PAL_CHECK_INIT()
    DEVICE_ERROR(device, nullptr)

    if (!buffer_desc) {
        PAL_ERROR(PAL_INVALID_POINTER, "Buffer desc is null");
        return nullptr;
    }

    PAL_Buffer* buffer = new PAL_Buffer();
    if (!buffer) {
        PAL_ERROR(PAL_OUT_OF_MEMORY, "Failed to allocate memory for the buffer");
        return nullptr;
    }

    buffer->device = device;
    buffer->type = buffer_desc->type;
    buffer->usage = buffer_desc->usage;
    buffer->handle = device->api->CreateBuffer(
        device->handle,
        buffer_desc
    );

    return buffer;
}

void PAL_DestroyBuffer(PAL_Buffer* buffer)
{
    if (!buffer) {
        PAL_ERROR(PAL_INVALID_POINTER, "Buffer is null "); return;
    }

    buffer->device->api->DestroyBuffer(buffer->handle);
    delete buffer;
    buffer = nullptr;
}

PAL_Layout* PAL_CreateLayout(PAL_Device* device, PAL_Element* elements, u32 count, u32 binding_index)
{
    PAL_CHECK_INIT()
    DEVICE_ERROR(device, nullptr)

    if (!elements || count <= 0) {
        PAL_ERROR(PAL_INVALID_POINTER, "Elements is null or count is less or equal to zero");
        return nullptr;
    }

    PAL_Layout* layout = new PAL_Layout();
    if (!layout) {
        PAL_ERROR(PAL_OUT_OF_MEMORY, "Failed to allocate memory for layout");
        return nullptr;
    }

    layout->device = device;
    layout->bindingIndex = binding_index;
    layout->count = count;
    layout->stride = 0;
    for (u32 i = 0; i < count; i++) {
        layout->elements[i] = elements[i];
        layout->elements[i].offset = layout->stride;
        layout->stride += PAL_GetFormatSize(elements[i].format);
    }

    layout->handle = device->api->CreateLayout(
        device->handle,
        layout->elements,
        count,
        binding_index
    );

    return layout;
}

void PAL_DestroyLayout(PAL_Layout* layout)
{
    if (!layout) {
        PAL_ERROR(PAL_INVALID_POINTER, "Layout is null "); return;
    }

    layout->device->api->DestroyLayout(
        layout->handle 
    );
    delete layout;
    layout = nullptr;
}

u32 PAL_GetLayoutStride(PAL_Layout* layout)
{
    if (!layout) {
        PAL_ERROR(PAL_INVALID_POINTER, "Layout is null "); return 0;
    }

    return layout->stride;
}