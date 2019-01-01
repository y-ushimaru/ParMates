/* **************************************************
 * Copyright (C) 2014 ADVENTURE Project
 * All Rights Reserved
 **************************************************** */
#include "GenerateVehicleController.h"
#include "GenerateVehicleIO.h"
#include "RoadMap.h"
#include "RoutingManager.h"
#include "Intersection.h"
#include "ODNode.h"
#include "ODNodeGroup.h"
#include "Section.h"
#include "Lane.h"
#include "Vehicle.h"
#include "VirtualOccupant.h"
#include "Random.h"
#include "TimeManager.h"
#include "ObjManager.h"
#include "GVManager.h"
#include "VehicleFamilyManager.h"
#include "VehicleFamilyIO.h"
#include "VehicleFamily.h"
#include "AmuConverter.h"
#include "AmuStringOperator.h"
#include "Conf.h"
#include "ParallelConf.h"
#include "Parallel.h"
#include <iostream>
#include <fstream>
#include <cassert>
#include <climits>
#include <cmath>
#ifdef INCLUDE_TRAMS
#include "tram/VehicleTram.h"
#endif //INCLUDE_TRAMS

using namespace std;

// 車両を確率的でなく等間隔で発生させるは以下のマクロを定義する
// #define GENERATE_VEHICLE_EQUAL_INTERVAL

//======================================================================
GenerateVehicleController::GenerateVehicleController()
{
    _generateVehicleQueue.clear();
    _generateVehiclePriorQueue.clear();
    _vehicleRoutingParams.clear();
    _vehicleRoutingPrefRank.clear();
    _odStartGroup.clear();
    _odGoalGroup.clear();
    _odPairGroup.clear();
    _startNodes.clear();
    for (unsigned int i=0; i<3; i++)
    {
        _startLevel[i].clear();
    }
    _goalNodes.clear();
    for (unsigned int i=0; i<3; i++)
    {
        _goalLevel[i].clear();
    }
    _rnd = Random::randomGenerator();
}

//======================================================================
GenerateVehicleController::~GenerateVehicleController()
{
    map<string, ODNodeGroup*>::iterator ito;
    for (ito = _odStartGroup.begin(); ito != _odStartGroup.end(); ito++)
    {
        delete (*ito).second;
    }
    _odStartGroup.clear();

    for (ito = _odGoalGroup.begin(); ito != _odGoalGroup.end(); ito++)
    {
        delete (*ito).second;
    }
    _odGoalGroup.clear();

    for (ito = _odPairGroup.begin(); ito != _odPairGroup.end(); ito++)
    {
        delete (*ito).second;
    }
    _odPairGroup.clear();

    Random::releaseRandomGenerator(_rnd);
}

//======================================================================
GenerateVehicleController& GenerateVehicleController::instance()
{
    static GenerateVehicleController instance;
    return instance;
}

//======================================================================
bool GenerateVehicleController::getReadyGeneration()
{
    //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    // 車種情報の設定

    // デフォルトで20(普通車), 50(大型車)を作成する
    double length, width, height;
    double weight = 0.0;
    double accel, decel;
    double r, g, b;

    length = GVManager::getNumeric("VEHICLE_LENGTH_PASSENGER");
    width  = GVManager::getNumeric("VEHICLE_WIDTH_PASSENGER");
    height = GVManager::getNumeric("VEHICLE_HEIGHT_PASSENGER");
    accel  = GVManager::getNumeric("MAX_ACCELERATION_PASSENGER");
    decel  = GVManager::getNumeric("MAX_DECELERATION_PASSENGER");
    r = 1.0;
    g = 0.0;
    b = 0.0;
    VFAttribute passenger(VehicleFamily::passenger(),
                          length, width, height, 1, weight,
                          accel, decel, r, g, b);
    VehicleFamilyManager::addVehicleFamily(passenger);

    length = GVManager::getNumeric("VEHICLE_LENGTH_TRUCK");
    width  = GVManager::getNumeric("VEHICLE_WIDTH_TRUCK");
    height = GVManager::getNumeric("VEHICLE_HEIGHT_TRUCK");
    accel  = GVManager::getNumeric("MAX_ACCELERATION_TRUCK");
    decel  = GVManager::getNumeric("MAX_DECELERATION_TRUCK");
    r = 0.3;
    g = 0.7;
    b = 1.0;
    VFAttribute truck(VehicleFamily::truck(),
                      length, width, height, 1, weight,
                      accel, decel, r, g, b);
    VehicleFamilyManager::addVehicleFamily(truck);

    // 車両情報をファイルから読み込む
    VehicleFamilyIO::getReadyVehicleFamily();
    if (GVManager::getFlag("FLAG_VERBOSE"))
    {
        VehicleFamilyIO::print();
    }

    //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    // 車両発生情報をファイルから読み込む
    GenerateVehicleIO& genIO = GenerateVehicleIO::instance();
    genIO.setSimulator(_sim);

    //------------------------------------------------------------------
    // 標準的な交通量(単路ごとの台/時) : 基本交通容量の10%
    /*
     * "レーンごと"ではない
     */
    _defaultTrafficVolume[0]
        = static_cast<int>(
            GVManager::getNumeric("DEFAULT_TRAFFIC_VOLUME_WIDE"));
    _defaultTrafficVolume[1]
        = static_cast<int>(
            GVManager::getNumeric("DEFAULT_TRAFFIC_VOLUME_NORMAL"));
    _defaultTrafficVolume[2]
        = static_cast<int>(
            GVManager::getNumeric("DEFAULT_TRAFFIC_VOLUME_NARROW"));

    //------------------------------------------------------------------
    // ランダム選択から除外するODノードの読み込み
    vector<string> excludedStarts;
    vector<string> excludedGoals;
    excludedStarts.clear();
    excludedGoals.clear();
    genIO.readExcludedODNodes(&excludedStarts, &excludedGoals);

    // ODノードののレベル分け
    _classifyODNodes(&excludedStarts, &excludedGoals);

    //------------------------------------------------------------------
    // ODグループ定義ファイルの読み込み
    genIO.readODGroupFile(this);
    if (GVManager::getFlag("FLAG_VERBOSE"))
    {
        _printODGroup();
    }

    //------------------------------------------------------------------
    // 車両発生定義ファイルの読み込み
    if (GVManager::getFlag("FLAG_INPUT_VEHICLE"))
    {
        genIO.setUpGeneratingTables(this);
    }

    // 車両発生が定義されていない交差点/時間帯のテーブルを作成する
    if (GVManager::getFlag("FLAG_GEN_RAND_VEHICLE"))
    {
        genIO.setUpRandomGeneratingTable(this);
    }

    //------------------------------------------------------------------
    // 経路探索用のパラメータの設定
    genIO.readVehicleRoutingParams(this);
    genIO.readVehicleRoutingPrefRank(this);

    if (GVManager::getFlag("FLAG_VERBOSE"))
    {
        _printVehicleRoutingParams();
        _printVehicleRoutingPrefRank();
    }
    return true;
}

