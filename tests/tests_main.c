
#include "PAL/PAL.h"

int main(int argc, char** argv)
{
    PAL_BOOL success = PAL_Init();
    if (success) {
        PAL_Terminate();
        return 0;
    }

    return -1;
}