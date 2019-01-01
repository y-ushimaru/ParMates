/* **************************************************
 * Copyright (C) 2014 ADVENTURE Project
 * All Rights Reserved
 **************************************************** */
#ifndef __INTERSECTION_H__
#define __INTERSECTION_H__

#include <vector>
#include <string>
#include "Signal.h"
#include "AmuLineSegment.h"
#include "AmuPoint.h"
#include "LaneBundle.h"
#include "RelativeDirectionTable.h"
#include "Border.h"
#include "TimeManager.h"
#ifdef INCLUDE_TRAMS
#include "tram/IntersectionTramExt.h"
#endif //INCLUDE_TRAMS
#ifdef INCLUDE_PEDESTRIANS
#include "pedestrian/IntersectionPedExt.h"
#endif //INCLUDE_PEDESTRIANS

class RoadMap;
class Lane;
class RoadEntity;
class Section;
class Connector;
class Vehicle;
class RouteStorage;

/// 交差点クラス
/**
 * 交差点のタイプを流入点／流出点の個数で記述する。
 *
 * @ingroup RoadNetwork
 */
class Intersection : public LaneBundle
{
#ifdef INCLUDE_TRAMS
    friend class IntersectionTramExt;
#endif //INCLUDE_TRAMS
#ifdef INCLUDE_PEDESTRIANS
    friend class IntersectionPedExt;
#endif //INCLUDE_PEDESTRIANS

public:
    Intersection(const std::string& id,
                 const std::string& type,
                 RoadMap* parent);
    virtual ~Intersection();

    //====================================================================
    /** @name 幾何形状に関するもの */
    /// @{

    /// 交差点の中心点を返す
    const AmuPoint center() const;

    /// 交差点の中心点をセットする
    void addCenter(AmuPoint center);

    /// @p inDir から @p outDir までの代表長さを返す
    double length(int inDir, int outDir) const;

    /// 交差点@p interに接続する辺を返す
    const AmuLineSegment edgeToNextInter(const Intersection* inter) const;

    /// 境界番号を辺番号に変換する
    int dir2edge(int i) const;

    /// 辺番号を境界番号に変換する
    int edge2dir(int i) const;

    /// @}

    //====================================================================
    /** @name 道路構造に関するもの */
    /// @{

    /// レーンチェック
    bool checkLane();

    /// 端コネクター、横断歩道考慮
    const Connector* edgeConnector(int idInt);

    /// @p laneが接続する次のレーン束オブジェクトを返す
    LaneBundle* nextBundle(Lane* lane) const;

    /// @p laneが接続する前のレーン束オブジェクトを返す
    LaneBundle* previousBundle(Lane* lane) const;

    /// 隣接交差点の個数を返す
    int numNext() const;

    /// 隣接する交差点をセットする
    void setNext(Intersection* ptInter);

    /// @p i番目の境界方向に隣接する交差点を返す
    Intersection* next(int i) const;

    /// 交差点@p from から交差点@p toに通過することができるか
    bool isReachable(const Intersection* from,
                     const Intersection* to) const;

    /// 交差点@p interに接続するか
    /**
     * 直接接続する必要はなく，同一のグラフ上に存在すればtrueを返す
     */
    bool isNetworked(const Intersection* start,
                     const Intersection* inter);

    /// 交差点@p interに接続するか
    bool isNetworked(std::vector<const Intersection*>* alreadySearched,
                     const Intersection* inter);

    /// 交差点@p interに向かう単路を返す
    Section* nextSection(const Intersection* inter) const;

    /// @p i番目の境界方向に接続する単路を返す
    Section* nextSection(int i) const;

    /// 交差点の@p laneに接続する単路を返す
    Section* nextSection(Lane* lane) const;

    /// 交差点の@p laneへ接続する単路を返す
    Section* previousSection(Lane* lane) const;

    /// 境界の集合を返す
    std::vector<Border*> borders() const;

    /// @p i番目の境界を返す
    const Border* border(int i) const;

    /// 境界をセットする
    void addBorder(Border* border, int edge);

    /// 交差点@p interに隣接する境界番号を返す
    int direction(const Intersection* inter) const;

