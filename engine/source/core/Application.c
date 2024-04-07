#include "Application.h"
#include "GameTypes.h"

#include "platform/Platform.h"

#include "core/Logger.h" 
#include "core/CMemory.h"
#include "core/Event.h"
#include "core/Input.h"
#include "core/Clock.h"

#include "renderer/RendererFrontend.h"

typedef struct ApplicationState
{
    Game* gameInst;
    b8 isRunning;
    b8 isSuspended;
    PlatformState platform;
    i16 width;
    i16 height;
    Clock clock;
    f64 lastTime;
}ApplicationState;

static b8 initialized = FALSE;
static ApplicationState appState;

//event handlers
b8 ApplicationOnEvent(u16 _code, void* _sender, void* _listenerInst, EventContext _context);
b8 ApplicationOnKey(u16 _code, void* _sender, void* _listenerInst, EventContext _context);
b8 ApplicationOnResize(u16 _code, void* _sender, void* _listenerInst, EventContext _context);

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
    EventRegister(EVENT_CODE_RESIZED, 0, ApplicationOnResize);

    if(!PlatformStartup(&appState.platform, 
        _gameInst->appConfig.name, 
        _gameInst->appConfig.startPosX, 
        _gameInst->appConfig.startPosY,
        _gameInst->appConfig.startWidth, 
        _gameInst->appConfig.startHeight))
    {
        return FALSE;
    }

    if(!RendererInitialize(_gameInst->appConfig.name, &appState.platform))
    {
        LOG_FATAL("Failed to initialize renderer. Aborting application.");
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
    ClockStart(&appState.clock);
    ClockUpdate(&appState.clock);
    appState.lastTime = appState.clock.elapsed;
    f64 runningTime = 0;
    u8 frameCount = 0;
    f64 targetFrameSeconds = 1.f / 60;

    LOG_INFO(GetMemoryUsageStr());

    while(appState.isRunning) 
    {
        if(!PlatformPumpMessages(&appState.platform))
            appState.isRunning = FALSE;

        if(!appState.isSuspended)
        {
            //update clock
            ClockUpdate(&appState.clock);
            f64 currentTime = appState.clock.elapsed;
            f64 delta = currentTime - appState.lastTime;
            f64 frameStartTime = PlatformGetAbsoluteTime();

            //update routine
            if(!appState.gameInst->update(appState.gameInst, (f32)delta))
            {
                LOG_FATAL("Game update failed, shutting down");
                appState.isRunning = FALSE;
                break;
            }

            //render routine
            if(!appState.gameInst->render(appState.gameInst, (f32)delta))
            {
                LOG_FATAL("Game render failed, shutting down");
                appState.isRunning = FALSE;
                break;
            }

            //TODO: change packet creation
            RenderPacket packet;
            packet.deltaTime = delta;
            RendererDrawFrame(&packet);

            //figure out how long frame took
            f64 frameEndTime = PlatformGetAbsoluteTime();
            f64 frameElapsedTime = frameEndTime - frameStartTime;
            runningTime += frameElapsedTime;
            f64 remainingSeconds = targetFrameSeconds - frameElapsedTime;

            if(remainingSeconds > 0)
            {
                u64 remainingMS = (remainingSeconds * 1000);

                //if there is time left give it back to OS
                b8 limitFrames = FALSE;
                if(remainingMS > 0 && limitFrames)
                    PlatformSleep(remainingMS - 1);

                frameCount++;
            }

            //NOTE: Input update/state chaning should be handled should be recorded
            //As a safety input is the last thing updated before end of frame.
            InputUpdate(delta);

            //update last time
            appState.lastTime = currentTime;
        }
    }

    appState.isRunning = FALSE;

    EventUnregister(EVENT_CODE_APPLICATION_QUIT, 0, ApplicationOnEvent);
    EventUnregister(EVENT_CODE_KEY_PRESSED, 0, ApplicationOnKey);
    EventUnregister(EVENT_CODE_KEY_RELEASED, 0, ApplicationOnKey);
    EventUnregister(EVENT_CODE_RESIZED, 0, ApplicationOnResize);

    EventShutdown();
    InputShutdown();

    RendererShutdown();

    PlatformShutdown(&appState.platform);

    return TRUE;
}

void ApplicationGetFramebufferSize(u32* _width, u32* _height)
{
    *_width = appState.width;
    *_height = appState.height;
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

b8 ApplicationOnResize(u16 _code, void* _sender, void* _listenerInst, EventContext _context)
{
    if(_code == EVENT_CODE_RESIZED)
    {
        u16 width = _context.data.u16[0];
        u16 height = _context.data.u16[1];

        //check if different, if so trigger event
        if(width != appState.width || height != appState.height)
        {
            appState.width = width;
            appState.height = height;

            LOG_DEBUG("Window resize: %i, %i", width, height);

            //handle minimized
            if(width == 0 || height == 0)
            {
                LOG_INFO("Window has been minimized, suspending application.");
                appState.isSuspended = TRUE;
                return TRUE;
            }
            else
            {
                if(appState.isSuspended)
                {
                    LOG_INFO("Window restored, resuming application.");
                    appState.isSuspended = FALSE;
                }

                appState.gameInst->onResize(appState.gameInst, width, height);
                RendererOnResize(width, height);
            }
        }
    }

    //event purposfully not handled to allow other listners to get event
    return FALSE;
}