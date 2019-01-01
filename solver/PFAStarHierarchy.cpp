/* **************************************************
 * Copyright (C) 2014 ADVENTURE Project
 * All Rights Reserved
 **************************************************** */
#include "PFAStarHierarchy.h"
#include "RouteComponentKeyBase.h"
#include "RouteComponentKeyHierarchy.h"
#include "RouteStorage.h"
#include "PFLogger.h"
#include "RoutingManager.h"
#include "RoutingNetwork.h"
#include "RoutingNode.h"
#include "RoutingLink.h"
#include "RoadMap.h"
#include "Intersection.h"
#include "Random.h"
#include <cfloat>
#include <algorithm>
#include <iostream>

using namespace std;

//======================================================================
PFAStarHierarchy::PFAStarHierarchy(RoutingManager* routingManager)
    : PFAStar(routingManager)
{
}

//======================================================================
PFAStarHierarchy::PFAStarHierarchy(RoutingManager* routingManager,
                                   const RoutingNetwork* routingNetwork)
    : PFAStar(routingManager)
{
    // _snodesの生成と_maxNetworkRankの決定
    _snodes.clear();
    _maxNetworkRank = 0;

    while (true)
    {
        const RNMAPN* nodes = routingNetwork->nodes();
        for (CITRNMAPN itn = nodes->begin(); itn != nodes->end(); itn++)
        {
            PFAHSearchNode* anode = new PFAHSearchNode();
            anode->setId((*itn).second->id());
            anode->setRoutingNode((*itn).second);
            anode->setPrevId("-1");
            if ((*itn).second->upperNode()
                && (*itn).second->upperNode()->downwardLink())
            {
                // 自分に流入するdownwardLinkがある
                anode->setUpperId((*itn).second->upperNode()->id());
            }
            else
            {
                anode->setUpperId("-1");
            }
            if ((*itn).second->downwardLink())
            {
                // 自分から流出するdownwardLinkがある
                anode->setLowerId((*itn).second->lowerNode()->id());
            }
            else
            {
                anode->setLowerId("-1");
            }
            anode->setDistance(DBL_MAX);
            anode->setCost(DBL_MAX);
            anode->setEstimatedCost(DBL_MAX);
            anode->setLabel(PFASearchNode::UNREACHED);
            _snodes.insert(make_pair(anode->id(), anode));
        }
        if (routingNetwork->upperNetwork())
        {
            routingNetwork = routingNetwork->upperNetwork();
            _maxNetworkRank++;
        }
        else
        {
            break;
        }
    }

    _preferredNetworkRank = _maxNetworkRank;
}

//======================================================================
PFAStarHierarchy::~PFAStarHierarchy()
{
}

//======================================================================
bool PFAStarHierarchy::generateRouteComponent
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

    // 第3カラムは選好ネットワークランク
    unsigned int prefRank = static_cast<unsigned int>
        (atoi(tokens[index].c_str()));
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
        = new RouteComponentKeyHierarchy(type, prefRank,
                                         weights, past, start, goal);

    const_cast<Intersection*>(past)
        ->routeStorage()
        ->addRouteComponentDirectly(key, count, &route);

    return true;
}

//======================================================================
double PFAStarHierarchy::_calcEstimatedCost(double fixedCost,
                          PFASearchNode* nextNode,
                          const RoutingLink* link)
{
    double estimatedCost  = fixedCost;
    double distanceToGoal = _minDistToGoals(nextNode->routingNode());

    // 距離のヒューリスティクスh(n)を加算(m)
    estimatedCost += distanceToGoal * _weights[0];

    // 時間のヒューリスティクスh(n)を加算(sec)
    /*
     * 180[km/h] = 3[km/min] = 50[m/sec]
     */
    estimatedCost += distanceToGoal * _weights[1] / 50.0;

    // 他のヒューリスティクスは0

    //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    // ネットワークの階層に応じてペナルティを課す
    double rankDiff
        = static_cast<double>
        (max(0,
             _preferredNetworkRank
             - static_cast<int>(nextNode->routingNode()
                                ->networkRank())));
    /*
     * 選好するランクとの差に応じてペナルティが増大(0-100%)
     * ゴールまでの推定距離に応じてペナルティが増大(直線距離の実数)
     * 係数1.0は適当で調整の余地あり
     */
    double penalty
        = 1.0 * distanceToGoal * rankDiff / _preferredNetworkRank;

    estimatedCost *= (1 + penalty);
    return estimatedCost;
}

