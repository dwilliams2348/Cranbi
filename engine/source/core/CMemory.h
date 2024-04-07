#pragma once

#include "Defines.h"

typedef enum MemoryTag
{
    //for temp use should be assigned one of the below tags or create a new tag
    MEMORY_TAG_UNKNOWN,
    MEMORY_TAG_ARRAY,
    MEMORY_TAG_LINEAR_ALLOCATOR,
    MEMORY_TAG_DARRAY,
    MEMORY_TAG_DICT,
    MEMORY_TAG_RING_QUEUE,
    MEMORY_TAG_BST,
    MEMORY_TAG_STRING,
    MEMORY_TAG_APPLICATION,
    MEMORY_TAG_JOB,
    MEMORY_TAG_TEXTURE,
    MEMORY_TAG_MATERIAL_INSTANCE,
    MEMORY_TAG_RENDERER,
    MEMORY_TAG_GAME,
    MEMORY_TAG_TRANSFORM,
    MEMORY_TAG_ENTITY,
    MEMORY_TAG_ENTITY_NODE,
    MEMORY_TAG_SCENE,

    MEMORY_TAG_MAX_TAGS
} MemoryTag;

CAPI void InitializeMemory();
CAPI void ShutdownMemory();

CAPI void* cAllocate(u64 _size, MemoryTag _tag);
CAPI void cFree(void* _block, u64 _size, MemoryTag _tag);
CAPI void* cZeroMemory(void* _block, u64 _size);
CAPI void* cCopyMemory(void* _dest, const void* _src, u64 _size);
CAPI void* cSetMemory(void* _dest, i32 _value, u64 _size);

CAPI char* GetMemoryUsageStr();

CAPI u64 GetMemoryAllocCount();