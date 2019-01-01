/* **************************************************
 * Copyright (C) 2014 ADVENTURE Project
 * All Rights Reserved
 **************************************************** */
#ifndef __SECTION_H__
#define __SECTION_H__

#include <vector>
#include <map>
#include <string>
#include <list>
#include "AmuVector.h"
#include "AmuPoint.h"
#include "LaneBundle.h"
#include "VehicleFamily.h"

#include <iostream>

class Intersection;
class RoadMap;

#ifdef INCLUDE_TRAMS
class SectionTramExt;
#endif //INCLUDE_TRAMS

/// 単路クラス
/**
 * @note Intersectionと共通の関数はLaneBundleに移行
 * @ingroup RoadNetwork
 */
class Section : public LaneBundle
{
public:
#ifdef INCLUDE_TRAMS
    friend class SectionTramExt;
#endif //INCLUDE_TRAMS

    Section(const std::string& id,
            Intersection* first, Intersection* second,
            RoadMap* parent);
    ~Section();
 
    //====================================================================
    /** @name 幾何形状に関するもの */
    /// @{

    /// 中心点を返す
    const AmuPoint center() const;

    /// 道路の長さを返す
    /**
     * @todo 単路をn角形(n>4)にする場合には要変更
     */
    double length() const;

    /// 道路幅を返す（レーンの長さ単位）
    /**
     * 現在の設定ではレーンごとの幅が一定であるため
     * 単にレーン数を返す
     */
    double width() const;

    /// 上り方向の道路幅を返す
    double upWidth() const;

    /// 下り方向の道路幅を返す
    double downWidth() const;

    /// @}
    //====================================================================
    /** @name 道路構造に関するもの */
    /// @{

    /// @p laneが接続する次のレーン束オブジェクトを返す
    LaneBundle* nextBundle(Lane* lane) const;

    /// @p laneが接続する前のレーン束オブジェクトを返す
    LaneBundle* previousBundle(Lane* lane) const;

    /// 指定した方向の交差点を返す
    /**
     * _adjInter[0]->_adjInter[1]が上り方向
     * isUpがtrueであれば_adjInter[1]を
     * isUpがfalseであれば_adjInter[0]を返す
     */
    Intersection* intersection(bool isUp) const;

    /// 指定した単路と共有する交差点を返す
    Intersection* sharedIntersection(const Section* another) const;

    /// 単路の端点のうち指定した以外の端点を返す
    Intersection* anotherIntersection(const Intersection* inter) const;

    /// 単路の@p laneから接続する交差点を返す
    Intersection* nextIntersection(Lane* lane) const;

    /// 単路の@p laneへ接続する交差点を返す
    Intersection* previousIntersection(Lane* lane) const;

    /// @p laneの次のレーンの集合を返す
    std::vector<Lane*> nextLanes(const Lane* lane) const;

    /// @p laneの前のレーンの集合を返す
    std::vector<Lane*> previousLanes(const Lane* lane) const;

    /// レーン@p laneから辿って，次の交差点の境界@p direction に達するか
    bool isReachable(const Lane* lane, int dir) const;

public:
    /// サブセクション@p entityの辺@p edgeと接するサブセクションを返す 
    /**
     * 接続する交差点も検索する
     */
    RoadEntity* pairedEntity(RoadEntity* entity, int edge) const;
    
    /// 指定された上り/下り方向レーンの集合を返す
    std::vector<Lane*> lanesWithDirection(bool isUp) const;

    /// 交差点 @p inter から単路に流入するレーンの集合を返す
    std::vector<Lane*> lanesFrom(const Intersection* inter) const;

    /// 単路から @p inter へ流出するレーンの集合を返す
    std::vector<Lane*> lanesTo(const Intersection* inter) const;

    /// レーンが上り方向かどうか
    /**
     * _adjInter[0]->_adjInter[1]が上り方向
     */
    bool isUp(const Lane* lane) const;

    /// @p fromから@p toが上り方向かどうか
    bool isUp(const Intersection* from,
              const Intersection* to) const;

    /// @}
    //====================================================================
    /** @name エージェントに関するもの */
    /// @{

protected:
    /// 単路の@p start地点から距離@p lenにいる@p isUp方向のエージェント数を返す
    int _numAgents(double start, double len, bool isUp) const;

public:
    /// 上り方向の@p start地点から距離@p lenにいるエージェント数を返す
    int numUpAgents(double start, double len) const;

    /// 下り方向の@p start地点から距離@p lenにいるエージェント数を返す
    int numDownAgents(double start, double len) const;

    /// 上り方向の平均速度を返す
    double averageUpVel() const;

    /// 下り方向平均速度を返す
    double averageDownVel() const;

    /// @p in1 から @p in2 へ向かう方向に通過するのに予想される所要時間を返す
    double time(Intersection* in1, Intersection* in2) const;

    /// 上り方向の予想所要時間を返す
    double upTime() const;

    /// 下り方向の予想所要時間を返す
    double downTime() const;

    /// 車種IDと確率を探索確率テーブルに追加する
    void addRoutingProbability(bool isUp, VehicleType type, 
                               double probability);

    /// 車種IDを単路通行のホワイトリストに追加する
    void addVehicleTypeToWhiteList(bool isUp, std::string type);

    /// ホワイトリストを返す
    const std::vector<std::string>* whitelistedVehicleTypes(bool isUp) const;

    /// 車種IDを単路通行のブラックリストに追加する
    void addVehicleTypeToBlackList(bool isUp, std::string type);

