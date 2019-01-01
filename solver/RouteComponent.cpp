/* **************************************************
 * Copyright (C) 2014 ADVENTURE Project
 * All Rights Reserved
 **************************************************** */
#include "RouteComponent.h"
#include "RouteComponentKeyBase.h"
#include "Intersection.h"
#include <iostream>
#include <algorithm>
#include <cmath>
#include <typeinfo>

using namespace std;

//======================================================================
RouteComponent::RouteComponent()
{
    _count = 0;
    _key = NULL;
    _route.clear();
    _isStatic = false;
}

//======================================================================
RouteComponent::~RouteComponent()
{
    if (_key)
    {
        delete _key;
    }
}

//======================================================================
void RouteComponent::setComponent
(const RouteComponentKeyBase* key,
 const vector<const Intersection*>* route)
{
    _key = key;
    if (_key->weights(VEHICLE_ROUTING_PARAMETER_OF_TIME)<1.0e-6)
    {
        _isStatic = true;
    }
    
    _route.resize(route->size());
    copy(route->begin(), route->end(), _route.begin());
}

//======================================================================
bool RouteComponent::hasEqualKey
(RouteComponentKeyBase* key) const
{
    if (typeid(*_key)!=typeid(*key))
    {
        return false;
    }
    return _key->isEqualKey(key);
}

//======================================================================
bool RouteComponent::isEqualComponent
(RouteComponentKeyBase* key,
 const vector<const Intersection*>* route) const
{
    if (!hasEqualKey(key))
    {
        return false;
    }
    if (_route.size() != route->size())
    {
        return false;
    }
    for (unsigned int i=0; i<_route.size(); i++)
    {
        if (_route[i]!=(*route)[i])
        {
            return false;
        }
    }
    return true;
}

//======================================================================
void RouteComponent::print(ostream& out) const
{
    out << _count << ",";
    _key->print(out);
    for (unsigned int i=0; i<_route.size(); i++)
    {
        out << "," << _route[i]->id();
    }
    out << endl;
}
