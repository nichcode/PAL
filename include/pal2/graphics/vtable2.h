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

#ifndef PAL_GRAPHICS_VTABLE_2_H
#define PAL_GRAPHICS_VTABLE_2_H

#include "vtable1.h"

/**
 * @struct PalGraphicsBackendVtable2
 * @brief Version 2 dispatch table for PAL graphics system backends.
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * @since Added in version 2.1
 */
typedef struct {
    /** TODO: */
    const PalGraphicsBackendVtable1* vtable1;

    /**
     * Backend implementation of ::palCanQueueShareOwnership.
     *
     * Must obey the rules and semantics documented in palCanQueueShareOwnership().
     */
    PalBool(PAL_CALL* canQueueShareOwnership)(
        PalQueue* a,
        PalQueue* b);

    /**
     * Backend implementation of ::palCanQueueUseUsageState.
     *
     * Must obey the rules and semantics documented in palCanQueueUseUsageState().
     */
    PalBool(PAL_CALL* canQueueUseUsageState)(
        PalQueue* queue,
        PalUsageState state);

    /**
     * Backend implementation of ::palCanQueueUsePipelineStages.
     *
     * Must obey the rules and semantics documented in palCanQueueUsePipelineStages().
     */
    PalBool(PAL_CALL* canQueueUsePipelineStages)(
        PalQueue* queue,
        PalPipelineStages stages);

    /**
     * Backend implementation of ::palCmdImageOwnershipTransfer.
     *
     * Must obey the rules and semantics documented in palCmdImageOwnershipTransfer().
     */
    void(PAL_CALL* cmdImageOwnershipTransfer)(
        PalCommandBuffer* srcCmdBuffer,
        PalCommandBuffer* dstCmdBuffer,
        PalImage* image,
        PalImageSubresourceRange* subresourceRange,
        PalUsageState srcUsageState,
        PalPipelineStages srcPipelineStages);

    /**
     * Backend implementation of ::palCmdBufferOwnershipTransfer.
     *
     * Must obey the rules and semantics documented in palCmdBufferOwnershipTransfer().
     */
    void(PAL_CALL* cmdBufferOwnershipTransfer)(
        PalCommandBuffer* srcCmdBuffer,
        PalCommandBuffer* dstCmdBuffer,
        PalBuffer* buffer,
        PalUsageState srcUsageState,
        PalPipelineStages srcPipelineStages);
} PalGraphicsBackendVtable2;

#endif // PAL_GRAPHICS_VTABLE_2_H