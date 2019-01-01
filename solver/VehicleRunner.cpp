/* **************************************************
 * Copyright (C) 2014 ADVENTURE Project
 * All Rights Reserved
 **************************************************** */
#include "Vehicle.h"
#include "Lane.h"
#include "Section.h"
#include "Intersection.h"
#include "GVManager.h"
#include "ParallelConf.h"
#include <cassert>
#include <chrono>

using namespace std;

//======================================================================
void Vehicle::run()
{
    // 行動時間の計測
    double start, end;
    if(TimeManager::time()%MEASURE_INTERVAL == 0)
    {
      start = getTime();
    }

    _isLanePassed  = false;
    _isNotifySet   = false;
    _isUnnotifySet = false;

    // スリープタイマのカウントダウン
    if (_sleepTime>0)
    {
        _sleepTime -= TimeManager::unit();
    }
    //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    // レーン方向の位置（レーン始点からの距離）の更新
    double length = _velocity * TimeManager::unit();
    _oldLength   =  _length;
    _length      += length;
    _totalLength += length;
    _tripLength  += length;

    //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    // 車線変更の処理
    /**
     * レーン垂直方向の位置の更新を含む
     */
    if (_laneShifter.isActive())
    {
        _error += _errorVelocity * TimeManager::unit();

        _laneShifter.proceedShift();

        // 十分に横に移動したら車線変更処理を終了する
        if (abs(_error) >= _section->laneWidth())
        {
            _laneShifter.endShift();
        }
    }
    else if (_section && _laneShifter.canShift())
    {
        _laneShifter.beginShift();
    }

    //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    // レーンへの登録処理
    if (_length < _lane->length())
    {
        //--------------------------------------------------------------
        // レーンの終点に達していない場合
        // 同一レーンに登録する
        _lane->putAgent(this);
        _lane->setUsed();

        // 単路にいる場合
        // 次の交差点まで50m以内で，次に右左折するときはウィンカーを点ける
        if (_section!=NULL)
        {

            if (_section->lengthToNext(_lane, _length)<=50
                && (_blinker.turning() != _localRoute.turning()))
            {
                // 交差点拡張方向を導入した場合は要修正
                if (_localRoute.turning()==RD_LEFT)
                {
                    _blinker.setLeft();
                }
                else if (_localRoute.turning()==RD_RIGHT)
                {
                    _blinker.setRight();
                }
                else
                {
                    _blinker.setNone();
                }
            }
        }
    }
    else
    {

        //--------------------------------------------------------------
        // 次のレーンに移る
        _lane->setUsed();
        _nextLane->setUsed();

        // 次に所属するレーンを基準にするので，oldLengthはマイナス
        _oldLength -= _lane->length();
        _length    -= _lane->length();

        // _lane, _intersection, _sectionの更新
        if (_section==NULL)
        {
            // 交差点を走行中の場合
            assert(_intersection!=NULL);

            // 一旦停止フラグの解除
            _hasPaused = false;

            if (_intersection->isMyLane(_nextLane))
            {
                // 交差点内の次のレーンへ
                _runIntersection2Intersection();
            }
            else
            {
                // 交差点から単路へ
                _totalLength = 0;
                _runIntersection2Section();

                // ウィンカーを消す
                if (_blinker.isLeft() || _blinker.isRight())
                {
                    _blinker.setNone();
                }
            }
        }
        else
        {
            // 単路を走行中の場合
            if (_section->isMyLane(_nextLane))
            {
                // 単路内の次のレーンへ
                _runSection2Section();

            }
            else
            {
                // 単路から交差点へ
                _totalLength = 0;
                _runSection2Intersection();
            }

        }
        _lane->putAgent(this);
        _lane->setLastArrivalTime(TimeManager::time());
    }

    // 行動時間の計測
    if(TimeManager::time()%MEASURE_INTERVAL == 0)
    {
       end = getTime();
       _calcTime += end-start;
    }
}

//======================================================================
void Vehicle::_runIntersection2Intersection()
{
    _prevLane = _lane;
    _lane     = _nextLane;

    _decideNextLane(_intersection, _lane);
}

//======================================================================
void Vehicle::_runIntersection2Section()
{
    _section = _intersection->nextSection(_lane);
    assert(_section);

    // _intersectionに交差点通過時間を通知
    int from = _intersection->direction(_prevIntersection);
    int to   = _intersection->direction(_section);
    assert(0<=from && from<_intersection->numNext());
    assert(0<=to && to<_intersection->numNext());

    _intersection
        ->addPassTime(from, to, TimeManager::time()-_entryTime);
    _entryTime = TimeManager::time();

    // _watchedVehiclesの登録状況の受渡し
    /*
     * 本来は車線変更中にレーン束を移ることはないはずだが...
     */
    if (_isNotifying)
    {
        _intersection->eraseWatchedVehicle(this);
        _section->addWatchedVehicle(this);
    }

    // 車線変更に失敗したとき場合などのために
    // 必要であればここでルートを再探索する。
    if(_prevIntersection != NULL)
    {
        Intersection* next
            = _intersection
            ->next(_intersection->direction(_section));
        if (dynamic_cast<ODNode*>(next)==NULL
            && _route ->next(_intersection, next)==NULL)
        {
            // cout << "vehicle: " << _id
            //      << ", rerouting after passing intersection: "
            //      << _intersection->id() << endl;
            TimeManager::startClock("REROUTE");

            // reroute(_intersection,
            //        _section->anotherIntersection(_intersection));

            // rerouteの代わりに次の交差点を代入する
            _route->resetRoute();
            _route->addIntersection(_intersection);
            const vector<const Intersection*>* gates = _route->gates();
            _route->addIntersection((*gates)[gates->size()-1]);

            _route->resetLastPassedIntersectionIndex();
            _route->setLastPassedIntersection(_intersection);
            TimeManager::stopClock("REROUTE");
        }
    }

    _prevIntersection = _intersection;
    _intersection = NULL;

    _prevLane = _lane;
    _lane     = _nextLane;

    _localRouter.clear();
    _localRouter.localReroute(_section, _lane, _length);
    _decideNextLane(_section, _lane);
}

