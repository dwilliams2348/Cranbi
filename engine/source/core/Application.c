#include "Application.h"
#include "GameTypes.h"

#include "Logger.h"
#include "platform/Platform.h"

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
        }
    }

    appState.isRunning = FALSE;

    PlatformShutdown(&appState.platform);

    return TRUE;
}