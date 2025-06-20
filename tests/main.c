
#include "pal/pal.h"

int main(int argc, char** argv)
{
    PAL_Result result;
    result = palInit();
    if (result == PAL_RESULT_OK) {

        palTerminate();
        return 0;
    }

    return -1;
}