#include "LinearAllocatorTests.h"
#include "../TestManager.h"
#include "../Expect.h"

#include <Defines.h>

#include <memory/LinearAllocator.h>
#include <core/CMemory.h>

u8 LinearAllocatorShouldCreateAndDestroy()
{
    LinearAllocator alloc;
    LinearAllocatorCreate(sizeof(u64), 0, &alloc);

    ExpectShouldNotBe(0, alloc.memory);
    ExpectShouldBe(sizeof(u64), alloc.totalSize);
    ExpectShouldBe(0, alloc.allocated);

    LinearAllocatorDestroy(&alloc);

    ExpectShouldBe(0, alloc.memory);
    ExpectShouldBe(0, alloc.totalSize);
    ExpectShouldBe(0, alloc.allocated);

    return true;
}

u8 LinearAllocatorSingleAllocationAllSpace()
{
    LinearAllocator alloc;
    LinearAllocatorCreate(sizeof(u64), 0, &alloc);

    //single allocation
    void* block = LinearAllocatorAllocate(&alloc, sizeof(u64));

    //validate it
    ExpectShouldNotBe(0, block);
    ExpectShouldBe(sizeof(u64), alloc.allocated);

    LinearAllocatorDestroy(&alloc);

    return true;
}

u8 LinearAllocatorMultiAllocationAllSpace()
{
    u64 maxAllocs = 1024;
    LinearAllocator alloc;
    LinearAllocatorCreate(sizeof(u64) * maxAllocs, 0, &alloc);

    //multiple allocations - full
    void* block;
    for(u64 i = 0; i < maxAllocs; ++i)
    {
        block = LinearAllocatorAllocate(&alloc, sizeof(u64));
        //validate it
        ExpectShouldNotBe(0, block);
        ExpectShouldBe(sizeof(u64) * (i + 1), alloc.allocated);
    }

    LinearAllocatorDestroy(&alloc);

    return true;
}

u8 LinearAllocatorMultiAllocationOverAllocate()
{
    u64 maxAllocs = 3;
    LinearAllocator alloc;
    LinearAllocatorCreate(sizeof(u64) * maxAllocs, 0, &alloc);

    //multiple allocations - full
    void* block;
    for(u64 i = 0; i < maxAllocs; ++i)
    {
        block = LinearAllocatorAllocate(&alloc, sizeof(u64));
        //validate it
        ExpectShouldNotBe(0, block);
        ExpectShouldBe(sizeof(u64) * (i + 1), alloc.allocated);
    }

    LOG_DEBUG("Note: The following error is intentionally caused by this test.");

    //ask for another allocation, should error and return 0
    block = LinearAllocatorAllocate(&alloc, sizeof(u64));
    //validate it
    ExpectShouldBe(0, block);
    ExpectShouldBe(sizeof(u64) * (maxAllocs), alloc.allocated);

    LinearAllocatorDestroy(&alloc);

    return true;
}

u8 LinearAllocatorMultiAllocationAllSpaceThenFree()
{
    u64 maxAllocs = 1024;
    LinearAllocator alloc;
    LinearAllocatorCreate(sizeof(u64) * maxAllocs, 0, &alloc);

    //multiple allocations - full
    void* block;
    for(u64 i = 0; i < maxAllocs; ++i)
    {
        block = LinearAllocatorAllocate(&alloc, sizeof(u64));
        //validate it
        ExpectShouldNotBe(0, block);
        ExpectShouldBe(sizeof(u64) * (i + 1), alloc.allocated);
    }

    //validate that pointer is reset.
    LinearAllocatorFreeAll(&alloc);
    ExpectShouldBe(0, alloc.allocated);

    LinearAllocatorDestroy(&alloc);

    return true;
}

void LinearAllocatorRegisterTests()
{
    TestManagerRegisterTest(LinearAllocatorShouldCreateAndDestroy, "Linear allocator should create and destroy.");
    TestManagerRegisterTest(LinearAllocatorSingleAllocationAllSpace, "Linear allocator single alloc for all space.");
    TestManagerRegisterTest(LinearAllocatorMultiAllocationAllSpace, "Linear allocator multi alloc for all space.");
    TestManagerRegisterTest(LinearAllocatorMultiAllocationOverAllocate, "Linear allocator try over allocate.");
    TestManagerRegisterTest(LinearAllocatorMultiAllocationAllSpaceThenFree, "Linear allocator allocated should be 0 after FreeAll");
}