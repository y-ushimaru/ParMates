/* **************************************************
 * Copyright (C) 2014 ADVENTURE Project
 * All Rights Reserved
 **************************************************** */
#include "LaneBundle.h"
#include "RoadMap.h"
#include "RoadEntity.h"
#include "Lane.h"
#include "Connector.h"
#include "AmuPoint.h"
#include "AmuLineSegment.h"
#include "RoadOccupant.h"
#include "Vehicle.h"
#include "ObjManager.h"
#include <algorithm>
#include <cassert>
#include <functional>
#include <string>

using namespace std;

//======================================================================
LaneBundle::LaneBundle(const string& id, RoadMap* parent)
{
    _id = id;
    _parent = parent;
    _isUsed = false;
}

//======================================================================
LaneBundle::~LaneBundle(){}

//======================================================================
int LaneBundle::numVertices() const
{
    return static_cast<signed int>(_vertices.size());
}

//======================================================================
const AmuPoint LaneBundle::vertex(int i) const
{
    assert(0<=i && i<static_cast<signed int>(_vertices.size()));
    return _vertices[i];
}

//======================================================================
void LaneBundle::addVertex(AmuPoint vertex)
{
    _vertices.push_back(vertex);
}

//======================================================================
const AmuLineSegment LaneBundle::edge(int i) const
{
    return AmuLineSegment(vertex(i), vertex((i+1)%numVertices()));
}

//======================================================================
const string& LaneBundle::id() const
{
    return _id;
}

//======================================================================
double LaneBundle::lengthToNext(const Lane* lane, double length) const
{
    assert(isMyLane(lane));

    double result = 0;
    const Lane* target = lane;
    result += target->length()-length;
    while (isNextLaneMine(target))
    {
        target = target->nextStraightLane();
        result += target->length();
    }
    return result;
}

//======================================================================
double LaneBundle::lengthFromPrevious(const Lane* lane, double length) const
{
    assert(isMyLane(lane));

    double result = 0;
    const Lane* target = lane;
    result += length;
    while (isPreviousLaneMine(target))
    {
        target = target->previousStraightLane();
        result += target->length();
    }
    return result;
}

//======================================================================
const RMAPCON* LaneBundle::innerConnectors() const
{
    return &_innerConnectors;
}

//======================================================================
const Connector* LaneBundle::connector(const string& id) const
{
    CITRMAPCON itc = _innerConnectors.find(id);
    if (itc!=_innerConnectors.end())
    {
        return (*itc).second;
    }
    else
    {
        cerr << "no lane bundle connector error " << _id
             << " " << id << endl;
        return 0;
    }
}

//======================================================================
const RMAPENT*LaneBundle::entities() const
{
    return &_entities;
}

//======================================================================
bool LaneBundle::isMyRoadEntity(const RoadEntity* entity) const
{
    bool flag = false;
    CITRMAPENT ite;
    for (ite=_entities.begin(); ite!=_entities.end(); ite++)
    {
        if ((*ite).second==entity)
        {
            flag = true;
            break;
        }
    }
    return flag;
}


//======================================================================
RoadEntity* LaneBundle::streetEntity()
{
    return _entities["00"];
}

//======================================================================
const RMAPLAN* LaneBundle::lanes() const
{
    return &_lanes;
}

//======================================================================
bool LaneBundle::isMyLane(const Lane* lane) const
{
    if (!lane)
    {
        cout << __FUNCTION__ << ": lane is null." << _id << endl;
        return false;
    }

    if (lane->parent()==this)
    {
        return true;
    }
    else
    {
        return false;
    }
}
//======================================================================
bool LaneBundle::isNextLaneMine(const Lane* lane) const
{
    bool flag = false;
    if (isMyLane(lane))
    {
        if (isMyLane(lane->nextLane(0)))
        {
            flag = true;
        }
    }
    return flag;
}

//======================================================================
bool LaneBundle::isPreviousLaneMine(const Lane* lane) const
{
    bool flag = false;
    if (isMyLane(lane))
    {
        if (isMyLane(lane->previousLane(0)))
        {
            flag = true;
        }
    }
    return flag;
}

