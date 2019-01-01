/* **************************************************
 * Copyright (C) 2014 ADVENTURE Project
 * All Rights Reserved
 **************************************************** */
#ifndef __PF_BASE_H__
#define __PF_BASE_H__

#include "VehicleFamily.h"
#include "Conf.h"
#include "RoutingNetwork.h"
#include <string>

class RoutingManager;
class Route;
class RouteStorage;
class RouteComponentKeyBase;
class RoadMap;
class Intersection;
class RandomGenerator;
class PFLogger;

/// 大域的経路探索器の基底クラス
/**
 * @ingroup Routing
 */
class PFBase
{
public:
    PFBase(RoutingManager* routingManager);
    virtual ~PFBase();

    /// コスト計算用のウェイトを設定する
    void setWeights(const double weights[]);

    /// 使用中フラグをオンにする
    void setInUseOn();

    /// 使用中フラグをオフにする
    void setInUseOff();

    /// 経路探索器が使用中であるかどうかを返す
    bool isInUse() const;

    /// 使用権を持つ車両のIDを登録する
    void setId(const std::string& id);

    /// 使用権を持つ車両の車種を登録する
    void setVehicleType(VehicleType type);

    /// ロガーを設定する
    void addLogger(PFLogger* logger);

    /// ロガーを解除する
    void removeLogger();

    /// ロガーを返す
    PFLogger* logger() const;

    /// 乱数生成器をセットする
    void setRandomGenerator(RandomGenerator* rnd);

    /// 文字列から経路探索要素を生成して登録する
    virtual bool generateRouteComponent
    (RoadMap* roadMap, std::vector<std::string> tokens) const;

    /// @p fromから@p nextを経由し，@p route で指定された経路を探索する
    virtual bool search(Route* route,
                        const Intersection* from,
                        const Intersection* next);

    /// 無効なODペアがあれば書き出す
    void writeInvalidODPairCache(std::string startId, std::string goalId);

protected:
    /// 経路探索用ネットワークにおける探索の始点を決定する
    /**
     * @p fromから@p nextへのsectionから探索を開始する．
     */
    virtual const RoutingNode* _startNodeForSearch
    (const Intersection* from,
     const Intersection* next);

    /// 経路探索用ネットワークにおける探索の終点を決定する
    /**
     * 本来のgoalIntersectionに接続するRoutingNode
     * (=RoadMapにおけるSection)は複数存在する場合がある
     */
    virtual void _findGoalNodes
    (const Intersection* goal,
     const std::vector<const Intersection*>* gates,
     int gateIndex);

    /// ノードが探索の終点に含まれるかどうかを返す
    /**
     * 比較対象がコピーであることを踏まえ，
     * ポインタでなくIDで判断する
     */
    virtual bool _isNodeIdIncluded
    (const RoutingNode* node,
     const std::vector<const RoutingNode*>* container) const;

    /// 探索結果を@p routeに追加する
    /**
     * RoutingNode，RoutingLinkからIntersectionへの変換を含む
     */
    virtual void _convertSearchResult
    (std::vector<const Intersection*>* result_inters,
     std::vector<RoutingNode*>* routingNodes,
     const Intersection* goal);

    /// 経路のRoutingNodeの集合を@p result_nodesに格納する
    /**
     * 派生クラスで必ず実装する
     */
    virtual void _search
    (std::vector<RoutingNode*>* result_nodes) = 0;

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

    /// 内部状態を元に戻す
    virtual void _resetPFStatus();

protected:
    /// 経路探索管理オブジェクト
    RoutingManager* _routingManager;

    /// スタート
    const RoutingNode* _startNode;

    /// ゴールの集合
    std::vector<const RoutingNode*> _goalNodes;

    /// 経路探索結果のストレージ
    RouteStorage* _routeStorage;

    /// コスト計算用のウェイト
    double _weights[VEHICLE_ROUTING_PARAMETER_SIZE];

    /// 使用中フラグ
    /**
     * 処理がバッティングしないようにするためのフラグ
     */
    bool _isInUse;

    /// ログ
    PFLogger* _logger;

    /// 使用主のID
    std::string _id;

    /// 使用主の車種
    VehicleType _vehicleType;

    /// 乱数生成器
    RandomGenerator* _rnd;
};

#endif //__PF_BASE_H__
