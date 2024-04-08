#include "Application.h"
#include "GameTypes.h"

#include "platform/Platform.h"

#include "core/Logger.h" 
#include "core/CMemory.h"
#include "core/Event.h"
#include "core/Input.h"
#include "core/Clock.h"

#include "memory/LinearAllocator.h"

#include "renderer/RendererFrontend.h"

typedef struct ApplicationState
{
    Game* gameInst;
    b8 isRunning;
    b8 isSuspended;
    i16 width;
    i16 height;
    Clock clock;
    f64 lastTime;

    LinearAllocator systemsAllocator;

    u64 eventSystemMemoryRequirements;
    void* eventSystemState;

    u64 memorySystemMemoryRequirement;
    void* memorySystemState;

    u64 loggingSystemMemoryRequirement;
    void* loggingSystemState;

    u64 inputSystemMemoryRequirements;
    void* inputSystemState;

    u64 platformSystemMemoryRequirements;
    void* platformSystemState;

    u64 rendererSystemMemoryRequirements;
    void* rendererSystemState;
} ApplicationState;

static ApplicationState* appState;

//event handlers
b8 ApplicationOnEvent(u16 _code, void* _sender, void* _listenerInst, EventContext _context);
b8 ApplicationOnKey(u16 _code, void* _sender, void* _listenerInst, EventContext _context);
b8 ApplicationOnResize(u16 _code, void* _sender, void* _listenerInst, EventContext _context);

b8 ApplicationCreate(Game* _gameInst)
{
    if(_gameInst->applicationState)
    {
        LOG_ERROR("ApplicationCreate called more than once\n");
        return false;
    }
    
    _gameInst->applicationState = cAllocate(sizeof(ApplicationState), MEMORY_TAG_APPLICATION);
    appState = _gameInst->applicationState;
    appState->gameInst = _gameInst;
    appState->isRunning = false;
    appState->isSuspended = false;

    u64 systemsAllocatorTotalSize = 64 * 1024 * 1024; //64 MiB
    LinearAllocatorCreate(systemsAllocatorTotalSize, 0, &appState->systemsAllocator);

    //initialize subsystems
    //events
    EventSystemInitialize(&appState->eventSystemMemoryRequirements, 0);
    appState->eventSystemState = LinearAllocatorAllocate(&appState->systemsAllocator, appState->eventSystemMemoryRequirements);
    EventSystemInitialize(&appState->eventSystemMemoryRequirements, appState->eventSystemState);

    //memory
    MemorySystemInitialize(&appState->memorySystemMemoryRequirement, 0);
    appState->memorySystemState = LinearAllocatorAllocate(&appState->systemsAllocator, appState->memorySystemMemoryRequirement);
    MemorySystemInitialize(&appState->memorySystemMemoryRequirement, appState->memorySystemState);

    //logging
    InitializeLogging(&appState->loggingSystemMemoryRequirement, 0);
    appState->loggingSystemState = LinearAllocatorAllocate(&appState->systemsAllocator, appState->loggingSystemMemoryRequirement);
    if(!InitializeLogging(&appState->loggingSystemMemoryRequirement, appState->loggingSystemState))
    {
        LOG_ERROR("Failed to initalize logging system, shutting down.");
        return false;
    }

    //input
    InputSystemInitialize(&appState->inputSystemMemoryRequirements, 0);
    appState->inputSystemState = LinearAllocatorAllocate(&appState->systemsAllocator, appState->inputSystemMemoryRequirements);
    InputSystemInitialize(&appState->inputSystemMemoryRequirements, appState->inputSystemState);

    //register engine level events
    EventRegister(EVENT_CODE_APPLICATION_QUIT, 0, ApplicationOnEvent);
    EventRegister(EVENT_CODE_KEY_PRESSED, 0, ApplicationOnKey);
    EventRegister(EVENT_CODE_KEY_RELEASED, 0, ApplicationOnKey);
    EventRegister(EVENT_CODE_RESIZED, 0, ApplicationOnResize);

    //platform startup
    PlatformSystemStartup(&appState->platformSystemMemoryRequirements, 0, 0, 0, 0, 0, 0);
    appState->platformSystemState = LinearAllocatorAllocate(&appState->systemsAllocator, appState->platformSystemMemoryRequirements);
    if(!PlatformSystemStartup(
        &appState->platformSystemMemoryRequirements,
        appState->platformSystemState,
        _gameInst->appConfig.name, 
        _gameInst->appConfig.startPosX, 
        _gameInst->appConfig.startPosY,
        _gameInst->appConfig.startWidth, 
        _gameInst->appConfig.startHeight))
    {
        return false;
    }

    //renderer startup
    RendererSystemInitialize(&appState->rendererSystemMemoryRequirements, 0, 0);
    appState->rendererSystemState = LinearAllocatorAllocate(&appState->systemsAllocator, appState->rendererSystemMemoryRequirements);
    if(!RendererSystemInitialize(&appState->rendererSystemMemoryRequirements, appState->rendererSystemState, _gameInst->appConfig.name))
    {
        LOG_FATAL("Failed to initialize renderer. Aborting application.");
        return false;
    }

    //initialize game
    if(!appState->gameInst->initialize(appState->gameInst))
    {
        LOG_FATAL("Game failed to initialize");
        return false;
    }

    appState->gameInst->onResize(appState->gameInst, appState->width, appState->height);

    return true;
}

