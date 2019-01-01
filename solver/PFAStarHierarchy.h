/* **************************************************
 * Copyright (C) 2014 ADVENTURE Project
 * All Rights Reserved
 **************************************************** */
#ifndef __PF_ASTAR_HIERARCHY__
#define __PF_ASTAR_HIERARCHY__

#include <map>
#include <string>
#include "PFBase.h"
#include "PFAStar.h"
#include "PFSearchNodeBase.h"
#include "AmuPriorityQueue.h"

/// 階層的ネットワーク上でA*法を用いた大域的経路探索器
/**
 * PFAStarクラスの派生クラス
 *
 * @ingroup Routing
 */
class PFAStarHierarchy : public PFAStar
{
public:
    PFAStarHierarchy(RoutingManager* routingManager);
    PFAStarHierarchy(RoutingManager* routingManager,
                     const RoutingNetwork* routingNetwork);
    virtual ~PFAStarHierarchy();
    
    /// 文字列から経路探索要素を生成して登録する
    virtual bool generateRouteComponent
    (RoadMap* roadMap, std::vector<std::string> tokens) const;

protected:
    /// 探索中のノード情報を格納するクラス
    class PFAHSearchNode : public PFASearchNode
    {
    public:
        PFAHSearchNode(){};
        virtual ~PFAHSearchNode(){};

    protected:
        /// 上位ノードのID
        std::string _upperId;

    public:
        /// 上位ノードのIDをセットする
        void setUpperId(const std::string& upperId)
        {
            _upperId = upperId;
        }
        
        /// 上位ノードのIDを返す
        std::string upperId() const
        {
            return _upperId;
        }

    protected:
        /// 下位ノードのID
        std::string _lowerId;

    public:
        /// 下位ノードのIDをセットする
        void setLowerId(const std::string& lowerId)
        {
            _lowerId = lowerId;
        }
        
        /// 下位ノードのIDを返す
        std::string lowerId() const
        {
            return _lowerId;
        }
    };

    /// ゴールまでの推定コストを計算する
    virtual double _calcEstimatedCost(double fixedCost,
                                      PFASearchNode* nextNode,
                                      const RoutingLink* link);

    /// nextNodeを評価する
    /**
     * ラベルおよび既知のコストを参照して優先度付きキューを操作する
     */
    virtual void _evaluateSearchNode(PFASearchNode* nextNode,
                                     const RoutingLink* nextLink,
                                     std::string prevId,
                                     double dist,
                                     double estimatedDist,
                                     double cost,
                                     double estimatedCost);

    /// 同じレイヤのリンクに展開可能かどうかを返す
    virtual bool _isHorizontalLinkExpandable
    (const RoutingNode* node, const RoutingLink* link) const;
 
    /// 上位リンクに展開可能かどうかを返す
    virtual bool _isUpwardLinkExpandable
    (const RoutingNode* node) const;

    /// 経路探索結果のストレージから結果をロードする
    virtual bool _loadCacheFromRouteStorage
    (const Intersection* from,
     const Intersection* next,
     const Intersection* goal,
     std::vector<const Intersection*>* result_route) const;

    /// 経路探索結果をストレージに保存する
    virtual void _saveCacheToRouteStorage
    (const Intersection* from,
     const Intersection* next,
     const Intersection* goal,
     std::vector<const Intersection*>* route) const;
    
    /// 探索状態を元に戻す
    /**
     * 探索器の状態を元に戻す_resetPFStatusとは異なる
     */
    virtual void _resetRoutingStatus();

    /// 内部状態を元に戻す
    virtual void _resetPFStatus();

protected:
    /// 対象ネットワークランクの上限
    int _maxNetworkRank;
    
    /// 選好するネットワークランクの下限
    int _preferredNetworkRank;

public:
    /// 選好するネットワークレベルの下限を設定する
    void setPreferredNetworkRank(int networkRank);

    /// 選好するネットワークレベルを返す
    int preferredNetworkRank() const;    
};

#endif //__PF_ASTAR_HIERARCHY__
