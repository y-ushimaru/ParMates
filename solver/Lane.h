/* **************************************************
 * Copyright (C) 2014 ADVENTURE Project
 * All Rights Reserved
 **************************************************** */
#ifndef __LANE_H__
#define __LANE_H__

#include <string>
#include <vector>
#include "Connector.h"
#include "AmuLineSegment.h"
#include "AmuInterval.h"
#include "TimeManager.h"

class LaneBundle;
class RoadOccupant;
class Vehicle;

/// 仮想走行レーンを表すクラス
/**
 * シミュレーション空間上のある点からある点へと向かう線分で定義される．
 *
 * 仮想走行レーンとは車が本来移動すべき道筋を表す．
 * すなわち車エージェントはレーンの位置・形状に関わりなく，
 * 自身のレーン上の一次元座標 @c length [m] を増加させることで移動する．
 * 終点にたどり着いた時点で次の隣接するレーンへ移動する．
 *
 * 単路の場合は車線にほぼ対応する．
 *
 * 交差点では単路の接続状況とその車線数にあわせて
 * 右折・直進･左折に対応するレーンが設置される．
 *
 * @ingroup RoadNetwork
 */
class Lane
{
public:
    Lane(const std::string& id,
         const Connector* ptBegin, const Connector* ptEnd,
         LaneBundle* parent);
    virtual ~Lane(){};

    /// 識別番号を設定する
    void setId(const std::string& id);

    /// 識別番号を返す
    const std::string& id() const;

    //======================================================================
    /** @name レーンの所属と接続に関する関数 */
    //@{

    /// 親オブジェクトを返す
    LaneBundle* parent() const;

    /// レーンの接続関係を構築する
    /**
     * _nextLanes, _previousLanesを作成する
     */
    virtual bool setConnection();

    /// レーンの交差関係を構築する
    virtual bool setCollision();

    /// 次のレーンの集合を返す
    const std::vector<Lane*>* nextLanes() const;

    /// @p i番目のnextLaneを返す
    Lane* nextLane(int i) const;

    /// 次の直進レーンを返す
    Lane* nextStraightLane() const;

    /// 前のレーンの集合を返す
    const std::vector<Lane*>* previousLanes() const;

    /// @p i番目のpreviousLaneを返す
    Lane* previousLane(int i) const;

    /// 前の直進レーンを返す
    Lane* previousStraightLane() const;

    /// 横のレーンを設定する
    void addSideLane(Lane* lane, bool isLeft);

    /// 横のレーンにおける位置を返す
    /**
     * @param originLength 自分のレーンの始点からの距離
     * @param isLeft trueなら左，falseなら右のレーンを探索
     * @param[out] result_sideLane 横のレーン
     * @param[out] result_length 横のレーンに投影した時の始点からの距離
     */
    virtual void getSideLaneLength(double originLength,
                                   bool isLeft,
                                   Lane** result_sideLane,
                                   double* result_length) const;

    //@}

public:
    //====================================================================
    /** @name レーン自身に関する関数 */
    //@{

    /// 始点を返す
    const Connector* beginConnector() const;

    /// 終点を返す
    const Connector* endConnector() const;

    /// レーンの長さを返す
    double length() const;

    /// 線分を返す
    const AmuLineSegment lineSegment() const;

    /// 方向ベクトルを返す
    const AmuVector directionVector() const;

    /// 制限速度を返す
    double speedLimit() const;

    /// 制限速度をセットする
    void setSpeedLimit(double limit);

    /// 最新の車両到達時刻を返す
    ulint lastArrivalTime() const;

    /// 最新の車両到達時刻をセットする
    void setLastArrivalTime(ulint arrivalTime);

    /// 始点から距離@p lengthにある点を返す
    bool getPosition(double length, AmuPoint* result_point) const;

    /// 交点を返す
    /**
     * @sa LineSegment::createIntersectionPoint()
     */
    bool createIntersectionPoint(const AmuLineSegment& crSLine,
                                 AmuPoint* result_point) const;
    /// 内分点を返す
    /**
     * @sa LineSegment::createInteriorPoint()
     */
    AmuPoint createInteriorPoint(double d0, double d1) const;

    /// 点@p pointに最も近いレーン上の点を返す
    const AmuPoint calcNearestPoint(AmuPoint point) const;

    /// @}

    //====================================================================
    /** @name レーン上のエージェントに関するもの */
    //@{

    /// レーンを使用中にする
    void setUsed();

    /// エージェントの集合を返す
    std::vector<RoadOccupant*>* agents();

    /// レーンの先頭のエージェントを返す
    RoadOccupant* headAgent();

    /// レーンの先頭の車両を返す
    Vehicle* headVehicle();

    /// レーンの最後尾のエージェントを返す
    RoadOccupant* tailAgent() const;

    /// レーンの最後尾の車両を返す
    Vehicle* tailVehicle();

    /// @p agentの一つ先のエージェントを返す
    RoadOccupant* frontAgent(RoadOccupant* agent);

    /// @p agentの一つ先の車両を返す
    Vehicle* frontVehicle(RoadOccupant* agent);

    /// @p agentの一つ先のエージェントを，レーンを越えて検索する
    /**
     * @param agent 基準となるエージェント
     * @param threshold 検索を打ち切る距離
     * @param[out] result_agent 見つかった先行エージェント
     * @param[out] result_distance 見つかったエージェントまでの距離
     */
    void getFrontAgentFar(RoadOccupant* agent, double threshold,
                          RoadOccupant** result_agent,
                          double* result_distance);

    /// 位置@p lengthの点の一つ先のエージェントを返す
    RoadOccupant* frontAgent(double length);

    /// 位置@p lengthの点の一つ先の車両を返す
    Vehicle* frontVehicle(double length);

