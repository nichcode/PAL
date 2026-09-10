
#include "helpers.h"

bool allocatorStruct();
bool versionStruct();
bool loggerStruct();

bool coreStructs()
{
    if (!(g_DumpFlags & ABI_DUMP_QUICK)) {
        palLog(nullptr, "");
        palLog(nullptr, "===========================================");
        palLog(nullptr, "Core Structs");
        palLog(nullptr, "===========================================");
        palLog(nullptr, "");
    }

    bool status = allocatorStruct();
    if (status == PAL_FALSE) {
        return PAL_FALSE;
    }

    status = versionStruct();
    if (status == PAL_FALSE) {
        return PAL_FALSE;
    }

    return loggerStruct();
}