
#ifndef PAL_EVENTS_H
#define PAL_EVENTS_H

#include "PAL_defines.h"

typedef struct PAL_Window PAL_Window;
typedef void (*PAL_CloseFun)(PAL_Window* window);

typedef enum PAL_EventTypes
{
    PAL_QUIT_EVENT

} PAL_EventTypes;

typedef struct PAL_Event
{
    Uint32 type;
    PAL_Window* window;

} PAL_Event;

PAL_API void PAL_SetCloseCallback(PAL_CloseFun callback);

PAL_API PAL_BOOL PAL_PollEvents(PAL_Event* event);
PAL_API PAL_BOOL PAL_WaitEvents(PAL_Event* event);

#endif // PAL_EVENTS_H