    /// 単路@p sectionに接続する境界番号を返す
    int direction(const Section* section) const;

    /// サブセクション@p entityが接する境界番号を返す
    int direction(const RoadEntity* entity) const;

    /// コネクタ@p connectorを持つ境界番号を返す
    int direction(const Connector* inter) const;

    /// レーン@p laneが接続する境界番号（上流側）を返す
    std::vector<int> directionsFrom(const Lane* lane) const;

    /// レーン@p laneが接続する境界番号（下流側）を返す
    std::vector<int> directionsTo(const Lane* lane) const;

    /// 境界方向@p directionの反対の境界番号を返す
    int oppositeDirection(int direction) const;

    /// レーン@p laneが交差点内中心レーンかどうか
    bool isMainLane(const Lane* lane) const;

    /// レーン@p laneから辿って境界@p direction に達するか
    bool isReachable(const Lane* lane, int direction) const;

    /// 中心サブセクションを通るレーンを返す
    std::vector<Lane*> mainLanes() const;

    /// @p laneと交錯するレーンを返す
    std::vector<Lane*> collisionLanes(Lane* lane) const;

    /// @p lanesと交錯するレーンの集合を取得する
    /**
     * @param lanes 交錯を判定するレーン
     * @param result_inter 交差点内の交錯レーン
     * @param result_section 交錯レーンに接続する上流単路内のレーン
     */
    void collisionLanes(const std::vector<Lane*>* lanes,
                        std::vector<Lane*>* result_inter,
                        std::vector<Lane*>* result_section);

    /// collisionLanesのうち、交点の位置が@p lengthより前方のものを返す
    std::vector<Lane*> collisionLanesFront(Lane* lane,
                                           double length) const;

    /// サブセクション@p entityの辺@p edgeと接するサブセクションを返す
    RoadEntity* pairedEntity(RoadEntity* entity, int edge) const;

    /// @p in0から@p in1という経路は右折か
    bool isRight(Intersection* in0, Intersection* in1) const;

    /// @p lane は右折レーンか
    bool isRight(const Lane* lane) const;

    /// @p in0から@p in1という経路は左折か
    bool isLeft(Intersection* in0, Intersection* in1) const;

    /// @p lane は左折レーンか
    bool isLeft(const Lane* lane) const;

    /// @p in0から@p in1という経路は直進か
    bool isStraight(Intersection* in0, Intersection* in1) const;

    /// @p lane は直進レーンか
    bool isStraight(const Lane* lane) const;

    /// @p in0から@p in1という経路の相対方向を返す
    RelativeDirection relativeDirection(Intersection* in0,
                                        Intersection* in1) const;

    /// @p 境界dir0から見た境界dir1の相対方向を返す
    RelativeDirection relativeDirection(int dir0, int dir1) const;

    /// @p lane の相対方向を返す
    RelativeDirection relativeDirection(const Lane* lane) const;

    /// @p dir方向の流入点数を返す
    int numIn(int dir) const;

    /// @p dir方向の流出点数を返す
    int numOut(int dir) const;

    /// @}

    //======================================================================
    /** @name 信号に関するもの */
    /// @{

    /// 信号をセットする
    void attachSignal(Signal* signal);

    /// 信号を返す
    Signal* signal() const;

    /// @p from から 進行方向 @p turning に向かう車両の進入許可を返す
    Signal::SignalPermission permission(int from,
                                        RelativeDirection turning,
                                        Vehicle* vehicle) const;

    /// @}

    //======================================================================
    /** @name エージェントや経路選択に関するもの */
    /// @{

    /// 経路探索結果の格納器を返す
    RouteStorage* routeStorage();

    /// 経路探索結果の格納器を削除する
    void deleteRouteStorage();

    /// @p fromから @p toへの通過時間を登録する
    void addPassTime(int from, int to, ulint passTime);

    /// @p fromから @p toへの平均通過時間を返す(ローカル経路探索用)
    double averagePassTime(int from, int to) const;

    /// @p fromから @p toへの平均通過時間を返す
    double averagePassTimeForGlobal(int from, int to) const;

