#pragma once

#include "Defines.h"
#include "MathTypes.h"

//TODO: remove
#include "core/CMemory.h"

#define RE_PI 3.14159265358979323846f
#define RE_PI_2 2.0f * RE_PI
#define RE_HALF_PI 0.5f * RE_PI
#define RE_QUARTER_PI 0.25f * RE_PI
#define RE_ONE_OVER_PI 1.0f / RE_PI
#define RE_ONE_OVER_TWO_PI 1.0f / RE_PI_2
#define RE_SQRT_TWO 1.41421356237309504880f
#define RE_SQRT_THREE 1.73205080756887729352f
#define RE_SQRT_ONE_OVER_TWO 0.70710678118654752440f
#define RE_SQRT_ONE_OVER_THREE 0.57735026918962576450f
#define RE_DEG2RAD_MULTIPLIER RE_PI / 180.0f
#define RE_RAD2DEG_MULTIPLIER 180.0f / RE_PI

//Multiplier to convert seconds to milliseconds
#define RE_SEC_TO_MS_MULTIPLIER 1000.f

//Multiplier to convert milliseconds to seconds
#define RE_MS_TO_SEC_MULTIPLIER 0.001f

//Large number that should be larger than any valid number being used
#define RE_INFINITY 1e30f

//smallest positive number where 1.0 + FLOAT_EPSILON != 0
#define RE_FLOAT_EPSILON 1.192092896e-07f


//-----------------------------------
//General math functions
//-----------------------------------
CAPI f32 re_sin(f32 x);
CAPI f32 re_cos(f32 x);
CAPI f32 re_tan(f32 x);
CAPI f32 re_acos(f32 x);
CAPI f32 re_sqrt(f32 x);
CAPI f32 re_abs(f32 x);

/**
 * Indicates if the value is a power of 2. 0 is considered _not_ a power of 2.
 * @param value The value to be interpreted.
 * @returns True if a power of 2, otherwise false.
 */
CINLINE b8 is_power_of_2(u64 _value) {
    return (_value != 0) && ((_value & (_value - 1)) == 0);
}

CAPI i32 re_rand();
CAPI i32 re_rand_range(i32 _min, i32 _max);

CAPI f32 re_frand();
CAPI f32 re_frand_range(f32 _min, f32 _max);


//-----------------------------------
//Vector 2 functions
//-----------------------------------

/**
 * @brief Creates and returns a new 2-component vector using the supplied values.
 * 
 * @param x The x value.
 * @param y The y value.
 * @return A new 2-component vector.
*/
inline vec2 vec2_create(f32 x, f32 y){
    vec2 out;
    out.x = x;
    out.y = y;
    return out;
}

/**
 * @brief Creates and returns a 2-component vector that has been zero'd.
*/
inline vec2 vec2_zero() { return (vec2){ 0.f, 0.f }; }

/**
 * @brief Creates and returns a 2-component vector with all 1s.
*/
inline vec2 vec2_one() { return (vec2){ 1.f, 1.f }; }

/**
 * @brief Creates and returns a 2-component up vector.
*/
inline vec2 vec2_up() { return (vec2){ 0.f, 1.f }; }

/**
 * @brief Creates and returns a 2-component down vector.
*/
inline vec2 vec2_down() { return (vec2){ 0.f, -1.f }; }

/**
 * @brief Creates and returns a 2-component right vector.
*/
inline vec2 vec2_right() { return (vec2){ 1.f, 0.f }; }

/**
 * @brief Creates and returns a 2-component left vector.
*/
inline vec2 vec2_left() { return (vec2){ -1.f, 0.f }; }

/**
 * @brief Adds vector1 to vector0 and returns a copy of the result.
 * 
 * @param vector0 The first vector.
 * @param vector1 The second vector.
 * @return The resulting vector.
*/
inline vec2 vec2_add(vec2 vector0, vec2 vector1){
    return (vec2){
        vector0.x + vector1.x,
        vector0.y + vector1.y };
}

/**
 * @brief Subtracts vector1 from vector0 and returns a copy of the result.
 * 
 * @param vector0 The first vector.
 * @param vector1 The second vector.
 * @return The resulting vector.
*/
inline vec2 vec2_sub(vec2 vector0, vec2 vector1){
    return (vec2){
        vector0.x - vector1.x,
        vector0.y - vector1.y};
}

/**
 * @brief Multiplies vector0 by vector1 and returns a copy of the result.
 * 
 * @param vector0 The first vector.
 * @param vector1 The second vector.
 * @return The resulting vector.
*/
inline vec2 vec2_mul(vec2 vector0, vec2 vector1){
    return (vec2){
        vector0.x * vector1.x,
        vector0.y * vector1.y};
}

/**
 * @brief Divides vector0 by vector1 and returns a copy of the result.
 * 
 * @param vector0 The first vector.
 * @param vector1 The second vector.
 * @return The resulting vector.
*/
inline vec2 vec2_div(vec2 vector0, vec2 vector1){
    return (vec2){
        vector0.x / vector1.x,
        vector0.y / vector1.y};
}

/**
 * @brief Returns the squared length of the supplied vector.
 * 
 * @param vector The vector to retrieve the squared length of.
 * @return The squared length.
*/
inline f32 vec2_length_squared(vec2 vector){ return vector.x * vector.x + vector.y * vector.y; }

