/* **************************************************
 * Copyright (C) 2014 ADVENTURE Project
 * All Rights Reserved
 **************************************************** */
#ifndef __VEHICLE_H__
#define __VEHICLE_H__

#include <vector>
#include <string>
#include <deque>
#include "Conf.h"
#include "RoadOccupant.h"
#include "AmuVector.h"
#include "LocalLaneRouter.h"
#include "LocalLaneRoute.h"
#include "Blinker.h"
#include "VehicleFamily.h"
#include "VehicleLaneShifter.h"
#include "RelativeDirection.h"
#include "TimeManager.h"

class Simulator;
class RoadMap;
class LaneBundle;
class Intersection;
class Section;
class Lane;
class ARouter;
class Route;
class VirtualLeader;
class RandomGenerator;
#ifdef INCLUDE_PEDESTRIANS
class VehiclePedExt;
#endif //INCLUDE_PEDESTRIANS

/**
 * @addtogroup Vehicle
 * @brief 自動車エージェント
 * @ingroup Agent
 */

/// 自動車エージェントの基底クラス
/**
 * @ingroup Vehicle
 */
class Vehicle : public RoadOccupant
{
    // フレンド宣言
    friend class VehicleLaneShifter;
#ifdef INCLUDE_PEDESTRIANS
    friend class VehiclePedExt;
#endif //INCLUDE_PEDESTRIANS

public:
    Vehicle();
    virtual ~Vehicle();

    //==================================================================
    /**
     * @name 認知に関する関数
     *
     * @note VehicleRecognizer.cppで定義されている
     */
    //@{

    /// 周囲の状況を認識する
    virtual void recognize();

    //@}

protected:
    //==================================================================
    /**
     * @name 認知に関するprotected関数
     *
     * @note VehicleRecognizer.cppで定義されている
     */
    //@{

    /// 希望速度を決定する
    void _determineDesiredVelocity(RelativeDirection turning);

    /// 先行エージェントを調べる
    /**
     * @note
     * Lane::getFrontAgentFarに類似するが，
     * Lane版はnextStraightLaneを次々と検索するのに対し，
     * Vehicle版はLocalRouteに沿って検索する
     *
     * @param threshold 検索を打ち切る距離
     */
    void _searchFrontAgent(double threshold);

    /// 前方の制限速度を調べる
    /**
     * @param threshold 検索を打ち切る距離
     */
    void _searchFrontSpeedLimit(double threshold);

    /// 信号によって停止するかどうか調べる
    /**
     * @param nextInter 次に進入する交差点
     * @param prevInter 既に通過したあるいは通過中の交差点
     * @param turning @p nextInterでの転回方向
     * @param threshold 検索を打ち切る距離
     * @return 次の交差点手前で停車するか
     */
    bool _isStoppedBySignal(Intersection* nextInter,
                            Intersection* prevInter,
                            RelativeDirection turning,
                            double threshold);

    /// 交差点内交錯レーンに車両があるか調べる
    /**
     * @note 単路を走行している車両が呼び出す
     * @return 次の交差点手前で停車するか
     */
    bool _isStoppedByCollisionInIntersection(Intersection* nextInter,
                                             std::vector<Lane*>* clInter,
                                             RelativeDirection turning);

    /// 単路から交差点上流単路内交錯レーンに車両があるか調べる
    /**
     * @note 単路を走行している車両が呼び出す
     * @return 次の交差点手前で停車するか
     */
    bool _isStoppedByCollisionInSection(Intersection* nextInter,
                                        std::vector<Lane*>* clSection,
                                        RelativeDirection turning);

    /// 右左折時交差点内に先行車がいないか調べる
    /**
     * @note 単路を走行している車両が呼び出す
     * @return 次の交差点手前で停車するか
     */
    bool _isStoppedByLaneInInter(Intersection* nextInter,
                                 RelativeDirection turning);

    /// 交差点下流（通過後）に十分な空きがあるか調べる
    /**
     * 先の単路内のレーン最後尾の車が交差点に近い位置に停車しているとき，
     * 渋滞が交差点内まで延伸するのを防ぐために手前で停止する必要がある
     *
     * @note 単路を走行している車両が呼び出す
     * @return 次の交差点手前で停車するか
     */
    bool _isStoppedByShortSpace();

    /// 最小ヘッドウェイの影響を調べる
    /**
     * 交差点右左折時には最小ヘッドウェイ（時間）が定義されており，
     * 車両はこれから進入しようとする交差点に最後に車両が進入してから
     * 少なくとも最小ヘッドウェイ分だけ間隔を空けなければならない
     *
     * @note 単路を走行している車両が呼び出す
     * @return 次の交差点に進入できるか
     */
    bool _isStoppedByMinHeadway(Intersection* nextInter,
                                RelativeDirection turning);

