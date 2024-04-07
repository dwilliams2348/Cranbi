#include "Game.h"

#include <core/Logger.h>
#include <core/CMemory.h>

#include<core/Input.h>

b8 GameInitialize(struct Game* _gameInst)
{
    LOG_DEBUG("GameInitialize() was called");
    return true;
}

b8 GameUpdate(struct Game* _gameInst, f32 _deltaTime)
{
    static u64 allocCount = 0;
    u64 prevAllocCount = allocCount;
    allocCount = GetMemoryAllocCount();

    if(InputIsKeyUp('M') && InputWasKeyDown('M'))
    {
        LOG_DEBUG("Allocations: %llu (%llu this frame)", allocCount, allocCount - prevAllocCount);
    }

    return true;
}

b8 GameRender(struct Game* _gameInst, f32 _deltaTime)
{
    return true;
}

void GameOnResize(struct Game* _gameInst, u32 _width, u32 _height)
{

}