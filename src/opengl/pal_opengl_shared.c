
/**
    PAL - Prime Abstraction Layer
    Copyright (C) 2025
    Licensed under the Zlib license. See LICENSE file in root.
 */

#include "pal/pal_opengl.h"
#include <stdlib.h>

const PalGLFBConfig* PAL_CALL palGetClosestGLFBConfig(
    PalGLFBConfig* configs,
    int32_t count,
    const PalGLFBConfig* desired)
{
    if (!configs || !desired) {
        return nullptr;
    }

    if (count == 0) {
        return nullptr;
    }

    int32_t score = 0;
    int32_t bestScore = 0x7FFFFFFF;
    PalGLFBConfig* best = nullptr;
    for (int32_t i = 0; i < count; i++) {
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