//======================================================================
void Vehicle::_runSection2Section()
{
    _prevLane = _lane;
    _lane     = _nextLane;

    if (!_localRouter.isSearched())
    {
        _localRouter.clear();
        _localRouter.localReroute(_section, _lane, _length);
    }
    _decideNextLane(_section, _lane);
}

//======================================================================
void Vehicle::_runSection2Intersection()
{
    _intersection = _section->nextIntersection(_lane);
    assert(_intersection);

    // 車線変更を行いながら交差点に入るとMATESが落ちるのでここでトラップ．
    // とりあえず車線変更を緊急中断
    if (_laneShifter.isActive())
    {
        // cerr << "WARNING: would enter intersection while lane shifting."
        //      << endl
        //      << "vehicle:" << _id
        //      << " in section:" << _section->id() << endl;
        _laneShifter.abortShift();
    }

    //最後に通過した交差点として新しい交差点を登録。
    if(_prevIntersection == NULL)
    {
        _route
            ->setLastPassedIntersection(_intersection);
    }
    else
    {
        _route
            ->setLastPassedIntersection(_prevIntersection,
                                        _intersection);
    }

    //_watchedVehiclesの登録状況の受渡し
    /*
     * 本来は車線変更中にレーン束を移ることはないはずだが...
     */
    if (_isNotifying)
    {
        _section->eraseWatchedVehicle(this);
        if (!(dynamic_cast<ODNode*>(_intersection)))
        {
            // ODノードに入る(次のステップで消える)ときにはaddしない
            _intersection->addWatchedVehicle(this);
        }
    }

    _section = NULL;

    _prevLane = _lane;
    _lane     = _nextLane;

    // ODノードでなければ次のレーンを検索
    if (dynamic_cast<ODNode*>(_intersection)==NULL)
    {
        _decideNextLane(_intersection, _lane);
    }
}

//======================================================================
void Vehicle::_decideNextLane(Intersection* inter, Lane* lane)
{
    // 次のレーン候補の数
    int num = lane->nextLanes()->size();

    if (num)
    {
        /*
         * LocalRouteは交差点を出るまでの経路を持っている
         * 正確には，交差点を出て次の単路に入った
         * 最初のレーンまでを保持している
         */
        if (_localRouter.isSearched())
        {
            _nextLane = _localRoute.next(lane);
        }
        else
        {
            _nextLane = lane->nextStraightLane();
        }
        assert(_nextLane);
    }
    else
    {
        // この後のrerouteに任せる
    }
}

//======================================================================
void Vehicle::_decideNextLane(Section* section, Lane* lane)
{
    // 次のレーン候補の数
    int num = lane->nextLanes()->size();
    if (num!=0)
    {
        if(_localRouter.isSearched())
        {
            _nextLane = _localRoute.next(lane);
        }
        else
        {
            _nextLane = lane->nextStraightLane();
        }
        assert(_nextLane);
    }
}

//======================================================================
bool Vehicle::_setLane(Intersection* inter, Lane* lane, double length)
{
    bool result = false;
    assert(_roadMap!=NULL && inter!=NULL && lane!=NULL);
    if (inter->isMyLane(lane))
    {
        _lane = lane;
        _decideNextLane(inter, lane);
        result = true;
    }
    return result;
}

//======================================================================
bool Vehicle::_setLane(Section* section, Lane* lane, double length)
{
    bool result = false;
    assert(_roadMap!=NULL && section!=NULL && lane!=NULL);
    if (section->isMyLane(lane))
    {
        _localRouter.clear();
        _localRouter.localReroute(section, lane, length);
        _lane = lane;
        _decideNextLane(section, lane);
        assert(_nextLane);
        result = true;
    }
    return result;
}

//======================================================================
bool Vehicle::addToSection(RoadMap* roadMap,
                           Section* section,
                           Lane* lane,
                           double length)
{
    // 車が初めて登場したときにだけ呼ばれる
    bool check = false;
    if (roadMap!=NULL)
    {
        _roadMap = roadMap;
        _section = section;
        _prevIntersection = section->intersection(!(section->isUp(lane)));
        _length = length;
        _entryTime = TimeManager::time();

        check = _setLane(section, lane, length);
        _prevLane = _lane;

#ifdef GENERATE_VEHICLE_VELOCITY_ZERO
        _velocity = 0;
#else //GENERATE_VEHICLE_VELOCITY_ZERO not defined
        if (lane->tailAgent())
        {
            _velocity = lane->tailAgent()->velocity();
        }
        else
        {
            _velocity = lane->speedLimit()/60/60;
        }
#endif //GENERATE_VEHICLE_VELOCITY_ZERO

        double limit
            = GVManager::getNumeric("GENERATE_VELOCITY_LIMIT")/60/60;
        if (limit >= 0.0 && _velocity > limit)
        {
            _velocity = limit;
        }
    }
    assert(check);
    lane->setUsed();
    return check;
}
