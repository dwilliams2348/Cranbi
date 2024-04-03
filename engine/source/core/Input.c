#include "core/Input.h"
#include "core/Event.h"
#include "core/CMemory.h"
#include "core/Logger.h"

typedef struct KeyboardState
{
    b8 keys[256];
} KeyboardState;

typedef struct MouseState
{
    i16 x;
    i16 y;
    u8 buttons[BUTTON_MAX_BUTTONS];
} MouseState;

typedef struct InputState
{
    KeyboardState keyboardCurr;
    KeyboardState keyboardPrev;
    MouseState mouseCurr;
    MouseState mousePrev;
} InputState;

//internal input state
static b8 initialized = FALSE;
static InputState state = {};

void InputInitialize()
{
    cZeroMemory(&state, sizeof(InputState));
    initialized = TRUE;
    LOG_INFO("Input subsystem initialized.");
}

void InputShutdown()
{
    //TODO: add shutdown routine when needed
    initialized = FALSE;
}

void InputUpdate(f64 _deltaTime)
{
    if(!initialized)
        return;

    //copy current states to previous states
    cCopyMemory(&state.keyboardPrev, &state.keyboardCurr, sizeof(KeyboardState));
    cCopyMemory(&state.mousePrev, &state.mouseCurr, sizeof(MouseState));
}

void InputProcessKey(Keys _key, b8 _pressed)
{
    //only handle if the state actually changed
    if(state.keyboardCurr.keys[_key] != _pressed)
    {
        //update internal state
        state.keyboardCurr.keys[_key] = _pressed;

        //fire off event for immediate processing
        EventContext context;
        context.data.u16[0] = _key;
        EventFire(_pressed ? EVENT_CODE_KEY_PRESSED : EVENT_CODE_KEY_RELEASED, 0, context);
    }
}

void InputProcessButton(Buttons _button, b8 _pressed)
{
    //only handle if the state has changed
    if(state.mouseCurr.buttons[_button] != _pressed)
    {
        //update internal state
        state.mouseCurr.buttons[_button] = _pressed;

        //fire off event for immediate processing
        EventContext context;
        context.data.u16[0] = _button;
        EventFire(_pressed ? EVENT_CODE_BUTTON_PRESSED : EVENT_CODE_BUTTON_RELEASED, 0, context);
    }
}

void InputProcessMouseMove(i16 _x, i16 _y)
{
    //only process if different
    if(state.mouseCurr.x != _x || state.mouseCurr.y != _y)
    {
        //NOTE: enable if debugging
        //LOG_DEBUG("Mouse pos: %i, %i", _x, _y);

        //update internal state
        state.mouseCurr.x = _x;
        state.mouseCurr.y = _y;

        //fire event
        EventContext context;
        context.data.u16[0] = _x;
        context.data.u16[1] = _y;
        EventFire(EVENT_CODE_MOUSE_MOVED, 0, context);
    }
}

void InputProcessMouseWheel(i8 _zDelta)
{
    //NOTE: no internal state to update

    //fire event
    EventContext context;
    context.data.u8[0] = _zDelta;
    EventFire(EVENT_CODE_MOUSE_WHEEL, 0, context);
}

b8 InputIsKeyDown(Keys _key)
{
    if(!initialized)
        return FALSE;
    
    return state.keyboardCurr.keys[_key] == TRUE;
}

b8 InputIsKeyUp(Keys _key)
{
    if(!initialized)
        return TRUE;
    
    return state.keyboardCurr.keys[_key] == FALSE;
}

b8 InputWasKeyDown(Keys _key)
{
    if(!initialized)
        return FALSE;
    
    return state.keyboardPrev.keys[_key] == TRUE;
}

b8 InputWasKeyUp(Keys _key)
{
    if(!initialized)
        return TRUE;
    
    return state.keyboardPrev.keys[_key] == FALSE;
}

b8 InputIsButtonDown(Buttons _button)
{
    if(!initialized)
        return FALSE;
    
    return state.mouseCurr.buttons[_button] == TRUE;
}

b8 InputIsButtonUp(Buttons _button)
{
    if(!initialized)
        return TRUE;
    
    return state.mouseCurr.buttons[_button] == FALSE;
}

b8 InputWasButtonDown(Buttons _button)
{
    if(!initialized)
        return FALSE;
    
    return state.mousePrev.buttons[_button] == TRUE;
}

b8 InputWasButtonUp(Buttons _button)
{
    if(!initialized)
        return TRUE;
    
    return state.mousePrev.buttons[_button] == FALSE;
}

void InputGetMousePosition(i32* _x, i32* _y)
{
    if(!initialized)
    {
        *_x = 0;
        *_y = 0;
        return;
    }

    *_x = state.mouseCurr.x;
    *_y = state.mouseCurr.y;
}

void InputGetPreviousMousePosition(i32* _x, i32* _y)
{
    if(!initialized)
    {
        *_x = 0;
        *_y = 0;
        return;
    }

    *_x = state.mousePrev.x;
    *_y = state.mousePrev.y;
}