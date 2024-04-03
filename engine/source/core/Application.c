#include "Application.h"
#include "GameTypes.h"

#include "platform/Platform.h"

#include "core/Logger.h" 
#include "core/CMemory.h"
#include "core/Event.h"
#include "core/Input.h"

typedef struct ApplicationState
{
    Game* gameInst;
    b8 isRunning;
    b8 isSuspended;
    PlatformState platform;
    i16 width;
    i16 height;
    f64 lastTime;
}ApplicationState;

static b8 initialized = FALSE;
static ApplicationState appState;

//event handlers
b8 ApplicationOnEvent(u16 _code, void* _sender, void* _listenerInst, EventContext _context);
b8 ApplicationOnKey(u16 _code, void* _sender, void* _listenerInst, EventContext _context);

b8 ApplicationCreate(Game* _gameInst)
{
    if(initialized)
    {
        LOG_ERROR("ApplicationCreate called more than once\n");
        return FALSE;
    }
    
    appState.gameInst = _gameInst;

    //initialize subsystems
    InitializeLogging();
    InputInitialize();

    //TODO: remove this
    LOG_FATAL("A fatal test message: %f", 3.14f);
    LOG_ERROR("An error test message: %d", 42);
    LOG_WARN("A warn test message: %s", "suck ween");
    LOG_INFO("An info test message: %f", 6.9f);
    LOG_DEBUG("A debug test message: %f", 42.0f);
    LOG_TRACE("A trace test message");

    //setting up application state
    appState.isRunning = TRUE;
    appState.isSuspended = FALSE;

    if(!EventInitialize())
    {
        LOG_ERROR("Event system failed to initialize, Application cannot continue.");
        return FALSE;
    }

    EventRegister(EVENT_CODE_APPLICATION_QUIT, 0, ApplicationOnEvent);
    EventRegister(EVENT_CODE_KEY_PRESSED, 0, ApplicationOnKey);
    EventRegister(EVENT_CODE_KEY_RELEASED, 0, ApplicationOnKey);

    if(!PlatformStartup(&appState.platform, 
        _gameInst->appConfig.name, 
        _gameInst->appConfig.startPosX, 
        _gameInst->appConfig.startPosY,
        _gameInst->appConfig.startWidth, 
        _gameInst->appConfig.startHeight))
    {
        return FALSE;
    }

    //initialize game
    if(!appState.gameInst->initialize(appState.gameInst))
    {
        LOG_FATAL("Game failed to initialize");
        return FALSE;
    }

    appState.gameInst->onResize(appState.gameInst, appState.width, appState.height);

    initialized = TRUE;

    return TRUE;
}

b8 ApplicationRun()
{
    LOG_INFO(GetMemoryUsageStr());

    while(appState.isRunning) 
    {
        if(!PlatformPumpMessages(&appState.platform))
            appState.isRunning = FALSE;

        if(!appState.isSuspended)
        {
            //update routine
            if(!appState.gameInst->update(appState.gameInst, (f32)0))
            {
                LOG_FATAL("Game update failed, shutting down");
                appState.isRunning = FALSE;
                break;
            }

            //render routine
            if(!appState.gameInst->render(appState.gameInst, (f32)0))
            {
                LOG_FATAL("Game render failed, shutting down");
                appState.isRunning = FALSE;
                break;
            }

            //NOTE: Input update/state chaning should be handled should be recorded
            //As a safety input is the last thing updated before end of frame.
            InputUpdate(0);
        }
    }

    appState.isRunning = FALSE;

    EventUnregister(EVENT_CODE_APPLICATION_QUIT, 0, ApplicationOnEvent);
    EventUnregister(EVENT_CODE_KEY_PRESSED, 0, ApplicationOnKey);
    EventUnregister(EVENT_CODE_KEY_RELEASED, 0, ApplicationOnKey);

    EventShutdown();
    InputShutdown();

    PlatformShutdown(&appState.platform);

    return TRUE;
}

b8 ApplicationOnEvent(u16 _code, void* _sender, void* _listenerInst, EventContext _context)
{
    switch(_code)
    {
        case EVENT_CODE_APPLICATION_QUIT:
        {
            LOG_INFO("EVENT_CODE_APPLICATION_QUIT recieved, shutting down.");
            appState.isRunning = FALSE;
            return TRUE;
        }
    }

    return FALSE;
}

b8 ApplicationOnKey(u16 _code, void* _sender, void* _listenerInst, EventContext _context)
{
    if(_code == EVENT_CODE_KEY_PRESSED)
    {
        u16 keyCode = _context.data.u16[0];
        if(keyCode == KEY_ESCAPE)
        {
            //NOTE: techinally firing an event to itself, but there may be other listeners
            EventContext data = {};
            EventFire(EVENT_CODE_APPLICATION_QUIT, 0, data);

            //block anything else from processing
            return TRUE;
        }
        else if(keyCode == KEY_A)
        {
            //example for checking for a key
            LOG_DEBUG("Explicit - KEY_A was pressed.");
        }
        else
        {
            LOG_DEBUG("'%c' key was pressed in window.", keyCode);
        }
    }
    else if(_code == EVENT_CODE_KEY_RELEASED)
    {
        u16 keyCode = _context.data.u16[0];
        if(keyCode == KEY_B)
        {
            LOG_DEBUG("Explicit - KEY_B was released.");
        }
        else
        {
            LOG_DEBUG("'%c' key was released in window.", keyCode);
        }
    }

    return FALSE;
}