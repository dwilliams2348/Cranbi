#include "Game.h"

#include <core/Logger.h>

b8 GameInitialize(struct Game* _gameInst)
{
    LOG_DEBUG("GameInitialize() was called");
    return TRUE;
}

b8 GameUpdate(struct Game* _gameInst, f32 _deltaTime)
{
    return TRUE;
}

b8 GameRender(struct Game* _gameInst, f32 _deltaTime)
{
    return TRUE;
}

void GameOnResize(struct Game* _gameInst, u32 _width, u32 _height)
{

}