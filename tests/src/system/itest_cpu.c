
#include "pal2/pal_system.h"
#include <string.h>

static const char* cpuArchToString(PalCpuArch arch)
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

int main(void)
{
    palLog(nullptr, "Running CPU itest...");

    PalCPUInfo info;
    palGetCPUInfo(nullptr, &info);

    palLog(nullptr, "Cpu: %s", info.model);
    palLog(nullptr, " Vendor: %s", info.vendor);
    palLog(nullptr, " Architecture: %s", cpuArchToString(info.architecture));
    palLog(nullptr, " Number Of Cores: %d", info.numCores);

    palLog(nullptr, " Number Of Processors: %d", info.numLogicalProcessors);
    palLog(nullptr, " L1 Cache KB: %d", info.cacheL1);
    palLog(nullptr, " L2 Cache KB: %d", info.cacheL2);
    palLog(nullptr, " L3 Cache KB: %d", info.cacheL3);

    char instructionSets[256] = {0};
    if (info.features & PAL_CPU_FEATURE_SSE) {
        strcat(instructionSets, "SSE");
    }

    if (info.features & PAL_CPU_FEATURE_SSE2) {
        strcat(instructionSets, " | SSE2");
    }

    if (info.features & PAL_CPU_FEATURE_SSE3) {
        strcat(instructionSets, " | SSE3");
    }

    if (info.features & PAL_CPU_FEATURE_SSSE3) {
        strcat(instructionSets, " | SSSE3");
    }

    if (info.features & PAL_CPU_FEATURE_SSE41) {
        strcat(instructionSets, " | SSE4.1");
    }

    if (info.features & PAL_CPU_FEATURE_SSE42) {
        strcat(instructionSets, " | SSE4.2");
    }

    if (info.features & PAL_CPU_FEATURE_AVX) {
        strcat(instructionSets, " | AVX");
    }

    if (info.features & PAL_CPU_FEATURE_AVX2) {
        strcat(instructionSets, " | AVX2");
    }

    if (info.features & PAL_CPU_FEATURE_AVX512F) {
        strcat(instructionSets, " | AVX-512F");
    }

    if (info.features & PAL_CPU_FEATURE_FMA3) {
        strcat(instructionSets, " | FMA3");
    }

    if (info.features & PAL_CPU_FEATURE_BMI1) {
        strcat(instructionSets, " | BMI1");
    }

    if (info.features & PAL_CPU_FEATURE_BMI2) {
        strcat(instructionSets, " | BMI2");
    }

    palLog(nullptr, " Instructions Sets: %s", instructionSets);

    return 0;
}