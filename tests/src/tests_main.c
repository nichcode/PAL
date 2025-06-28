
#include "tests.h"

#define MAX_TESTS 16 // will change

struct TestEntry
{
    const char* name;
    TestFn testFn;
};

struct Tests
{
    int count;
    struct TestEntry tests[MAX_TESTS];
};

// tests array
static struct TestEntry s_Tests[MAX_TESTS] = {};
static void* s_TestData = PAL_NULL;
static int s_Count = 0;

void registerTest(const char* pName, TestFn pTestFn)
{
    struct TestEntry entry;
    entry.name = pName;
    entry.testFn = pTestFn;

    s_Tests[s_Count++] = entry;
}

void runTests()
{
    for (int i = 0; i < s_Count; i++) {
        s_Tests[i].testFn();
    }
}

int main(int argc, char** argv)
{
    registerTest("Core Test", coreTest);
    registerTest("Display Test", displayTest);

    runTests();
    return 0;
}