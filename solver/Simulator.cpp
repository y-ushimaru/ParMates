/* **************************************************
 * Copyright (C) 2014 ADVENTURE Project
 * All Rights Reserved
 **************************************************** */
#include "Simulator.h"
#include "GVManager.h"
#include "GVInitializer.h"
#include "RoadMapBuilder.h"
#include "ObjManager.h"
#include "FileManager.h"
#include "LaneBundle.h"
#include "Intersection.h"
#include "ODNode.h"
#include "Section.h"
#include "Lane.h"
#include "RoadEntity.h"
#include "Vehicle.h"
#include "VehicleFamily.h"
#include "VehicleIO.h"
#include "SignalIO.h"
#include "RoutingManager.h"
#include "RouteStorage.h"
#include "RouteIO.h"
#include "Random.h"
#include "DetectorIO.h"
#include "DetectorUnit.h"
#include "GenerateVehicleCounterIO.h"
#include "ConvoyMonitorIO.h"
#include "GenerateVehicleController.h"
#include "VehicleFamilyManager.h"
#include "VehicleFamilyIO.h"
#include "ScheduleManager.h"
#include "AmuStringOperator.h"
#include "AmuConverter.h"
#include "Conf.h"
#include "Parallel.h"
#include "Partitioner.h"
#include "ParallelConf.h"
#include <iostream>
#include <algorithm>
#include <cassert>
#include <cstdlib>
#include <functional>
#include <fstream>
#include <cmath>
#include <map>
#include <unistd.h>
#ifdef _OPENMP
#include <omp.h>
#endif //_OPENMP
#ifdef INCLUDE_PEDESTRIANS
#include "pedestrian/GeneratePedestrianController.h"
#include "pedestrian/Pedestrian.h"
#endif //INCLUDE_PEDESTRIANS

using namespace std;

//======================================================================
Simulator::Simulator()
{
    _roadMap = 0;
    _checkLaneError = false;

    _genVehicleController = &GenerateVehicleController::instance();
    _genVehicleController->setSimulator(this);

    _routingManager = new RoutingManager();

    _vehicleIO = &VehicleIO::instance();
    _signalIO = &SignalIO::instance();

#ifdef INCLUDE_PEDESTRIANS
    _pedExt = new SimulatorPedExt(this);
#endif //INCLUDE_PEDESTRIANS

//parallel
_parallel = new Parallel();
_parallel->initialize();
_parallel->setSimulator(this);
}

//======================================================================
Simulator::~Simulator()
{
    // _RoutingManager削除前でなければならない
    if (GVManager::getFlag("FLAG_CACHE_ROUTING_WRITE"))
    {
        RouteIO* routeIO = &RouteIO::instance();
        routeIO->writeRouteCache(_roadMap,
                                 _routingManager);
    }

    TimeManager::printAllClockers();

    // Managerで管理するオブジェクトの開放
    TimeManager::deleteAllClockers();
    FileManager::deleteAllOFStreams();
    ObjManager::deleteAll();

    if (_roadMap)
    {
        delete _roadMap;
    }
    if (_routingManager)
    {
        delete _routingManager;
    }

#ifdef INCLUDE_PEDESTRIANS
    delete _pedExt;
#endif //INCLUDE_PEDESTRIANS

    Random::finalize();
}

//======================================================================
bool Simulator::hasInit() const
{
    if (_roadMap)
    {
        return true;
    }
    else
    {
        return false;
    }
}

