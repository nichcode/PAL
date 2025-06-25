
#ifndef _TESTS_H
#define _TESTS_H

#include "pal/pal.h"

typedef bool (*TestFn)(void* pData);

void registerTests(const char* pName, TestFn pTestFn);
void runTests();

void setTestsData(void* pData);
void* getTestsData();

#endif // _TESTS_H