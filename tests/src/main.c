
#include "tests.h"

int main(int argc, char** argv) {

    registerTest("Log Test", logTest);
    registerTest("Video Test", videoTest);

    runTests();
    return 0;
}