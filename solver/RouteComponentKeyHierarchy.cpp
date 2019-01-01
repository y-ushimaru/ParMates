/* **************************************************
 * Copyright (C) 2014 ADVENTURE Project
 * All Rights Reserved
 **************************************************** */
#include "RouteComponentKeyHierarchy.h"
#include "Intersection.h"
#include <iostream>

using namespace std;

//======================================================================
RouteComponentKeyHierarchy::RouteComponentKeyHierarchy
(const VehicleType type,
 const unsigned int prefRank,
 const double weights[],
 const Intersection* before,
 const Intersection* start,
 const Intersection* goal)
    : RouteComponentKeyBase(type, weights, before, start, goal),
      _prefRank(prefRank)
{}

//======================================================================
bool RouteComponentKeyHierarchy::isEqualKey
(RouteComponentKeyBase* anotherKey) const
{
    if (!RouteComponentKeyBase::isEqualKey(anotherKey))
    {
        return false;
    }
    
    RouteComponentKeyHierarchy* anotherKeyCast
        = dynamic_cast<RouteComponentKeyHierarchy*>(anotherKey);
    if (!anotherKeyCast
        || anotherKeyCast->prefRank() != _prefRank)
    {
        return false;
    }

    return true;
}

//======================================================================
void RouteComponentKeyHierarchy::print(ostream& out) const
{
    out << _type << "," << _prefRank;
    for (unsigned int i=0; i<VEHICLE_ROUTING_PARAMETER_SIZE; i++)
    {
        out << "," << _weights[i];
    }
    out << ","
        << _before->id() << ","
        << _start->id()  << ","
        << _goal->id();
}
