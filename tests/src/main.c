
#include "pal/pal.h"

int main(int argc, char** argv) {

    palLogInfo(PAL_NULL, "Pal v(%s)", palGetVersionString());
    palLogInfo(PAL_NULL, "àà");

    return 0;
}