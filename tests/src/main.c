
#include "pal/pal.h"

int main(int argc, char** argv) 
{
    PalVersion version;
    palGetVerion(&version);
    palLogConsoleInfo("PAL v(%i.%i.%i)", version.major, version.minor, version.patch);

    return 0;
}