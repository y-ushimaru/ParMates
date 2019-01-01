/* **************************************************
 * Copyright (C) 2014 ADVENTURE Project
 * All Rights Reserved
 **************************************************** */
#include "VehicleLaneShifter.h"
#include "Vehicle.h"
#include "VirtualLeader.h"
#include "RoadOccupant.h"
#include "Intersection.h"
#include "Lane.h"
#include "Section.h"
#include "LaneBundle.h"
#include "GVManager.h"
#include "Conf.h"
#include <algorithm>
#include <cmath>
#include <list>
#include <cassert>

using namespace std;

//======================================================================
VehicleLaneShifter::VehicleLaneShifter()
    : _lengthTo(0), _errorTo(0), _isActive(false), _canShift(false)
{
    _laneTo = NULL;
    _vehicle = NULL;
}

//======================================================================
void VehicleLaneShifter::setVehicle(Vehicle* vehicle)
{
    assert(vehicle != NULL);
    _vehicle = vehicle;
}

//======================================================================
bool VehicleLaneShifter::isActive() const
{
    return _isActive;
}

//======================================================================
bool VehicleLaneShifter::canShift() const
{
    return _canShift;
}

//======================================================================
Lane* VehicleLaneShifter::laneTo() const
{
    return _laneTo;
}

//======================================================================
double VehicleLaneShifter::lengthTo() const
{
    return _lengthTo;
}

//======================================================================
void VehicleLaneShifter::checkLaneSpace()
{
    _canShift = false;

    // 車線変更は単路内のみ
    assert(_vehicle->_section);

    // 隣のレーンと位置を取得する
    Lane* laneTo = NULL;
    double lengthTo;
    int shiftDirection = _vehicle->_localRouter.shiftDirection();
    _isLeft = (shiftDirection == 1);
    _vehicle->_lane->getSideLaneLength(_vehicle->_length, _isLeft,
                                       &laneTo, &lengthTo);

    if (!laneTo)
    {
        return;
    }

    //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    // 交差点手前10mは車線変更禁止とする．法律的には30mのはずだが...
    // MATESでは車線変更中にレーンを移ることを許さないための処置
    if(laneTo->length() - lengthTo < 10)
    {
        return;
    }

    double diff;

    //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    // 先行車との車間距離による判定
    RoadOccupant* nextLeader = NULL;
    laneTo->getFrontAgentFar(lengthTo, 30, &nextLeader, &diff);
    if (nextLeader)
    {
        diff -= (nextLeader->bodyLength()/2 + _vehicle->_bodyLength/2);
    }

    // 車間距離による判定
    if (nextLeader
        && (diff < (_vehicle->_velocity - nextLeader->velocity())*2000
            || diff < _vehicle->_bodyLength/2))
    {
        // nextLeaderに2000msec以内に追いつくようであれば
        // 現時点で車線変更不可

        // 減速を試みる
        if (_vehicle->_velocity
            >GVManager::getNumeric("VELOCITY_CRAWL")/60.0/60.0)
        {
#ifndef VL_DEBUG
            VirtualLeader* leader =
                new VirtualLeader(diff,
                                  GVManager::getNumeric("VELOCITY_CRAWL"));
#else //VL_DEBUG
            VirtualLeader* leader =
                new VirtualLeader(diff,
                                  GVManager::getNumeric("VELOCITY_CRAWL"),
                                  "DECTOSHIFT_FRONT:"+nextLeader->id());
#endif //VL_DEBUG
            _vehicle->_leaders.push_back(leader);
        }
        return;
    }

    //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    // 後続車との車間距離による判定
    RoadOccupant* nextFollower = NULL;
    laneTo->getFollowingAgentFar(lengthTo, 30, &nextFollower, &diff);
    if (nextFollower)
    {
        diff -= (_vehicle->_bodyLength/2 + nextFollower->bodyLength()/2);
    }

    // 車間距離による判定
    if (nextFollower
        && (diff < (nextFollower->velocity()-_vehicle->_velocity)*2000
            || diff < _vehicle->_bodyLength/2))
    {
        // nextFollowerに2000msec以内に追いつかれるようであれば
        // 現時点で車線変更不可

        // 何もせず通りすぎるのを待つ
        /**
         * 複数の選択肢から最小の加速度を選択するため，加速はできない
         */

        return;
    }

    //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    // 車線変更に要する時間を推定
    /**
     * @todo 車線変更に要する時間の推定は正確ではないので修正が必要
     */
    double timeToSideLane
        = _vehicle->_section->laneWidth()
        / (GVManager::getNumeric("ERROR_VELOCITY")/60.0/60.0);

    // 次の交差点に到達するまでに車線変更が完了しない場合は減速
    if (_vehicle->_velocity > 10.0/60.0/60.0
        && (_vehicle->_section->lengthToNext(laneTo, lengthTo)
            < _vehicle->_velocity * timeToSideLane))
    {
#ifndef VL_DEBUG
        VirtualLeader* leader
            = new VirtualLeader
            (_vehicle->_section->lengthToNext(laneTo, lengthTo),
             10.0/60/60);
#else
        VirtualLeader* leader
            = new VirtualLeader
            (_vehicle->_section->lengthToNext(laneTo, lengthTo),
             10.0/60/60,
             "DECTOSHIFT_DIST:"+_vehicle->_section->id());
#endif
        _vehicle->_leaders.push_back(leader);

        return;
    }

    //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    // ここまできたら，先行車・後続車とも十分に車間がとれている
    _canShift = true;
}

