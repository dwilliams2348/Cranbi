#pragma once

#include "Defines.h"

//vectors
//vec2 x, y
typedef union vec2U
{
    //an array of x, y
    f32 elements[2];
    struct {
        union {
            //first element
            f32 x, r, s, u;
        };
        union {
            //second element
            f32 y, g, t, v;
        };
    };
} vec2;

//vec3 x, y, z
typedef union vec3U
{
    //an array of x, y, z
    f32 elements[3];
    struct{
        union{
            //first element
            f32 x, r, s, u;
        };
        union{
            //second element
            f32 y, g, t, v;
        };
        union{
            //third element
            f32 z, b, p, w;
        };
    };
} vec3;

//vec4 x, y, z, w
typedef union vec4U
{
    //an array of x, y, z, w
    f32 elements[4];
    struct{
        union{
            //first element
            f32 x, r, s;
        };
        union{
            //second element
            f32 y, g, t;
        };
        union{
            //third element
            f32 z, b, p;
        };
        union{
            //fourth element
            f32 w, a, q;
        };
    };
} vec4;

//quaternion
typedef vec4 quat;

typedef union mat4U
{
    f32 data[16];
    vec4 rows[4];
    struct{
        union{
            vec4 row0;
        };
        union{
            vec4 row1;
        };
        union{
            vec4 row2;
        };
        union{
            vec4 row3;
        };
    };
#if defined(CUSE_SIMD)
    alignas(16) vec4 rows[4];
#endif
} mat4;

typedef struct Vertex3D
{
    vec3 position;
} Vertex3D;