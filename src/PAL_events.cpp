
#include "PAL_pch.h"
#include "PAL_platform.h"
#include "PAL_eventqueue.h"

void PAL_SetCloseCallback(PAL_CloseFun callback)
{
    s_Callbacks.close = callback;
}

PAL_BOOL PAL_PollEvents(PAL_Event* event)
{
    PAL_PlatformPollEvents();
    
    if (s_PAL.eventPolling) {
        return s_EventQueue.poll(event);
    } else {
        return PAL_FALSE;
    }
}

PAL_BOOL PAL_WaitEvents(PAL_Event* event)
{
    return PAL_FALSE;
}
