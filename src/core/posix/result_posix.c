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

#define _POSIX_C_SOURCE 200112L

#include "core/platform.h"
#include "pal2/core/result.h"
#include <string.h>

#if PLATFORM_POSIX

void platformFormatResult(
    PalResult result,
    char* buffer)
{
    buffer[0] = '\0';

    uint32_t code = palGetResultNativeCode(result);
    PalResultSource source = palGetResultSource(result);

    if (code != 0 && source == PAL_RESULT_SOURCE_POSIX) {
#if defined(__GLIBC__) && defined(_GNU_SOURCE)
        const char* msg = strerror_r((int)code, buffer, FORMAT_BUFFER_SIZE);
        if (msg && msg != buffer) {
            (void)strncpy(buffer, msg, FORMAT_BUFFER_SIZE - 1);
            buffer[FORMAT_BUFFER_SIZE - 1] = '\0';
        }
#else
        if (strerror_r((int)code, buffer, FORMAT_BUFFER_SIZE) != 0) {
            buffer[0] = '\0';
        }
#endif
        buffer[FORMAT_BUFFER_SIZE - 1] = '\0';
    }
}

#endif // PLATFORM_POSIX