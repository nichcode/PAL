
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

#ifdef _WIN32
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif // WIN32_LEAN_AND_MEAN

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
    // clang-format off
    __asm__ __volatile__("cpuid"
                         : "=a"(regs[0]),
                           "=b"(regs[1]),
                           "=c"(regs[2]),
                           "=d"(regs[3])
                         : "a"(leaf),
                           "b"(subLeaf));
    // clang-format on
#endif // _MSC_VER
}

void platformGetCPUInfo(
    const PalAllocator* allocator,
    PalCPUInfo* info)
{
    int regs[4] = {0};
    cpuid(regs, 0, 0);

    memcpy(info->vendor + 0, &regs[1], 4);
    memcpy(info->vendor + 4, &regs[3], 4);
    memcpy(info->vendor + 8, &regs[2], 4);
    info->vendor[12] = '\0';

    int tmp = (int)0x80000002;
    cpuid(regs, tmp, 0);
    memcpy(info->model, regs, 16);

    tmp = (int)0x80000003;
    cpuid(regs, tmp, 0);
    memcpy(info->model + 16, regs, 16);

    tmp = (int)0x80000004;
    cpuid(regs, tmp, 0);
    memcpy(info->model + 32, regs, 16);
    info->model[48] = '\0';

    SYSTEM_INFO sysInfo;
    GetSystemInfo(&sysInfo);
    info->numLogicalProcessors = sysInfo.dwNumberOfProcessors;

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

    info->architecture = PAL_CPU_ARCH_UNKNOWN;
    }

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

    cpuid(regs, 1, 0);
    int ecx = regs[2];
    int edx = regs[3];

    if (edx & (1 << 25)) {
        info->features |= PAL_CPU_FEATURE_SSE;
    }

    if (edx & (1 << 26)) {
        info->features |= PAL_CPU_FEATURE_SSE2;
    }

    if (ecx & (1 << 0)) {
        info->features |= PAL_CPU_FEATURE_SSE3;
    }

    if (ecx & (1 << 9)) {
        info->features |= PAL_CPU_FEATURE_SSSE3;
    }

    if (ecx & (1 << 19)) {
        info->features |= PAL_CPU_FEATURE_SSE41;
    }

    if (ecx & (1 << 20)) {
        info->features |= PAL_CPU_FEATURE_SSE42;
    }

    if (ecx & (1 << 28)) {
        info->features |= PAL_CPU_FEATURE_AVX;
    }

    if (ecx & (1 << 12)) {
        info->features |= PAL_CPU_FEATURE_FMA3;
    }

    cpuid(regs, 7, 0);
    int ebx = regs[1];

    if (ebx & (1 << 5)) {
        info->features |= PAL_CPU_FEATURE_AVX2;
    }

    if (ebx & (1 << 3)) {
        info->features |= PAL_CPU_FEATURE_BMI1;
    }

    if (ebx & (1 << 8)) {
        info->features |= PAL_CPU_FEATURE_BMI2;
    }

    if (ebx & (1 << 16)) {
        info->features |= PAL_CPU_FEATURE_AVX512F;
    }
}

#endif // _WIN32