
#include "tests.h"

int main(int argc, char** argv) {

    registerTest("Log Test", logTest);
    registerTest("Video Test", videoTest);
    registerTest("Display Test", displayTest);

    runTests();
    return 0;
}