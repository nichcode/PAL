
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

#include "video_egl.h"

static eglGetProcAddressFn s_Load = nullptr;

typedef union EGLSymbol
{
    void* raw;
    PalLibrarySymbol symbol;
} EGLSymbol;

PalLibrarySymbol loadEGLProc(const char* name)
{
    EGLSymbol sym;
    sym.raw = s_Load(name);
    return sym.symbol;
}

void loadVideoEGL()
{
    g_VideoEgl.handle = palLoadLibrary("libEGL.so");
    if (g_VideoEgl.handle) {
        s_Load = (eglGetProcAddressFn)palGetSymbol(g_VideoEgl.handle, "eglGetProcAddress");

        g_VideoEgl.initialize = (eglInitializeFn)loadEGLProc("eglInitialize");
        g_VideoEgl.terminate = (eglTerminateFn)loadEGLProc("eglTerminate");
        g_VideoEgl.getDisplay = (eglGetDisplayFn)loadEGLProc("eglGetDisplay");
        g_VideoEgl.chooseConfig = (eglChooseConfigFn)loadEGLProc("eglChooseConfig");
        g_VideoEgl.getError = (eglGetErrorFn)loadEGLProc("eglGetError");
        g_VideoEgl.bindAPI = (eglBindAPIFn)loadEGLProc("eglBindAPI");
        g_VideoEgl.getConfigs = (eglGetConfigsFn)loadEGLProc("eglGetConfigs");
        g_VideoEgl.getConfigAttrib = (eglGetConfigAttribFn)loadEGLProc("eglGetConfigAttrib");
    }
}

void unloadVideoEGL()
{
    palFreeLibrary(g_VideoEgl.handle);
}