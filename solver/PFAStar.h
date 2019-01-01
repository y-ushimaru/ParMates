/* **************************************************
 * Copyright (C) 2014 ADVENTURE Project
 * All Rights Reserved
 **************************************************** */
#ifndef __PF_ASTAR_H__
#define __PF_ASTAR_H__

#include <map>
#include <string>
#include "PFBase.h"
#include "PFSearchNodeBase.h"
#include "AmuPriorityQueue.h"

/// A*法を用いた大域的経路探索器
/**
 * PFBaseクラスの派生クラス
 *
 * @ingroup Routing
 */
class PFAStar : public PFBase
{
public:
    PFAStar(RoutingManager* routingManager);
    PFAStar(RoutingManager* routingManager,
            const RoutingNetwork* routingNetwork);
    virtual ~PFAStar();

protected:
    /// 探索中のノード情報を格納するクラス
    class PFASearchNode : public PFSearchNodeBase
    {
    public:
        PFASearchNode(){};
        virtual ~PFASearchNode(){};
        
    protected:
        /// スタートからゴールまでの推定コスト
        /**
         * f(n) = g(n) + h(n) における f(n)に相当
         * g(n) がスタートからノードまでの実コストであり，
         * (親クラスで定義された) cost に値が格納されている
         */
        double _estimatedCost;

    public:
        /// スタートからゴールまでの推定コストをセットする
        void setEstimatedCost(double estimatedCost)
        {
            _estimatedCost = estimatedCost;
        }
        
        /// スタートからゴールまでの推定コストを返す
        double estimatedCost() const
        {
            return _estimatedCost;
        }        

     protected:
        /// スタートからゴールまでの推定距離
        /**
         * 推定コストの距離成分のみ別に保存する
         */
        double _estimatedDistance;

    public:
        /// スタートからゴールまでの推定距離をセットする
        void setEstimatedDistance(double estimatedDistance)
        {
            _estimatedDistance = estimatedDistance;
        }

        /// スタートからゴールまでの推定距離を返す
        double estimatedDistance() const
        {
            return _estimatedDistance;
        }
   };

    /// 経路のRoutingNodeの集合を@p result_nodesに格納する
    /**
     * 目的地のIntersectionに接続するSectionは
     * 複数ある可能性があるため，goalNodeはvector．
     */
    virtual void _search
    (std::vector<RoutingNode*>* result_nodes);

    /// 次のノードのコストを計算する
    virtual double _calcCost(PFASearchNode* presentNode,
                             const RoutingLink* link);

    /// 次のノードの距離を計算する
    virtual double _calcDistance(PFASearchNode* presentNode,
                                 const RoutingLink* link);

    /// ゴールまでの推定コストを計算する
    virtual double _calcEstimatedCost(double fixedCost,
                                      PFASearchNode* nextNode,
                                      const RoutingLink* link);

    /// ゴールまでの推定距離を計算する
    virtual double _calcEstimatedDistance(double fixedDist,
                                          PFASearchNode* nextNode);

    /// nextNodeを評価する
    /**
     * ラベルおよび既知のコストを参照して優先度付きキューを操作する
     */
    virtual void _evaluateSearchNode(PFASearchNode* nextNodeId,
                                     const RoutingLink* nextLink,
                                     std::string prevId,
                                     double distance,
                                     double estimatedDist,
                                     double cost,
                                     double estimatedCost);
        
    /// nextNodeを展開する
    virtual void _expandSearchNode(PFASearchNode* nextNode,
                                   std::string prevId,
                                   double dist,
                                   double estimatedDist,
                                   double cost,
                                   double estimatedCost);

    /// nextNodeを更新する
    virtual void _updateSearchNode(PFASearchNode* nextNode,
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

    /// 下位リンクに展開可能かどうかを返す
    virtual bool _isDownwardLinkExpandable
    (const RoutingNode* upperNode,
     const RoutingLink* downwardLink) const;

    /// 探索後の_snodesから経路探索解を抽出する
    virtual void _extractSearchResult
    (std::vector<RoutingNode*>* result_nodes,
     const PFASearchNode* start,
     const PFASearchNode* goal);

    /// 探索状態を元に戻す
    /**
     * 探索器の状態を元に戻す_resetPFStatusとは異なる
     */
    virtual void _resetRoutingStatus();

    /// 内部状態を元に戻す
    virtual void _resetPFStatus();

    /// 目的地までの最小の直線距離を返す
    virtual double _minDistToGoals(const RoutingNode* node);

protected:
    /// 探索中の情報を保持するコンテナ
    std::map<std::string,
             PFASearchNode*,
             std::less<std::string> > _snodes;

public:
    // SearchNodeの比較関数
    static bool isPrior(PFASearchNode* lhs,
                        PFASearchNode* rhs);

protected:
    /// 探索状況を保存する優先度付きキュー
    AmuPriorityQueue<PFASearchNode*>* _pq;
};

#endif //__PF_ASTAR_H__
