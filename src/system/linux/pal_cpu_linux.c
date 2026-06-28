/**
    PAL - Prime Abstraction Layer
    Copyright (C) 2025
    Licensed under the Zlib license. See LICENSE file in root.
 */

#ifdef __linux__
#define _POSIX_C_SOURCE 200112L
#include "pal/pal_system.h"
#include <errno.h>
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
        // get and convert to KB
        char unit;
        if (sscanf(cache, "%ld%c", &cacheSize, &unit) == 2) {
            // check if size is in MB or KB
            if (unit == 'M' || unit == 'm') {
                cacheSize *= 1024;
            }
        }
    }
    fclose(file);
    return cacheSize;
}

PalResult PAL_CALL palGetCPUInfo(
    const PalAllocator* allocator,
    PalCPUInfo* info)
{
    if (!info) {
        return PAL_RESULT_CODE_INVALID_ARGUMENT;
    }

    // check invalid allocator
    if (allocator && (!allocator->allocate || !allocator->free)) {
        return PAL_RESULT_CODE_INVALID_ARGUMENT;
    }

    FILE* file = fopen("/proc/cpuinfo", "r");
    if (!file) {
        return palMakeResult(PAL_RESULT_CODE_PLATFORM_FAILURE, PAL_RESULT_SOURCE_POSIX, errno);
    }

    char line[1024];
    // parse cpu information
    while (fgets(line, sizeof(line), file)) {
        if (strncmp(line, "vendor_id", 9) == 0) {
            sscanf(line, "vendor_id : %15[^\n]", info->vendor);

        } else if (strncmp(line, "model name", 10) == 0) {
            sscanf(line, "model name : %63[^\n]", info->model);

        } else if (strncmp(line, "cpu cores", 9) == 0) {
            sscanf(line, "cpu cores : %d", &info->numCores);

        } else if (strncmp(line, "flags", 5) == 0) {
            // extensions
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

    // get architecture
    struct utsname arch;
    if (uname(&arch) != 0) {
        return palMakeResult(PAL_RESULT_CODE_PLATFORM_FAILURE, PAL_RESULT_SOURCE_POSIX, errno);
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

    return PAL_RESULT_SUCCESS;
}

#endif // __linux__