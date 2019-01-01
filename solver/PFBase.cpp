/* **************************************************
 * Copyright (C) 2014 ADVENTURE Project
 * All Rights Reserved
 **************************************************** */
#include "PFBase.h"
#include "PFLogger.h"
#include "Route.h"
#include "RouteStorage.h"
#include "RouteComponentKeyBase.h"
#include "RoadMap.h"
#include "Section.h"
#include "Intersection.h"
#include "RoutingManager.h"
#include "RoutingNetwork.h"
#include "RoutingNode.h"
#include "RoutingLink.h"
#include "VehicleFamily.h"
#include "GVManager.h"
#include "Random.h"
#include "Conf.h"
#include <vector>
#include <cassert>
#include <fstream>

using namespace std;

//======================================================================
PFBase::PFBase(RoutingManager* routingManager)
{
    _startNode = NULL;
    _goalNodes.clear();

    _isInUse = false;
    _resetPFStatus();
    _logger = NULL;

    _routingManager = routingManager;
}

//======================================================================
PFBase::~PFBase()
{
    if (_logger)
    {
        delete _logger;
    }
}

//======================================================================
void PFBase::setId(const string& id)
{
    assert(_isInUse);
    _id = id;
}

//======================================================================
void PFBase::setWeights(const double weights[])
{
    assert(_isInUse);
    for (int i=0; i<VEHICLE_ROUTING_PARAMETER_SIZE; i++)
    {
        _weights[i] = weights[i];
    }
}

//======================================================================
void PFBase::setInUseOn()
{
    _isInUse = true;
}

//======================================================================
void PFBase::setInUseOff()
{
    _isInUse = false;
}

//======================================================================
bool PFBase::isInUse() const
{
    return _isInUse;
}

//======================================================================
void PFBase::setVehicleType(VehicleType type)
{
    assert(_isInUse);
    _vehicleType = type;
}

//======================================================================
void PFBase::addLogger(PFLogger* logger)
{
    _logger = logger;
}

//======================================================================
void PFBase::removeLogger()
{
    _logger = NULL;
}

//======================================================================
PFLogger* PFBase::logger() const
{
    return _logger;
}

//======================================================================
void PFBase::setRandomGenerator(RandomGenerator* rnd)
{
    assert(_isInUse);
    _rnd = rnd;
}

//======================================================================
bool PFBase::generateRouteComponent
(RoadMap* roadMap, vector<string> tokens) const
{
    assert(tokens.size()>5);

    bool isValidLine   = true;
    unsigned int index = 0;

    // 第1カラムは使用回数
    int count = atoi(tokens[index].c_str());
    index++;

    // 第2カラムは車種ID
    VehicleType type = atoi(tokens[index].c_str());
    index++;

    // 第3カラムからは経路探索パラメータ
    double weights[VEHICLE_ROUTING_PARAMETER_SIZE];
    for (unsigned int i=0;
         i<VEHICLE_ROUTING_PARAMETER_SIZE;
         i++)
    {
        weights[i] = atof(tokens[index].c_str());
        index++;
    }

    // 次のカラムは出発地に至る交差点ID
    const Intersection* past
        = roadMap->intersection(tokens[index]);
    index++;

    // 次のカラムは出発地ID
    const Intersection* start
        = roadMap->intersection(tokens[index]);
    index++;

    // 次のカラムは到着地ID
    const Intersection* goal
        = roadMap->intersection(tokens[index]);
    index++;

    // 以降のカラムは経路
    vector<const Intersection*> route;
    route.clear();
    for (;index<tokens.size(); index++)
    {
        const Intersection* inter
            = roadMap->intersection(tokens[index]);
        if (!inter)
        {
            isValidLine = false;
            break;
        }
        route.push_back(inter);
    }

    if (!(past && start && goal && isValidLine))
    {
        return false;
    }

    RouteComponentKeyBase* key
        = new RouteComponentKeyBase(type, weights, past, start, goal);

    const_cast<Intersection*>(past)
        ->routeStorage()
        ->addRouteComponentDirectly(key, count, &route);

    return true;
}

