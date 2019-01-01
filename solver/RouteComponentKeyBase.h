/* **************************************************
 * Copyright (C) 2014 ADVENTURE Project
 * All Rights Reserved
 **************************************************** */
#ifndef __ROUTE_COMPONENT_KEY_BASE_H__
#define __ROUTE_COMPONENT_KEY_BASE_H__

#include <iostream>
#include <vector>
#include "VehicleFamily.h"
#include "Conf.h"

class Intersection;

// 経路探索結果格納器に格納する要素のキー
/**
 * キーを拡張可能にするためクラスを作成し適宜派生する
 */
class RouteComponentKeyBase
{
public:
    RouteComponentKeyBase(){};
    RouteComponentKeyBase(const VehicleType type,
                          const double weights[],
                          const Intersection* before,
                          const Intersection* start,
                          const Intersection* goal);

    virtual ~RouteComponentKeyBase(){};

    /// 車種を返す
    VehicleType type() const
    {
        return _type;
    }

    /// @p i番目の経路探索パラメータを返す
    double weights(unsigned int i) const
    {
        return _weights[i];
    }

    /// 起点に至る交差点を返す
    const Intersection* before() const
    {
        return _before;
    }

    /// 起点を返す
    const Intersection* start() const
    {
        return _start;
    }

    /// 終点を返す
    const Intersection* goal() const
    {
        return _goal;
    }

    /// キーを構成する要素が一致するかどうかを返す
    virtual bool isEqualKey
    (RouteComponentKeyBase* anotherKey) const;

    /// キーの情報を出力する
    virtual void print(std::ostream& out) const;
    
protected:
    /// 車種
    /**
     * 車種ごとに通れる道路が異なるため，探索結果が異なる
     */
    VehicleType _type;

    /// 経路探索パラメータ
    double _weights[VEHICLE_ROUTING_PARAMETER_SIZE];

    /// 起点に至る交差点
    const Intersection* _before;

    /// 起点
    const Intersection* _start;

    /// 終点
    const Intersection* _goal;
};

#endif //__ROUTE_COMPONENT_KEY_BASE_H__
