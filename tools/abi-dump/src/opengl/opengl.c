
#include "opengl.h"

bool openglStructs(void)
{
    if (!(g_DumpFlags & ABI_DUMP_QUICK)) {
        palLog(nullptr, "");
        palLog(nullptr, "===========================================");
        palLog(nullptr, "Opengl ABI Dump");
        palLog(nullptr, "===========================================");
        palLog(nullptr, "");
    }

    bool status = openglInfoStruct();
    if (status == PAL_FALSE) {
        return PAL_FALSE;
    }

    status = openglFBConfigStruct();
    if (status == PAL_FALSE) {
        return PAL_FALSE;
    }

    status = openglWindowStruct();
    if (status == PAL_FALSE) {
        return PAL_FALSE;
    }

    return contextCreateInfoStruct();
}