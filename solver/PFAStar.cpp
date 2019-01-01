/* **************************************************
 * Copyright (C) 2014 ADVENTURE Project
 * All Rights Reserved
 **************************************************** */
#include "PFAStar.h"
#include "PFLogger.h"
#include "RoutingNetwork.h"
#include "RoutingNode.h"
#include "RoutingLink.h"
#include "Random.h"
#include <cfloat>
#include <algorithm>
#include <iostream>

using namespace std;

//======================================================================
bool PFAStar::isPrior(PFASearchNode* lhs, PFASearchNode* rhs)
{
    // 推定コストが低い方が優先度が高い
    return (lhs->estimatedCost() < rhs->estimatedCost());
}

//======================================================================
PFAStar::PFAStar(RoutingManager* routingManager)
    : PFBase(routingManager)
{
    _pq = new AmuPriorityQueue<PFASearchNode*>(PFAStar::isPrior);
}

//======================================================================
PFAStar::PFAStar(RoutingManager* routingManager,
                 const RoutingNetwork* routingNetwork)
    : PFBase(routingManager)
{
    // _snodesの生成
    _snodes.clear();
    
    while (true)
    {
        const RNMAPN* nodes = routingNetwork->nodes();
        for (CITRNMAPN itn = nodes->begin(); itn != nodes->end(); itn++)
        {       
            PFASearchNode* anode = new PFASearchNode;
            anode->setId((*itn).second->id());
            anode->setRoutingNode((*itn).second);
            anode->setPrevId("-1");
            anode->setDistance(DBL_MAX);
            anode->setEstimatedDistance(DBL_MAX);
            anode->setCost(DBL_MAX);
            anode->setEstimatedCost(DBL_MAX);
            anode->setLabel(PFASearchNode::UNREACHED);
            _snodes.insert(make_pair(anode->id(), anode));
        }
        if (routingNetwork->upperNetwork())
        {
            routingNetwork = routingNetwork->upperNetwork();
        }
        else
        {
            break;
        }
    }

    _pq = new AmuPriorityQueue<PFASearchNode*>(PFAStar::isPrior);
}

//======================================================================
PFAStar::~PFAStar()
{
    // _snodesの消去
    for (map<string, PFASearchNode*, less<string> >::iterator itn
             = _snodes.begin();
         itn!=_snodes.end();
         itn++)
    {
        delete (*itn).second;
    }

    delete _pq;
}

//======================================================================
void PFAStar::_search(vector<RoutingNode*>* result_nodes)
{
    // フラグを使用中に
    _isInUse = true;
    _pq->clear();
    result_nodes->clear();

    // ロガーに静的な情報を記録
    /*
     * 経路探索器ごとにIDの付け方が異なる可能性があるため
     * 毎回登録し毎回クリアする
     */
    if (_logger)
    {
        _logger->clearSnodeId2RoutingNode();
        
        map<string, PFASearchNode*, less<string> >::iterator itn;
        for (itn=_snodes.begin(); itn!=_snodes.end(); itn++)
        {
            _logger->addSnodeId2RoutingNode
                ((*itn).second->id(), (*itn).second->routingNode());
        }
    }
    
    // スタートノードを優先度付きキューにpush
    PFASearchNode* start = _snodes[_startNode->id()];
    start->setDistance(0);
    start->setCost(0);
    _pq->insert(start);
    if (_logger)
    {
        _logger->addLog(start);
    }
    
    // 到達できたゴールを示すポインタ
    /*
     * ゴールは複数ある可能性があり，_extractSearchResultのために
     * 到達できたゴールを格納する
     */
    PFASearchNode* goal;

    // 反復処理
    while (true)
    {
        //--------------------------------------------------------------
        // 先頭(最優先)要素の取り出し
        PFASearchNode* presentNode = _pq->extractRoot();
        // cout << "present: " << presentNode->id() << endl;

        // ゴールに到達したら終了
        if (_isNodeIdIncluded(presentNode->routingNode()->lowestNode(),
                              &_goalNodes))
        {
            goal = presentNode;
            break;
        }

        //--------------------------------------------------------------
        // 先頭要素を始点とするリンクを辿る
        const vector<RoutingLink*>* links
            = presentNode->routingNode()->outLinks();
        
        for (unsigned int i = 0; i<links->size(); i++)
        {
            const RoutingLink* link = (*links)[i];
            
            //----------------------------------------------------------
            // downwardLink
            if (link == presentNode->routingNode()->downwardLink())
            {
                // 条件を満たさなければ展開しない
                /*
                 * ランク1->0の場合のみ展開するかどうか判別可能
                 */
                if (presentNode->routingNode()->networkRank()==1
                    && !_isDownwardLinkExpandable
                    (presentNode->routingNode(), link))
                {
                    continue;
                }
            }

            //----------------------------------------------------------
            // upwardLink
            else if (link == presentNode->routingNode()->upwardLink())
            {
                // 条件を満たさなければ展開しない
                if (!_isUpwardLinkExpandable(presentNode->routingNode()))
                {
                    continue;
                }
            }

            //----------------------------------------------------------
            // 同じレイヤのリンクを展開するかどうか判別
            else
            {
                if (!_isHorizontalLinkExpandable
                    (presentNode->routingNode(), link))
                {
                    continue;
                }
            
                //------------------------------------------------------
                // 展開不可(交通規制)
                if (!(presentNode->routingNode()->mayPass(_vehicleType))
                    || !(link->mayPass(_vehicleType)))
                {
                    continue;
                }
                
                // 展開不可(確率的経路探索)
                if (Random::uniform(_rnd)
                    >min(presentNode
                         ->routingNode()->routingProbability(_vehicleType),
                         link->routingProbability(_vehicleType)))
                {
                    continue;
                }
            }
            
            //----------------------------------------------------------
            PFASearchNode* nextNode = _snodes[link->endNode()->id()];

            // スタートからnextNodeまでの確定距離
            double nextNodeDistance
               = _calcDistance(presentNode, link);
  
            // ゴールまでの推定距離
            double nextNodeEstimatedDistance
                = _calcEstimatedDistance(nextNodeDistance,
                                         nextNode);

            // スタートからの確定コスト
            double nextNodeCost
                = _calcCost(presentNode, link);
                          
            // ゴールまでの推定コスト
            double nextNodeEstimatedCost
                = _calcEstimatedCost(nextNodeCost,
                                     nextNode,
                                     link);
            
            //----------------------------------------------------------
            // nextNodeの評価
            _evaluateSearchNode(nextNode,
                                link,
                                presentNode->id(),
                                nextNodeDistance,
                                nextNodeEstimatedDistance,
                                nextNodeCost,
                                nextNodeEstimatedCost);
        }
    
        presentNode->setLabel(PFASearchNode::SCANNED);

        /*
         * ここでキューが空になっているということは
         * 経路探索に失敗したことを意味する
         */
        if (_pq->empty())
        {
            _resetRoutingStatus();
            return;
        }
    }

    // 結果の格納
    /*
     * ゴールからprevIdを辿ってスタートまで
     */
    _extractSearchResult(result_nodes, start, goal);
    _resetRoutingStatus();
}

