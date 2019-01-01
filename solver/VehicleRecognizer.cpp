/* **************************************************
 * Copyright (C) 2014 ADVENTURE Project
 * All Rights Reserved
 **************************************************** */
#include "Vehicle.h"
#include "VirtualLeader.h"
#include "Lane.h"
#include "Signal.h"
#include "GVManager.h"
#include "AmuPoint.h"
#include "AmuVector.h"
#include "Random.h"
#include "ParallelConf.h"
#include <iostream>
#include <typeinfo>
#include <cassert>
#include <chrono>
#ifdef INCLUDE_PEDESTRIANS
#include "pedestrian/VehiclePedExt.h"
#include "pedestrian/Pedestrian.h"
#include "pedestrian/Zebra.h"
#endif //INCLUDE_PEDESTRIANS

using namespace std;

//#define VEHICLE_RECOG_MEASURE_TIME

//======================================================================
void Vehicle::recognize()
{
    // 計算時間の初期化
    if(TimeManager::time()%SEND_LOAD_INTERVAL == 0)
    {
      _calcTime = 0;
    }

    // 認知時間の計測
    double start, end;
    if(TimeManager::time()%MEASURE_INTERVAL == 0)
    {
      start = getTime();
    }

    //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    // 1ステップ前にセットされた仮想先行車のクリア
    for (unsigned int i=0; i<_leaders.size(); i++)
    {
        delete _leaders[i];
    }
    _leaders.clear();

    //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    // スリープ中は何もしない
    if (_sleepTime>0)
    {
        return;
    }

    // 単路中で交差点との境界に近く，レーンの先頭で，速度0の場合，
    // 一旦停止フラグを立てる
    if (_section
        && _section->lengthToNext(_lane, _length)<_bodyLength
        && !(_lane->frontAgent(this))
        && abs(_velocity)<1.0e-6
        && !_hasPaused)
    {
        _hasPaused = true;
    }

    //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    // ローカル経路探索

    // 隣のレーンを探索中の場合は探索を継続する
    /*
     * 単路が傾いていて隣のレーンの始点が異なる場合に，
     * 隣レーンがあるにも関わらず探索失敗することがあるため
     * (レーンに垂直に探索線分を作成していることに起因する)
     */
    if (_section
        && _localRouter.isSearchingSideLanes())
    {
        _localRouter.localReroute(_section, _lane, _length);
    }

    //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    // 車線変更の可否の判断
    if (_section
        && !_localRoute.desiredEmpty()
        && !_laneShifter.isActive()
        && !_laneShifter.canShift())
    {
        _laneShifter.checkLaneSpace();
    }

    //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    // VirtualLeaderの探索全般で用いる変数の決定

    // 次に通過する交差点
    Intersection* nextInter = NULL;
    // 直前に通過した，あるいは現在通過中の交差点
    Intersection* prevInter = NULL;
    if (_section)
    {
        nextInter = _section->intersection(_section->isUp(_lane));
        prevInter = _section->intersection(!(_section->isUp(_lane)));
    }
    else
    {
        if (_localRoute.desiredEmpty())
        {
            // 予定経路を守れている場合
            if(_prevIntersection)
            {
                nextInter
                    = const_cast<Intersection*>
                    (_route->next(_prevIntersection,
                                  _intersection));
            }
            else
            {
                nextInter
                    = const_cast<Intersection*>
                    (_route->next(_intersection));
            }
        }
        else
        {
            // 予定経路を守れていない場合
            /*
             * 通常，LocalRouteの最後から2つめには
             *  交差点内の最後のレーンが入っているはず
             */
            Lane* exitLane = _localRoute.previous(_localRoute.last());
            if (exitLane && _intersection->isMyLane(exitLane))
            {
                nextInter = _intersection
                    ->next(_intersection
                           ->direction(exitLane->endConnector()));
            }
            else
            {
                nextInter = _intersection->next(1);
            }
        }
        prevInter = _intersection;
    }

    // 次の交差点での進行方向
    RelativeDirection turning = _localRoute.turning();

    // 次の交差点に進入するか否か
    bool isNextInterEnterable = true;

    //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    // 希望走行速度の決定
    _determineDesiredVelocity(turning);

    //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    // 仮想先行車の取得
    /**
     * @todo
     * ひとまず，エージェントは毎ステップ必要な全ての情報を
     * 正しく認知できることを 前提とする．
     * 見落としなどを実装するためにはこの関数を修正する必要があろう．
     */

    //------------------------------------------------------------------
    // 先行エージェント
#ifdef VEHICLE_RECOG_MEASURE_TIME
    TimeManager::startClock("VEHICLE_FRONTAGENT");
#endif //VEHICLE_RECOG_MEASURE_TIME

    _searchFrontAgent(50);

#ifdef VEHICLE_RECOG_MEASURE_TIME
    TimeManager::stopClock("VEHICLE_FRONTAGENT");
#endif //VEHICLE_RECOG_MEASURE_TIME

    //------------------------------------------------------------------
    // 前方の制限速度
#ifdef VEHICLE_RECOG_MEASURE_TIME
   TimeManager::startClock("VEHICLE_FRONTSPEEDLIMIT");
#endif //VEHICLE_RECOG_MEASURE_TIME

   // 単路部の終端より30m以上手前であれば希望速度以上の情報は得られない
   if (_intersection
       || _section->lengthToNext(_lane, _length) < 30)
   {
       _searchFrontSpeedLimit(30);
   }

#ifdef VEHICLE_RECOG_MEASURE_TIME
   TimeManager::stopClock("VEHICLE_FRONTSPEEDLIMIT");
#endif //VEHICLE_RECOG_MEASURE_TIME

    //------------------------------------------------------------------
    // 信号
    if (isNextInterEnterable)
    {

#ifdef VEHICLE_RECOG_MEASURE_TIME
        TimeManager::startClock("VEHICLE_SIGNAL");
#endif //VEHICLE_RECOG_MEASURE_TIME

        isNextInterEnterable
            = !(_isStoppedBySignal(nextInter,
                                   prevInter,
                                   turning,
                                   50));

#ifdef VEHICLE_RECOG_MEASURE_TIME
        TimeManager::stopClock("VEHICLE_SIGNAL");
#endif //VEHICLE_RECOG_MEASURE_TIME
    }

#ifdef INCLUDE_PEDESTRIANS
    //------------------------------------------------------------------
    // 歩行者
    if (isNextInterEnterable)
    {
        isNextInterEnterable
            = _pedExt->recognizePedestrian(nextInter);
    }
#endif //INCLUDE_PEDESTRIANS

    //------------------------------------------------------------------
    // 単路内先頭車両の処理
    /*
     * 次に進入する交差点がODノードの場合は無視する
     */
    if (_section
        && isNextInterEnterable
        && _section->isHeadAgent(this, _lane)
        && typeid(*nextInter) != typeid(ODNode))
    {
        //--------------------------------------------------------------
        // 交錯レーン探索の準備

        // 交差点内交錯レーンのコンテナ
        vector<Lane*> clInter;
        // 上流の単路内レーンのコンテナ
        vector<Lane*> clSection;
        /*
         * 交差点内の交錯レーン（分岐、合流含む）clInterと
         * 交錯レーンの上流の単路内レーンclSectionを同時に取得する
         * この時点ではレーンの幾何的情報のみを元に抽出
         */
        nextInter->collisionLanes(_localRoute.lanesInIntersection(),
                                  &clInter, &clSection);

        //--------------------------------------------------------------
        // 交差点内交錯レーン
        if (isNextInterEnterable)
        {

#ifdef VEHICLE_RECOG_MEASURE_TIME
            TimeManager::startClock("VEHICLE_CLINTER");
#endif //VEHICLE_RECOG_MEASURE_TIME

            isNextInterEnterable
                = !(_isStoppedByCollisionInIntersection(nextInter,
                                                        &clInter,
                                                        turning));

#ifdef VEHICLE_RECOG_MEASURE_TIME
            TimeManager::stopClock("VEHICLE_CLINTER");
#endif //VEHICLE_RECOG_MEASURE_TIME

        }

        //--------------------------------------------------------------
        // 交差点上流交錯レーン
        if (isNextInterEnterable)
        {

#ifdef VEHICLE_RECOG_MEASURE_TIME
            TimeManager::startClock("VEHICLE_CLSECT");
#endif //VEHICLE_RECOG_MEASURE_TIME

            isNextInterEnterable
                = !(_isStoppedByCollisionInSection(nextInter,
                                                   &clSection,
                                                   turning));

#ifdef VEHICLE_RECOG_MEASURE_TIME
            TimeManager::stopClock("VEHICLE_CLSECT");
#endif //VEHICLE_RECOG_MEASURE_TIME

        }

        //--------------------------------------------------------------
        // 右左折時は交差点内に1台ずつしか進入できない
        if (isNextInterEnterable
            && !(clInter.empty()))
        {

#ifdef VEHICLE_RECOG_MEASURE_TIME
            TimeManager::startClock("VEHICLE_LANEININTER");
#endif //VEHICLE_RECOG_MEASURE_TIME

            isNextInterEnterable
                = !(_isStoppedByLaneInInter(nextInter,
                                            turning));

#ifdef VEHICLE_RECOG_MEASURE_TIME
            TimeManager::stopClock("VEHICLE_LANEININTER");
#endif //VEHICLE_RECOG_MEASURE_TIME

        }

        //--------------------------------------------------------------
        // 交差点下流（通過後）の空きスペース
        if (isNextInterEnterable)
        {

#ifdef VEHICLE_RECOG_MEASURE_TIME
            TimeManager::startClock("VEHICLE_SHORTSPACE");
#endif //VEHICLE_RECOG_MEASURE_TIME

            isNextInterEnterable
                = !(_isStoppedByShortSpace());

#ifdef VEHICLE_RECOG_MEASURE_TIME
            TimeManager::stopClock("VEHICLE_SHORTSPACE");
#endif //VEHICLE_RECOG_MEASURE_TIME

        }

        //--------------------------------------------------------------
        // 最小ヘッドウェイ
        if (isNextInterEnterable)
        {

#ifdef VEHICLE_RECOG_MEASURE_TIME
            TimeManager::startClock("VEHICLE_MIN_HEADWAY");
#endif //VEHICLE_RECOG_MEASURE_TIME

            isNextInterEnterable
                = !(_isStoppedByMinHeadway(nextInter,
                                           turning));

#ifdef VEHICLE_RECOG_MEASURE_TIME
            TimeManager::stopClock("VEHICLE_MIN_HEADWAY");
#endif //VEHICLE_RECOG_MEASURE_TIME

        }

        //--------------------------------------------------------------
        // 転回時事前減速
        if (isNextInterEnterable)
        {

#ifdef VEHICLE_RECOG_MEASURE_TIME
            TimeManager::startClock("VEHICLE_BEFORE_TURN");
#endif //VEHICLE_RECOG_MEASURE_TIME

            _determineTurningVelocity(turning);

#ifdef VEHICLE_RECOG_MEASURE_TIME
            TimeManager::stopClock("VEHICLE_BEFORE_TURN");
#endif //VEHICLE_RECOG_MEASURE_TIME

        }
    }

    //------------------------------------------------------------------
    // 単路内全車両の処理
    if (_section)
    {
        //--------------------------------------------------------------
        // 車線変更中であれば変更先の先行車
        if (_laneShifter.isActive())
        {
            _laneShifter.searchNextLeader();
        }

        //--------------------------------------------------------------
        // 車線変更を受ける場合

#ifdef VEHICLE_RECOG_MEASURE_TIME
        TimeManager::startClock("VEHICLE_SHIFTLANE_PASSIVE");
#endif //VEHICLE_RECOG_MEASURE_TIME

        _laneShifter.searchInterruption();

#ifdef VEHICLE_RECOG_MEASURE_TIME
        TimeManager::stopClock("VEHICLE_SHIFTLANE_PASSIVE");
#endif //VEHICLE_RECOG_MEASURE_TIME

    }

    //------------------------------------------------------------------
    // 交差点内全車両の処理
    if (_intersection)
    {
#ifdef VEHICLE_RECOG_MEASURE_TIME
        TimeManager::startClock("VEHICLE_IN_INTER");
#endif //VEHICLE_RECOG_MEASURE_TIME

        _searchPreferredAgentInIntersection(turning);

#ifdef VEHICLE_RECOG_MEASURE_TIME
        TimeManager::stopClock("VEHICLE_IN_INTER");
#endif //VEHICLE_RECOG_MEASURE_TIME
    }

    //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    // 先づまりを避けるためのローカル経路再探索
    /*
     * 次の交差点に進入可能で直進であるにも関わらず
     * 停止中あるいは減速中には別のローカル経路がないか探索してみる
     */
    if (_section
        && isNextInterEnterable
        && turning == RD_STRAIGHT
        && (_accel<0 || _velocity<1.0e-6)
        && _localRoute.desiredEmpty()
        && Random::uniform(_rnd)<0.05)
    {
#ifdef VEHICLE_RECOG_MEASURE_TIME
        TimeManager::startClock("VEHICLE_FRONTAGENT_LOCALREROUTE");
#endif //VEHICLE_RECOG_MEASURE_TIME

        _localRouter.localRerouteSide(_section, _lane, _length);

#ifdef VEHICLE_RECOG_MEASURE_TIME
        TimeManager::stopClock("VEHICLE_FRONTAGENT_LOCALREROUTE");
#endif //VEHICLE_RECOG_MEASURE_TIME
    }

    // 認知時間の計測
    if(TimeManager::time()%MEASURE_INTERVAL == 0)
    {
       end = getTime();
       _calcTime += end-start;
    }
}