    /// 右左折前に事前に減速する
    /**
     * @note 単路を走行している車両が呼び出す
     */
    void _determineTurningVelocity(RelativeDirection turning);

    /// 交差点内を走行中の優先車両を調べる
    void _searchPreferredAgentInIntersection(RelativeDirection turning);

    /// 相手を「見る」ことができるか
    /**
     * ここで言う「見る」とは，適用するアプリケーションによって
     * 目視によるものか，通信によるものか意味合いが異なる
     */
    bool _isVisible(Vehicle* other) const;


    /// 交差点への進入を相手に譲るか
    bool _isYielding(Intersection* inter,
                     int thisDir,
                     int thatDir,
                     RelativeDirection turning,
                     Vehicle* other);

    //@}

public:
    //======================================================================
    /**
     * @name 加速度決定に関する関数
     *
     * @note VehicleAccelDeterminer.cppで定義されている
     */
    //@{

    /// VirtualLeaderをもとに加速度を決定する
    virtual void determineAcceleration();

    //@}

    //======================================================================
    /** @name 移動に関する関数
     *
     * @note VehicleRunner.cppで定義されている
     */
    //@{

    /// 移動する
    virtual void run();

    //@}

protected:
    //======================================================================
    /**
     *@name 移動に関するprotected関数
     *
     * @note VehicleRunner.cppで定義されている
     */
    //@{

    /// 次に進むべきレーンを決める
    void _decideNextLane(Intersection* inter, Lane* lane);

    /// 次に進むべきレーンを決める
    void _decideNextLane(Section* section, Lane* lane);

    /// 交差点内でレーンを移る
    void _runIntersection2Intersection();

    /// 交差点から単路に移る
    void _runIntersection2Section();

    /// 単路内でレーンを移る
    void _runSection2Section();

    /// 単路から交差点に移る
    void _runSection2Intersection();

    /// @p interのレーン@p laneに自身をセットする
    bool _setLane(Intersection* inter, Lane* lane, double length);

    /// @p sectionのレーン@p laneに自身をセットする
    bool _setLane(Section* section, Lane* lane, double length);

    //@}

public:
    //====================================================================
    /** @name 車両属性へのアクセッサ */
    //@{

    /// 識別番号を返す
    const std::string& id() const;

    // 識別番号を設定する
    void setId(const std::string& id);

    /// 車種を返す
    VehicleType type() const;

    /// 車種を設定する
    void setType(VehicleType type);

    /// 車体の幅を返す
    double bodyWidth() const;

    /// 車体の長さを返す
    double bodyLength() const;

    /// 車体の高さを返す
    double bodyHeight() const;

    /// 車体の連接数を返す
    int bodyArticulation() const;

    /// 車体の大きさを設定する
    void setBodySize(double length, double width, double height);

    /// 車体の連接数を設定する
    void setBodyArticulation(int articulation);

    /// 性能を設定する
    void setPerformance(double accel, double brake);

    /// 車体色を設定する
    void setBodyColor(double r, double g, double b);

    /// 車体色を返す
    void getBodyColor(double* result_r,
                      double* result_g,
                      double* result_b) const;

    //@}

    //====================================================================
    /** @name 道路上の位置に関する変数へのアクセッサ */
    //@{

    /// 単路@p sectionのレーン@p lane上の距離@p lengthに登場する
    /**
     * @p roadMapの登録も兼ねる(発生時のみ呼び出される)
     */
    virtual bool addToSection(RoadMap* roadMap, Section* section,
		              Lane* lane, double length);

    /// レーンの始点からの距離を返す
    double length() const;

    /// 前タイムステップでの距離を返す
    double oldLength() const;

    /// 交差点や単路の始点からの距離を返す
    double totalLength() const;

    /// トリップ長を返す
    double tripLength() const;

    /// x座標を返す
    double x() const;

    /// y座標を返す
    double y() const;

    /// z座標を返す
    double z() const;

    /// 現在所属するレーン束を返す
    LaneBundle* laneBundle() const;

    /// 現在所属する単路を返す
    Section* section() const;

    /// 現在所属する交差点を返す
    Intersection* intersection() const;

    /// 前に所属していた交差点を返す
    Intersection* prevIntersection() const;

    /// 現在所属するレーンを返す
    Lane* lane() const;

    /// 次に走行するレーンを返す
    Lane* nextLane() const;

