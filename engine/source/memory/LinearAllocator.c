#include "LinearAllocator.h"

#include "core/CMemory.h"
#include "core/Logger.h"

void LinearAllocatorCreate(u64 _totalSize, void* _memory, LinearAllocator* _outAllocator) 
{
    if(_outAllocator)
    {
        _outAllocator->totalSize = _totalSize;
        _outAllocator->allocated = 0;
        _outAllocator->ownsMemory = _memory == 0;
        if(_memory)
        {
            _outAllocator->memory = _memory;
        }
        else
        {
            _outAllocator->memory = cAllocate(_totalSize, MEMORY_TAG_LINEAR_ALLOCATOR);
        }
    }
}

void LinearAllocatorDestroy(LinearAllocator* _allocator) 
{
    if(_allocator)
    {
        _allocator->allocated = 0;
        if(_allocator->ownsMemory && _allocator->memory)
        {
            cFree(_allocator->memory, _allocator->totalSize, MEMORY_TAG_LINEAR_ALLOCATOR);
        }
        _allocator->memory = 0;
        _allocator->totalSize = 0;
        _allocator->ownsMemory = false;
    }
}

void* LinearAllocatorAllocate(LinearAllocator* _allocator, u64 _size) 
{
    if(_allocator && _allocator->memory)
    {
        if(_allocator->allocated + _size > _allocator->totalSize)
        {
            u64 remaining = _allocator->totalSize - _allocator->allocated;
            LOG_ERROR("LinearAllocatorAllocate - Tried to allocate %lluB, only %lluB remaining", _size, remaining);
            return 0;
        }

        void* block = ((u8*)_allocator->memory) + _allocator->allocated;
        _allocator->allocated += _size;
        return block;
    }

    LOG_ERROR("LinearAllocatorAllocate - provided allocator not initalized.");
    return 0;
}

void LinearAllocatorFreeAll(LinearAllocator* _allocator) 
{
    if(_allocator && _allocator->ownsMemory)
    {
        _allocator->allocated = 0;
        cZeroMemory(_allocator->memory, _allocator->totalSize);
    }
}
