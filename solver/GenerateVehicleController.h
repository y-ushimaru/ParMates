/* **************************************************
 * Copyright (C) 2014 ADVENTURE Project
 * All Rights Reserved
 **************************************************** */
#ifndef __GENERATE_VEHICLE_CONTROLLER_H__
#define __GENERATE_VEHICLE_CONTROLLER_H__

#include "GeneratingTable.h"
#include "GeneratingTableCell.h"
#include "VehicleFamily.h"
#include <map>
#include <unordered_map>
#include <string>
#include <vector>

class Simulator;
class RoadMap;
class Intersection;
class ODNode;
class Section;
class Vehicle;
class RandomGenerator;
class ODNodeGroup;
class Parallel;

/// 車両の発生を制御するクラス
/**
 * @ingroup Running
 */
class GenerateVehicleController
{
private:
    GenerateVehicleController();
    ~GenerateVehicleController();

public:
    /// 唯一のインスタンスを返す
    static GenerateVehicleController& instance();

    /// シミュレータをセットする
    void setSimulator(Simulator* sim)
    {
        _sim = sim;
    }

    /// RoadMapをセットする
    void setRoadMap(RoadMap* roadMap)
    {
        _roadMap = roadMap;
    }

    /// RoadMapを返す
    RoadMap* roadMap() const
    {
        return _roadMap;
    }

    /// Originノードの集合を返す
    const std::vector<ODNode*>* startNodes() const
    {
        return &_startNodes;
    }

    /// Destinationの集合を返す
    const std::vector<ODNode*>* goalNodes() const
    {
        return &_goalNodes;
    }

    /** @name  車両発生定義テーブルを返す */
    /// @{
    const GeneratingTable* table() const
    {
        return &_table;
    }

    const GeneratingTable* defaultTable() const
    {
        return &_defaultTable;
    }

    const GeneratingTable* fixedTable() const
    {
        return &_fixedTable;
    }

    const GeneratingTable* groupedTable() const
    {
        return &_groupedTable;
    }

    const GeneratingTable* randomTable() const
    {
        return &_randomTable;
    }
    /// @}

    /// 車両発生に関する初期設定を行う
    /**
     * @return 設定に成功したかどうか
     */
    bool getReadyGeneration();

private:
    /// ODノードをレベル分けする
    void _classifyODNodes(std::vector<std::string>* excludedStarts,
                          std::vector<std::string>* excludedGoals);


public:
    //==================================================================
    /** @name エージェントの発生 */
    /// @{

    /// 車両を発生させる
    void generateVehicle();

    /// 手動で車両を発生させる
    void generateVehicleManual(const std::string& startId,
                               const std::string& goalId,
                               std::vector<std::string>* gateIds,
                               VehicleType vehicleType,
                               std::vector<double> params,
                               unsigned int prefRank);

    /// @}

private:
    //==================================================================
    /** @name エージェントの発生に用いるprivate関数 */
    /// @{

    /// 条件を満たすGeneratingTableCellを有効化する
    /**
     * Generate，Default Generate，Fixed Generate Tableが持つ
     * 各GeneratingTableCellおよび発生交通量がファイルで
     * 指定されていない各交差点のGeneratingTableCellのうち，
     * 現在時刻に適用されるものを有効化し，最初の車両発生時刻を決定する．
     *
     * 以降，車両発生時に次の車両発生時刻を決定する
     */
    void _activatePresentGTCells();

    /// 次の車両発生時刻を設定する
    /**
     * 車両の発生はポアソン分布を仮定する．
     * すなわち，単位時間あたりの車両発生台数がポアソン分布に従う．
     * このとき，車両発生の時間間隔は指数分布となる．
     *
     * @param startTime 時間間隔の基準となる時刻
     * @param cell 処理するGeneratingTableCell
     * @return 有効な時刻を設定できたか
     */
    bool _addNextGenerateTime(ulint startTime,
                              const GeneratingTableCell* cell);

