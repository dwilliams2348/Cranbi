#pragma once

#include "Defines.h"

#define LOG_WARN_ENABLED  1
#define LOG_INFO_ENABLED  1
#define LOG_DEBUG_ENABLED 1
#define LOG_TRACE_ENABLED 1

//disable debug and trace logging for release builds of engine
#if CRELEASE == 1
#   define LOG_DEBUG_ENABLED 0
#   define LOG_TRACE_ENABLED 0
#endif

typedef enum ELogLevel {
    FATAL   = 0, //cause engine to crash
    ERROR   = 1, //serious error where it wont crash but wont run right
    WARN    = 2, //suboptimal but can still run well
    INFO    = 3, //information
    DEBUG   = 4, //only in debug and useful for gathering info
    TRACE   = 5  //more verbose than debug
} LogLevel;

/**
 * @brief Initializes logging system. Call twice; once with state = 0 to get required memory size,
 * then a second time passing allocated memory to state.
 * 
 * @param _memoryRequirement A pointer to hold the required memory size of internal state.
 * @param _state 0 if just requesting memory requirement, otherwise allocated block of memory.
 * @return b8 True on success; otherwise false.
 */
b8 InitializeLogging(u64* _memoryRequirement, void* _state);
void ShutdownLogging(void* _state);

CAPI void LogOutput(LogLevel _level, const char* _msg, ...);

//logs a fatal level message
#define LOG_FATAL(message, ...) LogOutput(FATAL, message, ##__VA_ARGS__);

#ifndef LOG_ERROR
//logs an error level message
#   define LOG_ERROR(message, ...) LogOutput(ERROR, message, ##__VA_ARGS__);
#endif

#if LOG_WARN_ENABLED == 1
//logs a warning level message
#   define LOG_WARN(message, ...) LogOutput(WARN, message, ##__VA_ARGS__);
#else
//does nothing when warn logging != 1
#   define LOG_WARN(message, ...)
#endif

#if LOG_INFO_ENABLED == 1
//logs an info level message
#   define LOG_INFO(message, ...) LogOutput(INFO, message, ##__VA_ARGS__);
#else
//does nothing when info logging is != 1
#   define LOG_INFO(message, ...)
#endif

#if LOG_DEBUG_ENABLED == 1
//logs a debug level message
#   define LOG_DEBUG(message, ...) LogOutput(DEBUG, message, ##__VA_ARGS__);
#else
//does nothing when debug logging is != 1
#   define LOG_DEBUG(message, ...)
#endif

#if LOG_TRACE_ENABLED == 1
//logs a trace level message
#   define LOG_TRACE(message, ...) LogOutput(TRACE, message, ##__VA_ARGS__);
#else
//does nothing when trace logging is != 1
#   define LOG_TRACE(message, ...)
#endif