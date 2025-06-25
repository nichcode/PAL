
#ifndef _TESTS_H
#define _TESTS_H

#include "pal/pal.h"

typedef bool (*TestFn)(void* data);

void registerTest(const char* name, TestFn testFn);
void runTests();

void setTestsData(void* data);
void* getTestsData();

// Tests
bool systemTest(void* data);
bool videoTest(void* data);

#endif // _TESTS_H