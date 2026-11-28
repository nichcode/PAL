/**
 * @file vtable2.h
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

#include "vtable1.h"

/** This function has the same semantics and rules as palCanQueueShareOwnership(). */
typedef PalBool(PAL_CALL* CanQueueShareOwnershipFn)(
    PalQueue*,
    PalQueue*);

/** This function has the same semantics and rules as palCanQueueUseUsageState(). */
typedef PalBool(PAL_CALL* CanQueueUseUsageStateFn)(
    PalQueue*,
    PalUsageState);

/** This function has the same semantics and rules as palCanQueueUsePipelineStages(). */
typedef PalBool(PAL_CALL* CanQueueUsePipelineStagesFn)(
    PalQueue*,
    PalPipelineStages);

/** This function has the same semantics and rules as palCmdImageOwnershipTransfer(). */
typedef void(PAL_CALL* CmdImageOwnershipTransferFn)(
    PalCommandBuffer*,
    PalCommandBuffer*,
    PalImage*,
    PalImageSubresourceRange*,
    PalUsageState,
    PalPipelineStages);

/** This function has the same semantics and rules as palCmdBufferOwnershipTransfer(). */
typedef void(PAL_CALL* CmdBufferOwnershipTransferFn)(
    PalCommandBuffer*,
    PalCommandBuffer*,
    PalBuffer*,
    PalUsageState,
    PalPipelineStages);

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
    const PalGraphicsBackendVtable1* vtable1;
    CanQueueShareOwnershipFn canQueueShareOwnership;
    CanQueueUseUsageStateFn canQueueUseUsageState;
    CanQueueUsePipelineStagesFn canQueueUsePipelineStages;
    CmdImageOwnershipTransferFn cmdImageOwnershipTransfer;
    CmdBufferOwnershipTransferFn cmdBufferOwnershipTransfer;
} PalGraphicsBackendVtable2;

#endif // PAL_GRAPHICS_VTABLE2_H
