/* **************************************************
 * Copyright (C) 2014 ADVENTURE Project
 * All Rights Reserved
 **************************************************** */
#include "Conf.h"
#include "ObjManager.h"
#include "Connector.h"
#include "DetectorUnit.h"
#include "AmuConverter.h"
#include "AmuPoint.h"
#include "Vehicle.h"
#include <algorithm>
#include <cassert>

using namespace std;

vector<Connector*>    ObjManager::_connectors;
vector<DetectorUnit*> ObjManager::_detectorUnits;
vector<Vehicle*>      ObjManager::_vehicles;

int ObjManager::numVehicle = 0;

//======================================================================
void ObjManager::deleteAll()
{
    // コネクタを消去する
    vector<Connector*>::iterator itConnectors = _connectors.begin();
    while (itConnectors != _connectors.end())
    {
        delete (*itConnectors);
        itConnectors++;
    }
    _connectors.clear();

    // 車両感知器ユニットを消去する
    vector<DetectorUnit*>::iterator itDetectorUnits
        = _detectorUnits.begin();
    while (itDetectorUnits != _detectorUnits.end())
    {
        delete *itDetectorUnits;
        itDetectorUnits++;
    }
    _detectorUnits.clear();

    // エージェントを消去する
    deleteAllAgents();
}

//======================================================================
void ObjManager::deleteAllAgents()
{
    // 車両を消去する
    deleteAllVehicles();

#ifdef INCLUDE_PEDESTRIANS
    // 歩行者を消去する
    deleteAllPedestrians();
#endif //INCLUDE_PEDESTRIANS
}

//======================================================================
vector<Connector*>* ObjManager::connectors()
{
    return &_connectors;
}

//======================================================================
Connector* ObjManager::createConnector(double x, double y, double z)
{
    AmuPoint pt(x, y, z);
    Connector* ptConnector =
        new Connector(static_cast<int>(_connectors.size()), pt);
    _connectors.push_back(ptConnector);
    return ptConnector;
}

//======================================================================
vector<DetectorUnit*>* ObjManager::detectorUnits()
{
    return &_detectorUnits;
}

//======================================================================
DetectorUnit* ObjManager::detectorUnit(const string& id)
{
    for (unsigned int i=0; i<_detectorUnits.size(); i++)
    {
        if (_detectorUnits[i]->id().compare(id)==0)
        {
            return _detectorUnits[i];
        }
    }
    return NULL;
}

//======================================================================
bool ObjManager::addDetectorUnit(DetectorUnit* unit)
{
    assert(unit);
    bool flag = false;

    // 既に同じ感知器ユニットが生成されていないかチェックする
    if (detectorUnit(unit->id())==NULL)
    {
        flag = true;
    }
    if (flag)
    {
        _detectorUnits.push_back(unit);
    }
    return flag;
}

//======================================================================
vector<Vehicle*>* ObjManager::vehicles()
{
    return &_vehicles;
}

//======================================================================
Vehicle* ObjManager::vehicle(const string& id)
{
    for (unsigned int i=0; i<_vehicles.size(); i++)
    {
        if (_vehicles[i]->id().compare(id)==0)
        {
            return _vehicles[i];
        }
    }
    return NULL;
}

//======================================================================
Vehicle* ObjManager::createVehicle()
{
    Vehicle* tmpVehicle;
    tmpVehicle = new Vehicle();
    return tmpVehicle;
}

//======================================================================
bool ObjManager::addVehicleToReal(Vehicle* rv)
{
    assert(rv);
    bool flag = false;

    assert(rv->id().empty());

#ifdef _OPENMP
#pragma omp critical (addWatchedVehicle)
    {
#endif //_OPENMP
        rv->setId(AmuConverter::itos(numVehicle, NUM_FIGURE_FOR_VEHICLE));
        numVehicle++;

        // 既に同じ車両が生成されていないかチェックする
        if (vehicle(rv->id())==NULL)
        {
            flag = true;
        }
        if (flag)
        {
            _vehicles.push_back(rv);
        }

#ifdef _OPENMP
    }
#endif //_OPENMP

    return flag;
}

//======================================================================
void ObjManager::deleteAllVehicles()
{
    for (unsigned int i=0; i<_vehicles.size(); i++)
    {
        delete _vehicles[i];
    }
    _vehicles.clear();
    numVehicle = 0;
}

//======================================================================
void ObjManager::deleteVehicle(Vehicle* vehicle)
{
    vector<Vehicle*>::iterator itv = find(_vehicles.begin(),
                                          _vehicles.end(),
                                          vehicle);
    if (itv!=_vehicles.end())
    {
        delete *itv;
        _vehicles.erase(itv);
    }
}

//======================================================================
void ObjManager::printNumVehicle()
{
  cout << numVehicle << endl;
}
