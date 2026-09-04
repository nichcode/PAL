
/**
 * PAL - Prime Abstraction Layer (PAL)
 * A cross platform abstraction layer over graphics and windowing APIs
 * -------------------------------------------------------------------
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

#include "platform.h"
#if PLATFORM_POSIX_
#define _POSIX_C_SOURCE 200112L

#include "core/result_priv.h"

void formatResult_(
    PalResult result,
    char* buffer)
{
    uint32_t code = palGetResultNativeCode(result);
    PalResultSource source = palGetResultSource(result);

    if (code != 0 && source == PAL_RESULT_SOURCE_POSIX) {
        strerror_r(code, buffer, FORMAT_BUFFER_SIZE_);
    }
}

#endif // PLATFORM_POSIX_