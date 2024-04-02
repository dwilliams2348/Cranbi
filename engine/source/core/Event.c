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
static b8 isInitialized = FALSE;
static EventSystemState state;

b8 EventInitialize()
{
    if(isInitialized)
        return FALSE;

    isInitialized = FALSE;
    cZeroMemory(&state, sizeof(state));

    isInitialized = TRUE;
    return TRUE;
}

void EventShutdown()
{
    //free the event arrays, and objects pointed to should be destroyed on their own
    for(u16 i = 0; i < MAX_MESSAGE_CODES; ++i)
    {
        if(state.registered[i].events != 0)
        {
            DArrayDestroy(state.registered[i].events);
            state.registered[i].events = 0;
        }
    }
}

b8 EventRegister(u16 _code, void* _listener, PFNOnEvent _onEvent)
{
    if(isInitialized == FALSE)
    {
        return FALSE;
    }

    if(state.registered[_code].events == 0)
    {
        state.registered[_code].events = DArrayCreate(RegisteredEvent);
    }

    u64 registeredCount = DArrayLength(state.registered[_code].events);
    for(u64 i = 0; i < registeredCount; ++i)
    {
        if(state.registered[_code].events[i].listener == _listener)
        {
            //TODO: warn
            return FALSE;
        }
    }

    //if at this point no duplicates are found then proceed with registration
    RegisteredEvent event;
    event.listener = _listener;
    event.callback = _onEvent;
    DArrayPush(state.registered[_code].events, event);

    return TRUE;
}

b8 EventUnregister(u16 _code, void* _listener, PFNOnEvent _onEvent)
{
    if(isInitialized == FALSE)
        return FALSE;

    //nothing registered to the code boot out
    if(state.registered[_code].events == 0)
    {
        //TODO: warn
        return FALSE;
    }

    u64 registeredCount = DArrayLength(state.registered[_code].events);
    for(u64 i = 0; i < registeredCount; ++i)
    {
        RegisteredEvent e = state.registered[_code].events[i];
        if(e.listener == _listener && e.callback == _onEvent)
        {
            RegisteredEvent poppedEvent;
            DArrayPopAt(state.registered[_code].events, i, &poppedEvent);
            return TRUE;
        }
    }

    //nothing found
    return FALSE;
}

b8 EventFire(u16 _code, void* _sender, EventContext _context)
{
    if(isInitialized == FALSE)
        return FALSE;

    //if nothing is registered to code immediately boot
    if(state.registered[_code].events == 0)
        return FALSE;

    u64 registeredCount = DArrayLength(state.registered[_code].events);
    for(u64 i = 0; i < registeredCount; ++i)
    {
        RegisteredEvent e = state.registered[_code].events[i];
        if(e.callback(_code, _sender, e.listener, _context))
        {
            //message has been handled do not propigate to other listeners
            return TRUE;
        }
    }

    //nothing found
    return FALSE;
}