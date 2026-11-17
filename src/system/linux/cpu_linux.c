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

#include "system/platform.h"
#include "pal2/system/cpu.h"

#ifdef __linux__
#include <stdio.h>
#include <string.h>
#include <sys/utsname.h>
#include <unistd.h>
#include <stdbool.h>

static uint32_t parseCache(const char* path)
{
    long cacheSize = 0;
    FILE* file = fopen(path, "r");
    if (!file) {
        return 0;
    }

    char cache[16];
    if (fgets(cache, sizeof(cache), file)) {
        char unit = 0;
        if (sscanf(cache, "%ld%c", &cacheSize, &unit) >= 1) {
            if (unit == 'M' || unit == 'm') {
                cacheSize *= 1024;
            }
        }
    }

    fclose(file);

    return (cacheSize > 0) ? (uint32_t)cacheSize : 0;
}

void platformGetCPUInfo(
    const PalAllocator* allocator,
    PalCPUInfo* info)
{
    (void)allocator;

    long logical = sysconf(_SC_NPROCESSORS_ONLN);
    info->numLogicalProcessors = (logical > 0) ? (uint32_t)logical : 1;

    FILE* file = fopen("/proc/cpuinfo", "r");
    if (file) {
        char line[1024];
        bool parsedBasic = false;
        uint32_t coresPerSocket = 0;
        int32_t lastPhysicalId = -1;
        uint32_t socketCount = 0;

        while (fgets(line, sizeof(line), file)) {
            int32_t physicalId = 0;
            if (sscanf(line, "physical id : %d", &physicalId) == 1) {
                if (physicalId != lastPhysicalId) {
                    lastPhysicalId = physicalId;
                    socketCount++;
                }
            }

            if (parsedBasic) {
                continue;
            }

            if (strncmp(line, "vendor_id", 9) == 0) {
                sscanf(line, "vendor_id : %15[^\n]", info->vendor);

            } else if (strncmp(line, "model name", 10) == 0) {
                sscanf(line, "model name : %63[^\n]", info->model);

            } else if (strncmp(line, "cpu cores", 9) == 0) {
                sscanf(line, "cpu cores : %u", &coresPerSocket);

            } else if (strncmp(line, "flags", 5) == 0) {
                char* flags = strchr(line, ':');
                if (flags) {
                    flags++; // skip colon
                    char* savePtr = nullptr;
                    char* token = strtok_r(flags, " \t\n", &savePtr);
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

                        token = strtok_r(nullptr, " \t\n", &savePtr);
                    }
                }

                parsedBasic = true;
            }
        }

        fclose(file);

        if (coresPerSocket > 0) {
            uint32_t sockets = (socketCount > 0) ? socketCount : 1;
            info->numCores = coresPerSocket * sockets;
        } else {
            info->numCores = info->numLogicalProcessors;
        }
    } else {
        info->numCores = info->numLogicalProcessors;
    }

    info->cacheL1 = parseCache("/sys/devices/system/cpu/cpu0/cache/index0/size");
    info->cacheL2 = parseCache("/sys/devices/system/cpu/cpu0/cache/index2/size");
    info->cacheL3 = parseCache("/sys/devices/system/cpu/cpu0/cache/index3/size");

    struct utsname arch;
    if (uname(&arch) != 0) {
        return;
    }

    if (strcmp(arch.machine, "x86_64") == 0) {
        info->architecture = PAL_CPU_ARCH_X86_64;
    } else if (strcmp(arch.machine, "i386") == 0 || strcmp(arch.machine, "i686") == 0) {
        info->architecture = PAL_CPU_ARCH_X86;
    } else if (strcmp(arch.machine, "armv7l") == 0) {
        info->architecture = PAL_CPU_ARCH_ARM;
    } else if (strcmp(arch.machine, "aarch64") == 0) {
        info->architecture = PAL_CPU_ARCH_ARM64;
    }
}

#endif // __linux__