//======================================================================
double PFAStar::_calcCost(PFASearchNode* presentNode,
                          const RoutingLink* link)
{
    double cost = presentNode->cost();
    for (int i=0; i<VEHICLE_ROUTING_PARAMETER_SIZE; i++)
    {
        cost += _weights[i] * link->cost(i);
    }
    return cost;
}

//======================================================================
double PFAStar::_calcDistance(PFASearchNode* presentNode,
                              const RoutingLink* link)
{
    double distance = presentNode->distance();
    distance += link->cost(VEHICLE_ROUTING_PARAMETER_OF_LENGTH);
    return distance;
}

//======================================================================
double PFAStar::_calcEstimatedCost
(double fixedCost,
 PFASearchNode* nextNode,
 const RoutingLink* link)
{
    double estimatedCost = fixedCost;
    double distanceToGoal = _minDistToGoals(nextNode->routingNode());

    // 距離のヒューリスティクスh(n)を加算(m)
    estimatedCost += distanceToGoal * _weights[0];

    // 時間のヒューリスティクスh(n)を加算(sec)
    /*
     * 60[km/h] = 1000[m/min] = 100/6[m/sec]
     */
    estimatedCost += distanceToGoal * _weights[1] * 6.0 / 100;  

    // 他のヒューリスティクスは0

    return estimatedCost;
}
//======================================================================
double PFAStar::_calcEstimatedDistance(double fixedDist,
                                       PFASearchNode* nextNode)
{
    return fixedDist + _minDistToGoals(nextNode->routingNode());
}

//======================================================================
void PFAStar::_evaluateSearchNode
(PFASearchNode* nextNode,
 const RoutingLink* nextLink,
 string prevId,
 double dist,
 double estimatedDist,
 double cost,
 double estimatedCost)
{
    if (nextNode->label() == PFASearchNode::UNREACHED)
    {
        // 未探索であれば探索済に
        _expandSearchNode(nextNode,
                          prevId,
                          dist,
                          estimatedDist,
                          cost,
                          estimatedCost);
    }
    else if (nextNode->label() == PFASearchNode::LABELED)
    {
        // 探索済であればコストの更新
        _updateSearchNode(nextNode,
                          prevId,
                          dist,
                          estimatedDist,
                          cost,
                          estimatedCost);
    }
    // nextNode->labelがSCANNEDであれば何もしない
}

//======================================================================
void PFAStar::_expandSearchNode(PFASearchNode* nextNode,
                                string prevId,
                                double dist,
                                double estimatedDist,
                                double cost,
                                double estimatedCost)
{
    nextNode->setLabel(PFASearchNode::LABELED);
    nextNode->setPrevId(prevId);
    nextNode->setDistance(dist);
    nextNode->setEstimatedDistance(estimatedDist);
    nextNode->setCost(cost);
    nextNode->setEstimatedCost(estimatedCost);
    _pq->insert(nextNode);
    if (_logger)
    {
        _logger->addLog(nextNode);
    }
}

