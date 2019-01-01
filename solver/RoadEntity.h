/* **************************************************
 * Copyright (C) 2014 ADVENTURE Project
 * All Rights Reserved
 **************************************************** */
#ifndef __ROADENTITY_H__
#define __ROADENTITY_H__

#include <string>
#include <vector>
#include <map>
#include "AmuPoint.h"
#include "AmuLineSegment.h"

class LaneBundle;
class Signal;

/// 交通のカテゴリ
/**
 * 通行権を指定するときに用いる
 */
enum TrafficCategory
{
    TRAFFIC_NONE         = 0x0000, //< 通行禁止
    TRAFFIC_WALKER       = 0x0001, //< 歩行者
    //TRAFFIC_BICYCLE    = 0x0002, //< 自転車
    TRAFFIC_VEHICLE_S    = 0x0010, //< 普通車
    TRAFFIC_VEHICLE_L    = 0x0020, //< 大型車 
    TRAFFIC_VEHICLE_BUS  = 0x0040, //< バス 
    TRAFFIC_VEHICLE_TRAM = 0x0080, //< 路面電車
    TRAFFIC_VEHICLE_ANY  =(TRAFFIC_VEHICLE_S
                           | TRAFFIC_VEHICLE_L
                           | TRAFFIC_VEHICLE_BUS
                           | TRAFFIC_VEHICLE_TRAM), //< 任意の自動車
    TRAFFIC_ANY          =(TRAFFIC_WALKER
                           | TRAFFIC_VEHICLE_ANY)   //< 任意の交通
};

/// すべて通行可能な歩行者種別
#define TRAFFIC_WALKER_TYPE_ANY  -1

/// 通行権に対する信号種別、現在は歩行者専用
#define TRAFFIC_SIGNAL_BLUE  0
#define TRAFFIC_SIGNAL_RED   1
#define TRAFFIC_SIGNAL_NUM   2

/// サブセクションのタイプ
enum SubroadFactory
{
    SUBROAD_TYPE_STREET    =  0, //< 車道
    SUBROAD_TYPE_SIDEWALK  = 12, //< 歩道
    SUBROAD_TYPE_CROSSWALK = 11  //< 横断歩道
};

/// 道路エンティティクラス
/**
 * 交差点・単路をさらに細分化するためのクラス。
 * 歩行者の歩行領域の定義などに用いることを想定している。<br>
 *
 * 単路を分割するものをSubSection、
 * 交差点を分割するものをSubIntersectionとして継承する。<br>
 *
 * なおSubSectionの形状は四角形（台形）に限定する。
 * SubIntersectionは形状の自由度が高く、凸多角形であればよい。
 *
 * @ingroup roadNetwork
 */
class RoadEntity
{
public:
    RoadEntity(const std::string& id, SubroadFactory type);
    virtual ~RoadEntity();

    //====================================================================
    /** @name 幾何形状に関するもの */
    /// @{

    /// 頂点の個数
    int numVertices() const;

    /// 中心点（重心）を返す
    const AmuPoint center() const;

    /// サブセクションの中心点をセットする
    void createCenter();

    /// i番目の頂点の座標を返す
    const AmuPoint vertex(int i) const;

    /// 頂点配列をセットする
    void addVertex(AmuPoint vertex);

    /// i番目の辺を返す
    /**
     * 始点が頂点i、終点が頂点i+1の辺
     */
    const AmuLineSegment edge(int i) const;

    /// サブセクション@p entityを持つ辺を返す
    const AmuLineSegment edge(RoadEntity* entity) const;

    /// サブセクション@p entityを持つ辺番号を返す
    int edgeNum(RoadEntity* entity) const;

    /// 点@p pointが領域内に入っているか
    /**
     * 領域の点が左回り又は右回りで順番に_verticesに格納されている必要がある．
     */
    bool isInside(const AmuPoint point) const;

    /// @}

    //====================================================================
    /** @name 道路構造に関するもの */
    /// @{

    /// 識別番号を返す
    const std::string& id() const;

    /// 自身がどのレーン束に含まれるか通知する
    void setLaneBundle(LaneBundle* bundle);

    /// 自身が含まれるレーン束を返す
    LaneBundle* laneBundle() const;

    /// サブセクションの種別を設定する
    void setType(SubroadFactory t);

    /// サブセクションの種別を返す
    SubroadFactory type() const;

    /// 通行権を設定する
    /**
     * TRAFFIC_WALKER と initWalkerType を設定すると歩行者種別を初期化する
     * 青信号ならすべての歩行者が通行可能になり、赤信号なら通行不可になる
     */
    void setPermission(TrafficCategory t, bool initWalkerType = true);

    /// 通行可能な複数の歩行者種別を設定／追加する
    /**
     * TRAFFIC_WALKER_TYPE_ANY があればすべての歩行者が通行可能になる
     */
    void setWalkerTypes(const std::vector<int> &walkerTypes,
                        int signal = TRAFFIC_SIGNAL_BLUE);
    void addWalkerTypes(const std::vector<int> &walkerTypes,
                        int signal = TRAFFIC_SIGNAL_BLUE);

    /// 交通@p tが通行可能かどうかを返す
    bool mayPass(TrafficCategory t) const;

    /// 歩行者種別が通行可能かどうかを返す
    bool mayPassWalkerType(int walkerType,
                           int signal = TRAFFIC_SIGNAL_BLUE) const;

    /// 境界@p edgeに接するサブセクションを返す
    RoadEntity* adjEntity(int edge) const;

    /// 境界@p edgeに接するサブセクションをセットする
    void addAdjEntity(RoadEntity* entity, int edge);

    /// 信号を返す
    Signal* signal() const;

    /// 交差点参照方向を返す
    int signalDirection() const;

    /// 交差点参照方向@p dirを指定して信号@p singalを設置する
    void attachSignal(Signal* signal, int dir);

    /// テスト表示
    void print() const;

    /// @}
    //====================================================================
protected:
    /// 識別番号
    std::string _id;

    /// 自身が含まれるレーン束
    LaneBundle* _bundle;

    /// サブセクションのタイプ
    SubroadFactory _type;

    /// 通行権
    TrafficCategory _permission;

    /// 通行可能な歩行者種別
    /**
     * キーも値も同じ歩行者種別、歩行者通行権がある時のみ有効である
     */
    std::map<int, int> _trafficWalkerTypes[TRAFFIC_SIGNAL_NUM];

    /// 信号
    Signal* _signal;

    /// 交差点の参照方向
    int _directionInInter;

    /// 中心点（重心）
    AmuPoint _center;

    /// 頂点列
    std::vector<AmuPoint> _vertices;

    /// 隣接するサブセクション
    std::vector<RoadEntity*> _adjEntities;

    /// 厳密な内部判定
    bool _strictJudgeInside;

    /// 歩行者種別を追加する
    void _addWalkerType(int walkerType,
                        int signal = TRAFFIC_SIGNAL_BLUE);

};

#endif //__ROADENTITY_H__
