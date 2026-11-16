
#ifndef ABI_DUMP_H
#define ABI_DUMP_H

#include "helpers.h"

/**
 * We add a high level function that calls its system dump functions. This is
 * here to avoid bloating the main.c with dumps (eg. allocator dump, etc)
*/
bool coreStructs(void);

#if PAL_HAS_EVENT_MODULE
bool eventStructs(void);
#endif // PAL_HAS_EVENT_MODULE

#if PAL_HAS_THREAD_MODULE
bool threadStructs(void);
#endif // PAL_HAS_THREAD_MODULE

#if PAL_HAS_SYSTEM_MODULE
bool systemStructs(void);
#endif // PAL_HAS_SYSTEM_MODULE

#if PAL_HAS_VIDEO_MODULE
bool videoStructs(void);
#endif // PAL_HAS_VIDEO_MODULE

#if PAL_HAS_OPENGL_MODULE
bool openglStructs(void);
#endif // PAL_HAS_OPENGL_MODULE

#if PAL_HAS_GRAPHICS_MODULE
bool graphicsStructs(void);
#endif // PAL_HAS_GRAPHICS_MODULE

#endif // ABI_DUMP_H