/**
 * @brief Returns the length of the supplied vector.
 * 
 * @param vector The vector to retrieve the length of.
 * @return The length.
*/
inline f32 vec2_length(vec2 vector) { return re_sqrt(vec2_length_squared(vector)); }

/**
 * @brief Normalizes the supplied vector in place to a unit vector.
 * 
 * @param vector A pointer to the vector to be normalized.
*/
inline void vec2_normalize(vec2* vector){
    const f32 length = vec2_length(*vector);
    vector->x /= length;
    vector->y /= length;
}

/**
 * @brief Returns a normalized copy of the supplied vector.
 * 
 * @param vector The vector to be normalized.
 * @return A normalized copy of the supplied vector.
*/
inline vec2 vec2_normalized(vec2 vector){
    vec2_normalize(&vector);
    return vector;
}

/**
 * @brief Compares all elements of vector0 and vector1 and ensures the difference
 * is than the tolerance.
 * 
 * @param vector0 The first vector.
 * @param vector1 The second vector.
 * @param tolerance The difference tolerance. Typically RE_FLOAT_EPSILON or similar.
 * @return True if within tolerance, otherwise false.
*/
inline b8 vec2_compare(vec2 vector0, vec2 vector1, f32 tolerance){
    if(re_abs(vector0.x - vector1.x) > tolerance)
        return false;
    if(re_abs(vector0.y - vector1.y) > tolerance)
        return false;

    return true;
}

/**
 * @brief Returns distance between vector0 and vector1.
 * 
 * @param vector0 The first vector.
 * @param vector1 The second vector.
 * @return The distance between vector 0 and vector1.
*/
inline f32 vec2_distance(vec2 vector0, vec2 vector1){
    vec2 d = (vec2){
        vector0.x - vector1.x,
        vector0.y - vector1.y};
    return vec2_length(d);
}


//-----------------------------------
//Vector 3
//-----------------------------------

/**
 * @brief Creates and returns a new 3-component vector using the supplied values.
 * 
 * @param x The x value.
 * @param y The y value.
 * @param z the z value.
 * @return A new 3-component vector.
*/
inline vec3 vec3_create(f32 x, f32 y, f32 z) { return (vec3){ x, y, z }; }

/**
 * @brief Returns a new vec3 containing the x, y, and z components of the supplied
 * vec4, just drops the w component.
 * 
 * @param vector The 4-component vector to extract from.
 * @return A new vec3
*/
inline vec3 vec3_from_vec4(vec4 vector) { return (vec3){ vector.x, vector.y, vector.z }; }

/**
 * @brief Returns a new vec4 using vector as the x, y, and z components and w for w.
 * 
 * @param vector The 3-component vector.
 * @param w The w component.
 * @return A new vec4.
*/
//inline vec4 vec4_from_vec3(vec3 vector, f32 w) { return (vec4){ vector.x, vector.y, vector.z, w}; }

/**
 * @brief Creates and returns a 3-component vector that has been zero'd.
*/
inline vec3 vec3_zero() { return (vec3){ 0.f, 0.f, 0.f }; }

/**
 * @brief Creates and returns a 3-component vector that has all 1s.
*/
inline vec3 vec3_one() { return (vec3){ 1.f, 1.f, 1.f }; }

/**
 * @brief Creates and returns a 3-component up vector
*/
inline vec3 vec3_up() { return (vec3){ 0.f, 1.f, 0.f }; }

/**
 * @brief Creates and returns a 3-component down vector
*/
inline vec3 vec3_down() { return (vec3){ 0.f, -1.f, 0.f }; }

/**
 * @brief Creates and returns a 3-component right vector
*/
inline vec3 vec3_right() { return (vec3){ 1.f, 0.f, 0.f }; }

/**
 * @brief Creates and returns a 3-component left vector
*/
inline vec3 vec3_left() { return (vec3){ -1.f, 0.f, 0.f }; }

/**
 * @brief Creates and returns a 3-component forward vector
*/
inline vec3 vec3_forward() { return (vec3){ 0.f, 0.f, -1.f }; }

/**
 * @brief Creates and returns a 3-component backward vector
*/
inline vec3 vec3_back() { return (vec3){ 0.f, 0.f, 1.f }; }

/**
 * @brief Adds vector1 to vector0 and returns a copy of the result.
 * 
 * @param vector0 The first vector.
 * @param vector1 The second vector.
 * @return The resulting vector.
*/
inline vec3 vec3_add(vec3 vector0, vec3 vector1){
    return (vec3){
        vector0.x + vector1.x,
        vector0.y + vector1.y,
        vector0.z + vector1.z };
}

/**
 * @brief Subtracts vector1 from vector0 and returns a copy of the result.
 * 
 * @param vector0 The first vector.
 * @param vector1 The second vector.
 * @return The resulting vector.
*/
inline vec3 vec3_sub(vec3 vector0, vec3 vector1){
    return (vec3){
        vector0.x - vector1.x,
        vector0.y - vector1.y,
        vector0.z - vector1.z };
}

