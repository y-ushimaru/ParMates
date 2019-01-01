/* **************************************************
 * Copyright (C) 2014 ADVENTURE Project
 * All Rights Reserved
 **************************************************** */
#include "Blinker.h"

//======================================================================
Blinker::Blinker()
{
    _state = 0;
}

//======================================================================
Blinker::~Blinker()
{
}

//======================================================================
bool Blinker::isNone() const
{
    return (_state == 0);
}

//======================================================================
bool Blinker::isLeft() const
{
    return (_state == 1);
}

//======================================================================
bool Blinker::isRight() const
{
    return (_state == 2);
}

//======================================================================
bool Blinker::isHazard() const
{
    return (_state == 3);
}

//======================================================================
int Blinker::state() const
{
    return _state;
}

//======================================================================
RelativeDirection Blinker::turning() const
{
    switch (_state)
    {
    case 0:
        return RD_STRAIGHT;
    case 1:
        return RD_LEFT;
    case 2:
        return RD_RIGHT;
    case 3:
    default:
        return RD_NONE;
    }
}

//======================================================================
bool Blinker::operator==(Blinker& another) const
{
    return (_state == another._state);
}

//======================================================================
void Blinker::setNone()
{
    _state = 0;
}

//======================================================================
void Blinker::setLeft()
{
    _state = 1;
}

//======================================================================
void Blinker::setRight()
{
    _state = 2;
}

//======================================================================
void Blinker::setHazard()
{
    _state = 3;
}

