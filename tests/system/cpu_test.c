
#include "pal2/pal_system.h"
#include "tests.h"
#include <string.h> // for strcat

static inline const char* cpuArchToString(PalCpuArch arch)
{
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

PalBool cpuTest()
{
    PalResult result;
    PalCPUInfo cpuInfo;

    // user defined allocator, set to override the default one or nullptr for default
    palGetCPUInfo(nullptr, &cpuInfo);
    const char* archString = cpuArchToString(cpuInfo.architecture);
    int32_t processors = cpuInfo.numLogicalProcessors;

    palLog(nullptr, " Cpu: %s", cpuInfo.model);
    palLog(nullptr, "  Vendor: %s", cpuInfo.vendor);
    palLog(nullptr, "  Architecture: %s", archString);
    palLog(nullptr, "  Number Of Cores: %d", cpuInfo.numCores);
    palLog(nullptr, "  Number Of Processors: %d", processors);
    palLog(nullptr, "  L1 Cache KB: %d", cpuInfo.cacheL1);
    palLog(nullptr, "  L2 Cache KB: %d", cpuInfo.cacheL2);
    palLog(nullptr, "  L3 Cache KB: %d", cpuInfo.cacheL3);

    // get instruction sets
    char instructionSets[256] = {0};
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
        strcat(instructionSets, " | AVX2");
    }

    if (cpuInfo.features & PAL_CPU_FEATURE_AVX512F) {
        strcat(instructionSets, " | AVX-512F");
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

    palLog(nullptr, "  Instructions Sets: %s", instructionSets);

    return PAL_TRUE;
}
