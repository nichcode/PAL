
#include "tests.h"

typedef struct Test
{
    const char* name;
    TestFun fun;

} Test;

static Test s_Tests[5] = {};
static Uint32 s_Count = 0;

void addTest(const char* name, TestFun fun)
{
    Test test;
    test.fun = fun;
    test.name = name;
    s_Tests[s_Count++] = test;
}

void runTests()
{
    for (Uint32 i = 0; i < s_Count; i++) {
        s_Tests[i].fun();
    }
}

// tests sources

#include "allocator_test.c"