/* **************************************************
 * Copyright (C) 2014 ADVENTURE Project
 * All Rights Reserved
 **************************************************** */
#include "RouteComponentKeyBase.h"
#include "Intersection.h"
#include <iostream>

using namespace std;

//======================================================================
RouteComponentKeyBase::RouteComponentKeyBase(const VehicleType type,
                                             const double weights[],
                                             const Intersection* before,
                                             const Intersection* start,
                                             const Intersection* goal)
    : _type(type), _before(before), _start(start), _goal(goal)
{
    for (unsigned int i=0; i<VEHICLE_ROUTING_PARAMETER_SIZE; i++)
    {
        _weights[i] = weights[i];
    }
}

//======================================================================
bool RouteComponentKeyBase::isEqualKey
(RouteComponentKeyBase* anotherKey) const
{
    if (anotherKey->type()      != _type
        || anotherKey->before() != _before
        || anotherKey->start()  != _start
        || anotherKey->goal()   != _goal)
    {
        return false;
    }
    for (unsigned int i=0; i<VEHICLE_ROUTING_PARAMETER_SIZE; i++)
    {
        if (anotherKey->weights(i) != _weights[i])
        {
            return false;
        }
    }
    return true;
}

//======================================================================
void RouteComponentKeyBase::print(ostream& out) const
{
        out << _type;
    for (unsigned int i=0; i<VEHICLE_ROUTING_PARAMETER_SIZE; i++)
    {
        out << ", " << _weights[i];
    }
    out << ", "
        << _before->id() << ", "
        << _start->id() << ", "
        << _goal->id();
}
