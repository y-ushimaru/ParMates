/* **************************************************
 * Copyright (C) 2014 ADVENTURE Project
 * All Rights Reserved
 **************************************************** */
#include "LocalLaneRouter.h"
#include "Lane.h"
#include "Random.h"
#include "RelativeDirection.h"
#include "RoadOccupant.h"
#include "Vehicle.h"
#include "Signal.h"
#include "TimeManager.h"
#include "Random.h"
#include <algorithm>
#include <cassert>
#ifdef INCLUDE_TRAMS
#include "tram/VehicleTram.h"
#include "tram/TramLaneInIntersection.h"
#include "tram/TramLaneInSection.h"
#endif //INCLUDE_TRAMS

//#define DEBUG_LOCALROUTER

using namespace std;

//======================================================================
LocalLaneRouter::LocalLaneRouter()
{
    _roadMap = NULL;
    _route = NULL;
    _localRoute = NULL;
    _isSearched = false;
    _isSearchingSideLanes = false;
    _shiftDirection = 0;
}

//======================================================================
LocalLaneRouter::~LocalLaneRouter(){}

//======================================================================
void LocalLaneRouter::setRoadMap(RoadMap* roadMap)
{
    assert(roadMap != NULL);
    _roadMap = roadMap;
}

//======================================================================
void LocalLaneRouter::setVehicle(Vehicle* vehicle)
{
    assert(vehicle);
    _vehicle = vehicle;
}

//======================================================================
void LocalLaneRouter::setRoute(Route* route)
{
    assert(route != NULL);
    _route = route;
}

//======================================================================
void LocalLaneRouter::setLocalRoute(LocalLaneRoute* localRoute)
{
    assert(localRoute != NULL);
    _localRoute = localRoute;
}

//======================================================================
bool LocalLaneRouter::isSearched() const
{
    return _isSearched;
}

//======================================================================
bool LocalLaneRouter::isSearchingSideLanes() const
{
    return _isSearchingSideLanes;
}

//======================================================================
int LocalLaneRouter::shiftDirection() const
{
    return _shiftDirection;
}

//======================================================================
void LocalLaneRouter::clear()
{
    _isSearched = false;
    _isSearchingSideLanes = false;
    _shiftDirection = 0;
    if (_localRoute)
        _localRoute->clear();
}

//======================================================================
void LocalLaneRouter::localReroute(const Section* section,
				   const Lane* lane,
				   const double length)
{
    assert(section->isMyLane(lane));
    vector<Lane*> lanes0;
    vector<Lane*> lanes1;
    _localRoute->setTurning(RD_NONE);
    _localRoute->setMainLaneInIntersection(NULL);
    _localRoute->clearLanesInIntersection();
    _isSearched = _search(section, lane, length, &lanes0, & lanes1);
    _localRoute->setLocalRoute(lanes0);
    _localRoute->setDesiredLocalRoute(lanes1);
}

//======================================================================
void LocalLaneRouter::localRerouteSide(const Section* section,
				       const Lane* lane,
				       const double length)
{
    // 先に直接到達可能なレーンが探索済みでなければならない
    assert(section->isMyLane(lane));
    assert(!_localRoute->empty());
    vector<Lane*> lanes;
    bool isSearched = _searchSide(section, lane, length, &lanes);  
    if (isSearched)
    {
        _localRoute->setDesiredLocalRoute(lanes);
    }
}

