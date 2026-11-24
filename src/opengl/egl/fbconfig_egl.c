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

#include "shared.h"
#if PLATFORM_EGL

#include "egl.h"
#include "opengl/platform.h"
#include "pal2/opengl/fbconfig.h"
#include "pal2/opengl/init.h"

PalResult eglEnumerateGLFBConfigs(
    uint32_t* count,
    PalGLFBConfig* configs)
{
    uint32_t configCount = 0;
    uint32_t maxConfigCount = 0;

    if (configs) {
        maxConfigCount = *count;
    }

    EGLint numConfigs = 0;
    if (!g_Egl.getConfigs(g_Egl.display, nullptr, 0, &numConfigs)) {
        return palMakeResult(PAL_RESULT_CODE_PLATFORM_FAILURE, PAL_RESULT_SOURCE_EGL, (uint32_t)g_Egl.getError());
    }

    size_t configSize = sizeof(EGLConfig) * (size_t)numConfigs;
    EGLConfig* eglConfigs = palAllocate(g_Egl.allocator, configSize, 0);
    if (!eglConfigs) {
        return PAL_RESULT_CODE_OUT_OF_MEMORY;
    }

    g_Egl.getConfigs(g_Egl.display, eglConfigs, numConfigs, &numConfigs);
    for (EGLint i = 0; i < numConfigs; i++) {
        if (configs && configCount >= maxConfigCount) {
            break;
        }

        EGLint surfaceType = 0;
        EGLint renderable = 0;
        EGLint colorType = 0;

        EGLConfig config = eglConfigs[i];
        g_Egl.getConfigAttrib(g_Egl.display, config, EGL_SURFACE_TYPE, &surfaceType);
        g_Egl.getConfigAttrib(g_Egl.display, config, EGL_RENDERABLE_TYPE, &renderable);
        g_Egl.getConfigAttrib(g_Egl.display, config, EGL_COLOR_BUFFER_TYPE, &colorType);

        if (colorType != EGL_RGB_BUFFER) {
            continue;
        }

        if (g_Egl.apiTypeBit == EGL_OPENGL_ES3_BIT) {
            if (!(renderable & EGL_OPENGL_ES2_BIT) && !(renderable & EGL_OPENGL_ES3_BIT)) {
                continue;
            }
            
        } else if (g_Egl.apiTypeBit == EGL_OPENGL_ES2_BIT) {
            if (!(renderable & EGL_OPENGL_ES2_BIT)) {
                continue;
            }

        } else {
            if (!(renderable & EGL_OPENGL_BIT)) {
                continue;
            }
        }

        if (!(surfaceType & EGL_WINDOW_BIT)) {
            continue;
        }

        if (configs) {
            PalGLFBConfig* fbConfig = &configs[configCount];
            fbConfig->index = (uint16_t)i;

            EGLint redBits, greenBits, blueBits, alphaBits;
            EGLint depthBits, stencilBits, samples;

            g_Egl.getConfigAttrib(g_Egl.display, config, EGL_RED_SIZE, &redBits);
            g_Egl.getConfigAttrib(g_Egl.display, config, EGL_GREEN_SIZE, &greenBits);
            g_Egl.getConfigAttrib(g_Egl.display, config, EGL_BLUE_SIZE, &blueBits);
            g_Egl.getConfigAttrib(g_Egl.display, config, EGL_ALPHA_SIZE, &alphaBits);
            g_Egl.getConfigAttrib(g_Egl.display, config, EGL_DEPTH_SIZE, &depthBits);
            g_Egl.getConfigAttrib(g_Egl.display, config, EGL_STENCIL_SIZE, &stencilBits);
            g_Egl.getConfigAttrib(g_Egl.display, config, EGL_SAMPLES, &samples);

            if (samples == 0) {
                samples = 1;
            }

            fbConfig->redBits = (uint16_t)redBits;
            fbConfig->greenBits = (uint16_t)greenBits;
            fbConfig->blueBits = (uint16_t)blueBits;
            fbConfig->alphaBits = (uint16_t)alphaBits;
            fbConfig->depthBits = (uint16_t)depthBits;
            fbConfig->stencilBits = (uint16_t)stencilBits;
            fbConfig->samples = (uint16_t)samples;

            fbConfig->doubleBuffer = PAL_TRUE;
            fbConfig->stereo = PAL_FALSE;

            fbConfig->sRGB = PAL_FALSE;
            if (g_GLInfo.extensions & PAL_GL_EXTENSION_COLORSPACE_SRGB) {
                if (fbConfig->redBits >= 8 && fbConfig->greenBits >= 8 && fbConfig->blueBits >= 8) {
                    fbConfig->sRGB = PAL_TRUE;
                }
            }
        }
        configCount++;
    }

    palFree(g_Egl.allocator, eglConfigs);
    *count = configCount;

    return PAL_RESULT_SUCCESS;
}

#endif // PAL_HAS_WAYLAND_BACKEND