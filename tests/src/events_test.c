
#include "tests.h"

#define SIZE_TIME 500
#define POS_TIME 1000
#define END_TIME 1500

int g_EventTimer = 0;
void pushEvents();

bool eventsTest()
{
    PalError error = PAL_ERROR_NONE;
    if (!palInitEvent(PAL_NULL)) {
        error = palGetError();
        palLogError(palFormatError(error));
        return PAL_FALSE;
    }

    // register events
    // check for error. We dont because event system has been initialized
    palRegisterEvent(PAL_EVENT_QUIT, PAL_DISPATCH_POLL);
    palRegisterEvent(PAL_EVENT_WINDOW_MOVE, PAL_DISPATCH_POLL);
    palRegisterEvent(PAL_EVENT_WINDOW_RESIZE, PAL_DISPATCH_POLL);

    bool running = PAL_TRUE;
    while (running) {
        PalEvent event;
        while (palPollEvent(&event)) {
            switch (event.type) {
                case PAL_EVENT_QUIT:
                running = PAL_FALSE;
                break;

                case PAL_EVENT_WINDOW_MOVE: 
                palLogInfo("Pos (%i, %i)", event.windowPos.x, event.windowPos.y);
                break;

                case PAL_EVENT_WINDOW_RESIZE: 
                palLogInfo("Size (%i, %i)", event.size.width, event.size.height);
                break;
            }

        }

        g_EventTimer++;
        pushEvents();
    }

    palShutdownEvent();
    return PAL_TRUE;
}

void pushEvents()
{
    if (g_EventTimer == SIZE_TIME) {
        PalEvent event;
        event.windowID = 0;
        event.type = PAL_EVENT_WINDOW_RESIZE;
        event.size.width = 500;
        event.size.height = 100;
        palPushEvent(event);
    }

    if (g_EventTimer == POS_TIME) {
        PalEvent event;
        event.windowID = 0;
        event.type = PAL_EVENT_WINDOW_MOVE;
        event.windowPos.x = 200;
        event.windowPos.y = 100;
        palPushEvent(event);
    }

    if (g_EventTimer == END_TIME) {
        PalEvent event;
        event.windowID = 0;
        event.type = PAL_EVENT_QUIT;
        palPushEvent(event);
    }
}