//======================================================================
void GenerateVehicleController::generateVehicle()
{
    // 現在時刻に適用されるGeneratingTableCellを有効化する
    /*
     * 未activeなGeneratingTableCellをactivateするため，
     * 各GeneratingTableCellがactivateの対象になるのは1回のみ
     */
    _activatePresentGTCells();

    // 車両を確実に発生させる (バス等)
    _generateVehiclesFromPriorQueue();

    // 車両を確率的に発生させる
    _generateVehiclesFromQueue();

    /*
     * pushVehicleToRealが実行された順にIDが付与されるため，
     * 以下はシングルスレッドで実行しなければならない
     */

    vector<ODNode*> _tmpWaitingODNodes;
    for (unsigned int i=0; i<_waitingODNodes.size(); i++)
    {
        // 再分割後の不要な発生をトラップする
        if(_parallel->myId() == _waitingODNodes[i]->processId())
        {
          _tmpWaitingODNodes.push_back(_waitingODNodes[i]);
        }
        else
        {
          _waitingODNodes[i]->clearWaitingVehicles();
        }
    }

    vector<ODNode*> tmpODNodes;
    for (unsigned int i=0; i<_tmpWaitingODNodes.size(); i++)
    {
        assert(_parallel->myId() == _tmpWaitingODNodes[i]->processId());

        // 車両発生の確認
        // cout<<"myId:"<<_parallel->myId()<<" generate at "<<_waitingODNodes[i]->id()<<endl;

        _tmpWaitingODNodes[i]->pushVehicleToReal(_roadMap);
        if (_tmpWaitingODNodes[i]->hasWaitingVehicles())
        {
            // まだ登場しきれていないwaitingVehiclesが存在する場合
            // 次のタイムステップでpushVeicleを試みる
            tmpODNodes.push_back(_tmpWaitingODNodes[i]);
        }
        else
        {
            // 保持する全てのwaitingVehicleを登場させた場合，
            // 次にaddWaitingVehicleされるまで何もしない
            _tmpWaitingODNodes[i]->setWaitingToPushVehicle(false);
        }
    }
    _waitingODNodes.swap(tmpODNodes);
}

