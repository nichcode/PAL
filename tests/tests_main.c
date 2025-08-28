
#include "pal/pal_core.h"

int main(int argc, char**) {

    PalCpuInfo cpuInfo;
    palGetCpuInfo(&cpuInfo);

    return 0;
}