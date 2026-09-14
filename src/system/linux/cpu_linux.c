
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

#include "shared.h"
#include "system/system_platform.h"

#ifdef __linux__
#include <stdio.h>
#include <sys/utsname.h>
#include <unistd.h>

static uint32_t parseCache(const char* path)
{
    long cacheSize = 0;
    FILE* file = fopen(path, "r");
    if (!file) {
        return 0;
    }

    char cache[16];
    if (fgets(cache, sizeof(cache), file)) {
        char unit;
        if (sscanf(cache, "%ld%c", &cacheSize, &unit) == 2) {
            if (unit == 'M' || unit == 'm') {
                cacheSize *= 1024;
            }
        }
    }

    fclose(file);

    return (uint32_t)cacheSize;
}

void platformGetCPUInfo(
    const PalAllocator* allocator,
    PalCPUInfo* info)
{
    (void)allocator;

    FILE* file = fopen("/proc/cpuinfo", "r");
    if (!file) {
        return;
    }

    char line[1024];

    while (fgets(line, sizeof(line), file)) {
        if (strncmp(line, "vendor_id", 9) == 0) {
            sscanf(line, "vendor_id : %15[^\n]", info->vendor);

        } else if (strncmp(line, "model name", 10) == 0) {
            sscanf(line, "model name : %63[^\n]", info->model);

        } else if (strncmp(line, "cpu cores", 9) == 0) {
            sscanf(line, "cpu cores : %u", &info->numCores);

        } else if (strncmp(line, "flags", 5) == 0) {
            char* flags = strchr(line, ':');
            if (flags) {
                flags++; // skip colon
                char* token = strtok(flags, " \t\n");
                while (token) {
                    if (strcmp(token, "sse") == 0) {
                        info->features |= PAL_CPU_FEATURE_SSE;

                    } else if (strcmp(token, "sse2") == 0) {
                        info->features |= PAL_CPU_FEATURE_SSE2;

                    } else if (strcmp(token, "sse3") == 0) {
                        info->features |= PAL_CPU_FEATURE_SSE3;

                    } else if (strcmp(token, "ssse3") == 0) {
                        info->features |= PAL_CPU_FEATURE_SSSE3;

                    } else if (strcmp(token, "sse4_1") == 0) {
                        info->features |= PAL_CPU_FEATURE_SSE41;

                    } else if (strcmp(token, "sse4_2") == 0) {
                        info->features |= PAL_CPU_FEATURE_SSE42;

                    } else if (strcmp(token, "avx") == 0) {
                        info->features |= PAL_CPU_FEATURE_AVX;

                    } else if (strcmp(token, "avx2") == 0) {
                        info->features |= PAL_CPU_FEATURE_AVX2;

                    } else if (strcmp(token, "avx512f") == 0) {
                        info->features |= PAL_CPU_FEATURE_AVX512F;

                    } else if (strcmp(token, "fma") == 0) {
                        info->features |= PAL_CPU_FEATURE_FMA3;

                    } else if (strcmp(token, "bmi1") == 0) {
                        info->features |= PAL_CPU_FEATURE_BMI1;

                    } else if (strcmp(token, "bmi2") == 0) {
                        info->features |= PAL_CPU_FEATURE_BMI2;
                    }

                    token = strtok(nullptr, " \t\n");
                }
            }

            break; // all CPUs are the same
        }
    }

    fclose(file);

    uint32_t l1 = parseCache("/sys/devices/system/cpu/cpu0/cache/index0/size");
    uint32_t l2 = parseCache("/sys/devices/system/cpu/cpu0/cache/index2/size");
    uint32_t l3 = parseCache("/sys/devices/system/cpu/cpu0/cache/index3/size");

    info->numLogicalProcessors = (uint32_t)sysconf(_SC_NPROCESSORS_ONLN);
    info->cacheL1 = l1;
    info->cacheL2 = l2;
    info->cacheL3 = l3;

    struct utsname arch;
    if (uname(&arch) != 0) {
        return;
    }

    if (strcmp(arch.machine, "x86_64") == 0) {
        info->architecture = PAL_CPU_ARCH_X86_64;
    }

    if (strcmp(arch.machine, "i386") == 0) {
        info->architecture = PAL_CPU_ARCH_X86;
    }

    if (strcmp(arch.machine, "i686") == 0) {
        info->architecture = PAL_CPU_ARCH_X86;
    }

    if (strcmp(arch.machine, "armv71") == 0) {
        info->architecture = PAL_CPU_ARCH_ARM;
    }

    if (strcmp(arch.machine, "aarch64") == 0) {
        info->architecture = PAL_CPU_ARCH_ARM64;
    }
}

#endif // __linux__