//======================================================================
void GenerateVehicleController::generateVehicleManual
(const std::string& startId, const std::string& goalId,
 std::vector<std::string>* gateIds, VehicleType vehicleType,
 std::vector<double> params, unsigned int prefRank)
{
    // 出発地の取得
    ODNode* start
        = dynamic_cast<ODNode*>(_roadMap->intersection(startId));
    if (!start)
    {
        cout << "start:" << startId << " is not a OD node." << endl;
        return;
    }

    // 目的地の取得
    ODNode* goal;
    if (goalId == "******")
    {
        goal = _decideGoalRandomly(start);
        if (!goal)
        {
            cout << "goal:" << goalId << "cannot be decided." << endl;
            return;
        }
    }
    else
    {
        goal = dynamic_cast<ODNode*>(_roadMap->intersection(goalId));
        if (!goal)
        {
            cout << "goal:" << goalId << " is not a OD node." << endl;
            return;
        }
    }

    // 経由地の取得
    /*
     * 関数が呼びだされた時点でgateIdsの先頭はstart，末尾はgoal
     */
    vector<string> myGateIds;
    myGateIds.clear();
    for (unsigned int j=0; j<gateIds->size()-1; j++)
    {
        myGateIds.push_back((*gateIds)[j]);
    }
    myGateIds.push_back(goal->id());

    Vehicle* newVehicle
        = _createVehicle(start, goal,
                         start, start->next(0),
                         &myGateIds, vehicleType);

    newVehicle->setRoutingParams(params);
    newVehicle->setPreferredNetworkRank(prefRank);

    // startの_waitingVehiclesの先頭に加える
    start->addWaitingVehicleFront(newVehicle);

#ifdef _OPENMP
#pragma omp critical (isWaitingToPushVehicle)
    {
#endif //_OPENMP
        if (!(start->isWaitingToPushVehicle()))
        {
            _waitingODNodes.push_back(start);
            start->setWaitingToPushVehicle(true);
        }
#ifdef _OPENMP
    }
#endif
    newVehicle->route()->print(cout);
}

//======================================================================
void GenerateVehicleController::_activatePresentGTCells()
{
    // 現在時刻で有効なセルを抽出する
    vector<const GeneratingTableCell*> activeGTCells;
    _table.extractActiveGTCells(&activeGTCells);
    _defaultTable.extractActiveGTCells(&activeGTCells);
    _groupedTable.extractActiveGTCells(&activeGTCells);
    _randomTable.extractActiveGTCells(&activeGTCells);

    // 抽出したセル全てについて車両発生時刻を求める
    for (unsigned int i=0; i<activeGTCells.size(); i++)
    {
        _addNextGenerateTime(activeGTCells[i]->begin(),
                             activeGTCells[i]);
    }

    // 現在時刻で有効なセルを抽出する
    vector<const GeneratingTableCell*> activeFixedGTCells;
    if ((GVManager::getFlag("DEBUG_FLAG_GEN_FIXED_VEHICLE_ALL_AT_ONCE")))
    {
        _fixedTable.extractActiveGTCellsAllAtOnce(&activeFixedGTCells);
    }
    else
    {
        _fixedTable.extractActiveGTCells(&activeFixedGTCells);
    }

    // 抽出したセル全てについて車両発生時刻を求める
    for (unsigned int i=0; i<activeFixedGTCells.size(); i++)
    {
        _addFixedGenerateTime(activeFixedGTCells[i]);
    }
}

//======================================================================
bool GenerateVehicleController::_addNextGenerateTime
(ulint startTime, const GeneratingTableCell* cell)
{
    // 交通量0の場合は何もしない
    if (cell->volume()<1.0e-3)
    {
        return false;
    }

    // 平均時間間隔[msec]
    /*
     * 交通量から算出される
     */
    double meanInterval = (60.0*60.0*1000.0)/(cell->volume());

    // 時間間隔
    ulint interval;

#ifdef GENERATE_VEHICLE_EQUAL_INTERVAL //...............................

    if (cell->generatedVolume()==0)
    {
        // このcellによって初めて車両が生成される場合
        /*
         * 交通量が1[veh./hour]の場合は30minで最初の1台発生させる
         */
        interval = ceil(meanInterval/2.0);
    }
    else
    {
        // このcellから既に車両が生成されている場合
        /*
         * 交通量が1[veh./hour]の場合は60min間隔で発生させる
         */
        interval = ceil(meanInterval);
        interval += TimeManager::unit()-interval%TimeManager::unit();
    }

#else // GENERATE_VEHICLE_EQUAL_INTERVAL not defined ...................

    interval = ceil(-meanInterval*log(1-Random::uniform(_rnd)));
    interval += TimeManager::unit()-interval%TimeManager::unit();

#endif // GENERATE_VEHICLE_EQUAL_INTERVAL ..............................

    ulint nextTime = startTime + interval;

    if (nextTime <= cell->end())
    {

#ifdef _OPENMP
#pragma omp critical (addNextGenerateTime)
        {
#endif //_OPENMP

            _generateVehicleQueue.insert(
                pair<unsigned long, const GeneratingTableCell*>
                (nextTime, cell));
            const_cast<GeneratingTableCell*>(cell)
                ->incrementGeneratedVolume();

#ifdef _OPENMP
        }
#endif //_OPENMP

        return true;
    }
    else
    {
        return false;
    }
}

//======================================================================
bool GenerateVehicleController::_addFixedGenerateTime
(const GeneratingTableCell* cell)
{

#ifdef _OPENMP
#pragma omp critical (addFixedGenerateTime)
    {
#endif //_OPENMP

        _generateVehiclePriorQueue.insert(
            pair<unsigned long, const GeneratingTableCell*>
            (cell->begin(), cell));

#ifdef _OPENMP
    }
#endif //_OPENMP
    return true;
}

