
#include "PAL/PAL.h"

int main(int argc, char** argv)
{
    PAL_Init();
    u32 flags = PAL_APP_WINDOW | PAL_WINDOW_CENTER;
    PAL_Window* window = PAL_CreateWindow("PAL Window", 640, 480, flags);

    while (!PAL_WindowShouldClose(window)) {
        PAL_PullEvents();
    }
    
    PAL_DestroyWindow(window);
    PAL_Terminate();
    return 0;
}