/* **************************************************
 * Copyright (C) 2014 ADVENTURE Project
 * All Rights Reserved
 **************************************************** */
#ifndef __LOCAL_LANE_ROUTE_H__
#define __LOCAL_LANE_ROUTE_H__

#include <vector>
#include "RelativeDirection.h"
class Lane;

/// 局所的経路を格納するクラス
/**
 * 局所的経路とは,現在いる単路とその次の交差点内部の経路.
 * レーンの順序列で表現される.
 *
 * 現在のレーンから次の交差点の先のレーンまで
 * 直接到達可能か車線変更が必要かどうかで格納のされ方が異なることに注意．
 *
 * @sa LocalLaneRouter
 *
 * @ingroup Routing
 */
class LocalLaneRoute
{
public:
    LocalLaneRoute();
    ~LocalLaneRoute();

    /// 初期化
    void clear();

    /// 内部のベクターを返す
    const std::vector<Lane*>& localRoute() const;

    /// 直接到達可能なレーン列を設定する
    void setLocalRoute(const std::vector<Lane*>& localRoute);

    /// 希望経路のレーン列を設定する
    void setDesiredLocalRoute(const std::vector<Lane*>& desiredLocalRoute);

    /// 次のレーンを取得する
    Lane* next(const Lane* presentLane) const;

    /// 希望経路の次のレーンを取得する
    Lane* desiredNext(const Lane* presentLane) const;

    /// 前のレーンを取得する
    Lane* previous(const Lane* presentLane) const;

    /// 先頭のレーンを取得する
    Lane* begin() const;

    /// 希望経路の先頭のレーンを取得する
    Lane* desiredBegin() const;

    /// 最後のレーンを取得する
    Lane* last() const;

    /// 中身が入っているか
    bool empty() const
    {
        return _localRoute.empty();
    }

    /// 中身が入っているか
    bool desiredEmpty() const
    {
        return _desiredLocalRoute.empty();
    }

    /// 次の交差点における進行方向を設定する
    void setTurning(RelativeDirection rd)
    {
        _turning = rd;
    }

    /// 次の交差点における進行方向を返す
    RelativeDirection turning() {
        return _turning;
    }

    /// 交差点内中心レーンを設定する
    void setMainLaneInIntersection(Lane* lane)
    {
        _mainLaneInIntersection = lane;
    }

    /// 交差点内中心レーンを返す
    Lane* mainLaneInIntersection()
    {
        return _mainLaneInIntersection;
    }

    /// 交差点内レーンを_lanesInIntersectionに格納する
    void addLaneInIntersection(Lane* lane)
    {
        _lanesInIntersection.push_back(lane);
    }

    /// 交差点内レーンをクリアする
    void clearLanesInIntersection()
    {
        _lanesInIntersection.clear();
    }

    /// 交差点内レーンの集合を返す
    const std::vector<Lane*>* lanesInIntersection() const;

    /// 内部変数をコンソールに出力.デバッグ用.
    void print() const;

protected:
    /// 車線変更を行わないレーン列
    /*
     * 車両は必ずこのレーン列を通る
     */
    std::vector<Lane*> _localRoute;

    /// 車線変更を含めたレーン列
    /*
     * 車線変更を行わずに希望の経路を選択できる場合は
     * _desiredLocalRouteは空である．
     */
    std::vector<Lane*> _desiredLocalRoute;

    /// このレーン列を進む場合，次の交差点でどの相対方向に進むか
    RelativeDirection _turning;

    /// 次の交差点の中心レーン
    Lane* _mainLaneInIntersection;

    /// 次の交差点で通過するレーンの集合
    /**
     * 必ず通過順に格納する必要がある。
     * _localRouteを設定した後に頭からforループを回して
     * 交差点内レーンか判定しながらaddすれば上記の条件を満足するはず。
     */
    std::vector<Lane*> _lanesInIntersection;

    //======================================================================
    // parallel

  public:

    /// 局所的経路を追加する
    void addLocalRoute(Lane* lane);

    /// 次の交差点で通過するレーンを追加する
    void addLanesInIntersection(Lane* lane);

};

#endif
