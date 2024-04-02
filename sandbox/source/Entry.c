#include <Entry.h>
//TODO: remove
#include <platform/Platform.h>

#include "Game.h"

//define the funciton to create game
b8 CreateGame(Game* _outGame)
{
    //application configuration
    _outGame->appConfig.startPosX = 100;
    _outGame->appConfig.startPosY = 100;
    _outGame->appConfig.startWidth = 1280;
    _outGame->appConfig.startHeight = 720;
    _outGame->appConfig.name = "Cranbi Engine Sandbox";
    _outGame->initialize = GameInitialize;
    _outGame->update = GameUpdate;
    _outGame->render = GameRender;
    _outGame->onResize = GameOnResize;

    //create game state
    _outGame->state = PlatformAllocate(sizeof(GameState), FALSE);

    return TRUE;
}