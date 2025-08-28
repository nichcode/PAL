
#include "pal/pal_core.h"

// ==================================================
// Includes
// ==================================================

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

#include <windows.h>
#include <intrin.h> // for cpuid

// ==================================================
// Internal API
// ==================================================

static inline void getCpuid(
    int leaf, 
    Uint32 *eax, 
    Uint32 *ebx, 
    Uint32 *ecx, 
    Uint32 *edx) {

#if defined(_MSC_VER)
    int cpuidInfo[4];
    __cpuid(cpuidInfo, leaf);
    *eax = (Uint32)cpuidInfo[0];
    *ebx = (Uint32)cpuidInfo[1];
    *ecx = (Uint32)cpuidInfo[2];
    *edx = (Uint32)cpuidInfo[3];
#elif defined(__GNUC__) || defined(__clang__)
      Uint32 _eax, _ebx, _ecx, _edx;
    __get_cpuid(leaf, &_eax, &_ebx, &_ecx, &_edx);
    *eax = _eax;
    *ebx = _ebx;
    *ecx = _ecx;
    *edx = _edx;
#else
#error "Unknown compiler"
#endif // _MSC_VER
}

static inline void getCpuidCount(
    int leaf, 
    int subLeaf, 
    Uint32 *eax, 
    Uint32 *ebx, 
    Uint32 *ecx, 
    Uint32 *edx) {

#if defined(_MSC_VER)
    int cpuidInfo[4];
    __cpuidex(cpuidInfo, leaf, subLeaf);
    *eax = (Uint32)cpuidInfo[0];
    *ebx = (Uint32)cpuidInfo[1];
    *ecx = (Uint32)cpuidInfo[2];
    *edx = (Uint32)cpuidInfo[3];
#elif defined(__GNUC__) || defined(__clang__)
    __cpuid_count(leaf, subLeaf, eax, ebx, ecx, edx);
#else
#error "Unknown compiler"
#endif // _MSC_VER
}

// ==================================================
// Public API
// ==================================================

PalResult _PAPI palGetPlatformInfo(PalPlatformInfo *info) {

}

PalResult _PAPI palGetCpuInfo(PalCpuInfo *info) {

    if (!info) {
        return PAL_RESULT_NULL_POINTER;
    }

    memset(info, 0, sizeof(PalCpuInfo));

    // get cpu vendor
    int cpuInfo[4] = {};
    getCpuid(0, &cpuInfo[0], &cpuInfo[1], &cpuInfo[2], &cpuInfo[3]);

    memcpy(info->vendor + 0, &cpuInfo[1], 4);
    memcpy(info->vendor + 4, &cpuInfo[2], 4);
    memcpy(info->vendor + 8, &cpuInfo[3], 4);
    info->vendor[12] = '\0'; // null terminating character

    // get cpu model name
    getCpuid(0x80000002, &cpuInfo[0], &cpuInfo[1], &cpuInfo[2], &cpuInfo[3]);
    memcpy(info->model, cpuInfo, 16);

    getCpuid(0x80000003, &cpuInfo[0], &cpuInfo[1], &cpuInfo[2], &cpuInfo[3]);
    memcpy(info->model + 16, cpuInfo, 16);

    getCpuid(0x80000004, &cpuInfo[0], &cpuInfo[1], &cpuInfo[2], &cpuInfo[3]);
    memcpy(info->model + 32, cpuInfo, 16);
    info->model[48] = '\0';

  
    // get cpu info
    DWORD len = 0;
    GetLogicalProcessorInformationEx(
        RelationAll, 
        nullptr, 
        &len
    );

    SYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX* buffer = nullptr;
    buffer = malloc(len);
    if (!buffer) {
        return PAL_RESULT_OUT_OF_MEMORY;
    }

    WINBOOL success = GetLogicalProcessorInformationEx(
        RelationAll, 
        buffer, 
        &len
    );

    if (!success) {
        free(buffer);
        return PAL_RESULT_PLATFORM_FAILURE;
    }

    char* ptr = (char*)buffer;
    while (ptr < (char*)buffer + len) {
        SYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX* tmp = (void*)ptr;
        if (tmp->Relationship == RelationProcessorCore) {
            info->numCores++;

            // get the thread count for this core
            DWORD count = tmp->Processor.GroupCount;
            for (DWORD i = 0; i < count; i++) {
                KAFFINITY mask = tmp->Processor.GroupMask[i].Mask;
                info->numThreads += _popcnt64(mask);
            }

        } else if (tmp->Relationship == RelationCache) {
            // cache size
            CACHE_RELATIONSHIP cache = tmp->Cache;
            if (cache.Level == 1) {
                info->cacheKbL1 = cache.CacheSize / 1024;
            }

            if (cache.Level == 2) {
                info->cacheKbL2 = cache.CacheSize / 1024;
            }

            if (cache.Level == 3) {
                info->cacheKbL3 = cache.CacheSize / 1024;
            }

        }
        ptr += tmp->Size;
    }

    free(buffer);

    // features
    PalCpuFeatures features;
    getCpuid(1, &cpuInfo[0], &cpuInfo[1], &cpuInfo[2], &cpuInfo[3]);
    int ecx = cpuInfo[2];
    int edx = cpuInfo[3];

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

    if (ecx & (1 << 23)) {
        features |= PAL_CPU_FEATURE_POPCNT;
    }

    // extended features
    getCpuidCount(7, 0, &cpuInfo[0], &cpuInfo[1], &cpuInfo[2], &cpuInfo[3]);
    int ebx = cpuInfo[1];

    if (ebx & (1 << 5)) {
        features |= PAL_CPU_FEATURE_AVX2;
    }

    if (ebx & (1 << 16)) {
        features |= PAL_CPU_FEATURE_AVX512F;
    }

    if (ebx & (1 << 3)) {
        features |= PAL_CPU_FEATURE_BMI1;
    }

    if (ebx & (1 << 8)) {
        features |= PAL_CPU_FEATURE_BMI2;
    }

    if (ebx & (1 << 8)) {
        features |= PAL_CPU_FEATURE_BMI2;
    }

    getCpuid(0x80000001, &cpuInfo[0], &cpuInfo[1], &cpuInfo[2], &cpuInfo[3]);
    ecx = cpuInfo[2];

    if (ecx & (1 << 5)) {
        features |= PAL_CPU_FEATURE_LZCNT;
    }

    info->features = features;

    // check compile time architecture
#if defined(_M_X64) || defined(__x86_64__)
    info->architecture = PAL_CPU_ARCH_X86_64;
#elif defined(_M_IX86) || defined(__I386__)
    info->architecture = PAL_CPU_ARCH_X86;
#elif defined(_M_ARM64) || defined(__aarch64__)
    info->architecture = PAL_CPU_ARCH_ARM64;
#elif defined(_M_ARM) || defined(__arm__)
    info->architecture = PAL_CPU_ARCH_ARM;
#else 
    info->architecture = PAL_CPU_ARCH_UNKNOWN;
#endif // check compile time architecture
}

bool _PAPI palGetCpuid(
    Uint32 leaf, 
    Uint32 subLeaf, 
    PalCpuid *cpuid) {

    if (!cpuid) {
        return false;
    }

    getCpuidCount(
        leaf, 
        subLeaf, 
        &cpuid->eax,
        &cpuid->ebx,
        &cpuid->ecx,
        &cpuid->edx
    );
    return true;
}