//======================================================================
void GenerateVehicleController::_generateVehiclesFromQueue()
{
    /// @attention 本関数はシングルスレッドで実行すること
    /*
     * 車両生成順が固定できないと乱数の再現性が損なわれるため
     */

    // 現在のタイムステップで車両を発生させるGeneratingTableCell
    vector<const GeneratingTableCell*> validCells;
    _extractValidCellFromQueue
        (&_generateVehicleQueue, &validCells, false);
    if (validCells.empty())
    {
        return;
    }
    for (unsigned int i=0; i<validCells.size(); i++)
    {
       	// 自領域からの発生でない場合は処理をパス
      	if(_getStartNode(validCells[i])->processId()!=_parallel->myId())
        {
          // 次の発生時刻を予約
          _addNextGenerateTime(TimeManager::time(), validCells[i]);
          continue;
        }

        //--------------------------------------------------------------
        // 車両の生成
        Vehicle* newVehicle
            = _generateVehicleFromGTCell(validCells[i]);
        if (!newVehicle)
        {
            cerr << "ERROR: cannot generate vehicle at "
                 << validCells[i]->origin() << endl;
            continue;
        }

        // 経路探索パラメータの設定
        //  _setRoutingParamsRandomly(newVehicle);
         _setRoutingParamsManually(newVehicle);

        // 経路探索で選好するネットワークランクの設定
        //  _setRoutingPrefRankRandomly(newVehicle);
         _setRoutingPrefRankManually(newVehicle);

        //--------------------------------------------------------------
        // 出発地における処理
        ODNode* start = dynamic_cast<ODNode*>
            (const_cast<Intersection*>(newVehicle->route()->start()));
        assert(start);

        if (newVehicle)
        {
            // waitingVehiclesに追加
            start->addWaitingVehicle(newVehicle);
        }
        if (!(start->isWaitingToPushVehicle()))
        {
            _waitingODNodes.push_back(start);
            start->setWaitingToPushVehicle(true);
        }

        //--------------------------------------------------------------
        // 次の発生時刻を予約
        _addNextGenerateTime(TimeManager::time(), validCells[i]);
    }
}

//======================================================================
void GenerateVehicleController::_generateVehiclesFromPriorQueue()
{
    /// @attention 本関数はシングルスレッドで実行すること
    /*
     * 車両生成順が固定できないと乱数の再現性が損なわれるため
     */

    // 現在のタイムステップで車両を発生させるGeneratingTableCell
    vector<const GeneratingTableCell*> validCells;
    _extractValidCellFromQueue
        (&_generateVehiclePriorQueue, &validCells,
         GVManager::getFlag
         ("DEBUG_FLAG_GEN_FIXED_VEHICLE_ALL_AT_ONCE"));
    if (validCells.empty())
    {
        return;
    }
    for (unsigned int i=0; i<validCells.size(); i++)
    {
        //--------------------------------------------------------------
        // 車両の生成
        Vehicle* newVehicle
            = _generateVehicleFromGTCell(validCells[i]);
        if (!newVehicle)
        {
            cerr << "ERROR: cannot generate prior vehicle at "
                 << validCells[i]->origin() << endl;
            continue;
        }
        // 経路探索パラメータの設定
        _setRoutingParamsRandomly(newVehicle);
        // 経路探索で選好するネットワークランクの設定
        _setRoutingPrefRankRandomly(newVehicle);

        //--------------------------------------------------------------
        // 出発地における処理
        ODNode* start = dynamic_cast<ODNode*>
            (const_cast<Intersection*>(newVehicle->route()->start()));
        assert(start);

        // waitingVehiclesに追加
        /*
         * 出発を優先するのでwaitingVehicleの先頭に追加
         */
        start->addWaitingVehicleFront(newVehicle);

        if (!(start->isWaitingToPushVehicle()))
        {
            _waitingODNodes.push_back(start);
            start->setWaitingToPushVehicle(true);
        }

        // 1台のみ発生させるので，次の発生時刻は必要ない
    }
}

//======================================================================
void GenerateVehicleController::_extractValidCellFromQueue
(multimap<unsigned long, const GeneratingTableCell*>* queue,
 vector<const GeneratingTableCell*>* result_validCells,
 bool generateVehicleAllAtOnceIsOn)
{
    while(true)
    {
        if (queue->empty())
        {
            break;
        }
        if ((*(queue->begin())).first > TimeManager::time())
        {
            // これ以降は次ステップ以降に処理することになる
            if (!generateVehicleAllAtOnceIsOn)
            {
                break;
            }
        }
        result_validCells->push_back((*(queue->begin())).second);
        queue->erase(queue->begin());
    }

    return;
}

