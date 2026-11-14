
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

#ifndef VIDEO_PLATFORM_H
#define VIDEO_PLATFORM_H

#include "shared.h"
#include "pal2/video/init.h"
#include "pal2/video/cursor.h"
#include "pal2/video/icon.h"
#include "pal2/video/monitor.h"

#include "pal2/video/window.h"

/**
 * Platform contract for the video module.
 *
 * Every platform must implement every function declared here. A function
 * that says it has the same semantics and rules as a public `pal*` function
 * has only the platform requirements. A function with no public counterpart
 * is documented in full. These rules apply to all of them:
 *
 * - Arguments are validated by the caller. A platform function can assume
 *   every pointer it receives is valid and must not validate it again.
 *   Any exception will be documented by the function.
 */

/* =========================================================================
 * Init
 * ========================================================================= */

/** Gets the video driver of a video backend. 
 * 
 * Additional rules:
 * - The fucntion must exist before the backend is initialized
*/
typedef PalVideoDriver* (*getVideoDriverFn)();


/** A video backend. */
typedef struct VideoBackend {
    getVideoDriverFn getDriver;
} VideoBackend;

extern VideoBackend* s_VideoBackend; /**< Active video backend. */

/** Gets all supported backends per platform. */
const VideoBackend* const* platformGetVideoBackends(uint32_t* count);

#endif // VIDEO_PLATFORM_H