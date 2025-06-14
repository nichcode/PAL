
#include "tests.h"
#include "tests.c"

void onPALError(Uint32 code, const char* description)
{
    PAL_LogError(description);
}

int main(int argc, char** argv)
{
    PAL_SetErrorCallback(onPALError);

    //addTest("Allocator Test", allocatorTest);
    addTest("Window Test", windowTest);
    runTests();

    return 0;
}