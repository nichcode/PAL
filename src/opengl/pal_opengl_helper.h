
/**
    PAL - Prime Abstraction Layer
    Copyright (C) 2025
    Licensed under the Zlib license. See LICENSE file in root.
 */

#ifndef _PAL_OPENGL_HELPER_H
#define _PAL_OPENGL_HELPER_H

#include "pal/pal_opengl.h"

typedef struct {
    // clang-format off
    void (PAL_CALL *shutdownGL)();
    const PalGLInfo* (PAL_CALL *getGLInfo)();
    PalResult (PAL_CALL *enumerateGLFBConfigs)(int32_t*, PalGLFBConfig*);
    PalResult (PAL_CALL *createGLContext)(const PalGLContextCreateInfo*, PalGLContext**);
    void (PAL_CALL *destroyGLContext)(PalGLContext*);
    PalResult (PAL_CALL *makeContextCurrent)(PalGLWindow*, PalGLContext*);
    void* (PAL_CALL *getGLProcAddress)(const char*);
    
    PalResult (PAL_CALL *swapBuffers)(PalGLWindow*, PalGLContext*);
    PalResult (PAL_CALL *setSwapInterval)(int32_t);
    // clang-format on
} Backend;

typedef struct {
    PalBool initialized;
    void* handle;
    void* instance;
    const PalAllocator* allocator;
    Backend* backend;
    PalGLInfo info;
} Opengl;

extern Opengl s_GL;

#endif // _PAL_OPENGL_HELPER_H