//======================================================================
bool Simulator::getReadyRoadMap()
{

  if(_parallel->isMaster())
  {
    //_METIS形式に道路ネットワークを変換する
    _parallel->partitioner()->convertNetwork();

    //領域の初期分割を行う
    _parallel->partitioner()->initPartitioning();
  }

    RoadMapBuilder builder;

    // Parallelの設定
    builder.setParallel(_parallel);

    // 道路ネットワークの作成
    builder.buildRoadMap();

    // 制限速度の設定
    builder.setSpeedLimit();

    // 単路の通行規制の設定
    builder.setTrafficControlSection();

    // 単路の選択確率の設定
    builder.setRoutingProbability();

    if (GVManager::getFlag("FLAG_INPUT_SIGNAL"))
    {
        // 信号の作成
        builder.buildSignals();
    }
    else
    {
        // 全青信号の作成
        builder.buildSignalsAllBlue();
    }

    _roadMap = builder.roadMap();
    _signalIO->setRoadMap(_roadMap);

    // 経路探索用地図の作成
    _routingManager->setRoadMap(_roadMap);
    _routingManager->getReadyRoutingNetwork();

    if (GVManager::getFlag("FLAG_CACHE_ROUTING_READ"))
    {
        RouteIO* routeIO = &RouteIO::instance();
        routeIO->readRouteCache(_roadMap, _routingManager);
    }

    return _roadMap;
}

//======================================================================
bool Simulator::getReadyRoadsideUnit()
{
    assert(_roadMap);

    // 車両感知器設定ファイルの読み込み
    DetectorIO::getReadyDetectors(_roadMap);

    // 感知器データ出力ファイルの準備
    vector<DetectorUnit*>* detectorUnits = ObjManager::detectorUnits();
    DetectorIO::getReadyOutputFiles(detectorUnits);
    if (GVManager::getFlag("FLAG_VERBOSE"))
    {
        DetectorIO::print();
    }

    // 車両発生カウンタの設定ファイル読み込みと準備
    GenerateVehicleCounterIO::getReadyCounters(_roadMap);

    // 車列最後尾モニタの設定ファイル読み込みと準備
    ConvoyMonitorIO::getReadyMonitors(_roadMap);

    return true;
}

//======================================================================
bool Simulator::getReadyVehicles()
{
    assert(_roadMap);

    _genVehicleController->setRoadMap(_roadMap);
    _genVehicleController->getReadyGeneration();
    _genVehicleController->setParallel(_parallel);

    // 無効なODペアの読み込み
    // for(int i = 0; i < _parallel->commSize(); i++)
    // {
    //   if(_parallel->myId() == i)
    //     _genVehicleController->readInvalidODPairCache();
    //
    //   _parallel->barrier();
    // }

    return true;
}

//======================================================================
void Simulator::writeInitializedRoadMap()
{
    // mapInfo.txtの作成
    if (_roadMap)
    {
        _roadMap->writeMapInfo();
    }

    // コンソールへ地図情報を表示する
    if (GVManager::getFlag("FLAG_MORE_VERBOSE"))
    {
        _roadMap->dispIntersections();
    }

    // signal_count.txtの出力
    string fSignalCount;
    GVManager::getVariable("RESULT_SIGNAL_COUNT_FILE", &fSignalCount);
    ofstream ofs(fSignalCount.c_str(), ios::out);

    CITRMAPSI its;
    // 信号の総数 (信号ID=ノードIDの数とは一致しないので注意)
    int totalNumberOfSignals=0;
    for (its=_roadMap->signals()->begin();
         its!=_roadMap->signals()->end();
         its++)
    {
        totalNumberOfSignals += (*its).second->numDirections();
    }

    ofs << _roadMap->intersections()->size() << "\n" // 交差点の総数
        << totalNumberOfSignals;		     // 信号機の総数
}

//======================================================================
void Simulator::checkLane()
{
    // レーンチェック、エラー時は表示確認のため run のみ止める
    _checkLaneError = !_roadMap->checkIntersectionLane();
}

//======================================================================
bool Simulator::checkLaneError()
{
    return _checkLaneError;
}

//======================================================================
bool Simulator::run(ulint time)
{
    // レーンチェックエラー、表示確認のため run のみ止める
    if (_checkLaneError)
    {
        return false;
    }
    if (time>TimeManager::time())
    {
        //シミュレーション開始を合わせるため、ここで同期
        _parallel->barrier();

        while (time>TimeManager::time())
        {
            timeIncrement();
        }

        //結果を出力する
        _parallel->writeResult();
        _parallel->barrier();

        //MPIを終了する
        _parallel->finalize();

        return true;
    }
    else
    {
        return false;
    }
}