//======================================================================
vector<Lane*> LaneBundle::lanesFromConnector(const Connector* connector) const
{
    assert(connector);
    vector<Lane*> lanes;
    CITRMAPLAN itl;
    for (itl=_lanes.begin(); itl!=_lanes.end(); itl++)
    {
        if ((*itl).second->beginConnector()==connector)
        {
            lanes.push_back((*itl).second);
        }
    }
    return lanes;
}

//======================================================================
vector<Lane*> LaneBundle::lanesToConnector
(const Connector* connector) const
{
    assert(connector);
    vector<Lane*> lanes;
    CITRMAPLAN itl;
    for (itl=_lanes.begin(); itl!=_lanes.end(); itl++)
    {
        if ((*itl).second->endConnector()==connector)
        {
            lanes.push_back((*itl).second);
        }
    }
    return lanes;
}

//======================================================================
void LaneBundle::addUsedLane(Lane* usedLane)
{
#ifdef _OPENMP
#pragma omp critical (addUsedLane)
    {
#endif //_OPENMP
        _usedLanes.push_back(usedLane);
        if (!_isUsed)
        {
            _isUsed = true;
            _parent->addUsedLaneBundle(this);
        }
#ifdef _OPENMP
    }
#endif //_OPENMP

}

//======================================================================
void LaneBundle::renewAgentLine()
{
    for (unsigned int i=0; i<_usedLanes.size(); i++)
    {
        _usedLanes[i]->renewAgentLine();
    }
    _isUsed = false;
    _usedLanes.clear();
}

//======================================================================
void LaneBundle::renewAgentLineTmp()
{
    for (unsigned int i=0; i<_usedLanes.size(); i++)
    {
        _usedLanes[i]->renewAgentLineTmp();
    }
}

//======================================================================
bool LaneBundle::isHeadAgent(RoadOccupant* agent, Lane* lane) const
{
    assert(isMyLane(lane));
    bool result = false;

    // レーンの中で自分が先頭である
    if (lane->headAgent()==agent)
    {
        // 先頭レーンであればtrue
        if (isNextLaneMine(lane)==false)
        {
            result = true;
        }
        else
        {
            result = true;
            Lane* nextLane = lane;
            while (isNextLaneMine(nextLane))
            {
                nextLane = lane->nextStraightLane();
                if (nextLane->headAgent()!=NULL)
                {
                    result = false;
                    break;
                }
            }
        }
    }
    return result;
}

//======================================================================
list<Vehicle*>* LaneBundle::watchedVehicles()
{
    return &_watchedVehicles;
}

//======================================================================
void LaneBundle::addWatchedVehicle(Vehicle* vehicle)
{
    assert(vehicle != NULL);

#ifdef _OPENMP
#pragma omp critical (addWatchedVehicle)
    {
#endif //_OPENMP
        // 重複を認めない
        list<Vehicle*>::iterator itv = find(_watchedVehicles.begin(),
                                            _watchedVehicles.end(),
                                            vehicle);
        if (itv==_watchedVehicles.end())
        {
            _watchedVehicles.push_back(vehicle);
        }
        else
        {
            cerr << "_watchedVehicle has already " << vehicle->id() << endl;
        }
#ifdef _OPENMP
    }
#endif //_OPENMP
}

//======================================================================
void LaneBundle::eraseWatchedVehicle(Vehicle* vehicle)
{
    assert(vehicle != NULL);

#ifdef _OPENMP
#pragma omp critical (eraseWatchedVehicle)
    {
#endif //_OPENMP

    _watchedVehicles.remove(vehicle);

#ifdef _OPENMP
    }
#endif //_OPENMP
}
//======================================================================
void LaneBundle::deleteAgent()
{
  CITRMAPLAN itl;

  for (itl=_lanes.begin(); itl!=_lanes.end(); itl++)
  {
    vector<RoadOccupant*>* agentsOfLane = (*itl).second->agents();
    if (!agentsOfLane->empty())
    {
      (*itl).second->setUsed();
    }
    for (unsigned int i=0; i<agentsOfLane->size(); i++)
    {
      if (dynamic_cast<Vehicle*>((*agentsOfLane)[i]))
      {
          ObjManager::deleteVehicle(
            dynamic_cast<Vehicle*>((*agentsOfLane)[i]));
      }
    }
    (*itl).second->deleteAgent();
  }
}