//======================================================================
void Vehicle::_determineDesiredVelocity(RelativeDirection turning)
{
    if (_section)
    {
        // 単路の場合は制限速度
        _vMax = _lane->speedLimit()/60.0/60.0; //[km/h]->[m/msec]
    }
    else
    {
        // 交差点の場合は抜けた先の単路を参照
        _vMax = _localRoute.last()->speedLimit()/60.0/60.0;

        double tmpV = _vMax;
        if (_intersection->signal()
            && ((_intersection->signal()
                 ->permission(_intersection
                              ->direction(_prevIntersection))
                 == Signal::CRAWLING)
                || (_intersection->signal()
                    ->permission(_intersection
                                 ->direction(_prevIntersection))
                    == Signal::PAUSING)
                ))
        {
            // 信号が黄点滅や赤点滅の場合は徐行
            tmpV
                = GVManager::getNumeric("VELOCITY_CRAWL")
                / 60.0 / 60.0;
        }
        if (turning==RD_RIGHT)
        {
            // 右折時の走行速度
            tmpV
                = GVManager::getNumeric("VELOCITY_AT_TURNING_RIGHT")
                / 60.0 / 60.0;
        }
        else if (turning==RD_LEFT)
        {
            // 左折時の走行速度
            tmpV
                = GVManager::getNumeric("VELOCITY_AT_TURNING_LEFT")
                / 60.0 / 60.0;
        }
        if (tmpV<_vMax)
        {
            _vMax = tmpV;
        }
    }
}