//======================================================================
Vehicle* GenerateVehicleController::_generateVehicleFromGTCell
(const GeneratingTableCell* cell)
{
    ODNode* start = NULL;
    ODNode* goal  = NULL;

    // 出発地と目的地の取得
    if (cell->isPaired())
    {
        _getStartGoalNodes(cell, &start, &goal);
        assert(start && goal);
    }
    else
    {
        /*
         * 始点あるいは終点がODグループの場合か，
         * 始点・終点のいずれも単独の場合
         */
        start = _getStartNode(cell);
        assert(start);
        goal  = _getGoalNode(cell, start);
        assert(goal);
    }

    // 経由地の取得
    vector<string> gateIds;
    _getGateIds(cell, start, goal, &gateIds);

    // 車種の取得
    VehicleType type = cell->vehicleType();

    // 車両の生成
    return _createVehicle(start, goal,
                          start, start->next(0),
                          &gateIds, type);
}

//======================================================================
Vehicle* GenerateVehicleController::_createVehicle
(ODNode* start, ODNode* goal,
 Intersection* rear, Intersection* front,
 const vector<string>* gateIds,
 VehicleType vehicleType)
{
    assert(start!=NULL && goal!=NULL);

    // 車両の生成
    Vehicle* tmpVehicle;
    if(VehicleFamily::isTram(vehicleType))
    {
#ifdef INCLUDE_TRAMS
        tmpVehicle = ObjManager::createVehicleTram();
#else //INCLUDE_TRAMS not defined
        cerr << "WARNING: cannot create VehicleTram "
             << "without compiling INCLUDE_TRAMS flag on." << endl;
        return NULL;
#endif
    }
    else
    {
        tmpVehicle = ObjManager::createVehicle();
    }
    tmpVehicle->setSimulator(_sim);

    // 車両属性の設定
    tmpVehicle->setType(vehicleType);
    _setVehicleBodyAttributes(tmpVehicle);

    // 経由地の設定
    vector<const Intersection*> gates;
    _convertGateIds2Gates(gateIds, &gates, start, goal);
    tmpVehicle->route()->setGates(&gates);

    return tmpVehicle;
}

//======================================================================
void GenerateVehicleController::_setVehicleBodyAttributes
(Vehicle* vehicle)
{
    double length, width, height;
    int articulation;
    double accel, decel;
    double r, g, b;

    // 車種に対応付けられた属性を取得する
    VFAttribute* vfa
        = VehicleFamilyManager::vehicleFamilyAttribute(vehicle->type());
    if (!vfa)
    {
        if (VehicleFamily::isTruck(vehicle->type()))
        {
            vfa = VehicleFamilyManager::vehicleFamilyAttribute
                (VehicleFamily::truck());
        }
        else
        {
            vfa = VehicleFamilyManager::vehicleFamilyAttribute
                (VehicleFamily::passenger());
        }
    }
    vfa->getSize(&length, &width, &height);
    articulation = vfa->bodyArticulation();
    vfa->getPerformance(&accel, &decel);
    vfa->getBodyColor(&r, &g, &b);

    vehicle->setBodySize(length, width, height);
    vehicle->setBodyArticulation(articulation);
    vehicle->setPerformance(accel, decel);
    vehicle->setBodyColor(r, g, b);
}

//======================================================================
void GenerateVehicleController::_setRoutingParamsRandomly
(Vehicle* vehicle)
{
    vehicle->setRoutingParams
        (_vehicleRoutingParams
         [Random::uniform(_rnd, 0, _vehicleRoutingParams.size())]);
}

//======================================================================
void GenerateVehicleController::_setRoutingParamsManually
(Vehicle* vehicle)
{

    vector<double> params;
    for (unsigned int i=0;i<VEHICLE_ROUTING_PARAMETER_SIZE;i++)
    {
        if(i == 0)
          params.push_back(1);
        else
          params.push_back(0);
    }

    vehicle->setRoutingParams(params);
}

//======================================================================
void GenerateVehicleController::_setRoutingPrefRankRandomly
(Vehicle* vehicle)
{
    unsigned int rank = 1;
    double r = Random::uniform(_rnd);

    std::map<unsigned int, double>::iterator itr;
    for (itr = _vehicleRoutingPrefRank.begin();
         itr !=  _vehicleRoutingPrefRank.end();
         itr++)
    {
        if (r<=(*itr).second)
        {
            rank = (*itr).first;
            break;
        }
        else
        {
            r -= (*itr).second;
        }
    }
    vehicle->setPreferredNetworkRank(rank);
}

//======================================================================
void GenerateVehicleController::_setRoutingPrefRankManually
(Vehicle* vehicle)
{
    unsigned int rank = 1;
    vehicle->setPreferredNetworkRank(rank);
}

//======================================================================
void GenerateVehicleController::_getStartGoalNodes
(const GeneratingTableCell* cell,
 ODNode** result_start, ODNode** result_goal)
{
    string startId = cell->origin();
    string fmtStartId
        = AmuConverter::formatId
        (startId, NUM_FIGURE_FOR_OD_GROUP);
    ODNodeGroup* group = _odPairGroup[fmtStartId];
    if (!group)
    {
        cerr << "ERROR: OD pair -" << startId
             << " not found." << endl;
    }
    else
    {
        group->getODPair(result_start, result_goal);
    }
}

