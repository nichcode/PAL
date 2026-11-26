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

#ifdef _WIN32

#include "wgl.h"
#include "pal2/opengl/fbconfig.h"

PalResult wglEnumerateGLFBConfigs(
    uint32_t* count,
    PalGLFBConfig* configs)
{
    uint32_t configCount = 0;
    uint32_t maxConfigCount = 0;
    int32_t nativeCount = 0;
    const int32_t configAttrib = WGL_NUMBER_PIXEL_FORMATS_ARB;

    if (configs) {
        maxConfigCount = *count;
    }

    if (g_Wgl.getPixelFormatAttribivARB) {
        if (!g_Wgl.getPixelFormatAttribivARB(g_Wgl.hdc, 0, 0, 1, &configAttrib, &nativeCount)) {
            return palMakeResult(
                PAL_RESULT_CODE_PLATFORM_FAILURE,
                PAL_RESULT_SOURCE_WIN32,
                GetLastError());
        }

        int32_t attributes[] = {
            WGL_SUPPORT_OPENGL_ARB,
            WGL_DRAW_TO_WINDOW_ARB,
            WGL_PIXEL_TYPE_ARB,
            WGL_ACCELERATION_ARB,
            WGL_RED_BITS_ARB,
            WGL_GREEN_BITS_ARB,
            WGL_BLUE_BITS_ARB,
            WGL_ALPHA_BITS_ARB,
            WGL_DEPTH_BITS_ARB,
            WGL_STENCIL_BITS_ARB,
            WGL_SAMPLES_ARB,
            WGL_STEREO_ARB,
            WGL_DOUBLE_BUFFER_ARB,
            WGL_FRAMEBUFFER_SRGB_CAPABLE_ARB
        };

        int32_t values[sizeof(attributes) / sizeof(attributes[0])];
        for (int32_t i = 1; i <= nativeCount; i++) {
            if (configs && configCount >= maxConfigCount) {
                break;
            }

            if (!g_Wgl.getPixelFormatAttribivARB(
                    g_Wgl.hdc,
                    i,
                    0,
                    sizeof(attributes) / sizeof(attributes[0]),
                    attributes,
                    values)) {
                continue;
            }

            if (!values[0] || !values[1]) {
                continue;
            }

            if (values[2] != WGL_TYPE_RGBA_ARB) {
                continue;
            }

            if (values[3] == WGL_NO_ACCELERATION_ARB) {
                continue;
            }

            if (configs) {
                PalGLFBConfig* config = &configs[configCount];
                config->index = (uint16_t)i;

                config->redBits = (uint16_t)values[4];
                config->greenBits = (uint16_t)values[5];
                config->blueBits = (uint16_t)values[6];
                config->alphaBits = (uint16_t)values[7];
                config->depthBits = (uint16_t)values[8];
                config->stencilBits = (uint16_t)values[9];
                config->samples = (uint16_t)values[10];

                if (config->samples == 0) {
                    config->samples = 1;
                }

                config->stereo = values[11] ? PAL_TRUE : PAL_FALSE;
                config->doubleBuffer = values[12] ? PAL_TRUE : PAL_FALSE;
                config->sRGB = values[13] ? PAL_TRUE : PAL_FALSE;
            }
            configCount++;
        }

    } else {
        nativeCount = g_Gdi.describePixelFormat(g_Wgl.hdc, 1, 0, nullptr);

        for (int32_t i = 1; i <= nativeCount; i++) {
            if (configs && configCount >= maxConfigCount) {
                break;
            }

            PIXELFORMATDESCRIPTOR pfd;
            if (!g_Gdi.describePixelFormat(g_Wgl.hdc, i, sizeof(PIXELFORMATDESCRIPTOR), &pfd)) {
                continue;
            }

            if (!(pfd.dwFlags & PFD_SUPPORT_OPENGL) || !(pfd.dwFlags & PFD_DRAW_TO_WINDOW)) {
                continue;
            }

            if (pfd.iPixelType != PFD_TYPE_RGBA) {
                continue;
            }

            if (!(pfd.dwFlags & PFD_GENERIC_ACCELERATED) && (pfd.dwFlags & PFD_GENERIC_FORMAT)) {
                continue;
            }

            if (configs) {
                PalGLFBConfig* config = &configs[configCount];
                config->index = (uint16_t)i;

                config->redBits = pfd.cRedBits;
                config->greenBits = pfd.cGreenBits;
                config->blueBits = pfd.cBlueBits;
                config->alphaBits = pfd.cAlphaBits;
                config->depthBits = pfd.cDepthBits;
                config->stencilBits = pfd.cStencilBits;
                config->samples = 1;

                config->stereo = (pfd.dwFlags & PFD_STEREO) ? PAL_TRUE : PAL_FALSE;
                config->sRGB = PAL_FALSE;
                config->doubleBuffer = (pfd.dwFlags & PFD_DOUBLEBUFFER) ? PAL_TRUE : PAL_FALSE;
            }
            configCount++;
        }
    }

    *count = configCount;
    return PAL_RESULT_SUCCESS;
}

#endif // _WIN32