
#include "pal/pal.h"

int main(int argc, char** argv)
{
    if (palInitSystem(PAL_NULL, PAL_TRUE)) {
        const PalVersion* version = palGetVersion();


        palShutdownSystem();
        return 0;
    }

    return -1;
}