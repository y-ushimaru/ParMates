/* **************************************************
 * Copyright (C) 2014 ADVENTURE Project
 * All Rights Reserved
 **************************************************** */
#ifndef __ROUTING_NETWORK_H__
#define __ROUTING_NETWORK_H__

#include <vector>
#include <map>
#include <unordered_map>
#include <string>
#include "Conf.h"

class RoutingNode;
class RoutingLink;
class Section;
class Intersection;

// 型の別名を定義する
typedef std::unordered_map<std::string,
                           RoutingNode*> RNMAPN;
typedef std::unordered_map<std::string,
                           RoutingNode*>::iterator ITRNMAPN;
typedef std::unordered_map<std::string,
                           RoutingNode*>::const_iterator CITRNMAPN;

/// 経路探索用地図を表すクラス
/**
 * @ingroup Routing
 */
class RoutingNetwork
{
public:
    RoutingNetwork();
    ~RoutingNetwork();

    /// ノードの集合を返す
    const RNMAPN* nodes() const;

    /// 指定されたIDを持つノードを返す
    RoutingNode* node(std::string& id) const;

    /// ノードを追加する
    void addNode(RoutingNode* node);

    /// 単路@p sectに該当するRoutingNodeを返す
    RoutingNode* convertS2N(const Section* sect, bool isUp) const;

    /// リンクの集合を返す
    const std::vector<RoutingLink*>* links() const;

    /// 指定されたIDを持つリンクを返す
    RoutingLink* link(std::string& id) const;

    /// 指定された始点・終点を持つリンクを返す
    RoutingLink* link(RoutingNode* begin,
                      RoutingNode* end) const;

    /// リンクを追加する
    void addLink(RoutingLink* link);

    /// 交差点@p interに該当するRoutingLinkを返す
    RoutingLink* convertI2L(const Intersection* via,
                            const Intersection* from,
                            const Intersection* to) const;

    /// 初期リンクコストを付与する
    bool setInitialCosts();

    /// リンクコストを更新する
    bool renewCosts();

    /// ネットワークのランクを設定する
    void setNetworkRank(unsigned int rank);

    /// ネットワークのランクを返す
    unsigned int networkRank() const;

    /// 上位ネットワークを設定する
    void setUpperNetwork(RoutingNetwork* upperNetwork);

    /// 上位ネットワークを返す
    RoutingNetwork* upperNetwork() const;

    /// 下位ネットワークを設定する
    void setLowerNetwork(RoutingNetwork* lowerNetwork);

    /// 下位ネットワークを返す
    RoutingNetwork* lowerNetwork() const;
    
    /// ノードの最高ランクを返す
    unsigned int maxNodeRank();

    /// 情報を表示する
    void print() const;

private:
    /// 経路探索用ノードを保持するコンテナ
    /**
     * 階層化する場合，最下層のネットワークが所有する
     * 上位層のネットワークでnew & deleteしてはならない
     */
    RNMAPN _nodes;

    /// 経路探索用リンクを保持するコンテナ
    /**
     * リンクは各層で異なるため，
     * 各層でnew & deleteする
     */
    std::vector<RoutingLink*> _links;

    /// ネットワークのランク
    unsigned int _networkRank;

    /// 上位ネットワーク
    RoutingNetwork* _upperNetwork;

    /// 下位ネットワーク
    RoutingNetwork* _lowerNetwork;

    /// ノードの最高ランク
    unsigned int _maxNodeRank;
};

#endif //__ROUTING_NETWORK_H__
