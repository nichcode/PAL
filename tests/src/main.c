
#include "pal/pal.h"

int main(int argc, char** argv) {

    PalVersion version = palGetVersion();
    const char* versionString = palGetVersionString();

    Int32* pInt = palAllocate(PAL_NULL, sizeof(Int32));

    *pInt = 100;
    palLog("%i", *pInt);
    palLog("Unicode - àà");

    if (pInt) {
        palFree(PAL_NULL, pInt);
    }

    return 0;
}