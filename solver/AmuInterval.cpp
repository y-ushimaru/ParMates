/* **************************************************
 * Copyright (C) 2014 ADVENTURE Project
 * All Rights Reserved
 **************************************************** */
#include "AmuInterval.h"

//======================================================================
AmuInterval::AmuInterval(double lower, double upper)
{
    _lower = lower;
    _upper   = upper;
}

//======================================================================
double AmuInterval::lower() const
{
    return _lower;
}

//======================================================================
double AmuInterval::upper() const
{
    return _upper;
}

//======================================================================
bool AmuInterval::isIncluded(double num) const
{
    return (_lower<=num && num<=_upper);
}

//======================================================================
bool AmuInterval::operator<(const AmuInterval& rhs) const
{
    if (_upper <= rhs.lower())
    {
        return true;
    }
    else
    {
        return false;
    }
}

//======================================================================
bool AmuInterval::operator>(const AmuInterval& rhs) const
{
    if (_lower >= rhs.upper())
    {
        return true;
    }
    else
    {
        return false;
    }
}

//======================================================================
bool AmuInterval::operator==(const AmuInterval& rhs) const
{
    if (_upper <= rhs.lower() || rhs.upper() <= _lower)
    {
        return false;
    }
    else
    {
        return true;
    }
}