    /// 発生時刻が指定されたCellに従い車両発生情報を入力する
    bool _addFixedGenerateTime(const GeneratingTableCell* cell);

    /// 車両を発生させる
    void _generateVehiclesFromQueue();

    /// 優先的に車両を発生させる
    void _generateVehiclesFromPriorQueue();

    /// キューから現時点で処理すべき要素を抽出する
    /**
     * generateVehicleAllAtOnceIsOnがtrueの場合，時刻設定を無視して
     * queueに含まれるセルが定義する車両をすべて生成する．
     * fixedTableのデバッグ時に使用する．
     */
    void _extractValidCellFromQueue
    (std::multimap<unsigned long, const GeneratingTableCell*>* queue,
     std::vector<const GeneratingTableCell*>* result_validCells,
     bool generateVehicleAllAtOnceIsOn);

    /// GeneratingTableCellで指定された情報を持つ車両を生成する
    Vehicle* _generateVehicleFromGTCell(const GeneratingTableCell* cell);

    /// 車両を生成する
    /**
     * 乱数によって車両生成フラグが立った後の処理．
     * ObjManager::createVehicle()で車両を生成し，経路選択．
     *
     * 今後，経路の途中での生成も可能となるように，
     * start, goalとrear, frontを別々に設定
     */
    Vehicle* _createVehicle(ODNode* start, ODNode* goal,
                            Intersection* rear, Intersection* front,
                            const std::vector<std::string>* gateIds,
                            VehicleType vehicleType);

    /// 車体の属性を設定
    void _setVehicleBodyAttributes(Vehicle* vehicle);

    /// 経路探索パラメータをランダムに設定する
    void _setRoutingParamsRandomly(Vehicle* vehicle);

    /// 経路探索パラメータを手動で設定する
    void _setRoutingParamsManually(Vehicle* vehicle);

    /// 経路探索で選好するネットワークランクをランダムに設定する
    void _setRoutingPrefRankRandomly(Vehicle* vehicle);

    /// 経路探索で選好するネットワークランクを手動で設定する
    void _setRoutingPrefRankManually(Vehicle* vehicle);

    /// 出発地と目的地を返す
    /**
     * GeneratingTableCellが持つ識別番号をODNodeに変換する．
     * OD pairのみ別に処理する
     */
    void _getStartGoalNodes(const GeneratingTableCell* cell,
                            ODNode** result_start,
                            ODNode** result_goal);

    /// 出発地を返す
    /**
     * GeneratingTableCellが持つ識別番号をODNodeに変換する．
     * このときODNodeグループに関する処理も同時に行う．
     */
    ODNode* _getStartNode(const GeneratingTableCell* cell);

    /// 目的地を返す
    /**
     * GeneratingTableCellが持つ識別番号をODNodeに変換する．
     * このときODNodeグループに関する処理も同時に行う．
     */
    ODNode* _getGoalNode(const GeneratingTableCell* cell,
                         ODNode* start);

    /// 目的地をランダムに決定する
    ODNode* _decideGoalRandomly(ODNode* start);

    /// 経由地の識別番号を返す
    void _getGateIds(const GeneratingTableCell* cell,
                     ODNode* start, ODNode* goal,
                     std::vector<std::string>* result_gateIds);

    /// 経由地のID集合からIntersection型の経由地集合に変換する
    void _convertGateIds2Gates
    (const std::vector<std::string>* gateIds,
     std::vector<const Intersection*>* result_gates,
     ODNode* start, ODNode* goal);

    /// @}

    //==================================================================

public:
    /// ODノードのスタートレベルを返す
    /**
     * 単路への流入点でstartLebelを決定する
     * "単路の"流入点・流出点は"ODノードの"流入点・流出点と反対
     */
    int odNodeStartLevel(ODNode* node) const;

    /// ODノードのゴールレベルを返す
    /**
     * 単路からの流出点でgoalLevelを決定する
     * "単路の"流入点・流出点は"ODノードの"流入点・流出点と反対
     */
    int odNodeGoalLevel(ODNode* node) const;

