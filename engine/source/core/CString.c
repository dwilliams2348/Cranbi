#include "core/CString.h"
#include "core/CMemory.h"

#include <string.h>

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