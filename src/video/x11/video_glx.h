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

#ifndef VIDEO_GLX_H
#define VIDEO_GLX_H
#if PAL_HAS_X11_BACKEND

#include "pal2/core/library.h"

#include <X11/Xlib.h>
#include <X11/Xutil.h>

#define GLX_FBCONFIG_ID 0x8012
typedef struct __GLXFBConfigRec* GLXFBConfig;

typedef GLXFBConfig* (*GLXGetFBConfigsFn)(Display*, int, int*);
typedef int (*GLXGetFBConfigAttribFn)(Display*, GLXFBConfig, int, int*);
typedef XVisualInfo* (*GLXGetVisualFromFBConfigFn)(Display*, GLXFBConfig);
typedef void (*GLXProc)();

typedef GLXProc (*GLXGetProcAddressFn)(const unsigned char*);

typedef struct VideoGLX {
    PalLibrary* libglx;
    GLXGetFBConfigsFn getFBConfigs;
    GLXGetFBConfigAttribFn getFBConfigAttrib;
    GLXGetVisualFromFBConfigFn getVisualFromFBConfig;
} VideoGLX;

extern VideoGLX g_VideoGlx;

void loadVideoGLX();
void unloadVideoGLX();

#endif // PAL_HAS_X11_BACKEND
#endif // VIDEO_GLX_H