//======================================================================
bool Simulator::timeIncrement()
{

    // 時刻の更新
    TimeManager::increment();
    if (GVManager::getFlag("FLAG_VERBOSE"))
    // if(true)
    {
        if (TimeManager::time()%1000==0 && _parallel->isMaster())
        {
            cout << "Time: "
                 << TimeManager::time()/1000 << "[sec]" << endl;
        }
    }

#ifdef MEASURE_TIME
    TimeManager::startClock("TOTAL_RUN");
#endif //MEASURE_TIME

    //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    // エージェント列の更新
    //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
#ifdef DEBUG
    _parallel->barrier();
    cout << "myId:" << _parallel->myId() << " " << "RENEW_AGENT" << endl;
#endif //DEBUG

#ifdef MEASURE_TIME
    TimeManager::startClock("RENEW_AGENT");
#endif //MEASURE_TIME

    // レーン上のエージェントを更新
    _roadMap->renewAgentLine();

#ifdef MEASURE_TIME
    TimeManager::stopClock("RENEW_AGENT");
#endif //MEASURE_TIME
    
    //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    // モニタリング
    //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
#ifdef MEASURE_TIME
    TimeManager::startClock("MONITORING");
#endif //MEASURE_TIME

    if (GVManager::getFlag("FLAG_OUTPUT_MONITOR_D")
        || GVManager::getFlag("FLAG_OUTPUT_MONITOR_S"))
    {
        vector<DetectorUnit*>* detectorUnits
            = ObjManager::detectorUnits();
        for_each(detectorUnits->begin(),
                 detectorUnits->end(),
                 mem_fun(&DetectorUnit::monitorLanes));
        DetectorIO::writeTrafficData(detectorUnits);
    }
    if (GVManager::getFlag("FLAG_OUTPUT_CONVOY_MONITOR"))
    {
        ConvoyMonitorIO::writeConvoyData(_roadMap);
    }

#ifdef MEASURE_TIME
    TimeManager::stopClock("MONITORING");
#endif //MEASURE_TIME

    //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    // リンク旅行時間の更新
    //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
#ifdef INCLUDE_VEHICLES

#ifdef MEASURE_TIME
    TimeManager::startClock("RENEW_PASSTIME");
#endif //MEASURE_TIME

    /**
     * @note
     * この部分の処理を並列化する場合には
     * Intersectionをvectorで(も)管理する必要がある
     * あるいはOpenMPのtask構文を使用できるかも？
     */
    if (TimeManager::time()
        % (static_cast<int>
           (GVManager::getNumeric("INTERVAL_RENEW_LINK_TRAVEL_TIME"))
           *1000)
        == 0)
    {
        const RMAPI* intersections = _roadMap->intersections();
        for (CITRMAPI iti = intersections->begin();
             iti != intersections->end();
             iti++)
        {
            (*iti).second->renewPassTimeForGlobal();
            (*iti).second->routeStorage()->removeDynamicComponent();
        }

        // 経路探索用地図のコストの更新
        /*
         * 上記リンク旅行時間の更新後でなければならない
         */
        _routingManager->renewCosts();
     }

#ifdef MEASURE_TIME
    TimeManager::stopClock("RENEW_PASSTIME");
#endif //MEASURE_TIME

    //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    // 経路探索結果のストレージの更新
    //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

#ifdef MEASURE_TIME
    TimeManager::startClock("RENEW_ROUTE_STORAGE");
#endif //MEASURE_TIME

    _routingManager->renewRouteStorage();

#ifdef MEASURE_TIME
    TimeManager::stopClock("RENEW_ROUTE_STORAGE");
#endif //MEASURE_TIME

    //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    // 環境の更新
    //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

#ifdef MEASURE_TIME
    TimeManager::startClock("RENEW_ENV");
#endif //MEASURE_TIME

    ScheduleManager::renewSpeedLimit();

#ifdef MEASURE_TIME
    TimeManager::stopClock("RENEW_ENV");
#endif //MEASURE_TIME

#endif //INCLUDE_VEHICLES

    //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    // エージェントの消去
    //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
#ifdef MEASURE_TIME
    TimeManager::startClock("DELETE_AGENT");
#endif //MEASURE_TIME

    _roadMap->deleteArrivedAgents();

#ifdef MEASURE_TIME
    TimeManager::stopClock("DELETE_AGENT");
#endif //MEASURE_TIME

    //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    // 通信処理
    //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
#ifdef DEBUG
    _parallel->barrier();
    cout << "myId:" << _parallel->myId() << " " << "COMMUNICATE" << endl;
#endif //DEBUG

#ifdef MEASURE_TIME
    TimeManager::startClock("COMMUNICATE");
#endif //MEASURE_TIME

    _parallel->communicate();

    #ifdef MEASURE_TIME
        TimeManager::stopClock("COMMUNICATE");
    #endif //MEASURE_TIME

    //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    // エージェントの発生
    //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
#ifdef DEBUG
    _parallel->barrier();
    cout << "myId:" << _parallel->myId() << " " << "GENERATE" << endl;
#endif //DEBUG

    // 車両の発生
#ifdef INCLUDE_VEHICLES

#ifdef MEASURE_TIME
    TimeManager::startClock("GENERATE");
#endif //MEASURE_TIME

    // 車両の生成と登場
    _genVehicleController->generateVehicle();

#ifdef MEASURE_TIME
    TimeManager::stopClock("GENERATE");
#endif //MEASURE_TIME

#endif //INCLUDE_VEHICLES

    // 歩行者の発生
#ifdef INCLUDE_PEDESTRIANS

#ifdef MEASURE_TIME
    TimeManager::startClock("GENERATE_PEDESTRIAN");
#endif //MEASURE_TIME

    // 歩行者の生成と登場
    _pedExt->generatePedestrian();

#ifdef MEASURE_TIME
    TimeManager::stopClock("GENERATE_PEDESTRIAN");
#endif //MEASURE_TIME

#endif //INCLUDE_VEHICLES

    //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    // 認知
    //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
#ifdef DEBUG
    _parallel->barrier();
    cout << "myId:" << _parallel->myId() << " " << "RECOGNIZE" << endl;
#endif //DEBUG
    //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
    // 車両
#ifdef INCLUDE_VEHICLES
    vector<Vehicle*>* vehicles = ObjManager::vehicles();

#ifdef _OPENMP //-------------------------------------------------------

    int vehiclesSize = vehicles->size();

#ifdef MEASURE_TIME
    TimeManager::startClock("RECOGNIZE");
#endif //MEASURE_TIME

#pragma omp parallel for schedule (dynamic)
    for (int i = 0; i < vehiclesSize; i++)
    {
        (*vehicles)[i]->recognize();
    }

#ifdef MEASURE_TIME
    TimeManager::stopClock("RECOGNIZE");
#endif //MEASURE_TIME

#else //_OPENMP not defined --------------------------------------------

#ifdef MEASURE_TIME
    TimeManager::startClock("RECOGNIZE");
#endif //MEASURE_TIME

    for_each(vehicles->begin(), vehicles->end(),
             mem_fun(&Vehicle::recognize));

#ifdef MEASURE_TIME
    TimeManager::stopClock("RECOGNIZE");
#endif //MEASURE_TIME

#endif //_OPENMP -------------------------------------------------------

#endif //INCLUDE_VEHICLES

    //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
    // 歩行者
#ifdef INCLUDE_PEDESTRIANS
    vector<Pedestrian*>* pedestrians = ObjManager::pedestrians();

#ifdef _OPENMP //-------------------------------------------------------
    int pedestriansSize = pedestrians->size();

#ifdef MEASURE_TIME
    TimeManager::startClock("RECOGNIZE_PEDESTRIAN");
#endif //MEASURE_TIME

#pragma omp parallel for schedule (dynamic)
    for (int i = 0; i < pedestrians->size(); i++)
    {
    	(*pedestrians)[i]->recognize ();
    }

#ifdef MEASURE_TIME
    TimeManager::stopClock("RECOGNIZE_PEDESTRIAN");
#endif //MEASURE_TIME

#else //_OPENMP not defined --------------------------------------------

#ifdef MEASURE_TIME
    TimeManager::startClock("RECOGNIZE_PEDESTRIAN");
#endif //MEASURE_TIME

    for_each(pedestrians->begin(), pedestrians->end(),
             mem_fun(&Pedestrian::recognize));

#ifdef MEASURE_TIME
    TimeManager::stopClock("RECOGNIZE_PEDESTRIAN");
#endif //MEASURE_TIME

#endif //_OPENMP -------------------------------------------------------

#endif //INCLUDE_PEDESTRIANS

    //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    // 意志決定
    //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
#ifdef DEBUG
    _parallel->barrier();
    cout << "myId:" << _parallel->myId() << " " << "READYTORUN" << endl;
#endif //DEBUG
    //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
    // 車両
#ifdef INCLUDE_VEHICLES

#ifdef _OPENMP //-------------------------------------------------------

#ifdef MEASURE_TIME
    TimeManager::startClock("READYTORUN");
#endif //MEASURE_TIME

#pragma omp parallel for schedule (dynamic)
    for (int i = 0; i < vehiclesSize; i++)
    {
        (*vehicles)[i]->determineAcceleration();
    }

#ifdef MEASURE_TIME
    TimeManager::stopClock("READYTORUN");
#endif //MEASURE_TIME

#else //_OPENMP not defined --------------------------------------------

#ifdef MEASURE_TIME
    TimeManager::startClock("READYTORUN");
#endif //MEASURE_TIME

    for_each(vehicles->begin(), vehicles->end(),
             mem_fun(&Vehicle::determineAcceleration));

#ifdef MEASURE_TIME
    TimeManager::stopClock("READYTORUN");
#endif //MEASURE_TIME

#endif //_OPENMP -------------------------------------------------------

#endif //INCLUDE_VEHICLES

    //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
    // 歩行者
#ifdef INCLUDE_PEDESTRIANS

#ifdef _OPENMP //-------------------------------------------------------

#ifdef MEASURE_TIME
    TimeManager::startClock("READYTOWALK_PEDESTRIAN");
#endif //MEASURE_TIME

#pragma omp parallel for schedule (dynamic)
    for (int i = 0; i < pedestriansSize; i++)
    {
        /*Random::multiStockBeginMulti(i);*/
        (*pedestrians)[i]->determineToChangeVelocity();
    }

#ifdef MEASURE_TIME
    TimeManager::stopClock("READYTOWALK_PEDESTRIAN");
#endif //MEASURE_TIME

#else //_OPENMP not defined --------------------------------------------

#ifdef MEASURE_TIME
    TimeManager::startClock("READYTOWALK_PEDESTRIAN");
#endif //MEASURE_TIME

    for_each(pedestrians->begin(), pedestrians->end(),
             mem_fun(&Pedestrian::determineToChangeVelocity));

#ifdef MEASURE_TIME
    TimeManager::stopClock("READYTOWALK_PEDESTRIAN");
#endif //MEASURE_TIME

#endif //_OPENMP -------------------------------------------------------

#endif // INCLUDE_PEDESTRIANS

    //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    // 行動
    //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
#ifdef DEBUG
    _parallel->barrier();
    cout << "myId:" << _parallel->myId() << " " << "RUN" << endl;
#endif //DEBUG
    //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
    // 車両
#ifdef INCLUDE_VEHICLES

#ifdef _OPENMP //-------------------------------------------------------

#ifdef MEASURE_TIME
    TimeManager::startClock("RUN");
#endif //MEASURE_TIME

#pragma omp parallel for schedule (dynamic)
    for (int i = 0; i < vehiclesSize; i++)
    {
        (*vehicles)[i]->run();
    }

#ifdef MEASURE_TIME
    TimeManager::stopClock("RUN");
#endif //MEASURE_TIME

#else //_OPENMP not defined --------------------------------------------

#ifdef MEASURE_TIME
    TimeManager::startClock("RUN");
#endif //MEASURE_TIME

    for_each(vehicles->begin(), vehicles->end(),
             mem_fun(&Vehicle::run));

#ifdef MEASURE_TIME
    TimeManager::stopClock("RUN");
#endif //MEASURE_TIME

#endif //_OPENMP -------------------------------------------------------

#endif //INCLUDE_VEHICLES

    //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
    // 歩行者
#ifdef INCLUDE_PEDESTRIANS

#ifdef _OPENMP //-------------------------------------------------------

#ifdef MEASURE_TIME
    TimeManager::startClock("WALK_PEDESTRIAN");
#endif //MEASURE_TIME

#pragma omp parallel for schedule (dynamic)
    for (int i = 0; i < pedestriansSize; i++)
    {
        (*pedestrians)[i]->walk();
    }

#ifdef MEASURE_TIME
    TimeManager::stopClock("WALK_PEDESTRIAN");
#endif //MEASURE_TIME

#else //_OPENMP not defined --------------------------------------------

#ifdef MEASURE_TIME
    TimeManager::startClock("WALK_PEDESTRIAN");
#endif //MEASURE_TIME

    for_each(pedestrians->begin(), pedestrians->end(),
             mem_fun(&Pedestrian::walk));

#ifdef MEASURE_TIME
    TimeManager::stopClock("WALK_PEDESTRIAN");
#endif //MEASURE_TIME

#endif //_OPENMP -------------------------------------------------------

#endif //INCLUDE_PEDESTRIANS

    //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    // 時系列データ出力
    //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
#ifdef DEBUG
    _parallel->barrier();
    cout << "myId:" << _parallel->myId() << " " << "WRITE_RESULT" << endl;
#endif //DEBUG

// #ifdef MEASURE_TIME
//     TimeManager::startClock("WRITE_RESULT");
// #endif //MEASURE_TIME
//
//     writeRunInfo();
//     if (GVManager::getFlag("FLAG_OUTPUT_TIMELINE_D")
//         || GVManager::getFlag("FLAG_OUTPUT_TIMELINE_S"))
//     {
//         writeResult();
//     }
//
// #ifdef MEASURE_TIME
//     TimeManager::stopClock("WRITE_RESULT");
// #endif //MEASURE_TIME

#ifdef MEASURE_TIME
    TimeManager::stopClock("TOTAL_RUN");
#endif //MEASURE_TIME


    return true;
}

