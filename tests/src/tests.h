
#ifndef _TESTS_H
#define _TESTS_H

#include "pal/pal_core.h"

#define MAX_TESTS 32 // will change

typedef void (*TestFn)();

typedef struct Tests {
    TestFn tests[MAX_TESTS];
    int count;
} Tests;

static Tests s_Test;

static inline void registerTest(TestFn fn) {

    s_Test.tests[s_Test.count++] = fn;
}

static inline void runTests() {

    for (int i = 0; i < s_Test.count; i++) {
        s_Test.tests[i]();
    }
}

// Tests
void allocatorTest();
void timeTest();
void videoTest();
void displayTest();
void displayModeTest();
void windowTest();
void eventsTest();
void multiWindowTest();
void inputTest();
void keyboardTest();

#endif // _TESTS_H