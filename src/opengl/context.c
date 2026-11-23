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

#include "platform.h"
#include "pal2/opengl/context.h"

PalResult PAL_CALL palCreateGLContext(
    const PalGLContextCreateInfo* info,
    PalGLContext** context)
{
    ASSERT(g_GLInitialized != PAL_FALSE, "GL system not initialized");
    ASSERT(info != nullptr, "info parameter is nullptr");
    ASSERT(info->fbConfig != nullptr, "info->fbconfig parameter is nullptr");
    ASSERT(info->window != nullptr, "info->window parameter is nullptr");

    return g_GLBackend->createContext(info, context);
}

void PAL_CALL palDestroyGLContext(PalGLContext* context)
{
    ASSERT(context != nullptr, "context parameter is nullptr");

    g_GLBackend->destroyContext(context);
}

PalResult PAL_CALL palMakeContextCurrent(
    PalGLWindow* window,
    PalGLContext* context)
{
    ASSERT(window != nullptr, "window parameter is nullptr");

    return g_GLBackend->makeContextCurrent(window, context);
}

PalResult PAL_CALL palSwapBuffers(
    PalGLWindow* window,
    PalGLContext* context)
{
    ASSERT(window != nullptr, "window parameter is nullptr");
    ASSERT(context != nullptr, "context parameter is nullptr");

    return g_GLBackend->swapBuffers(window, context);
}

void PAL_CALL palSetSwapInterval(int32_t interval)
{
    g_GLBackend->setSwapInterval(interval);
}