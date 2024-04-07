#pragma once

#include "Defines.h"

typedef struct Clock
{
    f64 startTime;
    f64 elapsed;
} Clock;

//updates the provided clock, should be called just before checking elapsed time
// no effect on non-started clocks
CAPI void ClockUpdate(Clock* _clock);

//starts provided clock, resets elapsed time
CAPI void ClockStart(Clock* _clock);

//stops proved clock, does not reset elapsed time
CAPI void ClockStop(Clock* _clock);