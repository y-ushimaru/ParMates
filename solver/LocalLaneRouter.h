/* **************************************************
 * Copyright (C) 2014 ADVENTURE Project
 * All Rights Reserved
 **************************************************** */
#ifndef __LOCAL_LANE_ROUTER_H__
#define __LOCAL_LANE_ROUTER_H__

#include <string>
#include <map>
#include <iostream>
#include <cstdlib>
#include "RoadMap.h"
#include "Intersection.h"
#include "ODNode.h"
#include "Section.h"
#include "Route.h"
#include "LocalLaneRoute.h"
#include "GLColor.h"

class Vehicle;

/// 局所的経路を探索するクラス。
/**
 *  Vehicle での利用の形態から事実上車線変更を行うときとそうでないときは
 *  結果の LocalLaneRoute の中身の仕様が異なる．
 *
 *  実際，車線変更を必要とする場合は VehicleLaneShifter との連携を必要とする．
 *
 *  状態の取得には，isSearched() は直接到達可能かどうかのみを返し，
 *  可能な経路が全く見つからない場合は LocalLaneRoute に何も入れないという
 *  方法を取る．要区別．
 *
 *  @sa LocalRoute
 *  @ingroup Routing
 */
class LocalLaneRouter
{
public:
    LocalLaneRouter();
    ~LocalLaneRouter();

    /// 全体構造の設定
    void setRoadMap(RoadMap* roadMap);

    /// 担当する車両をセットする
    void setVehicle(Vehicle* vehicle);
  
    /// 大域経路の設定
    void setRoute(Route* route);

    /// 結果を格納するオブジェクトの設定
    /**
     * @param localRoute 探索して得られた局所的経路を格納するための変数をセット
     */
    void setLocalRoute(LocalLaneRoute* localRoute);

    /// 現在の位置から直接到達可能な経路，およびその場で車線変更して到達可能な経路を探索する.
    /**
     * Routeに2つ先までの Intersection がセットされている必要がある．
     * （1つ先が終端の場合を除く）
     */
    void localReroute(const Section* section,
                      const Lane* lane,
                      const double length);

    /// localRerouteの横車線のみ探索版
    /**
     * 直接到達可能であるが，空いている車線に移るための探索に使用されることを想定している．
     * 直接到達可能な経路が探索済みであることを前提とする．
     */
    void localRerouteSide(const Section* section,
                          const Lane* lane,
                          const double length);

    /// 経路が見つかったか
    /** 車線変更を必要とする経路がセットされている場合はfalseを返す */
    bool isSearched() const;

    /// 隣のレーン探索中か
    /* 
     * 車線変更を行う前に隣のレーンを取得する．
     * 車両の現在位置からレーンの方向に直交する探索ベクトルを作成し，
     * それが他のレーンと交わった場合に「隣のレーン」と判断している．
     * ところが，単路に入った直後には（単路の形状によっては）
     * 探索ベクトルが他のどのレーンとも交わらない場合がある．
     * その場合，走行しながら探索レーンを取得する必要がある．
     */
    bool isSearchingSideLanes() const;

    /// 車線変更の方向を返す
    /**
     * @return +1 左
     * @return -1 右
     * @return  0 必要なし
     */
    int shiftDirection() const;

    /// 探索結果を消去する
    /**
     * 道路地図と大域経路はそのまま
     */
    void clear();

private:
    /// localReroute の本体
    bool _search(const Section* section,
                 const Lane* lane,
                 const double length,
                 std::vector<Lane*>* result_lanes0,
                 std::vector<Lane*>* result_lanes1);

    /// localRerouteSide の本体
    bool _searchSide(const Section* section,
                     const Lane* lane,
                     const double length,
                     std::vector<Lane*>* result_lanes);

    /// レーンの左右のレーンを取得
    /**
     * lane から幅方向に車線変更した場合に移動可能なレーンを探索する.
     * @return レーンが一つでも見付かったか
     * @param result_lanes 移動先のレーン
     * @param result_length 移動先のレーン上での位置.インデックスは reslt_lane に対応.
     * @param section レーンを含む単路
     * @param lane   レーン
     * @param length レーン上の位置
     */
    bool _getSideLanes(std::vector<Lane*>* result_lanes,
                       std::vector<double>* result_length, 
                       const Section* section,
                       const Lane* lane,
                       const double length) const;
    void _getLeftSideLanes(std::vector<Lane*>* result_lanes,
                           std::vector<double>* result_length,
                           const Section* section,
                           const Lane* lane,
                           const double length) const;
    void _getRightSideLanes(std::vector<Lane*>* result_lanes,
                            std::vector<double>* result_length,
                            const Section* section,
                            const Lane* lane,
                            const double length) const;

