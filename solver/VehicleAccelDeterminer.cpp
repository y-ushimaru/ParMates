/* **************************************************
 * Copyright (C) 2014 ADVENTURE Project
 * All Rights Reserved
 **************************************************** */
#include "Vehicle.h"
#include "VirtualLeader.h"
#include "GVManager.h"
#include "Random.h"
#include "ParallelConf.h"
#include <vector>
#include <algorithm>
#include <cmath>
#include <cassert>
#include <chrono>

using namespace std;

//======================================================================
void Vehicle::determineAcceleration()
{
    // 意思決定時間の計測
    double start, end;
    if(TimeManager::time()%MEASURE_INTERVAL == 0)
    {
      start = getTime();
    }
 
    //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    // 車線変更時は車線に対して垂直方向(error方向)の速度を生じさせる
    // これはスリープ判定より前でなければならない．
    if (_laneShifter.isActive())
    {
        _errorVelocity = _laneShifter.activeErrorVelocity();
    }
    else
    {
        _errorVelocity = 0.0;
    }

    if (_sleepTime>0)
    {
        return;
    }

    //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    // 加速度決定

    // 加速度候補を入れるvector
    vector<double> accelCandidates;

    double vOpt;  //単位は[m/msec]
    double aOpt;  //d^2x/dt^2
    double accel;

    /**
     * 詳しくはGeneralized Force Modelを参照
     * D.Helbing and B.Tilch
     * "Generalized force model of traffic dynamics"
     * Phy.Rev.E Vol.58 No.1(1998), pp.133-138
     */
    double ta0 = 2450;     //[msec]
    double ta1 = 770;      //[msec]
    double ta2;
    double ra0 =  5.59;    //[m]
    double ra1 = 98.78;    //[m]
    double da  =  1.38;    //[m]      <- 最小車間距離
    double tt  =
        GVManager::getNumeric("REACTION_TIME_VEHICLE")*1000.0;
    //[sec]->[msec]   <- 反応遅れ時間

    // 自由走行（前方の状況を考慮しない）
    aOpt = (_vMax-_velocity)/ta0;
    accelCandidates.push_back(aOpt);

    // 前方の状況に応じた最適な加速度の決定
    for (unsigned int i=0; i<_leaders.size(); i++)
    {
        ta2 = ta1
            * exp((_leaders[i]->distance()
                   - (da+tt*_velocity))/ra1);
        if (_velocity > _leaders[i]->velocity())
        {
            vOpt = ( ta2
                     * _vMax * (1-exp(-(_leaders[i]->distance()
                                        -(da+tt*_velocity))/ra0))
                     + ta0 * _leaders[i]->velocity() ) / (ta0+ta2);
            aOpt = (vOpt-_velocity) * (1/ta0+1/ta2);
        }
        else
        {
            vOpt = _vMax*(1-exp(-(_leaders[i]->distance()
                                 -(da+tt*_velocity))/ra0));
            aOpt = (vOpt-_velocity) * (1/ta0);
        }
        accelCandidates.push_back(aOpt);
    }

    // 最も遅い候補を選ぶ
    // 効用を考えたければ速度と効用のpairを使う？
    assert(!accelCandidates.empty());
    accel = *min_element(accelCandidates.begin(),
                         accelCandidates.end());

    // 最大加速度，最大減速度を越えた場合の処理
    if (accel>_maxAcceleration)
    {
        accel = _maxAcceleration;
    }
    else if (accel<_maxDeceleration)
    {
        accel = _maxDeceleration;
    }

    // _vehicleの速度，加速度を更新する．
    _accel = accel;

    _velocity
        = _velocity+accel*TimeManager::unit();

    if (_velocity < 1.0e-5)
    {
        _velocity = 0.0;

        // 交差点内部での停車は想定外(ただし横断歩道はありうる)
        // デッドロックの要因となるため，確率的にスリープする
        if (_intersection
            && Random::uniform(_rnd)<0.1)
        {
            _sleepTime = 3.0 * 1000;
        }
    }

    // _velocityの保存
    if (GVManager::getFlag("VEHICLE_VELOCITY_HISTORY_RECORD")
        && (TimeManager::step() - startStep())
        % (int)GVManager::getNumeric("VEHICLE_VELOCITY_HISTORY_INTERVAL")
        == 0)
    {
        _velocityHistory.push_back(_velocity / _vMax);
        if (_velocityHistory.size()
            > GVManager::getNumeric("VEHICLE_VELOCITY_HISTORY_SIZE"))
        {
            _velocityHistory.pop_front();
        }
    }

    // 意思決定時間の計測
    if(TimeManager::time()%MEASURE_INTERVAL == 0)
    {
       end = getTime();
       _calcTime += end-start;
    }
}
