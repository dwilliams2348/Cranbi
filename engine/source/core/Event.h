#pragma once

#include "Defines.h"

typedef struct EventContext
{
    //128 bytes
    union
    {
        i64 i64[2];
        u64 u64[2];
        f64 f64[2];

        i32 i32[4];
        u32 u32[4];
        f32 f32[4];

        i16 i16[8];
        u16 u16[8];

        i8 i8[16];
        u8 u8[16];

        char c[16];
    } data;
} EventContext;

//should return true if handled
typedef b8(*PFNOnEvent)(u16 _code, void* _sender, void* _listenInst, EventContext _data);

void EventSystemInitialize(u64* _memoryRequirement, void* _state);
void EventSystemShutdown(void* _state);

/**
 * Register to listen for when events are sent with code.
 * Events with duplicate listener/callback combos will not be register again and will return false.
 * @param _code The event code to listen for.
 * @param _listener A pointer to a listener instance. Can be 0/NULL
 * @param _onEvent The callback function pointer to be invoked when the event code is fired.
 * @returns true if the event is successfully registered, otherwise false.
 */
CAPI b8 EventRegister(u16 _code, void* _listener, PFNOnEvent _onEvent);

/**
 * Unregister from listening for when events are sent.
 * If no matching registration is found this returns false
 * @param _code The event code to stop listening for.
 * @param _listener A pointer to a listener instance. Can be 0/NULL
 * @param _onEvent The callback function pointer to be unregistered.
 * @returns true if the event is successfully unregistered, otherwise false.
 */
CAPI b8 EventUnregister(u16 _code, void* _listener, PFNOnEvent _onEvent);

/**
 * Fires event to the listener of the code.
 * If an event handler returns true the event is considered handled and wont be passed to any other listeners.
 * @param _code The event code to fire.
 * @param _sender A pointer to the sender. Can be 0/NULL
 * @param _data The event data.
 * @returns true if the event is handled otherwise false.
 */
CAPI b8 EventFire(u16 _code, void* _sender, EventContext _context);

//system internal event codes, Application should use codes beyond 255
typedef enum SystemEventCode
{
    // Shuts the application down on the next frame.
    EVENT_CODE_APPLICATION_QUIT = 0x01,

    // Keyboard key pressed.
    /* Context usage:
     * u16 key_code = data.data.u16[0];
     */
    EVENT_CODE_KEY_PRESSED = 0x02,

    // Keyboard key released.
    /* Context usage:
     * u16 key_code = data.data.u16[0];
     */
    EVENT_CODE_KEY_RELEASED = 0x03,

    // Mouse button pressed.
    /* Context usage:
     * u16 button = data.data.u16[0];
     */
    EVENT_CODE_BUTTON_PRESSED = 0x04,

    // Mouse button released.
    /* Context usage:
     * u16 button = data.data.u16[0];
     */
    EVENT_CODE_BUTTON_RELEASED = 0x05,

    // Mouse moved.
    /* Context usage:
     * u16 x = data.data.u16[0];
     * u16 y = data.data.u16[1];
     */
    EVENT_CODE_MOUSE_MOVED = 0x06,

    // Mouse moved.
    /* Context usage:
     * u8 z_delta = data.data.u8[0];
     */
    EVENT_CODE_MOUSE_WHEEL = 0x07,

    // Resized/resolution changed from the OS.
    /* Context usage:
     * u16 width = data.data.u16[0];
     * u16 height = data.data.u16[1];
     */
    EVENT_CODE_RESIZED = 0x08,

    MAX_EVENT_CODE = 0xFF
} SystemEventCode;