/**
 * @brief Multiplies vector0 by vector1 and returns a copy of the result.
 * 
 * @param vector0 The first vector.
 * @param vector1 The second vector.
 * @return The resulting vector.
*/
inline vec3 vec3_mul(vec3 vector0, vec3 vector1){
    return (vec3){
        vector0.x * vector1.x,
        vector0.y * vector1.y,
        vector0.z * vector1.z };
}

/**
 * @brief Multiplies all elements of vector_0 by scalar and returns a copy of the result.
 * 
 * @param vector The vector to be multiplied.
 * @param scalar The scalar value.
 * @return A copy of the resulting vector.
 */
inline vec3 vec3_mul_scalar(vec3 vector, f32 scalar) {
    return (vec3){
        vector.x * scalar,
        vector.y * scalar,
        vector.z * scalar};
}

/**
 * @brief Divides vector0 by vector1 and returns a copy of the result.
 * 
 * @param vector0 The first vector.
 * @param vector1 The second vector.
 * @return The resulting vector.
*/
inline vec3 vec3_div(vec3 vector0, vec3 vector1){
    return (vec3){
        vector0.x / vector1.x,
        vector0.y / vector1.y,
        vector0.z / vector1.z };
}

/**
 * @brief Returns the squared length of the supplied vector.
 * 
 * @param vector The vector to retrieve the squared length of.
 * @return The squared length.
*/
inline f32 vec3_length_squared(vec3 vector) { return vector.x * vector.x + vector.y * vector.y + vector.z * vector.z; }

/**
 * @brief Returns the length of the supplied vector.
 * 
 * @param vector The vector to retrieve the length of.
 * @return The length.
*/
inline f32 vec3_length(vec3 vector) { return re_sqrt(vec3_length_squared(vector)); }

/**
 * @brief Normalizes the supplied vector in place to a unit vector.
 * 
 * @param vector A pointer to the vector to be normalized.
*/
inline void vec3_normalize(vec3* vector){
    const f32 length = vec3_length(*vector);
    vector->x /= length;
    vector->y /= length;
    vector->z /= length;
}

/**
 * @brief Returns a normalized copy of the supplied vector.
 * 
 * @param vector The vector to be normalized.
 * @return A normalized copy of the supplied vector.
*/
inline vec3 vec3_normalized(vec3 vector){
    vec3_normalize(&vector);
    return vector;
}

/**
 * @brief Returns the dot product between the supplied vectors. Typically used to
 * calculate the difference in direction.
 * 
 * @param vector0 The first vector.
 * @param vector1 The second vector.
 * @return The dot product.
*/
inline f32 vec3_dot(vec3 vector0, vec3 vector1){
    f32 p = 0;
    p += vector0.x * vector1.x;
    p += vector0.y * vector1.y;
    p += vector0.z * vector1.z;
    return p;
}

/**
 * @brief Calculates and returns the cross product of the supplied vectors.
 * The cross product is a new vector which is orthoganal to both supplied vectors.
 * 
 * @param vector0 The first vector.
 * @param vector1 The second vector.
 * @return The cross product
*/
inline vec3 vec3_cross(vec3 vector0, vec3 vector1){
    return (vec3){
        vector0.y * vector1.z - vector0.z * vector1.y,
        vector0.z * vector1.x - vector0.x * vector1.z,
        vector0.x * vector1.y - vector0.y * vector1.x};
}

/**
 * @brief Compares all elements of vector0 and vector1 and ensures the difference
 * is than the tolerance.
 * 
 * @param vector0 The first vector.
 * @param vector1 The second vector.
 * @param tolerance The difference tolerance. Typically RE_FLOAT_EPSILON or similar.
 * @return True if within tolerance, otherwise false.
*/
inline b8 vec3_compare(vec3 vector0, vec3 vector1, f32 tolerance){
    if(re_abs(vector0.x - vector1.x) > tolerance)
        return false;
    if(re_abs(vector0.y - vector1.y) > tolerance)
        return false;
    if(re_abs(vector0.z - vector1.z) > tolerance)
        return false;

    return true;
}

/**
 * @brief Returns distance between vector0 and vector1.
 * 
 * @param vector0 The first vector.
 * @param vector1 The second vector.
 * @return The distance between vector 0 and vector1.
*/
inline f32 vec3_distance(vec3 vector0, vec3 vector1){
    vec3 d = (vec3){
        vector0.x - vector1.x,
        vector0.y - vector1.y,
        vector0.z - vector1.z };
    return vec3_length(d);
}


//-----------------------------------
//Vector 4
//-----------------------------------


/**
 * @brief Creates and returns a new 4-component vector using the supplied values.
 * 
 * @param x The x value.
 * @param y The y value.
 * @param z The z value.
 * @param w The w value.
 * @return A new 4-component vector.
*/
inline vec4 vec4_create(f32 x, f32 y, f32 z, f32 w){
    vec4 out;
#if defined(CUSE_SIMD)
    out.data = _mm_setr_ps(x, y, z, w);
#else
    out.x = x;
    out.y = y;
    out.z = z;
    out.w = w;
#endif
    return out;
}

/**
 * @brief Returns a new vec3 containing the x, y, and z components of the supplied
 * vec4, essentially dropping the w component.
 * 
 * @param vector The 4-component vector to extract from.
 * @return A new vec3
*/
inline vec3 vec4_to_vec3(vec4 vector) { return (vec3){ vector.x, vector.y, vector.z }; }

