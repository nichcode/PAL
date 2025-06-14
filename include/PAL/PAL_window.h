
#ifndef PAL_WINDOW_H
#define PAL_WINDOW_H

#include "PAL_defines.h"

typedef struct PAL_Window PAL_Window;

typedef enum PAL_WindowFlags
{
    PAL_WINDOW_SHOWN = PAL_BIT(1),
    PAL_WINDOW_MINIMIZEBOX = PAL_BIT(2),
    PAL_WINDOW_RESIZABLE = PAL_BIT(3),
    PAL_WINDOW_MAXIMIZED = PAL_BIT(4),
    PAL_APP_WINDOW = PAL_WINDOW_SHOWN | PAL_WINDOW_MINIMIZEBOX | PAL_WINDOW_RESIZABLE

} PAL_WindowFlags;

PAL_API PAL_Window* PAL_CreateWindow(const char* title, Uint32 w, Uint32 h, Uint32 flags);
PAL_API void PAL_DestroyWindow(PAL_Window* window);

#endif // PAL_WINDOW_H