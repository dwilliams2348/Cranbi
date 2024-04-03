#include "DArray.h"

#include "core/CMemory.h"
#include "core/Logger.h"

void* _darray_create(u64 _length, u64 _stride)
{
    u64 headerSize = DARRAY_FIELD_LENGTH * sizeof(u64);
    u64 arraySize = _length * _stride;
    u64* newArray = cAllocate(headerSize + arraySize, MEMORY_TAG_DARRAY);
    cSetMemory(newArray, 0, headerSize + arraySize);
    newArray[DARRAY_CAPACITY] = _length;
    newArray[DARRAY_LENGTH] = 0;
    newArray[DARRAY_STRIDE] = _stride;
    return (void*)(newArray + DARRAY_FIELD_LENGTH);
}

void _darray_destroy(void* _array)
{
    u64* header = (u64*)_array - DARRAY_FIELD_LENGTH;
    u64 headerSize = DARRAY_FIELD_LENGTH * sizeof(u64);
    u64 totalSize = headerSize + header[DARRAY_CAPACITY] * header[DARRAY_STRIDE];
    cFree(header, totalSize, MEMORY_TAG_DARRAY);
}

u64 _darray_field_get(void* _array, u64 _field)
{
    u64* header = (u64*)_array - DARRAY_FIELD_LENGTH;
    return header[_field];
}

void _darray_field_set(void* _array, u64 _field, u64 _value)
{
    u64* header = (u64*)_array - DARRAY_FIELD_LENGTH;
    header[_field] = _value;
}

void* _darray_resize(void* _array)
{
    u64 length = DArrayLength(_array);
    u64 stride = DArrayStride(_array);
    void* temp = _darray_create((DARRAY_RESIZE_FACTOR * DArrayCapacity(_array)), stride);
    cCopyMemory(temp, _array, length * stride);

    _darray_field_set(temp, DARRAY_LENGTH, length);
    _darray_destroy(_array);
    return temp;
}

void* _darray_push(void* _array, const void* _valuePtr)
{
    u64 length = DArrayLength(_array);
    u64 stride = DArrayStride(_array);
    if(length >= DArrayCapacity(_array))
    {
        _array = _darray_resize(_array);
    }

    u64 addr = (u64)_array;
    addr += (length * stride);
    cCopyMemory((void*)addr, _valuePtr, stride);
    _darray_field_set(_array, DARRAY_LENGTH, length + 1);
    return _array;
}

void _darray_pop(void* _array, void* _dest)
{
    u64 length = DArrayLength(_array);
    u64 stride = DArrayStride(_array);
    u64 addr = (u64)_array;
    addr += ((length -1) * stride);
    cCopyMemory(_dest, (void*)addr, stride);
    _darray_field_set(_array, DARRAY_LENGTH, length - 1);
}

void* _darray_pop_at(void* _array, u64 _index, void* _dest)
{
    u64 length = DArrayLength(_array);
    u64 stride = DArrayStride(_array);
    if(_index >= length)
    {
        LOG_ERROR("Index outside the bounds of this array. Length: %i, index: %index", length, _index);
        return _array;
    }

    u64 addr = (u64)_array;
    cCopyMemory(_dest, (void*)(addr + (_index * stride)), stride);

    //if not last element snip out the entry and copyrest inward
    if(_index != length - 1)
    {
        cCopyMemory(
            (void*)(addr + (_index * stride)),
            (void*)(addr + ((_index + 1) * stride)),
            stride * (length - _index));
    }

    _darray_field_set(_array, DARRAY_LENGTH, length - 1);
    return _array;
}

void* _darray_insert_at(void* _array, u64 _index, void* _valuePtr)
{
    u64 length = DArrayLength(_array);
    u64 stride = DArrayStride(_array);
    if(_index >= length)
    {
        LOG_ERROR("Index outside the bounds of the array. Length: %i, index: %index", length, _index);
        return _array;
    }

    if(length >= DArrayCapacity(_array))
    {
        _array = _darray_resize(_array);
    }

    u64 addr = (u64)_array;

    //if not the last element copy rest outward
    if(_index != length - 1)
    {
        cCopyMemory(
            (void*)(addr + ((_index + 1) * stride)),
            (void*)(addr + (_index * stride)),
            stride * (length - _index));
    }

    //set value at index
    cCopyMemory((void*)(addr + (_index * stride)), _valuePtr, stride);

    _darray_field_set(_array, DARRAY_LENGTH, length + 1);
    return _array;
}