    /// 現在のレーンの直前に走行していたレーンを返す
    Lane* prevLane() const;

    /// 発生点から離れていて，結果に反映される自動車であるか
    /**
     * 発生時は速度が0であり，これは非現実的な設定である（本来は速度を持っているはず）．
     * これにより排気排出量に影響が生じる恐れがある．
     * 従って，発生点に近すぎる自動車は出力から除外する必要がある．
     *
     * 結果として出力しないだけであり，速度計算は通常通り行う．
     * なおその距離はNO_OUTPUT_LENGTH_FROM_ORIGIN_NODEというキーで
     * GVManagerの中で管理されている（初期値は0）．
     */
    bool isAwayFromOriginNode() const;

    //@}

public:
    //====================================================================
    /** @name 動きに関する変数へのアクセッサ */
    //@{

    /// 速度を返す
    double velocity() const;

    /// 速度ベクトルを返す
    const AmuVector velocityVector() const;

    /// 速度の平均値を返す
    double aveVelocity() const;

    /// 加速度を返す
    double accel() const;

    /// 速度の方向ベクトルを返す
    const AmuVector directionVector() const;

    ///レーン中心線からのずれを返す
    double error() const;

    /// 現在所属するレーン束オブジェクトに車線変更中であることを通知する
    void notify();

    /// 現在所属するレーン束オブジェクトに車線変更が終わったことを通知する
    void unnotify();

    /// ウィンカーを返す
    Blinker blinker() const;

    /// 次の交差点に流入する境界番号を返す
    int directionFrom() const;

    /// 次の交差点から流出する境界番号を返す
    int directionTo() const;

    /// 車線変更挙動定義オブジェクトを返す
    VehicleLaneShifter& laneShifter();

    /// 休止状態かどうか
    bool isSleep() const;

    /// 出発時刻を設定する
    void setStartTime(ulint startTime);

    /// 出発時刻を取得する
    ulint startTime() const;

    /// 出発ステップを返す
    ulint startStep() const;

    /// 生成時刻を取得する
    ulint genTime () const;

    /// 仮想先行エージェントの集合を返す
    const std::vector<VirtualLeader*>* virtualLeaders() const;

    //@}

public:
    //======================================================================
    /** @name 経路探索に関する関数 */
    //@{

    /// 経路探索パラメータをセットする
    void setRoutingParams(std::vector<double> params);

    /// 経路探索で選好するネットワークランクをセットする
    void setPreferredNetworkRank(int networkRank);

    /// 経路を返す
    Route* route();

    /// 経路探索オブジェクトを返す
    ARouter* router();

    /// 経路を探索して自身の_routeに設定する
    bool reroute(const Intersection* rear,
                 const Intersection* front);

    /// 交差点で通過するレーンの集合を返す
    const std::vector<Lane*>* lanesInIntersection() const;

    /// 局所的経路オブジェクトを返す
    LocalLaneRoute* localRoute();

    //@}

#ifdef _OPENMP
    //======================================================================
    /** @name マルチスレッドに関する関数 */
    //@{

    /// reroute 回数を返す（負荷確認）
    int rerouteCnt() const;

    /// reroute 回数を消去する
    void clearRerouteCnt();

    //@}
#endif //_OPENMP

    //======================================================================
    /** @name その他 */
    //@{

    /// シミュレータをセットする
    void setSimulator(Simulator* sim)
    {
        _sim = sim;
    }

    /// 乱数生成器へのポインタを返す
    /**
     * 外部からこの車両が持つ乱数生成器にアクセスする場合に利用する
     */
    RandomGenerator* randomGenerator();

    /// 情報を出力する
    void print() const;

    //@}

protected:
    //====================================================================
    /** @name 車両属性に関する変数 */
    //@{

    /// 識別番号
    std::string _id;

    /// 車種
    /**
     * 中身はint型．VehicleFamily.hを参照．
     */
    VehicleType _type;

    /// 車体の幅
    double _bodyWidth;

    /// 車体の長さ
    double _bodyLength;

    /// 車体の高さ
    double _bodyHeight;

    /// 車体の連接数
    int _bodyArticulation;

    /// 車体色の赤成分
    double _bodyColorR;

    /// 車体色の緑成分
    double _bodyColorG;

    /// 車体色の青成分
    double _bodyColorB;

    //@}

    //====================================================================
    /** @name 道路上の位置に関する変数 */
    //@{

    /// シミュレータオブジェクト
    Simulator* _sim;

    /// 現在自分が存在する道路ネットワーク
    RoadMap* _roadMap;

    /// 自分がいる交差点
    Intersection* _intersection;

