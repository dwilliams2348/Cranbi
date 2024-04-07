#include "CMemory.h"

#include "core/Logger.h"
#include "core/CString.h"
#include "platform/Platform.h"

//TODO: custom string lib
#include <string.h>
#include <stdio.h>

struct MemoryStats
{
    u64 totalAllocated;
    u64 taggedAllocations[MEMORY_TAG_MAX_TAGS];
};

static const char* memoryTagStrings[MEMORY_TAG_MAX_TAGS] = {
    "UNKNOWN    ",
    "ARRAY      ",
    "LINEAR_ALLC",
    "DARRAY     ",
    "DICT       ",
    "RING_QUEUE ",
    "BST        ",
    "STRING     ",
    "APPLICATION",
    "JOB        ",
    "TEXTURE    ",
    "MAT_INST   ",
    "RENDERER   ",
    "GAME       ",
    "TRANSFORM  ",
    "ENTITY     ",
    "ENTITY_NODE",
    "SCENE      "};

typedef struct MemorySystemState
{
    struct MemoryStats stats;
    u64 allocCount;
} MemorySystemState;

static MemorySystemState* pState;

void InitializeMemory(u64* _memoryRequirement, void* _state)
{
    *_memoryRequirement = sizeof(MemorySystemState);
    if(_state == 0)
        return;
    
    pState = _state;
    pState->allocCount = 0;
    PlatformZeroMem(&pState->stats, sizeof(pState->stats));
}

void ShutdownMemory(void* _state)
{
    pState = 0;
}

void* cAllocate(u64 _size, MemoryTag _tag)
{
    if(_tag == MEMORY_TAG_UNKNOWN)
    {
        LOG_WARN("CAllocate called using MEMORY_TAG_UKNOWN, re-class this allocation.");
    }

    if(pState)
    {
        pState->stats.totalAllocated += _size;
        pState->stats.taggedAllocations[_tag] += _size;
        pState->allocCount++;
    }

    //TODO: memory alignment
    void* block = PlatformAllocate(_size, false);
    PlatformZeroMem(block, _size);
    return block;
}

void cFree(void* _block, u64 _size, MemoryTag _tag)
{
    if(_tag == MEMORY_TAG_UNKNOWN)
    {
        LOG_WARN("CFree called using MEMORY_TAG_UKNOWN, re-class this allocation.");
    }

    pState->stats.totalAllocated -= _size;
    pState->stats.taggedAllocations[_tag] -= _size;

    //TODO: mem alignment
    PlatformFree(_block, false);
}

void* cZeroMemory(void* _block, u64 _size)
{
    return PlatformZeroMem(_block, _size);
}

void* cCopyMemory(void* _dest, const void* _src, u64 _size)
{
    return PlatformCopyMem(_dest, _src, _size);
}

void* cSetMemory(void* _dest, i32 _value, u64 _size)
{
    return PlatformSetMem(_dest, _value, _size);
}

char* GetMemoryUsageStr()
{
    const u64 gib = 1024 * 1024 * 1024;
    const u64 mib = 1024 * 1024;
    const u64 kib = 1024;

    char buffer[8000] = "System memory use (tagged):\n";
    u64 offset = strlen(buffer);
    for(u32 i = 0; i < MEMORY_TAG_MAX_TAGS; ++i)
    {
        char unit[4] = "XiB";
        float amount = 1.f;
        if(pState->stats.taggedAllocations[i] >= gib)
        {
            unit[0] = 'G';
            amount = pState->stats.taggedAllocations[i] / (float)gib;
        }
        else if(pState->stats.taggedAllocations[i] >= mib)
        {
            unit[0] = 'M';
            amount = pState->stats.taggedAllocations[i] / (float)mib;
        }
        else if(pState->stats.taggedAllocations[i] >= kib)
        {
            unit[0] = 'K';
            amount = pState->stats.taggedAllocations[i] / (float)kib;
        }
        else
        {
            unit[0] = 'B';
            unit[1] = 0;
            amount = (float)pState->stats.taggedAllocations[i];
        }

        i32 length = snprintf(buffer + offset, 8000, "  %s: %.2f%s\n", memoryTagStrings[i], amount, unit);
        offset += length;
    }

    
    char* outStr = StringDuplicate(buffer);

    return outStr;
}

u64 GetMemoryAllocCount()
{
    if(pState)
        return pState->allocCount;
    
    return 0;
}