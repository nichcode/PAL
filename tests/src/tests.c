
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
    void* testData = getTestsData();
    for (int i = 0; i < s_Count; i++) {
        s_Tests[i].testFn(testData);
    }
}

void setTestsData(void* pData)
{
    s_TestData = pData;
}

void* getTestsData()
{
    return s_TestData;
}