//======================================================================
bool LocalLaneRouter::_search(const Section* section,
			      const Lane* lane,
			      const double length,
			      std::vector<Lane*>* result_lanes0,
			      std::vector<Lane*>* result_lanes1)
{
    bool straightFound = false;
    result_lanes0->clear();
    result_lanes1->clear();

    // 目前の交差点
    const Intersection* frontIntersection =
        section->intersection(section->isUp(lane));
    assert(frontIntersection != NULL);

    // 通過した交差点
    const Intersection* backIntersection =
        section->intersection(!section->isUp(lane));
    assert(backIntersection != NULL);

    // 目前の交差点の次に目指している交差点
    const Intersection* nextIntersection = NULL;
    if (_route->size() > 1)
    {
        nextIntersection
            = _route->next(backIntersection, frontIntersection);
    }
    
    // 次に通行する単路（frontIntersection->nextIntersection）が短い場合には
    // あらかじめ車線変更する必要がある
    const Intersection* nextIntersection2 = NULL;
    if (_route->size() > 2
        && nextIntersection!=NULL
        && frontIntersection
        ->center().distance(nextIntersection->center())<70)
    {
        nextIntersection2
            = _route->next(frontIntersection, nextIntersection);
    }

    // 目前の交差点で向かう境界番号
    int objectiveDirection = -1;
    // nextIntersectionがないということは、終端の手前まで来ている場合。
    if (nextIntersection == NULL)
    {
        objectiveDirection = 1;
    }
    else
    {
        objectiveDirection = frontIntersection->direction(nextIntersection);
    }

    // さらに先の交差点で向かう境界番号
    int objectiveDirection2 = -1;
    if (nextIntersection2!=NULL)
    {
        objectiveDirection2 = nextIntersection->direction(nextIntersection2);
    }

    vector<Lane*> resultLanes;
   
    resultLanes =
        _straightSearch(section, lane, length,
                        const_cast<Intersection*>(frontIntersection),
                        objectiveDirection, objectiveDirection2, false);
  
    if (!resultLanes.empty())
    {
        // 直接到達可能
        assert(resultLanes.front() == lane);
        result_lanes0->swap(resultLanes);
        straightFound = true;
        _isSearchingSideLanes = false;
    }
    else
    {
#ifdef INCLUDE_TRAMS
        // 路面電車は車線変更しない
        if (dynamic_cast<VehicleTram*>(_vehicle))
        {
            _isSearchingSideLanes = false;
        }
        else
        {
            _isSearchingSideLanes = true;
        }
#else //INCLUDE_TRAMS not defined
        _isSearchingSideLanes = true;
#endif //INCLUDE_TRAM
    }

    // 目前の交差点がODNodeであれば車線変更は必要ない
    if (dynamic_cast<ODNode*>
        (const_cast<Intersection*>(frontIntersection)))
    {
        return straightFound;
    }

    if (_isSearchingSideLanes)
    {
        // 直接到達不可能な場合
        vector<Lane*> sideLanes;
        vector<Lane*>::iterator where;
        vector<double> sideLength;
        vector<double>::iterator how;
    
        // 現在の位置の真横のレーンを取得
        if(_getSideLanes(&sideLanes, &sideLength, section, lane, length))
        {
            where = sideLanes.begin();
            how = sideLength.begin();
      
#ifdef DEBUG_LOCALROUTER
            cout << "search side lanes.";
            for (unsigned int i=0; i<sideLanes.size(); i++)
            {
                cout << sideLanes[i]->id() << ",";
            }
            cout << endl;
#endif

            while (where!= sideLanes.end() && how != sideLength.end())
            {
                resultLanes = 
                    _straightSearch(section, (*where), (*how),
                                    const_cast<Intersection*>(frontIntersection),
                                    objectiveDirection, objectiveDirection2, true);
                
                if (!resultLanes.empty())
//                    && (result_lanes0->empty()
//                    || _isUncrowdedLane(section, (*where), (*how), lane, length)))
                {
                    // 候補が見つかった
                    assert(resultLanes.front() == *where);
                    _isSearchingSideLanes = false;
	
                    // 現在のレーンのどちら側か判定
                    if (lane->lineSegment().isLeftSide(resultLanes.front()
                                                       ->beginConnector()->point()))
                    {
                        _shiftDirection = 1;
                    }
                    else
                    {
                        _shiftDirection = -1;
                    }
	
                    result_lanes1->swap(resultLanes);
                    break;
                }
                where++;
                how++;
            }
        }
    }
  
    // 直接到達経路が見つかっていない場合，
    // 全ての分岐点で直進するものに仮設定する
    /*
     * 車線変更できない場合にはこの経路を走行する
     */
    if (!straightFound)
    {
        vector<Lane*> straightLanes;
        straightLanes = _alternativeSearch(section, lane, length,
                                           const_cast<Intersection*>(frontIntersection));
        if (!straightLanes.empty())
        {
            assert(straightLanes.front() == lane);
            result_lanes0->swap(straightLanes);
        }
    }

    // 実際に走行する経路のうち交差点内レーンのみ抜粋
    for (unsigned int i=0; i<result_lanes0->size(); i++)
    {
        if (frontIntersection->isMyLane((*result_lanes0)[i]))
        {
            _localRoute->addLaneInIntersection((*result_lanes0)[i]);
        }
    }
  
    return straightFound;
}

