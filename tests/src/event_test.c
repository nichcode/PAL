
#include "tests.h"
#include "pal/pal_event.h"

#define QUIT_TIME 100

PalResult eventTest()
{
    palLogConsoleInfo("");
    palLogConsoleInfo("===========================================");
    palLogConsoleInfo("Event Test");
    palLogConsoleInfo("===========================================");
    palLogConsoleInfo("");

    PalResult result;
    PalEventInstance* eventInstance = PAL_NULL;

    PalEventInstanceDesc desc;
    desc.allocator = PAL_NULL;
    desc.callback = PAL_NULL; // for callback event mode
    desc.queue = PAL_NULL; // use internal event queue
    result = palCreateEventInstance(&desc, &eventInstance);
    if (result == PAL_RESULT_FAIL) {
        PalError error = palGetError();
        palLogConsoleError(palErrorToString(error));
        return PAL_RESULT_FAIL;
    }

    // enable close event with poll mode(or callback mode)
    result = palEnableEvent(eventInstance, PAL_EVENT_QUIT, PAL_MODE_POLL);
    if (result == PAL_RESULT_FAIL) {
        PalError error =palGetError();
        palLogConsoleError(palErrorToString(error));
        return PAL_RESULT_FAIL;
    }

    bool running = PAL_TRUE;
    int timer = 0;
    while (running) {
        PalEvent event;
        while (palPollEvent(eventInstance, &event) == PAL_RESULT_OK) {
            if (event.type == PAL_EVENT_QUIT) {
                running = PAL_FALSE;
                timer = 0;
                break;
            }
        }

        timer++;
        if (timer >= QUIT_TIME) {
            PalEvent quitEvent;
            quitEvent.id = 0; // can be a window id or anything;
            quitEvent.type = PAL_EVENT_QUIT;

            // push the event. this either calls the callback or push to the queue
            // depending on the event mode
            palPushEvent(eventInstance, &quitEvent);
            if (result == PAL_RESULT_FAIL) {
                PalError error =palGetError();
                palLogConsoleError(palErrorToString(error));
                return PAL_RESULT_FAIL;
            }
        }
    }

    palDestroyEventInstance(eventInstance);
    return PAL_RESULT_OK;
}