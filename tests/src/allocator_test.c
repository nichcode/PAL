
#include "tests.h"
#include "pal/pal_core.h"

#include <stdlib.h>
#include <string.h>

#define ALIGNMENT 16

// struct to pass in allocator
typedef struct {
    Int64 numAllocations;
    Int64 numDeallocations;
} AllocatorData;

typedef struct {
    char name[32];
    Int16 id;
} AllocationData;

void* allocatorAlloc(void* userData, Uint64 size, Uint64 alignment) {

    AllocatorData* data = userData;
    if (!data) {
        return nullptr;
    }

    void* memory = malloc(size);
    if (memory) {
        memset(memory, 0, size);
    } else {
        return nullptr;
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

    AllocatorData* data = palAllocate(NULL, sizeof(AllocatorData), ALIGNMENT);
    if (!data) {
        palLog("allocation failed - Out of memory");
        return;
    }

    memset(data, 0, sizeof(AllocatorData));

    PalAllocator allocator;
    allocator.allocate = allocatorAlloc;
    allocator.free = allocatorFree;
    allocator.userData = data;

    Int32* pInt32 = nullptr;
    Int64* pInt64 = nullptr;
    AllocationData* allocatedData1 = nullptr;
    AllocationData* allocatedData2 = nullptr;

    pInt32 = palAllocate(&allocator, sizeof(Int32), ALIGNMENT);
    if (!pInt32) {
        palLog("allocation failed - Out of memory");
        return;
    }

    pInt64 = palAllocate(&allocator, sizeof(Int64), ALIGNMENT);
    if (!pInt64) {
        palLog("allocation failed - Out of memory");
        return;
    }

    allocatedData1 = palAllocate(&allocator, sizeof(AllocationData), ALIGNMENT);
    if (!allocatedData1) {
        palLog("allocation failed - Out of memory");
        return;
    }

    allocatedData2 = palAllocate(&allocator, sizeof(AllocationData), ALIGNMENT);
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

    palFree(nullptr, data);
}