//======================================================================
bool LocalLaneRouter::_searchSide(const Section* section,
				  const Lane* lane,
				  const double length,
				  vector<Lane*>* result_lanes)
{
#ifdef INCLUDE_TRAMS
    // 路面電車は車線変更しない
    if (dynamic_cast<VehicleTram*>(_vehicle))
    {
        return false;
    }
#endif //INCLUDE_TRAM

    bool isFound = false;
    result_lanes->clear();

    // 目前の交差点
    const Intersection* frontIntersection =
        section->intersection(section->isUp(lane));
    assert(frontIntersection != NULL);

    // 目前の交差点がODNodeがあれば車線変更は必要ない
    if (dynamic_cast<ODNode*>
        (const_cast<Intersection*>(frontIntersection)))
        return false;

    // 通過した交差点
    const Intersection* backIntersection =
        section->intersection(!section->isUp(lane));
    assert(backIntersection != NULL);

    // 目前の交差点の次に目指している交差点
    const Intersection* nextIntersection = NULL;
    if (_route->size() > 1)
    {
        nextIntersection
            = _route->next(backIntersection, frontIntersection);
    }

    // 次に通行する単路（frontIntersection->nextIntersection）が短い場合には
    // あらかじめ車線変更する必要がある
    const Intersection* nextIntersection2 = NULL;
    if (_route->size() > 2
        && nextIntersection!=NULL
        && frontIntersection->center().distance(nextIntersection->center())<70)
    {
        nextIntersection2
            = _route->next(frontIntersection,nextIntersection);
    }

    // 目前の交差点で向かう境界番号
    int objectiveDirection = -1;
    // nextIntersectionがないということは、終端の手前まで来ている場合
    if (nextIntersection == NULL)
    {
        objectiveDirection = 1;
    }
    else
    {
        objectiveDirection = frontIntersection->direction(nextIntersection);
    }

    // さらに先の交差点で向かう境界番号
    int objectiveDirection2 = -1;
    if (nextIntersection2!=NULL)
    {
        objectiveDirection2 = nextIntersection->direction(nextIntersection2);
    }

    vector<Lane*> resultLanes;
    vector<Lane*> sideLanes;
    vector<Lane*>::iterator where;
    vector<double> sideLength;
    vector<double>::iterator how;
    
    // 現在の位置の真横のレーンを取得
    if(_getSideLanes(&sideLanes, &sideLength, section, lane, length))
    {
        where = sideLanes.begin();
        how = sideLength.begin();
      
#ifdef DEBUG_LOCALROUTER
        cout << "search side lanes in _searchSide().";
        for (unsigned int i=0; i<sideLanes.size(); i++)
        {
            cout << sideLanes[i]->id() << ",";
        }
        cout << endl;
#endif

        while (where!= sideLanes.end() && how != sideLength.end())
        {
      
            resultLanes = 
                _straightSearch(section, (*where), (*how),
                                const_cast<Intersection*>(frontIntersection),
                                objectiveDirection, objectiveDirection2, true);
            if (!resultLanes.empty()
                && _isUncrowdedLane(section, (*where), (*how), lane, length))
            {
                // 候補が見つかった
                assert(resultLanes.front() == *where);
                _isSearchingSideLanes = false;
                isFound = true;
	
                // 現在のレーンのどちら側か判定
                if (lane->lineSegment().isLeftSide(resultLanes.front()
                                                   ->beginConnector()->point()))
                {
                    _shiftDirection = 1;
                }
                else
                {
                    _shiftDirection = -1;
                }
	
                result_lanes->swap(resultLanes);
                break;
            }
            where++;
            how++;
        }
    }
  
    return isFound;
}

