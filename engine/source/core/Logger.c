#include "Logger.h"
#include "Asserts.h"

//TODO: Temp, remove
#include <stdio.h>
#include <string.h>
#include <stdarg.h>

//all this does is log so it makes sense to be in Logger.c
void ReportAssertionFailure(const char* _expression, const char* _msg, const char* _file, i32 _line)
{
    LogOutput(FATAL, "Assertion Failure: %s, message: '%s', in file: %s, line: %d\n", _expression, _msg, _file, _line);
}

b8 InitializeLogging()
{
    //TODO: create log file
    return TRUE;
}

void ShutdownLogging()
{
    //TODO: cleanup logging/write queued entries
}

void LogOutput(LogLevel _level, const char* _msg, ...)
{
    const char* levelStrings[6] = {"[FATAL]: ", "[ERROR]: ", "[WARN]:  ", "[INFO]:  ", "[DEBUG]: ", "[TRACE]: "};
    //b8 isError = _level < 2;

    //Imposes a 32k character limit on a single entry but don't print that long
    char buffer[32000];
    memset(buffer, 0, sizeof(buffer));

    //Format original message
    //NOTE: MS's headers override the GCC/Clang va_list type with typedef char* va_list in some cases causing weird error
    //Workaround for now is to just use the __builtin_va_list which GCC/Clang expects
    __builtin_va_list argPtr;
    va_start(argPtr, _msg);
    vsnprintf(buffer, 32000, _msg, argPtr);
    va_end(argPtr);

    char outMsg[32000];
    sprintf(outMsg, "%s%s\n", levelStrings[_level], buffer);

    //TODO: platform specific output
    printf("%s", outMsg);
}