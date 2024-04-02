#include "CMemory.h"

#include "core/Logger.h"
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

static struct MemoryStats stats;

void InitializeMemory()
{
    PlatformZeroMem(&stats, sizeof(stats));
}

void ShutdownMemory()
{

}

void* cAllocate(u64 _size, MemoryTag _tag)
{
    if(_tag == MEMORY_TAG_UNKNOWN)
    {
        LOG_WARN("CAllocate called using MEMORY_TAG_UKNOWN, re-class this allocation.");
    }

    stats.totalAllocated += _size;
    stats.taggedAllocations[_tag] += _size;

    //TODO: detect mem alignment
    void* block = PlatformAllocate(_size, FALSE);
    PlatformZeroMem(block, _size);
    return block;
}

void cFree(void* _block, u64 _size, MemoryTag _tag)
{
    if(_tag == MEMORY_TAG_UNKNOWN)
    {
        LOG_WARN("CFree called using MEMORY_TAG_UKNOWN, re-class this allocation.");
    }

    stats.totalAllocated -= _size;
    stats.taggedAllocations[_tag] -= _size;

    //TODO: mem alignment
    PlatformFree(_block, FALSE);
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
        if(stats.taggedAllocations[i] >= gib)
        {
            unit[0] = 'G';
            amount = stats.taggedAllocations[i] / (float)gib;
        }
        else if(stats.taggedAllocations[i] >= mib)
        {
            unit[0] = 'M';
            amount = stats.taggedAllocations[i] / (float)mib;
        }
        else if(stats.taggedAllocations[i] >= kib)
        {
            unit[0] = 'K';
            amount = stats.taggedAllocations[i] / (float)kib;
        }
        else
        {
            unit[0] = 'B';
            unit[1] = 0;
            amount = (float)stats.taggedAllocations[i];
        }

        i32 length = snprintf(buffer + offset, 8000, "  %s: %.2f%s\n", memoryTagStrings[i], amount, unit);
        offset += length;
    }

    char* outStr = _strdup(buffer);
    return outStr;
}