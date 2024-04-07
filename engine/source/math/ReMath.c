#include "ReMath.h"
#include "platform/Platform.h"

#include <math.h>
#include <stdlib.h>

static b8 rand_seeded = false;

/**
 * Note that these are here in order to prevent having to import the
 * entire <math.h> everywhere.
 */
f32 re_sin(f32 x) {
    return sinf(x);
}

f32 re_cos(f32 x) {
    return cosf(x);
}

f32 re_tan(f32 x) {
    return tanf(x);
}

f32 re_acos(f32 x) {
    return acosf(x);
}

f32 re_sqrt(f32 x) {
    return sqrtf(x);
}

f32 re_abs(f32 x) {
    return fabsf(x);
}

i32 re_random() {
    if (!rand_seeded) {
        srand((u32)PlatformGetAbsoluteTime());
        rand_seeded = true;
    }
    return rand();
}

i32 re_random_in_range(i32 min, i32 max) {
    if (!rand_seeded) {
        srand((u32)PlatformGetAbsoluteTime());
        rand_seeded = true;
    }
    return (rand() % (max - min + 1)) + min;
}

f32 re_krandom() {
    return (float)re_random() / (f32)RAND_MAX;
}

f32 re_krandom_in_range(f32 min, f32 max) {
    return min + ((float)re_random() / ((f32)RAND_MAX / (max - min)));
}