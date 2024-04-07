#pragma once

#include "core/Application.h"

/*
    Represents the basic game state in a game
    Called for creation by application
*/

typedef struct Game
{
    //applicaiton configuration
    ApplicationConfig appConfig;

    //funciton pointer to games initialize function
    b8(*initialize)(struct Game* _gameInst);

    //function pointer to games update function
    b8(*update)(struct Game* _gameInst, f32 _deltaTime);

    //function pointer to games render function
    b8(*render)(struct Game* _gameInst, f32 _deltaTime);

    //function pointer to handle resize event
    void(*onResize)(struct Game* _gameInst, u32 _width, u32 _height);

    //game specific game sate, created and managed by the game
    void* state;

    //application state
    void* applicationState;
}Game;