//======================================================================
void VehicleLaneShifter::searchNextLeader()
{
    // 隣のレーンと位置を取得する
    RoadOccupant* nextLeader = NULL;
    double diff;
    _laneTo->getFrontAgentFar(_lengthTo, 30, &nextLeader, &diff);
    //_searchNextLeader(_laneTo, _lengthTo, 30, &nextLeader, &diff);

    if (nextLeader)
    {
        diff -= (nextLeader->bodyLength()/2
                 + _vehicle->_bodyLength/2);
#ifndef VL_DEBUG
        VirtualLeader* leader
            = new VirtualLeader(diff, nextLeader->velocity());
#else //VL_DEBUG
        VirtualLeader* leader
            = new VirtualLeader(diff, nextLeader->velocity(),
                                "SHIFTFRONT:"+nextLeader->id());
#endif
        _vehicle->_leaders.push_back(leader);
    }
}

//======================================================================
void VehicleLaneShifter::beginShift()
{
    assert(_vehicle->_section);

    int shiftDirection = _vehicle->_localRouter.shiftDirection();
    _isLeft = (shiftDirection == 1);
    _vehicle->_lane->getSideLaneLength(_vehicle->_length, _isLeft,
                                       &_laneTo, &_lengthTo);
    if (!_laneTo)
    {
        // cerr << "WARNING: next lane not found while lane shifting."
        //      << endl
        //      << "vehicle:" << _vehicle->_id
        //      << " in section:" << _vehicle->_section->id() << endl;
    }
    else
    {
        _vehicle->notify();

        _isActive = true;
    }
}

//======================================================================
void VehicleLaneShifter::proceedShift()
{
    // _laneTo, _lengthToの更新
    _vehicle->_lane->getSideLaneLength(_vehicle->_length, _isLeft,
                                       &_laneTo, &_lengthTo);

    if (!_laneTo)
    {
        cerr << "WARNING: next lane lost while lane shifting."
             << endl
             << "vehicle:" << _vehicle->_id
             << " in section:" << _vehicle->_section->id() << endl;
        abortShift();
    }
}

//======================================================================
void VehicleLaneShifter::endShift()
{
    assert(_isActive);

    _vehicle->_lane->setUsed();
    _laneTo->setUsed();

    if(_vehicle->_intersection)
    {
        _vehicle->_setLane(_vehicle->_intersection,
                           _laneTo,
                           _lengthTo);
    }
    else
    {
        _vehicle->_setLane(_vehicle->_section,
                           _laneTo,
                           _lengthTo);
    }
    _vehicle->_length = _lengthTo;
    _vehicle->_error = 0;

    _laneTo = NULL;
    _lengthTo = 0;

    _vehicle->unnotify();;
    _isActive = false;
    _canShift = false;
}

//======================================================================
void VehicleLaneShifter::abortShift()
{
    assert(_isActive);
    _isActive = false;
    _canShift = false;

    _vehicle->_error = 0;

    _laneTo = NULL;
    _lengthTo = 0;

    _vehicle->unnotify();
}

//======================================================================
void VehicleLaneShifter::searchInterruption() const
{
    assert(_vehicle->_section);

    // 車線変更中の車両を取得する
    list<Vehicle*>* notifyVehicles
        = _vehicle->_section->watchedVehicles();
    list<Vehicle*>::const_iterator itv;
    Vehicle* notifyVehicle = NULL;

    for(itv = notifyVehicles->begin();
        itv != notifyVehicles->end();
        itv++)
    {
        VehicleLaneShifter anotherLaneShifter = (*itv)->laneShifter();

        if (anotherLaneShifter.isActive()
            && anotherLaneShifter.laneTo() == _vehicle->_lane
            && anotherLaneShifter.lengthTo() > _vehicle->_length)
        {
            if(!notifyVehicle
               || notifyVehicle->length() > (*itv)->length())
            {
                notifyVehicle = (*itv);
            }
        }
    }

    if (notifyVehicle)
    {
        double diff
            = notifyVehicle->laneShifter().lengthTo()
            - _vehicle->length();
#ifndef VL_DEBUG
        VirtualLeader* leader
            = new VirtualLeader(diff, notifyVehicle->velocity());
#else
        VirtualLeader* leader
            = new VirtualLeader(diff, notifyVehicle->velocity(),
                                "INTERRUPT:"+notifyVehicle->id());
#endif
        _vehicle->_leaders.push_back(leader);
    }
}

//======================================================================
double VehicleLaneShifter::activeErrorVelocity() const
{
    double result = 0;
    if(_isActive)
    {
        if (_isLeft)
        {
            result
                = GVManager::getNumeric("ERROR_VELOCITY")/60.0/60.0;
        }
        else
        {
            result
                = -1 * GVManager::getNumeric("ERROR_VELOCITY")/60.0/60.0;
        }
    }
    return result;
}
