
#ifndef _PAL_OPENGL_H
#define _PAL_OPENGL_H

#include "pal_core.h"

typedef struct PalGLContext PalGLContext;

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

typedef enum {
    PAL_GL_PROFILE_NONE,
    PAL_GL_PROFILE_CORE,
    PAL_GL_PROFILE_COMPATIBILITY,
    PAL_GL_PROFILE_ES
} PalGLProfile;

typedef enum {
    PAL_GL_CONTEXT_RESET_NONE,
    PAL_GL_CONTEXT_RESET_NO_NOTIFICATION,
    PAL_GL_CONTEXT_RESET_LOSE_CONTEXT
} PalGLContextReset;

typedef enum {
    PAL_GL_RELEASE_BEHAVIOR_NONE,
    PAL_GL_CONTEXT_RESET_FLUSH
} PalGLRelease;

typedef struct {
    PalGLExtension extensions;
    Uint32 major;
    Uint32 minor;
    char vendor[32];
    char graphicsCard[64];
    char version[64];
} PalGLInfo;

typedef struct {
    bool doubleBuffer;
    bool stereo;
    bool sRGB;
    Uint16 index;
    Uint16 redBits;
    Uint16 greenBits;
    Uint16 blueBits;
    Uint16 alphaBits;
    Uint16 depthBits;
    Uint16 stencilBits;
    Uint16 samples;
} PalGLFBConfig;

typedef struct {
    void* display;
    void* window;
} PalGLWindow;

typedef struct {
    bool forward;
    bool robustness;
    bool noError;
    bool debug;
    Uint16 major;
    Uint16 minor;
    PalGLProfile profile;
    PalGLContextReset reset;
    PalGLRelease release;
    PalGLContext* shareContext;
    const PalAllocator* allocator;
    const PalGLFBConfig* fbConfig;
    const PalGLWindow* window;
} PalGLContextCreateInfo;

PAL_API PalResult PAL_CALL palInitGL(
    const PalAllocator* allocator);

PAL_API void PAL_CALL palShutdownGL();

PAL_API const PalGLInfo* PAL_CALL palGetGLInfo();

PAL_API PalResult PAL_CALL palEnumerateGLFBConfigs(
    PalGLWindow* glWindow,
    Int32 *count,
    PalGLFBConfig *configs);

PAL_API const PalGLFBConfig* PAL_CALL palGetClosestGLFBConfig(
    PalGLFBConfig *configs,
    Int32 count,
    const PalGLFBConfig* desired);

PAL_API PalResult PAL_CALL palCreateGLContext(
    const PalGLContextCreateInfo* info,
    PalGLContext** outContext);

PAL_API void PAL_CALL palDestroyGLContext(
    PalGLContext* context);

PAL_API PalResult PAL_CALL palMakeContextCurrent(
    PalGLWindow* glWindow,
    PalGLContext* context);

PAL_API PalResult PAL_CALL palSwapBuffers(
    PalGLWindow* glWindow,
    PalGLContext* context);

PAL_API void PAL_CALL palSetGLContextVsync(
    PalGLContext* context,
    bool enable);

#endif // _PAL_OPENGL_H