    /// @p fromから全方向への平均通過時間を返す(グローバル経路探索用)
    double averagePassTimeForGlobal(int from) const;

    /// _previousPassTimeTotal, _previousNumPassedVehicleTotalを更新する
    void renewPassTimeForGlobal();

    /// _passTimeTotal, _numPassedVehicleTotalをクリアする
    void clearPassTimeForGlobal();

    /// @}

    //====================================================================
    /// 情報を表示する
    void dispMapInfo() const;

    /// 詳細情報を出力する
    void printDetail(bool odNode) const;

protected:
    /// 中心点
    AmuPoint _center;

    /// 境界
    std::vector<Border*> _borders;

    /// 境界番号から辺番号へのマッピング
    /**
     * 境界番号がインデックス、要素の値が辺番号
     */
    std::vector<int> _dir2edge;

    /// 境界間の相対方向テーブル
    RelativeDirectionTable* _rdTable;

    /// 隣接する交差点
    /**
     * 重複を許す。隣接交差点の重複は単路が複数あることを表す
     */
    std::vector<Intersection*> _next;

    /// 接続する単路
    /**
     * キーは境界番号
     */
    std::vector<Section*> _incSections;

    /// 各境界からの流入レーン数
    std::vector<int> _numIn;

    /// 各境界への流出レーン数
    std::vector<int> _numOut;

    /// この交差点に設置されている信号
    Signal* _signal;

    /// 経路探索結果の格納器
    /**
     * 並列性および結果の再現性を考慮してIntersectionに持たせる．
     * from, next->goalのfromに相当する交差点が結果を保持する．
     * ODノードは必ずfromで指定される．
     */
    RouteStorage* _routeStorage;

    /// 前の単路に進入してから交差点の通過に要した時間 (個別に保持)
    std::vector<std::list<ulint> > _passTime;

    /// 前の単路に進入してから交差点の通貨に要した時間 (合計)
    /**
     * 指定された間隔でクリアされる
     */
    std::vector<ulint> _passTimeTotal;

    /// 前の単路に進入してから交差点の通貨に要した時間 (合計)
    /**
     * 指定された間隔で_passTimeTotalにより上書きされる
     */
    std::vector<ulint> _previousPassTimeTotal;

    /// 交差点を通過した方向別の台数
    /**
     * 指定された間隔でクリアされる
     */
    std::vector<ulint> _numPassedVehicleTotal;

    /// 交差点を通過した方向別の台数
    std::vector<ulint> _previousNumPassedVehicleTotal;

    /// 構造情報が設定されたかどうか
    bool _hasStructInfo;

    /// 歩道幅、横断歩道幅
    double _sidewalkWidth;
    double _crosswalkWidth;

    /// 車道多角形の頂点
    /**
     * 横断歩道がある場合は内側、そうでなければ頂点と同じ
     * 構造情報設定時に外側の頂点より先に設定する
     * 内側の点だけ先に計算して外側から作る、内→外→内の順
     * ファイルで設定した値はこちらに入れる、11/07 以前と動きが違う
     */
    std::vector<AmuPoint> _streetVertices;

    /// 車道境界の点
    /**
     * 横断歩道の外で単路の車道接続点、頂点と同時に作成する
     * 横断歩道がなければ頂点と同じ
     */
    std::vector<AmuPoint> _borderPoints;

    /// 入力したレーン開始終了位置
    std::vector<std::string> _laneBegEnd;

    //====================================================================
    /** @name 路面電車用拡張 */
    /// @{

#ifdef INCLUDE_TRAMS
public:
    /// 路面電車用拡張オブジェクトを返す
    IntersectionTramExt* tramExt();

protected:
    /// 路面電車用拡張オブジェクト
    IntersectionTramExt* _tramExt;
#endif //INCLUDE_TRAMS

    //====================================================================
    /** @name 歩行者用拡張 */
    /// @{

#ifdef INCLUDE_PEDESTRIANS
public:
    /// 路面電車用拡張オブジェクトを返す
    IntersectionPedExt* pedExt();

protected:
    /// 路面電車用拡張オブジェクト
    IntersectionPedExt* _pedExt;
#endif //INCLUDE_PEDESTRIANS

