/* **************************************************
 * Copyright (C) 2014 ADVENTURE Project
 * All Rights Reserved
 **************************************************** */
#ifndef __ROUTE_COMPONENT_H__
#define __ROUTE_COMPONENT_H__

#include <vector>
#include <iostream>
#include <algorithm>
#include "VehicleFamily.h"
#include "Conf.h"

class Intersection;
class RouteComponentKeyBase;

/// 1種類の経路探索結果を保持するためのクラス
/**
 * Route (交差点の順序列) と経路探索パラメータの対からなる．
 * RouteStorageに保存される要素
 */
class RouteComponent
{
public:
    RouteComponent();
    ~RouteComponent();

    /// 経路探索結果を格納する
    void setComponent
    (const RouteComponentKeyBase* key,
     const std::vector<const Intersection*>* route);
    
    /// 指定したのと同種の経路探索条件であるか
    bool hasEqualKey
    (RouteComponentKeyBase* key) const;
    
    /// 指定したのと同種の経路探索結果であるか
    bool isEqualComponent
    (RouteComponentKeyBase* key,
     const std::vector<const Intersection*>* route) const;

    /// 使用回数をセットする
    /**
     * キャッシュ利用時などに使う
     */
    void setCount(int count)
    {
        _count = count;
    }

    /// 使用回数を1増やす
    void countUp()
    {
        _count++;
    }

    /// 使用回数を@p nで除す
    /**
     * countが大きくなってきた場合の補正に使用する．
     * 端数の扱いは考慮していないので注意すること．
     */
    void divideCountBy(int num)
    {
        _count = std::max(1, _count/num);
    }
    
    /// 使用回数を返す
    int count() const
    {
        return _count;
    }
    
    /// 静的な結果か否かを返す
    bool isStatic() const
    {
        return _isStatic;
    }

    /// 経路を返す
    const std::vector<const Intersection*>* route() const
    {
        return &_route;
    }

    /// 指定した出力先に出力する
    void print(std::ostream& out) const;    

private:
    /// 使用回数
    int _count;
    
    /// 要素を検索する際のキー
    const RouteComponentKeyBase* _key;

    /// 静的な結果か否か
    bool _isStatic;
    
    /// 経路
    std::vector<const Intersection*> _route;
};

#endif //__ROUTE_COMPONENT_H__
