/* **************************************************
 * Copyright (C) 2014 ADVENTURE Project
 * All Rights Reserved
 **************************************************** */
#ifndef __LANE_BUNDLE_H__
#define __LANE_BUNDLE_H__

#include <vector>
#include <map>
#include <unordered_map>
#include <string>
#include <list>
#include "AmuPoint.h"

class RoadMap;
class Lane;
class RoadEntity;
class Connector;
class AmuLineSegment;
class RoadOccupant;
class Vehicle;

typedef std::unordered_map
<std::string, Lane*> RMAPLAN;
typedef std::unordered_map
<std::string, Lane*>::iterator ITRMAPLAN;
typedef std::unordered_map
<std::string, Lane*>::const_iterator CITRMAPLAN;

typedef std::unordered_map
<std::string, Connector*> RMAPCON;
typedef std::unordered_map
<std::string, Connector*>::iterator ITRMAPCON;
typedef std::unordered_map
<std::string, Connector*>::const_iterator CITRMAPCON;

typedef std::unordered_map
<std::string, RoadEntity*> RMAPENT;
typedef std::unordered_map
<std::string, RoadEntity*>::iterator ITRMAPENT;
typedef std::unordered_map
<std::string, RoadEntity*>::const_iterator CITRMAPENT;

/// レーン束クラス
/**
 * 単路と交差点の基底クラス
 *
 * @ingroup RoadNetwork
 */
class LaneBundle
{
public:
    LaneBundle(const std::string& id, RoadMap* parent);
    virtual ~LaneBundle();

    //====================================================================
    /** @name 幾何形状に関するもの */
    /// @{

    /// 頂点の数を返す
    int numVertices() const;

    /// 中心点を返す
    virtual const AmuPoint center() const = 0;

    /// i番目の頂点を返す
    const AmuPoint vertex(int i) const;

    /// 頂点を追加する
    void addVertex(AmuPoint vertex);

    /// i番目の辺を返す
    /**
     * i番目の辺とは始点が頂点i,終点が(i+1)%numCornersの線分を指す。
     */
    const AmuLineSegment edge(int i) const;

    /// @}

    //====================================================================
    /** @name 道路構造に関するもの */
    /// @{

    /// 識別番号を返す
    const std::string& id() const;

    /// @p laneが接続する次のレーン束オブジェクトを返す
    virtual LaneBundle* nextBundle(Lane* lane) const = 0;

    /// @p laneが接続する前のレーン束オブジェクトを返す
    virtual LaneBundle* previousBundle(Lane* lane) const = 0;

    /// このレーン束オブジェクト内の@p laneの位置@p lengthから前方の端までの距離を返す
    double lengthToNext(const Lane* lane, double length) const;

    /// このレーン束オブジェクト内の@p laneの位置@p lengthから後方の端までの距離を返す
    double lengthFromPrevious(const Lane* lane, double length) const;

    /// 内部コネクタの集合を返す
    const RMAPCON* innerConnectors() const;

    /// 識別番号@p idの内部コネクタを返す
    const Connector* connector(const std::string& id) const;

    /// エンティティの集合を返す
    const RMAPENT* entities() const;

    /// エンティティ@p entityはこのレーン束オブジェクトに含まれるかどうか
    bool isMyRoadEntity(const RoadEntity* entity) const;

    /// 車道エンティティを返す
    RoadEntity* streetEntity();

    /// 内部レーンの集合を返す
    const RMAPLAN* lanes() const;

    /// @p laneはこのレーン束オブジェクトに含まれるかどうか
    bool isMyLane(const Lane* lane) const;

    /// @p laneの次のレーンはこのレーン束オブジェクトに含まれるか
    bool isNextLaneMine(const Lane* lane) const;

    /// @p laneの前のレーンはこのレーン束オブジェクトに含まれるか
    bool isPreviousLaneMine(const Lane* lane) const;

    /// 内部レーンのうち、@p connectorを始点とするレーンの集合を返す
    std::vector<Lane*> lanesFromConnector(const Connector* connector) const;

    /// 内部レーンのうち、@p connectorを終点とするレーンの集合を返す
    std::vector<Lane*> lanesToConnector(const Connector* connector) const;

    /// @}

    //====================================================================
    /** @name エージェントに関するもの */
    /// @{

    /// 使用中のレーンを追加する
    void addUsedLane(Lane* usedLane);

    /// エージェント列を更新する
    void renewAgentLine();

    /// エージェント@p agentが先頭であるかどうか
    /**
     * レーンごとに判断するので，複数のエージェントが先頭になりうる
     */
    bool isHeadAgent(RoadOccupant* agent, Lane* lane) const;

    /// _watchedVehiclesを返す
    std::list<Vehicle*>* watchedVehicles();

    /// @p vehicleを_watchedVehicleに追加する
    void addWatchedVehicle(Vehicle* vehicle);

    /// @p vehicleを_watchedVehicleから削除する
    void eraseWatchedVehicle(Vehicle* vehicle);

    /// @}
    //====================================================================
protected:
    /// 識別番号
    std::string _id;

    /// 親ロードマップ
    RoadMap* _parent;

    /// 多角形の頂点
    std::vector<AmuPoint> _vertices;

    /// 内部コネクタ
    /**
     * @note ObjManagerでnew&deleteする
     *
     * コネクタのローカルID(mapのキー)は4桁とする<br>
     * 内部コネクタ[9] + グループ番号[1桁] + ポイントID<br>
     * レーンIDは始点コネクタと終点コネクタのIDの組で表す
     */
    RMAPCON _innerConnectors;

    /// 内部エンティティ
    RMAPENT _entities;

    /// 内部レーン
    RMAPLAN _lanes;

    /// 使用中のレーン
    std::vector<Lane*> _usedLanes;

    /// このレーン束が使用されているか
    /**
     * agentの動作時にセットされ，_isUsed=trueのレーン束のみ
     * renewAgentLineが実行される．
     * renewAgentLineの最後に_isUsed=falseに戻される．
     */
    bool _isUsed;

    /// 他の車両から注目を集める行動(車線変更など)をとっている車両
    /**
     * 車線変更中であるので，LaneではなくSectionに持たせる．
     * これはLane::_vehiclesのように各タイムステップでrenewされない．
     * 従って明示的なaddとeraseが必要
     * なお，Lane::_vehiclesと違ってlengthでソートされない
     */
    std::list<Vehicle*> _watchedVehicles;

        //====================================================================

      public:
        /// parallel

        /// エージェントを消去する
        void deleteAgent();
        
        /// エージェント列を仮に更新する
        void renewAgentLineTmp();

};

#endif //__LANE_BUNDLE_H__
