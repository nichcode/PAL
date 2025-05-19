
#include "PAL_pch.h"
#include "PAL_glapi.h"
#include "PAL_glfuncs.h"

#ifdef PAL_PLATFORM_WINDOWS
#include "PAL_glwin32context.h"
#endif // PAL_PLATFORM_WINDOWS

void* PAL_GLDynAPI::CreateDeviceHandle(void* window_handle, PAL_DeviceDesc* desc)
{
    PAL_GLDevice* device = new PAL_GLDevice();
    if (!device) {
        PAL_ERROR(PAL_PLATFORM_ERROR, "Failed to create Opengl device");
    }

    i32 major, minor;
    if (!desc->glVersionMajor) { major = glVersion.major; }
    if (!desc->glVersionMinor) { minor = glVersion.minor; }

    device->context = CreateGLContext(
        window_handle,
        desc->debug,
        major,
        minor
    );

    return device;
}

void PAL_GLDynAPI::DestroyDeviceHandle(void* handle)
{
    PAL_GLDevice* device = (PAL_GLDevice*)handle;
    device->context->Destroy();
    delete device;    
}

void PAL_GLDynAPI::SetClearColor(void* handle, f32 r, f32 g, f32 b, f32 a)
{
    glClearColor(r, g, b, a);
}

void PAL_GLDynAPI::Clear(void* handle)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void PAL_GLDynAPI::Present(void* dhandle)
{
    PAL_GLDevice* device = (PAL_GLDevice*)dhandle;
    device->context->Present();
}

PAL_GLContext* CreateGLContext(void* window_handle, b8 debug, u32 major, u32 minor)
{
#ifdef PAL_PLATFORM_WINDOWS
    return new PAL_GLWin32Context(window_handle, debug, major, minor);
#endif // PAL_PLATFORM_WINDOWS
}