    /// デフォルトのレベル別交通量を返す
    int defaultTrafficVolume(int level) const;

    /// ODグループを追加する
    /**
     * typeは"start", "goal", "pair"のいずれか
     */
    void addODGroup(const std::string& type, ODNodeGroup* group);

    /// 経路探索の効用関数に関するパラメータセットを返す
    const std::vector<std::vector<double> >*
    vehicleRoutingParams();

    /// 経路探索で選好するネットワークランクの選択確率を返す
    const std::map<unsigned int, double, std::greater<int> >*
    vehicleRoutingPrefRank();

    /// 無効なODペアを読み込む
    void readInvalidODPairCache();

private:
    /// ODグループの情報を表示する
    void _printODGroup() const;

    /// 経路探索の効用関数に関するパラメータセットを表示する
    void _printVehicleRoutingParams() const;

    /// 経路探索で選好するネットワークランクの選択確率を表示する
    void _printVehicleRoutingPrefRank() const;

    /// 無効なODペアでないかを返す
    bool _isValidODPair(ODNode* start, ODNode* goal);

private:
    /// シミュレータオブジェクト
    Simulator* _sim;

    /// 地図オブジェクト
    RoadMap* _roadMap;

    /// 車両発生イベント管理キュー
    /**
     * mapのインデックスは車両発生時刻，値は該当するGeneratingTableCell
     */
    std::multimap<unsigned long, const GeneratingTableCell*>
    _generateVehicleQueue;

    /// 優先車両発生イベント管理キュー
    /**
     * mapのインデックスは車両発生時刻，値は該当するGeneratingTableCell
     *
     * 発生時刻が厳密に指定されたバスなどはpush_backでなく
     * push_frontする必要があるため別に管理する
     */
    std::multimap<unsigned long, const GeneratingTableCell*>
    _generateVehiclePriorQueue;

    /// グループ化された始点OD
    std::map<std::string, ODNodeGroup*> _odStartGroup;

    /// グループ化された終点OD
    std::map<std::string, ODNodeGroup*> _odGoalGroup;

    /// グループ化されたODペア
    std::map<std::string, ODNodeGroup*> _odPairGroup;

    /// 車両発生を待つODノード
    std::vector<ODNode*> _waitingODNodes;

    /// 経路探索に失敗したODペア
    std::map<ODNode*, std::vector<ODNode*> > _invalidODPair;

    /** @name 車両発生定義テーブル */
    //@{
    GeneratingTable _table;        //!< 始点終点の双方を設定
    GeneratingTable _defaultTable; //!< 始点のみ設定
    GeneratingTable _fixedTable;   //!< 時刻，始点，終点を指定
    GeneratingTable _groupedTable; //!< グループ化されたODを設定
    GeneratingTable _randomTable;  //!< 無設定（デフォルト交通量）
    //@}

    /// Originノードの集合
    std::vector<ODNode*> _startNodes;

    /// レベル分けされたOriginノード
    std::vector<ODNode*> _startLevel[3];

    /// Destination-喉の集合
    std::vector<ODNode*> _goalNodes;

    /// レベル分けされたDestinationノード
    std::vector<ODNode*> _goalLevel[3];

    /// デフォルトのレベル別交通量
    int _defaultTrafficVolume[3];

    /// 経路探索の効用関数に関するパラメータセット
    std::vector<std::vector<double> > _vehicleRoutingParams;

    /// 経路探索で選好するネットワークランクの選択確率
    std::map<unsigned int, double, std::greater<int> >
    _vehicleRoutingPrefRank;

    /// 乱数生成器
    RandomGenerator* _rnd;

    //==================================================================
    // parallel

public:

    /// 車両を再構築する
    void rebuildVehicles(std::vector<double>* data);

    /// Parallel オブジェクトをセットする
    void setParallel(Parallel* parallel);

protected:

    // parallel オブジェクト
    Parallel* _parallel;
};

#endif //__GENERATE_VEHICLE_CONTROLLER_H__