//======================================================================
void Vehicle::_searchFrontAgent(double threshold)
{
    RoadOccupant* front = _lane->frontAgent(this);
    if (front)
    {
#ifndef VL_DEBUG
        VirtualLeader* leader =
            new VirtualLeader(
                front->length() - _length
                - (front->bodyLength()/2 + _bodyLength/2),
                front->velocity());
#else
        VirtualLeader* leader =
            new VirtualLeader(
                front->length() - _length
                - (front->bodyLength()/2 + _bodyLength/2),
                front->velocity(),
                "FRONT:"+front->id());
#endif
        _leaders.push_back(leader);
    }

    else
    {
        double totalLength = _lane->length() - _length;
        Lane* lookupLane   = _lane;
        LaneBundle* lookupBundle = _lane->parent();

        while (totalLength < threshold)
        {
            // 探索がレーン束を超える場合
            if (!(lookupBundle->isNextLaneMine(lookupLane)))
            {
                lookupBundle
                    = lookupBundle->nextBundle(lookupLane);
                // ODノードに到達したら探索を打ち切る
                if (typeid(*lookupBundle)==typeid(ODNode))
                {
                    break;
                }
            }

            // 次のレーンを探す
            /*
             * LocalRouteに含まれるレーンを優先し，
             * 見つからなければlookupLaneの直進レーンとする
             */
            Lane* nextLane = _localRoute.next(lookupLane);
            if (!nextLane)
            {
                nextLane = lookupLane->nextStraightLane();
            }
            lookupLane = nextLane;
            if (!lookupLane)
            {
                break;
            }

            front = lookupLane->tailAgent();
            if (front)
            {
#ifndef VL_DEBUG
                VirtualLeader* leader =
                    new VirtualLeader(
                        totalLength + front->length()
                        - (front->bodyLength()/2 + _bodyLength/2),
                        front->velocity());
#else
                VirtualLeader* leader =
                    new VirtualLeader(
                        totalLength + front->length()
                        - (front->bodyLength()/2 + _bodyLength/2),
                        front->velocity(),
                        "FRONT:"+front->id());
#endif
                _leaders.push_back(leader);
                break;
            }
            totalLength += lookupLane->length();
        }
    }
}

//======================================================================
void Vehicle::_searchFrontSpeedLimit(double threshold)
{
    // 現在のレーンの制限速度は希望速度に反映されている
    double totalLength = _lane->length() - _length;
    Lane* lookupLane   = _lane;
    LaneBundle* lookupBundle = _lane->parent();

    while (totalLength < threshold)
    {
        // 探索がレーン束を超える場合
        if (!(lookupBundle->isNextLaneMine(lookupLane)))
        {
            lookupBundle
                = lookupBundle->nextBundle(lookupLane);
            // ODノードに到達したら探索を打ち切る
            if (typeid(*lookupBundle)==typeid(ODNode))
            {
                break;
            }
        }

        // 次のレーンを探す
        /*
         * LocalRouteに含まれるレーンを優先し，
         * 見つからなければlookupLaneの直進レーンとする
         */
        Lane* nextLane = _localRoute.next(lookupLane);
        if (!nextLane)
        {
            nextLane = lookupLane->nextStraightLane();
        }
        lookupLane = nextLane;
        if (!lookupLane)
        {
            break;
        }

        /*
         * 対象のレーンの制限速度が現在の速度よりも小さければ事前に減速する
         */
        if (lookupLane->speedLimit()/3600.0<_velocity) // km/h->m/msec
        {
#ifndef VL_DEBUG
            VirtualLeader* leader =
                new VirtualLeader(
                    totalLength - _bodyLength/2,
                    lookupLane->speedLimit()/3600.0);
#else
            VirtualLeader* leader =
                new VirtualLeader(
                    totalLength - _bodyLength/2,
                    lookupLane->speedLimit()/3600.0,
                    "SPEEDLIMIT:"+lookupLane->id());
#endif
            _leaders.push_back(leader);
            break;
        }
        totalLength += lookupLane->length();
    }
}

