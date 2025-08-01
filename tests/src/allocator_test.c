
#include "tests.h"
#include "pal/pal_core.h"

#include <stdlib.h>
#include <string.h>

// struct to pass in allocator
typedef struct {
    Int64 numAllocations;
    Int64 numDeallocations;
} AllocatorData;

typedef struct {
    char name[32];
    Int16 id;
} AllocationData;

void* allocatorAlloc(void* userData, Uint64 size) {

    AllocatorData* data = userData;
    if (!data) {
        return PAL_NULL;
    }

    void* memory = malloc(size);
    if (memory) {
        palZeroMemory(memory, size);
    } else {
        return PAL_NULL;
    }

    data->numAllocations++;
    palLog("Num Allocations: %li", data->numAllocations);
    return memory;
}

void allocatorFree(void* userData, void* ptr) {

    if (!ptr || !userData) {
        return;
    }

    AllocatorData* data = userData;
    free(ptr);

    data->numDeallocations++;
    palLog("Num Deallocations: %li", data->numDeallocations);
}

void allocatorTest() {

    palLog("");
    palLog("===========================================");
    palLog("Allocator Test");
    palLog("===========================================");
    palLog("");

    AllocatorData* data = palAllocate(NULL, sizeof(AllocatorData));
    if (!data) {
        palLog("allocation failed - Out of memory");
        return;
    }

    palZeroMemory(data, sizeof(AllocatorData));

    PalAllocator allocator;
    allocator.allocate = allocatorAlloc;
    allocator.free = allocatorFree;
    allocator.userData = data;

    Int32* pInt32 = PAL_NULL;
    Int64* pInt64 = PAL_NULL;
    AllocationData* allocatedData1 = PAL_NULL;
    AllocationData* allocatedData2 = PAL_NULL;

    pInt32 = palAllocate(&allocator, sizeof(Int32));
    if (!pInt32) {
        palLog("allocation failed - Out of memory");
        return;
    }

    pInt64 = palAllocate(&allocator, sizeof(Int64));
    if (!pInt64) {
        palLog("allocation failed - Out of memory");
        return;
    }

    allocatedData1 = palAllocate(&allocator, sizeof(AllocationData));
    if (!allocatedData1) {
        palLog("allocation failed - Out of memory");
        return;
    }

    allocatedData2 = palAllocate(&allocator, sizeof(AllocationData));
    if (!allocatedData2) {
        palLog("allocation failed - Out of memory");
        return;
    }

    *pInt32 = 100;
    *pInt64 = 100000;

    allocatedData1->id = 1;
    strcpy(allocatedData1->name, "AllocationData1");

    allocatedData2->id = 2;
    strcpy(allocatedData2->name, "AllocationData2");

    palLog("Int32 Pointer - %i", *pInt32);
    palLog("Int64 Pointer - %li", *pInt64);
    palLog("Allocation Data1 - (%s, %i)", allocatedData1->name, allocatedData1->id);
    palLog("Allocation Data2 - (%s, %i)", allocatedData2->name, allocatedData2->id);

    palFree(&allocator, pInt32);
    palFree(&allocator, pInt64);
    palFree(&allocator, allocatedData1);
    palFree(&allocator, allocatedData2);

    palFree(PAL_NULL, data);
}