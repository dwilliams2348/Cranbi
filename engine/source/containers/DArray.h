#pragma once

#include "Defines.h"

/**
 * Memory layout
 * u64 capacity = number of elements that can be held.
 * u64 lenght = number of elements currently contained.
 * u64 stride = size of each element in bytes
 * void* elements
*/

enum 
{
    DARRAY_CAPACITY,
    DARRAY_LENGTH,
    DARRAY_STRIDE,
    DARRAY_FIELD_LENGTH
};

CAPI void* _darray_create(u64 _length, u64 _stride);
CAPI void _darray_destroy(void* _array);

CAPI u64 _darray_field_get(void* _array, u64 _field);
CAPI void _darray_field_set(void* _array, u64 _field, u64 _value);

CAPI void* _darray_resize(void* _array);

CAPI void* _darray_push(void* _array, const void* _valuePtr);
CAPI void _darray_pop(void* _array, void* _dest);

CAPI void* _darray_pop_at(void* _array, u64 _index, void* _dest);
CAPI void* _darray_insert_at(void* _array, u64 _index, void* _valuePtr);

#define DARRAY_DEFAULT_CAPACITY 1
#define DARRAY_RESIZE_FACTOR 2

//public interface
#define DArrayCreate(_type) \
    _darray_create(DARRAY_DEFAULT_CAPACITY, sizeof(_type))

#define DArrayReserve(_type, _capacity) \
    _darray_create(_capacity, sizeof(_type))

#define DArrayDestroy(_array) _darray_destroy(_array)

#define DArrayPush(_array, _value)              \
    {                                           \
        typeof(_value) temp = _value;           \
        _array = _darray_push(_array, &temp);   \
    }

#define DArrayPop(_array, _valuePtr) \
    _darray_pop(_array, _valuePtr)

#define DArrayInsertAt(_array, _index, _value)              \
    {                                                       \
        typeof(_value) temp = _value;                       \
        _array = _darray_insert_at(_array, _index, &temp);  \
    }

#define DArrayPopAt(_array, _index, _valuePtr) \
    _darray_pop_at(_array, _index, _valuePtr)

#define DArrayClear(_array) \
    _darray_field_set(_array, DARRAY_LENGTH, 0)

#define DArrayCapacity(_array) \
    _darray_field_get(_array, DARRAY_CAPACITY)

#define DArrayLength(_array) \
    _darray_field_get(_array, DARRAY_LENGTH)

#define DArrayStride(_array) \
    _darray_field_get(_array, DARRAY_STRIDE)

#define DArrayLengthSet(_array, _value) \
    _darray_field_set(_array, DARRAY_LENGTH, _value)