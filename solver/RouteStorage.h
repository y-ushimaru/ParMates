/* **************************************************
 * Copyright (C) 2014 ADVENTURE Project
 * All Rights Reserved
 **************************************************** */
#ifndef __ROUTE_STORAGE_H__
#define __ROUTE_STORAGE_H__

#include "VehicleFamily.h"
#include <iostream>
#include <vector>
#include <deque>
#include <map>
#include <string>

class RoutingManager;
class RouteComponent;
class RouteComponentKeyBase;
class Intersection;
class RandomGenerator;

/// 経路要素を保持するためのクラス
/**
 * 計算時間の削減のため，過去の経路要素を保存し，
 * 同条件の探索に対し保存した情報を返す
 *
 * ファイルを読み込んでメモリに展開することにより，
 * 過去のシミュレーション結果を反映できると同時に
 * 同一回の要素も再利用できる
 *
 * @ingroup Routing
 */
class RouteStorage
{
public:
    RouteStorage();
    RouteStorage(Intersection* inter);
    ~RouteStorage();

    /// この経路要素格納器を所有するintersectionを返す
    Intersection* intersection() const;

    /// 1回の経路要素を格納する
    /**
     * 経路要素のファイル読み込みなど，
     * 処理順が確定している場合に用いる．
     */
    void addRouteComponentDirectly
    (const RouteComponentKeyBase* key,
     int count,
     const std::vector<const Intersection*>* route);

    /// 1回の経路要素を格納する
    /**
     * マルチスレッド処理の対象で，どの順で処理されるか
     * 確定しない場合に用いる
     */
    void addRouteComponent
    (const std::string& userId,
     RoutingManager* routingManager,
     const RouteComponentKeyBase* key,
     const std::vector<const Intersection*>* route);    
    
    /// 過去の経路要素を検索する
    bool searchRouteComponent
    (const std::string& userId,
     RoutingManager* routingManager,
     RouteComponentKeyBase* key,
     std::vector<const Intersection*>* result_route);

    /// 経路要素を更新する
    /**
     * 前のプロセスで_tmpComponentsに追加された要素を
     * _componentsに移す
     */
    void renewRouteComponent();

    /// 動的な経路要素を消去する
    /**
     * ネットワークのコスト更新時に呼び出される
     */
    void removeDynamicComponent();

    // 出力
    void print(std::ostream& out) const;

    // 静的な結果のみ出力する
    void printStatic(std::ostream& out) const;

private:
    /// この経路要素格納器を所有するintersection
    Intersection* _inter;

    /// 経路要素のコンテナ
    /**
     * デストラクタで要素をdeleteする．
     */
    std::deque<RouteComponent*> _components;

    /// 追加すべき経路要素を一時的に格納するコンテナ
    /**
     * 複数コアによる処理順によらず，
     * 要素の順序が一意に定まらなければならない
     */
    std::map<std::string,
             RouteComponent*,
             std::less<std::string> > _componentsAdded;

    /// 再配置すべき経路要素を一時的に格納するコンテナ
    /**
     * 複数コアによる処理順によらず，
     * 要素の順序が一意に定まらなければならない
     */
    std::map<std::string,
             RouteComponent*,
             std::less<std::string> > _componentsReordered;
   
    /// 乱数生成器
    RandomGenerator* _rnd;

    /// 使用されたかどうか
    bool _isUsed;
};

#endif //__ROUTE_STORAGE_H__