//======================================================================
bool PFBase::search(Route* route,
                    const Intersection* from,
                    const Intersection* next)
{
    const vector<const Intersection*>* gates = route->gates();
    route->resetRoute();

    /*
     * 最初の探索で用いる startNode はfromからnextのへのsectionである．
     * sectionの下流側(next)から経路探索を開始する．
     *
     * 厳密にはstartNodeから出るRoutingLinkから探索を開始し，
     * 最後にRoutingLinkに含まれるIntersectionを返すことになる．
     */

    /*
     * 返ってくるIntersectionのリストの先頭はnextであり
     * fromでないので，まずはfromのみ最初にrouteに格納しておく．
     */
    route->addIntersection(from);

    for(int i=route->lastPassedGateIndex();
        i<static_cast<int>(gates->size())-1;
        i++)
    {
        // gateで分割された探索の始点に至る交差点
        const Intersection* pastIntersection  = NULL;
        // gateで分割された探索の始点
        const Intersection* startIntersection = (*gates)[i];
        // gateで分割された探索の終点
        const Intersection* goalIntersection  = (*gates)[i+1];

        //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
        // スタートノードの決定
        /*
         * 以下の処理の中で経路要素格納器を求めるため，
         * キャッシュの利用よりも先に処理する
         */
        if (i == route->lastPassedGateIndex())
        {
            _startNode
                = _startNodeForSearch(from, next);
            assert(_startNode);
            _routeStorage
                = const_cast<Intersection*>(from)
                ->routeStorage();
        }
        else
        {
            /*
             * route->routeの最後から2番目の要素には，
             * startIntersectionに至る直前のIntersectionが入っている
             */
            pastIntersection
                = (*(route->route()))[route->route()->size()-2];

            /*
             * pastIntersection->startIntersectionの探索は
             * 前回の探索と今回の探索で重複しているため，
             * 前回の探索結果の最後を削除
             */
            if (route->size()>=1
                && (*(route->route()))[route->size()-1]
                == startIntersection)
            {
                route->popBack();
            }

            _startNode
                = _startNodeForSearch(pastIntersection,
                                      startIntersection);
            assert(_startNode);
            _routeStorage
                = const_cast<Intersection*>(pastIntersection)
                ->routeStorage();
        }

        //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
        // キャッシュの利用
        bool isCacheFound = false;
        vector<const Intersection*> cachedInters;

        if (Random::uniform(_rnd)
            < GVManager::getNumeric(
                "VEHICLE_CACHE_ROUTING_PROBABILITY"))
        {
            if (i == route->lastPassedGateIndex())
            {
                isCacheFound
                    = _loadCacheFromRouteStorage
                    (from,
                     next,
                     goalIntersection,
                     &cachedInters);
            }
            else
            {
                isCacheFound
                    = _loadCacheFromRouteStorage
                    (pastIntersection,
                     startIntersection,
                     goalIntersection,
                     &cachedInters);
            }
        }

        /*
         * キャッシュを利用できた場合は探索を行う必要はない
         */
        if (isCacheFound)
        {
            for (unsigned int j=0; j<cachedInters.size(); j++)
            {
                route->addIntersection(cachedInters[j]);
            }
            continue;
        }

        //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
        // ゴールノードの決定
        _findGoalNodes(goalIntersection, gates, i);

        //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
        // 経路探索の実行
        vector<RoutingNode*> results;
        results.clear();

        _search(&results);

        //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
        // 経路探索に失敗した場合
        if (results.empty())
        {
            // writeInvalidODPairCache((*gates)[i]->id(), (*gates)[i+1]->id());

            // cerr << "search route(from:" << (*gates)[i]->id()
            //      << ", to:" << (*gates)[i+1]->id() << ") failed."
            //      << std::endl;
            /*
             * ひとまず手前の交差点とゴールを代入しておく
             * （もちろんこのような経路は存在しない）
             */
            route->resetRoute();
            route->addIntersection(from);
            route->addIntersection((*gates)[gates->size()-1]);
            // route->print(cout);

            // 属性を元に戻す
            _resetPFStatus();

            return false;
        }

        //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
        // routeに結果を格納する
        vector<const Intersection*> searchedInters;
        _convertSearchResult(&searchedInters, &results,
                             goalIntersection);

        for (unsigned int j=0; j<searchedInters.size(); j++)
        {
            route->addIntersection(searchedInters[j]);
        }

        //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
        // 結果を保存する
        if (i == route->lastPassedGateIndex())
        {
            _saveCacheToRouteStorage
                (from, next,
                 searchedInters[searchedInters.size()-1],
                 &searchedInters);
        }
        else
        {
            _saveCacheToRouteStorage
                (pastIntersection, startIntersection,
                 searchedInters[searchedInters.size()-1],
                 &searchedInters);
        }
    }

    // 属性を元に戻す
    _resetPFStatus();
    return true;
}