    /// 自分がいた交差点
    Intersection* _prevIntersection;

    /// 自分がいる単路
    Section* _section;

    /// 自分がいるレーン
    Lane* _lane;

    /// 次に進むレーン
    Lane* _nextLane;

    /// 前にいたレーン
    Lane* _prevLane;

    /// そのステップ内でレーンを移ったか
    bool _isLanePassed;

    /// レーン始点からの長さ[m]
    double _length;

    /// 前タイムステップでのレーン始点からの距離[m]
    /**
     * 特定のポイントを通過したタイムステップを感知する際に用いる．
     */
    double _oldLength;

    /// 単路始点からの長さ[m]
    /**
     * runSection2**で0にする
     */
    double _totalLength;

    /// トリップ長[m]
    /**
     * 発生してから消滅するまでの総走行距離
     */
    double _tripLength;

    /// レーン中心線からのずれ[m]
    double _error;

    /// 出発時刻
    ulint _startTime;

    /// 発生時刻
    ulint _genTime;

    //@}

    //====================================================================
    /** @name 動きに関する変数 */
    //@{

    /// 速度[m/msec]
    double _velocity;

    /// 速度履歴[%]
    /**
     * _vMaxに対する比率を保持する
     */
    std::deque<double> _velocityHistory;

    /// error方向の速度[m/msec]
    double _errorVelocity;

    /// 加速度[m/msec^2]
    double _accel;

    /// 最大加速度[m/msec^2]
    double _maxAcceleration;

    /// 最大減速度[m/msec^2]
    double _maxDeceleration;

    /// そのレーンにおける希望走行速度[m/msec]
    double _vMax;

    /// ウィンカー
    Blinker _blinker;

    /// 車線変更挙動定義オブジェクト
    /**
     * @note フレンドクラスのオブジェクトである
     */
    VehicleLaneShifter _laneShifter;

    /// 交錯を厳密に評価
    bool _strictCollisionCheck;

    /// 現在のセクションに入った時刻
    /**
     * _runIntersection2Sectionでリセットされる
     */
    ulint _entryTime;

    /// 現在特殊な行動（車線変更）をとっているかどうか
    bool _isNotifying;

    /// 交差点に侵入するときに一旦停止したかどうか
    bool _hasPaused;

    /// 休止時間[msec]
    /**
     * 休止状態の車両は全ての通行優先権を失う
     * 逆に言うと他車両は休止状態の車両を判断の材料としない
     */
    int _sleepTime;

    bool _isNotifySet;

    bool _isUnnotifySet;

    //@}

    //====================================================================
    /** @name 経路に関する変数 */
    //@{

    /// 経路探索パラメータ
    double _routingParams[VEHICLE_ROUTING_PARAMETER_SIZE];

    /// 選好するネットワークランク
    int _preferredNetworkRank;

    /// 大局的経路オブジェクト(Intersectionのvector)
    Route* _route;

    /// 局所的経路を生成するオブジェクト
    LocalLaneRouter _localRouter;

    /// 局所的経路オブジェクト（Laneのvector）
    LocalLaneRoute _localRoute;

    //@}

    //====================================================================
    /** @name 認知・加速度決定に関する変数 */
    //@{

    /// 仮想先行エージェントの集合
    std::vector<VirtualLeader*> _leaders;

    //@}

    /// 乱数生成器
    RandomGenerator* _rnd;

    //====================================================================
    /** @name 歩行者用拡張 */
    /// @{

#ifdef INCLUDE_PEDESTRIANS
public:
    /// 路面電車用拡張オブジェクトを返す
    VehiclePedExt* pedExt();

protected:
    /// 路面電車用拡張オブジェクト
    VehiclePedExt* _pedExt;
#endif //INCLUDE_PEDESTRIANS

    //======================================================================
    // parallel

public:

    /// 車両情報をセットする
    void setStatus(RoadMap* roadMap, std::vector<double>* status);

    /// 現在特殊な行動（車線変更）をとっているかどうかを返す
    bool isNotifying();

    /// 交差点に侵入するときに一旦停止したかどうかを返す
    bool hasPaused();

    /// 休止時間を返す
    int sleepTime();

    /// 経路探索パラメータを返す
    double* routingParams();

    /// 選好するネットワークランクを返す
    int preferredNetworkRank();

    /// 規定タイムステップあたりの計算時間を返す
    double calcTime();

    /// 計算時間を初期化する
    void clearCalcTime();

protected:

    /// 規定タイムステップあたり計算時間
    double _calcTime;
};

#endif //__VEHICLE_H__
