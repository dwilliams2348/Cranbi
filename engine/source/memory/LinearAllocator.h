#pragma once

#include "Defines.h"

typedef struct LinearAllocator
{
    u64 totalSize;
    u64 allocated;
    void* memory;
    b8 ownsMemory;
} LinearAllocator;

CAPI void LinearAllocatorCreate(u64 _totalSize, void* _memory, LinearAllocator* _outAllocator);
CAPI void LinearAllocatorDestroy(LinearAllocator* _allocator);

CAPI void* LinearAllocatorAllocate(LinearAllocator* _allocator, u64 _size);
CAPI void LinearAllocatorFreeAll(LinearAllocator* _allocator);