    /// ブラックリストを返す
    const std::vector<std::string>* blacklistedVehicleTypes(bool isUp) const;

    /// 経路探索時に@p typeの車両が単路の@p isUp 方向を展開する確率を返す
    double routingProbability(bool isUp, VehicleType type) const;

    /// 経路探索時に@p typeの車両が単路の@p inter 方向から展開する確率を返す
    double routingProbability(Intersection* inter, VehicleType type) const;

    /// 経路探索確率を返す
    const std::map<VehicleType, double>*
    routingProbabilities(bool isUp) const;
    
    /// 単路の@p isUp 方向を@p typeの車両が通行できるか
    bool mayPassVehicleType(bool isUp, VehicleType type);

    /// 単路の@p inter 方向から@p typeの車両が通行できるか
    bool mayPassVehicleType(Intersection* inter, VehicleType type);

    /// 車道を通行可能な歩行者種別を返す, 戻り値 false なら通さない
    bool streetTrafficWalkerTypes(std::vector<int> &walkerTypes);

    /// 歩行者種別が単路全体を通行可能かどうかを返す
    bool mayPassWalkerType(int walkerType);

    /// 車列最後尾情報を出力するかどうかを返す
    bool isConvoyMonitored(bool isUp) const;

    /// 車列最後尾情報の出力を設定する
    void setConvoyMonitored(bool isUp, bool isMonitored);

    /// @}
    //====================================================================
protected:
    /// 中心点(代表点)
    AmuPoint _center;

    /// 接続交差点
    Intersection* _adjInter[2];

    /// 境界からの流入レーン数
    /**
     * 隣の交差点の該当する境界からの流出レーン数
     */
    int _numIn[2];

    /// 境界への流出レーン数
    /**
     * 隣の交差点の該当する境界への流入レーン数
     */
    int _numOut[2];

    /// 構造情報が設定されたかどうか
    bool _hasStructInfo;

    /// レーン幅、路側幅、歩道幅（右/左）
    double _laneWidth;
    double _roadsideWidth;
    double _sidewalkWidthRight;
    double _sidewalkWidthLeft;

    /// 単路通行のホワイトリスト (通行許可)
    /**
     * 添字0は下り (isUpがfalse)，添字1は上り (isUpがtrue)を表す．
     *
     * @attention
     * ホワイトリストを優先する．
     * もしホワイトリストにいずれかの車種が指定されている場合は
     * ブラックリストへの登録にかかわらず他の車種を通行禁止とみなす．
     */
    std::vector<std::string> _whitelistedVehicleTypes[2];

    /// 単路通行のブラックリスト (通行禁止)
    /**
     * 添字0は下り (isUpがfalse)，添字1は上り (isUpがtrue)を表す
     */
    std::vector<std::string> _blacklistedVehicleTypes[2];

    /// 経路探索時の探索確率テーブル
    /**
     * 経路探索アルゴリズムの中で確率的にノードを展開する．
     * (実際にはリンクのコストを確率的に無限大とする．)
     *
     * 添字0は下り (isUpがfalse)，添字1は上り (isUpがtrue)を表す
     */
    std::map<VehicleType, double> _routingProbability[2];

    /// 車道を通行可能な歩行者種別
    /**
     * 設定ファイルの値で ANY や重複のチェックはしてない
     */
    std::vector<int> _streetTrafficWalkerTypes;

    /// 車列最後尾情報を出力するか否か
    /**
     * 0: 下り方向(isUp=false), 1: 上り方向(isUp=true)
     */
    bool _isConvoyMonitored[2];

    //====================================================================
    /** @name 路面電車用拡張 */
    /// @{

#ifdef INCLUDE_TRAMS
public:
    /// 路面電車用拡張オブジェクトを返す
    SectionTramExt* tramExt();

protected:
    /// 路面電車用拡張オブジェクト
    SectionTramExt* _tramExt;
#endif //INCLUDE_TRAMS

    /// @}

    //====================================================================
public:
    /**
     * @name 単路の詳細構造を作成する関数群
     * @note 長いので別ファイルで定義する
     * @sa SectionBuilder.cpp
     */
    ///@{

    /// 構造情報を設定する、設定済みなら無視する
    bool setStructInfo(double laneWidth, double roadsideWidth,
                       double sidewalkWidthRight, double sidewalkWidthLeft,
                       const std::vector<int> &streetTrafficWalkerTypes,
                       bool setDefault, int autoSidewalkLane);

    /// 詳細構造を作成する
    bool create();
    /// 接続情報を交差点に渡し、構造情報を完成させる
    void connect();

    /// ファイル指定がない場合の作成関数
    bool createFromTemplate();
    /// 頂点の作成
    bool createDefaultVertices();
    /// サブセクションの作成
    bool createDefaultSubsections();
    /// レーンの作成
    bool createDefaultLanes();

    /// 別の場所で作成したレーンの追加
    void addLane(Lane* lane);

    // ファイルから作成する
    // bool createFromFile();

    /// サブネットワークの構築
    bool createSubnetwork();

    /// レーン幅と路側幅を返す[m]
    double laneWidth() const;
    double roadsideWidth() const;

    /// 交差点@p interから見て{左,右}側に歩道を設定するか
    /**
     * 歩道が設定されているかどうか，ではない
     */
    double sidewalkWidth(Intersection* inter, bool leftSide) const;

    ///@}
};

#endif //__SECTION_H__