//======================================================================
void Simulator::writeResult() const
{
    const RMAPSI* signals
        = _roadMap->signals();
    _signalIO
        ->writeSignalsDynamicData(TimeManager::time(), signals);

    vector<Vehicle*>* vehicles = ObjManager::vehicles();
    _vehicleIO
        ->writeVehiclesDynamicData(TimeManager::time(), vehicles);

#ifdef INCLUDE_PEDESTRIANS
    // implement here!!
#endif //INCLUDE_PEDESTRIANS
}

//======================================================================
void Simulator::writeRunInfo() const
{
    string fRunInfo;
    GVManager::getVariable("RESULT_RUN_INFO_FILE", &fRunInfo);
    ofstream ofs(fRunInfo.c_str(), ios::trunc);
    if (!ofs.fail())
    {
        ofs << TimeManager::step() << "\n"
            << TimeManager::unit();
        ofs.close();
    }
}

//======================================================================
RoadMap* Simulator::roadMap()
{
    return _roadMap;
}

//======================================================================
GenerateVehicleController* Simulator::generateVehicleController()
{
    return _genVehicleController;
}

//======================================================================
#ifdef INCLUDE_PEDESTRIANS
SimulatorPedExt* Simulator::pedExt()
{
    return _pedExt;
}

#endif //INCLUDE_PEDESTRIANS
