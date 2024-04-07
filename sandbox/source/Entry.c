#include <Entry.h>

#include <core/CMemory.h>

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
    _outGame->state = cAllocate(sizeof(GameState), MEMORY_TAG_GAME);

    _outGame->applicationState = 0;

    return true;
}