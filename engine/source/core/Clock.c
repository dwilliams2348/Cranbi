#include "Clock.h"

#include "platform/Platform.h"

void ClockUpdate(Clock* _clock)
{
    if(_clock->startTime != 0)
        _clock->elapsed = PlatformGetAbsoluteTime() - _clock->startTime;
}

void ClockStart(Clock* _clock)
{
    _clock->startTime = PlatformGetAbsoluteTime();
    _clock->elapsed = 0;
}

void ClockStop(Clock* _clock)
{
    _clock->startTime = 0;
}