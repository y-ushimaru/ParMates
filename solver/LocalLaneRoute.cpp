/* **************************************************
 * Copyright (C) 2014 ADVENTURE Project
 * All Rights Reserved
 **************************************************** */
#include "LocalLaneRoute.h"
#include "Lane.h"
#include <iostream>
#include <algorithm>

using namespace std;

//======================================================================
LocalLaneRoute::LocalLaneRoute() {}

//======================================================================
LocalLaneRoute::~LocalLaneRoute() {}

//======================================================================
const vector<Lane*>& LocalLaneRoute::localRoute() const
{
    return _localRoute;
}

//======================================================================
void LocalLaneRoute::setLocalRoute(const std::vector<Lane*>& localRoute)
{
    _localRoute = localRoute;
}

//======================================================================
void LocalLaneRoute::setDesiredLocalRoute
(const vector<Lane*>& desiredLocarRoute)
{
    _desiredLocalRoute = desiredLocarRoute;
}

//======================================================================
void LocalLaneRoute::clear()
{
    _localRoute.clear();
    _desiredLocalRoute.clear();
}

//======================================================================
Lane* LocalLaneRoute::next(const Lane * presentLane) const
{
    Lane* result = NULL;
    vector<Lane*>::const_iterator where;

    where = find(_localRoute.begin(), _localRoute.end(), presentLane);
    if (where != _localRoute.end())
    {
        ++where;
        if (where != _localRoute.end())
        {
            result = *where;
        }
    }
    return result;
}

//======================================================================
Lane* LocalLaneRoute::desiredNext(const Lane * presentLane) const
{
    Lane* result = NULL;
    vector<Lane*>::const_iterator where;

    where = find(_desiredLocalRoute.begin(),
                 _desiredLocalRoute.end(),
                 presentLane);
    if(where != _desiredLocalRoute.end())
    {
        ++where;
        if(where != _desiredLocalRoute.end())
        {
            result = *where;
        }
    }
    return result;
}

//======================================================================
Lane* LocalLaneRoute::previous(const Lane * presentLane) const
{
    Lane* result = NULL;
    vector<Lane*>::const_reverse_iterator where
        = find(_localRoute.rbegin(),
               _localRoute.rend(),
               presentLane);
    if(where != _localRoute.rend())
    {
        ++where;
        if(where != _localRoute.rend())
        {
            result = *where;
        }
    }
    return result;
}

//======================================================================
Lane* LocalLaneRoute::begin() const
{
    if (_localRoute.empty())
    {
        return NULL;
    }
    else
    {
        return _localRoute.front();
    }
}

//======================================================================
Lane* LocalLaneRoute::desiredBegin() const
{
    if (_desiredLocalRoute.empty())
    {
        return NULL;
    }
    else
    {
        return _desiredLocalRoute.front();
    }
}

//======================================================================
Lane* LocalLaneRoute::last() const
{
    if (_localRoute.empty())
    {
        return NULL;
    }
    else
    {
        return _localRoute.back();
    }
}

//======================================================================
const vector<Lane*>* LocalLaneRoute::lanesInIntersection() const
{
    return &_lanesInIntersection;
}

//======================================================================
void LocalLaneRoute::print() const
{
    vector<Lane*>::const_iterator where;
    cout << "Actual Local Route:" << endl;
    for(where=_localRoute.begin(); where != _localRoute.end(); where++)
    {
        clog << "\t" << (*where)->id() << endl;
    }
    if (!_desiredLocalRoute.empty())
    {
        cout << "Desired Local Route:" << endl;
        for(where=_desiredLocalRoute.begin();
            where != _desiredLocalRoute.end();
            where++)
        {
            cout << "\t" << (*where)->id() << endl;
        }
    }
    else
    {
        cout << "Desired Local Route equals to Actual Route." << endl;
    }
    cout << "MainLane: ";
    if (_mainLaneInIntersection)
    {
        cout << _mainLaneInIntersection->id() << endl;
    }
    else
    {
        cout << "NULL" << endl;
    }
    cout << "Lanes in intersection:" << endl;
    for (where=_lanesInIntersection.begin();
         where!=_lanesInIntersection.end();
         where++)
    {
        cout << "\t" << (*where)->id() << endl;
    }
    cout << "Turning : ";
    switch (_turning)
    {
    case RD_NONE:
        cout << "NONE" << endl;
        break;
    case RD_BACK:
        cout << "BACK" << endl;
        break;
    case RD_STRAIGHT:
        cout << "STRAIGHT" << endl;
        break;
    case RD_LEFT:
        cout << "LEFT" << endl;
        break;
    case RD_RIGHT:
        cout << "RIGHT" << endl;
        break;
    default:
        cout << _turning << endl;
    }
}

//======================================================================
void LocalLaneRoute::addLocalRoute(Lane* lane)
{
  _localRoute.push_back(lane);
}

//======================================================================
void LocalLaneRoute::addLanesInIntersection(Lane* lane)
{
  _lanesInIntersection.push_back(lane);
}