b8 ApplicationRun()
{
    appState->isRunning = true;
    ClockStart(&appState->clock);
    ClockUpdate(&appState->clock);
    appState->lastTime = appState->clock.elapsed;
    f64 runningTime = 0;
    u8 frameCount = 0;
    f64 targetFrameSeconds = 1.f / 60;

    LOG_INFO(GetMemoryUsageStr());

    while(appState->isRunning) 
    {
        if(!PlatformPumpMessages())
            appState->isRunning = false;

        if(!appState->isSuspended)
        {
            //update clock
            ClockUpdate(&appState->clock);
            f64 currentTime = appState->clock.elapsed;
            f64 delta = currentTime - appState->lastTime;
            f64 frameStartTime = PlatformGetAbsoluteTime();

            //update routine
            if(!appState->gameInst->update(appState->gameInst, (f32)delta))
            {
                LOG_FATAL("Game update failed, shutting down");
                appState->isRunning = false;
                break;
            }

            //render routine
            if(!appState->gameInst->render(appState->gameInst, (f32)delta))
            {
                LOG_FATAL("Game render failed, shutting down");
                appState->isRunning = false;
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
                b8 limitFrames = false;
                if(remainingMS > 0 && limitFrames)
                    PlatformSleep(remainingMS - 1);

                frameCount++;
            }

            //NOTE: Input update/state chaning should be handled should be recorded
            //As a safety input is the last thing updated before end of frame.
            InputUpdate(delta);

            //update last time
            appState->lastTime = currentTime;
        }
    }

    appState->isRunning = false;

    EventUnregister(EVENT_CODE_APPLICATION_QUIT, 0, ApplicationOnEvent);
    EventUnregister(EVENT_CODE_KEY_PRESSED, 0, ApplicationOnKey);
    EventUnregister(EVENT_CODE_KEY_RELEASED, 0, ApplicationOnKey);
    EventUnregister(EVENT_CODE_RESIZED, 0, ApplicationOnResize);

    InputSystemShutdown(appState->inputSystemState);
    RendererSystemShutdown(appState->rendererSystemState);
    PlatformShutdown(&appState->platformSystemState);

    EventSystemShutdown(appState->eventSystemState);

    MemorySystemShutdown(appState->memorySystemState);

    return true;
}

void ApplicationGetFramebufferSize(u32* _width, u32* _height)
{
    *_width = appState->width;
    *_height = appState->height;
}

b8 ApplicationOnEvent(u16 _code, void* _sender, void* _listenerInst, EventContext _context)
{
    switch(_code)
    {
        case EVENT_CODE_APPLICATION_QUIT:
        {
            LOG_INFO("EVENT_CODE_APPLICATION_QUIT recieved, shutting down.");
            appState->isRunning = false;
            return true;
        }
    }

    return false;
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
            return true;
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

    return false;
}

b8 ApplicationOnResize(u16 _code, void* _sender, void* _listenerInst, EventContext _context)
{
    if(_code == EVENT_CODE_RESIZED)
    {
        u16 width = _context.data.u16[0];
        u16 height = _context.data.u16[1];

        //check if different, if so trigger event
        if(width != appState->width || height != appState->height)
        {
            appState->width = width;
            appState->height = height;

            LOG_DEBUG("Window resize: %i, %i", width, height);

            //handle minimized
            if(width == 0 || height == 0)
            {
                LOG_INFO("Window has been minimized, suspending application.");
                appState->isSuspended = true;
                return true;
            }
            else
            {
                if(appState->isSuspended)
                {
                    LOG_INFO("Window restored, resuming application.");
                    appState->isSuspended = false;
                }

                appState->gameInst->onResize(appState->gameInst, width, height);
                RendererOnResize(width, height);
            }
        }
    }

    //event purposfully not handled to allow other listners to get event
    return false;
}