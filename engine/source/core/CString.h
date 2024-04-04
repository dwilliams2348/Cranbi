#pragma once

#include "Defines.h"

//returns length of given string
CAPI u64 StringLength(const char* _str);

//duplicates given string and returns the copy
CAPI char* StringDuplicate(const char* _str);

//case sensitive string comparision, true if same, false otherwise
CAPI b8 StringsEqual(const char* _str0, const char* _str1);