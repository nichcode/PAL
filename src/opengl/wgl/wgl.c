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
#ifdef _WIN32

#include "opengl/platform.h"

PalResult wglInitGL(PalGLAPI, void*, const PalAllocator*);
void wglShutdownGL();
void* wglGetGLProcAddress(const char*);
const PalBool* wglGetSupportedGLAPIs(void*);
PalResult wglEnumerateGLFBConfigs(uint32_t*, PalGLFBConfig*);
PalResult wglCreateGLContext(const PalGLContextCreateInfo*, PalGLContext**);
void wglDestroyGLContext(PalGLContext*);
PalResult wglMakeContextCurrent(PalGLWindow*, PalGLContext*);
PalResult wglSwapBuffers(PalGLWindow*, PalGLContext*);
void wglSetSwapInterval(int32_t);

const GLBackend g_WglBackend = {
    .init = wglInitGL,
    .shutdown = wglShutdownGL,
    .getProcAddress = wglGetGLProcAddress,
    .enumerateFBConfigs = wglEnumerateGLFBConfigs,
    .createContext = wglCreateGLContext,
    .destroyContext = wglDestroyGLContext,
    .makeContextCurrent = wglMakeContextCurrent,
    .swapBuffers = wglSwapBuffers,
    .setSwapInterval = wglSetSwapInterval,
};

// clang-format on

#endif // _WIN32