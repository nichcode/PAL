/**
 * Copyright (C) 2025-2026 Nicholas Agbo <agbonicholas04@gmail.com>
 *
 * This software is provided 'as-is', without any express or implied
 * warranty.  In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 *
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 *
 * 3. This notice may not be removed or altered from any source distribution.
 */

// clang-format off
#if defined(PAL_HAS_WAYLAND_BACKEND) || defined(PAL_HAS_X11_BACKEND)

#include "opengl/platform.h"

PalResult eglInitGL(PalGLAPI, void*, const PalAllocator*);
void eglShutdownGL();
void* eglGetGLProcAddress(const char*);
const PalBool* eglGetSupportedGLAPIs(void*);
PalResult eglEnumerateGLFBConfigs(uint32_t*, PalGLFBConfig*);
PalResult eglCreateGLContext(const PalGLContextCreateInfo*, PalGLContext**);
void eglDestroyGLContext(PalGLContext*);
PalResult eglMakeContextCurrent(PalGLWindow*, PalGLContext*);
PalResult eglSwapBuffers(PalGLWindow*, PalGLContext*);
void eglSetSwapInterval(int32_t);

const GLBackend g_EglBackend = {
    .init = eglInitGL,
    .shutdown = eglShutdownGL,
    .getProcAddress = eglGetGLProcAddress,
    .enumerateFBConfigs = eglEnumerateGLFBConfigs,
    .createContext = eglCreateGLContext,
    .destroyContext = eglDestroyGLContext,
    .makeContextCurrent = eglMakeContextCurrent,
    .swapBuffers = eglSwapBuffers,
    .setSwapInterval = eglSetSwapInterval,
    .getSupportedAPIs = eglGetSupportedGLAPIs
};

// clang-format on

#endif // PAL_HAS_WAYLAND_BACKEND