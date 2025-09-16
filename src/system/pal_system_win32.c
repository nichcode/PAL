
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

// ==================================================
// Typedefs, enums and structs
// ==================================================

typedef LONG(WINAPI* RtlGetVersionPtr)(PRTL_OSVERSIONINFOW);

// ==================================================
// Internal API
// ==================================================

static inline void cpuid(int regs[4], int leaf, int subLeaf)
{

    __asm__ __volatile__("cpuid"
                         : "=a"(regs[0]), "=b"(regs[1]), "=c"(regs[2]), "=d"(regs[3])
                         : "a"(leaf), "b"(subLeaf));
}

static inline bool getVersionWin32(PalVersion* version)
{

    OSVERSIONINFOEXW ver    = {0};
    ver.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEXW);

    HINSTANCE        ntdll  = GetModuleHandleW(L"ntdll.dll");
    RtlGetVersionPtr getVer = (RtlGetVersionPtr)GetProcAddress(ntdll, "RtlGetVersion");
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

static inline bool isVersionWin32(PalVersion* osVersion, Uint16 major, Uint16 minor, Uint16 build)
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

// ==================================================
// Public API
// ==================================================

PalResult PAL_API palGetPlatformInfo(PalPlatformInfo* info)
{

    if (!info) {
        return PAL_RESULT_NULL_POINTER;
    }

    info->apiType = PAL_PLATFORM_API_WIN32;
    info->type    = PAL_PLATFORM_WINDOWS;

    // get windows build, version and combine them
    if (!getVersionWin32(&info->version)) {
        return PAL_RESULT_PLATFORM_FAILURE;
    }

    const char* name  = nullptr;
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
        name  = "Windows 11";
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
        info->totalMemory = total.QuadPart / (1024 * 1024 * 1024); // to GB
    }

    // get ram (size) in MB
    MEMORYSTATUSEX status = {0};
    status.dwLength       = sizeof(MEMORYSTATUSEX);
    if (GlobalMemoryStatusEx(&status)) {
        info->totalRAM = status.ullTotalPhys / (1024 * 1024); // to MB
    }

    return PAL_RESULT_SUCCESS;
}

PalResult PAL_API palGetCPUInfo(const PalAllocator* allocator, PalCPUInfo* info)
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
    int regs[4] = {};
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

    // get cpu info
    DWORD len = 0;
    GetLogicalProcessorInformationEx(RelationAll, nullptr, &len);

    SYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX* buffer = nullptr;
    buffer                                          = palAllocate(allocator, len, 16);
    if (!buffer) {
        return PAL_RESULT_OUT_OF_MEMORY;
    }

    WINBOOL success = GetLogicalProcessorInformationEx(RelationAll, buffer, &len);

    if (!success) {
        palFree(allocator, buffer);
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
                info->numLogicalProcessors += _popcnt64(mask);
            }

        } else if (tmp->Relationship == RelationCache) {
            // cache size
            CACHE_RELATIONSHIP cache = tmp->Cache;
            if (cache.Level == 1) {
                info->cache1 = cache.CacheSize / 1024;
            }

            if (cache.Level == 2) {
                info->cache2 = cache.CacheSize / 1024;
            }

            if (cache.Level == 3) {
                info->cache3 = cache.CacheSize / 1024;
            }
        }
        ptr += tmp->Size;
    }

    palFree(allocator, buffer);

    // features
    PalCpuFeatures features;
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

    return PAL_RESULT_SUCCESS;
}