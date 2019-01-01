/* **************************************************
 * Copyright (C) 2014 ADVENTURE Project
 * All Rights Reserved
 **************************************************** */
#include "Detector.h"
#include "Section.h"
#include "Lane.h"
#include "AmuVector.h"
#include "RoadOccupant.h"
#include "Vehicle.h"
#include <cassert>

using namespace std;

//======================================================================
Detector::Detector()
{
    _section = NULL;
    _lane = NULL;
    _length = 0;
}

//======================================================================
Detector::~Detector(){}


//======================================================================
void Detector::setLane(Section* section, Lane* lane, double length)
{
    _section = section;
    _lane = lane;
    _length = length;
}

//======================================================================
bool Detector::getPassedVehicleData(vector<PassedVehicleData>* result)
{
    assert(_lane);
    _detectedVehicles.clear();
    bool detectFlag = true;

    // 観測対象のレーンに存在するエージェントのうち，
    // 距離_lengthよりも先にいるものを感知器に近い順に取得する
    RoadOccupant* agent = _lane->frontAgent(_length);
    Vehicle* vehicle;
    for (agent = _lane->frontAgent(_length);
         agent != NULL;
         agent = _lane->frontAgent(agent))
    {
        vehicle = dynamic_cast<Vehicle*>(agent);
        if (vehicle!=NULL)
        {
            if (vehicle->oldLength()<_length)
            {
                PassedVehicleData pvd;
                clearPassedVehicleData(&pvd);
                pvd.lane =_lane;
                pvd.vehicle = vehicle;
                _detectedVehicles.push_back(pvd);
            }
            else
            {
                detectFlag = false;
                break;
            }
        }
    }

    // detectFlagがfalseであれば，最寄りの車両が検知済みである．
    // detectFlagがtrueであればもうひとつ先のレーンを見る必要がある．
    // 例えばレーンの先端に検知器が付いていて，
    // frontAgentが必ずNULLになる場合など．
    if (detectFlag)
    {
        // 先のレーンを取得する
        for (unsigned int i=0; i<_lane->nextLanes()->size(); i++)
        {
            Lane* nextLane = _lane->nextLane(i);
            for (agent = nextLane->tailAgent();
                 agent!=NULL;
                 agent = nextLane->frontAgent(agent))
            {
                vehicle = dynamic_cast<Vehicle*>(agent);
                if (vehicle!=NULL)
                {
                    double oldLength = vehicle->oldLength()+_lane->length();
                    if (oldLength<_length)
                    {
                        PassedVehicleData pvd;
                        clearPassedVehicleData(&pvd);
                        pvd.lane = _lane;
                        pvd.vehicle = vehicle;
                        _detectedVehicles.push_back(pvd);
                    }
                    else
                    {
                        break;
                    }
                }
                agent = nextLane->frontAgent(agent);
            }
        }
    }

    *result = _detectedVehicles;
    return !_detectedVehicles.empty();
}

//======================================================================
Lane* Detector::lane() const
{
    return _lane;
}

//======================================================================
double Detector::length() const
{
    return _length;
}

//======================================================================
double Detector::x() const
{
    AmuVector vec = _lane->directionVector();
    vec.normalize();

    return (_lane->beginConnector()->x()+vec.x()*_length);
}
//======================================================================
double Detector::y() const
{
    AmuVector vec = _lane->directionVector();
    vec.normalize();

    return (_lane->beginConnector()->y()+vec.y()*_length);
}
//======================================================================
double Detector::z() const
{
    AmuVector vec = _lane->directionVector();
    vec.normalize();

    return (_lane->beginConnector()->z()+vec.z()*_length);
}

//======================================================================
void Detector::clearPassedVehicleData(PassedVehicleData* pvd)
{
    pvd->lane = NULL;
    pvd->vehicle = NULL;
}
