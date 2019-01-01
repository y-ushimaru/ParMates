/* **************************************************
 * Copyright (C) 2014 ADVENTURE Project
 * All Rights Reserved
 **************************************************** */
#include "DetectorUnit.h"
#include "Conf.h"
#include "Section.h"
#include "Lane.h"
#include "Vehicle.h"
#include "VehicleFamily.h"
#include "LaneBundle.h"
#include <map>
#include <cassert>

using namespace std;

//======================================================================
DetectorUnit::DetectorUnit(const string& id)
{
    _id = id;
    clearPassedVehicleData();
    clearStatVehicleData();
    _unitSvd.totalAllTrucks = 0;
    _unitSvd.totalAllPassengers = 0;
}

//======================================================================
DetectorUnit::~DetectorUnit()
{
    // _detectorsの消去
    vector<Detector*>::iterator itd = _detectors.begin();
    while (itd != _detectors.end())
    {
        delete *itd;
        itd++;
    }
    _detectors.clear();
}

//======================================================================
const string& DetectorUnit::id() const
{
    return _id;
}

//======================================================================
Section* DetectorUnit::section() const
{
    return _section;
}

//======================================================================
bool DetectorUnit::isUp() const
{
    return _isUp;
}

//======================================================================
double DetectorUnit::length() const
{
    return _length;
}

//======================================================================
ulint DetectorUnit::interval() const
{
    return _interval;
}

//======================================================================
const vector<Detector*>* DetectorUnit::detectors() const
{
    return &_detectors;
}

//======================================================================
void DetectorUnit::setPosition(Section* section,
			       bool isUp,
			       double length,
			       ulint interval)
{
    _section = section;
    _isUp = isUp;
    _length = length;
    _interval = interval;

    // 観測対象となるレーンを取得する
    const RMAPLAN* lanes = section->lanes();
    CITRMAPLAN itl;

    for (itl=lanes->begin(); itl!=lanes->end(); itl++)
    {
        bool setFlag = false;
        double lengthOnLane = 0;
        // レーンの方向が一致しなければ観測しない
        if (section->isUp((*itl).second)!=isUp)
            continue;

        if (length>=0)
        {
            // lengthが正の場合は始点からの距離が定められている．
            // laneの始点から単路の後方端までの距離がlengthより小さく，
            // laneの終点から単路の後方端までの距離がlengthより大きいレーンが
            // 対象となる
            if (section->lengthFromPrevious((*itl).second, 0)<length
                && section->lengthFromPrevious
                ((*itl).second,
                 (*itl).second->length())>length)
            {
                setFlag = true;
                lengthOnLane
                    = length-section->lengthFromPrevious((*itl).second, 0);

            }
        }
        else{
            // lengthが負の場合は終点からの距離が定められている
            // laneの始点から単路の前方端までの距離が-lengthより大きく，
            // laneの終点から単路の前方端までの距離が-lengthより小さいレーンが
            // 対象となる
            if (section->lengthToNext((*itl).second, 0)>length*-1
                && section->lengthToNext
                ((*itl).second,
                 (*itl).second->length())<length*-1)
            {
                setFlag = true;
                lengthOnLane
                    = section->lengthToNext((*itl).second, 0)+length;
            }
        }

        if (setFlag)
        {
            // 感知器の生成
            Detector* tmpDetector = new Detector();
            tmpDetector->setLane(_section, (*itl).second, lengthOnLane);
            _detectors.push_back(tmpDetector);
        }
    }

    // 統計データを準備する
    _unitSvd.numPassengers.resize(_detectors.size());
    _unitSvd.numTrucks.resize(_detectors.size());
    _unitSvd.laneIds.resize(_detectors.size());

    for (unsigned int i=0; i<_unitSvd.laneIds.size(); i++)
    {
        _unitSvd.laneIds[i] = _detectors[i]->lane()->id();
    }
}

//======================================================================
void DetectorUnit::monitorLanes()
{
    for (unsigned int i=0; i<_detectors.size(); i++)
    {
        // 個々の感知器の処理
        vector<Detector::PassedVehicleData> result_pvd;
        if (_detectors[i]->getPassedVehicleData(&result_pvd))
        {
            _unitPvd.insert(_unitPvd.end(),
                            result_pvd.begin(),
                            result_pvd.end());
            for (unsigned int j=0; j<result_pvd.size(); j++)
            {
                if (VehicleFamily::isTruck(result_pvd[j].vehicle->type()))
                { 
                    // 大型車
                    _unitSvd.numTrucks[i]++;
                    _unitSvd.sumTrucks++;
                    _unitSvd.totalAllTrucks++;
                }
                else
                {
                    // 乗用車
                    _unitSvd.numPassengers[i]++;
                    _unitSvd.sumPassengers++;
                    _unitSvd.totalAllPassengers++;
                }
            }
        }
    }
}

//======================================================================
bool DetectorUnit::getPassedVehicleData
(vector<Detector::PassedVehicleData>* result) const
{
    *result = _unitPvd;
    return !_unitPvd.empty();
}

//======================================================================
void DetectorUnit::clearPassedVehicleData()
{
    _unitPvd.clear();
}

//======================================================================
void DetectorUnit::clearStatVehicleData()
{
    /*
     * タイムステップの最後で処理するため，
     * beginTimeには次のタイムステップの値を入れる
     */
    _unitSvd.beginTime
        = TimeManager::time() + TimeManager::unit();
    _unitSvd.sumPassengers
        = 0;
    for (unsigned int i=0; i<_unitSvd.numPassengers.size(); i++)
    {
        _unitSvd.numPassengers[i] = 0;
    }
    _unitSvd.sumTrucks = 0;
    for (unsigned int i=0; i<_unitSvd.numTrucks.size(); i++)
    {
        _unitSvd.numTrucks[i]=0;
    }
}

//======================================================================
DetectorUnit::StatVehicleData DetectorUnit::statVehicleData()
{
    return _unitSvd;
}

//======================================================================
void DetectorUnit::print() const
{
    cout << _id << ": "
         << "section=" << _section->id()
         << ", interval=" << _interval << endl;
    for (unsigned int i=0; i<_detectors.size(); i++)
    {
        cout << " " << i
             << ": lane=" << _detectors[i]->lane()->id()
             << ", length=" << _detectors[i]->length() << endl;
    }
}
