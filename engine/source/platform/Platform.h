#pragma once

#include "Defines.h"

typedef struct PlatformState
{
    void* InternalState;
} PlatformState;

b8 PlatformStartup(PlatformState* _state, const char* _appName, i32 _x, i32 _y, i32 _width, i32 _height);

void PlatformShutdown(PlatformState* _state);

b8 PlatformPumpMessages(PlatformState* _state);

void* PlatformAllocate(u64 _size, b8 _aligned);
void PlatformFree(void* _block, b8 _aligned);
void* PlatformZeroMem(void* _block, u64 _size);
void* PlatformCopyMem(void* _dest, const void* _src, u64 _size);
void* PlatformSetMem(void* _dest, i32 _value, u64 _size);

void PlatformConsoleWrite(const char* _msg, u8 _color);
void PlatformConsoleWriteError(const char* _msg, u8 _color);

f64 PlatformGetAbsoluteTime();

//sleep on thread for provided ms, blocks main thread.
//Should only be used for giving time back to the OS for unused update power, therefore not being exported
void PlatformSleep(u64 _ms);