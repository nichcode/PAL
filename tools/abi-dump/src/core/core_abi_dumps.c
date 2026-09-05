
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

#include "abi_dump.h"

bool allocatorDump(uint32_t flags);
bool versionDump(uint32_t flags);
bool loggerDump(uint32_t flags);

bool coreABIDumps(uint32_t flags)
{
    if (!(flags & ABI_DUMP_FLAG_QUICK)) {
        palLog(nullptr, "");
        palLog(nullptr, "===========================================");
        palLog(nullptr, "Core ABI Dumps");
        palLog(nullptr, "===========================================");
        palLog(nullptr, "");
    }

    bool status = allocatorDump(flags);
    if (status == PAL_FALSE) {
        return PAL_FALSE;
    }

    status = versionDump(flags);
    if (status == PAL_FALSE) {
        return PAL_FALSE;
    }

    return loggerDump(flags);
}