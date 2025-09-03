
#include "tests.h"
#include "pal/pal_system.h"

#include <string.h> // for strcat

static inline const char* platformToString(
    PalPlatformType type) {

    switch (type) {
        case PAL_PLATFORM_WINDOWS:
        return "Windows";

        case PAL_PLATFORM_LINUX:
        return "Linux";

        case PAL_PLATFORM_MACOS:
        return "MacOs";

        case PAL_PLATFORM_ANDROID:
        return "Android";

        case PAL_PLATFORM_IOS:
        return "Ios";
    }
    return nullptr;
}

static inline const char* platformApiToString(
    PalPlatformApiType type) {

    switch (type) {
        case PAL_PLATFORM_API_WIN32: 
        return "Win32";

        case PAL_PLATFORM_API_X11: 
        return "X11";

        case PAL_PLATFORM_API_WAYLAND: 
        return "Wayland";
    }
    return nullptr;
}

static inline const char* cpuArchToString(
    PalCpuArch arch) {

    switch (arch) {
        case PAL_CPU_ARCH_UNKNOWN:
        return "Unknown";

        case PAL_CPU_ARCH_X86:
        return "X86";

        case PAL_CPU_ARCH_X86_64:
        return "X64";

        case PAL_CPU_ARCH_ARM:
        return "Arm";

        case PAL_CPU_ARCH_ARM64:
        return "Arm64";
    }
    return nullptr;
}

bool systemTest() {

    palLog(nullptr, "");
    palLog(nullptr, "===========================================");
    palLog(nullptr, "System Test");
    palLog(nullptr, "===========================================");
    palLog(nullptr, "");

    PalResult result;
    PalCPUInfo cpuInfo;
    PalPlatformInfo platformInfo;

    // get the platform info. Users must cache this
    result = palGetPlatformInfo(&platformInfo);
    if (result != PAL_RESULT_SUCCESS) {
        palLog(nullptr, "Failed to get platform info %s", palFormatResult(result));
        return false;
    }

    // user defined allocator, set to override the default one or nullptr for default
    result = palGetCPUInfo(nullptr, &cpuInfo);
    if (result != PAL_RESULT_SUCCESS) {
        palLog(nullptr, "Failed to get Cpu info %s", palFormatResult(result));
        return false;
    }

    // log platform information
    palLog(nullptr, "Platform: %s", platformToString(platformInfo.type));
    palLog(nullptr, " Name: %s", platformInfo.name);
    palLog(nullptr, " API: %s", platformApiToString(platformInfo.apiType));
    palLog(nullptr, " Total RAM: %llu MB", platformInfo.totalRAM);
    palLog(nullptr, " Total Memory: %llu GB", platformInfo.totalMemory);

    Uint16 major, minor, build;
    major = platformInfo.version.major;
    minor = platformInfo.version.minor;
    build = platformInfo.version.build;
    palLog(nullptr, " Version: (%d.%d.%d)", major, minor, build);

    // log cpu information
    palLog(nullptr, " Cpu: %s", cpuInfo.model);
    palLog(nullptr, "  Vendor: %s", cpuInfo.vendor);
    palLog(nullptr, "  Architecture: %s", cpuArchToString(cpuInfo.architecture));
    palLog(nullptr, "  Number Of Cores: %d", cpuInfo.numCores);
    palLog(nullptr, "  Number Of Logical Processors: %d", cpuInfo.numLogicalProcessors);
    palLog(nullptr, "  L1 Cache KB: %d", cpuInfo.cache1);
    palLog(nullptr, "  L2 Cache KB: %d", cpuInfo.cache2);
    palLog(nullptr, "  L3 Cache KB: %d", cpuInfo.cache3);

    // get instruction sets
    char instructionSets[256] = {};
    if (cpuInfo.features & PAL_CPU_FEATURE_SSE) {
        strcat(instructionSets, "SSE");
    }

    if (cpuInfo.features & PAL_CPU_FEATURE_SSE2) {
        strcat(instructionSets, " | SSE2");
    }

    if (cpuInfo.features & PAL_CPU_FEATURE_SSE3) {
        strcat(instructionSets, " | SSE3");
    }

    if (cpuInfo.features & PAL_CPU_FEATURE_SSSE3) {
        strcat(instructionSets, " | SSSE3");
    }

    if (cpuInfo.features & PAL_CPU_FEATURE_SSE41) {
        strcat(instructionSets, " | SSE4.1");
    }

    if (cpuInfo.features & PAL_CPU_FEATURE_SSE42) {
        strcat(instructionSets, " | SSE4.2");
    }

    if (cpuInfo.features & PAL_CPU_FEATURE_AVX) {
        strcat(instructionSets, " | AVX");
    }

    if (cpuInfo.features & PAL_CPU_FEATURE_AVX2) {
        strcat(instructionSets, "| AVX2");
    }

    if (cpuInfo.features & PAL_CPU_FEATURE_AVX512F) {
        strcat(instructionSets, "| AVX-512F");
    }

    if (cpuInfo.features & PAL_CPU_FEATURE_FMA3) {
        strcat(instructionSets, " | FMA3");
    }

    if (cpuInfo.features & PAL_CPU_FEATURE_BMI1) {
        strcat(instructionSets, " | BMI1");
    }

    if (cpuInfo.features & PAL_CPU_FEATURE_BMI2) {
        strcat(instructionSets, " | BMI2");
    }

    int a = strlen(instructionSets);

    palLog(nullptr, "  Instructions Sets: %s", instructionSets);

    return true;
}