//======================================================================
bool LocalLaneRouter::_getSideLanes(vector<Lane*>* result_Lanes,
				    vector<double>* result_Length,
				    const Section* section,
				    const Lane* lane,
				    const double length) const
{
  
    // 左レーンを取得
    _getLeftSideLanes(result_Lanes, result_Length,
                      section, lane, length);

    // 右レーンを取得
    _getRightSideLanes(result_Lanes, result_Length,
                       section, lane, length);

    return ((*result_Lanes).size()!=0);
}

//======================================================================
void LocalLaneRouter::_getLeftSideLanes(vector<Lane*>* result_Lanes,
					vector<double>* result_Length,
					const Section* section,
					const Lane* lane,
					const double length) const
{
    // 再帰的に左のレーンの左のレーンの...を求める
    Lane* sideLane = NULL;
    double sideLength = 0;
    lane->getSideLaneLength(length, true, &sideLane, &sideLength);
    if (sideLane != NULL)
    {
#ifdef INCLUDE_TRAMS
        // 路面電車レーンは車線変更の対象とせず，探索を終了する
        if (dynamic_cast<TramLaneInSection*>(sideLane)
            || dynamic_cast<TramLaneInIntersection*>(sideLane))
        {
            return;
        }
#endif //INCLUDE_TRAMS

        (*result_Lanes).push_back(sideLane);
        (*result_Length).push_back(sideLength);
        _getLeftSideLanes(result_Lanes, result_Length,
                          section, sideLane, sideLength);
    }
}

//======================================================================
void LocalLaneRouter::_getRightSideLanes(vector<Lane*>* result_Lanes,
					 vector<double>* result_Length,
					 const Section* section,
					 const Lane* lane,
					 const double length) const
{
    // 再帰的に右のレーンの右のレーンの...を求める
    Lane* sideLane = NULL;
    double sideLength = 0;
    lane->getSideLaneLength(length, false, &sideLane, &sideLength);
    if (sideLane != NULL)
    {
#ifdef INCLUDE_TRAMS
        // 路面電車レーンは車線変更の対象とせず，探索を終了する
        if (dynamic_cast<TramLaneInSection*>(sideLane)
            || dynamic_cast<TramLaneInIntersection*>(sideLane))
        {
            return;
        }
#endif //INCLUDE_TRAMS

        (*result_Lanes).push_back(sideLane);
        (*result_Length).push_back(sideLength);
        _getRightSideLanes(result_Lanes, result_Length,
                           section, sideLane, sideLength);
    }
}

//======================================================================
vector<Lane*> LocalLaneRouter::_straightSearch(const Section* section,
					       const Lane* startLane,
					       const double length,
					       Intersection* frontIntersection,
					       int objectiveDirection,
					       int objectiveDirection2,
					       bool includeShiftLane) const
{
    vector<Lane*> exitLanes;
    if(_getExitLanesInStraightRoute(&exitLanes,
                                    const_cast<Section*>(section),
                                    const_cast<Lane*>(startLane),
                                    frontIntersection,
                                    objectiveDirection,
                                    objectiveDirection2,
                                    includeShiftLane))
    {
        assert(!exitLanes.empty()); //かならず開始レーンが含まれるはず
    }
    else
    {
        exitLanes.clear();
    }
    return exitLanes;
}

