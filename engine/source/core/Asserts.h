#pragma once

#include "Defines.h"

//disable assertions by commenting out line below
#define CASSERTIONS_ENABLED

#ifdef CASSERTIONS_ENABLED
#   if _MSC_VER
#       include <intrin.h>
#       define debugBreak() __debugbreak()
#   else
#       define debugBreak() __builtin_trap()
#   endif

CAPI void ReportAssertionFailure(const char* _expression, const char* _msg, const char* _file, i32 _line);

#   define CORE_ASSERT(expr)                                        \
    {                                                               \
        if(expr)  {}                                                \
        else                                                        \
        {                                                           \
            ReportAssertionFailure(#expr, "", __FILE__, __LINE__);  \
            debugBreak();                                           \
        }                                                           \
    }   

#   define CORE_ASSERT_MSG(expr, msg)                               \
    {                                                               \
        if(expr)  {}                                                \
        else                                                        \
        {                                                           \
            ReportAssertionFailure(#expr, msg, __FILE__, __LINE__); \
            debugBreak();                                           \
        }                                                           \
    }

#ifdef _DEBUG
#   define CORE_ASSERT_DEBUG(expr)                                  \
    {                                                               \
        if(expr)  {}                                                \
        else                                                        \
        {                                                           \
            ReportAssertionFailure(#expr, msg, __FILE__, __LINE__); \
            debugBreak();                                           \
        }                                                           \
    }
#else
#   define CORE_ASSERT_DEBUG(expr) //does nothing
#endif

#else
#   define CORE_ASSERT(expr)           //does nothing
#   define CORE_ASSERT_MSG(expr, msg)  //does nothing
#   define CORE_ASSERT_DEBUG(expr)     //does nothing
#endif