/**
 * @brief Returns a new vec4 using vector as the x, y and z components and w for w.
 * 
 * @param vector The 3-component vector.
 * @param w The w component.
 * @return A new vec4 
 */
inline vec4 vec4_from_vec3(vec3 vector, f32 w) {
#if defined(CUSE_SIMD)
    vec4 out;
    out_vector.data = _mm_setr_ps(x, y, z, w);
    return out;
#else
    return (vec4){ vector.x, vector.y, vector.z, w };
#endif
}

/**
 * @brief Creates and returns a 4-component vector that has been zero'd.
 */
inline vec4 vec4_zero() {
    return (vec4){0.0f, 0.0f, 0.0f, 0.0f};
}

/**
 * @brief Creates and returns a 4-component vector with all 1s.
 */
inline vec4 vec4_one() {
    return (vec4){1.0f, 1.0f, 1.0f, 1.0f};
}

/**
 * @brief Adds vector1 to vector0 and returns a copy of the result.
 * 
 * @param vector0 The first vector.
 * @param vector1 The second vector.
 * @return The resulting vector.
*/
inline vec4 vec4_add(vec4 vector0, vec4 vector1){
    vec4 result;
    for(u8 i = 0; i < 4; ++i)
        result.elements[i] = vector0.elements[i] + vector1.elements[i];
    
    return result;
}

/**
 * @brief Subtracts vector1 from vector0 and returns a copy of the result.
 * 
 * @param vector0 The first vector.
 * @param vector1 The second vector.
 * @return The resulting vector.
*/
inline vec4 vec4_sub(vec4 vector0, vec4 vector1){
    vec4 result;
    for(u8 i = 0; i < 4; ++i)
        result.elements[i] = vector0.elements[i] - vector1.elements[i];
    
    return result;
}

/**
 * @brief Multiplies vector0 by vector1 and returns a copy of the result.
 * 
 * @param vector0 The first vector.
 * @param vector1 The second vector.
 * @return The resulting vector.
*/
inline vec4 vec4_mul(vec4 vector0, vec4 vector1){
    vec4 result;
    for(u8 i = 0; i < 4; ++i)
        result.elements[i] = vector0.elements[i] * vector1.elements[i];
    
    return result;
}

/**
 * @brief Divides vector0 by vector1 and returns a copy of the result.
 * 
 * @param vector0 The first vector.
 * @param vector1 The second vector.
 * @return The resulting vector.
*/
inline vec4 vec4_div(vec4 vector0, vec4 vector1){
    vec4 result;
    for(u8 i = 0; i < 4; ++i)
        result.elements[i] = vector0.elements[i] / vector1.elements[i];
    
    return result;
}

/**
 * @brief Returns the squared length of the provided vector.
 * 
 * @param vector The vector to retrieve the squared length of.
 * @return The squared length.
 */
inline f32 vec4_length_squared(vec4 vector) { return vector.x * vector.x + vector.y * vector.y + vector.z * vector.z + vector.w * vector.w; }

/**
 * @brief Returns the length of the provided vector.
 * 
 * @param vector The vector to retrieve the length of.
 * @return The length.
 */
inline f32 vec4_length(vec4 vector) { return re_sqrt(vec4_length_squared(vector)); }

/**
 * @brief Normalizes the provided vector in place to a unit vector.
 * 
 * @param vector A pointer to the vector to be normalized.
 */
inline void vec4_normalize(vec4* vector) {
    const f32 length = vec4_length(*vector);
    vector->x /= length;
    vector->y /= length;
    vector->z /= length;
    vector->w /= length;
}

/**
 * @brief Returns a normalized copy of the supplied vector.
 * 
 * @param vector The vector to be normalized.
 * @return A normalized copy of the supplied vector 
 */
inline vec4 vec4_normalized(vec4 vector) {
    vec4_normalize(&vector);
    return vector;
}

inline f32 vec4_dot_f32(
    f32 a0, f32 a1, f32 a2, f32 a3,
    f32 b0, f32 b1, f32 b2, f32 b3) {
    f32 p;
    p =
        a0 * b0 +
        a1 * b1 +
        a2 * b2 +
        a3 * b3;
    return p;
}


//-----------------------------------
//Matrices
//-----------------------------------

/**
 * @brief Creates and returns an identity matrix:
 * 
 * {
 *   {1, 0, 0, 0},
 *   {0, 1, 0, 0},
 *   {0, 0, 1, 0},
 *   {0, 0, 0, 1}
 * }
 * 
 * @return A new identity matrix.
*/
inline mat4 mat4_identity(){
    mat4 out;
    cZeroMemory(out.data, sizeof(f32) * 16);
    out.data[0] = 1.f;
    out.data[5] = 1.f;
    out.data[10] = 1.f;
    out.data[15] = 1.f;
    return out;
}

/**
 * @brief Returns the result of multiplying matrix0 and matrix1.
 * 
 * @param matrix0 The first matrix.
 * @param matrix1 The second matrix.
 * @return The resulted matrix.
*/
inline mat4 mat4_mul(mat4 matrix0, mat4 matrix1){
    mat4 out = mat4_identity();

    const f32* m1 = matrix0.data;
    const f32* m2 = matrix1.data;
    f32* dst = out.data;

    for(i32 i = 0; i < 4; ++i)
    {
        for(i32 j = 0; j < 4; ++j)
        {
            *dst = 
            m1[0] * m2[0 + j] +
            m1[1] * m2[4 + j] +
            m1[2] * m2[8 + j] +
            m1[3] * m2[12 + j];
            dst++;
        }
        m1 += 4;
    }
    return out;
}

