#pragma once

#include "Defines.h"

struct Game;

typedef struct ApplicationConfig
{
    //window starting position x axis
    i16 startPosX;
    //window starting position y axis
    i16 startPosY;
    //window starting width
    i16 startWidth;
    //window starting height
    i16 startHeight;
    //application name
    char* name;
}ApplicationConfig;

CAPI b8 ApplicationCreate(struct Game* _gameInst);

CAPI b8 ApplicationRun();

void ApplicationGetFramebufferSize(u32* _width, u32* _height);