//======================================================================
bool LocalLaneRouter::_getExitLanesInStraightRoute(vector<Lane*>* result_lanes,
						   Section* section,
						   Lane* startLane,
						   Intersection* frontIntersection,
						   int objectiveDirection,
						   int objectiveDirection2,
						   bool includeShiftLane) const
{
#ifdef DEBUG_LOCALROUTER
    cout << "Section:" << section->id()
         << " Front:"  << frontIntersection->id() 
         << " Dir:"   << objectiveDirection
         << " Lane:" << startLane->id() << "-";
    for (unsigned int i=0; i<result_lanes->size(); i++)
        cout << (*result_lanes)[i]->id() << ",";
    cout << endl;
#endif  

    bool result = false;
    if(section->isNextLaneMine(startLane))
    {
        // 次のレーンもsectionの中にいる
        int loopMax = startLane->nextLanes()->size();
        //探索順序をランダムにすることでランダム選択可能にする．
        vector<int> order = Random::randomOrder(_vehicle->randomGenerator(),
                                                loopMax);
        // 次のレーンを順に探索する
        for(int i=0; i < loopMax && !result; i++)
        {
            result_lanes->push_back(startLane);
            Lane* nextLane = startLane->nextLane(order[i]);
            result = _getExitLanesInStraightRoute(result_lanes, 
                                                  section,
                                                  nextLane,
                                                  frontIntersection,
                                                  objectiveDirection,
                                                  objectiveDirection2,
                                                  includeShiftLane); //再帰
            if(!result)
            {
                // 探索失敗
                (*result_lanes).pop_back();

#ifdef DEBUG_LOCALROUTER
                cout << "Section:" << section->id()
                     << " Front:"  << frontIntersection->id() 
                     << " Dir:"   << objectiveDirection
                     << " Lane:" << startLane->id() << "-";
                for (unsigned int i=0; i<result_lanes->size(); i++)
                {
                    cout << (*result_lanes)[i]->id() << ",";
                }
                cout << endl;
#endif

                // このレーンを辿っても無駄なのでひとつ戻る
            }
        }
    }
    else
    {
        // 次のレーンが交差点までたどり着いた
        int loopMax = startLane->nextLanes()->size();
        // 探索順序をランダムにすることでランダム選択可能にする．
        vector<int> order = Random::randomOrder(_vehicle->randomGenerator(),
                                                loopMax);
        // 次のレーンを順に探索する
        for (int i=0; i<loopMax && !result; i++)
        {
            result_lanes->push_back(startLane);
            Lane* nextLane = startLane->nextLane(order[i]);

            if (dynamic_cast<ODNode*>(frontIntersection)!=NULL)
            {
                // frontIntersectionがODノードであれば
                // 探索成功（他の経路はとりようがない）
                result = true;
                (*result_lanes).push_back(nextLane);
                break;
            }
            result = _getExitLanesInFrontIntersection(result_lanes,
                                                      frontIntersection,
                                                      nextLane,
                                                      objectiveDirection,
                                                      objectiveDirection2,
                                                      includeShiftLane);
            if (!result)
            {
                // 探索失敗
                (*result_lanes).pop_back();

#ifdef DEBUG_LOCALROUTER
                cout << "Section:" << section->id()
                     << " Front:"  << frontIntersection->id() 
                     << " Dir:"   << objectiveDirection
                     << " Lane:" << startLane->id() << "-";
                for (unsigned int i=0; i<result_lanes->size(); i++)
                {
                    cout << (*result_lanes)[i]->id() << ",";
                }
                cout << endl;
#endif

                // このレーンを辿っても無駄なのでひとつ戻る
            }
        }
    }
    return result;
}