/**
 * @brief Creates and returns an orthographic projection matrix. Typically used to
 * render flat or 2D scenes.
 * 
 * @param left The left side of the view frustum.
 * @param right The right side of the view frustum.
 * @param bottom The bottom side of the view frustum.
 * @param top The top side of the view frustum.
 * @param nearClip The near clipping plane distance.
 * @param farClip The far clipping plane distance.
 * @return A new orthographic projection matrix. 
 */
inline mat4 mat4_orthographic(f32 left, f32 right, f32 bottom, f32 top, f32 nearClip, f32 farClip){
    mat4 out = mat4_identity();

    f32 lr = 1.f / (left - right);
    f32 bt = 1.f / (bottom - top);
    f32 nf = 1.f / (nearClip - farClip);

    out.data[0] = -2.f * lr;
    out.data[5] = -2.f * bt;
    out.data[10] = 2.f * nf;

    out.data[12] = (left + right) * lr;
    out.data[13] = (top + bottom) * bt;
    out.data[14] = (farClip + nearClip) * nf;
    return out;
}

/**
 * @brief Creates and returns a perspective matrix. Typically used to render 3d scenes.
 * 
 * @param fovRadians The field of view in radians.
 * @param aspectRatio The aspect ratio.
 * @param nearClip The near clipping plane distance.
 * @param farClip The far clipping plane distance.
 * @return A new perspective matrix. 
 */
inline mat4 mat4_perspective(f32 fovRadians, f32 aspectRatio, f32 nearClip, f32 farClip){
    f32 halfTanFOV = re_tan(fovRadians * 0.5f);
    mat4 out = mat4_identity();

    out.data[0] = 1.f / (aspectRatio * halfTanFOV);
    out.data[5] = 1.f / halfTanFOV;
    out.data[10] = -((farClip + nearClip) / (farClip / nearClip));
    out.data[11] = -1.f;
    out.data[14] = -((2.f * farClip * nearClip) / (farClip - nearClip));
    return out;
}

/**
 * @brief Creates and returns a look-at matrix, or a matrix looking 
 * at target from the perspective of position.
 * 
 * @param position The position of the matrix.
 * @param target The position to "look at".
 * @param up The up vector.
 * @return A matrix looking at target from the perspective of position. 
 */
inline mat4 mat4_look_at(vec3 position, vec3 target, vec3 up){
    mat4 out;
    vec3 zAxis;

    zAxis.x = target.x - position.x;
    zAxis.y = target.y - position.y;
    zAxis.z = target.z - position.z;

    zAxis = vec3_normalized(zAxis);
    vec3 xAxis = vec3_normalized(vec3_cross(zAxis, up));
    vec3 yAxis = vec3_cross(xAxis, zAxis);

    out.data[0] = xAxis.x;
    out.data[1] = yAxis.x;
    out.data[2] = -zAxis.x;
    out.data[3] = 0;
    out.data[4] = xAxis.y;
    out.data[5] = yAxis.y;
    out.data[6] = -zAxis.y;
    out.data[7] = 0;
    out.data[8] = xAxis.z;
    out.data[9] = yAxis.z;
    out.data[10] = -zAxis.z;
    out.data[11] = 0;
    out.data[12] = -vec3_dot(xAxis, position);
    out.data[13] = -vec3_dot(yAxis, position);
    out.data[14] = -vec3_dot(zAxis, position);
    out.data[15] = 1.f;

    return out;
}

/**
 * @brief Returns a transposed copy of the provided matrix (rows->colums)
 * 
 * @param matrix The matrix to be transposed.
 * @return A transposed copy of of the provided matrix.
 */
inline mat4 mat4_transposed(mat4 matrix) {
    mat4 out_matrix = mat4_identity();
    out_matrix.data[0] = matrix.data[0];
    out_matrix.data[1] = matrix.data[4];
    out_matrix.data[2] = matrix.data[8];
    out_matrix.data[3] = matrix.data[12];
    out_matrix.data[4] = matrix.data[1];
    out_matrix.data[5] = matrix.data[5];
    out_matrix.data[6] = matrix.data[9];
    out_matrix.data[7] = matrix.data[13];
    out_matrix.data[8] = matrix.data[2];
    out_matrix.data[9] = matrix.data[6];
    out_matrix.data[10] = matrix.data[10];
    out_matrix.data[11] = matrix.data[14];
    out_matrix.data[12] = matrix.data[3];
    out_matrix.data[13] = matrix.data[7];
    out_matrix.data[14] = matrix.data[11];
    out_matrix.data[15] = matrix.data[15];
    return out_matrix;
}

/**
 * @brief Creates and returns an inverse of the provided matrix.
 * 
 * @param matrix The matrix to be inverted.
 * @return A inverted copy of the provided matrix. 
 */