    /// 位置@p startLengthの点の一つ先のエージェントを，レーンを越えて検索する
    /**
     * @param startLength 基準となる位置
     * @param threshold 検索を打ち切る距離
     * @param[out] result_agent 見つかったエージェント
     * @param[out] result_distance 見つかったエージェントまでの距離
     */
    void getFrontAgentFar(double startLength, double threshold,
                          RoadOccupant** result_agent,
                          double* result_distance);

    /// @p agentの一つ後ろのエージェントを返す
    RoadOccupant* followingAgent(RoadOccupant* agent);

    /// @p agentの一つ後ろの車両を返す
    Vehicle* followingVehicle(RoadOccupant* agent);

    /// @p agentの一つ後ろのエージェントを，レーンを越えて検索する
    /**
     * @param agent 基準となるエージェント
     * @param threshold 検索を打ち切る距離
     * @param[out] result_agent 見つかったエージェント
     * @param[out] result_distance 見つかったエージェントまでの距離
     */
    void getFollowingAgentFar(RoadOccupant* agent, double threshold,
                              RoadOccupant** result_agent,
                              double* result_distance);

    /// 位置@p lengthの点の一つ後ろのエージェントを返す
    RoadOccupant* followingAgent(double length);

    /// 位置@p lengthの点の一つ後ろの車両を返す
    Vehicle* followingVehicle(double length);

    /// 位置@p lengthの一つ後ろのエージェントを，レーンを越えて検索する
    /**
     * @param length 基準となる位置
     * @param threshold 検索を打ち切る距離
     * @param[out] result_agent 見つかったエージェント
     * @param[out] result_distance 見つかったエージェントまでの距離
     */
    void getFollowingAgentFar(double length, double threshold,
                              RoadOccupant** result_agent,
                              double* result_distance);

    /// @p vehicleをレーンに登録する
    bool putAgent(RoadOccupant* agent);

    /// エージェントの順序列を更新する
    void renewAgentLine();

    /// 道路勾配を返す
    double gradient() const;

    /// レーン上の車両エージェントの平均速度を返す
    double averageVel() const;

    /// 指定した車両を削除する
    void deleteVehicle(Vehicle* vehicle);

    //@}

    //====================================================================
    /** @name その他 */
    //@{

    /// 情報を表示する
    virtual void print() const;

    //@}

protected:
    /// 識別番号
    /**
     * レーンを含むオブジェクト内で一意になるように設定される
     */
    std::string _id;

    /// 親オブジェクト
    /**
     * 自身が含まれるレーン束オブジェクト
     */
    LaneBundle* _parent;

    /// 自分の次のレーンの集合
    std::vector<Lane*> _nextLanes;

    /// _nextLanesの中で自分との角度差が最小のもの
    Lane* _nextStraightLane;

    /// 自分の前のレーンの集合
    std::vector<Lane*> _previousLanes;

    /// _previousLanesの中で自分との角度差が最小のもの
    Lane* _previousStraightLane;

    /// 自分の右のレーンの集合
    /**
     * @note Intersectionのレーンでは使用しない
     */
    std::map<AmuInterval, Lane*> _rightLanes;

    /// 自分の左のレーンの集合
    /**
     * @note Intersectionのレーンでは使用しない
     */
    std::map<AmuInterval, Lane*> _leftLanes;

    /// 交差点内交錯レーンの集合
    /**
     * @note
     * 現状，Sectionのレーンでは使用しない．
     * キープする
     */
    std::vector<Lane*> _collisionLanesInIntersection;

    /// 交差点上流単路部内交錯レーンの集合
    /**
     * @note
     * 現状，Sectionのレーンでは使用しない．
     */
    std::vector<Lane*> _collisionLanesInSection;

    /// 制限速度[km/h]
    double _speedLimit;

    /// 最新の車両到達時刻[msec]
    ulint _lastArrivalTime;

    /// 始点
    const Connector* _beginConnector;

    /// 終点
    const Connector* _endConnector;

    /// レーンを表す線分
    const AmuLineSegment _lineSegment;

    /// このレーンが使用されているか
    /**
     * agentの動作時にセットされ，_isUsed=trueのレーンのみ
     * renewAgentLineが実行される．
     * renewAgentLineの最後に_isUsed=falseに戻される
     *
     * @attention
     * 1ステップ前にエージェントがいた車線もtrueとする
     * (_agentsをクリアする必要があるため)
     */
    bool _isUsed;

    /// このレーンに配置されているRoadOccupant
    /**
     * エージェントが行動した後で_tmpAgentsに登録し、
     * renewAgentLineで_tmpAgentsから_agentにコピーする
     */
    std::vector<RoadOccupant*> _agents;

    /// このレーンに配置されているRoadOccupant
    /**
     * エージェントが行動した後で_tmpAgentsに登録し、
     * renewAgentLineで_tmpAgentsから_agentにコピーする
     */
    std::vector<RoadOccupant*> _tmpAgents;

    /// 道路勾配
    double _gradient;

    //====================================================================

  public:

    /// 領域境界かどうかのフラグを更新する
    void setIsShared(bool isShared);

    /// 領域境界かどうかのフラグを返す
    bool isShared();

    /// レーン上のエージェントを消去する
    void deleteAgent();

    /// 隣接領域に存在する仮想オブジェクトをレーンに登録する
    void putVirtualAgent(RoadOccupant* agent);
   
    /// エージェント列を仮に更新する
    void renewAgentLineTmp();
 
  protected:

    /// 領域境界かどうかのフラグ
    bool _isShared;

    /// 隣接領域に存在する仮想オブジェクト
    std::vector<RoadOccupant*> _virtualAgents;

};

#endif //__LANE_H__
