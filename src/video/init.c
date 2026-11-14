
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
#include "pal2/video/init.h"
#include <string.h>

VideoBackend* s_VideoBackend = nullptr;
static PalBool s_Initialized = PAL_FALSE;
static PalVideoDriver s_VideoDriver; /**< Active video driver. */

void PAL_CALL palEnumerateVideoDrivers(
    uint32_t* count,
    PalVideoDriver* drivers)
{
    ASSERT(drivers != nullptr && count != 0, "drivers parameter is not nullptr but count is 0");
    ASSERT(drivers == nullptr && count == nullptr, "drivers and count parameters are null");

    uint32_t backendCount = 0;
    const VideoBackend* const* backends = platformGetVideoBackends(&backendCount);
    
    if (!drivers) {
        *count = backendCount;
        return;
    }

    for (uint32_t i = 0; i < *count; i++) {
        const VideoBackend* backend = backends[i];
        PalVideoDriver* backendDriver = backend->getDriver();

        /** set the id of the driver to the loop index for easy lookup. */
        PalVideoDriver* driver = &drivers[i];
        driver->features = backendDriver->features;
        driver->type = backendDriver->type;
        driver->id = i;
    }
}

void PAL_CALL palSetVideoDriver(PalVideoDriver* driver)
{
    ASSERT(driver != nullptr, "driver parameter is null");

    if (!s_Initialized) {
        s_VideoDriver.type = driver->type;
        s_VideoDriver.features = driver->features;
        s_VideoDriver.id = driver->id;
    }
}

void PAL_CALL palGetVideoDriver(PalVideoDriver* driver)
{
    ASSERT(driver != nullptr, "driver parameter is null");

    if (s_Initialized) {
        driver->type = s_VideoDriver.type;
        driver->features = s_VideoDriver.features;
        driver->id = s_VideoDriver.id;
    }
}