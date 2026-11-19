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

#include "video_glx.h"
#include <string.h>

#if PAL_HAS_X11_BACKEND

VideoGLX g_VideoGlx = {0};
static GLXGetProcAddressFn s_Load = nullptr;

typedef union GLXSymbol
{
    void* raw;
    GLXProc symbol;
} GLXSymbol;

GLXProc loadGLXProc(const char* name)
{
    if (s_Load) {
        GLXSymbol sym;
        sym.raw = s_Load((const unsigned char*)name);
        if (sym.raw) {
            return sym.symbol;
        }
    }

    if (g_VideoGlx.libglx) {
        return palGetSymbol(g_VideoGlx.libglx, name);
    }

    return nullptr;
}

void loadVideoGLX(void)
{
    if (g_VideoGlx.libglx) {
        return;
    }

    g_VideoGlx.libglx = palLoadLibrary("libGL.so.1");
    if (g_VideoGlx.libglx) {
        s_Load = (GLXGetProcAddressFn)palGetSymbol(g_VideoGlx.libglx, "glXGetProcAddress");

        g_VideoGlx.getFBConfigs = (GLXGetFBConfigsFn)loadGLXProc("glXGetFBConfigs");
        g_VideoGlx.getFBConfigAttrib = (GLXGetFBConfigAttribFn)loadGLXProc("glXGetFBConfigAttrib");
        g_VideoGlx.getVisualFromFBConfig = (GLXGetVisualFromFBConfigFn)loadGLXProc("glXGetVisualFromFBConfig");
    }
}

void unloadVideoGLX(void)
{
    if (g_VideoGlx.libglx) {
        palFreeLibrary(g_VideoGlx.libglx);
    }

    memset(&g_VideoGlx, 0, sizeof(VideoGLX));
    s_Load = nullptr;
}

#endif // PAL_HAS_X11_BACKEND