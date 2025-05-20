
#pragma once

#include "PAL_pch.h"
#include "PAL_internal.h"
#include "PAL/PAL_device.h"

#include "opengl/PAL_glapi.h"

#define DEVICE_ERROR(device, ret) if (!device) \
            { PAL_ERROR(PAL_INVALID_POINTER, "Device is null "); return ret; }

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


//************************buffer**************************
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

void PAL_SetVertexBuffer(PAL_Buffer* vertex_buffer, u32 binding_slot, u32 stride, u32 offset)
{
    if (!vertex_buffer) {
        PAL_ERROR(PAL_INVALID_POINTER, "Vertexbuffer is null"); return;
    }

    if (!vertex_buffer->type == PAL_VERTEX_BUFFER) {
        PAL_ERROR(PAL_INVALID_POINTER, "The specified buffer is not a vertex buffer"); return;
    }

    vertex_buffer->device->api->SetVertexBuffer(
        vertex_buffer->handle,
        binding_slot,
        stride,
        offset
    );
}

void PAL_SetIndexBuffer(PAL_Buffer* index_buffer)
{
    if (!index_buffer) {
        PAL_ERROR(PAL_INVALID_POINTER, "Indexbuffer is null "); return;
    }

    if (!index_buffer->type == PAL_INDEX_BUFFER) {
        PAL_ERROR(PAL_INVALID_POINTER, "The specified buffer is not an index buffer"); return;
    }

    index_buffer->device->api->SetIndexBuffer(
        index_buffer->handle
    );

}