#include "core/Event.h"
#include "core/CMemory.h"

#include "containers/DArray.h"

typedef struct RegisteredEvent
{
    void* listener;
    PFNOnEvent callback;
} RegisteredEvent;

typedef struct EventCodeEntry
{
    RegisteredEvent* events;
} EventCodeEntry;

//should be enough codes
#define MAX_MESSAGE_CODES 16384

//state struct
typedef struct EventSystemState
{
    //lookup table for event codes
    EventCodeEntry registered[MAX_MESSAGE_CODES];
} EventSystemState;

/**
 * Event system internal state
*/
static EventSystemState* pState;

void EventSystemInitialize(u64* _memoryRequirement, void* _state)
{
    *_memoryRequirement = sizeof(EventSystemState);
    if(_state == 0)
        return;

    cZeroMemory(_state, sizeof(EventSystemState));
    pState = _state;
}

void EventSystemShutdown(void* _state)
{
    if(pState)
    {
    //free the event arrays, and objects pointed to should be destroyed on their own
    for(u16 i = 0; i < MAX_MESSAGE_CODES; ++i)
    {
        if(pState->registered[i].events != 0)
        {
            DArrayDestroy(pState->registered[i].events);
            pState->registered[i].events = 0;
        }
    }
    }
}

b8 EventRegister(u16 _code, void* _listener, PFNOnEvent _onEvent)
{
    if(!pState)
    {
        return false;
    }

    if(pState->registered[_code].events == 0)
    {
        pState->registered[_code].events = DArrayCreate(RegisteredEvent);
    }

    u64 registeredCount = DArrayLength(pState->registered[_code].events);
    for(u64 i = 0; i < registeredCount; ++i)
    {
        if(pState->registered[_code].events[i].listener == _listener)
        {
            //TODO: warn
            return false;
        }
    }

    //if at this point no duplicates are found then proceed with registration
    RegisteredEvent event;
    event.listener = _listener;
    event.callback = _onEvent;
    DArrayPush(pState->registered[_code].events, event);

    return true;
}

b8 EventUnregister(u16 _code, void* _listener, PFNOnEvent _onEvent)
{
    if(!pState)
        return false;

    //nothing registered to the code boot out
    if(pState->registered[_code].events == 0)
    {
        //TODO: warn
        return false;
    }

    u64 registeredCount = DArrayLength(pState->registered[_code].events);
    for(u64 i = 0; i < registeredCount; ++i)
    {
        RegisteredEvent e = pState->registered[_code].events[i];
        if(e.listener == _listener && e.callback == _onEvent)
        {
            RegisteredEvent poppedEvent;
            DArrayPopAt(pState->registered[_code].events, i, &poppedEvent);
            return true;
        }
    }

    //nothing found
    return false;
}

b8 EventFire(u16 _code, void* _sender, EventContext _context)
{
    if(!pState)
        return false;

    //if nothing is registered to code immediately boot
    if(pState->registered[_code].events == 0)
        return false;

    u64 registeredCount = DArrayLength(pState->registered[_code].events);
    for(u64 i = 0; i < registeredCount; ++i)
    {
        RegisteredEvent e = pState->registered[_code].events[i];
        if(e.callback(_code, _sender, e.listener, _context))
        {
            //message has been handled do not propigate to other listeners
            return true;
        }
    }

    //nothing found
    return false;
}