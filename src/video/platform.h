
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

#include "pal2/video/keyboard.h"
#include "pal2/video/mouse.h"
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
typedef PalVideoDriver* (*GetVideoDriverFn)(void);

/** Initializes the video backend. 
 * 
 * Additional rules:
 * - `preferredInstance` is the users requested instance. It may be nullptr.
 * - `selectedInstance` is the actual instance used. This may be the same as `preferredInstance`.
*/
typedef PalResult (*InitVideoFn)(
    const PalAllocator* allocator,
    struct PalEventDriver* eventDriver,
    const void* preferredInstance,
    void** selectedInstance);

/** Shutdowns the video backend. */
typedef void (*ShutdownVideoFn)(void);

/** Updates the video backend. 
 * 
 * Additional rules:
 * This function must not block.
*/
typedef void (*UpdateVideoFn)(void);


/* =========================================================================
 * Keyboard
 * ========================================================================= */

typedef struct Keyboard {
    PalBool scancodeState[PAL_SCANCODE_COUNT];
    PalBool keycodeState[PAL_KEYCODE_COUNT];
} Keyboard;


/* =========================================================================
 * Mouse
 * ========================================================================= */

typedef struct Mouse {
    PalBool state[PAL_MOUSE_BUTTON_COUNT];
    int32_t dx;
    int32_t dy;
    int32_t WheelX;
    int32_t WheelY;
} Mouse;


/** A video backend. */
typedef struct VideoBackend {
    GetVideoDriverFn getDriver;
    InitVideoFn init;
    ShutdownVideoFn shutdown;
    UpdateVideoFn update;
} VideoBackend;

extern const VideoBackend* g_VideoBackend; /**< Active video backend. */
extern Keyboard g_Keyboard;
extern Mouse g_Mouse;
extern PalBool g_VideoInitialized;

/** Gets all supported backends per platform. */
const VideoBackend* const* platformGetVideoBackends(uint32_t* count);

#endif // VIDEO_PLATFORM_H