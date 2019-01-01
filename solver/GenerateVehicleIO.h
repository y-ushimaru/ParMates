/* **************************************************
 * Copyright (C) 2014 ADVENTURE Project
 * All Rights Reserved
 **************************************************** */
#ifndef __GENERATE_VEHICLE_IO_H__
#define __GENERATE_VEHICLE_IO_H__

#include <string>
#include <vector>
#include "TimeManager.h"

class Simulator;
class RoadMap;
class GenerateVehicleController;
class GeneratingTable;

/// 車両の発生に関する情報を入出力するクラス
/**
 * @ingroup IO
 */
class GenerateVehicleIO
{
private:
    GenerateVehicleIO();
    ~GenerateVehicleIO(){};

public:
    /// 唯一のインスタンスを返す
    static GenerateVehicleIO& instance();

    /// シミュレータをセットする
    void setSimulator(Simulator* sim)
    {
        if (!_sim)
        {
            _sim = sim;
        }
    }
    
    /// ランダム選択から除外するODノードの読み込み
    void readExcludedODNodes
    (std::vector<std::string>* result_excludedStarts,
     std::vector<std::string>* result_excludedGoals);
    
    /// ODグループ定義ファイルの読み込み
    void readODGroupFile(GenerateVehicleController* controller);

    /// 車両発生定義テーブルの初期設定
    void setUpGeneratingTables
    (GenerateVehicleController* controller);

    /// ランダム車両発生定義テーブルの初期設定
    /**
     * 厳密にはファイル入力とは関連しないが，
     * setUpGeneratingTablesとの関連でこのクラスに入れる
     */
    void setUpRandomGeneratingTable
    (GenerateVehicleController* controller);

private:
    /** @name 車両発生テーブルの読み込みに関するprivate関数 */
    /// @{

    /// 車両発生定義テーブルの設定ファイルの読み込み
    /**
     * typeにより種別を分ける
     *
     * - normal: 通常のgenerateTable, defaultGeneratetable
     * (開始時刻, 終了時刻, 出発地ID, 目的地ID
     *  交通量, 車種, 経由地数, 経由地ID) の順で指定する
     *
     * - fixed: fixedGenerateTable
     * (出発時刻 (=開始時刻=終了時刻), 出発地ID, 目的地ID,
     *  車種, 経由地数, 経由地ID) の順で指定する
     *
     * - grouped: groupedGenerateTable
     * (開始時刻, 終了時刻,
     *  出発地グループ化フラグ, 出発地ID,
     *  目的地グループ化フラグ, 目的地ID,
     *  交通量, 車種, 経由地数, 経由地ID) の順で指定する
     */
    void _readGeneratingTableFile
    (GeneratingTable* table, const std::string& type,
     const std::string fileName);
    
    /// 文字列から時刻を得る
    ulint _str2time(std::string str) const;

    /// 文字列から交通量を得る
    double _str2volume(std::string str) const;

    /// 文字列から車種を得る
    int _str2vehicleType(std::string str) const;
    
    /// トークンから経由地を得る
    /**
     * posOrigin，posDestinationはvectorにおける出発地と目的地の位置，
     * posMidGatesは途中経由地の位置を示す
     */
    void _getGates(std::vector<std::string>* result_gates,
                   std::vector<std::string>* tokens,
                   unsigned int posOrigin,
                   unsigned int posDestination,
                   unsigned int posNumGates,
                   unsigned int posMidGates) const;

    /// @}

public:
    /// 経路探索の効用関数に関するパラメータセットの読み込み
    void readVehicleRoutingParams
    (GenerateVehicleController* controller);

    /// 経路探索で選好するネットワークランクの選択確率の読み込み
    void readVehicleRoutingPrefRank
    (GenerateVehicleController* controller);

private:
    /// シミュレータオブジェクト
    Simulator* _sim;
};

#endif //__GENERATE_VEHICLE_IO_H__
