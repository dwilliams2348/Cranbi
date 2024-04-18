#include "Logger.h"
#include "Asserts.h"
#include "platform/Platform.h"
#include "platform/Filesystem.h"
#include "core/CString.h"
#include "core/CMemory.h"

//TODO: Temp, remove
#include <stdarg.h>

typedef struct LoggerSystemState
{
    FileHandle logFileHandle;
} LoggerSystemState;

static LoggerSystemState* pState;

void AppendToLogFile(const char* _msg)
{
    if(pState && pState->logFileHandle.isValid)
    {
        //since message already contains a '\n', juist write in bytes directly.
        u64 length = StringLength(_msg);
        u64 written = 0;
        if(!FilesystemWrite(&pState->logFileHandle, length, _msg, &written))
        {
            PlatformConsoleWriteError("ERROR writing to console.log.", ERROR);
        }
    }
}

b8 InitializeLogging(u64* _memoryRequirement, void* _state)
{
    *_memoryRequirement = sizeof(LoggerSystemState);
    if(_state == 0)
        return true;

    pState = _state;

    //create new/wipe existing file then open it
    if(!FilesystemOpen("console.log", FILE_MODE_WRITE, false, &pState->logFileHandle))
    {
        PlatformConsoleWriteError("ERROR: Unable to open console.log for writing.", ERROR);
        return false;
    }

    //TODO: create log file
    return true;
}

void ShutdownLogging(void* _state)
{
    //TODO: cleanup logging/write queued entries
    pState = 0;
}

void LogOutput(LogLevel _level, const char* _msg, ...)
{
    //TODO: these string operations are slow, this needs to be moved to a different thread at some point.
    const char* levelStrings[6] = { "[FATAL]: ", "[ERROR]: ", "[WARN]:  ", "[INFO]:  ", "[DEBUG]: ", "[TRACE]: "};
    b8 isError = _level < 2;

    //imposes a 32k character limit on a single log entry but never do that
    char outMsg[32000];
    cZeroMemory(outMsg, sizeof(outMsg));

    //Format original message.
    __builtin_va_list argPtr;
    va_start(argPtr, _msg);
    StringFormatV(outMsg, _msg, argPtr);
    va_end(argPtr);

    //prepend log level to message.
    StringFormat(outMsg, "%s%s\n", levelStrings[_level], outMsg);

    //print accordingly
    if(isError) { PlatformConsoleWriteError(outMsg, _level); }
    else { PlatformConsoleWrite(outMsg, _level); }

    //queue a copy to be written to the log file.
    AppendToLogFile(outMsg);
}

//all this does is log so it makes sense to be in Logger.c
void ReportAssertionFailure(const char* _expression, const char* _msg, const char* _file, i32 _line)
{
    LogOutput(FATAL, "Assertion Failure: %s, message: '%s', in file: %s, line: %d\n", _expression, _msg, _file, _line);
}