//======================================================================
bool LocalLaneRouter::_getExitLanesInFrontIntersection(vector<Lane*>* result_lanes,
						       Intersection* intersection,
						       Lane* startLane,
						       int objectiveDirection,
						       int objectiveDirection2,
						       bool includeShiftLane) const
{
#ifdef DEBUG_LOCALROUTER
    cout << "Intersection:" << intersection->id()
         << "  Front:------"
         << " Dir:"   << objectiveDirection
         << " Lane:" << startLane->id() << "-";
    for (unsigned int i=0; i<result_lanes->size(); i++)
        cout << (*result_lanes)[i]->id() << ",";
    cout << endl;
#endif

    bool result = false;
    if (intersection->isNextLaneMine(startLane))
    {
        // 次のレーンも交差点の中にいる
        int loopMax = startLane->nextLanes()->size();
        // 探索順序をランダムにすることでランダム選択可能にする．
        vector<int> order = Random::randomOrder(_vehicle->randomGenerator(),
                                                loopMax);
        // 次のレーンを順に探索する
        for (int i=0; i<loopMax && !result; i++ )
        {
            result_lanes->push_back(startLane);
            if (intersection->isMainLane(startLane) && !includeShiftLane)
            {
                _localRoute->setTurning(intersection->relativeDirection(startLane));
                _localRoute->setMainLaneInIntersection(startLane);
            }
            Lane* nextLane = startLane->nextLane(order[i]);
            result = _getExitLanesInFrontIntersection(result_lanes,
                                                      intersection,
                                                      nextLane,
                                                      objectiveDirection,
                                                      objectiveDirection2,
                                                      includeShiftLane);
            if (!result)
            {
                // 探索失敗
                (*result_lanes).pop_back();
                if (intersection->isMainLane(startLane) && !includeShiftLane)
                {
                    _localRoute->setMainLaneInIntersection(NULL);
                    _localRoute->setTurning(RD_NONE);
                }

#ifdef DEBUG_LOCALROUTER
                cout << "Intersection:" << intersection->id()
                     << " Dir:"   << objectiveDirection
                     << " Lane:" << startLane->id() << "-";
                for (unsigned int i=0; i<result_lanes->size(); i++)
                {
                    cout << (*result_lanes)[i]->id() << ",";
                }
                cout << endl;
#endif
                // このレーンを辿っても無駄なのでひとつ戻る
            }
        }
    }
    else
    {
        // 交差点の最後のレーンに到達した
        if (intersection->direction(startLane->endConnector())==objectiveDirection)
        {
            result_lanes->push_back(startLane);
            if (intersection->isMainLane(startLane) && !includeShiftLane)
            {
                _localRoute->setTurning(intersection->relativeDirection(startLane));
                _localRoute->setMainLaneInIntersection(startLane);
            }
            if (objectiveDirection2==-1)
            { 
                result = true;
                int num = startLane->nextLanes()->size();
                (*result_lanes).push_back
                    (startLane->nextLane(Random::uniform(_vehicle->randomGenerator(),
                                                         num)));
            }
            else
            {
                // さらに先のレーンで希望する方向に進行できるか判断する
                int loopMax = startLane->nextLanes()->size();
                vector<int> order = Random::randomOrder(_vehicle->randomGenerator(),
                                                        loopMax);
                for (int i=0; i<loopMax && !result; i++)
                {
                    Lane* nextLane = startLane->nextLane(order[i]);
                    if (intersection->nextSection(objectiveDirection)
                        ->isReachable(nextLane, objectiveDirection2))
                    {
                        result = true;
                        (*result_lanes).push_back(nextLane);
                    }
                }
                if (!result)
                {
                    // 探索失敗
                    (*result_lanes).pop_back();
                    if (intersection->isMainLane(startLane) && !includeShiftLane)
                    {
                        _localRoute->setMainLaneInIntersection(NULL);
                        _localRoute->setTurning(RD_NONE);
                    }
                }
            }
        }
        else
        {
            result = false;
        }
    }
    return result;
}

//======================================================================
vector<Lane*> LocalLaneRouter::_alternativeSearch(const Section* section,
						  const Lane* lane,
						  const double length,
						  Intersection* frontIntersection) const
{
    vector<Lane*> exitLanes;
    Lane* startLane = const_cast<Lane*>(lane);
    exitLanes.push_back(startLane);

    // 単路内の探索
    while (section->isNextLaneMine(startLane))
    {
        startLane = startLane->nextStraightLane();
        assert(startLane);
        exitLanes.push_back(startLane);
    }
    startLane = startLane->nextStraightLane();
    assert(startLane);
    if (frontIntersection->isMainLane(startLane))
    {
        _localRoute->setTurning(frontIntersection->relativeDirection(startLane));
        _localRoute->setMainLaneInIntersection(startLane);
    }
    exitLanes.push_back(startLane);

    // 交差点内の探索
    while (frontIntersection->isNextLaneMine(startLane))
    {
        startLane = startLane->nextStraightLane();
        assert(startLane);
        if (frontIntersection->isMainLane(startLane))
        {
            _localRoute->setTurning(frontIntersection->relativeDirection(startLane));
            _localRoute->setMainLaneInIntersection(startLane);
        }
        exitLanes.push_back(startLane);
    }
    int num = startLane->nextLanes()->size();
    startLane = startLane->nextLane(Random::uniform(_vehicle->randomGenerator(),
                                                    num));
    assert(startLane);
    exitLanes.push_back(startLane);

    return exitLanes;
}