//======================================================================
ODNode* GenerateVehicleController::_getStartNode
(const GeneratingTableCell* cell)
{
    ODNode* start = NULL;

    string startId = cell->origin();
    if (cell->isPaired())
    {
        // ここには来ないはず
        cerr << "ERROR: OD pair must be processed in other function."
             << endl;
        assert(0);
    }
    else if (cell->isOriginGrouped())
    {
        // 始点ODグループ
        string fmtStartId
            = AmuConverter::formatId
            (startId, NUM_FIGURE_FOR_OD_GROUP);
        ODNodeGroup* group = _odStartGroup[fmtStartId];
        if (!group)
        {
            cerr << "ERROR: OriginGroup -" << startId
                 << " not found." << endl;
        }
        else
        {
            group->getODSolo(&start);
        }
    }
    else
    {
        // 単独の始点
        start = dynamic_cast<ODNode*>(_roadMap->intersection(startId));
    }

    return start;
}

//======================================================================
ODNode* GenerateVehicleController::_getGoalNode
(const GeneratingTableCell* cell, ODNode* start)
{
    ODNode* goal = NULL;

    string goalId  = cell->destination();
    if (goalId == "******")
    {
        goal = _decideGoalRandomly(start);

        while(true)
        {
          goal = _decideGoalRandomly(start);

          // 探索不可の経路の場合処理をやり直す
          if (_isValidODPair(start, goal))
          {
            break;
          }
          // cout << "start:" << start->id() << " goal:" << goal->id()
          // << " is invalid" << endl;
        }
        return goal;
    }

    if (cell->isPaired())
    {
        // ここには来ないはず
        cerr << "ERROR: OD pair must be processed in other function."
             << endl;
        assert(0);
    }
    else if (cell->isDestinationGrouped())
    {
        // 終点ODグループ
        string fmtGoalId
            = AmuConverter::formatId
            (goalId, NUM_FIGURE_FOR_OD_GROUP);
        ODNodeGroup* group = _odGoalGroup[fmtGoalId];
        if (!group)
        {
            cerr << "ERROR: DestinationGroup-" << goalId
                 << " not found." << endl;
        }
        else
        {
            group->getODSolo(&goal);
        }
    }
    else
    {
        // 単独の終点
        goal = dynamic_cast<ODNode*>(_roadMap->intersection(goalId));
    }

    return goal;
}

//======================================================================
ODNode* GenerateVehicleController::_decideGoalRandomly(ODNode* start)
{
    ODNode* result = NULL;

    int level[3];
    for(int i=0; i<3; i++)
    {
        level[i] = _defaultTrafficVolume[i];
        if (_goalLevel[i].size()==0)
        {
            level[i] = 0;
        }
    }

    unsigned int total = 0;
    for (int i=0; i<3; i++)
    {
        total += level[i] * _goalLevel[i].size();
    }
    unsigned int goalLevel = 0;

    if (total==0)
    {
        // totalが0になるのは想定していないが...
        cerr << "sum of DEFAULT_TRAFFIC_VOLUME must not be 0" << endl;
        return NULL;
    }
    // 目的地のレベルをランダムに選択
    unsigned int r1 = Random::uniform(_rnd, RAND_MAX) % total;
    for (unsigned int i=0; i<3; i++)
    {
        if (r1<level[i]*_goalLevel[i].size())
        {
            goalLevel = i;
        }
        else
        {
            r1 -= level[i] * _goalLevel[i].size();
            continue;
        }
    }

    // 指定されたレベルから目的地をランダムに選択
    unsigned int r2 = Random::uniform(_rnd,
                                      _goalLevel[goalLevel].size());
    while(true)
    {
        result = _goalLevel[goalLevel][r2];

#ifdef UNIQUE_NETWORK
        if (start != result)
        {
            // start以外のODノードであればwhileループから抜ける
            break;
        }
#else  //UNIQUE_NETWORK
        if (start != result
            && start->isNetworked(start, result))
        {
            /*
             * start以外であり，startから到達可能であれば
             * whileループから抜ける
             */
            break;
        }
#endif //UNIQUE_NETWORK

        // startと同じODノードを選択してしまった場合は再度選択
        if (_goalLevel[goalLevel].size()==1)
        {
            total = 0;
            for (unsigned int i=0; i<3; i++)
            {
                if (i != goalLevel)
                {
                    total += level[i] * _goalLevel[i].size();
                }
            }
            r1 = Random::uniform(_rnd, RAND_MAX) % total;
            for (unsigned int i=0; i<3; i++)
            {
                if (r1<level[i]*_goalLevel[i].size())
                {
                    goalLevel = i;
                }
                else
                {
                    r1 -= level[i] * _goalLevel[i].size();
                }
            }
        }
        // 区間[1, _goalLevel[goalLevel].size()-1]の乱数をrに加えて更新
        r2 = (r2 + Random::uniform(_rnd,
                                   1, _goalLevel[goalLevel].size()))
            % _goalLevel[goalLevel].size();
    }
    assert(result);
    return result;
}