//======================================================================
void PFAStar::_updateSearchNode(PFASearchNode* nextNode,
                                string prevId,
                                double dist,
                                double estimatedDist,
                                double cost,
                                double estimatedCost)
{
    if (cost < nextNode->cost())
    {
        nextNode->setPrevId(prevId);
        nextNode->setDistance(dist);
        nextNode->setEstimatedDistance(estimatedDist);
        nextNode->setCost(cost);
        nextNode->setEstimatedCost(estimatedCost);
        _pq->makeHeapDown(_pq->size()-1);
        if (_logger)
        {
            _logger->addLog(nextNode);
        }
    }
}

//======================================================================
bool PFAStar::_isHorizontalLinkExpandable
(const RoutingNode* node, const RoutingLink* link) const
{
    if (node->networkRank()==0
        && link->endNode()->upperNode())
    {
        return false;
    }
    return true;
}
    
//======================================================================
bool PFAStar::_isUpwardLinkExpandable(const RoutingNode* node) const
{
    if (node->networkRank()==0)
    {
        return true;
    }
    else
    {
        return false;
    }
}

//======================================================================
bool PFAStar::_isDownwardLinkExpandable(const RoutingNode* upperNode,
                                        const RoutingLink* downwardLink) const
{
    if (upperNode->networkRank()>=2)
    {
        return true;
    }

    bool result = false;

    //------------------------------------------------------------------
    //  upperNode->networkRank()==1である場合の展開可能条件

    // 条件1. 下位ノードがゴールである
    const RoutingNode* lowestNode = upperNode->lowestNode();
    result = _isNodeIdIncluded(lowestNode, &_goalNodes);
    if (result)
    {
        return true;
    }

    // 条件2. この先に集約リンクがあり，ゴールを含む
    const vector<RoutingLink*>* links = upperNode->outLinks();
    for (unsigned int i=0; i<links->size(); i++)
    {
        if ((*links)[i] == downwardLink
            || !(*links)[i]->isAggregatedLink())
        {
            continue;
        }
        vector<const RoutingNode*> abbrNodes;
        (*links)[i]->getAllAbbrNodes(&abbrNodes);
        for (unsigned int j=0; j<abbrNodes.size(); j++)
        {
            result = _isNodeIdIncluded(abbrNodes[j],
                                       &_goalNodes);
            if (result)
            {
                return true;
            }
        }
    }
    return false;
}

//======================================================================
void PFAStar::_extractSearchResult(vector<RoutingNode*>* result_nodes,
                                   const PFASearchNode* start,
                                   const PFASearchNode* goal)
{
    PFSearchNodeBase* scan = const_cast<PFASearchNode*>(goal);
    /*
     * ゴールからprevIdを辿ってスタートまで辿り，最後に反転させる
     */
    while (true)
    {
        RoutingNode* node = scan->routingNode();
        if (scan->id() == start->id())
        {
            // 最後の要素を追加して終了
            result_nodes->push_back(node);
            break;
        }
        
        RoutingNode* prevNode
            = _snodes[scan->prevId()]->routingNode();
        RoutingLink* inLink = node->inLink(prevNode);
         
        // prevNode->nodeがupwardLinkやdownwardLinkの場合
        // result_nodesに追加する必要はない
        if (inLink == prevNode->upwardLink()
            || inLink == prevNode->downwardLink())
        {
            scan = _snodes[scan->prevId()];
            continue;
        }
        
        // SearchNode自身が対応するノードを追加
        result_nodes->push_back(node->lowestNode());
        
        // inLinkが集約リンクである場合，展開する必要がある
        if (inLink->rank()>=1
            && inLink->isAggregatedLink())
        {
            const vector<const RoutingNode*>* abbrNodes
                = inLink->includedLowestNodes();
            for (int i=abbrNodes->size()-1; i>=0; i--)
            {
                result_nodes
                    ->push_back
                    (const_cast<RoutingNode*>((*abbrNodes)[i]));
            }
        }
        scan = _snodes[scan->prevId()];
    }
    
    reverse(result_nodes->begin(), result_nodes->end());
}

//======================================================================
void PFAStar::_resetRoutingStatus()
{
    for (map<string, PFASearchNode*, less<string> >::iterator itn
             = _snodes.begin();
         itn != _snodes.end();
         itn++)
    {
        (*itn).second->setPrevId("-1");
        (*itn).second->setDistance(DBL_MAX);
        (*itn).second->setEstimatedDistance(DBL_MAX);
        (*itn).second->setCost(DBL_MAX);
        (*itn).second->setEstimatedCost(DBL_MAX);
        (*itn).second->setLabel(PFASearchNode::UNREACHED);
    }

    _startNode = NULL;
    _goalNodes.clear();
    _pq->clear();
}

//======================================================================
void PFAStar::_resetPFStatus()
{
    PFBase::_resetPFStatus();
}

//======================================================================
double PFAStar::_minDistToGoals
(const RoutingNode* node)
{
    double minDistance = DBL_MAX;

    for (unsigned int i=0; i<_goalNodes.size(); i++)
    {
        double distance
            = node->point()
            .distance(_goalNodes[i]->point());
        if (distance < minDistance)
        {
            minDistance = distance;
        }
    }
    return minDistance;
}
