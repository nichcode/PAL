
#include "PAL/PAL.h"

int main(int argc, char** argv)
{
    PAL_Init();
    u32 flags = PAL_APP_WINDOW | PAL_WINDOW_CENTER;
    PAL_Window* window = PAL_CreateWindow("PAL Window", 640, 480, flags);

    PAL_DeviceDesc device_desc;
    device_desc.debug = true;
    device_desc.type = PAL_OPENGL_DEVICE;
    PAL_Device* device = PAL_CreateDevice(window, &device_desc);

    PAL_SetClearColor(device, .2f, .2f, .2f, 1.0f);

    while (!PAL_WindowShouldClose(window)) {
        PAL_PullEvents();

        PAL_Clear(device);
        PAL_Present(device);
    }
    
    PAL_DestroyDevice(device);
    PAL_DestroyWindow(window);
    PAL_Terminate();
    return 0;
}