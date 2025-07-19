
#include "tests.h"

int main(int argc, char** argv) {

    // registerTest("Log Test", logTest);
    // registerTest("Video Test", videoTest);
    // registerTest("Display Test", displayTest);
    // registerTest("Display Mode Test", displayModeTest);
    //registerTest("Window Test", windowTest);
    registerTest("Timer Test", timerTest);

    runTests();
    return 0;
}