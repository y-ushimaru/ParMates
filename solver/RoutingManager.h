/* **************************************************
 * Copyright (C) 2014 ADVENTURE Project
 * All Rights Reserved
 **************************************************** */
#ifndef __ROUTING_MANAGER_H__
#define __ROUTING_MANAGER_H__

#include <vector>
#include "VehicleFamily.h"

class RoadMap;
class RoutingNetwork;
class RoutingNode;
class Route;
class RouteStorage;
class PFBase;

/**
 * @addtogroup Routing
 * @brief 経路と経路探索機能を定義するモジュール
 */

/// 大域的経路探索を管理するクラス
/**
 * 経路探索用地図(RoutingMap)と経路探索器(Router)を管理する．
 *
 * @ingroup Routing
 */
class RoutingManager
{
public:
    RoutingManager();
    ~RoutingManager();

    /// RoadMapをセットする
    void setRoadMap(RoadMap* roadMap);

    /// RoutingNetworkを返す
    RoutingNetwork* routingNetwork();

    /// ランク @p rank のRoutingNetworkを返す
    RoutingNetwork* routingNetwork(unsigned int rank);

    /// RoutingNetworkの最高ランクを返す
    unsigned int maxRank() const;

    /// 経路探索用地図を作成する
    bool getReadyRoutingNetwork();

    /// 初期リンクコストを付与する
    bool setInitialCosts();

    /// リンクコストを更新する
    bool renewCosts();

    /// 使用された経路探索結果コンテナを追加する
    void addUsedRouteStorage(RouteStorage* routeStorage);

    /// 経路探索結果のコンテナを更新する
    void renewRouteStorage();

    /// 経路探索結果のコンテナを削除する
    void deleteAllRouteStorage();

    /// 利用可能な経路探索器を返す
    PFBase* assignPathfinder();

    /// 経路探索器を利用状態から開放する
    void releasePathfinder(PFBase* pathfinder);
    
    /// 管理するオブジェクトをすべて削除する
    void deleteAll();

    /// すべての経路探索器を削除する
    void deleteAllPathfinders();

private:
    /// 道路地図
    RoadMap* _roadMap;

    /// 経路探索用地図
    /**
     * 階層化を行う場合があるためvectorとする
     * 階層化した場合のランクとvectorのインデックスを一致させる
     */
    std::vector<RoutingNetwork*> _routingNetwork;

    /// 経路探索用地図の最高ランク
    unsigned int _maxRank;

    /// 経路探索器
    /**
     * 必要に応じてnewするため，最後でdeleteが必要
     */
    std::vector<PFBase*> _pathfinders;

    /// 更新のあった経路探索結果コンテナ
    /**
     * _usedRouteStorageの要素に対してのみ
     * renewRouteResultを実行する．
     * renewRouteStorageの最後にクリアする．
     */
    std::vector<RouteStorage*> _usedRouteStorage;
};

#endif //__ROUTING_MANAGER_H__
