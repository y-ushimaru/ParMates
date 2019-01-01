/* **************************************************
 * Copyright (C) 2014 ADVENTURE Project
 * All Rights Reserved
 **************************************************** */
#ifndef __ROUTING_NETWORK_BUILDER_H__
#define __ROUTING_NETWORK_BUILDER_H__

#include "Conf.h"
#include <vector>

class RoadMap;
class RoutingNetwork;

/// 経路探索用地図を作成するクラス
/**
 * @ingroup Routing
 */
class RoutingNetworkBuilder
{
public:
    RoutingNetworkBuilder();
    ~RoutingNetworkBuilder();

    /// 道路地図をセットする
    void setRoadMap(RoadMap* roadMap);

    /// 経路探索用地図のコンテナを登録する
    void setRoutingNetworkContainer
    (std::vector<RoutingNetwork*>* container);

    /// 経路探索用の線グラフ(Line Graph)を作成する
    bool buildRoutingLineGraph();
    
    /// 経路探索用の上位ネットワークを作成する
    bool buildRoutingHigherGraph
    (unsigned int networkRank);

    /// ランクが@p networkRankの経路探索用地図を作成する
    bool buildRoutingNetwork(unsigned int networkRank);

    /// レイヤ間のリンクを生成する
    bool buildInterlayerLinks(unsigned int networkRank);

    /// ノードのプロパティを設定する
    bool setNodeProperties(unsigned int networkRank);

    /// リンクのプロパティを設定する
    bool setLinkProperties(unsigned int networkRank);
    
private:
    /// ノードを生成する
    bool _buildRoutingNodes();
    /// 上位ネットワークのノードを生成する
    bool _buildRoutingHigherNodes(unsigned int networkRank);
    /// ノードのランクを昇格する
    /**
     * 上位ネットワークを拡張する
     */
    bool _promoteRoutingNodes(unsigned int networkRank);

    /// リンクを生成する
    bool _buildRoutingLinks();
    /// 上位ネットワークのリンクを生成する
    bool _buildRoutingHigherLinks(unsigned int networkRank);
 
public:
    /// 作成した経路探索用地図を返す
    RoutingNetwork* routingNetwork();

private:
    /// 道路地図
    RoadMap* _roadMap;

    /// 現在作成中のネットワーク
    RoutingNetwork* _currentRoutingNetwork;

    /// 経路探索用地図のコンテナ
    /**
     * RoutingManagerが持つコンテナへのポインタ
     */
    std::vector<RoutingNetwork*>* _routingNetworkContainer;

    // ノード総数
    ulint _numNodes;

    /// リンク総数
    ulint _numLinks;
};

#endif //__ROUTING_NETWORK_BUILDER_H__
