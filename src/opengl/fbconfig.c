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
#include "pal2/opengl/fbconfig.h"
#include <stdlib.h>

PalResult PAL_CALL palEnumerateGLFBConfigs(
    uint32_t* count,
    PalGLFBConfig* configs)
{
    ASSERT(g_GLInitialized != PAL_FALSE, "GL system not initialized");
    ASSERT(count != nullptr, "count parameter is nullptr");
    ASSERT(configs == nullptr || *count > 0, "configs parameter is not nullptr but *count is 0");

    return g_GLBackend->enumerateFBConfigs(count, configs);
}

const PalGLFBConfig* PAL_CALL palGetClosestGLFBConfig(
    PalGLFBConfig* configs,
    uint32_t count,
    const PalGLFBConfig* desired)
{
    int32_t score = 0;
    int32_t bestScore = 0x7FFFFFFF;
    PalGLFBConfig* best = nullptr;
    for (uint32_t i = 0; i < count; i++) {
        PalGLFBConfig* tmp = &configs[i];

        // filter out hard constraints
        if (desired->doubleBuffer && !tmp->doubleBuffer) {
            continue;
        }

        if (desired->stereo && !tmp->stereo) {
            continue;
        }

        score = 0;

        // score color bits
        score += abs(tmp->redBits - desired->redBits);
        score += abs(tmp->greenBits - desired->greenBits);
        score += abs(tmp->blueBits - desired->blueBits);
        score += abs(tmp->alphaBits - desired->alphaBits);
        score += abs(tmp->depthBits - desired->depthBits);
        score += abs(tmp->stencilBits - desired->stencilBits);

        // score soft constraints
        if (desired->samples != tmp->samples) {
            score += 1000;
        }

        if (desired->sRGB != tmp->sRGB) {
            score += 500;
        }

        if (score < bestScore) {
            bestScore = score;
            best = &configs[i];
        }
    }

    return best;
}