//======================================================================
bool Vehicle::_isStoppedBySignal(Intersection* nextInter,
                                 Intersection* prevInter,
                                 RelativeDirection turning,
                                 double threshold)
{
    bool isStopped = false;
    bool isSearchContinuing = true;
    if (!nextInter)
    {
        return isStopped;
    }

    // 次の交差点の信号
    int fromDir = nextInter->direction(prevInter);

    RelativeDirection rd = turning;
    double distanceToSignal;
    if (_section)
    {
        distanceToSignal
            = _section->lengthToNext(_lane, _length);
    }
    else
    {
        distanceToSignal
            = _intersection->lengthToNext(_lane, _length)
            + _intersection->nextSection(nextInter)->length();
    }

    Intersection* lookupFrom = prevInter;
    Intersection* lookupTo   = nextInter;
    while (distanceToSignal < threshold)
    {
        Signal* signal = NULL;
        signal = lookupTo->signal();
        if (signal)
        {
            switch (lookupTo->permission(fromDir, rd, this))
            {
            case Signal::PROHIBITION: // 進入許可なし（赤信号）
            {
#ifndef VL_DEBUG
                VirtualLeader* leader =
                    new VirtualLeader
                    (distanceToSignal-_bodyLength/2,
                     0);
#else
                VirtualLeader* leader =
                    new VirtualLeader
                    (distanceToSignal-_bodyLength/2,
                     0,
                     "RED:"+lookupTo->id());
#endif
                _leaders.push_back(leader);

                // 目前の交差点で停止する場合はフラグをtrueに
                if (lookupTo == nextInter)
                {
                    isStopped = true;
                }
                break;
            }
            case Signal::PERMISSION:  // 進入許可あり（青信号）
                break;
            case Signal::CRAWLING:    // 徐行制限（黄点滅）
            {
#ifndef VL_DEBUG
                VirtualLeader* leader =
                    new VirtualLeader
                    (distanceToSignal+_bodyLength,
                     GVManager::getNumeric("VELOCITY_CRAWL")/60/60);
#else
                VirtualLeader* leader =
                    new VirtualLeader
                    (distanceToSignal+_bodyLength,
                     GVManager::getNumeric("VELOCITY_CRAWL")/60/60,
                     "YELLOBLINK:"+lookupTo->id());
#endif
                _leaders.push_back(leader);
                isSearchContinuing = false;
                break;
            }
            case Signal::PAUSING:     // 一旦停止（赤点滅）
            {
                if (_hasPaused)       // 一旦停止後は徐行
                {
#ifndef VL_DEBUG
                    VirtualLeader* leader =
                        new VirtualLeader
                        (distanceToSignal+_bodyLength,
                         GVManager::getNumeric("VELOCITY_CRAWL")/60/60);
#else
                    VirtualLeader* leader =
                        new VirtualLeader
                        (distanceToSignal+_bodyLength,
                         GVManager::getNumeric("VELOCITY_CRAWL")/60/60,
                         "REDBLINK1:"+lookupTo->id());
#endif
                    _leaders.push_back(leader);
                    isSearchContinuing = false;
                    break;
                }
                else                  // 一旦停止前は停止
                {
#ifndef VL_DEBUG
                    VirtualLeader* leader =
                        new VirtualLeader
                        (distanceToSignal-_bodyLength/2,
                         0);
#else
                    VirtualLeader* leader =
                        new VirtualLeader
                        (distanceToSignal-_bodyLength/2,
                         0,
                         "REDBLINK:"+lookupTo->id());
#endif
                    _leaders.push_back(leader);

                    // 目前の交差点で停止する場合はフラグをtrueに
                    if (lookupTo == nextInter)
                    {
                        isStopped = true;
                    }
                    isSearchContinuing = false;
                    break;
                }
            }
            default:
                break;
            }
        }
        if (!isSearchContinuing)
        {
            break;
        }

        // 更に前方を見るための処理
        Intersection* lookupTmp = lookupTo;
        lookupTo
            = const_cast<Intersection*>
            (_route->next(lookupFrom, lookupTo));
        lookupFrom = lookupTmp;
        if (!lookupTo)
        {
            break;
        }
        distanceToSignal
            += lookupFrom->center().distance(lookupTo->center());
        if (distanceToSignal > threshold)
        {
            break;
        }
        fromDir = lookupTo->direction(lookupFrom);

        if (fromDir == -1)
        {
            cout << "vehicle: " << _id;
            _route->print(cout);
        }
    }

    return isStopped;
}

