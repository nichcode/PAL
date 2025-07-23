
#include "tests.h"

int main(int argc, char** argv) {

//     registerTest("Log Test", logTest);
//     registerTest("Timer Test", timerTest);

// #if PAL_HAS_VIDEO
//     registerTest("Video Test", videoTest);
    //registerTest("Display Test", displayTest);
//     registerTest("Display Mode Test", displayModeTest);
//     registerTest("Window Test", windowTest);
// #endif // PAL_HAS_VIDEO

// #if PAL_HAS_EVENT
//     registerTest("Event Test", eventTest);
// #endif // PAL_HAS_EVENT

#if PAL_HAS_INPUT
    registerTest("Input Test", inputTest);
#endif // PAL_HAS_INPUT

    runTests();
    return 0;
}