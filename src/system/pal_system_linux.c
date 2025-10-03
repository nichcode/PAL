
/**

Copyright (C) 2025 Nicholas Agbo

This software is provided 'as-is', without any express or implied
warranty.  In no event will the authors be held liable for any damages
arising from the use of this software.

Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it
freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not
   claim that you wrote the original software. If you use this software
   in a product, an acknowledgment in the product documentation would be
   appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be
   misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.

 */

// ==================================================
// Includes
// ==================================================

#include "pal/pal_system.h"

#include <sys/sysinfo.h>
#include <sys/statvfs.h>
#include <sys/utsname.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h> 

// ==================================================
// Typedefs, enums and structs
// ==================================================

// ==================================================
// Internal API
// ==================================================

static Uint32 parseCache(const char* path) 
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

// ==================================================
// Public API
// ==================================================

PalResult PAL_CALL palGetPlatformInfo(PalPlatformInfo* info)
{
    if (!info) {
        return PAL_RESULT_NULL_POINTER;
    }

    info->type = PAL_PLATFORM_LINUX;
    const char* session = getenv("XDG_SESSION_TYPE");
    if (session) {
        if (strcmp(session, "wayland") == 0) {
            info->apiType = PAL_PLATFORM_API_WAYLAND;
        } else {
            info->apiType = PAL_PLATFORM_API_X11;
        }
    } else {
        // default
        info->apiType = PAL_PLATFORM_API_X11;
    }

    FILE* file = fopen("/etc/os-release", "r");
    if (!file) {
        return PAL_RESULT_PLATFORM_FAILURE;
    }

    char line[256];
    char name[15];
    char version[15];
    // parse version and name from the release file
    while (fgets(line, sizeof(line), file)) {
        if (strncmp(line, "NAME=", 5) == 0) {
            sscanf(line, "NAME=\"%15[^\"]", name);

        } else if (strncmp(line, "VERSION_ID=", 11) == 0) {
            sscanf(line, "VERSION_ID=\"%15[^\"]", version);
        }
    }

    // combine to get name
    snprintf(info->name, PAL_PLATFORM_NAME_SIZE, "%s %s", name, version);
    sscanf(version, "%d.%d", &info->version.major, &info->version.minor);
    fclose(file);

    // get total memory and disk space for the root drive
    struct sysinfo sysInfo;
    sysinfo(&sysInfo);
    info->totalRAM = (Uint32)(sysInfo.totalram / (1024 * 1024));

    struct statvfs stats;
    if (statvfs("/", &stats) != 0) {
        return PAL_RESULT_PLATFORM_FAILURE;
    }

    Uint64 size = (stats.f_blocks * stats.f_frsize) / (1024 * 1024 * 1024);
    info->totalMemory = (Uint32)size;
    return PAL_RESULT_SUCCESS;
}

PalResult PAL_CALL palGetCPUInfo(
    const PalAllocator* allocator,
    PalCPUInfo* info)
{
    if (!info) {
        return PAL_RESULT_NULL_POINTER;
    }

    // check invalid allocator
    if (allocator && (!allocator->allocate || !allocator->free)) {
        return PAL_RESULT_INVALID_ALLOCATOR;
    }

    FILE* file = fopen("/proc/cpuinfo", "r");
    if (!file) {
        return PAL_RESULT_PLATFORM_FAILURE;
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

    Uint32 l1 = parseCache("/sys/devices/system/cpu/cpu0/cache/index0/size");
    Uint32 l2 = parseCache("/sys/devices/system/cpu/cpu0/cache/index2/size");
    Uint32 l3 = parseCache("/sys/devices/system/cpu/cpu0/cache/index3/size");

    info->numLogicalProcessors = (Uint32)sysconf(_SC_NPROCESSORS_ONLN);
    info->cacheL1 = l1;
    info->cacheL2 = l2;
    info->cacheL3 = l3;

    // get architecture
    struct utsname arch;
    if (uname(&arch) != 0) {
        return PAL_RESULT_PLATFORM_FAILURE;
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