
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

const VideoBackend* g_VideoBackend = nullptr;
PalBool g_VideoInitialized = PAL_FALSE;
static const void* s_Instance = nullptr;
static PalVideoDriver s_Driver = {0}; /**< Active video driver. */

Keyboard g_Keyboard = {0};
Mouse g_Mouse = {0};

void PAL_CALL palEnumerateVideoDrivers(
    uint32_t* count,
    PalVideoDriver* drivers)
{
    ASSERT(count != nullptr, "count parameter is nullptr");
    ASSERT(drivers == nullptr || *count > 0, "drivers parameter is not nullptr but *count is 0");

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

    if (!g_VideoInitialized) {
        s_Driver.type = driver->type;
        s_Driver.features = driver->features;
        s_Driver.id = driver->id;
    }
}

void PAL_CALL palGetVideoDriver(PalVideoDriver* driver)
{
    ASSERT(driver != nullptr, "driver parameter is null");

    if (g_VideoInitialized) {
        driver->type = s_Driver.type;
        driver->features = s_Driver.features;
        driver->id = s_Driver.id;
    }
}

PalResult PAL_CALL palInitVideo(
    const PalAllocator* allocator,
    struct PalEventDriver* eventDriver,
    const void* preferredInstance)
{
    if (g_VideoInitialized) {
        return PAL_RESULT_SUCCESS;
    }

    uint32_t backendCount = 0;
    const VideoBackend* const* backends = platformGetVideoBackends(&backendCount);

    // The default driver is always the first video driver so a direct lookup
    // will work even if a user does not select a driver.
    void* selectedInstance = nullptr;
    const VideoBackend* backend = backends[s_Driver.id];
    PalResult result = backend->init(
        allocator, 
        eventDriver, 
        preferredInstance, 
        &selectedInstance);

    if (result != PAL_RESULT_SUCCESS) {
        // Check if there are more drivers and use that rather
        if (backendCount == 1) {
            return palMakeResult(
                PAL_RESULT_CODE_PLATFORM_FAILURE, 
                PAL_RESULT_SOURCE_NONE, 
                0);
        }

        for (uint32_t i = 1; i < backendCount; i++) {
            const VideoBackend* backend = backends[i];
            result = backend->init(
                allocator, 
                eventDriver, 
                preferredInstance, 
                &selectedInstance);

            if (result == PAL_RESULT_SUCCESS) {
                PalVideoDriver* driver = backend->getDriver();
                s_Driver.features = driver->features;
                s_Driver.id = driver->id;
                s_Driver.type = driver->type;

                break;
            }

            continue;
        }
    }

    g_VideoBackend = backend;
    g_VideoInitialized = PAL_TRUE;
    s_Instance = selectedInstance;

    return PAL_RESULT_SUCCESS;
}

void PAL_CALL palShutdownVideo(void)
{
    ASSERT(g_VideoInitialized != PAL_FALSE, "Video system not initialized");

    g_VideoBackend->shutdown();
    g_VideoBackend = nullptr;
    s_Instance = nullptr;

    memset(&g_Keyboard, 0, sizeof(Keyboard));
    memset(&g_Mouse, 0, sizeof(Mouse));
    g_VideoInitialized = PAL_FALSE;
}

void PAL_CALL palUpdateVideo(void)
{
    ASSERT(g_VideoInitialized != PAL_FALSE, "Video system not initialized");

    g_VideoBackend->update();
}

PalVideoFeatures PAL_CALL palGetVideoFeatures(void)
{
    if (g_VideoInitialized) {
        return s_Driver.features;
    }

    return 0;
}

const void* PAL_CALL palGetInstance(void)
{
    if (g_VideoInitialized) {
        return s_Instance;
    }

    return nullptr;
}

const PalBool* PAL_CALL palGetKeycodeState(void)
{
    return g_Keyboard.keycodeState;
}

const PalBool* PAL_CALL palGetScancodeState(void)
{
    return g_Keyboard.scancodeState;
}

const PalBool* PAL_CALL palGetMouseState(void)
{
    return g_Mouse.state;
}

void PAL_CALL palGetMouseDelta(
    float* dx,
    float* dy)
{
    if (dx) {
        *dx = (float)g_Mouse.dx;
    }

    if (dy) {
        *dy = (float)g_Mouse.dy;
    }
}

void PAL_CALL palGetMouseWheelDelta(
    float* dx,
    float* dy)
{
    if (dx) {
        *dx = (float)g_Mouse.WheelX;
    }

    if (dy) {
        *dy = (float)g_Mouse.WheelY;
    }
}