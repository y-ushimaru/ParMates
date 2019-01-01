/* **************************************************
 * Copyright (C) 2014 ADVENTURE Project
 * All Rights Reserved
 **************************************************** */
#ifndef __ROUTING_NODE_H__
#define __ROUTING_NODE_H__

#include "VehicleFamily.h"
#include "AmuPoint.h"
#include "Conf.h"
#include <vector>
#include <string>
#include <map>

class Section;
class RoutingLink;

/// 経路探索用グラフのノード
/**
 * RoadMapにおけるSectionに相当する．
 *
 * RoadMapにおけるIntersectionがRoutingLinkとなり，
 * Section->Intersection->Sectionの接続関係を
 * RoutingNode->RoutingLink->RoutingNodeに変換することで
 * RoutingLinkに右左折コストを与えることができる．
 *
 * RoutingNode <--> Section，
 * RoutingLink <--> Intersection 間の変換に注意．
 *
 * @ingroup Routing
 */
class RoutingNode
{
public:
    RoutingNode(const Section* sect, bool isUp);
    ~RoutingNode();

    /// 識別番号を返す
    std::string id() const;

    /// サブIDを返す
    std::string subId() const;

    /// ノードのランクを返す
    unsigned int rank() const;

    /// ノードのランクを設定する
    void setRank(unsigned int rank);

    /// 所属するネットワークの最高ランクを返す
    unsigned int networkRank() const;

    /// 所属するネットワークの最高ランクを設定する
    void setNetworkRank(unsigned int networkRank);

    /// 該当する単路を返す
    const Section* section() const;

    /// 上りか下りかを返す
    bool isUp() const;

    /// 3次元の点を返す
    const AmuPoint point() const;

    /// 自分へ入るリンクを追加する
    void addInLink(RoutingLink* link);

    /// 自分へ入るリンクを返す
    const std::vector<RoutingLink*>* inLinks() const;

    /// @p prevから自分へ入るリンクを返す
    RoutingLink* inLink(const RoutingNode* prev) const;
 
    /// 上流リンクのうち，指定したランク以上のノードに接続するリンクの集合を返す
    void getInLinks(unsigned int rank,
                    std::vector<const RoutingLink*>* result_links) const;
    
    /// 上流ノードのうち，指定したランク以上のノードの集合を返す
    void getInNodes(unsigned int rank,
                    std::vector<const RoutingNode*>* result_nodes) const;

    /// 上流ノードのうち，なす角がもっとも小さなノードを返す
    const RoutingNode* straightInNode() const;
        
    /// 指定したランクのノードからの入次数を返す
    int indegree(unsigned int rank) const;

    /// 同じ階層のノードからの入次数を返す
    /**
     * 上層からのリンクを除外する
     */
    int indegreeInSameNetworkRank() const;
    
    /// 自分から出るリンクを追加する
    void addOutLink(RoutingLink* link);

    /// 自分から出るリンクを返す
    const std::vector<RoutingLink*>* outLinks() const;

    /// 自分から@p nextに出るリンクを返す
    RoutingLink* outLink(const RoutingNode* next) const;

    /// 自分から出る上位ネットワークへのリンクを追加する
    void addUpwardLink(RoutingLink* link);

    /// 自分から出る上位ネットワークへのリンクを返す
    RoutingLink* upwardLink() const;

    /// 自分から出る下位ネットワークへのリンクを追加する
    void addDownwardLink(RoutingLink* link);

    /// 自分から出る下位ネットワークへのリンクを返す
    RoutingLink* downwardLink() const;

    /// 下流リンクのうち，指定したランク以上のノードに接続するリンクの集合を返す
    void getOutLinks(unsigned int rank,
                     std::vector<const RoutingLink*>* result_links) const;
    
    /// 下流ノードのうち，指定したランク以上のノードの集合を返す
    void getOutNodes(unsigned int rank,
                     std::vector<const RoutingNode*>* result_nodes) const;

    /// 下流ノードのうち，なす角がもっとも小さなノードを返す
    const RoutingNode* straightOutNode() const;

    /// 指定したランクのノードへの出次数を返す
    int outdegree(unsigned int rank) const;

    /// 同じ階層のノードへの出次数を返す
    /**
     * 下層へのリンクを除外する
     */
    int outdegreeInSameNetworkRank() const;

    /// 上位のノードを追加する
    void setUpperNode(RoutingNode* node);

    /// 上位のノードを返す
    RoutingNode* upperNode() const;

    /// 下位のノードを追加する
    void setLowerNode(RoutingNode* node);

    /// 下位のノードを返す
    RoutingNode* lowerNode() const;

    /// 最下位のノードを設定する
    void setLowestNode(RoutingNode* node);

    /// 最下位のノードを返す
    RoutingNode* lowestNode() const;

    /// 属性を設定する
    void setProperty();

    /// このノードに設定されたホワイトリストを返す
    const std::vector<std::string>* whitelistedVehicleTypes() const;

    /// このノードに設定されたブラックリストを返す
    const std::vector<std::string>* blacklistedVehicleTypes() const;

    /// このリンクに設定された経路選択確率テーブルを返す
    const std::map<VehicleType, double>* routingProbability() const;

    /// このノードを指定した@p typeの車両が通過できるか
    bool mayPass(VehicleType type);

    /// 経路探索時に指定した@p typeの車両が展開する確率を返す
    double routingProbability(VehicleType type) const;

    /// 情報を表示する
    void print() const;

private:
    /// 識別番号
    /**
     * 階層+サブID
     */
    std::string _id;

    /// サブID
    /**
     * 上流Intersection ID，下流Intersection IDを結合したもの
     * SectionIDに類似（ただし上り下りで違うIDが付与される）
     */
    std::string _subId;

    /// ノードのランク
    /**
     * 経路探索における重要度に相当する
     * 例えば，幹線道路のノードのランクは高い（値が大きい）
     */
    unsigned int _rank;

    /// ネットワークのランク
    /**
     * ノードが所属する最上位ネットワークのランクを表す．
     * 基本的に _rank = _networkRankであるが，
     * ノードの次数が2で縮約される場合は _rank > _networkRank となる．
     */
    unsigned int _networkRank;
        
    /// 該当する単路
    const Section* _sect;

    /// 上りかどうか
    bool _isUp;

    /// 表示に用いる座標
    AmuPoint _coord;

    /// 自分を終点とするリンクの集合
    std::vector<RoutingLink*> _inlinks;

    /// 自分を始点とするリンクの集合
    std::vector<RoutingLink*> _outlinks;

    /// 自分を始点とする上位ネットワークへのリンク
    RoutingLink* _upwardLink;

    /// 自分を始点とする下位ネットワークへのリンク
    RoutingLink* _downwardLink;

    /// 上位ノード
    RoutingNode* _upperNode;

    /// 下位ノード
    RoutingNode* _lowerNode;

    /// 最下位ノード
    RoutingNode* _lowestNode;

    /// 通行権のホワイトリスト(通行許可)
    /**
     * @attention ホワイトリストを優先
     */
    std::vector<std::string> _whitelistedVehicleTypes;

    /// 通行権のブラックリスト
    std::vector<std::string> _blacklistedVehicleTypes;

    /// 経路探索時の探索確率テーブル
    std::map<VehicleType, double> _routingProbability;
};

#endif //__ROUTING_NODE_H__
