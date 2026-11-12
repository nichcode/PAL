
#include "video.h"

bool videoStructs(void)
{
    if (!(g_DumpFlags & ABI_DUMP_QUICK)) {
        palLog(nullptr, "");
        palLog(nullptr, "===========================================");
        palLog(nullptr, "Video ABI Dump");
        palLog(nullptr, "===========================================");
        palLog(nullptr, "");
    }

    bool status = monitorInfoStruct();
    if (status == PAL_FALSE) {
        return PAL_FALSE;
    }

    status = monitorModeStruct();
    if (status == PAL_FALSE) {
        return PAL_FALSE;
    }

    status = flashInfoStruct();
    if (status == PAL_FALSE) {
        return PAL_FALSE;
    }

    status = iconCreateInfoStruct();
    if (status == PAL_FALSE) {
        return PAL_FALSE;
    }

    status = cursorCreateInfoStruct();
    if (status == PAL_FALSE) {
        return PAL_FALSE;
    }

    status = windowHandleInfoStruct();
    if (status == PAL_FALSE) {
        return PAL_FALSE;
    }

    status = windowCreateInfoStruct();
    if (status == PAL_FALSE) {
        return PAL_FALSE;
    }

    return videoDriverStruct();
}