    /// @}

    //====================================================================
public:
    /**
     * @name 交差点の詳細構造を作成する関数群
     * @note 長いので別ファイルで定義する
     * @sa IntersectionBuilder.cpp
     */
    /// @{

    /// 構造情報を設定する、設定済みなら無視する
    bool setStructInfo(double sidewalkWidth, double crosswalkWidth);
    /// 基礎構造を生成する
    bool createBaseStructure(bool readFile);

    /// 交差点の詳細構造を作成する
    bool create();

    // 接続情報を単路から受け取り、構造情報を完成させる、今は使ってない
    //void connect();

    /// 接続する単路をセットする
    /**
     * 単路の生成段階で呼ばれる
     */
    void setIncidentSection(int dir, Section* ptSection);

    /// 相対方向テーブルの作成
    virtual bool createDefaultRDTable();

    /// 車道頂点の作成
    virtual bool createDefaultStreetVertices();
    /// 頂点の作成
    virtual bool createVertices();
    /// 境界の作成
    virtual bool createDefaultBorders();
    /// サブセクションの作成
    virtual bool createDefaultSubsections();
    /// コネクタの作成
    virtual bool createDefaultConnectors();
    /// レーンの作成
    virtual bool createDefaultLanes();

    /// ファイルから相対方向テーブルの作成
    virtual bool createRDTableFromFile(std::ifstream* isf);
    /// ファイル入力文字からRelativeDirection形式へ
    RelativeDirection atoRD(std::string* str);
    /// ファイルから読み込んだ車道頂点座標を_streetVerticesに格納
    virtual bool createStreetVerticesFromFile(std::ifstream* isf);
    /// ファイルからレーン情報の読み込み
    bool readLanesFromFile(std::ifstream* isf);
    /// ファイル入力した情報からレーンの作成
    virtual bool createLanesFromFile();
    /// １レーンの作成
    void createLane(int idInt, const Connector* pointBegin,
                    const Connector* pointEnd, bool setLaneBegEnd);

    /// 別の場所で作成したレーンの追加
    void addLane(Lane* lane, bool setLaneBegEnd);

    /// サブネットワークの構築
    bool createSubnetwork();

    /// @p i番目の隣接交差点に向かう内部方向ベクトルを返す
    /**
     * 接続単路の方向ベクトル（自身の_centerと隣接交差点の_centerを結んだもの）
     * とは限らない
     */
    const AmuVector _insideDirectionVector(int i);

    /// @p line 上にconnectorを作成する
    void _createConnectorsOnLine(AmuLineSegment line,
                                 int numIn, int numOut, int borderId);

    /// 境界@p dirの横断歩道の幅
    /**
     * 交差点の内部構造を決定するときに用いるので，
     * 実際に横断歩道が設置されているわけではない
     */
    double crosswalkWidth(int dir);

    /// @}

    //====================================================================
    // parallel

  public:

    /// 所属するプロセスIDをセットする, 再分割のたびに呼ばれる
    void setProcessId(int processId);

    /// プロセスIDを返す
    int processId();

    // 交差点要素レーンを登録する
    void addfactorLanes();

    // 交差点要素の単路上レーンを登録する
    void addfactorsectionLanes(Lane* lane, Section* section);

    // 交差点要素に既に登録されているかのフラグを返す
    bool addedfactorLanes(Lane* lane);

    // 交差点要素に含まれるレーン群を返す
    std::vector<Lane*>* interfactorLanes();

    // 交差点要素のエージェント数をカウントする
    double countAgentsNum();
    
    // 交差点要素のエージェント計算時間を計測する
    double countCalcTime();

    // 交差点要素の重みを加算する
    void addWeight(double weight);

    // 交差点要素の重みを返す
    double weight();

    // 交差点要素の重みをクリアする
    void clearWeight();

  protected:

    /// 交差点を担当するプロセスID
    int _processId;

    // 交差点要素の重み
    double _weight;

    // 交差点要素に含まれるレーン群
    std::vector<Lane*> _interfactorLanes;

  };

#endif //__INTERSECTION_H__
