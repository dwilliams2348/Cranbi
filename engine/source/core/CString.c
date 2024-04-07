#include "core/CString.h"
#include "core/CMemory.h"

#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include "CString.h"

u64 StringLength(const char* _str)
{
    return strlen(_str);
}

char* StringDuplicate(const char* _str)
{
    u64 length = StringLength(_str);
    char* copy = cAllocate(length + 1, MEMORY_TAG_STRING);
    cCopyMemory(copy, _str, length + 1);
    return copy;
}

b8 StringsEqual(const char* _str0, const char* _str1)
{
    return strcmp(_str0, _str1) == 0;
}

i32 StringFormat(char* _dest, const char* _format, ...) 
{
    if(_dest)
    {
        __builtin_va_list argPtr;
        va_start(argPtr, _format);
        i32 written = StringFormatV(_dest, _format, argPtr);
        va_end(argPtr);
        return written;
    }

    return -1;
}

i32 StringFormatV(char* _dest, const char* _format, void* _vaList) 
{
    if(_dest)
    {
        //big but fits on stack
        char buffer[32000];
        i32 written = vsnprintf(buffer, 32000, _format, _vaList);
        buffer[written] = 0;
        cCopyMemory(_dest, buffer, written + 1);

        return written;
    }

    return -1;
}