//======================================================================
bool Vehicle::_isStoppedByCollisionInIntersection(
    Intersection* nextInter,
    vector<Lane*>* clInter,
    RelativeDirection turning)
{
    bool isStopped   = false;

    for (unsigned int i=0; i<clInter->size(); i++)
    {
        RoadOccupant* clTail = (*clInter)[i]->tailAgent();
        for (RoadOccupant* clAgent = clTail;
             clAgent;
             clAgent = (*clInter)[i]->frontAgent(clAgent))
        {
            if (!clAgent)
            {
                break;
            }
            // 簡易検索の場合は最後尾エージェントのみ
            if (!_strictCollisionCheck
                && clAgent != clTail)
            {
                break;
            }
            // ないはずだが，自分自身は無視する
            if (clAgent == this)
            {
                continue;
            }

            // 交錯する地点までの距離を計算する
            bool hasPassedCollisionPoint = false;
            const vector<Lane*>* liInter
                = _localRoute.lanesInIntersection();

            for(unsigned j=0; j<liInter->size(); j++)
            {
                // 交錯地点を保持する変数
                AmuPoint cp;
                if ((*liInter)[j]
                    ->createIntersectionPoint
                    ((*clInter)[i]->lineSegment(),
                     &cp))
                {
                    // 交錯地点までの距離
                    double ltc
                        = (*clInter)[i]->lineSegment()
                        .pointBegin().distance(cp);

                    // clAgentがすでに交錯地点を通過しているか
                    if (ltc
                        < clAgent->length()
                        - clAgent->bodyLength()/2)
                    {
                        hasPassedCollisionPoint = true;
                    }
                    // 交錯地点が1つでも見つかればそれ以上探索しない
                    break;
                }
            }

            // まだ交錯地点を通過していない車両に対する処理処理
            if (!hasPassedCollisionPoint)
            {
                // 自分が交差点に到達するまでの時間(Time To Interseciton)
                double thisTti;
                if (_section->lengthToNext(_lane, _length) < 20)
                {
                    // 交差点に接近している場合は速度に関係なく0
                    thisTti = 0.0;
                }
                else if (_velocity>1.0e-6)
                {
                    // 速度が0でなければ実際にかかる時間
                    thisTti
                        = _section->lengthToNext(_lane, _length)
                        / _velocity;
                }
                else
                {
                    thisTti = 100.0*1000;
                }

                // 相手が交差点を通過するまでの時間(Time to Passing)
                double thatTtp;
                if (clAgent->velocity()<1.0e-6)
                {
                    // 停車中ならひとまず100秒
                    thatTtp = 100.0*1000;
                }
                else
                {
                    thatTtp
                        = nextInter->lengthToNext((*clInter)[i],
                                                  clAgent->length())
                        / clAgent->velocity();
                }

                // thisTti < thatTtpであれば交錯の可能性がある
                if (thisTti < thatTtp)
                {
#ifndef VL_DEBUG
                    VirtualLeader* leader =
                        new VirtualLeader
                        (_section->lengthToNext(_lane, _length)
                         -_bodyLength/2, 0);
#else
                    VirtualLeader* leader =
                        new VirtualLeader
                        (_section->lengthToNext(_lane, _length)
                         -_bodyLength/2, 0,
                         "CLINT:"+clAgent->id());
#endif
                    _leaders.push_back(leader);
                    isStopped = true;
                    break;
                }
            }
        }
    }
    return isStopped;
}

//======================================================================
bool Vehicle::_isStoppedByCollisionInSection(
    Intersection* nextInter,
    vector<Lane*>* clSection,
    RelativeDirection turning)
    /*
     * 交錯単路を走行する車両との判定は以下の手順で行っている．
     *
     * 0. 交差点内交錯レーンの上流にある単路内レーンは取得済みである
     * 1. 取得した単路内レーンについて、それぞれの先頭車両を取得する
     * 2. 「先頭車両を（目視などによって）認識できる場合に限り」、
     *    自車と交錯単路内レーンの先頭車両との位置関係、
     *    進行方向（ウィンカーで判別）から
     *    道を譲るべきかどうか判定(_isYielding)
     * 3. 道を譲ると判定され、実際に該当交差点に到達するまでの時間差が
     *    小さい場合に減速->停止
     *    （相手よりもかなり前に自分が交差点を通過できる場合は
     *      優先関係を無視する）
     *
     * 自車と同方向（同じ単路）から進入する車には注意が必要
     * _isYieldingの中で条件分岐して対応している．
     */
{
    bool isStopped   = false;

    Signal* signal = nextInter->signal();
    if (!signal)
    {
        return isStopped;
    }

    // 交差点から見た自分の進入方向
    int thisDir = nextInter->direction(_section);

    // ギャップアクセプタンス([sec]->[msec])
    double gap
        = GVManager::getNumeric("GAP_ACCEPTANCE_VEHICLE_CROSS")*1000;

    for (unsigned int i=0; i<clSection->size(); i++)
    {
        // 交差点から見た相手の進入方向
        int thatDir
            = nextInter->direction((*clSection)[i]->endConnector());
        assert(thatDir != -1);

        /*
         * 相手レーンの信号が赤，赤点滅の場合は自分を優先するため
         * 交錯レーンを探索しない
         */
        if (signal->permission(thatDir) == Signal::PROHIBITION
            || signal->permission(thatDir) == Signal::PAUSING)
        {
            continue;
        }

        // 交錯する可能性のあるエージェント
        Vehicle* headVehicle = (*clSection)[i]->headVehicle();
        if (!headVehicle
            || headVehicle==this
            /* || !(_isVisible(headVehicle)) */ )
            // この時点で見通しを考慮するならコメントを外す
        {
            continue;
        }

        // 交錯する可能性のあるエージェントが走行中の単路
        Section* collisionSection
            = nextInter->nextSection(thatDir);

        if (_isYielding(nextInter, thisDir, thatDir, turning, headVehicle))
        {
            /*
             * 相手に道を譲る状況であっても，
             * 衝突のおそれがなければ減速しなくてよい
             */

            // 自分が交差点に到達するまでの時間
            double thisTti;
            if (_section->lengthToNext(_lane, _length) < _bodyLength)
            {
                // 交差点に近ければひとまず3秒
                thisTti = 3.0*1000;
            }
            else if (_velocity>1.0e-6)
            {
                // 交差点から遠ければ実時間
                thisTti
                    = _section->lengthToNext(_lane, _length) / _velocity;
            }
            else
            {
                // _velocity=0だと無限大になってしまうためひとまず100秒に
                thisTti = 100.0*1000;
            }

            // 相手が交差点に到達するまでの時間
            double thatTti;
            // 相手が交差点の直前で加速中は別処理（優先車両の動き出し）
            /*
             * 動き出したばかりで低速の状態であると
             * thatTtiを過大に評価してしまうため
             */
            if (collisionSection->lengthToNext((*clSection)[i],
                                               headVehicle->length())
                < headVehicle->bodyLength()
                && headVehicle->velocity()>0)
            {
                thatTti = 0.0;
            }
            else if (headVehicle->velocity()>1.0e-6)
            {
                thatTti
                    = collisionSection->lengthToNext((*clSection)[i],
                                                     headVehicle->length())
                    / headVehicle->velocity();
            }
            else
            {
                thatTti = 50.0*1000;
            }

            /*
             * 自分が交差点に到達するまでの時間と
             * 相手が交差点に到達する時間の差がgap以下なら道を譲る
             */
            if (thatTti-thisTti < gap)
            {
#ifndef VL_DEBUG
                VirtualLeader* leader =
                    new VirtualLeader(_section->lengthToNext(_lane, _length)
                                      -_bodyLength/2, 0);
#else
                VirtualLeader* leader =
                    new VirtualLeader(_section->lengthToNext(_lane, _length)
                                      -_bodyLength/2, 0,
                                      "CLSEC:"+headVehicle->id());
#endif
                _leaders.push_back(leader);
                isStopped = true;
                break;
            }
        }
    }

    // 確率的にスリープし，デッドロックを解消する
    if (isStopped
        && _velocity<1.0e-6
        && Random::uniform(_rnd)<0.05)
    {
        _sleepTime = 1.5 * 1000;
    }

    return isStopped;
}

