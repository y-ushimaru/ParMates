/* **************************************************
 * Copyright (C) 2014 ADVENTURE Project
 * All Rights Reserved
 **************************************************** */
#include "PFDijkstra.h"
#include "PFLogger.h"
#include "RoutingManager.h"
#include "RoutingNetwork.h"
#include "RoutingNode.h"
#include "RoutingLink.h"
#include "Random.h"
#include "AmuPriorityQueue.h"
#include <vector>
#include <cfloat>
#include <algorithm>
#include <iostream>

using namespace std;

//======================================================================
bool PFDijkstra::isPrior(PFDSearchNode* lhs, PFDSearchNode* rhs)
{
    // コストが低い方が優先度が高い
    return (lhs->cost() < rhs->cost());
}

//======================================================================
PFDijkstra::PFDijkstra(RoutingManager* routingManager)
    : PFBase(routingManager)
{
}

//======================================================================
PFDijkstra::PFDijkstra(RoutingManager* routingManager,
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
            PFDSearchNode* anode = new PFDSearchNode();
            anode->setId((*itn).second->id());
            anode->setRoutingNode((*itn).second);
            anode->setPrevId("-1");
            anode->setDistance(DBL_MAX); 
            anode->setCost(DBL_MAX);
            anode->setLabel(PFDSearchNode::UNREACHED);
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
}

//======================================================================
PFDijkstra::~PFDijkstra()
{
    // _snodesの消去
    for (map<string, PFDSearchNode*, less<string> >::iterator itn
             = _snodes.begin();
         itn!=_snodes.end();
         itn++)
    {
        delete (*itn).second;
    }
}

//======================================================================
void PFDijkstra::_search(std::vector<RoutingNode*>* result_nodes)
{
    // フラグを使用中に
    _isInUse = true;
    result_nodes->clear();

    // 探索中の状況を保持する優先度付きキュー
    /*
     * 中身はSearchiNode*の配列
     * コスト最小のものが先頭に配置される
     */
    AmuPriorityQueue<PFDSearchNode*> pq(PFDijkstra::isPrior);

    // スタートノードを優先度付きキューにpush
    PFDSearchNode* start = _snodes[_startNode->id()];
    start->setDistance(0);
    start->setCost(0);
    pq.insert(start);
    if (_logger)
    {
        _logger->addLog(start);
    }

    PFDSearchNode* goal;

    // 反復処理
    while (true)
    {
        //--------------------------------------------------------------
        // 先頭(最優先)要素の取り出し
        PFDSearchNode* presentNode = pq.extractRoot();

        // ゴールに到達したら終了
        if (_isNodeIdIncluded(presentNode->routingNode(),
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

            // 暫定処置
            /*
             * upwardLinkは無視する
             */
            if (link == presentNode->routingNode()->upwardLink())
            {
                continue;
            }

            // 展開不可(交通規制)
            if (!(presentNode->routingNode()
                  ->mayPass(_vehicleType))
                || !(link->mayPass(_vehicleType)))
            {
                continue;
            }

            // 展開不可(確率的経路探索)
            if (Random::uniform(_rnd)
                >min(presentNode->routingNode()
                     ->routingProbability(_vehicleType),
                     link->routingProbability(_vehicleType)))
            {
                continue;
            }

            PFDSearchNode* nextNode = _snodes[link->endNode()->id()];
            double nextNodeCost  =  presentNode->cost();

            for (int j=0; j<VEHICLE_ROUTING_PARAMETER_SIZE; j++)
            {
                nextNodeCost += _weights[j] * link->cost(j);
            }
           
            if (nextNode->label() == PFDSearchNode::UNREACHED)
            {
                // 未探索であれば探索済に
                nextNode->setLabel(PFDSearchNode::LABELED);
                nextNode->setPrevId(presentNode->id());
                nextNode->setCost(nextNodeCost);
                pq.insert(nextNode);
            }
            
            else if (nextNode->label() == PFDSearchNode::LABELED)
            {
                // 探索済であればコストの更新
                if (nextNodeCost < nextNode->cost())
                {
                    nextNode->setPrevId(presentNode->id());
                    nextNode->setCost(nextNodeCost);
                    pq.makeHeapDown(pq.size()-1);
                }
            }
            
            // SCANNEDであれば何もしない
        }
        presentNode->setLabel(PFDSearchNode::SCANNED);

        /*
         * ここでキューが空になっているということは
         * 経路探索に失敗したことを意味する
         */
        if (pq.empty())
        {
            return;
        }
    }

    // 結果の格納
    /*
     * ゴールからprevIdを辿ってスタートまで
     */
    PFSearchNodeBase* scan = goal;
    while (true)
    {
        RoutingNode* node = scan->routingNode();
        result_nodes->push_back(node);
        if (scan->id() == _startNode->id())
        {
            break;
        }
        scan = _snodes[scan->prevId()];
    }
    reverse(result_nodes->begin(), result_nodes->end());
    _resetRoutingStatus();
}

//======================================================================
void PFDijkstra::_resetRoutingStatus()
{
    for (map<string, PFDSearchNode*, less<string> >::iterator itn
             = _snodes.begin();
         itn != _snodes.end();
         itn++)
    {
        (*itn).second->setPrevId("-1");
        (*itn).second->setDistance(DBL_MAX);
        (*itn).second->setCost(DBL_MAX);
        (*itn).second->setLabel(PFDSearchNode::UNREACHED);
    }
    _startNode = NULL;
    _goalNodes.clear();
}