inline mat4 mat4_inverse(mat4 matrix)
{
    const f32* m = matrix.data;

    f32 t0 = m[10] * m[15];
    f32 t1 = m[14] * m[11];
    f32 t2 = m[6] * m[15];
    f32 t3 = m[14] * m[7];
    f32 t4 = m[6] * m[11];
    f32 t5 = m[10] * m[7];
    f32 t6 = m[2] * m[15];
    f32 t7 = m[14] * m[3];
    f32 t8 = m[2] * m[11];
    f32 t9 = m[10] * m[3];
    f32 t10 = m[2] * m[7];
    f32 t11 = m[6] * m[3];
    f32 t12 = m[8] * m[13];
    f32 t13 = m[12] * m[9];
    f32 t14 = m[4] * m[13];
    f32 t15 = m[12] * m[5];
    f32 t16 = m[4] * m[9];
    f32 t17 = m[8] * m[5];
    f32 t18 = m[0] * m[13];
    f32 t19 = m[12] * m[1];
    f32 t20 = m[0] * m[9];
    f32 t21 = m[8] * m[1];
    f32 t22 = m[0] * m[5];
    f32 t23 = m[4] * m[1];

    mat4 out;
    f32* o = out.data;

    o[0] = (t0 * m[5] + t3 * m[9] + t4 * m[13]) - (t1 * m[5] + t2 * m[9] + t5 * m[13]);
    o[1] = (t1 * m[1] + t6 * m[9] + t9 * m[13]) - (t0 * m[1] + t7 * m[9] + t8 * m[13]);
    o[2] = (t2 * m[1] + t7 * m[5] + t10 * m[13]) - (t3 * m[1] + t6 * m[5] + t11 * m[13]);
    o[3] = (t5 * m[1] + t8 * m[5] + t11 * m[9]) - (t4 * m[1] + t9 * m[5] + t10 * m[9]);

    f32 d = 1.0f / (m[0] * o[0] + m[4] * o[1] + m[8] * o[2] + m[12] * o[3]);

    o[0] = d * o[0];
    o[1] = d * o[1];
    o[2] = d * o[2];
    o[3] = d * o[3];
    o[4] = d * ((t1 * m[4] + t2 * m[8] + t5 * m[12]) - (t0 * m[4] + t3 * m[8] + t4 * m[12]));
    o[5] = d * ((t0 * m[0] + t7 * m[8] + t8 * m[12]) - (t1 * m[0] + t6 * m[8] + t9 * m[12]));
    o[6] = d * ((t3 * m[0] + t6 * m[4] + t11 * m[12]) - (t2 * m[0] + t7 * m[4] + t10 * m[12]));
    o[7] = d * ((t4 * m[0] + t9 * m[4] + t10 * m[8]) - (t5 * m[0] + t8 * m[4] + t11 * m[8]));
    o[8] = d * ((t12 * m[7] + t15 * m[11] + t16 * m[15]) - (t13 * m[7] + t14 * m[11] + t17 * m[15]));
    o[9] = d * ((t13 * m[3] + t18 * m[11] + t21 * m[15]) - (t12 * m[3] + t19 * m[11] + t20 * m[15]));
    o[10] = d * ((t14 * m[3] + t19 * m[7] + t22 * m[15]) - (t15 * m[3] + t18 * m[7] + t23 * m[15]));
    o[11] = d * ((t17 * m[3] + t20 * m[7] + t23 * m[11]) - (t16 * m[3] + t21 * m[7] + t22 * m[11]));
    o[12] = d * ((t14 * m[10] + t17 * m[14] + t13 * m[6]) - (t16 * m[14] + t12 * m[6] + t15 * m[10]));
    o[13] = d * ((t20 * m[14] + t12 * m[2] + t19 * m[10]) - (t18 * m[10] + t21 * m[14] + t13 * m[2]));
    o[14] = d * ((t18 * m[6] + t23 * m[14] + t15 * m[2]) - (t22 * m[14] + t14 * m[2] + t19 * m[6]));
    o[15] = d * ((t22 * m[10] + t16 * m[2] + t21 * m[6]) - (t20 * m[6] + t23 * m[10] + t17 * m[2]));

    return out;
}

/**
 * @brief Returns the translated matrix from provided vec3 position.
 * 
 * @param position The 3-component position.
 * @return Translated matrix.
*/
inline mat4 mat4_translate(vec3 position){
    mat4 out = mat4_identity();
    out.data[12] = position.x;
    out.data[13] = position.y;
    out.data[14] = position.z;
    return out;
}

/**
 * @brief Returns a scale matrix using the provided scale.
 * 
 * @param scale The 3-component scale.
 * @return A scale matrix.
 */
inline mat4 mat4_scale(vec3 scale) {
    mat4 out = mat4_identity();
    out.data[0] = scale.x;
    out.data[5] = scale.y;
    out.data[10] = scale.z;
    return out;
}

inline mat4 mat4_euler_x(f32 angleRadians) {
    mat4 out = mat4_identity();
    f32 c = re_cos(angleRadians);
    f32 s = re_sin(angleRadians);

    out.data[5] = c;
    out.data[6] = s;
    out.data[9] = -s;
    out.data[10] = c;
    return out;
}

inline mat4 mat4_euler_y(f32 angleRadians) {
    mat4 out = mat4_identity();
    f32 c = re_cos(angleRadians);
    f32 s = re_sin(angleRadians);

    out.data[0] = c;
    out.data[2] = -s;
    out.data[8] = s;
    out.data[10] = c;
    return out;
}

