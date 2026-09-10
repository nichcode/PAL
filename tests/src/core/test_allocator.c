
#include "pal2/pal_core.h"
#include <stdbool.h>

typedef struct AllocatorContext
{
    uint32_t allocations;
    uint32_t deallocations;
} AllocatorContext;

void* PAL_CALL allocateFunction(
    void* userData, 
    uint64_t size, 
    uint64_t alignment)
{
    void* block = palAllocate(nullptr, size, alignment);
    if (!block) {
        return nullptr;
    }

    AllocatorContext* context = userData;
    context->allocations++;
    return block;
}

void PAL_CALL freeFunction(void* userData, void* ptr)
{
    palFree(nullptr, ptr);

    AllocatorContext* context = userData;
    context->deallocations++;
}

bool defaultAllocator()
{
    uint32_t* ptr1 = palAllocate(nullptr, sizeof(uint32_t), 0);
    uint64_t* ptr2 = palAllocate(nullptr, sizeof(uint64_t), 8);

    if (!ptr1 || !ptr2) {
        palLog(nullptr, "Failed to allocate memory with default allocator");
        return false;
    }

    *ptr1 = 100000;
    *ptr2 = 500000000;

    palFree(nullptr, ptr1);
    palFree(nullptr, ptr2);

    return true;
}

bool nullFree()
{
    uint32_t* ptr1 = palAllocate(nullptr, sizeof(uint32_t), 0);
    if (!ptr1) {
        palLog(nullptr, "Failed to allocate memory with default allocator");
        return false;
    }

    palFree(nullptr, ptr1);
    ptr1 = nullptr;
    palFree(nullptr, ptr1);

    return true;
}

bool customAllocator()
{
    AllocatorContext context = {0};
    context.allocations = 0;
    context.deallocations = 0;

    PalAllocator allocator = {0};
    allocator.allocate = allocateFunction;
    allocator.free = freeFunction;
    allocator.userData = &context;

    uint32_t* ptr1 = palAllocate(&allocator, sizeof(uint32_t), 0);
    uint64_t* ptr2 = palAllocate(&allocator, sizeof(uint64_t), 8);

    if (!ptr1 || !ptr2) {
        palLog(nullptr, "Failed to allocate memory with custom allocator");
        return false;
    }

    *ptr1 = 100000;
    *ptr2 = 500000000;

    palFree(&allocator, ptr1);
    palFree(&allocator, ptr2);

    return true;
}

int main(void)
{
    palLog(nullptr, "Running allocator test...");

    bool ret = defaultAllocator();
    if (!ret) {
        return -1;
    }

    ret = nullFree();
    if (!ret) {
        return -1;
    }

    ret = customAllocator();
    if (!ret) {
        return -1;
    }

    return 0;
}