#pragma once

#include "core/Application.h"
#include "core/Logger.h"
#include "core/CMemory.h"
#include "GameTypes.h"

//externally defined function to create game
extern b8 CreateGame(Game* _outGame);

/*
    Main entry point of application
*/
int main(void)
{
    InitializeMemory();

    //request game instance from application
    Game gameInstance;
    if(!CreateGame(&gameInstance))
    {
        LOG_FATAL("Could not create game");
        return -1;
    }

    //ensure that the function pointers exist
    if(!gameInstance.render || !gameInstance.update || !gameInstance.initialize || !gameInstance.onResize)
    {
        LOG_FATAL("The game's function pointers must be assigned");
        return -2;
    }
    
    //initialization
    if(!ApplicationCreate(&gameInstance))
    {
        LOG_FATAL("Application failed to create")
        return 1;
    }

    //begin game loop
    if(!ApplicationRun())
    {
        LOG_FATAL("Application did not shutdown gracefully.");
        return 2;
    }

    ShutdownMemory();

    return 0;
}