inline mat4 mat4_euler_z(f32 angleRadians) {
    mat4 out = mat4_identity();

    f32 c = re_cos(angleRadians);
    f32 s = re_sin(angleRadians);

    out.data[0] = c;
    out.data[1] = s;
    out.data[4] = -s;
    out.data[5] = c;
    return out;
}

inline mat4 mat4_euler_xyz(f32 xRadians, f32 yRadians, f32 zRadians) {
    mat4 rx = mat4_euler_x(xRadians);
    mat4 ry = mat4_euler_y(yRadians);
    mat4 rz = mat4_euler_z(zRadians);
    mat4 out = mat4_mul(rx, ry);
    out = mat4_mul(out, rz);
    return out;
}

/**
 * @brief Returns a forward vector relative to the provided matrix.
 * 
 * @param matrix The matrix from which to base the vector.
 * @return A 3-component directional vector.
 */
inline vec3 mat4_forward(mat4 matrix) {
    vec3 forward;
    forward.x = -matrix.data[2];
    forward.y = -matrix.data[6];
    forward.z = -matrix.data[10];
    vec3_normalize(&forward);
    return forward;
}

/**
 * @brief Returns a backward vector relative to the provided matrix.
 * 
 * @param matrix The matrix from which to base the vector.
 * @return A 3-component directional vector.
 */
inline vec3 mat4_backward(mat4 matrix) {
    vec3 backward;
    backward.x = matrix.data[2];
    backward.y = matrix.data[6];
    backward.z = matrix.data[10];
    vec3_normalize(&backward);
    return backward;
}

/**
 * @brief Returns a upward vector relative to the provided matrix.
 * 
 * @param matrix The matrix from which to base the vector.
 * @return A 3-component directional vector.
 */
inline vec3 mat4_up(mat4 matrix) {
    vec3 up;
    up.x = matrix.data[1];
    up.y = matrix.data[5];
    up.z = matrix.data[9];
    vec3_normalize(&up);
    return up;
}

/**
 * @brief Returns a downward vector relative to the provided matrix.
 * 
 * @param matrix The matrix from which to base the vector.
 * @return A 3-component directional vector.
 */
inline vec3 mat4_down(mat4 matrix) {
    vec3 down;
    down.x = -matrix.data[1];
    down.y = -matrix.data[5];
    down.z = -matrix.data[9];
    vec3_normalize(&down);
    return down;
}

/**
 * @brief Returns a left vector relative to the provided matrix.
 * 
 * @param matrix The matrix from which to base the vector.
 * @return A 3-component directional vector.
 */
inline vec3 mat4_left(mat4 matrix) {
    vec3 right;
    right.x = -matrix.data[0];
    right.y = -matrix.data[4];
    right.z = -matrix.data[8];
    vec3_normalize(&right);
    return right;
}

/**
 * @brief Returns a right vector relative to the provided matrix.
 * 
 * @param matrix The matrix from which to base the vector.
 * @return A 3-component directional vector.
 */
inline vec3 mat4_right(mat4 matrix) {
    vec3 left;
    left.x = matrix.data[0];
    left.y = matrix.data[4];
    left.z = matrix.data[8];
    vec3_normalize(&left);
    return left;
}


//-----------------------------------
//Quaternion
//-----------------------------------

inline quat quat_identity() { return (quat){ 0.f, 0.f, 0.f, 1.f }; }

inline f32 quat_normal(quat q) { return re_sqrt(q.x * q.x + q.y * q.y + q.z * q.z + q.w * q.w); }

inline quat quat_normalize(quat q){
    f32 normal = quat_normal(q);
    return (quat){ q.x / normal, q.y / normal, q.z / normal, q.w / normal };
}

inline quat quat_conjugate(quat q) { return (quat){ -q.x, -q.y, -q.z, q.w }; }

inline quat quat_inverse(quat q) { return quat_normalize(quat_conjugate(q)); }

inline quat quat_mul(quat q0, quat q1)
{
    quat out;

    out.x = q0.x * q1.w +
                q0.y * q1.z -
                q0.z * q1.y +
                q0.w * q1.x;

    out.y = -q0.x * q1.z +
                q0.y * q1.w +
                q0.z * q1.x +
                q0.w * q1.y;

    out.z = q0.x * q1.y -
                q0.y * q1.x +
                q0.z * q1.w +
                q0.w * q1.z;

    out.w = -q0.x * q1.x -
                q0.y * q1.y -
                q0.z * q1.z +
                q0.w * q1.w;

    return out;
}

inline f32 quat_dot(quat q0, quat q1) {
    return q0.x * q1.x +
           q0.y * q1.y +
           q0.z * q1.z +
           q0.w * q1.w;
}

