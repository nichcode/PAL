
#ifndef _PAL_OPENGL_H
#define _PAL_OPENGL_H

#include "pal_core.h"

typedef enum {
    PAL_GL_EXTENSION_CREATE_CONTEXT = PAL_BIT(0),
    PAL_GL_EXTENSION_CONTEXT_PROFILE = PAL_BIT(1),
    PAL_GL_EXTENSION_CONTEXT_PROFILE_ES2 = PAL_BIT(2),
    PAL_GL_EXTENSION_ROBUSTNESS = PAL_BIT(3),
    PAL_GL_EXTENSION_NO_ERROR = PAL_BIT(4),
    PAL_GL_EXTENSION_PIXEL_FORMAT = PAL_BIT(5),
    PAL_GL_EXTENSION_MULTISAMPLE = PAL_BIT(6),
    PAL_GL_EXTENSION_SWAP_CONTROL = PAL_BIT(7),
    PAL_GL_EXTENSION_FLUSH_CONTROL = PAL_BIT(8),
    PAL_GL_EXTENSION_COLORSPACE_SRGB = PAL_BIT(9)
} PalGLExtension;

typedef struct {
    PalGLExtension extensions;
    Uint32 major;
    Uint32 minor;
    char vendor[32];
    char graphicsCard[64];
    char version[64];
} PalGLInfo;

PAL_API PalResult PAL_CALL palInitGL(
    const PalAllocator* allocator);

PAL_API void PAL_CALL palShutdownGL();

PAL_API const PalGLInfo* PAL_CALL palGetGLInfo();

#endif // _PAL_OPENGL_H