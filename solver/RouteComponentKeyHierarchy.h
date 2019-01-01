/* **************************************************
 * Copyright (C) 2014 ADVENTURE Project
 * All Rights Reserved
 **************************************************** */
#ifndef __ROUTE_COMPONENT_KEY_HIERARCHY_H__
#define __ROUTE_COMPONENT_KEY_HIERARCHY_H__

#include <iostream>
#include <vector>
#include "RouteComponentKeyBase.h"
#include "VehicleFamily.h"
#include "Conf.h"

class Intersection;

// 経路探索結果格納器に格納する要素のキー
/**
 * キーを拡張可能にするためクラスを作成し適宜派生する
 */
class RouteComponentKeyHierarchy : public RouteComponentKeyBase
{
public:
    RouteComponentKeyHierarchy() : _prefRank(0) {};
    RouteComponentKeyHierarchy(const VehicleType type,
                               const unsigned int prefRank,
                               const double weights[],
                               const Intersection* before,
                               const Intersection* start,
                               const Intersection* goal);

    virtual ~RouteComponentKeyHierarchy(){};

    /// キーを構成する要素が一致するかどうかを返す
    virtual bool isEqualKey
    (RouteComponentKeyBase* anotherKey) const;

    /// キーの情報を出力する
    virtual void print(std::ostream& out) const;
    
private:
    /// 選好するネットワークランク
    const unsigned int _prefRank;

public:
    /// 選好するネットワークランクを返す
    unsigned int prefRank() const
    {
        return _prefRank;
    }
};

#endif //__ROUTE_COMPONENT_KEY_HIERARCHY_H__