//======================================================================
bool Vehicle::_isStoppedByLaneInInter(Intersection* inter,
                                      RelativeDirection turning)
{
    // 直進車は常にfalse(交差点進入可)
    if (turning==RD_STRAIGHT)
    {
        return false;
    }

    Lane* targetLane = _nextLane;
    // 交差点のレーンに辿り着くまでlocalRouteを読み飛ばし
    while (!(inter->isMyLane(targetLane)))
    {
        targetLane = _localRoute.next(targetLane);
    }
    // 交差点のレーンを処理
    while (inter->isMyLane(targetLane))
    {
        // 先行車がいたら進入しない
        if (targetLane->tailAgent())
        {
#ifndef VL_DEBUG
                VirtualLeader* leader =
                    new VirtualLeader(_section->lengthToNext(_lane, _length)
                                      -_bodyLength/2, 0);
#else
                VirtualLeader* leader =
                    new VirtualLeader(_section->lengthToNext(_lane, _length)
                                      -_bodyLength/2, 0,
                                      "INTLN:"+targetLane->id());
#endif
                _leaders.push_back(leader);
               return true;
        }
        targetLane = _localRoute.next(targetLane);
    }
    return false;
}

//======================================================================
bool Vehicle::_isStoppedByShortSpace()
{
    // 自分自身が停車するのに十分な長さ（2.0は車間距離）
    double enoughLength = _bodyLength + 2.0;

    // _localRouteの最後には交差点通過後の最初のレーン入っている
    RoadOccupant* tail = _localRoute.last()->tailAgent();
    if (!tail)
    {
        return false;
    }

    if (tail->length() - tail->bodyLength()/2 < enoughLength
        && (tail->velocity()<1.0e-6
            || tail->accel()<0))
    {
#ifndef VL_DEBUG
        VirtualLeader* leader =
            new VirtualLeader(_section->lengthToNext(_lane, _length)
                              -_bodyLength/2, 0);
#else
        VirtualLeader* leader =
            new VirtualLeader(_section->lengthToNext(_lane, _length)
                              -_bodyLength/2, 0,
                              "TAIL:"+tail->id());
#endif
        _leaders.push_back(leader);

        // 他の車両の進行を妨げないようにするためにスリープする
        if (_velocity<1.0e-6)
        {
            _sleepTime = 1.5 * 1000;
        }
        return true;
    }
    return false;
}

//======================================================================
bool Vehicle::_isStoppedByMinHeadway(Intersection* nextInter,
                                     RelativeDirection turning)
{
    assert(_section);

    if ((turning==RD_LEFT || turning==RD_RIGHT)
        && _velocity>1.0e-6
        && TimeManager::time()
        +_section->lengthToNext(_lane, _length)/_velocity
        -_nextLane->lastArrivalTime()
        < GVManager::getNumeric("MIN_HEADWAY_AT_TURNING")*1000)
        //[sec]->[msec]
    {
#ifndef VL_DEBUG
        VirtualLeader* leader =
            new VirtualLeader(_section->lengthToNext(_lane, _length)
                              -_bodyLength/2, 0);
#else
        VirtualLeader* leader =
            new VirtualLeader(_section->lengthToNext(_lane, _length)
                              -_bodyLength/2, 0,
                              "HEADWAY:"+nextInter->id());
#endif

        _leaders.push_back(leader);
        return true;
    }
    return false;
}

//======================================================================
void Vehicle::_determineTurningVelocity(RelativeDirection turning)
{
    // 直進の場合は何もしない
    if (turning == RD_STRAIGHT)
    {
        return;
    }

    // 右折前減速
    if (turning==RD_RIGHT)
    {
#ifndef VL_DEBUG
        VirtualLeader* leader =
            new VirtualLeader(
                _section->lengthToNext(_lane, _length)+_bodyLength*2,
                GVManager::getNumeric("VELOCITY_AT_TURNING_RIGHT")
                / 60.0 / 60.0);
#else
        VirtualLeader* leader =
            new VirtualLeader(
                _section->lengthToNext(_lane, _length)+_bodyLength*2,
                GVManager::getNumeric("VELOCITY_AT_TURNING_RIGHT")
                / 60.0 / 60.0,
                "RTURN:"+_section->id());
#endif
        _leaders.push_back(leader);
        return;
    }
    // 左折前減速
    else if (turning==RD_LEFT)
    {
#ifndef VL_DEBUG
        VirtualLeader* leader =
            new VirtualLeader(
                _section->lengthToNext(_lane, _length)+_bodyLength*2,
                GVManager::getNumeric("VELOCITY_AT_TURNING_LEFT")
                / 60.0 / 60.0);
#else
        VirtualLeader* leader =
            new VirtualLeader(
                _section->lengthToNext(_lane, _length)+_bodyLength*2,
                GVManager::getNumeric("VELOCITY_AT_TURNING_LEFT")
                / 60.0 / 60.0,
                "LTURN:"+_section->id());
#endif
        _leaders.push_back(leader);
        return;
    }
 }

