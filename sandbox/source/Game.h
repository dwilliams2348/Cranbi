#pragma once

#include <Defines.h>
#include <GameTypes.h>

typedef struct GameState
{
    f32 deltaTime;
} GameState;

b8 GameInitialize(struct Game* _gameInst);

b8 GameUpdate(struct Game* _gameInst, f32 _deltaTime);

b8 GameRender(struct Game* _gameInst, f32 _deltaTime);

void GameOnResize(struct Game* _gameInst, u32 _width, u32 _height);