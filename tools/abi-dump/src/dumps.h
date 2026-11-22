
#ifndef ABI_DUMP_H
#define ABI_DUMP_H

#include "helpers.h"

/**
 * We add a high level function that calls its system dump functions. This is
 * here to avoid bloating the main.c with dumps (eg. allocator dump, etc)
*/
PalBool coreStructs(void);

#if PAL_HAS_EVENT_MODULE
PalBool eventStructs(void);
#endif // PAL_HAS_EVENT_MODULE

#if PAL_HAS_THREAD_MODULE
PalBool threadStructs(void);
#endif // PAL_HAS_THREAD_MODULE

#if PAL_HAS_SYSTEM_MODULE
PalBool systemStructs(void);
#endif // PAL_HAS_SYSTEM_MODULE

#if PAL_HAS_VIDEO_MODULE
PalBool videoStructs(void);
#endif // PAL_HAS_VIDEO_MODULE

#if PAL_HAS_OPENGL_MODULE
PalBool openglStructs(void);
#endif // PAL_HAS_OPENGL_MODULE

#if PAL_HAS_GRAPHICS_MODULE
PalBool graphicsStructs(void);
#endif // PAL_HAS_GRAPHICS_MODULE

#endif // ABI_DUMP_H