//======================================================================
void Vehicle::_searchPreferredAgentInIntersection(RelativeDirection turning)
{

    // これから通過予定の交差点内レーンを決定する
    const vector<Lane*>* allLiInter
        = _localRoute.lanesInIntersection();
    vector<Lane*> frontLanes;
    unsigned int pos;
    // 前方のレーンが欲しいため，自分自身が登場するまでは読み飛ばし
    for (pos=0; pos<allLiInter->size(); pos++)
    {
        if ((*allLiInter)[pos] == _lane)
        {
            break;
        }
    }
    for (; pos<allLiInter->size(); pos++)
    {
        frontLanes.push_back((*allLiInter)[pos]);
    }
    if (frontLanes.empty())
    {
        return;
    }

    // 交錯レーンを取得する
    vector<Lane*> clInter;
    vector<Lane*> clSection;
    _intersection->collisionLanes(&frontLanes,
                                  &clInter,&clSection);

    int thisDir = _intersection->direction(_prevIntersection);

    for (unsigned int i=0; i<clInter.size(); i++)
    {
        Vehicle* clTail = clInter[i]->tailVehicle();
        for (Vehicle* clVehicle = clTail;
             clVehicle;
             clVehicle = clInter[i]->frontVehicle(clVehicle))
        {
            if (!clVehicle)
            {
                break;
            }
            // 簡易検索の場合は最後尾エージェントのみ
            if (!_strictCollisionCheck
                && clVehicle != clTail)
            {
                break;
            }
            // 自分自身は無視する
            if (clVehicle == this)
            {
                continue;
            }
            int thatDir = clVehicle->directionFrom();

            if (_isYielding(_intersection, thisDir, thatDir,
                            turning, clVehicle))
            {
                // レーン同士の交点の位置を計算
                Lane* lookupLane = NULL;
                const vector<Lane*>* thatLanes
                    = clVehicle->lanesInIntersection();
                vector<Lane*>::const_iterator itl
                    = find(thatLanes->begin(),
                           thatLanes->end(),
                           clInter[i]);
                assert(itl!=thatLanes->end());

                // 交錯点を持つ自分側のレーン
                Lane* thisClLane = NULL;
                // 自分側の交錯点までの距離
                double thisClLength = 0.0;
                // 交錯点を持つ相手側のレーン
                Lane* thatClLane = NULL;
                // 相手側の交錯点までの距離
                double thatClLength = 0.0;

                for (; itl!= thatLanes->end(); itl++)
                {
                    for (lookupLane = _lane;
                         _intersection->isMyLane(lookupLane);
                         lookupLane = _localRoute.next(lookupLane))
                    {
                        AmuPoint cp;
                        if (lookupLane
                            ->createIntersectionPoint
                            ((*itl)->lineSegment(), &cp))
                        {
                            // 交点が見つかった
                            thisClLane
                                = lookupLane;
                            thisClLength
                                = thisClLane->lineSegment()
                                .pointBegin().distance(cp);

                            thatClLane
                                = (*itl);
                            thatClLength
                                = thatClLane->lineSegment()
                                .pointBegin().distance(cp);
                         }
                        if (thisClLane)
                        {
                            break;
                        }
                    }
                    if (thatClLane)
                    {
                        break;
                    }
                }
                if (!thisClLane || !thatClLane)
                {
                    // 交錯レーンなし
                    continue;
                }
                if ((thisClLane == _lane
                     && thisClLength < _length+_bodyLength/2)
                    || (thatClLane == clVehicle->lane()
                        && thatClLength
                        < clVehicle->length()
                        + clVehicle->bodyLength()/2))
                {
                    /*
                     * 自分と相手のどちらかが
                     * 既に交錯点を通過している場合
                     */
                    continue;
                }

                // thisClLaneから遡って交錯点までの距離を算出する
                double lengthToStop = thisClLength;
                while (lookupLane!=_lane)
                {
                    lookupLane = _localRoute.previous(lookupLane);
                    assert(_intersection->isMyLane(lookupLane));
                    lengthToStop += lookupLane->length();
                }
                lengthToStop -= _length;

#ifndef VL_DEBUG
                VirtualLeader* leader =
                    new VirtualLeader(lengthToStop-_bodyLength/2, 0);
#else
                VirtualLeader* leader =
                    new VirtualLeader(lengthToStop-_bodyLength/2, 0,
                                      "CLINT2:"+clVehicle->id());
#endif
                _leaders.push_back(leader);
            }
        }
    }
}

//======================================================================
bool Vehicle::_isVisible(Vehicle* other) const
{
    /* この関数内で見通し計算についての関数を呼び出す */

    //------------------------------------------------------------------
    // 一般のシミュレータは他車を正しく認知できるため
    // この関数は常にtrueを返す
    return true;

    //------------------------------------------------------------------
    // サンプル: 自分と相手の位置を引数としての関数hoge()を呼ぶ
    // （hogeは別途定義）
    // return hoge(x(),y(),z(),other->x(),other->y(),other->z());
}

