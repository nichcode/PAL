
#include "tests.h"
#include "pal/pal_system.h"

static inline const char* getCpuArchString(PalCpuArch arch) {
    
    return nullptr;
}

bool systemTest() {

    PalResult result;
    PalCpuInfo cpuInfo;
    PalPlatformInfo platformInfo;

    // user defined allocator, set to override the default one or nullptr for default
    result = palGetCpuInfo(nullptr, &cpuInfo);
    if (result != PAL_RESULT_SUCCESS) {
        return false;
    }

    result = palGetPlatformInfo(&platformInfo);
    if (result != PAL_RESULT_SUCCESS) {
        return false;
    }

    // log information

    return true;
}