//======================================================================
void GenerateVehicleController::_getGateIds
(const GeneratingTableCell* cell,
 ODNode* start, ODNode* goal, vector<string>* result_gateIds)
{
    result_gateIds->clear();
    const vector<string>* gates = cell->gates();
    // gatesの最初の要素 = 出発地
    /*
     * GeneratingTableCellのIDはグループIDである可能性があるため，
     * 決定したstartノードのIDで書き換え
     */
    result_gateIds->push_back(start->id());
    for (unsigned int j=1; j<gates->size()-1; j++)
    {
        result_gateIds->push_back((*gates)[j]);
    }
    // gatesの最後の要素 = 目的地
    /*
     * GeneratingTableCellのIDはグループIDや"******"である
     * 可能性があるため，決定したgoalノードのIDで書き換え
     */
    result_gateIds->push_back(goal->id());
}

//======================================================================
void GenerateVehicleController::_convertGateIds2Gates
(const vector<string>* gateIds,
 vector<const Intersection*>* result_gates,
 ODNode* start, ODNode* goal)
{
    result_gates->clear();
    for (unsigned int i=0; i<gateIds->size(); i++)
    {
        Intersection* gate
            = _roadMap -> intersection((*gateIds)[i]);
        if (!gate)
        {
            cerr << "ERROR: gate id ("
                 << (*gateIds)[i] << ") is not found." << endl;
        }
        result_gates->push_back(gate);
        if (i==0)
        {
            assert(gate==start);
        }
        else if (i==gateIds->size()-1)
        {
            assert(gate==goal);
        }
    }
}

//======================================================================
void GenerateVehicleController::_classifyODNodes
(vector<string>* excludedStarts, vector<string>* excludedGoals)
{
    // 除外ファイルで指定されていないODノードをレベル分けして格納する
    vector<ODNode*> odNodes = _roadMap->odNodes();
    vector<string>::iterator its;
    for (unsigned int i=0; i<odNodes.size(); i++)
    {
        if (0<=odNodeStartLevel(odNodes[i])
            && odNodeStartLevel(odNodes[i])<3)
        {
            // excludedStartsに登録されていなければ分類する
            its = find(excludedStarts->begin(),
                       excludedStarts->end(),
                       odNodes[i]->id());
            if (its == excludedStarts->end())
            {
                _startLevel[odNodeStartLevel(odNodes[i])]
                    .push_back(odNodes[i]);
                _startNodes.push_back(odNodes[i]);
            }
        }
        if (0<=odNodeGoalLevel(odNodes[i])
            && odNodeGoalLevel(odNodes[i])<3)
        {
            // excludedGoalsに登録されていなければ分類する
            its = find(excludedGoals->begin(),
                       excludedGoals->end(),
                       odNodes[i]->id());
            if (its == excludedGoals->end())
            {
                _goalLevel[odNodeGoalLevel(odNodes[i])]
                    .push_back(odNodes[i]);
                _goalNodes.push_back(odNodes[i]);
            }
        }
    }
}

//======================================================================
int GenerateVehicleController::odNodeStartLevel(ODNode* node) const
{
    int result = -1;

    // ODノードから見たnumOut
    if (node->border(0)->numOut()>=3)
    {
        result = 0;
    }
    else if (node->border(0)->numOut()==2)
    {
        result = 1;
    }
    else if (node->border(0)->numOut()==1)
    {
        result = 2;
    }
    return result;
}

//======================================================================
int GenerateVehicleController::odNodeGoalLevel(ODNode* node) const
{
    // ODノードから見たnumIn
    int result = -1;
    if (node->border(0)->numIn()>=3)
    {
        result = 0;
    }
    else if (node->border(0)->numIn()==2)
    {
        result = 1;
    }
    else if (node->border(0)->numIn()==1)
    {
        result = 2;
    }
    return result;
}

//======================================================================
int GenerateVehicleController::defaultTrafficVolume(int level) const
{
    assert(0<=level && level<3);
    return _defaultTrafficVolume[level];
}

//======================================================================
void GenerateVehicleController::addODGroup(const string& type,
                                           ODNodeGroup* group)
{
    if (type=="start")
    {
        _odStartGroup.insert(make_pair(group->id(), group));
    }
    else if (type=="goal")
    {
        _odGoalGroup.insert(make_pair(group->id(), group));
    }
    else if (type=="pair")
    {
        _odPairGroup.insert(make_pair(group->id(), group));
    }
    else
    {
        cerr << "invalid od group type: " << type << endl;
    }
    return;
}

//======================================================================
const vector<vector<double> >*
GenerateVehicleController::vehicleRoutingParams()
{
    return &_vehicleRoutingParams;
}

