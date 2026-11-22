#include "pal2/system/cpu.h"
#include "pal2/core/log.h"
#include <stdio.h>
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

    return "Unknown";
}

static void appendFeature(char* dst, size_t dstSize, const char* name)
{
    size_t len = strlen(dst);
    if (len >= dstSize - 1) {
        return;
    }

    if (len > 0) {
        snprintf(dst + len, dstSize - len, " | %s", name);
    } else {
        snprintf(dst, dstSize, "%s", name);
    }
}

int main(void)
{
    palLog(nullptr, "Running CPU itest...");

    PalCPUInfo info = {0};
    palGetCPUInfo(nullptr, &info);

    palLog(nullptr, "Cpu: %s", info.model);
    palLog(nullptr, " Vendor: %s", info.vendor);
    palLog(nullptr, " Architecture: %s", cpuArchToString(info.architecture));
    palLog(nullptr, " Number Of Cores: %u", info.numCores);

    palLog(nullptr, " Number Of Processors: %u", info.numLogicalProcessors);
    palLog(nullptr, " L1 Cache KB: %u", info.cacheL1);
    palLog(nullptr, " L2 Cache KB: %u", info.cacheL2);
    palLog(nullptr, " L3 Cache KB: %u", info.cacheL3);

    char instructionSets[256] = {0};
    if (info.features & PAL_CPU_FEATURE_SSE) {
        appendFeature(instructionSets, sizeof(instructionSets), "SSE");
    }

    if (info.features & PAL_CPU_FEATURE_SSE2) {
        appendFeature(instructionSets, sizeof(instructionSets), "SSE2");
    }

    if (info.features & PAL_CPU_FEATURE_SSE3) {
        appendFeature(instructionSets, sizeof(instructionSets), "SSE3");
    }

    if (info.features & PAL_CPU_FEATURE_SSSE3) {
        appendFeature(instructionSets, sizeof(instructionSets), "SSSE3");
    }

    if (info.features & PAL_CPU_FEATURE_SSE41) {
        appendFeature(instructionSets, sizeof(instructionSets), "SSE4.1");
    }

    if (info.features & PAL_CPU_FEATURE_SSE42) {
        appendFeature(instructionSets, sizeof(instructionSets), "SSE4.2");
    }

    if (info.features & PAL_CPU_FEATURE_AVX) {
        appendFeature(instructionSets, sizeof(instructionSets), "AVX");
    }

    if (info.features & PAL_CPU_FEATURE_AVX2) {
        appendFeature(instructionSets, sizeof(instructionSets), "AVX2");
    }

    if (info.features & PAL_CPU_FEATURE_AVX512F) {
        appendFeature(instructionSets, sizeof(instructionSets), "AVX-512F");
    }

    if (info.features & PAL_CPU_FEATURE_FMA3) {
        appendFeature(instructionSets, sizeof(instructionSets), "FMA3");
    }

    if (info.features & PAL_CPU_FEATURE_BMI1) {
        appendFeature(instructionSets, sizeof(instructionSets), "BMI1");
    }
    
    if (info.features & PAL_CPU_FEATURE_BMI2) {
        appendFeature(instructionSets, sizeof(instructionSets), "BMI2");
    }

    palLog(
        nullptr,
        " Instruction Sets: %s",
        instructionSets[0] != '\0' ? instructionSets : "None");

    return 0;
}