//======================================================================
const RoutingNode* PFBase::_startNodeForSearch(const Intersection* from,
                                               const Intersection* next)
{

    const Section* section = from->nextSection(next);
    bool isUp = section->isUp(from, next);

    return _routingManager
        ->routingNetwork(0)->convertS2N(section, isUp);
}

//======================================================================
void PFBase::_findGoalNodes
(const Intersection* goal,
 const vector<const Intersection*>* gates,
 int gateIndex)
{
    _goalNodes.clear();

    for (int i=0; i<goal->numNext(); i++)
    {
        const RoutingNode* goalNode = NULL;

        if (goal->numIn(i)==0)
        {
            // goalnへ流入不可の単路は無視
            continue;
        }

        if (gateIndex<static_cast<int>(gates->size())-2
            && goal->next(i) == (*gates)[gateIndex+2])
        {
            /*
             * gates[gateIndex+1](=goalIntersection)と
             * gates[gateIndex+2]が隣接する場合には，この探索の次の，
             * gates[gateIndex+1]をstartIntersectionとした探索において
             * gates[gateIndex+1]とgates[gateIndex+2]とを
             * 直接結ぶsectionを選択したい
             *
             * そのためにはgates[i+1]をgoalIntersectionとした探索では
             * gates[i+2]->gates[i+1]をゴールとしてはならない
             *
             * Uターンを許せば可能となるため，場合によっては
             * ここのif文に条件を加える必要があるかもしれない
             *
             * なお，ゴールとはしないが通過はありうる
             */
            /*
             * ただし隣接する交差点A, Bが
             * A->B->Aのようにgateとして指定されている場合は
             * この限りではない (先に指定されたA->Bを優先して
             * 直接接続する単路を選び，B->Aはそれ以外の単路となる)
             */
            if (goal->next(i) != (*gates)[gateIndex])
            {
                continue;
            }
        }

        const Section* section = goal->nextSection(i);
        goalNode =  _routingManager
            ->routingNetwork(0)->convertS2N(section,
                                            section->isUp(goal->next(i),
                                                          goal));
        assert(goalNode);
        _goalNodes.push_back(goalNode);
    }
}

//======================================================================
bool PFBase::_isNodeIdIncluded
(const RoutingNode* node,
 const vector<const RoutingNode*>* container) const
{
    for (unsigned int i=0; i<container->size(); i++)
    {
        if (node->id() == (*container)[i]->id())
        {
            return true;
        }
    }
    return false;
}

//======================================================================
void PFBase::_convertSearchResult
(vector<const Intersection*>* result_inters,
 vector<RoutingNode*>* routingNodes,
 const Intersection* goal)
{
    // routeに結果を格納する
    for (unsigned int i=0; i<routingNodes->size(); i++)
    {
        const Intersection* inter
            = (*routingNodes)[i]->section()
            ->intersection((*routingNodes)[i]->isUp());
        result_inters->push_back(inter);
    }
}

//======================================================================
bool PFBase::_loadCacheFromRouteStorage
(const Intersection* from,
 const Intersection* next,
 const Intersection* goal,
 std::vector<const Intersection*>* result_route) const
{
    RouteComponentKeyBase key(_vehicleType, _weights, from, next, goal);
    return _routeStorage->searchRouteComponent(_id, _routingManager,
                                               &key, result_route);
}

//======================================================================
void PFBase::_saveCacheToRouteStorage
(const Intersection* from,
 const Intersection* next,
 const Intersection* goal,
 std::vector<const Intersection*>* route) const
{
    RouteComponentKeyBase* key
        = new RouteComponentKeyBase
        (_vehicleType, _weights, from, next, goal);
    _routeStorage->addRouteComponent(_id, _routingManager,
                                     key, route);
}

//======================================================================
void PFBase::_resetPFStatus()
{
    _weights[0] = 1;
    for (int i=1; i<VEHICLE_ROUTING_PARAMETER_SIZE; i++)
    {
        _weights[i] = 0;
    }
    _id = "0";
    _vehicleType = 0;
    _rnd = NULL;
}

//======================================================================
void PFBase::writeInvalidODPairCache(string startId, string goalId)
{
  ofstream fout(GVManager::getString("CACHE_INVALID_OD_PAIR_FILE").c_str(),ios::app);

  fout << startId << "," << goalId << endl;
}
