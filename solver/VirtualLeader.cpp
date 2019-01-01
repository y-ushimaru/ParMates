/* **************************************************
 * Copyright (C) 2014 ADVENTURE Project
 * All Rights Reserved
 **************************************************** */
#include "VirtualLeader.h"
#include <iostream>

using namespace std;

//======================================================================
VirtualLeader::VirtualLeader(){}

#ifndef VL_DEBUG
//======================================================================
VirtualLeader::VirtualLeader(double distance, double velocity)
{
    _distance = distance;
    _velocity = velocity;
}
#else
//======================================================================
VirtualLeader::VirtualLeader(double distance, double velocity,
                             std::string id)
{
    _distance = distance;
    _velocity = velocity;
    _id = id;
}
#endif
//======================================================================
VirtualLeader::~VirtualLeader(){}

//======================================================================
void VirtualLeader::setInformation(double distance, double velocity)
{
    _distance = distance;
    _velocity = velocity;
}

//======================================================================
double VirtualLeader::distance() const
{
    return _distance;
}

//======================================================================
double VirtualLeader::velocity() const
{
    return _velocity;
}

//======================================================================
const string& VirtualLeader::id() const
{
    return _id;
}

#ifdef VL_DEBUG
//======================================================================
void VirtualLeader::print() const
{
    cout << "  " << _id  << ", "
         << "distance:"<< _distance << ", "
         << "velocity:"   <<_velocity << endl;
}
#endif
