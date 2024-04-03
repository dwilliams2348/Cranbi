#pragma once

#include "Defines.h"

typedef struct Clock
{
    f64 startTime;
    f64 elapsed;
} Clock;

//updates the provided clock, should be called just before checking elapsed time
// no effect on non-started clocks
void ClockUpdate(Clock* _clock);

//starts provided clock, resets elapsed time
void ClockStart(Clock* _clock);

//stops proved clock, does not reset elapsed time
void ClockStop(Clock* _clock);