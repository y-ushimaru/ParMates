/* **************************************************
 * Copyright (C) 2014 ADVENTURE Project
 * All Rights Reserved
 **************************************************** */
#ifndef __ROUTE_IO_H__
#define __ROUTE_IO_H__

class RoadMap;
class RoutingManager;
class PFBase;

/// 経路に関する入出力を行うクラス
/**
 * @ingroup Routing
 */
class RouteIO
{
private:
    RouteIO(){};
    ~RouteIO(){};

public:
    /// 唯一のインスタンスを返す
    static RouteIO& instance();

    /// 経路探索結果をファイル入力する
    /**
     * 入力情報は経路探索器に依存する
     */
    void readRouteCache(RoadMap* roadMap,
                        RoutingManager* routingManager);

    /// 経路探索結果をファイル出力する
    /**
     * 出力情報は経路探索器に依存する
     */
    void writeRouteCache(RoadMap* roadMap,
                         RoutingManager* routingManager);
};

#endif //__ROUTE_IO_H__
