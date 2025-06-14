
#include "tests.h"

void onWindowClose(PAL_Window* windo)
{
    PAL_LogInfo("Window close button clicked from event callbacks");
}

PAL_BOOL windowTest()
{
    Uint32 flags = PAL_DEBUG | PAL_EVENT_POLLING;
    PAL_BOOL success = PAL_Init(flags);
    if (success) {
        PAL_BOOL running = PAL_TRUE;
        Uint32 windowFlags = PAL_APP_WINDOW;
        PAL_Window* window = PAL_CreateWindow("Window", 640, 480, windowFlags);

        // if not using polling, set event callbacks here
        PAL_SetCloseCallback(onWindowClose);

        while (running) {
            PAL_Event event;
            while (PAL_PollEvents(&event)) {
                // event polling
                switch (event.type) {
                    case PAL_QUIT_EVENT: {
                        PAL_LogInfo("Window close button clicked from event polling");
                        break;
                    } // PAL_QUIT_EVENT

                } // switch
            }

            // render and update
        }
        
        PAL_DestroyWindow(window);
        PAL_Terminate();
        
    } else { return PAL_FALSE; }

    return PAL_TRUE;
}