inline mat4 quat_to_mat4(quat q) {
    mat4 out = mat4_identity();

    // https://stackoverflow.com/questions/1556260/convert-quaternion-rotation-to-rotation-matrix

    quat n = quat_normalize(q);

    out.data[0] = 1.0f - 2.0f * n.y * n.y - 2.0f * n.z * n.z;
    out.data[1] = 2.0f * n.x * n.y - 2.0f * n.z * n.w;
    out.data[2] = 2.0f * n.x * n.z + 2.0f * n.y * n.w;

    out.data[4] = 2.0f * n.x * n.y + 2.0f * n.z * n.w;
    out.data[5] = 1.0f - 2.0f * n.x * n.x - 2.0f * n.z * n.z;
    out.data[6] = 2.0f * n.y * n.z - 2.0f * n.x * n.w;

    out.data[8] = 2.0f * n.x * n.z - 2.0f * n.y * n.w;
    out.data[9] = 2.0f * n.y * n.z + 2.0f * n.x * n.w;
    out.data[10] = 1.0f - 2.0f * n.x * n.x - 2.0f * n.y * n.y;

    return out;
}

// Calculates a rotation matrix based on the quaternion and the passed in center point.
inline mat4 quat_to_rotation_matrix(quat q, vec3 center) {
    mat4 out;

    f32* o = out.data;
    o[0] = (q.x * q.x) - (q.y * q.y) - (q.z * q.z) + (q.w * q.w);
    o[1] = 2.0f * ((q.x * q.y) + (q.z * q.w));
    o[2] = 2.0f * ((q.x * q.z) - (q.y * q.w));
    o[3] = center.x - center.x * o[0] - center.y * o[1] - center.z * o[2];

    o[4] = 2.0f * ((q.x * q.y) - (q.z * q.w));
    o[5] = -(q.x * q.x) + (q.y * q.y) - (q.z * q.z) + (q.w * q.w);
    o[6] = 2.0f * ((q.y * q.z) + (q.x * q.w));
    o[7] = center.y - center.x * o[4] - center.y * o[5] - center.z * o[6];

    o[8] = 2.0f * ((q.x * q.z) + (q.y * q.w));
    o[9] = 2.0f * ((q.y * q.z) - (q.x * q.w));
    o[10] = -(q.x * q.x) - (q.y * q.y) + (q.z * q.z) + (q.w * q.w);
    o[11] = center.z - center.x * o[8] - center.y * o[9] - center.z * o[10];

    o[12] = 0.0f;
    o[13] = 0.0f;
    o[14] = 0.0f;
    o[15] = 1.0f;
    return out;
}

inline quat quat_from_axis_angle(vec3 axis, f32 angle, b8 normalize) {
    const f32 half_angle = 0.5f * angle;
    f32 s = re_sin(half_angle);
    f32 c = re_cos(half_angle);

    quat q = (quat){s * axis.x, s * axis.y, s * axis.z, c};
    if (normalize) {
        return quat_normalize(q);
    }
    return q;
}

inline quat quat_slerp(quat q0, quat q1, f32 percentage) {
    quat out;
    // Source: https://en.wikipedia.org/wiki/Slerp
    // Only unit quaternions are valid rotations.
    // Normalize to avoid undefined behavior.
    quat v0 = quat_normalize(q0);
    quat v1 = quat_normalize(q1);

    // Compute the cosine of the angle between the two vectors.
    f32 dot = quat_dot(v0, v1);

    // If the dot product is negative, slerp won't take
    // the shorter path. Note that v1 and -v1 are equivalent when
    // the negation is applied to all four components. Fix by
    // reversing one quaternion.
    if (dot < 0.0f) {
        v1.x = -v1.x;
        v1.y = -v1.y;
        v1.z = -v1.z;
        v1.w = -v1.w;
        dot = -dot;
    }

    const f32 DOT_THRESHOLD = 0.9995f;
    if (dot > DOT_THRESHOLD) {
        // If the inputs are too close for comfort, linearly interpolate
        // and normalize the result.
        out = (quat){
            v0.x + ((v1.x - v0.x) * percentage),
            v0.y + ((v1.y - v0.y) * percentage),
            v0.z + ((v1.z - v0.z) * percentage),
            v0.w + ((v1.w - v0.w) * percentage)};

        return quat_normalize(out);
    }

    // Since dot is in range [0, DOT_THRESHOLD], acos is safe
    f32 theta_0 = re_acos(dot);          // theta_0 = angle between input vectors
    f32 theta = theta_0 * percentage;  // theta = angle between v0 and result
    f32 sin_theta = re_sin(theta);       // compute this value only once
    f32 sin_theta_0 = re_sin(theta_0);   // compute this value only once

    f32 s0 = re_cos(theta) - dot * sin_theta / sin_theta_0;  // == sin(theta_0 - theta) / sin(theta_0)
    f32 s1 = sin_theta / sin_theta_0;

    return (quat){
        (v0.x * s0) + (v1.x * s1),
        (v0.y * s0) + (v1.y * s1),
        (v0.z * s0) + (v1.z * s1),
        (v0.w * s0) + (v1.w * s1)};
}

/**
 * @brief Converts provided degrees to radians.
 * 
 * @param degrees The degrees to be converted.
 * @return The amount in radians.
 */
inline f32 deg_to_rad(f32 degrees) {
    return degrees * RE_DEG2RAD_MULTIPLIER;
}

/**
 * @brief Converts provided radians to degrees.
 * 
 * @param radians The radians to be converted.
 * @return The amount in degrees.
 */
inline f32 rad_to_deg(f32 radians) {
    return radians * RE_RAD2DEG_MULTIPLIER;
}