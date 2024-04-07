#include <core/Logger.h>
#include <math/ReMath.h>

/**
 * @brief Expects expected to be equal to actual.
 */
#define ExpectShouldBe(expected, actual)                                                              \
    if (actual != expected) {                                                                           \
        LOG_ERROR("--> Expected %lld, but got: %lld. File: %s:%d.", expected, actual, __FILE__, __LINE__); \
        return false;                                                                                   \
    }

/**
 * @brief Expects expected to NOT be equal to actual.
 */
#define ExpectShouldNotBe(expected, actual)                                                                   \
    if (actual == expected) {                                                                                    \
        LOG_ERROR("--> Expected %d != %d, but they are equal. File: %s:%d.", expected, actual, __FILE__, __LINE__); \
        return false;                                                                                            \
    }

/**
 * @brief Expects expected to be actual given a tolerance of K_FLOAT_EPSILON.
 */
#define ExpectFloatToVe(expected, actual)                                                        \
    if (kabs(expected - actual) > 0.001f) {                                                         \
        LOG_ERROR("--> Expected %f, but got: %f. File: %s:%d.", expected, actual, __FILE__, __LINE__); \
        return false;                                                                               \
    }

/**
 * @brief Expects actual to be true.
 */
#define ExpectToBeTrue(actual)                                                      \
    if (actual != true) {                                                              \
        LOG_ERROR("--> Expected true, but got: false. File: %s:%d.", __FILE__, __LINE__); \
        return false;                                                                  \
    }

/**
 * @brief Expects actual to be false.
 */
#define ExpectToBeFalse(actual)                                                     \
    if (actual != false) {                                                             \
        LOG_ERROR("--> Expected false, but got: true. File: %s:%d.", __FILE__, __LINE__); \
        return false;                                                                  \
    }