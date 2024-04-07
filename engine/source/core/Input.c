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
static InputState* pState;

void InputSystemInitialize(u64* _memoryRequirements, void* _state)
{
    *_memoryRequirements = sizeof(InputState);
    if(_state == 0)
        return;
    
    cZeroMemory(_state, sizeof(InputState));
    pState = _state;

    LOG_INFO("Input subsystem initialized.");
}

void InputSystemShutdown(void* _state)
{
    //TODO: add shutdown routine when needed
    pState = 0;
}

void InputUpdate(f64 _deltaTime)
{
    if(!pState)
        return;

    //copy current states to previous states
    cCopyMemory(&pState->keyboardPrev, &pState->keyboardCurr, sizeof(KeyboardState));
    cCopyMemory(&pState->mousePrev, &pState->mouseCurr, sizeof(MouseState));
}

void InputProcessKey(Keys _key, b8 _pressed)
{
    if (_key == KEY_LALT) 
    {
        LOG_INFO("Left alt pressed.");
    } 
    else if (_key == KEY_RALT) 
    {
        LOG_INFO("Right alt pressed.");
    }

    if (_key == KEY_LCONTROL) 
    {
        LOG_INFO("Left ctrl pressed.");
    } 
    else if (_key == KEY_RCONTROL) 
    {
        LOG_INFO("Right ctrl pressed.");
    }

    if (_key == KEY_LSHIFT) 
    {
        LOG_INFO("Left shift pressed.");
    } 
    else if (_key == KEY_RSHIFT) 
    {
        LOG_INFO("Right shift pressed.");
    }

    //only handle if the state actually changed
    if(pState->keyboardCurr.keys[_key] != _pressed)
    {
        //update internal state
        pState->keyboardCurr.keys[_key] = _pressed;

        //fire off event for immediate processing
        EventContext context;
        context.data.u16[0] = _key;
        EventFire(_pressed ? EVENT_CODE_KEY_PRESSED : EVENT_CODE_KEY_RELEASED, 0, context);
    }
}

void InputProcessButton(Buttons _button, b8 _pressed)
{
    //only handle if the state has changed
    if(pState->mouseCurr.buttons[_button] != _pressed)
    {
        //update internal state
        pState->mouseCurr.buttons[_button] = _pressed;

        //fire off event for immediate processing
        EventContext context;
        context.data.u16[0] = _button;
        EventFire(_pressed ? EVENT_CODE_BUTTON_PRESSED : EVENT_CODE_BUTTON_RELEASED, 0, context);
    }
}

void InputProcessMouseMove(i16 _x, i16 _y)
{
    //only process if different
    if(pState->mouseCurr.x != _x || pState->mouseCurr.y != _y)
    {
        //NOTE: enable if debugging
        //LOG_DEBUG("Mouse pos: %i, %i", _x, _y);

        //update internal state
        pState->mouseCurr.x = _x;
        pState->mouseCurr.y = _y;

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
    if(!pState)
        return false;
    
    return pState->keyboardCurr.keys[_key] == true;
}

b8 InputIsKeyUp(Keys _key)
{
    if(!pState)
        return true;
    
    return pState->keyboardCurr.keys[_key] == false;
}

b8 InputWasKeyDown(Keys _key)
{
    if(!pState)
        return false;
    
    return pState->keyboardPrev.keys[_key] == true;
}

b8 InputWasKeyUp(Keys _key)
{
    if(!pState)
        return true;
    
    return pState->keyboardPrev.keys[_key] == false;
}

b8 InputIsButtonDown(Buttons _button)
{
    if(!pState)
        return false;
    
    return pState->mouseCurr.buttons[_button] == true;
}

b8 InputIsButtonUp(Buttons _button)
{
    if(!pState)
        return true;
    
    return pState->mouseCurr.buttons[_button] == false;
}

b8 InputWasButtonDown(Buttons _button)
{
    if(!pState)
        return false;
    
    return pState->mousePrev.buttons[_button] == true;
}

b8 InputWasButtonUp(Buttons _button)
{
    if(!pState)
        return true;
    
    return pState->mousePrev.buttons[_button] == false;
}

void InputGetMousePosition(i32* _x, i32* _y)
{
    if(!pState)
    {
        *_x = 0;
        *_y = 0;
        return;
    }

    *_x = pState->mouseCurr.x;
    *_y = pState->mouseCurr.y;
}

void InputGetPreviousMousePosition(i32* _x, i32* _y)
{
    if(!pState)
    {
        *_x = 0;
        *_y = 0;
        return;
    }

    *_x = pState->mousePrev.x;
    *_y = pState->mousePrev.y;
}