    /// 直接到達可能な経路を探索
    /**
     * @p startLane からその先に接続するレーンを順次たどっていき,
     * @p frontIntersection で @p objectiveDirection につながるレーンに辿り着いたら探索終了.
     * @note 複数の経路がある場合も最初の一つしか探さない.
     * @note @p length は不要？
     */
    std::vector<Lane*> _straightSearch(const Section* section,
                                  const Lane* startLane,
                                  const double length,
                                  Intersection* frontIntersection,
                                  int objectiveDirection,
                                  int objectiveDirection2,
                                  bool includeShiftLane) const;


    /// _straightSearch()の本体．再帰処理
    /**
     * 交差点を目的の方向に曲がるレーンにたどり着くまで辿る．
     * @p result_lanes をスタックとして用い，探索中のレーンを順に積み上げる．
     *
     * @param[in,out] result_lanes 結果のレーン列
     * @param section 現在いる道路区間
     * @param startLane 現在いるレーン
     * @param frontIntersection  ひとつ先の交差点
     * @param objectiveDirection @p frontIntersection で曲がりたい方向
     * @param objectiveDirection2 @p frontIntersection の次で曲がりたい方向
     * @param includeShiftLane 車線変更が含まれるかどうか
     */
    bool _getExitLanesInStraightRoute(std::vector<Lane*>* result_lanes,
                                      Section* section,
                                      Lane* startLane,
                                      Intersection* frontIntersection,
                                      int objectiveDirection,
                                      int objectiveDirection2,
                                      bool includeShiftLane) const;

    /// _straightSearch()の本体．再帰処理
    /**
     * 従来の_searchは交差点に入るまで（交差点に入った最初のレーンまで）の探索であったが，
     * 新フォーマットの採用に伴い交差点内部のレーン分岐を許したため，
     * 交差点を出るまで（次の単路の最初のレーンまで）のレーンのリストを返すようにした．
     * この関数は_getExitLanesInStraightRouteの交差点版
     */
    bool _getExitLanesInFrontIntersection(std::vector<Lane*>* result_lanes,
                                          Intersection* intersection,
                                          Lane* startLane,
                                          int objectiveDirection,
                                          int objectiveDirection2,
                                          bool includeShiftLane) const;


    /// 全ての分岐点で直進するレーン（レーンの方向ベクトルの変化が最小）を選択する経路を求める
    /**
     * 車線変更が必要な場合，
     * 車線変更を含めた経路はLocalRoute::_desiredLocalRouteに保存される
     * このとき，実際の（車線変更を行わない）経路には直進する経路が設定される
     * 
     * 車線変更が実行された場合，_desiredLocalRouteが_localRouteにコピーされる
     * 車線変更が実行されない場合，_desiredLocalRouteに関係なく_localRouteを走行する
     */
    std::vector<Lane*> _alternativeSearch(const Section* section,
                                     const Lane* startLane,
                                     const double length,
                                     Intersection* frontIntersection) const;

    /// @p thatLaneは@p thisLaneよりも空いているか
    /**
     * @p thisLaneの@p thisLengthの前方の車両と
     * @p thatLaneの@p thatLengthの前方の車両の速度を比較する
     *
     * 車線変更によって混んでいる車線を避けるための判定に用いるので，
     * 前方が赤信号のときには常にfalseを返す．
     */
    bool _isUncrowdedLane(const Section* section,
                          const Lane* thatLane,
                          const double thatLength,
                          const Lane* thisLane,
                          const double thisLength) const;

    bool _isUncrowdedRoute(const std::vector<Lane*>* route0,
                           const std::vector<Lane*>* route1) const;

    bool _isUncrowdedRoute(const Section* section,
                           const Intersection* intersection,
                           const Lane* thatLane,
                           const Lane* thisLane) const;

private:
    /// 担当するVehicle
    Vehicle* _vehicle;

    /// 直接到達可能な経路が見つかったか
    bool _isSearched;

    /// 隣のレーン探索中か
    bool _isSearchingSideLanes;

    /// 車線変更の方向
    /*
     * +1: 左
     *  0: 車線変更必要無し
     * -1: 右
     */
    int _shiftDirection;

    /// 道路地図全体
    const RoadMap* _roadMap;

    /// 大域経路
    const Route* _route;

    /// 見つかった局所経路
    LocalLaneRoute* _localRoute;
};

#endif // __LOCAL_LANE_ROUTER__
