/**
 * @file next.h
 * 
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

#ifndef PAL_GRAPHICS_VTABLE2_H
#define PAL_GRAPHICS_VTABLE2_H

#include "types.h"

/**
 * @struct PalGraphicsBackendVtable2
 * @brief Graphics backend dispatch table version 2.
 *
 * This struct must remain valid until @ref palShutdownGraphics() is called.
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * @since Added in version 2.1
 * @ingroup pal_graphics
 */
typedef struct PalGraphicsBackendVtable2 {
    //clang-format off
    const PalGraphicsBackendVtable1* vtable1; /**< Version 1 vtable. */
    /** Implements @ref palCanQueueShareOwnership(). */
    PalBool(PAL_CALL* canQueueShareOwnership)(PalQueue*, PalQueue*);
    /** Implements @ref palCanQueueUseUsageState(). */
    PalBool(PAL_CALL* canQueueUseUsageState)(PalQueue*, PalUsageState);
    /** Implements @ref palCanQueueUsePipelineStages(). */
    PalBool(PAL_CALL* canQueueUsePipelineStages)(PalQueue*, PalPipelineStages);
    /** Implements @ref palCmdImageOwnershipTransfer(). */
    void(PAL_CALL* cmdImageOwnershipTransfer)(PalCommandBuffer*, PalCommandBuffer*, PalImage*, PalImageSubresourceRange*, PalUsageState, PalPipelineStages);
    /** Implements @ref palCmdBufferOwnershipTransfer(). */
    void(PAL_CALL* cmdBufferOwnershipTransfer)(PalCommandBuffer*, PalCommandBuffer*, PalBuffer*, PalUsageState, PalPipelineStages);
    //clang-format on
} PalGraphicsBackendVtable2;

#endif // PAL_GRAPHICS_VTABLE2_H