//======================================================================
void PFAStarHierarchy::_evaluateSearchNode(PFASearchNode* nextNode,
                                           const RoutingLink* nextLink,
                                           string prevId,
                                           double dist,
                                           double estimatedDist,
                                           double cost,
                                           double estimatedCost)
{
    bool isLabelPropagative = false;
    PFAHSearchNode* nextAHNode
        = dynamic_cast<PFAHSearchNode*>(nextNode);

    if (nextAHNode->label() == PFAHSearchNode::UNREACHED)
    {
        // 未探索であれば探索済に
        _expandSearchNode(nextNode,
                          prevId,
                          dist,
                          estimatedDist,
                          cost,
                          estimatedCost);
        if (nextAHNode->lowerId()!="-1"
            && nextLink
            != _snodes[prevId]->routingNode()->upwardLink())
        {
            isLabelPropagative = true;
        }
    }
    else if (nextAHNode->label() == PFAHSearchNode::LABELED)
    {
        // 探索済であればコストの更新
        _updateSearchNode(nextNode,
                          prevId,
                          dist,
                          estimatedDist,
                          cost,
                          estimatedCost);
    }
    else if (nextAHNode->label() == PFAHSearchNode::UPPERLABELED)
    {
        if (nextLink
            == nextAHNode->routingNode()
            ->upperNode()->downwardLink())
        {
            // 上位ノードが探索済みであれば，downwardLinkのみ処理
            _expandSearchNode(nextNode,
                              prevId,
                              dist,
                              estimatedDist,
                              cost,
                              estimatedCost);
        }
    }

    // nextNode->labelがSCANNEDであれば何もしない

    // nextNodeがdownwardLinkを持つ場合はラベルを伝播
    if (isLabelPropagative)
    {
        PFAHSearchNode* target = nextAHNode;
        while (true)
        {
            target
                = dynamic_cast<PFAHSearchNode*>
                (_snodes[target->lowerId()]);

            target->setLabel(PFAHSearchNode::UPPERLABELED);
            if (target->lowerId()=="-1")
            {
                break;
            }
        }
    }
}

//======================================================================
bool PFAStarHierarchy::_isHorizontalLinkExpandable
(const RoutingNode* node, const RoutingLink* link) const
{
    if (node->networkRank()==0)
    {
        if (node->upperNode()
            && link->endNode()->upperNode())
        {
            return false;
        }
        return true;
    }
    else if (node->networkRank()>=
             static_cast<unsigned int>(_preferredNetworkRank))
    {
        return true;
    }
    else
    {
        const RoutingNode* nextNode  = link->endNode();
        const RoutingLink* upperLink = link->upperLink();
        if (!upperLink)
        {
            return true;
        }

        // 以下の条件を満たす場合は上位ネットワークから到達可能
        if (nextNode->upperNode()
            && nextNode->upperNode()->downwardLink()
            && upperLink->beginNode()->lowerNode() == node
            && upperLink->endNode()->lowerNode() == nextNode)
        {
            return false;
        }
        else
        {
            return true;
        }
    }
    return true;
}

//======================================================================
bool PFAStarHierarchy::_isUpwardLinkExpandable
(const RoutingNode* node) const
{
    assert(node->upwardLink());
    if (node->upwardLink()->endNode()->networkRank()
        > static_cast<unsigned int>(_preferredNetworkRank))
    {
        return false;
    }
    return true;
}

//======================================================================
bool PFAStarHierarchy::_loadCacheFromRouteStorage
(const Intersection* from,
 const Intersection* next,
 const Intersection* goal,
 std::vector<const Intersection*>* result_route) const
{
    RouteComponentKeyHierarchy key(_vehicleType,
                                   _preferredNetworkRank,
                                   _weights, from, next, goal);
    return _routeStorage->searchRouteComponent(_id, _routingManager,
                                               &key, result_route);
}

//======================================================================
void PFAStarHierarchy::_saveCacheToRouteStorage
(const Intersection* from,
 const Intersection* next,
 const Intersection* goal,
 std::vector<const Intersection*>* route) const
{
    RouteComponentKeyHierarchy* key
        = new RouteComponentKeyHierarchy
        (_vehicleType, _preferredNetworkRank,
         _weights, from, next, goal);
    _routeStorage->addRouteComponent(_id, _routingManager,
                                     key, route);
}

//======================================================================
void PFAStarHierarchy::_resetRoutingStatus()
{
    for (map<string, PFASearchNode*, less<string> >::iterator itn
             = _snodes.begin();
         itn != _snodes.end();
         itn++)
    {
        (*itn).second->setPrevId("-1");
        (*itn).second->setDistance(DBL_MAX);
        (*itn).second->setCost(DBL_MAX);
        (*itn).second->setEstimatedCost(DBL_MAX);
        (*itn).second->setLabel(PFASearchNode::UNREACHED);

        // upperId, lowerIdはそのままでよい
        // _preferredNetworkRankはそのままでよい
    }
}

//======================================================================
void PFAStarHierarchy::setPreferredNetworkRank(int networkRank)
{
    _preferredNetworkRank = networkRank;
}

//======================================================================
int PFAStarHierarchy::preferredNetworkRank() const
{
    return _preferredNetworkRank;
}

//======================================================================
void PFAStarHierarchy::_resetPFStatus()
{
    PFBase::_resetPFStatus();
    _preferredNetworkRank = _maxNetworkRank;
}
