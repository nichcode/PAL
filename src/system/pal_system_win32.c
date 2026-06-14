
/**
    PAL - Prime Abstraction Layer
    Copyright (C) 2025
    Licensed under the Zlib license. See LICENSE file in root.
 */

#include "pal/pal_system.h"

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

#include <string.h>
#include <windows.h>

#if defined(_MSC_VER)
#include <intrin.h>
#endif // _MSC_VER

typedef LONG(WINAPI* RtlGetVersionFn)(PRTL_OSVERSIONINFOW);

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

static inline PalBool getVersionWin32(PalVersion* version)
{
    OSVERSIONINFOEXW ver = {0};
    ver.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEXW);
    HINSTANCE ntdll = GetModuleHandleW(L"ntdll.dll");
    RtlGetVersionFn getVer = (RtlGetVersionFn)GetProcAddress(ntdll, "RtlGetVersion");
    if (!getVer) {
        return false;
    }

    if (getVer((PRTL_OSVERSIONINFOW)&ver)) {
        return false;
    }

    version->major = ver.dwMajorVersion;
    version->minor = ver.dwMinorVersion;
    version->build = ver.dwBuildNumber;
    return true;
}

static inline PalBool isVersionWin32(
    PalVersion* osVersion,
    uint16_t major,
    uint16_t minor,
    uint16_t build)
{
    if (osVersion->major > major) {
        return true;
    }

    if (osVersion->major < major) {
        return false;
    }

    if (osVersion->minor > minor) {
        return true;
    }

    if (osVersion->minor < minor) {
        return false;
    }

    return osVersion->build >= build;
}

PalResult PAL_CALL palGetPlatformInfo(PalPlatformInfo* info)
{
    if (!info) {
        return PAL_RESULT_NULL_POINTER;
    }

    info->apiType = PAL_PLATFORM_API_WIN32;
    info->type = PAL_PLATFORM_WINDOWS;

    // get windows build, version and combine them
    if (!getVersionWin32(&info->version)) {
        return PAL_RESULT_PLATFORM_FAILURE;
    }

    const char* name = nullptr;
    const char* build = nullptr;
    // check the versions and set the appropriate name
    if (isVersionWin32(&info->version, 5, 1, 0)) {
        name = "Windows XP";
    }

    if (isVersionWin32(&info->version, 6, 0, 0)) {
        name = "Windows Vista";
    }

    if (isVersionWin32(&info->version, 6, 1, 0)) {
        name = "Windows 7";
    }

    if (isVersionWin32(&info->version, 6, 2, 0)) {
        name = "Windows 8";
    }

    if (isVersionWin32(&info->version, 6, 3, 0)) {
        name = "Windows 8.1";
    }

    if (isVersionWin32(&info->version, 10, 0, 0)) {
        name = "Windows 10";
    }

    if (isVersionWin32(&info->version, 10, 0, 22000)) {
        name = "Windows 11";
        build = ".22000";
    }

    // combine them into a single string
    strcpy(info->name, name);
    if (build) {
        strcat(info->name, build);
    }

    // get total disk memory (size) in GB
    ULARGE_INTEGER free, total, available;
    if (GetDiskFreeSpaceExW(L"C:\\", &available, &total, &free)) {
        info->totalMemory = (uint32_t)(total.QuadPart / (1024 * 1024 * 1024));
    }

    // get ram (size) in MB
    MEMORYSTATUSEX status = {0};
    status.dwLength = sizeof(MEMORYSTATUSEX);
    if (GlobalMemoryStatusEx(&status)) {
        info->totalRAM = (uint32_t)(status.ullTotalPhys / (1024 * 1024));
    }

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
        return PAL_RESULT_OUT_OF_MEMORY;
    }

    BOOL ret = GetLogicalProcessorInformationEx(RelationAll, buffer, &len);
    if (!ret) {
        palFree(allocator, buffer);
        return PAL_RESULT_PLATFORM_FAILURE;
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
    return PAL_RESULT_SUCCESS;
}

#endif // _WIN32