/* **************************************************
 * Copyright (C) 2014 ADVENTURE Project
 * All Rights Reserved
 **************************************************** */
#ifndef __SIMULATOR_H__
#define __SIMULATOR_H__

#include "RoadMap.h"
#include "TimeManager.h"
#include "VehicleFamily.h"
#include "VehicleIO.h"
#include "SignalIO.h"
#include "Parallel.h"
#ifdef INCLUDE_PEDESTRIANS
#include "pedestrian/SimulatorPedExt.h"
#endif //INCLUDE_PEDESTRIANS

class GenerateVehicleController;
class RoutingManager;
class ODNode;
class Vehicle;
class Parallel;

/**
 * @addtogroup Running
 * @brief シミュレーションの実行
 * @ingroup Procedure
 */

/// シミュレータ本体
/**
 * @ingroup Initialization Running
 */
class Simulator
{
public:
#ifdef INCLUDE_PEDESTRIANS
    // フレンド宣言
    friend class SimulatorPedExt;
#endif //INCLUDE_PEDESTRIANS

    Simulator();
    ~Simulator();

    //====================================================================
    /** @name シミュレーションの準備 */
    //@{

    /// 初期化は適切に完了しているか
    bool hasInit() const;

    /// ファイルを読み込んでRoadMapを作成する
    /**
     * @return 作成に成功したかどうか
     */
    bool getReadyRoadMap();

    /// RoadsideUnitに関する初期設定を行う
    /**
     * @return 設定に成功したかどうか
     */
    bool getReadyRoadsideUnit();

    /// Vehicleに関する初期設定を行う
    /**
     * @return 設定に成功したかどうか
     */
    bool getReadyVehicles();

    /// 作成したRoadMapの情報を出力する
    void writeInitializedRoadMap();

    /// レーンチェック、エラー時は表示確認のため run のみ止める
    void checkLane();

    /// レーンチェックエラー取得
    bool checkLaneError();

    //@}

    //====================================================================
    /** @name シミュレーションの実行 */
    //@{

    /// @p time までシミュレータを動かす
    bool run(ulint time);

    /// 1ステップ分シミュレータを動かす
    bool timeIncrement();

    //@}

public:
    //====================================================================
    /** @name ファイル入出力 */
    //@{

    /// 時系列データを出力する
    void writeResult() const;

    /// run_infoを出力する
    /**
     * @note
     * 本来はシミュレーション終了時に出力すればよいはずだが，
     * 実行時エラーが発生したりCtrl-Cで強制終了した場合に対応するため
     * 各タイムステップの処理が終わるたびに更新することにする．
     */
    void writeRunInfo() const;

    //@}

    //====================================================================
    /** @name 内部の地図や車両発生コントローラへのアクセッサ */
    //@{

    /// 地図を返す
    RoadMap* roadMap();

    /// 経路探索管理オブジェクトを返す
    RoutingManager* routingManager() const
    {
        return _routingManager;
    }

    /// 車両発生コントローラを返す
    GenerateVehicleController* generateVehicleController();

    //@}

private:
    /// 地図オブジェクト
    RoadMap* _roadMap;

    /// 経路探索を担当するオブジェクト
    RoutingManager* _routingManager;

    /// 車両発生を担当するコントローラ
    GenerateVehicleController* _genVehicleController;

    /// レーンチェックエラー、表示確認のため run のみ止める
    bool _checkLaneError;

    /// parallelオブジェクト
    Parallel* _parallel;
    
    //====================================================================
    /** @name 入出力用オブジェクトとフラグ*/
    /// @{

    /// 車両情報の入出力を担当するオブジェクト
    VehicleIO* _vehicleIO;

    /// 信号情報の入出力を担当するオブジェクト
    SignalIO* _signalIO;
    //@}

    //====================================================================
    /** @name 歩行者用拡張 */
    /// @{

#ifdef INCLUDE_PEDESTRIANS
public:
    /// 歩行者用拡張オブジェクトを返す
    SimulatorPedExt* pedExt();

protected:
    /// 歩行者用拡張オブジェクト
    SimulatorPedExt* _pedExt;
#endif //INCLUDE_PEDESTRIANS

    /// @}

};

#endif //__SIMULATOR_H__
