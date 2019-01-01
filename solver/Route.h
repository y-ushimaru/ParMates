/* **************************************************
 * Copyright (C) 2014 ADVENTURE Project
 * All Rights Reserved
 **************************************************** */
#ifndef __ROUTE_H__
#define __ROUTE_H__
#include <iostream>
#include <vector>

class Intersection;

/// 大域的経路を表すクラス
/**
 * 交差点の順序列で表現される。
 *  ARouter で探索した経路を1つ格納する。
 *
 *  @ingroup Routing
 */
class Route
{
public:
    Route();
    ~Route();

    /** @name ゲート(始点・終点を含む通過すべき交差点)にまつわる関数 */
    /// @{

    /// 通過すべき交差点を設定する
    /**
     * gatesの最初の要素がスタート，最後の要素がゴール
     */
    void setGates(std::vector<const Intersection*>* gates);

    /// 通過すべき交差点を設定する
    void addGate(const Intersection* gate);

    /// 通過すべき交差点の集合を返す
    const std::vector<const Intersection*>* gates() const;

    /// 最後に通過したゲートのindexを返す
    int lastPassedGateIndex() const;

    /// 最後に通過したゲートを設定する
    void setLastPassedGate(const Intersection* gate);

    /// 最後に通過したゲートのindexを設定する
    void setLastPassedGateIndex(int lastPassedGateIndex);

    /// 始点を返す
    const Intersection* start() const;

    /// 終点を返す
    const Intersection* goal() const;

    /// @}

    /** @name 経路にまつわる関数 */
    /// @{

    /// _routeをリセットする
    void resetRoute();

    /// _routeに交差点を追加する
    void addIntersection(const Intersection* inter);

    /// 含まれる交差点を返す
    const std::vector<const Intersection*>* route() const;

    /// 最後に通過した交差点のindexを返す
    int lastPassedIntersectionIndex() const;

    /// 最後に通過した交差点を設定する
    /**
     * 同時に，必要であれば最後に通過したゲートも設定する
     */
    void setLastPassedIntersection(const Intersection* from,
                                   const Intersection* last);
                                   
    /// 最後に通過した交差点のindexを設定する
    void setLastPassedIntersectionIndex(int lastPassedIntersectionIndex);

    /// 最後に通過した交差点を設定する
    void setLastPassedIntersection(const Intersection* last);

    /// 最後に通過した交差点のindexをリセットする
    void resetLastPassedIntersectionIndex();

    /// 末尾の交差点を削除する
    void popBack();

    /// 次の交差点を返す
    /**
     * 同じ交差点を複数回通る場合，この関数では一意に決まらないので
     * できるだけnext(const Intersection*, const Intersection*)を使うこと
     */
    const Intersection* next(const Intersection* inter) const;

    /// 次の交差点を返す
    /**
     * 同じ単路を複数回通ることを想定していないが，先頭から順に探索すれば
     * 所望の機能を果たすと考えられる．
     */
    const Intersection* next(const Intersection* rear,
                             const Intersection* front) const;

    /// 含まれる交差点の数を返す
    int size() const;

    /// @}

    /// ルートを出力する
    void print(std::ostream& out) const;


#ifdef NODEF
    // 以下は旧版の関数

    /// 最後尾に交差点を追加
    void push(const Intersection*);

    /// 最後尾に複数の交差点を追加
    void push(std::vector<const Intersection*>& isec_vec);

    /// 先頭の交差点を削除
    const Intersection* pop();

    /// 最後尾の交差点を取得する
    const Intersection* back();

    /// 前の交差点を返す
    const Intersection* prev(const Intersection*) const;

    /// 含まれる交差点を返す
    const std::vector<const Intersection*>* route()  const;

    /// @p i 番目の交差点を返す
    const Intersection* inter(int) const;

    /// この経路のコストを返す
    double cost() const;

    /// この経路のコストを設定する
    void setCost(double);
#endif

protected:
    /// 経路本体
    std::vector<const Intersection*> _route;

    /// 最後に通過した交差点のindex
    int _lastPassedIntersectionIndex;

    /// 通過すべきゲート（交差点）のリスト
    std::vector<const Intersection*> _gates;

    /// 最後に通過したゲートのindex
    int _lastPassedGateIndex;

#ifdef NODEF
    // 以下は旧版の変数

    /// 経路探索での価値判断に用いるコスト
    double _cost;
#endif
};
#endif