//======================================================================
const std::map<unsigned int, double, std::greater<int> >*
GenerateVehicleController::vehicleRoutingPrefRank()
{
    return &_vehicleRoutingPrefRank;
}

//======================================================================
void GenerateVehicleController::_printODGroup() const
{
    if (_odStartGroup.size()>0
        || _odGoalGroup.size()>0
        || _odPairGroup.size()>0)
    {
        cout << endl;
        cout << "*** OD Group Information ***" << endl;
    }

    map<string, ODNodeGroup*>::const_iterator ito;
    if (_odStartGroup.size()>0)
    {
        cout << "Start OD Group:" << endl;
        for (ito=_odStartGroup.begin(); ito!=_odStartGroup.end(); ito++)
        {
            (*ito).second->print();
        }
    }
    if (_odGoalGroup.size()>0)
    {
        cout << "Goal OD Group:" << endl;
        for (ito=_odGoalGroup.begin(); ito!=_odGoalGroup.end(); ito++)
        {
            (*ito).second->print();
        }
    }
    if (_odPairGroup.size()>0)
    {
        cout << "Pair OD Group:" << endl;
        for (ito=_odPairGroup.begin(); ito!=_odPairGroup.end(); ito++)
        {
            (*ito).second->print();
        }
    }
}

//======================================================================
void GenerateVehicleController::_printVehicleRoutingParams() const
{
    cout << endl << "*** Vehicle Routing Parameters ***" << endl;
    cout << "NumParams: " << _vehicleRoutingParams.size() << ", "
         << _vehicleRoutingParams[0].size() << endl;
    for (unsigned int i=0; i<_vehicleRoutingParams.size(); i++)
    {
        cout << i << ": ";
        for (unsigned int j=0; j<_vehicleRoutingParams[i].size(); j++)
        {
            cout << _vehicleRoutingParams[i][j] << ",";
        }
        cout << endl;
    }
}

//======================================================================
void GenerateVehicleController::_printVehicleRoutingPrefRank() const
{
    cout << endl << "*** Vehicle Routing Preferred Network Rank ***"
         << endl;
    map<unsigned int, double, greater<double> >::const_iterator where;
    for (where = _vehicleRoutingPrefRank.begin();
         where != _vehicleRoutingPrefRank.end();
         where++)
    {
        cout << "Rank" << (*where).first << ": "
             << (*where).second << endl;
    }
}

//======================================================================
bool GenerateVehicleController::_isValidODPair(ODNode* start, ODNode* goal)
{
  bool flag = true;
  std::vector<ODNode*> invalidGoals = _invalidODPair[start];

  // ベクターにgoalが含まれるか調べる
  vector<ODNode*>::iterator iti = find(invalidGoals.begin(), invalidGoals.end(), goal);

  if(iti != invalidGoals.end())
  {
    flag = false;
  }

  return flag;
}

//======================================================================
void GenerateVehicleController::readInvalidODPairCache()
{
  cout<< "read Invalid ODPair Cache." << endl;
  ifstream fin(GVManager::getString("CACHE_INVALID_OD_PAIR_FILE").c_str());

  string str;
  while(fin.good())
  {

    getline(fin,str);
    if(!str.empty())
    {
      vector<string> tokens;
      AmuStringOperator::getTokens(&tokens, str, ',');
      ODNode* start = dynamic_cast<ODNode*>(_roadMap->intersection(tokens[0]));
      ODNode* goal = dynamic_cast<ODNode*>(_roadMap->intersection(tokens[1]));

      _invalidODPair[start].push_back(goal);
    }
  }
}

//======================================================================
void GenerateVehicleController::setParallel(Parallel* parallel)
{
  _parallel = parallel;
}

//======================================================================
void GenerateVehicleController::rebuildVehicles(std::vector<double>* data)
{

  //受信データ
  vector<double> _data = *data;

  //車両ステータス
  vector<double> _status;

  for(int i = 0; i < _data.size(); i++)
  {
    //車両識別子まで_statusに詰め込む
    if(_data[i] != REAL_VEHICLE || _data[i] != VIRTUAL_VEHICLE)
    {
      _status.push_back(_data[i]);
    }

    //車両識別子に到達したら車両を構築する
    if(_data[i] == REAL_VEHICLE)
    {
      // 車両の生成
      Vehicle* tmpVehicle = ObjManager::createVehicle();

      // 車両の設定
      tmpVehicle->setSimulator(_sim);
      tmpVehicle->setStatus(_roadMap, &_status);

      // 車両属性の設定
      _setVehicleBodyAttributes(tmpVehicle);

      //シミュレーションに登場
      bool result = ObjManager::addVehicleToReal(tmpVehicle);
      assert(result);
      _status.clear();
    }

    if(_data[i] == VIRTUAL_VEHICLE)
    {
      // 仮想車両の生成
      VirtualOccupant* tmpOccupant;
      tmpOccupant = new VirtualOccupant();

      // 仮想車両の設定
      tmpOccupant->setStatus(_roadMap, &_status);

      _status.clear();
    }
  }
}
