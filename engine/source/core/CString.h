#pragma once

#include "Defines.h"

//returns length of given string
CAPI u64 StringLength(const char* _str);

//duplicates given string and returns the copy
CAPI char* StringDuplicate(const char* _str);

//case sensitive string comparision, true if same, false otherwise
CAPI b8 StringsEqual(const char* _str0, const char* _str1);

// Performs string formatting to dest given format string and parameters.
CAPI i32 StringFormat(char* _dest, const char* _format, ...);

//
/**
 * Performs variadic string formatting to dest given format string and va_list.
 * @param dest The destination for the formatted string.
 * @param format The string to be formatted.
 * @param va_list The variadic argument list.
 * @returns The size of the data written.
 */
CAPI i32 StringFormatV(char* _dest, const char* _format, void* _vaList);