//======================================================================
bool LocalLaneRouter::_isUncrowdedLane(const Section* section,
				       const Lane* thatLane,
				       const double thatLength,
				       const Lane* thisLane,
				       const double thisLength) const
{
    assert(section && thisLane && thatLane);

    Intersection* inter = section->intersection(section->isUp(thisLane));

    // ODノードならfalse
    if (dynamic_cast<ODNode*>(inter)!=NULL)
    {
        return false;
    }

    // 前方が赤信号ならfalse
    int dir = inter->direction(section);
    if ((inter->signal()->permission(dir))==Signal::PROHIBITION)
    {
        return false;
    }

    // 先行車がいなければfalse
    RoadOccupant* thisFront
        = const_cast<Lane*>(thisLane)->frontAgent(thisLength);
    if (thisFront==NULL)
    {
        return false;
    }

    RoadOccupant* thatFront
        = const_cast<Lane*>(thatLane)->frontAgent(thatLength);
    /*
     * ここの判断は暫定的なものなので要検討
     *
     * 隣レーンに先行車がいない場合には，自レーンの先行車が減速中であればtrue
     * 隣レーンに先行車がいる場合には，車間距離と速度が自レーンより大きければtrue
     */
    if (thatFront==NULL)
    {
        if (thisFront->accel()<0
            || thisFront->velocity() < 1.0e-6)
        { 
            return true;
        }
        else
        {
            return false;
        }
    }
    else
    {
        if (thatFront->velocity()-thisFront->velocity() > 5.0/60/60
            && thatFront->length()-thatFront->bodyLength()/2-thatLength
            > thisFront->length()-thisFront->bodyLength()/2-thisLength)
        {

#ifdef DEBUG_LOCALROUTER
            cout << section->id() << endl
                 << " this:" << thisLane->id() << ":" << thisLength << endl
                 << " that:" << thatLane->id() << ":" << thatLength << endl
                 << " frt1:" << thisFront->id() << endl
                 << " frt2:" << thatFront->id() << endl;
#endif
            return true;
        }
        else
        {
            return false;
        }
    }
}
 
//======================================================================
bool LocalLaneRouter::_isUncrowdedRoute(
    const std::vector<Lane*>* route0,
    const std::vector<Lane*>* route1) const
{
    return false;
}
//======================================================================
bool LocalLaneRouter::_isUncrowdedRoute(const Section* section,
					const Intersection* intersection,
					const Lane* thatLane,
					const Lane* thisLane) const
{
    assert(section && intersection && thatLane && thisLane);
    return false;

#ifdef THIS_IS_OLD_CODE
    // 単路の先頭に停止中or減速中の車両がいなければfalse
    Lane* headLane = const_cast<Lane*>(thisLane);
    while (headLane && section->isNextLaneMine(headLane))
    {
        headLane = _localRoute->next(headLane);
    }
    if (headLane==NULL
        || headLane->headAgent()==NULL
        || (headLane->headAgent()->velocity()<1.0e-6
            || headLane->headAgent()->accel()<0))
    {
        return false;
    }

    int from = intersection->direction(section);
    double thatPassTime = 0.0;
    double thisPassTime = 0.0;

    for (int i=0; i<intersection->numNext(); i++)
    {
        if (section->isReachable(thatLane, i))
        {
            thatPassTime += intersection->averagePassTime(from, i);
        }
        if (section->isReachable(thisLane, i))
        {
            thisPassTime += intersection->averagePassTime(from, i);
        }
    }

    // thisPassTimeが0 = その経路を走行するのはその車両が初めて = 車線変更必要無し
    if (thisPassTime==0)
    {
        return false; 
    }

    if (thisPassTime>thatPassTime)
    {
        double r = Random::uniform(_vehicle->randomGenerator());
        // 隣レーンが空いていれば空いている程高確率で車線変更
        if (r<(thisPassTime-thatPassTime)/thisPassTime)
        {
            return true;
        }
    }
    return false;
#endif // THIS_IS_OLD_CODE
}