//======================================================================
bool Vehicle::_isYielding(Intersection* inter,
                          int thisDir,
                          int thatDir,
                          RelativeDirection turning,
                          Vehicle* other)
{
    /*
     * この段階では交錯単路の先頭車両を取得しただけなので，
     * 車両同士が本当に交錯するかどうかは不明であることに注意．
     *
     * これを解決するためには車両同士の
     * mainLaneが交差するかどうかを判断すればよいが
     * 実際に車両間の意思疎通はウインカーを通じて行うため，
     * 相手のmainLaneまで正確に取得するのは現実的ではない．
     * 以下のコードでは進入方向とウインカーの状態を元にして判断している．
     */

    AmuPoint crossPoint;

    // 交錯を厳密に評価、交錯点上に車がいたら相手側が避ける
    if (_strictCollisionCheck &&
        _lane->createIntersectionPoint(other->lane()->lineSegment(),
                                       &crossPoint))
    {
        AmuPoint thisPoint(x(), y(), z());
        AmuPoint thatPoint(other->x(), other->y(), other->z());
        double thisDistance = crossPoint.distance(thisPoint)
            - _bodyLength / 2;
        double thatDistance = crossPoint.distance(thatPoint)
            - other->bodyLength() / 2;
        if (thisDistance < 0 || thatDistance < 0)
        {
            if (thisDistance > thatDistance)
            {
                return true;
            }
            else
            {
                return false;
            }
        }
    }

    //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    // スリープ状態の自動車は無視（自分の優先度が高い）
    if (other->isSleep())
    {
        return false;
    }

    //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    // 交差点内での進路が交わらない場合は無視
    bool hasCrossPoint = false;
    const vector<Lane*>* thisLanes = _localRoute.lanesInIntersection();
    const vector<Lane*>* thatLanes = other->lanesInIntersection();
    for (unsigned int i=0; i<thisLanes->size(); i++)
    {
        for (unsigned int j=0; j<thatLanes->size(); j++)
        {
            if ((*thisLanes)[i]
                ->createIntersectionPoint((*thatLanes)[j]->lineSegment(),
                                          &crossPoint))
            {
                hasCrossPoint = true;
                break;
            }
        }
        if (hasCrossPoint==true)
        {
            break;
        }
    }
    if (!hasCrossPoint)
    {
        return false;
    }

    //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    // 赤点滅の優先順位は最下位
    if (inter->signal()!=NULL)
    {
        // 自分が赤点滅でなく、相手が赤点滅の場合は無視
        if (inter->signal()->permission(thisDir)!=Signal::PAUSING
            && inter->signal()->permission(thatDir)==Signal::PAUSING)
        {
            return false;
        }
        // 自分が赤点滅で、相手が赤点滅でなければ必ず譲る
        if (inter->signal()->permission(thisDir)==Signal::PAUSING
            && inter->signal()->permission(thatDir)!=Signal::PAUSING)
        {
            return true;
        }
    }

    bool result = false;

    //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    // thisDir==thatDirの場合は別の優先順位判定とする
    if (thisDir == thatDir)
    {
        // 直進優先
        if (turning == RD_STRAIGHT)
        {
            // 処理は必要ない
        }
        // 左折は第2位
        // 自分より左の車線を直進する車両がいれば譲る
        else if (turning == RD_LEFT)
        {
            if (other->blinker().isNone())
            {
                // 自分と相手を結ぶベクトルを作成し、
                // 自分の進行方向となす角を求める
                /**
                 * 曲線レーンを作成する場合は要修正
                 */
                AmuVector posVector(AmuPoint(x(),y(),z()),
                                    AmuPoint(other->x(),
                                             other->y(),other->z()));
                if (directionVector().calcAngle(posVector)<0)
                {
                    result = true;
                }
            }
        }
        // 右折は第3位
        // 自分より右の車線を直進および左折する車両がいれば譲る
        else if (turning == RD_RIGHT)
        {
            if (other->blinker().isNone()
                || other->blinker().isLeft())
            {
                // 自分と相手を結ぶベクトルを作成し、
                // 自分の進行方向となす角を求める
                /**
                 * 曲線レーンを作成する場合は要修正
                 */
                AmuVector posVector(AmuPoint(x(),y(),z()),
                                    AmuPoint(other->x(),
                                             other->y(),other->z()));
                if (directionVector().calcAngle(posVector)>0)
                {
                    result = true;
                }
            }
        }
        // その他？
        else
        {
            result = true;
        }
    }

    //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    // thisDir!=thatDir 通常の優先順位の判定
    else {
        // 道路交通法第三十七条
        /*
         * 車両等は，交差点で右折する場合において，
         * 当該交差点において直進し，
         * 又は左折しようとする車両等があるときは，
         * 当該車両等の進行妨害をしては成らない．
         *
         */
        if (turning == RD_RIGHT){
            if (other->blinker().isNone())
            {
                // 自分->右折 相手->直進
                result = true;
            }
            else if (other->blinker().isLeft())
            {
                // 自分->右折 相手->左折
                if (inter->relativeDirection(thisDir, thatDir)
                    ==(RD_RIGHT|RD_LEFT))
                {
                    // 相手が右から進入する場合は交錯しない
                    // 相手が左から進入する場合も交錯しないはず
                    result = false;
                }
                else
                {
                    result = true;
                }
            }
            else if (other->blinker().isRight()) {
                // 自分->右折 相手->右折
                if (inter->relativeDirection(thisDir, thatDir)==RD_LEFT)
                {
                    // 相手が左から進入する場合のみ譲る（左方優先）
                    result = true;
                }
                else
                {
                    result = false;
                }
            }
        }

        // 道路交通法第三十六条2
        /*
         * 車両等は，交通整理の行なわれていない交差点においては，
         * その通行している道路が優先道路である場合を除き，
         * 交差道路が優先道路であるとき、
         * 又はその通行している道路の幅員よりも
         * 交差道路の幅員が明らかに広いものであるときは、
         * 当該交差道路を通行する車両等の進行妨害をしてはならない。
         */
        else if (inter->numIn(thisDir)+inter->numOut(thisDir)
                 > inter->numIn(thatDir)+inter->numOut(thatDir)+4)
        {
            // 本来は厳密に道幅を定義すべき
            result = false;
        }
        else if (inter->numIn(thisDir)+inter->numOut(thisDir)+4
                 < inter->numIn(thatDir)+inter->numOut(thatDir))
        {
            // 本来は厳密に道幅を定義すべき
            result = true;
        }

        // 道路交通法第三十六条1
        /*
         * 車両等は，交通整理の行なわれていない交差点においては，
         * 次項の規定が適用される場合を除き、次の各号に掲げる区分に従い，
         * 当該各号に掲げる車両等の進行妨害をしてはならない．
         * 一 車両である場合
         *    交差道路を左方から進行してくる車両及び
         *    交差道路を通行する路面電車
         * 二 路面電車である場合
         *    交差道路を左方から進行してくる路面電車
         *
         * (修正...法律的に正しいかは不明)
         * 「左方から進行」ではなく，「左方から直進」とする．
         * これにより直進優先が左方優先に勝る．
         */
        else if (inter->relativeDirection(thisDir, thatDir)==RD_LEFT)
        {
            if (other->blinker().isNone() &&
                turning!=RD_LEFT)
            {
                // 自分->直進or左折 相手->直進
                result = true;
            }
            else
            {
                result = false;
            }
        }

        // その他、直進優先（法律には明記されていない）
        else
        {
            if (other->blinker().isNone() && turning!=RD_STRAIGHT)
            {
                result = true;
            }
            else
            {
                result =  false;
            }
        }
    }

    return result;
}
