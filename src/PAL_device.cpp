
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
        PAL_ERROR(PAL_PLATFORM_ERROR, "Failed to create Device");
        return nullptr;
    }

    device->type = device_desc->type;
    switch (device->type) {
        case PAL_OPENGL_DEVICE: {
            device->api = new PAL_GLDynAPI();
            device->handle = device->api->CreateDeviceHandle(
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
    device->api->DestroyDeviceHandle(device->handle);
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