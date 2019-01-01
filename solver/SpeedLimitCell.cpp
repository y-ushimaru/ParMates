/* **************************************************
 * Copyright (C) 2014 ADVENTURE Project
 * All Rights Reserved
 **************************************************** */
#include "SpeedLimitCell.h"

//======================================================================
SpeedLimitCell::SpeedLimitCell(ulint time, Section* section, bool isUp, double speedLimit)
    : _time(time), _section(section), _isUp(isUp), _speedLimit(speedLimit){}

//======================================================================
SpeedLimitCell::~SpeedLimitCell(){}

//======================================================================
ulint SpeedLimitCell::time() const
{
    return _time;
}

//======================================================================
const Section* SpeedLimitCell::section() const
{
    return _section;
}

//======================================================================
bool SpeedLimitCell::isUp() const
{
    return _isUp;
}

//======================================================================
double SpeedLimitCell::speedLimit() const
{
    return _speedLimit;
}
