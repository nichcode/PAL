
/**
    PAL - Prime Abstraction Layer
    Copyright (C) 2025
    Licensed under the Zlib license. See LICENSE file in root.
 */

#ifdef _WIN32
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif // WIN32_LEAN_AND_MEAN

#ifndef NOMINMAX
#define NOMINMAX
#endif // NOMINMAX

// set unicode
#ifndef UNICODE
#define UNICODE
#endif // UNICODE

#include "pal2/pal_system.h"
#include <string.h>
#include <windows.h>

#if defined(_MSC_VER)
#include <intrin.h>
#endif // _MSC_VER

static inline void cpuid(
    int regs[4],
    int leaf,
    int subLeaf)
{
#if defined(_MSC_VER)
    __cpuidex(regs, leaf, subLeaf);
#else
    // gcc, clang
    __asm__ __volatile__("cpuid"
                         : "=a"(regs[0]), "=b"(regs[1]), "=c"(regs[2]), "=d"(regs[3])
                         : "a"(leaf), "b"(subLeaf));
#endif // _MSC_VER
}

void PAL_CALL palGetCPUInfo(
    const PalAllocator* allocator,
    PalCPUInfo* info)
{
    memset(info, 0, sizeof(PalCPUInfo));

    // get cpu vendor
    int regs[4] = {0};
    cpuid(regs, 0, 0);

    memcpy(info->vendor + 0, &regs[1], 4);
    memcpy(info->vendor + 4, &regs[3], 4);
    memcpy(info->vendor + 8, &regs[2], 4);
    info->vendor[12] = '\0'; // null terminating character

    // get cpu model name
    cpuid(regs, 0x80000002, 0);
    memcpy(info->model, regs, 16);

    cpuid(regs, 0x80000003, 0);
    memcpy(info->model + 16, regs, 16);

    cpuid(regs, 0x80000004, 0);
    memcpy(info->model + 32, regs, 16);
    info->model[48] = '\0';

    SYSTEM_INFO sysInfo;
    GetSystemInfo(&sysInfo);
    info->numLogicalProcessors = sysInfo.dwNumberOfProcessors;

    // get architecture
    switch (sysInfo.wProcessorArchitecture) {
        case PROCESSOR_ARCHITECTURE_INTEL: {
            info->architecture = PAL_CPU_ARCH_X86;
            break;
        }

        case PROCESSOR_ARCHITECTURE_AMD64: {
            info->architecture = PAL_CPU_ARCH_X86_64;
            break;
        }

        case PROCESSOR_ARCHITECTURE_ARM: {
            info->architecture = PAL_CPU_ARCH_ARM;
            break;
        }

        case PROCESSOR_ARCHITECTURE_ARM64: {
            info->architecture = PAL_CPU_ARCH_ARM64;
            break;
        }

        default: {
            info->architecture = PAL_CPU_ARCH_UNKNOWN;
        }
    }

    // get cpu info
    DWORD len = 0;
    GetLogicalProcessorInformationEx(RelationAll, nullptr, &len);
    SYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX* buffer = nullptr;
    buffer = palAllocate(allocator, len, 16);
    if (!buffer) {
        return;
    }

    BOOL ret = GetLogicalProcessorInformationEx(RelationAll, buffer, &len);
    if (!ret) {
        palFree(allocator, buffer);
        return;
    }

    char* ptr = (char*)buffer;
    while (ptr < (char*)buffer + len) {
        SYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX* tmp = (void*)ptr;
        if (tmp->Relationship == RelationProcessorCore) {
            info->numCores++;

        } else if (tmp->Relationship == RelationCache) {
            // cache size
            CACHE_RELATIONSHIP cache = tmp->Cache;
            if (cache.Level == 1) {
                info->cacheL1 = cache.CacheSize / 1024;
            }

            if (cache.Level == 2) {
                info->cacheL2 = cache.CacheSize / 1024;
            }

            if (cache.Level == 3) {
                info->cacheL3 = cache.CacheSize / 1024;
            }
        }
        ptr += tmp->Size;
    }

    palFree(allocator, buffer);

    // features
    PalCpuFeatures features = 0;
    cpuid(regs, 1, 0);
    int ecx = regs[2];
    int edx = regs[3];

    if (edx & (1 << 25)) {
        features |= PAL_CPU_FEATURE_SSE;
    }

    if (edx & (1 << 26)) {
        features |= PAL_CPU_FEATURE_SSE2;
    }

    if (ecx & (1 << 0)) {
        features |= PAL_CPU_FEATURE_SSE3;
    }

    if (ecx & (1 << 9)) {
        features |= PAL_CPU_FEATURE_SSSE3;
    }

    if (ecx & (1 << 19)) {
        features |= PAL_CPU_FEATURE_SSE41;
    }

    if (ecx & (1 << 20)) {
        features |= PAL_CPU_FEATURE_SSE42;
    }

    if (ecx & (1 << 28)) {
        features |= PAL_CPU_FEATURE_AVX;
    }

    if (ecx & (1 << 12)) {
        features |= PAL_CPU_FEATURE_FMA3;
    }

    // extended features
    cpuid(regs, 7, 0);
    int ebx = regs[1];

    if (ebx & (1 << 5)) {
        features |= PAL_CPU_FEATURE_AVX2;
    }

    if (ebx & (1 << 3)) {
        features |= PAL_CPU_FEATURE_BMI1;
    }

    if (ebx & (1 << 8)) {
        features |= PAL_CPU_FEATURE_BMI2;
    }

    if (ebx & (1 << 16)) {
        features |= PAL_CPU_FEATURE_AVX512F;
    }

    info->features = features;
}

#endif // _WIN32